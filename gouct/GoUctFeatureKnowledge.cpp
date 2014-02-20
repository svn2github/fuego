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
/** @todo This is a tunable constant. */
const float GoUctFeatureKnowledge::VALUE_MULTIPLIER = 10.0f;

namespace {
    
inline float sigmoid(float x) // it is really sigmoid(-x) computed here
{
    return 1 / (1 + exp(x));
}

} // namespace
//----------------------------------------------------------------------------

GoUctFeatureKnowledge::GoUctFeatureKnowledge(const GoBoard& bd,
                         const FeFeatureWeights& weights)
    : GoAdditiveKnowledge(bd), GoUctKnowledge(bd),
      m_weights(weights),
      m_policy(bd, GoUctPlayoutPolicyParam()),
      m_useAsAdditivePredictor(true),
      m_useAsVirtualWins(false)
{ }

void GoUctFeatureKnowledge::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    if (m_useAsVirtualWins)
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
        float moveValue = (move == SG_PASS) ? passEval : eval[move];
        if (m_useAsAdditivePredictor)
            moves[i].m_predictorValue = sigmoid(VALUE_MULTIPLIER * moveValue);
        if (m_useAsVirtualWins)
            SetWinsLosses(move, moveValue);
    }
    if (m_useAsVirtualWins)
        AddValuesTo(moves);
}

/** Convert moveValue into a number of virtual wins/losses */
void GoUctFeatureKnowledge::SetWinsLosses(SgPoint move, float moveValue)
{
    SgUctValue value = 1 - sigmoid(VALUE_MULTIPLIER * moveValue);
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
