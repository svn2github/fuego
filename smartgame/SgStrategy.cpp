//----------------------------------------------------------------------------
/** @file SgStrategy.cpp
    See SgStrategy.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgStrategy.h"

#include <iostream>
#include "SgWrite.h"

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, SgStrategyStatus f)
{
    static const char* s_string[nuSgStrategyStatus] = 
        {
            "strAchieved",
            "strThreatened",
            "strUnknown",
            "strFailed"
        };

    stream << s_string[f];
    return stream;
}

//----------------------------------------------------------------------------

SgStrategy::SgStrategy(SgBlackWhite player)
    : m_player(player)
{ }

std::ostream& operator<<(std::ostream& stream, const SgStrategy& s)
{
    s.Write(stream);
    return stream;
}

void SgStrategy::Write(std::ostream& stream) const
{
    stream << "Player " << BW(m_player)
           << ", Status " << Status()
           << '\n';
}

void SgStrategy::Clear()
{
    m_code.Clear();
}
