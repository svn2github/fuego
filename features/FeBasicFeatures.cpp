//----------------------------------------------------------------------------
/** @file FeBasicFeatures.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeBasicFeatures.h"

#include <iostream>
#include <string>
#include "FePatternBase.h"
#include "Go3x3Pattern.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoLadder.h"
#include "GoOpeningKnowledge.h"
#include "GoPattern12Point.h"
#include "GoSafetySolver.h"
#include "GoSetupUtil.h"
#include "SgPointSet.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

// @todo  NEED TO MAKE SURE that move is evaluated with last move info intact.

//----------------------------------------------------------------------------
namespace {



inline FeBasicFeature ComputeFeature(FeBasicFeature baseFeature,
                                     int baseValue, int value)
{
    return static_cast<FeBasicFeature>(static_cast<int>(baseFeature)
                                       + value - baseValue);
}

/** move is liberty of both lastMove and some other opponent block */
bool PreventConnection(const GoBoard& bd, SgPoint move,
                       SgPoint anchor, const SgBlackWhite opp)
{
    return bd.IsLibertyOfBlock(move, anchor)
        && GoBoardUtil::IsCuttingPoint(bd, move, opp);
}
   
int TotalNuStones(const GoBoard& bd,
                  const SgPoint anchors[])
{
    int nuStones = 0;
    for (int i=0; anchors[i] != SG_ENDPOINT; ++i)
    nuStones += bd.NumStones(anchors[i]);
    return nuStones;
}

/** Map number of stones into a category 1..7,
 then compute the corresponding feature for that category.
 The mapping is as follows:
 1..3 --> category 1..3,
 4..6 --> category 4,
 7..10 --> category 5,
 11..20 --> category 6,
 21 or more --> category 7. */
FeBasicFeature ComputeStoneFeature(const int nuStones,
                                   FeBasicFeature baseFeature)
{
    SG_ASSERT(nuStones > 0);
    int category;
    if (nuStones <= 3)
        category = nuStones;
    else if (nuStones <= 6)
        category = 4;
    else if (nuStones <= 10)
        category = 5;
    else if (nuStones <= 20)
        category = 6;
    else
        category = 7;
    
    return ComputeFeature(baseFeature, 1, category);
}


int NuAdjacentStones(const GoBoard& bd, const SgPoint p,
                     const SgBlackWhite blockColor)
{
    SgPoint anchors[4 + 1];
    bd.NeighborBlocks(p, blockColor, 3, anchors);
    int nuStones = 0;
    for (int i = 0; anchors[i] != SG_ENDPOINT; ++i)
    nuStones += bd.NumStones(anchors[i]);
    return nuStones;
}

void SetStoneFeature(const GoBoard& bd,
                     SgPoint p,
                     const SgBlackWhite blockColor,
                     FeBasicFeature baseFeature,
                     FeMoveFeatures& features)
{
    SG_ASSERT(bd.IsEmpty(p));
    int nuStones = NuAdjacentStones(bd, p, blockColor);
    if (blockColor == bd.ToPlay()) // add one for the future stone on p
        ++nuStones;
    const FeBasicFeature f = ComputeStoneFeature(nuStones, baseFeature);
    features.Set(f);
}

bool IsLastMove(const GoBoard& bd, SgPoint anchor)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (SgIsSpecialMove(lastMove))
        return false;
    // assumes no suicide, last move is occupied
    SG_ASSERT(bd.IsColor(lastMove, bd.Opponent()));
    return bd.Anchor(lastMove) == anchor;
}
    
void FindCaptureFeatures(const GoBoard& bd, SgPoint anchor,
                         SgPoint theLib,
                         FeMoveFeatures& features)
{
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opp = SgOppBW(toPlay);

    FeBasicFeature f = FE_NONE;
    // our own neighbor is in atari
    if (GoBoardUtil::HasAdjacentBlocks(bd, anchor, 1))
       f = FE_CAPTURE_ADJ_ATARI;
    else if (IsLastMove(bd, anchor))
    {
        if (bd.CapturingMove())
            f = FE_CAPTURE_RECAPTURE;
        else if (PreventConnection(bd, theLib, anchor, opp))
            f = FE_CAPTURE_PREVENT_CONNECTION;
    }
    if (f != FE_NONE)
        features.Set(f);
    else // other capture
    {
        const GoLadderStatus ls =
              GoLadderUtil::LadderStatus(bd, anchor);
        f = (ls == GO_LADDER_CAPTURED) ? FE_CAPTURE_LADDER
                                       : FE_CAPTURE_NOT_LADDER;
        features.Set(f);
    }
}

void FindExtensionFeatures(const GoBoard& bd, SgPoint anchor,
                           FeMoveFeatures& features)
{
    SG_ASSERT(bd.NumLiberties(anchor) == 1);
    GoLadderStatus ls = GoLadderUtil::LadderStatus(bd, anchor);
    if (ls == GO_LADDER_CAPTURED)
        features.Set(FE_EXTENSION_LADDER);
    else
    {
        features.Set(FE_EXTENSION_NOT_LADDER);
        const FeBasicFeature saveF =
            ComputeStoneFeature(bd.NumStones(anchor), FE_SAVE_STONES_1);
        features.Set(saveF);
    }
}

void FindAtariFeatures(const GoBoard& bd, SgPoint anchor,
                       SgPoint move,
                       FeMoveFeatures& features)
{
    //FE_ATARI_LADDER,        // Ladder atari
    //FE_ATARI_KO,            // Atari when there is a ko
    //FE_ATARI_OTHER,         // Other atari
    SG_ASSERT(bd.NumLiberties(anchor) == 2);
    SG_ASSERT(bd.IsLibertyOfBlock(move, anchor));
              
    bool hasFeature = false;
    if (GoLadderUtil::IsLadderCaptureMove(bd, anchor, move))
    {
        features.Set(FE_ATARI_LADDER);
        SetStoneFeature(bd, move, bd.Opponent(), FE_KILL_STONES_1, features);
        hasFeature = true;
    }
    if (bd.KoPoint() != SG_NULLPOINT)
        features.Set(FE_ATARI_KO);
    else if (! hasFeature)
        features.Set(FE_ATARI_OTHER);
}

void FindCfgFeatures(const GoBoard& bd, SgPoint focus,
                     FeBasicFeature baseFeature, int baseDistance,
                     const GoPointList& legalBoardMoves,
                     GoEvalArray<FeMoveFeatures>& features)
{
    const int MAX_DIST = 3;
    SgPointArray<int> dist = GoBoardUtil::CfgDistance(bd, focus, MAX_DIST);
    for (GoPointList::Iterator it(legalBoardMoves); it; ++it)
    {
        const SgPoint p = *it;
        SG_ASSERT(p != SG_PASS);
        SG_ASSERT(dist[p] >= baseDistance);
        SG_ASSERT(  dist[p] <= MAX_DIST
                 || dist[p] == std::numeric_limits<int>::max());
        const int distance = std::min(dist[p], MAX_DIST + 1);
        const FeBasicFeature f = ComputeFeature(baseFeature,
                                                baseDistance, distance);
        features[*it].Set(f);
    }
}

