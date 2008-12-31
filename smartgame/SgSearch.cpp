//----------------------------------------------------------------------------
/** @file SgSearch.cpp
    See SgSearch.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgSearch.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <math.h>
#include "SgDebug.h"
#include "SgHashTable.h"
#include "SgList.h"
#include "SgMath.h"
#include "SgNode.h"
#include "SgTime.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

const bool DEBUG_SEARCH = false;
const bool DEBUG_SEARCH_ITERATIONS = false;

//----------------------------------------------------------------------------

/** Set '*s' to the string for this value.
    e.g. "B+3.5", "W+20", or "W+(ko)[12]". The value is divided by
    'unitPerPoint' to determine the number of points.
*/
string SgValue::ToString(int unitPerPoint) const
{
    if (m_v == 0)
        return "0";
    ostringstream o;
    o << (m_v > 0 ? "B+" : "W+");
    if (IsEstimate())
    {
        if (unitPerPoint == 1)
            o << (abs(m_v) / unitPerPoint);
        else
            o << setprecision(1)
              << (static_cast<float>(abs(m_v)) / unitPerPoint);
    }
    else
    {
        if (KoLevel() != 0)
            o << "(ko)";
        if (Depth() != 0)
        {
            o << " (" << Depth() << " moves)";
        }
    }
    return o.str();
}

bool SgValue::FromString(const string& s)
{
    SG_UNUSED(s);
    SG_ASSERT(false); // AR: not yet implemented
    return false;
}

int SgValue::KoLevel() const
{
    if (IsEstimate())
        return 0;
    else
    {
        int level = (abs(m_v) - 1) / MAX_DEPTH;
        return (MAX_LEVEL - 1) - level;
    }
}

//----------------------------------------------------------------------------

void SgKiller::MarkKiller(SgMove killer)
{
    if (killer == m_killer1)
        ++m_count1;
    else if (killer == m_killer2)
    {
        ++m_count2;
        if (m_count1 <= m_count2)
        {
            swap(m_killer1, m_killer2);
            swap(m_count1, m_count2);
        }
    }
    else if (m_killer1 == SG_NULLMOVE)
    {
        m_killer1 = killer;
        m_count1 = 1;
    }
    else
    {
        m_killer2 = killer;
        m_count2 = 1;
    }
}

void SgKiller::Clear()
{
    m_killer1 = SG_NULLMOVE;
    m_count1 = 0;
    m_killer2 = SG_NULLMOVE;
    m_count2 = 0;
}

//----------------------------------------------------------------------------

bool SgSearchHashData::IsBetterThan(const SgSearchHashData& data) const
{
    if (m_depth > data.m_depth)
        return true;
    if (m_depth < data.m_depth)
        return false;
    return (! m_isUpperBound && ! m_isLowerBound)
        || (m_isLowerBound && data.m_isLowerBound && m_value > data.m_value)
        || (m_isUpperBound && data.m_isUpperBound && m_value < data.m_value);
}

//----------------------------------------------------------------------------

namespace {

void WriteMoves(const SgSearch& search, const SgList<SgMove>& sequence)
{
    for (SgListIterator<SgMove> it(sequence); it; ++it)
        SgDebug() << ' ' << search.MoveString(*it);
}

void PrintPV(const SgSearch& search, int depth, int value,
             const SgList<SgMove>& sequence,
             bool isExactValue)
{
    SgDebug() << "Iteration d = " << depth
        << ", value = " << value
        << ", exact = " << isExactValue
        << ", sequence = ";
    WriteMoves(search, sequence);
    SgDebug() << '\n';
}

} // namespace

//----------------------------------------------------------------------------

const int SgSearch::SG_INFINITY = numeric_limits<int>::max();

SgSearch::SgSearch(SgSearchHashTable* hash)
    : m_traceNode(0),
      m_hash(hash),
      m_currentDepth(0),
      m_useScout(false),
      m_useKillers(false),
      m_useOpponentBest(0),
      m_useNullMove(0),
      m_nullMoveDepth(2),
      m_aborted(false),
      m_foundNewBest(false),
      m_reachedDepthLimit(false),
      m_mustReturnExactResult(false),
      m_stat(),
      m_timerLevel(0),
      m_control(0),
      m_probcut(0),
      m_abortFrequency(1)
{
    InitSearch();
}

