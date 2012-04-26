//----------------------------------------------------------------------------
/** @file GoUctLadderKnowledgeTest.cpp
    Unit tests for GoUctLadderKnowledge. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "GoUctLadderKnowledge.h"
#include "SgPoint.h"
#include "SgUctSearch.h"

using SgPointUtil::Pt;
using namespace GoUctLadderKnowledgeParameters;

//----------------------------------------------------------------------------

namespace {

class GoUctLadderKnowledgeTester : public GoUctKnowledge
{
public:
    GoUctLadderKnowledgeTester(const GoBoard& bd);

    /** Compute knowledge for the current position */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
};

GoUctLadderKnowledgeTester::GoUctLadderKnowledgeTester(const GoBoard& bd)
	: GoUctKnowledge(bd)
{ }

void GoUctLadderKnowledgeTester::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    GoUctLadderKnowledge ladderKnowledge(Board(), *this);
    ladderKnowledge.ProcessPosition();
    TransferValues(moves);
}

void Init(std::vector<SgUctMoveInfo>& moves, const GoBoard& bd)
{
    for (GoBoard::Iterator it(bd); it; ++it)
        moves.push_back(SgUctMoveInfo(*it));
}

void CheckValue(const SgUctMoveInfo& moveInfo, SgPoint p,
                SgUctValue value, SgUctValue count)
{
    BOOST_CHECK_EQUAL(moveInfo.m_move, p);
    BOOST_CHECK_EQUAL(moveInfo.m_value, SgUctSearch::InverseEstimate(value));
    BOOST_CHECK_EQUAL(moveInfo.m_count, count);
    BOOST_CHECK_EQUAL(moveInfo.m_raveValue, value);
    BOOST_CHECK_EQUAL(moveInfo.m_raveCount, count);
}

void CheckUndefined(const SgUctMoveInfo& moveInfo, SgPoint p)
{
    BOOST_CHECK_EQUAL(moveInfo.m_move, p);
    BOOST_CHECK_EQUAL(moveInfo.m_value, SgUctValue(0));
    BOOST_CHECK_EQUAL(moveInfo.m_count, SgUctValue(0));
    BOOST_CHECK_EQUAL(moveInfo.m_raveValue, SgUctValue(0));
    BOOST_CHECK_EQUAL(moveInfo.m_raveCount, SgUctValue(0));
}

const SgUctMoveInfo* FindMoveInfo(const std::vector<SgUctMoveInfo>& moves, 
                            SgPoint p)
{
    /* @todo write it using a find template.
    std::vector<SgUctMoveInfo>::iterator it = find(moves, p);
    BOOST_REQUIRE_NE(it, moves.end());
	CheckValue(*it, p, value, count);
    */
    std::vector<SgUctMoveInfo>::const_iterator it = moves.begin();
    for (; it != moves.end(); ++it)
        if (it->m_move == p)
            return &(*it);
    BOOST_ERROR("move not found");
    return 0;
}

void CheckValue(const std::vector<SgUctMoveInfo>& moves, SgPoint p,
                SgUctValue value, SgUctValue count)
{
    const SgUctMoveInfo* m = FindMoveInfo(moves, p);
    if (m)
    	CheckValue(*m, p, value, count);
}

void CheckUndefined(const std::vector<SgUctMoveInfo>& moves, SgPoint p)
{
    const SgUctMoveInfo* m = FindMoveInfo(moves, p);
    if (m)
		CheckUndefined(*m, p);
}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoUctLadderKnowledgeTest_GoodLadderEscapeBonus)
{
    std::string s(".........\n"
                  ".........\n"
                  "..X......\n"
                  ".XO......\n"
                  ".X.......\n"
                  ".........\n"
                  ".....O...\n"
                  ".........\n"
                  ".........");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(4,6)); // need last move to be set.

    GoUctLadderKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    Init(moves, bd);
    tester.ProcessPosition(moves);
    
    // ladder escape move
    CheckValue(moves, Pt(3,5), SgUctValue(1), 
               SgUctValue(GOOD_LADDER_ESCAPE_BONUS));
    CheckUndefined(moves, Pt(4,5));
}

BOOST_AUTO_TEST_CASE(GoUctLadderKnowledgeTest_BadLadderEscapePenalty)
{
    std::string s(".........\n"
                  ".........\n"
                  "..X......\n"
                  ".XO......\n"
                  ".X.......\n"
                  ".........\n"
                  ".........\n"
                  ".........\n"
                  ".........");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(4,6)); // need last move to be set.

    GoUctLadderKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    Init(moves, bd);
    tester.ProcessPosition(moves);
    
    // failed ladder escape move, bad.
    CheckValue(moves, Pt(3,5), SgUctValue(0), 
               SgUctValue(BAD_LADDER_ESCAPE_PENALTY));
    CheckUndefined(moves, Pt(4,5));
}

BOOST_AUTO_TEST_CASE(GoUctLadderKnowledgeTest_LadderCaptureBonus)
{
    std::string s(".........\n"
                  ".........\n"
                  "..X......\n"
                  ".XOX.....\n"
                  ".X.......\n"
                  ".........\n"
                  ".........\n"
                  ".........\n"
                  ".........");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(3,5)); // need last move to be set.

    GoUctLadderKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    Init(moves, bd);
    tester.ProcessPosition(moves);
    
    // ladder escape move
    CheckValue(moves, Pt(3,4), SgUctValue(1), 
               SgUctValue(LADDER_CAPTURE_BONUS));
    CheckUndefined(moves, Pt(4,5));
}

BOOST_AUTO_TEST_CASE(GoUctLadderKnowledgeTest_Good2LibTacticsLadderBonus)
{
    std::string s(".........\n"
                  ".........\n"
                  ".........\n"
                  ".........\n"
                  ".........\n"
                  ".........\n"
                  ".........\n"
                  "...OX....\n"
                  "..OXX....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(5,3)); // need last move to be set.

    GoUctLadderKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    Init(moves, bd);
    tester.ProcessPosition(moves);
    
    // two lib, combined ladder escape/ opponent ladder capture move
    CheckValue(moves, Pt(3,2), SgUctValue(1), 
               SgUctValue(GOOD_2_LIB_TACTICS_LADDER_BONUS));
    // not for plain ladder escape by extending.
    CheckUndefined(moves, Pt(6,2));
}

//----------------------------------------------------------------------------

} // namespace
