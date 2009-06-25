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

BOOST_AUTO_TEST_CASE(GoEyeUtilTest_IsNakadeShape)
{
    SgPointSet area;
    BOOST_CHECK(! IsNakadeShape(area));
    area.Include(Pt(5,5)); // single stone
    BOOST_CHECK(IsNakadeShape(area));
    area.Include(Pt(5,6)); // two in a row
    BOOST_CHECK(IsNakadeShape(area));
    area.Include(Pt(5,4)); // three in a row
    BOOST_CHECK(IsNakadeShape(area));
    area.Include(Pt(6,5)); // T-shape
    BOOST_CHECK(IsNakadeShape(area));
    area.Include(Pt(4,5)); // cross
    BOOST_CHECK(IsNakadeShape(area));
    area.Include(Pt(4,4)); // rabbity six
    BOOST_CHECK(IsNakadeShape(area));
    area.Exclude(Pt(6,5)); // bulky five
    BOOST_CHECK(IsNakadeShape(area));
    area.Exclude(Pt(5,6)); // bulky four, 2x2 block
    BOOST_CHECK(IsNakadeShape(area));
    
    area.Clear();
    area.Include(Pt(5,4));
    area.Include(Pt(5,5));
    area.Include(Pt(5,6));
    area.Include(Pt(5,7)); // 4 in a row
    BOOST_CHECK(! IsNakadeShape(area));
    area.Exclude(Pt(5,7));
    BOOST_CHECK(IsNakadeShape(area));
    area.Include(Pt(6,6)); // L-shape
    BOOST_CHECK(! IsNakadeShape(area));
    area.Include(Pt(4,6)); // U-shape
    BOOST_CHECK(! IsNakadeShape(area));
    area.Include(Pt(5,6)); // 2x3 block
    BOOST_CHECK(! IsNakadeShape(area));
}

} // namespace

//----------------------------------------------------------------------------

