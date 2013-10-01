//----------------------------------------------------------------------------
/** @file GoKomi.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoKomi.h"

#include <sstream>

//----------------------------------------------------------------------------

namespace {

std::string GetInvalidKomiErrorMessage(float komi)
{
    std::ostringstream buffer;
    buffer << "Invalid komi value: " << komi;
    return buffer.str();
}

} // namespace

//----------------------------------------------------------------------------

GoKomi::InvalidKomi::InvalidKomi(float komi)
    : SgException(GetInvalidKomiErrorMessage(komi))
{ }

GoKomi::InvalidKomi::InvalidKomi(const std::string& komi)
    : SgException("Invalid komi value: " + komi)
{ }

//----------------------------------------------------------------------------

GoKomi::GoKomi(const std::string& komi)
{
    {
        std::istringstream buffer(komi);
        std::string trimmedString;
        buffer >> trimmedString;
        if (! buffer)
        {
            m_isUnknown = true;
            m_value = 0;
            return;
        }
    }
    {
        std::istringstream buffer(komi);
        float value;
        buffer >> value;
        if (! buffer)
            throw InvalidKomi(komi);
        *this = GoKomi(value);
    }
}

std::string GoKomi::ToString() const
{
    if (m_isUnknown)
        return "";
    if (m_value % 2 == 0)
    {
        std::ostringstream buffer;
        buffer << (m_value / 2);
        return buffer.str();
    }
    else if (m_value == -1)
        return "-0.5";
    else
    {
        std::ostringstream buffer;
        buffer << (m_value / 2) << ".5";
        return buffer.str();
    }
}

//----------------------------------------------------------------------------
