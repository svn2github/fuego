//----------------------------------------------------------------------------
/** @file SgUctTreeTest.cpp
    Unit tests for classes in SgUctTree.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "SgUctTree.h"
#include "SgUctTreeUtil.h"

using namespace std;
using SgUctTreeUtil::FindChildWithMove;

//----------------------------------------------------------------------------

namespace {

/** Test SgUctTreeIterator on a small tree. */
BOOST_AUTO_TEST_CASE(SgUctTreeIteratorTest_Simple)
{
    /* Test tree
       numbers in nodes: position indices
       (SgMove integers are node index times 10)

           (0)
          / | \
         /  |  \
       (1) (2) (3)
           /
          /
         (4)
    */
    SgUctTree tree;
    tree.CreateAllocators(1);
    tree.SetMaxNodes(10);
    vector<SgMove> moves;
    moves.push_back(10);
    moves.push_back(20);
    moves.push_back(30);
    const SgUctNode& root = tree.Root();
    tree.CreateChildren(0, root, moves);
    const SgUctNode& node1 = *FindChildWithMove(tree, root, 10);
    const SgUctNode& node2 = *FindChildWithMove(tree, root, 20);
    const SgUctNode& node3 = *FindChildWithMove(tree, root, 30);

    moves.clear();
    moves.push_back(40);
    tree.CreateChildren(0, node2, moves);
    const SgUctNode& node4 = *FindChildWithMove(tree, node2, 40);

    // Check that iteration works and is depth-first
    SgUctTreeIterator it(tree);
    BOOST_CHECK_EQUAL(&root, &(*it));
    ++it;
    BOOST_CHECK_EQUAL(&node1, &(*it));
    ++it;
    BOOST_CHECK_EQUAL(&node2, &(*it));
    ++it;
    BOOST_CHECK_EQUAL(&node4, &(*it));
    ++it;
    BOOST_CHECK_EQUAL(&node3, &(*it));
    ++it;
    BOOST_CHECK(! it);
}

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
