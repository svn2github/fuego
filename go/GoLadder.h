//----------------------------------------------------------------------------
/** @file GoLadder.h
    Fast ladder algorithm, computes ladders and snapbacks.

    @note
    Will become obsolete when class GoBoard is fast enough for computing
    ladders.
*/
//----------------------------------------------------------------------------

#ifndef GO_LADDER_H
#define GO_LADDER_H

#include "SgBoardColor.h"
#include "SgList.h"
#include "SgPoint.h"

class GoBoard;

//----------------------------------------------------------------------------

namespace GoLadder {

enum Status
{
    /** Don't know anything about the status of this block. */
    Unknown,

    /** Definitely captured, regardless of who plays first. */
    Captured,

    /** Captured if first player can win ko. */
    CapturedKo,

    /** Can be captured, not sure whether it can escape. */
    CanBeCaptured,

    /** Capture or escape depends on who plays first. */
    Unsettled,

    /** Status depends on who plays first and who wins ko. */
    UnsettledKo,

    /** Can escape, not sure whether it can be captured. */
    CanEscape,

    /** Definitely escaped, regardless of who plays first. */
    Escaped,

    /** Escaped if first player can win ko. */
    EscapedKo
};

/** Return whether or not the block at 'prey' can be captured in a ladder when
    'toPlay' plays first.
    True means capture, false means escape. If 'sequence' is not 0, return a
    sequence of moves to capture or escape (need not be the optimal sequence).
    Return an empty sequence if the prey is already captured or has escaped,
    without needing to play a move.
    If the prey can be temporarily removed from the board but can capture
    back immediately (snapback), return that the prey cannot be captured.
*/
bool Ladder(GoBoard& board, SgPoint prey, SgBlackWhite toPlay,
            bool fTwoLibIsEscape = false, SgList<SgPoint>* sequence = 0);

/** Return whether the block at 'prey' is captured, escaped, or unsettled
    with regards to capture in a ladder.
    If it is unsettled, set '*toCapture' and '*toEscape' (if not 0) to the
    capturing/escaping move to play.
    Otherwise, leave '*toCapture' and '*toEscape' unchanged. The point at
    'prey' must be occupied. 
*/
Status LadderStatus(GoBoard& bd, SgPoint prey, bool fTwoLibIsEscape = false,
                    SgPoint* toCapture = 0, SgPoint* toEscape = 0);

/** Check if this is a chain connection point, or a ko cut point.
    Try to play there as opponent, then check:
    - Suicide
    - self removal, self atari
    - can be captured by ladder?
    - is it a Ko?
*/
bool IsProtectedLiberty(const GoBoard& bd, SgPoint liberty, SgBlackWhite col,
                        bool& byLadder, bool& isKoCut, bool tryLadder = true);

/** Simple form, calls the complex form and ignores bool results */
bool IsProtectedLiberty(const GoBoard& bd, SgPoint liberty, SgBlackWhite col);

SgPoint TryLadder(GoBoard& bd, SgPoint prey, SgBlackWhite firstPlayer);

} // namespace GoLadder

//----------------------------------------------------------------------------

#endif // GO_LADDER_H

