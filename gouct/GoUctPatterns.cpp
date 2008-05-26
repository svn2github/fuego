//----------------------------------------------------------------------------
/** @file GoUctPatterns.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctPatterns.h"

//----------------------------------------------------------------------------

// The coding of point neighborhoods as a single integer index
// relies on the following settings.

BOOST_STATIC_ASSERT(SG_BLACK == 1);
BOOST_STATIC_ASSERT(SG_WHITE == 2);
BOOST_STATIC_ASSERT(SG_EMPTY == 4);

//----------------------------------------------------------------------------
