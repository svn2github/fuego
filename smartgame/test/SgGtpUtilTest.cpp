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

    BOOST_CHECK_EQUAL(SgRGB(1,2,3) + SgRGB(10,20,30), SgRGB(11,22,33));

    std::string s = rgb.ToString();
    BOOST_CHECK_EQUAL(s, "#abcdef");

    {
        std::ostringstream buffer;
        buffer << rgb;
        BOOST_CHECK_EQUAL(buffer.str(), "#abcdef");
    }
    {
        std::ostringstream buffer;
        buffer << SgRGB(0,0,0);
        BOOST_CHECK_EQUAL(buffer.str(), "#000000");
    }
    {
        std::ostringstream buffer;
        buffer << SgRGB(0xff,0xff,0xff);
        BOOST_CHECK_EQUAL(buffer.str(), "#ffffff");
    }
}

BOOST_AUTO_TEST_CASE(SgGtpUtilTest_SgColorGradient)
{
    SgRGB rgb1(0x0, 0xff, 0x0);
    SgRGB rgb2(0xff, 0x0, 0xff);
    SgColorGradient cg(rgb1, -1.0, rgb2, 1.0);
    BOOST_CHECK_EQUAL(cg.ColorOf(0.0), SgRGB(0x7f, 0x7f, 0x7f));
    BOOST_CHECK_EQUAL(cg.ColorOf(-0.5), SgRGB(0x3f, 0xbf, 0x3f));
    BOOST_CHECK_EQUAL(cg.ColorOf(0.5), SgRGB(0xbf, 0x3f, 0xbf));
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

