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
#include "SgVector.h"

//----------------------------------------------------------------------------

namespace SgListUtility
{
    /** Keep only elements in list that also occur in list2 */
    void Intersection(SgList<int>* list, const SgList<int>& list2);

    /** Exclude all elements of list2 from list */
    void Difference(SgList<int>* list, const SgList<int>& list2);

    /** Reverse order of elements in list */
    void Reverse(SgList<int>* list);
    
    /** Utility function, as long as both SgList and SgVector used */
    template<typename T>
    void ListToVector(const SgList<T>& list, SgVector<T>& vec);
    
    /** Utility function, as long as both SgList and SgVector used */
    template<typename T>
    void VectorToList(const SgVector<T>& vec, SgList<T>& list);
}

//----------------------------------------------------------------------------
template<typename T>
void SgListUtility::ListToVector(const SgList<T>& list, SgVector<T>& vec)
{
    SG_ASSERT(vec.IsEmpty());
    for (SgListIterator<T> it(list); it; ++it)
    {
        vec.PushBack(*it);
    }
}

template<typename T>
void SgListUtility::VectorToList(const SgVector<T>& vec, SgList<T>& list)
{
    SG_ASSERT(list.IsEmpty());
    for (SgVectorIterator<T> it(vec); it; ++it)
    {
        list.PushBack(*it);
    }
}


//----------------------------------------------------------------------------

#endif // SG_LISTUTILITY_H
