//----------------------------------------------------------------------------
/** @file GoUctGlobalSearchPlayer.h
    Class GoUctGlobalSearchPlayer.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_GLOBALSEARCHPLAYER_H
#define GOUCT_GLOBALSEARCHPLAYER_H

#include <boost/scoped_ptr.hpp>
#include <vector>
#include "GoBoard.h"
#include "GoPlayer.h"
#include "GoUctDefaultPlayoutPolicy.h"
#include "GoUctGlobalSearch.h"
#include "GoUctObjectWithSearch.h"
#include "GoUctRootFilter.h"
#include "GoTimeControl.h"
#include "SgPointArray.h"

template<typename T,int SIZE> class SgSList;

//----------------------------------------------------------------------------

/** What kind of SgPriorKnowledge to use in GoUctGlobalSearchPlayer */
enum GoUctGlobalSearchPrior
{
    /** Don't use any prior knowledge. */
    GOUCT_PRIORKNOWLEDGE_NONE,

    /** Use SgUctPriorKnowledgeEven */
    GOUCT_PRIORKNOWLEDGE_EVEN,

    /** Use GoUctDefaultPriorKnowledge */
    GOUCT_PRIORKNOWLEDGE_DEFAULT
};

//----------------------------------------------------------------------------

/** What search mode to use in GoUctGlobalSearchPlayer to select a move. */
enum GoUctGlobalSearchMode
{
    /** No search, use playout policy to select a move. */
    GOUCT_SEARCHMODE_PLAYOUTPOLICY,

    /** Use UCT search. */
    GOUCT_SEARCHMODE_UCT,

    /** Do a 1-ply MC search. */
    GOUCT_SEARCHMODE_ONEPLY
};

//----------------------------------------------------------------------------

