//----------------------------------------------------------------------------
/** @file SgStackTest.cpp
    Unit tests for SgStack.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgStack.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgStackTestIsEmpty)
{
    SgStack<int, 5> s;
    BOOST_CHECK(s.IsEmpty());
    s.Push(3);
    BOOST_CHECK(! s.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgStackTestNonEmpty)
{
    SgStack<int, 5> s;
    BOOST_CHECK(! s.NonEmpty());
    s.Push(3);
    BOOST_CHECK(s.NonEmpty());
}

BOOST_AUTO_TEST_CASE(SgStackTestPushPop)
{
    SgStack<int, 5> s;
    BOOST_CHECK(s.Size() == 0);
    s.Push(3);
    BOOST_CHECK_EQUAL(s[0],  3);
    BOOST_CHECK_EQUAL(s.Top(),  3);
    BOOST_CHECK(s.Size() == 1);
    s.Push(5);
    BOOST_CHECK(s.Size() == 2);
    BOOST_CHECK_EQUAL(s[0],  3);
    BOOST_CHECK_EQUAL(s[1],  5);
    BOOST_CHECK_EQUAL(s.Top(),  5);
    s.Pop();
    BOOST_CHECK(s.Size() == 1);
    BOOST_CHECK_EQUAL(s[0],  3);
    BOOST_CHECK_EQUAL(s.Top(),  3);
    s.Pop();
    BOOST_CHECK(s.Size() == 0);
}

} // namespace

//----------------------------------------------------------------------------

