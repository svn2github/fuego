//----------------------------------------------------------------------------
/** @file FeKnowledge.h
    Convert feature weights from FeFeatureWeights into additive knowledge. */
//----------------------------------------------------------------------------


#ifndef FE_KNOWLEDGE_H
#define FE_KNOWLEDGE_H

#include "FeFeatureWeights.h"
#include "GoAdditiveKnowledge.h"

//----------------------------------------------------------------------------

class FeKnowledge
    : public GoAdditiveKnowledge
{
public:
    //static const float VALUE_MULTIPLIER;

    FeKnowledge(const GoBoard& bd, const FeFeatureWeights& weights);

    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);

private:
    FeFeatureWeights m_weights;
};

//----------------------------------------------------------------------------

#endif // FE_KNOWLEDGE_H
