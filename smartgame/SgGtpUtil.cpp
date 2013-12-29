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

