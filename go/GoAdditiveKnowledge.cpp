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
    : m_startMove(0), m_endMove(10000), m_bd(bd)
{ }

bool GoAdditiveKnowledge::InMoveRange(int moveNumber) const
{
	return moveNumber >= m_startMove
        && moveNumber <= m_endMove;
}

void GoAdditiveKnowledge::SetMoveRange(int startMove, int endMove)
{
	m_startMove = startMove;
    m_endMove = endMove;
}

GoAdditiveKnowledge::~GoAdditiveKnowledge()
{ }

//----------------------------------------------------------------------------
