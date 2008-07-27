//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.cpp
    See GoUctDefaultRootFilter.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultRootFilter.h"

#include "GoBensonSolver.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "GoSafetySolver.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctDefaultRootFilter::GoUctDefaultRootFilter(const GoBoard& bd)
    : m_bd(bd),
      m_checkLadders(true),
      m_minLadderLength(6)
{
}

vector<SgPoint> GoUctDefaultRootFilter::Get()
{
    vector<SgPoint> rootFilter;
    SgBlackWhite toPlay = m_bd.ToPlay();
    SgBlackWhite opp = SgOppBW(toPlay);

    // Safe territory

    GoModBoard modBoard(m_bd);
    GoBoard& bd = modBoard.Board();
    SgBWSet alternateSafe;
    bool isAllAlternateSafe = false;
    if (! bd.Rules().CaptureDead())
    {
        // Alternate safety is only used to prune moves in opponent territory
        // if everything is alive under alternate play and rules do not
        // require to capture dead
        GoSafetySolver safetySolver(bd);
        safetySolver.FindSafePoints(&alternateSafe);
        isAllAlternateSafe = (alternateSafe.Both() == bd.AllPoints());
    }
    // Benson solver guarantees that capturing moves of dead blocks are
    // liberties of the dead blocks and that no move in safe territory
    // is a Ko threat
    GoBensonSolver bensonSolver(bd);
    SgBWSet unconditionalSafe;
    bensonSolver.FindSafePoints(&unconditionalSafe);
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.IsLegal(p))
        {
            bool isUnconditionalSafe = unconditionalSafe[toPlay].Contains(p);
            bool isUnconditionalSafeOpp = unconditionalSafe[opp].Contains(p);
            bool isAlternateSafeOpp = alternateSafe[opp].Contains(p);
            bool hasOppNeighbors = bd.HasNeighbors(p, opp);
            // Always generate capturing moves in own safe territory, even
            // if current rules do no use CaptureDead(), because the UCT
            // player always scores with Tromp-Taylor after two passes in the
            // in-tree phase
            if ((isAllAlternateSafe && isAlternateSafeOpp)
                || isUnconditionalSafeOpp
                || (isUnconditionalSafe && ! hasOppNeighbors))
                rootFilter.push_back(p);
        }
    }

    // Loosing ladder defense moves
    if (m_checkLadders)
        for (GoBlockIterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (m_bd.GetStone(p) == toPlay && m_bd.InAtari(p))
            {
                if (m_ladder.Ladder(m_bd, p, toPlay, &m_ladderSequence,
                                    false/*twoLibIsEscape*/) < 0)
                {
                    if (m_ladderSequence.Length() >= m_minLadderLength)
                        rootFilter.push_back(m_bd.TheLiberty(p));
                }
            }

        }

    return rootFilter;
}

//----------------------------------------------------------------------------
