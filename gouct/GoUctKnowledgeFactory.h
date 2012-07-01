//----------------------------------------------------------------------------
/** @file GoUctKnowledgeFactory.h 
    Factory for creating different types of additive knowledge.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_KNOWLEDGE_FACTORY_H
#define GOUCT_KNOWLEDGE_FACTORY_H

#include "GoBoard.h"
#include "GoUctAdditiveKnowledgeGreenpeep.h"

//----------------------------------------------------------------------------
class GoUctKnowledgeFactory
{
public:
	GoUctKnowledgeFactory(const GoUctPlayoutPolicyParam& param);
	~GoUctKnowledgeFactory();

    GoUctAdditiveKnowledge* Create(const GoBoard& bd);

    GoUctAdditiveKnowledgeParamGreenpeep& GreenpeepParam();

private:
    GoUctAdditiveKnowledgeParamGreenpeep* m_greenpeepParam; 

    /** The param used for additive knowledge */
    const GoUctPlayoutPolicyParam& m_param;
};
//----------------------------------------------------------------------------

#endif //GOUCT_KNOWLEDGE_FACTORY_H
