//----------------------------------------------------------------------------
/** @file GoLadderTest.cpp
    Unit tests for GoLadder. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoLadder.h"
#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Immediate capture. */
BOOST_AUTO_TEST_CASE(GoLadderTest_Captured_1)
{
    std::string s("......\n"
                  ".XOX..\n"
                  "..X...\n"
                  "......\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    SgVector<SgPoint> sequence1;
    bool result1 = GoLadderUtil::Ladder(bd, Pt(3, 5), SG_BLACK, 
                                        false, &sequence1);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result1, true);
    BOOST_CHECK_EQUAL(sequence1.Length(), 1);
    BOOST_CHECK_EQUAL(sequence1[0], Pt(3, 6));

    SgVector<SgPoint> sequence2;
    bool result2 = GoLadderUtil::Ladder(bd, Pt(3, 5), SG_WHITE, 
                                        false, &sequence2);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result2, true); // true means it is captured.

    GoLadderStatus status = GoLadderUtil::LadderStatus(bd, Pt(3, 5),
                                                        false, 0, 0);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(status, GO_LADDER_CAPTURED);
}


BOOST_AUTO_TEST_CASE(GoLadderTest_Unsettled_1)
{
    std::string s("......\n"
                  ".XO...\n"
                  "..X...\n"
                  "......\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    SgVector<SgPoint> sequence1;
    bool result1 = GoLadderUtil::Ladder(bd, Pt(3, 5), SG_BLACK, 
                                        false, &sequence1);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result1, true);
    BOOST_CHECK_EQUAL(sequence1.Length(), 5);
    BOOST_CHECK_EQUAL(sequence1[0], Pt(4, 5));

    SgVector<SgPoint> sequence2;
    bool result2 = GoLadderUtil::Ladder(bd, Pt(3, 5), SG_WHITE, 
                                        false, &sequence2);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result2, false); // true means it is captured.
    BOOST_CHECK_EQUAL(sequence2.Length(), 1);
    BOOST_CHECK(sequence2[0] == Pt(4, 5) || sequence2[0] == Pt(3, 6));

    GoLadderStatus status = GoLadderUtil::LadderStatus(bd, Pt(3, 5),
                                                        false, 0, 0);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(status, GO_LADDER_UNSETTLED);
}


BOOST_AUTO_TEST_CASE(GoLadderTest_Escaped_1)
{
    std::string s(".OX...\n"
                  ".XOX..\n"
                  "..OX..\n"
                  "......\n"
                  ".O....\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    SgVector<SgPoint> sequence1;
    bool result1 = GoLadderUtil::Ladder(bd, Pt(3, 5), SG_BLACK, 
                                        false, &sequence1);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result1, false);

    SgVector<SgPoint> sequence2;
    bool result2 = GoLadderUtil::Ladder(bd, Pt(3, 5), SG_WHITE, 
                                        false, &sequence2);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result2, false); // true means it is captured.
    BOOST_CHECK_EQUAL(sequence2.Length(), 1);

    GoLadderStatus status = GoLadderUtil::LadderStatus(bd, Pt(3, 5),
                                                        false, 0, 0);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(status, GO_LADDER_ESCAPED);
}


BOOST_AUTO_TEST_CASE(GoLadderTest_TwoLib_1)
{
    std::string s("X.OX..\n"
                  ".OOX..\n"
                  "OOXX..\n"
                  "XXX...\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);

    SgVector<SgPoint> sequence1;
    bool result1 = GoLadderUtil::Ladder(bd, Pt(3, 6), SG_BLACK, 
                                        false, &sequence1);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result1, true);
    BOOST_CHECK_EQUAL(sequence1.Length(), 5);
    BOOST_CHECK(sequence1[0] == Pt(2, 6) || sequence1[0] == Pt(1, 5));

    SgVector<SgPoint> sequence2;
    bool result2 = GoLadderUtil::Ladder(bd, Pt(3, 6), SG_WHITE, 
                                        false, &sequence2);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result2, true); // true means it is captured.

    GoLadderStatus status = GoLadderUtil::LadderStatus(bd, Pt(3, 6),
                                                        false, 0, 0);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(status, GO_LADDER_CAPTURED);
}


