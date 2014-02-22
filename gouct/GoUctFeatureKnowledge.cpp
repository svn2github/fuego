//----------------------------------------------------------------------------
/** @file GoUctFeatureKnowledge.cpp
 See GoUctFeatureKnowledge.h
 */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctFeatureKnowledge.h"

#include "FeBasicFeatures.h"
#include "GoUctFeatures.h"
#include "SgDebug.h"

//----------------------------------------------------------------------------
namespace {
    
inline float Sigmoid(float x)
{
    return 1 / (1 + exp(-x));
}

} // namespace
//----------------------------------------------------------------------------
GoUctFeatureKnowledgeParam::GoUctFeatureKnowledgeParam()
    :
    m_useAsAdditivePredictor(true),
    m_useAsVirtualWins(false),
    m_additiveFeatureMultiplier(1.0),
    m_additiveFeatureSigmoidFactor(10.0)
{ }

inline float GoUctFeatureKnowledgeParam::PredictorValue(float moveValue) const
{
    return m_additiveFeatureMultiplier *
    ::Sigmoid(m_additiveFeatureSigmoidFactor * moveValue);
}

inline float GoUctFeatureKnowledgeParam::
ProbabilityValue(float moveValue) const
{
    return ::Sigmoid(m_additiveFeatureSigmoidFactor * moveValue);
}

//----------------------------------------------------------------------------

GoUctFeatureKnowledge::GoUctFeatureKnowledge(
                         const GoBoard& bd,
                         const FeFeatureWeights& weights)
    : GoAdditiveKnowledge(bd), GoUctKnowledge(bd),
      m_weights(weights),
      m_policy(bd, GoUctPlayoutPolicyParam())
{ }

void GoUctFeatureKnowledge::
ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    SG_UNUSED(moves);
    SG_ASSERT(false);
}

void GoUctFeatureKnowledge::
ApplyAdditivePredictor(std::vector<SgUctMoveInfo>& moves,
                       const GoUctFeatureKnowledgeParam& param)
{
    if (param.m_useAsVirtualWins)
        ClearValues();
    const GoBoard& bd = GoAdditiveKnowledge::Board();
    SgPointArray<FeFeatures::FeMoveFeatures> features;
    FeFeatures::FeMoveFeatures passFeatures;
    GoUctFeatures::FindAllFeatures(bd, m_policy, features, passFeatures);
    SgPointArray<float> eval = EvaluateFeatures(bd, features, m_weights);
    float passEval = EvaluateMoveFeatures(passFeatures, m_weights);
    
    for (size_t i = 0; i < moves.size(); ++i) // todo just use iterator
    {
        const SgPoint move = moves[i].m_move;
        SG_ASSERT(bd.IsLegal(move));
        const float moveValue = (move == SG_PASS) ? passEval : eval[move];
        if (param.m_useAsAdditivePredictor) // -moveValue for opp. view?
            moves[i].m_predictorValue = param.PredictorValue(-moveValue);
        if (param.m_useAsVirtualWins)
            SetWinsLosses(move, moveValue, param);
    }
    if (param.m_useAsVirtualWins)
        AddValuesTo(moves);
}

/** Convert moveValue into a number of virtual wins/losses */
void GoUctFeatureKnowledge::
SetWinsLosses(SgPoint move, float moveValue,
              const GoUctFeatureKnowledgeParam& param)
{
    SgUctValue value = param.ProbabilityValue(moveValue);
    SgUctValue count = 23;
    Add(move, value, count);
}

//----------------------------------------------------------------------------
GoUctFeatureKnowledgeFactory::GoUctFeatureKnowledgeFactory() : m_weights(0, 0)
{
    ReadWeights();
}
    
GoUctFeatureKnowledgeFactory::~GoUctFeatureKnowledgeFactory()
{ }

GoAdditiveKnowledge* GoUctFeatureKnowledgeFactory::Create(const GoBoard& bd)
{
    return new GoUctFeatureKnowledge(bd, m_weights);
}

void GoUctFeatureKnowledgeFactory::ReadWeights()
{
    m_weights = FeFeatureWeights::ReadDefaultWeights();
    SgDebug() << "GoUctFeatureKnowledgeFactory Read weights for "
              << m_weights.m_nuFeatures
              << " features with k = " << m_weights.m_k
              << ", minID = " << m_weights.m_minID
              << ", maxID = " << m_weights.m_maxID
              << '\n';
}

//----------------------------------------------------------------------------
