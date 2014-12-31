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
    return 1 / (1 + expf(-x));
}

} // namespace
//----------------------------------------------------------------------------
GoUctFeatureKnowledgeParam::GoUctFeatureKnowledgeParam()
    :
    m_useAsAdditivePredictor(false),
    m_additiveFeatureMultiplier(1.0),
    m_additiveFeatureSigmoidFactor(10.0),
    m_priorKnowledgeType(PRIOR_SCALE_NU_GAMES),
    m_priorKnowledgeWeight(7.0),
    m_topN(3)
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
    switch (m_param.m_priorKnowledgeType)
    {
        case PRIOR_SIMPLE:
            SetPriorsSimple(moves);
            break;
        case PRIOR_SCALE_NU_GAMES:
            SetPriorsScaleNuGames(moves);
            break;
        case PRIOR_SCALE_PROBABILITIES_LINEAR:
            SetPriorsScaleProbabilitiesLinearly(moves);
            break;
        case PRIOR_TOP_N:
            SetPriorsTopN(moves);
            break;
        default:
            SG_ASSERT(false);
    }
    AddValuesTo(moves);
}

void GoUctFeatureKnowledge::
ComputeMinAndMaxValues(const std::vector<SgUctMoveInfo>& moves,
                       float& smallest,
                       float& largest) const
{
    // We cannot just take the min and max over the whole eval array
    // since it contains illegal moves with value 0, which may be smaller
    // (or larger) than the true min/max.
    smallest = std::numeric_limits<float>::max();
    largest = -std::numeric_limits<float>::max();
    for (std::vector<SgUctMoveInfo>::const_iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        const float moveValue = MoveValue(move);
        smallest = std::min(smallest, moveValue);
        largest = std::max(largest, moveValue);
    }
}

void GoUctFeatureKnowledge::
SetPriorsScaleNuGames(std::vector<SgUctMoveInfo>& moves)
{
    const float eps = 0.001f;
    const float tinyEps = 0.00001f;
    float smallest;
    float largest;
    ComputeMinAndMaxValues(moves, smallest, largest);

    float sum = 0;
    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const float moveValue = MoveValue(it->m_move);
        sum += std::abs(moveValue);
    }

     // todo there could be significant positive and negative values which cancel out.
     // could compute positiveSum, negativeSum separately.
     // avoid numerical problems
     // average positive knowledge should be m_param.m_priorKnowledgeWeight
    const float factor = (sum > eps) ?
            m_param.m_priorKnowledgeWeight * moves.size() / sum
            : 0.0f;

    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        const float moveValue = MoveValue(move);
        SgUctValue value = 0;
        SgUctValue weight = 0;
        if (moveValue > 0)
        {
            if (largest > tinyEps)
            {
                value = 0.5 * (1 + moveValue / largest);
                SG_ASSERT(value >= 0.5);
                SG_ASSERT(value <= 1);
                weight = factor * moveValue;
            }
            // else just leave as value = weight = 0
        }
        else if (smallest < -tinyEps)
        {
            value = 0.5 * (1 - moveValue / smallest);
            SG_ASSERT(value >= 0.0);
            SG_ASSERT(value <= 0.5);
            weight = -factor * moveValue;
        }
        // else just leave as value = weight = 0
        Add(move, value, weight);
    }
}

void GoUctFeatureKnowledge::
SetPriorsScaleProbabilitiesLinearly(std::vector<SgUctMoveInfo>& moves)
{
    float smallest;
    float largest;
    ComputeMinAndMaxValues(moves, smallest, largest);
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
SetPriorsSimple(std::vector<SgUctMoveInfo>& moves)
{
    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        const float moveValue = MoveValue(move);
        SetWinsLosses(move, moveValue);
    }
}

typedef std::pair<SgPoint,float> EvalPair;

inline bool CompareValue(const EvalPair& p1, const EvalPair& p2)
{
    return p1.second > p2.second;
}

void GoUctFeatureKnowledge::
SetPriorsTopN(std::vector<SgUctMoveInfo>& moves)
{
    const int N = std::min(m_param.m_topN, static_cast<int>(moves.size()));
    std::vector<EvalPair> sorted;
    for (std::vector<SgUctMoveInfo>::iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        const SgPoint move = it->m_move;
        sorted.push_back(std::make_pair(move, MoveValue(move)));
    }
    std::sort(sorted.begin(), sorted.end(), CompareValue);

    for (int i=0; i < N; ++i)
    {
        Add(sorted[i].first, 1.0, m_param.m_priorKnowledgeWeight);
        //SgDebug() << "Good move: " << SgWritePoint(sorted[i].first)
        //<< " Eval = " << sorted[i].second << '\n';
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
