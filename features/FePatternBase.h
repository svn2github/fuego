//----------------------------------------------------------------------------
/** @file FePatternBase.h */
//----------------------------------------------------------------------------

#ifndef FE_PATTERN_BASE_H
#define FE_PATTERN_BASE_H

//----------------------------------------------------------------------------

#include <bitset>
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Axes on the board.
 Set of 3 bits, converted to int[0..7] for loops, array indices.
 */
enum PaAxFlag
{
    PA_MIRROR_X,
    PA_MIRROR_Y,
    PA_SWAP_X_Y
};

const int PA_NU_AXES = 3;

typedef std::bitset<PA_NU_AXES> PaAx;

/** PaAxSet and PA_NU_AX_SETS provide a quick and dirty way 
    to iterate over all possible symmetry axes.
    Uses the bitset constructor that takes a long long bit pattern.
    Also see FePatternBaseTest_PaAxSet_To_PaAx.

    for (PaAxSet s = 0; s < PA_NU_AX_SETS; ++s)
    {
        PaAx ax(s);
        ...
    }
*/
const unsigned int PA_NU_AX_SETS = 1 << PA_NU_AXES;

typedef unsigned int PaAxSet;
//----------------------------------------------------------------------------

/** Location and rotation information for matching a pattern on a board */
class PaSpot
{
public:
    int m_x;
    
    int m_y;
    
    PaAx m_ax;
    
    bool m_inverse;
    
    PaSpot(int x, int y, PaAx ax, bool inverse);
    
    bool operator==(const PaSpot& rhs) const;
};

inline PaSpot::PaSpot(int x, int y, PaAx ax, bool inverse)
  : m_x(x),
    m_y(y),
    m_ax(ax),
    m_inverse(inverse)
{ }

inline bool PaSpot::operator==(const PaSpot& rhs) const
{
    return m_x == rhs.m_x
    && m_y == rhs.m_y
    && m_ax == rhs.m_ax
    && m_inverse == rhs.m_inverse;
}

//----------------------------------------------------------------------------
typedef int PointIntArray[SG_MAXPOINT];
/** Constant bitmasks for fast pattern matching using 32-bit TCell */

class PaConst
{
public:
    static void Init();
    
    static int Offset(int deltaX, int deltaY);
    static int DeltaX(int offset);
    static int DeltaY(int offset);
    
    //static PointIntArray s_xCoordinate, s_yCoordinate;
};

SgPoint AXBoardToBoard(int x, int y, PaAx ax, int boardSize);

/** Convert pattern move to point on board.
 Pattern starting at spot:
 find point on board corresponding to index 'n' in pattern
 Returns 0 if out of board
 Called e.g. to get move on Board from move index in pattern
 @todo could be static, could be utility function only
 */
SgPoint PatternToPoint(int n, const PaSpot& spot, int boardSize);

//----------------------------------------------------------------------------

/** adding SHIFT to deltaX and deltaY to force numbers to be positive.
    @todo precompute arrays faster?
*/
// need to deal with both positive and negative deltas.
// They should not overlap between rows, so need double space
// compared to GoBoard.
const int SHIFT = SG_NS;
const int OFFSET = 2 * SHIFT;
const int COMBINED_SHIFT = OFFSET * SHIFT + SHIFT;

inline int PaConst::Offset(int deltaX, int deltaY)
{
    return OFFSET * deltaX + deltaY + COMBINED_SHIFT;
}

inline int PaConst::DeltaX(int offset)
{
    return offset / OFFSET - SHIFT;
}

inline int PaConst::DeltaY(int offset)
{
    return offset % OFFSET - SHIFT;
}


//----------------------------------------------------------------------------
#endif // FE_PATTERN_BASE_H

