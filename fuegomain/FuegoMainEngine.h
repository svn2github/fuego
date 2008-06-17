//----------------------------------------------------------------------------
/** @file FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#ifndef FUEGOMAIN_ENGINE_H
#define FUEGOMAIN_ENGINE_H

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
                    int initialBoardSize, const char* programPath = 0);

    ~FuegoMainEngine();

    void CmdAnalyzeCommands(GtpCommand& cmd);
    void CmdName(GtpCommand& cmd);
    void CmdVersion(GtpCommand& cmd);

private:
    GoUctCommands m_uctCommands;
};

//----------------------------------------------------------------------------

#endif // FUEGOMAIN_ENGINE_H

