//----------------------------------------------------------------------------
/** @file SgListUtilityTest.cpp
    Unit tests for SgListUtility.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgListUtility.h"

//----------------------------------------------------------------------------
namespace {
//----------------------------------------------------------------------------

void AddToList(int from, int to, SgList<int>& list)
{
    for (int i = from; i <= to; ++i)
        list.Append(i);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestIntersection)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgList<int> b;
    AddToList(8,12,b);
    SgListUtility::Intersection(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a[1], 8);
    BOOST_CHECK_EQUAL(a[2], 9);
    BOOST_CHECK_EQUAL(a[3], 10);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestDifference)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgList<int> b;
    AddToList(8,12,b);
    SgListUtility::Difference(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a[1], 5);
    BOOST_CHECK_EQUAL(a[2], 6);
    BOOST_CHECK_EQUAL(a[3], 7);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestReverse)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgListUtility::Reverse(&a);
    BOOST_CHECK_EQUAL(a.Length(), 6);
    BOOST_CHECK_EQUAL(a[1], 10);
    BOOST_CHECK_EQUAL(a[2], 9);
    BOOST_CHECK_EQUAL(a[3], 8);
    BOOST_CHECK_EQUAL(a[4], 7);
    BOOST_CHECK_EQUAL(a[5], 6);
    BOOST_CHECK_EQUAL(a[6], 5);
}

//----------------------------------------------------------------------------
} // namespace
//----------------------------------------------------------------------------

