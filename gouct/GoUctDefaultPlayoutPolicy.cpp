//----------------------------------------------------------------------------
/** @file GoUctDefaultPlayoutPolicy.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPlayoutPolicy.h"

#include <algorithm>
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "GoUctUtil.h"
#include "SgDebug.h"
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

const char* GoUctDefaultPlayoutPolicyTypeStr(
                                          GoUctDefaultPlayoutPolicyType type)
{
    BOOST_STATIC_ASSERT(_GOUCT_NU_DEFAULT_PLAYOUT_TYPE == 9);
    switch (type)
    {
    case GOUCT_ATARI_CAPTURE:
        return "AtariCapture";
    case GOUCT_ATARI_DEFEND:
        return "AtariDefend";
    case GOUCT_LOWLIB:
        return "LowLib";
    case GOUCT_PATTERN:
        return "Pattern";
    case GOUCT_CAPTURE:
        return "Capture";
    case GOUCT_RANDOM:
        return "Random";
    case GOUCT_SELFATARI_CORRECTION:
        return "SelfAtariCorr";
    case GOUCT_CLUMP_CORRECTION:
        return "ClumpCorr";
    case GOUCT_PASS:
        return "Pass";
    default:
        return "?";
    }
}

//----------------------------------------------------------------------------

void GoUctDefaultPlayoutPolicyStat::Clear()
{
    m_nuMoves = 0;
    m_nonRandLen.Clear();
    m_moveListLen.Clear();
    fill(m_nuMoveType.begin(), m_nuMoveType.end(), 0);
}

void GoUctDefaultPlayoutPolicyStat::Write(std::ostream& out) const
{
    SgStreamFmtRestorer restorer(out);
    out << fixed << setprecision(2)
        << SgWriteLabel("NuMoves") << m_nuMoves << '\n';
    for (int i = 0; i < _GOUCT_NU_DEFAULT_PLAYOUT_TYPE; ++i)
    {
        GoUctDefaultPlayoutPolicyType type =
            static_cast<GoUctDefaultPlayoutPolicyType>(i);
        size_t n = m_nuMoveType[type];
        out << SgWriteLabel(GoUctDefaultPlayoutPolicyTypeStr(type))
            << (m_nuMoves > 0 ? n * 100 / m_nuMoves : 0) << "%\n";
    }
    out << SgWriteLabel("NonRandLen");
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