void FindCfgFeatures(const GoBoard& bd,
                     const GoPointList& legalBoardMoves,
                     GoEvalArray<FeMoveFeatures>& features)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (! SgIsSpecialMove(lastMove))
        FindCfgFeatures(bd, lastMove,
                        FE_CFG_DISTANCE_LAST_1, 1,
                        legalBoardMoves,
                        features);

    const SgPoint lastMove2 = bd.Get2ndLastMove();
    if (! SgIsSpecialMove(lastMove2))
        FindCfgFeatures(bd, lastMove2,
                        FE_CFG_DISTANCE_LAST_OWN_0, 0,
                        legalBoardMoves,
                        features);
}

void FindCornerMoveFeatures(const GoBoard& bd,
                GoEvalArray<FeMoveFeatures>& features)
{
    const std::vector<SgPoint>
    corner(GoOpeningKnowledge::FindCornerMoves(bd));
    for (std::vector<SgPoint>::const_iterator it
         = corner.begin(); it != corner.end(); ++it)
    {
        features[*it].Set(FE_CORNER_OPENING_MOVE);
    }
}

void FindGamePhaseFeature(const GoBoard& bd,
                          const GoPointList& legalBoardMoves,
                          GoEvalArray<FeMoveFeatures>& features)
{
    const int phase = std::min(12, bd.MoveNumber() / 30 + 1);
    FeBasicFeature f = ComputeFeature(FE_GAME_PHASE_1, 1, phase);
    SG_ASSERT(f >= FE_GAME_PHASE_1);
    SG_ASSERT(f <= FE_GAME_PHASE_12);
    for (GoPointList::Iterator it(legalBoardMoves); it; ++it)
        features[*it].Set(f);
}

inline void FindBlockAnchors(const GoBoard& bd, int maxNuLiberties,
                             GoPointList& anchors)
{
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        if (  bd.Occupied(*it)
           && bd.Anchor(*it) == *it
           && bd.NumLiberties(*it) <= maxNuLiberties
           )
            anchors.PushBack(*it);
    }
}

void FindAtariCaptureFeatures(const GoBoard& bd, SgPoint anchor,
                              GoEvalArray<FeMoveFeatures>& features)
{
    // FE_CAPTURE_MULTIPLE,    // capture more than one block
    // FE_SNAPBACK, // move is a single stone capture but opponent can snap back

    const SgBlackWhite color = bd.GetStone(anchor);
    SG_ASSERT(bd.Opponent() == color);
    const SgPoint theLib = bd.TheLiberty(anchor);
    SgPoint anchors[4 + 1];
    bd.NeighborBlocks(theLib, color, 1, anchors);
    SG_ASSERT(anchors[0] != SG_ENDPOINT);
    if (anchors[1] != SG_ENDPOINT)
        features[theLib].Set(FE_CAPTURE_MULTIPLE);
    else if (GoBoardUtil::IsSnapback(bd, anchors[0]))
        features[theLib].Set(FE_SNAPBACK);
    FindCaptureFeatures(bd, anchor, theLib, features[theLib]);
    const int nuStones = TotalNuStones(bd, anchors);
    const FeBasicFeature stoneF =
        ComputeStoneFeature(nuStones, FE_KILL_STONES_1);
    features[theLib].Set(stoneF);
}

bool IsDoubleAtari(const GoBoard& bd, SgPoint lib,
                   const SgBlackWhite blockColor)
{
    SgPoint anchors[4 + 1];
    bd.NeighborBlocks(lib, blockColor, 2, anchors);
    SG_ASSERT(anchors[0] != SG_ENDPOINT);
    return anchors[1] != SG_ENDPOINT;
}

void FindPlayDoubleAtariFeatures(const GoBoard& bd, SgPoint anchor,
                             GoEvalArray<FeMoveFeatures>& features)
{
    // FE_DOUBLE_ATARI,        // atari two or more opponent stones
    const SgBlackWhite color = bd.GetStone(anchor);
    SG_ASSERT(bd.Opponent() == color);
    SG_ASSERT(bd.NumLiberties(anchor) == 2);

    for (GoBoard::LibertyIterator it(bd, anchor); it; ++it)
        if (  IsDoubleAtari(bd, *it, color)
           && ! GoBoardUtil::SelfAtari(bd, *it)
           )
        {
            features[*it].Set(FE_DOUBLE_ATARI);
            SetStoneFeature(bd, *it, color, FE_KILL_STONES_1, features[*it]);
        }
}

void FindDefendDoubleAtariFeatures(const GoBoard& bd, SgPoint anchor,
                                 GoEvalArray<FeMoveFeatures>& features)
{
    // FE_DOUBLE_ATARI_DEFEND, // prevent double atari move, e.g. connect
    const SgBlackWhite color = bd.GetStone(anchor);
    SG_ASSERT(bd.ToPlay() == color);
    SG_ASSERT(bd.NumLiberties(anchor) == 2);
    for (GoBoard::LibertyIterator it(bd, anchor); it; ++it)
        if (  IsDoubleAtari(bd, *it, color)
           && ! GoBoardUtil::SelfAtari(bd, *it)
           )
            // todo other tests that it's not suicidal, e.g. bad 2-lib, 3-lib
        {
            features[*it].Set(FE_DOUBLE_ATARI_DEFEND);
            // todo: set to min. number of stones for 2 blocks, not sum.
            // todo what is best for 3 blocks??? 2nd largest?
            // todo there should also be some "threat value" for attacking
            // largest (and 3rd largest) block.
            // maybe make these all separate features ????
            SetStoneFeature(bd, *it, color, FE_SAVE_STONES_1, features[*it]);
        }
}

inline void TryLadderEscapeMove(const GoBoard& bd, SgPoint anchor,
                                SgPoint move,
                                GoPointList& tried,
                                GoPointList& works
                                )
{
    if (! tried.Contains(move))
    {
        tried.PushBack(move);
        if (GoLadderUtil::IsLadderEscapeMove(bd, anchor, move))
            works.PushBack(move);
    }
}

inline void FindOtherLadderEscapeMoves(const GoBoard& bd, SgPoint anchor,
                                SgPoint toEscape,
                                GoEvalArray<FeMoveFeatures>& features)
{
    GoPointList tried;
    GoPointList works;
    tried.PushBack(toEscape);
    works.PushBack(toEscape);
    for (GoBoard::LibertyCopyIterator it(bd, anchor); it; ++it)
    {
        TryLadderEscapeMove(bd, anchor, *it, tried, works);
        // Try 1 away from liberties: diagonal and 1 point jump
        for (GoNbIterator nb(bd, *it); nb; ++nb)
            if (bd.IsEmpty(*nb))
                TryLadderEscapeMove(bd, anchor, *nb, tried, works);
    }

    for (GoPointList::Iterator it(works); it; ++it)
    {
        features[*it].Set(FE_TWO_LIB_SAVE_LADDER);
        
        // Todo: this will set separate flags if several blocks
        // are saved by the same move.
        // but we do not have the information here to add up.
        const FeBasicFeature saveF =
        ComputeStoneFeature(bd.NumStones(anchor), FE_SAVE_STONES_1);
        features[*it].Set(saveF);
    }

    // todo: try more moves, e.g. one point jump,
    // liberty of adjacent weak stones, diagonal move, bamboo connection
}

void FindOpp2LibFeatures(const GoBoard& bd, SgPoint anchor,
                         GoEvalArray<FeMoveFeatures>& features)
{
    for (GoBoard::LibertyCopyIterator it(bd, anchor); it; ++it)
        FindAtariFeatures(bd, anchor, *it, features[*it]);
    FindPlayDoubleAtariFeatures(bd, anchor, features);
}

