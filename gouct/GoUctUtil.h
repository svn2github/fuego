//----------------------------------------------------------------------------
/** @file GoUctUtil.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_UTIL_H
#define GOUCT_UTIL_H

#include <iosfwd>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoEyeUtil.h"
#include "GoModBoard.h"
#include "GoUctBoard.h"
#include "SgBlackWhite.h"
#include "SgPoint.h"
#include "SgRandom.h"

class SgBWSet;
template<typename T,int N> class SgSList;
class SgUctNode;
class SgUctSearch;
class SgUctTree;

//----------------------------------------------------------------------------

/** General utility functions used in GoUct.
    These functions are used in GoUct, but should not depend on other classes
    in GoUct to avoid cyclic dependencies.
*/
namespace GoUctUtil
{
    /** reject random move if it was self atari */
    const bool REMOVE_SELF_ATARI = false;

    const int SELF_ATARI_LIMIT = 8;

    /** Conservative clump correction.
        Only "very clumpy" moves are replaced
    */
    const bool CONSERVATIVE_CLUMP = true;

    /** Used in clump correction. */
    const int LINE_1_LIMIT = CONSERVATIVE_CLUMP ? 4 : 3;

    /** Used in clump correction. */
    const int LINE_2_OR_MORE_LIMIT = CONSERVATIVE_CLUMP ? 6 : 5;

    /** Check if move is self-atari and find other liberty, if yes.
        This can be applied as a filter in the playout policy, after a move
        was generated. It is a useful correction to the move generation using
        GoBoardUtil::IsCompletelySurrounded, which prunes backfilling moves.
        Does not check if the move is legal, because this is usually already
        checked in the playout policy, and requires that the move is no
        suicide move (checked with an assertion).

        1. If the move is a self-atari
        of more than 1 stone it is replaced by the liberty of the resulting
        block, unless it is also a self-atari.

        2. If p is single stone self-atari, possibly replace by neighbor.
        If p is a single stone with only one liberty and it is not a capture
        move: check if neighboring point has more liberties. If
        yes, replace by neighbor.
        @param bd
        @param p A (legal) move
        @return The replacement move, if one was found, otherwise the
        original move.
     */
    template<class BOARD>
    bool DoSelfAtariCorrection(const BOARD& bd, SgPoint& p);

    /** Check if p makes ugly clump. Possibly replace by neighbor.
        If p is close to many own stones:
        check if neighboring point looks better. If
        yes, replace by neighbor.
    */
    template<class BOARD>
    bool DoClumpCorrection(const BOARD& bd, SgPoint& p);

    /** Filter for generating moves in random phase.
        Checks if a point (must be empty) is a legal move and
        GoBoardUtil::IsCompletelySurrounded() returns false.
        If a policy generates no pass move as long as there are still moves
        on the board that this function would return true for, then the
        end position can be scored with GoBoardUtil::ScoreSimpleEndPosition().
    */
    template<class BOARD>
    bool GeneratePoint(const BOARD& bd, SgPoint p, SgBlackWhite toPlay);

    /** Print information about search as Gfx commands for GoGui.
        Can be used for GoGui live graphics during the search or GoGui
        analyze command type "gfx" after the search (see http://gogui.sf.net).
        The following information is output:
        - Best move and best response move as variation (shown as
          shadow stones in GoGui, however the move number labels in the
          variation are overwritten by the move counts, see below)
        - Move values as influence
        - Move counts as labels
        - Status line text:
          - N = Number games
          - V = Value of root node
          - Len = Average simulation sequence length
          - Tree = Average/maximum moves of simulation sequence in tree
          - Abrt = Percentage of games aborted (due to maximum game length)
          - Gm/s = Simulations per second
        @param search The search containing the tree and statistics
        @param toPlay The color toPlay at the root node of the tree
        @param out The stream to write the gfx commands to
    */
    void GoGuiGfx(const SgUctSearch& search, SgBlackWhite toPlay,
                  std::ostream& out);

    /** Alternative for GoGuiGfx that shows the main variation.
        Shows the main variation on the board instead of the counts and values
        (main variation and counts cannot be shown at the same time, because
        the sequence numbers conflict with the counts).
        The status line shows the same information as in GoGuiGfx()
    */
    void GoGuiGfx2(const SgUctSearch& search, SgBlackWhite toPlay,
                   std::ostream& out);

