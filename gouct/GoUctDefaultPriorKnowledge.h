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

    /** Get the number of move types.
        @see GetType
    */
    int GetNuTypes() const;

    /** Get the type of a move.
        A move type is a class of moves that are initialized with a certain
        value/count pair. Move types are dependent on the implementation of
        this class and identified by an integer between 0 and
        GetNuTypes() - 1. The move type is interesting for debugging and for
        tuning the value/count initialization pairs used in the
        implementation.
    */
    int GetType(SgPoint p) const;

private:
    const GoBoard& m_bd;

    GoUctPlayoutPolicy<GoBoard> m_policy;

    SgArray<float,SG_PASS+1> m_values;

    SgArray<std::size_t,SG_PASS+1> m_counts;

    SgArray<std::size_t,SG_PASS+1> m_types;

    bool FindGlobalPatternAndAtariMoves(SgPointSet& pattern,
                                        SgPointSet& atari) const;

    void Initialize(SgPoint p, float value, std::size_t count, int type);
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
