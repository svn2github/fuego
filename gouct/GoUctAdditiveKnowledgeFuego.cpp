//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeFuego.cpp
    See GoUctAdditiveKnowledgeFuego.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctAdditiveKnowledgeFuego.h"

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeFuego::GoUctAdditiveKnowledgeFuego(const GoBoard& bd)
    : GoUctAdditiveKnowledge(bd,
                             true /* probabilityBased */, 
    						 0.03f /* scale */,
                             0.0001f /* minimum */
                            )
{
    // Knowledge applies to all moves
    SetMoveRange(0, 10000); 
}

// Assumes that SgMoveInfo has m_raveValue (and m_raveCount) populated by prior knowledge.
// @todo This is a bit of a hack. Should call prior knowledge directly.
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
