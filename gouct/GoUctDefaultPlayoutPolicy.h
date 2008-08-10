//----------------------------------------------------------------------------
/** @file GoUctDefaultPlayoutPolicy.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTPLAYOUTPOLICY_H
#define GOUCT_DEFAULTPLAYOUTPOLICY_H

#include <iostream>
#include <boost/array.hpp>
#include "GoUctGlobalSearch.h"
#include "GoUctPatterns.h"
#include "GoUctPureRandomGenerator.h"

//----------------------------------------------------------------------------

/** Parameters for GoUctDefaultPlayoutPolicy. */
class GoUctDefaultPlayoutPolicyParam
{
public:
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

/** Move types used in GoUctDefaultPlayoutPolicy. */
enum GoUctDefaultPlayoutPolicyType
{
    GOUCT_ATARI_CAPTURE,

    GOUCT_ATARI_DEFEND,

    GOUCT_LOWLIB,

    GOUCT_PATTERN,

    GOUCT_CAPTURE,

    GOUCT_RANDOM,

    GOUCT_SELFATARI_CORRECTION,

    GOUCT_CLUMP_CORRECTION,

    GOUCT_PASS,

    _GOUCT_NU_DEFAULT_PLAYOUT_TYPE
};

const char* GoUctDefaultPlayoutPolicyTypeStr(
                                          GoUctDefaultPlayoutPolicyType type);

//----------------------------------------------------------------------------

/** Statistics collected by GoUctDefaultPlayoutPolicy */
struct GoUctDefaultPlayoutPolicyStat
{
    /** Number of moves generated. */
    std::size_t m_nuMoves;

    /** Length of sequences of consecutive non-pure-random moves. */
    SgUctStatistics m_nonRandLen;

    /** Length of list of equivalent best moves.
        Does not include the length of the move list for pure random moves.
    */
    SgUctStatistics m_moveListLen;

    /** Number of moves of a certain type. */
    boost::array<std::size_t,_GOUCT_NU_DEFAULT_PLAYOUT_TYPE> m_nuMoveType;

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
    */
    GoUctDefaultPlayoutPolicy(const BOARD& bd,
                              const GoUctDefaultPlayoutPolicyParam& param);


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

    /** Return the type of the last move generated. */
    GoUctDefaultPlayoutPolicyType MoveType() const;

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


    /** Return the list of equivalent best moves from last move generation.
        The played move was randomly selected from this list.
    */
    GoPointList GetEquivalentBestMoves() const;

    /** Make pattern matcher available for other uses.
        Avoids that a user of the playout policy who also wants to use the
        pattern matcher for other purposes needs to allocate a second
        matcher (Use case: prior knowledge)
    */
    const GoUctPatterns<BOARD>& Patterns() const;

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
        void Generate(GoPointList& moves);

    private:
        const BOARD& m_bd;

        /** Anchor stones of blocks that need to be checked for atari. */
        std::vector<SgPoint> m_candidates;
    };

    /** Use patterns around last own move, too */
    static const bool SECOND_LAST_MOVE_PATTERNS = true;

    static const bool DEBUG_CORRECT_MOVE = false;

    const GoUctDefaultPlayoutPolicyParam& m_param;

    GoUctPatterns<BOARD> m_patterns;

    /** m_moves have already been checked, skip GeneratePoint test.  */
    bool m_checked;

    /** Type of the last generated move. */
    GoUctDefaultPlayoutPolicyType m_moveType;

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
    GoPointList m_moves;

    SgRandom m_random;

    CaptureGenerator m_captureGenerator;

    GoUctPureRandomGenerator<BOARD> m_pureRandomGenerator;

    GoUctDefaultPlayoutPolicyStat m_statistics;

    /** Try to correct the proposed move, typically by moving it to a
        'better' point such as other liberty or neighbor.
        Examples implemented: self-ataries, clumps.
    */
    bool CorrectMove(
                    GoUctDefaultPlayoutPolicy<BOARD>::Corrector& corrFunction,
                    SgPoint& mv, GoUctDefaultPlayoutPolicyType moveType);

    /** Test whether playing on lib increases the liberties of anchor block */
    bool GainsLiberties(SgPoint anchor, SgPoint lib) const;

    /** Captures if last move was self-atari */
    bool GenerateAtariCaptureMove();

    /** Generate escapes if last move was atari. */
    bool GenerateAtariDefenseMove();

    /** Generate low lib moves around lastMove */
    bool GenerateLowLibMove(SgPoint lastMove);

    /** Generate pattern move around last two moves */
    bool GeneratePatternMove();

    void GeneratePatternMove(SgPoint p);

    void GeneratePatternMove2(SgPoint p, SgPoint lastMove);

