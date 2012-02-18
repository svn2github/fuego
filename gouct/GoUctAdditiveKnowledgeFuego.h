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

class GoUctAdditiveKnowledgeParamFuego
    : public GoUctAdditiveKnowledgeParam
{
public:
    GoUctAdditiveKnowledgeParamFuego();
};


class GoUctAdditiveKnowledgeFuego
    : public GoUctAdditiveKnowledge
{
public:

    static const float VALUE_MULTIPLIER = 4.0f;

    GoUctAdditiveKnowledgeFuego(const GoBoard& bd,
				const GoUctAdditiveKnowledgeParamFuego& param);

    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);
    
private:
    const GoUctAdditiveKnowledgeParamFuego& m_param;
};

//----------------------------------------------------------------------------

#endif // GOUCT_ADDITIVEKNOWLEDGEFUEGO_H
