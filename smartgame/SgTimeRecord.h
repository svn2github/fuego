//----------------------------------------------------------------------------
/** @file SgTimeRecord.h
    Time management.
*/
//----------------------------------------------------------------------------

#ifndef SG_TIMERECORD_H
#define SG_TIMERECORD_H

#include <iosfwd>
#include "SgBWArray.h"

class SgNode;

//----------------------------------------------------------------------------

/** Clock states. */
enum SgClockState {
    /** The clock is turned off; no time is counted for either player. */
    SG_CLOCK_OFF,

    /** The time is running for one of the players. */
    SG_CLOCK_RUNNING,

    /** The running clock was turned off by the computer; some
        actions may automatically cause it to turn back on. */
    SG_CLOCK_SUSPENDED
};

//----------------------------------------------------------------------------

/** A time record contains time-related information.
    How much time each player has left, how overtime is regulated, and how
    many moves there are left in this overtime period. This information is
    updated when replaying a game depending on time-related properties stored
    in the game tree.

    A time contest: dynamic time record of an ongoing game.
    The time left at each point in the game is stored as properties in the
    tree. The time stored is the time left when entering a new node; the
    time left while the clock is running at a node is not stored anywhere
    except in the TimeContest object. Thus leaving a node and returning to
    it later will restore the time left when first entering the node, not
    when leaving the node.

*/
class SgTimeRecord
{
public:
    explicit SgTimeRecord(int numMoves = 0, double period = 0,
                        double overhead = 0, bool loseOnTime = false);

    /** Constructor.
        'fOneMoveOnly' must be true for the second constructor (just to
        distinguish the two constructors). The second one creates a time
        record that can be used to set a specific time period for one move
        only.
    */
    SgTimeRecord(bool fOneMoveOnly, double timeForMove);

    ~SgTimeRecord() { }

    bool UseOvertime() const { return m_OTNumMoves > 0; }

    int OTNumMoves() const { return m_OTNumMoves; }

    double OTPeriod() const { return m_OTPeriod; }

    double Overhead() const { return m_overhead; }

    bool LoseOnTime() const { return m_loseOnTime; }

    void SetOTNumMoves(int numMoves) { m_OTNumMoves = numMoves; }

    void SetOTPeriod(double period) { m_OTPeriod = period; }

    void SetOverhead(double overhead) { m_overhead = overhead; }

    void SetLoseOnTime(bool lose) { m_loseOnTime = lose; }

    /** Returns the time stored at the given node or its most recent
        ancestor. */
    static SgBWArray<double> GetTimeFromTree(SgNode& node);

    /** Returns the number of moves left to play in overtime as determined by
        the given node or an ancestor with that property.
    */
    static SgBWArray<int> GetOTMovesFromTree(SgNode& node);

    /** Return the state of the clock: stopped, running, or suspended. */
    SgClockState GetClockState() const;

    bool ClockIsRunning() const { return m_clockIsOn; }

    /** Returns the current time left.
        Call UpdateTimeLeft first to get the newest information.
    */
    double TimeLeft(SgBlackWhite player) const;

    /** The number of moves left to play in this overtime period.
        This is zero if the game is in main time.
    */
    int MovesLeft(SgBlackWhite color) const { return m_movesLeft[color]; }

    /** Sets the time property at the given node for both players. */
    static void SetTimeInTree(SgNode& node, SgBWArray<double> time);

    void SetTimeLeft(SgBlackWhite color, double timeLeft) {
        m_timeLeft[color] = timeLeft; }

    /** Turn the clock on or off.
        @todo Set gUserAbort if the time is turned off, so that for example a
        search is aborted.
    */
    void TurnClockOn(bool turnOn);

    /** Set number of moves left to play in this overtime period.
        This is zero if the game is in main time.
    */
    void SetMovesLeft(SgBlackWhite color, int moves)
    {
        SG_ASSERT(moves >= 0); m_movesLeft[color] = moves;
    }

    /** Set the clock into suspended state. */
    void SuspendClock();

    /** Store the time left for the given player as a property in the tree. */
    void SetClock(SgNode& node, SgBlackWhite player, double time);

    /** Update the internal m_timeLeft.
        If the time left is negative, m_movesLeft is set if overtime is
        starting, or a "lost on time" is shown if m_loseOnTime is true.
    */
    void UpdateTimeLeft();

    /** Called by GoGame to react to user events.
        Called when a node is entered after moving in the tree or just to
        refresh the current state
        If the clock is on and the node is not m_atNode, the clock is
        suspended. m_player is set to the given player, and m_timeLeft is set
        to reflect the time left at that node.
    */
    void EnterNode(SgNode& node, SgBlackWhite player);

    /** Called by GoGame to react to user events.
        Called when a move was played on the board.
        If the clock was suspended, the clock is turned back on.
        Otherwise, if the clock was running, the time left is stored as
        properties in the new node.
    */
    void PlayedMove(SgNode& node, SgBlackWhite player);

private:
    /** How many moves to play in one overtime period.
        zero if there is no overtime.
    */
    int m_OTNumMoves;

    /** The length of one overtime period. */
    double m_OTPeriod;

    /** How much time to subtract for each move due to time
        used by the operator of the program (move entry, etc.).
    */
    double m_overhead;

    /** Whether to end the game when a player has negative time left. */
    bool m_loseOnTime;

    /** The player whose clock is running.
        Only relevant if the clock is running.
    */
    SgBlackWhite m_player;

    /** Whether the clock is turned on and running. */
    bool m_clockIsOn;

    /** Whether the clock is suspended.
        Note: m_clockIsOn and m_suspended cannot both be true.
    */
    bool m_suspended;

    /** The current node at which the time is running. */
    SgNode* m_atNode;

    /** The time left for both players.
        This is updated frequently, reflecting the time stored in fAtNode
        minus the time spent at that node.
    */
    SgBWArray<double> m_timeLeft;

    /**The number of moves left to play in this overtime period.
       This is zero if game is in main time.
    */
    SgBWArray<int> m_movesLeft;
                
    /** The time at which fTimeLeft was last updated. */
    double m_timeOfLastUpdate;
};

//----------------------------------------------------------------------------

/** Output SgTimeRecord to stream.
    @relatesalso SgTimeRecord
*/
std::ostream& operator<<(std::ostream& out, const SgTimeRecord& time);

/** Output SgClockState to stream. */
std::ostream& operator<<(std::ostream& out, SgClockState clockState);

//----------------------------------------------------------------------------

#endif // SG_TIMERECORD_H
