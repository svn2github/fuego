//----------------------------------------------------------------------------
/** @file GoPlayer.cpp
    @see GoPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoPlayer.h"

#include <limits>
#include "GoBoard.h"

using namespace std;

//----------------------------------------------------------------------------

GoPlayer::GoPlayer(GoBoard& board)
    : GoBoardSynchronizer(board),
      m_currentNode(0),
      m_board(0),
      m_variant(0)
{
    m_board = new GoBoard(board.Size(), GoSetup(), board.Rules());
    SetSubscriber(*m_board);
}

GoPlayer::~GoPlayer()
{
    SG_ASSERT(m_board);
    delete m_board;
    m_board = 0;
}

int GoPlayer::MoveValue(SgPoint p)
{
    SG_UNUSED(p);
    return numeric_limits<int>::min();
}

void GoPlayer::OnGameFinished()
{
}

void GoPlayer::OnNewGame()
{
}

void GoPlayer::Ponder()
{
}

//----------------------------------------------------------------------------

