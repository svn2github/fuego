//----------------------------------------------------------------------------
/** @file GoBoardUtil.cpp
    See GoBoardUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardUtil.h"

#include <iomanip>
#include <sstream>
#include <string>
#include "GoBoard.h"
#include "GoModBoard.h"
#include "GoMoveExecutor.h"
#include "GoSafetySolver.h"
#include "SgDebug.h"
#include "SgException.h"
#include "SgNbIterator.h"
#include "SgProp.h"

using namespace std;
using SgPropUtil::PointToSgfString;

//----------------------------------------------------------------------------

void GoBoardUtil::AddNeighborBlocksOfColor(const GoBoard& bd, SgPoint p,
                                           SgBlackWhite c,
                                           SgList<SgPoint>& neighbors)
{
    if (bd.IsColor(p - SG_NS, c))
        neighbors.Include(bd.Anchor(p - SG_NS));
    if (bd.IsColor(p - SG_WE, c))
        neighbors.Include(bd.Anchor(p - SG_WE));
    if (bd.IsColor(p + SG_WE, c))
        neighbors.Include(bd.Anchor(p + SG_WE));
    if (bd.IsColor(p + SG_NS, c))
        neighbors.Include(bd.Anchor(p + SG_NS));
}

void GoBoardUtil::AddWall(GoBoard& bd,
                          SgBlackWhite color,
                          SgPoint start,
                          int length,
                          int direction)
{
    for (SgPoint p = start; length > 0; --length)
    {
        bd.Play(p, color);
        p += direction;
    }
}

void GoBoardUtil::AdjacentStones(const GoBoard& bd, SgPoint point,
                                 SgList<SgPoint>* stones)
{
    SG_ASSERT(stones);
    SG_ASSERT(bd.IsValidPoint(point));
    SG_ASSERT(bd.Occupied(point));
    const SgBlackWhite other = SgOppBW(bd.GetStone(point));
    SgPoint a[SG_MAXPOINT];
    int n = 0;
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (GoBoard::StoneIterator it(bd, point); it; ++it)
    {
        if (bd.NumNeighbors(*it, other) > 0)
        {
            SgPoint p = *it;
            if (bd.IsColor(p - SG_NS, other) && mark.NewMark(p - SG_NS))
                a[n++] = p - SG_NS;
            if (bd.IsColor(p - SG_WE, other) && mark.NewMark(p - SG_WE))
                a[n++] = p - SG_WE;
            if (bd.IsColor(p + SG_WE, other) && mark.NewMark(p + SG_WE))
                a[n++] = p + SG_WE;
            if (bd.IsColor(p + SG_NS, other) && mark.NewMark(p + SG_NS))
                a[n++] = p + SG_NS;
        }
    };
    stones->SetTo(a, n);
}

void GoBoardUtil::AdjacentBlocks(const GoBoard& bd, SgPoint p, int maxLib,
                                 SgList<SgPoint>* blocks)
{
    SG_ASSERT(blocks);
    SgPoint a[SG_MAXPOINT];
    int n = bd.AdjacentBlocks(p, maxLib, a, SG_MAXPOINT);
    blocks->SetTo(a, n);
}

void GoBoardUtil::BlocksAdjacentToPoints(const GoBoard& bd,
                                         const SgList<SgPoint>& points,
                                         SgBlackWhite c,
                                         SgList<SgPoint>* blocks)
{
    SG_ASSERT(blocks);
    // Mark all points to avoid n^2 algorithm.
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (SgListIterator<SgPoint> it1(points); it1; ++it1)
        mark.Include(*it1);
    // Add the anchor of each adjacent block to the list of blocks.
    SgPoint a[SG_MAXPOINT];
    int n = 0;
    for (SgListIterator<SgPoint> it2(points); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.NumNeighbors(p, c) > 0)
        {
            if (bd.IsColor(p - SG_NS, c)
                && mark.NewMark(bd.Anchor(p - SG_NS)))
                a[n++] = bd.Anchor(p - SG_NS);
            if (bd.IsColor(p - SG_WE, c)
                && mark.NewMark(bd.Anchor(p - SG_WE)))
                a[n++] = bd.Anchor(p - SG_WE);
            if (bd.IsColor(p + SG_WE, c)
                && mark.NewMark(bd.Anchor(p + SG_WE)))
                a[n++] = bd.Anchor(p + SG_WE);
            if (bd.IsColor(p + SG_NS, c)
                && mark.NewMark(bd.Anchor(p + SG_NS)))
                a[n++] = bd.Anchor(p + SG_NS);
        }
    }
    blocks->SetTo(a, n);
}

void GoBoardUtil::BlocksAdjacentToPoints(const GoBoard& bd,
                                         const SgPointSet& points,
                                         SgBlackWhite c,
                                         SgList<SgPoint>* blocks)
{
    // exact copy from list version above
    SG_ASSERT(blocks);
    // Mark all points to avoid n^2 algorithm.
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (SgSetIterator it1(points); it1; ++it1)
        mark.Include(*it1);
    // Add the anchor of each adjacent block to the list of blocks.
    SgPoint a[SG_MAXPOINT];
    int n = 0;
    for (SgSetIterator it2(points); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.NumNeighbors(p, c) > 0)
        {
            if (bd.IsColor(p - SG_NS, c)
                && mark.NewMark(bd.Anchor(p - SG_NS)))
                a[n++] = bd.Anchor(p - SG_NS);
            if (bd.IsColor(p - SG_WE, c)
                && mark.NewMark(bd.Anchor(p - SG_WE)))
                a[n++] = bd.Anchor(p - SG_WE);
            if (bd.IsColor(p + SG_WE, c)
                && mark.NewMark(bd.Anchor(p + SG_WE)))
                a[n++] = bd.Anchor(p + SG_WE);
            if (bd.IsColor(p + SG_NS, c)
                && mark.NewMark(bd.Anchor(p + SG_NS)))
                a[n++] = bd.Anchor(p + SG_NS);
        }
    }
    blocks->SetTo(a, n);
}

bool GoBoardUtil::BlockIsAdjacentTo(const GoBoard& bd, SgPoint block,
                                    const SgPointSet& walls)
{
    for (GoBoard::StoneIterator it(bd, block); it; ++it)
    {
        if (   walls.Contains((*it) + SG_NS)
            || walls.Contains((*it) - SG_NS)
            || walls.Contains((*it) + SG_WE)
            || walls.Contains((*it) - SG_WE)
           )
            return true;
    }
    return false;
}

GoSetup GoBoardUtil::CurrentPosSetup(const GoBoard& bd)
{
    GoSetup setup;
    setup.m_player = bd.ToPlay();
    for (GoBoard::Iterator it2(bd); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.Occupied(p))
            setup.m_stones[bd.GetColor(p)].Append(p);
    }
    return setup;
}

void GoBoardUtil::DumpBoard(const GoBoard& bd, std::ostream& out)
{
    const int size = bd.Size();
    out << bd;
    if (bd.MoveNumber() == 0)
        return;
    out << "(;SZ[" << size << "]\n";
    const GoSetup& setup = bd.Setup();
    if (! setup.IsEmpty())
    {
        for (SgBWIterator it; it; ++it)
        {
            SgBlackWhite c = *it;
            int stoneNumber = 0;
            out << (c == SG_BLACK ? "AB" : "AW");
            for (SgPointSList::Iterator it2(setup.m_stones[c]); it2; ++it2)
            {
                SgPoint p = *it2;
                ++stoneNumber;
                out << '[' << PointToSgfString(p, size, SG_PROPPOINTFMT_GO)
                    << ']';
                if (stoneNumber % 10 == 0)
                    out << '\n';
            }
            out << '\n';
        }
        out << "PL[" << (setup.m_player == SG_BLACK ? 'B' : 'W') << "]\n";
    }
    int moveNumber = 0;
    for (int i = 0; i < bd.MoveNumber(); ++i)
    {
        GoPlayerMove move = bd.Move(i);
        out << ';';
        out << (move.Color() == SG_BLACK ? "B" : "W");
        ++moveNumber;
        out << '[' << PointToSgfString(move.Point(), size, SG_PROPPOINTFMT_GO)
            << ']';
        if (moveNumber % 10 == 0)
            out << '\n';
    }
    out << ")\n";
}

void GoBoardUtil::ExpandToBlocks(const GoBoard& board, SgPointSet& pointSet)
{
    // @todo faster to use GoBoard::StoneIterator in GoBoard?
    SG_ASSERT(pointSet.SubsetOf(board.Occupied()));
    int size = board.Size();
    for (SgBlackWhite color = SG_BLACK; color <= SG_WHITE; ++color)
    {
        SgPointSet set = pointSet & board.All(color);
        bool change(true);
        while (change)
        {
            change = false;
            SgPointSet next = set | (set.Border(size) & board.All(color));
            if (next != set)
            {
                change = true;
                set = next;
            }
        }
        pointSet |= set;
    }
}

void GoBoardUtil::DiagonalsOfColor(const GoBoard& bd, SgPoint p, int c,
                                   SgList<SgPoint>* diagonals)
{
    diagonals->Clear();
    if (bd.IsColor(p - SG_NS - SG_WE, c))
        diagonals->Append(p - SG_NS - SG_WE);
    if (bd.IsColor(p - SG_NS + SG_WE, c))
        diagonals->Append(p - SG_NS + SG_WE);
    if (bd.IsColor(p + SG_NS - SG_WE, c))
        diagonals->Append(p + SG_NS - SG_WE);
    if (bd.IsColor(p + SG_NS + SG_WE, c))
        diagonals->Append(p + SG_NS + SG_WE);
}

bool GoBoardUtil::EndOfGame(const GoBoard& bd)
{
    SgBlackWhite toPlay = bd.ToPlay();
    GoPlayerMove passToPlay(toPlay, SG_PASS);
    GoPlayerMove passOpp(SgOppBW(toPlay), SG_PASS);
    int moveNumber = bd.MoveNumber();
    if (bd.Rules().TwoPassesEndGame())
    {
        return moveNumber >= 2
            && bd.Move(moveNumber - 1) == passOpp
            && bd.Move(moveNumber - 2) == passToPlay;
    }
    else // Three passes in a row end the game.
    {
        return moveNumber >= 3
            && bd.Move(moveNumber - 1) == passOpp
            && bd.Move(moveNumber - 2) == passToPlay
            && bd.Move(moveNumber - 3) == passOpp;
    }
}


bool GoBoardUtil::GenerateIfLegal(const GoBoard& bd, SgPoint move,
                                  SgList<SgPoint>* moves)
{
    if (bd.IsLegal(move))
    {
        if (moves)
            moves->Include(move);
        /* */ return true; /* */
    }
    return false;
}

