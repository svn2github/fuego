//----------------------------------------------------------------------------
/** @file SgUctSearch.h
    Class SgUctSearch and helper classes.
*/
//----------------------------------------------------------------------------

#ifndef SG_UCTSEARCH_H
#define SG_UCTSEARCH_H

#include <fstream>
#include <vector>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgTimer.h"
#include "SgUctBiasTermPrecomp.h"
#include "SgUctTree.h"

#define SG_UCTFASTLOG 1
#if SG_UCTFASTLOG
#include "SgFastLog.h"
#endif

//----------------------------------------------------------------------------

/** @defgroup sguctgroup Monte Carlo tree search
    Game-independent Monte Carlo tree search using UCT.

    The main class SgUctSearch keeps a tree with statistics for each node
    visited more than a certain number of times, and then continues with
    random playout (not necessarily uniform random).
    Within the tree, the move with the highest upper confidence bound is
    chosen according to the basic UCT formula:
    @f[ \bar{X}_j + c \sqrt{\frac{\log{n}}{T_j(n)}} @f]
    with:
    - @f$ j @f$ the move index
    - @f$ X_{j,\gamma} @f$ reward for move @f$ j @f$ at sample @f$ \gamma @f$
    - @f$ n @f$ number of times the father node was visited
    - @f$ T_j(n) @f$ number of times the move has been played
    - @f$ c @f$ an appropriate constant

    References:
    - Kocsis, Szepesvari:
      <a href="http://zaphod.aml.sztaki.hu/papers/ecml06.pdf">
      Bandit based Monte-Carlo Planning</a>
    - Auer, Cesa-Bianchi, Fischer:
      <a href="http://homes.dsi.unimi.it/~cesabian/Pubblicazioni/ml-02.pdf">
      Finite-time Analysis of the Multiarmed Bandit Problem</a>
    - Gelly, Wang, Munos, Teytaud:
      <a href="http://hal.inria.fr/docs/00/12/15/16/PDF/RR-6062.pdf">
      Modification of UCT with patterns in Monte-Carlo Go</a>
    - Silver, Gelly:
      <a href=
      "http://imls.engr.oregonstate.edu/www/htdocs/proceedings/icml2007/papers/387.pdf">
      Combining Online and Offline Knowledge in UCT</a>

    @see
    - @ref sguctsearchlockfree
    - @ref sguctsearchweights
*/

/** @page sguctsearchlockfree Lock-free usage of SgUctSearch
    SgUctSearch can be used in a lock-free way for improved multi-threaded
    performance. Then, the threads access the common data structures (like
    the tree) without any locking. Lock-free usage is enabled with
    SgUctSearch::SetLockFree(true).

    It depends on the memory model of the platform, if lock-free usage
    works. It assumes that writes of some basic types (size_t, int, float,
    pointers) are atomic and that the CPU does not reorder certain
    instructions. In particular, SgUctNode::SetNuChildren() is always called
    after the children were created, such that SgUctNode::HasChildren()
    returns only true, if the children are ready to use. Because of the
    platform-dependency, lock-free is not enabled by default.
    Statistics collected during the search may not be accurate in
    lock-free usage, because counts and mean values may be manipulated
    concurrently.

    In particular, the IA-32 and Intel-64 architecture guarantees these
    assumptions. Writes of the used data types are atomic (if properly
    aligned) and writes by one CPU are seen in the same order by other CPUs
    (the critical data is declared as volatile to avoid that the compiler
    reorders writes for optimization purposes). In addition, the architecture
    synchronizes CPU caches after writes. See
    <a href="http://download.intel.com/design/processor/manuals/253668.pdf">
    Intel 64 and IA-32 Architectures Software Developer's Manual</a>, chapter
    7.1 (Locked Atomic Operations) and 7.2 (Memory Ordering).
*/

