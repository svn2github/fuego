//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTPRIORKNOWLEDGE_H
#define GOUCT_DEFAULTPRIORKNOWLEDGE_H

#include "GoUctPlayoutPolicy.h"
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Default prior knowledge heuristic.
    Mainly uses GoUctPlayoutPolicy to generate prior knowledge.
*/
class GoUctDefaultPriorKnowledge
{
public:
    GoUctDefaultPriorKnowledge(const GoBoard& bd,
                               const GoUctPlayoutPolicyParam& param);

    void ProcessPosition(std::vector<SgMoveInfo>& moves);

private:
    const GoBoard& m_bd;

    GoUctPlayoutPolicy<GoBoard> m_policy;

    SgArray<SgStatisticsBase<float,std::size_t>,SG_PASS+1> m_values;

    void Add(SgPoint p, float value, std::size_t count);

    void AddLocalityBonus(GoPointList& emptyPoints, bool isSmallBoard);

    bool FindGlobalPatternAndAtariMoves(SgPointSet& pattern,
                                        SgPointSet& atari,
                                        GoPointList& empty) const;

    void Initialize(SgPoint p, float value, std::size_t count);
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPRIORKNOWLEDGE_H
