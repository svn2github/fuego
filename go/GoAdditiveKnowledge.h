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

#ifndef GO_ADDITIVEKNOWLEDGE_H
#define GO_ADDITIVEKNOWLEDGE_H

#include <vector>
#include "GoBoard.h"
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Knowledge type is used for knowledge factories. */
enum GoKnowledgeType
{
    KNOWLEDGE_NONE,
    KNOWLEDGE_GREENPEEP,
    KNOWLEDGE_RULEBASED,
    KNOWLEDGE_FEATURES,
    KNOWLEDGE_BOTH
};

/** Combination Type is used for combining multiple additive knowledge. */
enum GoKnowledgeCombinationType
{
    COMBINE_MULTIPLY,
    COMBINE_GEOMETRIC_MEAN,
    COMBINE_ADD,
    COMBINE_AVERAGE,
    COMBINE_MAX
};

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
    Note that all predictors are assumed to be of type float.
    In contrast, values used in SgUctSearch are of type SgUctValue,
    which is user definable but is a double by default.
    Care must be taken to minimize expensive conversions.
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
    float RaiseToMinValue(float value) const;
    
    /** The minimum value allowed by this predictor */
    virtual float MinValue() const = 0;

private:

    /** The board on which prior knowledge is computed */
    const GoBoard& m_bd;
};

//----------------------------------------------------------------------------
inline const GoBoard& GoAdditiveKnowledge::Board() const
{
	return m_bd;
}

inline float GoAdditiveKnowledge::RaiseToMinValue(float value) const
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
    float MinValue() const;
};
//----------------------------------------------------------------------------

inline float GoUctAdditiveKnowledgeStdProb::MinValue() const
{
	return 0.0001f;
}

inline GoPredictorType GoUctAdditiveKnowledgeStdProb::PredictorType() const
{
	return GO_PRED_TYPE_PROBABILITY_BASED;
}

#endif // GO_ADDITIVEKNOWLEDGE_H
