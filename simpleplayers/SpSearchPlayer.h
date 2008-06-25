//----------------------------------------------------------------------------
/** @file SpSearchPlayer.h
    Base class for simple search-based Go playing algorithms.
*/
//----------------------------------------------------------------------------

#ifndef SP_SEARCHPLAYER_H
#define SP_SEARCHPLAYER_H

#include "GoBoard.h"
#include "GoPlayer.h"
#include "SgSearch.h"
#include "SgTimeRecord.h"
#include "SpChaineyPlayer.h"

//----------------------------------------------------------------------------

class SpSearchPlayer
    : public GoPlayer
{
public:
    SpSearchPlayer(GoBoard& board);

    virtual ~SpSearchPlayer();

    SgPoint GenMove(const SgTimeRecord& time, SgBlackWhite toPlay);

    virtual int MoveValue(SgPoint p);

private:
    SpChaineyMoveGenerator m_generator;
    SgSearchHashTable* m_hash;
};
//----------------------------------------------------------------------------

#endif // SP_SEARCHPLAYER_H

