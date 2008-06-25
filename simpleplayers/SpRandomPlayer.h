//----------------------------------------------------------------------------
/** @file SpRandomPlayer.h
    Random player
*/
//----------------------------------------------------------------------------

#ifndef SP_RANDOMPLAYER_H
#define SP_RANDOMPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"
#include "SgDebug.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Plays random moves, but does not fill obvious 1-point eyes */
class SpRandomMoveGenerator
    : public SpStaticMoveGenerator
{
public:
    SpRandomMoveGenerator(GoBoard& board, bool atarigo)
        : SpStaticMoveGenerator(board),
          m_atariGo(atarigo)
    { }

    virtual int Score(SgPoint p);
    
private:
    bool m_atariGo;
};

//----------------------------------------------------------------------------

/** Simple player using SpRandomMoveGenerator */
class SpRandomPlayer
    : public SpSimplePlayer
{
public:
    SpRandomPlayer(GoBoard& board, bool atarigo = false)
        : SpSimplePlayer(board, new SpRandomMoveGenerator(board, atarigo),
                         atarigo)
    { }

    std::string Name() const
    {
        return "Random";
    }

protected:
    bool UseFilter() const
    {
        return false;
    }
};

//----------------------------------------------------------------------------

#endif

