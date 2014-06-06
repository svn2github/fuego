//----------------------------------------------------------------------------
/** @file FePatternBaseTest.cpp
 Unit tests for FePatternBase. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "FePatternBase.h"

#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {
    
#if 0
void CheckOffset(int deltaX, int deltaY)
{
    int offset = PaConst::Offset(deltaX, deltaY);
    int newX = PaConst::DeltaX(offset);
    int newY = PaConst::DeltaY(offset);
    BOOST_CHECK_EQUAL(deltaX, newX);
    BOOST_CHECK_EQUAL(deltaY, newY);
}
#endif

BOOST_AUTO_TEST_CASE(FePatternBaseTest_PaAxSet_To_PaAx)
{
    for (PaAxSet s = 0; s < PA_NU_AX_SETS; ++s)
    {
        PaAx ax(s);
        BOOST_CHECK_EQUAL(ax.test(PA_MIRROR_X), static_cast<bool>(s&1));
        BOOST_CHECK_EQUAL(ax.test(PA_MIRROR_Y), static_cast<bool>(s&2));
        BOOST_CHECK_EQUAL(ax.test(PA_SWAP_X_Y), static_cast<bool>(s&4));
    }
}

#if 0
BOOST_AUTO_TEST_CASE(FePatternBaseTest_Offset)
{
    CheckOffset(0,0);
    CheckOffset(0,1);
    CheckOffset(0,2);
    CheckOffset(1,0);
    CheckOffset(0,-1);
    CheckOffset(-1,0);
    CheckOffset(-1,-1);
    CheckOffset(-8,-3);
}
#endif

BOOST_AUTO_TEST_CASE(FePatternBaseTest_PaSpot)
{
    const int BOARDSIZE = 19;
    const int B_1 = BOARDSIZE + 1;
    const int x = 2;
    const int y = 3;
    PaAx ax; // no rotation
    PaSpot s(x,y,ax,false);
    SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
    BOOST_CHECK_EQUAL(Pt(x,y), p);
    
    ax.set(PA_MIRROR_X);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(B_1 - x, y), p);
    }
    ax.set(PA_MIRROR_Y);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(B_1 - x, B_1 - y), p);
    }
    
    ax.reset(PA_MIRROR_X);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(x, B_1 - y), p);
    }
    ax.reset(PA_MIRROR_Y);
    ax.set(PA_SWAP_X_Y);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(y, x), p);
    }
    ax.set(PA_MIRROR_X);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(B_1 - y, x), p);
    }
    ax.set(PA_MIRROR_Y);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(B_1 - y, B_1 - x), p);
    }
    
    ax.reset(PA_MIRROR_X);
    {
        PaSpot s(x,y,ax,false);
        SgPoint p = AXBoardToBoard(s.m_x, s.m_y, s.m_ax, BOARDSIZE);
        BOOST_CHECK_EQUAL(Pt(y, B_1 - x), p);
    }
}

} // namespace

//----------------------------------------------------------------------------
