//----------------------------------------------------------------------------
/** @file GoGtpEngineTest.cpp
    Unit tests for GoGtpEngine. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include "GoGtpEngine.h"
#include "GoNodeUtil.h"
#include <boost/test/auto_unit_test.hpp>

using namespace std;

//----------------------------------------------------------------------------

namespace {

// nullStream to avoid output of ExecuteCommand() to cerr during a unit test
ofstream nullStream;

void Execute(GoGtpEngine& engine, const string& cmd)
{
    engine.ExecuteCommand(cmd, nullStream);
}

} // namespace

//----------------------------------------------------------------------------

namespace {

/** Test for bug that failed to set the komi in the game tree after a
    @c clear_board.
    See also http://sourceforge.net/apps/trac/fuego/ticket/41 */
BOOST_AUTO_TEST_CASE(GoGtpEngineTest_CmdClearBoard_KomiInGameAfterClearBoard)
{
    GoGtpEngine engine;
    Execute(engine, "komi 1");
    Execute(engine, "clear_board");
    const SgNode& root = engine.Game().Root();
    BOOST_CHECK_EQUAL(GoKomi(1), GoNodeUtil::GetKomi(&root));
}

BOOST_AUTO_TEST_CASE(GoGtpEngineTest_CmdKomi)
{
    GoGtpEngine engine;
    const SgNode& root = engine.Game().Root();
    Execute(engine, "komi 1");
    BOOST_CHECK_EQUAL(GoKomi(1), GoNodeUtil::GetKomi(&root));
    Execute(engine, "play b a1");
    Execute(engine, "komi 2");
    BOOST_CHECK_EQUAL(GoKomi(2), GoNodeUtil::GetKomi(&root));
    Execute(engine, "clear_board");
    BOOST_CHECK_EQUAL(GoKomi(2), GoNodeUtil::GetKomi(&root));
}

} // namespace

//----------------------------------------------------------------------------
