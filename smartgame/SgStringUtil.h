//----------------------------------------------------------------------------
/** @file SgStringUtil.h */
//----------------------------------------------------------------------------

#ifndef SG_STRINGUTIL_H
#define SG_STRINGUTIL_H

#include <string>
#include <boost/filesystem.hpp>
#include <vector>

//----------------------------------------------------------------------------

/** String utility functions. */
namespace SgStringUtil
{
	/** Convert generic into native file name */
    std::string GetNativeFileName(
    	const boost::filesystem::path& file);
	
    /** Split command line into arguments.
        Allows " for words containing whitespaces. */
    std::vector<std::string> SplitArguments(std::string s);
}

//----------------------------------------------------------------------------

#endif // SG_STRINGUTIL_H
