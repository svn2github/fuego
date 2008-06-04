//----------------------------------------------------------------------------
/** @file SgUctTree.cpp
    See SgUctTree.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctTree.h"

#include <boost/format.hpp>
#include "SgDebug.h"

using namespace std;
using boost::format;
using boost::shared_ptr;

//----------------------------------------------------------------------------

bool SgUctAllocator::Contains(const SgUctNode& node) const
{
    return (&node >= &m_nodes[0] && &node <= &m_nodes[m_nodes.size() - 1]);
}

void SgUctAllocator::Swap(SgUctAllocator& allocator)
{
    m_nodes.swap(allocator.m_nodes);
}

//----------------------------------------------------------------------------

SgUctTree::SgUctTree()
    : m_maxNodes(0),
      m_root(SG_NULLMOVE)
{
}

void SgUctTree::ApplyFilter(std::size_t allocatorId, const SgUctNode& node,
                            const vector<SgMove>& rootFilter)
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(Allocator(allocatorId).HasCapacity(node.NuChildren()));
    if (! node.HasChildren())
        return;

    SgUctAllocator& allocator = Allocator(allocatorId);
    vector<SgUctNode>& nodes = allocator.m_nodes;
    const SgUctNode* firstChild = &nodes[nodes.size()];

    int nuChildren = 0;
    for (SgUctChildIterator it(*this, node); it; ++it)
    {
        SgMove move = (*it).Move();
        if (find(rootFilter.begin(), rootFilter.end(), move)
            == rootFilter.end())
        {
            SgUctNode child(move);
            nodes.push_back(child);
            child.CopyDataFrom(*it);
            int childNuChildren = (*it).NuChildren();
            child.SetNuChildren(childNuChildren);
            if (childNuChildren > 0)
                child.SetFirstChild((*it).FirstChild());
            ++nuChildren;
        }
    }

    // SetNuChildren() must be called last, because of SgUctTree guarantees
    // that the children exist, if NuChildren() is greater zero.
    // See multi-threading comment at class SgUctSearch.
    // NOTE: no guarantee yet, that compiler does not reorder these
    // instructions
    SgUctNode& nonConstNode = const_cast<SgUctNode&>(node);
    nonConstNode.SetFirstChild(firstChild);
    nonConstNode.SetNuChildren(nuChildren);
}

void SgUctTree::CheckConsistency() const
{
    for (SgUctTreeIterator it(*this); it; ++it)
        if (! Contains(*it))
            ThrowConsistencyError(str(format("! Contains(%1%)") % &(*it)));
}

void SgUctTree::Clear()
{
    for (size_t i = 0; i < NuAllocators(); ++i)
        Allocator(i).Clear();
    m_root = SgUctNode(SG_NULLMOVE);
}

/** Check if node is in tree.
    Only used for assertions. May not be available in future implementations.
*/
bool SgUctTree::Contains(const SgUctNode& node) const
{
    if (&node == &m_root)
        return true;
    for (size_t i = 0; i < NuAllocators(); ++i)
        if (Allocator(i).Contains(node))
            return true;
    return false;
}

/** Recursive function used by SgUctTree::ExtractSubtree.
    @param target The target tree.
    @param targetNode The target node; it is already created but the content
    not yet copied
    @param node The node in the source tree to be copied.
    @param currentAllocatorId The current node allocator. Will be incremented
    in each call to CopySubtree to use node allocators of target tree evenly.
*/
void SgUctTree::CopySubtree(SgUctTree& target, SgUctNode& targetNode,
                            const SgUctNode& node,
                            std::size_t& currentAllocatorId,
                            bool warnTruncate) const
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(target.Contains(targetNode));
    targetNode.CopyDataFrom(node);

    if (! node.HasChildren())
        return;

    SgUctAllocator& targetAllocator = target.Allocator(currentAllocatorId);
    int nuChildren = node.NuChildren();
    if (! targetAllocator.HasCapacity(nuChildren))
    {
        // This can happen even if target tree has same maximum number of
        // nodes, because allocators are used differently. We don't copy
        // the children and set the pos count to zero (should reflect the sum
        // of children move counts)
        if (warnTruncate)
            SgDebug() <<
                "SgUctTree::CopySubtree: "
                "Tree truncated (low allocator capacity)\n";
        targetNode.SetPosCount(0);
        return;
    }

    vector<SgUctNode>& targetNodes = targetAllocator.m_nodes;

    size_t firstTargetChild = targetNodes.size();
    targetNode.SetFirstChild(&targetNodes[firstTargetChild]);
    targetNode.SetNuChildren(nuChildren);

    // Create target nodes first (must be contiguous in the target tree)
    for (int i = 0; i < nuChildren; ++i)
    {
        // Move will be copied later with CopyDataFrom
        SgUctNode targetChild(SG_NULLMOVE);
        SG_ASSERT(targetAllocator.HasCapacity(1));
        targetNodes.push_back(targetChild);
    }

    // Recurse
    size_t i = 0;
    for (SgUctChildIterator it(*this, node); it; ++it, ++i)
    {
        const SgUctNode& child = *it;
        ++currentAllocatorId; // Cycle to use allocators uniformly
        if (currentAllocatorId >= target.NuAllocators())
            currentAllocatorId = 0;
        CopySubtree(target, targetNodes[firstTargetChild + i], child,
                    currentAllocatorId, warnTruncate);
    }
}

