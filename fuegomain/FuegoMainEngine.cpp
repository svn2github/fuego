//----------------------------------------------------------------------------
/** @file FuegoMainEngine.cpp
    See FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoMainEngine.h"

#include "FuegoMainUtil.h"
#include "GoGtpCommandUtil.h"
#include "GoUctPlayer.h"

using namespace std;

//----------------------------------------------------------------------------

FuegoMainEngine::FuegoMainEngine(istream& in, ostream& out,
                                 int fixedBoardSize, const char* programPath,
                                 bool noHandicap)
    : GoGtpEngine(in, out, fixedBoardSize, programPath, false, noHandicap),
      m_uctCommands(Board(), m_player),
      m_safetyCommands(Board())
{
    m_uctCommands.Register(*this);
    m_safetyCommands.Register(*this);
    SetPlayer(new GoUctPlayer(Board()));
}

FuegoMainEngine::~FuegoMainEngine()
{
}

void FuegoMainEngine::CmdAnalyzeCommands(GtpCommand& cmd)
{
    GoGtpEngine::CmdAnalyzeCommands(cmd);
    m_uctCommands.AddGoGuiAnalyzeCommands(cmd);
    m_safetyCommands.AddGoGuiAnalyzeCommands(cmd);
    string response = cmd.Response();
    cmd.SetResponse(GoGtpCommandUtil::SortResponseAnalyzeCommands(response));
}

void FuegoMainEngine::CmdName(GtpCommand& cmd)
{
    cmd << "Fuego";
}

/** Return Fuego version.
    @see FuegoMainUtil::Version()
*/
void FuegoMainEngine::CmdVersion(GtpCommand& cmd)
{
    cmd << FuegoMainUtil::Version();
}

//----------------------------------------------------------------------------