void GoBoardUtil::GetCoordString(SgMove p, std::string* s, int boardSize)
{
    SG_ASSERT(s);
    SG_ASSERT(p != SG_NULLMOVE);
    if (p == SG_PASS)
        *s = "Pass";
    else if (p == SG_COUPONMOVE)
        *s = "Coupon";
    else
    {
        int col = SgPointUtil::Col(p);
        int row = SgPointUtil::Row(p);
        if (9 <= col)
            ++col; // skip 'I'
        ostringstream o;
        o << static_cast<char>('A' + col - 1) << (boardSize + 1 - row);
        *s = o.str();
    }
}

bool GoBoardUtil::HasAdjacentBlocks(const GoBoard& bd, SgPoint p,
                                    int maxLib)
{
    SG_ASSERT(bd.Occupied(p));
    const SgBlackWhite other = SgOppBW(bd.GetStone(p));
    for (GoBoard::StoneIterator stone(bd, p); stone; ++stone)
        for (SgNb4Iterator nb(*stone); nb; ++nb)
            if (bd.IsColor(*nb, other) && bd.AtMostNumLibs(*nb, maxLib))
                return true;
    return false;
}

bool GoBoardUtil::IsHandicapPoint(SgGrid size, SgGrid col, SgGrid row)
{
    SgGrid line1;
    SgGrid line3;
    if (size < 9)
        return false;
    if (size <= 11)
    {
        line1 = 3;
        line3 = size - 2;
    }
    else
    {
        line1 = 4;
        line3 = size - 3;
    }
    if (size > 11 && size % 2 != 0) // mark mid points
    {
        SgGrid line2 = size / 2 + 1;
        return (row == line1 || row == line2 || row == line3)
            && (col == line1 || col == line2 || col == line3);
    }
    else
        return (row == line1 || row == line3)
            && (col == line1 || col == line3);
}

