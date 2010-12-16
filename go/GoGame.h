//----------------------------------------------------------------------------
/** @file GoGame.h
    GoGameRecord class, play and replay moves in a game tree.

    GoGame implements the GoGameRecord class, which provides functions for
    playing and replaying moves in a game tree.
    Derived from it is the GoGame class, which allows two Player objects
    to play a game against each other. */
//----------------------------------------------------------------------------

#ifndef GO_GAME_H
#define GO_GAME_H

#include <string>
#include "GoBoard.h"
#include "GoBoardUpdater.h"
#include "GoBoardUtil.h"
#include "SgNode.h"
#include "SgPoint.h"
#include "SgTimeRecord.h"

class GoPlayer;
class GoPlayerMove;
class SgSearchStatistics;

//----------------------------------------------------------------------------

/** Game stored as a tree of moves.
    Contains a game tree, a board and a pointer to a current node. The
    current node is always a valid node of the tree and the board reflects
    the game state at the current node.
    @note This class is in the process of being refactored to provide
    encapsulation and to guarantee its class invariants. In the future, the
    tree, current node and board will only be modifiable through member
    functions of this class. Non-const accessors to these member variables
    will be removed. */
class GoGameRecord
{
public:
    /** Create a game record for replaying games on the given board. */
    explicit GoGameRecord(int boardSize = GO_DEFAULT_SIZE);

    virtual ~GoGameRecord();

    /** @name Virtual functions
        @note Still needed because subclass GoGame tracks changes to the
        current node by callback (virtual) functions. This functionality of
        GoGameRecord may be removed in the future, it is not recommended to
        create more such subclasses. */
    // @{

    /** Init from an existing game tree */
    virtual void InitFromRoot(SgNode* root);

    /** Delete the old game record and start with a fresh one.
        Init the board with the given parameters, and create a root node
        to start with. */
    virtual void Init(int size, const GoRules& rules);

    /** Hook for subclasses.
        Default implementation does nothing. */
    virtual void OnGoToNode(SgNode* dest);

    // @} // name


    /** Get the board associated with this game record. */
    const GoBoard& Board() const;

    /** Deprecated.
        @deprecated In the future, only this class should be allowed to modify
        the board to guarantee its class invariants (i.e. the current node
        is always a valid node of the tree and the board reflects the state
        in the current node. */
    GoBoard& NonConstBoard();

    /** Deprecated.
        @deprecated In the future, only this class should be allowed to modify
        the tree to guarantee its class invariants (i.e. the current node
        is always a valid node of the tree and the board reflects the state
        in the current node. */
    SgNode& NonConstRoot();

    /** Return the root of this tree. */
    const SgNode& Root() const;

    /** Add move to the game record.
        Add move as the next move at the current position.
        If a node with that move already exists, then don't add a new one.
        Return the node with that move.
        Also add any statistics from 'stat' and time left to that node. */
    SgNode* AddMove(SgMove move, SgBlackWhite player,
                    const SgSearchStatistics* stat = 0);

    /** Add a node with a comment that a player resigned.
        For informational purposes only, the resign node will not be made
        the current node. */
    SgNode* AddResignNode(SgBlackWhite player);

    /** Play to the given node.
        @c dest must be in this tree, or 0.
        Also updates the clock. */
    void GoToNode(SgNode* dest);

    /** Play to the next node in the given direction. */
    void GoInDirection(SgNode::Direction dir);

    /** Return whether there is a next node in the given direction. */
    bool CanGoInDirection(SgNode::Direction dir) const;

    /** Set the current player. */
    void SetToPlay(SgBlackWhite player);

    /** Return whether the game is finished. */
    bool EndOfGame() const;

    /** The time left in the game at the current position. */
    SgTimeRecord& Time();

    /** The time left in the game at the current position. */
    const SgTimeRecord& Time() const;

    /** Return the current position in the tree.
        @todo changed from protected to public because of getting
        the current time left. */
    SgNode* CurrentNode();

    /** Return the current position in the tree.
        @todo changed from protected to public because of getting
        the current time left. */
    const SgNode* CurrentNode() const;

    /** Return the move of the current node.
        Return NullMove if no current move. */
    SgMove CurrentMove() const;

    /** Get the number of moves since root or last node with setup
        properties. */
    int CurrentMoveNumber() const;


    /** @name Query or change game info properties */
    // @{

    /** Set komi property in the root node and delete all komi properties
        in the tree. */
    void SetKomiGlobal(GoKomi komi);

    /** Set time settings properties in the root node and delete all time
        settings properties in the tree. */
    void SetTimeSettingsGlobal(double mainSeconds, double overtimeSeconds,
                               int overtimeStones);

    /** Get the player name.
        Searches to nearest game info node on the path to the root node that
        has a player property. Returns an empty string if unknown. */
    std::string GetPlayerName(SgBlackWhite player) const;

    /** Set the player name at root node or most recent node with this
        property. */
    void UpdatePlayerName(SgBlackWhite player, const std::string& name);

    /** Set the date at root node or most recent node with this property. */
    void UpdateDate(const std::string& date);

    /** Get the game result.
        Searches to nearest game info node on the path to the root node that
        has a result property. Returns an empty string if unknown. */
    std::string GetResult() const;

