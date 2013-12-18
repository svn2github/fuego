//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeMultiple.h 
    A class for applying more than one kind of additive knowledge.
    
    Also see GoUctAdditiveKnowledge.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_ADDITIVE_KNOWLEDGE_MULTIPLE_H
#define GOUCT_ADDITIVE_KNOWLEDGE_MULTIPLE_H

#include "GoBoard.h"
#include "GoUctAdditiveKnowledge.h"
#include "GoUctPlayoutPolicy.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

/** @todo a similar typedef could be used globally. */
typedef std::vector<SgUctMoveInfo> InfoVector;

//----------------------------------------------------------------------------

/** A container used for applying multiple compatible types of knowledge.
    The container is initially empty. Knowledge must be added using
    AddKnowledge before using this container. All added knowledge must
    share the same parameters ProbabilityBased(), Minimum() and Scale().
 
    The knowledge is additive in the sense that its combined value is
    added in the UCT child selection formula. However, the method
    of combining multiple types of knowledge can be chosen - @see
    GoUctKnowledgeCombinationType.
*/
class GoUctAdditiveKnowledgeMultiple: public GoUctAdditiveKnowledge
{
public:
    GoUctAdditiveKnowledgeMultiple(const GoBoard& bd,
                                   SgUctValue scale,
                                   SgUctValue minimum,
                                   GoUctKnowledgeCombinationType
                                   combinationType);
    
    ~GoUctAdditiveKnowledgeMultiple();
    
    /** GoUctAdditiveKnowledgeMultiple assumes ownership of added knowledge */
    void AddKnowledge(GoUctAdditiveKnowledge* knowledge);

    const GoBoard& Board() const;
    
	bool ProbabilityBased() const;

    /** The minimum value allowed by this predictor */
    SgUctValue Minimum() const;

    /** The scaling factor for this predictor */
    SgUctValue Scale() const;

    void ProcessPosition(InfoVector& moves);

private:

    SgVector<GoUctAdditiveKnowledge*> m_additiveKnowledge;

    SgUctValue m_minimum;
    
    SgUctValue m_scale;
    
    GoUctKnowledgeCombinationType m_combinationType;

    const GoUctAdditiveKnowledge* FirstKnowledge() const;
    
    void InitPredictorValues(InfoVector& moves) const;
};

//----------------------------------------------------------------------------

inline SgUctValue GoUctAdditiveKnowledgeMultiple::Minimum() const
{
	return m_minimum;
}

inline SgUctValue GoUctAdditiveKnowledgeMultiple::Scale() const
{
	return m_scale;
}

//----------------------------------------------------------------------------

#endif // GOUCT_ADDITIVE_KNOWLEDGE_MULTIPLE_H
