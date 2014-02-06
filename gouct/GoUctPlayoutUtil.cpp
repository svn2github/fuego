
#include "SgSystem.h"
#include "GoUctPlayoutUtil.h"

std::pair<GoPointList,GoPointList>
GoUctPlayoutUtil::FindCorrections(
           GoUctPlayoutPolicy<GoBoard>::Corrector& corrFunction,
           const GoBoard& bd)
{
    // first = from-points, second = to-points GOUCT_REPLACE_CAPTURE
    GoPointList from;
    GoPointList to;

    for (GoBoard::Iterator it(bd); it; ++it)
    {
        if (bd.IsLegal(*it))
        {
            SgPoint move = *it;
            if (corrFunction(bd, move))
            {
                SG_ASSERT(move != *it);
                from.PushBack(*it);
                to.PushBack(move);
            }
        }
    }
    
    return std::make_pair(from, to);
}