/** Player using GoUctGlobalSearch. */
class GoUctGlobalSearchPlayer
    : public GoPlayer,
      public GoUctObjectWithSearch,
      public SgObjectWithDefaultTimeControl
{
public:
    /** Statistics collected by GoUctGlobalSearchPlayer. */
    struct Statistics
    {
        std::size_t m_nuGenMove;

        SgStatisticsExt<float,std::size_t> m_reuse;

        SgStatisticsExt<float,std::size_t> m_gamesPerSecond;

        Statistics();

        void Clear();

        /** Write in human readable format. */
        void Write(std::ostream& out) const;
    };

    GoUctDefaultPlayoutPolicyParam m_playoutPolicyParam;

    /** Constructor.
        @param bd The board.
    */
    GoUctGlobalSearchPlayer(GoBoard& bd);

    ~GoUctGlobalSearchPlayer();


    /** @name Virtual functions of GoBoardSynchronizer */
    // @{

    void OnBoardChange();

    // @} // @name


    /** @name Virtual functions of GoPlayer */
    // @{

    SgPoint GenMove(const SgTimeRecord& time, SgBlackWhite toPlay);

    std::string Name() const;

    void Ponder();

    void OnNewGame();

    // @} // @name


    /** @name Virtual functions of SgObjectWithDefaultTimeControl */
    // @{

    SgDefaultTimeControl& TimeControl();

    const SgDefaultTimeControl& TimeControl() const;

    // @} // @name


    /** @name Virtual functions of GoUctObjectWithSearch */
    // @{

    GoUctSearch& Search();

    const GoUctSearch& Search() const;

    // @} // @name


    /** @name Parameters */
    // @{

    /** Automatically adapt the search parameters optimized for the current
        board size.
        If on, GoUctGlobalSearch::SetDefaultParameters will automatically
        be called, if the board size changes.
    */
    bool AutoParam() const;

    /** See AutoParam() */
    void SetAutoParam(bool enable);

    /** Ignore time settings of the game.
        Ignore time record given to GenMove() and only obeys maximum
        number of games and maximum time. Default is true.
    */
    bool IgnoreClock() const;

    /** See IgnoreClock() */
    void SetIgnoreClock(bool enable);

    /** Maximum number of nodes in search tree.
        Nore: If ReuseSubtree() is enabled, then the player keeps a second
        tree of the same size to be used during extracting the subtree.
    */
    std::size_t MaxNodes() const;

    /** See MaxNodes()
        @param maxNodes Maximum number of nodes (>= 1)
    */
    void SetMaxNodes(std::size_t maxNodes);

    /** Limit on number of simulated games per move. */
    std::size_t MaxGames() const;

    /** See MaxGames() */
    void SetMaxGames(std::size_t maxGames);

    /** Timelimit per move in seconds.
        Default is 1e10
    */
    double MaxTime() const;

    void SetMaxTime(double maxTime);

    /** Think during the opponents time.
        For enabling pondering, ReuseSubtree() also has to be true.
        Pondering search will be terminated after MaxGames() or 60 min.
    */
    bool EnablePonder() const;

    /** See EnablePonder() */
    void SetEnablePonder(bool enable);

    /** Use the root filter. */
    bool UseRootFilter() const;

    /** See UseRootFilter() */
    void SetUseRootFilter(bool enable);

    /** Reuse subtree from last search.
        Reuses the subtree from the last search, if the current position is
        a number of regular game moves later than the position that the
        previous search corresponds to.
        @bug Does not work yet
    */
    bool ReuseSubtree() const;

    /** See ReuseSubtree() */
    void SetReuseSubtree(bool enable);

    /** Threshold for position value to resign.
        Default is 0.01.
    */
    double ResignThreshold() const;

    /** See ResignThreshold() */
    void SetResignThreshold(double threshold);

    /** See GoUctGlobalSearchMode */
    GoUctGlobalSearchMode SearchMode() const;

    /** See GoUctGlobalSearchMode */
    void SetSearchMode(GoUctGlobalSearchMode mode);

    /** See GoUctGlobalSearchPrior */
    GoUctGlobalSearchPrior PriorKnowledge() const;

    /** See GoUctGlobalSearchPrior */
    void SetPriorKnowledge(GoUctGlobalSearchPrior prior);

    // @} // @name


    /** @name Virtual functions of SgObjectWithDefaultTimeControl */
    // @{

    const Statistics& GetStatistics() const;

    void ClearStatistics();

    // @} // @name

    void StartSearch();

    /** Is the root position of the tree the position of the current node. */
    bool TreeValidForCurrentNode() const;

    /** Indicate that search was used outside the scope of the player.
        Informs the player that the search tree is no longer valid for the
        game position the player did the last search in.
    */
    void ClearTreeValidForNode();

    GoUctGlobalSearch& GlobalSearch();

    const GoUctGlobalSearch& GlobalSearch() const;

    /** Return the current root filter. */
    GoUctRootFilter& RootFilter();

    /** Set a new root filter.
        Deletes the old root filter and takes ownership of the new filter.
    */
    void SetRootFilter(GoUctRootFilter* filter);

private:
    /** See GoUctGlobalSearchMode */
    GoUctGlobalSearchMode m_searchMode;

    /** See AutoParam() */
    bool m_autoParam;

    /** See IgnoreClock() */
    bool m_ignoreClock;

    /** See EnablePonder() */
    bool m_enablePonder;

    /** See UseRootFilter() */
    bool m_useRootFilter;

    /** See ReuseSubtree() */
    bool m_reuseSubtree;

    /** See MaxTime() */
    double m_maxTime;

    /** See ResignThreshold() */
    double m_resignThreshold;

    /** Used in OnBoardChange() */
    int m_lastBoardSize;

    GoUctGlobalSearchPrior m_priorKnowledge;

    std::size_t m_maxGames;

    GoUctGlobalSearch m_search;

    /** Remember what position the current tree is valid for for extraction
        of a reusable subtree in a follow-up position.
        @bug The node address is not a unique ID for a position, since a node
        address can be reused after freeing a node and allocating a new node.
        Right now, this works under the assumption that
        - GoUctGlobalSearchPlayer is used in GoGtpEngine, which never frees
          nodes while in the same game
        - GoPlayer::OnNewGame() will be called whenever a new game tree is
          created (GoUctGlobalSearchPlayer::OnNewNewGame() clears
          m_treeValidForNode). Note that GoPlayer says that the player should
          not rely on OnNewGame() to be called, but GoGtpEngine calls it in
          the clear_board command.
        A new implementation should not make these assumptions (maybe store
        the whole position as an ID instead)
    */
    SgNode* m_treeValidForNode;

    GoTimeControl m_timeControl;

    Statistics m_statistics;

    boost::scoped_ptr<GoUctRootFilter> m_rootFilter;

    /** Initial tree if subtree of last search is reused.
        This variable is used only locally and only a member to avoid frequent
        allocation and deallocation.
    */
    SgUctTree m_initTree;

    SgMove GenMovePlayoutPolicy(SgBlackWhite toPlay);

    SgPoint DoSearch(SgBlackWhite toPlay, double maxTime,
                     bool isDuringPondering);

    void FindInitTree(SgBlackWhite toPlay, double maxTime);
};