/** @page sguctsearchweights Estimator weights in SgUctSearch
    The weights of the estimators (move value, RAVE value) are chosen by
    assuming that the estimators are uncorrelated and modeling the mean
    squared error of estimator @f$ i @f$ by a function that depends on the
    number of samples and parameter constants, which represent the variance
    and bias of the estimator and need to be determined experimentally:

    @f[
    w_i = \frac{1}{Z} \frac{1}{{\rm MSE}_i}
    \qquad Z = \sum_i \frac{1}{{\rm MSE}_i}
    \qquad {\rm MSE}_i = \frac{c_{\rm variance}}{N_i} + c_{\rm bias}^2
    @f]

    Note that this formula is nearly equivalent to the formula suggested
    by David Silver on the Computer Go mailing list for the case of two
    estimators (move value and RAVE value) and used in newer versions of MoGo.
    However, MoGo uses the measured variance of the current RAVE value (for
    both the move weight and RAVE weight) instead variance parameter
    constants.

    The formula is then reformulated to use different constants that describe
    the initial steepness and final asymptotic value of the unnormalized
    weight:

    @f[
    Z w_i =
    \frac{c_{\rm initial}}
         {\frac{1}{N} + \frac{c_{\rm initial}}{c_{\rm final}}}
    @f]

    with:
    - @f$ N @f$ sample count of the estimator
    - @f$ c_{\rm initial} @f$ Initial weight parameter; this is they weight if
      @f$ N = 1 @f$ and @f$ c_{\rm final} \gg c_{\rm initial} @f$
    - @f$ c_{\rm final} @f$ Final weight parameter; this is they weight if
      @f$ N \rightarrow \infty @f$

    For the move value, @f$ c_{\rm initial} = c_{\rm final} = 0 @f$, so the
    weight is simply @f$ N_{\rm move} @f$.
    If no estimator has a sample count yet, the first-play-urgency parameter
    is used for the value estimate.
*/

//----------------------------------------------------------------------------

typedef SgStatistics<float,std::size_t> SgUctStatistics;

typedef SgStatisticsExt<float,std::size_t> SgUctStatisticsExt;

//----------------------------------------------------------------------------

/** Game result, sequence and nodes of one Monte-Carlo game in SgUctSearch.
    @ingroup sguctgroup
*/
struct SgUctGameInfo
{
    /** The game result of the playout(s).
        The result is from the view of the player at the root.
    */
    std::vector<float> m_eval;

    /** The sequence of the in-tree phase. */
    std::vector<SgMove> m_inTreeSequence;

    /** The sequence of the playout(s).
        For convenient usage, they also include the moves from
        m_inTreeSequence, even if they are the same for each playout.
    */
    std::vector<std::vector<SgMove> > m_sequence;

    /** Was the playout aborted due to maxGameLength (stored for each
        playout).
    */
    std::vector<bool> m_aborted;

    /** Nodes visited in the in-tree phase. */
    std::vector<const SgUctNode*> m_nodes;

    /** Flag to skip RAVE update for moves of the playout(s).
        For convenient usage, the index corresponds to the move number from
        the root position on, even if the flag is currently only used for
        moves in the playout phase, so the flag is false for all moves in the
        in-tree phase.
    */
    std::vector<std::vector<bool> > m_skipRaveUpdate;

    void Clear(std::size_t numberPlayouts);
};

//----------------------------------------------------------------------------

/** Provides an initialization of unknown states.
    @ingroup sguctgroup
*/
class SgUctPriorKnowledge
{
public:
    virtual ~SgUctPriorKnowledge();

    /** Called in each position before any calls to InitializeMove().
        @param[out] deepenTree Used for selective deepening of the tree.
        Initialized with @c false by the caller. Set to @c true, if the
        in-tree phase should continue for another move.
    */
    virtual void ProcessPosition(bool& deepenTree) = 0;

    /** Initialize the value for a move in the current state.
        @param move The move to initialize.
        @param[out] value The initial value for the state.
        @param[out] count The initial count for the state.
    */
    virtual void InitializeMove(SgMove move, float& value,
                                std::size_t& count) = 0;
};

//----------------------------------------------------------------------------

class SgUctThreadState;

/** Create SgUctPriorKnowledge instances.
    Needs one per thread.
    @ingroup sguctgroup
*/
class SgUctPriorKnowledgeFactory
{
public:
    virtual ~SgUctPriorKnowledgeFactory();

    virtual SgUctPriorKnowledge* Create(SgUctThreadState& state) = 0;
};

//----------------------------------------------------------------------------

/** Move selection strategy after search is finished.
    @ingroup sguctgroup
*/
enum SgUctMoveSelect
{
    /** Select move with highest mean value. */
    SG_UCTMOVESELECT_VALUE,

    /** Select move with highest count. */
    SG_UCTMOVESELECT_COUNT,

    /** Use UCT bound (or combined bound if RAVE is enabled). */
    SG_UCTMOVESELECT_BOUND,

    /** Use the weighted sum of UCT and RAVE value (but without
        bias terms).
    */
    SG_UCTMOVESELECT_ESTIMATE
};

