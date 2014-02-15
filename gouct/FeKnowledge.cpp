//----------------------------------------------------------------------------
/** @file FeKnowledge.cpp
 See FeKnowledge.h
 */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeKnowledge.h"

#include "FeBasicFeatures.h"
#include "GoUctFeatures.h"
#include "SgDebug.h"

//----------------------------------------------------------------------------
/** @todo This is a tunable constant. */
const float FeKnowledge::VALUE_MULTIPLIER = 10.0f;

namespace {
    inline float sigmoid(float x) // it is really sigmoid(-x) computed here
{
    return 1 / (1 + exp(x));
}
}
//----------------------------------------------------------------------------

FeKnowledge::FeKnowledge(const GoBoard& bd,
                         const FeFeatureWeights& weights)
    : GoAdditiveKnowledge(bd),
      m_weights(weights)
{ }

void FeKnowledge::ProcessPosition(std::vector<SgUctMoveInfo>& moves)
{
    const GoBoard& bd = Board();
    SgPointArray<FeFeatures::FeMoveFeatures> features;
    FeFeatures::FeMoveFeatures passFeatures;
    GoUctFeatures::FindAllFeatures(bd, features, passFeatures);
    SgPointArray<float> eval = EvaluateFeatures(bd, features, m_weights);
    float passEval = EvaluateMoveFeatures(passFeatures, m_weights);
    
    for (size_t i = 0; i < moves.size(); ++i) // todo just use iterator
    {
        const SgPoint move = moves[i].m_move;
        SG_ASSERT(bd.IsLegal(move));
        float moveVal = (move == SG_PASS) ? passEval : eval[move];
        moves[i].m_predictorValue = sigmoid(VALUE_MULTIPLIER * moveVal);
    }
}

//----------------------------------------------------------------------------
FeKnowledgeFactory::FeKnowledgeFactory() : m_weights(0, 0)
{
    ReadWeights();
}
    
FeKnowledgeFactory::~FeKnowledgeFactory()
{ }

GoAdditiveKnowledge* FeKnowledgeFactory::Create(const GoBoard& bd)
{
    return new FeKnowledge(bd, m_weights);
}

void FeKnowledgeFactory::ReadWeights()
{
    const std::string fileName =
    "/Users/mmueller/Projects/fuego/data/features.model"; // TODO
    try
    {
        std::ifstream in(fileName.c_str());
        if (! in)
        throw SgException("Cannot find file " + fileName);
        m_weights = FeFeatureWeights::Read(in);
    }
    catch (const SgException& e)
    {
        SgDebug() << "Loading features file failed: " << e.what();
    }
    SgDebug() << "FeKnowledgeFactory Read weights for "
              << m_weights.m_nuFeatures
              << " features with k = " << m_weights.m_k << '\n';
}


//----------------------------------------------------------------------------
