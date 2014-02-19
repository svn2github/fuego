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

#if UNUSED
bool PutUsIntoAtari(const GoBoard& bd, SgPoint lastMove)
{
    SG_ASSERT(! SgIsSpecialMove(lastMove));
    GoAdjBlockIterator<GoBoard> it(bd, lastMove, 1);
    return it;
}
#endif

/** Check if any of these neighbors-in-atari
    is adjacent to any us-in-atari block */
bool HasAnyNeighborInAtari(const GoBoard& bd,
                           SgPoint oppInAtari[4 + 1])
{
    for (int i = 0; oppInAtari[i] != SG_ENDPOINT; ++i)
        if (GoBoardUtil::HasAdjacentBlocks(bd, oppInAtari[i], 1))
            return true;
    return false;
}

bool MoveCapturesBlock(const GoBoard& bd, SgPoint move, SgPoint lastMove)
{
    // assumes no suicide - lastMove cannot be empty
    SG_ASSERT(bd.Occupied(lastMove));
    
    return ! SgIsSpecialMove(lastMove)
        && bd.InAtari(lastMove)
        && bd.TheLiberty(lastMove) == move;
}

/** move is liberty of both lastMove and some other opponent block */
bool PreventConnection(const GoBoard& bd, SgPoint move,
                       SgPoint lastMove, const SgBlackWhite opp)
{
    // assumes no suicide - lastMove cannot be empty
    SG_ASSERT(bd.Occupied(lastMove));
    
    return bd.IsLibertyOfBlock(move, bd.Anchor(lastMove))
        && GoBoardUtil::IsCuttingPoint(bd, move, opp);
}
                    
// faster to globally find all such moves.
void FindCaptureFeatures(const GoBoard& bd, SgPoint move,
                         FeBasicFeatureSet& features)
{
    // TODO what to do if multiple preys? FE_CAPTURE_MULTIPLE?
    
    // previous implementation checked only if move captures lastmove
    // that put us into atari. Should this be a separate feature?
    // Now we check if capture relieves any atari on one of our blocks .
    //if (PutUsIntoAtari(bd, lastMove))
    //    f = FE_CAPTURE_ADJ_ATARI;
    
    const SgBlackWhite toPlay = bd.ToPlay();
    if (! bd.CanCapture(move, toPlay))
        return;

    FeBasicFeature f = FE_NONE;
    SgPoint oppInAtari[4 + 1];
    const SgBlackWhite opp = SgOppBW(toPlay);
    bd.NeighborBlocks(move, opp, 1, oppInAtari);
    if (HasAnyNeighborInAtari(bd, oppInAtari))
       f = FE_CAPTURE_ADJ_ATARI;
    else
    {
        const SgPoint lastMove = bd.GetLastMove();
        if (! SgIsSpecialMove(lastMove))
        {
            // assumes no suicide, last move is occupied
            SG_ASSERT(bd.IsColor(lastMove, opp));
            if (  bd.CapturingMove()
               && MoveCapturesBlock(bd, move, lastMove)
               )
                f = FE_CAPTURE_RECAPTURE;
            else if (PreventConnection(bd, move, lastMove, opp))
                f = FE_CAPTURE_PREVENT_CONNECTION;
        }
    }
    if (f != FE_NONE)
        features.set(f);
    else // other capture
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

void FindCfgFeatures(const GoBoard& bd, SgPoint focus,
                     FeBasicFeature baseFeature, int baseDistance,
                     const GoPointList& legalBoardMoves,
                     SgPointArray<FeFeatures::FeMoveFeatures>& features)
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
        features[*it].m_basicFeatures.set(f);
    }
}

void FindCfgFeatures(const GoBoard& bd,
                     const GoPointList& legalBoardMoves,
                     SgPointArray<FeFeatures::FeMoveFeatures>& features)
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
                SgPointArray<FeFeatures::FeMoveFeatures>& features)
{
    const std::vector<SgPoint>
    corner(GoOpeningKnowledge::FindCornerMoves(bd));
    for (std::vector<SgPoint>::const_iterator it
         = corner.begin(); it != corner.end(); ++it)
    {
        features[*it].m_basicFeatures.set(FE_CORNER_OPENING_MOVE);
    }
}

