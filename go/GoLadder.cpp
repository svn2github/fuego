//----------------------------------------------------------------------------
/** @file GoLadder.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoLadder.h"

#include <algorithm>
#include <memory>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "SgList.h"
#include "SgStack.h"

using namespace std;
using GoBoardUtil::NeighborsOfColor;
using GoBoardUtil::PlayIfLegal;

//----------------------------------------------------------------------------

namespace {

const int GOOD_FOR_PREY = 1000;

const int GOOD_FOR_HUNTER = -1000;

} // namespace

//----------------------------------------------------------------------------

GoLadder::GoLadder(GoBoard& board)
    : m_board(board),
      m_partOfPrey()
{
}

inline bool GoLadder::CheckMoveOverflow() const
{
    return (m_board.MoveNumber() >= m_maxMoveNumber);
}

void GoLadder::InitMaxMoveNumber()
{
    // reserve is the maximum number of moves played before recursion
    // 5 is an optimistic bound
    const int reserve = 5;
    m_maxMoveNumber = min(m_board.MoveNumber() + MAX_LADDER_MOVES,
                          GO_MAX_NUM_MOVES - reserve);
}

bool GoLadder::IsBoard(const GoBoard& board) const
{
    return &m_board == &board;
}

/** Marks all stones in the block p as part of the prey.
    If 'stones' is not 0, then append the stones to the existing list.
*/
void GoLadder::MarkStonesAsPrey(SgPoint p, SgList<SgPoint>* stones)
{
    SG_ASSERT(m_board.IsValidPoint(p));
    if (m_board.Occupied(p))
    {
        for (GoBoard::StoneIterator it(m_board, p); it; ++it)
        {
            SgPoint p = *it;
            m_partOfPrey.Include(p);
            if (stones)
                stones->Append(p);
        }
    }
}

/** Filter out captured blocks, blocks not in atari, and blocks not adjacent
    to the prey.
    The latter are found by checking whether blocks adjacent to the block in
    question are the prey or not. Does not return the correct blocks if the
    prey has more than three liberties, but in that case, the prey has
    escaped anyway.
*/
void GoLadder::FilterAdjacent(SgList<SgPoint>* adjBlocks)
{
    SgList<SgPoint> temp;
    for (SgListIterator<SgPoint> it(*adjBlocks); it; ++it)
    {
        SgPoint block = *it;
        if (m_board.IsColor(block, m_hunterColor)
            && m_board.InAtari(block)
            && BlockIsAdjToPrey(block, 1))
        {
            temp.Append(block);
        }
    }
    adjBlocks->SwapWith(&temp);
    ReduceToBlocks(adjBlocks);
}

bool GoLadder::PointIsAdjToPrey(SgPoint p)
{
    return  m_partOfPrey[p - SG_NS]
         || m_partOfPrey[p - SG_WE]
         || m_partOfPrey[p + SG_WE]
         || m_partOfPrey[p + SG_NS];
}

bool GoLadder::BlockIsAdjToPrey(SgPoint p, int numAdj)
{
    SG_ASSERT(m_board.IsColor(p, m_hunterColor));
    for (GoBoard::StoneIterator it(m_board, p); it; ++it)
        if (PointIsAdjToPrey(*it) && --numAdj == 0)
            return true;
    return false;
}

