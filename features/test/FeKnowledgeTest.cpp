//----------------------------------------------------------------------------
/** @file FeKnowledgeTest.cpp
    Unit tests for FeKnowledge. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "FeKnowledge.h"

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
    //FeKnowledge k(bd, weights);
}

} // namespace

//----------------------------------------------------------------------------
