//----------------------------------------------------------------------------
/** @file Go3x3PatternTest.cpp
 Unit tests for Go3x3Pattern. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "Go3x3Pattern.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"

using SgPointUtil::Pt;
using namespace Go3x3Pattern;

//----------------------------------------------------------------------------

namespace {
    
//----------------------------------------------------------------------------

/** Test 3x3 pattern. */
BOOST_AUTO_TEST_CASE(Go3x3PatternTest_EBWCodeOfPoint)
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

