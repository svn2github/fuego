//----------------------------------------------------------------------------
/** @file SgAutoUnitTest.h
    Workaround for warning messages within boost */
//----------------------------------------------------------------------------

#ifndef SG_AUTO_UNIT_TEST_H
#define SG_AUTO_UNIT_TEST_H

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#include <boost/test/auto_unit_test.hpp>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // SG_AUTO_UNIT_TEST_H
