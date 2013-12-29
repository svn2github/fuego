//----------------------------------------------------------------------------
/** @file SgGtpUtilTest.cpp
    Unit tests for GtpSgUtil. */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGtpUtil.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "GtpEngine.h"
#include "SgPoint.h"
#include "SgPointSet.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgGtpUtilTest_SgRGB)
{
    SgRGB rgb(0xab, 0xcd, 0xef);
    BOOST_CHECK_EQUAL(rgb.m_r, 0xab);
    BOOST_CHECK_EQUAL(rgb.m_g, 0xcd);
    BOOST_CHECK_EQUAL(rgb.m_b, 0xef);

    SgRGB rgb2 = 0.5 * rgb;
    BOOST_CHECK_EQUAL(rgb2.m_r, 0xab / 2);
    BOOST_CHECK_EQUAL(rgb2.m_g, 0xcd / 2);
    BOOST_CHECK_EQUAL(rgb2.m_b, 0xef / 2);

    std::string s = rgb.ToString();
    BOOST_CHECK_EQUAL(s, "#abcdef");

    std::ostringstream buffer;
    buffer << rgb;
    BOOST_CHECK_EQUAL(buffer.str(), "#abcdef");
}


BOOST_AUTO_TEST_CASE(SgGtpUtilTest_RespondPointSet)
{
    SgPointSet pointSet;
    pointSet.Include(Pt(1, 1));
    pointSet.Include(Pt(1, 2));
    pointSet.Include(Pt(2, 1));
    GtpCommand cmd;
    SgGtpUtil::RespondPointSet(cmd, pointSet);
    BOOST_CHECK_EQUAL(cmd.Response(), "A1 B1 A2");
}

} // namespace

//----------------------------------------------------------------------------

