//----------------------------------------------------------------------------
/** @file FeBasicFeatures.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeBasicFeatures.h"

#include "FePatternBase.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoLadder.h"
#include "GoSetupUtil.h"
#include "GoUctPatterns.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------
namespace {

void FindPassFeatures(const GoBoard& bd, FeBasicFeatureSet& features)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (lastMove == SG_PASS)
        features.set(FE_PASS_CONSECUTIVE);
    else
        features.set(FE_PASS_NEW);
}

bool PutUsIntoAtari(const GoBoard& bd, SgPoint lastMove)
{
    SG_ASSERT(! SgIsSpecialMove(lastMove));
    GoAdjBlockIterator<GoBoard> it(bd, lastMove, 1);
    return it;
}

// @todo  NEED TO MAKE SURE that move is evaluated with last move info intact.

// faster to globally find all such moves.
void FindCaptureFeatures(const GoBoard& bd, SgPoint move,
                         FeBasicFeatureSet& features)
{
    const SgBlackWhite toPlay = bd.ToPlay();
    if (! bd.CanCapture(move, toPlay))
        return;

    FeBasicFeature f = FE_NONE;
    SgPoint oppInAtari[4 + 1];
    const SgBlackWhite opp = SgOppBW(toPlay);
    bd.NeighborBlocks(move, opp, 1, oppInAtari);
    // TODO what to do if multiple preys? FE_CAPTURE_MULTIPLE?
    const SgPoint lastMove = bd.GetLastMove();
    if (! SgIsSpecialMove(lastMove))
    {
        // assume no suicide, last move is occupied
        SG_ASSERT(bd.IsColor(lastMove, opp));
        if (PutUsIntoAtari(bd, lastMove))
            f = FE_CAPTURE_ADJ_ATARI;
        if (f == FE_NONE && bd.CapturingMove() && bd.InAtari(lastMove))
            f = FE_CAPTURE_RECAPTURE;
        if (f == FE_NONE && bd.IsLibertyOfBlock(move, bd.Anchor(lastMove))
            && true // TODO use oppInAtari?
            )
            // move is liberty of both lastmove and prey)
            f = FE_CAPTURE_PREVENT_CONNECTION;
    }
    if (f != FE_NONE)
        features.set(f);
    else
    {
        for (int i = 0; oppInAtari[i] != SG_ENDPOINT; ++i)
        {
            const GoLadderStatus ls =
                  GoLadderUtil::LadderStatus(bd, oppInAtari[i]);
            f = (ls == GO_LADDER_CAPTURED) ? FE_CAPTURE_LADDER
                                           : FE_CAPTURE_NOT_LADDER;
            features.set(f);
        }
    }
}

/*
GoLadderStatus LadderStatus(const GoBoard& bd, SgPoint prey,
                            bool fTwoLibIsEscape = false,
                            SgPoint* toCapture = 0, SgPoint* toEscape = 0);
*/

void FindExtensionFeatures(const GoBoard& bd, SgPoint move,
                           FeBasicFeatureSet& features)
{
    const SgBlackWhite toPlay = bd.ToPlay();
    for (GoNeighborBlockIterator it(bd, move, toPlay, 1); it; ++it)
    {
        GoLadderStatus ls = GoLadderUtil::LadderStatus(bd, *it);
        if (ls == GO_LADDER_CAPTURED)
            features.set(FE_EXTENSION_LADDER);
        else
            features.set(FE_EXTENSION_NOT_LADDER);
    }
}

void FindSelfatariFeatures(const GoBoard& bd, SgPoint move,
                           FeBasicFeatureSet& features)
{
    if (GoBoardUtil::SelfAtari(bd, move))
        features.set(FE_SELFATARI);
 // @todo FE_SELFATARI_NAKADE, FE_SELFATARI_THROWIN
}

// TODO check that move is not SelfAtari first??
bool GivesAtari(const GoBoard& bd, SgPoint move)
{
    SG_ASSERT(! SgIsSpecialMove(move));
    const SgBlackWhite opp = bd.Opponent();
    for (GoNeighborBlockIterator it(bd, move, opp, 2); it; ++it)
        if (bd.NumLiberties(*it) == 2)
            return true;
    return false;
}

