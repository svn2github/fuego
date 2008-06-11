//----------------------------------------------------------------------------
/** @file FuegoMainUtil.h */
//----------------------------------------------------------------------------

#ifndef FUEGOMAIN_UTIL_H
#define FUEGOMAIN_UTIL_H

#include <string>

//----------------------------------------------------------------------------

namespace FuegoMainUtil
{
    /** Return Fuego version.
        If the macro VERSION was defined by the build system during compile
        time, its value will be used as the version, otherwise the version
        is "(__DATE__)". If compiled in debug mode, " (dbg)" will be added.
    */
    std::string Version();
}

//----------------------------------------------------------------------------

#endif // FUEGOMAIN_UTIL_H
