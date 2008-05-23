//----------------------------------------------------------------------------
/** @file SgException.h
    Base class for exceptions.
*/
//----------------------------------------------------------------------------

#ifndef SGEXCEPTION_H
#define SGEXCEPTION_H

#include <exception>
#include <string>

//----------------------------------------------------------------------------

/** Base class for exceptions. */
class SgException
    : public std::exception
{
public:
    /** Construct with no message. */
    SgException();

    /** Construct with message. */
    SgException(const std::string& message);
    
    /** Destructor. */
    virtual ~SgException() throw();

    /** Implementation of std::exception::what(). */
    const char* what() const throw();

private:
    std::string m_message;
};

//----------------------------------------------------------------------------

#endif // SGEXCEPTION_H

