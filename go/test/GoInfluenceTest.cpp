//----------------------------------------------------------------------------
/** @file GoInfluenceTest.cpp
 Unit tests for GoInfluence. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoInfluence.h"

#include "GoSetupUtil.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {
    
    //----------------------------------------------------------------------------
    
    BOOST_AUTO_TEST_CASE(GoInfluenceTest_Distance_Black)
    {
        std::string s(". . . .\n"
                      ". . . .\n"
                      ". . X .\n"
                      ". . . .\n");
        int boardSize;
        GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
        GoBoard bd(boardSize, setup);
        SgPointArray<int> distance;
        GoInfluence::FindDistanceToStones(bd, SG_BLACK, distance);
        BOOST_CHECK_EQUAL(distance[Pt(3, 2)], 0);
        BOOST_CHECK_EQUAL(distance[Pt(2, 2)], 1);
        BOOST_CHECK_EQUAL(distance[Pt(3, 1)], 1);
        BOOST_CHECK_EQUAL(distance[Pt(3, 3)], 1);
        BOOST_CHECK_EQUAL(distance[Pt(1, 3)], 3);
        BOOST_CHECK_EQUAL(distance[Pt(1, 4)], 4);
        BOOST_CHECK_EQUAL(distance[Pt(4, 3)], 2);
   }
    
    BOOST_AUTO_TEST_CASE(GoInfluenceTest_Distance_White)
    {
        std::string s(". . . .\n"
                      ". . . .\n"
                      ". . . .\n"
                      ". . . O\n");
        int boardSize;
        GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
        GoBoard bd(boardSize, setup);
        SgPointArray<int> distance;
        GoInfluence::FindDistanceToStones(bd, SG_WHITE, distance);
        BOOST_CHECK_EQUAL(distance[Pt(4, 1)], 0);
        BOOST_CHECK_EQUAL(distance[Pt(4, 2)], 1);
        BOOST_CHECK_EQUAL(distance[Pt(3, 1)], 1);
        BOOST_CHECK_EQUAL(distance[Pt(3, 2)], 2);
        BOOST_CHECK_EQUAL(distance[Pt(1, 3)], 5);
        BOOST_CHECK_EQUAL(distance[Pt(1, 4)], 6);
        BOOST_CHECK_EQUAL(distance[Pt(4, 3)], 2);
    }
    
    //----------------------------------------------------------------------------
    
} // namespace
