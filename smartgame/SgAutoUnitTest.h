//----------------------------------------------------------------------------
/** @file SgAutoUnitTest.h
    Workaround for warning messages within boost */
//----------------------------------------------------------------------------

#ifndef SG_AUTO_UNIT_TEST_H
#define SG_AUTO_UNIT_TEST_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
    #include <boost/test/auto_unit_test.hpp>
#pragma clang diagnostic pop
#endif // SG_AUTO_UNIT_TEST_H
