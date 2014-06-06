//----------------------------------------------------------------------------
/** @file GoPatternBase.h
 Generic base code for patterns on a GoBoard, independent of size.

 Patterns are used extensively in GoUctFeatures.
 */
//----------------------------------------------------------------------------

#ifndef GO_PATTERN_BASE_H
#define GO_PATTERN_BASE_H

#include <vector>
#include "SgBoardColor.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------
namespace GoPatternBase
{
    /** Convert code into list of board colors */
    std::vector<SgEmptyBlackWhite> Decode(int code, std::size_t length);

    template<class BOARD>
    int EBWCodeOfPoint(const BOARD& bd, SgPoint p);

    int MakeCode(const std::vector<SgEmptyBlackWhite>& colors);

    /** return NS for input WE or -WE, return WE for input NS or -NS */
    int OtherDir(int dir);
} // namespace GoPatternBase

//----------------------------------------------------------------------------

template<class BOARD>
inline int GoPatternBase::EBWCodeOfPoint(const BOARD& bd, SgPoint p)
{
    SG_ASSERT(bd.IsValidPoint(p));
    BOOST_STATIC_ASSERT(SG_BLACK == 0);
    BOOST_STATIC_ASSERT(SG_WHITE == 1);
    BOOST_STATIC_ASSERT(SG_EMPTY == 2);
    return bd.GetColor(p);
}

inline int GoPatternBase::MakeCode(const std::vector<SgEmptyBlackWhite>& colors)
{
    int code = 0;
    for (std::vector<SgEmptyBlackWhite>::const_iterator it = colors.begin();
         it != colors.end(); ++it)
    {
        code *= 3;
        code += *it;
    }
    return code;
}

inline int GoPatternBase::OtherDir(int dir)
{
    if (dir == SG_NS || dir == -SG_NS)
        return SG_WE;
    SG_ASSERT(dir == SG_WE || dir == -SG_WE);
    return SG_NS;
}

//----------------------------------------------------------------------------

#endif // GO_PATTERN_BASE_H
