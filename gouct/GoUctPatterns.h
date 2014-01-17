//----------------------------------------------------------------------------
/** @file GoUctPatterns.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_PATTERNS_H
#define GOUCT_PATTERNS_H

#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoUctGlobalPatternData.h"
#include "GoUctLocalPatternData.h"
#include "GoUctPatternData.h"
#include "SgBoardColor.h"
#include "SgBWArray.h"
#include "SgPoint.h"
#include <cstdio>
#include <utility>
#include <string>

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
private:
	/** Match any of the center patterns, and return gamma */
	float MatchAnyCenterForGamma(SgPoint p, const SgBlackWhite toPlay) const;
    
	/** Match any of the edge patterns, and return gamma */
	float MatchAnyEdgeForGamma(SgPoint p, const SgBlackWhite toPlay) const;

	/** Copy gamma values from pt into table */
    template<class TABLE>
    void SetGammaValues(const GoUctPatternData::BWTable& pt, TABLE& table);
    
    /** 3^5 = size of edge pattern table */
    static const int GOUCT_POWER3_5 = 3 * 3 * 3 * 3 * 3;

    /** 3^8 = size of center pattern table. */
    static const int GOUCT_POWER3_8 = 3 * 3 * 3 * 3 * 3 * 3 * 3 * 3;

    /** See m_edgeTable. */
    typedef SgArray<PatternInfo, GOUCT_POWER3_5> GoUctEdgePatternTable;

    /** See m_table. */
    typedef SgArray<PatternInfo, GOUCT_POWER3_8> GoUctPatternTable;

    const BOARD& m_bd;

    /** lookup table for 8-neighborhood of a move candidate */
    SgBWArray<GoUctPatternTable> m_table;

    /** lookup table on the edge of board */
    SgBWArray<GoUctEdgePatternTable> m_edgeTable;

    static bool CheckCut1(const GoBoard& bd, SgPoint p, SgBlackWhite c,
                          int cDir, int otherDir);

    static bool CheckCut2(const GoBoard& bd, SgPoint p, const SgBlackWhite c,
                          int cDir, int otherDir);

    static bool CheckHane1(const GoBoard& bd, SgPoint p, SgBlackWhite c,
                           SgBlackWhite opp, int cDir, int otherDir);

    static int EdgeDirection(GoBoard& bd, SgPoint p, int index);

    static int EBWCodeOfPoint(const BOARD& bd, SgPoint p);

    static int FindDir(const GoBoard& bd, SgPoint p, SgBlackWhite c);

    static void InitCenterPatternTable(SgBWArray<GoUctPatternTable>& table);

    static void InitEdgePatternTable(SgBWArray<GoUctEdgePatternTable>&
                                     edgeTable);

    static bool MatchCut(const GoBoard& bd, SgPoint p);

    static bool MatchEdge(const GoBoard& bd, SgPoint p, const int nuBlack,
                          const int nuWhite);

    static bool MatchHane(const GoBoard& bd, SgPoint p, const int nuBlack,
                          const int nuWhite);


    /** Match any of the center patterns. */
    bool MatchAnyCenter(SgPoint p) const;

    /** Match any of the center patterns, and put value to gamma*/
    bool MatchAnyCenter(SgPoint p, float& gamma) const;

    /** Match any of the edge patterns. */
    bool MatchAnyEdge(SgPoint p) const;

    /** Match any of the edge patterns, and put value to gamma */
    bool MatchAnyEdge(SgPoint p, float& gammma) const;

    static bool MatchAnyPattern(const GoBoard& bd, SgPoint p);

    static int OtherDir(int dir);

    static int SetupCodedEdgePosition(GoBoard& bd, int code);

    static int SetupCodedPosition(GoBoard& bd, int code);
};

template<class BOARD>
GoUctPatterns<BOARD>::GoUctPatterns(const BOARD& bd, PatternType patternType)
    : m_bd(bd)
{
    //must be initialized before MoGo-style patterns
    InitializeGammaPatternFromProcessedData(patternType);
    InitCenterPatternTable(m_table);
    InitEdgePatternTable(m_edgeTable);
}

