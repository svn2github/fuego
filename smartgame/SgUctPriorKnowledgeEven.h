//----------------------------------------------------------------------------
/** @file SgUctPriorKnowledgeEven.h */
//----------------------------------------------------------------------------

#ifndef SGUCTPRIORKNOWLEDGEEVEN_H
#define SGUCTPRIORKNOWLEDGEEVEN_H

#include "SgUctSearch.h"

//----------------------------------------------------------------------------

/** Initialize states with the even-game heuristic.
    Takes advantage of the fact that most positions encountered on-policy are
    likely to be close. Initializes moves with the value 0.5 and a
    configurable count (by default 50, which worked well in 9x9-Go).
*/
class SgUctPriorKnowledgeEven
    : public SgUctPriorKnowledge
{
public:
    /** Constructor.
        @param count The count to initialize moves with (e.g. 50, which
        worked well for 9x9-Go in the Gelly/Silver 2007 paper)
    */
    SgUctPriorKnowledgeEven(std::size_t count);

    void InitializeMove(SgMove move, float& value, std::size_t& count);

private:
    const std::size_t m_count;
};

//----------------------------------------------------------------------------

class SgUctPriorKnowledgeEvenFactory
    : public SgUctPriorKnowledgeFactory
{
public:
    SgUctPriorKnowledgeEvenFactory(std::size_t count);

    SgUctPriorKnowledge* Create(SgUctThreadState& state);

private:
    const std::size_t m_count;
};

//----------------------------------------------------------------------------

#endif // SGUCTPRIORKNOWLEDGEEVEN_H
