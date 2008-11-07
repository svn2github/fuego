//----------------------------------------------------------------------------
/** @file GoUctPlayer.h
    Class GoUctPlayer.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_PLAYER_H
#define GOUCT_PLAYER_H

#include <boost/scoped_ptr.hpp>
#include <vector>
#include "GoBoard.h"
#include "GoPlayer.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctGlobalSearch.h"
#include "GoUctObjectWithSearch.h"
#include "GoUctRootFilter.h"
#include "GoTimeControl.h"
#include "SgPointArray.h"

template<typename T,int SIZE> class SgSList;

//----------------------------------------------------------------------------

/** What kind of SgPriorKnowledge to use in GoUctPlayer */
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

/** What search mode to use in GoUctPlayer to select a move. */
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
class GoUctPlayer
    : public GoPlayer,
      public GoUctObjectWithSearch,
      public SgObjectWithDefaultTimeControl
{
public:
    /** Statistics collected by GoUctPlayer. */
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

    GoUctPlayoutPolicyParam m_playoutPolicyParam;

    /** Constructor.
        @param bd The board.
    */
    GoUctPlayer(GoBoard& bd);

    ~GoUctPlayer();


    /** @name Virtual functions of GoBoardSynchronizer */
    // @{

    void OnBoardChange();

    // @} // @name


    /** @name Virtual functions of GoPlayer */
    // @{

    SgPoint GenMove(const SgTimeRecord& time, SgBlackWhite toPlay);

    std::string Name() const;

    void Ponder();

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

    /** Pass early.
        Aborts search early, if value is above 1 - ResignThreshold(), and
        performs a second search to see, if it is still a win and all points
        are safe (using territory statistics) after playing a pass. If this
        is true, it plays a pass.
    */
    bool EarlyPass() const;

    /** See EarlyPass() */
    void SetEarlyPass(bool enable);

    /** Ignore time settings of the game.
        Ignore time record given to GenMove() and only obeys maximum
        number of games and maximum time. Default is true.
    */
    bool IgnoreClock() const;

    /** See IgnoreClock() */
    void SetIgnoreClock(bool enable);

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

    /** Minimum number of simulations to check for resign.
        This minimum number of simulations is also required to apply the
        early pass check (see EarlyPass()).
        Default is 3000.
    */
    std::size_t ResignMinGames() const;

    /** See ResignMinGames()     */
    void SetResignMinGames(std::size_t n);

    /** Use the root filter. */
    bool UseRootFilter() const;

    /** See UseRootFilter() */
    void SetUseRootFilter(bool enable);

    /** Reuse subtree from last search.
        Reuses the subtree from the last search, if the current position is
        a number of regular game moves later than the position that the
        previous search corresponds to.
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

    GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
        GlobalSearch();

    const GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
        GlobalSearch() const;

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

    /** See EarlyPass() */
    bool m_earlyPass;

    /** See MaxTime() */
    double m_maxTime;

    /** See ResignThreshold() */
    double m_resignThreshold;

    /** Used in OnBoardChange() */
    int m_lastBoardSize;

    GoUctGlobalSearchPrior m_priorKnowledge;

    std::size_t m_maxGames;

    std::size_t m_resignMinGames;

    GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> > m_search;

    GoTimeControl m_timeControl;

    Statistics m_statistics;

    boost::scoped_ptr<GoUctRootFilter> m_rootFilter;

    /** Playout policy used if search mode is GOUCT_SEARCHMODE_PLAYOUTPOLICY.
    */
    boost::scoped_ptr<GoUctPlayoutPolicy<GoBoard> > m_playoutPolicy;

    SgMove GenMovePlayoutPolicy(SgBlackWhite toPlay);

    bool DoEarlyPassSearch(size_t maxGames, double maxTime, SgPoint& move);

    SgPoint DoSearch(SgBlackWhite toPlay, double maxTime,
                     bool isDuringPondering);

    void FindInitTree(SgUctTree& initTree, SgBlackWhite toPlay,
                      double maxTime);

    bool VerifyNeutralMove(size_t maxGames, double maxTime, SgPoint move);
};

inline bool GoUctPlayer::AutoParam() const
{
    return m_autoParam;
}

inline GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                         GoUctPlayoutPolicyFactory<GoUctBoard> >&
GoUctPlayer::GlobalSearch()
{
    return m_search;
}

inline const GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                               GoUctPlayoutPolicyFactory<GoUctBoard> >&
GoUctPlayer::GlobalSearch() const
{
    return m_search;
}

inline bool GoUctPlayer::EarlyPass() const
{
    return m_earlyPass;
}

inline bool GoUctPlayer::EnablePonder() const
{
    return m_enablePonder;
}

inline bool GoUctPlayer::IgnoreClock() const
{
    return m_ignoreClock;
}

inline std::size_t GoUctPlayer::MaxGames() const
{
    return m_maxGames;
}

inline double GoUctPlayer::MaxTime() const
{
    return m_maxTime;
}

inline GoUctGlobalSearchPrior GoUctPlayer::PriorKnowledge() const
{
    return m_priorKnowledge;
}

inline bool GoUctPlayer::UseRootFilter() const
{
    return m_useRootFilter;
}

inline std::size_t GoUctPlayer::ResignMinGames() const
{
    return m_resignMinGames;
}

inline double GoUctPlayer::ResignThreshold() const
{
    return m_resignThreshold;
}

inline bool GoUctPlayer::ReuseSubtree() const
{
    return m_reuseSubtree;
}

inline GoUctRootFilter& GoUctPlayer::RootFilter()
{
    return *m_rootFilter;
}

inline GoUctGlobalSearchMode GoUctPlayer::SearchMode() const
{
    return m_searchMode;
}

inline void GoUctPlayer::SetAutoParam(bool enable)
{
    m_autoParam = enable;
}

inline void GoUctPlayer::SetEarlyPass(bool enable)
{
    m_earlyPass = enable;
}

inline void GoUctPlayer::SetEnablePonder(bool enable)
{
    m_enablePonder = enable;
}

inline void GoUctPlayer::SetIgnoreClock(bool enable)
{
    m_ignoreClock = enable;
}

inline void GoUctPlayer::SetMaxGames(std::size_t maxGames)
{
    m_maxGames = maxGames;
}

inline void GoUctPlayer::SetMaxTime(double maxTime)
{
    m_maxTime = maxTime;
}

inline void GoUctPlayer::SetUseRootFilter(bool enable)
{
    m_useRootFilter = enable;
}

inline void GoUctPlayer::SetResignMinGames(std::size_t n)
{
    m_resignMinGames = n;
}

inline void GoUctPlayer::SetResignThreshold(double threshold)
{
    m_resignThreshold = threshold;
}

inline void GoUctPlayer::SetRootFilter(GoUctRootFilter* filter)
{
    m_rootFilter.reset(filter);
}

inline void GoUctPlayer::SetSearchMode(GoUctGlobalSearchMode mode)
{
    m_searchMode = mode;
}

//----------------------------------------------------------------------------

#endif // GOUCT_PLAYER_H