void SgUctTree::CreateAllocators(std::size_t nuThreads)
{
    Clear();
    m_allocators.clear();
    for (size_t i = 0; i < nuThreads; ++i)
    {
        boost::shared_ptr<SgUctAllocator> allocator(new SgUctAllocator());
        m_allocators.push_back(allocator);
    }
}

void SgUctTree::CreateChildren(std::size_t allocatorId, const SgUctNode& node,
                               const vector<SgMove>& moves)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    SgUctNode& nonConstNode = const_cast<SgUctNode&>(node);
    size_t nuChildren = moves.size();
    SG_ASSERT(nuChildren > 0);
    SgUctAllocator& allocator = Allocator(allocatorId);
    SG_ASSERT(allocator.HasCapacity(nuChildren));

    // In lock-free multi-threading, a node can be expanded multiple times
    // (the later thread overwrites the children information of the previous
    // thread)
    SG_ASSERT(NuAllocators() > 1 || ! node.HasChildren());

    vector<SgUctNode>& nodes = allocator.m_nodes;
    const SgUctNode* firstChild = &nodes[nodes.size()];
    for (vector<SgMove>::const_iterator it = moves.begin(); it != moves.end();
         ++it)
    {
        SgMove move = *it;
        SgUctNode child(move);
        nodes.push_back(child);
    }

    // SetNuChildren() must be called last, because of SgUctTree guarantees
    // that the children exist, if NuChildren() is greater zero.
    // See multi-threading comment at class SgUctSearch.
    // NOTE: no guarantee yet, that compiler does not reorder these
    // instructions
    nonConstNode.SetFirstChild(firstChild);
    nonConstNode.SetNuChildren(nuChildren);
}

void SgUctTree::DumpDebugInfo(std::ostream& out) const
{
    for (size_t i = 0; i < NuAllocators(); ++i)
        out << "Allocator " << i
            << " size=" << Allocator(i).m_nodes.size()
            << " begin=" << &(*Allocator(i).m_nodes.begin())
            << " end=" << &(*Allocator(i).m_nodes.end()) << '\n';
}

void SgUctTree::ExtractSubtree(SgUctTree& target, const SgUctNode& node,
                               bool warnTruncate) const
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(&target != this);
    SG_ASSERT(target.MaxNodes() == MaxNodes());
    SG_ASSERT(Contains(node));
    target.Clear();
    size_t allocatorId = 0;
    CopySubtree(target, target.m_root, node, allocatorId, warnTruncate);
}

std::size_t SgUctTree::NuNodes() const
{
    size_t nuNodes = 1; // Count root node
    for (size_t i = 0; i < NuAllocators(); ++i)
        nuNodes += Allocator(i).NuNodes();
    return nuNodes;
}

void SgUctTree::SetMaxNodes(std::size_t maxNodes)
{
    Clear();
    size_t nuAllocators = NuAllocators();
    if (nuAllocators == 0)
    {
        SgDebug() << "SgUctTree::SetMaxNodes: no allocators registered\n";
        SG_ASSERT(false);
        return;
    }
    m_maxNodes = maxNodes;
    size_t maxNodesPerAlloc = maxNodes / nuAllocators;
    for (size_t i = 0; i < NuAllocators(); ++i)
        Allocator(i).SetMaxNodes(maxNodesPerAlloc);
}

void SgUctTree::Swap(SgUctTree& tree)
{
    SG_ASSERT(MaxNodes() == tree.MaxNodes());
    SG_ASSERT(NuAllocators() == tree.NuAllocators());
    swap(m_root, tree.m_root);
    for (size_t i = 0; i < NuAllocators(); ++i)
        Allocator(i).Swap(tree.Allocator(i));
}

void SgUctTree::ThrowConsistencyError(const string& message) const
{
    DumpDebugInfo(SgDebug());
    throw SgException("SgUctTree::ThrowConsistencyError: " + message);
}

//----------------------------------------------------------------------------

SgUctTreeIterator::SgUctTreeIterator(const SgUctTree& tree)
    : m_tree(tree),
      m_current(&tree.Root())
{
}

const SgUctNode& SgUctTreeIterator::operator*() const
{
    return *m_current;
}

void SgUctTreeIterator::operator++()
{
    if (m_current->HasChildren())
    {
        SgUctChildIterator* it = new SgUctChildIterator(m_tree, *m_current);
        m_stack.push(shared_ptr<SgUctChildIterator>(it));
        m_current = &(**it);
        return;
    }
    while (! m_stack.empty())
    {
        SgUctChildIterator& it = *m_stack.top();
        SG_ASSERT(it);
        ++it;
        if (it)
        {
            m_current = &(*it);
            return;
        }
        else
        {
            m_stack.pop();
            m_current = 0;
        }
    }
    m_current = 0;
}

SgUctTreeIterator::operator bool() const
{
    return (m_current != 0);
}

//----------------------------------------------------------------------------
