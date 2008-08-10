//----------------------------------------------------------------------------
/** @file FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#ifndef FUEGOMAIN_ENGINE_H
#define FUEGOMAIN_ENGINE_H

#include "GoGtpEngine.h"
#include "GoSafetyCommands.h"
#include "GoUctCommands.h"

class GoUctPlayer;

//----------------------------------------------------------------------------

/** GTP engine for GoUctPlayer. */
class FuegoMainEngine
    : public GoGtpEngine
{
public:
    FuegoMainEngine(std::istream& in, std::ostream& out,
                    int fixedBoardSize, const char* programPath = 0,
                    bool noHandicap = false);

    ~FuegoMainEngine();

    void CmdAnalyzeCommands(GtpCommand& cmd);
    void CmdName(GtpCommand& cmd);
    void CmdVersion(GtpCommand& cmd);

private:
    GoUctCommands m_uctCommands;

    GoSafetyCommands m_safetyCommands;
};

//----------------------------------------------------------------------------

#endif // FUEGOMAIN_ENGINE_H

