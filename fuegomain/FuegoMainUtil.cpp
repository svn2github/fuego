//----------------------------------------------------------------------------
/** @file FuegoMainUtil.cpp
    See FuegoMainUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoMainUtil.h"

#include <sstream>
#include <boost/preprocessor/stringize.hpp>

using namespace std;

//----------------------------------------------------------------------------

std::string FuegoMainUtil::Version()
{
    ostringstream s;
    #ifdef VERSION
    s << BOOST_PP_STRINGIZE(VERSION);
#else
    s << "(" __DATE__ ")";
#endif
#ifdef _DEBUG
    s << " (dbg)";
#endif
    return s.str();
}

//----------------------------------------------------------------------------
