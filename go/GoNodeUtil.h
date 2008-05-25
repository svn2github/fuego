//----------------------------------------------------------------------------
/** @file GoNodeUtil.h
    Utility functions for Go trees.
*/
//----------------------------------------------------------------------------

#ifndef GONODEUTIL_H
#define GONODEUTIL_H

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

#endif // GONODEUTIL_H