bool IsLadderCaptureMove(const GoBoard& bd, SgPoint move)
{
    SG_ASSERT(! SgIsSpecialMove(move));
    const SgBlackWhite opp = bd.Opponent();
    for (GoNeighborBlockIterator it(bd, move, opp, 2); it; ++it)
        if (  bd.NumLiberties(*it) == 2
           && GoLadderUtil::IsLadderCaptureMove(bd, *it, move)
           )
            return true;
    return false;
}

void FindAtariFeatures(const GoBoard& bd, SgPoint move,
                       FeBasicFeatureSet& features)
{
    //FE_ATARI_LADDER,        // Ladder atari
    //FE_ATARI_KO,            // Atari when there is a ko
    //FE_ATARI_OTHER,         // Other atari
    if (GivesAtari(bd, move))
    {
        if (IsLadderCaptureMove(bd, move))
            features.set(FE_ATARI_LADDER);
        if (bd.KoPoint() != SG_NULLPOINT)
            features.set(FE_ATARI_KO);
        if (  ! features.test(FE_ATARI_LADDER)
           && ! features.test(FE_ATARI_KO))
            features.set(FE_ATARI_OTHER);
    }
}

void FindLineFeatures(const GoBoard& bd, SgPoint move, FeBasicFeatureSet& features)
{
    FeBasicFeature f = FE_NONE;
    switch (bd.Line(move))
    {
        case 1: f = FE_LINE_1;
            break;
        case 2: f = FE_LINE_2;
            break;
        case 3: f = FE_LINE_3;
            break;
        case 4: f = FE_LINE_4;
            break;
        default:
            break;
    }
    if (f != FE_NONE)
        features.set(f);
}

const int CORNER_INDEX_3x3 = 1000; // we don't have features on Pt(1,1)
const int EDGE_START_INDEX_3x3 = 1001;
const int CENTER_START_INDEX_3x3 = 2000;

inline int Find2x3EdgeFeature(const GoBoard& bd, SgPoint move)
{
    return EDGE_START_INDEX_3x3
         + GoUctPatterns<GoBoard>::CodeOfEdgeNeighbors(bd, move);
}

inline int Find3x3CenterFeature(const GoBoard& bd, SgPoint move)
{
    return CENTER_START_INDEX_3x3
         + GoUctPatterns<GoBoard>::CodeOf8Neighbors(bd, move);
}

inline int Find3x3Feature(const GoBoard& bd, SgPoint p)
{
    return bd.Pos(p) == 1  ? CORNER_INDEX_3x3
         : bd.Line(p) == 1 ? Find2x3EdgeFeature(bd, p)
                           : Find3x3CenterFeature(bd, p);
}

int Distance(SgPoint p1, SgPoint p2)
{
    SG_ASSERT(! SgIsSpecialMove(p1));
    SG_ASSERT(! SgIsSpecialMove(p2));
    int dx = abs(SgPointUtil::Col(p1) - SgPointUtil::Col(p2));
    int dy = abs(SgPointUtil::Row(p1) - SgPointUtil::Row(p2));
    return dx + dy + std::max(dx, dy);
}

inline FeBasicFeature ComputeFeature(FeBasicFeature baseFeature,
                                int baseValue, int value)
{
    return static_cast<FeBasicFeature>(static_cast<int>(baseFeature)
                                  + value - baseValue);
}
    

void FindDistPrevMoveFeatures(const GoBoard& bd, SgPoint move, FeBasicFeatureSet& features)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (! SgIsSpecialMove(lastMove))
    {
        int distance = Distance(move, lastMove);
        SG_ASSERT(distance >= 2);
        if (distance <= 17)
        {
            FeBasicFeature f = ComputeFeature(FE_DIST_PREV_2, 2, distance);
            features.set(f);
        }
    }

    const SgPoint lastMove2 = bd.Get2ndLastMove();
    if (! SgIsSpecialMove(lastMove2))
    {
        int distance = Distance(move, lastMove2);
        SG_ASSERT(distance >= 2);
        if (distance <= 17)
        {
            FeBasicFeature f = ComputeFeature(FE_DIST_PREV_OWN_2, 2, distance);
            features.set(f);
        }
    }
}

