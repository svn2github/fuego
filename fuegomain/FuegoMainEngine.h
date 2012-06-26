//----------------------------------------------------------------------------
/** @file FuegoMainEngine.h */
//----------------------------------------------------------------------------

#ifndef FUEGOMAIN_ENGINE_H
#define FUEGOMAIN_ENGINE_H

#include "GoGtpEngine.h"
#include "GoSafetyCommands.h"
#include "GoUctCommands.h"
#include "GoUctBookBuilderCommands.h"

//----------------------------------------------------------------------------

/** GTP engine for GoUctPlayer. */
class FuegoMainEngine
    : public GoGtpEngine
{
public:
    FuegoMainEngine(int fixedBoardSize, const char* programPath = 0,
                    bool noHandicap = false);

    ~FuegoMainEngine();

    void CmdAnalyzeCommands(GtpCommand& cmd);
    void CmdLicense(GtpCommand& cmd);
    void CmdName(GtpCommand& cmd);
    void CmdVersion(GtpCommand& cmd);

private:
    GoUctCommands m_uctCommands;

    /** A GoUctPlayer player has two template parameters: its search class
    	and its search state class. The search class GoUctGlobalSearch
        used has two more template parameters: policy and policy factory.
        Here, they are GoUctPlayoutPolicy and GoUctPlayoutPolicyFactory
        The search state uses the same policy class GoUctPlayoutPolicy.
        Both GoUctPlayoutPolicy and GoUctPlayoutPolicyFactory take the
        Go board as a template argument - GoUctBoard here.
    */
    typedef GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                    GoUctPlayoutPolicyFactory<GoUctBoard> >,
                    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > >
    PlayerType;

    GoUctBookBuilderCommands<PlayerType> m_autoBookCommands;

    GoSafetyCommands m_safetyCommands;
};

//----------------------------------------------------------------------------

#endif // FUEGOMAIN_ENGINE_H

