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
    SgAdditiveKnowledge();

    /** The rate at which the additive predictor decays */
    SgUctValue PredictorDecay() const;
    
 	/** Compute weight from decay and posCount */
    SgUctValue PredictorWeight(SgUctValue posCount) const;

    /** See PredictorDecay() */
    void SetPredictorDecay(SgUctValue value);

private:
    /* See PredictorDecay() */
    SgUctValue m_predictorDecay;
};

inline SgAdditiveKnowledge::SgAdditiveKnowledge()
    : m_predictorDecay(5.0f)
{ }

inline SgUctValue SgAdditiveKnowledge::PredictorDecay() const
{
    return m_predictorDecay;
}

inline void SgAdditiveKnowledge::SetPredictorDecay(SgUctValue value)
{
    m_predictorDecay = value;
}

inline SgUctValue SgAdditiveKnowledge::PredictorWeight(SgUctValue posCount)
const
{
    return sqrt(m_predictorDecay / (posCount + m_predictorDecay));
}


//----------------------------------------------------------------------------

#endif // SG_ADDITIVEKNOWLEDGE_H
