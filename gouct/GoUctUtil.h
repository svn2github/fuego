//----------------------------------------------------------------------------
/** @file GoUctUtil.h */
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
#include "SgUctSearch.h"
#include "SgUtil.h"

class SgBWSet;
template<typename T,int N> class SgArrayList;

//----------------------------------------------------------------------------

/** General utility functions used in GoUct.
    These functions are used in GoUct, but should not depend on other classes
    in GoUct to avoid cyclic dependencies. */
namespace GoUctUtil
{
    /** reject random move if it was self atari */
    const bool REMOVE_SELF_ATARI = false;

    /** reject random move if it was both atari and self atari */
    const bool REMOVE_MUTUAL_ATARI = true;

    const int SELF_ATARI_LIMIT = 8;

    const int MUTUAL_ATARI_LIMIT = 2;

    /** Conservative clump correction.
        Only "very clumpy" moves are replaced.
        If false, more "clumps" are replaced. */
    const bool CONSERVATIVE_CLUMP = true;

    /** Used in clump correction. */
    const int LINE_1_LIMIT = CONSERVATIVE_CLUMP ? 4 : 3;

    /** Used in clump correction. */
    const int LINE_2_OR_MORE_LIMIT = CONSERVATIVE_CLUMP ? 6 : 5;

    void ClearStatistics(SgPointArray<SgUctStatistics>& stats);

    /** Check if move would make an ugly clump. Try to replace by neighbor.
        If move is close to many own stones: check if neighboring point 
        looks better. If yes, replace move by neighbor. */
    template<class BOARD>
    bool DoClumpCorrection(const BOARD& bd, SgPoint& move);

    /** Check if move would make a false eye. Try to replace by capture. */
    template<class BOARD>
    bool DoFalseEyeToCaptureCorrection(const BOARD& bd, SgPoint& move);

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
        original move. */
    template<class BOARD>
    bool DoSelfAtariCorrection(const BOARD& bd, SgPoint& p);

    /** Check, if playing at a lib gains liberties.
        Does not handle capturing moves for efficiency. Not needed, because
        capturing moves have a higher priority in the playout. */
    template<class BOARD>
    bool GainsLiberties(const BOARD& bd, SgPoint anchor, SgPoint lib);

    /** Generate a forced opening move.
        This function can be used to generate opening moves instead of doing a
        Monte Carlo tree search, which often returns random looking moves in
        the opening on large boards. Experiments showed also an improvement in
        playing strength if this function is used. The function currently
        generates a move on the 4-4 point of an empty corner under the
        following conditions:
        # The board size is 13 or larger
        # There are no more than 5 stones of each color on the board (avoids
          that the move generation triggers in positions containing lots of
          setup stones)
        # All points in the corner up to and including the 5th row are empty
        @return A randomly selected move that fulfills the conditions or
        SG_NULLPOINT if no such move exists. */
    SgPoint GenForcedOpeningMove(const GoBoard& bd);

    /** Filter for generating moves in random phase.
        Checks if a point (must be empty) is a legal move and
        GoBoardUtil::IsCompletelySurrounded() returns false.
        If a policy generates no pass move as long as there are still moves
        on the board that this function would return true for, then the
        end position can be scored with GoBoardUtil::ScoreSimpleEndPosition(). */
    template<class BOARD>
    bool GeneratePoint(const BOARD& bd, SgPoint p, 
                       SgBlackWhite toPlay);

    /** Print information about search as Gfx commands for GoGui.
        Can be used for GoGui live graphics during the search or GoGui
        analyze command type "gfx" after the search (see http://gogui.sf.net).
        Best move and best response move as variation (shown as
        shadow stones in GoGui).
        @param search The search containing the tree and statistics
        @param toPlay The color toPlay at the root node of the tree
        @param out The stream to write the gfx commands to */
    void GfxBestMove(const SgUctSearch& search, SgBlackWhite toPlay,
                     std::ostream& out);

    /** Print move counts as Gfx commands for GoGui.
        Can be used for GoGui live graphics during the search or GoGui
        analyze command type "gfx" after the search (see http://gogui.sf.net).
        Prints a LABEL command to display the move counts.
        @param tree
        @param out The stream to write the gfx commands to */
    void GfxCounts(const SgUctTree& tree, std::ostream& out);

