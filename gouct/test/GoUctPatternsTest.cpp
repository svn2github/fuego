//----------------------------------------------------------------------------
/** @file GoUctPatternsTest.cpp
 Unit tests for GoUctPatterns. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoUctPatterns.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {
    
    BOOST_AUTO_TEST_CASE(GoUctPatternsTest_3x3_Color_SwapCode)
    {
        using namespace Pattern3x3;
        std::vector<SgEmptyBlackWhite> black;
        std::vector<SgEmptyBlackWhite> white;
        for(int i=0; i<5; ++i)
        {
            black.push_back(SG_BLACK);
            white.push_back(SG_WHITE);
        }
        int codeB = MakeCode(black);
        int codeW = MakeCode(white);
        BOOST_CHECK_EQUAL(codeB, SwapEdgeColor(codeW));
        BOOST_CHECK_EQUAL(codeW, SwapEdgeColor(codeB));
        for(int i=0; i<3; ++i) // 5 + 3 = 8 for center
        {
            black.push_back(SG_BLACK);
            white.push_back(SG_WHITE);
        }
        codeB = MakeCode(black);
        codeW = MakeCode(white);
        BOOST_CHECK_EQUAL(codeB, SwapCenterColor(codeW));
        BOOST_CHECK_EQUAL(codeW, SwapCenterColor(codeB));
    }
    
} // namespace

//----------------------------------------------------------------------------
