//----------------------------------------------------------------------------
/** @file GtpOutputStream.cpp */
//----------------------------------------------------------------------------
#include "GtpOutputStream.h"

//----------------------------------------------------------------------------

GtpOutputStream::GtpOutputStream(std::ostream &out)
    : m_out(out)
{ }

GtpOutputStream::~GtpOutputStream()
{ }

void GtpOutputStream::Write(const std::string &line)
{
    m_out << line;
}

void GtpOutputStream::Flush()
{
    m_out.flush();
}
    
//----------------------------------------------------------------------------