/** Play hunter move and update all the relevant information.
    Play at one of the two liberties of the prey.
*/
int GoLadder::PlayHunterMove(int depth, SgPoint move, SgPoint lib1,
                             SgPoint lib2, const SgList<SgPoint>& adjBlk,
                             SgList<SgPoint>* sequence)
{
    SG_ASSERT(move == lib1 || move == lib2);
    // TODO: only pass move and otherLib
    int result = 0;
    if (PlayIfLegal(m_board, move, m_hunterColor))
    {
        // Find new adjacent blocks: only block just played can be new 
        // in atari.
        // But other blocks previously in atari may have gained new liberties
        // because the move captured a stone, or the move may have extended a
        // block previously in atari.
        //   If it was in atari before, and the move doesn't capture
        // anything, then the block will still be in atari afterwards - no
        // need to check again.
        SgList<SgPoint> newAdj;
        if (m_board.InAtari(move))
            newAdj.Append(move);
        for (SgListIterator<SgPoint> iter(adjBlk); iter; ++iter)
        {
            SgPoint block = *iter;
            if (! m_board.AreInSameBlock(block, move))
            {
                if (! m_board.CapturingMove() || m_board.InAtari(block))
                    newAdj.Append(block);
            }
        }
        if (move == lib1)
            lib1 = lib2;
        result = PreyLadder(depth + 1, lib1, newAdj, sequence);
        if (sequence)
            sequence->Push(move);
        m_board.Undo();
    }
    else
    {
        if (sequence)
            sequence->Clear();
        result = GOOD_FOR_PREY - depth;
    }
    return result;
}

/** Play prey move and update all the relevant information.
    Extend the prey by playing at its only liberty, or capture a block
    adjacent to the prey.
*/
int GoLadder::PlayPreyMove(int depth, SgPoint move, SgPoint lib1,
                           const SgList<SgPoint>& adjBlk,
                           SgList<SgPoint>* sequence)
{
    int result = 0;
    SgList<SgPoint> newAdj(adjBlk);
    SgList<SgPoint> newLib;
    SgList<SgPoint> newStones;
    SgList<SgPoint> neighbors;
    if (move == lib1)
    {
        NeighborsOfColor(m_board, move, m_preyColor, &neighbors);
        for (SgListIterator<SgPoint> iter(neighbors); iter; ++iter)
        {
            SgPoint block = *iter;
            if (! m_partOfPrey[block])
            {
                MarkStonesAsPrey(block, &newStones);
                SgList<SgPoint> temp;
                GoBoardUtil::AdjacentStones(m_board, block, &temp);
                newAdj.Concat(&temp);
                for (GoBoard::LibertyIterator it(m_board, block); it; ++it)
                    newLib.Include(*it);
            }
        }
        m_partOfPrey.Include(move);
    }
    if (PlayIfLegal(m_board, move, m_preyColor))
    {
        if (move == lib1)
        {
            NeighborsOfColor(m_board, move, SG_EMPTY, &neighbors);
            for (SgListIterator<SgPoint> iter(newLib); iter; ++iter)
            {
                SgPoint point = *iter;
                // Test for Empty is necessary because newLib will include
                // the move just played.
                if (m_board.IsEmpty(point))
                    neighbors.Include(point);
            }
        }
        else
        {
            neighbors.Append(lib1);
        }
        if (m_board.CapturingMove())
        {   // Add the points at the captured stones that are adjacent to the
            // prey to the liberties, at least if exactly one stone captured.
            for (GoPointSList::Iterator it(m_board.CapturedStones()); it;
                 ++it)
            {
                SgPoint stone = *it;
                if (PointIsAdjToPrey(stone))
                    neighbors.Include(stone);
            }
        }
        SG_ASSERT(! neighbors.IsEmpty());
        lib1 = neighbors.Pop();
        SG_ASSERT(m_board.IsEmpty(lib1));
        SgPoint lib2;
        int numLib;
        if (neighbors.IsEmpty())
        {
            numLib = 1;
            lib2 = 0;
        }
        else
        {
            lib2 = neighbors.Pop();
            numLib = neighbors.IsEmpty() ? 2 : 3 /* or more */;
            SG_ASSERT(m_board.IsEmpty(lib2));
        }

        SgList<SgPoint> temp;
        NeighborsOfColor(m_board, move, m_hunterColor, &temp);
        newAdj.Concat(&temp);
        FilterAdjacent(&newAdj);
        result = HunterLadder(depth+1, numLib, lib1, lib2, newAdj, sequence);
        if (sequence)
            sequence->Push(move);
        m_board.Undo();
    }
    else
    {
        if (sequence)
            sequence->Clear();
        result = GOOD_FOR_HUNTER+depth;
    }
    m_partOfPrey.Exclude(move);
    m_partOfPrey.Exclude(newStones);

    return result;
}

