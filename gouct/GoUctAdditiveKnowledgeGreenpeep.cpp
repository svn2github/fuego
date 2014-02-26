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

#include "GoBoardUtil.h"
#include "SgPlatform.h"
#include "SgStringUtil.h"

struct PatternEntry // common data structure used in both 9x9 and 19x19
{
    unsigned int index;
    unsigned short code;
};

#include "GoUctGreenpeepPatterns9.h"
#include "GoUctGreenpeepPatterns19.h"

using std::string;
//----------------------------------------------------------------------------
namespace {

const short unsigned int NEUTRALPREDICTION = 512;
const float NEUTRALPREDICTION_FLOAT = static_cast<float>(NEUTRALPREDICTION);
const short unsigned int PASSPREDICTION = 2;

/* 19x19-only.  Different scale from PASSPREDICTION. */
const short unsigned int DEFENSIVEPREDICTION = 5 * NEUTRALPREDICTION; 

const unsigned int PASS_CONTEXT = UINT_MAX;
const unsigned int KO_BIT = 1U << 24;
const unsigned int ATARI_BIT = 1U << 25;

/** A bit is set iff point p is either of color c or off the board. */
inline unsigned int ColorOrBorderBit(const GoBoard& bd, SgPoint p, int c)
{
    const SgBoardColor color = bd.GetColor(p);
    return static_cast<unsigned int> (color == c || color == SG_BORDER);
}

// TODO optimize: Border mask is constant per p and
// should be precomputed in a table and OR-ed in once, separately.
// or maybe things should be recoded to use GetColor() directly as 2 bits.

/** 8 bits describing the 8 neighbors of p.
	A bit is set iff the point is either of color c or off the board. */
inline unsigned int SimpleContext(const GoBoard& bd, SgMove p, int c)
{
	return  ColorOrBorderBit(bd, p - SG_NS - SG_WE, c)
           |
           (ColorOrBorderBit(bd, p - SG_WE        , c) << 1) 
           |
           (ColorOrBorderBit(bd, p + SG_NS - SG_WE, c) << 2) 
           |
           (ColorOrBorderBit(bd, p - SG_NS        , c) << 3) 
           |
           (ColorOrBorderBit(bd, p + SG_NS        , c) << 4) 
           |
           (ColorOrBorderBit(bd, p - SG_NS + SG_WE, c) << 5) 
           |
           (ColorOrBorderBit(bd, p + SG_WE        , c) << 6) 
           |
           (ColorOrBorderBit(bd, p + SG_NS + SG_WE, c) << 7) 
           ;
}

/** Build extended context in one direction dir */
inline unsigned int CheckDirection(const GoBoard &bd, 
                                   SgPoint p,
                                   int dir, 
                                   unsigned int v2, 
                                   unsigned int v1, 
                                   unsigned int occupancy,
                                   unsigned int mask,
                                   SgBlackWhite toPlay,
                                   SgBlackWhite opponent)
{
    unsigned int extendedcontext = 0U;
    const SgPoint p1 = p + dir;
    if ((occupancy & mask) > 0U)
    {
        int nuLib = bd.NumLiberties(p1);
        if (nuLib >= 3)
        	extendedcontext |= v2;
        else if (nuLib == 2)
        	extendedcontext |= v1;
    } 
    else
    { /* *** provide info on point 2 away in this direction */
        if (bd.IsBorder(p1))
            extendedcontext |= (v1 | v2); /* off-edge */
        else
        {
            const SgPoint p2 = p1 + dir;
            if (bd.IsBorder(p2))
                extendedcontext |= (v1 | v2); /* off-edge */
            else if (bd.IsColor(p2, toPlay))
                extendedcontext |= v1;
            else if (bd.IsColor(p2, opponent))
                extendedcontext |= v2;
        }
    }
    return extendedcontext;
}

/** Build extended context bitset for point p */
inline unsigned int ExtendedContext(const GoBoard &bd,
                             SgMove p,
                             unsigned int occupancy,
                             SgBlackWhite toPlay,
                             SgBlackWhite opponent
                             ) 
{
    const unsigned int extendedcontext =
      CheckDirection(bd, p, -SG_WE,  0x2U,  0x1U, occupancy, 0x2U,
                     toPlay, opponent)
    | CheckDirection(bd, p, -SG_NS,  0x8U,  0x4U, occupancy, 0x8U,
                     toPlay, opponent)
    | CheckDirection(bd, p, +SG_NS, 0x20U, 0x10U, occupancy, 0x10U,
                     toPlay, opponent)
    | CheckDirection(bd, p, +SG_WE, 0x80U, 0x40U, occupancy, 0x40U,
                     toPlay, opponent);
    return extendedcontext;
}

inline unsigned int Context(const GoBoard& bd, SgPoint p,
                            const SgBlackWhite toPlay,
                            const SgBlackWhite opponent)
{
    const unsigned int toPlayContext = SimpleContext(bd, p, toPlay);
    const unsigned int oppContext = SimpleContext(bd, p, opponent);
    const unsigned int extendedcontext =
        ExtendedContext(bd, p, toPlayContext ^ oppContext, toPlay, opponent);
    return extendedcontext | (toPlayContext << 8) | (oppContext << 16);
}

void ComputeContexts19(const GoBoard& bd,
                     vector<SgUctMoveInfo>::const_iterator begin,
                     vector<SgUctMoveInfo>::const_iterator end,
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
            unsigned int context = Context(bd, p, toPlay, opponent);
            contexts[i] = context;
        }
        else // Pass
            contexts[i] = PASS_CONTEXT;
    }
}

