//----------------------------------------------------------------------------
/** @file SgEBWArray.h
    Arrays indexed by color.
*/
//----------------------------------------------------------------------------

#ifndef SG_EBWARRAY_H
#define SG_EBWARRAY_H

#include "SgBoardColor.h"

//----------------------------------------------------------------------------

/** An array of three values of type T, indexed by SG_EMPTY, SG_BLACK and
    SG_WHITE.
    Stores index SG_EMPTY (=4) at array[0].
*/
template <class T>
class SgEBWArray
{
public:
    /** Constructor.
        Constructs elements with the default constructor of type T.
        @note Previously, EBWArray automatically initialized primitive types
        like ints or pointers with 0, and there was a second class
        EBWConstrArray used for non-primitive types. This has changed,
        because it is not the standard semantics for container classes in C++,
        and because it does not allow use cases with incremental
        initialization after construction. If you want to initialize for
        example an SgBWArray<int> with 0, use the constructor that takes a
        default value.
    */
    SgEBWArray()
    {
    }

    SgEBWArray(const T& val)
    {
        m_array[0] = val;
        m_array[1] = val;
        m_array[2] = val;
    }

    SgEBWArray(const T& empty, const T& black, const T& white)
    {
        m_array[0] = empty;
        m_array[1] = black;
        m_array[2] = white;
    }

    const T& operator[](int i) const
    {
        SG_ASSERT_EBW(i);
        return i == SG_EMPTY ? m_array[0] : m_array[i];
    }

    T& operator[](int i)
    {
        SG_ASSERT_EBW(i);
        return i == SG_EMPTY? m_array[0] : m_array[i];
    }

private:
    T m_array[3];
};

//----------------------------------------------------------------------------

#endif // SG_EBWARRAY_H