inline bool GoUctGlobalSearchPlayer::AutoParam() const
{
    return m_autoParam;
}

inline void GoUctGlobalSearchPlayer::ClearTreeValidForNode()
{
    m_treeValidForNode = 0;
}

inline GoUctGlobalSearch& GoUctGlobalSearchPlayer::GlobalSearch()
{
    return m_search;
}

inline const GoUctGlobalSearch& GoUctGlobalSearchPlayer::GlobalSearch() const
{
    return m_search;
}

inline bool GoUctGlobalSearchPlayer::IgnoreClock() const
{
    return m_ignoreClock;
}

inline std::size_t GoUctGlobalSearchPlayer::MaxGames() const
{
    return m_maxGames;
}

inline std::size_t GoUctGlobalSearchPlayer::MaxNodes() const
{
    size_t maxNodes = m_search.MaxNodes();
    SG_ASSERT(! m_reuseSubtree || m_initTree.MaxNodes() == maxNodes);
    return maxNodes;
}

inline double GoUctGlobalSearchPlayer::MaxTime() const
{
    return m_maxTime;
}

inline bool GoUctGlobalSearchPlayer::EnablePonder() const
{
    return m_enablePonder;
}

inline GoUctGlobalSearchPrior GoUctGlobalSearchPlayer::PriorKnowledge() const
{
    return m_priorKnowledge;
}

inline bool GoUctGlobalSearchPlayer::UseRootFilter() const
{
    return m_useRootFilter;
}

inline double GoUctGlobalSearchPlayer::ResignThreshold() const
{
    return m_resignThreshold;
}

inline bool GoUctGlobalSearchPlayer::ReuseSubtree() const
{
    return m_reuseSubtree;
}

inline GoUctRootFilter& GoUctGlobalSearchPlayer::RootFilter()
{
    return *m_rootFilter;
}

inline GoUctGlobalSearchMode GoUctGlobalSearchPlayer::SearchMode() const
{
    return m_searchMode;
}

inline void GoUctGlobalSearchPlayer::SetAutoParam(bool enable)
{
    m_autoParam = enable;
}

inline void GoUctGlobalSearchPlayer::SetIgnoreClock(bool enable)
{
    m_ignoreClock = enable;
}

inline void GoUctGlobalSearchPlayer::SetMaxGames(std::size_t maxGames)
{
    m_maxGames = maxGames;
}

inline void GoUctGlobalSearchPlayer::SetMaxTime(double maxTime)
{
    m_maxTime = maxTime;
}

inline void GoUctGlobalSearchPlayer::SetEnablePonder(bool enable)
{
    m_enablePonder = enable;
}

inline void GoUctGlobalSearchPlayer::SetUseRootFilter(bool enable)
{
    m_useRootFilter = enable;
}

inline void GoUctGlobalSearchPlayer::SetResignThreshold(double threshold)
{
    m_resignThreshold = threshold;
}

inline void GoUctGlobalSearchPlayer::SetRootFilter(GoUctRootFilter* filter)
{
    m_rootFilter.reset(filter);
}

inline void GoUctGlobalSearchPlayer::SetSearchMode(GoUctGlobalSearchMode mode)
{
    m_searchMode = mode;
}

inline void GoUctGlobalSearchPlayer::StartSearch()
{
    m_search.StartSearch();
    m_treeValidForNode = CurrentNode();
}

inline bool GoUctGlobalSearchPlayer::TreeValidForCurrentNode() const
{
    return (m_treeValidForNode == CurrentNode());
}

//----------------------------------------------------------------------------

#endif // GOUCT_GLOBALSEARCHPLAYER_H
