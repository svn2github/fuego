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
/** Parameters for GoUctFeatureKnowledge */
struct GoUctFeatureKnowledgeParam
{
    GoUctFeatureKnowledgeParam();
    
    /** Map moveValue to additive term .
        Also see ProbabilityValue and m_additiveFeatureMultiplier. */
    float PredictorValue(float moveValue) const;

    /** Map moveValue to [0..1] */
    float ProbabilityValue(float moveValue) const;

    /** Scale feature values so that the smallest is mapped to 0, largest to 1
        before applying them as prior knowledge. */
    bool m_linearlyScaleProbabilities;

    /** Use feature knowledge as an additive term */
    bool m_useAsAdditivePredictor;
    
    /** Use feature knowledge as "virtual" wins/losses */
    bool m_useAsPriorKnowledge;

    /** Multiplier from probability to additive predictor value.
        PredictorValue() = m_additiveFeatureMultiplier * ProbabilityValue()
    */
    float m_additiveFeatureMultiplier;
    
    /** Factor to multiply moveValue with before computing sigmoid */
    float m_additiveFeatureSigmoidFactor;

    /** Number of "virtual" simulations when used as prior knowledge */
    float m_priorKnowledgeWeight;
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
    SgUctValue Minimum() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;

private:
    
    float MoveValue(const SgPoint move) const;
    
    void SetLinearlyScaledPriors(std::vector<SgUctMoveInfo>& moves);

    void SetSimplePriors(std::vector<SgUctMoveInfo>& moves);

    void SetWinsLosses(SgPoint move, float moveValue);

    bool UpToDate() const;
    
    SgHashCode m_code;
    
    GoEvalArray<float> m_eval;
        
    GoUctFeatureKnowledgeParam m_param;

    GoUctPlayoutPolicy<GoBoard> m_policy;
    
    FeFeatureWeights m_weights;
    
};

//----------------------------------------------------------------------------
inline GoPredictorType GoUctFeatureKnowledge::PredictorType() const
{
    return GO_PRED_TYPE_PLAIN;
}

inline SgUctValue GoUctFeatureKnowledge::Minimum() const
{
    return -1; // TODO
}

inline float GoUctFeatureKnowledge::MoveValue(const SgPoint move) const
{
    SG_ASSERT(UpToDate());
    return m_eval[move];
}

inline SgUctValue GoUctFeatureKnowledge::Scale() const
{
    return 1.0; // TODO
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