//----------------------------------------------------------------------------

/** Base class for the thread state.
    Subclasses must be thread-safe, it must be possible to use different
    instances of this class in different threads (after construction, the
    constructor does not need to be thread safe). Beware not to use classes
    that are not thread-safe, because they use global variables
    (e.g. SgRandom::Global(), SgList)
    @note Technically it is possible to use a non-thread safe implementation
    of subclasses, as long as the search is run with only one thread.
    @ingroup sguctgroup
*/
class SgUctThreadState
{
public:
    /** Number of the thread between 0 and SgUctSearch::NumberThreads() - 1 */
    const std::size_t m_threadId;

    SgUctGameInfo m_gameInfo;

    /** Local variable for SgUctSearch::UpdateRaveValues().
        Reused for efficiency. Stores the first time a move was played
        by the color to play at the root position (move is used as an index,
        do m_moveRange must be > 0); numeric_limits<size_t>::max(), if the
        move was not played.
    */
    boost::scoped_array<std::size_t> m_firstPlay;

    /** Local variable for SgUctSearch::UpdateRaveValues().
        Like m_firstPlayToPlay, but for opponent color.
    */
    boost::scoped_array<std::size_t> m_firstPlayOpp;

    /** Local variable for SgUctSearch::PlayInTree().
        Reused for efficiency.
    */
    std::vector<SgMove> m_moves;

    std::auto_ptr<SgUctPriorKnowledge> m_priorKnowledge;

    SgUctThreadState(size_t threadId, int moveRange = 0);

    virtual ~SgUctThreadState();

    /** @name Pure virtual functions */
    // @{

    /** Evaluate end-of-game position.
        Will only be called if GenerateAllMoves() or GeneratePlayoutMove()
        returns no moves. Should return larger values if position is better
        for the player to move.
    */
    virtual float Evaluate() = 0;

    /** Execute a move.
        @param move The move
     */
    virtual void Execute(SgMove move) = 0;

    /** Execute a move in the playout phase.
        For optimization if the subclass uses uses a different game state
        representation in the playout phase. Otherwise the function can be
        implemented in the subclass by simply calling Execute().
        @param move The move
     */
    virtual void ExecutePlayout(SgMove move) = 0;

    /** Generate moves.
        Moves will be explored in the order of the returned list.
        @param[out] moves The generated moves or empty list at end of game
    */
    virtual void GenerateAllMoves(std::vector<SgMove>& moves) = 0;

    /** Generate random move.
        Generate a random move in the play-out phase (outside the UCT tree).
        @param[out] skipRaveUpdate This value should be set to true, if the
        move should be excluded from RAVE updates. Otherwise it can be
        ignored.
        @return The move or SG_NULLMOVE at the end of the game.
    */
    virtual SgMove GeneratePlayoutMove(bool& skipRaveUpdate) = 0;

    /** Start search.
        This function should do any necessary preparations for playing games
        in the thread, like initializing the thread's copy of the game state
        from the global game state. The function does not have to be
        thread-safe.
    */
    virtual void StartSearch() = 0;

    /** Take back moves played in the in-tree phase. */
    virtual void TakeBackInTree(std::size_t nuMoves) = 0;

    /** Take back moves played in the playout phase.
        The search engine does not assume that the moves are really taken back
        after this function is called. If the subclass implements the playout
        in s separate state, which is initialized in StartPlayout() and does
        not support undo, the implementation of this function can be left
        empty in the subclass.
    */
    virtual void TakeBackPlayout(std::size_t nuMoves) = 0;

    // @} // name


    /** @name Virtual functions */
    // @{

    /** Function that will be called by PlayGame() before the game.
        Default implementation does nothing.
    */
    virtual void GameStart();

    /** Function that will be called at the beginning of the playout phase.
        Will be called only once (not once per playout!). Can be used for
        example to save some state of the current position for more efficient
        implementation of TakeBackPlayout().
        Default implementation does nothing.
    */
    virtual void StartPlayouts();

    /** Function that will be called at the beginning of each playout.
        Default implementation does nothing.
    */
    virtual void StartPlayout();

    /** Function that will be called after each playout.
        Default implementation does nothing.
    */
    virtual void EndPlayout();

    // @} // name
};

//----------------------------------------------------------------------------

class SgUctSearch;

/** Create game specific thread state.
    @see SgUctThreadState
    @ingroup sguctgroup
*/
class SgUctThreadStateFactory
{
public:
    virtual ~SgUctThreadStateFactory();

