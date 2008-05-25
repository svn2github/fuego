//----------------------------------------------------------------------------
/** @file GoBoardRestorer.h
*/
//----------------------------------------------------------------------------

#ifndef GOBOARDRESTORER_H
#define GOBOARDRESTORER_H

#include "GoBoard.h"

//----------------------------------------------------------------------------

/** Remembers the state of a board and restores it in its destructor. */
class GoBoardRestorer
{
public:
    GoBoardRestorer(GoBoard& bd);

    ~GoBoardRestorer();

private:
    typedef SgSList<GoPlayerMove,GO_MAX_NUM_MOVES> MoveList;

    GoBoard& m_bd;

    int m_size;

    GoRules m_rules;

    MoveList m_moves;
};

//----------------------------------------------------------------------------

#endif // GOBOARDRESTORER_H
