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
    /** Constructor.
        @param in
        @param out
        @param initialBoardSize
        @param programPath
        @param player Player ID as in CreatePlayer()
    */
    FuegoTestEngine(std::istream& in, std::ostream& out,
                    int initialBoardSize, const char* programPath = 0,
                    const std::string& player = "");

    ~FuegoTestEngine();

    /** @page fuegotestenginecommands FuegoTestEngine Commands
        - @link CmdPlayer() @c fuegotest_player @endlink
    */
    void CmdAnalyzeCommands(GtpCommand& cmd);
    void CmdName(GtpCommand& cmd);
    void CmdPlayer(GtpCommand& cmd);
    void CmdVersion(GtpCommand& cmd);

private:
    GoGtpExtraCommands m_extraCommands;

    /** Player ID as in CreatePlayer() */
    std::string m_playerId;

    GoPlayer* CreatePlayer(const std::string& name);

    void RegisterCmd(const std::string& name,
                     GtpCallback<FuegoTestEngine>::Method method);

    void SetPlayer(const std::string& playerId);
};

//----------------------------------------------------------------------------

#endif // FUEGOTEST_ENGINE_H

