//----------------------------------------------------------------------------
/** @file GoUctKnowledgeFactory.cpp
    See GoUctKnowledgeFactory.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctKnowledgeFactory.h"

#include "FeKnowledge.h"
#include "GoAdditiveKnowledge.h"
#include "GoUctAdditiveKnowledgeFuego.h"
#include "GoUctAdditiveKnowledgeGreenpeep.h"
#include "GoUctAdditiveKnowledgeMultiple.h"

//----------------------------------------------------------------------------
GoUctKnowledgeFactory::GoUctKnowledgeFactory(
    const GoUctPlayoutPolicyParam& param) :
    m_greenpeepParam(0),
    m_param(param),
    m_featureKnowledgeFactory()
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

GoAdditiveKnowledge* GoUctKnowledgeFactory::Create(const GoBoard& bd)
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
    case KNOWLEDGE_FEATURES:
    	return m_featureKnowledgeFactory.Create(bd);
    break;
    case KNOWLEDGE_BOTH:
    {
        GoUctAdditiveKnowledgeFuego* f = new GoUctAdditiveKnowledgeFuego(bd);
        SgUctValue scale = f->Scale();
        SgUctValue minimum = f->Minimum();

        GoUctAdditiveKnowledgeMultiple* m =
        new GoUctAdditiveKnowledgeMultiple(bd, scale,  minimum,
                                           m_param.m_combinationType);
        m->AddKnowledge(f);
        m->AddKnowledge(
            new GoUctAdditiveKnowledgeGreenpeep(bd, GreenpeepParam()));
        return m;
    }
    break;
    default:
    	SG_ASSERT(false);
        return 0;
    }
}
//----------------------------------------------------------------------------
