//----------------------------------------------------------------------------
/** @file GoPattern3x3.h
    3x3 patterns on the center of a GoBoard, plus 2x3 edge patterns
    
    These patterns are used extensively in the GoUct playout policy
    and in GoUctFeatures.
    */
//----------------------------------------------------------------------------

#ifndef GO_PATTERN_3x3_H
#define GO_PATTERN_3x3_H

#include <boost/array.hpp>
#include <vector>
#include "GoBoard.h"
#include "GoPatternBase.h"
#include "SgArray.h"
#include "SgBWArray.h"
#include "SgPoint.h"

using GoPatternBase::EBWCodeOfPoint;

/** class PatternInfo holds gamma value and MoGo-pattern flag for one move. */
class PatternInfo
{
public:
    PatternInfo();
    
	void SetGammaValue(float value);
    
	float GetGammaValue() const;
    
	void SetIsPattern(bool is);
    
	bool IsPattern() const;
    
private:
    float m_gammaValue;
    
    bool m_isPattern;
};

//----------------------------------------------------------------------------

inline PatternInfo::PatternInfo()
    : m_gammaValue(0.f),
      m_isPattern(false)
{ }

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
namespace GoPattern3x3
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
    
    template<class BOARD>
    int CodeOf8Neighbors(const BOARD& bd, SgPoint p);
        
    template<class BOARD>
    int CodeOfEdgeNeighbors(const BOARD& bd, SgPoint p);

    /** Inverse mapping for Map3x3CenterCode */
    int DecodeCenterIndex(int index);
    
    /** Inverse mapping for Map2x3EdgeCode */
    int DecodeEdgeIndex(int index);
    
    void InitCenterPatternTable(SgBWArray<GoUctPatternTable>& table);
    
    void InitEdgePatternTable(SgBWArray<GoUctEdgePatternTable>& edgeTable);

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
        
    void ReduceCenterSymmetry(SgBWArray<GoUctPatternTable>& table);
    
    void ReduceEdgeSymmetry(SgBWArray<GoUctEdgePatternTable>& edgeTable);
    
    int SwapCenterColor(int code);
    
    int SwapEdgeColor(int code);
    
    void Write2x3EdgePattern(std::ostream& stream, int code);
    
    void Write3x3CenterPattern(std::ostream& stream, int code);
} // namespace GoPattern3x3

//----------------------------------------------------------------------------

template<class BOARD>
inline int GoPattern3x3::CodeOf8Neighbors(const BOARD& bd, SgPoint p)
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
    SG_ASSERT(code < GoPattern3x3::GOUCT_POWER3_8);
    return code;
}

template<class BOARD>
inline int GoPattern3x3::CodeOfEdgeNeighbors(const BOARD& bd, SgPoint p)
{
    SG_ASSERT(bd.Line(p) == 1);
    SG_ASSERT(bd.Pos(p) > 1);
    const int up = bd.Up(p);
    const int other = GoPatternBase::OtherDir(up);
    int code = (((EBWCodeOfPoint(bd, p + other) * 3
                  + EBWCodeOfPoint(bd, p + up + other)) * 3
                 + EBWCodeOfPoint(bd, p + up)) * 3
                + EBWCodeOfPoint(bd, p + up - other)) * 3
    + EBWCodeOfPoint(bd, p - other);
    SG_ASSERT(code >= 0);
    SG_ASSERT(code < GoPattern3x3::GOUCT_POWER3_5);
    return code;
}


//----------------------------------------------------------------------------

#endif // GO_PATTERN_3x3_H
