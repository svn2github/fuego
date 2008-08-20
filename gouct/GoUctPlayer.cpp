//----------------------------------------------------------------------------
/** @file GoUctPlayer.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctPlayer.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include "GoBoardRestorer.h"
#include "GoBoardUtil.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctDefaultPriorKnowledge.h"
#include "GoUctDefaultRootFilter.h"
#include "GoUctUtil.h"
#include "SgDebug.h"
#include "SgNbIterator.h"
#include "SgNode.h"
#include "SgRestorer.h"
#include "SgSList.h"
#include "SgTime.h"
#include "SgTimer.h"
#include "SgUctPriorKnowledgeEven.h"
#include "SgUctTreeUtil.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

bool HasMove(const SgNode* node, SgBlackWhite color)
{
    return ((color == SG_BLACK && node->HasProp(SG_PROP_MOVE_BLACK))
            || (color == SG_WHITE && node->HasProp(SG_PROP_MOVE_WHITE)));
}

bool HasSetup(const SgNode* node)
{
    return (node->HasProp(SG_PROP_ADD_BLACK)
            || node->HasProp(SG_PROP_ADD_WHITE)
            || node->HasProp(SG_PROP_ADD_EMPTY));
}

} // namespace

//----------------------------------------------------------------------------

GoUctPlayer::Statistics::Statistics()
{
    Clear();
}

void GoUctPlayer::Statistics::Clear()
{
    m_nuGenMove = 0;
    m_gamesPerSecond.Clear();
    m_reuse.Clear();
}

void GoUctPlayer::Statistics::Write(ostream& out) const
{
    out << SgWriteLabel("NuGenMove") << m_nuGenMove << '\n'
        << SgWriteLabel("GamesPerSec");
    m_gamesPerSecond.Write(out);
    out << '\n'
        << SgWriteLabel("Reuse");
    m_reuse.Write(out);
    out << '\n';
}

//----------------------------------------------------------------------------

GoUctPlayer::GoUctPlayer(GoBoard& bd)
    : GoPlayer(bd),
      m_searchMode(GOUCT_SEARCHMODE_UCT),
      m_autoParam(true),
      m_ignoreClock(true),
      m_enablePonder(false),
      m_useRootFilter(true),
      m_reuseSubtree(false),
      m_earlyPass(true),
      m_maxTime(1e10),
      m_resignThreshold(0.04),
      m_lastBoardSize(-1),
      m_priorKnowledge(GOUCT_PRIORKNOWLEDGE_DEFAULT),
      m_maxGames(100000),
      m_search(Board(),
               new GoUctPlayoutPolicyFactory<GoUctBoard>(
                                                      m_playoutPolicyParam)),
      m_timeControl(Board()),
      m_rootFilter(new GoUctDefaultRootFilter(Board()))
{
    m_timeControl.SetFastOpenMoves(0);
    m_timeControl.SetFinalSpace(1);
    m_timeControl.SetMinTime(0);
    m_timeControl.SetReserveMovesConstant(0.2);
    m_timeControl.SetRemainingConstant(0.3);
    SetPriorKnowledge(m_priorKnowledge);
}

GoUctPlayer::~GoUctPlayer()
{
}

void GoUctPlayer::ClearStatistics()
{
    m_statistics.Clear();
}

/** Perform a search after playing a pass and see if it is still a win and
    all points are safe as determined by territory statistics.
    @param maxGames Maximum simulations for the search
    @param maxTime Maximum time for the search
    @param[out] move The move to play (pass or a neutral point to fill)
    @return @true, if it is still a win and everything is safe after a pass
*/
bool GoUctPlayer::DoEarlyPassSearch(size_t maxGames, double maxTime,
                                    SgPoint& move)
{
    SgDebug() << "GoUctPlayer: doing a search if early pass is possible\n";
    GoBoard& bd = Board();
    bd.Play(SG_PASS);
    SgRestorer<bool> restorer(&m_search.m_param.m_territoryStatistics);
    m_search.m_param.m_territoryStatistics = true;
    vector<SgPoint> sequence;
    double value = m_search.Search(maxGames, maxTime, sequence);
    value = m_search.InverseEval(value);
    bd.Undo();
    if (value < 1 - m_resignThreshold)
    {
        SgDebug() << "GoUctPlayer: no early pass possible (no win)\n";
        return false;
    }
    move = SG_PASS;
    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >& threadState =
        dynamic_cast<
             GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&>(
                                                     m_search.ThreadState(0));
    SgPointArray<SgUctStatistics> territory =
        threadState.m_territoryStatistics;
    for (GoBoard::Iterator it(bd); it; ++it)
        if (territory[*it].Count() == 0)
        {
            // No statistics, maybe all simulations aborted due to
            // max length or mercy rule.
            SgDebug() << "GoUctPlayer: no early pass possible (no stat)\n";
            return false;
        }
    const float threshold = 0.2; // Safety threshold
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        float mean = territory[*it].Mean();
        if (mean > threshold && mean < 1 - threshold)
        {
            // Check if neutral point
            bool isSafeToPlayAdj = false;
            bool isSafeOppAdj = false;
            for (SgNb4Iterator it2(*it); it2; ++it2)
                if (! bd.IsBorder(*it2))
                {
                    if (mean < threshold)
                        isSafeToPlayAdj = true;
                    if (mean > 1 - threshold)
                        isSafeOppAdj = true;
                }
            if (isSafeToPlayAdj && isSafeOppAdj)
                move = *it;
            else
            {
                SgDebug()
                    << "GoUctPlayer: no early pass possible (unsafe point)\n";
                return false;
            }
        }
    }
    if (move == SG_PASS)
        SgDebug() << "GoUctPlayer: early pass is possible\n";
    else
        SgDebug() << "GoUctPlayer: generate play on neutral point\n";
    return true;
}

