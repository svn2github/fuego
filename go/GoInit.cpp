//----------------------------------------------------------------------------
/** @file GoInit.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoInit.h"

#include "GoRegion.h"
#include "SgException.h"
#include "SgInit.h"
#include "SgProp.h"

//----------------------------------------------------------------------------

namespace {

bool s_isGoInitialized = false;

/** Register Go-game specific properties. */
void RegisterGoProps()
{
    SgProp* moveProp = new SgPropMove(0);
    SG_PROP_MOVE_BLACK
        = SgProp::Register(moveProp, "B", fMoveProp + fBlackProp);
    SG_PROP_MOVE_WHITE
        = SgProp::Register(moveProp, "W", fMoveProp + fWhiteProp);
}

} // namespace

//----------------------------------------------------------------------------

void GoFini()
{
    GoRegion::Fini();
    s_isGoInitialized = false;
}

void GoInit()
{
    SgInitCheck();
    RegisterGoProps();
    s_isGoInitialized = true;
}

void GoInitCheck()
{
    if (! s_isGoInitialized)
        throw SgException("GoInit not called");
}

//----------------------------------------------------------------------------

