//----------------------------------------------------------------------------
/** @file GoInfluence.cpp
    See GoInfluence.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoInfluence.h"

#include <vector>
#include "SgNbIterator.h"
#include "SgPointset.h"

//----------------------------------------------------------------------------
namespace {
//----------------------------------------------------------------------------

/** Spread influence, drop off by 0.5 to the neighbors */
void Spread(const GoBoard& bd, SgPoint p, const SgPointSet& stopPts,
            int val, SgPointArray<int>& influence)
{
    influence[p] += val;
    val /= 2;
    if (val > 0)
        for (GoNbIterator it(bd, p); it; ++it)
        {
            SG_ASSERT(bd.IsValidPoint(*it));
            if (! stopPts.Contains(*it))
                Spread(bd, *it, stopPts, val, influence);
        }
}

//----------------------------------------------------------------------------
} // namespace
//----------------------------------------------------------------------------

void GoInfluence::ComputeInfluence(const GoBoard& bd,
                                   const SgBWSet& stopPts,
                                   SgBWArray<SgPointArray<int> >* influence)
{
    const int MAX_INFLUENCE = 64;
    for (SgBWIterator cit; cit; ++cit)
    {
        SgBlackWhite color = *cit;
        ((*influence)[color]).Fill(0);
        for (GoBoard::Iterator it(bd); it; ++it)
        {
            SgPoint p(*it);
            if (bd.IsColor(p, color))
                Spread(bd, p, stopPts[color], MAX_INFLUENCE, 
                       (*influence)[color]);
        }
    }
}

void GoInfluence::FindDistanceToStones(const GoBoard& bd,
                                       SgBlackWhite color,
                                       SgPointArray<int>& distance)
{
    for (GoBoard::Iterator it(bd); it; ++it)
        distance[*it] = DISTANCE_INFINITE;
    
    std::vector<SgPoint> queue;
    SgMarker marker;
    for (SgSetIterator it(bd.All(color)); it; ++it)
    {
        queue.push_back(*it);
        marker.Include(*it);
    }
    
    int d = 0;
    while (! queue.empty())
    {
        std::vector<SgPoint> next;
        for(std::vector<int>::const_iterator it = queue.begin(); it != queue.end(); ++it)
        {
            const SgPoint p = *it;
            distance[p] = d;
            for (GoNbIterator it2(bd, p); it2; ++it2)
            {
                const SgPoint nb = *it2;
                if (marker.NewMark(nb) && bd.IsEmpty(nb))
                    next.push_back(nb);
            }
        }
        ++d;
        queue.swap(next);
    }
}

void GoInfluence::FindInfluence(const GoBoard& board,
                                int nuExpand,
                                int nuShrink,
                                SgBWSet* influence)
{
    SgBWSet result = SgBWSet(board.All(SG_BLACK), board.All(SG_WHITE));
    SgBWSet next;
    const int size = board.Size();
    for (int i = 1; i <= nuExpand; ++i)
    {
        for (SgBlackWhite c = SG_BLACK; c <= SG_WHITE; ++c)
        {
            SgBlackWhite opp = SgOppBW(c);
            next[c] = result[c].Border(size) - result[opp];
        }
        result[SG_BLACK] |= (next[SG_BLACK] - next[SG_WHITE]);
        result[SG_WHITE] |= (next[SG_WHITE] - next[SG_BLACK]);
    }

    for (int i = 1; i <= nuShrink; ++i)
    {
        result[SG_BLACK] = result[SG_BLACK].Kernel(size);
        result[SG_WHITE] = result[SG_WHITE].Kernel(size);
    }
    
    *influence = result;
}

int GoInfluence::Influence(const GoBoard& board,
                           SgBlackWhite color,
                           int nuExpand,
                           int nuShrink)
{
    SgBWSet result;
    FindInfluence(board, nuExpand, nuShrink, &result);
    return result[color].Size();
}