bool GoBoardUtil::IsSimpleEyeOfBlock(const GoBoard& bd, SgPoint lib,
                                     SgPoint blockAnchor,
                                     const SgList<SgPoint>& eyes)
{
    SgBlackWhite color = bd.GetStone(blockAnchor);
    // need IsColor test for nbs because might be off board.
    if (bd.IsColor(lib - SG_NS, color)
        && bd.Anchor(lib - SG_NS) != blockAnchor)
        return false;
    if (bd.IsColor(lib + SG_NS, color)
        && bd.Anchor(lib + SG_NS) != blockAnchor)
        return false;
    if (bd.IsColor(lib - SG_WE, color)
        && bd.Anchor(lib - SG_WE) != blockAnchor)
        return false;
    if (bd.IsColor(lib + SG_WE, color)
        && bd.Anchor(lib + SG_WE) != blockAnchor)
        return false;
    int nuForFalse = (bd.Line(lib) == 1) ? 1 : 2;
    // no need to check diagonals for same block since direct neighbors are.
    for (SgNb4DiagIterator it(lib); it; ++it)
    {
        SgPoint nb(*it);
        if (! bd.IsBorder(nb) && ! bd.IsColor(nb, color)
            && ! eyes.Contains(nb))
            if (--nuForFalse <= 0)
                return false;
    }
    return true;
}

