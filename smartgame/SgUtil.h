//----------------------------------------------------------------------------
/** @file SgUtil.h
    Small utility functions
*/
//----------------------------------------------------------------------------

#ifndef SG_UTIL_H
#define SG_UTIL_H

//----------------------------------------------------------------------------

namespace SgUtil {

template<class T>
inline void ForceInRange(const T& min, T* p, const T& max)
{
    if (*p < min)
        *p = min;
    if (*p > max)
        *p = max;
}

template <class T>
inline bool InRange(const T& i, const T& from, const T& to) 
{
    return (i >= from) && (i <= to);
}

template<class T>
inline void LowerLimit(T& x, const T& limit)
{
    if (x < limit)
        x = limit;
}

template<class T>
inline void UpperLimit(T& x, const T& limit)
{
    if (x > limit)
        x = limit;
}

} // namespace SgUtil

//----------------------------------------------------------------------------

#endif // SG_UTIL_H

