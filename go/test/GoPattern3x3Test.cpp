//----------------------------------------------------------------------------
/** @file GoPattern3x3Test.cpp
 Unit tests for GoPattern3x3. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoPattern3x3.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"

using SgPointUtil::Pt;
using namespace GoPattern3x3;

//----------------------------------------------------------------------------

namespace {
    
//----------------------------------------------------------------------------

/** Test 3x3 pattern. */
BOOST_AUTO_TEST_CASE(GoPattern3x3Test_EBWCodeOfPoint)
{
    std::string s(
                  "O . X X X\n"
                  "O O O X X\n"
                  "X X O O .\n"
                  ". O O O O\n"
                  ". . . . .\n"
                  );
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(EBWCodeOfPoint(bd, Pt(1, 1)), SG_EMPTY);
    BOOST_CHECK_EQUAL(EBWCodeOfPoint(bd, Pt(2, 2)), SG_WHITE);
    BOOST_CHECK_EQUAL(EBWCodeOfPoint(bd, Pt(5, 5)), SG_BLACK);
}

//----------------------------------------------------------------------------

} // namespace

