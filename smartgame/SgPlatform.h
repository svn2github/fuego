//----------------------------------------------------------------------------
/** @file SgPlatform.h */
//----------------------------------------------------------------------------

#ifndef SG_PLATFORM_H
#define SG_PLATFORM_H

#include <boost/filesystem/path.hpp>
#include <cstddef>

//----------------------------------------------------------------------------

/** Get information about the current computer. */
namespace SgPlatform
{
    /** Get native path for a file in fuego/data directory */
    std::string GetDataFileNativePath(const std::string& filename);

    /** @see SetProgramDir */
    const boost::filesystem::path& GetProgramDir();

    /** @see SetTopSourceDir */
    const boost::filesystem::path& GetTopSourceDir();

    /** Set the program directory. Should be called by main(). */
    void SetProgramDir(const boost::filesystem::path& dir);

    /** Set the top-level source directory (fuego). */
    void SetTopSourceDir(const boost::filesystem::path& dir);

    /** Get total amount of memory available on the system.
        @return The total memory in bytes or 0 if the memory cannot be
        determined. */
    std::size_t TotalMemory();

}

//----------------------------------------------------------------------------

#endif // SG_PLATFORM_H
