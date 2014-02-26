//----------------------------------------------------------------------------
/** @file GoAdditiveKnowledge.h 
    Base classes for additive knowledge.
    See Chris Rosin, Multi-armed bandits with episode context. 
    Annals of Mathematics and Artificial Intelligence, 2011. 
    DOI: 10.1007/s10472-011-9258-6

    The implementation is analogous to GoUctDefaultPriorKnowledge.h.
    But it is not completely compatible.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_ADDITIVEKNOWLEDGE_H
#define GOUCT_ADDITIVEKNOWLEDGE_H

#include "GoBoard.h"
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Base class for predictor param; constructed once and shared by threads. */
class GoUctAdditiveKnowledgeParam
{
public:
    GoUctAdditiveKnowledgeParam();

    virtual ~GoUctAdditiveKnowledgeParam();
};


enum GoPredictorType
{
    GO_PRED_TYPE_PROBABILITY_BASED, GO_PRED_TYPE_PUCB, GO_PRED_TYPE_PLAIN
};

/** Base class for predictors that will be used additively in UCT. 
    Constructed once per thread (as with GoUctDefaultPriorKnowledge). 
*/
class GoAdditiveKnowledge
{
public:
    GoAdditiveKnowledge(const GoBoard& bd);

    virtual ~GoAdditiveKnowledge();

    virtual void ProcessPosition(std::vector<SgUctMoveInfo>& moves) = 0;

    virtual const GoBoard& Board() const;
    
    virtual GoPredictorType PredictorType() const = 0;
    
    /** return value, but lower bound capped by MinValue() */
    SgUctValue RaiseToMinValue(SgUctValue value) const;
    
    /** The minimum value allowed by this predictor */
    virtual SgUctValue MinValue() const = 0;

    /** The scaling factor for this predictor */
    virtual SgUctValue Scale() const = 0;

private:

    /** The board on which prior knowledge is computed */
    const GoBoard& m_bd;
};

//----------------------------------------------------------------------------
inline const GoBoard& GoAdditiveKnowledge::Board() const
{
	return m_bd;
}

inline SgUctValue GoAdditiveKnowledge::RaiseToMinValue(SgUctValue value) const
{
	return std::max(value, MinValue());
}

//----------------------------------------------------------------------------
/** Utility class to provide a standard probability-based additive predictor
*/
class GoUctAdditiveKnowledgeStdProb : public GoAdditiveKnowledge
{
public:
    GoUctAdditiveKnowledgeStdProb(const GoBoard& bd)
     : GoAdditiveKnowledge(bd)
    { }

    GoPredictorType PredictorType() const;
    
    /** The minimum value allowed by this predictor */
    SgUctValue MinValue() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;
};
//----------------------------------------------------------------------------

inline SgUctValue GoUctAdditiveKnowledgeStdProb::MinValue() const
{
	return 0.0001f;
}

inline GoPredictorType GoUctAdditiveKnowledgeStdProb::PredictorType() const
{
	return GO_PRED_TYPE_PROBABILITY_BASED;
}

inline SgUctValue GoUctAdditiveKnowledgeStdProb::Scale() const
{
	return 0.03f;
}

#endif // GOUCT_ADDITIVEKNOWLEDGE_H
