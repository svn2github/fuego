//----------------------------------------------------------------------------
/** @file SgListUtility.h
    %List utility classes.
    
    List operations that are useful but not basic enough to be included 
    in SgList itself.
*/
//----------------------------------------------------------------------------

#ifndef SG_LISTUTILITY_H
#define SG_LISTUTILITY_H

#include "SgList.h"

//----------------------------------------------------------------------------

namespace SgListUtility
{
    /** Keep only elements in list that also occur in list2 */
    void Intersection(SgList<int>* list, const SgList<int>& list2);

    /** Exclude all elements of list2 from list */
    void Difference(SgList<int>* list, const SgList<int>& list2);

    /** Reverse order of elements in list */
    void Reverse(SgList<int>* list);
}

//----------------------------------------------------------------------------

#endif // SG_LISTUTILITY_H
