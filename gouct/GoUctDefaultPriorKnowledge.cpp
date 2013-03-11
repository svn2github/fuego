//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.cpp
    See GoUctDefaultPriorKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardUtil.h"
#include "GoUctDefaultPriorKnowledge.h"
#include "GoUctLadderKnowledge.h"

//----------------------------------------------------------------------------
namespace {

/** Test if playing on p puts any opponent block into atari */
bool SetsAtari(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT(bd.IsEmpty(p)); // Already checked
    SgBlackWhite opp = SgOppBW(bd.ToPlay());
    if (bd.NumNeighbors(p, opp) > 0)
        for (GoNbIterator it(bd, p); it; ++it)
            if (   bd.IsColor(*it, opp)
                && bd.NumLiberties(*it) == 2
               )
                return true;
    return false;
}

bool HasMinNuOfAdjBlocks(const GoBoard& bd, SgPoint p,
                         int minNuBlocks)
{
    static const int MANY_LIBS = SG_MAX_SIZE * SG_MAX_SIZE;
    SG_ASSERT(bd.IsSingleStone(p));
    int nuBlocks = 0;
    for (GoAdjBlockIterator<GoBoard> it(bd, p, MANY_LIBS); it; ++it)
        if (++nuBlocks >= minNuBlocks)
            return true;
    return false;
}

// @todo write test cases
inline SgPoint OtherLiberty(const GoBoard& bd, SgPoint ourStone, SgPoint p)
{
    SG_ASSERT(bd.NumLiberties(ourStone) == 2);
    GoBoard::LibertyIterator it(bd, ourStone);
    SG_ASSERT(it);
    if (*it == p) // get next
    {
        ++it;
        SG_ASSERT(it);
        SG_ASSERT(*it != p);
        return *it;
    }
    else
    {
        SG_ASSERT(bd.IsLibertyOfBlock(p, ourStone));
        return *it;
    }
}

inline SgPoint FindNeighborNotInColor(const GoBoard& bd, SgPoint p,
                                         SgEmptyBlackWhite c)
{
    for (GoNbIterator it(bd, p); it; ++it)
        if (! bd.IsColor(*it, c))
            return *it;
    SG_ASSERT(false);
    return p;
}


inline bool IsCorridorEndPoint(const GoBoard& bd, SgPoint p,
                               SgBlackWhite oppColor)
{
    return bd.NumNeighbors(p, oppColor) + bd.NumEmptyNeighbors(p) == 1;
}

inline bool Is2PointEye(const GoBoard& bd, SgPoint p,
                        SgBlackWhite eyeColor, SgPoint& other)
{
    const SgBlackWhite oppColor = SgOppBW(eyeColor);
    if (! IsCorridorEndPoint(bd, p, oppColor))
        return false;

    other = FindNeighborNotInColor(bd, p, eyeColor);
    return IsCorridorEndPoint(bd, other, oppColor);
}

bool Is3PointEye(const GoBoard& bd, SgPoint p, SgBlackWhite eyeColor,
                 SgPoint* other)
{
    const SgBlackWhite oppColor = SgOppBW(eyeColor);
    const int nuInsideNb =   bd.NumNeighbors(p, oppColor)
                           + bd.NumEmptyNeighbors(p);
    if (nuInsideNb == 2) // check if center point
    {
        int index = 0;
        for (GoNbIterator it(bd, p); it; ++it)
            if (bd.GetColor(*it) != eyeColor)
            {
                SG_ASSERT(bd.GetColor(*it) != SG_BORDER);
                if (! IsCorridorEndPoint(bd, *it, oppColor))
                    return false;
                SG_ASSERT(index < 2);
                other[index] = *it;
                ++index;
            }
        return true;
    }
    else if (nuInsideNb == 1) // end point. check if neighbors are center
    // and other end point
    {
        const SgPoint mid = FindNeighborNotInColor(bd, p, eyeColor);
        const int midNuInsideNb =  bd.NumNeighbors(mid, oppColor)
                                 + bd.NumEmptyNeighbors(mid);
        if (midNuInsideNb != 2)
            return false;
        for (GoNbIterator it(bd, mid); it; ++it)
            // find endpoint other than p. It must exist.
            if (*it != p && bd.GetColor(*it) != eyeColor)
            {
                SG_ASSERT(bd.GetColor(*it) != SG_BORDER);
                return IsCorridorEndPoint(bd, *it, oppColor);
            }
        SG_ASSERT(false);
    }
    return false; // nuInsideNb has other value
}

inline bool IsKoStone(const GoBoard& bd, SgPoint block)
{
    SG_ASSERT(bd.InAtari(block));
    SG_ASSERT(bd.NumStones(block) == 1);

    const SgBlackWhite oppColor = SgOppBW(bd.GetStone(block));
    const SgPoint lib = bd.TheLiberty(block);
    if (bd.NumEmptyNeighbors(lib) + bd.NumNeighbors(lib, oppColor) > 0)
        return false;
    int nuInAtari = 0;
    for (GoNbIterator it(bd, lib); it; ++it)
        if (bd.InAtari(*it) && ++nuInAtari > 1)
            return false;
    return true;
}

inline bool CanBeCapturedCleanly(const GoBoard& bd, SgPoint block)
{
    return   bd.InAtari(block)
          && (bd.NumStones(block) > 1 || ! IsKoStone(bd, block));
}

//  recognize simple one point false eyes:
//  center        : >= 2 diagonal points occupied by non-dead opponent stones
//  corner, edge  : >= 1 diagonal points occupied by non-dead opponent stones
bool IsFalseEyePoint(const GoBoard& bd,
                     SgPoint p,
                     SgBlackWhite eyeColor)
{
    const SgBlackWhite oppColor = SgOppBW(eyeColor);
    if (! IsCorridorEndPoint(bd, p, oppColor))
        return false;
    int nuForFalse = (bd.Line(p) == 1) ? 1 : 2;
    int nuDiag = 0;
    for (SgNb4DiagIterator it(p); it; ++it)
    {
        const SgPoint diag(*it);
        if (bd.IsColor(diag, oppColor) && ! CanBeCapturedCleanly(bd, diag))
        // @todo could check if those stones have any life
        // as in Explorer code, ExEyeStatus CheckDiagonals
        {
            if (++nuDiag >= nuForFalse)
                return true;
        }
    }
    return false;
}

/** Test if p is inside a small eye space surrounded by eyeColor.
    @todo program cases 4, 5, 6.
    @todo add test cases.
*/
bool MayMakeFalseEye(const GoBoard& bd,
                     SgPoint p,
                     SgBlackWhite eyeColor)
{
    SG_ASSERT(bd.IsEmpty(p)); // this is used in the code below
    SG_ASSERT_BW(eyeColor);
    const SgBlackWhite toPlay = SgOppBW(eyeColor);

    // eliminate areas that are not well surrounded
    const int nuEmpty = bd.NumEmptyNeighbors(p);
    SG_ASSERT(nuEmpty < 2); // otherwise no selfatari
        // or just return false; ?? depends if used in more cases later.
    const int nuInsideNb = bd.NumNeighbors(p, toPlay) + nuEmpty;
    if (nuInsideNb > 2)
        return false;

    /*
    // Case 4, 5, 6: Our selfatari will also atari the eyeColor block.
    // @todo we could check further if that eyeColor block
    // has other captures on the outside - then
    // our selfatari is probably futile
    Case 4:
    OOOO <-- no outside liberties
    O.aO
    
    Case 5:
    OOOOO <-- no outside liberties
    O.XaO
    
    Case 6:
    OOOOO <-- no outside liberties
    O.aXO
    if (GoBoardUtil::PointHasAdjacentBlock(bd, p, eyeColor, 2))
    {
        return true;
    }
    */

    if (nuInsideNb == 2)
    /*  Case 3: OOOOX
                O.aXO */
    {
        SgPoint other[2];
        if (! Is3PointEye(bd, p, eyeColor, other))
            return false;
        if (nuEmpty == 0)// two own nb and selfatari.
            // Complex case, bailing out.
            // e.g. A1 empty and both nb are ours.
            // Might still be false eye, but...
            return false;

        // Case 3: cutting stone - might be false eye.
        const SgPoint ourStone = GoBoardUtil::FindNeighbor(bd, p, toPlay);
        if (  bd.NumEmptyNeighbors(ourStone) > 1
           || bd.NumStones(ourStone) > 1
           )
                return false; // not small eye
        if (! IsFalseEyePoint(bd, ourStone, eyeColor))
            return false;
        return HasMinNuOfAdjBlocks(bd, ourStone, 2);
    }
    SG_ASSERT(nuInsideNb == 1); // check case 1 and 2
    if (! IsFalseEyePoint(bd, p, eyeColor))
        return false;
    SgPoint other;
    if (nuEmpty == 1)
    /*  Case 1: OOOX    OOOX    |OOX
                O.aO    O.aO    |.aO
                OOOX    ----    ---- */
    {
        if (! Is2PointEye(bd, p, eyeColor, other))
            return false;
        const SgPoint theEmpty = GoBoardUtil::FindNeighbor(bd, p, SG_EMPTY);
        bool result =    bd.NumEmptyNeighbors(theEmpty) == 1 // p
                      && bd.NumNeighbors(theEmpty, toPlay) == 0;
        if (result)
            return true;
    }
    else
    /* Case 2: OOOOX   OOOOX   |OOOX
               O.XaO   O.XaO   |.XaO
               OOOOX   -----   ----- */
    {
        SgPoint other[2];
        if (! Is3PointEye(bd, p, eyeColor, other))
            return false;
        const SgPoint ourStone = GoBoardUtil::FindNeighbor(bd, p, toPlay);
        if (  bd.NumEmptyNeighbors(ourStone) != 2 // p and the other lib
           || bd.NumStones(ourStone) != 1)
            return false;
        const SgPoint otherLib = OtherLiberty(bd, ourStone, p);
        if (  bd.NumEmptyNeighbors(otherLib) == 0
           && bd.NumNeighbors(otherLib, toPlay) == 1)
            return true;
    }
    return false;
}

/** Heuristic for bad selfatari moves
	@todo: should eliminate useless moves, but allow play in nakade.
*/
bool BadSelfAtari(const GoBoard& bd, SgPoint p)
{
    int numStones;
    SgBlackWhite opp = bd.Opponent();
    if (GoBoardUtil::SelfAtari(bd, p, numStones))
    {
    	if (numStones > GoEyeUtil::NAKADE_LIMIT)
        	return true;
        if (MayMakeFalseEye(bd, p, opp))
        	return false;
        return true;
    }
    return false;
}

} // namespace

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledge::GoUctDefaultPriorKnowledge(const GoBoard& bd,
                              const GoUctPlayoutPolicyParam& param)
    : GoUctKnowledge(bd),
      m_policy(bd, param)
{ }

