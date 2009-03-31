//----------------------------------------------------------------------------
/** @file GoSetupUtilTest.cpp
    Unit tests for GoSetupUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "GoBoard.h"
#include "GoSetup.h"
#include "GoSetupUtil.h"


//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test that not the full remaining time is used, if close to the
    expected final number of stones on the board, but no overtime is used.
*/
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_EmptyBoard)
{
    std::istringstream in("");
    BOOST_REQUIRE(in);
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in);
    BOOST_CHECK(setup.m_stones.BothEmpty());
}

//----------------------------------------------------------------------------

} // namespace
