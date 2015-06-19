//----------------------------------------------------------------------------
/** @file GoEvalArrayTest.cpp
 Unit tests for GoEvalArray. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoEvalArray.h"

#include "GoBoard.h"
#include "SgPoint.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {
    
//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoEvalArrayTest_GoEvalArray)
{
    GoEvalArray<int> eval(0);
    BOOST_CHECK_EQUAL(eval[0], 0);
    BOOST_CHECK_EQUAL(eval[SG_PASS], 0);
    BOOST_CHECK_EQUAL(eval[Pt(5, 5)], 0);
    eval[SG_PASS] = 9;
    BOOST_CHECK_EQUAL(eval[SG_PASS], 9);
}

BOOST_AUTO_TEST_CASE(GoEvalArrayTest_GoEvalArray_2)
{
    GoEvalArray<int> eval(5);
    BOOST_CHECK_EQUAL(eval[0], 5);
    BOOST_CHECK_EQUAL(eval[SG_PASS], 5);
    BOOST_CHECK_EQUAL(eval[Pt(5, 5)], 5);
}

    //----------------------------------------------------------------------------
    
} // namespace

