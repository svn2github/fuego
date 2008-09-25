//----------------------------------------------------------------------------
/** @file GoRegionUtil.cpp
    See GoRegionUtil.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoRegionUtil.h"

#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoEyeUtil.h"
#include "SgList.h"
#include "SgPointSet.h"

//----------------------------------------------------------------------------

namespace {

bool Has2IntersectionPoints(const GoBoard& board, const SgPointSet& region, 
                            const SgList<SgPoint>& boundaryAnchors)
{
    if (boundaryAnchors.IsLength(1)) // single block
    {
        int nuIPs = 0;
        for (GoBoard::LibertyIterator it(board, boundaryAnchors.Tail()); it;
             ++it)
        {
            if (region.Contains(*it) && GoEyeUtil::IsSplitPt(*it, region))
            {
                ++nuIPs;
                if (nuIPs >= 2)
                    return true;
            }
        }
        return false;
    }
    else // compute joint liberties of all blocks in region.
    {
        SgList<SgPoint> sharedLibs;
        for (GoBoard::LibertyIterator it(board, boundaryAnchors.Top()); it;
             ++it)
            if (region.Contains(*it))
                sharedLibs.Append(*it);
        if (sharedLibs.MaxLength(1))
            return false;
        bool first = true;
        for (SgListIterator<SgPoint> it(boundaryAnchors); it; ++it)
        {
            if (first) // we already have the liberties of first block.
                first = false;
            else // keep those sharedLibs that are libs of block
            {
                SgList<SgPoint> newShared;
                SgPoint block = *it;
                for (GoBoard::LibertyIterator it(board, block); it; ++it)
                    if (sharedLibs.Contains(*it))
                        newShared.Append(*it);
                if (newShared.MaxLength(1))
                    return false;
                else
                    newShared.SwapWith(&sharedLibs);
            }
        }
        
        // now we have at least two shared libs, check if at least two are
        // split points that divide the area into two eyes.
        int nuIPs = 0;
        for (SgListIterator<SgPoint> it(sharedLibs); it; ++it)
        {
            if (GoEyeUtil::IsSplitPt(*it, region))
            {   ++nuIPs;
                if (nuIPs >= 2)
                    /* */ return true; /* */
            }
        }
        return false;
    }
}

/** Is p adjacent to all blocks represented by anchors?
    GoRegion has an identical function taking a list of GoBlock's.
*/
inline bool IsAdjacentToAll(const GoBoard& board, SgPoint p,
                            const SgList<SgPoint>& anchors)
{
    for (SgListIterator<SgPoint> it(anchors); it; ++it)
        if (! board.IsLibertyOfBlock(p, *it))
            return false;
    return true;
}

