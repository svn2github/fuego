//----------------------------------------------------------------------------
/** @file SgBoardColorTest.cpp
    Unit tests for SgColorIterator. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgBoardColor.h"

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgOppTest)
{
    BOOST_CHECK_EQUAL(SgOpp(SG_BLACK), SG_WHITE);
    BOOST_CHECK_EQUAL(SgOpp(SG_WHITE), SG_BLACK);
    BOOST_CHECK_EQUAL(SgOpp(SG_EMPTY), SG_EMPTY);
}

BOOST_AUTO_TEST_CASE(SgEBWTest)
{
    BOOST_CHECK_EQUAL(SgEBW(SG_BLACK), 'B');
    BOOST_CHECK_EQUAL(SgEBW(SG_WHITE), 'W');
    BOOST_CHECK_EQUAL(SgEBW(SG_EMPTY), 'E');
}

BOOST_AUTO_TEST_CASE(SgBoardColorCharTest)
{
    BOOST_CHECK_EQUAL(SgBoardColorChar(SG_BLACK), 'B');
    BOOST_CHECK_EQUAL(SgBoardColorChar(SG_WHITE), 'W');
    BOOST_CHECK_EQUAL(SgBoardColorChar(SG_EMPTY), 'E');
    BOOST_CHECK_EQUAL(SgBoardColorChar(SG_BORDER), '#');
}

BOOST_AUTO_TEST_CASE(SgEBWIteratorTest)
{
    SgEBWIterator i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_BLACK);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_WHITE);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_EMPTY);
    ++i;
    BOOST_CHECK(! i);
}

} // namespace

//----------------------------------------------------------------------------

