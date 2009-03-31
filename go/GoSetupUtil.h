//----------------------------------------------------------------------------
/** @file GoSetupUtil.h
    utilities to create setup information for GoBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_SETUPUTIL_H
#define GO_SETUPUTIL_H

#include "GoBoard.h"
#include "GoSetup.h"

//----------------------------------------------------------------------------
namespace GoSetupUtil
{

    /** Get a setup instance with the current position on board. */
    GoSetup CurrentPosSetup(const GoBoard& bd);

} // namespace GoSetupUtil

//----------------------------------------------------------------------------

#endif // GO_SETUPUTIL_H

