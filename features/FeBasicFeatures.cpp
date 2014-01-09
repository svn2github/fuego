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
    const SgBlackWhite toPlay = bd.ToPlay();
    GoNeighborBlockIterator it(bd, lastMove, toPlay, 1);
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
        else if (bd.KoPoint() != SG_NULLPOINT) // not sure about the else here
            features.set(FE_ATARI_KO);
        else
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


namespace FeBasicFeatures {

void FindFeatures(const GoBoard& bd, SgPoint move, FeBasicFeatureSet& features)
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
} // namespace FeBasicFeatures