    virtual SgUctThreadState* Create(std::size_t threadId,
                                     const SgUctSearch& search) = 0;
};

//----------------------------------------------------------------------------

/** Statistics of the last search performed by SgUctSearch. */
struct SgUctSearchStat
{
    double m_time;

    /** Games per second.
        Useful values only if search time is higher than resolution of
        SgTime::Get().
    */
    double m_gamesPerSecond;

    SgUctStatisticsExt m_gameLength;

    SgUctStatisticsExt m_movesInTree;

    SgUctStatistics m_aborted;

    void Clear();

    void Write(std::ostream& out) const;
};

//----------------------------------------------------------------------------

/** Monte Carlo tree search using UCT.
    The evaluation function is assumed to be in <code>[0..1]</code> and
    inverted with <code>1 - eval</code>.
    @ingroup sguctgroup
*/
class SgUctSearch
{
public:
    static float InverseEval(float eval);

    /** Constructor.
        @param threadStateFactory The tread state factory (takes ownership).
        Can be null and set later (before using the search) with
        SetThreadStateFactory to allow multi-step construction.
        @param moveRange Upper integer limit (exclusive) used for move
        representation. Certain enhancements of SgUctSearch (e.g. Rave())
        need to store data in arrays using the move as an index for
        efficient implementation. If the game does not use a small integer
        range for its move representation, this parameter should be 0.
        Then, enhancements that require a small move range cannot be enabled.
        @param precompMaxPos Maximum position count for precomputed bias
        term (see SgUctBiasTermPrecomp). The optimal value depends on the
        values of other search parameters. It should cover the majority of
        count values encountered during the search. A value too large
        increases the construction time and the memory consumption and even
        slower runtime due to bad main memory caching. The default value
        worked well for a 9x9-Go search with 30000 simulations,
        pattern-based playout and RAVE enabled.
        @param precompMaxMove Maximum move count for precomputed bias term.
    */
    SgUctSearch(SgUctThreadStateFactory* threadStateFactory,
                int moveRange = 0,
                std::size_t precompMaxPos = 6000,
                std::size_t precompMaxMove = 300);

    virtual ~SgUctSearch();

    void SetThreadStateFactory(SgUctThreadStateFactory* factory);

    /** @name Pure virtual functions */
    // @{

    /** Convert move to string (game dependent).
        This function needs to be thread-safe.
    */
    virtual std::string MoveString(SgMove move) const = 0;

    /** Evaluation value to use if evaluation is unknown.
        This value will be used, if games are aborted, because they exceed
        the maximum game length.
        This function needs to be thread-safe.
    */
    virtual float UnknownEval() const = 0;

    // @} // name


    /** @name Virtual functions */
    // @{

    /** Hook function that will be called by Search() after each game.
        Default implementation does nothing.
        This function does not need to be thread-safe.
        @param gameNumber The number of this iteration.
        @param threadId
        @param info The game info of the thread which played this iteration.
        @warning If LockFree() is enabled, this function will be called from
        multiple threads without locking. The subclass should handle this
        case appropriately by using its own lock or disabling functionality
        that will not work without locking.
    */
    virtual void OnSearchIteration(std::size_t gameNumber, int threadId,
                                   const SgUctGameInfo& info);

    /** Hook function that will be called by StartSearch().
        Default implementation does nothing.
        This function does not need to be thread-safe.
    */
    virtual void OnStartSearch();

    // @} // name


    /** @name Search functions */
    // @{

    /** Get a list of all generated moves.
        Sets up thread state 0 for a seach and calls GenerateAllMoves
        of the thread state.
    */
    void GenerateAllMoves(std::vector<SgMove>& moves);

    /** Play a single game.
        Plays a single game using the thread state of the first thread.
        Call StartSearch() before calling this function.
        @param isTreeOutOfMem Was the game aborted because the maximum tree
        size was reached?
    */
    void PlayGame(bool& isTreeOutOfMem);

    /** Start search.
        Initializes search for current position and clears statistics.
        @param rootFilter Moves to filter at the root node
        @param initTree The tree to initialize the search with. 0 for no
        initialization. The trees are actually swapped, not copied.
    */
    void StartSearch(const std::vector<SgMove>& rootFilter
                     = std::vector<SgMove>(),
                     SgUctTree* initTree = 0);

