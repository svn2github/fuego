//----------------------------------------------------------------------------
/** @file GoChain.cpp
    See GoChain.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoChain.h"

#include <iostream>
#include "GoBlock.h"
#include "GoRegion.h"
#include "GoRegionBoard.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

const bool CHECK = SG_CHECK && true,
           HEAVYCHECK = SG_HEAVYCHECK && CHECK && false;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

GoChain::GoChain(const GoChain* c1, const GoChain* c2,
                       GoChainCondition* cond) :
    GoBlock(c1, c1->Stones() | c2->Stones(),
    c1->Healthy()), // AR: cannot do union here, no template -> wrong type.
    m_isSingleBlock(false),
    m_freeLiberties(c1->FreeLiberties() | c2->FreeLiberties())
{
    ++s_alloc;
    SG_ASSERT(c1 != c2);
    m_healthy.Union(c2->Healthy());
    SG_ASSERT(! cond->Overlaps(m_chainConditions));
    m_chainConditions.Append(cond);

    if (cond->UsesLibs())
    {
        SG_ASSERT(m_freeLiberties.Contains(cond->Lib1()));
        m_freeLiberties.Exclude(cond->Lib1());
        SG_ASSERT(m_freeLiberties.Contains(cond->Lib2()));
        m_freeLiberties.Exclude(cond->Lib2());
    }
}


void GoChain::TestFor1Eye(const GoRegionBoard* ra)
{
    SgListOf<GoBlock> blocks;
    GetBlocks(ra, &blocks);
    for (SgListIteratorOf<GoBlock> it(blocks); it; ++it)
        if ((*it)->Has1Eye())
        {
            m_has1Eye = true;
            /* */ return; /* */
        }
    for (SgListIteratorOf<GoRegion> it(ra->AllRegions(Color())); it; ++it)
        if ((*it)->GetFlag(is1vc) && (*it)->Chains().Contains(this))
        {
            m_has1Eye = true;
            /* */ return; /* */
        }

    m_has1Eye = false; 
}

void GoChain::GetBlocks(const GoRegionBoard* ra, 
                        SgListOf<GoBlock>* blocks) const
{
    SgBlackWhite color = Color();
    SgPointSet chainPts = Stones();
    for (SgListIteratorOf<GoBlock> it(ra->AllBlocks(color)); it; ++it)
        if (chainPts.Contains((*it)->Anchor()))
            blocks->Append(*it);
}

bool GoChain::AllEmptyAreLiberties(const SgPointSet& area) const
{
    return (area & m_bd.AllEmpty()).SubsetOf(Liberties());
}

void GoChain::WriteID(std::ostream& stream) const
{
    stream << ' '<< SgBW(Color()) << " GoChain "
           << SgWritePoint(Anchor());
}

void GoChain::Fini()
{
    SG_ASSERT(s_alloc == s_free);
}

void GoChain::Write(std::ostream& stream) const
{
    GoBlock::Write(stream);
    stream << "Chain conditions: ";
    for (SgListIteratorOf<GoChainCondition> it(ChainConditions()); it; ++it)
        stream << **it;
    if (ChainConditions().IsEmpty())
        stream << "none";
    stream << '\n';
} 

//----------------------------------------------------------------------------

int GoChain::s_alloc = 0;
int GoChain::s_free = 0;

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const GoChain& c)
{
    c.Write(stream);
    return stream;
}

//----------------------------------------------------------------------------

bool GoChainCondition::Overlaps(const GoChainCondition& condition) const
{
    if (! UsesLibs() || ! condition.UsesLibs())
        return false; // AR needs some other check.
        
    return   m_lib1 == condition.m_lib1
          || m_lib1 == condition.m_lib2
          || m_lib2 == condition.m_lib1
          || m_lib2 == condition.m_lib2;
}

bool GoChainCondition::Overlaps(const SgListOf<GoChainCondition>& conditions)
    const
{
    for (SgListIteratorOf<GoChainCondition> it(conditions); it; ++it)
    {
        if (Overlaps(**it))
            /* */ return true; /* */
    }
    return false;
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, GoChainConditionType f)
{
    static const char* s_string[nuGoChainConditionType] = 
        {
            "twoLibsInRegion",
            "twoSeparateLibs",
            "chainBySearch"
        };

    stream << s_string[f];
    return stream;
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const GoChainCondition& c)
{
    stream << " GoChainCondition: type = " << c.Type();
    if (c.UsesLibs())
        stream << ", lib1 = " << SgWritePoint(c.Lib1())
               << ", lib2 = " << SgWritePoint(c.Lib2());
    return stream;
}

//----------------------------------------------------------------------------