template<class BOARD>
GoUctPatterns<BOARD>::GoUctPatterns(const BOARD& bd)
    : m_bd(bd)
{
    InitCenterPatternTable(m_table);
    InitEdgePatternTable(m_edgeTable);
}

template<class BOARD>
bool GoUctPatterns<BOARD>::CheckHane1(const GoBoard& bd, SgPoint p,
                                      SgBlackWhite c, SgBlackWhite opp,
                                      int cDir, int otherDir)
{
    return    bd.IsColor(p + cDir, c)
           && bd.IsColor(p + cDir + otherDir, opp)
           && bd.IsColor(p + cDir - otherDir, opp)
           && bd.IsEmpty(p + otherDir)
           && bd.IsEmpty(p - otherDir)
           ;
}

template<class BOARD>
bool GoUctPatterns<BOARD>::CheckCut1(const GoBoard& bd, SgPoint p,
                                     SgBlackWhite c, int cDir, int otherDir)
{
    SG_ASSERT_BW(c);
    return bd.IsColor(p + otherDir, c)
        && bd.IsColor(p + cDir + otherDir, SgOppBW(c));
}

template<class BOARD>
bool GoUctPatterns<BOARD>::CheckCut2(const GoBoard& bd, SgPoint p,
                                     const SgBlackWhite c, int cDir,
                                     int otherDir)
{
    SG_ASSERT_BW(c);
    SG_ASSERT(bd.IsColor(p + cDir, c));
    const SgBlackWhite opp = SgOppBW(c);
    return   bd.IsColor(p - cDir, c)
          && ( (  bd.IsColor(p + otherDir, opp)
               && ! bd.IsColor(p - otherDir + cDir, c)
               && ! bd.IsColor(p - otherDir - cDir, c)
               )
             ||
               (  bd.IsColor(p - otherDir, opp)
               && ! bd.IsColor(p + otherDir + cDir, c)
               && ! bd.IsColor(p + otherDir - cDir, c)
               )
             );
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
    SG_ASSERT(code < GOUCT_POWER3_8);
    return code;
}

