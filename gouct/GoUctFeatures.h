//----------------------------------------------------------------------------
/** @file GoUctFeatures.h
 Non-pattern as in Remi Coulom's work, and 3x3 pattern features.
 */
//----------------------------------------------------------------------------

#ifndef GOUCT_FEATURES_H
#define GOUCT_FEATURES_H

//----------------------------------------------------------------------------

#include <iosfwd>
#include "FeBasicFeatures.h"
#include "FeFeatureWeights.h"
#include "GoBoard.h"
#include "GoUctPlayoutPolicy.h"

namespace GoUctFeatures {

void FindAllFeatures(const GoBoard& bd,
                     GoUctPlayoutPolicy<GoBoard>& policy,
                     SgPointArray<FeFeatures::FeMoveFeatures>& features,
                     FeFeatures::FeMoveFeatures& passFeatures);

/** Inefficient, calls full board function, use only for UI */
void FindMoveFeaturesUI(const GoBoard& bd,
                        GoUctPlayoutPolicy<GoBoard>& policy,
                        SgPoint move,
                        FeFeatures::FeMoveFeatures& features);

/** Computes features for the position - preceding - the current one
 and writes them using WriteBoardFeaturesWistuba.
 The computation cannot be for the current positon when used as a GTP
 command since the next move is not available to the engine, but it is
 needed since we need the chosen move. */
void WriteFeatures(std::ostream& stream,
                   GoUctPlayoutPolicy<GoBoard>& policy,
                   const GoBoard& bd,
                   const bool writeComment);

} // namespace GoUctFeatures

#endif // GOUCT_FEATURES_H
