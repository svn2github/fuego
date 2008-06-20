//----------------------------------------------------------------------------
/** @file FuegoTestEngine.cpp
    See FuegoTestEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoTestEngine.h"

#include <boost/preprocessor/stringize.hpp>
#include "GoGtpCommandUtil.h"
#include "GoGtpExtraCommands.h"

using namespace std;

//----------------------------------------------------------------------------

FuegoTestEngine::FuegoTestEngine(istream& in, ostream& out,
                                 int initialBoardSize,
                                 const char* programPath)
    : GoGtpEngine(in, out, initialBoardSize, programPath, true),
      m_extraCommands(Board())
{
    m_extraCommands.Register(*this);
}

FuegoTestEngine::~FuegoTestEngine()
{
}

void FuegoTestEngine::CmdAnalyzeCommands(GtpCommand& cmd)
{
    GoGtpEngine::CmdAnalyzeCommands(cmd);
    m_extraCommands.AddGoGuiAnalyzeCommands(cmd);
    string response = cmd.Response();
    cmd.SetResponse(GoGtpCommandUtil::SortResponseAnalyzeCommands(response));
}

void FuegoTestEngine::CmdName(GtpCommand& cmd)
{
    cmd << "FuegoTest";
}

void FuegoTestEngine::CmdVersion(GtpCommand& cmd)
{
#ifdef VERSION
    cmd << BOOST_PP_STRINGIZE(VERSION);
#else
    cmd << "(" __DATE__ ")";
#endif
#ifdef _DEBUG
    cmd << " (dbg)";
#endif
}

//----------------------------------------------------------------------------
