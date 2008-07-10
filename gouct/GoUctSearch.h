//----------------------------------------------------------------------------
/** @file GoUctSearch.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_SEARCH_H
#define GOUCT_SEARCH_H

#include <iosfwd>
#include "GoBoard.h"
#include "GoBoardSynchronizer.h"
#include "GoUctBoard.h"
#include "SgUctSearch.h"
#include "SgBlackWhite.h"
#include "SgStatistics.h"

class SgNode;

//----------------------------------------------------------------------------

/** Thread state for GoUctSearch.
*/
class GoUctState
    : public SgUctThreadState
{
public:
    /** Constructor.
        @param threadId The number of the thread. Needed for passing to
        constructor of SgUctThreadState.
        @param bd The board with the current position. The state has is own
        board that will be synchronized with the currently searched position
        in StartSearch()
    */
    GoUctState(std::size_t threadId, const GoBoard& bd);

    /** @name Pure virtual functions of SgUctThreadState */
    // @{

    void StartSearch();

    /** Implementation of SgUctSearch::Execute */
    void Execute(SgMove move);

    /** Implementation of SgUctSearch::ExecutePlayout */
    void ExecutePlayout(SgMove move);

    void TakeBackInTree(std::size_t nuMoves);

    void TakeBackPlayout(std::size_t nuMoves);

    SgBlackWhite ToPlay() const;

    // @} // @name


    /** @name Virtual functions of SgUctThreadState */
    // @{

    void GameStart();

    void StartPlayout();

    void StartPlayouts();

    std::size_t GetSignature(SgMove mv) const;

    // @} // @name

    /** Board used during in-tree phase. */
    const GoBoard& Board() const;

    /** Board used during playout phase. */
    const GoUctBoard& UctBoard() const;

    bool IsInPlayout() const;

    void Dump(std::ostream& out) const;

private:
    /** Assertion handler to dump the state of a GoUctState. */
    class AssertionHandler
        : public SgAssertionHandler
    {
    public:
        AssertionHandler(const GoUctState& state);

        void Run();

    private:
        const GoUctState& m_state;
    };

    AssertionHandler m_assertionHandler;

    /** Board used for in-tree phase. */
    GoBoard m_bd;

    /** Board used for playout phase. */
    GoUctBoard m_uctBd;

    GoBoardSynchronizer m_synchronizer;

    bool m_isInPlayout;
};

inline const GoBoard& GoUctState::Board() const
{
    return m_bd;
}

inline bool GoUctState::IsInPlayout() const
{
    return m_isInPlayout;
}

inline const GoUctBoard& GoUctState::UctBoard() const
{
    return m_uctBd;
}

//----------------------------------------------------------------------------

/** Live-gfx mode used in GoUctSearch.
    @see GoUctSearch::LiveGfx.
*/
enum GoUctLiveGfxMode
{
    /** Don't print live-gfx information. */
    GOUCT_LIVEGFXMODE_NONE,

    /** Print values and counts as in GoUctUtil::GoGuiGfx() */
    GOUCT_LIVEGFXMODE_COUNTS,

    /** Print sequence as in GoUctUtil::GoGuiGfx2() */
    GOUCT_LIVEGFXMODE_SEQUENCE
};

//----------------------------------------------------------------------------

