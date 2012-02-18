//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeFuego.cpp
    See GoUctAdditiveKnowledgeFuego.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctAdditiveKnowledgeFuego.h"

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeParamFuego::GoUctAdditiveKnowledgeParamFuego()
{ }

GoUctAdditiveKnowledgeFuego::GoUctAdditiveKnowledgeFuego(const GoBoard& bd,
                             const GoUctAdditiveKnowledgeParamFuego& param)
    : GoUctAdditiveKnowledge(bd),
      m_param(param)
{
    // Knowledge applies to all moves
    SetMoveRange(0, 10000); 
}

// Assumes that SgMoveInfo has m_raveValue (and m_raveCount) populated.
void 
GoUctAdditiveKnowledgeFuego::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    float sum = 0.0;
    for (size_t i = 0; i < moves.size(); ++i) 
    {
        moves[i].m_predictorValue 
            = exp(VALUE_MULTIPLIER * moves[i].m_raveValue);
        sum += moves[i].m_predictorValue;
    }
    for (size_t i = 0; i < moves.size(); ++i) 
        moves[i].m_predictorValue /= sum;
}

//----------------------------------------------------------------------------
