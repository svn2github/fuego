//----------------------------------------------------------------------------
/** @file GoUctKnowledgeFactory.cpp
    See GoUctKnowledgeFactory.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctKnowledgeFactory.h"

#include "GoUctAdditiveKnowledge.h"
#include "GoUctAdditiveKnowledgeFuego.h"
#include "GoUctAdditiveKnowledgeGreenpeep.h"

//----------------------------------------------------------------------------
GoUctKnowledgeFactory::GoUctKnowledgeFactory(
    const GoUctPlayoutPolicyParam& param) :
    m_greenpeepParam(0),
    m_param(param)
{ }

GoUctKnowledgeFactory::~GoUctKnowledgeFactory()
{
	if (m_greenpeepParam)
    	delete m_greenpeepParam;
}

GoUctAdditiveKnowledgeParamGreenpeep& GoUctKnowledgeFactory::GreenpeepParam()
{
	if (! m_greenpeepParam)
    	m_greenpeepParam = new GoUctAdditiveKnowledgeParamGreenpeep();
    return *m_greenpeepParam;
}

GoUctAdditiveKnowledge* GoUctKnowledgeFactory::Create(const GoBoard& bd)
{
	KnowledgeType type = m_param.m_knowledgeType;
    
    switch(type)
    {
    case KNOWLEDGE_NONE:
        return 0;
    case KNOWLEDGE_GREENPEEP:
    	return new GoUctAdditiveKnowledgeGreenpeep(bd,
                        GreenpeepParam());
    break;
    case KNOWLEDGE_RULEBASED:
    	return new GoUctAdditiveKnowledgeFuego(bd);
    break;
    default:
    	SG_ASSERT(false);
        return 0;
    }
}
//----------------------------------------------------------------------------
