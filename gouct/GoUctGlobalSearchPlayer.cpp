//----------------------------------------------------------------------------
/** @file GoUctGlobalSearchPlayer.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctGlobalSearchPlayer.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include "GoBoardRestorer.h"
#include "GoBoardUtil.h"
#include "GoUctDefaultPlayoutPolicy.h"
#include "GoUctUtil.h"
#include "SgDebug.h"
#include "SgNbIterator.h"
#include "SgNode.h"
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

GoUctGlobalSearchPlayer::Statistics::Statistics()
{
    Clear();
}

void GoUctGlobalSearchPlayer::Statistics::Clear()
{
    m_nuGenMove = 0;
    m_gamesPerSecond.Clear();
    m_reuse.Clear();
}

void GoUctGlobalSearchPlayer::Statistics::Load(std::istream& in)
{
    string label;
    in >> label;
    if (label != "NU_COMPUTE_MOVE")
    {
        in.setstate(ios_base::failbit);
        return;
    }
    in >> m_nuGenMove;
    in >> label;
    if (label != "GAMES_PER_SEC")
    {
        in.setstate(ios_base::failbit);
        return;
    }
    m_gamesPerSecond.LoadFromText(in);
    in >> label;
    if (label != "REUSE")
    {
        in.setstate(ios_base::failbit);
        return;
    }
    m_reuse.LoadFromText(in);
}

void GoUctGlobalSearchPlayer::Statistics::Write(ostream& out) const
{
    out << SgWriteLabel("NuGenMove") << m_nuGenMove << '\n'
        << SgWriteLabel("GamesPerSec");
    m_gamesPerSecond.Write(out);
    out << '\n'
        << SgWriteLabel("Reuse");
    m_reuse.Write(out);
    out << '\n';
}

void GoUctGlobalSearchPlayer::Statistics::Save(std::ostream& out) const
{
    out << "NU_GEN_MOVE " << m_nuGenMove << '\n'
        << "GAMES_PER_SEC ";
    m_gamesPerSecond.SaveAsText(out);
    out << '\n'
        << "REUSE ";
    m_reuse.SaveAsText(out);
    out << '\n';
}

//----------------------------------------------------------------------------

GoUctGlobalSearchPlayer::GoUctGlobalSearchPlayer(GoBoard& bd)
    : GoPlayer(bd),
      m_searchMode(GOUCT_SEARCHMODE_UCT),
      m_ignoreClock(true),
      m_enablePonder(false),
      m_pruneRootMoves(false),
      m_reuseSubtree(false),
      m_maxTime(1e10),
      m_resignThreshold(0.03),
      m_priorKnowledge(GOUCT_PRIORKNOWLEDGE_POLICY),
      m_maxGames(100000),
      m_search(Board(),
               new GoUctDefaultPlayoutPolicyFactory<GoUctBoard>(
                                                      m_playoutPolicyParam)),
      m_treeValidForNode(0),
      m_timeControl(Board()),
      m_rootFilter(Board())
{
    m_timeControl.SetFastOpenMoves(0);
    m_timeControl.SetFinalSpace(1);
    m_timeControl.SetMinTime(0);
    m_timeControl.SetReserveMovesConstant(0.2);
    m_timeControl.SetRemainingConstant(0.3);
    SetPriorKnowledge(m_priorKnowledge);
}

GoUctGlobalSearchPlayer::~GoUctGlobalSearchPlayer()
{
}

void GoUctGlobalSearchPlayer::ClearStatistics()
{
    m_statistics.Clear();
}

SgMove GoUctGlobalSearchPlayer::GenMove(const SgTimeRecord& time,
                                        SgBlackWhite toPlay)
{
    ++m_statistics.m_nuGenMove;
    if (m_searchMode == GOUCT_SEARCHMODE_PLAYOUTPOLICY)
        return GenMovePlayoutPolicy(toPlay);
    SgMove move = SG_NULLMOVE;
    if (GoBoardUtil::PassWins(Board(), toPlay))
    {
        move = SG_PASS;
        SgDebug() <<
            "GoUctGlobalSearchPlayer::GenMove: "
            "Pass wins (Tromp-Taylor rules)\n";
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
        }
        else
        {
            SG_ASSERT(m_searchMode == GOUCT_SEARCHMODE_UCT);
            move = DoSearch(toPlay, maxTime);
            value = m_search.Tree().Root().Mean();
            m_statistics.m_gamesPerSecond.Add(m_search.GamesPerSecond());
            move = GoUctSearchUtil::TrompTaylorPassCheck(move, m_search);
        }
        if (move == SG_NULLMOVE)
        {
            // Shouldn't happen ?
            SgWarning() <<
                "GoUctGlobalSearchPlayer::GenMove: "
                "Search generated SG_NULLMOVE\n";
            move = SG_PASS;
        }
        else if (value < m_resignThreshold)
            move = SG_RESIGN;
    }
    SaveMonitorStatFile();
    return move;
}

SgMove GoUctGlobalSearchPlayer::GenMovePlayoutPolicy(SgBlackWhite toPlay)
{
    GoBoard& bd = Board();
    GoBoardRestorer restorer(bd);
    bd.SetToPlay(toPlay);
    SgBWSet safe; // Not used
    SgPointArray<bool> allSafe(false); // Not used
    GoUctDefaultPlayoutPolicy<GoBoard> policy(bd, m_playoutPolicyParam, safe,
                                              allSafe);
    policy.StartPlayout();
    SgPoint move = policy.GenerateMove();
    policy.EndPlayout();
    if (move == SG_NULLMOVE)
    {
        SgDebug() <<
            "GoUctGlobalSearchPlayer::GenMove: "
            "GoUctPlayoutPolicy generated SG_NULLMOVE\n";
        return SG_PASS;
    }
    return move;
}

/** Run the search for a given color.
    @param toPlay
    @param maxTime
    @return The best move or SG_NULLMOVE if terminal position
*/
SgPoint GoUctGlobalSearchPlayer::DoSearch(SgBlackWhite toPlay, double maxTime)
{
    SgUctTree* initTree = 0;
    if (m_reuseSubtree)
    {
        // FindInitTree can take measurable time for large trees
        SgTimer timer;
        FindInitTree(toPlay);
        initTree = &m_initTree;
        maxTime -= timer.GetTime();
    }
    m_search.SetToPlay(toPlay);
    vector<SgPoint> sequence;
    vector<SgMove> rootFilter;
    if (PruneRootMoves())
        rootFilter = m_rootFilter.Get();
    double value =
        m_search.Search(m_maxGames, maxTime, sequence, rootFilter, initTree);
    m_search.WriteStatistics(SgDebug());
    SgDebug() << fixed << setprecision(1) << "Games/s: "
              << m_search.GamesPerSecond() << '\n'
              << "Value: " << setprecision(2) << value << '\n'
              << "Sequence: " << SgWritePointList(sequence, "", false);

    //SgDebug() << "Move values: " << '\n';
    //GoUctUtil::WriteChildrenStatistics(*m_search, 0, SgDebug());

    m_treeValidForNode = CurrentNode();
    if (sequence.empty())
        return SG_NULLMOVE;
    return *(sequence.begin());
}

