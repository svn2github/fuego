//----------------------------------------------------------------------------
/** @file SgPlatform.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgPlatform.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

//----------------------------------------------------------------------------

long SgPlatform::TotalMemory()
{
#ifdef WIN32
    MEMORYSTATUSEX status;
    GetMemoryStatusEx(&status);
    return status.ullTotalPhys;
#else
    return sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
#endif
}

//----------------------------------------------------------------------------

