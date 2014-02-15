//----------------------------------------------------------------------------
/** @file FeNestedPattern.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeNestedPattern.h"

#include "FePattern.h"
#include "GoSetupUtil.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

FeNestedPattern::FeNestedPattern(int minSize, int maxSize)
: FePattern(PointsForSize(maxSize)),
    m_patterns(),
    m_minSize(minSize),
    m_maxSize(maxSize)
{ }

int FeNestedPattern::PointsForSize(int size)
{
    switch (size)
    {
        case 2: return 4;
        default: return 8 * size; // TODO
    }
}