/** Check if playing our own liberty would set up a ladder for opponent */
void CheckSelfLadder(const GoBoard& bd, SgPoint anchor,
                     GoEvalArray<FeMoveFeatures>& features)
{
    //    FE_TWO_LIB_SELF_LADDER
    const SgBlackWhite toPlay = bd.ToPlay();
    for (GoBoard::LibertyCopyIterator it(bd, anchor); it; ++it)
        if (! GoLadderUtil::IsLadderEscapeMove(bd, anchor, *it))
        {
            features[*it].Set(FE_TWO_LIB_SELF_LADDER);
            SetStoneFeature(bd, *it, toPlay, FE_KILL_OWN_STONES_1,
                            features[*it]);
        }
}

bool WouldBeLadderCaptured(const GoBoard& constBd, SgPoint move)
{
    GoModBoard mbd(constBd);
    GoBoard& bd = mbd.Board();
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opponent = SgOppBW(toPlay);
    SG_ASSERT(bd.IsEmpty(move));
    SG_ASSERT(bd.NumEmptyNeighbors(move) == 2);
    SG_ASSERT(bd.NumNeighbors(move, toPlay) == 0);
    bd.Play(move);
    const bool wouldBeCaptured = GoLadderUtil::Ladder(bd, move, opponent);
    bd.Undo();
    return wouldBeCaptured;
}

void CheckProtectedLiberty(const GoBoard& bd,
                            SgPoint p,
                            GoEvalArray<FeMoveFeatures>& features)
{
    const SgBlackWhite toPlay = bd.ToPlay();
    bool byLadder;
    SG_UNUSED(byLadder);
    bool isKoCut;
    const bool isOurProtected =
    GoLadderUtil::IsProtectedLiberty(bd, p, toPlay,
                                     byLadder, isKoCut, true);
    if (isOurProtected)
        features[p].Set(FE_OUR_PROTECTED_LIBERTY);
    else if (isKoCut)
        features[p].Set(FE_OUR_CUT_WITH_KO);

    const bool isOppProtected =
    GoLadderUtil::IsProtectedLiberty(bd, p, SgOppBW(toPlay),
                                     byLadder, isKoCut, true);
    if (isOppProtected)
    {
        features[p].Set(FE_OPP_PROTECTED_LIBERTY);
        if (bd.HasNeighbors(p, toPlay))
            SetStoneFeature(bd, p, toPlay, FE_KILL_OWN_STONES_1, features[p]);
    }
    else if (isKoCut)
        features[p].Set(FE_OPP_CUT_WITH_KO);

}

void FindNewSelfLadderMoves(const GoBoard& bd,
                            const GoPointList& legalMoves,
                            GoEvalArray<FeMoveFeatures>& features)
{
    // FE_TWO_LIB_NEW_SELF_LADDER
    // FE_OUR_PROTECTED_LIBERTY, // opponent could be captured there
    // FE_OPP_PROTECTED_LIBERTY, // we would be captured there
    // FE_OUR_CUT_WITH_KO, // can cut to start a ko here.
    // FE_OPP_CUT_WITH_KO, // can cut to start a ko here.

    const SgBlackWhite toPlay = bd.ToPlay();
    for (GoPointList::Iterator it(legalMoves); it; ++it)
        if (  bd.NumEmptyNeighbors(*it) == 2    // new stone with 2 liberties
           && bd.NumNeighbors(*it, toPlay) == 0
           && WouldBeLadderCaptured(bd, *it)
           )
        {
            features[*it].Set(FE_TWO_LIB_NEW_SELF_LADDER);
            features[*it].Set(FE_KILL_OWN_STONES_1);
        }
        else if (bd.NumEmptyNeighbors(*it) < 3)
            CheckProtectedLiberty(bd, *it, features);
}

void FindOwn2LibFeatures(const GoBoard& bd, SgPoint anchor,
                      GoEvalArray<FeMoveFeatures>& features)
{
    // FE_TWO_LIB_SAVE_LADDER, // save own 2 lib block from ladder capture

    const SgBlackWhite toPlay = bd.ToPlay();
    SG_ASSERT(toPlay == bd.GetStone(anchor));
    SgPoint toCapture(SG_NULLPOINT);
    SgPoint toEscape(SG_NULLPOINT);

    const GoLadderStatus status =
    GoLadderUtil::LadderStatus(bd, anchor, false, &toCapture, &toEscape);
    if (status == GO_LADDER_UNSETTLED)
    {
        SG_ASSERT(toEscape != SG_NULLPOINT);
        SG_ASSERT(toEscape != SG_PASS);
        SG_ASSERT(bd.IsLegal(toEscape));
        features[toEscape].Set(FE_TWO_LIB_SAVE_LADDER);
        FindOtherLadderEscapeMoves(bd, anchor, toEscape, features);
    }
    else if (status == GO_LADDER_CAPTURED) // no escape by playing on libs
        for (GoBoard::LibertyIterator it(bd, anchor); it; ++it)
        {
            features[*it].Set(FE_TWO_LIB_STILL_LADDER);
            SetStoneFeature(bd, *it, toPlay, FE_KILL_OWN_STONES_1,
                            features[*it]);
        }
    else
        CheckSelfLadder(bd, anchor, features);

    FindDefendDoubleAtariFeatures(bd, anchor, features);
}

void FindOpp3LibFeatures(const GoBoard& bd, SgPoint anchor,
                         GoEvalArray<FeMoveFeatures>& features)
{
    SG_ASSERT(bd.Opponent() == bd.GetStone(anchor));
    // Play liberty of opponent 3 lib block.
    // Todo: have some different categories? Good/bad such moves?
    // Separate category for ladder threats. E.g. use as ko threats.
    for (GoBoard::LibertyIterator it(bd, anchor); it; ++it)
        features[*it].Set(FE_THREE_LIB_REDUCE_OPP_LIB);
}

void FindOwn3LibFeatures(const GoBoard& bd, SgPoint anchor,
                       GoEvalArray<FeMoveFeatures>& features)
{
    SG_ASSERT(bd.ToPlay() == bd.GetStone(anchor));

    // Find own-liberty-reducing moves, probably bad.
    for (GoBoard::LibertyIterator it(bd, anchor); it; ++it)
        if (! GoBoardUtil::KeepsOrGainsLiberties(bd, anchor, *it))
        {
            // todo check ladder status after move on *it
            features[*it].Set(FE_THREE_LIB_REDUCE_OWN_LIB);
        }
}

/* Try to find one other anchor in list. */
inline bool FindOther(const SgPoint anchors[5],
                      const SgPoint anchor,
                      SgPoint& other)
{
    SG_ASSERT(anchors[0] != SG_ENDPOINT);
    if (anchors[1] == SG_ENDPOINT)
        return false;
    if (anchors[1] != anchor)
        other = anchors[1];
    else
    {
        SG_ASSERT(anchors[0] != anchor);
        other = anchors[0];
    }
    return true;
}

