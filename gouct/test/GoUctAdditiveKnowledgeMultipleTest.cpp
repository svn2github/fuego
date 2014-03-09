//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeMultipleTest.cpp
    Unit tests for GoUctAdditiveKnowledgeMultiple. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "GoUctAdditiveKnowledgeMultiple.h"
#include "SgDebug.h"
#include "SgPoint.h"
#include "SgUctSearch.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

class Knowledge1 : public GoUctAdditiveKnowledgeStdProb
{
public:
    Knowledge1(const GoBoard& bd);
    
    /** Compute knowledge for the current position */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
};

Knowledge1::Knowledge1(const GoBoard& bd)
: GoUctAdditiveKnowledgeStdProb(bd)
{ }

void Knowledge1::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    SG_ASSERT(moves.size() > 2);
    moves[0].m_predictorValue = 5.0;
    moves[1].m_predictorValue = 2.0;
}
    
//----------------------------------------------------------------------------

class Knowledge2 : public GoUctAdditiveKnowledgeStdProb
{
public:
    Knowledge2(const GoBoard& bd);
    
    /** Compute knowledge for the current position */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
};

Knowledge2::Knowledge2(const GoBoard& bd)
: GoUctAdditiveKnowledgeStdProb(bd)
{ }

void Knowledge2::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    SG_ASSERT(moves.size() > 2);
    moves[1].m_predictorValue = 3.0;
    moves[2].m_predictorValue = 10.0;
}

//----------------------------------------------------------------------------

void Init(std::vector<SgUctMoveInfo>& moves, const GoBoard& bd)
{
    moves.clear();
    SG_ASSERT(moves.size() == 0);
    for (GoBoard::Iterator it(bd); it; ++it)
        moves.push_back(SgUctMoveInfo(*it));
}
    
void TestType(const GoBoard& bd,
              std::vector<SgUctMoveInfo>& moves,
              GoUctKnowledgeCombinationType combinationType,
              int nuKnowledge)
{
    const SgUctValue minimum = 0.0001f;
    GoUctAdditiveKnowledgeMultiple m(bd, minimum, combinationType);
    Knowledge1* k1 = new Knowledge1(bd);
    m.AddKnowledge(k1);
    if (nuKnowledge >1)
    {
        Knowledge2* k2 = new Knowledge2(bd);
        m.AddKnowledge(k2);
    }
    m.ProcessPosition(moves);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoUctAdditiveKnowledgeMultipleTest_ProcessPosition)
{
    GoBoard bd(9);
    std::vector<SgUctMoveInfo> moves;
    Init(moves, bd);
    TestType(bd, moves, COMBINE_MULTIPLY, 1);
    BOOST_CHECK_EQUAL(moves[0].m_predictorValue, 5.0);
    BOOST_CHECK_EQUAL(moves[1].m_predictorValue, 2.0);
    BOOST_CHECK_EQUAL(moves[2].m_predictorValue, 1.0);
    Init(moves, bd);
    TestType(bd, moves, COMBINE_MULTIPLY, 2);
    BOOST_CHECK_EQUAL(moves[0].m_predictorValue, 5.0);
    BOOST_CHECK_EQUAL(moves[1].m_predictorValue, 6.0);
    BOOST_CHECK_EQUAL(moves[2].m_predictorValue, 10.0);
    BOOST_CHECK_EQUAL(moves[3].m_predictorValue, 1.0);
    
    Init(moves, bd);
    TestType(bd, moves, COMBINE_GEOMETRIC_MEAN, 1);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 2.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 1.0, 1e-5f);
    Init(moves, bd);
    TestType(bd, moves, COMBINE_GEOMETRIC_MEAN, 2);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, sqrt(5.0),  1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, sqrt(6.0),  1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, sqrt(10.0), 1e-5f);
    BOOST_CHECK_CLOSE(moves[3].m_predictorValue, sqrt(1.0),  1e-5f);

    Init(moves, bd);
    TestType(bd, moves, COMBINE_ADD, 1);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 2.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 0.0, 1e-5f);
    Init(moves, bd);
    TestType(bd, moves, COMBINE_ADD, 2);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0,  1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 5.0,  1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 10.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[3].m_predictorValue, 0.0,  1e-5f);
    
    Init(moves, bd);
    TestType(bd, moves, COMBINE_AVERAGE, 1);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 2.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 0.0, 1e-5f);
    Init(moves, bd);
    TestType(bd, moves, COMBINE_AVERAGE, 2);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0/2,  1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 5.0/2,  1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 10.0/2, 1e-5f);
    BOOST_CHECK_CLOSE(moves[3].m_predictorValue, 0.0,  1e-5f);

    Init(moves, bd);
    TestType(bd, moves, COMBINE_MAX, 1);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 2.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 0.0, 1e-5f);
    Init(moves, bd);
    TestType(bd, moves, COMBINE_MAX, 2);
    BOOST_CHECK_CLOSE(moves[0].m_predictorValue, 5.0,  1e-5f);
    BOOST_CHECK_CLOSE(moves[1].m_predictorValue, 3.0,  1e-5f);
    BOOST_CHECK_CLOSE(moves[2].m_predictorValue, 10.0, 1e-5f);
    BOOST_CHECK_CLOSE(moves[3].m_predictorValue, 0.0,  1e-5f);

}


//----------------------------------------------------------------------------
    
} // namespace
