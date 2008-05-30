//----------------------------------------------------------------------------
/** @file GoNodeUtil.h
    Utility functions for Go trees.
*/
//----------------------------------------------------------------------------

#ifndef GO_NODEUTIL_H
#define GO_NODEUTIL_H

#include "GoKomi.h"

class GoBoard;
class SgNode;

//----------------------------------------------------------------------------

namespace GoNodeUtil
{
    /** Create a root node containing a given board position. */
    SgNode* CreateRoot(const GoBoard& board);

    /** Find komi that is valid for this node.
        Search parent nodes until a node with a komi property is found.
    */
    GoKomi GetKomi(const SgNode* node);
}

//----------------------------------------------------------------------------

#endif // GO_NODEUTIL_H