template<class BOARD>
inline int GoUctPatterns<BOARD>::CodeOfEdgeNeighbors(const BOARD& bd,
                                                     SgPoint p)
{
    SG_ASSERT(bd.Line(p) == 1);
    SG_ASSERT(bd.Pos(p) > 1);
    const int up = bd.Up(p);
    const int other = OtherDir(up);
    int code = (((EBWCodeOfPoint(bd, p + other) * 3
                + EBWCodeOfPoint(bd, p + up + other)) * 3
                + EBWCodeOfPoint(bd, p + up)) * 3
                + EBWCodeOfPoint(bd, p + up - other)) * 3
                + EBWCodeOfPoint(bd, p - other);
    SG_ASSERT(code >= 0);
    SG_ASSERT(code < GOUCT_POWER3_5);
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
int GoUctPatterns<BOARD>::EdgeDirection(GoBoard& bd, SgPoint p, int index)
{
    const int up = bd.Up(p);
    switch (index)
    {
        case 0:
            return OtherDir(up);
        case 1:
            return up + OtherDir(up);
        case 2:
            return up;
        case 3:
            return up - OtherDir(up);
        default:
            SG_ASSERT(index == 4);
            return - OtherDir(up);
    }
}

/** Find direction of a neighboring stone in color c */
template<class BOARD>
int GoUctPatterns<BOARD>::FindDir(const GoBoard& bd, SgPoint p,
                                  SgBlackWhite c)
{
    if (bd.IsColor(p + SG_NS, c))
        return SG_NS;
    if (bd.IsColor(p - SG_NS, c))
        return -SG_NS;
    if (bd.IsColor(p + SG_WE, c))
        return SG_WE;
    SG_ASSERT(bd.IsColor(p - SG_WE, c));
    return -SG_WE;
}

template<class BOARD>
void GoUctPatterns<BOARD>::InitEdgePatternTable(
                                  SgBWArray<GoUctEdgePatternTable>& edgeTable)
{
    GoBoard bd(5);
    const SgPoint p = SgPointUtil::Pt(1, 3);
    for (int i = 0; i < GOUCT_POWER3_5; ++i)
    {
        int count = SetupCodedEdgePosition(bd, i);
        for (SgBWIterator it; it; ++it)
        {
            bd.SetToPlay(*it);
            const bool isPattern = MatchAnyPattern(bd, p);
            edgeTable[*it][i].SetIsPattern(isPattern);
        }
        while (count-- > 0)
            bd.Undo();
    }
}

template<class BOARD>
void GoUctPatterns<BOARD>::InitCenterPatternTable(
                                          SgBWArray<GoUctPatternTable>& table)
{
    GoBoard bd(5);
    const SgPoint p = SgPointUtil::Pt(3, 3);
    for (int i = 0; i < GOUCT_POWER3_8; ++i)
    {
        int count = SetupCodedPosition(bd, i);
        for (SgBWIterator it; it; ++it)
        {
            bd.SetToPlay(*it);
            const bool isPattern = MatchAnyPattern(bd, p);
            table[*it][i].SetIsPattern(isPattern);
        }
        while (count-- > 0)
            bd.Undo();
    }
}

template<class BOARD>
bool GoUctPatterns<BOARD>::MatchCut(const GoBoard& bd, SgPoint p)
{
    if (bd.Num8EmptyNeighbors(p) > 6)
        return false;

    const int nuEmpty = bd.NumEmptyNeighbors(p);
    //cut1
    const SgEmptyBlackWhite c1 = bd.GetColor(p + SG_NS);
    if (   c1 != SG_EMPTY
       && bd.NumNeighbors(p, c1) >= 2
       && ! (bd.NumNeighbors(p, c1) == 3 && nuEmpty == 1)
       && (  CheckCut1(bd, p, c1, SG_NS, SG_WE)
          || CheckCut1(bd, p, c1, SG_NS, -SG_WE)
          )
       )
        return true;
    const SgEmptyBlackWhite c2 = bd.GetColor(p - SG_NS);
    if (  c2 != SG_EMPTY
       && bd.NumNeighbors(p, c2) >= 2
       && ! (bd.NumNeighbors(p, c2) == 3 && nuEmpty == 1)
       && (  CheckCut1(bd, p, c2, -SG_NS, SG_WE)
          || CheckCut1(bd, p, c2, -SG_NS, -SG_WE)
          )
       )
        return true;
    //cut2
    if (  c1 != SG_EMPTY
       && bd.NumNeighbors(p, c1) == 2
       && bd.NumNeighbors(p, SgOppBW(c1)) > 0
       && bd.NumDiagonals(p, c1) <= 2
       && CheckCut2(bd, p, c1, SG_NS, SG_WE)
       )
        return true;
    const SgEmptyBlackWhite c3 = bd.GetColor(p + SG_WE);
    if (  c3 != SG_EMPTY
       && bd.NumNeighbors(p, c3) == 2
       && bd.NumNeighbors(p, SgOppBW(c3)) > 0
       && bd.NumDiagonals(p, c3) <= 2
       && CheckCut2(bd, p, c3, SG_WE, SG_NS)
       )
        return true;
    return false;
}

template<class BOARD>
bool GoUctPatterns<BOARD>::MatchEdge(const GoBoard& bd, SgPoint p,
                                     const int nuBlack, const int nuWhite)
{
    const int up = bd.Up(p);
    const int side = OtherDir(up);
    const int nuEmpty = bd.NumEmptyNeighbors(p);
    const SgEmptyBlackWhite upColor = bd.GetColor(p + up);
    // edge1
    if (  nuEmpty > 0
       && (nuBlack > 0 || nuWhite > 0)
       && upColor == SG_EMPTY
       )
    {
        const SgEmptyBlackWhite c1 = bd.GetColor(p + side);
        if (c1 != SG_EMPTY && bd.GetColor(p + side + up) == SgOppBW(c1))
            return true;
        const SgEmptyBlackWhite c2 = bd.GetColor(p - side);
        if (c2 != SG_EMPTY && bd.GetColor(p - side + up) == SgOppBW(c2))
            return true;
    }

    // edge2
    if (  upColor != SG_EMPTY
       && (  (upColor == SG_BLACK && nuBlack == 1 && nuWhite > 0)
          || (upColor == SG_WHITE && nuWhite == 1 && nuBlack > 0)
          )
       )
        return true;

    const SgBlackWhite toPlay = bd.ToPlay();
    // edge3
    if (  upColor == toPlay
       && bd.NumDiagonals(p, SgOppBW(upColor)) > 0
       )
        return true;

    // edge4
    if (  upColor == SgOppBW(toPlay)
       && bd.NumNeighbors(p, upColor) <= 2
       && bd.NumDiagonals(p, toPlay) > 0
       )
    {
        if (  bd.GetColor(p + side + up) == toPlay
           && bd.GetColor(p + side) != upColor
           )
            return true;
        if (  bd.GetColor(p - side + up) == toPlay
           && bd.GetColor(p - side) != upColor
           )
            return true;
    }
    // edge5
    if (  upColor == SgOppBW(toPlay)
       && bd.NumNeighbors(p, upColor) == 2
       && bd.NumNeighbors(p, toPlay) == 1
       )
    {
        if (  bd.GetColor(p + side + up) == toPlay
           && bd.GetColor(p + side) == upColor
           )
            return true;
        if (  bd.GetColor(p - side + up) == toPlay
           && bd.GetColor(p - side) == upColor
           )
            return true;
    }
    return false;
}

template<class BOARD>
bool GoUctPatterns<BOARD>::MatchHane(const GoBoard& bd, SgPoint p,
                                     const int nuBlack, const int nuWhite)
{
    const int nuEmpty = bd.NumEmptyNeighbors(p);
    if (nuEmpty < 2 || nuEmpty > 3)
        return false;
    if (  (nuBlack < 1 || nuBlack > 2)
       && (nuWhite < 1 || nuWhite > 2)
       )
        return false;
    if (nuEmpty == 2) // hane3 pattern
    {
        if (nuBlack == 1 && nuWhite == 1)
        {
            const int dirB = FindDir(bd, p, SG_BLACK);
            const int dirW = FindDir(bd, p, SG_WHITE);
            if (! bd.IsEmpty(p + dirB + dirW))
                return true;
        }
    }
    else if (nuEmpty == 3) // hane2 or hane4
    {
        SG_ASSERT(nuBlack + nuWhite == 1);
        const SgBlackWhite col = (nuBlack == 1) ? SG_BLACK : SG_WHITE;
        const SgBlackWhite opp = SgOppBW(col);
        const int dir = FindDir(bd, p, col);
        const int otherDir = OtherDir(dir);
        if (  bd.IsEmpty(p + dir + otherDir)
           && bd.IsColor(p + dir - otherDir, opp)
           )
            return true; // hane2
        if (  bd.IsEmpty(p + dir - otherDir)
           && bd.IsColor(p + dir + otherDir, opp)
           )
            return true; // hane2
        if (bd.ToPlay() == opp)
        {
            const SgEmptyBlackWhite c1 = bd.GetColor(p + dir + otherDir);
            if (c1 != SG_EMPTY)
            {
                const SgEmptyBlackWhite c2 =
                bd.GetColor(p + dir - otherDir);
                if (SgOppBW(c1) == c2)
                    return true; // hane4
            }
        }
    }

    // hane1 pattern
    const int nuBlackDiag = bd.NumDiagonals(p, SG_BLACK);
    if (  nuBlackDiag >= 2
       && nuWhite > 0
       && (  CheckHane1(bd, p, SG_WHITE, SG_BLACK, SG_NS, SG_WE)
          || CheckHane1(bd, p, SG_WHITE, SG_BLACK, -SG_NS, SG_WE)
          || CheckHane1(bd, p, SG_WHITE, SG_BLACK, SG_WE, SG_NS)
          || CheckHane1(bd, p, SG_WHITE, SG_BLACK, -SG_WE, SG_NS)
          )
       )
        return true;
    const int nuWhiteDiag = bd.NumDiagonals(p, SG_WHITE);
    if (  nuWhiteDiag >= 2
       && nuBlack > 0
       && (  CheckHane1(bd, p, SG_BLACK, SG_WHITE, SG_NS, SG_WE)
          || CheckHane1(bd, p, SG_BLACK, SG_WHITE, -SG_NS, SG_WE)
          || CheckHane1(bd, p, SG_BLACK, SG_WHITE, SG_WE, SG_NS)
          || CheckHane1(bd, p, SG_BLACK, SG_WHITE, -SG_WE, SG_NS)
          )
       )
        return true;
    return false;
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

/** Procedural matching function - used to initialize the table. */
template<class BOARD>
bool GoUctPatterns<BOARD>::MatchAnyPattern(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT(bd.IsEmpty(p));
    const int nuBlack = bd.NumNeighbors(p, SG_BLACK);
    const int nuWhite = bd.NumNeighbors(p, SG_WHITE);

    // Quick refutation using the incremental neighbor counts of the board
    // All patterns have at least one adjacent stone
    if (nuBlack == 0 && nuWhite == 0)
        return false;

    // Filter edge moves on (1,1) points in corners
    if (bd.Pos(p) == 1)
        return false;
    if (bd.Line(p) == 1)
        return MatchEdge(bd, p, nuBlack, nuWhite);
    else // Center
        return   MatchHane(bd, p, nuBlack, nuWhite)
              || MatchCut(bd, p);
}

template<class BOARD>
inline int GoUctPatterns<BOARD>::OtherDir(int dir)
{
    if (dir == SG_NS || dir == -SG_NS)
        return SG_WE;
    return SG_NS;
}

template<class BOARD>
int GoUctPatterns<BOARD>::SetupCodedEdgePosition(GoBoard& bd, int code)
{
    const SgPoint p = SgPointUtil::Pt(1, 3);
    int count = 0;
    for (int i = 4; i >= 0; --i) // decoding gives points in reverse order
    {
        const SgPoint nb = p + EdgeDirection(bd, p, i);
        int c = code % 3;
        code /= 3;
        if (c != SG_EMPTY)
        {
            ++count;
            bd.Play(nb, c);
        }
    }
    return count;
}

template<class BOARD>
int GoUctPatterns<BOARD>::SetupCodedPosition(GoBoard& bd, int code)
{
    const SgPoint p = SgPointUtil::Pt(3, 3);
    int count = 0;
    for (int i = 7; i >= 0; --i) // decoding gives points in reverse order
    {
        const SgPoint nb = p + SgNb8Iterator::Direction(i);
        int c = code % 3;
        code /= 3;
        if (c != SG_EMPTY)
        {
            ++count;
            bd.Play(nb, c);
        }
    }
    return count;
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
inline bool GoUctPatterns<BOARD>::MatchAnyCenter(SgPoint p, float& gamma) const
{
	const PatternInfo& pi = m_table[m_bd.ToPlay()][CodeOf8Neighbors(m_bd, p)];
    gamma = pi.GetGammaValue();
	return pi.IsPattern();
}

template<class BOARD>
inline bool GoUctPatterns<BOARD>::MatchAnyEdge(SgPoint p, float& gamma) const
{
	const PatternInfo& pi = m_edgeTable[m_bd.ToPlay()][CodeOfEdgeNeighbors(m_bd, p)];
    gamma = pi.GetGammaValue();
	return pi.IsPattern();
}

//----------------------------------------------------------------------------
#endif // GOUCT_PATTERNS_H
