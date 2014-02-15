//----------------------------------------------------------------------------
/** @file GoUctFeatureKnowledge.h
    Convert feature weights from FeFeatureWeights into additive knowledge. */
//----------------------------------------------------------------------------


#ifndef GOUCT_FEATURE_KNOWLEDGE_H
#define GOUCT_FEATURE_KNOWLEDGE_H

#include "FeFeatureWeights.h"
#include "GoAdditiveKnowledge.h"
#include "GoBoard.h"
#include "GoUctPlayoutPolicy.h"

//----------------------------------------------------------------------------

class GoUctFeatureKnowledge
    : public GoAdditiveKnowledge
{
public:
    static const float VALUE_MULTIPLIER;

    GoUctFeatureKnowledge(const GoBoard& bd, const FeFeatureWeights& weights);

    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);

    GoPredictorType PredictorType() const;
    
    /** The minimum value allowed by this predictor */
    SgUctValue Minimum() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;

private:
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
