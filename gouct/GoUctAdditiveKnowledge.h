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
    GoUctAdditiveKnowledge(const GoBoard& bd);

    virtual ~GoUctAdditiveKnowledge();

    virtual void ProcessPosition(std::vector<SgUctMoveInfo>& moves) = 0;

    virtual const GoBoard& Board() const;
    
    virtual bool ProbabilityBased() const = 0;
    
    /** return value, but lower bound capped by Minimum() */
    SgUctValue CappedValue(SgUctValue value) const;
    
    /** The minimum value allowed by this predictor */
    virtual SgUctValue Minimum() const = 0;

    /** The scaling factor for this predictor */
    virtual SgUctValue Scale() const = 0;

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
};

//----------------------------------------------------------------------------
inline const GoBoard& GoUctAdditiveKnowledge::Board() const
{
	return m_bd;
}

inline SgUctValue GoUctAdditiveKnowledge::CappedValue(SgUctValue value) const
{
	return std::max(value, Minimum());
}

//----------------------------------------------------------------------------
/** Utility class to provide a standard probability-based additive predictor
*/
class GoUctAdditiveKnowledgeStdProb : public GoUctAdditiveKnowledge
{
public:
    GoUctAdditiveKnowledgeStdProb(const GoBoard& bd)
     : GoUctAdditiveKnowledge(bd)
    { }

    bool ProbabilityBased() const;
    
    /** The minimum value allowed by this predictor */
    SgUctValue Minimum() const;
    
    /** The scaling factor for this predictor */
    SgUctValue Scale() const;
};
//----------------------------------------------------------------------------

inline SgUctValue GoUctAdditiveKnowledgeStdProb::Minimum() const
{
	return 0.0001f;
}

inline bool GoUctAdditiveKnowledgeStdProb::ProbabilityBased() const
{
	return true;
}

inline SgUctValue GoUctAdditiveKnowledgeStdProb::Scale() const
{
	return 0.03f;
}

#endif // GOUCT_ADDITIVEKNOWLEDGE_H