bool GoBoardUtil::IsSnapback(const GoBoard& constBd, SgPoint p)
{
    SG_ASSERT(constBd.IsValidPoint(p));
    SG_ASSERT(constBd.Occupied(p));

    bool snapback = false;
    if (constBd.IsSingleStone(p) && constBd.InAtari(p))
    {
        const SgPoint lib = constBd.TheLiberty(p);
        GoModBoard mbd(constBd);
        GoBoard& bd = mbd.Board();
        const bool isLegal =
            GoBoardUtil::PlayIfLegal(bd, lib, SgOppBW(bd.GetStone(p)));
        if (   isLegal
            && bd.InAtari(lib)
            && ! bd.IsSingleStone(lib)
           )
            snapback = true;
        if (isLegal)
            bd.Undo();
    }
    return snapback;
}

bool GoBoardUtil::ManySecondaryLibs(const GoBoard& bd, SgPoint block)
{
    // was always 8, not enough for loose ladder in CAPTURES.SGB, problem 2
    // one liberty can have 3 new secondary, total of 4 which are taken by
    // opp. move.
    // current value is just a guess, experiment.
    const int limit = 9;
    static SgMarker m;
    m.Clear();
    int nu = 0;
    for (GoBoard::LibertyIterator it(bd, block); it; ++it)
    {
        SgPoint p(*it);
        if (m.NewMark(p))
            if (++nu >= limit)
                return true;
        for (SgNb4Iterator itn(p); itn; ++itn)
        {
            if (bd.IsEmpty(*itn) && m.NewMark(*itn))
                if (++nu >= limit)
                    return true;
        }
    }
    return (nu >= limit);
}