void FindGamePhaseFeature(const GoBoard& bd, FeBasicFeatureSet& features)
{
    const int phase = std::min(12, bd.MoveNumber() / 30 + 1);
    FeBasicFeature f = ComputeFeature(FE_GAME_PHASE_1, 1, phase);
    SG_ASSERT(f >= FE_GAME_PHASE_1);
    SG_ASSERT(f <= FE_GAME_PHASE_12);
    features.set(f);
}

void FindLineFeature(const GoBoard& bd, SgPoint move,
                     FeBasicFeatureSet& features)
{
    const int line = std::min(5, bd.Line(move));
    FeBasicFeature f = ComputeFeature(FE_LINE_1, 1, line);
    SG_ASSERT(f >= FE_LINE_1);
    SG_ASSERT(f <= FE_LINE_5_OR_MORE);
    features.set(f);
}

void FindPassFeatures(const GoBoard& bd, FeBasicFeatureSet& features)
{
    const SgPoint lastMove = bd.GetLastMove();
    if (lastMove == SG_PASS)
        features.set(FE_PASS_CONSECUTIVE);
    else
        features.set(FE_PASS_NEW);
}

void FindPosFeature(const GoBoard& bd, SgPoint move,
                    FeBasicFeatureSet& features)
{
    const int pos = std::min(10, bd.Pos(move));
    FeBasicFeature f = ComputeFeature(FE_POS_1, 1, pos);
    SG_ASSERT(f >= FE_POS_1);
    SG_ASSERT(f <= FE_POS_10);
    features.set(f);
}
    
void FindSelfatariFeatures(const GoBoard& bd, SgPoint move,
                           FeBasicFeatureSet& features)
{
    if (GoBoardUtil::SelfAtari(bd, move))
        features.set(FE_SELFATARI);
    // @todo FE_SELFATARI_NAKADE, FE_SELFATARI_THROWIN
}

void FindSideExtensionFeatures(const GoBoard& bd,
                        SgPointArray<FeFeatures::FeMoveFeatures>& features)
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
        features[p].m_basicFeatures.set(f);
    }
}

const int EDGE_START_INDEX_3x3 = 1000;
const int CENTER_START_INDEX_3x3 = 1200;

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
    return CENTER_START_INDEX_3x3 + code;
}

inline int Find3x3Feature(const GoBoard& bd, SgPoint p)
{
    return bd.Pos(p) == 1  ? FeFeatures::INVALID_3x3_INDEX
         : bd.Line(p) == 1 ? Find2x3EdgeFeature(bd, p)
                           : Find3x3CenterFeature(bd, p);
}

void Write3x3(std::ostream& stream, int index)
{
    SG_ASSERT(index != FeFeatures::INVALID_3x3_INDEX); // stream << "\nCORNER\n";
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

void FindDistPrevMoveFeatures(const GoBoard& bd, SgPoint move,
                              FeBasicFeatureSet& features)
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
        SG_ASSERT(distance == 0 || distance >= 2);
        if (distance == 0)
            features.set(FE_DIST_PREV_OWN_0);
        if (distance <= 17)
        {
            FeBasicFeature f =
                ComputeFeature(FE_DIST_PREV_OWN_2, 2, distance);
            features.set(f);
        }
    }
}

void WritePatternFeatures(std::ostream& stream,
                          const FeFeatures::FeMoveFeatures& features)
{
    if (features.m_3x3Index != FeFeatures::INVALID_3x3_INDEX)
    {
        stream << " 3x3-index " << features.m_3x3Index;
        Write3x3(stream, features.m_3x3Index);
    }
}