/** Base class for UCT searches in Go. */
class GoUctSearch
    : public SgUctSearch
{
public:
    /** Constructor.
        @param bd The board
        @param factory
    */
    GoUctSearch(GoBoard& bd, SgUctThreadStateFactory* factory);

    ~GoUctSearch();


    /** @name Pure virtual functions of SgUctSearch */
    // @{

    std::string MoveString(SgMove move) const;

    /** Color to play at the root node of the last search. */
    SgBlackWhite ToPlay() const;

    // @} // @name


    /** @name Virtual functions of SgUctSearch */
    // @{

    void OnSearchIteration(std::size_t gameNumber, int threadId,
                           const SgUctGameInfo& info);

    void OnStartSearch();

    std::size_t SignatureRange() const;

    // @} // @name


    /** @name Go-specific functions */
    // @{

    GoBoard& Board();

    const GoBoard& Board() const;

    /** See SetKeepGames()
        @throws SgException if KeepGames() was false at last invocation of
        StartSearch()
    */
    void SaveGames(const std::string& fileName) const;

    /** See GoUctUtil::SaveTree() */
    void SaveTree(std::ostream& out, int maxDepth = -1) const;

    /** Set initial color to play. */
    void SetToPlay(SgBlackWhite toPlay);

    std::size_t GetSignature(SgPoint mv) const;

    // @} // @name


    /** @name Go-specific parameters */
    // @{

    /** Keep a SGF tree of all games.
        This is reset in OnStartSearch() and can be saved with SaveGames().
    */
    bool KeepGames() const;

    /** See KeepGames() */
    void SetKeepGames(bool enable);

    /** Output live graphics commands for GoGui.
        If enabled, LiveGfx commands for GoGui are outputted to the debug
        stream every n games. Note that GoUctUtil::GoGuiGfx() outputs values
        as influence data and assumes values in [0:1].
        @see GoUctLiveGfxMode, LiveGfxInterval()
    */
    GoUctLiveGfxMode LiveGfx() const;

    /** See LiveGfx() */
    void SetLiveGfx(GoUctLiveGfxMode mode);

    /** Interval for outputting of live graphics commands for GoGui.
        Default is every 5000 games.
        @see LiveGfx()
    */
    int LiveGfxInterval() const;

    /** See LiveGfxInterval() */
    void SetLiveGfxInterval(int interval);

    // @} // @name

private:
    /** See SetKeepGames() */
    bool m_keepGames;

    /** See SetLiveGfx() */
    bool m_liveGfx;

    /** See SetLiveGfxInterval() */
    int m_liveGfxInterval;

    /** Board move number at root node of search. */
    int m_initialMoveNumber;

    /** Color to play.
        Does not use GoBoard::ToPlay(), because the color to play at the
        root node of the search could be needed after the board has
        changed (e.g. when saving the tree after a move was generated and
        played on the board).
    */
    SgBlackWhite m_toPlay;

    /** Stones of position that the current search tree belongs to.
        Needed such that the tree can be saved even if the board has
        changed (e.g. after a move was generated and played).
    */
    SgBWSet m_stones;

    GoBoard& m_bd;

    /** See SetKeepGames() */
    SgNode* m_root;

    GoUctLiveGfxMode m_liveGfxMode;

    /** Not implemented */
    GoUctSearch(const GoUctSearch& search);

    /** Not implemented */
    GoUctSearch& operator=(const GoUctSearch& search);
};

inline GoBoard& GoUctSearch::Board()
{
    return m_bd;
}

inline const GoBoard& GoUctSearch::Board() const
{
    return m_bd;
}

inline bool GoUctSearch::KeepGames() const
{
    return m_keepGames;
}

inline GoUctLiveGfxMode GoUctSearch::LiveGfx() const
{
    return m_liveGfxMode;
}

inline int GoUctSearch::LiveGfxInterval() const
{
    return m_liveGfxInterval;
}

inline void GoUctSearch::SetKeepGames(bool enable)
{
    m_keepGames = enable;
}

inline void GoUctSearch::SetLiveGfx(GoUctLiveGfxMode mode)
{
    m_liveGfxMode = mode;
}

inline void GoUctSearch::SetLiveGfxInterval(int interval)
{
    SG_ASSERT(interval > 0);
    m_liveGfxInterval = interval;
}

inline void GoUctSearch::SetToPlay(SgBlackWhite toPlay)
{
    m_toPlay = toPlay;
    m_bd.SetToPlay(toPlay);
}

//----------------------------------------------------------------------------

/** Utility functions for users of GoUctSearch. */
namespace GoUctSearchUtil
{
    /** Checks if move is a bad pass move, if Tromp-Taylor rules are used,
        and tries to fix it.
        The rationale for this function is, that a UCT search sometimes plays
        an early pass move, which allows the opponent to end the game by also
        playing pass, and might lose the game, because all stones on the board
        are considered alive under Tromp-Taylor rules. This happens mostly
        if the UCT search uses RAVE, because the pass move by the opponent
        will not be explored for a while, because a pass move will get a low
        RAVE value. Fortunately, this can be fixed after the search: if the
        Tromp-Taylor score of the current position is worse that the value
        of the root node of the search, then we extract the second best move
        from the search tree.
        @param move The move returned by the search
        @param search The search, containing the tree with the last search
        and other needed information (like board, rules,
        SgUctSearch::ToPlay(), SgUctSearch::UseCount())
        @return The second best move, if the above applies and such a move
        exists.
    */
    SgPoint TrompTaylorPassCheck(SgPoint move, const GoUctSearch& search);
}

//----------------------------------------------------------------------------

#endif // GOUCT_SEARCH_H