void GoBoardUtil::NeighborsOfColor(const GoBoard& bd, SgPoint p, int c,
                                   SgList<SgPoint>* neighbors)
{
    neighbors->Clear();
    if (bd.IsColor(p - SG_NS, c))
        neighbors->Append(p - SG_NS);
    if (bd.IsColor(p - SG_WE, c))
        neighbors->Append(p - SG_WE);
    if (bd.IsColor(p + SG_WE, c))
        neighbors->Append(p + SG_WE);
    if (bd.IsColor(p + SG_NS, c))
        neighbors->Append(p + SG_NS);
}

bool GoBoardUtil::PassWins(const GoBoard& bd, SgBlackWhite toPlay)
{
    if (toPlay != bd.ToPlay())
        // Not defined if non-alternating moves
        return false;
    if (! bd.Rules().CaptureDead() || bd.Rules().JapaneseScoring())
        return false;
    if (bd.GetLastMove() != SG_PASS)
        return false;
    float komi = bd.Rules().Komi().ToFloat();
    float score = GoBoardUtil::TrompTaylorScore(bd, komi);
    if ((score > 0 && toPlay == SG_BLACK)
        || (score < 0 && toPlay == SG_WHITE))
        return true;
    return false;
}

bool GoBoardUtil::PlayIfLegal(GoBoard& bd, SgPoint p, SgBlackWhite player)
{
    if (p != SG_PASS && p != SG_COUPONMOVE)
    {
        if (! bd.IsEmpty(p))
            return false;
        if (! bd.Rules().AllowSuicide() && bd.IsSuicide(p, player))
            return false;
    }
    bd.Play(p, player);
    if (bd.LastMoveInfo(isIllegal))
    {
        bd.Undo();
        return false;
    }
    return true;
}

void GoBoardUtil::ReduceToAnchors(const GoBoard& bd, SgList<SgPoint>* stones)
{
    SG_ASSERT(stones);
    SgList<SgPoint> result;
    for (SgListIterator<SgPoint> stone(*stones); stone; ++stone)
        if (bd.Occupied(*stone))
            result.Insert(bd.Anchor(*stone));
    // Trick to avoid list copy needed by: *stones = result.
    stones->SwapWith(&result);
}

void GoBoardUtil::ReduceToAnchors(const GoBoard& bd,
                                  const SgList<SgPoint>& stones,
                                  SgSList<SgPoint,SG_MAXPOINT>& anchors)
{
    anchors.Clear();
    for (SgListIterator<SgPoint> stone(stones); stone; ++stone)
    {
        if (bd.Occupied(*stone))
        {
            for (int i = anchors.Length() - 1; i >= 0; --i)
            {
                if (bd.Anchor(*stone) == anchors[i])
                    goto LB_RTA;
            }
            anchors.Append(bd.Anchor(*stone));
        }
    LB_RTA:
        continue;
    }
}

void GoBoardUtil::RegionCode(const GoBoard& bd, const SgList<SgPoint>& region,
                             SgHashCode* c)
{
    c->Clear();
    for (SgListIterator<SgPoint> it(region); it; ++it)
    {
        SgPoint p = *it;
        if (bd.Occupied(p))
            SgHashUtil::XorZobrist(*c,
                                   p + (bd.GetStone(p) - 1) * SG_MAXPOINT);
    }
}

bool GoBoardUtil::RemainingChineseHandicap(const GoBoard& bd)
{
    const GoRules& rules = bd.Rules();
    return (! rules.JapaneseHandicap()
            && rules.Handicap() > bd.TotalNumStones(SG_BLACK));
}

float GoBoardUtil::ScoreEndPosition(const GoBoard& bd, float komi,
                                    bool noCheck)
{
    GoModBoard mbd(bd);
    GoSafetySolver solver(mbd); // todo: safety solver should take const board
    SgBWSet safe;
    solver.FindSafePoints(&safe);
    return ScoreEndPosition(bd, komi, safe, noCheck);
}

float GoBoardUtil::ScoreSimpleEndPosition(const GoBoard& bd, float komi,
                                          bool noCheck)
{
    int score = 0;
    for (GoBoard::Iterator it(bd); it; ++it)
        score += ScorePoint(bd, *it, noCheck);
    return (score - komi);
}

