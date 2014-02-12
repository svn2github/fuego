//----------------------------------------------------------------------------
/** @file FeKnowledge.cpp
 See FeKnowledge.h
 */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeKnowledge.h"
//----------------------------------------------------------------------------


FeKnowledge::FeKnowledge(const GoBoard& bd,
                         const FeFeatureWeights& weights)
    : GoUctAdditiveKnowledge(bd),
      m_weights(weights)
{ }

void FeKnowledge::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    SG_UNUSED(moves);
    // TODO
}
//----------------------------------------------------------------------------
