//----------------------------------------------------------------------------
/** @file SgGtpUtil.h
    Utility functions for GTP and GoGui commands */
//----------------------------------------------------------------------------

#ifndef SG_GTPUTIL_H
#define SG_GTPUTIL_H

#include <boost/io/ios_state.hpp>
#include <iomanip>
#include <iostream>
#include <string>
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

class GtpCommand;
class SgPointSet;

//----------------------------------------------------------------------------

/** SgRGB holds color information as used by the GoGui cboard response type 
    @see http://gogui.sourceforge.net/doc/analyze.html#idp5719360 */
struct SgRGB
{
public:
    SgRGB(unsigned char r, unsigned char g, unsigned char b)
    : m_r(r), m_g(g), m_b(b)
    { }

    /** Print as string in the format rrggbb used by GoGui cboard. */
    std::string ToString() const;

    SgRGB operator+(const SgRGB& color) const;

    bool operator==(const SgRGB& color) const;

    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
};

//----------------------------------------------------------------------------

inline SgRGB SgRGB::operator+(const SgRGB& color) const
{
    return SgRGB(m_r + color.m_r,
                 m_g + color.m_g,
                 m_b + color.m_b);
}

inline bool SgRGB::operator==(const SgRGB& color) const
{
    return    m_r == color.m_r
           && m_g == color.m_g
           && m_b == color.m_b;
}

/** Scale color brightness. Also used for gradients between two colors
    @see SgGradient */
inline SgRGB operator*(float f, const SgRGB& color)
{
    return SgRGB(static_cast<unsigned char>(f * color.m_r),
                 static_cast<unsigned char>(f * color.m_g),
                 static_cast<unsigned char>(f * color.m_b));
}

/** Output to stream in the format rrggbb used by GoGui cboard */
inline std::ostream& operator<<(std::ostream& stream, const SgRGB& color)
{
    boost::io::ios_flags_saver saver(stream);
    stream << '#' << std::hex << std::setfill('0')
           << std::setw(2) << int(color.m_r)
           << std::setw(2) << int(color.m_g)
           << std::setw(2) << int(color.m_b);
	return stream;
}

//----------------------------------------------------------------------------

/** Utility struct for creating colors on a gradient between two colors */
struct SgColorGradient
{
public:
    SgColorGradient(SgRGB start, float startVal, SgRGB end, float endVal);

    /** Interpolate between start and end color
        value == startVal yields start, value = endVal yields end color 
        startVal < endVal must be true. */
    SgRGB ColorOf(float value);

private:

    SgRGB m_start;

    float m_startVal;

    SgRGB m_end;

    float m_endVal;
};

//----------------------------------------------------------------------------

namespace SgGtpUtil
{
    /** Append a point set to the response of a command.
        The response will be formatted as a single line with the points
        ordered (A1, B1, ..., A2, ...) to facilitate writing GTP regression
        tests using this command. */
    void RespondPointSet(GtpCommand& cmd, const SgPointSet& pointSet);

    SgUctMoveSelect MoveSelectArg(const GtpCommand& cmd, size_t number);
    
    std::string MoveSelectToString(SgUctMoveSelect moveSelect);

} // namespace SgGtpUtil

//----------------------------------------------------------------------------

#endif // SG_GTPUTIL_H