// a simple test if the 2-vital search has produced a two-eyed group.
// to be replaced by incremental region code, which will recognize the
// eye regions.
// Right now, testing only for single block 2 eyes, not even Benson.
// 2006/1/9, added simple check for 2 blocks @todo: more general!
bool TwoSeparateEyes(const GoBoard& bd, const SgPointSet& pts,
                     const SgPointSet& boundary, SgBlackWhite color)
{
    SG_ASSERT((pts & bd.AllEmpty()).SubsetOf(boundary.Border(bd.Size())));

    //return IsSingleBlock(bd, boundary, color) 
    //        && pts.Disjoint(bd.All(color)) // no weak own stones inside
    //        && ! pts.IsConnected();
    
    if (pts.Disjoint(bd.All(color)) && ! pts.IsConnected())
    {
        if (GoRegionUtil::IsSingleBlock(bd, boundary, color))
            return true;
        else
        {
            SgPointSet area = pts & bd.AllEmpty();
            if ( area.MinSetSize(2))
            {
                for (SgSetIterator it(area); it; ++it)
                {
                    if (bd.IsLegal(*it, SgOppBW(color)))
                    {   
                        return false;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

} // namespace

//----------------------------------------------------------------------------

void GoRegionUtil::FindCurrentAnchors(const GoBoard& board,
                                      const SgList<SgPoint>& origAnchors,
                                      SgList<SgPoint>* currentAnchors)
{
    SG_ASSERT(currentAnchors->IsEmpty());
    for (SgListIterator<SgPoint> it(origAnchors); it; ++it)
    {
        SgPoint a = board.Anchor(*it);
        currentAnchors->Insert(a);
    }
}

bool GoRegionUtil::Has2IPorEyes(const GoBoard& board, const SgPointSet& pts,
                                SgBlackWhite color,
                                const SgList<SgPoint>& boundaryAnchors)
{
    return     Has2IntersectionPoints(board, pts, boundaryAnchors) 
            || (   boundaryAnchors.Length() == 1 
                && pts.Disjoint(board.All(color)) 
                && ! pts.IsConnected()
                );
}

bool GoRegionUtil::Has2SureLiberties(const GoBoard& board,
                                     const SgPointSet& pts,
                                     SgBlackWhite color,
                                     const SgList<SgPoint>& boundaryAnchors)
{
    const int size = board.Size();
    SgPointSet boundary(pts.Border(size));
    if (! boundary.SubsetOf(board.All(color)))
    {
        return false; // no result of opened area
    }
    else
    {
        boundary -= board.AllEmpty();
        SG_ASSERT(boundary.SubsetOf(board.All(color)));
    }
    // Cond 1: all empty points are in liberties of some boundary block
    // Cond 2: two intersection points
    
    if ( ( (pts & board.AllEmpty()).SubsetOf(boundary.Border(size)))
          &&  (Has2IntersectionPoints(board, pts, boundaryAnchors)
              || TwoSeparateEyes(board, pts, boundary, color))
         )
    {
        /* */ return true; /* */
    }
    
    return false;
}

bool GoRegionUtil::IsSingleBlock(const GoBoard& board, const SgPointSet& pts,
                                 SgBlackWhite color)
{
    SG_DEBUG_ONLY(color);
    // exception for completely empty board. @todo catch this elsewhere???
    SG_ASSERT(pts.NonEmpty()
              || board.TotalNumEmpty() == board.Size() * board.Size());
    
    SgPoint firstAnchor(SG_NULLPOINT);
    for (SgSetIterator it(pts); it; ++it)
    {
        SG_ASSERT(board.IsColor(*it, color));
        SgPoint anchor = board.Anchor(*it);
        if (anchor != firstAnchor)
        {
            if (firstAnchor == SG_NULLPOINT)
                firstAnchor = anchor;
            else
                return false;
        }
    }
    return true;
}

bool GoRegionUtil::IsSmallRegion(const GoBoard& board, const SgPointSet& pts,
                                 SgBlackWhite opp)
{
    const int size = board.Size();
    return pts.Kernel(size).SubsetOf(board.All(opp));
}

bool GoRegionUtil::StaticIs1VitalAndConnected(const GoBoard& board,
                                              const SgPointSet& pts,
                                              SgBlackWhite color)
{
    // type 1:small region with two connection points for all blocks
    SgList<SgPoint> anchors;
    GoBoardUtil::BlocksAdjacentToPoints(board, pts, color, &anchors);
    
    bool is1Vital = false;
    
    if (IsSmallRegion(board, pts, SgOppBW(color)))
    {
        if (anchors.MaxLength(1)) // single block, connected.
            /* */ return true; /* */
        else if (anchors.MinLength(5)) 
        // no way so many blocks can be connected.
            return false;
            
        int nuConn = 0;
        for (SgSetIterator it(pts); it; ++it)
        {
            SgPoint p(*it);
            if (board.IsEmpty(p) && IsAdjacentToAll(board, p, anchors))
            {   // test if boundary stones can be connected by playing p
                if (++nuConn >= 2)
                {
                    is1Vital = true;
                    break;
                }
            }
        }
    }
    return is1Vital;    
}

//----------------------------------------------------------------------------
