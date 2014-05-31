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
    
    void PrintContext(unsigned int context, std::ostream& str)
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
        
        str << '\n';
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
    
} // namespace GoPattern12Point
