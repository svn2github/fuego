//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeFuego.cpp
    See GoUctAdditiveKnowledgeFuego.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctAdditiveKnowledgeFuego.h"

#include <cmath>
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** @todo This is a tunable constant. */
const float GoUctAdditiveKnowledgeFuego::VALUE_MULTIPLIER = 4.0f;

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeFuego::GoUctAdditiveKnowledgeFuego(const GoBoard& bd)
    : GoUctAdditiveKnowledgeStdProb(bd)
{ }

// m_raveValue is a SgUctValue which is double by default, but holds
// a predictor value which only has float precision
inline float RaveValueAsFloat(const SgUctMoveInfo& info)
{
    return static_cast<float>(info.m_raveValue);
}

// @todo Assumes that SgMoveInfo has m_raveValue (and m_raveCount) populated
// by prior knowledge.
// This is a hack. Should call prior knowledge directly.
void 
GoUctAdditiveKnowledgeFuego::ProcessPosition(std::vector<SgUctMoveInfo>&
                                             moves)
{
    float sum = 0.0f;
    float values[SG_MAX_MOVES];
    SG_ASSERT(moves.size() <= static_cast<unsigned int>(SG_MAX_MOVES));
    for (size_t i = 0; i < moves.size(); ++i) 
    {
        values[i] = expf(VALUE_MULTIPLIER * RaveValueAsFloat(moves[i]));
        sum += values[i];
    }
    if (sum > 0.0)
        for (size_t i = 0; i < moves.size(); ++i)
            moves[i].m_predictorValue = values[i] / sum;
}

//----------------------------------------------------------------------------
