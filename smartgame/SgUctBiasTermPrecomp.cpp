//----------------------------------------------------------------------------
/** @file SgUctBiasTermPrecomp.cpp
    See SgUctBiasTermPrecomp.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctBiasTermPrecomp.h"

using namespace std;

//----------------------------------------------------------------------------

SgUctBiasTermPrecomp::SgUctBiasTermPrecomp(std::size_t maxPosCount,
                                           std::size_t maxMoveCount)
    : m_maxPosCount(maxPosCount),
      m_maxMoveCount(maxMoveCount),
      m_values(maxPosCount * maxMoveCount)
#if SG_UCTFASTLOG
    , m_fastLog(10)
#endif
{
    for (size_t posCount = 0; posCount < maxPosCount; ++posCount)
        for (size_t moveCount = 0; moveCount < maxMoveCount; ++moveCount)
            Value(posCount, moveCount) =
                Compute(log(static_cast<float>(posCount)), moveCount);
}

//----------------------------------------------------------------------------
