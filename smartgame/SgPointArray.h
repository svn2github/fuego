//----------------------------------------------------------------------------
/** @file SgPointArray.h
    Array indexed by points. */
//----------------------------------------------------------------------------

#ifndef SG_POINTARRAY_H
#define SG_POINTARRAY_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include "SgArray.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** An array of SG_MAXPOINT values of type T, indexed by SgPoint.
    Also enforces that all elements are initialized in the constructor,
    either with T(0), if T can be constructed in such a way or be providing
    an initialization value. */
template<class T>
class SgPointArray
    : public SgArray<T,SG_MAXPOINT>
{
public:
    /** Constructor; values are initialized by default value. */
    SgPointArray();

    /** Constructor; values are initialized by init value. */
    SgPointArray(const T& value);

    /** Constructor; initialized as copy of other point array. */
    SgPointArray(const SgPointArray& pointArray);
};

template<class T>
inline SgPointArray<T>::SgPointArray()
{ }

template<class T>
inline SgPointArray<T>::SgPointArray(const T& value)
    : SgArray<T,SG_MAXPOINT>(value)
{ }

template<class T>
inline SgPointArray<T>::SgPointArray(const SgPointArray& pointArray)
    : SgArray<T,SG_MAXPOINT>(pointArray)
{ }

//----------------------------------------------------------------------------
/** Write data for a Go board stored in an array.
 Computes the maximum string representation length of each element in the
 array to write out aligned columns with minimum space in between. */
template<typename T, int SIZE>
std::ostream& SgWriteBoardFromArray(std::ostream& out,
                                    const SgArray<T,SIZE>& array,
                                    SgGrid boardSize)
{
    std::ostringstream buffer;
    int maxLength = 0;
    for (SgGrid row = 1; row <= boardSize; ++row)
    for (SgGrid col = 1; col <= boardSize; ++col)
    {
        buffer.str("");
        buffer << array[SgPointUtil::Pt(col, row)];
        int length = static_cast<int>(buffer.str().length());
        maxLength = std::max(maxLength, length);
    }
    for (SgGrid row = boardSize; row >= 1; --row)
    {
        for (SgGrid col = 1; col <= boardSize; ++col)
        {
            SgPoint point = SgPointUtil::Pt(col, row);
            out << std::setw(maxLength) << array[point];
            if (col < boardSize)
            out << ' ';
        }
        out << '\n';
    }
    return out;
}

//----------------------------------------------------------------------------
/** Write a point array. See SgWriteBoardFromArray */
template<typename T>
class SgWritePointArray
{
public:
    SgWritePointArray(const SgPointArray<T>& array, SgGrid boardSize)
        : m_boardSize(boardSize),
          m_array(array)
    { }

    std::ostream& Write(std::ostream& out) const;

private:
    SgGrid m_boardSize;

    const SgPointArray<T>& m_array;
};

template<typename T>
std::ostream& SgWritePointArray<T>::Write(std::ostream& out) const
{
    SgWriteBoardFromArray(out, m_array, m_boardSize);
    return out;
}

/** @relatesalso SgWritePointArray */
template<typename T>
std::ostream& operator<<(std::ostream& out,
                         const SgWritePointArray<T>& write)
{
    return write.Write(out);
}

//----------------------------------------------------------------------------
template<typename FLOAT, int SIZE>
std::ostream& SgWriteBoardFromArrayFloat(std::ostream& out,
                                         const SgArray<FLOAT,SIZE>& array,
                                         SgGrid boardSize,
                                         bool fixed, int precision)
{
    SgArray<std::string,SIZE> stringArray;
    std::ostringstream buffer;
    if (fixed)
        buffer << std::fixed;
    buffer << std::setprecision(precision);
    for (SgGrid row = 1; row <= boardSize; ++row)
    for (SgGrid col = 1; col <= boardSize; ++col)
    {
        buffer.str("");
        SgPoint p = SgPointUtil::Pt(col, row);
        buffer << array[p];
        stringArray[p] = buffer.str();
    }
    SgWriteBoardFromArray(out, stringArray, boardSize);
    return out;
}

//----------------------------------------------------------------------------

/** Write a float point array.
    Enhanced version of SgWritePointArray for float or double types.
    Allows to specify some formatting options for floating point numbers. */
template<typename FLOAT>
class SgWritePointArrayFloat
{
public:
    SgWritePointArrayFloat(const SgPointArray<FLOAT>& array, SgGrid boardSize,
                           bool fixed, int precision)
        : m_fixed(fixed),
          m_precision(precision),
          m_boardSize(boardSize),
          m_array(array)
    { }

    std::ostream& Write(std::ostream& out) const;

private:
    bool m_fixed;

    int m_precision;

    SgGrid m_boardSize;

    const SgPointArray<FLOAT>& m_array;
};

template<typename FLOAT>
std::ostream& SgWritePointArrayFloat<FLOAT>::Write(std::ostream& out) const
{
    SgWriteBoardFromArrayFloat(out, m_array, m_boardSize,
                               m_fixed, m_precision);
    return out;
}

/** @relatesalso SgWritePointArrayFloat */
template<typename FLOAT>
std::ostream& operator<<(std::ostream& out,
                         const SgWritePointArrayFloat<FLOAT>& write)
{
    return write.Write(out);
}

//----------------------------------------------------------------------------

#endif // SG_POINTARRAY_H