void GoUctDefaultPriorKnowledge::AddBonusNearPoint(GoPointList& emptyPoints,
                                                  SgUctValue count,
                                                  SgPoint focus,
                                                  SgUctValue v1,
                                                  SgUctValue v2,
                                                  SgUctValue v3,
                                                  bool addPass
                                                  )
{
    if (focus == SG_NULLMOVE || focus == SG_PASS)
        return;
    
    const GoBoard& bd = Board();
    SgPointArray<int> dist = GoBoardUtil::CfgDistance(bd, focus, 3);
    for (GoPointList::Iterator it(emptyPoints); it; ++it)
    {
        const SgPoint p = *it;
        SgUctValue v;
        switch (dist[p])
        {
            case 1:  v = v1;
                break;
            case 2:  v = v2;
                break;
            case 3:  v = v3;
                break;
            default: v = SgUctValue(0.1);
                break;
        }
        Add(p, v, count);
    }
    if (addPass)
        Add(SG_PASS, SgUctValue(0.1), count);
}

void GoUctDefaultPriorKnowledge::AddLocalityBonus(GoPointList& emptyPoints,
                                                  bool isSmallBoard)
{
    const GoBoard& bd = Board();
    const SgPoint last = bd.GetLastMove();
    SgUctValue count = isSmallBoard ? 4 : 5;
    AddBonusNearPoint(emptyPoints, count, last,
                     SgUctValue(1.0),
                     SgUctValue(0.6),
                     SgUctValue(0.6),
                     true /* addPass*/);

    const SgPoint last2 = bd.Get2ndLastMove();
    count = isSmallBoard ? 2 : 3;
    AddBonusNearPoint(emptyPoints, count, last2,
                     SgUctValue(0.8),
                     SgUctValue(0.55),
                     SgUctValue(0.55),
                     false /* don't addPass*/);
}

