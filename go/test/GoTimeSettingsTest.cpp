//----------------------------------------------------------------------------
/** @file GoTimeSettingsTest.cpp
    Unit tests for GoTimeSettings. */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoTimeSettings.h"

#include <boost/test/auto_unit_test.hpp>

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Test if no time limits is properly encoded as defined in GTP standard. */
BOOST_AUTO_TEST_CASE(GoTimeSettingsTest_DefaultConstructor)
{
    GoTimeSettings timeSettings;
    BOOST_CHECK(timeSettings.Overtime() > 0);
    BOOST_CHECK_EQUAL(timeSettings.OvertimeMoves(), 0);
}

BOOST_AUTO_TEST_CASE(GoTimeSettingsTest_NoTimeLimits)
{
    GoTimeSettings timeSettings;
    BOOST_CHECK(timeSettings.NoTimeLimits());
}

} // namespace

//----------------------------------------------------------------------------