BOOST_AUTO_TEST_CASE(GoLadderTest_SnapBack_1)
{
    std::string s(".........\n"
                  ".........\n"
                  "..XXX....\n"
                  "..O......\n"
                  "..OX.....\n"
                  "..XO.....\n"
                  "..XOX....\n"
                  "...X.....\n"
                  ".........");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);

    SgVector<SgPoint> sequence1;
    bool result1 = GoLadderUtil::Ladder(bd, Pt(4, 4), SG_BLACK, 
                                        false, &sequence1);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result1, true);
    BOOST_CHECK_EQUAL(sequence1.Length(), 1);
    BOOST_CHECK_EQUAL(sequence1[0], Pt(5, 4));

    SgVector<SgPoint> sequence2;
    bool result2 = GoLadderUtil::Ladder(bd, Pt(4, 4), SG_WHITE, 
                                        false, &sequence2);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(result2, true); // true means it is captured.
    BOOST_CHECK(sequence2.Length() >= 14);
    BOOST_CHECK_EQUAL(sequence2[0], Pt(5, 4));

    GoLadderStatus status = GoLadderUtil::LadderStatus(bd, Pt(4, 4),
                                                        false, 0, 0);
    bd.CheckConsistency();
    BOOST_CHECK_EQUAL(status, GO_LADDER_CAPTURED);
}

BOOST_AUTO_TEST_CASE(GoLadderTest_IsLadderCaptureMove_1)
{
    std::string s("......\n"
                  ".XO...\n"
                  "..X...\n"
                  "......\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    bool result1 = GoLadderUtil::IsLadderCaptureMove(bd, Pt(3, 5), Pt(4, 5));
    BOOST_CHECK_EQUAL(result1, true);
    bd.CheckConsistency();

    bool result2 = GoLadderUtil::IsLadderCaptureMove(bd, Pt(3, 5), Pt(3, 6));
    BOOST_CHECK_EQUAL(result2, false);
    bd.CheckConsistency();
}

BOOST_AUTO_TEST_CASE(GoLadderTest_IsLadderCaptureMove_2)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  ".XOOX.\n"
                  "..XX..\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    bool result1 = GoLadderUtil::IsLadderCaptureMove(bd, Pt(3, 3), Pt(3, 4));
    BOOST_CHECK_EQUAL(result1, true);
    bd.CheckConsistency();

    bool result2 = GoLadderUtil::IsLadderCaptureMove(bd, Pt(3, 3), Pt(4, 4));
    BOOST_CHECK_EQUAL(result2, true);
    bd.CheckConsistency();
}

BOOST_AUTO_TEST_CASE(GoLadderTest_IsLadderEscapeMove_1)
{
    std::string s("......\n"
                  ".XOX..\n"
                  "..X...\n"
                  "......\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    bool result = GoLadderUtil::IsLadderEscapeMove(bd, Pt(3, 5), Pt(3, 6));
    BOOST_CHECK_EQUAL(result, false);
    bd.CheckConsistency();
    
    SgVector<SgPoint> escapeMoves;
    GoLadderUtil::FindLadderEscapeMoves(bd, Pt(3, 5), escapeMoves);
    BOOST_CHECK(escapeMoves.IsEmpty());
}

BOOST_AUTO_TEST_CASE(GoLadderTest_IsLadderEscapeMove_2)
{
    std::string s("......\n"
                  "......\n"
                  ".XOX..\n"
                  "..X...\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    bool result = GoLadderUtil::IsLadderEscapeMove(bd, Pt(3, 4), Pt(3, 5));
    BOOST_CHECK_EQUAL(result, true);
    bd.CheckConsistency();
    
    result = GoLadderUtil::IsLadderEscapeMove(bd, Pt(3, 4), Pt(2, 3));
    BOOST_CHECK_EQUAL(result, false);
    bd.CheckConsistency();
    
    SgVector<SgPoint> escapeMoves;
    GoLadderUtil::FindLadderEscapeMoves(bd, Pt(3, 4), escapeMoves);
    BOOST_CHECK_EQUAL(escapeMoves.Length(), 1);
    BOOST_CHECK_EQUAL(escapeMoves[0], Pt(3, 5));
}

BOOST_AUTO_TEST_CASE(GoLadderTest_IsLadderEscapeMove_3)
{
    std::string s("..X...\n"
                  ".XO...\n"
                  ".XOX..\n"
                  ".OXO..\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    const GoBoard bd(boardSize, setup);

    bool result = GoLadderUtil::IsLadderEscapeMove(bd, Pt(3, 4), Pt(4, 5));
    BOOST_CHECK_EQUAL(result, true);
    bd.CheckConsistency();
    
    result = GoLadderUtil::IsLadderEscapeMove(bd, Pt(3, 4), Pt(3, 2));
    BOOST_CHECK_EQUAL(result, true);
    bd.CheckConsistency();
    
    SgVector<SgPoint> escapeMoves;
    GoLadderUtil::FindLadderEscapeMoves(bd, Pt(3, 4), escapeMoves);
    BOOST_CHECK_EQUAL(escapeMoves.Length(), 2);
    BOOST_CHECK(escapeMoves[0] == Pt(4, 5) || escapeMoves[0] == Pt(3, 2));
    BOOST_CHECK(escapeMoves[1] == Pt(4, 5) || escapeMoves[1] == Pt(3, 2));
    BOOST_CHECK(escapeMoves[0] != escapeMoves[1]);
}


//----------------------------------------------------------------------------

} // namespace