void GoBoardUtil::SharedLiberties(const GoBoard& bd, SgPoint block1,
                                  SgPoint block2, SgList<SgPoint>* sharedLibs)
{
    SG_ASSERT(sharedLibs);
    SG_ASSERT(bd.Occupied(block1));
    SG_ASSERT(bd.Occupied(block2));
    block1 = bd.Anchor(block1);
    block2 = bd.Anchor(block2);
    sharedLibs->Clear();
    for (GoBoard::LibertyIterator libIter(bd, block1); libIter; ++libIter)
    {
        SgPoint lib = *libIter;
        if (bd.IsLibertyOfBlock(lib, block2))
            sharedLibs->Append(lib);
    }
}

void GoBoardUtil::SharedLibertyBlocks(const GoBoard& bd, SgPoint anchor,
                                      int maxLib, SgList<SgPoint>* blocks)
{
    SG_ASSERT(blocks);
    // Mark all points and previous blocks.
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (GoBoard::StoneIterator it1(bd, anchor); it1; ++it1)
        mark.Include(*it1);
    for (SgListIterator<SgPoint> it(*blocks); it; ++it)
    {
        SgPoint a = *it;
        for (GoBoard::StoneIterator it(bd, a); it; ++it)
            mark.Include(*it);
    }
    SgBlackWhite c = bd.GetStone(anchor);
    // Add the anchor of each adjacent block to the list of blocks.
    for (GoBoard::LibertyIterator it2(bd, anchor); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.NumNeighbors(p, c) > 0)
        {
            if (bd.IsColor(p - SG_NS, c) && mark.NewMark(bd.Anchor(p - SG_NS))
                && bd.AtMostNumLibs(p - SG_NS, maxLib))
                blocks->Append(bd.Anchor(p - SG_NS));
            if (bd.IsColor(p - SG_WE, c) && mark.NewMark(bd.Anchor(p - SG_WE))
                && bd.AtMostNumLibs(p - SG_WE, maxLib))
                blocks->Append(bd.Anchor(p - SG_WE));
            if (bd.IsColor(p + SG_WE, c) && mark.NewMark(bd.Anchor(p + SG_WE))
                && bd.AtMostNumLibs(p + SG_WE, maxLib))
                blocks->Append(bd.Anchor(p + SG_WE));
            if (bd.IsColor(p + SG_NS, c) && mark.NewMark(bd.Anchor(p + SG_NS))
                && bd.AtMostNumLibs(p + SG_NS, maxLib))
                blocks->Append(bd.Anchor(p + SG_NS));
        }
    }
}

void GoBoardUtil::UndoAll(GoBoard& bd)
{
    while (bd.CanUndo())
        bd.Undo();
}

bool GoBoardUtil::AtLeastTwoSharedLibs(const GoBoard& bd, SgPoint block1,
                                       SgPoint block2)
{
    SG_ASSERT(bd.Occupied(block1));
    SG_ASSERT(bd.Occupied(block2));
    //block1 = bd.Anchor(block1);
    block2 = bd.Anchor(block2);
    bool fHasOneShared = false;
    for (GoBoard::LibertyIterator libIter(bd, block1); libIter; ++libIter)
    {
        if (bd.IsLibertyOfBlock(*libIter, block2))
        {
            if (fHasOneShared)
                return true;
            fHasOneShared = true;
        }
    }
    return false;
}

void GoBoardUtil::TestForChain(GoBoard& bd, SgPoint block, SgPoint block2,
                               SgPoint lib, SgList<SgPoint>* extended)
{
    if (AtLeastTwoSharedLibs(bd, block, block2))
        extended->Append(block);
    else // protected lib.
    {
        GoRestoreToPlay r(bd);
        bd.SetToPlay(SgOppBW(bd.GetStone(block)));
        if (MoveNotLegalOrAtari(bd, lib))
            extended->Append(block);
    }
}

