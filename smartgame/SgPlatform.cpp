//----------------------------------------------------------------------------
/** @file SgPlatform.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgPlatform.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

//----------------------------------------------------------------------------

std::size_t SgPlatform::TotalMemory()
{
#ifdef WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof (status);
    GlobalMemoryStatusEx(&status);
    return static_cast<size_t>(status.ullTotalPhys);
#else
    return sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
#endif
}

//----------------------------------------------------------------------------

