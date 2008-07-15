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
    : m_bd(bd)
{
}

vector<SgPoint> GoUctDefaultRootFilter::Get()
{
    vector<SgPoint> rootFilter;
    GoModBoard modBoard(m_bd);
    GoBoard& bd = modBoard.Board();
    GoSafetySolver safetySolver(bd);
    SgBWSet safe;
    safetySolver.FindSafePoints(&safe);
    SgBlackWhite toPlay = bd.ToPlay();
    SgBlackWhite opp = SgOppBW(toPlay);
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
    return rootFilter;
}

//----------------------------------------------------------------------------
