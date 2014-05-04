//----------------------------------------------------------------------------
/** @file GoPattern12Point.cpp
    See GoPattern12Point.h
*/
//----------------------------------------------------------------------------
#include "SgSystem.h"
#include "GoPattern12Point.h"

namespace {
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
    inline unsigned int
    CheckDirection(const GoBoard &bd,
                   SgPoint p, int dir,
                   unsigned int v2, unsigned int v1,
                   unsigned int occupancy, unsigned int mask,
                   SgBlackWhite toPlay, SgBlackWhite opponent)
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
} // namespace

//----------------------------------------------------------------------------

namespace GoPattern12Point {
    
    /** The 24 bit code for the pattern */
    unsigned int Context(const GoBoard& bd, SgPoint p,
                         const SgBlackWhite toPlay,
                         const SgBlackWhite opponent)
    {
        const unsigned int toPlayContext = SimpleContext(bd, p, toPlay);
        const unsigned int oppContext = SimpleContext(bd, p, opponent);
        const unsigned int extendedcontext =
        ExtendedContext(bd, p, toPlayContext ^ oppContext, toPlay, opponent);
        return extendedcontext | (toPlayContext << 8) | (oppContext << 16);
    }
    
} // namespace GoPattern12Point
