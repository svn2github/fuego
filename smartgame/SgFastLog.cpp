//----------------------------------------------------------------------------
/** @file SgFastLog.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgFastLog.h"

#include <limits>
#include <cmath>
#include <boost/static_assert.hpp>

//----------------------------------------------------------------------------

namespace {

// The fast log algorithm requires 4 byte integers
BOOST_STATIC_ASSERT(sizeof(int) == 4);

// The fast log algorithm requires that floats use IEEE 754 format
BOOST_STATIC_ASSERT(std::numeric_limits<float>::is_iec559);

} // namespace

//----------------------------------------------------------------------------

SgFastLog::SgFastLog(int mantissaBits)
    : m_mantissaBitsDiff(MAX_MANTISSA_BITS - mantissaBits)
{
    // Store the result of the shift operation in a variable to prevent a
    // compiler warning in 64-bit VC++ builds (C4334)
    size_t sizeOfLookupTable = 1 << mantissaBits;
    m_lookupTable = new float[sizeOfLookupTable];
    IntFloat x;
    x.m_int = 0x3F800000;
    int incr = (1 << m_mantissaBitsDiff);
    int p = static_cast<int>(pow(2.0f, mantissaBits));
    float invLogTwo = 1.f / log(2.f);
    for (int i = 0; i < p; ++i)
    {
        m_lookupTable[i] = log(x.m_float) * invLogTwo;
        x.m_int += incr;
    }
}

SgFastLog::~SgFastLog()
{
    delete[] m_lookupTable;
}

//----------------------------------------------------------------------------
