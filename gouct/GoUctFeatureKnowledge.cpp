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
    m_linearlyScaleProbabilities(false),
    m_useAsAdditivePredictor(true),
    m_useAsPriorKnowledge(false),
    m_additiveFeatureMultiplier(1.0),
    m_additiveFeatureSigmoidFactor(10.0),
    m_priorKnowledgeWeight(23.0)
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
    : GoAdditiveKnowledge(bd),
      GoUctKnowledge(bd),
      m_code(),
      m_moveValue(0),
      m_param(),
      m_policy(bd, GoUctPlayoutPolicyParam()),
      m_weights(weights)
{ }

void GoUctFeatureKnowledge::
Compute(const GoUctFeatureKnowledgeParam& param)
{
    m_param = param; // todo avoid copy? Lifetime?
    const GoBoard& bd = GoAdditiveKnowledge::Board();
    FeFullBoardFeatures features(bd);
    GoUctFeatures::FindAllFeatures(bd, m_policy, features);
    m_moveValue = features.EvaluateFeatures(m_weights);
    m_code = bd.GetHashCodeInclToPlay();
}

void GoUctFeatureKnowledge::
ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    SG_ASSERT(UpToDate());
    const GoBoard& bd = GoAdditiveKnowledge::Board();
    SG_DEBUG_ONLY(bd);
    
    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        SG_ASSERT(bd.IsLegal(move));
        const float moveValue = MoveValue(move);
        it->m_predictorValue = m_param.PredictorValue(-moveValue);
    }
}

void GoUctFeatureKnowledge::
SetPriorKnowledge(std::vector<SgUctMoveInfo>& moves)
{
    SG_ASSERT(UpToDate());
    ClearValues();
    if (m_param.m_linearlyScaleProbabilities)
        SetLinearlyScaledPriors(moves);
    else
        SetSimplePriors(moves);
    AddValuesTo(moves);
}

void GoUctFeatureKnowledge::
SetLinearlyScaledPriors(std::vector<SgUctMoveInfo>& moves)
{
    // We cannot just take the min and max of the whole eval array
    // since it contains illegal moves with value 0, which may be smaller
    // (or larger) than the true min/max.
    float smallest = std::numeric_limits<float>::max();
    float largest = -std::numeric_limits<float>::max();
    for (std::vector<SgUctMoveInfo>::const_iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        const float moveValue = MoveValue(move);
        smallest = std::min(smallest, moveValue);
        largest = std::max(largest, moveValue);
    }
    const float scale = largest - smallest;
    SG_ASSERT(scale >= 0.0);
    if (scale > 0.001) // skip if all values are nearly identical
    {
        const float invScale = 1/scale;
        for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
             it != moves.end(); ++it)
        {
            const SgPoint move = it->m_move;
            const float moveValue = MoveValue(move);
            const SgUctValue scaledValue = (moveValue - smallest) * invScale;
            SG_ASSERT(scaledValue >= 0.0);
            SG_ASSERT(scaledValue <= 1.0
                                   + std::numeric_limits<float>::epsilon());

            Add(move, scaledValue, m_param.m_priorKnowledgeWeight);
        }
    }
}

void GoUctFeatureKnowledge::
SetSimplePriors(std::vector<SgUctMoveInfo>& moves)
{
    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        const float moveValue = MoveValue(move);
        SetWinsLosses(move, moveValue);
    }
}

/** Convert moveValue into a number of virtual wins/losses */
inline void GoUctFeatureKnowledge::
SetWinsLosses(SgPoint move, float moveValue)
{
    SgUctValue value = m_param.ProbabilityValue(moveValue);
    Add(move, value, m_param.m_priorKnowledgeWeight);
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