/** Find initial tree for search, if subtree reusing is enabled.
    Goes back in the tree until the node is found, the search tree is valid
    for and checks if the path of nodes corresponds to an alternating
    sequence of moves starting with the color to play of the search tree.
    @see SetReuseSubtree
*/
void GoUctGlobalSearchPlayer::FindInitTree(SgBlackWhite toPlay)
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

    if (m_treeValidForNode == 0)
    {
        SgDebug() <<
            "GoUctGlobalSearchPlayer::FindInitTree: "
            "No tree to reuse exists\n";
        return;
    }
    vector<SgMove> sequence;
    const SgNode* node = CurrentNode();
    while (node != m_treeValidForNode)
    {
        if (HasSetup(node))
        {
            SgDebug() <<
                "GoUctGlobalSearchPlayer::FindInitTree: "
                "Cannot reuse tree (setup stones in path to node)\n";
            return;
        }
        if (node->HasProp(SG_PROP_MOVE))
        {
            if (! HasMove(node, OppBW(toPlay)))
            {
                SgDebug() <<
                    "GoUctGlobalSearchPlayer::FindInitTree: "
                    "Cannot reuse tree (non-alternating moves in path)\n";
                return;
            }
            SgPoint p = node->NodeMove();
            SG_ASSERT(p != SG_NULLMOVE);
            sequence.insert(sequence.begin(), p);
            toPlay = OppBW(toPlay);
        }
        node = node->Father();
        if (node == 0)
        {
            SgDebug() <<
                "GoUctGlobalSearchPlayer::FindInitTree: "
                "No tree to reuse found\n";
            return;
        }
    }
    if (m_search.ToPlay() != toPlay)
    {
        SgDebug() <<
            "GoUctGlobalSearchPlayer::FindInitTree: "
            "Cannot reuse tree (search had different color)\n";
        return;
    }
    SgUctTreeUtil::ExtractSubtree(m_search.Tree(), m_initTree, sequence,
                                  true);
    size_t initTreeNodes = m_initTree.NuNodes();
    size_t oldTreeNodes = m_search.Tree().NuNodes();
    if (oldTreeNodes > 1 && initTreeNodes > 1)
    {
        float reuse = static_cast<float>(initTreeNodes) / oldTreeNodes;
        int reusePercent = static_cast<int>(100 * reuse);
        SgDebug() << "GoUctGlobalSearchPlayer::FindInitTree: Reusing "
                  << initTreeNodes << " nodes (" << reusePercent << "%)\n"
                  << SgWritePointList(sequence, "Sequence", false);
        m_statistics.m_reuse.Add(reuse);
    }
    else
    {
        SgDebug() <<
            "GoUctGlobalSearchPlayer::FindInitTree: "
            "Subtree to reuse has 0 nodes\n";
        m_statistics.m_reuse.Add(0.f);
    }
}