int GoLadder::PreyLadder(int depth, SgPoint lib1,
                         const SgList<SgPoint>& adjBlk,
                         SgList<SgPoint>* sequence)
{
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    int result = 0;
    for (SgListIterator<SgPoint> iter(adjBlk); iter; ++iter)
    {
        SgPoint block = *iter;
        SgPoint move = *GoBoard::LibertyIterator(m_board, block);
        if (BlockIsAdjToPrey(block, 2))
        {
            if (sequence)
                sequence->SetTo(move);
            result = GOOD_FOR_PREY - depth;
        }
        else if (move != lib1)
        {
            result = PlayPreyMove(depth, move, lib1, adjBlk, sequence);
        }
        if (0 < result)
            break;
    }
    if (result <= 0)
    {
        if (sequence)
        {
            SgList<SgPoint> seq2;
            int result2 = PlayPreyMove(depth, lib1, lib1, adjBlk, &seq2);
            if (result < result2 || result == 0)
            {
                result = result2;
                sequence->SwapWith(&seq2);
            }
        }
        else
        {
            int result2 = PlayPreyMove(depth, lib1, lib1, adjBlk, 0);
            if (result < result2 || result == 0)
                result = result2;
        }
    }
    return result;
}

int GoLadder::HunterLadder(int depth, int numLib, SgPoint lib1,
                           SgPoint lib2, const SgList<SgPoint>& adjBlk,
                           SgList<SgPoint>* sequence)
{
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    int result = 0;
    switch (numLib)
    {
    case 1:
        {
            if (sequence)
                sequence->SetTo(lib1);
            // TODO: should probably test for IsSnapback here, but don't have
            // the right information available.
            result = GOOD_FOR_HUNTER + depth;
            break;
        }
    case 2:
        {
            if (m_board.NumEmptyNeighbors(lib1)
                < m_board.NumEmptyNeighbors(lib2))
            {
                swap(lib1, lib2);
            }
            if (m_board.NumEmptyNeighbors(lib1) == 3
                && ! SgPointUtil::AreAdjacent(lib1, lib2))
            {
                // If not playing at lib1, then prey will play at lib1 and
                // get three liberties; little to update in this case.
                m_board.Play(lib1, m_hunterColor);
                result = PreyLadder(depth + 1, lib2, adjBlk, sequence);
                if (sequence)
                    sequence->Push(lib1);
                m_board.Undo();
            }
            else
            {
                // Two liberties, hunter to play, but not standard case.
                if (! adjBlk.IsEmpty()
                    && *GoBoard::LibertyIterator(m_board, adjBlk.Top())
                       == lib2)
                {
                    swap(lib1, lib2); // protect hunter blocks in atari
                }
                result = PlayHunterMove(depth, lib1, lib1, lib2,
                                        adjBlk, sequence);
                if (0 <= result) // escaped
                {
                    if (sequence)
                    {
                        SgList<SgPoint> seq2;
                        int result2 = PlayHunterMove(depth, lib2, lib1, lib2,
                                                     adjBlk, &seq2);
                        if (result2 < result)
                        {   result = result2;
                            sequence->SwapWith(&seq2);
                        }
                    }
                    else
                    {
                        int result2 = PlayHunterMove(depth, lib2, lib1, lib2,
                                                     adjBlk, 0);
                        if (result2 < result)
                            result = result2;
                    }
                }
            }
            break;
        }
    default: // 3 <= numLib
        {
            if (sequence)
                sequence->Clear();
            result = GOOD_FOR_PREY - depth;
            break;
        }
    }
    return result;
}

