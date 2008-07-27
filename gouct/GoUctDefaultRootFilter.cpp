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
    SgBWSet safe;
    bool pruneAll = false;
    if (! bd.Rules().CaptureDead())
    {
        // Safety solver is only used to determine if everything is alive.
        GoSafetySolver safetySolver(bd);
        safetySolver.FindSafePoints(&safe);
        pruneAll = (safe.Both() == bd.AllPoints());
    }
    // Benson solver guarantees that capturing moves of dead blocks are
    // liberties of the dead blocks and that no move in safe territory
    // is a Ko threat
    GoBensonSolver bensonSolver(bd);
    safe.Clear();
    bensonSolver.FindSafePoints(&safe);
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.IsLegal(p))
        {
            bool isSafe = safe[toPlay].Contains(p);
            bool isSafeOpp = safe[opp].Contains(p);
            bool hasOppNeighbors = bd.HasNeighbors(p, opp);
            // Always generate capturing moves in own safe territory, even
            // if current rules do no use CaptureDead(), because the UCT
            // player always scores with Tromp-Taylor after two passes in the
            // in-tree phase
            if (pruneAll || isSafeOpp || (isSafe && ! hasOppNeighbors))
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
