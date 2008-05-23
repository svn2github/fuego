//----------------------------------------------------------------------------
/** @file SgStrategyTest.cpp
    Unit tests for SgStrategy.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgMiaiMap.h"
#include "SgMiaiStrategy.h"
#include "SgStrategy.h"

#include "SgBlackWhite.h"
#include "SgPoint.h"
#include "SgPointSet.h"
#include "SgPointSetUtil.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgStrategyTest_SgMiaiStrategy)
{
    /* Set up simple miai strategy with two points p1, p2 */
    SgPoint p1(Pt(3,3));
    SgPoint p2(Pt(5,3));
    SgMiaiPair p(p1, p2);
    SgMiaiStrategy s(SG_BLACK);
    s.AddPair(p);
    
    SgPointSet set1 = s.Dependency();
    SgPointSet set2;
    set2.Include(p1);
    set2.Include(p2);
    BOOST_CHECK_EQUAL(set1, set2);
    BOOST_CHECK_EQUAL(s.Status(), strAchieved);
    BOOST_CHECK(s.OpenThreats().IsEmpty());
    
    s.ExecuteMove(p1, SG_WHITE);
    BOOST_CHECK_EQUAL(s.Status(), strThreatened);
    BOOST_CHECK(s.OpenThreats().IsLength1());
    BOOST_CHECK_EQUAL(s.OpenThreatMove(), p2);

    SgMiaiStrategy s2(s);
    
    s.ExecuteMove(p2, SG_BLACK);
    BOOST_CHECK_EQUAL(s.Status(), strAchieved);
    set1 = s.Dependency();
    SgPointSet emptySet;
    BOOST_CHECK_EQUAL(set1, emptySet);
    
    s2.ExecuteMove(p2, SG_WHITE);
    BOOST_CHECK_EQUAL(s2.Status(), strFailed);
    set1 = s2.Dependency();
    BOOST_CHECK_EQUAL(set1, emptySet);
}

BOOST_AUTO_TEST_CASE(SgStrategyTest_SgMiaiMap)
{
    /* Set up simple miai strategy with two points p1, p2 */
    SgPoint p1(Pt(3,3));
    SgPoint p2(Pt(5,3));
    SgMiaiPair p(p1, p2);
    SgMiaiStrategy s(SG_BLACK);
    s.AddPair(p);

    SgMiaiMap m;
    m.ConvertFromSgMiaiStrategy(s);
    BOOST_CHECK_EQUAL(m.Status(), strAchieved);
    m.ExecuteMove(p1, SG_BLACK);
    BOOST_CHECK_EQUAL(m.Status(), strThreatened);
    BOOST_CHECK_EQUAL(m.ForcedMove(), p2);
    
    // todo: convert strategy with open threat.
}

//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

