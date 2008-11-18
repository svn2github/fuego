//----------------------------------------------------------------------------
/** @file SgUctPriorKnowledgeEven.cpp
    See SgUctPriorKnowledgeEven.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctPriorKnowledgeEven.h"

using namespace std;

//----------------------------------------------------------------------------

SgUctPriorKnowledgeEven::SgUctPriorKnowledgeEven(float count)
    : m_count(count)
{
}

void SgUctPriorKnowledgeEven::ProcessPosition(bool& deepenTree)
{
    SG_UNUSED(deepenTree);
}

void SgUctPriorKnowledgeEven::InitializeMove(SgMove move, float& value,
                                             float& count)
{
    SG_UNUSED(move);
    value = 0.5f;
    count = m_count;
}

//----------------------------------------------------------------------------

SgUctPriorKnowledgeEvenFactory
::SgUctPriorKnowledgeEvenFactory(float count)
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
