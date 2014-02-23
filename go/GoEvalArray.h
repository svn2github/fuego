//----------------------------------------------------------------------------
/** @file GoEvalArray.h
 Array indexed by legal moves including SG_PASS. */
//----------------------------------------------------------------------------

#ifndef GO_EVAL_ARRAY_H
#define GO_EVAL_ARRAY_H

#include <boost/static_assert.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "SgArray.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------
const int GO_MAX_MOVE = SG_PASS + 1;

BOOST_STATIC_ASSERT(SG_PASS > SG_MAXPOINT);

/** An array of GO_MAX_MOVE values of type T, indexed by Go moves
    including moves on the board and SG_PASS.
 Also enforces that all elements are initialized in the constructor,
 either with T(0), if T can be constructed in such a way or be providing
 an initialization value. */
template<class T>
class GoEvalArray
    : public SgArray<T,GO_MAX_MOVE>
{
    public:
    /** Constructor; values are initialized by default value. */
    GoEvalArray();
    
    /** Constructor; values are initialized by init value. */
    GoEvalArray(const T& value);
    
    /** Constructor; initialized as copy of other point array. */
    GoEvalArray(const GoEvalArray& array);
    
    void Write(std::ostream& out, SgGrid boardSize) const;
};

template<class T>
inline GoEvalArray<T>::GoEvalArray()
{ }

template<class T>
inline GoEvalArray<T>::GoEvalArray(const T& value)
    : SgArray<T,GO_MAX_MOVE>(value)
{ }

template<class T>
inline GoEvalArray<T>::GoEvalArray(const GoEvalArray& array)
    : SgArray<T,GO_MAX_MOVE>(array)
{ }

template<class T>
inline void GoEvalArray<T>::Write(std::ostream& out,
                                  SgGrid boardSize) const
{
    SgWriteBoardFromArray(out, *this, boardSize);
    out << "Pass: " /* TODO << operator[](SG_PASS) */ << '\n';
}


//----------------------------------------------------------------------------

#endif // GO_EVAL_ARRAY_H