void CheckCutConnect(const GoBoard& bd,
                     const SgPoint anchor,
                     const SgBlackWhite color,
                     const SgPoint liberty,
                     GoEvalArray<FeMoveFeatures>& features)
{
    SgPoint anchors[5];
    bd.NeighborBlocks(liberty, color, anchors);
    SgPoint other;
    if (  FindOther(anchors, anchor, other)
       && GoBoardUtil::GainsLiberties(bd, anchor, liberty)
       && GoBoardUtil::GainsLiberties(bd, other, liberty)
       )
    {
        const FeBasicFeature f = (bd.ToPlay() == color) ? FE_CONNECT : FE_CUT;
        features[liberty].Set(f);
    }
}

void FindCutConnectFeatures(const GoBoard& bd, SgPoint anchor,
                            GoEvalArray<FeMoveFeatures>& features)
{
    const SgBlackWhite color = bd.GetStone(anchor);
    for (GoBoard::LibertyCopyIterator it(bd, anchor); it; ++it)
        if (  bd.NumNeighbors(*it, color) > 1
           && ! GoLadderUtil::IsProtectedLiberty(bd, *it, color)
           )
            CheckCutConnect(bd, anchor, color, *it, features);
}

void FindLadderFeature(const GoBoard& bd, SgPoint anchor,
                       GoEvalArray<FeMoveFeatures>& features)
{
    switch (bd.NumLiberties(anchor))
    {
        case 1:
            if (bd.Opponent() == bd.GetStone(anchor))
                FindAtariCaptureFeatures(bd, anchor, features);
            else
            {
                const SgPoint theLib = bd.TheLiberty(anchor);
                FindExtensionFeatures(bd, anchor, features[theLib]);
            }
            break;
        case 2:
            if (bd.ToPlay() == bd.GetStone(anchor))
                FindOwn2LibFeatures(bd, anchor, features);
            else
                FindOpp2LibFeatures(bd, anchor, features);
            break;
        case 3:
            if (bd.ToPlay() == bd.GetStone(anchor))
                FindOwn3LibFeatures(bd, anchor, features);
            else
                FindOpp3LibFeatures(bd, anchor, features);
            break;
        default:
            SG_ASSERT(false);
    }
    FindCutConnectFeatures(bd, anchor, features);
}

void SetFeatureForPoints(const FeBasicFeature f,
                         const SgPointSet& points,
                         const GoPointList& legalMoves,
                         GoEvalArray<FeMoveFeatures>& features)
{
    for (GoPointList::Iterator it(legalMoves); it; ++it)
        if (points[*it])
            features[*it].Set(f);
}

void FindSafeOwnTerritoryFeatures(const SgPointSet& safe,
                                  const GoPointList& legalMoves,
                                  const bool isKo,
                                  GoEvalArray<FeMoveFeatures>& features)
{
    //FE_SAFE_TERRITORY_OWN_NO_KO, // play in own territory, no active ko
    //FE_SAFE_TERRITORY_OWN_KO,
    if (safe.IsEmpty())
        return;
    const FeBasicFeature f = isKo ? FE_SAFE_TERRITORY_OWN_KO
                                  : FE_SAFE_TERRITORY_OWN_NO_KO;
    SetFeatureForPoints(f, safe, legalMoves, features);
}

void FindSafeOppTerritoryFeatures(const SgPointSet& safe,
                                  const GoPointList& legalMoves,
                                  const bool isKo,
                                  GoEvalArray<FeMoveFeatures>& features)
{
    //FE_SAFE_TERRITORY_OPP_NO_KO,
    //FE_SAFE_TERRITORY_OPP_KO,  // play in opponent territory, active ko
    if (safe.IsEmpty())
        return;
    const FeBasicFeature f = isKo ? FE_SAFE_TERRITORY_OPP_KO
                                  : FE_SAFE_TERRITORY_OPP_NO_KO;
    SetFeatureForPoints(f, safe, legalMoves, features);
}

void FindSafeTerritoryFeatures(const GoBoard& bd,
                               const GoPointList& legalMoves,
                               GoEvalArray<FeMoveFeatures>& features)
{
    //FE_SAFE_TERRITORY_OWN_NO_KO, // play in own territory, no active ko
    //FE_SAFE_TERRITORY_OWN_KO,
    //FE_SAFE_TERRITORY_OPP_NO_KO,
    //FE_SAFE_TERRITORY_OPP_KO,  // play in opponent territory, active ko

    SgBWSet safe;
    GoSafetySolver safetySolver(bd);
    safetySolver.FindSafePoints(&safe);
    if (! safe.BothEmpty())
    {
        const bool isKo = bd.KoPoint() != SG_NULLPOINT;
        FindSafeOwnTerritoryFeatures(safe[bd.ToPlay()],
                                     legalMoves, isKo, features);
        FindSafeOppTerritoryFeatures(safe[bd.Opponent()],
                                     legalMoves, isKo, features);
    }
}

void FindSelfatariFeatures(const GoBoard& bd, SgPoint move,
                           FeMoveFeatures& features)
{
    if (GoBoardUtil::SelfAtari(bd, move))
    {
        features.Set(FE_SELFATARI);
        SetStoneFeature(bd, move, bd.ToPlay(),
                        FE_KILL_OWN_STONES_1, features);
    }
    // @todo FE_SELFATARI_NAKADE, FE_SELFATARI_THROWIN
}


void FindLadderFeatures(const GoBoard& bd,
                        const GoPointList& legalMoves,
                        GoEvalArray<FeMoveFeatures>& features)
{
    GoPointList anchors;
    FindBlockAnchors(bd, 3, anchors);
    for (GoPointList::Iterator it(anchors); it; ++it)
        FindLadderFeature(bd, *it, features);
    FindNewSelfLadderMoves(bd, legalMoves, features);

    for (GoPointList::Iterator it(legalMoves); it; ++it)
        FindSelfatariFeatures(bd, *it, features[*it]);
}

void FindLinePosFeatures(const GoBoard& bd,
                         const GoPointList& legalBoardMoves,
                         GoEvalArray<FeMoveFeatures>& features)
{
    for (GoPointList::Iterator it(legalBoardMoves); it; ++it)
    {
        const int line = std::min(5, bd.Line(*it));
        FeBasicFeature f = ComputeFeature(FE_LINE_1, 1, line);
        SG_ASSERT(f >= FE_LINE_1);
        SG_ASSERT(f <= FE_LINE_5_OR_MORE);
        features[*it].Set(f);

        const int pos = std::min(10, bd.Pos(*it));
        FeBasicFeature fp = ComputeFeature(FE_POS_1, 1, pos);
        SG_ASSERT(fp >= FE_POS_1);
        SG_ASSERT(fp <= FE_POS_10);
        features[*it].Set(fp);
    }
}

void FindSideExtensionFeatures(const GoBoard& bd,
                        GoEvalArray<FeMoveFeatures>& features)
{
    const int MAX_BONUS = 20;
    std::vector<GoOpeningKnowledge::MoveBonusPair>
         extensions(GoOpeningKnowledge::FindSideExtensions(bd));
    for (std::vector<GoOpeningKnowledge::MoveBonusPair>::const_iterator it
        = extensions.begin(); it != extensions.end(); ++it)
    {
        const SgPoint p = it->first;
        const int bonus = std::min(MAX_BONUS, it->second);
        SG_ASSERT(bonus >= 3);
        FeBasicFeature f = ComputeFeature(FE_SIDE_EXTENSION_3, 3, bonus);
        features[p].Set(f);
    }
}

