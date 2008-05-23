//----------------------------------------------------------------------------
/** @file SgSList.h
    Static list.
*/
//----------------------------------------------------------------------------

#ifndef SGSLIST_H
#define SGSLIST_H

//----------------------------------------------------------------------------

/** Static list not using dynamic memory allocation.
    Elements need to have a default constructor.
    They should be value-types, not entity-types, because operations like
    Clear() do not call the destructor of the old elements immediately.
*/
template <typename T, int SIZE>
class SgSList
{
public:
    /** Local const iterator */
    class Iterator
    {
    public:
        Iterator(const SgSList& list)
            : m_end(list.m_array + list.Length()),
              m_current(list.m_array)
        { }

        const T& operator*() const
        {
            SG_ASSERT(*this);
            return *m_current;
        }
        
        void operator++()
        {
            ++m_current;
        }

        operator bool() const
        {
            return m_current < m_end;
        }

    private:
        const T* m_end;

        const T* m_current;
    };

    /** Local non-const iterator */
    class NonConstIterator
    {
    public:
        NonConstIterator(SgSList& list)
            : m_end(list.m_array + list.Length()),
              m_current(list.m_array)
        { }

        T& operator*() const
        {
            SG_ASSERT(*this);
            return *m_current;
        }

        void operator++()
        {
            ++m_current;
        }

        operator bool() const
        {
            return m_current < m_end;
        }

    private:
        const T* m_end;

        T* m_current;
    };

    SgSList()
        : m_len(0)
    { }

    explicit SgSList(const T& val)
    {
        SetTo(val);
        m_len = 1;
        m_array[0] = val;
    }

    SgSList(const SgSList<T, SIZE>& list)
    {
        *this = list;
    }
    
    SgSList& operator=(const SgSList& list);

    bool operator==(const SgSList& list) const;

    bool operator!=(const SgSList& list) const
    {
        return ! this->operator==(list);
    }

    T& operator[](int index)
    {
        SG_ASSERT(index >= 0);
        SG_ASSERT(index < m_len);
        return m_array[index];
    }

    const T& operator[](int index) const
    {
        SG_ASSERT(index >= 0);
        SG_ASSERT(index < m_len);
        return m_array[index];
    }

    void Append(const T& val)
    {
        SG_ASSERT(m_len < SIZE);
        m_array[m_len++] = val;
    }

    void Append(const SgSList& list)
    {
        for (Iterator it(list); it; ++it)
            Append(*it);
    }

    void Clear()
    {
        m_len = 0;
    }

    bool Contains(const T& val) const;

    /** Remove first occurrence of a value.
        Like RemoveFirst, but more efficient and does not preserve
        order of remaining elements. The first occurrence of the value is
        replaced by the last element.
        @return false, if element was not found
    */
    bool Exclude(const T& val);

    /** Build intersection with other list.
        List may not contain duplicate entries.
    */
    SgSList Intersect(const SgSList<T,SIZE>& list) const;

    bool IsEmpty() const
    {
        return m_len == 0;
    }

    T& Last()
    {
        SG_ASSERT(m_len > 0);
        return m_array[m_len - 1];
    }

    const T& Last() const
    {
        SG_ASSERT(m_len > 0);
        return m_array[m_len - 1];
    }

    int Length() const
    {
        return m_len;
    }

    void Pop()
    {
        SG_ASSERT(m_len > 0);
        --m_len;
    }

    /** Remove first occurence of a value.
        Preserves order of remaining elements.
        @see Exclude
    */
    void RemoveFirst(const T& val);

    /** Resize list.
        If new length is greater than current length, then the elements
        at a place greater than the old length are not initialized, they are
        just the old elements at this place.
        This is necessary if elements are re-used for efficiency and will be
        initialized later.
    */
    void Resize(int length)
    {
        SG_ASSERT(length >= 0);
        SG_ASSERT(length <= SIZE);
        m_len = length;
    }

    bool SameElements(const SgSList& list) const;

    void SetTo(const T& val)
    {
        m_len = 1;
        m_array[0] = val;
    }

private:
    friend class Iterator;
    friend class NonConstIterator;

    int m_len;

    T m_array[SIZE];
};

//----------------------------------------------------------------------------

template <typename T, int SIZE>
SgSList<T, SIZE>& SgSList<T, SIZE>::operator=(const SgSList& list)
{
    m_len = list.m_len;
    T* p = m_array;
    const T* pp = list.m_array;
    for (int i = m_len; i--; ++p, ++pp)
        *p = *pp;
    return *this;
}

template <typename T, int SIZE>
bool SgSList<T, SIZE>::operator==(const SgSList& list) const
{
    if (m_len != list.m_len)
        return false;
    const T* p = m_array;
    const T* pp = list.m_array;
    for (int i = m_len; i--; ++p, ++pp)
        if (*p != *pp)
            return false;
    return true;
}

template <typename T, int SIZE>
bool SgSList<T, SIZE>::Contains(const T& val) const
{
    int i;
    const T* t = m_array;
    for (i = m_len; i--; ++t)
        if (*t == val)
            return true;
    return false;
 }

template <typename T, int SIZE>
bool SgSList<T, SIZE>::Exclude(const T& val)
{
    // Go backwards through list, because with game playing programs
    // it is more likely that a recently added element is removed first
    T* t = m_array + m_len - 1;
    for (int i = m_len; i--; --t)
        if (*t == val)
        {
            --m_len;
            if (m_len > 0)
                *t = *(m_array + m_len);
            return true;
        }
    return false;
}

template <typename T, int SIZE>
SgSList<T, SIZE> SgSList<T, SIZE>::Intersect(const SgSList<T, SIZE>& list)
    const
{
    SgSList <T, SIZE> result;
    const T* t = m_array;
    for (int i = m_len; i--; ++t)
        if (list.Contains(*t))
        {
            SG_ASSERT(! result.Contains(*t));
            result.Append(*t);
        }
    return result;
}

template <typename T, int SIZE>
void SgSList<T, SIZE>::RemoveFirst(const T& val)
{
    int i;
    T* t = m_array;
    for (i = m_len; i--; ++t)
        if (*t == val)
        {
            for ( ; i--; ++t)
                *t = *(t + 1);
            --m_len;
            break;
        }
}

template <typename T, int SIZE>
bool SgSList<T, SIZE>::SameElements(const SgSList& list) const
{
    if (m_len != list.m_len)
        return false;
    const T* p = m_array;
    for (int i = m_len; i--; ++p)
        if (! list.Contains(*p))
            return false;
    return true;
}

//----------------------------------------------------------------------------

#endif // SGSLIST_H

