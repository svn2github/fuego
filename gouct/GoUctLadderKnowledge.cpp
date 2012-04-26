//----------------------------------------------------------------------------
/** @file GoUctLadderKnowledge.cpp
    See GoUctLadderKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctLadderKnowledge.h"

#include "GoBoardUtil.h"

using GoLadderUtil::IsLadderCaptureMove;
using namespace GoUctLadderKnowledgeParameters;

//----------------------------------------------------------------------------
namespace
{    

    /** Copy into list, in case bd is modified and LibertyIterator can not
        be used directly */
    inline void GetLiberties(const GoBoard& bd, SgPoint block,
                             SgVector<SgPoint>& liberties)
    {
        SG_ASSERT_BW(bd.GetColor(block));
        SG_ASSERT(liberties.IsEmpty());
        for (GoBoard::LibertyIterator it(bd, block);  it; ++it)
            liberties.PushBack(*it);
    }

    /** Try liberties of blocks to find which ones can be captured */
    void CheckLadders(const GoBoard& bd,
                      const SgVector<SgPoint>& targetBlocks,
                      SgVector<SgPoint>& ladderCaptureBlocks)
    {
        for (SgVectorIterator<SgPoint> it(targetBlocks); it; ++it)
        {
            const SgPoint block = *it;
            if (  bd.NumLiberties(block) == 2
               && bd.IsColor(block, bd.ToPlay())
               )
            {
                SgVector<SgPoint> liberties;
                GetLiberties(bd, block, liberties);
                for (SgVectorIterator<SgPoint> it(liberties);  it; ++it)
                {
                    const SgPoint lib = *it;
                    if (IsLadderCaptureMove(bd, block, lib))
                    {
                        ladderCaptureBlocks.PushBack(block);
                        break;
                    }
                }
            }
        }
    }

    /** Check if block at p has many adjacent opponent blocks
        @todo: Only a dummy implementation now. This rule is dubious 
        in general, but it recognizes many bad ladders quickly */
    bool ManyAdjacentBlocks(const GoBoard& bd, SgPoint block)
    {
    	SgVector<SgPoint> blocks;
        GoBoardUtil::AdjacentBlocks(bd, block, SG_MAXPOINT, &blocks);
    	return blocks.Length() >= 3;
    }
    
    /** Check if block might be a nakade, which must be extended
        to almost-fill an eye space, even if that is a bad ladder move.
        Example (top left corner, block X):
        . X X . O    If this ends up as part of a semeai, then Black must
        O O O O O    extend and add a third stone to capture White.
        @todo: only a dummy implementation now. */
    bool MightBeNakadeStones(const GoBoard& bd, SgPoint block)
    {
    	return bd.NumStones(block) < 6 
            && ! ManyAdjacentBlocks(bd, block);
    }

} // namespace

GoUctLadderKnowledge::GoUctLadderKnowledge(const GoBoard& bd,
                           GoUctKnowledge& knowledge)
                           : m_bd(bd), m_knowledge(knowledge)
{ }

void GoUctLadderKnowledge::InitializeLadderAttackMoves()
{
    const SgPoint last = m_bd.GetLastMove();
    if (! SgIsSpecialMove(last) && m_bd.NumLiberties(last) == 2)
        LadderAttack(last);
}

void GoUctLadderKnowledge::InitializeLadderDefenseMoves()
{
    // ladder defense for neighbor blocks of last move
    SgPoint last = m_bd.GetLastMove();
    if (! SgIsSpecialMove(last))
    {
        SgVector<SgPoint> blocksJustPutInAtari;
        GoBoardUtil::AdjacentBlocks(m_bd, last, 1, &blocksJustPutInAtari);
        for (SgVectorIterator<SgPoint> it(blocksJustPutInAtari); it; ++it)
            LadderDefense(*it);
    }
}

void GoUctLadderKnowledge::Initialize2LibTacticsLadderMoves()
{
    SgPoint last = m_bd.GetLastMove();
    if (SgIsSpecialMove(last))
        /* */ return; /* */

    // own blocks with 2 lib, can be laddered
    SgVector<SgPoint> blocks2LibsLadder; 
    SgVector<SgPoint> atMostTwoLibBlocks; 

    GoBoardUtil::AdjacentBlocks(m_bd, last, 2, &atMostTwoLibBlocks);
    CheckLadders(m_bd, atMostTwoLibBlocks, blocks2LibsLadder);

    SgVector<SgPoint> good2LibTacticMove; // ladder moves to capture opponents
    for (SgVectorIterator<SgPoint> it(blocks2LibsLadder);  it; ++it)
    {
        const SgPoint anchor = *it;
        for (GoAdjBlockIterator<GoBoard> it(m_bd, anchor, 2); it; ++it)
        {
            const SgPoint oppAnchor = *it;
            SG_ASSERT(oppAnchor == m_bd.Anchor(oppAnchor));
            if (m_bd.NumLiberties(oppAnchor) == 2)
            {
                SgVector<SgPoint> liberties;
                GetLiberties(m_bd, oppAnchor, liberties);
                for (SgVectorIterator<SgPoint> it(liberties); it; ++it)
                {
                    const SgPoint lib = *it;
                    if (IsLadderCaptureMove(m_bd, oppAnchor, lib))
                        good2LibTacticMove.PushBack(lib);
                }
            }
        }
    }

    for (SgVectorIterator<SgPoint> it(good2LibTacticMove); it; ++it)
        m_knowledge.Add(*it, 1.0, GOOD_2_LIB_TACTICS_LADDER_BONUS);
}

void GoUctLadderKnowledge::LadderAttack(SgPoint p)
{
    const SgBlackWhite toPlay = m_bd.ToPlay();
    const SgBlackWhite opp = SgOppBW(toPlay);
    
	SG_DEBUG_ONLY(opp);
	SG_ASSERT(m_bd.GetStone(p) == opp);
    SG_ASSERT(m_bd.NumLiberties(p) == 2);
    
    SgVector<SgPoint> liberties;
    GetLiberties(m_bd, p, liberties);
    bool captured = false;
    for (SgVectorIterator<SgPoint> it(liberties); it; ++it)
    {
        if (IsLadderCaptureMove(m_bd, p, *it)) 
        {
            m_knowledge.Add(*it, 1.0, LADDER_CAPTURE_BONUS);
            captured = true;
        }
    }
}

void GoUctLadderKnowledge::LadderDefense(SgPoint p)
{
    SG_ASSERT(p == m_bd.Anchor(p));
    SG_ASSERT(m_bd.GetStone(p) == m_bd.ToPlay());
    SG_ASSERT(m_bd.InAtari(p));

    SgVector<SgPoint> escapeMoves;
    GoLadderUtil::FindLadderEscapeMoves(m_bd, p, escapeMoves);
    if (escapeMoves.IsEmpty()) // Do not try to escape
    {
        if (! MightBeNakadeStones(m_bd, p))
            m_knowledge.Initialize(m_bd.TheLiberty(p), 
            					   0.0, BAD_LADDER_ESCAPE_PENALTY);
    }
    else // Can escape, running away may be good.
    {
        for (SgVectorIterator<SgPoint> it(escapeMoves); it; ++it)
            m_knowledge.Add(*it, 1.0, GOOD_LADDER_ESCAPE_BONUS);
    }
}

void GoUctLadderKnowledge::ProcessPosition()
{
	InitializeLadderDefenseMoves();
	InitializeLadderAttackMoves();
	Initialize2LibTacticsLadderMoves();
}
