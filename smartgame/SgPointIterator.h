//----------------------------------------------------------------------------
/** @file SgPointIterator.h
    Class SgPointIterator.
*/
//----------------------------------------------------------------------------

#ifndef SG_POINTITERATOR_H
#define SG_POINTITERATOR_H

#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Iterate through an array of points
    terminated by END_POINT (defined to be zero for performance).
*/
class SgPointIterator
{
public:
    SgPointIterator(const SgPoint* first)
        : m_point(first)
    { }

    virtual ~SgPointIterator() { }

    /** Advance the state of the iteration to the next element. */
    void operator++()
    {
        ++m_point;
    }

    /** Return the value of the current element. */
    SgPoint operator*() const
    {
        return *m_point;
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return *m_point != SG_ENDPOINT;
    }

private:
    const SgPoint* m_point;

    /** Not implemented. */
    SgPointIterator(const SgPointIterator&);

    /** Not implemented. */
    SgPointIterator& operator=(const SgPointIterator&);
};

//----------------------------------------------------------------------------

#endif // SG_POINTITERATOR_H
