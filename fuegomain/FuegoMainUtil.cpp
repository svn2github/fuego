//----------------------------------------------------------------------------
/** @file FuegoMainUtil.cpp
    See FuegoMainUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoMainUtil.h"

#include <sstream>

using namespace std;

//----------------------------------------------------------------------------

std::string FuegoMainUtil::Version()
{
    ostringstream s;
#ifdef VERSION
    s << VERSION;
#else
    s << "(" __DATE__ ")";
#endif
#ifdef _DEBUG
    s << " (dbg)";
#endif
    return s.str();
}

//----------------------------------------------------------------------------