    /** Calls StartSearch() and then maxGames times PlayGame().
        @param maxGames The maximum number of games (greater or equal one).
        @param maxTime The maximum time in seconds.
        @param[out] sequence The move sequence with the best value.
        @param rootFilter Moves to filter at the root node
        @param initTree The tree to initialize the search with. 0 for no
        initialization. The trees are actually swapped, not copied.
        @param earlyAbort Abort search after half of the resources (max time,
        max nodes) are spent, if the value is above a threshold.
        @param earlyAbortThreshold See parameter earlyAbort
        @return The value of the root position.
    */
    float Search(std::size_t maxGames, double maxTime,
                 std::vector<SgMove>& sequence,
                 const std::vector<SgMove>& rootFilter
                 = std::vector<SgMove>(),
                 SgUctTree* initTree = 0, bool earlyAbort = false,
                 float earlyAbortThreshold = 0.9);

    /** Do a one-ply Monte-Carlo search instead of the UCT search.
        @param maxGames
        @param maxTime
        @param[out] value The value of the best move
    */
    SgPoint SearchOnePly(size_t maxGames, double maxTime, float& value);

    /** Find child node with best move.
        @param node The father node.
        @param excludeMoves Optional list of moves to ignore in the children
        nodes.
        @return The best child or 0 if no child nodes exists.
    */
    const SgUctNode*
    FindBestChild(const SgUctNode& node,
                  const std::vector<SgMove>* excludeMoves = 0) const;

    /** Extract sequence of best moves from root node.
        @param[out] sequence The resulting sequence.
    */
    void FindBestSequence(std::vector<SgMove>& sequence) const;

    /** Return the bound of a move.
        This is the bound that was used for move selection. It can be the
        pure UCT bound or the combined bound if RAVE is used.
        @param node The node corresponding to the position
        @param child The node corresponding to the move
    */
    float GetBound(const SgUctNode& node, const SgUctNode& child) const;

    // @} // name


    /** @name Search data */
    // @{

    /** Info for last game.
        Returns the last game info of the first thread.
        This function is not thread-safe.
    */
    const SgUctGameInfo& LastGameInfo() const;

    /** One-line summary text for last game.
        Contains: move, count and mean for all nodes; result
        Returns the last game info of the first thread.
        This function is not thread-safe.
    */
    std::string LastGameSummaryLine() const;

    /** See parameter earlyAbort in Search() */
    bool WasEarlyAbort() const;

    const SgUctTree& Tree() const;

    // @} // name


    /** @name Parameters */
    // @{

    /** Constant c in the bias term.
        This constant corresponds to 2 C_p in the original UCT paper.
        The default value is 0.7, which works well in 9x9 Go.
    */
    float BiasTermConstant() const;

    /** See BiasTermConstant() */
    void SetBiasTermConstant(float biasTermConstant);

    /** Don't use a bias term.
        Logically equivalent to setting the bias term constant to zero,
        but faster, because the bias term computation is skipped.
    */
    bool NoBiasTerm() const;

    /** See NoBiasTerm() */
    void SetNoBiasTerm(bool enable);

    /** Maximum number of nodes in the tree. */
    std::size_t MaxNodes() const;

    /** See MaxNodes()
        @param maxNodes Maximum number of nodes (>= 1)
    */
    void SetMaxNodes(std::size_t maxNodes);

    /** The number of threads to use during the search. */
    std::size_t NumberThreads() const;

    /** See SetNumberThreads() */
    void SetNumberThreads(std::size_t n);

    /** Lock-free usage of multi-threaded search.
        @ref sguctsearchlockfree
    */
    bool LockFree() const;

    /** See LockFree() */
    void SetLockFree(bool enable);

    /** Don't update RAVE value if opponent played the same move first.
        Default is false (since it depends on the game and move
        representation, if it should be used).
    */
    bool RaveCheckSame() const;

    /** See RaveCheckSame() */
    void SetRaveCheckSame(bool enable);

    /** First play urgency.
        The value for unexplored moves. According to UCT they should
        always be preferred to moves, that have been played at least once.
        According to the MoGo tech report, it can be useful to use smaller
        values (as low as 1) to encorouge early exploitation.
        Default value is 10000.
        @see @ref sguctsearchweights
    */
    float FirstPlayUrgency() const;

    /** See FirstPlayUrgency() */
    void SetFirstPlayUrgency(float firstPlayUrgency);

    /** Log one-line summary for each game during Search() to a file.
        @todo File name still is hard-coded to "uctsearch.log"
    */
    bool LogGames() const;

    /** See LogGames() */
    void SetLogGames(bool enable);