    void GeneratePureRandom();

    bool GeneratePoint(SgPoint p) const;

    SgPoint SelectRandom();

    void UpdateStatistics();
};

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
    if (m_bd.NumNeighbors(lastMove, m_bd.ToPlay()) == 0)
        return;
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
::CaptureGenerator::Generate(GoPointList& moves)
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
                                 const GoUctDefaultPlayoutPolicyParam& param)
    : GoUctPlayoutPolicy<BOARD>(bd),
      m_param(param),
      m_patterns(bd),
      m_checked(false),
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
                    SgPoint& mv, GoUctDefaultPlayoutPolicyType moveType)
{
#if DEBUG
    const SgPoint oldMv = mv;
#endif
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (! corrFunction(bd, mv))
        return false;

    m_moves.Clear();
    m_moves.Append(mv);
    m_moveType = moveType;

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
bool GoUctDefaultPlayoutPolicy<BOARD>::GenerateAtariCaptureMove()
{
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (bd.InAtari(m_lastMove))
    {
        SgMove mv = bd.TheLiberty(m_lastMove);
        m_moves.Append(mv);
        return true;
    }
    return false;
}

template<class BOARD>
bool GoUctDefaultPlayoutPolicy<BOARD>::GenerateAtariDefenseMove()
{
    SG_ASSERT(m_moves.IsEmpty());
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    SgBlackWhite toPlay = bd.ToPlay();
    if (bd.NumNeighbors(m_lastMove, toPlay) == 0)
        return false;
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
bool GoUctDefaultPlayoutPolicy<BOARD>::GenerateLowLibMove(SgPoint lastMove)
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    SG_ASSERT(! SgIsSpecialMove(lastMove));
    SG_ASSERT(! bd.IsEmpty(lastMove));
    const SgBlackWhite toPlay = bd.ToPlay();

    // take liberty of last move
    if (bd.NumLiberties(lastMove) == 2)
    {
        for (typename BOARD::LibertyIterator it(bd, lastMove); it; ++it)
        {
            if (GainsLiberties(bd.Anchor(lastMove), *it)
                && ! GoBoardUtil::SelfAtari(bd, *it)
               )
                m_moves.Append(*it);
        }
    }

    if (bd.NumNeighbors(lastMove, toPlay) != 0)
    {
        // play liberties of neighbor blocks
        SgSList<SgPoint,4> anchorList;
        for (SgNb4Iterator it(lastMove); it; ++it)
        {
            if (bd.GetColor(*it) == toPlay
                && bd.NumLiberties(*it) == 2)
            {
                const SgPoint anchor = bd.Anchor(*it);
                if (! anchorList.Contains(anchor))
                {
                    anchorList.Append(anchor);
                    for (typename BOARD::LibertyIterator it(bd, anchor); it;
                         ++it)
                        if (GainsLiberties(anchor, *it)
                            && ! GoBoardUtil::SelfAtari(bd, *it))
                        {
                            m_moves.Append(*it);
                        }
                }
            }
        }
    }

    return ! m_moves.IsEmpty();
}

template<class BOARD>
SgPoint GoUctDefaultPlayoutPolicy<BOARD>::GenerateMove()
{
    m_moves.Clear();
    m_checked = false;
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    SgPoint mv = SG_NULLMOVE;
    m_lastMove = bd.GetLastMove();
    if (   ! SgIsSpecialMove(m_lastMove) // skip if Pass or Null
        && ! bd.IsEmpty(m_lastMove) // skip if move was suicide
       )
    {
        if (GenerateAtariCaptureMove())
        {
            m_moveType = GOUCT_ATARI_CAPTURE;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GenerateAtariDefenseMove())
        {
            m_moveType = GOUCT_ATARI_DEFEND;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GenerateLowLibMove(m_lastMove))
        {
            m_moveType = GOUCT_LOWLIB;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GeneratePatternMove())
        {
            m_moveType = GOUCT_PATTERN;
            mv = SelectRandom();
        }
    }
    if (mv == SG_NULLMOVE)
    {
        m_moveType = GOUCT_CAPTURE;
        m_captureGenerator.Generate(m_moves);
        mv = SelectRandom();
    }
    if (mv == SG_NULLMOVE)
    {
        m_moveType = GOUCT_RANDOM;
        mv = m_pureRandomGenerator.Generate();
    }

    if (mv == SG_NULLMOVE)
    {
        m_moveType = GOUCT_PASS;
        mv = SG_PASS;
    }
    else
    {
        SG_ASSERT(bd.IsLegal(mv));
        m_checked = CorrectMove(GoUctUtil::DoSelfAtariCorrection, mv,
                                GOUCT_SELFATARI_CORRECTION);
        if (m_param.m_useClumpCorrection && ! m_checked)
            CorrectMove(GoUctUtil::DoClumpCorrection, mv,
                        GOUCT_CLUMP_CORRECTION);
    }
    SG_ASSERT(bd.IsLegal(mv));
    SG_ASSERT(mv == SG_PASS || ! bd.IsSuicide(mv));

    if (m_param.m_statisticsEnabled)
        UpdateStatistics();

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
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    GeneratePatternMove(m_lastMove + SG_NS - SG_WE);
    GeneratePatternMove(m_lastMove + SG_NS);
    GeneratePatternMove(m_lastMove + SG_NS + SG_WE);
    GeneratePatternMove(m_lastMove - SG_WE);
    GeneratePatternMove(m_lastMove + SG_WE);
    GeneratePatternMove(m_lastMove - SG_NS - SG_WE);
    GeneratePatternMove(m_lastMove - SG_NS);
    GeneratePatternMove(m_lastMove - SG_NS + SG_WE);
    if (SECOND_LAST_MOVE_PATTERNS)
    {
        const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
        const SgPoint lastMove2 = bd.Get2ndLastMove();
        if (! SgIsSpecialMove(lastMove2))
        {
            GeneratePatternMove2(lastMove2 + SG_NS - SG_WE, m_lastMove);
            GeneratePatternMove2(lastMove2 + SG_NS,         m_lastMove);
            GeneratePatternMove2(lastMove2 + SG_NS + SG_WE, m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_WE,         m_lastMove);
            GeneratePatternMove2(lastMove2 + SG_WE,         m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_NS - SG_WE, m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_NS,         m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_NS + SG_WE, m_lastMove);
        }
    }
    return ! m_moves.IsEmpty();
}

template<class BOARD>
inline void GoUctDefaultPlayoutPolicy<BOARD>::GeneratePatternMove(SgPoint p)
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (bd.IsEmpty(p)
        && m_patterns.MatchAny(p)
        && ! GoBoardUtil::SelfAtari(bd, p))
        m_moves.Append(p);
}

