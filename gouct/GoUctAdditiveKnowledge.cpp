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
//----------------------------------------------------------------------------

GoUctAdditiveKnowledge::GoUctAdditiveKnowledge(const GoBoard& bd, 
                                               bool probabilityBased,
                                               SgUctValue scale, 
                                               SgUctValue minimum)
    : m_startMove(0), m_endMove(10000), m_bd(bd), 
      m_probabilityBased(probabilityBased),
      m_scale(scale), m_minimum(minimum)
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
