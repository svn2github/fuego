//----------------------------------------------------------------------------
/** @file GoNodeUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoNodeUtil.h"

#include "GoBoard.h"
#include "SgNode.h"

using namespace std;

//----------------------------------------------------------------------------

SgNode* GoNodeUtil::CreateRoot(const GoBoard& board)
{
    SgNode* node = new SgNode();
    node->Add(new SgPropInt(SG_PROP_SIZE, board.Size()));
    for (SgBWIterator it; it; ++it)
    {
        SgList<SgPoint> pointList;
        board.All(*it).ToList(&pointList);
        SgPropID id =
            *it == SG_BLACK ? SG_PROP_ADD_BLACK : SG_PROP_ADD_WHITE;
        node->Add(new SgPropAddStone(id, pointList));
    }
    node->Add(new SgPropPlayer(SG_PROP_PLAYER, board.ToPlay()));
    return node;
}

GoKomi GoNodeUtil::GetKomi(const SgNode* node)
{
    while (node != 0)
    {
        if (node->HasProp(SG_PROP_KOMI))
        {
            try
            {
                return GoKomi(node->GetRealProp(SG_PROP_KOMI));
            }
            catch (const GoKomi::InvalidKomi& e)
            {
                break;
            }
        }
        node = node->Father();
    }
    return GoKomi();
}

//----------------------------------------------------------------------------

