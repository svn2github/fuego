//----------------------------------------------------------------------------
/** @file GoUctKnowledge.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_KNOWLEDGE_H
#define GOUCT_KNOWLEDGE_H

#include "GoBoard.h"
#include "SgUctTree.h"
#include "SgUctValue.h"

//----------------------------------------------------------------------------

/** Virtual base class for Go knowledge for MCTS in Fuego. 
	Knowledge is used to initialize nodes in the game tree with
    "fake" simulations in order to bias the search. 
    These "fake" simulations are represented by an average value and a count.
    Knowledge is computed for the current position on a GoBoard.
*/
class GoUctKnowledge
{
public:
    GoUctKnowledge(const GoBoard& bd);

    virtual ~GoUctKnowledge();

    /** Compute knowledge for the current position */
    virtual void ProcessPosition(std::vector<SgUctMoveInfo>& moves) = 0;

    /** Add to the existing knowledge for move */
    void Add(SgPoint move, SgUctValue value, SgUctValue count);

    /** Add values and counts to vector of SgUctMoveInfo */
    void AddValuesTo(std::vector<SgUctMoveInfo>& moves) const;

    /** Clear value and count for this move */
    inline void Clear(SgPoint move);

    /** Clear value and count for all moves */
    void ClearValues();

    /** The board for which knowledge is computed */
    inline const GoBoard& Board() const;

    /** Get the current knowledge for move */
    void Get(SgPoint move, SgUctValue& value, SgUctValue& count) const;

    /** Initialize knowledge for move.
        Replaces existing knowledge for this move, if any.
    */
    void Initialize(SgPoint move, SgUctValue value, SgUctValue count);
    
    /** Copy values and counts into vector of SgUctMoveInfo */
    void TransferValues(std::vector<SgUctMoveInfo>& moves) const;
    
private:
    /** @see Board() */
    const GoBoard& m_bd;

    /** Array holding value and count for each point */
    SgArray<SgStatisticsBase<SgUctValue, SgUctValue>, SG_PASS + 1> m_values;

};

//----------------------------------------------------------------------------

inline const GoBoard& GoUctKnowledge::Board() const
{
    return m_bd;
}

inline void GoUctKnowledge::Clear(SgPoint move)
{
    m_values[move].Clear();
}

//----------------------------------------------------------------------------

#endif // GOUCT_KNOWLEDGE_H
