//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledge.cpp
    See GoUctAdditiveKnowledge.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctAdditiveKnowledge.h"

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeParam::GoUctAdditiveKnowledgeParam()
{ }

GoUctAdditiveKnowledgeParam::~GoUctAdditiveKnowledgeParam()
{ }

GoUctAdditiveKnowledge::GoUctAdditiveKnowledge(const GoBoard& bd)
    : m_startMove(0), m_endMove(10000), m_bd(bd)
{ }

bool GoUctAdditiveKnowledge::InMoveRange(int moveNumber) const
{
	return moveNumber >= m_startMove
        && moveNumber <= m_endMove;
}

void GoUctAdditiveKnowledge::SetMoveRange(int startMove, int endMove)
{
	m_startMove = startMove;
    m_endMove = endMove;
}

GoUctAdditiveKnowledge::~GoUctAdditiveKnowledge()
{ }

//----------------------------------------------------------------------------


