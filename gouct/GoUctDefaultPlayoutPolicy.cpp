//----------------------------------------------------------------------------
/** @file GoUctDefaultPlayoutPolicy.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPlayoutPolicy.h"

#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "GoUctUtil.h"
#include "SgIO.h"
#include "SgStreamFmtRestorer.h"

using namespace std;
using GoUctUtil::SelectRandom;

//----------------------------------------------------------------------------

GoUctDefaultPlayoutPolicyParam::GoUctDefaultPlayoutPolicyParam()
    : m_pureRandom(false),
      m_useClumpCorrection(false),
      m_statisticsEnabled(false)
{
}

//----------------------------------------------------------------------------

void GoUctDefaultPlayoutPolicyStat::Clear()
{
    m_nuMoves = 0;
    m_nuRandMoves = 0;
    m_nonRandLen.Clear();
    m_moveListLen.Clear();
}

void GoUctDefaultPlayoutPolicyStat::Write(std::ostream& out) const
{
    SgStreamFmtRestorer restorer(out);
    out << fixed << setprecision(2)
        << SgWriteLabel("NuMoves") << m_nuMoves << '\n'
        << SgWriteLabel("RandMoves")
        << (m_nuMoves > 0 ? m_nuRandMoves * 100 / m_nuMoves : 0) << "%\n"
        << SgWriteLabel("NonRandLen");
    m_nonRandLen.Write(out);
    out << '\n'
        << SgWriteLabel("MoveListLen");
    m_moveListLen.Write(out);
    out << '\n';
}

//----------------------------------------------------------------------------

GoUctPolicyPriorKnowledge::GoUctPolicyPriorKnowledge(const GoBoard& bd,
                              const GoUctDefaultPlayoutPolicyParam& param,
                              const SgBWSet& safe,
                              const SgPointArray<bool>& allSafe)
    : m_policy(bd, param, safe, allSafe)
{
}

void GoUctPolicyPriorKnowledge::ProcessPosition()
{
    m_policy.GetPriorKnowledge(m_values, m_counts);
}

void GoUctPolicyPriorKnowledge::InitializeMove(SgMove move, float& value,
                                               std::size_t& count)
{
    value = m_values[move];
    count = m_counts[move];
}

//----------------------------------------------------------------------------

GoUctPolicyPriorKnowledgeFactory
::GoUctPolicyPriorKnowledgeFactory(const GoUctDefaultPlayoutPolicyParam&
                                   param)
    : m_param(param)
{
}

SgUctPriorKnowledge*
GoUctPolicyPriorKnowledgeFactory::Create(SgUctThreadState& state)
{
    GoUctGlobalSearchState& globalSearchState
        = dynamic_cast<GoUctGlobalSearchState&>(state);
    return new GoUctPolicyPriorKnowledge(globalSearchState.Board(),
                                         m_param,
                                         globalSearchState.m_safe,
                                         globalSearchState.m_allSafe);
}

//----------------------------------------------------------------------------
