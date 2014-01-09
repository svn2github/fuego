//----------------------------------------------------------------------------
/** @file FePatternBase.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FePatternBase.h"

#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------
void PaConst::Init()
{
/*
    for (int i = 0; i <=  SG_MAX_SIZE; ++i)
        for (int j = 0; j <=  SG_MAX_SIZE; ++j)
        {
            SgPoint p = j + SG_NS * i;
            s_yCoordinate[p] = i;
            s_xCoordinate[p] = j;
        }
*/
}

SgPoint AXBoardToBoard(int x, int y, PaAx ax, int boardSize)
{
    // **** should be made faster using a constant array ****
    if ((x >= 1) && (x <= boardSize) && (y >= 1) && (y <= boardSize))
    {
        if (ax.test(PA_SWAP_X_Y))
        {
            int temp = y;
            y = x;
            x = temp;
        }
        if (ax.test(PA_MIRROR_X))
        {
            x = boardSize + 1 - x;
        }
        if (ax.test(PA_MIRROR_Y))
        {
            y = boardSize + 1 - y;
        }
        return SG_NS * y + x;
    }
    else return 0;
}

SgPoint PatternToPoint(int offset, const PaSpot& spot, int boardSize)
{
    return AXBoardToBoard(spot.m_x + PaConst::DeltaX(offset),
                          spot.m_y + PaConst::DeltaY(offset),
                          spot.m_ax, boardSize);
}