const int EDGE_START_INDEX_3x3 = 1000;
const int NU_2x3_EDGE_FEATURES = 135;
const int CENTER_START_INDEX_3x3 = 1200;
const int NU_3x3_CENTER_FEATURES = 954;
    
bool Is2x3EdgeID(int id)
{
    return id >= EDGE_START_INDEX_3x3
    && id < EDGE_START_INDEX_3x3 + NU_2x3_EDGE_FEATURES;
}

bool Is3x3CenterID(int id)
{
    return id >= CENTER_START_INDEX_3x3
        && id < CENTER_START_INDEX_3x3 + NU_3x3_CENTER_FEATURES;
}

inline int Find2x3EdgeFeature(const GoBoard& bd, SgPoint move)
{
    int code = Go3x3Pattern::CodeOfEdgeNeighbors(bd, move);
    code = Go3x3Pattern::Map2x3EdgeCode(code, bd.ToPlay());
    SG_ASSERT(EDGE_START_INDEX_3x3 + code < CENTER_START_INDEX_3x3);
    return EDGE_START_INDEX_3x3 + code;
}

inline int Find3x3CenterFeature(const GoBoard& bd, SgPoint move)
{
    int code = Go3x3Pattern::CodeOf8Neighbors(bd, move);
    code = Go3x3Pattern::Map3x3CenterCode(code, bd.ToPlay());
    SG_ASSERT(code < 1000);
    return CENTER_START_INDEX_3x3 + code;
}

inline int Find3x3Feature(const GoBoard& bd, SgPoint p)
{
    return bd.Pos(p) == 1  ? INVALID_PATTERN_INDEX
         : bd.Line(p) == 1 ? Find2x3EdgeFeature(bd, p)
                           : Find3x3CenterFeature(bd, p);
}

void Write3x3(std::ostream& stream, int index)
{
    SG_ASSERT(index != INVALID_PATTERN_INDEX);
    if (index < CENTER_START_INDEX_3x3)
        Go3x3Pattern::Write2x3EdgePattern(stream,
            Go3x3Pattern::DecodeEdgeIndex(index - EDGE_START_INDEX_3x3));
    else
        Go3x3Pattern::Write3x3CenterPattern(stream,
            Go3x3Pattern::DecodeCenterIndex(index - CENTER_START_INDEX_3x3));
}

int Distance(SgPoint p1, SgPoint p2)
{
    SG_ASSERT(! SgIsSpecialMove(p1));
    SG_ASSERT(! SgIsSpecialMove(p2));
    int dx = abs(SgPointUtil::Col(p1) - SgPointUtil::Col(p2));
    int dy = abs(SgPointUtil::Row(p1) - SgPointUtil::Row(p2));
    return dx + dy + std::max(dx, dy);
}

void SetDistancesLastMove(const SgPoint lastMove,
                          const GoPointList& legalBoardMoves,
                          GoEvalArray<FeMoveFeatures>& features)
{
    for (GoPointList::Iterator it(legalBoardMoves); it; ++it)
    {
        const int distance = Distance(*it, lastMove);
        SG_ASSERT(distance >= 2);
        if (distance <= 17)
        {
            FeBasicFeature f = ComputeFeature(FE_DIST_PREV_2, 2, distance);
            features[*it].Set(f);
        }
    }
}
    
void SetDistances2ndLastMove(const SgPoint lastMove2,
                          const GoPointList& legalBoardMoves,
                          GoEvalArray<FeMoveFeatures>& features)
{
    for (GoPointList::Iterator it(legalBoardMoves); it; ++it)
    {
        const int distance = Distance(*it, lastMove2);
        if (distance == 0)
            features[*it].Set(FE_DIST_PREV_OWN_0);
        else if (distance <= 17)
        {
            SG_ASSERT(distance >= 2);
            FeBasicFeature f =
            ComputeFeature(FE_DIST_PREV_OWN_2, 2, distance);
            features[*it].Set(f);
        }
    }
}

void FindDistPrevMoveFeatures(const GoBoard& bd,
                              const GoPointList& legalBoardMoves,
                              GoEvalArray<FeMoveFeatures>& features)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (! SgIsSpecialMove(lastMove))
        SetDistancesLastMove(lastMove, legalBoardMoves, features);

    const SgPoint lastMove2 = bd.Get2ndLastMove();
    if (! SgIsSpecialMove(lastMove2))
        SetDistances2ndLastMove(lastMove2, legalBoardMoves, features);
}

int FindClosestStoneDistance(const GoBoard& bd,
                             SgPoint empty, SgBlackWhite color)
{
    int distance = 99999;

    for (GoBoard::Iterator it(bd); it; ++it) // @todo: slow, loop from
                                             // closest to furtherst instead
        if (  bd.GetColor(*it) == color
           && Distance(empty, *it) < distance)
        {
            distance = Distance(empty, *it);
        }

    SG_ASSERT(distance < 99999);
    return distance;
}

void FindClosestDistanceFeaturesForColor(const GoBoard& bd,
                                         SgBlackWhite color,
                                         const GoPointList& legalBoardMoves,
                                         GoEvalArray<FeMoveFeatures>& features,
                                         FeBasicFeature baseFeature)
{
    if (bd.All(color).IsEmpty())
        return;

    for (GoPointList::Iterator it(legalBoardMoves); it; ++it)
    {
        int distance = FindClosestStoneDistance(bd, *it, color);
        SG_ASSERT(distance >= 2);
        if (distance > MAX_CLOSEST_DISTANCE)
            distance = MAX_CLOSEST_DISTANCE;
        const FeBasicFeature f = ComputeFeature(baseFeature, 2, distance);
        features[*it].Set(f);
    }
}

