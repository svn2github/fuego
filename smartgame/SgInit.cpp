//----------------------------------------------------------------------------
/** @file SgInit.cpp
    See SgInit.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgInit.h"

#include <iostream>
#include "SgException.h"
#include "SgMemCheck.h"
#include "SgProp.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

bool s_isSgInitialized = false;

} // namespace

//----------------------------------------------------------------------------

void SgFini()
{
    SgProp::Fini();
    SgMemCheck();
    s_isSgInitialized = false;
}

void SgInitImpl(bool compiledInDebugMode)
{
    // Compiling the library and user code with inconsistent definition
    // of _DEBUG causes undefined behavoior, since some of the SmartGame
    // classes contain additional debugging variables in debug mode, and the
    // user code will have different opinions about the size and layout of
    // these classes.
    // This function must not be inline, it needs to use the setting of
    // _DEBUG at the compile-time of the library.
#ifdef _DEBUG
    if (! compiledInDebugMode)
    {
        cerr <<
            "Incompatible library: SmartGame was compiled "
            "with _DEBUG, but main program without\n";
        abort();
    }
#else
    if (compiledInDebugMode)
    {
        cerr << "Incompatible library: SmartGame was compiled "
            "without _DEBUG, but main program with\n";
        abort();
    }
#endif

    SgProp::Init();
    s_isSgInitialized = true;
}

void SgInitCheck()
{
    if (! s_isSgInitialized)
        throw SgException("SgInit not called");
}

//----------------------------------------------------------------------------

