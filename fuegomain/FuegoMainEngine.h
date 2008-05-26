//----------------------------------------------------------------------------
/** @file FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#ifndef FUEGOMAINENGINE_H
#define FUEGOMAINENGINE_H

#include "GoGtpEngine.h"
#include "GoUctCommands.h"

class GoUctGlobalSearchPlayer;

//----------------------------------------------------------------------------

/** GTP engine for GoUctGlobalSearchPlayer. */
class FuegoMainEngine
    : public GoGtpEngine
{
public:
    FuegoMainEngine(std::istream& in, std::ostream& out,
                    const char* programPath = 0);

    ~FuegoMainEngine();

    void CmdAnalyzeCommands(GtpCommand& cmd);

    void CmdName(GtpCommand& cmd);

private:
    GoUctCommands m_uctCommands;
};

//----------------------------------------------------------------------------

#endif // FUEGOMAINENGINE_H

