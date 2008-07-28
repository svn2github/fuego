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
                              const GoUctDefaultPlayoutPolicyParam& param)
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
    GoUctDefaultPlayoutPolicyType moveType = m_policy.MoveType();
    if (moveType == GOUCT_RANDOM)
        m_counts.Fill(0);
    else
    {
        Initialize(SG_PASS, 0, 9);

        // Initialize all moves with 9 * 0.5, self-atari moves with 9 * 0
        for (GoBoard::Iterator it(m_bd); it; ++it)
            if (m_bd.IsEmpty(*it) && GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0, 9);
            else
                Initialize(*it, 0.5, 9);

        // Initialize moves that would have been played by playout policy
        // with 9 * 1
        GoPointList moves = m_policy.GetEquivalentBestMoves();
        for (GoPointList::Iterator it(moves); it; ++it)
            Initialize(*it, 1, 9);
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
                                          m_param);
}

//----------------------------------------------------------------------------
