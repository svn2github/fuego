//----------------------------------------------------------------------------
/** @file GoUctDefaultPlayoutPolicy.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTPLAYOUTPOLICY_H
#define GOUCT_DEFAULTPLAYOUTPOLICY_H

#include <iostream>
#include "GoUctGlobalSearch.h"
#include "GoUctPatterns.h"
#include "GoUctPureRandomGenerator.h"

//----------------------------------------------------------------------------

/** Parameters for GoUctDefaultPlayoutPolicy. */
class GoUctDefaultPlayoutPolicyParam
{
public:
    /** Don't use any move generating heuristics. */
    bool m_pureRandom;

    /** Shift move to neighbor if it would make an ugly clump.
        See GoUctUtil::DoClumpCorrection
    */
    bool m_useClumpCorrection;

    /** Enable collection of statistics.
        Has a negative impact on performance. Default is false.
    */
    bool m_statisticsEnabled;

    GoUctDefaultPlayoutPolicyParam();
};

//----------------------------------------------------------------------------

/** Statistics collected by GoUctDefaultPlayoutPolicy */
struct GoUctDefaultPlayoutPolicyStat
{
    /** Number of moves generated. */
    std::size_t m_nuMoves;

    /** Number of pure random moves played. */
    std::size_t m_nuRandMoves;

    /** Length of sequences of consecutive non-pure-random moves. */
    SgUctStatistics m_nonRandLen;

    /** Length of list of equivalent best moves.
        Does not include the length of the move list for pure random moves.
    */
    SgUctStatistics m_moveListLen;

    void Clear();

    void Write(std::ostream& out) const;
};

//----------------------------------------------------------------------------

