//----------------------------------------------------------------------------
/** @file FeNestedPatternTest.cpp
 Unit tests for FeNestedPattern. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "FeNestedPattern.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

    BOOST_AUTO_TEST_CASE(FeNestedPatternTest_FeNestedPattern)
    {
        FeNestedPattern p(2, 13);
        BOOST_CHECK_EQUAL(p.MinSize(), 2);
        BOOST_CHECK_EQUAL(p.MaxSize(), 13);
    }

} // namespace

//----------------------------------------------------------------------------
