//----------------------------------------------------------------------------
/** @file GoUctKnowledgeFactory.h 
    Factory for creating different types of additive knowledge.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_KNOWLEDGE_FACTORY_H
#define GOUCT_KNOWLEDGE_FACTORY_H

#include "GoBoard.h"
#include "GoUctAdditiveKnowledgeGreenpeep.h"
#include "GoUctFeatureKnowledge.h"

//----------------------------------------------------------------------------
class GoUctKnowledgeFactory
{
public:
	GoUctKnowledgeFactory(const GoUctPlayoutPolicyParam& param);
	~GoUctKnowledgeFactory();

    GoAdditiveKnowledge* Create(const GoBoard& bd);

    GoAdditiveKnowledge* CreateByType(const GoBoard& bd, KnowledgeType type);

    GoUctAdditiveKnowledgeParamGreenpeep& GreenpeepParam();

private:
    GoUctAdditiveKnowledgeParamGreenpeep* m_greenpeepParam; 

    /** The param used for additive knowledge */
    const GoUctPlayoutPolicyParam& m_param;
    
    GoUctFeatureKnowledgeFactory m_featureKnowledgeFactory;
};
//----------------------------------------------------------------------------

#endif //GOUCT_KNOWLEDGE_FACTORY_H
