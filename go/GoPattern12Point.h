//----------------------------------------------------------------------------
/** @file GoPattern12Point.h */
//----------------------------------------------------------------------------

#ifndef GO_PATTERN_12_POINT_H
#define GO_PATTERN_12_POINT_H

#include "GoBoard.h"
#include "GoEvalArray.h"

//----------------------------------------------------------------------------
namespace GoPattern12Point {
    
    const unsigned int KO_BIT = 1U << 24;
    const unsigned int ATARI_BIT = 1U << 25;

    /** The 24 bit code for the pattern */
    unsigned int Context(const GoBoard& bd, SgPoint p,
                         const SgBlackWhite toPlay,
                         const SgBlackWhite opponent);

    /** Print a pattern given its pattern code.
     3 typical examples:
     
     Example 1: the all empty pattern. @b shows its black turn (all patterns
     are black turn). Because the four neighbor points of @b are empty, the
     pattern shows the 4 points one step further in the four directions.
     They are all empty in this example.
     [  ]
     [  ][  ][  ]
     [  ][  ][@b][  ][  ]
     [  ][  ][  ]
     [  ]
     
     Example 2: this pattern has only 12 points.
     Since b3, west of @b, is taken by black, the extra 2 bits are now
     used to encode the number of liberties (3) of this black block.
     
     [##], two steps to the north, encodes the border.
     [##]
     [  ][  ][b ]
     [b3][@b][  ][b ]
     [b ][  ][b ]
     [w ]
     
     Example 3: an edge pattern. It is also a capture since w1 is a
     white block with 1 liberty.
     [##][b1][b ]
     [##][##][@b][w1]
     [##][  ][b ]
     [  ]
     */
    void PrintContext(unsigned int context, std::ostream& stream);

} // namespace GoPattern12Point

//----------------------------------------------------------------------------

#endif // GO_PATTERN_12_POINT_H
