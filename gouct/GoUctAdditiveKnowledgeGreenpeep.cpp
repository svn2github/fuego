//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeGreenpeep.cpp
    See GoUctAdditiveKnowledgeGreenpeep.h
*/
//----------------------------------------------------------------------------
#include "SgSystem.h"
#include "GoUctAdditiveKnowledgeGreenpeep.h"

#include <bitset>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>

#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoPattern12Point.h"
#include "SgPlatform.h"

struct PatternEntry // common data structure used in both 9x9 and 19x19
{
    unsigned int index;
    unsigned short code;
};

#include "GoUctGreenpeepPatterns9.h"
#include "GoUctGreenpeepPatterns19.h"

//----------------------------------------------------------------------------
namespace {

const short unsigned int NEUTRALPREDICTION = 512;
const float NEUTRALPREDICTION_FLOAT = static_cast<float>(NEUTRALPREDICTION);
const short unsigned int PASSPREDICTION = 2;

const unsigned int PASS_CONTEXT = UINT_MAX;

void ComputeContexts19(const GoBoard& bd,
                     std::vector<SgUctMoveInfo>::const_iterator begin,
                     std::vector<SgUctMoveInfo>::const_iterator end,
                     unsigned int contexts[])
{
    SG_ASSERT(bd.Size() >= 15);
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opponent = bd.Opponent();

    for (int i = 0; begin != end; ++begin, ++i)
    {
        SgMove p = begin->m_move;
        if (p != SG_PASS)
        {
            unsigned int context =
                GoPattern12Point::Context(bd, p, toPlay, opponent);
            contexts[i] = context;
        }
        else // Pass
            contexts[i] = PASS_CONTEXT;
    }
}

void ComputeContexts9(const GoBoard& bd,
                     std::vector<SgUctMoveInfo>::const_iterator begin,
                     std::vector<SgUctMoveInfo>::const_iterator end,
                     unsigned int contexts[])
{
    SG_ASSERT(bd.Size() < 15);
    std::bitset<SG_MAXPOINT + 1> atariBits;
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opponent = bd.Opponent();
    const bool koExists = bd.KoPoint() != SG_NULLPOINT;
    const SgMove lastMove = bd.GetLastMove();
    if (  ! SgIsSpecialMove(lastMove) // skip if Pass or Nullmove
       && ! bd.IsEmpty(lastMove)   // skip if last move was suicide
       )
    {
        GoPointList defenses;
        GoBoardUtil::AtariDefenseMoves(bd, lastMove, defenses);
        for (GoPointList::Iterator it(defenses); it; ++it) 
            atariBits[*it] = 1;
    }

    for (int i = 0; begin != end; ++begin, ++i) 
    {
        SgMove p = begin->m_move;
        if (p != SG_PASS)
        {
            unsigned int context =
                GoPattern12Point::Context(bd, p, toPlay, opponent);
            if (koExists)
                context |= GoPattern12Point::KO_BIT;
            if (! SgIsSpecialMove(p) && atariBits[p])
                context |= GoPattern12Point::ATARI_BIT;
            contexts[i] = context;
        }
        else // Pass
            contexts[i] = PASS_CONTEXT;
    }
}

void ReadPatternArray(unsigned short predictor[], int size,
                      PatternEntry patternEntry[], unsigned int nuPatterns)
{
    for (int i = 0; i < size; ++i)
        predictor[i] = NEUTRALPREDICTION;

    for (unsigned int i = 0; i < nuPatterns; ++i)
    {
        unsigned int context = patternEntry[i].index;
        SG_ASSERT(context < static_cast<unsigned int>(size));
        predictor[context] = patternEntry[i].code;
    }
}

void ReadPatterns(unsigned short predictor9[],
                  unsigned short predictor19[])
{
    ReadPatternArray(predictor9, NUMPATTERNS9X9,
                     greenpeepPatterns9, nuGreenpeepPatterns9);
    ReadPatternArray(predictor19, NUMPATTERNS19X19,
                     greenpeepPatterns19, nuGreenpeepPatterns19);
}

} // namespace

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeParamGreenpeep::GoUctAdditiveKnowledgeParamGreenpeep()
{
    ReadPatterns(m_predictor9x9, m_predictor19x19);
}

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeGreenpeep::GoUctAdditiveKnowledgeGreenpeep(
                        const GoBoard& bd,
                        const GoUctAdditiveKnowledgeParamGreenpeep& param)
  : GoAdditiveKnowledge(bd),
    m_param(param)
{ }

void GoUctAdditiveKnowledgeGreenpeep::
ProcessPosition19(std::vector<SgUctMoveInfo>& moves)
{
    SG_ASSERT(Board().Size() >= 15);
    const unsigned short* predictor(m_param.m_predictor19x19);
    ComputeContexts19(Board(), moves.begin(), moves.end(), m_contexts);

    for (std::size_t i = 0; i < moves.size(); ++i)
    {
        float& value = moves[i].m_predictorValue;
        const unsigned int context = m_contexts[i];
        if (context == PASS_CONTEXT)
        {
            SG_ASSERT(moves[i].m_move == SG_PASS);
            value = PASSPREDICTION / NEUTRALPREDICTION_FLOAT;
        }
        else
            value = predictor[context] / NEUTRALPREDICTION_FLOAT;
    }
}

void GoUctAdditiveKnowledgeGreenpeep::
ProcessPosition9(std::vector<SgUctMoveInfo>& moves)
{
    SG_ASSERT(Board().Size() < 15);
    const unsigned short* predictor = m_param.m_predictor9x9;
    ComputeContexts9(Board(), moves.begin(), moves.end(), m_contexts);

    for (std::size_t i = 0; i < moves.size(); ++i)
    {
        float& value = moves[i].m_predictorValue;
        const unsigned int context = m_contexts[i];
        if (context == PASS_CONTEXT)
        {
            SG_ASSERT(moves[i].m_move == SG_PASS);
            value = PASSPREDICTION;
        }
        else if (context & GoPattern12Point::ATARI_BIT)
        {
            // Hmm, we could do this max in the feature weights at the
            // end of training instead.
            const unsigned int altContext =
                context & ~GoPattern12Point::ATARI_BIT;
            value = std::max(predictor[context], predictor[altContext]);
        }
        else
            value = predictor[context];
	    value /= NEUTRALPREDICTION_FLOAT;
    }
}

//----------------------------------------------------------------------------
