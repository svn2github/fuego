//----------------------------------------------------------------------------
/** @file GoUctPlayoutUtil.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_PLAYOUT_UTIL_H
#define GOUCT_PLAYOUT_UTIL_H

#include "GoBoard.h"
#include "GoUctPlayoutPolicy.h"

namespace GoUctPlayoutUtil {

std::pair<GoPointList,GoPointList> FindCorrections(
    GoUctPlayoutPolicy<GoBoard>::Corrector& corrFunction,
    const GoBoard& bd);

} // namespace GoUctPlayoutUtil

#endif // GOUCT_PLAYOUT_UTIL_H