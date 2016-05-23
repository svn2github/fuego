//----------------------------------------------------------------------------
/** @file GoUctPatterns.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_PATTERNS_H
#define GOUCT_PATTERNS_H

#include <boost/array.hpp>
#include <cstdio>
#include <utility>
#include <string>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoPattern3x3.h"
#include "GoUctGlobalPatternData.h"
#include "GoUctLocalPatternData.h"
#include "GoUctPatternData.h"
#include "SgBoardColor.h"
#include "SgBWArray.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Hard-coded pattern matching routines to match patterns used by MoGo.
    See <a href="http://hal.inria.fr/docs/00/11/72/66/PDF/MoGoReport.pdf">
    Modification of UCT with Patterns in Monte-Carlo Go</a>.
    
    In addition, a machine-learned gamma value for move urgency
    is used in Fuego.

    The move is always in the center of the pattern or at the middle edge
    point (lower line) for edge patterns. The patterns are matched for both
    colors, unless specified otherwise. Notation:
    @verbatim
    O  White            x = Black or Empty
    X = Black           o = White or Empty
    . = Empty           B = Black to Play
    ? = Don't care      W = White to Play
    @endverbatim

    Patterns for Hane. <br>
    True is returned if any pattern is matched.
    @verbatim
    X O X   X O .   X O ?   X O O
    . . .   . . .   X . .   . . .
    ? ? ?   ? . ?   ? . ?   ? . ? B
    @endverbatim

    Patterns for Cut1. <br>
    True is returned if the first pattern is matched, but not the next two.
    @verbatim
    X O ?   X O ?   X O ?
    O . ?   O . O   O . .
    ? ? ?   ? . ?   ? O ?
    @endverbatim

    Pattern for Cut2.
    @verbatim
    ? X ?
    O . O
    x x x
    @endverbatim

    Pattern for Edge. <br>
    True is returned if any pattern is matched.
    @verbatim
    X . ?   ? X ?   ? X O    ? X O    ? X O
    O . ?   o . O   ? . ? B  ? . o W  O . X W
    @endverbatim */

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

template<class BOARD>
class GoUctPatterns
{
public:

    enum PatternType
    {
        PATTERN_GLOBAL,
        PATTERN_LOCAL
    };

    GoUctPatterns(const BOARD& bd);

    GoUctPatterns(const BOARD& bd, PatternType patternType);

	float GetPatternGamma(const BOARD& bd, const SgPoint p,
			const SgBlackWhite toPlay) const;

    /** Match any of the standard MoGo patterns. */
    bool MatchAny(SgPoint p) const;

    /** If matches any MoGo pattern, return true and lookup gamma value.*/
    bool MatchAny(SgPoint p, float& gamma) const;

	/** provide interface of other gamma patterns*/
	void InitializeGammaPatternFromProcessedData(PatternType patternType);

    /** Gamma value for given 8-neighbor code */
    float CenterGamma(const SgBlackWhite toPlay, int code) const;

    /** Gamma value for given 5-neighbor code */
    float EdgeGamma(const SgBlackWhite toPlay, int code) const;

private:
	/** Match any of the center patterns, and return gamma */
	float MatchAnyCenterForGamma(SgPoint p, const SgBlackWhite toPlay) const;
    
	/** Match any of the edge patterns, and return gamma */
	float MatchAnyEdgeForGamma(SgPoint p, const SgBlackWhite toPlay) const;

	/** Copy gamma values from pt into table */
    template<class TABLE>
    void SetGammaValues(const GoUctPatternData::BWTable& pt, TABLE& table);
    
    const BOARD& m_bd;

    /** lookup table for 8-neighborhood of a move candidate */
    SgBWArray<GoPattern3x3::GoPatternTable> m_table;

    /** lookup table on the edge of board */
    SgBWArray<GoPattern3x3::GoEdgePatternTable> m_edgeTable;

    /** Match any of the center patterns. */
    bool MatchAnyCenter(SgPoint p) const;

    /** Match any of the center patterns, and put value to gamma*/
    bool MatchAnyCenter(SgPoint p, float& gamma) const;

    /** Match any of the edge patterns. */
    bool MatchAnyEdge(SgPoint p) const;

    /** Match any of the edge patterns, and put value to gamma */
    bool MatchAnyEdge(SgPoint p, float& gammma) const;    
};

//----------------------------------------------------------------------------

template<class BOARD>
GoUctPatterns<BOARD>::GoUctPatterns(const BOARD& bd, PatternType patternType)
    : m_bd(bd)
{
    InitializeGammaPatternFromProcessedData(patternType);
    GoPattern3x3::InitCenterPatternTable(m_table);
    GoPattern3x3::InitEdgePatternTable(m_edgeTable);
}

template<class BOARD>
GoUctPatterns<BOARD>::GoUctPatterns(const BOARD& bd)
    : m_bd(bd)
{
    GoPattern3x3::InitCenterPatternTable(m_table);
    GoPattern3x3::InitEdgePatternTable(m_edgeTable);
}