    /** Maximum game length.
        If the number of moves in a game exceed this length, it will be
        counted as a loss.
        The default is @c numeric_limits<size_t>::max()
    */
    std::size_t MaxGameLength() const;

    /** See MaxGameLength() */
    void SetMaxGameLength(std::size_t maxGameLength);

    /** Required number of simulations to expand a node in the tree.
        The default is 2, which means a node will be expanded on the second
        visit.
    */
    std::size_t ExpandThreshold() const;

    /** See ExpandThreshold() */
    void SetExpandThreshold(std::size_t expandThreshold);

    /** The number of playouts per simulated game.
        Useful for multi-threading to increase the workload of the threads.
        Default is 1.
    */
    std::size_t NumberPlayouts() const;

    void SetNumberPlayouts(std::size_t n);

    /** Use the RAVE algorithm (Rapid Action Value Estimation).
        See Gelly, Silver 2007 in the references in the class description.
        In difference to the original description of the RAVE algorithm,
        no "RAVE bias term" is used. The estimated value of a move is the
        weighted mean of the move value and the RAVE value and then a
        single UCT-like bias term is added.
        @see RaveWeightFunc
    */
    bool Rave() const;

    /** See Rave() */
    void SetRave(bool enable);

    /** Set initializer for unknown moves.
        Takes ownership. Default is 0 (no initializer).
    */
    void SetPriorKnowledge(SgUctPriorKnowledgeFactory* factory);

    /** See SgUctMoveSelect */
    SgUctMoveSelect MoveSelect() const;

    /** See SgUctMoveSelect */
    void SetMoveSelect(SgUctMoveSelect moveSelect);

    /** See @ref sguctsearchweights. */
    float RaveWeightInitial() const;

    /** See @ref sguctsearchweights. */
    void SetRaveWeightInitial(float value);

    /** See @ref sguctsearchweights. */
    float RaveWeightFinal() const;

    /** See @ref sguctsearchweights. */
    void SetRaveWeightFinal(float value);

    // @} // name


    /** @name Statistics */
    // @{

    const SgUctSearchStat& Statistics() const;

    void WriteStatistics(std::ostream& out) const;

    // @} // name

    /** Get state of one of the threads.
        Requires: ThreadsCreated()
    */
    SgUctThreadState& ThreadState(int i) const;

    /** Check if threads are already created.
        The threads are created at the beginning of the first search
        (to allow multi-step construction with setting the policy after
        the constructor call).
    */
    bool ThreadsCreated() const;

    /** Create threads.
        The threads are created at the beginning of the first search
        (to allow multi-step construction with setting the policy after
        the constructor call). This function needs to be called explicitely
        only, if a thread state is going to be used before the first search.
    */
    void CreateThreads();

private:
    typedef boost::recursive_mutex::scoped_lock GlobalLock;

    friend class Thread;

    class Thread
    {
    public:
        std::auto_ptr<SgUctThreadState> m_state;

        Thread(SgUctSearch& search, std::auto_ptr<SgUctThreadState> state);

        ~Thread();

        void StartPlay();

        void WaitPlayFinished();

    private:
        /** Copyable function object that invokes Thread::operator().
            Needed because the the constructor of boost::thread copies the
            function object argument.
        */
        class Function
        {
        public:
            Function(Thread& thread);

            void operator()();

        private:
            Thread& m_thread;
        };

        friend class Thread::Function;

        SgUctSearch& m_search;

        bool m_quit;

        boost::barrier m_threadReady;

        boost::mutex m_startPlayMutex;

        boost::mutex m_playFinishedMutex;

        boost::condition m_startPlay;

        boost::condition m_playFinished;

        boost::mutex::scoped_lock m_playFinishedLock;

        GlobalLock m_globalLock;

        /** The thread.
            Order dependency: must be constructed as the last member, because
            the constructor starts the thread.
        */
        boost::thread m_thread;

        void operator()();

        void PlayGames();
    };

    std::auto_ptr<SgUctThreadStateFactory> m_threadStateFactory;

    /** See LogGames() */
    bool m_logGames;

    /** See Rave() */
    bool m_rave;

    /** See NoBiasTerm() */
    bool m_noBiasTerm;

    /** See parameter earlyAbort in Search() */
    bool m_earlyAbort;

    /** See parameter earlyAbort in Search() */
    bool m_wasEarlyAbort;

    /** See SgUctMoveSelect */
    SgUctMoveSelect m_moveSelect;

    /** See RaveCheckSame() */
    bool m_raveCheckSame;