void WritePatternFeatureIndex(std::ostream& stream,
                              const FeFeatures::FeMoveFeatures& features)
{
    if (features.m_3x3Index != FeFeatures::INVALID_3x3_INDEX)
        stream << ' ' << features.m_3x3Index;
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
        "FE_GOUCT_REPLACE_CAPTURE_NOT_MOVED",
        "FE_GOUCT_SELFATARI_CORRECTION_FROM",
        "FE_GOUCT_SELFATARI_CORRECTION_TO",
        "FE_GOUCT_SELFATARI_CORRECTION_NOT_MOVED",
        "FE_GOUCT_CLUMP_CORRECTION_FROM",
        "FE_GOUCT_CLUMP_CORRECTION_TO",
        "FE_GOUCT_CLUMP_CORRECTION_NOT_MOVED",
        "FE_MC_OWNER_1", // 0−7 wins/63 sim.
        "FE_MC_OWNER_2", // 8−15
        "FE_MC_OWNER_3", // 16−23
        "FE_MC_OWNER_4", // 24−31
        "FE_MC_OWNER_5", // 32−39
        "FE_MC_OWNER_6", // 40−47
        "FE_MC_OWNER_7", // 48−55
        "FE_MC_OWNER_8",  // 56−63
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
        "FE_NONE"
    };
    SG_ASSERT(f >= FE_PASS_NEW);
    SG_ASSERT(f < _NU_FE_FEATURES);
    stream << s_string[f];
    return stream;
}

//----------------------------------------------------------------------------
void WriteFeatureFromID(std::ostream& stream, int id)
{
    if (id < static_cast<int>(_NU_FE_FEATURES))
        stream << static_cast<FeBasicFeature>(id);
    else // 3x3 pattern
        Write3x3(stream, id);
}

//----------------------------------------------------------------------------

std::ostream& FeFeatures::operator<<(std::ostream& stream,
                         const FeFeatures::FeEvalDetail& f)
{
    stream << '('; WriteFeatureFromID(stream, f.m_feature);
    stream << ", w = " << std::setprecision(2) << f.m_w
           << ", v_sum = " << f.m_v_sum << ')';
    return stream;
}

//----------------------------------------------------------------------------

size_t FeFeatures::ActiveFeatures(const FeMoveFeatures& features,
                                  FeActiveArray& active)
{
    size_t nuActive = 0;
    for (int i = 0; i < _NU_FE_FEATURES; ++i)
        if (features.m_basicFeatures.test(i))
        {
            active[nuActive] = i;
            if (++nuActive >= MAX_ACTIVE_LENGTH)
                return nuActive;
        }
    // invalid for pass move and (1,1) points
    if (features.m_3x3Index != INVALID_3x3_INDEX)
    {
        SG_ASSERT(nuActive < MAX_ACTIVE_LENGTH);
        active[nuActive] = features.m_3x3Index;
        ++nuActive;
    }
    return nuActive;
}

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

SgPointArray<float> FeFeatures::EvaluateFeatures(const GoBoard& bd,
                             const SgPointArray<FeMoveFeatures>& features,
                             const FeFeatureWeights& weights)
{
    SgPointArray<float> eval(0);
    for(GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsLegal(*it))
            eval[*it] = EvaluateMoveFeatures(features[*it], weights);
    return eval;
}

float FeFeatures::EvaluateMoveFeatures(const FeMoveFeatures& features,
                                       const FeFeatureWeights& weights)
{
    FeActiveArray active;
    const size_t nuActive = ActiveFeatures(features, active);
    return EvaluateActiveFeatures(active, nuActive, weights);
}

std::vector<FeFeatures::FeEvalDetail>
FeFeatures::EvaluateMoveFeaturesDetail(const FeMoveFeatures& features,
                                       const FeFeatureWeights& weights)
{
    FeActiveArray active;
    const size_t nuActive = ActiveFeatures(features, active);
    std::vector<FeFeatures::FeEvalDetail> detail;
    for (FeActiveIterator it = active.begin();
                          it < active.begin() + nuActive; ++it)
    {
        const float w = weights.m_w[*it];
        float v = 0.0;
        for (FeActiveIterator it2 = active.begin();
                              it2 < active.begin() + nuActive; ++it2)
            if (it != it2)
                v += weights.Combine(*it, *it2);
        detail.push_back(FeFeatures::FeEvalDetail(*it, w, v/2));
    }
    return detail;
}

