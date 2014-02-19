//----------------------------------------------------------------------------
/** @file GoOpeningKnowledge.cpp
 See GoOpeningKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoOpeningKnowledge.h"

#include "GoBoard.h"
#include "SgDebug.h"

//----------------------------------------------------------------------------
namespace
{

bool IsEmpty3x3Box(const GoBoard& bd, SgPoint p)
{
    return bd.IsEmpty(p)
        && bd.Num8EmptyNeighbors(p) == 8;
}

/** How many  */
int ScanSide(const GoBoard& bd, SgPoint start, int direction)
{
    int dist = 0;
    for (SgPoint p = start + direction;
                 bd.Pos(p) >= 3 && IsEmpty3x3Box(bd, p);
                 p += direction
        )
        ++dist;
    return dist;
}

/** empty corner or approach move */
void Check(int x, int y, const GoBoard& bd,
           std::vector<SgPoint>& moves)
{
    const SgPoint p = SgPointUtil::Pt(x, y);
    if (IsEmpty3x3Box(bd, p))
        moves.push_back(p);
}

/** knight's move approach/shimari */
void CheckStarPoint(int x, int y, const GoBoard& bd,
                    std::vector<SgPoint>& moves)
{
    const SgPoint p = SgPointUtil::Pt(x, y);
    SG_ASSERT(bd.Line(p) == 4);
    SG_ASSERT(bd.Pos(p) == 4);
    if (  ! bd.IsEmpty(p)
        && bd.Num8EmptyNeighbors(p) == 8
        )
        for (int side = 0; side <= 1; ++side)
        {
            const SgPoint p1 = p - bd.Up(p) + 3 * bd.Side(p, side);
            SG_ASSERT(bd.Line(p1) == 3);
            SG_ASSERT(bd.Pos(p1) == 6);
            if (IsEmpty3x3Box(bd, p1))
                moves.push_back(p1);
        }
}

} // namespace

//----------------------------------------------------------------------------

std::vector<SgPoint> GoOpeningKnowledge::FindCornerMoves(const GoBoard& bd)
{
    const int size1 = bd.Size() + 1;
    std::vector<SgPoint> moves;
    for (int mirrorX = 0; mirrorX <= 1; ++mirrorX)
        for (int mirrorY = 0; mirrorY <= 1; ++mirrorY)
            for (int x = 3; x <= 5; ++x)
                for (int y = 3; y <= 5; ++y)
                {
                    if (x != 5 || y != 5)
                        Check(mirrorX ? size1 - x : x,
                              mirrorY ? size1 - y : y,
                              bd, moves);
                    if (x == 4 && y == 4)
                        CheckStarPoint(mirrorX ? size1 - x : x,
                                       mirrorY ? size1 - y : y,
                                       bd, moves);

                }
    return moves;
}

std::vector<GoOpeningKnowledge::MoveBonusPair>
GoOpeningKnowledge::FindSideExtensions(const GoBoard& bd)
{
    static int maxB = 0;
    std::vector<MoveBonusPair> extensions;
    const SgBoardConst& bc = bd.BoardConst();
    // skipping corners for now, we have forced 4-4 moves
    for (SgLineIterator it(bc, 3); it; ++it)
    {
        const SgPoint p = *it;
        if (  IsEmpty3x3Box(bd, p)
            && bc.SideExtensions().Contains(p)
            )
        {
            int leftSpace = ScanSide(bd, p, bc.Left(p));
            int rightSpace = ScanSide(bd, p, bc.Right(p));
            if (leftSpace >= 1 && rightSpace >= 1)
            {
                const int bonus = leftSpace + rightSpace
                + std::min(leftSpace, rightSpace);
                extensions.push_back(std::make_pair(p, bonus));
                if (bonus > maxB)
                {
                    SgDebug() << "bonus " << bonus << '\n';
                    maxB = bonus;
                }
            }
        }
    }
    return extensions;
}


//----------------------------------------------------------------------------
