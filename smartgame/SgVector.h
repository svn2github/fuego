//----------------------------------------------------------------------------
/** @file SgVector.h
    std::vector-based replacement for SgList.
*/
//----------------------------------------------------------------------------
#ifndef SG_VECTOR_H
#define SG_VECTOR_H

#include <functional>
#include <vector>

using std::greater;
using std::less;
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
    
    /** delete element at specified index */
    void DeleteAt(int index);

    /** Remove the first occurence of element.
        If <code>elt</code> is in the list, remove the first
        occurence of <code>elt</code> from the list, and return
        <code>true</code>. Otherwise do nothing and return <code>false</code>.
    */
    bool Exclude(const T& elt);

    /** Find position of element.
        @returns The position of <code>elt</code> in the list,
        in range <code>0..length-1</code>. Returns -1 if <code>elt</code>
        is not in the list.
    */
    int Index(const T& elt) const;

    /** Includes the element in a ascending sorted list at the right place.
        Does nothing and returns <code>false</code> if the element is
        already in the list;
        returns <code>true</code> if the element is inserted.
        @todo made two separate functions for efficiency, should be
        a template taking a compare template arg.
        Same for <code>Merge()</code> below.
    */
    bool Insert(const T& elt);

    /** Return whether this list contains zero elements. */
    bool IsEmpty() const
    {
        return m_vec.empty();
    }

    /** Test whether a list is as long as a given length. */
    bool IsLength (int length) const
    {
        return Length() == length;
    }

    /** Returns whether the list is sorted in ascending order. */
    bool IsSorted(bool ascending = true) const;

    /** Return the number of elements in this list. */
    int Length() const
    {
        return m_vec.size();
    }
    
    /** Test whether a list is as long as or longer than a given length.
    */ 
    bool MinLength(int length) const
    {
        return Length() >= length;
    }

    /** Test whether a list is shorter than or equal to a given length.
    */ 
    bool MaxLength(int length) const
    {
        return Length() <= length;
    }

    /** Return whether this list contains more than zero elements. */
    bool NonEmpty() const
    {
        return ! IsEmpty();
    }

    /** Remove the head of the list.
        The list must not be empty.
        @return The head of the list.
        @deprecated Don't use this function; it is slow. 
        Only exists for SgList compatibility.
    */
    T Pop();

    /** Remove the last element of the list.
        The list must not be empty.
    */
    void PopBack();

    /** Insert element at the beginning of the vector.
        @deprecated Don't use this function; it is slow.
        Only exists for SgList compatibility.
    */
    void Push(const T& elt);

    /** Add a single element at the end of the list. */
    void PushBack(const T& elt)
    {
        m_vec.push_back(elt);
    }

    /** Clear this list and set it to contain only <code>elt</code>. */
    void SetTo(const T& elt)
    {
        Clear();
        PushBack(elt);
    }

    /** Swap the entire contents of this list with <code>*list</code>. */
    void SwapWith(SgVector<T>* vec)
    {
        std::swap(m_vec, vec->m_vec);
    }

    /** Returns the last element of the list.
        Asserts if the list is empty.
    */
    const T& Tail() const
    {
        SG_ASSERT(NonEmpty());
        return m_vec.back();
    }

    /** Returns the Nth-last element of the list. It must exist.
    */
    const T& TopNth(int index) const
    {
        SG_ASSERT(NonEmpty());
        SG_ASSERT(index >= 1);
        SG_ASSERT(index <= static_cast<int>(m_vec.size()));
        return m_vec[m_vec.size() - index];
    }

    /** Returns the head of the list.
        Asserts if the list is empty.
    */
    const T& Top() const
    {
        SG_ASSERT(NonEmpty());
        return m_vec[0];
    }

    std::vector<T>& Vector()
    {
        return m_vec;
    }
    
    const std::vector<T>& Vector() const
    {
        return m_vec;
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
void SgVector<T>::DeleteAt(int index)
{
    SG_ASSERT(index >= 0);
    SG_ASSERT(index < Length());
    m_vec.erase(m_vec.begin() + index);
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

template<typename T>
int SgVector<T>::Index(const T& elt) const
{
    typename vector<T>::const_iterator end = m_vec.end();
    typename vector<T>::const_iterator pos = find(m_vec.begin(), end, elt);
    if (pos == end)
        return -1;
    else
        return pos - m_vec.begin();
}

template<typename T>
bool SgVector<T>::Insert(const T& elt)
{
    SG_ASSERT(IsSorted());
    typename vector<int>::iterator location = 
    lower_bound( m_vec.begin(), m_vec.end(), elt);

    if (*location == elt)
        return false;
    else
    {
        m_vec.insert(location, elt);
        SG_ASSERT(IsSorted());
    }
    return true;
}

template<typename T>
bool SgVector<T>::IsSorted(bool ascending) const
{
    typename vector<T>::const_iterator result;
    if (ascending)
        result = adjacent_find(m_vec.begin(), m_vec.end(), greater<T>());
    else
        result = adjacent_find(m_vec.begin(), m_vec.end(), less<T>());
    return result == m_vec.end();
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
void SgVector<T>::PopBack()
{
    SG_ASSERT(NonEmpty());
    m_vec.pop_back();
}

template<typename T>
void SgVector<T>::Push(const T& elt)
{
    m_vec.insert(m_vec.begin(), elt);
}


//----------------------------------------------------------------------------
/** List iterator.
    More concise way to iterate (from "Large-Scale C++ Software Design" by
    John Lakos):
    <pre>
      for (SgListIterator<T> it(list); it; ++it) { ... it() ... }
    </pre>
    Better performance because every method is inline.
*/
template<typename T>
class SgVectorIterator
{
public:
    /** Create a list iterator to iterate through list. */
    SgVectorIterator(const SgVector<T>& vec)
        : m_vec(vec),
          m_it(m_vec.Vector().begin())
    { }

    /** Copy current state of iterator.
        Useful for creating a new iterator that only runs from the current
        position to the list end. See <code>UniqueElements()</code>
        for an example.
    */
    SgVectorIterator(const SgVectorIterator& it)
        : m_vec(it.m_vec)
    { }

    virtual ~SgVectorIterator() { }

    /** Advance the state of the iteration to the next element. */
    SgVectorIterator& operator++()
    {
        ++m_it;
        return *this;
    }

    /** Return the value of the current element. */
    const T& operator*() const
    {
        SG_ASSERT(*this);
        return *m_it;
    };

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_it != m_vec.Vector().end(); // @todo cache end.
    }

private:
    const SgVector<T>& m_vec;

    typename vector<T>::const_iterator m_it;
    
    /** not implemented */
    SgVectorIterator& operator=(const SgVectorIterator&);
};

/** Typed list of pointers to T. Pointers cannot be 0. */
template<class T>
class SgVectorOf
    : public SgVector<void*>
{
public:

    T* operator[] (int index) const
    {
        return static_cast<T*>(SgVector<void*>::operator[](index));
    }

    void Append(const T* element)
    {
        SG_ASSERT(element);
        SgVector<void*>::Append(GetVoidPtr(element));
    }
    
    bool Contains(const T* element) const
    {
        SG_ASSERT(element);
        return SgVector<void*>::Contains(GetVoidPtr(element));
    }

    /** Append <code>elt</code> at the end of the list if it's not
        already in the list. */
    void Include(const T* element)
    {
        SG_ASSERT(element);
        if (! Contains(element))
            Append(element);
    }

    bool Exclude(const T* element)
    {
        return SgVector<void*>::Exclude(GetVoidPtr(element));
    }

    void Exclude(const SgVectorOf<T>& list)
    {
        SgVector<void*>::Exclude(list);
    }

    void Push(const T* element)
    {
        SG_ASSERT(element);
        SgVector<void*>::Push(GetVoidPtr(element));
    }

    T* Pop()
    {
        return static_cast<T*>(SgVector<void*>::Pop());
    }

    T* Top() const
    {
        return static_cast<T*>(SgVector<void*>::Top());
    }

    T* Tail() const
    {
        return static_cast<T*>(SgVector<void*>::Tail());
    }

#if UNUSED
    bool Insert(const T* element)
    {
        return SgVector<void*>::Insert(GetVoidPtr(element));
    }

    bool Extract(const T* element)
    {
        return SgVector<void*>::Extract(GetVoidPtr(element));
    }

    // The following are defined below since they use SgVectorIteratorOf

    bool ContainsContent(const T& element) const;

    void RemoveDuplicateContent();
#endif
private:

    /** Conversion of element pointer to non-const void pointer.
        @note Not sure if there is a better way without needing the
        const cast.
    */
    static void* GetVoidPtr(const T* element)
    {
        return const_cast<void*>(static_cast<const void*>(element));
    }
};

//----------------------------------------------------------------------------

/** Iterator for ListOf<T> typed list of pointers to T */
template<class T>
class SgVectorIteratorOf
    : private SgVectorIterator<void*>
{
public:
    /** Create a list iterator to iterate through list. */
    SgVectorIteratorOf(const SgVectorOf<T>& list)
        : SgVectorIterator<void*>(static_cast<const SgVector<void*>&>(list))
    { }

#if UNUSED
    void Reset()
    {
        SgVectorIterator<void*>::Reset();
    }
#endif
    void operator++()
    {
        SgVectorIterator<void*>::operator++();
    }

    T* operator*() const
    {
        return static_cast<T*>(SgVectorIterator<void*>::operator*());
    }

    operator bool() const
    {
        return SgVectorIterator<void*>::operator bool();
    }
};
//----------------------------------------------------------------------------

#endif // SG_VECTOR_H
