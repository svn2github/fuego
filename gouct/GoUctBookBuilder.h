//----------------------------------------------------------------------------
/** @file GoUctBookBuilder.h */
//----------------------------------------------------------------------------

#ifndef GOBOOKBUILDER_H
#define GOBOOKBUILDER_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <set>
#include "SgBookBuilder.h"
#include "SgThreadedWorker.h"
#include "GoBoard.h"
#include "GoAutoBook.h"
#include "GoBoardSynchronizer.h"

//----------------------------------------------------------------------------

/** Expands a Book using the given player to evaluate game positions.
    Supports multithreaded evaluation of children.
    @todo Copy settings from passed player to other players. */
template<class PLAYER>
class GoUctBookBuilder : public SgBookBuilder
{
public:
    GoUctBookBuilder(const GoBoard& brd);
    
    ~GoUctBookBuilder();

    //---------------------------------------------------------------------

    /** Sets the player to use. Settings are copied from this player
        to the players used for each thread.
        @todo Currently not used! Since it is not clear how to copy
        settings from one player to another nicely. */
    void SetPlayer(PLAYER& player);

    /** Sets the state to start work from. */
    void SetState(GoAutoBook& book);

    //---------------------------------------------------------------------    

    /** Number of workers to use during leaf expansion. Each worker
        may use a multi-threaded search. Should speed up the expansion
        of leaf states by a factor of (very close to) NumWorkers(). */
    std::size_t NumWorkers() const;

    /** See NumWorkers() */
    void SetNumWorkers(std::size_t num);

    /** Number of threads to use for each search */
    std::size_t NumThreadsPerWorker() const;

    /** See NumThreadsPerWorker() */
    void SetNumThreadsPerWorker(std::size_t num);

    /** Maximum amount of memory to use for the search.  The memory
        will be divided equally among all the workers. */
    std::size_t MaxMemory() const;

    /** See MaxMemory */
    void SetMaxMemory(std::size_t memory);

    /** Number of games to play when evaluation a state. */
    SgUctValue NumGamesPerEvaluation() const;

    /** See NumGamesPerEvaluation(). */
    void SetNumGamesPerEvaluation(SgUctValue num);

    /** Number of games to play when sorting children. */
    SgUctValue NumGamesPerSort() const;

    /** See NumGamesForSort() */
    void SetNumGamesPerSort(SgUctValue num);

    //---------------------------------------------------------------------    

    float InverseEval(float eval) const;

    bool IsLoss(float eval) const;

    float Value(const SgBookNode& node) const;

protected:
    std::string MoveString(SgMove move) const;

    void PrintMessage(std::string msg);

    void PlayMove(SgMove move);

    void UndoMove(SgMove move);

    bool GetNode(SgBookNode& node) const;

    void WriteNode(const SgBookNode& node);

    void FlushBook();

    void EnsureRootExists();

    bool GenerateMoves(std::size_t count, std::vector<SgMove>& moves, 
                       float& value);

    void GetAllLegalMoves(std::vector<SgMove>& moves);

    void EvaluateChildren(const std::vector<SgMove>& childrenToDo,
                          std::vector<std::pair<SgMove, float> >& scores);
    void Init();

    void StartIteration();

    void EndIteration();

    void BeforeEvaluateChildren();

    void AfterEvaluateChildren();

    void Fini();

    void ClearAllVisited();
    
    void MarkAsVisited();
    
    bool HasBeenVisited();
        
private:
    /** Copyable worker. */
    class Worker
    {
    public:
        Worker(std::size_t id, PLAYER& player);

        float operator()(const SgMove& move);

    private:
        std::size_t m_id;
        
        PLAYER* m_player;
    };

    /** Book this builder is expanding */
    GoAutoBook* m_book;
   
    PLAYER* m_origPlayer;

    GoAutoBookState m_state;

    std::set<SgHashCode> m_visited;

    /** See MaxMemory() */
    std::size_t m_maxMemory;

    /** See NumWorkers() */
    std::size_t m_numWorkers;

    /** See NumThreadsPerWorker() */
    std::size_t m_numThreadsPerWorker;

    /** See NumGamesPerEvaluation. */
    SgUctValue m_numGamesPerEvaluation;

    /** See NumGamesForSort() */
    SgUctValue m_numGamesPerSort;

    std::size_t m_numEvals;

    std::size_t m_numWidenings;

    std::size_t m_valueUpdates;

    std::size_t m_priorityUpdates;

    std::size_t m_internalNodes;

    std::size_t m_leafNodes;

    std::size_t m_terminalNodes;

    /** Players for each thread. */
    std::vector<PLAYER*> m_players;