void FindClosestDistanceFeatures(const GoBoard& bd,
                                 const GoPointList& legalBoardMoves,
                                 GoEvalArray<FeMoveFeatures>& features)
{
    FindClosestDistanceFeaturesForColor(bd, bd.ToPlay(),
                                   legalBoardMoves, features,
                                   FE_DIST_CLOSEST_OWN_STONE_2);
    FindClosestDistanceFeaturesForColor(bd, bd.Opponent(),
                                        legalBoardMoves, features,
                                        FE_DIST_CLOSEST_OPP_STONE_2);
}

} // namespace

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, FeBasicFeature f)
{
    static const char* s_string[_NU_FE_FEATURES] =
    {
        "FE_PASS_NEW",            // pass, previous move was not pass
        "FE_PASS_CONSECUTIVE",    // pass, previous move was also pass
        "FE_CAPTURE_ADJ_ATARI",   // String contiguous to new string in atari
        "FE_CAPTURE_RECAPTURE",   // Re-capture previous move
        "FE_CAPTURE_PREVENT_CONNECTION", // Prevent connection to prev. move
        "FE_CAPTURE_NOT_LADDER",  // String not in a ladder
        "FE_CAPTURE_LADDER",      // String in a ladder
        "FE_CAPTURE_MULTIPLE",
    // FE_CAPTURE_SNAPBACK,    // set up a capture using a snapback
        "FE_EXTENSION_NOT_LADDER", // New atari, not in a ladder
        "FE_EXTENSION_LADDER",    // New atari, in a ladder
                                  // todo distinguish extending 1 stone only?
        "FE_TWO_LIB_SAVE_LADDER", // save own 2 lib block from ladder capture
        "FE_TWO_LIB_STILL_LADDER", // block remains captured even when playing
                                   // this liberty
        "FE_TWO_LIB_SELF_LADDER", // block was safe but becomes unsettled
        "FE_THREE_LIB_REDUCE_OWN_LIB",
        "FE_THREE_LIB_REDUCE_OPP_LIB",
        "FE_SELFATARI",
        // "FE_SELFATARI_NAKADE",
        // "FE_SELFATARI_THROWIN",
        "FE_ATARI_LADDER",        // Ladder atari
        "FE_ATARI_KO",            // Atari when there is a ko
        "FE_ATARI_OTHER",         // Other atari
        "FE_DOUBLE_ATARI",        // atari two or more opponent stones
        "FE_DOUBLE_ATARI_DEFEND", // prevent double atari move, e.g. connect
        "FE_LINE_1",
        "FE_LINE_2",
        "FE_LINE_3",
        "FE_LINE_4",
        "FE_LINE_5+",
        "FE_DIST_PREV_2", // d(dx,dy) = |dx|+|dy|+max(|dx|,|dy|)
        "FE_DIST_PREV_3",
        "FE_DIST_PREV_4",
        "FE_DIST_PREV_5",
        "FE_DIST_PREV_6",
        "FE_DIST_PREV_7",
        "FE_DIST_PREV_8",
        "FE_DIST_PREV_9",
        "FE_DIST_PREV_10",
        "FE_DIST_PREV_11",
        "FE_DIST_PREV_12",
        "FE_DIST_PREV_13",
        "FE_DIST_PREV_14",
        "FE_DIST_PREV_15",
        "FE_DIST_PREV_16",
        "FE_DIST_PREV_17",
        "FE_DIST_PREV_OWN_0", // play back in at same point after capture
        "FE_DIST_PREV_OWN_2",
        "FE_DIST_PREV_OWN_3",
        "FE_DIST_PREV_OWN_4",
        "FE_DIST_PREV_OWN_5",
        "FE_DIST_PREV_OWN_6",
        "FE_DIST_PREV_OWN_7",
        "FE_DIST_PREV_OWN_8",
        "FE_DIST_PREV_OWN_9",
        "FE_DIST_PREV_OWN_10",
        "FE_DIST_PREV_OWN_11",
        "FE_DIST_PREV_OWN_12",
        "FE_DIST_PREV_OWN_13",
        "FE_DIST_PREV_OWN_14",
        "FE_DIST_PREV_OWN_15",
        "FE_DIST_PREV_OWN_16",
        "FE_DIST_PREV_OWN_17",
        "FE_GOUCT_FILLBOARD",
        "FE_GOUCT_NAKADE",
        "FE_GOUCT_ATARI_CAPTURE",
        "FE_GOUCT_ATARI_DEFEND",
        "FE_GOUCT_LOWLIB",
        "FE_GOUCT_PATTERN",
        "FE_GOUCT_CAPTURE",
        "FE_GOUCT_RANDOM_PRUNED",
        "FE_GOUCT_REPLACE_CAPTURE_FROM",
        "FE_GOUCT_REPLACE_CAPTURE_TO",
        "FE_GOUCT_SELFATARI_CORRECTION_FROM",
        "FE_GOUCT_SELFATARI_CORRECTION_TO",
        "FE_GOUCT_CLUMP_CORRECTION_FROM",
        "FE_GOUCT_CLUMP_CORRECTION_TO",
        "FE_POS_1", // Position of a point p according to GoBoard::Pos(p)
        "FE_POS_2",
        "FE_POS_3",
        "FE_POS_4",
        "FE_POS_5",
        "FE_POS_6",
        "FE_POS_7",
        "FE_POS_8",
        "FE_POS_9",
        "FE_POS_10",
        "FE_GAME_PHASE_1", // Game phase as in Wistuba - 30 moves per phase
        "FE_GAME_PHASE_2",
        "FE_GAME_PHASE_3",
        "FE_GAME_PHASE_4",
        "FE_GAME_PHASE_5",
        "FE_GAME_PHASE_6",
        "FE_GAME_PHASE_7",
        "FE_GAME_PHASE_8",
        "FE_GAME_PHASE_9",
        "FE_GAME_PHASE_10",
        "FE_GAME_PHASE_11",
        "FE_GAME_PHASE_12",
        "FE_SIDE_EXTENSION_3",
        "FE_SIDE_EXTENSION_4",
        "FE_SIDE_EXTENSION_5",
        "FE_SIDE_EXTENSION_6",
        "FE_SIDE_EXTENSION_7",
        "FE_SIDE_EXTENSION_8",
        "FE_SIDE_EXTENSION_9",
        "FE_SIDE_EXTENSION_10",
        "FE_SIDE_EXTENSION_11",
        "FE_SIDE_EXTENSION_12",
        "FE_SIDE_EXTENSION_13",
        "FE_SIDE_EXTENSION_14",
        "FE_SIDE_EXTENSION_15",
        "FE_SIDE_EXTENSION_16",
        "FE_SIDE_EXTENSION_17",
        "FE_SIDE_EXTENSION_18",
        "FE_SIDE_EXTENSION_19",
        "FE_SIDE_EXTENSION_20",
        "FE_CORNER_OPENING_MOVE",
        "FE_CFG_DISTANCE_LAST_1",
        "FE_CFG_DISTANCE_LAST_2",
        "FE_CFG_DISTANCE_LAST_3",
        "FE_CFG_DISTANCE_LAST_4_OR_MORE",
        "FE_CFG_DISTANCE_LAST_OWN_0",
        "FE_CFG_DISTANCE_LAST_OWN_1",
        "FE_CFG_DISTANCE_LAST_OWN_2",
        "FE_CFG_DISTANCE_LAST_OWN_3",
        "FE_CFG_DISTANCE_LAST_OWN_4_OR_MORE",
        "FE_TWO_LIB_NEW_SELF_LADDER", // new 2-lib block ladder unsafe
        "FE_OUR_PROTECTED_LIBERTY", // opponent could be captured there
        "FE_OPP_PROTECTED_LIBERTY", // we would be captured there
        "FE_OUR_CUT_WITH_KO", // can cut to start a ko here.
        "FE_OPP_CUT_WITH_KO", // can cut to start a ko here.
        "FE_SAVE_STONES_1", // save a single stone
        "FE_SAVE_STONES_2",
        "FE_SAVE_STONES_3",
        "FE_SAVE_STONES_4_6",
        "FE_SAVE_STONES_7_10",
        "FE_SAVE_STONES_11_20",
        "FE_SAVE_STONES_21_OR_MORE",
        "FE_KILL_STONES_1", // kill a single stones
        "FE_KILL_STONES_2",
        "FE_KILL_STONES_3",
        "FE_KILL_STONES_4_6",
        "FE_KILL_STONES_7_10",
        "FE_KILL_STONES_11_20",
        "FE_KILL_STONES_21_OR_MORE",
        "FE_KILL_OWN_STONES_1", // a single own stone can be killed after this move
        "FE_KILL_OWN_STONES_2",
        "FE_KILL_OWN_STONES_3",
        "FE_KILL_OWN_STONES_4_6",
        "FE_KILL_OWN_STONES_7_10",
        "FE_KILL_OWN_STONES_11_20",
        "FE_KILL_OWN_STONES_21_OR_MORE",
        "FE_SNAPBACK", // move captures single stone, opponent can snap back
        "FE_SAFE_TERRITORY_OWN_NO_KO", // play in own territory, no active ko
        "FE_SAFE_TERRITORY_OWN_KO",
        "FE_SAFE_TERRITORY_OPP_NO_KO",
        "FE_SAFE_TERRITORY_OPP_KO", // play in opponent territory, active ko
        "FE_POSSIBLE_SEMEAI",
        "FE_CUT",
        "FE_CONNECT",
        "FE_DIST_CLOSEST_OWN_STONE_2",
        "FE_DIST_CLOSEST_OWN_STONE_3",
        "FE_DIST_CLOSEST_OWN_STONE_4",
        "FE_DIST_CLOSEST_OWN_STONE_5",
        "FE_DIST_CLOSEST_OWN_STONE_6",
        "FE_DIST_CLOSEST_OWN_STONE_7",
        "FE_DIST_CLOSEST_OWN_STONE_8",
        "FE_DIST_CLOSEST_OWN_STONE_9",
        "FE_DIST_CLOSEST_OWN_STONE_10",
        "FE_DIST_CLOSEST_OWN_STONE_11",
        "FE_DIST_CLOSEST_OWN_STONE_12",
        "FE_DIST_CLOSEST_OWN_STONE_13",
        "FE_DIST_CLOSEST_OWN_STONE_14",
        "FE_DIST_CLOSEST_OWN_STONE_15",
        "FE_DIST_CLOSEST_OWN_STONE_16",
        "FE_DIST_CLOSEST_OWN_STONE_17",
        "FE_DIST_CLOSEST_OWN_STONE_18",
        "FE_DIST_CLOSEST_OWN_STONE_19",
        "FE_DIST_CLOSEST_OWN_STONE_20_OR_MORE",
        "FE_DIST_CLOSEST_OPP_STONE_2",
        "FE_DIST_CLOSEST_OPP_STONE_3",
        "FE_DIST_CLOSEST_OPP_STONE_4",
        "FE_DIST_CLOSEST_OPP_STONE_5",
        "FE_DIST_CLOSEST_OPP_STONE_6",
        "FE_DIST_CLOSEST_OPP_STONE_7",
        "FE_DIST_CLOSEST_OPP_STONE_8",
        "FE_DIST_CLOSEST_OPP_STONE_9",
        "FE_DIST_CLOSEST_OPP_STONE_10",
        "FE_DIST_CLOSEST_OPP_STONE_11",
        "FE_DIST_CLOSEST_OPP_STONE_12",
        "FE_DIST_CLOSEST_OPP_STONE_13",
        "FE_DIST_CLOSEST_OPP_STONE_14",
        "FE_DIST_CLOSEST_OPP_STONE_15",
        "FE_DIST_CLOSEST_OPP_STONE_16",
        "FE_DIST_CLOSEST_OPP_STONE_17",
        "FE_DIST_CLOSEST_OPP_STONE_18",
        "FE_DIST_CLOSEST_OPP_STONE_19",
        "FE_DIST_CLOSEST_OPP_STONE_20_OR_MORE",
        "FE_NONE"
    };
    SG_ASSERT(f >= FE_PASS_NEW);
    SG_ASSERT(f < _NU_FE_FEATURES);
    stream << s_string[f];
    return stream;
}

