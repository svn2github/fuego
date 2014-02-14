//----------------------------------------------------------------------------
/** @file GoUctFeatures.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctFeatures.h"

#include "FeBasicFeatures.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctPlayoutUtil.h"

namespace {
    GoPointList GetUctPolicyMoves(const GoBoard& bd,
                                  GoUctPlayoutPolicyType type)
    {
        static GoUctPlayoutPolicy<GoBoard>* s_policy = 0;
        static const GoBoard* s_bd = 0;
        if (&bd != s_bd)
        {
            delete s_policy;
            s_policy =
            new GoUctPlayoutPolicy<GoBoard>(bd, GoUctPlayoutPolicyParam());
            s_bd = &bd;
            SgDebug() << "new policy\n";
        }
        return s_policy->GetPolicyMoves(type);
    }
    
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
    
    void FindPolicyFeatures(const GoBoard& bd,
                            SgPointArray<FeFeatures::FeMoveFeatures>& features,
                            GoUctPlayoutPolicyType type,
                            FeBasicFeature f)
    {
        const GoPointList moves = GetUctPolicyMoves(bd, type);
        for (GoPointList::Iterator it(moves); it; ++it)
        features[*it].m_basicFeatures.set(f);
    }

    void FindRandomPruned(const GoBoard& bd,
                          SgPointArray<FeFeatures::FeMoveFeatures>& features)
    {
        const GoPointList moves = GetUctPolicyMoves(bd, GOUCT_RANDOM);
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
                               SgPointArray<FeFeatures::FeMoveFeatures>& features)
    {
        // TODO? FE_GOUCT_FILLBOARD
        FindPolicyFeatures(bd, features, GOUCT_NAKADE, FE_GOUCT_NAKADE);
        FindPolicyFeatures(bd, features, GOUCT_ATARI_CAPTURE,
                           FE_GOUCT_ATARI_CAPTURE);
        FindPolicyFeatures(bd, features, GOUCT_ATARI_DEFEND,
                           FE_GOUCT_ATARI_DEFEND);
        FindPolicyFeatures(bd, features, GOUCT_LOWLIB, FE_GOUCT_LOWLIB);
        FindPolicyFeatures(bd, features, GOUCT_PATTERN, FE_GOUCT_PATTERN);
        FindPolicyFeatures(bd, features, GOUCT_CAPTURE, FE_GOUCT_CAPTURE);
        FindRandomPruned(bd, features);
        FindAllCorrectionFeatures(bd, features);
    }
    
    
} // namespace

void GoUctFeatures::FindAllFeatures(const GoBoard& bd,
                                 SgPointArray<FeFeatures::FeMoveFeatures>& features,
                                 FeFeatures::FeMoveFeatures& passFeatures)
{
    for(GoBoard::Iterator it(bd); it; ++it)
    if (bd.IsLegal(*it))
    FeFeatures::FindMoveFeatures(bd, *it, features[*it]);
    FeFeatures::FindMoveFeatures(bd, SG_PASS, passFeatures);
    FindAllPolicyFeatures(bd, features);
}

void GoUctFeatures::FindMoveFeaturesUI(const GoBoard& bd, SgPoint move,
                                    FeFeatures::FeMoveFeatures& features)
{
    if (! bd.IsLegal(move))
    return;
    FindMoveFeatures(bd, move, features);
    // move != SG_PASS
    SgPointArray<FeFeatures::FeMoveFeatures> boardFeatures;
    FindAllPolicyFeatures(bd, boardFeatures);
    features.m_basicFeatures |= boardFeatures[move].m_basicFeatures;
}

void GoUctFeatures::WriteFeatures(std::ostream& stream,
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
        FindAllFeatures(bd, features, passFeatures);
        FeFeatures::WistubaFormat::WriteBoardFeatures(
                           stream, features, passFeatures,
                           bd, chosenMove, writeComment);
        bd.Play(chosenMove);
    }
}