SgSearch::~SgSearch()
{
}

void SgSearch::CallGenerate(SgList<SgMove>* moves, int depth)
{
    Generate(moves, depth);
    if (DEBUG_SEARCH)
    {
        SgDebug() << "SgSearch::CallGenerate: d=" << depth;
        WriteMoves(*this, *moves);
        SgDebug() << '\n';
    }
}

void SgSearch::InitSearch(int startDepth)
{
    m_currentDepth = startDepth;
    m_moveStack.Clear();
    m_moveStack.Push(SG_NULLMOVE);
    m_moveStack.Push(SG_NULLMOVE);
    if (m_useKillers)
    {
        for (int i = 0; i <= MAX_KILLER_DEPTH; ++i)
            m_killers[i].Clear();
    }
}

bool SgSearch::LookupHash(SgSearchHashData& data) const
{
    SG_ASSERT(! data.IsValid());
    if (m_hash == 0 || ! m_hash->Lookup(GetHashCode(), &data))
        return false;
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::LookupHash: " << GetHashCode() << ' '
                  << MoveString(data.BestMove()) << " exact="
                  << data.IsExactValue() << ", v=" << data.Value() << '\n';
    return true;
}

/** Move killer moves to the front.
    Add them to the list of moves if they were not otherwise generated.
*/
void SgSearch::MoveKillersToFront(SgList<SgMove>& moves)
{
    if (m_useKillers && m_currentDepth <= MAX_KILLER_DEPTH)
    {
        SgMove killer2 = m_killers[m_currentDepth].GetKiller2();
        if (killer2 != SG_NULLMOVE)
        {
            moves.Exclude(killer2);
            moves.Push(killer2);
        }
        SgMove killer1 = m_killers[m_currentDepth].GetKiller1();
        if (killer1 != SG_NULLMOVE)
        {
            moves.Exclude(killer1);
            moves.Push(killer1);
        }
    }
}

void SgSearch::OnStartSearch()
{
    // Default implementation does nothing
}

void SgSearch::SetSearchControl(SgSearchControl* control)
{
    m_control = control;
}

void SgSearch::SetProbCut(SgProbCut* probcut)
{
    m_probcut = probcut;
}

void SgSearch::StoreHash(int depth, int value, SgMove move,
                         bool isUpperBound, bool isLowerBound, bool isExact)
{
    SgSearchHashData data(depth, value, move, isUpperBound, isLowerBound,
                          isExact);
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::StoreHash: " << GetHashCode() << ": move="
                  << MoveString(move) << " exact=" << isExact << " v="
                  << value << '\n';
    m_hash->Store(GetHashCode(), data);
}

bool SgSearch::TraceIsOn() const
{
    return false;
}

bool SgSearch::AbortSearch()
{
    if (! m_aborted)
    {
        // Abortion checking is potentially expensive
        // Only check for abort every m_abortFrequency nodes
        if (m_stat.NumNodes() % m_abortFrequency != 0)
            return false;

        m_aborted =
            (m_control && m_control->Abort(m_timer.GetTime(), NumNodes()));
        if (! m_aborted)
        {
            if (SgUserAbort())
                m_aborted = true;
        }
        if (m_aborted && m_traceNode)
            TraceComment("aborted");
    }
    return m_aborted;
}

