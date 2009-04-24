//----------------------------------------------------------------------------
/** @file SgVectorTest.cpp
    Unit tests for SgVector.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgVector.h"

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgVectorTestConstructor)
{
    SgVector<int> a;
    BOOST_CHECK(a.IsEmpty());
    BOOST_CHECK(! a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 0);
}


BOOST_AUTO_TEST_CASE(SgVectorTestAppend)
{
    SgVector<int> a;
    a.Append(123);
    BOOST_CHECK_EQUAL(a.Length(), 1);
    BOOST_CHECK_EQUAL(a.Top(), 123);
    BOOST_CHECK_EQUAL(a.Tail(), 123);
    a.Append(456);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(a.Top(), 123);
    BOOST_CHECK_EQUAL(a.Tail(), 456);
}

BOOST_AUTO_TEST_CASE(SgVectorTestAssign)
{
    SgVector<int> a;
    a.Append(123);
    a.Append(444);
    a.Append(789);
    SgVector<int> b;
    b = a;
    BOOST_CHECK_EQUAL(b[0], 123);
    BOOST_CHECK_EQUAL(b[1], 444);
    BOOST_CHECK_EQUAL(b[2], 789);
    BOOST_CHECK(a == b);
}

BOOST_AUTO_TEST_CASE(SgVectorTestAssignElement)
{
    SgVector<int> a;
    a.Append(0);
    a.Append(789);
    BOOST_CHECK_EQUAL(a[1], 789);
    a[1] = 444;
    BOOST_CHECK_EQUAL(a[1], 444);
    BOOST_CHECK_EQUAL(a.Length(), 2);
}

BOOST_AUTO_TEST_CASE(SgVectorTestClear)
{
    SgVector<int> a;
    a.Clear();
    BOOST_CHECK(a.IsEmpty());
    BOOST_CHECK(! a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 0);
    a.Append(123);
    BOOST_CHECK(! a.IsEmpty());
    BOOST_CHECK(a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 1);
    a.Clear();
    BOOST_CHECK(a.IsEmpty());
    BOOST_CHECK(! a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 0);
    a.Append(0);
    a.Append(789);
    a.Append(123);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    a.Clear();
    SG_ASSERT(a.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgVectorTestAppendList)
{
    SgVector<int> a;
    a.Append(1);
    a.Append(2);
    a.Append(3);
    SgVector<int> b;
    b.Append(30);
    b.Append(20);
    b.Append(10);
    a.AppendList(b);
    BOOST_CHECK(b.IsLength(3));
    BOOST_CHECK(a.IsLength(6));
    BOOST_CHECK_EQUAL(a[0], 1);
    BOOST_CHECK_EQUAL(a[1], 2);
    BOOST_CHECK_EQUAL(a[2], 3);
    BOOST_CHECK_EQUAL(a[3], 30);
    BOOST_CHECK_EQUAL(a[4], 20);
    BOOST_CHECK_EQUAL(a[5], 10);
    BOOST_CHECK_EQUAL(b[0], 30);
    BOOST_CHECK_EQUAL(b[1], 20);
    BOOST_CHECK_EQUAL(b[2], 10);
}

BOOST_AUTO_TEST_CASE(SgVectorTestConcat)
{
    SgVector<int> a;
    a.Append(1);
    a.Append(2);
    a.Append(3);
    SgVector<int> b;
    b.Append(3);
    b.Append(2);
    b.Append(1);
    a.Concat(&b);
    BOOST_CHECK(b.IsEmpty());
    BOOST_CHECK(a.IsLength(6));
}

BOOST_AUTO_TEST_CASE(SgVectorTestContains)
{
    SgVector<int> a;
    BOOST_CHECK(! a.Contains(0));
    BOOST_CHECK(! a.Contains(1));
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    for (int i = 0; i < 10; ++i)
        BOOST_CHECK(a.Contains(i));
    BOOST_CHECK(! a.Contains(10));
    BOOST_CHECK(! a.Contains(11));
    BOOST_CHECK(! a.Contains(-1));
}

BOOST_AUTO_TEST_CASE(SgVectorTestExclude)
{
    SgVector<int> a;
    a.Append(789);
    a.Append(666);
    a.Append(123);
    BOOST_CHECK(! a.Exclude(555));
    BOOST_CHECK(a.Exclude(666));
    BOOST_CHECK(! a.Contains(666));
}

BOOST_AUTO_TEST_CASE(SgVectorTestIterator)
{
    SgVector<int> a;                 
    for (int i = 0; i < 10; ++i)
        a.Append(i);
    int count = 0;
    for (SgVectorIterator<int> it(a); it; ++it)
    {
        BOOST_CHECK_EQUAL(*it, count);
        ++count;
    }
    BOOST_CHECK_EQUAL(count, 10);    
}

BOOST_AUTO_TEST_CASE(SgVectorTestLargeList)
{
    SgVector<int> a;                 
    for (int i = 1; i <= 1000; ++i)
        a.Append(i);
    BOOST_CHECK(a.MinLength(1000));
    BOOST_CHECK(a.IsLength(1000));
    BOOST_CHECK(a.Contains(233));
    BOOST_CHECK(a.Contains(234));
    for (int i = 1; i <= 1000; i += 2)
        a.Exclude(i);
    BOOST_CHECK(a.MaxLength(1000));
    BOOST_CHECK(a.IsLength(500));
    BOOST_CHECK(! a.Contains(233));
    BOOST_CHECK(a.Contains(234));
    int y = a.Index(2);
    BOOST_CHECK_EQUAL(y, 0);
    y = a.Index(456);
    BOOST_CHECK_EQUAL(y, 227);
    BOOST_CHECK(a.Contains(456));
    a.DeleteAt(y);
    BOOST_CHECK(! a.Contains(456));
    BOOST_CHECK(a.Contains(2));
    a.DeleteAt(0);
    BOOST_CHECK(! a.Contains(2));
    BOOST_CHECK(a.Contains(1000));
    a.DeleteAt(a.Length() - 1);
    BOOST_CHECK(! a.Contains(1000));
}


BOOST_AUTO_TEST_CASE(SgVectorTestPop)
{
    SgVector<int> a;
    a.Append(123);
    a.Append(456);
    int x = a.Pop();
    BOOST_CHECK_EQUAL(x, 123);
    BOOST_CHECK_EQUAL(a.Length(), 1);
    BOOST_CHECK_EQUAL(a.Top(), 456);
    BOOST_CHECK_EQUAL(a.Tail(), 456);
    a.Append(x);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(a[0], 456);
    BOOST_CHECK_EQUAL(a[1], 123);
}

BOOST_AUTO_TEST_CASE(SgVectorTestPopBack)
{
    SgVector<int> a;
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    BOOST_CHECK_EQUAL(a.Length(), 10);
    for (int i = 9; i >= 0; --i)
    {
        a.PopBack();
        BOOST_CHECK_EQUAL(a.Length(), i);
    }
    BOOST_CHECK(a.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgVectorTestPush)
{
    SgVector<int> a;
    a.Push(0);
    a.Push(1);
    BOOST_CHECK_EQUAL(a[0], 1);
    BOOST_CHECK_EQUAL(a[1], 0);
    BOOST_CHECK_EQUAL(a.Length(), 2);
}

BOOST_AUTO_TEST_CASE(SgVectorTestPushBack)
{
    SgVector<int> a;
    a.PushBack(0);
    a.PushBack(1);
    BOOST_CHECK_EQUAL(a[0], 0);
    BOOST_CHECK_EQUAL(a[1], 1);
    BOOST_CHECK_EQUAL(a.Length(), 2);
}

BOOST_AUTO_TEST_CASE(SgVectorTestSwapWith)
{
    SgVector<int> a;
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);
    SgVector<int> b;
    b.PushBack(4);
    b.PushBack(5);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(b.Length(), 2);
    a.SwapWith(&b);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK_EQUAL(a[0], 4);
    BOOST_CHECK_EQUAL(a[1], 5);
    BOOST_CHECK_EQUAL(b[0], 1);
    BOOST_CHECK_EQUAL(b[1], 2);
    BOOST_CHECK_EQUAL(b[2], 3);
    b.Clear();
    b.SwapWith(&a);
    BOOST_CHECK_EQUAL(a.Length(), 0);
    BOOST_CHECK_EQUAL(b.Length(), 2);
    b.SwapWith(&a);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(b.Length(), 0);
}

BOOST_AUTO_TEST_CASE(SgVectorTestTopNth)
{
    SgVector<int> a;
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    for (int i = 1; i <= 10; ++i)
        BOOST_CHECK_EQUAL(a.TopNth(i), 10 - i);
}

void MakeTestVector(SgVector<int>& a, SgVectorOf<int>& pa)
{
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    for (int i = 0; i < 10; ++i)
        pa.Append(&a[i]);
}

BOOST_AUTO_TEST_CASE(SgVectorOfTestAppend)
{
    SgVector<int> a;
    SgVectorOf<int> pa;
    MakeTestVector(a, pa);
    for (int i = 0; i < 10; ++i)
    {
        BOOST_CHECK_EQUAL(pa[i], &a[i]);
        BOOST_CHECK_EQUAL(*pa[i], i);
    }
}

BOOST_AUTO_TEST_CASE(SgVectorOfTestContains)
{
    SgVector<int> a;
    SgVectorOf<int> pa;
    MakeTestVector(a, pa);
    for (int i = 0; i < 10; ++i)
    {
        BOOST_CHECK(pa.Contains(&a[i]));
    }
}

BOOST_AUTO_TEST_CASE(SgVectorIteratorOfTest)
{
    SgVector<int> a;
    SgVectorOf<int> pa;
    MakeTestVector(a, pa);
    
    int i = 0;
    for (SgVectorIteratorOf<int> it(pa); it; ++it,++i)
    {
        BOOST_CHECK_EQUAL(*it, &a[i]);
        BOOST_CHECK_EQUAL(**it, i);
    }
    BOOST_CHECK_EQUAL(i, 10);
}


} // namespace

//----------------------------------------------------------------------------