bool GoBoardUtil::HasStonesOfBothColors(const GoBoard& bd,
                                        const SgList<SgPoint>& stones)
{
    SgBWArray<bool> has(false);
    for (SgListIterator<SgPoint> it(stones); it; ++it)
    {
        if (bd.Occupied(*it))
        {
            SgBlackWhite color(bd.GetStone(*it));
            has[color] = true;
            if (has[SgOppBW(color)])
                return true;
        }
    }
    return false;
}

bool GoBoardUtil::MoveNotLegalOrAtari(GoBoard& bd, SgPoint move)
{
    GoMoveExecutor execute(bd, move);
    return (! execute.IsLegal() || bd.InAtari(move));
}

bool GoBoardUtil::MoveLegalAndNotAtari(GoBoard& bd, SgPoint move)
{
    GoMoveExecutor execute(bd, move);
    return (execute.IsLegal() && ! bd.InAtari(move));
}

bool GoBoardUtil::IsCapturingMove(const GoBoard& bd, SgPoint p)
{
    SgPoint anchors[4 + 1];
    bd.NeighborBlocks(p, bd.Opponent(), 1, anchors);
    // check if at least one neighbor in atari exists
    return anchors[0] != SG_ENDPOINT;
}

template<class BOARD>
bool GoBoardUtil::SelfAtari(const BOARD& bd, SgPoint p, int& numStones,
                            bool countStones)
{
    ASSERT(bd.IsEmpty(p));
    // No self-atari, enough liberties
    if (bd.NumEmptyNeighbors(p) >= 2)
        return false;
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opp = SgOppBW(toPlay);
    SgPoint lib = SG_NULLPOINT;
    bool hasOwnNb = false;
    bool hasCapture = false;
    for (SgNb4Iterator it(p); it; ++it)
    {
        const SgPoint nb = *it;
        const SgBlackWhite nbColor = bd.GetColor(nb);
        if (nbColor == SG_EMPTY)
        {
            if (lib == SG_NULLPOINT)
                lib = nb;
            else if (lib != nb)
                return false;
        }
        else if (nbColor == toPlay) // own stones
        {
            if (bd.NumLiberties(nb) > 2)
                return false;
            else // check block's liberties other than p
                for (GoBoard::LibertyIterator it(bd, nb); it; ++it)
                {
                    if (*it != p)
                    {
                        if (lib == SG_NULLPOINT)
                            lib = *it;
                        else if (lib != *it)
                            return false;
                    }
                }
            hasOwnNb = true;
        }
        else if (nbColor == opp) // opponent stones - count as lib if in atari
        {
            if (bd.InAtari(nb))
            {
                if (lib == SG_NULLPOINT)
                {
                    lib = *it;
                    hasCapture = true;
                }
                else if (lib != *it)
                    return false;
            }
        }
    }

    if (lib == SG_NULLPOINT) // suicide
        return false;
    if (! hasOwnNb && hasCapture) // ko-type capture, OK
         return false;
    if (hasOwnNb && hasCapture) // check if we gained other liberties
    {
        // lib == one of the captured stones.
       SgPoint anchors[4 + 1];
       bd.NeighborBlocks(p, toPlay, 1, anchors);
       ASSERT(bd.IsColor(lib, opp));
       for (GoBoard::StoneIterator it(bd, lib); it; ++it)
       {
           if (*it != lib && IsNeighborOfSome(bd, *it, anchors, toPlay))
               return false;
       }
    }
    if (countStones)
    {
        numStones = 1;
        if (hasOwnNb)
        {
            SgPoint anchors[4 + 1];
            bd.NeighborBlocks(p, toPlay, 1, anchors);
            for (int i = 0; anchors[i] != SG_ENDPOINT; ++i)
                numStones += bd.NumStones(anchors[i]);
        }
    }
    return true;
}

