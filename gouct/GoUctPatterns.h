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
/** class PatternInfo holds gamma value and MoGo-pattern flag for one move. */
class PatternInfo
{
public:
    PatternInfo() : m_gammaValue(0.f), m_isPattern(false)
    { }

	void SetGammaValue(float value);

	float GetGammaValue() const;

	void SetIsPattern(bool is);

	bool IsPattern() const;

private:
    float m_gammaValue;
    
    bool m_isPattern;
};

inline void PatternInfo::SetGammaValue(float value)
{
    m_gammaValue = value;
}

inline float PatternInfo::GetGammaValue() const
{
    return m_gammaValue;
}

inline void PatternInfo::SetIsPattern(bool is)
{
    m_isPattern = is;
}

inline bool PatternInfo::IsPattern() const
{
    return m_isPattern;
}

//----------------------------------------------------------------------------
namespace Pattern3x3
{
    /** 3^5 = size of edge pattern table */
    static const int GOUCT_POWER3_5 = 3 * 3 * 3 * 3 * 3;
    
    /** 3^8 = size of center pattern table. */
    static const int GOUCT_POWER3_8 = 3 * 3 * 3 * 3 * 3 * 3 * 3 * 3;
    
    /** See m_edgeTable. */
    typedef SgArray<PatternInfo, GOUCT_POWER3_5> GoUctEdgePatternTable;
    typedef boost::array<int, GOUCT_POWER3_5> EdgeCodeTable;

    /** See m_table. */
    typedef SgArray<PatternInfo, GOUCT_POWER3_8> GoUctPatternTable;
    typedef boost::array<int, GOUCT_POWER3_8> CenterCodeTable;

    /** Convert code into list of board colors */
    std::vector<SgEmptyBlackWhite> Decode(int code, std::size_t length);
    
    /** Inverse mapping for Map3x3CenterCode */
    int DecodeCenterIndex(int index);

    /** Inverse mapping for Map2x3EdgeCode */
    int DecodeEdgeIndex(int index);

    void InitCenterPatternTable(SgBWArray<GoUctPatternTable>& table);
    
    void InitEdgePatternTable(SgBWArray<GoUctEdgePatternTable>& edgeTable);

    int MakeCode(const std::vector<SgEmptyBlackWhite>& colors);
    
    /** Utility mapping function. Warning: global array, not threadsafe */
    int Map3x3CenterCode(int code, SgBlackWhite toPlay);

    /** Utility mapping function. Warning: global array, not threadsafe */
    int Map2x3EdgeCode(int code, SgBlackWhite toPlay);

    /** Map pattern codes to a shared code considering rotations.
        Returns number of unique patterns.
        The shared code is in the range 0 .. #distinct patterns - 1.
        Use case: shared feature weights for identical patterns. */
    int MapCenterPatternsToMinimum(CenterCodeTable& indexCode);

    /** Map pattern codes to a shared code considering rotations.
        Returns number of unique patterns.
        The shared code is in the range 0 .. #distinct patterns - 1.
        Use case: shared feature weights for identical patterns. */
    int MapEdgePatternsToMinimum(EdgeCodeTable& indexCode);

    /** Procedural matching function - used to initialize the table. */
    bool MatchAnyPattern(const GoBoard& bd, SgPoint p);

    /** return NS for input WE or -WE, return WE for input NS or -NS */
    int OtherDir(int dir);

    void ReduceCenterSymmetry(SgBWArray<GoUctPatternTable>& table);

    void ReduceEdgeSymmetry(SgBWArray<GoUctEdgePatternTable>& edgeTable);
    
    int SwapCenterColor(int code);
    
    int SwapEdgeColor(int code);

    void Write2x3EdgePattern(std::ostream& stream, int code);
    
    void Write3x3CenterPattern(std::ostream& stream, int code);
}
//----------------------------------------------------------------------------

inline int Pattern3x3::MakeCode(const std::vector<SgEmptyBlackWhite>& colors)
{
    int code = 0;
    for (vector<SgEmptyBlackWhite>::const_iterator it = colors.begin();
         it != colors.end(); ++it)
    {
        code *= 3;
        code += *it;
    }
    return code;
}

inline int Pattern3x3::OtherDir(int dir)
{
    if (dir == SG_NS || dir == -SG_NS)
        return SG_WE;
    SG_ASSERT(dir == SG_WE || dir == -SG_WE);
    return SG_NS;
}

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

    static int CodeOf8Neighbors(const BOARD& bd, SgPoint p);

    static int CodeOfEdgeNeighbors(const BOARD& bd, SgPoint p);
    
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
    SgBWArray<Pattern3x3::GoUctPatternTable> m_table;

    /** lookup table on the edge of board */
    SgBWArray<Pattern3x3::GoUctEdgePatternTable> m_edgeTable;

    static int EBWCodeOfPoint(const BOARD& bd, SgPoint p);

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
    Pattern3x3::InitCenterPatternTable(m_table);
    Pattern3x3::InitEdgePatternTable(m_edgeTable);
}

