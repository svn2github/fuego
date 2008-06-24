//----------------------------------------------------------------------------
/** @file SpMinLibPlayer.h
    Minimum liberty player - blocks with fewest liberties are most urgent.
*/
//----------------------------------------------------------------------------

#ifndef SPMINLIBPLAYER_H
#define SPMINLIBPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"


//----------------------------------------------------------------------------

/** Tries to maximize minimum liberty of own minus opponent blocks.
*/
class SpMinLibMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    explicit SpMinLibMoveGenerator(GoBoard& board)
        : Sp1PlyMoveGenerator(board)
    { }

    virtual int Evaluate();

    int LibertyMinimum(SgBlackWhite toplay);
};

//----------------------------------------------------------------------------

/** Simple player using SpMinLibMoveGenerator */
class SpMinLibPlayer
    : public SpSimplePlayer
{
public:
    SpMinLibPlayer(GoBoard& board, bool atarigo = false)
        : SpSimplePlayer(board, new SpMinLibMoveGenerator(board), atarigo)
    { }

    std::string Name() const
    {
        return "MinLib";
    }
    
    bool UseFilter() const
    { 
        return ! PlaysAtariGo(); 
    }
};

#endif

