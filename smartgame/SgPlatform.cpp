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
    status.dwLength = sizeof(status);
    if (! GlobalMemoryStatusEx(&status))
        return 0;
    return static_cast<size_t>(status.ullTotalPhys);
#else
#ifdef _SC_PHYS_PAGES
    long pages = sysconf(_SC_PHYS_PAGES);
    if (pages < 0)
        return 0;
    long pageSize = sysconf(_SC_PAGE_SIZE);
    if (pageSize < 0)
        return 0;
    return static_cast<size_t>(pages) * static_cast<size_t>(pageSize);
#else
	return 0;
#endif
#endif
}

//----------------------------------------------------------------------------