void GoLadder::ReduceToBlocks(SgList<SgPoint>* stones)
{
    // Single block is frequent case, don't compute block.
    if (stones->IsEmpty())
        ; // nothing to do
    else if (stones->MaxLength(1))
    {
        if (m_board.IsEmpty(stones->Top()))
            stones->Clear();
    }
    else
    {
        SgList<SgPoint> visited;
        // AR: should use some kind of marks for efficiency, but
        //     cannot use M since calling Stones, which uses it.
        SgList<SgPoint> result;
        for (SgListIterator<SgPoint> iter(*stones); iter; ++iter)
        {
            SgPoint stone = *iter;
            if (m_board.Occupied(stone) && ! visited.Contains(stone))
            {
                result.Append(stone);
                for (GoBoard::StoneIterator it(m_board, stone); it; ++it)
                visited.Append(*it);
            }
        }
        stones->SwapWith(&result);
    }
}

/** Main ladder routine */
int GoLadder::Ladder(SgPoint prey, SgBlackWhite toPlay,
                     SgList<SgPoint>* sequence, bool twoLibIsEscape)
{
    InitMaxMoveNumber();
    if (sequence)
        sequence->Clear();
    if (! m_board.Occupied(prey))
        return 0;
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    int result = 0;
    m_preyColor = m_board.GetStone(prey);
    m_hunterColor = SgOppBW(m_preyColor);
    int numLib = m_board.NumLiberties(prey);
    if (2 < numLib)
        result = GOOD_FOR_PREY;
    else
    {
        SgList<SgPoint> libs;
        for (GoBoard::LibertyIterator it(m_board, prey); it; ++it)
            libs.Append(*it);
        SgPoint lib1 = libs.Pop();
        m_partOfPrey.Clear();
        MarkStonesAsPrey(prey);
        SgList<SgPoint> adjBlk;
        GoBoardUtil::AdjacentStones(m_board, prey, &adjBlk);
        FilterAdjacent(&adjBlk);
        if (toPlay == m_preyColor)
        {
            if (numLib == 1)
                result = PreyLadder(0, lib1, adjBlk, sequence);
            else if (twoLibIsEscape) // prey to play, numLib >= 2
                // For example, Explorer cannot treat this case as a ladder,
                // it messes up the logic
                result = GOOD_FOR_PREY;
            else
            {
                // Prey to play, two liberties. This is usually good, but
                // need to prove that there is some move that really
                //  escapes laddercapture.
                // Try liberties of adjacent blocks with at most two
                // liberties, try lib1 and lib2, and try moves one away
                // from the two liberties.
                // Good example with three blocks that test this case:
                // (;GM[1]SZ[19]FF[3]
                // AB[qa][pa][pb][pd][pc][qe][re][rd][rc][se]
                // AW[pe][pf][qf][qd][qc][rb][qb][sa][sc][rf][rg][sg])
                SgList<SgPoint> movesToTry;

                // Liberties of adj. blocks with at most two liberties.
                GoBoardUtil::AdjacentStones(m_board, prey, &adjBlk);
                ReduceToBlocks(&adjBlk);
                for (SgListIterator<SgPoint> iterAdj(adjBlk);
                     iterAdj; ++iterAdj)
                {
                    SgPoint block = *iterAdj;
                    SG_ASSERT(m_board.IsColor(block, m_hunterColor));
                    SG_ASSERT(BlockIsAdjToPrey(block, 1));
                    if (m_board.NumLiberties(block) <= 2)
                        for (GoBoard::LibertyIterator it(m_board, block); it;
                             ++it)
                            movesToTry.Append(*it);
                }

                // Liberties of blocks.
                SgPoint lib2 = libs.Top();
                movesToTry.Append(lib1);
                movesToTry.Append(lib2);

                // Moves one away from liberties.
                SgList<SgPoint> neighbors;
                NeighborsOfColor(m_board, lib1, SG_EMPTY, &neighbors);
                movesToTry.Concat(&neighbors);
                NeighborsOfColor(m_board, lib2, SG_EMPTY, &neighbors);
                movesToTry.Concat(&neighbors);

                // Try whether any of these moves lead to escape.
                for (SgListIterator<SgPoint> it(movesToTry); it; ++it)
                {
                    if (PlayIfLegal(m_board, *it, m_preyColor))
                    {
                        if (Ladder(prey, m_hunterColor, 0, twoLibIsEscape)
                            > 0)
                        {
                            if (sequence)
                                sequence->Append(*it);
                            result = GOOD_FOR_PREY;
                        }
                        m_board.Undo();
                    }
                    if (result != 0)
                        break;
                }

                // If none of those moves worked, prey can't escape.
                // This is a bit pessimistic, there may be other moves
                // that do lead to escape (e.g. approach moves), but
                // ladder algorithm doesn't know about those.
                if (result == 0)
                    result = GOOD_FOR_HUNTER;
            }
        }
        else
        {
            if (IsSnapback(prey))
                result = GOOD_FOR_PREY;
            else
            {
                // AR: split HunterLadder into two methods (1 vs 2 libs)
                SgPoint lib2 = libs.IsEmpty() ? SG_NULLMOVE : libs.Top();
                result = HunterLadder(0, numLib, lib1, lib2, adjBlk,
                                      sequence);
            }
        }
    }   
    return result;
}