    /** Print best sequence of search in GoGui live-gfx format. */
    void PrintBestSequence(const SgUctSearch& search, SgBlackWhite toPlay,
                           std::ostream& out);

    /** Save tree contained in a search as a Go SGF file.
        The SGF file is written directly without using SgGameWriter to avoid
        a memory-intensive construction of an intermediate game tree.
        @param tree The tree
        @param boardSize The size of the board
        @param stones The Go position corresponding to the root node of the
        tree
        @param toPlay The color toPlay at the root node of the tree
        @param out The stream to save to tree to
    */
    void SaveTree(const SgUctTree& tree, int boardSize, const SgBWSet& stones,
                  SgBlackWhite toPlay, std::ostream& out);

    /** Select a random move from a list of empty points.
        The check if GeneratePoint() returns true for the point is done after
        the random selection to avoid calling this function for every point in
        the list. If GeneratePoint() returns false, the point is removed from
        the list and the process is repeated.
        @param bd The board
        @param toPlay The color to generate the move for
        @param emptyPts The list of empty points (will potentially be modified
        in this function for efficiency reasons)
        @param random The random generator
        @return The move or SG_NULLMOVE if no empty point is a legal move that
        should be generated
    */
    template<class BOARD>
    SgPoint SelectRandom(const BOARD& bd, SgBlackWhite toPlay,
                         GoPointList& emptyPts,
                         SgRandom& random);

    /** Return statistics of all children of a node.
        @param search The search containing the tree and statistics
        @param bSort Whether sort the children
        @param node The node
    */
    std::string ChildrenStatistics(const SgUctSearch& search,
                                   bool bSort, const SgUctNode& node);

    /** check if anchors[] are subset of neighbor blocks of nb */
    template<class BOARD>
    bool SubsetOfBlocks(const BOARD& bd, const SgPoint anchor[], SgPoint nb);
}

template<class BOARD>
bool GoUctUtil::DoClumpCorrection(const BOARD& bd, SgPoint& p)
{
    // if not a clump, don't correct p.
    if (bd.NumEmptyNeighbors(p) != 1)
        return false;
    const SgBlackWhite toPlay = bd.ToPlay();
    if (bd.Line(p) == 1)
    {
        if (   bd.Num8Neighbors(p, toPlay) < LINE_1_LIMIT
            || bd.NumNeighbors(p, toPlay) != 2
           )
            return false;
    }
    else if (   bd.Num8Neighbors(p, toPlay) < LINE_2_OR_MORE_LIMIT
             || bd.NumNeighbors(p, toPlay) != 3
            )
            return false;

    // only swap if nb is not worse than p
    const SgPoint nb = GoEyeUtil::EmptyNeighbor(bd, p);
    if (   bd.Num8Neighbors(nb, toPlay) <= bd.Num8Neighbors(p, toPlay)
        && bd.NumNeighbors(nb, toPlay) <= bd.NumNeighbors(p, toPlay)
        &&
           (   bd.NumEmptyNeighbors(nb) >= 2
            || ! GoBoardUtil::SelfAtari(bd, nb)
           )
       )
    {
        if (CONSERVATIVE_CLUMP) // no further tests, nb is good
        {
            p = nb;
            return true;
        }
        // keep p if it was a connection move and nb does not connect at least
        // the same blocks
        SgPoint anchor[4 + 1];
        bd.NeighborBlocks(p, toPlay, anchor);
        SG_ASSERT(anchor[0] != SG_ENDPOINT); // at least 1 block
        if (anchor[1] == SG_ENDPOINT // no connection, only 1 block
            || SubsetOfBlocks<BOARD>(bd, anchor, nb))
        {
            p = nb;
            return true;
        }
    }
    return false;
}

