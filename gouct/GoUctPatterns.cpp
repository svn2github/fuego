//----------------------------------------------------------------------------
/** @file GoUctPatterns.cpp
 See GoUctPatterns.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctPatterns.h"

#include "GoBoard.h"

//----------------------------------------------------------------------------

namespace {

bool CheckCut1(const GoBoard& bd, SgPoint p,
               SgBlackWhite c, int cDir, int otherDir)
{
    SG_ASSERT_BW(c);
    return bd.IsColor(p + otherDir, c)
        && bd.IsColor(p + cDir + otherDir, SgOppBW(c));
}

bool CheckCut2(const GoBoard& bd, SgPoint p,
               const SgBlackWhite c, int cDir,
               int otherDir)
{
    SG_ASSERT_BW(c);
    SG_ASSERT(bd.IsColor(p + cDir, c));
    const SgBlackWhite opp = SgOppBW(c);
    return bd.IsColor(p - cDir, c)
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

bool CheckHane1(const GoBoard& bd, SgPoint p,
                SgBlackWhite c, SgBlackWhite opp,
                int cDir, int otherDir)
{
    return bd.IsColor(p + cDir, c)
        && bd.IsColor(p + cDir + otherDir, opp)
        && bd.IsColor(p + cDir - otherDir, opp)
        && bd.IsEmpty(p + otherDir)
        && bd.IsEmpty(p - otherDir)
        ;
}

int EdgeDirection(GoBoard& bd, SgPoint p, int index)
{
    const int up = bd.Up(p);
    switch (index)
    {
        case 0:
            return Pattern3x3::OtherDir(up);
        case 1:
            return up + Pattern3x3::OtherDir(up);
        case 2:
            return up;
        case 3:
            return up - Pattern3x3::OtherDir(up);
        default:
            SG_ASSERT(index == 4);
            return - Pattern3x3::OtherDir(up);
    }
}

/** Find direction of a neighboring stone in color c */
int FindDir(const GoBoard& bd, SgPoint p, SgBlackWhite c)
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

bool MatchCut(const GoBoard& bd, SgPoint p)
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

