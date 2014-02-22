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

    /** Apply as additive predictor */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
    
    void ApplyAdditivePredictor(std::vector<SgUctMoveInfo>& moves,
                         const GoUctFeatureKnowledgeParam& param);
    
    GoPredictorType PredictorType() const;
    
    /** The minimum value allowed by this predictor */
    SgUctValue Minimum() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;

private:
    void SetWinsLosses(SgPoint move, float moveValue,
                       const GoUctFeatureKnowledgeParam& param);

    FeFeatureWeights m_weights;

    GoUctPlayoutPolicy<GoBoard> m_policy;
    
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

inline SgUctValue GoUctFeatureKnowledge::Scale() const
{
    return 1.0; // TODO
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
