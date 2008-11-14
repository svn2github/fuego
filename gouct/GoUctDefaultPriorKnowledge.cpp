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

/** Find global moves that match a playout pattern or set a block into atari.
    @param[out] pattern
    @param[out] atari
    @return @c true if any such moves was found
*/
bool GoUctDefaultPriorKnowledge::FindGlobalPatternAndAtariMoves(
                                                      SgPointSet& pattern,
                                                      SgPointSet& atari) const
{
    const GoUctPatterns<GoBoard>& patterns = m_policy.Patterns();
    bool result = false;
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it))
        {
            if (patterns.MatchAny(*it))
            {
                pattern.Include(*it);
                result = true;
            }
            if (SetsAtari(m_bd, *it))
            {
                atari.Include(*it);
                result = true;
            }
        }
    return result;
}

void GoUctDefaultPriorKnowledge::ProcessPosition(bool& deepenTree)
{
    SG_UNUSED(deepenTree);
    m_policy.StartPlayout();
    m_policy.GenerateMove();
    GoUctPlayoutPolicyType type = m_policy.MoveType();
    bool isFullBoardRandom =
        (type == GOUCT_RANDOM || type == GOUCT_FILLBOARD);
    const int sz = m_bd.Size();
    SgPointSet pattern;
    SgPointSet atari;
    bool anyHeuristic = FindGlobalPatternAndAtariMoves(pattern, atari);

    // The initialization values/counts are mainly tuned by selfplay
    // experiments and games vs MoGo Rel 3 and GNU Go 3.6 on 9x9 and 19x19.
    // If different values are used for the small and large board, the ones
    // from the 9x9 experiments are used for board sizes < 15, the ones from
    // 19x19 otherwise.

    Initialize(SG_PASS, 0.1, sz < 15 ? 9 : 13);
    if (isFullBoardRandom && ! anyHeuristic)
    {
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it) || m_bd.IsSuicide(*it))
                Initialize(*it, 0.1, sz < 15 ? 9 : 13);
            else
                Initialize(*it, 0.5, 0); // Don't initialize
        }
    }
    else if (isFullBoardRandom && anyHeuristic)
    {
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it) || m_bd.IsSuicide(*it))
                Initialize(*it, 0.1, sz < 15 ? 9 : 13);
            else if (atari[*it])
                Initialize(*it, 1.0, 3);
            else if (pattern[*it])
                Initialize(*it, 0.9, 3);
            else
                Initialize(*it, 0.5, 3);
        }
    }
    else
    {
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it) || m_bd.IsSuicide(*it))
                Initialize(*it, 0.1, sz < 15 ? 9 : 13);
            else if (atari[*it])
                Initialize(*it, 0.8, sz < 15 ? 9 : 13);
            else if (pattern[*it])
                Initialize(*it, 0.6, sz < 15 ? 9 : 13);
            else
                Initialize(*it, 0.4, sz < 15 ? 9 : 13);
        }
        GoPointList moves = m_policy.GetEquivalentBestMoves();
        for (GoPointList::Iterator it(moves); it; ++it)
            Initialize(*it, 1.0, sz < 15 ? 9 : 13);
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
