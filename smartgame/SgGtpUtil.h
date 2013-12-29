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

//----------------------------------------------------------------------------

class GtpCommand;
class SgPointSet;

//----------------------------------------------------------------------------

/** SgRGB holds color information as used by the GoGui cboard response type */
struct SgRGB
{
public:
    SgRGB(unsigned char r, unsigned char g, unsigned char b)
    : m_r(r), m_g(g), m_b(b) {}

    /** Print as string in the format #rrggbb used by GoGui cboard. */
    std::string ToString() const;

    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
};

//----------------------------------------------------------------------------

/** Scale color brightness. Also used for gradients between two colors 
    @see SgGradient */
inline SgRGB operator*(float f, const SgRGB& color)
{
    return SgRGB(f * color.m_r,
                 f * color.m_g,
                 f * color.m_b);
}

/** Output to stream in the format #rrggbb used by GoGui cboard */
inline std::ostream& operator<<(std::ostream& stream, const SgRGB& color)
{
    boost::io::ios_flags_saver saver(stream);
    stream << '#' << std::hex << std::setfill('0') << std::setw(2)
           << int(color.m_r)
           << int(color.m_g)
           << int(color.m_b);
	return stream;
}

//----------------------------------------------------------------------------

namespace SgGtpUtil
{
    /** Append a point set to the response of a command.
        The response will be formatted as a single line with the points
        ordered (A1, B1, ..., A2, ...) to facilitate writing GTP regression
        tests using this command. */
    void RespondPointSet(GtpCommand& cmd, const SgPointSet& pointSet);

} // namespace SgGtpUtil

//----------------------------------------------------------------------------

#endif // SG_GTPUTIL_H
