//----------------------------------------------------------------------------
/** @file GoPatternBase.cpp
 See GoPatternBase.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoPatternBase.h"

#include "GoBoardUtil.h"
#include "SgDebug.h"

//----------------------------------------------------------------------------

std::vector<SgEmptyBlackWhite> GoPatternBase::Decode(int code, std::size_t length)
{
    const std::size_t origLength = length;
    SG_DEBUG_ONLY(origLength);

    std::vector<SgEmptyBlackWhite> colors;
    while (length-- > 0)
    {
        colors.push_back(code % 3);
        code /= 3;
    }
    std::reverse(colors.begin(), colors.end());
    // if this is time critical,
    // could work with reversed vectors and use rbegin, rend to access.
    SG_ASSERT_EQUAL(colors.size(), origLength);
    return colors;
}
//----------------------------------------------------------------------------
