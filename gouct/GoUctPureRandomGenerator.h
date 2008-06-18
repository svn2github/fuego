//----------------------------------------------------------------------------
/** @file GoUctPureRandomGenerator.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_PURERANDOMGENERATOR_H
#define GOUCT_PURERANDOMGENERATOR_H

#include <vector>
#include "GoBoard.h"
#include "GoUctUtil.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Randomly select from empty points on the board.
    Finds and shuffles the empty points on the board at the beginning
    to avoid repeated loops over the board to find empty points.
*/
template<class BOARD>
class GoUctPureRandomGenerator
{
public:
    GoUctPureRandomGenerator(const BOARD& bd, SgRandom& random);

    /** Finds and shuffles the empty points currently on the board. */
    void Start();

    /** Update state.
        Must be called after each play on the board.
    */
    void OnPlay();

    /** Return a list of points that are currently potentially empty.
        As a side-benefit, the generator can be used to get the list of empty
        points on the board to speed up full-board loops over empty points
        or to get a shuffled list of the empty points (e.g. for finding
        legal moves when expanding a node in the in-tree-phase of UCT).
        Points in the list are candidates, they still have to be tested, if
        they are really empty.
    */
    const std::vector<SgPoint>& Candidates() const;

    /** Generate a pure random move.
        Randomly select an empty point on the board that fulfills
        GoUctUtil::GeneratePoint() for the color currently to play on the
        board.
    */
    SgPoint Generate();

private:
    const BOARD& m_bd;

    SgRandom& m_random;

    /** Points that are potentially empty. */
    std::vector<SgPoint> m_candidates;

    void CheckConsistency() const;

    void Insert(SgPoint p);
};

template<class BOARD>
GoUctPureRandomGenerator<BOARD>::GoUctPureRandomGenerator(const BOARD& bd,
                                                          SgRandom& random)
    : m_bd(bd),
      m_random(random)
{
    m_candidates.reserve(GO_MAX_NUM_MOVES);
}

template<class BOARD>
inline const std::vector<SgPoint>&
GoUctPureRandomGenerator<BOARD>::Candidates()
    const
{
    return m_candidates;
}

template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::CheckConsistency() const
{
#if 0 // Expensive check, enable only for debugging
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.IsEmpty(p))
            if (find(m_candidates.begin(), m_candidates.end(), p)
                == m_candidates.end())
            {
                SgDebug() << m_bd
                          << "Candidates: " << SgWritePointList(m_candidates)
                          << "does not contain: " << SgWritePoint(p)
                          << "\nm_bd.CapturedStones(): "
                          << SgWriteSPointList<SG_MAX_ONBOARD + 1>
                                                   (m_bd.CapturedStones())
                          << "Last move: "
                          << SgWritePoint(m_bd.GetLastMove()) << '\n';
                ASSERT(false);
            }
    }
#endif
}

template<class BOARD>
inline SgPoint GoUctPureRandomGenerator<BOARD>::Generate()
{
    CheckConsistency();
    SgBlackWhite toPlay = m_bd.ToPlay();
    size_t i = m_candidates.size();
    while (true)
    {
        if (i == 0)
            break;
        --i;
        SgPoint p = m_candidates[i];
        if (! m_bd.IsEmpty(p))
        {
            m_candidates[i] = m_candidates[m_candidates.size() - 1];
            m_candidates.pop_back();
            continue;
        }
        if (GoUctUtil::GeneratePoint(m_bd, p, toPlay))
        {
            CheckConsistency();
            return p;
        }
    }
    CheckConsistency();
    return SG_NULLMOVE;
}

template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::OnPlay()
{
    // Don't remove stone played, too expensive, check later in Generate()
    // that generated point is still empty
    for (GoPointList::Iterator it(m_bd.CapturedStones()); it; ++it)
        Insert(*it);
    CheckConsistency();
}

/** Insert new candidate at random place. */
template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::Insert(SgPoint p)
{
    size_t size = m_candidates.size();
    if (size == 0)
        m_candidates.push_back(p);
    else
    {
        SgPoint& swapPoint = m_candidates[m_random.Int(size)];
        m_candidates.push_back(swapPoint);
        swapPoint = p;
    }
}

template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::Start()
{
    m_candidates.clear();
    for (typename BOARD::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it))
            Insert(*it);
    CheckConsistency();
}

//----------------------------------------------------------------------------

#endif // GOUCT_PURERANDOMGENERATOR_H