    /** See LockFree() */
    bool m_lockFree;

    std::size_t m_numberThreads;

    std::size_t m_numberPlayouts;

    std::size_t m_maxNodes;

    /** See parameter moveRange in constructor */
    const int m_moveRange;

    /** See MaxGameLength() */
    std::size_t m_maxGameLength;

    /** See ExpandThreshold() */
    std::size_t m_expandThreshold;

    /** Number of games limit for the current search. */
    std::size_t m_maxGames;

    /** Number of games played in the current search. */
    std::size_t m_numberGames;

    /** Interval in number of games in which to check time abort.
        Avoids that the potentially expensive SgTime::Get() is called after
        every game. The interval is updated dynamically according to the
        current games/sec, such that it is called ten times per second
        (if the total search time is at least one second, otherwise ten times
        per total maximum search time)
    */
    std::size_t m_checkTimeInterval;

    /** See BiasTermConstant() */
    float m_biasTermConstant;

    /** See FirstPlayUrgency() */
    float m_firstPlayUrgency;

    /** See @ref sguctsearchweights. */
    float m_raveWeightInitial;

    /** See @ref sguctsearchweights. */
    float m_raveWeightFinal;

    /** 1 / m_raveWeightInitial precomputed for efficiency */
    float m_raveWeightParam1;

    /** m_raveWeightInitial / m_raveWeightFinal precomputed for efficiency */
    float m_raveWeightParam2;

    /** See parameter earlyAbortThreshold in Search() */
    float m_earlyAbortThreshold;

    /** Time limit for current search. */
    double m_maxTime;

    std::auto_ptr<SgUctPriorKnowledgeFactory> m_priorKnowledgeFactory;

    std::string m_logFileName;

    SgTimer m_timer;

    SgUctTree m_tree;

    /** See parameter rootFilter in function Search() */
    std::vector<SgMove> m_rootFilter;

    std::ofstream m_log;

    /** Mutex for protecting global variables during multi-threading.
        Currently, only the play-out phase of games is thread safe, therefore
        this lock is always locked elsewhere (in-tree phase, updating of
        values and statistics, etc.)
    */
    boost::recursive_mutex m_globalMutex;

    SgUctBiasTermPrecomp m_biasTermPrecomp;

    SgUctSearchStat m_statistics;

    /** List of threads.
        The elements are owned by the vector (shared_ptr is only used because
        auto_ptr should not be used with standard containers)
    */
    std::vector<boost::shared_ptr<Thread> > m_threads;

#if SG_UCTFASTLOG
    SgFastLog m_fastLog;
#endif

    void ApplyRootFilter(std::vector<SgMove>& moves);

    bool CheckAbortSearch(const SgUctThreadState& state);

    bool CheckCountAbort(std::size_t remainingGames) const;

    void Debug(const SgUctThreadState& state, const std::string& textLine);

    void DeleteThreads();

    bool EarlyAbort() const;

    void ExpandNode(SgUctThreadState& state, const SgUctNode& node,
                    bool& isTreeOutOfMem, bool& deepenTree);

    void InitPriorKnowledge(SgUctThreadState& state, const SgUctNode& node,
                            bool& deepenTree);

    float GetBound(std::size_t posCount, float logPosCount,
                   const SgUctNode& child) const;

    float GetValueEstimate(const SgUctNode& child) const;

    float GetValueEstimateRave(const SgUctNode& child) const;

    float Log(std::size_t x) const;

    void PlayGame(SgUctThreadState& state, GlobalLock* lock,
                  bool& isTreeOutOfMem);

    bool PlayInTree(SgUctThreadState& state, bool& isTerminal,
                    bool& isTreeOutOfMem);

    bool PlayoutGame(SgUctThreadState& state, std::size_t playout);

    void SearchLoop(SgUctThreadState& state, GlobalLock* lock);

    const SgUctNode& SelectChild(const SgUctNode& node);

    std::string SummaryLine(const SgUctGameInfo& info) const;

    void UpdateCheckTimeInterval(double time);

    void UpdateDynRaveBias();

    void UpdateRaveValues(SgUctThreadState& state);

    void UpdateRaveValues(SgUctThreadState& state, std::size_t playout);

    void UpdateRaveValues(SgUctThreadState& state, float eval, std::size_t i,
                          const std::size_t firstPlay[],
                          const std::size_t firstPlayOpp[]);

    void UpdateStatistics(const SgUctGameInfo& info);

