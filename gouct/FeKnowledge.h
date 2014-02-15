//----------------------------------------------------------------------------
/** @file FeKnowledge.h
    Convert feature weights from FeFeatureWeights into additive knowledge. */
//----------------------------------------------------------------------------


#ifndef FE_KNOWLEDGE_H
#define FE_KNOWLEDGE_H

#include "FeFeatureWeights.h"
#include "GoAdditiveKnowledge.h"
#include "GoBoard.h"
#include "GoUctPlayoutPolicy.h"

//----------------------------------------------------------------------------

class FeKnowledge
    : public GoAdditiveKnowledge
{
public:
    static const float VALUE_MULTIPLIER;

    FeKnowledge(const GoBoard& bd, const FeFeatureWeights& weights);

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
inline GoPredictorType FeKnowledge::PredictorType() const
{
    return GO_PRED_TYPE_PLAIN;
}

inline SgUctValue FeKnowledge::Minimum() const
{
    return -1; // TODO
}

inline SgUctValue FeKnowledge::Scale() const
{
    return 1.0; // TODO
}

//----------------------------------------------------------------------------
class FeKnowledgeFactory
{
public:
    
	FeKnowledgeFactory();
    
	~FeKnowledgeFactory();
    
    GoAdditiveKnowledge* Create(const GoBoard& bd);
    
private:
    
    void ReadWeights();
    
    FeFeatureWeights m_weights;
};
//----------------------------------------------------------------------------

#endif // FE_KNOWLEDGE_H
