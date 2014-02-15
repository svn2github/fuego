//----------------------------------------------------------------------------
/** @file SgAdditiveKnowledge.h
    Additive knowledge used by SgUctSearch */
//----------------------------------------------------------------------------

#ifndef SG_ADDITIVEKNOWLEDGE_H
#define SG_ADDITIVEKNOWLEDGE_H

#include "SgUctValue.h"

//----------------------------------------------------------------------------

/** Additive knowledge used by SgUctSearch. */
class SgAdditiveKnowledge
{
public:

	SgAdditiveKnowledge() : m_knowledgeWeight(1),
                            m_predictorDecay(5.0f)
	{ }

    /** Strength of bias in upper confidence bound for child selection.
        bound -= m_additiveknowledgeweight * additive_knowledge */
    SgUctValue KnowledgeWeight() const;

    /** The rate at which the additive predictor decays */
    SgUctValue PredictorDecay() const;
    
 	/** Compute weight from decay and posCount */
    SgUctValue PredictorWeight(SgUctValue posCount) const;

    /** See KnowledgeWeight() */
    void SetKnowledgeWeight(SgUctValue weight);

    /** See PredictorDecay() */
    void SetPredictorDecay(SgUctValue value);

private:
    /** See KnowledgeWeight() */
    SgUctValue m_knowledgeWeight;

    /* See PredictorDecay() */
    SgUctValue m_predictorDecay;

};

inline SgUctValue SgAdditiveKnowledge::KnowledgeWeight() const
{
	return m_knowledgeWeight;
}

inline SgUctValue SgAdditiveKnowledge::PredictorDecay() const
{
    return m_predictorDecay;
}

inline void SgAdditiveKnowledge::SetKnowledgeWeight(SgUctValue weight)
{
	m_knowledgeWeight = weight;
}

inline void SgAdditiveKnowledge::SetPredictorDecay(SgUctValue value)
{
    m_predictorDecay = value;
}

inline SgUctValue SgAdditiveKnowledge::PredictorWeight(SgUctValue posCount)
const
{
    return   m_knowledgeWeight
           * sqrt(m_predictorDecay / (posCount + m_predictorDecay));
}


//----------------------------------------------------------------------------

#endif // SG_ADDITIVEKNOWLEDGE_H