int NuWins()
{
    return 42; // TODO
}

void FindMCOwnerFeatures(const GoBoard& bd, SgPoint move, FeBasicFeatureSet& features)
{
    // TODO run 63 simulations
    SG_UNUSED(bd);
    SG_UNUSED(move);
    
    FeBasicFeature f = FE_NONE;
    int n = NuWins() / 8;
    switch(n)
    {
        case 0: f = FE_MC_OWNER_1;
            break;
        case 1: f = FE_MC_OWNER_2;
            break;
        case 2: f = FE_MC_OWNER_3;
            break;
        case 3: f = FE_MC_OWNER_4;
            break;
        case 4: f = FE_MC_OWNER_5;
            break;
        case 5: f = FE_MC_OWNER_6;
            break;
        case 6: f = FE_MC_OWNER_7;
            break;
        case 7: f = FE_MC_OWNER_8;
            break;
        default: SG_ASSERT(false);
            break;
    }
    if (f != FE_NONE)
        features.set(f);
}

} // namespace

std::ostream& operator<<(std::ostream& stream, FeBasicFeature f)
{
    static const char* s_string[_NU_FE_FEATURES] =
    {
        "FE_PASS_NEW",            // pass, previous move was not pass
        "FE_PASS_CONSECUTIVE",    // pass, previous move was also pass
        "FE_CAPTURE_ADJ_ATARI",   // String contiguous to new string in atari
        "FE_CAPTURE_RECAPTURE",   // Re-capture previous move
        "FE_CAPTURE_PREVENT_CONNECTION", // Prevent connection to previous move
        "FE_CAPTURE_NOT_LADDER",  // String not in a ladder
        "FE_CAPTURE_LADDER",      // String in a ladder
                                // "FE_CAPTURE_MULTIPLE",
        "FE_EXTENSION_NOT_LADDER", // New atari, not in a ladder
        "FE_EXTENSION_LADDER",    // New atari, in a ladder
                                // todo distinguish extending 1 stone only?
        "FE_SELFATARI",
        // "FE_SELFATARI_NAKADE",
        // "FE_SELFATARI_THROWIN",
        "FE_ATARI_LADDER",        // Ladder atari
        "FE_ATARI_KO",            // Atari when there is a ko
        "FE_ATARI_OTHER",         // Other atari
        "FE_LINE_1",
        "FE_LINE_2",
        "FE_LINE_3",
        "FE_LINE_4",
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
        "FE_MC_OWNER_1", // 0−7 wins/63 sim.
        "FE_MC_OWNER_2", // 8−15
        "FE_MC_OWNER_3", // 16−23
        "FE_MC_OWNER_4", // 24−31
        "FE_MC_OWNER_5", // 32−39
        "FE_MC_OWNER_6", // 40−47
        "FE_MC_OWNER_7", // 48−55
        "FE_MC_OWNER_8",  // 56−63
        "FE_NONE"
    };
    stream << s_string[f];
    return stream;
}

