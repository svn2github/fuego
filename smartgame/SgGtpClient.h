//----------------------------------------------------------------------------
/** @file SgGtpClient.h */
//----------------------------------------------------------------------------

#ifndef SG_GTPCLIENT_H
#define SG_GTPCLIENT_H

#include <iostream>
#include <string>
#include "SgException.h"

//----------------------------------------------------------------------------

/** Error thrown by SgGtpClient::Send() if command fails or connection is
    broken.
*/
class SgGtpFailure
    : public SgException
{
public:
    /** Constructor.
        @param message The failure response of the command or other error
        message.
    */
    SgGtpFailure(const std::string& message);
};

//----------------------------------------------------------------------------

/** Client connection to an external GTP engine. */
class SgGtpClient
{
public:
    /** Constructor.
        @param in Input stream.
        @param out Output stream.
        @param verbose Log stream to SgDebug()
    */
    SgGtpClient(std::istream& in, std::ostream& out, bool verbose = false);

    virtual ~SgGtpClient();

    /** Send a command.
        @return The response if command succeeds (without status character
        and whitespace after status character)
        @throws SgGtpFailure If command fails or connection is broken.
    */
    std::string Send(const std::string& command);

private:
    bool m_verbose;

    std::istream& m_in;

    std::ostream& m_out;
};

//----------------------------------------------------------------------------

#endif // SG_GTPCLIENT_H
