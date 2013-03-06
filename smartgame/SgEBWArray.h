//----------------------------------------------------------------------------
/** @file SgEBWArray.h
    Arrays indexed by color. */
//----------------------------------------------------------------------------

#ifndef SG_EBWARRAY_H
#define SG_EBWARRAY_H

#include "SgBoardColor.h"

//----------------------------------------------------------------------------

/** Array of three values of type T, indexed by SG_BLACK, SG_WHITE, SG_EMPTY.
*/
template <class T>
class SgEBWArray
{
public:
    /** Constructor.
        Constructs elements with the default constructor of type T. */
    SgEBWArray()
    { }

    /** Constructor. Sets all three elements to val. */
    SgEBWArray(const T& val)
    {
        m_array[SG_BLACK] = val;
        m_array[SG_WHITE] = val;
        m_array[SG_EMPTY] = val;
    }

    SgEBWArray(const T& empty, const T& black, const T& white)
    {
        m_array[SG_BLACK] = black;
        m_array[SG_WHITE] = white;
        m_array[SG_EMPTY] = empty;
    }

    const T& operator[](SgEmptyBlackWhite c) const
    {
        SG_ASSERT_EBW(c);
        return m_array[c];
    }

    T& operator[](SgEmptyBlackWhite c)
    {
        SG_ASSERT_EBW(c);
        return m_array[c];
    }

private:
    T m_array[3];
};

//----------------------------------------------------------------------------

#endif // SG_EBWARRAY_H