template<class BOARD>
float GoUctPatterns<BOARD>::CenterGamma(const SgBlackWhite toPlay, int code)
const
{
    return m_table[toPlay][code].GetGammaValue();
}

template<class BOARD>
float GoUctPatterns<BOARD>::EdgeGamma(const SgBlackWhite toPlay, int code)
const
{
    return m_edgeTable[toPlay][code].GetGammaValue();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyCenter(SgPoint p) const
{
    return m_table[m_bd.ToPlay()]
                  [GoPattern3x3::CodeOf8Neighbors(m_bd, p)].IsPattern();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyEdge(SgPoint p) const
{
    return
        m_edgeTable[m_bd.ToPlay()]
                   [GoPattern3x3::CodeOfEdgeNeighbors(m_bd, p)].IsPattern();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAny(SgPoint p) const
{
    // Quick refutation using the incremental neighbor counts of the board:
    // all patterns have at least one adjacent stone
    if (  m_bd.NumNeighbors(p, SG_BLACK) == 0
       && m_bd.NumNeighbors(p, SG_WHITE) == 0
       )
        return false;
    if (m_bd.Line(p) > 1)
       return MatchAnyCenter(p);
    else if (m_bd.Pos(p) > 1)
        return MatchAnyEdge(p);
    else
        return false;
}

template<class BOARD>
float GoUctPatterns<BOARD>::GetPatternGamma(const BOARD& bd,
		const SgPoint p, const SgBlackWhite toPlay) const
{
    if (bd.Line(p) > 1)
        return MatchAnyCenterForGamma(p, toPlay);
    else if (bd.Pos(p) > 1)
        return MatchAnyEdgeForGamma(p, toPlay);
    else
        return 0;
}

template<class BOARD>
template<class TABLE>
void GoUctPatterns<BOARD>
	::SetGammaValues(const GoUctPatternData::BWTable& pt, TABLE& table)
{
    for (SgBWIterator it; it; ++it)
    {
        const SgBlackWhite color = *it;
        for (int i = 0; i < pt[color].m_nuPatterns; ++i)
        {
            const int code = pt[color].m_patternArray[i].m_code;
            if (code != -1)
                table[color][code].
                SetGammaValue(pt[color].m_patternArray[i].m_value);
        }
	}
}

template<class BOARD>
void GoUctPatterns<BOARD>
	::InitializeGammaPatternFromProcessedData(PatternType patternType)
{
    m_table[SG_BLACK].Fill(PatternInfo());
    m_table[SG_WHITE].Fill(PatternInfo());
    m_edgeTable[SG_BLACK].Fill(PatternInfo());
    m_edgeTable[SG_WHITE].Fill(PatternInfo());

    const GoUctPatternData::PatternData& pt =
        patternType == PATTERN_LOCAL ? GoUctLocalPatternData::gData :
                                       GoUctGlobalPatternData::gData;
    SetGammaValues(pt.m_edgePatterns, m_edgeTable);
    SetGammaValues(pt.m_centerPatterns, m_table);
}

template<class BOARD>
inline float GoUctPatterns<BOARD>::
MatchAnyCenterForGamma(SgPoint p, const SgBlackWhite toPlay) const
{
    return m_table[toPlay]
                  [GoPattern3x3::CodeOf8Neighbors(m_bd, p)].GetGammaValue();
}

template<class BOARD>
inline float GoUctPatterns<BOARD>::
MatchAnyEdgeForGamma(SgPoint p, const SgBlackWhite toPlay) const
{
    return m_edgeTable[toPlay]
                      [GoPattern3x3::CodeOfEdgeNeighbors(m_bd, p)]
                      .GetGammaValue();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAny(SgPoint p, float& gamma) const
{
    // Quick refutation using the incremental neighbor counts of the board:
    // all patterns have at least one adjacent stone
    if (  m_bd.NumNeighbors(p, SG_BLACK) == 0
       && m_bd.NumNeighbors(p, SG_WHITE) == 0
       )
        return false;
    if (m_bd.Line(p) > 1)
       return MatchAnyCenter(p, gamma);
    else if (m_bd.Pos(p) > 1)
        return MatchAnyEdge(p, gamma);
    else
        return false;
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyCenter(SgPoint p, float& gamma)
const
{
	const PatternInfo& pi = m_table[m_bd.ToPlay()]
                                   [GoPattern3x3::CodeOf8Neighbors(m_bd, p)];
    gamma = pi.GetGammaValue();
	return pi.IsPattern();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyEdge(SgPoint p, float& gamma) const
{
	const PatternInfo& pi =
            m_edgeTable[m_bd.ToPlay()]
                       [GoPattern3x3::CodeOfEdgeNeighbors(m_bd, p)];
    gamma = pi.GetGammaValue();
	return pi.IsPattern();
}

//----------------------------------------------------------------------------
#endif // GOUCT_PATTERNS_H
