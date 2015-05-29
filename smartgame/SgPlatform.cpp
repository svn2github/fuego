//----------------------------------------------------------------------------
/** @file SgPlatform.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgPlatform.h"

#include <algorithm>

#ifdef WIN32

// MinGW already defines NOMINMAX
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#else
#include <unistd.h>
#endif
#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

//----------------------------------------------------------------------------
/** The program directory. Used for finding data files */
boost::filesystem::path s_programDir;

/** The top-level source directory (fuego). Used for finding data files */
boost::filesystem::path s_topSourceDir;

//----------------------------------------------------------------------------

const boost::filesystem::path& SgPlatform::GetProgramDir()
{
	return s_programDir;
}

void SgPlatform::SetProgramDir(const boost::filesystem::path& dir)
{
    s_programDir = dir;
}

const boost::filesystem::path& SgPlatform::GetTopSourceDir()
{
	return s_topSourceDir;
}

void SgPlatform::SetTopSourceDir(const boost::filesystem::path& dir)
{
    s_topSourceDir = dir;
}

//----------------------------------------------------------------------------

std::size_t SgPlatform::TotalMemory()
{
#if defined WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (! GlobalMemoryStatusEx(&status))
        return 0;
    size_t totalVirtual = static_cast<size_t>(status.ullTotalVirtual);
    size_t totalPhys = static_cast<size_t>(status.ullTotalPhys);
    return std::min(totalVirtual, totalPhys);
#elif defined _SC_PHYS_PAGES
    long pages = sysconf(_SC_PHYS_PAGES);
    if (pages < 0)
        return 0;
    long pageSize = sysconf(_SC_PAGE_SIZE);
    if (pageSize < 0)
        return 0;
    return static_cast<size_t>(pages) * static_cast<size_t>(pageSize);
#elif defined HW_PHYSMEM
    // Mac OSX, BSD
    unsigned int mem;
    size_t len = sizeof mem;
    int mib[2] = { CTL_HW, HW_PHYSMEM };
    if (sysctl(mib, 2, &mem, &len, 0, 0) != 0 || len != sizeof mem)
        return 0;
    else
        return mem;
#else
    return 0;
#endif
}

//----------------------------------------------------------------------------