void FeFeatures::FindBasicMoveFeatures(const GoBoard& bd, SgPoint move,
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
    FindLineFeature(bd, move, features);
    FindDistPrevMoveFeatures(bd, move, features);
    FindPosFeature(bd, move, features);
    FindGamePhaseFeature(bd, features);
}

inline GoPointList AllLegalMoves(const GoBoard& bd)
{
    GoPointList legalBoardMoves;
    for (GoBoard::Iterator it(bd); it; ++it)
    if (bd.IsLegal(*it))
        legalBoardMoves.PushBack(*it);
    return legalBoardMoves;
}

void FeFeatures::FindFullBoardFeatures(const GoBoard& bd,
                           SgPointArray<FeFeatures::FeMoveFeatures>& features)
{
    FindCornerMoveFeatures(bd, features);
    FindSideExtensionFeatures(bd, features);
    const GoPointList legalBoardMoves(AllLegalMoves(bd));
    FindCfgFeatures(bd, legalBoardMoves, features);
}

void FeFeatures::FindMoveFeatures(const GoBoard& bd, SgPoint move,
                                  FeMoveFeatures& features)
{
    FindBasicMoveFeatures(bd, move, features.m_basicFeatures);
    if (move != SG_PASS)
        features.m_3x3Index = Find3x3Feature(bd, move);
}

int FeFeatures::Get3x3Feature(const GoBoard& bd, SgPoint p)
{
    return Find3x3Feature(bd, p);
}

void FeFeatures::WriteBoardFeatures(std::ostream& stream,
                        const SgPointArray<FeMoveFeatures>& features,
                        const GoBoard& bd)
{
    for (GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsLegal(*it))
            WriteFeatures(stream, *it, features[*it]);
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

void FeFeatures::WriteFeatures(std::ostream& stream,
                   SgPoint move,
                   const FeMoveFeatures& features)
{
    WriteFeatureSetAsText(stream, move, features.m_basicFeatures);
    WritePatternFeatures(stream, features);
    stream << '\n';
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
    
namespace FeFeatures {

namespace WistubaFormat{
namespace { // TODO needs to be local within WistubaFormat for now because of
            // name conflicts with above. Needs cleanup
    
void WriteFeatureSet(std::ostream& stream,
                     const FeBasicFeatureSet& features)
{
    for (int f = FE_PASS_NEW; f < _NU_FE_FEATURES; ++f)
        if (features.test(f))
            stream << ' ' << f;
}

void WriteFeatures(std::ostream& stream,
                   const int isChosen,
                   const FeMoveFeatures& features,
                   const int moveNumber,
                   const bool writeComment)
{
    const int SHAPE_SIZE = 3; // TODO make this variable
                              // when big pattern features are implemented
    stream << isChosen;
    WriteFeatureSet(stream, features.m_basicFeatures);
    WritePatternFeatureIndex(stream, features);
    if (writeComment)
        stream << " #0_" << moveNumber << ' ' << SHAPE_SIZE;
    stream << '\n';
}

} // namespace
} // namespace WistubaFormat
    
void WistubaFormat::WriteBoardFeatures(std::ostream& stream,
                        const SgPointArray<FeMoveFeatures>& features,
                        const FeMoveFeatures& passFeatures,
                        const GoBoard& bd,
                        const SgPoint chosenMove,
                        const bool writeComment)
{
    const int moveNumber = bd.MoveNumber() + 1; // + 1 because we did undo
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        const SgPoint p = *it;
        if (p != chosenMove && bd.IsLegal(p))
            WriteFeatures(stream, 0, features[p], moveNumber, writeComment);
    }
    if (SG_PASS == chosenMove)
        WriteFeatures(stream, 1, passFeatures, moveNumber, writeComment);
    else
    {
        WriteFeatures(stream, 0, passFeatures, moveNumber, writeComment);
        WriteFeatures(stream, 1, features[chosenMove], moveNumber,
                      writeComment);
    }
}

} // namespace FeFeatures

//----------------------------------------------------------------------------
