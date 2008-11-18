//----------------------------------------------------------------------------
/** @file SgUctPriorKnowledgeEven.h */
//----------------------------------------------------------------------------

#ifndef SG_UCTPRIORKNOWLEDGEEVEN_H
#define SG_UCTPRIORKNOWLEDGEEVEN_H

#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Initialize states with the even-game heuristic.
    Takes advantage of the fact that most positions encountered on-policy are
    likely to be close. Initializes moves with the value 0.5 and a
    configurable count (by default 50, which worked well in 9x9-Go).
    @ingroup sguctgroup
*/
class SgUctPriorKnowledgeEven
    : public SgUctPriorKnowledge
{
public:
    /** Constructor.
        @param count The count to initialize moves with (e.g. 50, which
        worked well for 9x9-Go in the Gelly/Silver 2007 paper)
    */
    SgUctPriorKnowledgeEven(float count);

    void ProcessPosition(bool& deepenTree);

    void InitializeMove(SgMove move, float& value, float& count);

private:
    const float m_count;
};

//----------------------------------------------------------------------------

/** Factory for SgUctPriorKnowledgeEven.
    @ingroup sguctgroup
*/
class SgUctPriorKnowledgeEvenFactory
    : public SgUctPriorKnowledgeFactory
{
public:
    SgUctPriorKnowledgeEvenFactory(float count);

    SgUctPriorKnowledge* Create(SgUctThreadState& state);

private:
    const float m_count;
};

//----------------------------------------------------------------------------

#endif // SG_UCTPRIORKNOWLEDGEEVEN_H
