//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTPRIORKNOWLEDGE_H
#define GOUCT_DEFAULTPRIORKNOWLEDGE_H

#include "GoUctDefaultPlayoutPolicy.h"
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Default prior knowledge heuristic.
    Mainly uses GoUctDefaultPlayoutPolicy to generate prior knowledge.
*/
class GoUctDefaultPriorKnowledge
    : public SgUctPriorKnowledge
{
public:
    GoUctDefaultPriorKnowledge(const GoBoard& bd,
                               const GoUctDefaultPlayoutPolicyParam& param);

    void ProcessPosition(bool& deepenTree);

    void InitializeMove(SgMove move, float& value, std::size_t& count);

private:
    const GoBoard& m_bd;

    GoUctDefaultPlayoutPolicy<GoBoard> m_policy;

    SgArray<float,SG_PASS+1> m_values;

    SgArray<std::size_t,SG_PASS+1> m_counts;

    void Initialize(SgPoint p, float value, std::size_t count);
};

//----------------------------------------------------------------------------

class GoUctDefaultPriorKnowledgeFactory
    : public SgUctPriorKnowledgeFactory
{
public:
    /** Stores a reference to param */
    GoUctDefaultPriorKnowledgeFactory(const GoUctDefaultPlayoutPolicyParam&
                                      param);

    SgUctPriorKnowledge* Create(SgUctThreadState& state);

private:
    const GoUctDefaultPlayoutPolicyParam& m_param;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTPRIORKNOWLEDGE_H
