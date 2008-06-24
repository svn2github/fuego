//----------------------------------------------------------------------------
/** @file GoUctGlobalSearch.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctGlobalSearch.h"

#include "GoBoardUtil.h"
#include "GoEyeUtil.h"
#include "GoSafetySolver.h"
#include "GoUctDefaultPlayoutPolicy.h"
#include "GoUctUtil.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctGlobalSearchStateParam::GoUctGlobalSearchStateParam()
    : m_mercyRule(true),
      m_scoreModification(0.02)
{
}

//----------------------------------------------------------------------------

GoUctGlobalSearchState::GoUctGlobalSearchState(std::size_t threadId,
         const GoBoard& bd, GoUctPlayoutPolicy<GoUctBoard>* policy,
         const GoUctGlobalSearchStateParam& param,
         const SgBWSet& safe, const SgPointArray<bool>& allSafe)
    : GoUctState(threadId, bd),
      m_safe(safe),
      m_allSafe(allSafe),
      m_param(param),
      m_policy(policy)
{
}

GoUctGlobalSearchState::~GoUctGlobalSearchState()
{
}

/** See SetMercyRule() */
bool GoUctGlobalSearchState::CheckMercyRule()
{
    SG_ASSERT(m_param.m_mercyRule);
    // Only used in playout; m_stoneDiff only defined in playout
    SG_ASSERT(IsInPlayout());
    if (m_stoneDiff >= m_mercyRuleThreshold)
    {
        m_mercyRuleTriggered = true;
        m_mercyRuleResult = (ToPlay() == SG_BLACK ? 1 : 0);
    }
    else if (m_stoneDiff <= -m_mercyRuleThreshold)
    {
        m_mercyRuleTriggered = true;
        m_mercyRuleResult = (ToPlay() == SG_WHITE ? 1 : 0);
    }
    else
        SG_ASSERT(! m_mercyRuleTriggered);
    return m_mercyRuleTriggered;
}

float GoUctGlobalSearchState::Evaluate()
{
    float komi = Board().Rules().Komi().ToFloat();
    if (IsInPlayout())
        return EvaluateBoard(UctBoard(), komi);
    else
        return EvaluateBoard(Board(), komi);
}

template<class BOARD>
float GoUctGlobalSearchState::EvaluateBoard(const BOARD& bd,
                                            float komi) const
{
    float score;
    if (m_passMovesPlayoutPhase < 2)
        // Two passes not in playout phase, see comment in GenerateAllMoves()
        score = GoBoardUtil::TrompTaylorScore(bd, komi);
    else
    {
        if (m_param.m_mercyRule && m_mercyRuleTriggered)
            return m_mercyRuleResult;
        score = GoBoardUtil::ScoreEndPosition(bd, komi, m_safe);
    }
    if (ToPlay() != SG_BLACK)
        score *= -1;
    if (score > 0)
        return ((1 - m_param.m_scoreModification)
                + m_param.m_scoreModification * score * m_invMaxScore);
    else
        return (m_param.m_scoreModification
                + m_param.m_scoreModification * score * m_invMaxScore);
}

void GoUctGlobalSearchState::ExecutePlayout(SgMove move)
{
    GoUctState::ExecutePlayout(move);
    const GoUctBoard& bd = UctBoard();
    if (ToPlay() == SG_BLACK)
        m_stoneDiff -= bd.NuCapturedStones();
    else
        m_stoneDiff += bd.NuCapturedStones();
    m_policy->OnPlay();
}

void GoUctGlobalSearchState::GenerateAllMoves(vector<SgMove>& moves)
{
    SG_ASSERT(! IsInPlayout());
    const GoBoard& bd = Board();
    SG_ASSERT(! bd.Rules().AllowSuicide());

    if (GoBoardUtil::TwoPasses(bd))
        // This will be evaluated with TrompTaylorScore in Evaluate
        // It is not clear how to handle other rules, because we do not
        // have an evaluation function for other terminal positions
        return;

    SgBlackWhite toPlay = ToPlay();
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        if (bd.IsEmpty(p)
            && ! GoEyeUtil::IsSimpleEye(bd, p, toPlay)
            && ! m_allSafe[p]
            && bd.IsLegal(p, toPlay))
            moves.push_back(p);
    }
    // Full randomization is too expensive and in most cases not necessary,
    // if prior knowledge is available for initialization or RAVE values are
    // available after playing the first move. However we put a random move
    // to the front, because the first move in a Go board iteration is often
    // a bad corner move
    if (moves.size() > 1)
        swap(moves[0], moves[m_random.Int(moves.size())]);
    moves.push_back(SG_PASS);
}

SgMove GoUctGlobalSearchState::GeneratePlayoutMove(bool& skipRaveUpdate)
{
    SG_ASSERT(IsInPlayout());
    if (m_param.m_mercyRule && CheckMercyRule())
        return SG_NULLMOVE;
    const GoUctBoard& bd = UctBoard();
    SG_DEBUG_ONLY(bd);
    SgPoint move = m_policy->GenerateMove();
    SG_ASSERT(move != SG_NULLMOVE);
#ifndef NDEBUG
    // Check that policy generates pass only if no points are left for which
    // GeneratePoint() returns true. See GoUctPlayoutPolicy::GenerateMove()
    if (move == SG_PASS)
        for (GoUctBoard::Iterator it(bd); it; ++it)
            SG_ASSERT(   bd.Occupied(*it)
                     || m_safe.OneContains(*it)
                     || GoBoardUtil::SelfAtari(bd, *it)
                     // with probabilistic self atari removal.
                     || ! GoUctUtil::GeneratePoint(bd, *it, ToPlay()));
    else
        SG_ASSERT(! m_safe.OneContains(move) || m_policy->WasCleanupMove());
#endif
    // The position guaranteed to be a terminal position, which can be
    // evaluated with GoBoardUtil::ScoreSimpleEndPosition(), only after two
    // passes in a row, both of them generated by GeneratePlayoutMove() in
    // the playout phase
    if (move == SG_PASS)
    {
        skipRaveUpdate = true; // Don't update RAVE values for pass moves
        if (m_passMovesPlayoutPhase < 2)
            ++m_passMovesPlayoutPhase;
        else
            return SG_NULLMOVE;
    }
    else
        m_passMovesPlayoutPhase = 0;
    return move;
}