bool SgSearch::ProbCut(int depth, int alpha, int beta,
                       SgList<SgMove>* sequence, bool* isExactValue,
                       int* value)
{
    SG_ASSERT(m_probcut);
    SG_ASSERT(m_probcut->IsEnabled());

    m_probcut->SetEnabled(false);

    SgProbCut::Cutoff c;
    int index = 0;
    while (m_probcut->GetCutoff(depth / DEPTH_UNIT, index++, c))
    {
        SgList<SgMove> seq;
        bool isExact;
        float threshold = m_probcut->GetThreshold();

        if (beta < SG_INFINITY-1)
        {
            float b = (+threshold * c.sigma + beta - c.b) / c.a;
            int bound = SgMath::RoundToInt(b);
            int res = SearchEngine(c.shallow * DEPTH_UNIT,
                                   bound-1, bound, &seq, &isExact);
            if (res >= bound)
            {
                m_probcut->SetEnabled(true);
                sequence->Concat(&seq);
                *isExactValue = isExact;
                *value = beta;
                return true;
            }
        }

        if (alpha > -SG_INFINITY + 1)
        {
            float b = (-threshold * c.sigma + alpha - c.b) / c.a;
            int bound = SgMath::RoundToInt(b);
            int res = SearchEngine(c.shallow * DEPTH_UNIT,
                                   bound, bound+1, &seq, &isExact);

            if (res <= bound)
            {
                m_probcut->SetEnabled(true);
                sequence->Concat(&seq);
                *isExactValue = isExact;
                *value = alpha;
                return true;
            }
        }
    }
    m_probcut->SetEnabled(true);
    return false;
}

bool SgSearch::NullMovePrune(int depth, int delta, int beta)
{
    SgList<SgMove> nullSeq;
    bool childIsExact = true;
    if (beta >= SG_INFINITY - 1)
        return false;
    if (CallExecute(SG_PASS, &delta, depth))
    {
        float nullvalue = -SearchEngine(depth - delta,
            -beta, -beta + 1, &nullSeq, &childIsExact, true);
        CallTakeBack();
        if (nullvalue >= beta)
        {
            if (m_traceNode)
                TraceComment("null-move-cut");
            return true;
        }
    }
    return false;
}

void SgSearch::GetStatistics(SgSearchStatistics* stat)
{
    m_stat.SetTimeUsed(m_timer.GetTime());
    *stat = m_stat;
}

void SgSearch::AddStatisticsTo(SgSearchStatistics* stat)
{
    SG_ASSERT(m_timer.IsStopped());
    m_stat.SetTimeUsed(m_timer.GetTime());
    *stat += m_stat;
}

void SgSearch::StartTime()
{
    if (m_timerLevel++ == 0)
    {
        m_stat.Clear();
        m_timer.Start();
    }
}

void SgSearch::StopTime()
{
    if (--m_timerLevel == 0)
    {
        if (! m_timer.IsStopped())
            m_timer.Stop();
    }
}

int SgSearch::NumNodes() const
{
    return m_stat.NumNodes();
}

int SgSearch::CallEvaluate(int depth, int alpha, int beta,
                           SgList<SgMove>* sequence, bool* isExact)
{
    // Alpha, beta could be needed in Evaluate. Not implemented yet.
    SG_UNUSED(alpha);
    SG_UNUSED(beta);
    int v = Evaluate(sequence, isExact, depth);
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::CallEvaluate d=" << depth << ", v=" << v
                  << '\n';
    return v;
}

bool SgSearch::CallExecute(SgMove move, int* delta, int depth)
{
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::CallExecute: d=" << depth << ' '
                  << SgBW(GetToPlay()) << ' ' << MoveString(move) << '\n';
    if (Execute(move, delta, depth))
    {
        m_stat.IncNumMoves();
        if (move == SG_PASS)
            m_stat.IncNumPassMoves();
        m_moveStack.Push(move);
        ++m_currentDepth;
        return true;
    }
    return false;
}

void SgSearch::CallTakeBack()
{
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::CallTakeBack\n";
    TakeBack();
    m_moveStack.Pop();
    --m_currentDepth;
}

