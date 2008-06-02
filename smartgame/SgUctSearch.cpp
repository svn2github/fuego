//----------------------------------------------------------------------------
/** @file SgUctSearch.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctSearch.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/version.hpp>
#include "SgDebug.h"
#include "SgHashTable.h"
#include "SgMath.h"
#include "SgWrite.h"

using namespace std;
using boost::barrier;
using boost::condition;
using boost::format;
using boost::mutex;
using boost::shared_ptr;
using boost::trim_copy;

#define BOOST_VERSION_MAJOR (BOOST_VERSION / 100000)
#define BOOST_VERSION_MINOR (BOOST_VERSION / 100 % 1000)

//----------------------------------------------------------------------------

namespace {

const bool DEBUG_THREADS = false;

/** Check that an integer is greater zero, after it was converted to float. */
bool IsGreaterZero(float x)
{
    return (x > 0.9f);
}

void Notify(mutex& aMutex, condition& aCondition)
{
    mutex::scoped_lock lock(aMutex);
    aCondition.notify_all();
}

} // namespace

//----------------------------------------------------------------------------

void SgUctGameInfo::Clear(std::size_t numberPlayouts)
{
    m_nodes.clear();
    m_inTreeSequence.clear();
    m_inTreeSignatures.clear();
    if (numberPlayouts != m_sequence.size())
    {
        m_sequence.resize(numberPlayouts);
        m_skipRaveUpdate.resize(numberPlayouts);
        m_signatures.resize(numberPlayouts);
        m_eval.resize(numberPlayouts);
        m_aborted.resize(numberPlayouts);
    }
    for (size_t i = 0; i < numberPlayouts; ++i)
    {
        m_sequence[i].clear();
        m_skipRaveUpdate[i].clear();
        m_signatures[i].clear();
    }
}

//----------------------------------------------------------------------------

SgUctPriorKnowledge::~SgUctPriorKnowledge()
{
}

void SgUctPriorKnowledge::ProcessPosition()
{
    // Default implementation does nothing
}

//----------------------------------------------------------------------------

SgUctPriorKnowledgeFactory::~SgUctPriorKnowledgeFactory()
{
}

//----------------------------------------------------------------------------

SgUctInverseEvalFunc::~SgUctInverseEvalFunc()
{
}

//----------------------------------------------------------------------------

SgUctThreadState::SgUctThreadState(size_t threadId, int moveRange)
    : m_threadId(threadId)
{
    if (moveRange > 0)
    {
        m_firstPlay.reset(new size_t[moveRange]);
        m_firstPlayOpp.reset(new size_t[moveRange]);
    }
}

SgUctThreadState::~SgUctThreadState()
{
}

void SgUctThreadState::EndPlayout()
{
    // Default implementation does nothing
}

void SgUctThreadState::GameStart()
{
    // Default implementation does nothing
}

std::size_t SgUctThreadState::GetSignature(SgMove mv) const
{
    SG_UNUSED(mv);
    return numeric_limits<size_t>::max();
}

void SgUctThreadState::StartPlayout()
{
    // Default implementation does nothing
}

void SgUctThreadState::StartPlayouts()
{
    // Default implementation does nothing
}

//----------------------------------------------------------------------------

SgUctThreadStateFactory::~SgUctThreadStateFactory()
{
}

//----------------------------------------------------------------------------

SgUctSearch::Thread::Function::Function(Thread& thread)
    : m_thread(thread)
{
}

void SgUctSearch::Thread::Function::operator()()
{
    m_thread();
}

SgUctSearch::Thread::Thread(SgUctSearch& search,
                            auto_ptr<SgUctThreadState> state)
    : m_state(state),
      m_search(search),
      m_quit(false),
      m_threadReady(2),
      m_playFinishedLock(m_playFinishedMutex),
#if BOOST_VERSION_MAJOR == 1 && BOOST_VERSION_MINOR <= 34
      m_globalLock(search.m_globalMutex, false),
#else
      m_globalLock(search.m_globalMutex, boost::defer_lock),
#endif
      m_thread(Function(*this))
{
    m_threadReady.wait();
}

SgUctSearch::Thread::~Thread()
{
    m_quit = true;
    StartPlay();
    m_thread.join();
}

void SgUctSearch::Thread::operator()()
{
    if (DEBUG_THREADS)
        SgDebug() << "SgUctSearch::Thread: starting thread "
                  << m_state->m_threadId << '\n';
    mutex::scoped_lock lock(m_startPlayMutex);
    m_threadReady.wait();
    while (true)
    {
        m_startPlay.wait(lock);
        if (m_quit)
            break;
        m_search.SearchLoop(*m_state, &m_globalLock);
        Notify(m_playFinishedMutex, m_playFinished);
    }
    if (DEBUG_THREADS)
        SgDebug() << "SgUctSearch::Thread: finishing thread "
                  << m_state->m_threadId << '\n';
}

void SgUctSearch::Thread::StartPlay()
{
    Notify(m_startPlayMutex, m_startPlay);
}

