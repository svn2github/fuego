//----------------------------------------------------------------------------
/** @file GoOpeningKnowledgeTest.cpp
 Unit tests for GoOpeningKnowledge. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <algorithm>
#include "GoOpeningKnowledge.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"

using SgPointUtil::Pt;
using namespace GoOpeningKnowledge;

//----------------------------------------------------------------------------

namespace {

    //----------------------------------------------------------------------------

    BOOST_AUTO_TEST_CASE(GoOpeningKnowledgeTest_CornerMoves)
    {
        GoBoard bd(19);

        std::vector<SgPoint> moves(FindCornerMoves(bd));

        BOOST_CHECK_EQUAL(moves.size(), 4*8u); // 8 moves each corner
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(3, 3)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(3, 4)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(3, 5)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(4, 3)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(4, 4)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(4, 5)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(5, 3)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(5, 4)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(5, 5)) ==
                    moves.end());

        // sample other corners
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(17, 3)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(3, 17)) !=
                    moves.end());
        BOOST_CHECK(std::find(moves.begin(), moves.end(), Pt(17, 17)) !=
                    moves.end());

        // check that there are no duplicates
        std::sort(moves.begin(), moves.end());
        const std::vector<SgPoint>::iterator last =
                    std::unique(moves.begin(), moves.end());
        BOOST_CHECK(moves.end() == last);
    }

    BOOST_AUTO_TEST_CASE(GoOpeningKnowledgeTest_SideExtensions)
    {
        GoBoard bd(19);
        std::vector<MoveBonusPair> moves(FindSideExtensions(bd));

        BOOST_CHECK_EQUAL(moves.size(), 13*4u); // 4 sides, no corners
    }
    
    //----------------------------------------------------------------------------
    
} // namespace