    void UpdateTree(const SgUctGameInfo& info);
};

inline float SgUctSearch::BiasTermConstant() const
{
    return m_biasTermConstant;
}

inline std::size_t SgUctSearch::ExpandThreshold() const
{
    return m_expandThreshold;
}

inline float SgUctSearch::FirstPlayUrgency() const
{
    return m_firstPlayUrgency;
}

inline float SgUctSearch::InverseEval(float eval)
{
    return (1 - eval);
}

inline bool SgUctSearch::LockFree() const
{
    return m_lockFree;
}

inline const SgUctGameInfo& SgUctSearch::LastGameInfo() const
{
    return ThreadState(0).m_gameInfo;
}

inline bool SgUctSearch::LogGames() const
{
    return m_logGames;
}

inline std::size_t SgUctSearch::MaxGameLength() const
{
    return m_maxGameLength;
}

inline std::size_t SgUctSearch::MaxNodes() const
{
    return m_maxNodes;
}

inline SgUctMoveSelect SgUctSearch::MoveSelect() const
{
    return m_moveSelect;
}

inline bool SgUctSearch::NoBiasTerm() const
{
    return m_noBiasTerm;
}

inline std::size_t SgUctSearch::NumberThreads() const
{
    return m_numberThreads;
}

inline std::size_t SgUctSearch::NumberPlayouts() const
{
    return m_numberPlayouts;
}

inline void SgUctSearch::PlayGame(bool& isTreeOutOfMem)
{
    PlayGame(ThreadState(0), 0, isTreeOutOfMem);
}

inline bool SgUctSearch::Rave() const
{
    return m_rave;
}

inline bool SgUctSearch::RaveCheckSame() const
{
    return m_raveCheckSame;
}

inline float SgUctSearch::RaveWeightInitial() const
{
    return m_raveWeightInitial;
}

inline float SgUctSearch::RaveWeightFinal() const
{
    return m_raveWeightFinal;
}

inline void SgUctSearch::SetBiasTermConstant(float biasTermConstant)
{
    m_biasTermConstant = biasTermConstant;
}

inline void SgUctSearch::SetExpandThreshold(std::size_t expandThreshold)
{
    SG_ASSERT(expandThreshold >= 1);
    m_expandThreshold = expandThreshold;
}

inline void SgUctSearch::SetFirstPlayUrgency(float firstPlayUrgency)
{
    m_firstPlayUrgency = firstPlayUrgency;
}

inline void SgUctSearch::SetLockFree(bool enable)
{
    m_lockFree = enable;
}

inline void SgUctSearch::SetLogGames(bool enable)
{
    m_logGames = enable;
}

inline void SgUctSearch::SetMaxGameLength(std::size_t maxGameLength)
{
    m_maxGameLength = maxGameLength;
}

inline void SgUctSearch::SetMaxNodes(std::size_t maxNodes)
{
    m_maxNodes = maxNodes;
    if (m_threads.size() > 0) // Threads already created
        m_tree.SetMaxNodes(m_maxNodes);
}

inline void SgUctSearch::SetMoveSelect(SgUctMoveSelect moveSelect)
{
    m_moveSelect = moveSelect;
}

inline void SgUctSearch::SetNoBiasTerm(bool enable)
{
    m_noBiasTerm = enable;
}

inline void SgUctSearch::SetNumberPlayouts(std::size_t n)
{
    SG_ASSERT(n >= 1);
    m_numberPlayouts = n;
}

inline void SgUctSearch::SetRaveCheckSame(bool enable)
{
    m_raveCheckSame = enable;
}

inline void SgUctSearch::SetRaveWeightFinal(float value)
{
    m_raveWeightFinal = value;
}

inline void SgUctSearch::SetRaveWeightInitial(float value)
{
    m_raveWeightInitial = value;
}

inline const SgUctSearchStat& SgUctSearch::Statistics() const
{
    return m_statistics;
}

inline bool SgUctSearch::ThreadsCreated() const
{
    return (m_threads.size() > 0);
}

inline SgUctThreadState& SgUctSearch::ThreadState(int i) const
{
    SG_ASSERT(static_cast<std::size_t>(i) < m_threads.size());
    return *m_threads[i]->m_state;
}

inline const SgUctTree& SgUctSearch::Tree() const
{
    return m_tree;
}

inline bool SgUctSearch::WasEarlyAbort() const
{
    return m_wasEarlyAbort;
}

//----------------------------------------------------------------------------

#endif // SG_UCTSEARCH_H
