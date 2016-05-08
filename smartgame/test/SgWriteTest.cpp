//----------------------------------------------------------------------------
/** @file SgWriteTest.cpp
    Unit tests for classes in SgWrite. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include "SgAutoUnitTest.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgWriteLabelOperator)
{
    {
        std::ostringstream buffer;
        buffer << SgWriteLabel("");
        BOOST_CHECK_EQUAL(buffer.str(), "               ");
    }
    {
        std::ostringstream buffer;
        buffer << SgWriteLabel("abcde");
        BOOST_CHECK_EQUAL(buffer.str(), "abcde          ");
    }
    {
        std::ostringstream buffer;
        buffer << SgWriteLabel("longlonglonglonglong");
        BOOST_CHECK_EQUAL(buffer.str(), "longlonglonglonglong ");
    }
}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgWritePointListOperator)
{
// @todo test cases with non-empty label, with writeSize = true,
// with multi-line output

    std::vector<SgPoint> list;
    {
        std::ostringstream buffer;
        buffer << SgWritePointList(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "\n");
    }

    list.push_back(Pt(1,1));
    {
        std::ostringstream buffer;
        buffer << SgWritePointList(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "A1 \n");
    }

    list.push_back(Pt(1,2));
    {
        std::ostringstream buffer;
        buffer << SgWritePointList(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "A1 A2 \n");
    }

    list.push_back(Pt(3,1));
    {
        std::ostringstream buffer;
        buffer << SgWritePointList(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "A1 A2 C1 \n");
    }

}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgWriteSPointListOperator)
{
    SgArrayList<SgPoint, 10> list;
    {
        std::ostringstream buffer;
        buffer << SgWriteSPointList<10>(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "\n");
    }

    list.PushBack(Pt(1,1));
    {
        std::ostringstream buffer;
        buffer << SgWriteSPointList<10>(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "A1 \n");
    }

    list.PushBack(Pt(1,2));
    {
        std::ostringstream buffer;
        buffer << SgWriteSPointList<10>(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "A1 A2 \n");
    }

    list.PushBack(Pt(3,1));
    {
        std::ostringstream buffer;
        buffer << SgWriteSPointList<10>(list, "", false);
        BOOST_CHECK_EQUAL(buffer.str(), "A1 A2 C1 \n");
    }

}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgWriteLineOperator)
{
    std::ostringstream buffer;
    buffer << SgWriteLine();
    BOOST_CHECK_EQUAL(buffer.str(), "----------"
                                    "----------"
                                    "----------"
                                    "----------"
                                    "----------"
                                    "----------"
                                    "----------"
                                    "--------\n"); // 78 chars
}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgWriteBooleanOperator)
{
    {
        std::ostringstream buffer;
        buffer << SgWriteBoolean(false);
        BOOST_CHECK_EQUAL(buffer.str(), "false");
    }
    {
        std::ostringstream buffer;
        buffer << SgWriteBoolean(true);
        BOOST_CHECK_EQUAL(buffer.str(), "true");
    }
}
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgWriteBoolAsIntWrite)
{
    {
        SgWriteBoolAsInt b0(false);
        std::ostringstream buffer;
        b0.Write(buffer);
        BOOST_CHECK_EQUAL(buffer.str(), "0");
    }
    {
        SgWriteBoolAsInt b1(true);
        std::ostringstream buffer;
        b1.Write(buffer);
        BOOST_CHECK_EQUAL(buffer.str(), "1");
    }
}

BOOST_AUTO_TEST_CASE(SgWriteBoolAsIntOperator)
{
    {
        std::ostringstream buffer;
        buffer << SgWriteBoolAsInt(false);
        BOOST_CHECK_EQUAL(buffer.str(), "0");
    }
    {
        std::ostringstream buffer;
        buffer << SgWriteBoolAsInt(true);
        BOOST_CHECK_EQUAL(buffer.str(), "1");
    }
}
//----------------------------------------------------------------------------

} // namespace

