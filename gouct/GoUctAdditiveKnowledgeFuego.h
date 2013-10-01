//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeFuego.h
	A simple implementation of additive knowledge using RAVE values.
    @todo some details.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_ADDITIVEKNOWLEDGEFUEGO_H
#define GOUCT_ADDITIVEKNOWLEDGEFUEGO_H

#include "GoUctAdditiveKnowledge.h"
#include "GoUctPlayoutPolicy.h"

//----------------------------------------------------------------------------

class GoUctAdditiveKnowledgeFuego
    : public GoUctAdditiveKnowledge
{
public:

    static const float VALUE_MULTIPLIER;

    GoUctAdditiveKnowledgeFuego(const GoBoard& bd);

    /** The minimum value allowed by this predictor */
    SgUctValue Minimum() const;

    bool ProbabilityBased() const;

    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);

    /** The scaling factor for this predictor */
    SgUctValue Scale() const;
};

//----------------------------------------------------------------------------

inline SgUctValue GoUctAdditiveKnowledgeFuego::Minimum() const
{
	return 0.0001f;
}

inline bool GoUctAdditiveKnowledgeFuego::ProbabilityBased() const
{
	return true;
}

inline SgUctValue GoUctAdditiveKnowledgeFuego::Scale() const
{
	return 0.03f;
}

//----------------------------------------------------------------------------

#endif // GOUCT_ADDITIVEKNOWLEDGEFUEGO_H
