//----------------------------------------------------------------------------
/** @file FeKnowledgeTest.cpp
    Unit tests for GoUctFeatureKnowledge. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoUctFeatureKnowledge.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(FeKnowledgeTest_FeKnowledge)
{
    GoBoard bd(19);
    // TODO const FeWeights& weights = FeWeights::TheWeights();
    //GoUctFeatureKnowledge k(bd, weights);
}

} // namespace

//----------------------------------------------------------------------------
