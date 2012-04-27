//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledge.h 
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

/** Base class for predictors that will be used additively in UCT. 
    Constructed once per thread (as with GoUctDefaultPriorKnowledge). 
*/
class GoUctAdditiveKnowledge
{
public:
    GoUctAdditiveKnowledge(const GoBoard& bd, bool probabilityBased,
        SgUctValue scale, SgUctValue minimum);

    virtual ~GoUctAdditiveKnowledge();

    virtual void ProcessPosition(std::vector<SgUctMoveInfo>& moves) = 0;

    virtual const GoBoard& Board() const;
    
    virtual bool ProbabilityBased() const;
    
    /** return value, but lower bound capped by m_minimum */
    SgUctValue CappedValue(SgUctValue value) const;
    
    SgUctValue Minimum() const;

    SgUctValue Scale() const;

	/** Should predictor be appied for given move number? */
    bool InMoveRange(int moveNumber) const;
    
	/** @see m_startMove, m_endMove */
    void SetMoveRange(int startMove, int endMove);

private:
    /** Global move number where this predictor first applies. 
        It is compared against GoBoard::MoveNumber().
        Subclass constructor must configure it. 
    */
    int m_startMove;

    /** Global move number where this predictor last applies. 
        It is compared against GoBoard::MoveNumber().
        Subclass constructor must configure it. 
    */
    int m_endMove;

    /** The board on which prior knowledge is computed */
    const GoBoard& m_bd;

    /** Flag: all predictors must share the same type: true for
    probability-based predictor type (the usual case), false for
    PUCB-type predictor as in Rosin, Multi-armed bandits with episode context.
    */
    bool m_probabilityBased;
    
    SgUctValue m_scale;

    SgUctValue m_minimum;
};

//----------------------------------------------------------------------------
inline const GoBoard& GoUctAdditiveKnowledge::Board() const
{
	return m_bd;
}

inline bool GoUctAdditiveKnowledge::ProbabilityBased() const
{
	return m_probabilityBased;
}

inline SgUctValue GoUctAdditiveKnowledge::CappedValue(SgUctValue value) const
{
	return std::max(value, m_minimum);
}

inline SgUctValue GoUctAdditiveKnowledge::Minimum() const
{
	return m_minimum;
}

inline SgUctValue GoUctAdditiveKnowledge::Scale() const
{
	return m_scale;
}

//----------------------------------------------------------------------------

#endif // GOUCT_ADDITIVEKNOWLEDGE_H
