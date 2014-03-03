//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTPRIORKNOWLEDGE_H
#define GOUCT_DEFAULTPRIORKNOWLEDGE_H

#include "GoBoard.h"
#include "GoUctKnowledge.h"
#include "GoUctPlayoutPolicy.h"

//----------------------------------------------------------------------------

/** Default prior knowledge heuristic.
    Generates prior knowledge from:
    1. GoUctPlayoutPolicy, including pattern gamma values
    2. Global pattern gammas
    3. bonus near block of last move - nearness measured by cfg metric
    4. bonus for large sideextensions in the opening
*/
class GoUctDefaultPriorKnowledge
: public GoUctKnowledge
{
public:
    GoUctDefaultPriorKnowledge(const GoBoard& bd,
                               const GoUctPlayoutPolicyParam& param);

    void ProcessPosition(std::vector<SgUctMoveInfo>& moves);

    bool FindGlobalPatternAndAtariMoves(SgPointSet& pattern,
                                        SgPointSet& atari,
                                        GoPointList& empty);

    void SetPriorWeight(float weight);

private:

    GoUctPlayoutPolicy<GoBoard> m_policy;

    void AddBonusNearPoint(GoPointList& emptyPoints,
                           SgUctValue count,
                           SgPoint focus,
                           SgUctValue v1,
                           SgUctValue v2,
                           SgUctValue v3,
                           bool addPass
                           );
    
    void AddLocalityBonus(GoPointList& emptyPoints, bool isSmallBoard);

    void AddOpeningBonus();
    
    void InitializeForRandomPolicyMove(const GoPointList& empty,
                                       SgUctValue nuSimulations);
                                       
    void InitializeForGlobalHeuristic(const GoPointList& empty,
                                      const SgPointSet& pattern,
                                      const SgPointSet& atari,
                                      SgUctValue nuSimulations);

	void InitializeForNonRandomPolicyMove(const GoPointList& empty,
                                          const SgPointSet& pattern,
                                          const SgPointSet& atari,
                                          SgUctValue nuSimulations);

	/** Temporary variable to hold max. of m_patternGammas for current move */
	float m_maxPatternGamma;

    /** Tunable parameter - weight to multiply everything by. */
    float m_defaultPriorWeight;

	/** Gamma values used as prior knowledge for pattern moves */
	SgArray<float,SG_MAXPOINT> m_patternGammas;
};

//----------------------------------------------------------------------------

inline void GoUctDefaultPriorKnowledge::SetPriorWeight(float weight)
{
    m_defaultPriorWeight = weight;
}

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPRIORKNOWLEDGE_H
