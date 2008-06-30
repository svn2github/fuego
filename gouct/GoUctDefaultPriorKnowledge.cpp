//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.cpp
    See GoUctDefaultPriorKnowledge.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPriorKnowledge.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledge::GoUctDefaultPriorKnowledge(const GoBoard& bd,
                              const GoUctDefaultPlayoutPolicyParam& param,
                              const SgBWSet& safe,
                              const SgPointArray<bool>& allSafe)
    : m_bd(bd),
      m_policy(bd, param, safe, allSafe)
{
}

/** Check if ladder attack of last stone played succeeds.
    @param[out] move The attack move, if successful
    @return @true, if ladder attack is successful.
*/
bool GoUctDefaultPriorKnowledge::CheckLadderAttack(SgPoint& move)
{
    SgPoint last = m_bd.GetLastMove();
    if (last == SG_NULLMOVE || last == SG_PASS
        || ! m_bd.Occupied(last) /* Suicide could be allowed in in-tree
                                    phase */
        || m_bd.NumLiberties(last) != 2)
        return false;
    m_ladderSequence.Clear();
    if (m_ladder.Ladder(m_bd, last, SgOppBW(m_bd.GetStone(last)),
                        &m_ladderSequence, true)
        && ! m_ladderSequence.IsEmpty())
    {
        move = m_ladderSequence[1];
        return true;
    }
    return false;
}

void GoUctDefaultPriorKnowledge::Initialize(SgPoint p, float value,
                                            std::size_t count)
{
    m_values[p] = value;
    m_counts[p] = count;
}

void GoUctDefaultPriorKnowledge::ProcessPosition()
{
    m_policy.StartPlayout();
    m_policy.GenerateMove();
    GoUctDefaultPlayoutPolicyType moveType = m_policy.MoveType();
    if (moveType == GOUCT_RANDOM)
        m_counts.Fill(0);
    else
    {
        m_values[SG_PASS] = 0;

        // Initialize all moves with 9 * 0.5, self-atari moves with 9 * 0
        for (GoBoard::Iterator it(m_bd); it; ++it)
            if (m_bd.IsEmpty(*it) && GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0, 9);
            else
                Initialize(*it, 0.5, 9);

        // Check if ladder attack of last stone played succeeds and, if so,
        // initialize it with n * 1 (unless playout policy would have played
        // an atari defend/attack move, which is more urgent). The count n
        // is roughly the length of a board-traversing ladder.
        SgPoint ladderAttackMove;
        if (GOUCT_DEFAULTPRIORKNOWLEDGE_LADDERS
            && moveType != GOUCT_ATARI_CAPTURE
            && moveType != GOUCT_ATARI_DEFEND
            && CheckLadderAttack(ladderAttackMove))
            Initialize(ladderAttackMove, 1, 3 * m_bd.Size());
        else
        {
            // Initialize moves that would have been played by playout policy
            // with 9 * 1
            GoPointList moves = m_policy.GetEquivalentBestMoves();
            for (GoPointList::Iterator it(moves); it; ++it)
                Initialize(*it, 1, 9);
        }
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
::GoUctDefaultPriorKnowledgeFactory(const GoUctDefaultPlayoutPolicyParam&
                                    param)
    : m_param(param)
{
}

SgUctPriorKnowledge*
GoUctDefaultPriorKnowledgeFactory::Create(SgUctThreadState& state)
{
    GoUctGlobalSearchState& globalSearchState
        = dynamic_cast<GoUctGlobalSearchState&>(state);
    return new GoUctDefaultPriorKnowledge(globalSearchState.Board(),
                                          m_param,
                                          globalSearchState.m_safe,
                                          globalSearchState.m_allSafe);
}

//----------------------------------------------------------------------------
