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
        for (GoBoard::Iterator it(m_bd); it; ++it)
            if (m_bd.IsEmpty(*it) && GoBoardUtil::SelfAtari(m_bd, *it))
                m_values[*it] = 0;
            else
                m_values[*it] = 0.5;
        GoPointList moves = m_policy.GetEquivalentBestMoves();
        for (GoPointList::Iterator it(moves); it; ++it)
            m_values[*it] = 1;
        m_counts.Fill(9);
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
