//----------------------------------------------------------------------------
/** @file GoGtpExtraCommands.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoGtpExtraCommands.h"

#include "GoBoard.h"
#include "GoGtpCommandUtil.h"
#include "GoLadder.h"
#include "GoStaticLadder.h"

using namespace std;

using GoGtpCommandUtil::StoneArg;

//----------------------------------------------------------------------------

GoGtpExtraCommands::GoGtpExtraCommands(GoBoard& bd)
    : m_bd(bd)
{
}

void GoGtpExtraCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
        "string/Go Ladder/go_ladder %p\n"
        "string/Go Static Ladder/go_static_ladder %p\n";
}

/** Return fast ladder status.
    Arguments: prey point<br>
    Returns: escaped|captured|unsettled<br>
    @see FastLadderStatus
*/
void GoGtpExtraCommands::CmdLadder(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgPoint prey = StoneArg(cmd, 0, m_bd);
    GoLadder::Status status = GoLadder::LadderStatus(m_bd, prey);
    switch (status)
    {
    case GoLadder::Escaped:
        cmd << "escaped";
        break;
    case GoLadder::Captured:
        cmd << "captured";
        break;
    case GoLadder::Unsettled:
        cmd << "unsettled";
        break;
    default:
        throw GtpFailure() << "Unexpected ladder status: " << status;
    }
}

/** Return static ladder status.
    Arguments: prey point<br>
    Returns: escaped|captured|unsettled<br>
    @see GoStaticLadder
*/
void GoGtpExtraCommands::CmdStaticLadder(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgPoint p = StoneArg(cmd, 0, m_bd);
    SgBlackWhite c = m_bd.GetColor(p);
    if (GoStaticLadder::IsLadder(m_bd, p, c))
        cmd << "captured";
    else if (GoStaticLadder::IsLadder(m_bd, p, OppBW(c)))
        cmd << "unsettled";
    else
        cmd << "escaped";
}

void GoGtpExtraCommands::Register(GtpEngine& e)
{
    Register(e, "go_ladder", &GoGtpExtraCommands::CmdLadder);
    Register(e, "go_static_ladder", &GoGtpExtraCommands::CmdStaticLadder);
}

void GoGtpExtraCommands::Register(GtpEngine& engine,
                               const std::string& command,
                               GtpCallback<GoGtpExtraCommands>::Method method)
{
    engine.Register(command,
                    new GtpCallback<GoGtpExtraCommands>(this, method));
}

//----------------------------------------------------------------------------
