//----------------------------------------------------------------------------
/** @file GoChain.h
    A chain contained in a @see GoRegionBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_CHAIN_H
#define GO_CHAIN_H

#include <iosfwd>
#include "GoBlock.h"
#include "GoBoard.h"
#include "SgBlackWhite.h"
#include "SgPoint.h"
#include "SgList.h"

//----------------------------------------------------------------------------

class GoRegionBoard;

//----------------------------------------------------------------------------

/** The reason why two blocks or chains are merged into a single chain */
enum GoChainConditionType
{
    twoLibsInRegion, twoSeparateLibs,
    chainBySearch,
    nuGoChainConditionType
};

std::ostream& operator<<(std::ostream& stream, GoChainConditionType f);

/** Condition that explains why two blocks or chains can be merged 
    into a new, larger chain.
*/
class GoChainCondition
{
public:
     
    /** A condition with no parameters */
   GoChainCondition(GoChainConditionType type)
        : m_type(type), 
          m_lib1(SG_NULLPOINT),
          m_lib2(SG_NULLPOINT)
    {
        SG_ASSERT(type==chainBySearch);
    }

    /** A condition depending on two liberties */
    GoChainCondition(GoChainConditionType type, SgPoint lib1, SgPoint lib2)
        : m_type(type), 
          m_lib1(lib1),
          m_lib2(lib2)
    {
        SG_ASSERT(type == twoLibsInRegion);
        // @todo: || type==twoSeparateLibs
    }

    /** Is there a potential conflict between this and condition? */
    bool Overlaps(const GoChainCondition& condition) const;
    
    /** Is there a potential conflict between this and conditions? */
    bool Overlaps(const SgListOf<GoChainCondition>& conditions) const;
    
    /** Are liberties used? @todo make a base class without libs */
    bool UsesLibs() const {return m_type != chainBySearch;}
    
    GoChainConditionType Type() const {return m_type;}

    /** first liberty used in condition */
    SgPoint Lib1() const
    {
        SG_ASSERT(m_type != chainBySearch);
        return m_lib1;
    }

    /** second liberty used in condition */
    SgPoint Lib2() const
    {
        SG_ASSERT(m_type != chainBySearch);
        return m_lib2;
    }

private:
    /** Type @todo replace by class */
    GoChainConditionType m_type;
    
    /** condition depends on these two liberties */
    SgPoint m_lib1, m_lib2;
};

std::ostream& operator<<(std::ostream& stream, const GoChainCondition& c);

//----------------------------------------------------------------------------

/** Set of GoBlock's that are a connected unit */
class GoChain : public GoBlock
{
public:
    
    /** Constructor from single block */
    GoChain(const GoBlock* b, const GoBoard& board)
    :   GoBlock(b->Color(), b->Anchor(), board), 
        m_isSingleBlock(true),
        m_freeLiberties(b->Liberties())
    {   ++s_alloc;
        if (b->IsSafe()) SetToSafe();
    }
    
    /** Constructor by merging two chains */
    GoChain(const GoChain* c1, const GoChain* c2,
               GoChainCondition* cond);
    
    /** Destructor */
    virtual ~GoChain()
    {
        ++s_free;
    }

    /** For debugging */
    void CheckConsistency(const GoBoard& bd) const;

    /** Write data of chain */
    void Write(std::ostream& out) const;

    /** Write short identifier */
    void WriteID(std::ostream& out) const;

    /** are all empty points in area liberties of this? */
    virtual bool AllEmptyAreLiberties(const SgPointSet& area) const;
    
    /** Does the chain consist of a single block, or more than one?*/
    bool IsSingleBlock() const {return m_isSingleBlock;}
    
    /** does chain have one eye? Uses blocks' eyes and 1vc regions */
    void TestFor1Eye(const GoRegionBoard* ra);

   /** Free liberties are liberties not used to define chains */
    const SgPointSet& FreeLiberties() const {return m_freeLiberties;}

    /** See m_chainConditions */
    const SgListOf<GoChainCondition>& ChainConditions() const
    {return m_chainConditions;}
    
    /** returns list of all blocks in chain */
    void GetBlocks(const GoRegionBoard* ra,
                   SgListOf<GoBlock>* blocks) const;

    /** class Finalization */
    static void Fini();
private:
    
    /** Does the chain consist of a single block, or more than one?*/
    bool m_isSingleBlock;
    
    /** Free liberties are liberties not used to define chains */
    SgPointSet m_freeLiberties; // .
    
    /** All conditions used to create chain */
    SgOwnerListOf<GoChainCondition> m_chainConditions;
    
    /** Used for debugging only */
    static int s_alloc, s_free;
};

std::ostream& operator<<(std::ostream& stream, const GoChain& c);

//----------------------------------------------------------------------------

#endif // GO_CHAIN_H
