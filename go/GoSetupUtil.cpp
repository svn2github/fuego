//----------------------------------------------------------------------------
/** @file GoSetupUtil.cpp
    See GoSetupUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoard.h"
#include "GoSetup.h"
#include "GoSetupUtil.h"

//----------------------------------------------------------------------------

GoSetup GoSetupUtil::CurrentPosSetup(const GoBoard& bd)
{
    GoSetup setup;
    setup.m_player = bd.ToPlay();
    for (GoBoard::Iterator it2(bd); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.Occupied(p))
            setup.m_stones[bd.GetColor(p)].Include(p);
    }
    return setup;
}

//----------------------------------------------------------------------------