/** Random playout policy for global UCT search.
    Parametrized by the board class to make it usable with both GoBoard
    and GoUctBoard.
    If all heuristics are disabled, the policy plays purely random moves.
    The order and types of the heuristics are inspired by the first
    technical report about the MoGo program.
    Instances of this class must be thread-safe during a search.
*/
template<class BOARD>
class GoUctDefaultPlayoutPolicy
    : public GoUctPlayoutPolicy<BOARD>
{
public:
    /** Constructor.
        @param bd
        @param param The parameters. The policy stores a reference to @c param
        to allow changing the parameters of a group of playout policies later.
        Therefore the lifetime of @c param must exceed the lifetime of the
        policy.
        @param safe Safety information. Stores a reference. Lifetime of
        argument must exceed lifetime of this object.
        @param allSafe Safety information. Stores a reference. Lifetime of
        argument must exceed lifetime of this object.
    */
    GoUctDefaultPlayoutPolicy(const BOARD& bd,
                              const GoUctDefaultPlayoutPolicyParam& param,
                              const SgBWSet& safe,
                              const SgPointArray<bool>& allSafe);


    /** @name Pure virtual functions of GoUctPlayoutPolicy */
    // @{

    /** Implementation of GoUctPlayoutPolicy::GenerateMove().
        Generates a random move in the following order:
        -# Atari heuristic (if enabled)
        -# Proximity heuristic (if enabled) (using patterns if enabled)
        -# Capture heuristic (if enabled)
        -# Purely random
    */
    SgPoint GenerateMove();

    // @} // @name


    /** @name Virtual functions of GoUctPlayoutPolicy */
    // @{

    void EndPlayout();

    void StartPlayout();

    void OnPlay();

    /** true if most recently generated move was cleanup */
    bool WasCleanupMove();

    // @} // @name


    /** @name Statistics */
    // @{

    /** Return current statistics.
        The statistics are only collected, if enabled with
        EnableStatistics().
    */
    const GoUctDefaultPlayoutPolicyStat& Statistics() const;

    void ClearStatistics();

    // @} // @name


    /** Use the list of equivalent best moves to initialize prior
        knowledge values.
        The resulting value array is indexed by moves. If the move was pure
        random, all init counts are 0. Otherwise the values for all equivalent
        best moves is 1, for all other moves 0.
        @see PriorInitCount()
    */
    void GetPriorKnowledge(SgArray<float,SG_PASS+1>& values,
                           SgArray<std::size_t,SG_PASS+1>& counts);

    /** Return the list of equivalent best moves from last move generation.
        The played move was randomly selected from this list.
    */
    SgSList<SgPoint,SG_MAXPOINT> GetEquivalentBestMoves() const;

private:
    /** A function that possibly corrects a given point */
    typedef bool Corrector(const BOARD&, SgPoint&);

    /** Incrementally keeps track of blocks in atari. */
    class CaptureGenerator
    {
    public:
        CaptureGenerator(const BOARD& bd);

        void StartPlayout();

        void OnPlay();

        /** Generate capture moves.
            @param[out] moves The resulting list of capture moves. The passed
            in list is expected to be empty.
        */
        void Generate(SgPointSList& moves);

    private:
        const BOARD& m_bd;

        /** Anchor stones of blocks that need to be checked for atari. */
        std::vector<SgPoint> m_candidates;
    };

    /** Maximum number of liberties in low-lib policy */
    static const int LIB_LIMIT = 2;

    /** Use patterns around last own move, too */
    static const bool SECOND_LAST_MOVE_PATTERNS = true;

    static const bool DEBUG_CORRECT_MOVE = false;

    const GoUctDefaultPlayoutPolicyParam& m_param;

    GoUctPatterns<BOARD> m_patterns;

    /** m_moves have already been checked, skip GeneratePoint test.  */
    bool m_checked;

    /** true if most recent move was generated in cleanup phase.  */
    bool m_wasCleanupMove;

    /** Was the last move randomly selected from all legal moves.
        True, if none of the non-random rules triggered.
    */
    bool m_wasPureRandom;

    /** See GoUctDefaultPlayoutPolicyStat::m_nonRandLen. */
    std::size_t m_nonRandLen;

    /** Last move.
        Stored in member variable to avoid multiple calls to
        GoBoard::GetLastMove during GenerateMove.
    */
    SgPoint m_lastMove;

    /** List of equivalent best moves generated by the policy.
        The highest priority heuristic will generate all moves in this list.
        Moves in this list are not yet checked, if they are legal.
        This list is not used in GenerateMove(), if a pure random move
        is generated.
    */
    SgPointSList m_moves;

    /** The area for which moves should not be generated.
        Set by search.
    */
    const SgBWSet& m_safe;

    /** Safe points of either color. Stored for efficiency.
        m_allSafe == m_safe.Both().
    */
    const SgPointArray<bool>& m_allSafe;

    SgRandom m_random;

    CaptureGenerator m_captureGenerator;

    GoUctPureRandomGenerator<BOARD> m_pureRandomGenerator;

    GoUctDefaultPlayoutPolicyStat m_statistics;

    /** Try to correct the proposed move, typically by moving it.
        Examples: selfataries, clumps.
    */
    bool CorrectMove(
                    GoUctDefaultPlayoutPolicy<BOARD>::Corrector& corrFunction,
                    SgPoint& mv);

    bool GainsLiberties(SgPoint anchor, SgPoint lib) const;

    /** Captures if last move was self-atari */
    bool GenerateAtariCaptureMove();

    /** Generate escapes if last move was atari. */
    bool GenerateAtariDefenseMove();

    /** Generate low lib moves around last move */
    bool GenerateLowLibMove(SgPoint lastMove);

    bool GeneratePatternMove();

    void GeneratePureRandom();

    bool GeneratePoint(SgPoint p) const;

    SgPoint SelectRandom();

    void UpdateStatistics();
};

// Macro to keep compile-time option to experiment with safety code without
// impact on runtime. Remove, if no longer used
// Note that for the safety code, GoUctUtil::SelectRandom also needs to
// be passed m_allSafe to not generate moves in safe areas.
#define GOUCT_ISSAFE(p) false
//define GOUCT_ISSAFE(p) m_allSafe(p)