template<class BOARD>
inline void GoUctDefaultPlayoutPolicy<BOARD>::GeneratePatternMove2(
                                                  SgPoint p, SgPoint lastMove)
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (bd.IsEmpty(p)
        && ! SgPointUtil::In8Neighborhood(lastMove, p)
        && m_patterns.MatchAny(p)
        && ! GoBoardUtil::SelfAtari(bd, p))
        m_moves.Append(p);
}

template<class BOARD>
inline bool GoUctDefaultPlayoutPolicy<BOARD>::GeneratePoint(SgPoint p) const
{
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    return GoUctUtil::GeneratePoint(bd, p, bd.ToPlay());
}

template<class BOARD>
GoPointList GoUctDefaultPlayoutPolicy<BOARD>::GetEquivalentBestMoves() const
{
    GoPointList result;
    const BOARD& bd = GoUctPlayoutPolicy<BOARD>::Board();
    if (m_moveType == GOUCT_RANDOM)
    {
        for (typename BOARD::Iterator it(bd); it; ++it)
            if (bd.IsEmpty(*it) && GeneratePoint(*it))
                result.Append(*it);
    }
    // Move in m_moves are not checked yet, if legal etc.
    for (GoPointList::Iterator it(m_moves); it; ++it)
        if (m_checked || GeneratePoint(*it))
            result.Append(*it);
    return result;
}

template<class BOARD>
GoUctDefaultPlayoutPolicyType GoUctDefaultPlayoutPolicy<BOARD>::MoveType()
    const
{
    return m_moveType;
}

template<class BOARD>
void GoUctDefaultPlayoutPolicy<BOARD>::OnPlay()
{
    m_captureGenerator.OnPlay();
    m_pureRandomGenerator.OnPlay();
}


template<class BOARD>
const GoUctPatterns<BOARD>& GoUctDefaultPlayoutPolicy<BOARD>::Patterns()
    const
{
    return m_patterns;
}

template<class BOARD>
inline SgPoint GoUctDefaultPlayoutPolicy<BOARD>::SelectRandom()
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
    ++m_statistics.m_nuMoveType[m_moveType];
    if (m_moveType == GOUCT_RANDOM)
    {
        if (m_nonRandLen > 0)
        {
            m_statistics.m_nonRandLen.Add(m_nonRandLen);
            m_nonRandLen = 0;
        }
    }
    else
    {
        ++m_nonRandLen;
        m_statistics.m_moveListLen.Add(GetEquivalentBestMoves().Length());
    }
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
    return new GoUctDefaultPlayoutPolicy<BOARD>(bd, m_param);
}

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPLAYOUTPOLICY_H
