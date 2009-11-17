//----------------------------------------------------------------------------
/** @file SgProbCut.cpp
    See SgProbCut.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgProbCut.h"

#include "SgMath.h"
#include "SgSearch.h"

//----------------------------------------------------------------------------

bool SgProbCut::ProbCut(SgSearch& search, int depth, int alpha, int beta,
                       SgVector<SgMove>* sequence, bool* isExactValue,
                       int* value)
{
    SG_ASSERT(IsEnabled());
    SetEnabled(false);

    Cutoff c;
    int index = 0;
    while (GetCutoff(depth / SgSearch::DEPTH_UNIT, index++, c))
    {
        SgVector<SgMove> seq;
        bool isExact;
        float threshold = GetThreshold();

        if (beta < SgSearch::SG_INFINITY-1)
        {
            float b = (+threshold * c.sigma + beta - c.b) / c.a;
            int bound = SgMath::RoundToInt(b);
            int res = search.SearchEngine(c.shallow * SgSearch::DEPTH_UNIT,
                                   bound-1, bound, &seq, &isExact);
            if (res >= bound)
            {
                SetEnabled(true);
                sequence->Concat(&seq);
                *isExactValue = isExact;
                *value = beta;
                return true;
            }
        }

        if (alpha > -SgSearch::SG_INFINITY + 1)
        {
            float b = (-threshold * c.sigma + alpha - c.b) / c.a;
            int bound = SgMath::RoundToInt(b);
            int res = search.SearchEngine(c.shallow * SgSearch::DEPTH_UNIT,
                                   bound, bound+1, &seq, &isExact);

            if (res <= bound)
            {
                SetEnabled(true);
                sequence->Concat(&seq);
                *isExactValue = isExact;
                *value = alpha;
                return true;
            }
        }
    }
    SetEnabled(true);
    return false;
}

//----------------------------------------------------------------------------
