//----------------------------------------------------------------------------
/** @file SgGtpUtil.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGtpUtil.h"

#include <sstream>
#include <string>
#include "SgPointSet.h"
#include "GtpEngine.h"

//----------------------------------------------------------------------------

std::string SgRGB::ToString() const
{
    std::ostringstream buffer;
    buffer << *this;
    return buffer.str();
}

//----------------------------------------------------------------------------

SgColorGradient::SgColorGradient(SgRGB start, float startVal,
                                 SgRGB end, float endVal)
    : m_start(start), m_startVal(startVal),
      m_end(end), m_endVal(endVal)
{
    SG_ASSERT(m_startVal < m_endVal);
}

SgRGB SgColorGradient::ColorOf(float value)
{
    SG_ASSERT(value >= m_startVal);
    SG_ASSERT(value <= m_endVal);

    float r = (value - m_startVal) / (m_endVal - m_startVal);
    SG_ASSERT(r >= 0.0);
    SG_ASSERT(r <= 1.0);
    return (1 - r) * m_start + r * m_end;
}

//----------------------------------------------------------------------------

void SgGtpUtil::RespondPointSet(GtpCommand& cmd, const SgPointSet& pointSet)
{
    bool isFirst = true;
    for (SgSetIterator it(pointSet); it; ++it)
    {
        if (! isFirst)
            cmd << ' ';
        isFirst = false;
        cmd << SgWritePoint(*it);
    }
}

//----------------------------------------------------------------------------
SgUctMoveSelect SgGtpUtil::MoveSelectArg(const GtpCommand& cmd, size_t number)
{
    std::string arg = cmd.ArgToLower(number);
    if (arg == "value")
        return SG_UCTMOVESELECT_VALUE;
    if (arg == "count")
        return SG_UCTMOVESELECT_COUNT;
    if (arg == "bound")
        return SG_UCTMOVESELECT_BOUND;
    if (arg == "estimate")
        return SG_UCTMOVESELECT_ESTIMATE;
    throw GtpFailure() << "unknown move select argument \"" << arg << '"';
}

std::string SgGtpUtil::MoveSelectToString(SgUctMoveSelect moveSelect)
{
    switch (moveSelect)
    {
        case SG_UCTMOVESELECT_VALUE:
            return "value";
        case SG_UCTMOVESELECT_COUNT:
            return "count";
        case SG_UCTMOVESELECT_BOUND:
            return "bound";
        case SG_UCTMOVESELECT_ESTIMATE:
            return "estimate";
        default:
            SG_ASSERT(false);
            return "?";
    }
}


