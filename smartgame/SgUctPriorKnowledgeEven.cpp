//----------------------------------------------------------------------------
/** @file SgUctPriorKnowledgeEven.cpp
    See SgUctPriorKnowledgeEven.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctPriorKnowledgeEven.h"

using namespace std;

//----------------------------------------------------------------------------

SgUctPriorKnowledgeEven::SgUctPriorKnowledgeEven(std::size_t count)
    : m_count(count)
{
}

void SgUctPriorKnowledgeEven::ProcessPosition(bool& deepenTree)
{
    SG_UNUSED(deepenTree);
}

void SgUctPriorKnowledgeEven::InitializeMove(SgMove move, float& value,
                                             std::size_t& count)
{
    SG_UNUSED(move);
    value = 0.5f;
    count = m_count;
}

//----------------------------------------------------------------------------

SgUctPriorKnowledgeEvenFactory
::SgUctPriorKnowledgeEvenFactory(std::size_t count)
     : m_count(count)
{
}

SgUctPriorKnowledge*
SgUctPriorKnowledgeEvenFactory::Create(SgUctThreadState& state)
{
    SG_UNUSED(state);
    return new SgUctPriorKnowledgeEven(m_count);
}

//----------------------------------------------------------------------------
