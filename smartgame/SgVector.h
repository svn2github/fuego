//----------------------------------------------------------------------------
/** @file SgVector.h
    std::vector-based replacement for SgList.
*/
//----------------------------------------------------------------------------
#ifndef SG_VECTOR_H
#define SG_VECTOR_H

#include <vector>

using std::vector;

template<typename T>
class SgVector
{
public:
    /** Construct empty list. */
    SgVector()
        : m_vec()
    { }
    
    /** Return reference to element.
        @param index Position of the element in range <code>0..length-1</code>.
    */
    T& operator[](int index)
    {
        return m_vec[index];
    }

    /** Return const reference to element.
        @param index Position of the element in range <code>0..length-1</code>.
    */
    const T& operator[](int index) const
    {
        return m_vec[index];
    }

    /** Compare whether the contents of two lists are identical.
        Same length, and the same elements in the same sequence.
    */
    bool operator==(const SgVector<T>& rhs) const
    {
        return m_vec == rhs.m_vec;
    }

    /** Compare whether the contents of two lists are not identical. */
    bool operator!=(const SgVector& rhs) const
    {
        return ! (*this == rhs);
    }
    
    /** Add a single element at the end of the list. */
    void Append(const T& elt)
    {
        PushBack(elt);
    }
    
    /** Append all elements from <code>list</code> at the end of this list. */
    void AppendList(const SgVector<T>& list);
    
    /** Remove all elements in this list. */
    void Clear()
    {
        m_vec.clear();
    }

    /** Append <code>*tail</code> at the end of this list,
        and empty the <code>*tail</code> list.
    */
    void Concat(SgVector<T>* tail);

    /** Check if element is in list.
        @return <code>true</code> if <code>elt</code> is included in this
        list, <code>false</code> if not.
    */
    bool Contains(const T& elt) const;
    
    /** Remove the first occurence of element.
        If <code>elt</code> is in the list, remove the first
        occurence of <code>elt</code> from the list, and return
        <code>true</code>. Otherwise do nothing and return <code>false</code>.
    */
    bool Exclude(const T& elt);

    /** Return whether this list contains zero elements. */
    bool IsEmpty() const
    {
        return Length() == 0;
    }

    /** Test whether a list is as long as a given length. */
    bool IsLength (int length) const
    {
        return Length() == length;
    }

    /** Return the number of elements in this list. */
    int Length() const
    {
        return m_vec.size();
    }
    
    /** Return whether this list contains more than zero elements. */
    bool NonEmpty() const
    {
        return Length() != 0;
    }

    /** Remove the head of the list.
        The list must not be empty.
        @return The head of the list.
    */
    T Pop();

    /** Add a single element at the end of the list. */
    void PushBack(const T& elt)
    {
        m_vec.push_back(elt);
    }

    /** Returns the last element of the list.
        Asserts if the list is empty.
    */
    const T& Tail() const
    {
        SG_ASSERT(NonEmpty());
        return m_vec[m_vec.size() - 1];
    }

    /** Returns the head of the list.
        Asserts if the list is empty.
    */
    const T& Top() const
    {
        SG_ASSERT(NonEmpty());
        return m_vec[0];
    }

private:
    std::vector<T> m_vec;
};

//----------------------------------------------------------------------------
template<typename T>
void SgVector<T>::AppendList(const SgVector<T>& v)
{
    copy(v.m_vec.begin(), v.m_vec.end(), back_inserter(m_vec));
}
    
template<typename T>
void SgVector<T>::Concat(SgVector<T>* tail)
{
    AppendList(*tail);
    tail->Clear();
}

template<typename T>
bool SgVector<T>::Contains(const T& elt) const
{
    typename vector<T>::const_iterator end = m_vec.end();
    typename vector<T>::const_iterator pos = find(m_vec.begin(), end, elt);
    return pos != end;
}

template<typename T>
T SgVector<T>::Pop()
{
    SG_ASSERT(NonEmpty());
    T elt = Top();
    m_vec.erase(m_vec.begin());
    return elt;
}

template<typename T>
bool SgVector<T>::Exclude(const T& elt)
{
    typename vector<T>::iterator end = m_vec.end();
    typename vector<T>::iterator pos = find(m_vec.begin(), end, elt);
    if (pos != end)
    {
        m_vec.erase(pos);
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------

#endif // SG_VECTOR_H