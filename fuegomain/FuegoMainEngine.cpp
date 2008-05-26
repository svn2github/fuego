//----------------------------------------------------------------------------
/** @file FuegoMainEngine.cpp
    See FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoMainEngine.h"

#include <boost/preprocessor/stringize.hpp>
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

void FuegoMainEngine::CmdName(GtpCommand& cmd)
{
    cmd << "Fuego";
}

/** Return Fuego version.
    If the macro VERSION was defined by the build system during the compile
    time, its value will be used as the version, otherwise the version
    is "(__DATE__)"
*/
void FuegoMainEngine::CmdVersion(GtpCommand& cmd)
{
#ifdef VERSION
    cmd << BOOST_PP_STRINGIZE(VERSION);
#else
    cmd << "(" __DATE__ ")";
#endif
}

//----------------------------------------------------------------------------