/** Run the search for a given color.
    @param toPlay
    @param maxTime
    @param isDuringPondering Hint that search is done during pondering (this
    handles the decision to discard an aborted FindInitTree differently)
    @return The best move or SG_NULLMOVE if terminal position (can also
    happen, if @c isDuringPondering, no search was performed, because
    DoSearch() was aborted during FindInitTree()).
*/
SgPoint GoUctPlayer::DoSearch(SgBlackWhite toPlay, double maxTime,
                              bool isDuringPondering)
{
    SgUctTree* initTree = 0;
    SgTimer timer;
    double timeInitTree = 0;
    if (m_reuseSubtree)
    {
        timeInitTree = -timer.GetTime();
        FindInitTree(toPlay, maxTime);
        timeInitTree += timer.GetTime();
        initTree = &m_initTree;
        if (SgUserAbort() && isDuringPondering)
            // If abort occurs during pondering, better don't start a search
            // with a truncated init tree. The search would be aborted after
            // one game anyway, because it also checks SgUserAbort(). There is
            // a higher chance to reuse a larger part of the current tree in
            // the next regular move search.
            return SG_NULLMOVE;
    }
    vector<SgMove> rootFilter;
    double timeRootFilter = 0;
    if (m_useRootFilter)
    {
        timeRootFilter = -timer.GetTime();
        rootFilter = m_rootFilter->Get();
        timeRootFilter += timer.GetTime();
    }
    maxTime -= timer.GetTime();
    m_search.SetToPlay(toPlay);
    vector<SgPoint> sequence;
    const bool earlyAbort = true;
    const float earlyAbortThreshold = 1 - m_resignThreshold;
    float value = m_search.Search(m_maxGames, maxTime, sequence, rootFilter,
                                  initTree, earlyAbort, earlyAbortThreshold);

    // Write debug output to a string stream first to avoid intermingling
    // of debug output with response in GoGui GTP shell
    ostringstream out;
    m_search.WriteStatistics(out);
    out << SgWriteLabel("Value") << fixed << setprecision(2) << value << '\n'
        << SgWriteLabel("Sequence") << SgWritePointList(sequence, "", false);
    if (m_reuseSubtree)
        out << SgWriteLabel("TimeInitTree") << fixed << setprecision(2)
            << timeInitTree << '\n';
    if (m_useRootFilter)
        out << SgWriteLabel("TimeRootFilter") << fixed << setprecision(2)
            << timeRootFilter << '\n';
    SgDebug() << out.str();

    if (value < m_resignThreshold)
        return SG_RESIGN;

    SgPoint move;
    if (sequence.empty())
        move = SG_PASS;
    else
    {
        move = *(sequence.begin());
        move = GoUctSearchUtil::TrompTaylorPassCheck(move, m_search);
    }

    // If SgUctSearch aborted after half the time/nodes, because of early
    // abort, we use the remaining time/nodes for doing a search, if an early
    // pass is possible
    if (m_search.WasEarlyAbort())
    {
        maxTime -= timer.GetTime();
        SgPoint earlyPassMove;
        if (DoEarlyPassSearch(m_maxGames / 2, maxTime, earlyPassMove))
            return earlyPassMove;
    }

    return move;
}