namespace FeFeatures {

void FindBasicMoveFeatures(const GoBoard& bd, SgPoint move,
                           FeBasicFeatureSet& features)
{
    if (move == SG_PASS)
    {
        FindPassFeatures(bd, features);
        return;
    }

    FindCaptureFeatures(bd, move, features);
    FindExtensionFeatures(bd, move, features);
    FindSelfatariFeatures(bd, move, features);
    FindAtariFeatures(bd, move, features);
    FindLineFeatures(bd, move, features);
    FindDistPrevMoveFeatures(bd, move, features);
    FindMCOwnerFeatures(bd, move, features);
}

void FindMoveFeatures(const GoBoard& bd, SgPoint move,
                      FeMoveFeatures& features)
{
    FindBasicMoveFeatures(bd, move, features.m_basicFeatures);
    if (move != SG_PASS)
        features.m_3x3Index = Find3x3Feature(bd, move);
}

void FindAllFeatures(const GoBoard& bd,
                          SgPointArray<FeMoveFeatures>& features,
                          FeMoveFeatures& passFeatures)
{
    for(GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsLegal(*it))
            FindMoveFeatures(bd, *it, features[*it]);
    FindMoveFeatures(bd, SG_PASS, passFeatures);
}

void WriteFeatureSet(std::ostream& stream,
                     SgPoint move,
                     const FeBasicFeatureSet& features)
{
    stream << SgWritePoint(move) << ' ';
    for (int f = FE_PASS_NEW; f < _NU_FE_FEATURES; ++f)
    {
        if (features.test(f))
            stream << ' ' << f;
    }
}

void WritePatternFeatures(std::ostream& stream,
                          const FeMoveFeatures& features)
{
    if (features.m_3x3Index != INVALID_3x3_INDEX)
        stream << ' ' << features.m_3x3Index;
}

void WriteFeatures(std::ostream& stream,
                   SgPoint move,
                   const FeMoveFeatures& features)
{
    WriteFeatureSet(stream, move, features.m_basicFeatures);
    WritePatternFeatures(stream, features);
    stream << '\n';
}

void WriteBoardFeatures(std::ostream& stream,
                        const SgPointArray<FeMoveFeatures>& features,
                        const GoBoard& bd)
{
    for (GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsLegal(*it))
            WriteFeatures(stream, *it, features[*it]);
}
    
namespace WistubaFormat {

void WriteFeatureSet(std::ostream& stream,
                     const FeBasicFeatureSet& features)
{
    for (int f = FE_PASS_NEW; f < _NU_FE_FEATURES; ++f)
        if (features.test(f))
            stream << ' ' << f;
}

void WriteFeatures(std::ostream& stream,
                   int isChosen,
                   const FeMoveFeatures& features,
                   int moveNumber,
                   bool writeComment)
{
    const int SHAPE_SIZE = 3; // TODO make this variable
                              // when big pattern features are implemented
    stream << isChosen;
    WriteFeatureSet(stream, features.m_basicFeatures);
    WritePatternFeatures(stream, features);
    if (writeComment)
    {
        stream << " #0_" << moveNumber << ' ' << SHAPE_SIZE;
    }
    stream << '\n';
}

void WriteBoardFeatures(std::ostream& stream,
                        const SgPointArray<FeMoveFeatures>& features,
                        const FeMoveFeatures& passFeatures,
                        const GoBoard& bd,
                        SgPoint chosenMove,
                        bool writeComment)
{
    const int moveNumber = bd.MoveNumber() + 1; // + 1 because we did undo
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        const SgPoint p = *it;
        if (p != chosenMove && bd.IsLegal(p))
            WriteFeatures(stream, 0, features[p], moveNumber, writeComment);
    }
    if (SG_PASS != chosenMove)
        WriteFeatures(stream, 0, passFeatures, moveNumber, writeComment);
    WriteFeatures(stream, 1, features[chosenMove], moveNumber, writeComment);
}

void WriteFeatures(std::ostream& stream, const GoBoard& constBd,
                   bool writeComment)
{
    SgPoint chosenMove = constBd.GetLastMove();
    GoModBoard mod(constBd);
    GoBoard& bd = mod.Board();
    bd.Undo();
    SgPointArray<FeMoveFeatures> features;
    FeMoveFeatures passFeatures;
    FeFeatures::FindAllFeatures(bd, features, passFeatures);
    if (chosenMove != SG_NULLMOVE)
        WriteBoardFeatures(stream, features, passFeatures,
                                  bd, chosenMove, writeComment);
    bd.Play(chosenMove);
}

} // namespace WistubaFormat
} // namespace FeBasicFeatures

//----------------------------------------------------------------------------
