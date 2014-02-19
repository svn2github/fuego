//----------------------------------------------------------------------------
/** @file GoUctFeatures.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctFeatures.h"

#include "FeBasicFeatures.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctPlayoutUtil.h"

namespace {

void FindCorrectionFeatures(const GoBoard& bd,
                            SgPointArray<FeFeatures::FeMoveFeatures>& features,
                            GoUctPlayoutPolicy<GoBoard>::Corrector corrFunction,
                            FeBasicFeature fromFeature,
                            FeBasicFeature toFeature
                            )
{
    std::pair<GoPointList,GoPointList> corrected =
    GoUctPlayoutUtil::FindCorrections(corrFunction, bd);
    for (GoPointList::Iterator it(corrected.first); it; ++it) // from
        features[*it].m_basicFeatures.set(fromFeature);
    for (GoPointList::Iterator it(corrected.second); it; ++it) // to
        features[*it].m_basicFeatures.set(toFeature);
}

void FindAllCorrectionFeatures(const GoBoard& bd,
                               SgPointArray<FeFeatures::FeMoveFeatures>& features)
{
    FindCorrectionFeatures(bd, features,
                           GoUctUtil::DoFalseEyeToCaptureCorrection<GoBoard>,
                           FE_GOUCT_REPLACE_CAPTURE_FROM,
                           FE_GOUCT_REPLACE_CAPTURE_TO
                           );
    
    FindCorrectionFeatures(bd, features,
                           GoUctUtil::DoSelfAtariCorrection<GoBoard>,
                           FE_GOUCT_SELFATARI_CORRECTION_FROM,
                           FE_GOUCT_SELFATARI_CORRECTION_TO
                           );
    
    FindCorrectionFeatures(bd, features,
                           GoUctUtil::DoClumpCorrection<GoBoard>,
                           FE_GOUCT_CLUMP_CORRECTION_FROM,
                           FE_GOUCT_CLUMP_CORRECTION_TO
                           );
}

void FindPolicyFeatures(GoUctPlayoutPolicy<GoBoard>& policy,
                        SgPointArray<FeFeatures::FeMoveFeatures>& features,
                        GoUctPlayoutPolicyType type,
                        FeBasicFeature f)
{
    const GoPointList moves = policy.GetPolicyMoves(type);
    for (GoPointList::Iterator it(moves); it; ++it)
        features[*it].m_basicFeatures.set(f);
}

void FindRandomPruned(const GoBoard& bd,
                      GoUctPlayoutPolicy<GoBoard>& policy,
                      SgPointArray<FeFeatures::FeMoveFeatures>& features)
{
    const GoPointList moves = policy.GetPolicyMoves(GOUCT_RANDOM);
    SgPointSet moveSet;
    for (GoPointList::Iterator it(moves); it; ++it)
        moveSet.Include(*it);
    for (GoBoard::Iterator it(bd); it; ++it)
        if (  bd.IsLegal(*it)
            && ! moveSet.Contains(*it)
            )
            features[*it].m_basicFeatures.set(FE_GOUCT_RANDOM_PRUNED);
}

void FindAllPolicyFeatures(const GoBoard& bd,
                           GoUctPlayoutPolicy<GoBoard>& policy,
                           SgPointArray<FeFeatures::FeMoveFeatures>& features)
{
    FindPolicyFeatures(policy, features, GOUCT_NAKADE, FE_GOUCT_NAKADE);
    FindPolicyFeatures(policy, features, GOUCT_ATARI_CAPTURE,
                       FE_GOUCT_ATARI_CAPTURE);
    FindPolicyFeatures(policy, features, GOUCT_ATARI_DEFEND,
                       FE_GOUCT_ATARI_DEFEND);
    FindPolicyFeatures(policy, features, GOUCT_LOWLIB, FE_GOUCT_LOWLIB);
    FindPolicyFeatures(policy, features, GOUCT_PATTERN, FE_GOUCT_PATTERN);
    FindPolicyFeatures(policy, features, GOUCT_CAPTURE, FE_GOUCT_CAPTURE);
    FindRandomPruned(bd, policy, features);
    FindAllCorrectionFeatures(bd, features);
}
    
} // namespace

void GoUctFeatures::
FindAllFeatures(const GoBoard& bd,
                GoUctPlayoutPolicy<GoBoard>& policy,
                SgPointArray<FeFeatures::FeMoveFeatures>& features,
                FeFeatures::FeMoveFeatures& passFeatures)
{
    for(GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsLegal(*it))
            FeFeatures::FindMoveFeatures(bd, *it, features[*it]);
    FeFeatures::FindMoveFeatures(bd, SG_PASS, passFeatures);
    FeFeatures::FindFullBoardFeatures(bd, features);
    FindAllPolicyFeatures(bd, policy, features);
}

void GoUctFeatures::
FindMoveFeaturesUI(const GoBoard& bd,
                   GoUctPlayoutPolicy<GoBoard>& policy,
                   SgPoint move,
                   FeFeatures::FeMoveFeatures& features)
{
    SG_ASSERT(move != SG_PASS);
    if (! bd.IsLegal(move))
        return;
    FindMoveFeatures(bd, move, features);
    SgPointArray<FeFeatures::FeMoveFeatures> boardFeatures;
    FeFeatures::FindFullBoardFeatures(bd, boardFeatures);
    FindAllPolicyFeatures(bd, policy, boardFeatures);
    features.m_basicFeatures |= boardFeatures[move].m_basicFeatures;
}

void GoUctFeatures::WriteFeatures(std::ostream& stream,
                                  GoUctPlayoutPolicy<GoBoard>& policy,
                                  const GoBoard& constBd,
                                  const bool writeComment)
{
    SgPoint chosenMove = constBd.GetLastMove();
    GoModBoard mod(constBd);
    GoBoard& bd = mod.Board();
    if (chosenMove != SG_NULLMOVE)
    {
        bd.Undo();
        SgPointArray<FeFeatures::FeMoveFeatures> features;
        FeFeatures::FeMoveFeatures passFeatures;
        FindAllFeatures(bd, policy, features, passFeatures);
        FeFeatures::WistubaFormat::
        WriteBoardFeatures(stream, features, passFeatures,
                           bd, chosenMove, writeComment);
        bd.Play(chosenMove);
    }
}

//FindMCOwnerFeatures(bd, move, features);
#if UNUSED // TODO
int NuWins()
{
    return 42; // TODO
}

void FindMCOwnerFeatures(const GoBoard& bd, SgPoint move,
                         FeBasicFeatureSet& features)
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
#endif