/** Find initial tree for search, if subtree reusing is enabled.
    Goes back in the tree until the node is found, the search tree is valid
    for and checks if the path of nodes corresponds to an alternating
    sequence of moves starting with the color to play of the search tree.
    @see SetReuseSubtree
*/
void GoUctPlayer::FindInitTree(SgBlackWhite toPlay, double maxTime)
{
    m_initTree.Clear();
    // Make sure that tree has same number of allocators and max nodes
    // as m_search.Tree() (such that it can be swapped with m_search.Tree()).
    // Use m_search.NumberThreads() (not m_search.Tree().NuAllocators()) and
    // m_search.MaxNodes() (not m_search.Tree().MaxNodes()), because of the
    // delayed thread (and thereby allocator) creation in m_search
    if (m_initTree.NuAllocators() != m_search.NumberThreads())
        m_initTree.CreateAllocators(m_search.NumberThreads());
    if (m_initTree.MaxNodes() != m_search.MaxNodes())
        m_initTree.SetMaxNodes(m_search.MaxNodes());

    Board().SetToPlay(toPlay);
    GoBoardHistory currentPosition;
    currentPosition.SetFromBoard(Board());
    vector<SgPoint> sequence;
    if (! currentPosition.IsAlternatePlayFollowUpOf(m_search.BoardHistory(),
                                                    sequence))
    {
        SgDebug() << "GoUctPlayer: No tree to reuse found\n";
        return;
    }
    SgUctTreeUtil::ExtractSubtree(m_search.Tree(), m_initTree, sequence,
                                  true, maxTime);
    size_t initTreeNodes = m_initTree.NuNodes();
    size_t oldTreeNodes = m_search.Tree().NuNodes();
    if (oldTreeNodes > 1 && initTreeNodes > 1)
    {
        float reuse = static_cast<float>(initTreeNodes) / oldTreeNodes;
        int reusePercent = static_cast<int>(100 * reuse);
        SgDebug() << "GoUctPlayer: Reusing " << initTreeNodes
                  << " nodes (" << reusePercent << "%)\n";

        //SgDebug() << SgWritePointList(sequence, "Sequence", false);
        m_statistics.m_reuse.Add(reuse);
    }
    else
    {
        SgDebug() << "GoUctPlayer:: Subtree to reuse has 0 nodes\n";
        m_statistics.m_reuse.Add(0.f);
    }

    // Check consistency
    for (SgUctChildIterator it(m_initTree, m_initTree.Root()); it; ++it)
        if (! Board().IsLegal((*it).Move()))
        {
            SgWarning() <<
                "GoUctPlayer: illegal move in root child of init tree\n";
            m_initTree.Clear();
            // Should not happen, if no bugs
            assert(false);
        }
}

SgPoint GoUctPlayer::GenMove(const SgTimeRecord& time, SgBlackWhite toPlay)
{
    ++m_statistics.m_nuGenMove;
    if (m_searchMode == GOUCT_SEARCHMODE_PLAYOUTPOLICY)
        return GenMovePlayoutPolicy(toPlay);
    SgMove move = SG_NULLMOVE;
    if (GoBoardUtil::PassWins(Board(), toPlay))
    {
        move = SG_PASS;
        SgDebug() << "GoUctPlayer: Pass wins (Tromp-Taylor rules)\n";
    }
    else
    {
        double maxTime;
        if (m_ignoreClock)
            maxTime = m_maxTime;
        else
            maxTime = min(m_maxTime, m_timeControl.TimeForCurrentMove(time));
        float value;
        if (m_searchMode == GOUCT_SEARCHMODE_ONEPLY)
        {
            m_search.SetToPlay(toPlay);
            move = m_search.SearchOnePly(m_maxGames, maxTime, value);
            if (move == SG_NULLMOVE)
                move = SG_PASS;
            else
            {
                float value = m_search.Tree().Root().Mean();
                if (value < m_resignThreshold)
                    move = SG_RESIGN;
            }
        }
        else
        {
            SG_ASSERT(m_searchMode == GOUCT_SEARCHMODE_UCT);
            move = DoSearch(toPlay, maxTime, false);
            m_statistics.m_gamesPerSecond.Add(
                                      m_search.Statistics().m_gamesPerSecond);
        }
    }
    return move;
}

