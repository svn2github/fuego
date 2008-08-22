//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.cpp
    See GoUctDefaultPriorKnowledge.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPriorKnowledge.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

bool SetsAtari(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT(bd.IsEmpty(p)); // Already checked
    SgBlackWhite opp = SgOppBW(bd.ToPlay());
    if (bd.NumNeighbors(p, opp) == 0)
        return false;
    if (! bd.IsBorder(p + SG_NS) && bd.GetColor(p + SG_NS) == opp
        && bd.NumLiberties(p + SG_NS) == 2)
        return true;
    if (! bd.IsBorder(p - SG_NS) && bd.GetColor(p - SG_NS) == opp
        && bd.NumLiberties(p - SG_NS) == 2)
        return true;
    if (! bd.IsBorder(p + SG_WE) && bd.GetColor(p + SG_WE) == opp
        && bd.NumLiberties(p + SG_WE) == 2)
        return true;
    if (! bd.IsBorder(p - SG_WE) && bd.GetColor(p - SG_WE) == opp
        && bd.NumLiberties(p - SG_WE) == 2)
        return true;
    return false;
}

} // namespace

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledge::GoUctDefaultPriorKnowledge(const GoBoard& bd,
                              const GoUctPlayoutPolicyParam& param)
    : m_bd(bd),
      m_policy(bd, param)
{
}

void GoUctDefaultPriorKnowledge::Initialize(SgPoint p, float value,
                                            std::size_t count)
{
    m_values[p] = value;
    m_counts[p] = count;
}

void GoUctDefaultPriorKnowledge::ProcessPosition(bool& deepenTree)
{
    SG_UNUSED(deepenTree);
    m_policy.StartPlayout();
    m_policy.GenerateMove();
    GoUctPlayoutPolicyType type = m_policy.MoveType();
    const GoUctPatterns<GoBoard>& patterns = m_policy.Patterns();
    SgPointSet patternMatch;
    SgPointSet setsAtari;
    bool anyHeuristic = false;
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it))
        {
            if (patterns.MatchAny(*it))
            {
                patternMatch.Include(*it);
                anyHeuristic = true;
            }
            if (SetsAtari(m_bd, *it))
            {
                setsAtari.Include(*it);
                anyHeuristic = true;
            }
        }

    if (type == GOUCT_RANDOM && ! anyHeuristic)
    {
        Initialize(SG_PASS, 0.1, 9);
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it) || m_bd.IsSuicide(*it))
                Initialize(*it, 0.1, 9);
            else
                m_counts[*it] = 0; // Don't initialize
        }
    }
    else if (type == GOUCT_RANDOM && anyHeuristic)
    {
        Initialize(SG_PASS, 0.1, 9);
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it) || m_bd.IsSuicide(*it))
                Initialize(*it, 0.1, 9);
            else if (setsAtari[*it])
                Initialize(*it, 1.0, 3);
            else if (patternMatch[*it])
                Initialize(*it, 0.9, 3);
            else
                Initialize(*it, 0.5, 3);
        }
    }
    else
    {
        Initialize(SG_PASS, 0.1, 9);
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it) || m_bd.IsSuicide(*it))
                Initialize(*it, 0.1, 9);
            else if (setsAtari[*it])
                Initialize(*it, 0.8, 9);
            else if (patternMatch[*it])
                Initialize(*it, 0.6, 9);
            else
                Initialize(*it, 0.4, 9);
        }
        GoPointList moves = m_policy.GetEquivalentBestMoves();
        for (GoPointList::Iterator it(moves); it; ++it)
            Initialize(*it, 1.0, 9);
    }

    m_policy.EndPlayout();
}

void GoUctDefaultPriorKnowledge::InitializeMove(SgMove move, float& value,
                                                std::size_t& count)
{
    value = m_values[move];
    count = m_counts[move];
}

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledgeFactory
::GoUctDefaultPriorKnowledgeFactory(const GoUctPlayoutPolicyParam& param)
    : m_param(param)
{
}

SgUctPriorKnowledge*
GoUctDefaultPriorKnowledgeFactory::Create(SgUctThreadState& state)
{
    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&
        globalSearchState = dynamic_cast<
      GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&>(state);
    return new GoUctDefaultPriorKnowledge(globalSearchState.Board(), m_param);
}

//----------------------------------------------------------------------------