void SgSearch::AddSequenceToHash(const SgList<SgMove>& sequence, int depth)
{
    if (! m_hash)
        return;
    int numMovesToUndo = 0;
    for (SgListIterator<SgMove> iter(sequence); iter; ++iter)
    {
        SgMove move = *iter;
        int delta = DEPTH_UNIT;
        if (CallExecute(move, &delta, depth))
        {
            // Store move with position before the move is played.
            CallTakeBack();

            // Move is only relevant data we're seeding the hash table with.
            SgSearchHashData data(0, 0, move);
            SG_ASSERT(move != SG_NULLMOVE);
            m_hash->Store(GetHashCode(), data);
            if (DEBUG_SEARCH)
                SgDebug() << "SgSearch::AddSequenceToHash: "
                          << MoveString(move) << '\n';
            // Execute move again.
            int delta = DEPTH_UNIT;
            if (CallExecute(move, &delta, depth))
                ++numMovesToUndo;
        }
        else
            break;
    }

    // Restore the original position.
    for (int i = 1; i <= numMovesToUndo; ++i)
        CallTakeBack();
}

int SgSearch::DFS(int startDepth, int depthLimit,
                int boundLo, int boundHi,
                SgList<SgMove>* sequence, bool* isExactValue)
{
    InitSearch(startDepth);
    SG_ASSERT(m_currentDepth == startDepth);
    m_aborted = false;
    m_foundNewBest = false;
    int value = 0;
    value = SearchEngine(depthLimit * DEPTH_UNIT, boundLo, boundHi, sequence,
                         isExactValue);
    return value;
}

int SgSearch::DepthFirstSearch(int depthLimit, int boundLo, int boundHi,
                               SgList<SgMove>* sequence, bool clearHash,
                               SgNode* traceNode)
{
    SG_ASSERT(sequence);
    OnStartSearch();
    if (! m_traceNode && traceNode)
    {
        m_traceNode = traceNode->NewRightMostSon();
        TraceComment("DFS tree");
    }

    StartTime();

    // Clear the hash table before a new search, because don't know
    // whether the goal and evaluation is still the same as for the
    // last time it was called.
    if (clearHash && m_hash)
    {
        m_hash->Clear();
        AddSequenceToHash(*sequence, 0);
    }

    // IteratedSearchDepthLimit checked to decide on aborting.
    m_depthLimit = 0;

    bool isExactValue = true;
    int value = DFS(0, depthLimit, boundLo, boundHi, sequence, &isExactValue);
    StopTime();

    return value;
}

int SgSearch::IteratedSearch(int depthMin, int depthMax, int boundLo,
                             int boundHi, SgList<SgMove>* sequence,
                             bool clearHash, SgNode* traceNode)
{
    SG_ASSERT(sequence);
    OnStartSearch();
    if (! m_traceNode && traceNode)
        m_traceNode = traceNode;
    StartTime();

    // Clear the hash table before a new search, because don't know
    // whether the goal and evaluation is still the same as for the
    // last time it was called.
    if (clearHash && m_hash)
    {
        m_hash->Clear();
        AddSequenceToHash(*sequence, 0);
    }

    int value = 0;
    m_depthLimit = depthMin;
    // done in DFS, but that's too late, is tested after StartOfDepth
    m_aborted = false;

    // Keep track of value and sequence of previous iteration in case search
    // gets aborted (since value and sequence are then ill-defined).
    m_prevValue = 0;
    m_prevSequence.Clear();
    bool isExactValue = true;

    do
    {
        if (m_control != 0
            && ! m_control->StartNextIteration(m_depthLimit,
                                               m_timer.GetTime(), NumNodes()))
            SetAbortSearch();
        if (m_aborted)
            break;
        StartOfDepth(m_depthLimit);

        // Record depth limit of depths where we actually do some search.
        m_stat.SetDepthReached(m_depthLimit);

        // Remember whether we actually reach the depth limit. If not, no
        // sense in increasing the depth limit, won't find anything new.
        m_reachedDepthLimit = false;
        isExactValue = true;
        m_foundNewBest = false;

        // Depth-first search.
        value = DFS(0, m_depthLimit, boundLo, boundHi, sequence,
                    &isExactValue);

        // Restore result of previous iteration if aborted.
        if (m_aborted)
        {
            if (m_prevSequence.NonEmpty() && ! m_foundNewBest)
            {
                // save depth 1 move.
                value = m_prevValue;
                *sequence = m_prevSequence;
            }
            break;
        }
        else
        {
            if (DEBUG_SEARCH_ITERATIONS)
                PrintPV(*this, m_depthLimit, value, *sequence, isExactValue);
            m_prevValue = value;
            m_prevSequence = *sequence;
        }

        // Stop iteration as soon as exact result or a bounding value found.
        if (isExactValue || value <= boundLo || boundHi <= value)
            break;

        ++m_depthLimit;

    } while (m_depthLimit <= depthMax
             && ! isExactValue
             && ! m_aborted
             && (! CheckDepthLimitReached() || m_reachedDepthLimit));

    StopTime();

    return value;
}

