//----------------------------------------------------------------------------
/** @file SpSimplePlayer.h
    Base class for simple Go playing algorithms.
*/
//----------------------------------------------------------------------------

#ifndef SPSIMPLEPLAYER_H
#define SPSIMPLEPLAYER_H

#include "GoBoard.h"
#include "GoPlayer.h"
#include "SgList.h"

class SgTimeRecord;
class SpMoveGenerator;
class SpRandomMoveGenerator;

//----------------------------------------------------------------------------

/** SimplePlayer has one move generator. 
    It generates random moves if no other move is found.
*/
class SpSimplePlayer
    : public GoPlayer
{
public:
    virtual ~SpSimplePlayer();

    SgPoint GenMove(const SgTimeRecord& time, SgBlackWhite toPlay);

    virtual int MoveValue(SgPoint p);

protected:
    SpSimplePlayer(GoBoard& board, SpMoveGenerator* generator,
                   bool atarigo = false);

    bool PlaysAtariGo() const
    {
        return m_atariGo;
    }
            
private:
    /** Move generator */
    SpMoveGenerator* m_generator;

    /** Use random generator if no other move found */
    SpRandomMoveGenerator* m_randomGenerator;

    /** AtariGo flag causes following changes:
        -Always capture if possible. 
        -Pass move disabled. 
        -Random moves may fill own eyes. */
    const bool m_atariGo;

    /** Don't play on safe points */
    virtual bool UseFilter() const 
    {
        return true;
    }    
};

//----------------------------------------------------------------------------

#endif // SPSIMPLEPLAYER_H