SgMove GoUctPlayer::GenMovePlayoutPolicy(SgBlackWhite toPlay)
{
    GoBoard& bd = Board();
    GoBoardRestorer restorer(bd);
    bd.SetToPlay(toPlay);
    if (m_playoutPolicy.get() == 0)
        m_playoutPolicy.reset(
            new GoUctPlayoutPolicy<GoBoard>(bd, m_playoutPolicyParam));
    m_playoutPolicy->StartPlayout();
    SgPoint move = m_playoutPolicy->GenerateMove();
    m_playoutPolicy->EndPlayout();
    if (move == SG_NULLMOVE)
    {
        SgDebug() <<
            "GoUctPlayer: GoUctPlayoutPolicy generated SG_NULLMOVE\n";
        return SG_PASS;
    }
    return move;
}

const GoUctPlayer::Statistics& GoUctPlayer::GetStatistics() const
{
    return m_statistics;
}

string GoUctPlayer::Name() const
{
    return "GoUctPlayer";
}

void GoUctPlayer::OnBoardChange()
{
    int size = Board().Size();
    if (m_autoParam && size != m_lastBoardSize)
    {
        SgDebug() << "GoUctPlayer: Setting default parameters for size "
                  << size << '\n';
        m_search.SetDefaultParameters(size);
        m_lastBoardSize = size;
    }
}

void GoUctPlayer::Ponder()
{
    if (! m_enablePonder || GoBoardUtil::EndOfGame(Board())
        || m_searchMode != GOUCT_SEARCHMODE_UCT)
        return;
    if (! m_reuseSubtree)
    {
        // Don't ponder, wouldn't use the result in the next GenMove
        // anyway if reuseSubtree is not enabled
        SgWarning() << "Pondering needs reuse_subtree enabled.\n";
        return;
    }
    SgDebug() << "GoUctPlayer::Ponder: start\n";
    // Don't ponder forever to avoid hogging the machine
    double maxTime = 3600; // 60 min
    DoSearch(Board().ToPlay(), maxTime, true);
    SgDebug() << "GoUctPlayer::Ponder: end\n";
}

GoUctSearch& GoUctPlayer::Search()
{
    return m_search;
}

const GoUctSearch& GoUctPlayer::Search() const
{
    return m_search;
}

void GoUctPlayer::SetMaxNodes(std::size_t maxNodes)
{
    m_search.SetMaxNodes(maxNodes);
    if (m_reuseSubtree)
        m_initTree.SetMaxNodes(maxNodes);
}

void GoUctPlayer::SetPriorKnowledge(GoUctGlobalSearchPrior prior)
{
    SgUctPriorKnowledgeFactory* factory;
    switch (prior)
    {
    case GOUCT_PRIORKNOWLEDGE_NONE:
        factory = 0;
        break;
    case GOUCT_PRIORKNOWLEDGE_EVEN:
        factory = new SgUctPriorKnowledgeEvenFactory(30);
        break;
    case GOUCT_PRIORKNOWLEDGE_DEFAULT:
        factory = new GoUctDefaultPriorKnowledgeFactory(m_playoutPolicyParam);
        break;
    default:
        SG_ASSERT(false);
        factory = 0;
    }
    m_search.SetPriorKnowledge(factory);
    m_priorKnowledge = prior;
}

void GoUctPlayer::SetReuseSubtree(bool enable)
{
    if (m_reuseSubtree && ! enable)
        // Free some memory, if initTree is no longer used
        m_initTree.SetMaxNodes(0);
    m_reuseSubtree = enable;
}

SgDefaultTimeControl& GoUctPlayer::TimeControl()
{
    return m_timeControl;
}

const SgDefaultTimeControl& GoUctPlayer::TimeControl() const
{
    return m_timeControl;
}

//----------------------------------------------------------------------------