void SgUctSearch::Thread::WaitPlayFinished()
{
    m_playFinished.wait(m_playFinishedLock);
}

//----------------------------------------------------------------------------

SgUctSearch::SgUctSearch(SgUctThreadStateFactory* threadStateFactory,
                         int moveRange, std::size_t precompMaxPos,
                         std::size_t precompMaxMove)
    : m_threadStateFactory(threadStateFactory),
      m_logGames(false),
      m_rave(false),
      m_moveSelect(SG_UCTMOVESELECT_COUNT),
      m_priorInit(SG_UCTPRIORINIT_BOTH),
      m_raveCheckSame(false),
      m_lockFree(false),
      m_useSignatures(false),
      m_numberThreads(1),
      m_numberPlayouts(1),
      m_maxNodes(5000000),
      m_moveRange(moveRange),
      m_maxGameLength(numeric_limits<size_t>::max()),
      m_expandThreshold(1),
      m_biasTermConstant(0.7f),
      m_firstPlayUrgency(10000),
      m_raveWeightInitial(1.f),
      m_raveWeightFinal(20000),
      m_signatureWeightInitial(0.2),
      m_signatureWeightFinal(4000),
      m_gamesPerSecond(0),
      m_logFileName("uctsearch.log"),
      m_biasTermPrecomp(precompMaxPos, precompMaxMove),
      m_fastLog(10)
{
    // Don't create thread states here, because the factory passes the search
    // (which is no fully constructed here, because the subclass constructors
    // are not called yet) as an argument to the Create() function
}

SgUctSearch::~SgUctSearch()
{
    DeleteThreads();
}

void SgUctSearch::ApplyRootFilter(vector<SgMove>& moves)
{
    // Filter without changing the order of the unfiltered moves
    vector<SgMove> filteredMoves;
    for (vector<SgMove>::const_iterator it = moves.begin();
         it != moves.end(); ++it)
        if (find(m_rootFilter.begin(), m_rootFilter.end(), *it)
            == m_rootFilter.end())
            filteredMoves.push_back(*it);
    moves = filteredMoves;
}

bool SgUctSearch::CheckAbortSearch(const SgUctThreadState& state)
{
    if (SgUserAbort())
    {
        Debug(state, "SgUctSearch::CheckAbortSearch: abort flag");
        return true;
    }
    if (m_numberGames >= m_maxGames)
    {
        Debug(state, "SgUctSearch::CheckAbortSearch: max games reached");
        return true;
    }
    if (m_numberGames % m_checkTimeInterval == 0)
    {
        double time = m_timer.GetTime();
        if (time > m_maxTime)
        {
            Debug(state, "SgUctSearch::CheckAbortSearch: max time reached");
            return true;
        }
        UpdateCheckTimeInterval(time);
        // Check for time greater zero, otherwise m_gamesPerSecond is set to
        // zero and the following will not work
        if (time > numeric_limits<double>::epsilon()
            && m_moveSelect == SG_UCTMOVESELECT_COUNT)
        {
            double remainingGamesDouble = m_maxGames - m_numberGames - 1;
            double remainingTime = m_maxTime - time;
            remainingGamesDouble =
                min(remainingGamesDouble, remainingTime * m_gamesPerSecond);
            size_t sizeTypeMax = numeric_limits<size_t>::max();
            size_t remainingGames;
            if (remainingGamesDouble > static_cast<double>(sizeTypeMax - 1))
                remainingGames = sizeTypeMax;
            else
                remainingGames = static_cast<size_t>(remainingGamesDouble);
            if (CheckCountAbort(remainingGames))
            {
                Debug(state,
                      "SgUctSearch::CheckAbortSearch: best move cannot"
                      " change anymore (count select)");
                return true;
            }
        }
    }
    return false;
}

bool SgUctSearch::CheckCountAbort(std::size_t remainingGames) const
{
    const SgUctNode& root = m_tree.Root();
    const SgUctNode* bestChild = FindBestChild(root);
    if (bestChild == 0)
        return false;
    size_t bestCount = bestChild->MoveCount();
    vector<SgMove> excludeMoves;
    excludeMoves.push_back(bestChild->Move());
    const SgUctNode* secondBestChild = FindBestChild(root, &excludeMoves);
    if (secondBestChild == 0)
        return false;
    size_t secondBestCount = secondBestChild->MoveCount();
    SG_ASSERT(secondBestCount <= bestCount || m_numberThreads > 1);
    return (secondBestCount + remainingGames <= bestCount);
}

void SgUctSearch::CreateThreads()
{
    DeleteThreads();
    for (size_t i = 0; i < m_numberThreads; ++i)
    {
        auto_ptr<SgUctThreadState> state(
                                      m_threadStateFactory->Create(i, *this));
        if (m_priorKnowledgeFactory.get() != 0)
        {
            SgUctPriorKnowledge* priorKnowledge =
                m_priorKnowledgeFactory->Create(*state);
            state->m_priorKnowledge.reset(priorKnowledge);
        }
        shared_ptr<Thread> thread(new Thread(*this, state));
        m_threads.push_back(thread);
    }
    m_tree.CreateAllocators(m_numberThreads);
    m_tree.SetMaxNodes(m_maxNodes);
}

