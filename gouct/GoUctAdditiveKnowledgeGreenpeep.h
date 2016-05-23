//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeGreenpeep.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_ADDITIVEKNOWLEDGEGREENPEEP_H
#define GOUCT_ADDITIVEKNOWLEDGEGREENPEEP_H

#include "GoAdditiveKnowledge.h"


/* max 26-bit: 16-bit 8-neighbor core, 8-bit liberty & 2-away extension, 
	1 bit "ko exists", 1 bit defensive move */
const int NUMPATTERNS9X9 = 1<<26;

/* 24-bit: 16-bit 8-neighbor core, 8-bit liberty & 2-away extension */
const int NUMPATTERNS19X19 = 1<<24;

//----------------------------------------------------------------------------

class GoUctAdditiveKnowledgeParamGreenpeep: public GoUctAdditiveKnowledgeParam
{
private:
public:
    GoUctAdditiveKnowledgeParamGreenpeep();

    unsigned short m_predictor9x9[NUMPATTERNS9X9];
    
    unsigned short m_predictor19x19[NUMPATTERNS19X19];
};

/** Use Greenpeep-style pattern values to make predictions. */
class GoUctAdditiveKnowledgeGreenpeep : public GoAdditiveKnowledge
{
public:
    GoUctAdditiveKnowledgeGreenpeep(const GoBoard& bd,
				 const GoUctAdditiveKnowledgeParamGreenpeep& param);

    /** The minimum value allowed by this predictor */
    float MinValue() const;

    GoPredictorType PredictorType() const;

    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);

private:

    void ProcessPosition9(std::vector<SgUctMoveInfo>& moves);

    void ProcessPosition19(std::vector<SgUctMoveInfo>& moves);

    const GoUctAdditiveKnowledgeParamGreenpeep& m_param;

    unsigned int m_contexts[SG_MAX_ONBOARD + 1];
};

//----------------------------------------------------------------------------

inline float GoUctAdditiveKnowledgeGreenpeep::MinValue() const
{
	return PredictorType() == GO_PRED_TYPE_PROBABILITY_BASED ? 0.0001f
                                                             : 0.05f;
}

inline GoPredictorType GoUctAdditiveKnowledgeGreenpeep::PredictorType() const
{
	return Board().Size() >= 15 ? GO_PRED_TYPE_PROBABILITY_BASED
                                : GO_PRED_TYPE_PUCB;
}

inline void GoUctAdditiveKnowledgeGreenpeep::
ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    if (Board().Size() < 15)
        ProcessPosition9(moves);
    else
        ProcessPosition19(moves);
}

//----------------------------------------------------------------------------

#endif // GOUCT_ADDITIVEKNOWLEDGEGREENPEEP_H