//----------------------------------------------------------------------------

std::ostream& FeFeatures::operator<<(std::ostream& stream,
                         const FeFeatures::FeEvalDetail& f)
{
    stream << '(';
    WriteFeatureFromID(stream, f.m_feature);
    stream << ", w = " << std::setprecision(2) << f.m_w
           << ", v_sum = " << f.m_v_sum << ')';
    return stream;
}

//----------------------------------------------------------------------------

float FeFeatures::EvaluateActiveFeatures(const FeActiveArray& active,
                                         size_t nuActive,
                                         const FeFeatureWeights& weights)
{
    float value = 0.0;
    for (FeActiveIterator it = active.begin();
                          it < active.begin() + nuActive; ++it)
    {
        value += weights.m_w[*it];
        for (FeActiveIterator it2 = it + 1;
                              it2 < active.begin() + nuActive; ++it2)
            value += weights.Combine(*it, *it2);
    }
    return value;
}

float FeFeatures::EvaluateMoveFeatures(const FeMoveFeatures& features,
                                       const FeFeatureWeights& weights)
{
    FeActiveArray active;
    const size_t nuActive = features.ActiveFeatures(active);
    return EvaluateActiveFeatures(active, nuActive, weights);
}

std::vector<FeFeatures::FeEvalDetail>
FeFeatures::EvaluateMoveFeaturesDetail(const FeMoveFeatures& features,
                                       const FeFeatureWeights& weights)
{
    FeActiveArray active;
    const size_t nuActive = features.ActiveFeatures(active);
    std::vector<FeFeatures::FeEvalDetail> detail;
    for (FeActiveIterator it = active.begin();
                          it < active.begin() + nuActive; ++it)
    {
        const bool outOfBounds =
            static_cast<size_t>(*it) >= weights.m_w.size();
        if (outOfBounds)
        {
            //SgDebug() << "EvaluateMoveFeaturesDetail: skipping feature "
            //          << *it << '\n';
        }
        const float w = outOfBounds ? 0 : weights.m_w[*it];
        float v = 0.0;
        if (! outOfBounds)
            for (FeActiveIterator it2 = active.begin();
                                  it2 < active.begin() + nuActive; ++it2)
                if (it != it2)
                    v += weights.Combine(*it, *it2);
        detail.push_back(FeFeatures::FeEvalDetail(*it, w, v/2));
    }
    return detail;
}

void FeFeatures::FindBasicMoveFeaturesUI(const GoBoard& bd, SgPoint move,
                                         FeBasicFeatureSet& features)
{
    FeFullBoardFeatures f(bd);
    f.FindAllFeatures();
    features = f.BasicFeatures(move);
}

void FeFeatures::FindPassFeatures(const GoBoard& bd,
                                  FeBasicFeatureSet& features)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (lastMove == SG_PASS)
        features.set(FE_PASS_CONSECUTIVE);
    else
        features.set(FE_PASS_NEW);
}

int FeFeatures::Get3x3Feature(const GoBoard& bd, SgPoint p)
{
    return Find3x3Feature(bd, p);
}

bool FeFeatures::IsBasicFeatureID(int id)
{
    return id >= 0 && id < _NU_FE_FEATURES;
}
    
bool FeFeatures::Is3x3PatternID(int id)
{
    return Is2x3EdgeID(id) || Is3x3CenterID(id);
}

void FeFeatures::WriteEvalDetail(std::ostream& stream,
                     const std::vector<FeEvalDetail>& detail)
{
    float w = 0;
    float v = 0;
    for (std::vector<FeFeatures::FeEvalDetail>::const_iterator it
         = detail.begin(); it != detail.end(); ++it)
    {
        stream << *it << '\n';
        w += (*it).m_w;
        v += (*it).m_v_sum;
    }
    stream << " Total w = " << w << " + v = " << v << " = " << w + v << '\n';
}

static bool IsPattern12PointID(int id) // @todo
{
    SG_UNUSED(id);
    return true;
}

