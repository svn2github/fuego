//----------------------------------------------------------------------------
/** @file SgMemCheck.cpp
    @see SgMemCheck.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgMemCheck.h"

#include "SgList.h"
#include "SgNode.h"

//----------------------------------------------------------------------------

void SgMemCheck()
{
    SgList<int>::MemCheck();
    SgList<void*>::MemCheck();
    SgNode::MemCheck();
}

//----------------------------------------------------------------------------

