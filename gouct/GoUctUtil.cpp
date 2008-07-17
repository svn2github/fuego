//----------------------------------------------------------------------------
/** @file GoUctUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctUtil.h"

#include <iomanip>
#include <iostream>
#include "SgBWSet.h"
#include "SgPointSet.h"
#include "SgProp.h"
#include "SgStreamFmtRestorer.h"
#include "SgUctSearch.h"

using namespace std;
using SgPropUtil::PointToSgfString;

//----------------------------------------------------------------------------

namespace {

/** Recursive function to save the UCT tree in SGF format. */
void SaveNode(ostream& out, const SgUctTree& tree, const SgUctNode& node,
              SgBlackWhite toPlay, int boardSize, int maxDepth, int depth)
{
    out << "C[MoveCount " << node.MoveCount()
        << "\nPosCount " << node.PosCount()
        << "\nMean " << fixed << setprecision(2) << node.Mean();
    if (! node.HasChildren())
    {
        out << "]\n";
        return;
    }
    out << "\n\nRave:";
    for (SgUctChildIterator it(tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        SgPoint move = child.Move();
        if (child.RaveCount() > 0)
        {
            out << '\n' << SgWritePoint(move) << ' '
                << fixed << setprecision(2) << child.RaveValue()
                << " (" << child.RaveCount() << ')';
        }
    }
    out << "]\nLB";
    for (SgUctChildIterator it(tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        SgPoint move = child.Move();
        size_t count = child.MoveCount();
        if (count == 0)
            continue;
        out << "[" << PointToSgfString(move, boardSize, SG_PROPPOINTFMT_GO)
            << ':' << count << ']';
    }
    out << '\n';
    if (maxDepth >= 0 && depth >= maxDepth)
        return;
    for (SgUctChildIterator it(tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        if (child.MoveCount() == 0)
            continue;
        SgPoint move = child.Move();
        out << "(;" << (toPlay == SG_BLACK ? 'B' : 'W') << '['
            << PointToSgfString(move, boardSize, SG_PROPPOINTFMT_GO) << ']';
        SaveNode(out, tree, child, SgOppBW(toPlay), boardSize, maxDepth,
                 depth + 1);
        out << ")\n";
    }
}

} // namespace

//----------------------------------------------------------------------------

void GoUctUtil::GfxBestMove(const SgUctSearch& search, SgBlackWhite toPlay,
                         ostream& out)
{
    const SgUctTree& tree = search.Tree();
    const SgUctNode& root = tree.Root();
    out << "VAR";
    const SgUctNode* bestValueChild = search.FindBestChild(root);
    if (bestValueChild != 0)
    {
        SgPoint move = bestValueChild->Move();
        out << ' ' << (toPlay == SG_BLACK ? 'B' : 'W') << ' '
            << SgWritePoint(move);
        const SgUctNode* bestValueGrandChild =
            search.FindBestChild(*bestValueChild);
        if (bestValueGrandChild != 0)
        {
            SgPoint move = bestValueGrandChild->Move();
            out << ' ' << (toPlay == SG_WHITE ? 'B' : 'W') << ' '
                << SgWritePoint(move);
        }
    }
    out << '\n';
}

void GoUctUtil::GfxCounts(const SgUctTree& tree, ostream& out)
{
    const SgUctNode& root = tree.Root();
    out << "LABEL";
    if (root.HasChildren())
        for (SgUctChildIterator it(tree, root); it; ++it)
        {
            const SgUctNode& child = *it;
            size_t count = child.MoveCount();
            if (count > 0)
                out << ' ' << SgWritePoint(child.Move()) << ' ' << count;
        }
    out << '\n';
}

void GoUctUtil::GfxMoveValues(const SgUctSearch& search, SgBlackWhite toPlay,
                              ostream& out)
{
    const SgUctTree& tree = search.Tree();
    const SgUctNode& root = tree.Root();
    out << "INFLUENCE";
    if (root.HasChildren())
        for (SgUctChildIterator it(tree, root); it; ++it)
        {
            const SgUctNode& child = *it;
            if (child.MoveCount() == 0)
                continue;
            float value = search.InverseEval(child.Mean());
            // Scale to [-1,+1], black positive
            double influence = value * 2 - 1;
            if (toPlay == SG_WHITE)
                influence *= -1;
            SgPoint move = child.Move();
            out << ' ' << SgWritePoint(move) << ' ' << fixed
                << setprecision(2) << influence;
        }
    out << '\n';
}
\
void GoUctUtil::GfxSequence(const SgUctSearch& search, SgBlackWhite toPlay,
                            ostream& out)
{
    vector<SgMove> sequence;
    search.FindBestSequence(sequence);
    out << "VAR";
    for (size_t i = 0; i < sequence.size(); ++i)
    {
        out << (toPlay == SG_BLACK ? " B ": " W ")
            << SgWritePoint(sequence[i]);
        toPlay = SgOppBW(toPlay);
    }
    out << '\n';
}

void GoUctUtil::GfxStatus(const SgUctSearch& search, ostream& out)
{
    const SgUctTree& tree = search.Tree();
    const SgUctNode& root = tree.Root();
    int abortPercent = static_cast<int>(search.AbortedStat().Mean() * 100);
    out << "TEXT N=" << root.MoveCount()
        << " V=" << setprecision(2) << root.Mean()
        << " Len=" << static_cast<int>(search.GameLengthStat().Mean())
        << " Tree=" << setprecision(1) << search.MovesInTreeStat().Mean()
        << "/" << static_cast<int>(search.MovesInTreeStat().Max())
        << " Abrt=" << abortPercent << '%'
        << " Gm/s=" << static_cast<int>(search.GamesPerSecond()) << '\n';
}

void GoUctUtil::GfxTerritoryStatistics(
                     const SgPointArray<SgUctStatistics>& territoryStatistics,
                     const GoBoard& bd, std::ostream& out)
{
    SgStreamFmtRestorer restorer(out);
    out << fixed << setprecision(3) << "INFLUENCE";
    for (GoBoard::Iterator it(bd); it; ++it)
        if (territoryStatistics[*it].Count() > 0)
            // Scale to [-1,+1], black positive
            out << ' ' << SgWritePoint(*it) << ' '
                << territoryStatistics[*it].Mean() * 2 - 1;
    out << '\n';
}

void GoUctUtil::SaveTree(const SgUctTree& tree, int boardSize,
                         const SgBWSet& stones, SgBlackWhite toPlay,
                         ostream& out, int maxDepth)
{
    out << "(;FF[4]GM[1]SZ[" << boardSize << "]\n";
    for (SgBWIterator itColor; itColor; ++itColor)
    {
        const SgPointSet& stonesColor = stones[*itColor];
        if (stonesColor.Size() == 0)
            continue;
        out << ((*itColor) == SG_BLACK ? "AB" : "AW");
        for (SgSetIterator it(stonesColor); it; ++it)
            out << '[' << PointToSgfString(*it, boardSize, SG_PROPPOINTFMT_GO)
                << ']';
        out << '\n';
    }
    out << "PL[" << (toPlay == SG_BLACK ? "B" : "W") << "]\n";
    SaveNode(out, tree, tree.Root(), toPlay, boardSize, maxDepth, 0);
    out << ")\n";
}

namespace
{

/** Assist to sort nodes in GoUctUtil::ChildrenStatistics */
bool IsMeanLess(const SgUctNode* lhs, const SgUctNode* rhs)
{
    return (lhs->Mean() < rhs->Mean());
}

} // namespace

string GoUctUtil::ChildrenStatistics(const SgUctSearch& search,
                                     bool bSort, const SgUctNode& node)
{
    ostringstream out;
    vector<const SgUctNode*> vec;
    const SgUctTree& tree = search.Tree();
    for (SgUctChildIterator it(tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        vec.push_back(&child);
    }
    if (bSort)
        sort(vec.begin(), vec.end(), IsMeanLess);
    for (vector<const SgUctNode*>::iterator it = vec.begin(); it != vec.end();
         ++it)
    {
        const SgUctNode& child = **it;
        out << search.MoveString(child.Move()) << " -" << " value="
            << child.Mean() << " count=" << child.MoveCount() << '\n';
    }
    return out.str();
}

//----------------------------------------------------------------------------
