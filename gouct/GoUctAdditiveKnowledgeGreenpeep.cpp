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
	return static_cast<unsigned int> (bd.IsColor(p,c) || bd.IsBorder(p));
}

/** 8 bits describing the 8 neighbors of p.
	A bit is set iff the point is either of color c or off the board. */
unsigned int SimpleContext(const GoBoard& bd, SgMove p, int c) 
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
                           SgBlackWhite toplay,
                           SgBlackWhite opponent)
{
    unsigned int extendedcontext = 0U;
    SgPoint p1 = p + dir;
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
            SgPoint p2 = p1 + dir;
            if (bd.IsBorder(p2))
                extendedcontext |= (v1 | v2); /* off-edge */
            else if (bd.IsColor(p2, toplay))
                extendedcontext |= v1;
            else if (bd.IsColor(p2, opponent))
                extendedcontext |= v2;
        }
    }
    return extendedcontext;
}

/** Build extended context bitset for point p */
unsigned int ExtendedContext(const GoBoard &bd, 
                             SgMove p,
                             unsigned int occupancy,
                             SgBlackWhite toplay,
                             SgBlackWhite opponent
                             ) 
{
    unsigned int extendedcontext = 
      CheckDirection(bd, p, -SG_WE,  0x2U,  0x1U, occupancy, 0x2U,
                     toplay, opponent)
    | CheckDirection(bd, p, -SG_NS,  0x8U,  0x4U, occupancy, 0x8U,
                     toplay, opponent)
    | CheckDirection(bd, p, +SG_NS, 0x20U, 0x10U, occupancy, 0x10U,
                     toplay, opponent)
    | CheckDirection(bd, p, +SG_WE, 0x80U, 0x40U, occupancy, 0x40U,
                     toplay, opponent);
    return extendedcontext;
}

void ComputeContexts(const GoBoard &bd,
                     vector<SgUctMoveInfo>::const_iterator begin,
                     vector<SgUctMoveInfo>::const_iterator end,
                     unsigned int contexts[])
{
    bool use9x9flag = bd.Size() < 15;
    std::bitset<SG_MAXPOINT + 1> atariBits;
    SgBlackWhite toplay = bd.ToPlay();
    SgBlackWhite opponent = bd.Opponent();
    bool koExists = bd.KoPoint() != SG_NULLPOINT;
    SgMove lastMove = bd.GetLastMove();
    if (  use9x9flag 
       && ! SgIsSpecialMove(lastMove) // skip if Pass or Nullmove
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
            unsigned int blackcontext = SimpleContext(bd, p, SG_BLACK);
            unsigned int whitecontext = SimpleContext(bd, p, SG_WHITE);
            unsigned int occupancy = blackcontext ^ whitecontext;
            unsigned int extendedcontext = 
                ExtendedContext(bd, p, occupancy, toplay, opponent);
            unsigned int context = 0;

            if (toplay == SG_BLACK)
                context = extendedcontext | (blackcontext << 8) | 
                    (whitecontext << 16);
            else
                context = extendedcontext | (whitecontext << 8) | 
                    (blackcontext << 16);

            if (use9x9flag) 
            {
                if (koExists)
                    context |= KO_BIT;
        
                if (! SgIsSpecialMove(p) && atariBits[p])
                    context |= ATARI_BIT;
            }

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
  : GoUctAdditiveKnowledge(bd),
    m_param(param)
{
    // Knowledge applies to all moves
    SetMoveRange(0, 10000); 
}


void 
GoUctAdditiveKnowledgeGreenpeep::ProcessPosition(
									std::vector<SgUctMoveInfo>& moves)
{
    bool use9x9flag;
    const unsigned short *pred;

    if (Board().Size() < 15)
    {
        use9x9flag = true;
        pred = m_param.m_predictor9x9;
    }
    else
    {
        use9x9flag = false;
        pred = m_param.m_predictor19x19;
    }

    ComputeContexts(Board(), moves.begin(), moves.end(), m_contexts);
    for (std::size_t i = 0; i < moves.size(); ++i)
    {
        float& value = moves[i].m_predictorValue;
        if (m_contexts[i] == PASS_CONTEXT) 
            value = PASSPREDICTION;
        else
        {
            if (m_contexts[i] & ATARI_BIT)
            {
                if (use9x9flag) 
                {
                    // Hmm, we could do this max in the feature weights at the
                    // end of training instead.
                    int altContext = m_contexts[i] & ~ATARI_BIT;
                    value = std::max(pred[m_contexts[i]],
                                        pred[altContext]);
                }
                else 
                {
                    /* default, for 19x19 */
                    value = std::max(pred[m_contexts[i]],
                                        DEFENSIVEPREDICTION);
                }
            }
            else
            {
                value = pred[m_contexts[i]];
            }
        }
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
