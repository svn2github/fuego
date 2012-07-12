//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.cpp
    See GoUctDefaultPriorKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPriorKnowledge.h"
#include "GoUctLadderKnowledge.h"

//----------------------------------------------------------------------------
namespace {

/** Test if playing on p puts any opponent block into atari */
bool SetsAtari(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT(bd.IsEmpty(p)); // Already checked
    SgBlackWhite opp = SgOppBW(bd.ToPlay());
    if (bd.NumNeighbors(p, opp) == 0)
        return false;
    if (! bd.IsBorder(p + SG_NS) && bd.GetColor(p + SG_NS) == opp
        && bd.NumLiberties(p + SG_NS) == 2)
        return true;
    if (! bd.IsBorder(p - SG_NS) && bd.GetColor(p - SG_NS) == opp
        && bd.NumLiberties(p - SG_NS) == 2)
        return true;
    if (! bd.IsBorder(p + SG_WE) && bd.GetColor(p + SG_WE) == opp
        && bd.NumLiberties(p + SG_WE) == 2)
        return true;
    if (! bd.IsBorder(p - SG_WE) && bd.GetColor(p - SG_WE) == opp
        && bd.NumLiberties(p - SG_WE) == 2)
        return true;
    return false;
}

/** Test if p is inside a small eye space surrounded by eyeColor.
	Simple first version: small means <= 3, p not at end point - 
    only neighbors of p tested. */
bool InSmallEyeSpace(const GoBoard& bd, SgPoint p, SgBlackWhite eyeColor)
{
    SG_ASSERT(bd.IsEmpty(p)); // this is used in the code below
    SG_ASSERT_BW(eyeColor);
    const SgBlackWhite attacker = SgOppBW(eyeColor);
    
    int size = 1 + bd.NumNeighbors(p, attacker) + bd.NumEmptyNeighbors(p);
    if (size > 3)
        return false;
    for (SgNb4Iterator it(p); it; ++it)
    {
    	if (bd.IsEmpty(*it) || bd.IsColor(*it, attacker))
        {
            size += bd.NumNeighbors(*it, attacker) 
                  + bd.NumEmptyNeighbors(*it) 
                  - 1; 
            // -1 for p. We are double-counting diagonals. Too bad...
            if (size > 3)
                return false;
        }
    }
    return true;
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
        if (InSmallEyeSpace(bd, p, opp))
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

void GoUctDefaultPriorKnowledge::AddLocalityBonus(GoPointList& emptyPoints,
                                                  bool isSmallBoard)
{
    const GoBoard& bd = Board();
    SgPoint last = bd.GetLastMove();
    if (last != SG_NULLMOVE && last != SG_PASS)
    {
        SgPointArray<int> dist = GoBoardUtil::CfgDistance(bd, last, 3);
        const SgUctValue count = isSmallBoard ? 4 : 5;
        for (GoPointList::Iterator it(emptyPoints); it; ++it)
        {
            const SgPoint p = *it;
            switch (dist[p])
            {
            case 1:
                Add(p, SgUctValue(1.0), count);
                break;
            case 2:
                Add(p, SgUctValue(0.6), count);
                break;
            case 3:
                Add(p, SgUctValue(0.6), count);
                break;
            default:
                Add(p, SgUctValue(0.1), count);
                break;
            }
        }
        Add(SG_PASS, SgUctValue(0.1), count);
    }
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
    GoUctLadderKnowledge ladderKnowledge(Board(), *this);
    ladderKnowledge.ProcessPosition();

    m_policy.EndPlayout();
    TransferValues(outmoves);
}

//----------------------------------------------------------------------------
