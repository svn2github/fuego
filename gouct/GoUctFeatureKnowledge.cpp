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
    : GoAdditiveKnowledge(bd),
      GoUctKnowledge(bd),
      m_code(),
      m_eval(0),
      m_passEval(0),
      m_param(),
      m_policy(bd, GoUctPlayoutPolicyParam()),
      m_weights(weights)
{ }

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
    const GoBoard& bd = GoAdditiveKnowledge::Board();
    SG_DEBUG_ONLY(bd);

    ClearValues();
    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        SG_ASSERT(bd.IsLegal(move));
        const float moveValue = MoveValue(move);
        SetWinsLosses(move, moveValue);
    }
    AddValuesTo(moves);
}

void GoUctFeatureKnowledge::
Compute(const GoUctFeatureKnowledgeParam& param)
{
    m_param = param;
    const GoBoard& bd = GoAdditiveKnowledge::Board();
    FeFullBoardFeatures features(bd);
    GoUctFeatures::FindAllFeatures(bd, m_policy, features);
    m_eval = features.EvaluateFeatures(m_weights);
    m_code = bd.GetHashCodeInclToPlay();
}

/** Convert moveValue into a number of virtual wins/losses */
void GoUctFeatureKnowledge::
SetWinsLosses(SgPoint move, float moveValue)
{
    SgUctValue value = m_param.ProbabilityValue(moveValue);
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