int SgSearch::SearchEngine(int depth, int alpha, int beta,
                           SgList<SgMove>* sequence, bool* isExactValue,
                           bool lastNullMove)
{
    SG_ASSERT(sequence);
    SG_ASSERT(sequence->IsEmpty() || sequence->Top() != SG_NULLMOVE);
    SG_ASSERT(alpha < beta);

    // Only place we check whether the search has been newly aborted. In all
    // other places, just check whether search was aborted before.
    // AR: what to return here?
    // if - (SG_INFINITY-1), then will be positive on next level?
    if (AbortSearch())
    {
        *isExactValue = false;
        return alpha;
    }

    // Null move pruning
    if (m_useNullMove && depth > 0 && !lastNullMove
        && NullMovePrune(depth, DEPTH_UNIT * (1 + m_nullMoveDepth), beta))
    {
        *isExactValue = false;
        return beta;
    }

    // ProbCut
    if (m_probcut && m_probcut->IsEnabled())
    {
        int probCutVal;
        if (ProbCut(depth, alpha, beta, sequence, isExactValue, &probCutVal))
            return probCutVal;
    }

    m_stat.IncNumNodes();
    bool fHasMove = false; // true if a move has been executed at this level
    int loValue = -(SG_INFINITY - 1);
    m_reachedDepthLimit = m_reachedDepthLimit || (depth <= 0);

    // check whether position is solved from hash table.
    SgSearchHashData data; // initialized to ! data.IsValid()
    if (LookupHash(data))
    {
        if (data.IsExactValue()) // exact value: stop search
        {
            *isExactValue = true;
            if (data.BestMove() == SG_NULLMOVE)
                sequence->Clear();
            else
                sequence->SetTo(data.BestMove());
            if (m_traceNode)
                TraceValue(data.Value(), "exact-hash", true);
            return data.Value();
        }
    }

    bool allExact = true; // Do all moves have exact evaluation?
    if (depth > 0 && ! EndOfGame())
    {
        // Check whether current position has already been encountered.
        SgMove tryFirst = SG_NULLMOVE;
        SgMove opponentBest = SG_NULLMOVE;
        if (data.IsValid())
        {
            if (data.Depth() > 0)
            {
                tryFirst = data.BestMove();
                SG_ASSERT(tryFirst != SG_NULLMOVE);
            }

            // If data returned from hash table is based on deeper search
            // than what we plan to do right now, just use that data. The
            // hash table may have deeper data for the current position
            // since the same number of moves may result in more 'depth'
            // left if the 'delta' for the moves has been smaller, which
            // will happen when most moves came from the cache.
            if (depth <= data.Depth())
            {
                // Rely on value returned from hash table to be for the
                // current position. In Go, it can happen that the move is
                // not legal (ko recapture)
                int delta = DEPTH_UNIT/2;
                bool fCanExecute = CallExecute(tryFirst, &delta, depth);
                if (fCanExecute)
                    CallTakeBack();
                else
                {
                    //SG_ASSERT(false);
                    tryFirst = SG_NULLMOVE;
                }
                if (tryFirst != SG_NULLMOVE || data.IsExactValue())
                {
                    // getting a deep enough hash hit or an exact value
                    // is as good as reaching the depth limit by search.
                    m_reachedDepthLimit = true;

                    // Update bounds with data from cache.
                    data.AdjustBounds(&alpha, &beta);

                    if (alpha >= beta)
                    {
                        *isExactValue = data.IsExactValue();
                        if (tryFirst == SG_NULLMOVE)
                            sequence->Clear();
                        else
                            sequence->SetTo(tryFirst);
                        if (m_traceNode)
                            TraceValue(data.Value(), "Hash hit",
                                       *isExactValue);
                        return data.Value();
                    }
                }
            }

            int delta = DEPTH_UNIT;
            if (tryFirst != SG_NULLMOVE
                && CallExecute(tryFirst, &delta, depth))
            {
                bool childIsExact = true;
                loValue = -SearchEngine(depth-delta, -beta, -alpha, sequence,
                                        &childIsExact);
                if (m_traceNode)
                    TraceComment("tryFirst");
                CallTakeBack();
                fHasMove = true;
                if (m_aborted)
                {
                    if (m_traceNode)
                        TraceComment("aborted");
                    *isExactValue = false;
                    return (1 < m_currentDepth) ? alpha : loValue;
                }
                if (sequence->NonEmpty())
                {
                    opponentBest = sequence->Top();
                    SG_ASSERT(opponentBest != SG_NULLMOVE);
                }
                sequence->Push(tryFirst);
                if (! childIsExact)
                   allExact = false;
                if (loValue >= beta)
                {
                    if (m_traceNode)
                        TraceValue(loValue);
                    // store in hash table. Known to be exact only if
                    // SgValue::MAX_VALUE reached for one player.
                    bool isExact = (abs(loValue) == SgValue::MAX_VALUE);
                    StoreHash(depth, loValue, tryFirst,
                              (loValue <= alpha) /*isUpperBound*/,
                              (beta <= loValue) /*isLowerBound*/, isExact);
                    *isExactValue = isExact;
                    if (m_traceNode)
                        TraceValue(loValue, "b-cut", isExact);
                    return loValue;
                }
            }
        }

        // Generate the moves for this position.
        SgList<SgMove> moves;
        if (! m_aborted)
        {
            CallGenerate(&moves, depth);
           // If hash table suggested a move for a position where no move
           // would get generated, then evaluate position rather than using
           // the outcome of that wrongly generated move.
           if (moves.IsEmpty())
               fHasMove = false;
        }

        MoveKillersToFront(moves);

        // Heuristic: "a good move for my opponent is a good move for me"
        if (m_useOpponentBest && opponentBest != SG_NULLMOVE)
        {
            moves.Exclude(opponentBest);
            moves.Push(opponentBest);
        }

        // Don't execute 'tryFirst' again.
        if (tryFirst != SG_NULLMOVE && moves.NonEmpty())
            moves.Exclude(tryFirst);

        // 'hiValue' is equal to 'beta' for alpha-beta algorithm, and gets set
        // to alpha+1 for Scout, except for the first move.
        int hiValue =
            (fHasMove && m_useScout) ? max(loValue, alpha) + 1 : beta;

        SgList<SgMove> newSeq;
        // Iterate through all the moves to find the best move and
        // correct value for this position.
        for (SgListIterator<SgMove> it(moves); it; ++it)
        {
            SgMove move = *it;
            int delta = DEPTH_UNIT;
            if (CallExecute(move, &delta, depth))
            {
                fHasMove = true;
                bool childIsExact = true;
                int merit = -SearchEngine(depth-delta, -hiValue,
                                          -max(loValue, alpha), &newSeq,
                                          &childIsExact);
                if (loValue < merit && ! m_aborted)
                {
                    loValue = merit;
                    if (m_useScout && ! m_aborted)
                    {
                        // If getting a move that's better than what we have
                        // so far, not good enough to cause a cutoff, was
                        // searched with a narrow window, and doesn't
                        // immediately lead to a terminal node, then search
                        // again with a wide window to get a more precise
                        // value.
                        if (alpha < merit && merit < beta && fHasMove
                            && delta < depth)
                        {
                            childIsExact = true;
                            loValue = -SearchEngine(depth-delta, -beta,
                                                    -merit, &newSeq,
                                                    &childIsExact);
                        }
                        hiValue = max(loValue, alpha) + 1;
                    }
                    sequence->SwapWith(&newSeq);
                    sequence->Push(move);
                    SG_ASSERT(move != SG_NULLMOVE);
                    if (m_currentDepth == 1 && ! m_aborted)
                        m_foundNewBest = true;
                }
                if (! childIsExact)
                    allExact = false;
                CallTakeBack();
                if (m_aborted)
                {
                    if (m_traceNode)
                        TraceComment("ABORTED");
                    *isExactValue = false;
                    return (1 < m_currentDepth) ? alpha : loValue;
                }
                if (loValue >= beta)
                {
                    // Move generated a cutoff: add this move to the list of
                    // killers.
                    if (m_useKillers && m_currentDepth <= MAX_KILLER_DEPTH)
                        m_killers[m_currentDepth].MarkKiller(move);
                    if (m_traceNode)
                        TraceComment("b-cut");
                    break;
                }
            }
        }

        // Make sure the move added to the hash table really got generated.
#ifndef NDEBUG
        if (fHasMove && sequence->NonEmpty() && ! m_aborted)
        {
            SgMove bestMove = sequence->Top();
            SG_ASSERT(bestMove != SG_NULLMOVE);
            SG_ASSERT(bestMove == tryFirst || moves.Contains(bestMove));
        }
#endif
    }

    bool isSolved = ! m_aborted;
    if (! m_aborted)
    {
        // Evaluate position if terminal node (either no moves generated, or
        // none of the generated moves were legal).
        bool solvedByEval = false;
        if (! fHasMove)
        {
            m_stat.IncNumEvals();
            sequence->Clear();
            loValue = CallEvaluate(depth, alpha, beta, sequence,
                                   &solvedByEval);
        }

        // Save data about current position in the hash table.
        isSolved = solvedByEval
            || (abs(loValue) == SgValue::MAX_VALUE)
            || (fHasMove && allExact);
        // || EndOfGame(); bug: cannot store exact score after two passes.
        if (   m_hash
            && ! m_aborted
            && (isSolved || sequence->NonEmpty())
           )
        {
            SgMove bestMove = SG_NULLMOVE;
            if (sequence->NonEmpty())
            {
                bestMove = sequence->Top();
                SG_ASSERT(bestMove != SG_NULLMOVE);
            }
            SG_ASSERT(alpha <= beta);
            StoreHash(depth, loValue, bestMove,
                      (loValue <= alpha) /* upper */,
                      (beta <= loValue) /* lower*/, isSolved);
        }
    }

    // If aborted search and didn't find any values, just return alpha.
    // Can't return best found so far, since may not have tried the optimal
    // counter-move yet. However, return best value found so far on top
    // level, since assuming hash move will have been tried first.
    if (m_aborted && (1 < m_currentDepth || loValue < alpha))
        loValue = alpha;

    *isExactValue = isSolved;
    if (m_traceNode)
        TraceValue(loValue, 0, *isExactValue);
    SG_ASSERT(sequence->IsEmpty() || sequence->Top() != SG_NULLMOVE);
    return loValue;
}

