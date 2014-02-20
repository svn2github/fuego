//----------------------------------------------------------------------------
/** @file GoUctFeatureKnowledge.cpp
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

BOOST_AUTO_TEST_CASE(GoUctFeatureKnowledgeTest_GoUctFeatureKnowledge)
{
    GoBoard bd(19);
    FeFeatureWeights weights(0,0);
    GoUctFeatureKnowledge k(bd, weights);

    // TODO test something...
}

} // namespace

//----------------------------------------------------------------------------
