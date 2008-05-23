//----------------------------------------------------------------------------
/** @file SgStringUtil.h */
//----------------------------------------------------------------------------

#ifndef SGSTRINGUTIL_H
#define SGSTRINGUTIL_H

#include <vector>
#include <string>

//----------------------------------------------------------------------------

/** String utility functions. */
namespace SgStringUtil
{
    /** Split command line into arguments.
        Allows " for words containing whitespaces.
    */
    std::vector<std::string> SplitArguments(std::string s);
}

//----------------------------------------------------------------------------

#endif // SGSTRINGUTIL_H
