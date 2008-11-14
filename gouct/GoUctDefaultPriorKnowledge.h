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
    : public SgUctPriorKnowledge
{
public:
    GoUctDefaultPriorKnowledge(const GoBoard& bd,
                               const GoUctPlayoutPolicyParam& param);

    void ProcessPosition(bool& deepenTree);

    void InitializeMove(SgMove move, float& value, std::size_t& count);

private:
    const GoBoard& m_bd;

    GoUctPlayoutPolicy<GoBoard> m_policy;

    SgArray<float,SG_PASS+1> m_values;

    SgArray<std::size_t,SG_PASS+1> m_counts;

    bool FindGlobalPatternAndAtariMoves(SgPointSet& pattern,
                                        SgPointSet& atari) const;

    void Initialize(SgPoint p, float value, std::size_t count);
};

//----------------------------------------------------------------------------

class GoUctDefaultPriorKnowledgeFactory
    : public SgUctPriorKnowledgeFactory
{
public:
    /** Stores a reference to param */
    GoUctDefaultPriorKnowledgeFactory(const GoUctPlayoutPolicyParam& param);

    SgUctPriorKnowledge* Create(SgUctThreadState& state);

private:
    const GoUctPlayoutPolicyParam& m_param;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPRIORKNOWLEDGE_H
