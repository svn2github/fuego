//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeMultiple.cpp
    See GoUctAdditiveKnowledgeMultiple.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctAdditiveKnowledgeMultiple.h"

#include "SgVector.h"

//----------------------------------------------------------------------------
namespace {
    void ComputeArithmeticMean(std::vector<SgUctMoveInfo>& moves,
                               int nuPredictors)
    {
        const float exponent = 1.0/nuPredictors;
        for (size_t j = 0; j < moves.size(); ++j)
            moves[j].m_predictorValue =
            std::pow(moves[j].m_predictorValue, exponent);
    }
    
    void ComputeAverage(std::vector<SgUctMoveInfo>& moves,
                        int nuPredictors)
    {
        for (size_t j = 0; j < moves.size(); ++j)
            moves[j].m_predictorValue /= nuPredictors;
    }
} // namespace
//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeMultiple::GoUctAdditiveKnowledgeMultiple(
        const GoBoard& bd,
        SgUctValue scale,
        SgUctValue minimum,
        GoUctKnowledgeCombinationType combinationType)
    :
    GoUctAdditiveKnowledge(bd),
    m_minimum(minimum),
    m_scale(scale),
    m_combinationType(combinationType)
{ }

GoUctAdditiveKnowledgeMultiple::~GoUctAdditiveKnowledgeMultiple()
{
    for (SgVectorIterator<GoUctAdditiveKnowledge*> it(m_additiveKnowledge); 
         it; ++it)
        delete *it;
}

void GoUctAdditiveKnowledgeMultiple::AddKnowledge(
	GoUctAdditiveKnowledge* knowledge)
{
	if (m_additiveKnowledge.NonEmpty())
    	SG_ASSERT(   knowledge->ProbabilityBased() 
                  == m_additiveKnowledge[0]->ProbabilityBased());
    SG_ASSERT(! m_additiveKnowledge.Contains(knowledge));
    m_additiveKnowledge.PushBack(knowledge);
}

const GoUctAdditiveKnowledge* 
	GoUctAdditiveKnowledgeMultiple::FirstKnowledge() const
{
	SG_ASSERT(m_additiveKnowledge.NonEmpty());
	return m_additiveKnowledge[0];
}

const GoBoard& GoUctAdditiveKnowledgeMultiple::Board() const
{
	return FirstKnowledge()->Board();
}

void GoUctAdditiveKnowledgeMultiple::InitPredictorValues(InfoVector& moves)
const
{
    for (size_t j = 0; j < moves.size(); ++j)
        switch (m_combinationType)
    {
        case COMBINE_MULTIPLY:
        case COMBINE_GEOMETRIC_MEAN:
            moves[j].m_predictorValue = 1.0;
            break;
            
        case COMBINE_ADD:
        case COMBINE_AVERAGE:
        case COMBINE_MAX:
            moves[j].m_predictorValue = 0.0;
            break;
            
        default: SG_ASSERT(false);
            break;
    }
}

bool GoUctAdditiveKnowledgeMultiple::ProbabilityBased() const
{
	return FirstKnowledge()->ProbabilityBased();
}

inline void Combine(float& v, float newV,
                    GoUctKnowledgeCombinationType combinationType)
{
    switch (combinationType)
    {
        case COMBINE_MULTIPLY:
        case COMBINE_GEOMETRIC_MEAN:
            v *= newV;
            break;
            
        case COMBINE_ADD:
        case COMBINE_AVERAGE:
            v += newV;
            break;
            
        case COMBINE_MAX:
            v = std::max(v, newV);
            break;
            
        default:
            SG_ASSERT(false);
            break;
    }
}

inline void PostProcess(InfoVector& moves,
                        int nuPredictors,
                        GoUctKnowledgeCombinationType combinationType)
{
    switch (combinationType)
    {
        case COMBINE_GEOMETRIC_MEAN:
            ComputeArithmeticMean(moves, nuPredictors);
            break;
            
        case COMBINE_AVERAGE:
            ComputeAverage(moves, nuPredictors);
            break;
            
        default: // done, no postprocessing needed
            break;
    }
}

void GoUctAdditiveKnowledgeMultiple::ProcessPosition(InfoVector& moves)
{
    InitPredictorValues(moves);
    const int moveNum = Board().MoveNumber();
    int nuPredictors = 0;
    for (SgVectorIterator<GoUctAdditiveKnowledge*> it(m_additiveKnowledge);
         it; ++it)
    {
        if ((*it)->InMoveRange(moveNum))
        {
            ++nuPredictors; // count the predictors we are actually using.
            std::vector<SgUctMoveInfo> movesCopy = moves;
            InitPredictorValues(movesCopy);
            (*it)->ProcessPosition(movesCopy);
            for (std::size_t j = 0; j < moves.size(); ++j)
                Combine(moves[j].m_predictorValue,
                        movesCopy[j].m_predictorValue,
                        m_combinationType);
        }
    }
    
    if (nuPredictors > 1)
        PostProcess(moves, nuPredictors, m_combinationType);
}
