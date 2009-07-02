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

void AddToVector(int from, int to, SgVector<int>& vector)
{
    for (int i = from; i <= to; ++i)
        vector.Append(i);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestIntersection)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgList<int> b;
    AddToList(8,12,b);
    SgListUtility::Intersection(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a.At(1), 8);
    BOOST_CHECK_EQUAL(a.At(2), 9);
    BOOST_CHECK_EQUAL(a.At(3), 10);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestDifference)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgList<int> b;
    AddToList(8,12,b);
    SgListUtility::Difference(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a.At(1), 5);
    BOOST_CHECK_EQUAL(a.At(2), 6);
    BOOST_CHECK_EQUAL(a.At(3), 7);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestReverse)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgListUtility::Reverse(&a);
    BOOST_CHECK_EQUAL(a.Length(), 6);
    BOOST_CHECK_EQUAL(a.At(1), 10);
    BOOST_CHECK_EQUAL(a.At(2), 9);
    BOOST_CHECK_EQUAL(a.At(3), 8);
    BOOST_CHECK_EQUAL(a.At(4), 7);
    BOOST_CHECK_EQUAL(a.At(5), 6);
    BOOST_CHECK_EQUAL(a.At(6), 5);
}

BOOST_AUTO_TEST_CASE(SgListUtilityTestVectorConversion)
{
    SgList<int> a;
    AddToList(5,10,a);
    SgVector<int> b;
    SgListUtility::ListToVector(a, b);
    BOOST_CHECK_EQUAL(b.Length(), 6);
    BOOST_CHECK_EQUAL(b[0], 5);
    BOOST_CHECK_EQUAL(b[1], 6);
    BOOST_CHECK_EQUAL(b[2], 7);
    BOOST_CHECK_EQUAL(b[3], 8);
    BOOST_CHECK_EQUAL(b[4], 9);
    BOOST_CHECK_EQUAL(b[5], 10);
    
    b.Clear();
    BOOST_CHECK_EQUAL(b.Length(), 0);
    b = SgListUtility::ListToVector(a);
    BOOST_CHECK_EQUAL(b.Length(), 6);
    BOOST_CHECK_EQUAL(b[0], 5);
    BOOST_CHECK_EQUAL(b[1], 6);
    BOOST_CHECK_EQUAL(b[2], 7);
    BOOST_CHECK_EQUAL(b[3], 8);
    BOOST_CHECK_EQUAL(b[4], 9);
    BOOST_CHECK_EQUAL(b[5], 10);

    SgList<int> c;
    SgListUtility::VectorToList(b, c);
    BOOST_CHECK(a == c);
    c = SgListUtility::VectorToList(b);
    BOOST_CHECK(a == c);
}

BOOST_AUTO_TEST_CASE(SgVectorUtilityTestDifference)
{
    SgVector<int> a;
    a.PushBack(5);
    a.PushBack(6);
    a.PushBack(7);
    a.PushBack(8);
    a.PushBack(-56);
    a.PushBack(9);
    a.PushBack(10);
    SgVector<int> b;
    b.PushBack(8);
    b.PushBack(-56);
    b.PushBack(9);
    b.PushBack(10);
    b.PushBack(11);
    b.PushBack(12);
    SgVectorUtility::Difference(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a[0], 5);
    BOOST_CHECK_EQUAL(a[1], 6);
    BOOST_CHECK_EQUAL(a[2], 7);
}

BOOST_AUTO_TEST_CASE(SgVectorUtilityTestIntersection)
{
    SgVector<int> a;
    AddToVector(5,10,a);
    SgVector<int> b;
    AddToVector(8,12,b);
    SgVectorUtility::Intersection(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a[0], 8);
    BOOST_CHECK_EQUAL(a[1], 9);
    BOOST_CHECK_EQUAL(a[2], 10);
}


//----------------------------------------------------------------------------
} // namespace
//----------------------------------------------------------------------------