template<class BOARD>
GoUctDefaultPlayoutPolicy<BOARD>::CaptureGenerator
::CaptureGenerator(const BOARD& bd)
    : m_bd(bd)
{
    m_candidates.reserve(GO_MAX_NUM_MOVES);
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::CaptureGenerator::StartPlayout()
{
    m_candidates.clear();
    for (typename BOARD::Iterator it(m_bd); it; ++it)
    {
        const SgPoint p = *it;
        if (m_bd.Occupied(p) && m_bd.Anchor(p) == p && m_bd.InAtari(p))
            m_candidates.push_back(p);
    }
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::CaptureGenerator::OnPlay()
{
    SgPoint lastMove = m_bd.GetLastMove();
    if (lastMove == SG_NULLMOVE || lastMove == SG_PASS)
        return;
    if (m_bd.OccupiedInAtari(lastMove))
        m_candidates.push_back(m_bd.Anchor(lastMove));
    if (m_bd.OccupiedInAtari(lastMove + SG_NS))
        m_candidates.push_back(m_bd.Anchor(lastMove + SG_NS));
    if (m_bd.OccupiedInAtari(lastMove - SG_NS))
        m_candidates.push_back(m_bd.Anchor(lastMove - SG_NS));
    if (m_bd.OccupiedInAtari(lastMove + SG_WE))
        m_candidates.push_back(m_bd.Anchor(lastMove + SG_WE));
    if (m_bd.OccupiedInAtari(lastMove - SG_WE))
        m_candidates.push_back(m_bd.Anchor(lastMove - SG_WE));
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>
::CaptureGenerator::Generate(SgPointSList& moves)
{
    SG_ASSERT(moves.IsEmpty());
    const SgBlackWhite opp = m_bd.Opponent();
    // For efficiency reasons, this function does not check, if the same
    // move is generated multiple times (and will therefore played with
    // higher probabilty, if there are also other capture moves), because in
    // nearly all cases, there is zero or one global capture move on the
    // board. Most captures are done immediately by the atari heuristic
    for (size_t i = 0; i < m_candidates.size(); ++i)
    {
        const SgPoint p = m_candidates[i];
        if (! m_bd.OccupiedInAtari(p))
        {
            m_candidates[i] = m_candidates[m_candidates.size() - 1];
            m_candidates.pop_back();
            --i;
            continue;
        }
        if (m_bd.GetColor(p) == opp)
            moves.Append(m_bd.TheLiberty(p));
    }
}

template<class BOARD>
GoUctDefaultPlayoutPolicy<BOARD>::GoUctDefaultPlayoutPolicy(
                                 const BOARD& bd,
                                 const GoUctDefaultPlayoutPolicyParam& param,
                                 const SgBWSet& safe,
                                 const SgPointArray<bool>& allSafe)
    : GoUctPlayoutPolicy<BOARD>(bd),
      m_param(param),
      m_patterns(bd),
      m_checked(false),
      m_wasCleanupMove(false),
      m_safe(safe),
      m_allSafe(allSafe),
      m_captureGenerator(bd),
      m_pureRandomGenerator(bd, m_random)
{
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::ClearStatistics()
{
    m_statistics.Clear();
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::CorrectMove(
                    GoUctDefaultPlayoutPolicy<BOARD>::Corrector& corrFunction,
                    SgPoint& mv)
{
#if DEBUG
    const SgPoint oldMv = mv;
#endif
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (! corrFunction(bd, mv))
        return false;

    m_moves.Clear();
    m_moves.Append(mv);
    m_wasPureRandom = false;

#if DEBUG
    if (DEBUG_CORRECT_MOVE)
        SgDebug() << bd
                  << "Replace " << SgWriteMove(oldMv, bd.ToPlay())
                  << " by " << SgWriteMove(mv, bd.ToPlay()) << '\n';
#endif
    return true;
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::EndPlayout()
{
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GainsLiberties(SgPoint anchor,
                                                      SgPoint lib) const
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    const SgBlackWhite color = bd.GetStone(anchor);
    SG_ASSERT(bd.IsEmpty(lib));
    int nu = -1; // lose 1 lib by playing on lib itself.
    for (SgNb4Iterator it(lib); it; ++it)
    {
        if (bd.IsEmpty(*it))
        {
            if (! bd.IsLibertyOfBlock(*it, anchor))
            {
                if (++nu > 0)
                    return true;
            }
        }
        else if (bd.IsColor(*it, color)) // merge with block
        {
            const SgPoint anchor2 = bd.Anchor(*it);
            if (anchor != anchor2)
                for (typename BOARD::LibertyIterator it(bd, anchor2); it;
                     ++it)
                    if (! bd.IsLibertyOfBlock(*it, anchor))
                    {
                        if (++nu > 0)
                            return true;
                    }
        }
        // else capture - ignore since it already has higher priority in UCT
    }
    return false;
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GenerateLowLibMove(SgPoint lastMove)
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    SG_ASSERT(! IsSpecialMove(lastMove));
    SG_ASSERT(! bd.IsEmpty(lastMove));
    const SgBlackWhite toPlay = bd.ToPlay();

    // take liberty of last move
    if (bd.NumLiberties(lastMove) <= LIB_LIMIT)
    {
        for (typename BOARD::LibertyIterator it(bd, lastMove); it; ++it)
        {
            if (   GainsLiberties(bd.Anchor(lastMove), *it)
                && ! GoBoardUtil::SelfAtari(bd, *it)
               )
                m_moves.Append(*it);
        }
    }

    // play liberties of neighbor blocks
    SgSList<SgPoint,4> anchorList;
    for (SgNb4Iterator it(lastMove); it; ++it)
    {
        if (   bd.GetColor(*it) == toPlay
            && ! GOUCT_ISSAFE(*it)
            && bd.NumLiberties(*it) <= LIB_LIMIT
           )
        {
            const SgPoint anchor = bd.Anchor(*it);
            if (! anchorList.Contains(anchor))
            {
                anchorList.Append(anchor);
                for (typename BOARD::LibertyIterator it(bd, anchor); it; ++it)
                    if (   GainsLiberties(anchor, *it)
                        && ! GoBoardUtil::SelfAtari(bd, *it)
                       )
                    {
                        m_moves.Append(*it);
                    }
            }
        }
    }
    return ! m_moves.IsEmpty();
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GenerateAtariCaptureMove()
{
    SG_ASSERT(! IsSpecialMove(m_lastMove));
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (bd.InAtari(m_lastMove))
    {
        SgMove mv = bd.TheLiberty(m_lastMove);
        SG_ASSERT (! m_allSafe[mv]);
        m_moves.Append(mv);
        return true;
    }
    return false;
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GenerateAtariDefenseMove()
{
    SG_ASSERT(m_moves.IsEmpty());
    SG_ASSERT(! IsSpecialMove(m_lastMove));
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    SgBlackWhite toPlay = bd.ToPlay();
    SgSList<SgPoint,4> anchorList;
    for (SgNb4Iterator it(m_lastMove); it; ++it)
    {
        if (bd.GetColor(*it) != toPlay || ! bd.InAtari(*it))
            continue;
        SgPoint anchor = bd.Anchor(*it);
        if (anchorList.Contains(anchor))
            continue;
        anchorList.Append(anchor);

        // Check if move on last liberty would escape the atari
        SgPoint theLiberty = bd.TheLiberty(anchor);
        if (! GoBoardUtil::SelfAtari(bd, theLiberty))
            m_moves.Append(theLiberty);

        // Capture adjacent blocks
        for (GoAdjBlockIterator<BOARD> it2(bd, anchor, 1); it2; ++it2)
        {
            SgPoint oppLiberty = bd.TheLiberty(*it2);
            // If opponent's last liberty is not my last liberty, we know
            // that we will have two liberties after capturing (my last
            // liberty + at least one stone captured). If both last liberties
            // are the same, we already checked above with
            // GoBoardUtil::SelfAtari(theLiberty), if the move escapes the
            // atari
            if (oppLiberty != theLiberty)
                m_moves.Append(oppLiberty);
        }
    }
    return ! m_moves.IsEmpty();
}

template<class BOARD>
SgPoint GoUctDefaultPlayoutPolicy<BOARD>::GenerateMove()
{
    m_moves.Clear();
    m_checked = false;
    m_wasCleanupMove = false;
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    SgPoint mv = SG_NULLMOVE;
    if (m_param.m_pureRandom)
    {
        m_wasPureRandom = true;
        mv = m_pureRandomGenerator.Generate();
        if (mv == SG_NULLMOVE)
            mv = SG_PASS;
        return mv;
    }
    m_wasPureRandom = false;
    m_lastMove = bd.GetLastMove();
    if (   ! IsSpecialMove(m_lastMove) // skip if Pass or Null
        && ! bd.IsEmpty(m_lastMove) // skip if move was suicide
       )
    {
        if (GenerateAtariCaptureMove())
            mv = SelectRandom();
        if (mv == SG_NULLMOVE && GenerateAtariDefenseMove())
            mv = SelectRandom();
        if (mv == SG_NULLMOVE && GenerateLowLibMove(m_lastMove))
            mv = SelectRandom();
        if (mv == SG_NULLMOVE && GeneratePatternMove())
            mv = SelectRandom();
    }
    if (mv == SG_NULLMOVE)
    {
        m_captureGenerator.Generate(m_moves);
        mv = SelectRandom();
    }
    if (mv == SG_NULLMOVE)
    {
        m_wasPureRandom = true;
        mv = m_pureRandomGenerator.Generate();
    }

    if (m_param.m_statisticsEnabled)
        UpdateStatistics();

    if (mv == SG_NULLMOVE)
        mv = SG_PASS;
    else
    {
        SG_ASSERT(bd.IsLegal(mv));
        m_checked =
            CorrectMove(GoUctUtil::DoSelfAtariCorrection, mv);
        SG_ASSERT(m_wasCleanupMove || ! m_allSafe[mv]);
        if (m_param.m_useClumpCorrection && ! m_checked)
            CorrectMove(GoUctUtil::DoClumpCorrection, mv);
    }
    SG_ASSERT(mv == SG_PASS || m_wasCleanupMove || ! m_allSafe[mv]);
    SG_ASSERT(bd.IsLegal(mv));
    SG_ASSERT(mv == SG_PASS || ! bd.IsSuicide(mv));
    return mv;
}

/** Pattern heuristic.
    Use patterns (only in 3x3 neighborhood of last move)
    @see GoUctPatterns
*/
template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GeneratePatternMove()
{
    SG_ASSERT(m_moves.IsEmpty());
    SG_ASSERT(! IsSpecialMove(m_lastMove));
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (! GOUCT_ISSAFE(m_lastMove]))
    {
        for (SgNb8Iterator it(m_lastMove); it; ++it)
            if (   bd.IsEmpty(*it)
                && m_patterns.MatchAny(*it)
                && ! GoBoardUtil::SelfAtari(bd, *it)
                )
                m_moves.Append(*it);
    }
    if (SECOND_LAST_MOVE_PATTERNS)
    {
        const SgPoint lastMove2 = bd.Get2ndLastMove();
        if (! IsSpecialMove(lastMove2) && ! GOUCT_ISSAFE(lastMove2))
        {
            for (SgNb8Iterator it(lastMove2); it; ++it)
                if (   bd.IsEmpty(*it)
                    && ! SgPointUtil::In8Neighborhood(m_lastMove, *it)
                    && m_patterns.MatchAny(*it)
                    && ! GoBoardUtil::SelfAtari(bd, *it)
                    )
                    m_moves.Append(*it);
        }
    }
    return ! m_moves.IsEmpty();
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GeneratePoint(SgPoint p) const
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    return GoUctUtil::GeneratePoint(bd, p, bd.ToPlay());
}

template<class BOARD>
SgSList<SgPoint,SG_MAXPOINT>
GoUctDefaultPlayoutPolicy<BOARD>::GetEquivalentBestMoves() const
{
    SgSList<SgPoint,SG_MAXPOINT> result;
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (m_wasPureRandom)
    {
        for (typename BOARD::Iterator it(bd); it; ++it)
            if (bd.IsEmpty(*it) && GeneratePoint(*it))
                result.Append(*it);
    }
    // Move in m_moves are not checked yet, if legal etc.
    for (SgPointSList::Iterator it(m_moves); it; ++it)
        if (m_checked || GeneratePoint(*it))
            result.Append(*it);
    return result;
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>
::GetPriorKnowledge(SgArray<float,SG_PASS+1>& values,
                    SgArray<std::size_t,SG_PASS+1>& counts)
{
    StartPlayout();
    GenerateMove();
    if (m_wasPureRandom)
        counts.Fill(0);
    else
    {
        const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
        values[SG_PASS] = 0;
        for (typename BOARD::Iterator it(bd); it; ++it)
            if (bd.IsEmpty(*it) && GoBoardUtil::SelfAtari(bd, *it))
                values[*it] = 0;
            else
                values[*it] = 0.5;
        for (SgPointSList::Iterator it(m_moves); it; ++it)
            // Move in m_moves are not checked yet, if legal etc.
            if (GoUctUtil::GeneratePoint<BOARD>(bd, *it, bd.ToPlay()))
                values[*it] = 1;
        counts.Fill(9);
    }
    EndPlayout();
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::OnPlay()
{
    m_captureGenerator.OnPlay();
    m_pureRandomGenerator.OnPlay();
}

template<class BOARD>
SgPoint GoUctDefaultPlayoutPolicy<BOARD>::SelectRandom()
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    return GoUctUtil::SelectRandom(bd, bd.ToPlay(), m_moves, m_random);
}

template<class BOARD>
const GoUctDefaultPlayoutPolicyStat&
GoUctDefaultPlayoutPolicy<BOARD>::Statistics() const
{
    return m_statistics;
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::StartPlayout()
{
    m_captureGenerator.StartPlayout();
    m_pureRandomGenerator.Start();
    m_nonRandLen = 0;
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::UpdateStatistics()
{
    ++m_statistics.m_nuMoves;
    if (m_wasPureRandom)
    {
        if (m_nonRandLen > 0)
        {
            m_statistics.m_nonRandLen.Add(m_nonRandLen);
            m_nonRandLen = 0;
        }
        ++m_statistics.m_nuRandMoves;
    }
    else
    {
        ++m_nonRandLen;
        m_statistics.m_moveListLen.Add(GetEquivalentBestMoves().Length());
    }
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::WasCleanupMove()
{
    return m_wasCleanupMove;
}

//----------------------------------------------------------------------------

template<class BOARD>
class GoUctDefaultPlayoutPolicyFactory
    : public GoUctPlayoutPolicyFactory<BOARD>
{
public:
    /** Constructor.
        @param param Playout policy parameters. Stores a reference. Lifetime
        of the argument must exceed the lifetime of this factory and created
        objects.
    */
    GoUctDefaultPlayoutPolicyFactory(const GoUctDefaultPlayoutPolicyParam&
                                     param);

    GoUctPlayoutPolicy<BOARD>* Create(const BOARD& bd);

private:
    const GoUctDefaultPlayoutPolicyParam& m_param;
};

template<class BOARD>
GoUctDefaultPlayoutPolicyFactory<BOARD>
::GoUctDefaultPlayoutPolicyFactory(const GoUctDefaultPlayoutPolicyParam&
                                   param)
    : m_param(param)
{
}

template<class BOARD>
GoUctPlayoutPolicy<BOARD>*
GoUctDefaultPlayoutPolicyFactory<BOARD>::Create(const BOARD& bd)
{
    SG_ASSERT(GoUctPlayoutPolicyFactory<BOARD>::m_safe != 0);
    SG_ASSERT(GoUctPlayoutPolicyFactory<BOARD>::m_allSafe != 0);
    return new GoUctDefaultPlayoutPolicy<BOARD>(bd, m_param,
                                *GoUctPlayoutPolicyFactory<BOARD>::m_safe,
                                *GoUctPlayoutPolicyFactory<BOARD>::m_allSafe);
}

//----------------------------------------------------------------------------

/** Uses GoUctDefaultPlayoutPolicy to generate prior knowledge.
    See GoUctDefaultPlayoutPolicy::GetPriorKnowledge
*/
class GoUctPolicyPriorKnowledge
    : public SgUctPriorKnowledge
{
public:
    GoUctPolicyPriorKnowledge(const GoBoard& bd,
                              const GoUctDefaultPlayoutPolicyParam& param,
                              const SgBWSet& safe,
                              const SgPointArray<bool>& allSafe);

    void ProcessPosition();

    void InitializeMove(SgMove move, float& value, std::size_t& count);

private:
    GoUctDefaultPlayoutPolicy<GoBoard> m_policy;

    SgArray<float,SG_PASS+1> m_values;

    SgArray<std::size_t,SG_PASS+1> m_counts;
};

//----------------------------------------------------------------------------

class GoUctPolicyPriorKnowledgeFactory
    : public SgUctPriorKnowledgeFactory
{
public:
    /** Stores a reference to param */
    GoUctPolicyPriorKnowledgeFactory(const GoUctDefaultPlayoutPolicyParam&
                                     param);

    SgUctPriorKnowledge* Create(SgUctThreadState& state);

private:
    const GoUctDefaultPlayoutPolicyParam& m_param;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPLAYOUTPOLICY_H
