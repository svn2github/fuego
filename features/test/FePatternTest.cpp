//----------------------------------------------------------------------------
/** @file FePatternTest.cpp
 Unit tests for FePattern. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "FePattern.h"

#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(FePatternTest_Rect_3x3_1)
{
    FeRectPattern p(3,3);
    p.Init("..."\
           ".xo"\
           ".o.");
    p.SetMove(Pt(2,2), SG_BLACK);
    p.SetValue(23);
    //SgDebug() << p;

    std::string s("...\n"
                  ".xo\n"
                  ".o.");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);

    FeRectPatternMatcher m;
    m.Add(&p);
    FePatternMatchResult* r = m.Match(bd);
    BOOST_CHECK_EQUAL(r->Length(), 1u);
    const FePatternMatch& pm = r->Match(0);
    BOOST_CHECK_EQUAL(pm.Move(), Pt(3,3));
    BOOST_CHECK_EQUAL(pm.MoveColor(), SG_BLACK);
    BOOST_CHECK_EQUAL(pm.Value(), 23);
    delete r;
}
    
} // namespace

//----------------------------------------------------------------------------
