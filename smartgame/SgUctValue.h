//----------------------------------------------------------------------------
/** @file SgUctValue.h
    Defines the floating point type used in SgUctSearch
*/
//----------------------------------------------------------------------------

#ifndef SG_UCTVALUE_H
#define SG_UCTVALUE_H

#include <limits>
#include <boost/static_assert.hpp>
#include "SgStatistics.h"

//----------------------------------------------------------------------------

/** @typedef SgUctValue
    The floating type used for mean values and counts in SgUctSearch.
    The default type is @c double, but it is possible to use @c float to reduce
    the node size and to get some performance gains (especially on 32-bit
    systems). However, using @c float sets a practical limit on the number of
    simulations before the count and mean values go into saturation. This
    maximum is given by 2^d-1 with d being the digits in the mantissa (=23 for
    IEEE 754 float's). The search will terminate when this number is
    reached.
*/

#ifdef SG_UCT_VALUE_TYPE
typedef SG_UCT_VALUE_TYPE SgUctValue;
#else
typedef double SgUctValue;
#endif

BOOST_STATIC_ASSERT(! std::numeric_limits<SgUctValue>::is_exact);

typedef SgStatisticsBase<SgUctValue,SgUctValue> SgUctStatistics;

typedef SgStatisticsBase<volatile SgUctValue,volatile SgUctValue>
  SgUctStatisticsVolatile;

//----------------------------------------------------------------------------

#endif // SG_UCTVALUE_H
