//----------------------------------------------------------------------------
/** @file SgListUtility.cpp
    See SgListUtility.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgListUtility.h"

#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------

void SgListUtility::Intersection(SgList<int>* list, const SgList<int>& list2)
{
     SgList<int> newList;
     for (SgListIterator<int> it(*list); it; ++it)
     {
         // @todo speed up by hash tags, if used in time-critical code
         if (list2.Contains(*it))
             newList.PushBack(*it);
     }
     newList.SwapWith(list);
}

void SgListUtility::Difference(SgList<int>* list, const SgList<int>& list2)
{
    // @todo speed up by using tags, if used in time-critical code.
    for (SgListIterator<int> it(list2); it; ++it)
        list->Exclude(*it);
}

void SgListUtility::Reverse(SgList<int>* list)
{
    SgList<int> rev;
    for (SgListIterator<int> it(*list); it; ++it)
        rev.Push(*it);
    if (rev.NonEmpty())
    {
        SG_ASSERT(list->Top() == rev.Tail());
        SG_ASSERT(list->Tail() == rev.Top());
        rev.SwapWith(list);
    }
}

//----------------------------------------------------------------------------
void SgVectorUtility::Difference(SgVector<int>* vector,
                                 const SgVector<int>& vector2)
{
    // @todo speed up by using tags, if used in time-critical code.
    for (SgVectorIterator<int> it(vector2); it; ++it)
        vector->Exclude(*it);
}

void SgVectorUtility::Intersection(SgVector<int>* vector,
                                   const SgVector<int>& vector2)
{
     SgVector<int> newVector;
     for (SgVectorIterator<int> it(*vector); it; ++it)
     {
         // @todo speed up by hash tags, if used in time-critical code
         if (vector2.Contains(*it))
             newVector.PushBack(*it);
     }
     newVector.SwapWith(vector);
}