/** Write a debugging line of text from within a thread.
    Prepends the line with the thread number if number of threads is greater
    than one. Also ensures that the line is written as a single string to
    avoid intermingling of text lines from different threads.
    @param state The state of the thread (only used for state.m_threadId)
    @param textLine The line of text without trailing newline character.
*/
void SgUctSearch::Debug(const SgUctThreadState& state,
                        const std::string& textLine)
{
    if (m_numberThreads > 1)
    {
        // SgDebug() is not necessarily thread-safe
        GlobalLock lock(m_globalMutex);
        SgDebug() << (format("[%1%] %2%\n") % state.m_threadId % textLine);
    }
    else
        SgDebug() << (format("%1%\n") % textLine);
}

void SgUctSearch::DeleteThreads()
{
    m_threads.clear();
}

/** Expand a node.
    @param state The thread state with state.m_moves already computed.
    @param node The node to expand.
    @param[out] isTreeOutOfMem Will be set to true, if node was not expanded
    because maximum tree size was reached.
*/
void SgUctSearch::ExpandNode(SgUctThreadState& state, const SgUctNode& node,
                             bool& isTreeOutOfMem)
{
    size_t threadId = state.m_threadId;
    if (! m_tree.HasCapacity(threadId, state.m_moves.size()))
    {
        Debug(state,
              str(format("SgUctSearch::PlayInTree: "
                         "Maximum tree size %1% reached")
                  % m_tree.MaxNodes()));
        isTreeOutOfMem = true;
        return;
    }
    m_tree.CreateChildren(threadId, node, state.m_moves);
    if (m_useSignatures)
        for (SgUctChildIterator it(m_tree, node); it; ++it)
        {
            const SgUctNode& child = *it;
            m_tree.SetSignature(child, state.GetSignature(child.Move()));
        }
    if (state.m_priorKnowledge.get() != 0)
        InitPriorKnowledge(state, node);
}

