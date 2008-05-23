//----------------------------------------------------------------------------
/** @file SgInit.cpp
    @see SgInit.h
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

void SgInit()
{
    SgProp::Init();
    s_isSgInitialized = true;
}

void SgInitCheck()
{
    if (! s_isSgInitialized)
        throw SgException("SgInit not called");
}

//----------------------------------------------------------------------------

