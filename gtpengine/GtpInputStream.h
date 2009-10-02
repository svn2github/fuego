//----------------------------------------------------------------------------
/** @file GtpInputStream.h
*/
//----------------------------------------------------------------------------

#ifndef GTP_INPUT_STREAM_H_
#define GTP_INPUT_STREAM_H_

#include <iostream>
#include <string>

//----------------------------------------------------------------------------

/** Base class for input streams used by GtpEngine.
    This implementation only forwards calls to std::istream.
*/
class GtpInputStream
{
public:
    GtpInputStream(std::istream &in);

    virtual ~GtpInputStream();

    virtual bool EndOfInput();

    virtual bool GetLine(std::string &line);

private:
    std::istream &m_in;
};

//----------------------------------------------------------------------------

#endif // GTP_INPUT_STREAM_H_