    /** Workers for each thread. */
    std::vector<Worker> m_workers;

    SgThreadedWorker<SgMove,float,Worker>* m_threadedWorker;

    void CreateWorkers();

    void DestroyWorkers();
};

//----------------------------------------------------------------------------

template<class PLAYER>
inline std::size_t GoUctBookBuilder<PLAYER>::NumWorkers() const
{
    return m_numWorkers;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetNumWorkers(std::size_t num)
{
    m_numWorkers = num;
}

template<class PLAYER>
inline std::size_t GoUctBookBuilder<PLAYER>::MaxMemory() const
{
    return m_maxMemory;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetMaxMemory(std::size_t memory)
{
    m_maxMemory = memory;
}

template<class PLAYER>
inline std::size_t GoUctBookBuilder<PLAYER>::NumThreadsPerWorker() const
{
    return m_numThreadsPerWorker;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetNumThreadsPerWorker(std::size_t num)
{
    m_numThreadsPerWorker = num;
}

template<class PLAYER>
inline SgUctValue GoUctBookBuilder<PLAYER>::NumGamesPerEvaluation() const
{
    return m_numGamesPerEvaluation;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetNumGamesPerEvaluation(SgUctValue num)
{
    m_numGamesPerEvaluation = num;
}

template<class PLAYER>
inline SgUctValue GoUctBookBuilder<PLAYER>::NumGamesPerSort() const
{
    return m_numGamesPerSort;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetNumGamesPerSort(SgUctValue num)
{
    m_numGamesPerSort = num;
}

//----------------------------------------------------------------------------

template<class PLAYER>
GoUctBookBuilder<PLAYER>::GoUctBookBuilder(const GoBoard& bd)
    : SgBookBuilder(), 
      m_book(0),
      m_origPlayer(0),
      m_state(bd),
      m_maxMemory(8500000 * 2 * sizeof(SgUctNode)),
      m_numWorkers(1),
      m_numThreadsPerWorker(1),
      m_numGamesPerEvaluation(10000),
      m_numGamesPerSort(10000)
{
    SetAlpha(30.0);
    SetExpandWidth(8);
}

template<class PLAYER>
GoUctBookBuilder<PLAYER>::~GoUctBookBuilder()
{ }

//----------------------------------------------------------------------------

/** Copies the player and board and creates the threads. */
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::CreateWorkers()
{
    PrintMessage("GoUctBookBuilder::CreateWorkers()\n");
    for (std::size_t i = 0; i < m_numWorkers; ++i)
    {
        PLAYER* newPlayer = new PLAYER(m_state.Board());

        // TODO: COPY SETTINGS SOMEHOW
        //newPlayer->CopySettingsFrom(m_origPlayer);
        
        // Always search, don't use forced moves
        newPlayer->SetForcedOpeningMoves(false);
        // Ensure all games are played; ie, do not use early count abort.
        newPlayer->Search().SetMoveSelect(SG_UCTMOVESELECT_ESTIMATE);
        newPlayer->Search().SetMaxNodes(m_maxMemory / (m_numWorkers * 2 * sizeof(SgUctNode)));
        newPlayer->Search().SetNumberThreads(m_numThreadsPerWorker);
        newPlayer->SetReuseSubtree(false);
        newPlayer->SetWriteDebugOutput(false);

        m_players.push_back(newPlayer);
        m_workers.push_back(Worker(i, *m_players[i]));
    }
    m_threadedWorker 
        = new SgThreadedWorker<SgMove,float,Worker>(m_workers);
}

/** Destroys copied players, boards, and threads. */
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::DestroyWorkers()
{
    PrintMessage("GoUctBookBuilder::DestroyWorkers()\n");
    for (std::size_t i = 0; i < m_numWorkers; ++i)
        delete m_players[i];
    delete m_threadedWorker;
    m_workers.clear();
    m_players.clear();
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::Init()
{
    CreateWorkers();
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::Fini()
{
    DestroyWorkers();
}

//----------------------------------------------------------------------------

template<class PLAYER>
GoUctBookBuilder<PLAYER>::Worker::Worker(std::size_t id, PLAYER& player)

    : m_id(id), 
      m_player(&player)
{ }

template<class PLAYER>
float GoUctBookBuilder<PLAYER>::Worker::operator()(const SgMove& move)
{
    m_player->UpdateSubscriber();
    if (move >= 0)
        m_player->Board().Play(move);
    m_player->GenMove(SgTimeRecord(true, 9999), m_player->Board().ToPlay());
    GoUctSearch& search 
        = dynamic_cast<GoUctSearch&>(m_player->Search());
    float score = static_cast<float>(search.Tree().Root().Mean());
    return score;
}

//----------------------------------------------------------------------------

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetPlayer(PLAYER& player)
{
    m_origPlayer = &player;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetState(GoAutoBook& book)
{
    m_book = &book;
    m_state.Synchronize();
}

template<class PLAYER>
inline std::string GoUctBookBuilder<PLAYER>::MoveString(SgMove move) const
{
    return SgPointUtil::PointToString(move);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::PrintMessage(std::string msg)
{
    SgDebug() << msg;
}

template<class PLAYER>
inline float GoUctBookBuilder<PLAYER>::InverseEval(float eval) const
{
    return 1.f - eval;
}

template<class PLAYER>
inline bool GoUctBookBuilder<PLAYER>::IsLoss(float eval) const
{
    return eval < -100;
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::PlayMove(SgMove move)
{
    m_state.Play(move);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::UndoMove(SgMove move)
{
    SG_UNUSED(move);
    m_state.Undo();
}

template<class PLAYER>
bool GoUctBookBuilder<PLAYER>::GetNode(SgBookNode& node) const
{
    return m_book->Get(m_state, node);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::WriteNode(const SgBookNode& node)
{
    m_book->Put(m_state, node);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::FlushBook()
{
    SgDebug() << "Flushing DB...\n";
    m_book->Flush();
}

template<class PLAYER>
float GoUctBookBuilder<PLAYER>::Value(const SgBookNode& node) const
{
    return node.m_value;
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::GetAllLegalMoves(std::vector<SgMove>& moves)
{
    for (GoBoard::Iterator it(m_state.Board()); it; ++it)
        if (m_state.Board().IsLegal(*it))
            moves.push_back(*it);
}

/** Creates root node if necessary. */
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::EnsureRootExists()
{
    SgBookNode root;
    if (! GetNode(root))
    {
        BeforeEvaluateChildren();
        PrintMessage("Creating root node...\n");
        float value = m_workers[0](SG_NULLMOVE);
        WriteNode(SgBookNode(value));
    }
}

/** Computes an ordered set of moves to consider. */
template<class PLAYER>
bool GoUctBookBuilder<PLAYER>::GenerateMoves(std::size_t count, 
                                             std::vector<SgMove>& moves,
                                             float& value)
{
    SG_UNUSED(count);
    SG_UNUSED(value);

    // Search for a few seconds.
    SgDebug() << m_state.Board() << '\n';
    m_players[0]->SetMaxGames(m_numGamesPerSort);
    m_workers[0](SG_NULLMOVE);
    std::vector<std::pair<SgUctValue, SgMove> > ordered;
    // Store counts for each move in vector.
    {
        const SgUctTree& tree = m_players[0]->Search().Tree();
        const SgUctNode& root = tree.Root();
        for (GoBoard::Iterator it(m_state.Board()); it; ++it)
            if (m_state.Board().IsLegal(*it))
            {
                SgMove move = *it;
                const SgUctNode* node = 
                    SgUctTreeUtil::FindChildWithMove(tree, root, move);
                if (node && node->PosCount() > 0)
                {
                    ordered.push_back(std::make_pair(-node->PosCount(), move));
                }
            }
    }
    // Sort moves based on count of this search. 
    std::stable_sort(ordered.begin(), ordered.end());
    for (std::size_t i = 0; i < ordered.size(); ++i)
        moves.push_back(ordered[i].second);
    SgDebug() << '\n';
    return false;
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::BeforeEvaluateChildren()
{
    for (std::size_t i = 0; i < m_numWorkers; ++i)
        m_players[i]->SetMaxGames(m_numGamesPerEvaluation);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>
::EvaluateChildren(const std::vector<SgMove>& childrenToDo,
                   std::vector<std::pair<SgMove, float> >& scores)
{
    SgDebug() << "Evaluating children:";
    for (std::size_t i = 0; i < childrenToDo.size(); ++i)
        SgDebug() << ' ' << SgWritePoint(childrenToDo[i]);
    SgDebug() << '\n';
    m_threadedWorker->DoWork(childrenToDo, scores);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::AfterEvaluateChildren()
{ }

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::StartIteration()
{ }

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::EndIteration()
{ }

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::ClearAllVisited()
{
    m_visited.clear();
}
    
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::MarkAsVisited()
{
    m_visited.insert(m_state.GetHashCode());
}
    
template<class PLAYER>
bool GoUctBookBuilder<PLAYER>::HasBeenVisited()
{
    return m_visited.count(m_state.GetHashCode()) == 1;
}

//----------------------------------------------------------------------------

#endif // GOBOOKBUILDER_HPP
