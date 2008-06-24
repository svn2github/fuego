//----------------------------------------------------------------------------
/** @file SpUtil.h
    Utility functions
*/
//----------------------------------------------------------------------------

#ifndef SPUTIL_H
#define SPUTIL_H

#include "SgBlackWhite.h"
#include "SgPointSet.h"

class GoBoard;

//----------------------------------------------------------------------------

namespace SpUtil
{
    /** Get all legal moves.
        Filters out moves into own safe territory with no opponent stones
        adjacent if useFilter.
    */
    SgPointSet GetRelevantMoves(GoBoard& bd, SgBlackWhite toPlay,
                                bool useFilter);
}

//----------------------------------------------------------------------------

#endif // SPUTIL_H