void SgSearch::StartOfDepth(int depthLimit)
{
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::StartOfDepth: " << depthLimit << '\n';
    // add another separate search tree. We are either at the root of the
    // previous depth tree or, if depthLimit==0, one level higher at the
    // linking node.
    if (m_traceNode && ! m_aborted)
    {
        if (depthLimit > 0 && m_traceNode->HasFather())
        {
            // true for each depth except the very first
            // AR: the 0 should really be the depthMin parameter of iterated
            // search. this will break if depthMin != 0 and generate strange
            // trace trees.
            m_traceNode = m_traceNode->Father();
            // go from root of previous level to root
        }
        m_traceNode = m_traceNode->NewRightMostSon();
        SG_ASSERT(m_traceNode);
        m_traceNode->SetIntProp(SG_PROP_MAX_DEPTH, depthLimit);
        ostringstream o;
        o << "Iteration d=" << depthLimit << ' ';
        m_traceNode->AddComment(o.str());

        // @todo would be interesting to know time used for each depth,
        // create SG_PROP_TIME_USED property at EndOfDepth (doesn't exist yet)
    }
}

void SgSearch::TraceValue(int value) const
{
    // The value needs to be recorded in absolute terms, not relative to
    // the current player.
    int v = (GetToPlay() == SG_WHITE) ? -value : +value;
    m_traceNode->Add(new SgPropValue(SG_PROP_VALUE, v));
    ostringstream comment;
    comment << "v=" << value;
    TraceComment(comment.str().c_str());
}

