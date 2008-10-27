//----------------------------------------------------------------------------
/** @file SgStatisticsTest.cpp
    Unit tests for SgStatistics.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "SgStatistics.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgStatisticsBaseTest_AddWeighted)
{
    SgStatisticsBase<double,double> statistics;
    statistics.AddWeighted(2., 1.);
    BOOST_CHECK_CLOSE(statistics.Mean(), 2., 0.1);
    statistics.AddWeighted(5., 0.5);
    BOOST_CHECK_CLOSE(statistics.Mean(), 3., 0.1);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgStatisticsTest_Basics)
{
    typedef SgStatistics<double,std::size_t> Statistics;

    Statistics statistics;
    statistics.Add(1.0);
    statistics.Add(2.0);
    statistics.Add(3.0);
    BOOST_CHECK_EQUAL(statistics.Count(), 3u);
    BOOST_CHECK_CLOSE(statistics.Mean(), 2., 0.1);
    BOOST_CHECK_CLOSE(statistics.Deviation(), 0.816, 0.1);

    Statistics statistics2;
    statistics2.Add(-1.0);
    statistics2.Add(2.5);
    statistics2.Add(2.5);
    statistics2.Add(2.7);
    BOOST_CHECK_EQUAL(statistics2.Count(), 4u);
    BOOST_CHECK_CLOSE(statistics2.Mean(), 1.675, 0.1);
    BOOST_CHECK_CLOSE(statistics2.Deviation(), 1.547, 0.1);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgHistogramTest_Basics)
{
    typedef SgHistogram<double,std::size_t> Histogram;
    Histogram histo(-2, 6, 4);
    histo.Add(-0.5);
    histo.Add(-1);
    histo.Add(5);
    BOOST_CHECK_EQUAL(histo.Count(), 3u);
    BOOST_CHECK_EQUAL(histo.Count(0), 2u);
    BOOST_CHECK_EQUAL(histo.Count(1), 0u);
    BOOST_CHECK_EQUAL(histo.Count(2), 0u);
    BOOST_CHECK_EQUAL(histo.Count(3), 1u);
}

//----------------------------------------------------------------------------

} // namespace