void ComputeContexts9(const GoBoard& bd,
                     vector<SgUctMoveInfo>::const_iterator begin,
                     vector<SgUctMoveInfo>::const_iterator end,
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
            unsigned int context = Context(bd, p, toPlay, opponent);
            if (koExists)
                context |= KO_BIT;
            if (! SgIsSpecialMove(p) && atariBits[p])
                context |= ATARI_BIT;
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
//----------------------------------------------------------------------------

const int START_INDEX_12_POINT = 3000;

} // namespace

//----------------------------------------------------------------------------

void GoUct12PointPattern::
Find12PointFeatures(const GoBoard& bd,
                    GoEvalArray<FeMoveFeatures>& features)
{
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opponent = bd.Opponent();
    
    for(GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsLegal(*it))
        {
            const SgPoint p = *it;
            unsigned int context = Context(bd, p, toPlay, opponent);
            features[p].Set12PointIndex(START_INDEX_12_POINT + context);
        }
}
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
        else if (context & ATARI_BIT)
        {
            // Hmm, we could do this max in the feature weights at the
            // end of training instead.
            const unsigned int altContext = context & ~ATARI_BIT;
            value = std::max(predictor[context], predictor[altContext]);
        }
        else
            value = predictor[context];
	    value /= NEUTRALPREDICTION_FLOAT;
    }
}

void
GoUctAdditiveKnowledgeGreenpeep::PrintContext(unsigned int context, 
                                              std::ostream& str)
{
    enum codes
    {
        W, W1, W2, W3, B, B1, B2, B3, EMPTY, BORDER, MOVE, ERROR, COUNT
    };

    int friendlyCodes[] = {B, B1, B2, B3};
    int enemyCodes[] = {W, W1, W2, W3};
    int extraCodes[] = {ERROR, EMPTY, B, W, BORDER};

    int strWidth = 2;
    std::string strs[COUNT];
    strs[W] = "w ";
    strs[W1] = "w1";
    strs[W2] = "w2";
    strs[W3] = "w3";
    strs[B] = "b ";
    strs[B1] = "b1";
    strs[B2] = "b2";
    strs[B3] = "b3";
    strs[EMPTY] = "  ";
    strs[BORDER] = "##";
    strs[MOVE] = "@b";
    strs[ERROR] = "?!";

    unsigned int friendly = (context >> 8) & 0xff;
    unsigned int enemy = (context >> 16) & 0xff;
    unsigned int occupancy = friendly ^ enemy;
    unsigned int borders = friendly & enemy;
    friendly &= occupancy;

    int pattern[5][5];
    memset(&pattern, -1, sizeof(pattern));
    pattern[2][2] = MOVE;

    const int NUM_PTS = 8;
    int* pointMap[NUM_PTS] =
    {
        &pattern[3][1], &pattern[2][1], &pattern[1][1],
        &pattern[3][2], &pattern[1][2],
        &pattern[3][3], &pattern[2][3], &pattern[1][3]
    };

    for (int i = 0; i < NUM_PTS; i++)
    {
        int mask = 1 << i;

        int ext = -1;
        int* extPoint;
        switch (i)
        {
            case 1:
                ext = context & 0x3;
                extPoint = &pattern[2][0];
                break;
            case 3:
                ext = (context >> 2) & 0x3;
                extPoint = &pattern[4][2];
                break;
            case 4:
                ext = (context >> 4) & 0x3;
                extPoint = &pattern[0][2];
                break;
            case 6:
                ext = (context >> 6) & 0x3;
                extPoint = &pattern[2][4];
                break;
            default:
                break;
        }
        ++ext;

        if (occupancy & mask)
        {
            // ext represents the liberty count. Currently the value 0x3 is not
            // used.
            SG_ASSERT(ext <= 3);

            if (friendly & mask)
            	*(pointMap[i]) = friendlyCodes[ext];
            else
            	*(pointMap[i]) = enemyCodes[ext];
        }
        else
        {
            if (borders & mask)
            	*(pointMap[i]) = BORDER;
            else
            	*(pointMap[i]) = EMPTY;

            if (ext > 0)
            	*extPoint = extraCodes[ext];
        }
    }

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            if (pattern[i][j] < 0)
            	str << std::string(strWidth + 2, ' ');
            else
            	str << '[' << strs[pattern[i][j]] << ']';
        }
        str << '\n';
    }

    if (context & KO_BIT)
    	str << "Ko exists\n";
    if (context & ATARI_BIT)
    	str << "Atari defense move\n";
}

//----------------------------------------------------------------------------