/** Find global moves that match a playout pattern or set a block into atari.
    @param[out] pattern
    @param[out] atari
    @param[out] empty As a side effect, this function finds all empty points
    on the board
    @return @c true if any such moves was found */
bool GoUctDefaultPriorKnowledge::FindGlobalPatternAndAtariMoves(
                                                     SgPointSet& pattern,
                                                     SgPointSet& atari,
                                                     GoPointList& empty)
{
	// Minimum value for pattern gamma to be used.
    static const float EPSILON = 0.00000000001;
    const GoBoard& bd = Board();
    SG_ASSERT(empty.IsEmpty());
    const GoUctPatterns<GoBoard>& patterns = m_policy.GlobalPatterns();
    bool result = false;
    m_maxPatternGamma = -1.f;
    for (GoBoard::Iterator it(bd); it; ++it)
        if (bd.IsEmpty(*it))
        {
            empty.PushBack(*it);
            float gamma = patterns.GetPatternGamma(bd, *it, bd.ToPlay());
            if (gamma > EPSILON)
            {
                pattern.Include(*it);
                result = true;
                m_patternGammas[*it] = gamma;
                if (gamma > m_maxPatternGamma)
                    m_maxPatternGamma = gamma;
            }
            if (SetsAtari(bd, *it))
            {
                atari.Include(*it);
                result = true;
            }
        }
    return result;
}

