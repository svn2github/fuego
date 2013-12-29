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