    /** Print the move values as Gfx commands for GoGui.
        Can be used for GoGui live graphics during the search or GoGui
        analyze command type "gfx" after the search (see http://gogui.sf.net).
        The values of the moves in the root node are shown using an
        INFLUENCE gfx command.
        @param search The search containing the tree and statistics
        @param toPlay The color to play in the root node of the UCT tree
        @param out The stream to write the gfx commands to */
    void GfxMoveValues(const SgUctSearch& search, SgBlackWhite toPlay,
                       std::ostream& out);

    /** Print best sequence of search in GoGui live-gfx format. */
    void GfxSequence(const SgUctSearch& search, SgBlackWhite toPlay,
                     std::ostream& out);

    /** Print information about search as GoGui Gfx commands for text
        in the status line.
        Can be used for GoGui live graphics during the search or GoGui
        analyze command type "gfx" after the search (see http://gogui.sf.net).
        The following information is in the status line:
        - N = Number games
        - V = Value of root node
        - Len = Average simulation sequence length
        - Tree = Average/maximum moves of simulation sequence in tree
        - Abrt = Percentage of games aborted (due to maximum game length)
        - Gm/s = Simulations per second
        @param search The search containing the tree and statistics
        @param out The stream to write the gfx commands to */
    void GfxStatus(const SgUctSearch& search, std::ostream& out);

    /** Print territory statistics as GoGui gfx commands.
        Can be used for GoGui live graphics during the search or GoGui
        analyze command type "gfx" after the search (see http://gogui.sf.net).
        Uses INFLUENCE gfx command. */
    void GfxTerritoryStatistics(
            const SgPointArray<SgUctStatistics>& territoryStatistics,
            const GoBoard& bd, std::ostream& out);

    /** selfatari of a larger number of stones and also atari on opponent. */
    template<class BOARD>
    bool IsMutualAtari(const BOARD& bd, SgPoint p, SgBlackWhite toPlay);
                                 
    /** Save tree contained in a search as a Go SGF file.
        The SGF file is written directly without using SgGameWriter to avoid
        a memory-intensive construction of an intermediate game tree.
        @param tree The tree
        @param boardSize The size of the board
        @param stones The Go position corresponding to the root node of the
        tree
        @param toPlay The color toPlay at the root node of the tree
        @param out The stream to save to tree to
        @param maxDepth Only save tree to a certain depth. -1 means no limit. */
    void SaveTree(const SgUctTree& tree, int boardSize, const SgBWSet& stones,
                  SgBlackWhite toPlay, std::ostream& out, int maxDepth = -1);

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
        should be generated */
    template<class BOARD>
    SgPoint SelectRandom(const BOARD& bd, SgBlackWhite toPlay,
                         GoPointList& emptyPts,
                         SgRandom& random);

    /** Utility function used in DoClumpCorrection() */
    template<class BOARD>
    void SetEdgeCorrection(const BOARD& bd, SgPoint p, int& edgeCorrection);

    /** Return statistics of all children of a node.
        @param search The search containing the tree and statistics
        @param bSort Whether sort the children
        @param node The node */
    std::string ChildrenStatistics(const SgUctSearch& search,
                                   bool bSort, const SgUctNode& node);

    /** check if anchors[] are subset of neighbor blocks of nb */
    template<class BOARD>
    bool SubsetOfBlocks(const BOARD& bd, const SgPoint anchor[], SgPoint nb);
}

//----------------------------------------------------------------------------

