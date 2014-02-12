//----------------------------------------------------------------------------
/** @file FeNestedPattern.h */
//----------------------------------------------------------------------------

#ifndef FE_NESTED_PATTERN_H
#define FE_NESTED_PATTERN_H

#include <iostream>
#include <string>
#include "FePattern.h"
#include "FePatternBase.h"
#include "GoBoard.h"

//----------------------------------------------------------------------------

class FeNestedPattern : public FePattern
{
public:
    FeNestedPattern(int minSize, int maxSize);

    /** Nested pattern centered around p */
    void Define(const GoBoard& bd, SgPoint p);

    /** Return largest size which matches at p */
    int Match(SgPoint p) const;

    int MinSize() const;

    int MaxSize() const;

    static int PointsForSize(int size);

private:
    std::vector<FePattern*> m_patterns;

    int m_minSize;

    int m_maxSize;
};

//----------------------------------------------------------------------------

inline int FeNestedPattern::MinSize() const
{
    return m_minSize;
}

inline int FeNestedPattern::MaxSize() const
{
    return m_maxSize;
}

//----------------------------------------------------------------------------
#endif // FE_NESTED_PATTERN_H
