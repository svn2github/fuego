//----------------------------------------------------------------------------
/** @file GoEyeUtilTest.cpp
    Unit tests for GoEyeUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoEyeUtil.h"

using namespace std;
using namespace GoEyeUtil;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GoEyeUtilTest_IsSimpleEye_1)
{
    // . . .
    // X X .
    // . X .
    GoBoard bd(3);
    bd.Play(Pt(1, 2), SG_BLACK);
    BOOST_CHECK(! IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
    bd.Play(Pt(2, 1), SG_BLACK);
    BOOST_CHECK(! IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
    bd.Play(Pt(2, 2), SG_BLACK);
    BOOST_CHECK(IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
}

BOOST_AUTO_TEST_CASE(GoEyeUtilTest_IsSimpleEye_2)
{
    // . . .
    // X X .
    // . O .
    GoBoard bd(3);
    bd.Play(Pt(1, 2), SG_BLACK);
    BOOST_CHECK(! IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(! IsSimpleEye(bd, Pt(1, 1), SG_WHITE));
    bd.Play(Pt(2, 2), SG_BLACK);
    BOOST_CHECK(! IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
}

BOOST_AUTO_TEST_CASE(GoEyeUtilTest_IsSimpleEye_3)
{
    // . . . .
    // X X . .
    // X . X .
    // . X X .
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.AddBlack(Pt(1, 3));
    setup.AddBlack(Pt(2, 3));
    setup.AddBlack(Pt(2, 1));
    setup.AddBlack(Pt(3, 1));
    GoBoard bd(4, setup);
    BOOST_CHECK(! IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
    BOOST_CHECK(! IsSimpleEye(bd, Pt(2, 2), SG_BLACK));
    bd.Play(Pt(3, 2), SG_BLACK);
    BOOST_CHECK(IsSimpleEye(bd, Pt(1, 1), SG_BLACK));
    BOOST_CHECK(IsSimpleEye(bd, Pt(2, 2), SG_BLACK));
}

} // namespace

//----------------------------------------------------------------------------

