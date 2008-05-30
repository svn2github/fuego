//----------------------------------------------------------------------------
/** @file GoUctGlobalSearch.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_GLOBALSEARCH_H
#define GOUCT_GLOBALSEARCH_H

#include "GoBoard.h"
#include "GoRegionBoard.h"
#include "GoUctSearch.h"

//----------------------------------------------------------------------------

const bool USE_SAFETY_SOLVER = false;

//----------------------------------------------------------------------------

/** Policy for the random play-out phase of GoUctGlobalSearch. */
template<class BOARD>
class GoUctPlayoutPolicy
{
public:
    GoUctPlayoutPolicy(const BOARD& bd);

    virtual ~GoUctPlayoutPolicy();

    /** Called after playout.
        Default implementation does nothing.
    */
    virtual void EndPlayout();

    /** Called at beginning of playout.
        Default implementation does nothing.
    */
    virtual void StartPlayout();

    /** Called after a move eas played
        Default implementation does nothing.
    */
    virtual void OnPlay();

    /** Generate a move in the random play-out phase of GoUctSearch.
        The policy is not allowed to return SG_PASS, as long as there are
        still points on the board, for which GoUctUtil::GeneratePoint
        returns true, because GoUctSearch relies on the fact that it can
        score positions with GoBoardUtil::ScoreSimpleEndPosition after
        two consecutive passes.
    */
    virtual SgPoint GenerateMove() = 0;

    /** true if most recently genenerated move was cleanup
        Default implementation returns false
    */
    virtual bool WasCleanupMove();

protected:
    const BOARD& Board() const;

private:
    const BOARD& m_bd;
};

template<class BOARD>
inline GoUctPlayoutPolicy<BOARD>::GoUctPlayoutPolicy(const BOARD& bd)
    : m_bd(bd)
{
}

template<class BOARD>
GoUctPlayoutPolicy<BOARD>::~GoUctPlayoutPolicy()
{
}

