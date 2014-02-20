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

class GoUctFeatureKnowledge
    : public GoAdditiveKnowledge, GoUctKnowledge
{
public:
    GoUctFeatureKnowledge(const GoBoard& bd, const FeFeatureWeights& weights);

    bool DoesUseAdditivePredictor() const;

    bool DoesUseAsVirtualWins() const;

    /** Apply as additive predictor */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);

    GoPredictorType PredictorType() const;
    
    /** The minimum value allowed by this predictor */
    SgUctValue Minimum() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;

    void UseAsAdditivePredictor(bool use);

    void UseAsVirtualWins(bool use);

private:
    void SetWinsLosses(SgPoint move, float moveValue);

    static const float VALUE_MULTIPLIER;

    FeFeatureWeights m_weights;

    GoUctPlayoutPolicy<GoBoard> m_policy;

    bool m_useAsAdditivePredictor;

    bool m_useAsVirtualWins;
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

inline void GoUctFeatureKnowledge::UseAsAdditivePredictor(bool use)
{
    m_useAsAdditivePredictor = use;
}

inline void GoUctFeatureKnowledge::UseAsVirtualWins(bool use)
{
    m_useAsVirtualWins = use;
}

inline bool GoUctFeatureKnowledge::DoesUseAdditivePredictor() const
{
    return m_useAsAdditivePredictor;
}

inline bool GoUctFeatureKnowledge::DoesUseAsVirtualWins() const
{
    return m_useAsVirtualWins;
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
