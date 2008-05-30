//----------------------------------------------------------------------------
/** @file GoBensonSolver.cpp
    See GoBensonSolver.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBensonSolver.h"

#include "GoBlock.h"
#include "SgDebug.h"

const bool DEBUG_BENSON = false;

//----------------------------------------------------------------------------
 
void GoBensonSolver::FindSafePoints(SgBWSet* safe)
{
    safe->Clear();
    GoStaticSafetySolver::FindSafePoints(safe);

    if (DEBUG_BENSON)
    {
        SgPointSet proved = safe->Both();
        int totalRegions = 0;
        int provedRegions = 0;
        int totalBlocks = 0;
        int provedBlocks = 0;  
    
        for (SgBWIterator it; it; ++it)
        {
            SgBlackWhite color(*it);
            for (SgListIteratorOf<GoRegion> it(AllRegions(color)); it; ++it)
            {
                ++totalRegions;
                if ((*it)->Points().SubsetOf(proved))
                    ++provedRegions;
            }
            for (SgListIteratorOf<GoBlock> it(AllBlocks(color)); it; ++it)
            {
                ++totalBlocks;
                if (proved.Overlaps((*it)->Stones()))
                    ++provedBlocks;
            }
        }
    
        SgDebug() << "\n****GoBensonSolver Result****"
                  << "\nTotal proved points = " << proved.Size()
                  << "\nTotal regions =  " << totalRegions
                  << " Proved regions = " << provedRegions
                  << "\nTotal blocks =  " << totalBlocks
                  << " Proved blocks = " << provedBlocks 
                  << "\n";                 
    }
}