bool MatchEdge(const GoBoard& bd, SgPoint p,
               const int nuBlack, const int nuWhite)
{
    const int up = bd.Up(p);
    const int side = Pattern3x3::OtherDir(up);
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

bool MatchHane(const GoBoard& bd, SgPoint p,
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
        const int otherDir = Pattern3x3::OtherDir(dir);
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

int SetupCodedEdgePosition(GoBoard& bd, int code)
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

int SetupCodedPosition(GoBoard& bd, int code)
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

const int PA_NU_AXES = 3;

const unsigned int PA_NU_AX_SETS = 1 << PA_NU_AXES;

typedef unsigned int PaAxSet;
// todo copied from features

inline int EBWCodeOfPoint(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT(bd.IsValidPoint(p));
    BOOST_STATIC_ASSERT(SG_BLACK == 0);
    BOOST_STATIC_ASSERT(SG_WHITE == 1);
    BOOST_STATIC_ASSERT(SG_EMPTY == 2);
    return bd.GetColor(p);
}

int AxCodeOf8Neighbors(const GoBoard& bd, SgPoint p, PaAxSet axes)
{
    using std::make_pair;
    static const std::pair<int,int> axDir[PA_NU_AX_SETS] =
    {
        make_pair( SG_NS,  SG_WE),
        make_pair( SG_NS, -SG_WE),
        make_pair(-SG_NS,  SG_WE),
        make_pair(-SG_NS, -SG_WE),
        make_pair( SG_WE,  SG_NS),
        make_pair( SG_WE, -SG_NS),
        make_pair(-SG_WE,  SG_NS),
        make_pair(-SG_WE, -SG_NS)
    };

    const std::pair<int,int> dir = axDir[axes];
    SG_ASSERT(bd.Line(p) > 1);
    int code = ((((((   EBWCodeOfPoint(bd, p + dir.first + dir.second) * 3
                     + EBWCodeOfPoint(bd, p + dir.first)) * 3
                    + EBWCodeOfPoint(bd, p + dir.first - dir.second)) * 3
                   + EBWCodeOfPoint(bd, p + dir.second)) * 3
                  + EBWCodeOfPoint(bd, p - dir.second)) * 3
                 + EBWCodeOfPoint(bd, p - dir.first + dir.second)) * 3
                + EBWCodeOfPoint(bd, p - dir.first)) * 3
               + EBWCodeOfPoint(bd, p - dir.first - dir.second);
    SG_ASSERT(code >= 0);
    SG_ASSERT(code < Pattern3x3::GOUCT_POWER3_8);
    return code;
}

int MinCodeOf8Neighbors(const GoBoard& bd, SgPoint p)
{
    int minCode = Pattern3x3::GOUCT_POWER3_8;
    for (PaAxSet s = 0; s < PA_NU_AX_SETS; ++s)
    {
        const int code = AxCodeOf8Neighbors(bd, p, s);
        minCode = std::min(minCode, code);
    }
    SG_ASSERT(minCode >= 0);
    SG_ASSERT(minCode < Pattern3x3::GOUCT_POWER3_8);
    return minCode;
}

int MirrorCodeOfEdgeNeighbors(const GoBoard& bd,
                              SgPoint p)
{
    SG_ASSERT(bd.Line(p) == 1);
    SG_ASSERT(bd.Pos(p) > 1);
    const int up = bd.Up(p);
    int other = -Pattern3x3::OtherDir(up);
    int code = (((EBWCodeOfPoint(bd, p + other) * 3
                  + EBWCodeOfPoint(bd, p + up + other)) * 3
                 + EBWCodeOfPoint(bd, p + up)) * 3
                + EBWCodeOfPoint(bd, p + up - other)) * 3
               + EBWCodeOfPoint(bd, p - other);
    SG_ASSERT(code >= 0);
    SG_ASSERT(code < Pattern3x3::GOUCT_POWER3_5);
    return code;
}

int MinEdgeCode(const GoBoard& bd, SgPoint p)
{
    return std::min(GoUctPatterns<GoBoard>::CodeOfEdgeNeighbors(bd, p),
                    MirrorCodeOfEdgeNeighbors(bd, p));
}

} // namespace

#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

//----------------------------------------------------------------------------
void Pattern3x3::MapCenterPatternsToMinimum()
{
    GoBoard bd(5);
    int code[GOUCT_POWER3_8];
    const SgPoint p = SgPointUtil::Pt(3, 3);
    for (int i = 0; i < GOUCT_POWER3_8; ++i)
    {
        int count = SetupCodedPosition(bd, i);
        code[i] = MinCodeOf8Neighbors(bd, p);
        //SgDebug() << "Min Code of " << i << " is " << code[i] << '\n';
        while (count-- > 0)
            bd.Undo();
    }
    //int minCode = *std::min_element(code, code + GOUCT_POWER3_8);
    //int maxCode = *std::max_element(code, code + GOUCT_POWER3_8);
    //SgDebug() << "min Code " << minCode << " max Code " << maxCode << '\n';
    std::sort(code, code + GOUCT_POWER3_8);
    std::copy(code, code + GOUCT_POWER3_8, std::ostream_iterator<int>(SgDebug(), " "));
    int* last = std::unique(code, code + GOUCT_POWER3_8);
    SgDebug() <<  last - code << " unique elements " << '\n';
    // TODO remap to consecutive ints;
    // create mapping;
    // collect weights and compute average (?) 
    //std::copy(code, last, std::ostream_iterator<int>(SgDebug(), " "));

}

void Pattern3x3::MapEdgePatternsToMinimum()
{
    GoBoard bd(5);
    int code[GOUCT_POWER3_5];
    const SgPoint p = SgPointUtil::Pt(1, 3);
    for (int i = 0; i < GOUCT_POWER3_5; ++i)
    {
        int count = SetupCodedEdgePosition(bd, i);
        code[i] = MinEdgeCode(bd, p);
        while (--count >= 0)
            bd.Undo();
    }
    //int minCode = *std::min_element(code, code + GOUCT_POWER3_8);
    //int maxCode = *std::max_element(code, code + GOUCT_POWER3_8);
    //SgDebug() << "min Code " << minCode << " max Code " << maxCode << '\n';
    std::sort(code, code + GOUCT_POWER3_5);
    std::copy(code, code + GOUCT_POWER3_5, std::ostream_iterator<int>(SgDebug(), " "));
    int* last = std::unique(code, code + GOUCT_POWER3_5);
    SgDebug() <<  last - code << " unique elements " << '\n';
    // TODO remap to consecutive ints;
    // create mapping;
    // collect weights and compute average (?)
    //std::copy(code, last, std::ostream_iterator<int>(SgDebug(), " "));

}

void Pattern3x3::InitEdgePatternTable(SgBWArray<GoUctEdgePatternTable>&
                                      edgeTable)
{
    GoBoard bd(5);
    const SgPoint p = SgPointUtil::Pt(1, 3);
    for (int i = 0; i < GOUCT_POWER3_5; ++i)
    {
        int count = SetupCodedEdgePosition(bd, i);
        for (SgBWIterator it; it; ++it)
        {
            bd.SetToPlay(*it);
            const bool isPattern = Pattern3x3::MatchAnyPattern(bd, p);
            edgeTable[*it][i].SetIsPattern(isPattern);
        }
        while (count-- > 0)
            bd.Undo();
    }
}

void Pattern3x3::InitCenterPatternTable(SgBWArray<GoUctPatternTable>& table)
{
    GoBoard bd(5);
    const SgPoint p = SgPointUtil::Pt(3, 3);
    for (int i = 0; i < GOUCT_POWER3_8; ++i)
    {
        int count = SetupCodedPosition(bd, i);
        for (SgBWIterator it; it; ++it)
        {
            bd.SetToPlay(*it);
            const bool isPattern = Pattern3x3::MatchAnyPattern(bd, p);
            table[*it][i].SetIsPattern(isPattern);
        }
        while (count-- > 0)
            bd.Undo();
    }

    //Pattern3x3::MapCenterPatternsToMinimum();
    //Pattern3x3::MapEdgePatternsToMinimum();
}

bool Pattern3x3::MatchAnyPattern(const GoBoard& bd, SgPoint p)
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
        return MatchHane(bd, p, nuBlack, nuWhite)
            || MatchCut(bd, p);
}

void Pattern3x3::Write2x3EdgePattern(std::ostream& stream, int code)
{
    stream << '\n';
    GoBoard bd(5);
    SetupCodedEdgePosition(bd, code);
    for (int i = 1; i <= 2; ++i)
    {
        for (int j = 2; j <= 4; ++j)
        {
            const SgPoint p = SgPointUtil::Pt(j, i); // todo rotation
            stream << SgEBW(bd.GetColor(p));
        }
        stream << '\n';
    }
}

void Pattern3x3::Write3x3CenterPattern(std::ostream& stream, int code)
{
    stream << '\n';
    GoBoard bd(5);
    SetupCodedPosition(bd, code);
    for (int i = 4; i >= 2; --i)
    {
        for (int j = 2; j <= 4; ++j)
        {
            const SgPoint p = SgPointUtil::Pt(j, i);
            stream << SgEBW(bd.GetColor(p));
        }
        stream << '\n';
    }
}

