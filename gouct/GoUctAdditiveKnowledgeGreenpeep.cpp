//----------------------------------------------------------------------------
/** @file GoUctAdditiveKnowledgeGreenpeep.cpp
    See GoUctAdditiveKnowledgeGreenpeep.h
*/
//----------------------------------------------------------------------------
#include "SgSystem.h"
#include "GoUctAdditiveKnowledgeGreenpeep.h"

#include <bitset>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>

#include "GoBoardUtil.h"
#include "SgPlatform.h"
#include "SgStringUtil.h"

using boost::filesystem::path;
using std::string;
//----------------------------------------------------------------------------
namespace {

const unsigned int PASS_CONTEXT = UINT_MAX;

inline unsigned int IsColorOrBorder(const GoBoard& bd, SgPoint p, int c)
{
	return static_cast<unsigned int> (bd.IsColor(p,c) || bd.IsBorder(p));
}

unsigned int SimpleContext(const GoBoard& bd, SgMove p, int c) 
{
	return  IsColorOrBorder(bd, p - SG_NS - SG_WE, c)
           |
           (IsColorOrBorder(bd, p - SG_WE        , c) << 1) 
           |
           (IsColorOrBorder(bd, p + SG_NS - SG_WE, c) << 2) 
           |
           (IsColorOrBorder(bd, p - SG_NS        , c) << 3) 
           |
           (IsColorOrBorder(bd, p + SG_NS        , c) << 4) 
           |
           (IsColorOrBorder(bd, p - SG_NS + SG_WE, c) << 5) 
           |
           (IsColorOrBorder(bd, p + SG_WE        , c) << 6) 
           |
           (IsColorOrBorder(bd, p + SG_NS + SG_WE, c) << 7) 
           ;
}

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
                    context |= (0x1U << 24);
        
                if (! SgIsSpecialMove(p) && atariBits[p])
                    context |= (0x1U << 25);
            }

            contexts[i] = context;
        }
        else // Pass
            contexts[i] = PASS_CONTEXT;
    }
}

bool ReadFile(std::istream& stream, const unsigned int nuPatterns, 
			  unsigned short predictor[])
{
    for (unsigned int i = 0; i < nuPatterns; ++i)
        predictor[i] = NEUTRALPREDICTION;

    if (! stream.good())
    {
        SgDebug() << "Failed to read pattern file\n";
        return false;
    }

    int count = 0;
    for (;; ++count)
    {
        unsigned int context;
        stream >> std::hex >> context;
        if (! stream.good())
            break;
        if (context >= nuPatterns)
        {
            SgDebug() << "pattern file: bad context " << context << '\n';
            return false;
        }
        stream >> std::dec >> predictor[context];
    }
    SgDebug() << "Read " << count << " patterns\n";
    return true;
}

bool ReadPatternFile(const path& file, const unsigned int nuPatterns, 
			  unsigned short predictor[])
{
    string nativeFile = SgStringUtil::GetNativeFileName(file);
    SgDebug() << "Loading pattern file from '" << nativeFile << "'... ";
    std::ifstream in(nativeFile.c_str());
    if (! in)
    {
        SgDebug() << "not found\n";
        return false;
    }
    try
    {
        ReadFile(in, nuPatterns, predictor);
    }
    catch (const SgException& e)
    {
        SgDebug() << "error: " << e.what() << '\n';
        return false;
    }
    return true;
}

/** try to find the pattern file in the "default" locations */
bool TryReadFile(const string& fileName, const unsigned int nuPatterns, 
			  unsigned short predictor[])
{
    path topSourceDir = SgPlatform::GetTopSourceDir();
    if (ReadPatternFile(topSourceDir / "book" / fileName, 
                        nuPatterns, predictor))
        return true;

    path programDir = SgPlatform::GetProgramDir();
    if (ReadPatternFile(programDir / fileName, nuPatterns, predictor))
        return true;

	// for case where program is in 
    // fuego/build/<build-type>/fuegomain directory.
    // and patterns are in fuego/book directory
	path parent3Dir = programDir.parent_path().parent_path().parent_path();
    if (ReadPatternFile(parent3Dir / "book" / fileName, 
                        nuPatterns, predictor))
        return true;
    
#ifdef ABS_TOP_SRCDIR
    if (ReadPatternFile(path(ABS_TOP_SRCDIR) / "book" / fileName, 
    					nuPatterns, predictor))
        return true;
#endif
#if defined(DATADIR) && defined(PACKAGE)
    if (ReadPatternFile(path(DATADIR) / PACKAGE / fileName, 
                        nuPatterns, predictor))
        return true;
#endif
    SgWarning() << "Pattern file not found: " << fileName 
    			<< ". continuing without.\n";
    return false;
}

void ReadPatternFiles(unsigned short predictor9[], 
                      unsigned short predictor19[])
{
    const string fileName9 = "patterns9.dat";
    const string fileName19 = "patterns19.dat";

    TryReadFile(fileName9, NUMPATTERNS9X9, predictor9);
    TryReadFile(fileName19, NUMPATTERNS19X19, predictor19);
}

} // namespace

//----------------------------------------------------------------------------

GoUctAdditiveKnowledgeParamGreenpeep::GoUctAdditiveKnowledgeParamGreenpeep()
{
    ReadPatternFiles(m_predictor9x9, m_predictor19x19);
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
        if (m_contexts[i] == PASS_CONTEXT) 
            moves[i].m_predictorValue = PASSPREDICTION;
        else
        {
            // Really should define constants for these bit offsets
            if (m_contexts[i] & (0x1U << 25))
            {
                if (use9x9flag) 
                {
                    // Hmm, we could do this max in the feature weights at the
                    // end of training instead.
                    int altContext = m_contexts[i] & ~(0x1U << 25);
                    moves[i].m_predictorValue = 
                    	std::max(pred[m_contexts[i]], 
                                 pred[altContext]);
                }
                else 
                {
                    /* default, for 19x19 */
                    moves[i].m_predictorValue = 
                    	std::max(pred[m_contexts[i]], 
                                 DEFENSIVEPREDICTION);
                }
            }
            else
            {
                moves[i].m_predictorValue = pred[m_contexts[i]];
            }
        }
	    moves[i].m_predictorValue /= (float) NEUTRALPREDICTION;
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
    enemy &= occupancy;

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
        int mask = (0x1 << i);

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

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (pattern[i][j] < 0)
            	str << std::string(strWidth + 2, ' ');
            else
            	str << '[' << strs[pattern[i][j]] << ']';
        }
        str << '\n';
    }

    if ((context >> 24) & 0x1)
    	str << "Ko exists\n";
    if ((context >> 25) & 0x1)
    	str << "Atari defense move\n";
}

//----------------------------------------------------------------------------