void 
GoUctDefaultPriorKnowledge::InitializeForGlobalHeuristic(
	const GoPointList& empty,
    const SgPointSet& pattern,
    const SgPointSet& atari,
    int nuSimulations)
{
    const GoBoard& bd = Board();
    for (GoPointList::Iterator it(empty); it; ++it)
    {
        const SgPoint p = *it;
        SG_ASSERT (bd.IsEmpty(p));
        if (BadSelfAtari(bd, p))
            Initialize(p, 0.1f, nuSimulations);
        else if (atari[p])
            Initialize(p, 1.0f, 3);
        else if (pattern[p])
            Initialize(*it, 
                       0.6 + (m_patternGammas[*it] / m_maxPatternGamma) * 0.4,
                       3);
		else
            Initialize(p, 0.5f, 3);
    }
}

void 
GoUctDefaultPriorKnowledge::InitializeForNonRandomPolicyMove(
	const GoPointList& empty,
    const SgPointSet& pattern,
    const SgPointSet& atari,
    int nuSimulations)
{
    const GoBoard& bd = Board();
    for (GoPointList::Iterator it(empty); it; ++it)
    {
        const SgPoint p = *it;
        SG_ASSERT (bd.IsEmpty(p));
        if (BadSelfAtari(bd, p))
            Initialize(p, 0.1f, nuSimulations);
        else if (atari[p])
            Initialize(p, 0.8f, nuSimulations);
        else if (pattern[p])
            Initialize(*it,
                       0.5 + (m_patternGammas[*it] / m_maxPatternGamma) * 0.3,
                       nuSimulations);
        else
            Initialize(p, 0.4f, nuSimulations);
    }
    const GoPointList moves = m_policy.GetEquivalentBestMoves();
    for (GoPointList::Iterator it(moves); it; ++it)
        Initialize(*it, 1.0, nuSimulations);
}


void 
GoUctDefaultPriorKnowledge::InitializeForRandomPolicyMove(
	const GoPointList& empty,
    int nuSimulations)
{
    const GoBoard& bd = Board();
    for (GoPointList::Iterator it(empty); it; ++it)
    {
        const SgPoint p = *it;
        SG_ASSERT (bd.IsEmpty(p));
        if (BadSelfAtari(bd, p))
            Initialize(p, 0.1f, nuSimulations);
        else
            Clear(p); // Don't initialize
    }
}

namespace
{
    bool IsEmpty3x3Box(const GoBoard& bd, SgPoint p)
    {
        return bd.IsEmpty(p)
            && bd.Num8EmptyNeighbors(p) == 8;
    }
    
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
}

void
GoUctDefaultPriorKnowledge::AddOpeningBonus()
{
    const GoBoard& bd = Board();
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
                
                SgUctValue ignoreValue;
                SgUctValue count;
                Get(p, ignoreValue, count);
                Initialize(p, SgUctValue(1.0), count + bonus/3);
            }
        }
    }
}

void
GoUctDefaultPriorKnowledge::ProcessPosition(std::vector<SgUctMoveInfo>&
                                            outmoves)
{
    m_policy.StartPlayout();
    m_policy.GenerateMove();
    GoUctPlayoutPolicyType type = m_policy.MoveType();
    bool isFullBoardRandom =
        (type == GOUCT_RANDOM || type == GOUCT_FILLBOARD);
    SgPointSet pattern;
    SgPointSet atari;
    GoPointList empty;
    bool anyHeuristic = FindGlobalPatternAndAtariMoves(pattern, atari, empty);

    // The initialization values/counts are mainly tuned by selfplay
    // experiments and games vs MoGo Rel 3 and GNU Go 3.6 on 9x9 and 19x19.
    // If different values are used for the small and large board, the ones
    // from the 9x9 experiments are used for board sizes < 15, the ones from
    // 19x19 otherwise.
    const bool isSmallBoard = (Board().Size() < 15);
    const int defaultNuSimulations = isSmallBoard ? 9 : 18;

    Initialize(SG_PASS, 0.1f, defaultNuSimulations);
    if (isFullBoardRandom && ! anyHeuristic)
    	InitializeForRandomPolicyMove(empty, defaultNuSimulations);
    else if (isFullBoardRandom && anyHeuristic)
    	InitializeForGlobalHeuristic(empty, pattern, atari,
                                     defaultNuSimulations);
    else
    	InitializeForNonRandomPolicyMove(empty, pattern, atari,
                                         defaultNuSimulations);

    AddLocalityBonus(empty, isSmallBoard);
    if (! isSmallBoard)
        AddOpeningBonus();
    GoUctLadderKnowledge ladderKnowledge(Board(), *this);
    ladderKnowledge.ProcessPosition();

    m_policy.EndPlayout();
    TransferValues(outmoves);
}

//----------------------------------------------------------------------------
