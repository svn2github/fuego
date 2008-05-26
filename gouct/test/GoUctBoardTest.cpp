//----------------------------------------------------------------------------
/** @file GoUctBoardTest.cpp
    Unit tests for GoUctBoard.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoUctBoard.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Copied from GoBoardTest_IsLibertyOfBlock */
BOOST_AUTO_TEST_CASE(GoUctBoardTest_IsLibertyOfBlock)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddBlack(Pt(2, 2));
    GoBoard board(19, setup);
    GoUctBoard bd(board);
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(1, 1), bd.Anchor(Pt(1, 2))));
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(1, 1), bd.Anchor(Pt(2, 1))));
    BOOST_CHECK(! bd.IsLibertyOfBlock(Pt(1, 1), bd.Anchor(Pt(2, 2))));
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(3, 2), bd.Anchor(Pt(2, 2))));
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(2, 3), bd.Anchor(Pt(2, 2))));
    BOOST_CHECK(! bd.IsLibertyOfBlock(Pt(2, 3), bd.Anchor(Pt(1, 2))));
}

//----------------------------------------------------------------------------

} // namespace