const SgUctNode*
SgUctSearch::FindBestChild(const SgUctNode& node,
                           const vector<SgMove>* excludeMoves) const
{
    if (! node.HasChildren())
        return 0;
    const SgUctNode* bestChild = 0;
    float bestValue = 0;
    for (SgUctChildIterator it(m_tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        if (excludeMoves != 0)
        {
            vector<SgMove>::const_iterator begin = excludeMoves->begin();
            vector<SgMove>::const_iterator end = excludeMoves->end();
            if (find(begin, end, child.Move()) != end)
                continue;
        }
        size_t raveCount = child.RaveCount();
        if (child.MoveCount() == 0
            && ! ((m_moveSelect == SG_UCTMOVESELECT_BOUND
                   || m_moveSelect == SG_UCTMOVESELECT_ESTIMATE)
                  && m_rave && raveCount > 0))
            continue;
        float moveValue = InverseEval(child.Mean());
        size_t moveCount = child.MoveCount();
        float value;
        switch (m_moveSelect)
        {
        case SG_UCTMOVESELECT_VALUE:
            value = moveValue;
            break;
        case SG_UCTMOVESELECT_COUNT:
            value = moveCount;
            break;
        case SG_UCTMOVESELECT_BOUND:
            value = GetBound(node, child);
            break;
        case SG_UCTMOVESELECT_ESTIMATE:
            value = GetValueEstimate(child);
            break;
        default:
            SG_ASSERT(false);
            value = SG_UCTMOVESELECT_VALUE;
        }
        if (bestChild == 0 || value > bestValue)
        {
            bestChild = &child;
            bestValue = value;
        }
    }
    return bestChild;
}

void SgUctSearch::FindBestSequence(vector<SgMove>& sequence) const
{
    sequence.clear();
    const SgUctNode* current = &m_tree.Root();
    while (true)
    {
        current = FindBestChild(*current);
        if (current == 0)
            break;
        sequence.push_back(current->Move());
        if (! current->HasChildren())
            break;
    }
}

void SgUctSearch::GenerateAllMoves(std::vector<SgMove>& moves)
{
    if (m_threads.size() == 0)
        CreateThreads();
    moves.clear();
    OnStartSearch();
    SgUctThreadState& state = ThreadState(0);
    state.StartSearch();
    state.GenerateAllMoves(moves);
}

float SgUctSearch::GetBound(const SgUctNode& node,
                            const SgUctNode& child) const
{
    size_t posCount = node.PosCount();
    return GetBound(posCount, Log(posCount), child);
}

float SgUctSearch::GetBound(std::size_t posCount, float logPosCount,
                            const SgUctNode& child) const
{
    float value;
    if (m_rave && ! m_useSignatures)
        value = GetValueEstimateRaveNoSig(child);
    else
        value = GetValueEstimate(child);
    size_t moveCount = child.MoveCount();
    float bound = value + m_biasTermConstant
        * m_biasTermPrecomp.Get(posCount, logPosCount, moveCount + 1);
    return bound;
}

const SgUctStatisticsBase& SgUctSearch::GetSignatureStat(SgMove mv) const
{
    if (m_threads.size() == 0)
        throw SgException("SgUctSearch::GetSignatureStat: "
                          "statistics unavailable");
    size_t sig = ThreadState(0).GetSignature(mv);
    if (sig == numeric_limits<size_t>::max())
        throw SgException("SgUctSearch::GetSignatureStat: "
                          "invalid move or statistics unavailable");
    return m_signatureStat[mv];
}

float SgUctSearch::GetValueEstimate(const SgUctNode& child) const
{
    float value = 0.f;
    float weightSum = 0.f;
    bool hasValue = false;
    size_t moveCount = child.MoveCount();
    if (moveCount > 0)
    {
        float weight = moveCount;
        value += weight * InverseEval(child.Mean());
        weightSum += weight;
        hasValue = true;
    }
    if (m_rave)
    {
        size_t raveCount = child.RaveCount();
        if (raveCount > 0)
        {
            float weight;
            weight =
                raveCount
                / (m_raveWeightParam1 + m_raveWeightParam2 * raveCount);
            value += weight * child.RaveValue();
            weightSum += weight;
            hasValue = true;
        }
    }
    if (m_useSignatures)
    {
        size_t sig = child.Signature();
        if (sig != numeric_limits<size_t>::max())
        {
            SG_ASSERT(sig < m_signatureStat.size());
            SgUctStatisticsBase stat = m_signatureStat[sig];
            size_t sigCount = stat.Count();
            if (sigCount > 0)
            {
                float weight =
                    sigCount
                    / (m_sigWeightParam1 + m_sigWeightParam2 * sigCount);
                value += weight * stat.Mean();
                weightSum += weight;
                hasValue = true;
            }
        }
    }
    if (hasValue)
        return (value / weightSum);
    else
        return m_firstPlayUrgency;
}

/** Optimized version of GetValueEstimate() if RAVE and no signatures are
    used.
*/
float SgUctSearch::GetValueEstimateRaveNoSig(const SgUctNode& child) const
{
    SG_ASSERT(m_rave);
    SG_ASSERT(! m_useSignatures);
    size_t moveCount = child.MoveCount();
    size_t raveCount = child.RaveCount();
    float value;
    if (moveCount == 0)
    {
        if (raveCount > 0)
            value = child.RaveValue();
        else
            value = m_firstPlayUrgency;
    }
    else
    {
        float moveValue = InverseEval(child.Mean());
        if (raveCount == 0)
        {
            SG_ASSERT(m_numberThreads > 1);
            value = moveValue;
        }
        else
        {
            float raveCountFloat = static_cast<float>(raveCount);
            float weight =
                raveCountFloat
                / (moveCount
                   * (m_raveWeightParam1
                      + m_raveWeightParam2 * raveCountFloat)
                   + raveCountFloat);
            value = weight * child.RaveValue() + (1.f - weight) * moveValue;
        }
    }
    SG_ASSERT(m_numberThreads > 1
              || fabs(value - GetValueEstimate(child)) < 1e-3/*epsilon*/);
    return value;
}

void SgUctSearch::InitPriorKnowledge(SgUctThreadState& state,
                                     const SgUctNode& node)
{
    SgUctPriorKnowledge* priorKnowledge = state.m_priorKnowledge.get();
    SG_ASSERT(priorKnowledge != 0);
    priorKnowledge->ProcessPosition();
    for (SgUctChildIterator it(m_tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        SgMove move = child.Move();
        float value;
        size_t count;
        priorKnowledge->InitializeMove(move, value, count);
        if (count == 0)
            continue;
        if (m_priorInit == SG_UCTPRIORINIT_MOVE
            || m_priorInit == SG_UCTPRIORINIT_BOTH)
            m_tree.InitializeValue(node, child, InverseEval(value), count);
        if (m_rave &&
            (m_priorInit == SG_UCTPRIORINIT_RAVE
             || m_priorInit == SG_UCTPRIORINIT_BOTH))
            m_tree.InitializeRaveValue(child, value, count);
    }
}

string SgUctSearch::LastGameSummaryLine() const
{
    return SummaryLine(LastGameInfo());
}

float SgUctSearch::Log(std::size_t x) const
{
    // TODO: can we speed up the computation of the logarithm by taking
    // advantage of the fact that the argument is an integer type?
    // Maybe round result to integer (then it is simple the position of the
    // highest bit
#if SG_UCTFASTLOG
    return m_fastLog.Log(static_cast<float>(x));
#else
    return log(static_cast<float>(x));
#endif
}

void SgUctSearch::OnStartSearch()
{
}

void SgUctSearch::OnSearchIteration(std::size_t gameNumber, int threadId,
                                    const SgUctGameInfo& info)
{
    SG_UNUSED(gameNumber);
    SG_UNUSED(threadId);
    SG_UNUSED(info);
}

void SgUctSearch::PlayGame(SgUctThreadState& state, GlobalLock* lock,
                           bool& isTreeOutOfMem)
{
    state.GameStart();
    SgUctGameInfo& info = state.m_gameInfo;
    info.Clear(m_numberPlayouts);
    bool isTerminal;
    bool abortInTree = ! PlayInTree(state, isTerminal, isTreeOutOfMem);

    // The playout phase is always unlocked
    if (lock != 0)
        lock->unlock();

    size_t nuMovesInTree = info.m_inTreeSequence.size();
    state.StartPlayouts();
    for (size_t i = 0; i < m_numberPlayouts; ++i)
    {
        state.StartPlayout();
        info.m_sequence[i] = info.m_inTreeSequence;
        // skipRaveUpdate only used in playout phase
        info.m_skipRaveUpdate[i].assign(nuMovesInTree, false);
        if (m_useSignatures)
            info.m_signatures[i] = info.m_inTreeSignatures;
        bool abort = abortInTree;
        if (! isTreeOutOfMem && ! abort && ! isTerminal)
            abort = ! PlayoutGame(state, i);
        float eval;
        if (abort || isTreeOutOfMem)
            eval = UnknownEval();
        else
            eval = state.Evaluate();
        size_t nuMoves = info.m_sequence[i].size();
        if (nuMoves % 2 != 0)
            eval = InverseEval(eval);
        info.m_aborted[i] = abort;
        info.m_eval[i] = eval;
        state.EndPlayout();
        state.TakeBackPlayout(nuMoves - nuMovesInTree);
    }
    state.TakeBackInTree(nuMovesInTree);

    // End of unlocked part if ! m_lockFree
    if (lock != 0)
        lock->lock();

    UpdateTree(info);
    if (m_rave)
        UpdateRaveValues(state);
    if (m_useSignatures)
        UpdateSignatures(info);
    UpdateStatistics(info);
}

/** Play game until it leaves the tree.
    @param state
    @param[out] isTerminal Was the sequence terminated because of a real
    terminal position (GenerateAllMoves() returned an empty list)?
    @param[out] isTreeOutOfMem Didn't the tree have the capacity to create
    new children
    @return @c false, if game was aborted
 */
bool SgUctSearch::PlayInTree(SgUctThreadState& state,
                             bool& isTerminal, bool& isTreeOutOfMem)
{
    vector<SgMove>& sequence = state.m_gameInfo.m_inTreeSequence;
    vector<size_t>& signatures = state.m_gameInfo.m_inTreeSignatures;
    vector<const SgUctNode*>& nodes = state.m_gameInfo.m_nodes;
    const SgUctNode* root = &m_tree.Root();
    const SgUctNode* current = root;
    nodes.push_back(current);
    bool breakAfterSelect = false;
    isTerminal = false;
    isTreeOutOfMem = false;
    while (true)
    {
        if (sequence.size() == m_maxGameLength)
            return false;
        if (! current->HasChildren())
        {
            state.m_moves.clear();
            state.GenerateAllMoves(state.m_moves);
            if (current == root)
                ApplyRootFilter(state.m_moves);
            if (state.m_moves.empty())
            {
                isTerminal = true;
                break;
            }
            if (current->MoveCount() >= m_expandThreshold)
            {
                ExpandNode(state, *current, isTreeOutOfMem);
                if (isTreeOutOfMem)
                    return false;
                breakAfterSelect = true;
            }
            else
                break;
        }
        current = &SelectChild(*current);
        nodes.push_back(current);
        SgMove move = current->Move();
        if (m_useSignatures)
            signatures.push_back(current->Signature());
        state.Execute(move);
        sequence.push_back(move);
        if (breakAfterSelect)
            break;
    }
    return true;
}

/** Finish the game using GenerateRandomMove().
    @param state The thread state.
    @param playout The number of the playout.
    @return @c false if game was aborted
*/
bool SgUctSearch::PlayoutGame(SgUctThreadState& state, std::size_t playout)
{
    SgUctGameInfo& info = state.m_gameInfo;
    vector<SgMove>& sequence = info.m_sequence[playout];
    vector<size_t>& signatures = info.m_signatures[playout];
    vector<bool>& skipRaveUpdate = info.m_skipRaveUpdate[playout];
    while (true)
    {
        if (sequence.size() == m_maxGameLength)
            return false;
        bool skipRave = false;
        SgMove move = state.GenerateRandomMove(skipRave);
        if (move == SG_NULLMOVE)
            break;
        if (m_useSignatures)
            signatures.push_back(state.GetSignature(move));
        state.Execute(move);
        sequence.push_back(move);
        skipRaveUpdate.push_back(skipRave);
    }
    return true;
}

float SgUctSearch::Search(std::size_t maxGames, double maxTime,
                          vector<SgMove>& sequence,
                          const vector<SgMove>& rootFilter,
                          SgUctTree* initTree)
{
    m_timer.Start();
    m_rootFilter = rootFilter;
    if (m_logGames)
    {
        m_log.open(m_logFileName.c_str());
        m_log << "StartSearch maxGames=" << maxGames << '\n';
    }
    StartSearch(rootFilter, initTree);
    m_maxGames = maxGames;
    m_maxTime = maxTime;
    m_checkTimeInterval = 1;
    m_gamesPerSecond = 0;
    m_numberGames = 0;
    for (size_t i = 0; i < m_threads.size(); ++i)
        m_threads[i]->StartPlay();
    for (size_t i = 0; i < m_threads.size(); ++i)
        m_threads[i]->WaitPlayFinished();
    double time = m_timer.GetTime();
    if (time > numeric_limits<double>::epsilon())
        m_gamesPerSecond = m_numberGames / time;
    if (m_logGames)
        m_log.close();
    FindBestSequence(sequence);
    return m_tree.Root().Mean();
}

/** Loop invoked by multiple-threads for playing games. */
void SgUctSearch::SearchLoop(SgUctThreadState& state, GlobalLock* lock)
{
    if (NumberThreads() == 1 || m_lockFree)
        lock = 0;
    if (lock != 0)
        lock->lock();
    while (true)
    {
        bool isTreeOutOfMem;
        PlayGame(state, lock, isTreeOutOfMem);
        if (isTreeOutOfMem)
            break;
        OnSearchIteration(m_numberGames + 1, state.m_threadId,
                          state.m_gameInfo);
        if (m_logGames)
            m_log << SummaryLine(state.m_gameInfo) << '\n';
        ++m_numberGames;
        if (CheckAbortSearch(state))
            break;
    }
    if (lock != 0)
        lock->unlock();
}

SgPoint SgUctSearch::SearchOnePly(size_t maxGames, double maxTime,
                                  float& value)
{
    if (m_threads.size() == 0)
        CreateThreads();
    OnStartSearch();
    // SearchOnePly is not multi-threaded.
    // It uses the state of the first thread.
    SgUctThreadState& state = ThreadState(0);
    state.StartSearch();
    vector<SgMove> moves;
    state.GenerateAllMoves(moves);
    vector<SgUctStatisticsBase> statistics(moves.size());
    size_t games = 0;
    m_timer.Start();
    SgUctGameInfo& info = state.m_gameInfo;
    while (games < maxGames && m_timer.GetTime() < maxTime && ! SgUserAbort())
    {
        for (size_t i = 0; i < moves.size(); ++i)
        {
            state.GameStart();
            info.Clear(1);
            SgMove move = moves[i];
            state.Execute(move);
            info.m_inTreeSequence.push_back(move);
            info.m_sequence[0].push_back(move);
            info.m_skipRaveUpdate[0].push_back(false);
            state.StartPlayouts();
            state.StartPlayout();
            bool abortGame = ! PlayoutGame(state, 0);
            float eval;
            if (abortGame)
                eval = UnknownEval();
            else
                eval = state.Evaluate();
            state.EndPlayout();
            state.TakeBackPlayout(info.m_sequence[0].size() - 1);
            state.TakeBackInTree(1);
            statistics[i].Add(info.m_sequence[0].size() % 2 == 0 ?
                              eval : InverseEval(eval));
            OnSearchIteration(games + 1, 0, info);
            ++games;
        }
    }
    SgMove bestMove = SG_NULLMOVE;
    for (size_t i = 0; i < moves.size(); ++i)
    {
        SgDebug() << SgWritePoint(moves[i]) << ' ' << statistics[i].Mean()
                  << '\n';
        if (bestMove == SG_NULLMOVE || statistics[i].Mean() > value)
        {
            bestMove = moves[i];
            value = statistics[i].Mean();
        }
    }
    return bestMove;
}

const SgUctNode& SgUctSearch::SelectChild(const SgUctNode& node)
{
    size_t posCount = node.PosCount();
    if (posCount == 0)
        // If position count is zero, return first child
        return *SgUctChildIterator(m_tree, node);
    float logPosCount = Log(posCount);
    const SgUctNode* bestChild = 0;
    float bestUpperBound = 0;
    for (SgUctChildIterator it(m_tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        float bound = GetBound(posCount, logPosCount, child);
        if (bestChild == 0 || bound > bestUpperBound)
        {
            bestChild = &child;
            bestUpperBound = bound;
        }
    }
    SG_ASSERT(bestChild != 0);
    return *bestChild;
}

void SgUctSearch::SetNumberThreads(std::size_t n)
{
    SG_ASSERT(n >= 1);
    if (m_numberThreads == n)
        return;
    m_numberThreads = n;
    CreateThreads();
}

void SgUctSearch::SetPriorKnowledge(SgUctPriorKnowledgeFactory* factory)
{
    m_priorKnowledgeFactory.reset(factory);
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        SgUctPriorKnowledge* priorKnowledge;
        SgUctThreadState& state = ThreadState(i);
        if (factory == 0)
            priorKnowledge = 0;
        else
            priorKnowledge = factory->Create(state);
        state.m_priorKnowledge.reset(priorKnowledge);
    }
}

void SgUctSearch::SetRave(bool enable)
{
    if (enable && m_moveRange <= 0)
        throw SgException("RAVE not supported for this game");
    m_rave = enable;
}

void SgUctSearch::SetThreadStateFactory(SgUctThreadStateFactory* factory)
{
    SG_ASSERT(m_threadStateFactory.get() == 0);
    m_threadStateFactory.reset(factory);
    DeleteThreads();
    // Don't create states here, because this function could be called in the
    // constructor of the subclass, and the factory passes the search (which
    // is not fully constructed) as an argument to the Create() function
}

std::size_t SgUctSearch::SignatureRange() const
{
    return 0;
}

void SgUctSearch::StartSearch(const vector<SgMove>& rootFilter,
                              SgUctTree* initTree)
{
    if (m_threads.size() == 0)
        CreateThreads();
    if (m_numberThreads > 1 && SgTime::DefaultMode() == SG_TIME_CPU)
        // Using CPU time with multiple threads makes the measured time
        // and games/sec not very meaningful; the total cputime is not equal
        // to the total real time, even if there is no other load on the
        // machine, because the time, while threads are waiting for a lock
        // does not contribute to the cputime.
        SgWarning() << "SgUctSearch: using cpu time with multiple threads\n";
    if (m_useSignatures)
    {
        size_t range = SignatureRange();
        if (range == 0)
            SgWarning() << "SgUctSearch: signatures enabled but range is 0\n";
        m_signatureStat.resize(range);
        for (size_t i = 0; i < range; ++i)
            m_signatureStat[i].Clear();
    }
    m_raveWeightParam1 = 1.f / m_raveWeightInitial;
    m_raveWeightParam2 = m_raveWeightInitial / m_raveWeightFinal;
    m_sigWeightParam1 = 1.f / m_signatureWeightInitial;
    m_sigWeightParam2 = m_signatureWeightInitial / m_signatureWeightFinal;
    if (initTree == 0)
        m_tree.Clear();
    else
    {
        m_tree.Swap(*initTree);
        if (m_tree.HasCapacity(0, m_tree.Root().NuChildren()))
            m_tree.ApplyFilter(0, m_tree.Root(), rootFilter);
        else
            SgWarning() <<
                "SgUctSearch::StartSearch: "
                "root filter not applied (tree reached maximum size)\n";
    }
    m_gameLengthStat.Clear();
    m_movesInTreeStat.Clear();
    m_abortedStat.Clear();
    OnStartSearch();
    for (size_t i = 0; i < m_threads.size(); ++i)
        ThreadState(i).StartSearch();
}

string SgUctSearch::SummaryLine(const SgUctGameInfo& info) const
{
    ostringstream buffer;
    const vector<const SgUctNode*>& nodes = info.m_nodes;
    for (size_t i = 1; i < nodes.size(); ++i)
    {
        const SgUctNode* node = nodes[i];
        SgMove move = node->Move();
        buffer << ' ' << MoveString(move) << " (" << fixed << setprecision(2)
               << node->Mean() << ',' << node->MoveCount() << ')';
    }
    for (size_t i = 0; i < info.m_eval.size(); ++i)
        buffer << ' ' << fixed << setprecision(2) << info.m_eval[i];
    return buffer.str();
}

void SgUctSearch::UpdateCheckTimeInterval(double time)
{
    if (time < numeric_limits<double>::epsilon())
        return;
    // Dynamically update m_checkTimeInterval (see comment at definition of
    // m_checkTimeInterval)
    if (time < numeric_limits<double>::min())
    {
        m_checkTimeInterval *= 2;
        return;
    }
    m_gamesPerSecond = m_numberGames / time;
    double gamesPerSecondPerThread = m_gamesPerSecond / m_numberThreads;
    if (m_maxTime < 0.1)
        m_checkTimeInterval =
            static_cast<size_t>(m_maxTime * gamesPerSecondPerThread / 10);
    else
        m_checkTimeInterval =
            static_cast<size_t>(gamesPerSecondPerThread / 10);
    if (m_checkTimeInterval == 0)
        m_checkTimeInterval = 1;
}

/** Update the RAVE values in the tree for both players after a game was
    played.
    @see SgUctSearch::Rave()
*/
void SgUctSearch::UpdateRaveValues(SgUctThreadState& state)
{
    for (size_t i = 0; i < m_numberPlayouts; ++i)
        UpdateRaveValues(state, i);
}

void SgUctSearch::UpdateRaveValues(SgUctThreadState& state,
                                   std::size_t playout)
{
    SgUctGameInfo& info = state.m_gameInfo;
    const vector<SgMove>& sequence = info.m_sequence[playout];
    if (sequence.size() == 0)
        return;
    SG_ASSERT(m_moveRange > 0);
    size_t* firstPlay = state.m_firstPlay.get();
    size_t* firstPlayOpp = state.m_firstPlayOpp.get();
    fill_n(firstPlay, m_moveRange, numeric_limits<size_t>::max());
    fill_n(firstPlayOpp, m_moveRange, numeric_limits<size_t>::max());
    const vector<const SgUctNode*>& nodes = info.m_nodes;
    const vector<bool>& skipRaveUpdate = info.m_skipRaveUpdate[playout];
    float eval = info.m_eval[playout];
    float invEval = InverseEval(eval);
    size_t nuNodes = nodes.size();
    size_t i = sequence.size() - 1;
    bool opp = (i % 2 != 0);

    // Update firstPlay, firstPlayOpp arrays using playout moves
    for ( ; i >= nuNodes; --i)
    {
        SG_ASSERT(i < skipRaveUpdate.size());
        SG_ASSERT(i < sequence.size());
        if (! skipRaveUpdate[i])
        {
            SgMove mv = sequence[i];
            size_t& first = (opp ? firstPlayOpp[mv] : firstPlay[mv]);
            if (i < first)
                first = i;
        }
        opp = ! opp;
    }

    while (true)
    {
        SG_ASSERT(i < skipRaveUpdate.size());
        SG_ASSERT(i < sequence.size());
        // skipRaveUpdate currently not used in in-tree phase
        SG_ASSERT(i >= info.m_inTreeSequence.size() || ! skipRaveUpdate[i]);
        if (! skipRaveUpdate[i])
        {
            SgMove mv = sequence[i];
            size_t& first = (opp ? firstPlayOpp[mv] : firstPlay[mv]);
            if (i < first)
                first = i;
            if (opp)
                UpdateRaveValues(state, invEval, i, firstPlayOpp, firstPlay);
            else
                UpdateRaveValues(state, eval, i, firstPlay, firstPlayOpp);
        }
        if (i == 0)
            break;
        --i;
        opp = ! opp;
    }
}

void SgUctSearch::UpdateRaveValues(SgUctThreadState& state, float eval,
                                   std::size_t i,
                                   const std::size_t firstPlay[],
                                   const std::size_t firstPlayOpp[])
{
    SG_ASSERT(i < state.m_gameInfo.m_nodes.size());
    const SgUctNode* node = state.m_gameInfo.m_nodes[i];
    if (! node->HasChildren())
        return;
    for (SgUctChildIterator it(m_tree, *node); it; ++it)
    {
        const SgUctNode& child = *it;
        SgMove mv = child.Move();
        size_t first = firstPlay[mv];
        SG_ASSERT(first >= i);
        if (first == numeric_limits<size_t>::max())
            continue;
        if  (m_raveCheckSame && SgUtil::InRange(firstPlayOpp[mv], i, first))
            continue;
        m_tree.AddRaveValue(child, eval);
    }
}

void SgUctSearch::UpdateSignatures(const SgUctGameInfo& info)
{
    for (size_t playout = 0; playout < m_numberPlayouts; ++playout)
    {
        float eval = info.m_eval[playout];
        float invEval = InverseEval(eval);
        const vector<size_t>& signatures = info.m_signatures[playout];
        SG_ASSERT(signatures.size() == info.m_sequence[playout].size());
        for (size_t i = 0; i < signatures.size(); ++i)
        {
            size_t sig = signatures[i];
            if (sig != numeric_limits<size_t>::max())
            {
                SG_ASSERT(sig < m_signatureStat.size());
                m_signatureStat[sig].Add(eval);
            }
            swap(eval, invEval);
        }
    }
}

void SgUctSearch::UpdateStatistics(const SgUctGameInfo& info)
{
    m_movesInTreeStat.Add(static_cast<float>(info.m_inTreeSequence.size()));
    for (size_t i = 0; i < m_numberPlayouts; ++i)
    {
        m_gameLengthStat.Add(static_cast<float>(info.m_sequence[i].size()));
        m_abortedStat.Add(info.m_aborted[i] ? 1.f : 0.f);
    }
}

void SgUctSearch::UpdateTree(const SgUctGameInfo& info)
{
    // We count all playouts as one result
    float eval = 0;
    for (size_t i = 0; i < m_numberPlayouts; ++i)
        eval += info.m_eval[i];
    eval /= m_numberPlayouts;
    float inverseEval = InverseEval(eval);
    const vector<const SgUctNode*>& nodes = info.m_nodes;
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const SgUctNode& node = *nodes[i];
        const SgUctNode* father = (i > 0 ? nodes[i - 1] : 0);
        m_tree.AddGameResult(node, father, i % 2 == 0 ? eval : inverseEval);
    }
}

void SgUctSearch::WriteStatistics(ostream& out) const
{
    out << SgWriteLabel("Count") << m_tree.Root().MoveCount() << '\n'
        << SgWriteLabel("Nodes") << m_tree.NuNodes() << '\n'
        << fixed << setprecision(1)
        << SgWriteLabel("GameLength");
    m_gameLengthStat.Write(out);
    out << '\n'
        << SgWriteLabel("MovesInTree");
    m_movesInTreeStat.Write(out);
    out << '\n'
        << SgWriteLabel("Aborted")
        << static_cast<int>(100 * m_abortedStat.Mean()) << "%\n";
}

//----------------------------------------------------------------------------
