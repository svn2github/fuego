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
    Mainly uses GoUctPlayoutPolicy to generate prior knowledge. */
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
private:

    GoUctPlayoutPolicy<GoBoard> m_policy;

    void AddLocalityBonus(GoPointList& emptyPoints, bool isSmallBoard);

    void InitializeForRandomPolicyMove(const GoPointList& empty,
                                       int nuSimulations);
                                       
    void InitializeForGlobalHeuristic(const GoPointList& empty,
                                      const SgPointSet& pattern,
                                      const SgPointSet& atari,
                                      int nuSimulations);

	void InitializeForNonRandomPolicyMove(const GoPointList& empty,
                                          const SgPointSet& pattern,
                                          const SgPointSet& atari,
                                          int nuSimulations);

	/** Temporary variable to hold max. of m_patternGammas for current move */
	float m_maxPatternGamma;

	/** Gamma values used as prior knowledge for pattern moves */
	SgArray<float,SG_MAXPOINT> m_patternGammas;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPRIORKNOWLEDGE_H
