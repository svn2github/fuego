//----------------------------------------------------------------------------
/** @file FuegoMainEngine.cpp
    See FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoMainEngine.h"

#include "GoGtpCommandUtil.h"
#include "GoUctGlobalSearchPlayer.h"

using namespace std;

//----------------------------------------------------------------------------

FuegoMainEngine::FuegoMainEngine(istream& in, ostream& out,
                                 const char* programPath)
    : GoGtpEngine(in, out, programPath),
      m_uctCommands(Board(), m_player)
{
    m_uctCommands.Register(*this);
    SetPlayer(new GoUctGlobalSearchPlayer(Board()));
}

FuegoMainEngine::~FuegoMainEngine()
{
}

void FuegoMainEngine::CmdAnalyzeCommands(GtpCommand& cmd)
{
    GoGtpEngine::CmdAnalyzeCommands(cmd);
    //ASSERT(ResponseEmptyOrEndsWithNewLine(cmd));
    m_uctCommands.AddGoGuiAnalyzeCommands(cmd);
    //ASSERT(ResponseEmptyOrEndsWithNewLine(cmd));
    string response = cmd.Response();
    cmd.SetResponse(GoGtpCommandUtil::SortResponseAnalyzeCommands(response));
}

//----------------------------------------------------------------------------
