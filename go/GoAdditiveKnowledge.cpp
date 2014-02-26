//----------------------------------------------------------------------------
/** @file GoAdditiveKnowledge.cpp
    See GoAdditiveKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoAdditiveKnowledge.h"

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeParam::GoUctAdditiveKnowledgeParam()
{ }

GoUctAdditiveKnowledgeParam::~GoUctAdditiveKnowledgeParam()
{ }

//----------------------------------------------------------------------------

GoAdditiveKnowledge::GoAdditiveKnowledge(const GoBoard& bd)
    : m_bd(bd)
{ }

GoAdditiveKnowledge::~GoAdditiveKnowledge()
{ }

//----------------------------------------------------------------------------
