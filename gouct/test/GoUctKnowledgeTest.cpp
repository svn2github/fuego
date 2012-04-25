//----------------------------------------------------------------------------
/** @file GoUctKnowledgeTest.cpp
    Unit tests for GoUctKnowledge. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "GoUctKnowledge.h"
#include "SgPoint.h"
#include "SgUctSearch.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {

class GoUctKnowledgeTester : public GoUctKnowledge
{
public:
    GoUctKnowledgeTester(const GoBoard& bd);

    /** Compute knowledge for the current position */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
};

GoUctKnowledgeTester::GoUctKnowledgeTester(const GoBoard& bd)
	: GoUctKnowledge(bd)
{ }

void GoUctKnowledgeTester::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    Add(Pt(3, 1), SgUctValue(0.5), SgUctValue(10));
    Add(Pt(3, 2), SgUctValue(0.1), SgUctValue(20));
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

/** Test GoUctUtil::DoSelfAtariCorrection (self-atari) */
BOOST_AUTO_TEST_CASE(GoUctKnowledgeTest_ProcessPosition)
{
    GoBoard bd(9);
    GoUctKnowledgeTester tester(bd);
    std::vector<SgUctMoveInfo> moves;
    Init(moves, bd);
    tester.TransferValues(moves);
    CheckUndefined(moves, Pt(3, 1));
    CheckUndefined(moves, Pt(3, 2));
    CheckUndefined(moves, Pt(5, 5));
    tester.ProcessPosition(moves);
    CheckValue(moves, Pt(3, 1), SgUctValue(0.5), SgUctValue(10));
    CheckValue(moves, Pt(3, 2), SgUctValue(0.1), SgUctValue(20));
    CheckUndefined(moves, Pt(5, 5));
}

//----------------------------------------------------------------------------

} // namespace
