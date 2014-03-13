//----------------------------------------------------------------------------
/** @file GoUctDefaultMoveFilter.cpp
    See GoUctDefaultMoveFilter.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultMoveFilter.h"

#include "GoBensonSolver.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "GoSafetySolver.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------
namespace {
inline bool IsEmpty2x3Box(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT (bd.Line(p) == 1);
    SG_ASSERT (bd.Pos(p) > 1);
    return bd.IsEmpty(p) && bd.Num8EmptyNeighbors(p) == 5;
}

inline bool IsEmptyOrInCorner(const GoBoard& bd, SgPoint p, int direction)
{
    return bd.Pos(p + direction) == 1
        || IsEmpty2x3Box(bd, p + direction);
}

/** Return true, if point is on edge line and no stone is within a
    Manhattan distance of 4. */
bool IsEmptyEdge(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT (bd.IsEmpty(p));
    SG_ASSERT (bd.Line(p) == 1);
    if (bd.Num8EmptyNeighbors(p) < 5 && bd.Pos(p) > 1)
        return false;
    const SgPoint pUp = p + bd.Up(p);
    SG_ASSERT(bd.Line(pUp) == 2);
    SG_ASSERT(bd.Pos(pUp) >= 2); // (1,1) goes to (2,2)
    if (bd.Num8EmptyNeighbors(pUp) < 8)
        return false;

    switch (bd.Pos(p))
    {
        case 1: // (1,1) point
        case 2: // (1,2) point
        case 3:
            return IsEmptyOrInCorner(bd, p, bd.Left(p))
                && IsEmptyOrInCorner(bd, p, bd.Right(p));
            // assume in empty corner, 1st line is always
            // dominated bymove on 2nd line above
        default: // > 3, can test both sides easily
            return   IsEmpty2x3Box(bd, p + 2*bd.Left(p))
                  && IsEmpty2x3Box(bd, p + 2*bd.Right(p))
                  ;
    }
}

} // namespace
//----------------------------------------------------------------------------

GoUctDefaultMoveFilterParam::GoUctDefaultMoveFilterParam()
    :  m_checkLadders(true),
       m_checkOffensiveLadders(true),
       m_minLadderLength(6),
       m_filterFirstLine(true),
       m_checkSafety(true)
{ }

//----------------------------------------------------------------------------

GoUctDefaultMoveFilter::GoUctDefaultMoveFilter(const GoBoard& bd, const GoUctDefaultMoveFilterParam &param)
    : m_bd(bd),
      m_param(param)
{ }

vector<SgPoint> GoUctDefaultMoveFilter::Get()
{
    vector<SgPoint> rootFilter;
    const SgBlackWhite toPlay = m_bd.ToPlay();
    const SgBlackWhite opp = SgOppBW(toPlay);

    // Safe territory
    if (m_param.m_checkSafety)
    {
        SgBWSet alternateSafe;
        // Alternate safety is used to prune moves only in opponent territory
        // and only if everything is alive under alternate play. This ensures that
        // capturing moves that are not liberties of dead blocks and ko threats
        // will not be pruned. This alternate safety pruning is not going to
        // improve or worsen playing strength, but may cause earlier passes,
        // which is nice in games against humans
        GoSafetySolver safetySolver(m_bd);
        safetySolver.FindSafePoints(&alternateSafe);

        // Benson solver guarantees that capturing moves of dead blocks are
        // liberties of the dead blocks and that no move in Benson safe territory
        // is a ko threat
        GoBensonSolver bensonSolver(m_bd);
        SgBWSet unconditionalSafe;
        bensonSolver.FindSafePoints(&unconditionalSafe);

        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            const SgPoint p = *it;
            if (m_bd.IsLegal(p))
            {
                bool isUnconditionalSafe = unconditionalSafe[toPlay].Contains(p);
                bool isUnconditionalSafeOpp = unconditionalSafe[opp].Contains(p);
                bool isAlternateSafeOpp = alternateSafe[opp].Contains(p);
                bool hasOppNeighbors = m_bd.HasNeighbors(p, opp);
                // Always generate capturing moves in own safe territory, even
                // if current rules do no use CaptureDead(), because the UCT
                // player always scores with Tromp-Taylor after two passes in the
                // in-tree phase
                // if (  (isAllAlternateSafe && isAlternateSafeOpp)
                if (  isAlternateSafeOpp
                   || isUnconditionalSafeOpp
                   || (isUnconditionalSafe && ! hasOppNeighbors)
                   || (  alternateSafe[toPlay].Contains(p)
                      && ! safetySolver.PotentialCaptureMove(p, toPlay)
                      )
                   )
                    rootFilter.push_back(p);
            }
        }
    }

    // Losing ladder defense moves
    if (m_param.m_checkLadders)
    {
        for (GoBlockIterator it(m_bd); it; ++it)
        {
            const SgPoint p = *it;
            if (m_bd.GetStone(p) == toPlay && m_bd.InAtari(p))
            {
                if (m_ladder.Ladder(m_bd, p, toPlay, &m_ladderSequence,
                                    false/*twoLibIsEscape*/) < 0)
                {
                    if (m_ladderSequence.Length() >= m_param.m_minLadderLength)
                        rootFilter.push_back(m_bd.TheLiberty(p));
                }
            }

        }
    }

    if (m_param.m_checkOffensiveLadders)
    {
        for (GoBlockIterator it(m_bd); it; ++it)
        {
            const SgPoint p = *it;
            if (m_bd.GetStone(p) == opp
                && m_bd.NumStones(p) >= 5
                && m_bd.NumLiberties(p) == 2
                && m_ladder.Ladder(m_bd, p, toPlay, &m_ladderSequence,
                                    false/*twoLibIsEscape*/) > 0
                && m_ladderSequence.Length() >= m_param.m_minLadderLength
                )
                    rootFilter.push_back(m_ladderSequence[0]);
        }
    }

    if (m_param.m_filterFirstLine)
    {
        // Moves on edge of board, if no stone is near
        const SgBoardConst& bc = m_bd.BoardConst();
        for (SgLineIterator it(bc, 1); it; ++it)
        {
            const SgPoint p = *it;
            if (m_bd.IsEmpty(p) && IsEmptyEdge(m_bd, p))
                rootFilter.push_back(p);
        }
    }

    return rootFilter;
}

//----------------------------------------------------------------------------