template<class BOARD>
inline bool GoUctUtil::DoSelfAtariCorrection(const BOARD& bd, SgPoint& p)
{
    // Function is inline despite its large size, because it returns quickly
    // on average, which makes the function call an overhead

    const SgBlackWhite toPlay = bd.ToPlay();
    // no self-atari
    if (bd.NumEmptyNeighbors(p) >= 2)
        return false;
    if (bd.NumNeighbors(p, toPlay) > 0) // p part of existing block(s)
    {
        if (! GoBoardUtil::SelfAtari(bd, p))
            return false;
        SgBlackWhite opp = SgOppBW(toPlay);
        SgPoint replaceMove = SG_NULLMOVE;
        // Replace move is the liberty we would have after playing at p
        for (SgNb4Iterator it(p); it; ++it)
        {
            SgBoardColor c = bd.GetColor(*it);
            if (c == SG_EMPTY)
                replaceMove = *it;
            else if (c == toPlay)
            {
                for (typename BOARD::LibertyIterator it2(bd, *it); it2; ++it2)
                    if (*it2 != p)
                    {
                        replaceMove = *it2;
                        break;
                    }
            }
            else if (c == opp && bd.InAtari(*it))
                replaceMove = *it;
            if (replaceMove != SG_NULLMOVE)
                break;
        }
        SG_ASSERT(replaceMove != SG_NULLMOVE);
        if (   bd.IsLegal(replaceMove)
            && ! GoBoardUtil::SelfAtari(bd, replaceMove)
            )
        {
            p = replaceMove;
            return true;
        }
    }
    else if (bd.NumEmptyNeighbors(p) > 0)
    // single stone with empty neighbors - possibly replace
    {
        // should we shift to nb? Is it better than p?
        const SgPoint nb = GoEyeUtil::EmptyNeighbor(bd, p);
        // Check if legal, could violate superko (with BOARD=GoBoard in
        // GoUctDefaultPriorKnowledge)
        if (bd.IsLegal(nb))
        {
            if (bd.NumEmptyNeighbors(nb) >= 2)
            {
                // check if p is a capturing move: then stay with p.
                SgPoint anchors[4 + 1];
                bd.NeighborBlocks(p, SgOppBW(toPlay), 1, anchors);
                if (anchors[0] != SG_ENDPOINT)
                    // at least one neighbor in atari exists
                    return false;
            }
            // nb seems better than p - switch.
            p = nb;
            return true;
        }
    }
    // no replacement found
    return false;
}

template<class BOARD>
inline bool GoUctUtil::GeneratePoint(const BOARD& bd, SgPoint p,
                                     SgBlackWhite toPlay)
{
    SG_ASSERT(bd.IsEmpty(p));
    SG_ASSERT(bd.ToPlay() == toPlay);
    if (   GoBoardUtil::IsCompletelySurrounded(bd, p)
        //|| GoEyeUtil::IsTwoPointEye(bd, p, toPlay)
        || ! bd.IsLegal(p, toPlay)
       )
        return false;
    if (REMOVE_SELF_ATARI)
    {
        SG_ASSERT(false);
        // The following code uses SgRandom, which is not thread-safe

        int nuStones = 0;
        if (   GoBoardUtil::SelfAtari(bd, p, nuStones, true)
            && nuStones > SELF_ATARI_LIMIT
            // todo: check for nakade shapes here.
           )
        {
            //SgDebug() << m_bd << "Removed selfatari"
            //<< SgWriteMove(mv, toPlay);
            return false;
        }
    }
    return true;
}

template<class BOARD>
inline SgPoint GoUctUtil::SelectRandom(const BOARD& bd,
                                       SgBlackWhite toPlay,
                                       GoPointList& emptyPts,
                                       SgRandom& random)
{
    while (true)
    {
        int length = emptyPts.Length();
        if (length == 0)
            break;
        int index = random.Int(length);
        SgPoint p = emptyPts[index];
        SG_ASSERT(bd.IsEmpty(p));
        if (GeneratePoint(bd, p, toPlay))
            return p;
        emptyPts[index] = emptyPts[length - 1];
        emptyPts.Pop();
    }
    return SG_NULLMOVE;
}

template<class BOARD>
bool SubsetOfBlocks(const BOARD& bd, const SgPoint anchor[], SgPoint nb)
{
    SgPoint nbanchor[4 + 1];
    bd.NeighborBlocks(nb, bd.ToPlay(), nbanchor);
    if (nbanchor[0] == SG_ENDPOINT)
        return false;
    for (int i = 0; anchor[i] != SG_ENDPOINT; ++i)
        if (! GoBoardUtil::ContainsAnchor(nbanchor, anchor[i]))
            return false;
    return true;
}

//----------------------------------------------------------------------------

#endif // GOUCT_UTIL_H
