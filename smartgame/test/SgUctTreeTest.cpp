//----------------------------------------------------------------------------
/** @file SgUctTreeTest.cpp
    Unit tests for classes in SgUctTree.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "SgUctTree.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Test SgUctTreeIterator on a tree having only a root node. */
BOOST_AUTO_TEST_CASE(SgUctTreeIteratorTest_OnlyRoot)
{
    SgUctTree tree;
    SgUctTreeIterator it(tree);
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(&tree.Root(), &(*it));
    ++it;
    BOOST_CHECK(! it);
}

} // namespace

//----------------------------------------------------------------------------