void FeFeatures::WriteFeatureFromID(std::ostream& stream, int id)
{
    if (IsBasicFeatureID(id))
        stream << static_cast<FeBasicFeature>(id);
    else if (Is3x3PatternID(id))
        Write3x3(stream, id);
    else if (IsPattern12PointID(id))
        GoPattern12Point::PrintContext(id, stream);
    else
        SG_ASSERT(false);
}

void FeFeatures::WriteFeatureSet(std::ostream& stream,
                                 SgPoint move,
                                 const FeBasicFeatureSet& features)
{
    stream << SgWritePoint(move);
    for (int f = FE_PASS_NEW; f < _NU_FE_FEATURES; ++f)
        if (features.test(f))
            stream << ' ' << f;
}

void FeFeatures::WriteFeatureSetAsText(std::ostream& stream,
                                       SgPoint move,
                                       const FeBasicFeatureSet& features)
{
    stream << SgWritePoint(move);
    for (int f = FE_PASS_NEW; f < _NU_FE_FEATURES; ++f)
    {
        if (features.test(f))
            stream << ' ' << static_cast<FeBasicFeature>(f);
    }
}

//-------------------------------------
    

namespace {
    
void WriteFeatureSetNumeric(std::ostream& stream,
                     const FeBasicFeatureSet& features)
{
    for (int f = FE_PASS_NEW; f < _NU_FE_FEATURES; ++f)
        if (features.test(f))
            stream << ' ' << f;
}
} // namespace

//----------------------------------------------------------------------------

size_t FeMoveFeatures::ActiveFeatures(FeActiveArray& active) const
{
    size_t nuActive = 0;
    for (int i = 0; i < _NU_FE_FEATURES; ++i)
        if (m_basicFeatures.test(i))
        {
            active[nuActive] = i;
            if (++nuActive >= MAX_ACTIVE_LENGTH)
                return nuActive;
        }
    // invalid for pass move and (1,1) points
    if (m_3x3Index != INVALID_PATTERN_INDEX)
    {
        SG_ASSERT(nuActive < MAX_ACTIVE_LENGTH);
        active[nuActive] = m_3x3Index;
        ++nuActive;
    }
    if (m_12PointIndex != INVALID_PATTERN_INDEX)
    {
        SG_ASSERT(nuActive < MAX_ACTIVE_LENGTH);
        active[nuActive] = m_12PointIndex;
        ++nuActive;
    }
    return nuActive;
}

void FeMoveFeatures::FindMoveFeatures(const GoBoard& bd, SgPoint move)
{
    SG_ASSERT(move != SG_PASS);
    m_3x3Index = Find3x3Feature(bd, move);
}

void FeMoveFeatures::FindPassFeatures(const GoBoard& bd)
{
    FeFeatures::FindPassFeatures(bd, m_basicFeatures);
}

void FeMoveFeatures::WriteNumeric(std::ostream& stream,
                                  const int isChosen,
                                  const int moveNumber,
                                  const bool writeComment) const
{
    const int SHAPE_SIZE = 3; // TODO make this variable
                              // when big pattern features are implemented
    stream << isChosen;
    WriteFeatureSetNumeric(stream, m_basicFeatures);
    WritePatternFeatureIndex(stream);
    if (writeComment)
        stream << " #0_" << moveNumber << ' ' << SHAPE_SIZE;
    stream << '\n';
}

void FeMoveFeatures::WritePatternFeatureIndex(std::ostream& stream) const
{
    if (m_3x3Index != INVALID_PATTERN_INDEX)
        stream << ' ' << m_3x3Index;
    if (m_12PointIndex != INVALID_PATTERN_INDEX)
        stream << ' ' << m_12PointIndex;
}

void FeMoveFeatures::WritePatternFeatures(std::ostream& stream) const
{
    if (m_12PointIndex != INVALID_PATTERN_INDEX)
    {
        stream << " m_12PointIndex " << m_12PointIndex;
        GoPattern12Point::PrintContext(m_12PointIndex, stream);
    }
    if (m_3x3Index != INVALID_PATTERN_INDEX)
    {
        stream << " 3x3-index " << m_3x3Index;
        Write3x3(stream, m_3x3Index);
    }
}

void FeMoveFeatures::WriteFeatures(std::ostream& stream,
                                   SgPoint move) const
{
    FeFeatures::WriteFeatureSetAsText(stream, move, m_basicFeatures);
    WritePatternFeatures(stream);
    stream << '\n';
}

//----------------------------------------------------------------------------
GoEvalArray<float> FeFullBoardFeatures::
EvaluateFeatures(const FeFeatureWeights& weights) const
{
    GoEvalArray<float> eval(0);
    for (GoPointList::Iterator it(m_legalMoves); it; ++it)
            eval[*it] = FeFeatures::EvaluateMoveFeatures(m_features[*it], weights);
    eval[SG_PASS] = FeFeatures::EvaluateMoveFeatures(m_features[SG_PASS], weights);
    return eval;
}

void FeFullBoardFeatures::FindAllFeatures()
{
    for (GoPointList::Iterator it(m_legalMoves); it; ++it)
    {
        SG_ASSERT(m_bd.IsLegal(*it));
        m_features[*it].FindMoveFeatures(m_bd, *it);
    }
    m_features[SG_PASS].FindPassFeatures(m_bd);
    FindFullBoardFeatures();
}

void FeFullBoardFeatures::FindFullBoardFeatures()
{
    if (m_bd.Size() >= 15)
    {
        FindCornerMoveFeatures(m_bd, m_features);
        FindSideExtensionFeatures(m_bd, m_features);
    }
    FindGamePhaseFeature(m_bd, m_legalMoves, m_features);
    FindDistPrevMoveFeatures(m_bd, m_legalMoves, m_features);
    FindLinePosFeatures(m_bd, m_legalMoves, m_features);
    FindCfgFeatures(m_bd, m_legalMoves, m_features);
    FindLadderFeatures(m_bd, m_legalMoves, m_features);
    FindClosestDistanceFeatures(m_bd, m_legalMoves, m_features);
    if (m_bd.MoveNumber() >= m_bd.Size() * m_bd.Size() * 0.5)
        FindSafeTerritoryFeatures(m_bd, m_legalMoves, m_features);
}

void FeFullBoardFeatures::WriteBoardFeatures(std::ostream& stream) const
{
    for (GoPointList::Iterator it(m_legalMoves); it; ++it)
    {
        SG_ASSERT(m_bd.IsLegal(*it));
        m_features[*it].WriteFeatures(stream, *it);
    }
    m_features[SG_PASS].WriteFeatures(stream, SG_PASS);
}

void FeFullBoardFeatures::WriteNumeric(std::ostream& stream,
             const SgPoint chosenMove,
             const bool writeComment) const
{
    const int moveNumber = m_bd.MoveNumber() + 1; // + 1 because we did undo
    for (GoPointList::Iterator it(m_legalMoves); it; ++it)
    {
        SG_ASSERT(m_bd.IsLegal(*it));
        if (*it != chosenMove)
            m_features[*it].WriteNumeric(stream, 0, moveNumber, writeComment);
    }
    if (SG_PASS != chosenMove)
        m_features[SG_PASS].WriteNumeric(stream, 0, moveNumber, writeComment);
    m_features[chosenMove].WriteNumeric(stream, 1, moveNumber, writeComment);
}

//----------------------------------------------------------------------------