template<class BOARD>
bool GoUctUtil::DoClumpCorrection(const BOARD& bd, SgPoint& move)
{
    // if not a clump, don't correct move.
    if (bd.NumEmptyNeighbors(move) != 1)
        return false;
    const SgBlackWhite toPlay = bd.ToPlay();
    if (bd.Line(move) == 1)
    {
        if (   bd.Num8Neighbors(move, toPlay) < LINE_1_LIMIT
            || bd.NumNeighbors(move, toPlay) != 2
           )
            return false;
    }
    else if (   bd.Num8Neighbors(move, toPlay) < LINE_2_OR_MORE_LIMIT
             || bd.NumNeighbors(move, toPlay) != 3
            )
            return false;

    // only swap if nb is better than move
    const SgPoint nb = GoEyeUtil::EmptyNeighbor(bd, move);
    int edgeCorrection_move = 0;
    int edgeCorrection_nb = 0;
    SetEdgeCorrection(bd, move, edgeCorrection_move);
    SetEdgeCorrection(bd, nb, edgeCorrection_nb);
    if (   bd.Num8Neighbors(nb, toPlay) + edgeCorrection_nb < 
           bd.Num8Neighbors(move, toPlay) + edgeCorrection_move
        && bd.NumNeighbors(nb, toPlay) <= bd.NumNeighbors(move, toPlay)
        &&
           (   bd.NumEmptyNeighbors(nb) >= 2
            || ! GoBoardUtil::SelfAtari(bd, nb)
           )
       )
    {
        if (CONSERVATIVE_CLUMP) // no further tests, nb is assumed to be good
        {
            move = nb;
            return true;
        }
        else
        {
            // keep move if it was a connection move and nb does not 
            // connect at least the same blocks
            SgPoint anchor[4 + 1];
            bd.NeighborBlocks(move, toPlay, anchor);
            SG_ASSERT(anchor[0] != SG_ENDPOINT); // at least 1 block
            if (anchor[1] == SG_ENDPOINT // no connection, only 1 block
               || SubsetOfBlocks<BOARD>(bd, anchor, nb)
               )
            {
                move = nb;
                return true;
            }
        }
    }
    return false;
}

template<class BOARD>
inline bool GoUctUtil::DoFalseEyeToCaptureCorrection(const BOARD& bd, 
                                                     SgPoint& move)
{
    SG_ASSERT(bd.IsEmpty(move));
    const SgBlackWhite opp = bd.Opponent();
    if (bd.HasEmptyNeighbors(move) || bd.HasNeighbors(move, opp))
    	return false;
    if (bd.Line(move) == 1)
    {
        if (  bd.NumDiagonals(move, SG_EMPTY) > 0 
           || bd.NumDiagonals(move, opp) > 1
           )
        	return false;
    }
    else if (  bd.NumDiagonals(move, SG_EMPTY) > 1
            || bd.NumDiagonals(move, opp) > 2
            )
    	return false;

    for (SgNb4DiagIterator it(move); it; ++it)
    {
        const SgPoint p = *it;
        if (bd.GetColor(p) == opp && bd.InAtari(p)) // try to capture p
        {
            const SgPoint lib = bd.TheLiberty(p);
            if (bd.IsLegal(lib))
            {
               	move = lib;
        		return true;
            }
        }
    }

    // no replacement found
    return false;
}

