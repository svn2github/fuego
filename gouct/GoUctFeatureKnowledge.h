//----------------------------------------------------------------------------
/** @file GoUctFeatureKnowledge.h
    Convert feature weights from FeFeatureWeights into additive knowledge. */
//----------------------------------------------------------------------------


#ifndef GOUCT_FEATURE_KNOWLEDGE_H
#define GOUCT_FEATURE_KNOWLEDGE_H

#include "FeFeatureWeights.h"
#include "GoAdditiveKnowledge.h"
#include "GoBoard.h"
#include "GoUctKnowledge.h"
#include "GoUctPlayoutPolicy.h"

//----------------------------------------------------------------------------
/** Parameters for GoUctFeatureKnowledge */
struct GoUctFeatureKnowledgeParam
{
    GoUctFeatureKnowledgeParam();
    
    /** map moveValue to additive term */
    float PredictorValue(float moveValue) const;

    /** map moveValue to [0..1] */
    float ProbabilityValue(float moveValue) const;

    bool m_useAsAdditivePredictor;
    
    bool m_useAsVirtualWins;

    float m_additiveFeatureMultiplier;
    
    float m_additiveFeatureSigmoidFactor;
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
    
    void SetWinsLosses(SgPoint move, float moveValue);

    bool UpToDate() const;
    
    SgHashCode m_code;
    
    SgPointArray<float> m_eval;
    
    float m_passEval;
    
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
    return (move == SG_PASS) ? m_passEval : m_eval[move];
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