template<class BOARD>
inline const BOARD& GoUctPlayoutPolicy<BOARD>::Board() const
{
    return m_bd;
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::EndPlayout()
{
    // Default implementation does nothing
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::OnPlay()
{
    // Default implementation does nothing
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::StartPlayout()
{
    // Default implementation does nothing
}

template<class BOARD>
inline bool GoUctPlayoutPolicy<BOARD>::WasCleanupMove()
{
    return false;
}

//----------------------------------------------------------------------------

/** Create instances of the playout policies.
    Safety information will be set later (before using) with SetSafe() and
    SetAllSafe(), because this class needs a multi-step construction
    in GoUctGlobalSearch.
*/
template<class BOARD>
class GoUctPlayoutPolicyFactory
{
public:
    GoUctPlayoutPolicyFactory();

    virtual ~GoUctPlayoutPolicyFactory();

    virtual GoUctPlayoutPolicy<BOARD>* Create(const BOARD& bd) = 0;

    void SetSafe(const SgBWSet* safe);

    void SetAllSafe(const SgPointArray<bool>* allSafe);

protected:
    const SgBWSet* m_safe;

    const SgPointArray<bool>* m_allSafe;
};

template<class BOARD>
inline void GoUctPlayoutPolicyFactory<BOARD>::SetSafe(const SgBWSet* safe)
{
    m_safe = safe;
}

template<class BOARD>
inline void GoUctPlayoutPolicyFactory<BOARD>::SetAllSafe(
                                           const SgPointArray<bool>* allSafe)
{
    m_allSafe = allSafe;
}

template<class BOARD>
GoUctPlayoutPolicyFactory<BOARD>::GoUctPlayoutPolicyFactory()
    : m_safe(0),
      m_allSafe(0)
{
}

template<class BOARD>
GoUctPlayoutPolicyFactory<BOARD>::~GoUctPlayoutPolicyFactory()
{
}

//----------------------------------------------------------------------------

/** Parameters for GoUctGlobalSearchState */
struct GoUctGlobalSearchStateParam
{
    /** Use the mercy rule.
        Count games early as win or loss, if stone difference on board
        exceeds a threshold of 30% of the total number of points on board.
    */
    bool m_mercyRule;

    /** Modify game result by score.
        This modifies the win/loss result (1/0) by the score of the end
        position. The modification is added for losses and subtracted for
        wins. The modification value is the score divided by the maximum
        score, which can be reached on the board, times the maximum
        score modification value.
        This helps to play moves to maximize the score even if the game is
        already clearly lost or won. Otherwise all moves look equal in
        clearly won or lost positions.
        It can also reduce the typical game length and could even have a
        positive effect on the playing strength.
        The modification can be disabled by setting the maximum score
        modification value to zero. The default value is 0.02.
    */
    float m_scoreModification;

    GoUctGlobalSearchStateParam();
};

//----------------------------------------------------------------------------

/** @page gouctpassmoves Handling of pass-moves in GoUctGlobalSearch

    @section gouctpassplayout Play-out phase

    Pass moves are not played in the play-out phase of the simulations as
    long as there are still moves for which GoUctUtil::GeneratePoint
    returns true, which are mainly moves that don't fill single point
    eyes (see GoUctUtil::GeneratePoint and GoBoardUtil::IsCompletelySurrounded
    for an exact definition). Therefore, it is a requirement on
    GoUctPlayoutPolicy::GenerateMove, not to return pass moves earlier.

    This requirement ensures that all simulated games terminate (as long as
    there is no super-ko cycle, because for speed reasons only simple ko's
    are checked) and that the terminal position after two passes in a row is
    a position that can be quickly evaluated with
    GoBoardUtil::ScoreSimpleEndPosition.

    @section gouctpassintree In-tree phase

    In the in-tree-phase of the game, pass moves are always allowed to avoid
    zugzwang situations, if there is a seki on the board. After two
    passes the game is terminated and scored with
    GoBoardUtil::TrompTaylorScore.
*/

/** Global UCT-Search for Go.
    - @ref gouctpassmoves
*/
class GoUctGlobalSearchState
    : public GoUctState
{
public:
    const SgBWSet& m_safe;

    const SgPointArray<bool>& m_allSafe;

    /** Constructor.
        @param threadId The number of the thread. Needed for passing to
        constructor of SgUctThreadState.
        @param bd The board
        @param policy The random policy (takes ownership). It is possible to
        set the policy to null at construction time to allowed a multi-step
        construction; but then a policy has to be set with SetPolicy(), before
        the search is used.
        @param param Parameters. Stores a reference to the argument.
        @param safe Safety information. Stores a reference to the argument.
        @param allSafe Safety information. Stores a reference to the argument.
    */
    GoUctGlobalSearchState(std::size_t threadId, const GoBoard& bd,
                           GoUctPlayoutPolicy<GoUctBoard>* policy,
                           const GoUctGlobalSearchStateParam& param,
                           const SgBWSet& safe,
                           const SgPointArray<bool>& allSafe);

    ~GoUctGlobalSearchState();

    float Evaluate();

    void GenerateAllMoves(std::vector<SgMove>& moves);

    SgMove GenerateRandomMove(bool& skipRaveUpdate);

    void Execute(SgMove move);

    void GameStart();

    void EndPlayout();

    void StartPlayout();

    void StartPlayouts();

    void StartSearch();

    GoUctPlayoutPolicy<GoUctBoard>* Policy();

    /** Set random policy.
        Sets a new random policy and deletes the old one, if it existed.
    */
    void SetPolicy(GoUctPlayoutPolicy<GoUctBoard>* policy);

private:
    const GoUctGlobalSearchStateParam& m_param;

    /** See SetMercyRule() */
    bool m_mercyRuleTriggered;

    /** Number of pass moves played in a row in the playout phase. */
    int m_passMovesPlayoutPhase;

    /** See SetMercyRule() */
    int m_mercyRuleThreshold;

    /** Difference of stones on board.
        Black counts positive.
    */
    int m_stoneDiff;

    /** The area in which moves should be generated. */
    SgPointSList m_area;

    /** See SetMercyRule() */
    float m_mercyRuleResult;

    /** Inverse of maximum score, one can reach on a board of the current
        size.
    */
    float m_invMaxScore;

    SgRandom m_random;

    std::auto_ptr<GoUctPlayoutPolicy<GoUctBoard> > m_policy;

    /** Not implemented */
    GoUctGlobalSearchState(const GoUctGlobalSearchState& search);

    /** Not implemented */
    GoUctGlobalSearchState& operator=(const GoUctGlobalSearchState& search);

    bool CheckMercyRule();

    template<class BOARD>
    float EvaluateBoard(const BOARD& bd, float komi) const;
};

inline GoUctPlayoutPolicy<GoUctBoard>* GoUctGlobalSearchState::Policy()
{
    return m_policy.get();
}

//----------------------------------------------------------------------------

class GoUctGlobalSearchStateFactory
    : public SgUctThreadStateFactory
{
public:
    /** Constructor.
        @param bd
        @param playoutPolicyFactory Factory for playout policies.
        Stores a reference. Lifetime of parameter must exceed the lifetime of
        this instance.
        @param safe
        @param allSafe
    */
    GoUctGlobalSearchStateFactory(GoBoard& bd,
                GoUctPlayoutPolicyFactory<GoUctBoard>& playoutPolicyFactory,
                const SgBWSet& safe,
                const SgPointArray<bool>& allSafe);

    SgUctThreadState* Create(std::size_t threadId, const SgUctSearch& search);

private:
    GoBoard& m_bd;

    GoUctPlayoutPolicyFactory<GoUctBoard>& m_playoutPolicyFactory;

    const SgBWSet& m_safe;

    const SgPointArray<bool>& m_allSafe;
};

//----------------------------------------------------------------------------

class GoUctGlobalSearch
    : public GoUctSearch
{
public:
    GoUctGlobalSearchStateParam m_param;

    /** Constructor.
        @param bd
        @param playoutPolicyFactory Creates multiple instances of the playout
        policies. Takes ownership. playoutPolicyFactory should not have
        SetSafe() and SetAllSafe() already set, because the search will call
        these functions using its own safety information.
    */
    GoUctGlobalSearch(GoBoard& bd,
                GoUctPlayoutPolicyFactory<GoUctBoard>* playoutPolicyFactory);

    /** @name Pure virtual functions of SgUctSearch */
    // @{

    float InverseEval(float eval) const;

    float UnknownEval() const;

    // @} // @name


    /** @name Virtual functions of SgUctSearch */
    // @{

    void OnStartSearch();

    // @} // @name

private:
    SgBWSet m_safe;

    SgPointArray<bool> m_allSafe;

    std::auto_ptr<GoUctPlayoutPolicyFactory<GoUctBoard> >
    m_playoutPolicyFactory;

    GoRegionBoard m_regions;
};

//----------------------------------------------------------------------------

#endif // GOUCT_GLOBALSEARCH_H
