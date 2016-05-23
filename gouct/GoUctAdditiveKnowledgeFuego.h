//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeFuego.h
	A simple implementation of additive knowledge, using Fuego's
    rule-based prior knowledge.
    @todo hack: uses the fact that these prior values are already stored in
    RAVE values of each SgUctMoveInfo.
*/
//----------------------------------------------------------------------------


#ifndef GOUCT_ADDITIVEKNOWLEDGEFUEGO_H
#define GOUCT_ADDITIVEKNOWLEDGEFUEGO_H

#include "GoAdditiveKnowledge.h"

//----------------------------------------------------------------------------

class GoUctAdditiveKnowledgeFuego
    : public GoUctAdditiveKnowledgeStdProb
{
public:
    /** TODO tunable constant */
    static const float VALUE_MULTIPLIER;

    GoUctAdditiveKnowledgeFuego(const GoBoard& bd);

    /** Apply as additive predictor */
    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
};

//----------------------------------------------------------------------------

#endif // GOUCT_ADDITIVEKNOWLEDGEFUEGO_H
