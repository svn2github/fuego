//----------------------------------------------------------------------------
/** @file GoUctFeatureKnowledge.h
    Convert feature weights from FeFeatureWeights into additive knowledge. */
//----------------------------------------------------------------------------


#ifndef GOUCT_FEATURE_KNOWLEDGE_H
#define GOUCT_FEATURE_KNOWLEDGE_H

#include "FeFeatureWeights.h"
#include "GoAdditiveKnowledge.h"
#include "GoBoard.h"
#include "GoEvalArray.h"
#include "GoUctKnowledge.h"
#include "GoUctPlayoutPolicy.h"

//----------------------------------------------------------------------------

enum GoUctFeaturePriorType
{
    /** Do not use features as a prior */
    PRIOR_NONE,
    /** See SetPriorsSimple() */
    PRIOR_SIMPLE,
    /** Scale number of prior wins/losses depending on strength of features */
    PRIOR_SCALE_NU_GAMES,
    /** Scale feature values so that the smallest is mapped to 0, largest to 1
        before applying them as prior knowledge. */
    PRIOR_SCALE_PROBABILITIES_LINEAR,
    /** Add a prior only to the top m_topN moves acording to features */
    PRIOR_TOP_N
};

//----------------------------------------------------------------------------
/** Parameters for GoUctFeatureKnowledge */
struct GoUctFeatureKnowledgeParam
{
    GoUctFeatureKnowledgeParam();
    
    /** Map moveValue to additive term .
        Also see ProbabilityValue and m_additiveFeatureMultiplier. */
    float PredictorValue(float moveValue) const;

    /** Map moveValue to [0..1] */
    float ProbabilityValue(float moveValue) const;

    /** Use feature knowledge as an additive term */
    bool m_useAsAdditivePredictor;
    
    /** Multiplier from probability to additive predictor value.
        PredictorValue() = m_additiveFeatureMultiplier * ProbabilityValue()
    */
    float m_additiveFeatureMultiplier;
    
    /** Factor to multiply moveValue with before computing sigmoid */
    float m_additiveFeatureSigmoidFactor;

    /** If and how to use feature knowledge as "virtual" wins/losses */
    GoUctFeaturePriorType m_priorKnowledgeType;

    /** Number of "virtual" simulations when used as prior knowledge.
        Depending on m_priorKnowledgeType this value can be used as 
        a constant, as a maximum, or a scaling factor.
     */
    float m_priorKnowledgeWeight;

    /** Limit on number of top moves that get a prior.
        Used if m_priorKnowledgeType == PRIOR_TOP_N */
    int m_topN;
};

//----------------------------------------------------------------------------
class GoUctFeatureKnowledge
    : public GoAdditiveKnowledge, GoUctKnowledge
{
public:
    GoUctFeatureKnowledge(const GoBoard& bd, const FeFeatureWeights& weights);

    void Compute(const GoUctFeatureKnowledgeParam& param);
    
    /** Apply as additive predictor */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
    
    void SetPriorKnowledge(std::vector<SgUctMoveInfo>& moves);
    
    GoPredictorType PredictorType() const;
    
    /** The minimum value allowed by this predictor */
    SgUctValue MinValue() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;

private:
    
    void
    ComputeMinAndMaxValues(const std::vector<SgUctMoveInfo>& moves,
                           float& smallest,
                           float& largest) const;

    float MoveValue(const SgPoint move) const;
    
    void SetPriorsScaleProbabilitiesLinearly(
                                        std::vector<SgUctMoveInfo>& moves);

    void SetPriorsScaleNuGames(std::vector<SgUctMoveInfo>& moves);

    void SetPriorsSimple(std::vector<SgUctMoveInfo>& moves);

    void SetPriorsTopN(std::vector<SgUctMoveInfo>& moves);

    void SetWinsLosses(SgPoint move, float moveValue);

    bool UpToDate() const;
    
    SgHashCode m_code;
    
    GoEvalArray<float> m_moveValue;
        
    GoUctFeatureKnowledgeParam m_param;

    GoUctPlayoutPolicy<GoBoard> m_policy;
    
    FeFeatureWeights m_weights;
};

//----------------------------------------------------------------------------
inline GoPredictorType GoUctFeatureKnowledge::PredictorType() const
{
    return GO_PRED_TYPE_PROBABILITY_BASED;
}

inline SgUctValue GoUctFeatureKnowledge::MinValue() const
{
    return 0.0001; // TODO
}

inline float GoUctFeatureKnowledge::MoveValue(const SgPoint move) const
{
    SG_ASSERT(UpToDate());
    return m_moveValue[move];
}

inline SgUctValue GoUctFeatureKnowledge::Scale() const
{
    return 0.01; // TODO
}

inline bool GoUctFeatureKnowledge::UpToDate() const
{
    return m_code == GoAdditiveKnowledge::Board().GetHashCodeInclToPlay();
}

//----------------------------------------------------------------------------
class GoUctFeatureKnowledgeFactory
{
public:
    
	GoUctFeatureKnowledgeFactory();
    
	~GoUctFeatureKnowledgeFactory();
    
    GoAdditiveKnowledge* Create(const GoBoard& bd);
    
private:
    
    void ReadWeights();
    
    FeFeatureWeights m_weights;
};
//----------------------------------------------------------------------------

#endif // GOUCT_FEATURE_KNOWLEDGE_H
