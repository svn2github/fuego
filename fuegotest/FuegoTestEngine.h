//----------------------------------------------------------------------------
/** @file FuegoTestEngine.h
*/
//----------------------------------------------------------------------------

#ifndef FUEGOTEST_ENGINE_H
#define FUEGOTEST_ENGINE_H

#include "GoGtpEngine.h"
#include "GoGtpExtraCommands.h"

//----------------------------------------------------------------------------

/** GTP interface with commands for testing functionality of the Fuego
    libraries.
    @see @ref fuegotestoverview
*/
class FuegoTestEngine
    : public GoGtpEngine
{
public:
    FuegoTestEngine(std::istream& in, std::ostream& out,
                    int initialBoardSize, const char* programPath = 0);

    ~FuegoTestEngine();

    void CmdAnalyzeCommands(GtpCommand& cmd);
    void CmdName(GtpCommand& cmd);
    void CmdVersion(GtpCommand& cmd);

private:
    GoGtpExtraCommands m_extraCommands;
};

//----------------------------------------------------------------------------

#endif // FUEGOTEST_ENGINE_H

