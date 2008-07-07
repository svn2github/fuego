//----------------------------------------------------------------------------
/** @file GoNodeUtil.h
    Utility functions for Go trees.
*/
//----------------------------------------------------------------------------

#ifndef GO_NODEUTIL_H
#define GO_NODEUTIL_H

#include "GoKomi.h"
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgList.h"
#include "SgPoint.h"

class GoBoard;
class SgNode;

//----------------------------------------------------------------------------

namespace GoNodeUtil
{
    /** Create a root node containing a given board position. */
    template<class BOARD> SgNode* CreateRoot(const BOARD& board);

    /** Create a position with given size, toPlay, b and w points */
    SgNode* CreatePosition(int boardSize, SgBlackWhite toPlay,
                               const SgList<SgPoint>& bPoints,
                               const SgList<SgPoint>& wPoints );

    /** Find komi that is valid for this node.
        Search parent nodes until a node with a komi property is found.
    */
    GoKomi GetKomi(const SgNode* node);
}

//----------------------------------------------------------------------------
template<class BOARD>
SgNode* GoNodeUtil::CreateRoot(const BOARD& board)
{
    SgBWArray<SgList<SgPoint> > pointList;
    for (typename BOARD::Iterator it(board); it; ++it)
    {
        if (board.Occupied(*it))
            pointList[board.GetColor(*it)].Append(*it);
    }
    return CreatePosition(board.Size(), board.ToPlay(),
                pointList[SG_BLACK], pointList[SG_WHITE]);
}

//----------------------------------------------------------------------------

#endif // GO_NODEUTIL_H