/** Check whether the block at 'prey' is caught in a snapback.
    Snapback means that it can be captured, but it's only a single stone, and
    the prey can capture right back.
*/
bool GoLadder::IsSnapback(SgPoint prey)
{
    bool isSnapback = false;
    if (m_board.IsSingleStone(prey) && m_board.InAtari(prey))
    {
        SgPoint liberty = *GoBoard::LibertyIterator(m_board, prey);
        if (PlayIfLegal(m_board, liberty, SgOppBW(m_board.GetStone(prey))))
        {
            isSnapback = (m_board.InAtari(liberty)
                          && ! m_board.IsSingleStone(liberty));
            m_board.Undo();
        }
    }
    return isSnapback;
}

//----------------------------------------------------------------------------

bool GoLadderUtil::Ladder(GoBoard& bd, SgPoint prey, SgBlackWhite toPlay,
                          bool twoLibIsEscape, SgList<SgPoint>* sequence)
{
    SG_ASSERT(bd.IsValidPoint(prey));
    SG_ASSERT(bd.Occupied(prey));
    // AR: from Martin: for an unsettled block with 2 liberties, it
    // immediately says it can escape, but does not return a move.
    // Sequence is empty.  So I have to special case this and look for
    // moves that escape from ladder myself.
    // ---> need to tell Martin if I find this
#ifdef _DEBUG
    SgHashCode oldHash = bd.GetHashCode();
#endif    
    GoLadder ladder(bd);
    int result = ladder.Ladder(prey, toPlay, sequence, twoLibIsEscape);
#ifdef _DEBUG
    // Make sure Ladder didn't change the board position.
    SG_ASSERT(oldHash == bd.GetHashCode());
#endif
    SG_ASSERT(result != 0);
    return (result < 0);
}

