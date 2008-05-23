//----------------------------------------------------------------------------
/** @file SgIO.h
    Logging stream.
 */
//----------------------------------------------------------------------------

#ifndef SGIO_H
#define SGIO_H

#include <iosfwd>
#include <sstream>

//----------------------------------------------------------------------------

/** Current logging stream. */
std::ostream& SgDebug();

//----------------------------------------------------------------------------

/** Set logging stream to file.
    Uses the filename "explorer.out" by default.
*/
void SgDebugToFile(const char* filename = "explorer.out");

/** Set logging stream to null stream.
    Discards everything written to SgDebug().
*/
void SgDebugToNull();

/** Set logging stream to console window.
    Uses std::cerr if UNIX is defined, std::cout otherwise.
*/
void SgDebugToWindow();

std::ostream* SgSwapDebugStr(std::ostream* newStr);

//----------------------------------------------------------------------------

/** Temporarily redirect IO to file for lifetime of this object */
class SgDebugToNewFile
{
public:
    explicit SgDebugToNewFile(const char* filename);

    explicit SgDebugToNewFile();

    void SetFile(const char* filename);
    
    ~SgDebugToNewFile();

private:
    std::ostream* m_old;
};

//----------------------------------------------------------------------------

/** Temporarily redirect IO to a string buffer for lifetime of this object */
class SgDebugToString
{
public:
    /** Constructor.
        @param writeToOldDebugStr Also write the content of the string to the
        old stream, after it was reset in the desctructor.
    */
    SgDebugToString(bool writeToOldDebugStr);

    ~SgDebugToString();

    std::string GetString() const { return m_str.str(); }

private:
    bool m_writeToOldDebugStr;

    std::ostringstream m_str;

    std::ostream* m_old;
};

//----------------------------------------------------------------------------

#endif // SGIO_H

