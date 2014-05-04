//----------------------------------------------------------------------------
/** @file GoPattern12Point.h */
//----------------------------------------------------------------------------

#ifndef GO_PATTERN_12_POINT_H
#define GO_PATTERN_12_POINT_H

#include "GoBoard.h"
#include "GoEvalArray.h"

//----------------------------------------------------------------------------
namespace GoPattern12Point {
    
    /** The 24 bit code for the pattern */
    unsigned int Context(const GoBoard& bd, SgPoint p,
                         const SgBlackWhite toPlay,
                         const SgBlackWhite opponent);

} // namespace GoPattern12Point

//----------------------------------------------------------------------------

#endif // GO_PATTERN_12_POINT_H