GoLadderStatus GoLadderUtil::LadderStatus(GoBoard& bd, SgPoint prey,
                                          bool twoLibIsEscape,
                                          SgPoint* toCapture,
                                          SgPoint* toEscape)
{
    SG_ASSERT(bd.IsValidPoint(prey));
    SG_ASSERT(bd.Occupied(prey));
#ifdef _DEBUG
    SgHashCode oldHash = bd.GetHashCode();
#endif
    // Unsettled only if can capture when hunter plays first, and can escape
    // if prey plays first.
    GoLadder ladder(bd);
    SgBlackWhite preyColor = bd.GetStone(prey);
    SgList<SgPoint> captureSequence;
    GoLadderStatus status = GO_LADDER_ESCAPED;
    if (ladder.Ladder(prey, SgOppBW(preyColor), &captureSequence,
                      twoLibIsEscape) < 0)
    {
        SgList<SgPoint> escapeSequence;
        if (ladder.Ladder(prey, preyColor, &escapeSequence,
                          twoLibIsEscape) < 0)
            status = GO_LADDER_CAPTURED;
        else
        {
            status = GO_LADDER_UNSETTLED;
            // Unsettled = ladder depends on who plays first, so there must
            // be a move that can be played.
            SG_ASSERT(captureSequence.NonEmpty());
            // escapeSequence can be empty in 2 libs, prey to play case
            SG_ASSERT(twoLibIsEscape || escapeSequence.NonEmpty());
            if (toCapture)
                *toCapture = captureSequence.Top();
            if (toEscape)
                *toEscape = escapeSequence.IsEmpty() ? SG_PASS :
                                                       escapeSequence.Top();
        }
    }
#ifdef _DEBUG
    // Make sure Ladder didn't change the board position.
    SG_ASSERT(oldHash == bd.GetHashCode());
#endif    
    return status;
}

bool GoLadderUtil::IsProtectedLiberty(const GoBoard& bd, SgPoint liberty,
                                      SgBlackWhite color)
{
    bool ignoreLadder;
    bool ignoreKo;
    return IsProtectedLiberty(bd, liberty, color, ignoreLadder, ignoreKo,
                              true);
}

bool GoLadderUtil::IsProtectedLiberty(const GoBoard& bd1, SgPoint liberty,
                                      SgBlackWhite col, bool& byLadder,
                                      bool& isKoCut, bool tryLadder)
{
    byLadder = false;
    isKoCut = false;
    GoModBoard mbd(bd1);
    GoBoard& bd = mbd.Board();
    
    const SgBlackWhite toPlay = bd1.ToPlay();
    bd.SetToPlay(SgOppBW(col));
    bool isProtected;
    if (! PlayIfLegal(bd, liberty))
        isProtected = bd.LastMoveInfo(isSuicide);
        // opponent cannot play there
    else
    {
        if (bd.LastMoveInfo(isSuicide)) 
           isProtected = true;
        else
        {
            if (bd.InAtari(liberty))
            {
                if (bd.NumStones(liberty) > 1)
                    isProtected = true; 
                else
                {
                    SgPoint p = bd.TheLiberty(liberty);
                    if (PlayIfLegal(bd, p))
                    {
                        isProtected =    (bd.NumStones(p) != 1)
                                      || (bd.NumLiberties(p) != 1);
                                      // yes, can re-capture there
                        bd.Undo();
                    }
                    else
                        isProtected = false;
                        
                    if (! isProtected)
                        isKoCut = true;
                }
            }
            else if (tryLadder)
            {
                SgList<SgPoint> sequence;
                // AR same?
                // isProtected = (Ladder(liberty, sequence) >= SureValue);
                isProtected = Ladder(bd, liberty, bd.ToPlay(), true,
                                     &sequence);
                if (isProtected)
                    byLadder = true;
            }
            else // don't try ladder
                isProtected = false;
        }
        bd.Undo();
    }
    bd.SetToPlay(toPlay);
    return isProtected;
}

/** try to escape/capture prey block
    Possible return values:
    - SG_PASS if already escaped/captured
    - the point to play
    - SG_NULLMOVE in case of failure
*/
SgPoint GoLadderUtil::TryLadder(GoBoard& bd, SgPoint prey,
                                SgBlackWhite firstPlayer)
{
    SgList<SgPoint> sequence;
    bool isCaptured = Ladder(bd, prey, firstPlayer, true, &sequence);
    // if move is same color as prey, we want to escape
    // else we want to capture.
    SgPoint p;
    if (isCaptured != (firstPlayer == bd.GetStone(prey)))
        p = sequence.IsEmpty() ? SG_PASS : sequence.Top();
    else
        p = SG_NULLMOVE;
    return p;
}

//----------------------------------------------------------------------------