void SgSearch::TraceValue(int value, const char* comment, bool isExact) const
{
    TraceValue(value);
    if (comment != 0)
        TraceComment(comment);
    if (isExact)
    {
        m_traceNode->Add(new SgPropMultiple(SG_PROP_CHECK, 1));
        TraceComment("exact");
    }
}

void SgSearch::TraceComment(const char* comment) const
{
    if (m_traceNode)
    {
        m_traceNode->AddComment(comment);
        m_traceNode->AddComment("\n");
    }
}

void SgSearch::AddTraceNode(SgMove move, SgBlackWhite player)
{
    if (m_traceNode)
    {
        m_traceNode = m_traceNode->NewRightMostSon();
        AddMoveProp(m_traceNode, move, player);
    }
}

void SgSearch::TakeBackTraceNode()
{
    if (m_traceNode)
        m_traceNode = m_traceNode->Father();
}

void SgSearch::InitTracing(const string& type)
{
    SG_ASSERT(! m_traceNode);
    if (TraceIsOn())
    {
        m_traceNode = new SgNode();
        m_traceNode->Add(new SgPropText(SG_PROP_COMMENT, type));
    }
}

void SgSearch::AppendTrace(SgNode* toNode)
{
    if (m_traceNode)
    {
        m_traceNode->Root()->AppendTo(toNode);
        m_traceNode = 0;
    }
}