int GoBoardUtil::Stones(const GoBoard& bd, SgPoint p, SgPoint stones[])
{
    SG_ASSERT(bd.IsValidPoint(p));
    SG_ASSERT(bd.Occupied(p));
    if (bd.IsSingleStone(p))
    {
        stones[0] = p;
        return 1;
    }
    else
    {
        int nm = 0;
        for (GoBoard::StoneIterator it(bd, bd.Anchor(p)); it; ++it)
            stones[nm++] = p;
        return nm;
    }
}

bool GoBoardUtil::TwoPasses(const GoBoard& bd)
{
    SgBlackWhite toPlay = bd.ToPlay();
    GoPlayerMove passToPlay(toPlay, SG_PASS);
    GoPlayerMove passOpp(SgOppBW(toPlay), SG_PASS);
    int moveNumber = bd.MoveNumber();
    return (   moveNumber >= 2
            && bd.Move(moveNumber - 1) == passOpp
            && bd.Move(moveNumber - 2) == passToPlay);
}

SgPointSet GoBoardUtil::Lines(const GoBoard& bd, SgGrid from, SgGrid to)
{
    SG_ASSERT(from >= 1);
    SG_ASSERT(from <= to);
    SG_ASSERT(to <= (bd.Size() + 1) / 2);
    SgPointSet lines;
    for (SgGrid i = from; i <= to; ++i)
        lines |= bd.LineSet(i);
    return lines;
}

SgRect GoBoardUtil::GetDirtyRegion(const GoBoard& bd, SgMove move,
                                   SgBlackWhite colour, bool checklibs,
                                   bool premove)
{
    SgRect dirty;
    if (move == SG_PASS)
        return dirty;

    SgBlackWhite opp = SgOppBW(colour);

    // Point played has changed
    dirty.Include(move);

    SgPointSet blocks;

    // This move adjusts libs for all adjacent blocks
    if (checklibs)
    {
        for (SgNb4Iterator inb(move); inb; ++inb)
            if (bd.Occupied(*inb))
                for (GoBoard::StoneIterator istone(bd, *inb); istone;
                     ++istone)
                    dirty.Include(*istone);
    }

    // Check if this move will make a capture
    if (premove)
    {
        for (SgNb4Iterator inb(move); inb; ++inb)
        {
            if (bd.IsColor(*inb, opp) && bd.NumLiberties(*inb) == 1)
            {
                for (GoBoard::StoneIterator icap(bd, *inb); icap; ++icap)
                {
                    dirty.Include(*icap);

                    // Track blocks who gain libs as a result of capture
                    if (checklibs)
                    {
                        for (SgNb4Iterator inb2(*icap); inb2; ++inb2)
                            if (bd.IsColor(*inb2, colour))
                                blocks.Include(bd.Anchor(*inb2));
                    }
                }
            }
        }
    }

    // Check if this move did make a capture
    if (!premove && bd.CapturingMove())
    {
        for (SgPointSListIterator icaptures(bd.CapturedStones()); icaptures;
             ++icaptures)
        {
            dirty.Include(*icaptures);

            // Track blocks who gained liberties as a result of a capture
            if (checklibs)
            {
                for (SgNb4Iterator inb(*icaptures); inb; ++inb)
                    if (bd.IsColor(*inb, colour))
                        blocks.Include(bd.Anchor(*inb));
            }
        }
    }

    // Now mark all stones of blocks that gained liberties
    if (checklibs)
    {
        for (SgSetIterator iblocks(blocks); iblocks; ++iblocks)
            for (GoBoard::StoneIterator istone(bd, *iblocks); istone;
                 ++istone)
                dirty.Include(*istone);
    }

    return dirty;
}

int GoBoardUtil::Approx2Libs(const GoBoard& board, SgPoint block,
    SgPoint p, SgBlackWhite color)
{
    int libs2 = 0;
    for (SgNb4Iterator inb(p); inb; ++inb)
    {
        SgPoint nb = *inb;
        if (board.IsEmpty(nb))
            libs2++;
        else if (board.IsColor(nb, color)
            && board.Anchor(nb) != board.Anchor(block))
            libs2 += board.NumLiberties(nb); // May double count libs
    }

    return libs2;
}

//----------------------------------------------------------------------------
