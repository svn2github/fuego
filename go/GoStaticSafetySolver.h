//----------------------------------------------------------------------------
/** @file GoStaticSafetySolver.h
    Recognize safe stones and territories statically.
*/
//----------------------------------------------------------------------------

#ifndef GO_STATICSAFETYSOLVER_H
#define GO_STATICSAFETYSOLVER_H

#include "GoBoard.h"
#include "GoRegion.h"
#include "GoRegionBoard.h"

//----------------------------------------------------------------------------

/** Common algorithm for static safety.
    The algorithm is used to implement both Benson's algorithm for
    unconditional safety and a solver using the extended 1-vitality and
    2-vitality definitions from Martin Mueller's thesis [Mueller 95, p. 62-63]
    and from [Mueller 97b].
*/
class GoStaticSafetySolver
{
public:
    /** Constructor. If regions = 0, allocates own. */
    GoStaticSafetySolver(const GoBoard& board, GoRegionBoard* regions = 0);

    /** Destructor, deallocates if there are own regions */
    virtual ~GoStaticSafetySolver();

protected:
    /** @name Accessors */
    // @{

    /** See GoBoard::All */
    const SgPointSet& All(SgBlackWhite color) const;

    /** See GoBoard::AllEmpty */
    const SgPointSet& AllEmpty() const;

    /** See GoBoard::AllPoints */
    const SgPointSet& AllPoints() const;

    /** our board */
    const GoBoard& Board() const;

    // @} // @name


    /** @name Forwarding accessors for GoRegionBoard */
    // @{

    /** See GoRegionBoard::UpToDate */
    virtual bool UpToDate() const;

    /** our regions */
    const GoRegionBoard* Regions() const;

    /** our regions */
    GoRegionBoard* Regions();

    /** See GoRegionBoard::AllBlocks */
    SgListOf<GoBlock>& AllBlocks(SgBlackWhite color);

    /** See GoRegionBoard::AllBlocks */
    const SgListOf<GoBlock>& AllBlocks(SgBlackWhite color) const;

    /** See GoRegionBoard::AllChains */
    SgListOf<GoChain>& AllChains(SgBlackWhite color);

    /** See GoRegionBoard::AllChains */
    const SgListOf<GoChain>& AllChains(SgBlackWhite color) const;

    /** See GoRegionBoard::AllRegions */
    SgListOf<GoRegion>& AllRegions(SgBlackWhite color);

    /** See GoRegionBoard::AllRegions */
    const SgListOf<GoRegion>& AllRegions(SgBlackWhite color) const;

    /** See GoRegionBoard::GetBlock */
    GoBlock* GetBlock(const SgPointSet& boundary, SgBlackWhite color) const;

    // @} // @name


    /** Main step of Benson's algorithm */
    virtual void FindTestSets(SgListOf<SgListOf<GoBlock> >* sets,
                              SgBlackWhite color) const;

    /** Compute closure of blocks set for Benson's algorithm.
        Expand set of blocks until all blocks adjacent to all adjacent
        regions are in set.
        see [Benson] for explanation.
    */
    virtual void FindClosure(SgListOf<GoBlock>* blocks) const;

    /** Compute all GoBlock's and GoRegion's on board*/
    virtual void GenBlocksRegions();

    /** Is r healthy for b? Implements Benson, override for better tests
        Benson's classic healthyness test: all empty points of region must be
        liberties of the block.
    */
    virtual bool RegionHealthyForBlock(const GoRegion& r,
                                       const GoBlock& b) const;

    /** Main function, compute all safe points on board */
    virtual void FindSafePoints(SgBWSet* safe);


    /** Find healthy regions for block, calls RegionHealthyForBlock */
    virtual void FindHealthy(); //

    /** Test if list of Benson blocks forms a living group.
        Each block must have a sure liberty count of at least 2.
        A region provides one sure liberty if it is healthy and its
        boundary consists only of blocks in the list.
    */
    void TestAlive(SgListOf<GoBlock>* blocks, SgBWSet* safe,
                   SgBlackWhite color);

    /** Reduce regions: keep only if completely surrounded by blocks */
    void TestAdjacent(SgListOf<GoRegion>* regions,
                      const SgListOf<GoBlock>& blocks) const;

private:
    /** The board we are computing on */
    const GoBoard& m_board;

    /** Contains the GoRegion's and GoBlock's we are using */
    GoRegionBoard* m_regions;

    /** Did we allocate the GoRegionBoard or did the user supply it? */
    bool m_allocRegion;

    /** not implemented */
    GoStaticSafetySolver(const GoStaticSafetySolver&);

    /** not implemented */
    GoStaticSafetySolver& operator=(const GoStaticSafetySolver&);
};

inline const SgPointSet& GoStaticSafetySolver::All(SgBlackWhite color) const
{
    return m_board.All(color);
}

inline SgListOf<GoBlock>& GoStaticSafetySolver::AllBlocks(SgBlackWhite color)
{
    return Regions()->AllBlocks(color);
}

inline const SgListOf<GoBlock>&
GoStaticSafetySolver::AllBlocks(SgBlackWhite color) const
{
    return Regions()->AllBlocks(color);
}

inline SgListOf<GoChain>& GoStaticSafetySolver::AllChains(SgBlackWhite color)
{
    return Regions()->AllChains(color);
}

inline const SgListOf<GoChain>&
GoStaticSafetySolver::AllChains(SgBlackWhite color) const
{
    return Regions()->AllChains(color);
}

inline const SgPointSet& GoStaticSafetySolver::AllEmpty() const
{
    return m_board.AllEmpty();
}

inline SgListOf<GoRegion>&
GoStaticSafetySolver::AllRegions(SgBlackWhite color)
{
    return Regions()->AllRegions(color);
}

inline const SgListOf<GoRegion>&
GoStaticSafetySolver::AllRegions(SgBlackWhite color) const
{
    return Regions()->AllRegions(color);
}

inline const SgPointSet& GoStaticSafetySolver::AllPoints() const
{
    return m_board.AllPoints();
}

inline const GoBoard& GoStaticSafetySolver::Board() const
{
    return m_board;
}

inline GoBlock* GoStaticSafetySolver::GetBlock(const SgPointSet& boundary,
                                               SgBlackWhite color) const
{
    return Regions()->GetBlock(boundary, color);
}

inline GoRegionBoard* GoStaticSafetySolver::Regions()
{
    SG_ASSERT(m_regions);
    return m_regions;
}

inline const GoRegionBoard* GoStaticSafetySolver::Regions() const
{
    SG_ASSERT(m_regions);
    return m_regions;
}

//----------------------------------------------------------------------------

#endif // GO_STATICSAFETYSOLVER_H