//----------------------------------------------------------------------------

SgSearchControl::~SgSearchControl()
{
}

bool SgSearchControl::StartNextIteration(int depth, double elapsedTime,
                                         int numNodes)
{
    SG_UNUSED(depth);
    SG_UNUSED(elapsedTime);
    SG_UNUSED(numNodes);
    return true;
}

//----------------------------------------------------------------------------

SgTimeSearchControl::SgTimeSearchControl(double maxTime)
    : m_maxTime(maxTime)
{
}

SgTimeSearchControl::~SgTimeSearchControl()
{
}

bool SgTimeSearchControl::Abort(double elapsedTime, int numNodes)
{
    SG_UNUSED(numNodes);
    return elapsedTime >= m_maxTime;
}

//----------------------------------------------------------------------------

SgNodeSearchControl::SgNodeSearchControl(int maxNumNodes)
    : m_maxNumNodes(maxNumNodes)
{ }

SgNodeSearchControl::~SgNodeSearchControl()
{
}

bool SgNodeSearchControl::Abort(double elapsedTime, int numNodes)
{
    SG_UNUSED(elapsedTime);
    return numNodes >= m_maxNumNodes;
}

//----------------------------------------------------------------------------

SgCombinedSearchControl::~SgCombinedSearchControl()
{
}

bool SgCombinedSearchControl::Abort(double elapsedTime, int numNodes)
{
    return (numNodes >= m_maxNumNodes || elapsedTime >= m_maxTime);
}

//----------------------------------------------------------------------------

SgRelaxedSearchControl::~SgRelaxedSearchControl()
{
}

bool SgRelaxedSearchControl::Abort(double elapsedTime, int numNodes)
{
    return (elapsedTime >= m_maxTime
            && numNodes >= MIN_NODES_PER_SECOND * m_maxTime);
}

//----------------------------------------------------------------------------

