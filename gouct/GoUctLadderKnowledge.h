//----------------------------------------------------------------------------
/** @file GoUctLadderKnowledge.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_LADDERKNOWLEDGE_H
#define GOUCT_LADDERKNOWLEDGE_H

#include "GoBoard.h"
#include "GoLadder.h"
#include "GoUctKnowledge.h"

namespace GoUctLadderKnowledgeParameters
{
    /** penalty for running away with captured stones */
    const int BAD_LADDER_ESCAPE_PENALTY = 10;

    /** Bonus for ladder escape */
    const int GOOD_LADDER_ESCAPE_BONUS = 3;

    /** Bonus for good 2-liberty moves */
    const int GOOD_2_LIB_TACTICS_LADDER_BONUS = 8;

    /** Bonus for ladder capture moves */
    const int LADDER_CAPTURE_BONUS = 3;
}

//----------------------------------------------------------------------------

/** Add ladder knowledge to calling GoUctKnowledge object */
class GoUctLadderKnowledge 
{
public:
    GoUctLadderKnowledge(const GoBoard& bd,
                         GoUctKnowledge& knowledge);

    /** Compute the ladder knowledge */
    void ProcessPosition();

private:

    /** The board for which ladders are computed
    	It is const in the interface but temporarily modified during ladder
        computations. 
    */
    const GoBoard& m_bd;
    
    /** The knowledge object we are adding to */
    GoUctKnowledge& m_knowledge;

    /** For computing ladders. Kept as a field for efficiency. */
    GoLadder m_ladder;
    
    /** Defend our blocks next to last move */
    void InitializeLadderDefenseMoves();

    /** Attack last opponent move */
    void InitializeLadderAttackMoves();

	/** Two liberty tactics based on ladder search.
        If a block b is reduced from 3 to 2 liberties by the last move:
		1. Check if b will be ladder-captured by the opponent.
		2. Try to ladder-capture all adjacent opponent blocks.*/
	void Initialize2LibTacticsLadderMoves();

    /** Attack block */
    void LadderAttack(SgPoint block);
    
    /** Defend block */
	void LadderDefense(SgPoint block);
};

//----------------------------------------------------------------------------

#endif // GOUCT_LADDERKNOWLEDGE_H