    /** Set the result at root node or most recent node with this property. */
    void UpdateResult(const std::string& result);

    /** Get the game name.
        Searches to nearest game info node on the path to the root node that
        has a game name property. Returns an empty string if unknown. */
    std::string GetGameName() const;

    /** Set the game name at root node or most recent node with this
        property. */
    void UpdateGameName(const std::string& name);

    // @} // name

private:
    GoBoard m_board;

    /** The position in the current tree. */
    SgNode* m_current;

    GoBoardUpdater m_updater;

    /** A record of the clock settings and time left. */
    SgTimeRecord m_time;

    /** Comment display. */
    SgNode* m_oldCommentNode;

    /** Moves inserted into a line of play instead of added at the end. */
    int m_numMovesToInsert;

    /** Not implemented. */
    GoGameRecord(const GoGameRecord&);

    /** Not implemented. */
    GoGameRecord& operator=(const GoGameRecord&);

    /** Delete the tree and initialize the state associated with the position
        in the tree. */
    void DeleteTreeAndInitState();

    std::string GetGameInfoStringProp(SgPropID id) const;

    void InitHandicap(const GoRules& rules, SgNode* root);

    void UpdateGameInfoStringProp(SgPropID id, const std::string& value);
};

inline const GoBoard& GoGameRecord::Board() const
{
    return m_board;
}

inline GoBoard& GoGameRecord::NonConstBoard()
{
    return m_board;
}

inline SgNode& GoGameRecord::NonConstRoot()
{
    SG_ASSERT(m_current);
    return *m_current->Root();
}

inline const SgNode& GoGameRecord::Root() const
{
    SG_ASSERT(m_current);
    return *m_current->Root();
}

inline SgTimeRecord& GoGameRecord::Time()
{
    return m_time;
}

inline const SgTimeRecord& GoGameRecord::Time() const
{
    return m_time;
}

inline SgNode* GoGameRecord::CurrentNode()
{
    return m_current;
}

inline const SgNode* GoGameRecord::CurrentNode() const
{
    return m_current;
}

//----------------------------------------------------------------------------

/** Utility functions for GoGameRecord. */
namespace GoGameUtil
{
    /** Goto last node in main variation before move number.
        This function can be used for implementing the loadsgf GTP command.
        @param game (current node must be root)
        @param moveNumber move number (-1 means goto last node in main
        variation)
        @return false if moveNumber greater than moves in main variation */
    bool GotoBeforeMove(GoGameRecord* game, int moveNumber);
}

//----------------------------------------------------------------------------

/** Game record played between two players.
    A GoGame object handles the synchronization between the players
    and the board they're playing on, and keeps track of the time. */
class GoGame
    : public GoGameRecord
{
public:
    explicit GoGame(int boardSize = GO_DEFAULT_SIZE);

    virtual ~GoGame();

    /** Needed to avoid hiding of inherited virtual function by the
        other variants of GoGame::Init. */
    virtual void InitFromRoot(SgNode* root)
    {
        GoGameRecord::InitFromRoot(root);
    }

    /** Needed to avoid hiding of inherited virtual function by the
        other variants of GoGame::Init. */
    void Init(int size, const GoRules& rules);

    void Init(SgNode* root, bool fDeletePlayers);

    void Init(int size, const GoRules& rules, bool fDeletePlayers);

    void OnGoToNode(SgNode* dest);

    /** Turn the clock on or off.
        Call this method instead of Time().TurnClockOn
        so that computer player is started and stopped appropriately. */
    void TurnClockOn(bool turnOn);

    /** Return whether the clock is running (not stopped or suspended). */
    bool ClockIsRunning() const { return Time().ClockIsRunning(); }

    /** Set the player 'color' to the player algorithm 'player'.
        The old player of that color is stopped and deleted, and the new
        player is initialized to the current board state.
        A 0 player means user interaction. */
    void SetPlayer(SgBlackWhite color, GoPlayer* player);

    /** Delete the player 'color' (set it to interactive input).
        @note If black and white point to the same player, delete will be
        called only once, but both players will be set to null. */
    void DeletePlayer(SgBlackWhite color);

    /** Whether the human move at 'move' can be played at this point.
        The move must be legal, and either it's a human player to play
        or the clock is inactive. */
    bool CanPlayHumanMove(SgMove move, SgBlackWhite player) const;

    /** Adds a move to the game record and tells all other players to update
        their boards.
        Returns whether the move was added to the game record.
        @todo AR: to be replaced by HumanPlayer. */
    bool PlayHumanMove(SgMove move, SgBlackWhite player);

    void PlayComputerMove(const GoPlayerMove* playerMove);

    GoPlayer* Player(SgBlackWhite player);

    /** Compute and play one move using the given player. */
    GoPlayerMove PlayOneMove(SgBlackWhite playerColor);

private:
    /** Black and white player. */
    SgBWArray<GoPlayer*> m_player;

    /** Not implemented. */
    GoGame(const GoGame&);

    /** Not implemented. */
    GoGame& operator=(const GoGame&);

    void UpdatePlayer(SgBlackWhite color);

    void UpdatePlayers();
};

inline GoPlayer* GoGame::Player(SgBlackWhite player)
{
    return m_player[player];
}

//----------------------------------------------------------------------------

#endif // GO_GAME_H

