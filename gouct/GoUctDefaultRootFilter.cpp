//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.cpp
    See GoUctDefaultRootFilter.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultRootFilter.h"

#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "GoSafetySolver.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctDefaultRootFilter::GoUctDefaultRootFilter(const GoBoard& bd)
    : m_bd(bd),
      m_checkLadders(false),
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
    GoSafetySolver safetySolver(bd);
    SgBWSet safe;
    safetySolver.FindSafePoints(&safe);
    bool captureDead = bd.Rules().CaptureDead();
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.IsLegal(p))
        {
            bool isSafe = safe[toPlay].Contains(p);
            bool isSafeOpp = safe[opp].Contains(p);
            bool hasOppNeighbors = bd.HasNeighbors(p, opp);
            // Filter moves in safe territory, but generate moves on liberties
            // of opponent dead stones, if captureDead
            // TODO: are other capturing moves possible?
            if ((isSafe && (! captureDead || ! hasOppNeighbors)) || isSafeOpp)
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
                if (m_ladder.Ladder(m_bd, p, toPlay, &m_ladderSequence, true))
                {
                    if (m_ladderSequence.Length() >= m_minLadderLength)
                        rootFilter.push_back(m_bd.TheLiberty(p));
                }
            }

        }

    return rootFilter;
}

//----------------------------------------------------------------------------