void GoUctGlobalSearchState::GameStart()
{
    GoUctState::GameStart();
    int size = Board().Size();
    m_mercyRuleThreshold = static_cast<int>(0.3 * size * size);
}

void GoUctGlobalSearchState::SetPolicy(GoUctPlayoutPolicy<GoUctBoard>* policy)
{
    m_policy.reset(policy);
}

void GoUctGlobalSearchState::EndPlayout()
{
    GoUctState::EndPlayout();
    m_policy->EndPlayout();
}

void GoUctGlobalSearchState::StartPlayout()
{
    GoUctState::StartPlayout();
    m_passMovesPlayoutPhase = 0;
    m_mercyRuleTriggered = false;
    const GoBoard& bd = Board();
    m_stoneDiff = bd.All(SG_BLACK).Size() - bd.All(SG_WHITE).Size();
    m_policy->StartPlayout();
}

void GoUctGlobalSearchState::StartPlayouts()
{
    GoUctState::StartPlayouts();
}

void GoUctGlobalSearchState::StartSearch()
{
    GoUctState::StartSearch();
    const GoBoard& bd = Board();
    int size = bd.Size();
    float maxScore = size * size + bd.Rules().Komi().ToFloat();
    m_invMaxScore = 1 / maxScore;
}

//----------------------------------------------------------------------------

GoUctGlobalSearchStateFactory
::GoUctGlobalSearchStateFactory(GoBoard& bd,
                  GoUctPlayoutPolicyFactory<GoUctBoard>& playoutPolicyFactory,
                  const SgBWSet& safe,
                  const SgPointArray<bool>& allSafe)
    : m_bd(bd),
      m_playoutPolicyFactory(playoutPolicyFactory),
      m_safe(safe),
      m_allSafe(allSafe)
{
}

SgUctThreadState* GoUctGlobalSearchStateFactory::Create(
                              std::size_t threadId, const SgUctSearch& search)
{
    const GoUctGlobalSearch& globalSearch =
        dynamic_cast<const GoUctGlobalSearch&>(search);
    GoUctGlobalSearchState* state =
        new GoUctGlobalSearchState(threadId, globalSearch.Board(), 0,
                                   globalSearch.m_param, m_safe, m_allSafe);
    GoUctPlayoutPolicy<GoUctBoard>* policy =
        m_playoutPolicyFactory.Create(state->UctBoard());
    state->SetPolicy(policy);
    return state;
}

//----------------------------------------------------------------------------

GoUctGlobalSearch::GoUctGlobalSearch(GoBoard& bd,
                        GoUctPlayoutPolicyFactory<GoUctBoard>* playoutFactory)
    : GoUctSearch(bd, 0),
      m_regions(bd)
{
    playoutFactory->SetSafe(&m_safe);
    playoutFactory->SetAllSafe(&m_allSafe);
    SgUctThreadStateFactory* stateFactory =
        new GoUctGlobalSearchStateFactory(bd, *playoutFactory, m_safe,
                                          m_allSafe);
    SetThreadStateFactory(stateFactory);
    SetDefaultParameters(bd.Size());
}

float GoUctGlobalSearch::InverseEval(float eval) const
{
    return (1 - eval);
}

void GoUctGlobalSearch::OnStartSearch()
{
    GoUctSearch::OnStartSearch();
    m_safe.Clear();
    m_allSafe.Fill(false);
    if (GOUCT_USE_SAFETY_SOLVER)
    {
        GoBoard& bd = Board();
        GoSafetySolver solver(bd, &m_regions);
        solver.FindSafePoints(&m_safe);
        for (GoBoard::Iterator it(bd); it; ++it)
            m_allSafe[*it] = m_safe.OneContains(*it);
    }
}

void GoUctGlobalSearch::SetDefaultParameters(int boardSize)
{
    SetFirstPlayUrgency(1);
    SetMoveSelect(SG_UCTMOVESELECT_COUNT);
    SetRave(true);
    SetRaveWeightInitial(1.0);
    SetRaveWeightFinal(5000);
    SetUseSignatures(false);
    SetSignatureWeightInitial(0.2);
    SetSignatureWeightFinal(500);
    if (boardSize <= 13)
    {
        // These parameters were mainly tested on 9x9
        SetNoBiasTerm(false);
        SetBiasTermConstant(0.02);
    }
    else
    {
        // These parameters were mainly tested on 19x19
        SetNoBiasTerm(true);
    }
}

float GoUctGlobalSearch::UnknownEval() const
{
    // Note: 0.5 is not a possible value for a Bernoulli variable, better
    // use 0?
    return 0.5;
}

//----------------------------------------------------------------------------