template<class BOARD>
GoUctPatterns<BOARD>::GoUctPatterns(const BOARD& bd)
    : m_bd(bd)
{
    Pattern3x3::InitCenterPatternTable(m_table);
    Pattern3x3::InitEdgePatternTable(m_edgeTable);
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
inline int GoUctPatterns<BOARD>::CodeOf8Neighbors(const BOARD& bd, SgPoint p)
{
    SG_ASSERT(bd.Line(p) > 1);
    int code = ((((((EBWCodeOfPoint(bd, p - SG_NS - SG_WE) * 3
                   + EBWCodeOfPoint(bd, p - SG_NS)) * 3
                   + EBWCodeOfPoint(bd, p - SG_NS + SG_WE)) * 3
                   + EBWCodeOfPoint(bd, p - SG_WE)) * 3
                   + EBWCodeOfPoint(bd, p + SG_WE)) * 3
                   + EBWCodeOfPoint(bd, p + SG_NS - SG_WE)) * 3
                   + EBWCodeOfPoint(bd, p + SG_NS)) * 3
                   + EBWCodeOfPoint(bd, p + SG_NS + SG_WE);
    SG_ASSERT(code >= 0);
    SG_ASSERT(code < Pattern3x3::GOUCT_POWER3_8);
    return code;
}

template<class BOARD>
inline int GoUctPatterns<BOARD>::CodeOfEdgeNeighbors(const BOARD& bd,
                                                     SgPoint p)
{
    SG_ASSERT(bd.Line(p) == 1);
    SG_ASSERT(bd.Pos(p) > 1);
    const int up = bd.Up(p);
    const int other = Pattern3x3::OtherDir(up);
    int code = (((EBWCodeOfPoint(bd, p + other) * 3
                + EBWCodeOfPoint(bd, p + up + other)) * 3
                + EBWCodeOfPoint(bd, p + up)) * 3
                + EBWCodeOfPoint(bd, p + up - other)) * 3
                + EBWCodeOfPoint(bd, p - other);
    SG_ASSERT(code >= 0);
    SG_ASSERT(code < Pattern3x3::GOUCT_POWER3_5);
    return code;
}

template<class BOARD>
inline int GoUctPatterns<BOARD>::EBWCodeOfPoint(const BOARD& bd, SgPoint p)
{
    SG_ASSERT(bd.IsValidPoint(p));
    BOOST_STATIC_ASSERT(SG_BLACK == 0);
    BOOST_STATIC_ASSERT(SG_WHITE == 1);
    BOOST_STATIC_ASSERT(SG_EMPTY == 2);
    return bd.GetColor(p);
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyCenter(SgPoint p) const
{
    return m_table[m_bd.ToPlay()][CodeOf8Neighbors(m_bd, p)].IsPattern();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyEdge(SgPoint p) const
{
    return
        m_edgeTable[m_bd.ToPlay()][CodeOfEdgeNeighbors(m_bd, p)].IsPattern();
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

    const GoUctPatternData::PatternData& pt = patternType == PATTERN_LOCAL ?
                                              GoUctLocalPatternData::gData :
                                              GoUctGlobalPatternData::gData;
    SetGammaValues(pt.m_edgePatterns, m_edgeTable);
    SetGammaValues(pt.m_centerPatterns, m_table);
}

template<class BOARD>
inline float GoUctPatterns<BOARD>::
MatchAnyCenterForGamma(SgPoint p, const SgBlackWhite toPlay) const
{
    return m_table[toPlay][CodeOf8Neighbors(m_bd, p)].GetGammaValue();
}

template<class BOARD>
inline float GoUctPatterns<BOARD>::
MatchAnyEdgeForGamma(SgPoint p, const SgBlackWhite toPlay) const
{
    return m_edgeTable[toPlay][CodeOfEdgeNeighbors(m_bd, p)].GetGammaValue();
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
	const PatternInfo& pi = m_table[m_bd.ToPlay()][CodeOf8Neighbors(m_bd, p)];
    gamma = pi.GetGammaValue();
	return pi.IsPattern();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyEdge(SgPoint p, float& gamma) const
{
	const PatternInfo& pi =
            m_edgeTable[m_bd.ToPlay()][CodeOfEdgeNeighbors(m_bd, p)];
    gamma = pi.GetGammaValue();
	return pi.IsPattern();
}

//----------------------------------------------------------------------------
#endif // GOUCT_PATTERNS_H
