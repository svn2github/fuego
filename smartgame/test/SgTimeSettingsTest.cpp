//----------------------------------------------------------------------------
/** @file SgTimeSettingsTest.cpp
    Unit tests for SgTimeSettings. */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgTimeSettings.h"

#include <boost/test/auto_unit_test.hpp>

//----------------------------------------------------------------------------

namespace {

const double EPSILON = 1e-6;
    
BOOST_AUTO_TEST_CASE(SgTimeSettingsTest_DefaultConstructor)
{
    SgTimeSettings timeSettings;
    BOOST_CHECK(timeSettings.IsUnknown());
}

BOOST_AUTO_TEST_CASE(SgTimeSettingsTest_MainTime)
{
    const double mainTime = 4.5;
    SgTimeSettings t(mainTime);
    BOOST_CHECK_CLOSE(t.MainTime(), mainTime, EPSILON);
    BOOST_CHECK_CLOSE(t.Overtime(), 0, EPSILON);
    BOOST_CHECK_EQUAL(t.OvertimeMoves(), 0);
    BOOST_CHECK(! t.IsUnknown());
}

BOOST_AUTO_TEST_CASE(SgTimeSettingsTest_OverTime)
{
    const double mainTime = 4.5;
    const double overtime = 30.4;
    const int overtimeMoves = 17;
    SgTimeSettings t(mainTime, overtime, overtimeMoves);
    BOOST_CHECK_CLOSE(t.MainTime(), mainTime, EPSILON);
    BOOST_CHECK_CLOSE(t.Overtime(), overtime, EPSILON);
    BOOST_CHECK_EQUAL(t.OvertimeMoves(), overtimeMoves);
    BOOST_CHECK(! t.IsUnknown());
}

} // namespace

//----------------------------------------------------------------------------