template<class BOARD>
inline bool GoUctUtil::DoSelfAtariCorrection(const BOARD& bd, SgPoint& move)
{
    // Function is inline despite its large size, because it returns quickly
    // on average, which makes the function call an overhead

    const SgBlackWhite toPlay = bd.ToPlay();
    // no self-atari
    if (bd.NumEmptyNeighbors(move) >= 2)
        return false;
    if (bd.NumNeighbors(move, toPlay) > 0) // move part of existing block(s)
    {
        if (! GoBoardUtil::SelfAtari(bd, move))
            return false;
        SgBlackWhite opp = SgOppBW(toPlay);
        SgPoint replaceMove = SG_NULLMOVE;
        // ReplaceMove is the liberty we would have after playing at p
        for (GoNb4Iterator<BOARD> it(bd, move); it; ++it)
        {
            SgBoardColor c = bd.GetColor(*it);
            if (c == SG_EMPTY)
                replaceMove = *it;
            else if (c == toPlay)
            {
                for (typename BOARD::LibertyIterator it2(bd, *it); it2; ++it2)
                    if (*it2 != move)
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
            move = replaceMove;
            return true;
        }
    }
    else if (bd.NumEmptyNeighbors(move) > 0 && ! bd.CanCapture(move, toPlay))
    // single stone with empty neighbors - possibly replace
    {
        // should we shift to nb? Is it better than move?
        const SgPoint nb = GoEyeUtil::EmptyNeighbor(bd, move);
        // Check if legal, could violate superko (with BOARD = GoBoard in
        // GoUctDefaultPriorKnowledge)
        if (  bd.IsLegal(nb)
           && (  bd.NumEmptyNeighbors(nb) >= 2
              || bd.CanCapture(nb, toPlay)
              )
           )
        {
            // nb seems better than move - switch.
            move = nb;
            return true;
        }
    }
    // no replacement found
    return false;
}

template<class BOARD>
bool GoUctUtil::GainsLiberties(const BOARD& bd, SgPoint anchor, SgPoint lib)
{
    SG_ASSERT(bd.IsEmpty(lib));
    SG_ASSERT(bd.Anchor(anchor) == anchor);
    const SgBlackWhite color = bd.GetStone(anchor);
    int nu = -2; // need 2 new libs (lose 1 lib by playing on lib itself)
    for (GoNb4Iterator<BOARD> it(bd, lib); it; ++it)
    {
        SgEmptyBlackWhite c = bd.GetColor(*it);
        if (c == SG_EMPTY)
        {
            if (! bd.IsLibertyOfBlock(*it, anchor))
                if (++nu >= 0)
                    return true;
        }
        else if (c == color) // merge with block
        {
            const SgPoint anchor2 = bd.Anchor(*it);
            if (anchor != anchor2)
                for (typename BOARD::LibertyIterator it(bd, anchor2); it;
                     ++it)
                    if (! bd.IsLibertyOfBlock(*it, anchor))
                        if (++nu >= 0)
                            return true;
        }
        // else capture - not handled, see function documentation
    }
    return false;
}

template<class BOARD>
inline bool GoUctUtil::IsMutualAtari(const BOARD& bd, 
                                     SgPoint p, SgBlackWhite toPlay)
{
    int nuStones = 0;
    if (   GoBoardUtil::SelfAtari(bd, p, nuStones)
        && nuStones > MUTUAL_ATARI_LIMIT
        && (   nuStones > GoEyeUtil::NAKADE_LIMIT
            || ! GoEyeUtil::MakesNakadeShape(bd, p, toPlay)
           )
       )
    {
        SG_ASSERT(bd.ToPlay() == toPlay);
        SgBlackWhite opp = SgOppBW(toPlay);
        bool selfatari =
                bd.HasNeighbors(p, opp) &&
                GoBoardUtil::SelfAtariForColor(bd, p, opp);
        if (selfatari)
            return true;
    }
    return false;
}

template<class BOARD>
inline bool GoUctUtil::GeneratePoint(const BOARD& bd,
                                     SgPoint p, SgBlackWhite toPlay)
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
        int nuStones = 0;
        if (   GoBoardUtil::SelfAtari(bd, p, nuStones)
            && nuStones > SELF_ATARI_LIMIT
            // todo: check for nakade shapes here.
           )
        {
            return false;
        }
    }
    
    if (REMOVE_MUTUAL_ATARI && IsMutualAtari(bd, p, toPlay))
        return false;
    return true;
}

template<class BOARD>
inline SgPoint GoUctUtil::SelectRandom(const BOARD& bd,
                                       SgBlackWhite toPlay,
                                       GoPointList& emptyPts,
                                       SgRandom& random)
{
    for (;;)
    {
        int length = emptyPts.Length();
        if (length == 0)
            break;
        int index = random.SmallInt(length);
        SgPoint p = emptyPts[index];
        SG_ASSERT(bd.IsEmpty(p));
        if (GeneratePoint(bd, p, toPlay))
            return p;
        emptyPts[index] = emptyPts[length - 1];
        emptyPts.PopBack();
    }
    return SG_NULLMOVE;
}

template<class BOARD>
void GoUctUtil::SetEdgeCorrection(const BOARD& bd, SgPoint p,
                                  int& edgeCorrection)
{
    if (bd.Line(p) == 1)
    {
        edgeCorrection += 3;
        if (bd.Pos(p) == 1)
            edgeCorrection += 2;
    }
}

template<class BOARD>
bool GoUctUtil::SubsetOfBlocks(const BOARD& bd, const SgPoint anchor[],
                               SgPoint nb)
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
