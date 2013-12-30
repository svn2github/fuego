//----------------------------------------------------------------------------
/** @file GoGtpCommandUtilTest.cpp
    Unit tests for GoGtpCommandUtil. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoGtpCommandUtil.h"
#include "GtpEngine.h"
#include "SgPoint.h"
#include "SgPointArray.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

SgPoint ParsePoint(const std::string& s)
{
    std::istringstream in(s);
    SgPoint p;
    in >> SgReadPoint(p);
    SG_ASSERT(in);
    return p;
}

} // namespace

//----------------------------------------------------------------------------

namespace {

/** Test that GoGtpCommandUtil::GetHandicapStones() returns stones according
    to the GTP standard. */
BOOST_AUTO_TEST_CASE(GoGtpCommandUtilTest_GetHandicapStones)
{
    // GTP locations are defined up to size 25, but SG_MAX_SIZE could be
    // smaller
    int maxSize = std::min(SG_MAX_SIZE, 25);
    for (int size = 1; size <= maxSize; ++size)
    {
        for (int n = 0; n < 20; ++n)
        {
            std::ostringstream buffer;
            buffer << "size=" << size << " n=" << n;
            std::string message = buffer.str();
            SgVector<SgPoint> stones;
            bool didThrow = false;
            try
            {
                stones = GoGtpCommandUtil::GetHandicapStones(size, n);
            }
            catch (const GtpFailure& e)
            {
                didThrow = true;
            }
            if (n == 0)
                BOOST_CHECK_MESSAGE(stones.IsEmpty(), message);
            else if (size <= 6 || n == 1 || n > 9)
                BOOST_CHECK_MESSAGE(didThrow, message);
            else if (size == 7 || size % 2 == 0)
            {
                if (n > 4)
                    BOOST_CHECK_MESSAGE(didThrow, message);
                else
                {
                    BOOST_CHECK_EQUAL(n, stones.Length());
                    BOOST_CHECK_MESSAGE(stones.Length() == n, message);
                }
            }
            else
            {
                if (n > 9)
                    BOOST_CHECK_MESSAGE(didThrow, message);
                else
                {
                    BOOST_CHECK_EQUAL(n, stones.Length());
                    BOOST_CHECK_MESSAGE(stones.Length() == n, message);
                }
            }
            if (size == 19 && n >= 2 && n <= 9)
            {
                BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("D4")),
                                    message);
                BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("Q16")),
                                    message);
                if (n >= 3)
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("D16")),
                                        message);
                if (n >= 4)
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("Q4")),
                                        message);
                if (n == 5 || n == 7 || n == 9)
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("K10")),
                                        message);
                if (n >= 6)
                {
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("D10")),
                                        message);
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("Q10")),
                                        message);
                }
                if (n >= 8)
                {
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("K4")),
                                        message);
                    BOOST_CHECK_MESSAGE(stones.Contains(ParsePoint("K16")),
                                        message);
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(GoGtpCommandUtilTest_RespondColorGradientData)
{
    GoBoard board(3);
    SgPointArray<float> data(0.0);
    data[Pt(1,1)] = 0.5;
    data[Pt(2,1)] = -0.5;
    data[Pt(2,2)] = -1.0;
    data[Pt(3,3)] = 1.0;
    float minValue = -1.0;
    float maxValue = 1.0;
    GtpCommand cmd;
    GoGtpCommandUtil::RespondColorGradientData(cmd, data, minValue,
                                               maxValue, board);
    BOOST_CHECK_EQUAL(cmd.Response(), "#007f7f #007f7f #0000ff\n"
                                      "#007f7f #00ff00 #007f7f\n"
                                      "#003fbf #00bf3f #007f7f\n");
}

} // namespace

//----------------------------------------------------------------------------