const GoUctGlobalSearchPlayer::Statistics&
GoUctGlobalSearchPlayer::GetStatistics() const
{
    return m_statistics;
}

string GoUctGlobalSearchPlayer::Name() const
{
    return "GoUctGlobalSearchPlayer";
}

void GoUctGlobalSearchPlayer::Ponder()
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
    SgDebug() << "GoUctGlobalSearchPlayer::Ponder Start\n";
    // Don't ponder forever to avoid hogging the machine
    double maxTime = 3600; // 60 min
    DoSearch(Board().ToPlay(), maxTime);
    SgDebug() << "GoUctGlobalSearchPlayer::Ponder End\n";
}

void GoUctGlobalSearchPlayer::SaveMonitorStatFile() const
{
    if (m_monitorStatFile == "")
        return;
    ofstream out(m_monitorStatFile.c_str());
    m_statistics.Save(out);
    if (! out)
        SgDebug() << "GoUctGlobalSearchPlayer::SetMonitorStatFile: "
            "failed to save '" << m_monitorStatFile << "'\n";
}

GoUctSearch& GoUctGlobalSearchPlayer::Search()
{
    return m_search;
}

const GoUctSearch& GoUctGlobalSearchPlayer::Search() const
{
    return m_search;
}

void GoUctGlobalSearchPlayer::SetMaxNodes(std::size_t maxNodes)
{
    m_search.SetMaxNodes(maxNodes);
    if (m_reuseSubtree)
        m_initTree.SetMaxNodes(maxNodes);
}

void GoUctGlobalSearchPlayer::SetMonitorStatFile(const std::string& fileName)
{
    if (fileName == m_monitorStatFile)
        return;
    m_monitorStatFile = fileName;
    if (fileName == "")
        return;
    ifstream in(fileName.c_str());
    if (! in.is_open())
        return;
    SgDebug() << "GoUctGlobalSearchPlayer::SetMonitorStatFile: Reading '"
              << fileName << "'\n";
    m_statistics.Load(in);
    if (! in)
        SgDebug() << "GoUctGlobalSearchPlayer::SetMonitorStatFile: "
            "failed to read file\n";
}

void GoUctGlobalSearchPlayer::SetPriorKnowledge(GoUctGlobalSearchPrior prior)
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
    case GOUCT_PRIORKNOWLEDGE_POLICY:
        factory = new GoUctPolicyPriorKnowledgeFactory(m_playoutPolicyParam);
        break;
    default:
        SG_ASSERT(false);
        factory = 0;
    }
    m_search.SetPriorKnowledge(factory);
    m_priorKnowledge = prior;
}

void GoUctGlobalSearchPlayer::SetReuseSubtree(bool enable)
{
    if (m_reuseSubtree && ! enable)
        // Free some memory, if initTree is no longer used
        m_initTree.SetMaxNodes(0);
    m_reuseSubtree = enable;
}

SgDefaultTimeControl& GoUctGlobalSearchPlayer::TimeControl()
{
    return m_timeControl;
}

const SgDefaultTimeControl& GoUctGlobalSearchPlayer::TimeControl() const
{
    return m_timeControl;
}

//----------------------------------------------------------------------------
