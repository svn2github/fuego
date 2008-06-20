//----------------------------------------------------------------------------
/** @file SgList.h
    %List classes.
*/
//----------------------------------------------------------------------------

#ifndef SG_LIST_H
#define SG_LIST_H

#include <algorithm>
#include <climits>

//----------------------------------------------------------------------------

template<typename T>
class SgListIterator;

template<typename T>
class SgListPairIterator;

//----------------------------------------------------------------------------

const int SGLIST_NUM_LIST_REC = 1024;

/** Switch expensive asserts
    that test whether lists are indeed sorted on or off.
*/
const bool SGLIST_CHECK_SORTED = false;

//----------------------------------------------------------------------------

/** Contains one data field for SgList. */
template<typename T>
struct SgListRec
{
    SgListRec* m_next;

    T m_data;
};

//----------------------------------------------------------------------------

/** Block of SGLIST_NUM_LIST_REC SgListRec's.
    Responsible for allocation of SgListRec objects. For efficiency, list
    records are always allocated in blocks. SgListRecBlock keeps a list of all
    the blocks allocated, and it keeps the free list.
*/
template<typename T>
class SgListRecBlock
{
public:
    /** Create a new block of SgListRec.
        All linked together, with the last one pointing to 0.
    */
    SgListRecBlock();

    ~SgListRecBlock();

    /** Return a pointer to a new SgListRec.
        Create one if necessary.
    */
    static SgListRec<T>* NewSgListRec();

    /** Return a pointer to one of 'count' new circularly linked list records.
        Create them if necessary.
    */
    static SgListRec<T>* NewSgListRec(int count);

    /** Return all the list records in the list from 'head' to 'tail'. */
    static void FreeSgListRec(SgListRec<T>* head, SgListRec<T>* tail);

    /** Count the number of allocated list elements, and the number currently
        in use.
    */
    static void GetStatistics(int* numAlloc, int* numUsed);

    /** Try to dispose unused list record blocks.
        This may be slow.
    */
    static void CollectGarbage();

private:
    /** Return whether the SgListRec 'p' is in this block of list records. */
    bool IsInBlock(SgListRec<T>* p) const
    {
        return (&m_listRec[0] <= p)
            && (p <= &m_listRec[SGLIST_NUM_LIST_REC - 1]);
    }

    /** Return the address of the first SgListRec in this block of list
        records.
    */
    SgListRec<T>* FirstSgListRec()
    {
        return &m_listRec[0];
    }

    SgListRec<T>* LastSgListRec()
    {
        return &m_listRec[SGLIST_NUM_LIST_REC - 1];
    }

    bool AllFree() const
    {
        return m_numFree == SGLIST_NUM_LIST_REC;
    }

    /** Return the block that 'p' is in.
        Return 0 if not found.
    */
    static SgListRecBlock* FindBlock(SgListRec<T>* p);

    /** Create a new block of free list records, and adds them to the free
        list.
        Can only be called when s_free is 0.
    */
    static void NewFreeBlock();

    SgListRecBlock* m_nextBlock;

    /** Only valid while counting */
    int m_numFree;

    SgListRec<T> m_listRec[SGLIST_NUM_LIST_REC];

    /** Points to the head of a list of allocated blocks linked
        by 'm_nextBlock'.
    */
    static SgListRecBlock* s_alloc;

    /** Points to a list of free list records, linked by 'm_next'.
        Note that each thread has its own s_free and s_alloc.
    */
    static SgListRec<T>* s_free;
};

//----------------------------------------------------------------------------

/** %List class optimized for small data types.

    An iterator makes it easy to traverse a list from head to tail.
    Efficient (constant-time) operations are: create and dispose a list,
    append an element at the tail, and insert and remove elements at the head.
    The sequences can be used as lists, stacks, or sets.

    Empty lists do not tie up any resources local to SgList.
    There is no limit to the length of a list or the total length of all lists
    except for memory constraints.
    Memory is allocated as needed; garbage collection may free some of the
    memory that's no longer used.
    Non-empty lists are identified by a pointer to their last element; the
    last element is linked in a circular manner to the first element.
    This makes it easy to insert new elements both at the head and at the tail
    of the list.

    @warning This class uses static member variables for its internal
    memory management, it is not reentrant (even with respect to different
    instances) and should not be used in multi-threaded code.
    Use std::vector<T> instead and reuse vector instances to avoid frequent
    dynamic allocations.
*/
template<typename T>
class SgList
{
public:
    /** Construct empty list. */
    SgList()
        : m_last(0)
    { }

    /** Create a list with one element. */
    explicit SgList(const T& elt);

    /** Copy constructor.
        Create new list with the identical content of this one.
    */
    SgList(const SgList<T>& list);

    /** Release all memory this list is using.
        @warning If the list contains pointers, the clients are responsible
        for that memory.
    */
    virtual ~SgList();

    /** Assignment operator.
        Copy whole right-hand side list to this list.
    */
    SgList<T>& operator=(const SgList<T>& rhs);

    /** Swap the entire contents of this list with <code>*list</code>. */
    void SwapWith(SgList* list)
    {
        std::swap(m_last, list->m_last);
    }

    /** Remove all elements in this list. */
    void Clear();

    /** Clear this list and set it to contain only <code>elt</code>.
        Optimized method that's equivalent to
        <code>Clear(); Append(elt)</code>.
    */
    void SetTo(const T& elt);

    /** Clear this list and set it to contain the <code>%count</code>
        elements from
        <code>array[0]</code> to <code>array[%count - 1]</code>.
        If <code>%count</code> is zero, the list is just cleared.
    */
    void SetTo(const T* array, int count);

    /** Return whether this list contains zero elements. */
    bool IsEmpty() const
    {
        return (m_last == 0);
    }

    /** Return whether this list contains more than zero elements. */
    bool NonEmpty() const
    {
        return (m_last != 0);
    }

    /** Return the number of elements in this list.
        @note This is an O(n) operation: the length is counted each time,
        it is not saved.
    */
    int Length() const;

    /** Count the number of elements in this list up to <code>limit</code>.
        @return The correct number if <code>length <= limit</code>,
        and <code>limit+1</code> if it's longer.
    */
    int Length(int limit) const;

    /** Test whether a list is as long as a given length. */
    bool IsLength (int length) const
    {
        return Length(length) == length;
    }

    /** Test whether a list is as long as or longer than a given length. */
    bool MinLength(int length) const
    {
        return Length(length) >= length;
    }

    /** Test whether a list is shorter than or equal to a given length. */
    bool MaxLength(int length) const
    {
        return Length(length) <= length;
    }

    /** Test whether a list has length one.
        Optimized version of <code>IsLength(1)</code>.
    */
    bool IsLength1() const
    {
        return m_last && m_last->m_next == m_last;
    }

    /** Test whether a list has at least length two.
        Optimized version of <code>MinLength(2)</code>.
    */
    bool MinLength2() const
    {
        return m_last && m_last->m_next != m_last;
    }

    /** Test whether a list is shorter than a given length. */
    bool ShorterThan(const SgList<T>& list) const;

    /** Return reference to element.
        @param index Position of the element in range <code>1..length</code>.
    */
    T& operator[](int index);

    /** Return copy of element.
        @param index Position of the element in range <code>1..length</code>.
    */
    const T& operator[](int index) const;

    /** Compare whether the contents of two lists are identical.
        Same length, and the same elements in the same sequence.
    */
    bool operator==(const SgList<T>& rhs) const;

    /** Compare whether the contents of two lists are not identical. */
    bool operator!=(const SgList& rhs) const
    {
        return ! (*this == rhs);
    }

    /** Add a single element at the end of the list. */
    void Append(const T& elt);

    /** Append <code>*tail</code> at the end of this list,
        and empty the <code>*tail</code> list.
    */
    void Concat(SgList<T>* tail);

    /** Append all elements from <code>list</code> at the end of this list. */
    void AppendList(const SgList<T>& list);

    /** Include all elements from <code>set</code> into this list.
        Appends new elements at the end of this list.
    */
    void Union(const SgList<T>& set);

    /** Check if element is in list.
        @return <code>true</code> if <code>elt</code> is included in this
        list, <code>false</code> if not.
    */
    bool Contains(const T& elt) const
    {
        // Special cases for zero and one element lists.
        if (m_last == 0)
            return false;
        else if (m_last->m_next == m_last)
            return (m_last->m_data == elt);
        else
            return LongContains(elt); // List at least two long
    }

    /** Check for duplicate elements.
        @return <code>true</code> if there are no duplicate elements in
        the list.
        Useful for debugging.
        @todo speed it up
    */
    bool UniqueElements() const;

    /** Count element.
        @returns How often <code>elt</code> is included in this list.
    */
    int NumContained(const T& elt) const;

    /** Removes all but the first copy of each element from the list.
        After calling @c RemoveDuplicates(), @c UniqueElements() is @c true.
        @return true, if at least one duplicate was removed
    */
    bool RemoveDuplicates();

    /** Find position of element.
        @returns The position of <code>elt</code> in the list,
        in range <code>1..length</code>. Zero if <code>elt</code>
        is not in the list.
    */
    int Index(const T& elt) const;

    /** Append <code>elt</code> at the end of the list
        if it's not already in the list.
    */
    void Include(const T& elt)
    {
        if (! Contains(elt))
            Append(elt);
    }

    /** Remove the first occurence of element.
        If <code>elt</code> is in the list, remove the first
        occurence of <code>elt</code> from the list, and return
        <code>true</code>. Otherwise do nothing and return <code>false</code>.
    */
    bool Exclude(const T& elt);

    /** Replace element.
        If <code>oldElt</code> is in the list, replace the first
        occurence of <code>oldElt</code> in the list with <code>newElt</code>,
        and return <code>true</code>.
        Otherwise do nothing and return <code>false</code>.
    */
    bool Replace(const T& oldElt, const T& newElt);

    /** Exclude each element of <code>list</code> from this list. */
    void Exclude(const SgList<T>& list);

    /** Return the smallest element in this list.
        The list must not be empty.
    */
    const T& Minimum() const;

    /** Insert element at the head of the list.
        (which is the top of the stack).
    */
    void Push(const T& elt);

    /** Remove the head of the list.
        The list must not be empty.
        @return The head of the list.
    */
    T Pop();

    /** Returns the head of the list.
        Asserts if the list is empty.
    */
    const T& Top() const
    {
        SG_ASSERT(NonEmpty());
        return First()->m_data;
    }

    /** Returns the last element of the list.
        Asserts if the list is empty.
    */
    const T& Tail() const
    {
        SG_ASSERT(NonEmpty());
        return m_last->m_data;
    }

    /** Order the elements of the list in ascending order.
        (smallest element at the head of the list).
        @bug duplicates will be deleted
        @todo could add: bool ascending = true
    */
    void Sort();

    /** Includes the element in a ascending sorted list at the right place.
        Does nothing and returns <code>false</code> if the element is
        already in the list;
        returns <code>true</code> if the element is inserted.
        @todo made two separate functions for efficiency, should be
        a template taking a compare template arg.
        Same for <code>Merge()</code> below.
    */
    bool Insert(const T& elt);

    /** Includes the element in a descending sorted list at the right place.
        Does nothing and returns false if the element is already in the list;
        returns true if the element is inserted.
        @todo made two separate functions for efficiency, should be
        a template taking a compare template arg.
        Same for <code>Merge()</code> below.
    */
    bool InsertR(const T& elt);

    /** Excludes the element from an ascending sorted list.
        @return <code>true</code> if the element was in the list.
    */
    bool Extract(const T& elt);

    /** Merges this sorted list with the sorted <code>list</code>.
        Equivalent to:
        <pre>
        for (SgListIterator<T> iter(list); iter; ++iter)
            Insert(*iter).
        </pre>
    */
    void Merge(const SgList<T>& list);

    /** Cut off list after at most <code>length</code> elements. */
    void LimitListLength (int length);

    /** Returns whether the list is sorted in ascending order. */
    bool IsSorted(bool ascending = true) const;

    /** Returns whether the list is sorted in ascending order and has no
        duplicate elements.
        @todo name? the elements are unique, not the list...
    */
    bool IsSortedAndUnique(bool ascending = true) const;

    bool SetsAreEqual(const SgList<T>& list2) const;

    void DeleteAt(int index);

    /** Try to dispose unused list record blocks.
        This may be slow.
    */
    static void CollectGarbage();

    /** Count the number of allocated list elements, and the number currently
        in use. */
    static void GetStatistics(int* numAlloc, int* numUsed);

    /** Check for memory leaks.
        Checks that no more instances exist with an assertion.
    */
    static void MemCheck();

protected:
    int Data(const SgListRec<T>* rec) const
    {
        return rec->m_data;
    }

    SgListRec<T>* Next(const SgListRec<T>* rec) const
    {
        return rec->m_next;
    }

    SgListRec<T>* First() const
    {
        return m_last->m_next;
    }

    SgListRec<T>* Last() const
    {
        return m_last;
    }

    void AppendData(const SgListRec<T>* from, SgListRec<T>** to,
                    bool adjustLast = false);

private:
    bool LongContains(const T& elt) const;

    SgListRec<T>* m_last;

    friend class SgListIterator<T>;
    friend class SgListPairIterator<T>;
};

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
class SgListIterator
{
public:
    /** Create a list iterator to iterate through list. */
    SgListIterator(const SgList<T>& list)
        : m_list(list),
          m_nextItem(list.m_last ? list.First() : 0)
    { }

    /** Copy current state of iterator.
        Useful for creating a new iterator that only runs from the current
        position to the list end. See <code>UniqueElements()</code>
        for an example.
    */
    SgListIterator(const SgListIterator& it)
        : m_list(it.m_list),
          m_nextItem(it.m_nextItem)
    { }

    virtual ~SgListIterator() { }

    /** Reset the iterator to restart at the beginning of the list.
        E.g. after the contents of the list have changed.
    */
    void Reset()
    {
        m_nextItem = m_list.m_last ? m_list.First() : 0;
    }

    /** Advance the state of the iteration to the next element. */
    SgListIterator& operator++()
    {
        SG_ASSERT(m_nextItem);
        m_nextItem = (m_nextItem == m_list.m_last)
            ? 0 : m_nextItem->m_next;
        return *this;
    }

    /** Return the value of the current element. */
    const T& operator*() const
    {
        SG_ASSERT(m_nextItem);
        return m_nextItem->m_data;
    };

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_nextItem != 0;
    }

private:
    const SgList<T>& m_list;

    SgListRec<T>* m_nextItem;

    /** not implemented */
    SgListIterator& operator=(const SgListIterator&);
};

//----------------------------------------------------------------------------

/** Iterator to iterate through all possible pairs of list elements.

    Each pair is returned exactly once, i.e. with
    @verbatim
        list = e1, e2, ... en
    @endverbatim
    the returned pairs are (in order):
    @verbatim
        (e1, e2), (e1, e3), ... (e1, en), (e2,e 3), ... (e(n - 1), en)
    @endverbatim
    @todo use standard iterator format.
*/
template<typename T>
class SgListPairIterator
{
public:
    SgListPairIterator(const SgList<T>& list);

    virtual ~SgListPairIterator() { }

    /** Find the next pair of data elements.
        Return <code>true</code> and change <code>elt1</code> and
        <code>elt2</code> if not reached the end of
        the list; return <code>false</code> and don't touch
        the parameters if at the end of the list.
    */
    bool NextPair(T& elt1, T& elt2);

private:
    const SgList<T>& m_list;

    SgListRec<T>* m_nextItem;

    SgListRec<T>* m_nextPair;
};

//----------------------------------------------------------------------------

/** Typed list of pointers to T. Pointers cannot be 0. */
template<class T>
class SgListOf
    : public SgList<void*>
{
public:
    T* operator[] (int index) const
    {
        return static_cast<T*>(SgList<void*>::operator[](index));
    }

    void Append(const T* element)
    {
        SG_ASSERT(element);
        SgList<void*>::Append(GetVoidPtr(element));
    }

    bool Contains(const T* element) const
    {
        SG_ASSERT(element);
        return SgList<void*>::Contains(GetVoidPtr(element));
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
        return SgList<void*>::Exclude(GetVoidPtr(element));
    }

    void Exclude(const SgListOf<T>& list)
    {
        SgList<void*>::Exclude(list);
    }

    void Push(const T* element)
    {
        SG_ASSERT(element);
        SgList<void*>::Push(GetVoidPtr(element));
    }

    T* Pop()
    {
        return static_cast<T*>(SgList<void*>::Pop());
    }

    T* Top() const
    {
        return static_cast<T*>(SgList<void*>::Top());
    }

    T* Tail() const
    {
        return static_cast<T*>(SgList<void*>::Tail());
    }

    bool Insert(const T* element)
    {
        return SgList<void*>::Insert(GetVoidPtr(element));
    }

    bool Extract(const T* element)
    {
        return SgList<void*>::Extract(GetVoidPtr(element));
    }

    // The following are defined below since they use SgListIteratorOf

    bool ContainsContent(const T& element) const;

    void RemoveDuplicateContent();

private:
    T* Data(const SgListRec<void*>* rec) const
    {
        return static_cast<T*>(SgList<void*>::Data(rec));
    }

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
class SgListIteratorOf
    : private SgListIterator<void*>
{
public:
    /** Create a list iterator to iterate through list. */
    SgListIteratorOf(const SgListOf<T>& list)
        : SgListIterator<void*>(static_cast<const SgList<void*>&>(list))
    { }

    void Reset()
    {
        SgListIterator<void*>::Reset();
    }

    void operator++()
    {
        SgListIterator<void*>::operator++();
    }

    T* operator*() const
    {
        return static_cast<T*>(SgListIterator<void*>::operator*());
    }

    operator bool() const
    {
        return SgListIterator<void*>::operator bool();
    }
};

//----------------------------------------------------------------------------

/** Iterator for all possible pairs of ListOf<T> elements
    Each pair is returned exactly once, i.e. with
    <code>list = e1, e2,...en</code>
    the returned pairs are (in order):
    <code>(e1,e2), (e1,e3), ... (e1,en), (e2,e3),...(e(n-1),en)</code>
*/
template<class T>
class SgListPairIteratorOf
    : public SgListPairIterator<void*>
{
public:
    /** Create an iterator to iterate through all possible
        pairs of list elements.
    */
    SgListPairIteratorOf(const SgListOf<T>& list)
        : SgListPairIterator<void*>(static_cast<const SgList<void*>&>(list))
    { }

    /** Find the next pair of data elements.
        Return @c true and change @c elt1 and @c elt2 if not reached the end
        of the list; return <code>false</code> and don't touch the parameters
        if at the end of the list.
    */
    bool NextPair(T*& elt1, T*& elt2)
    {
        void* voidPtr1;
        void* voidPtr2;
        if (SgListPairIterator<void*>::NextPair(voidPtr1, voidPtr2))
        {
            elt1 = static_cast<T*>(voidPtr1);
            elt2 = static_cast<T*>(voidPtr2);
            return true;
        }
        return false;
    }
};

//----------------------------------------------------------------------------

/** OwnerListOf own the objects of type T pointed to by the elements
    in the list.
    When the owner list is destructed, it deletes the elements.

    @todo OwnerListOf can't handle double entries, as that would lead
    to double deletion. Append and Push check for this, but there are
    other ways to
    to add elements that should still be checked, e.g. SwapWith.

    @todo comment from Martin 97-05-25:
    check to avoid double entries -> double deletion in list.
    check done for Append and Push now MM 99-06-14, but there are other
    ways to add elts such as SwapWith.
*/
template<class T>
class SgOwnerListOf
    : public SgListOf<T>
{
public:
    SgOwnerListOf()
        : SgListOf<T>()
    { }

    ~SgOwnerListOf()
    {
        DeleteAll();
    }

    bool Extract(T* elt)
    {
        if (SgListOf<T>::Extract(elt))
        {
            SG_ASSERT(! Contains(elt));
            delete elt;
            return true;
        }
        else
            return false;
    }

    bool Exclude(T* elt)
    {
        if (SgListOf<T>::Exclude(elt))
        {
            SG_ASSERT(! Contains(elt));
            delete elt;
            return true;
        }
        else
            return false;
    }

    void Append(T* elt)
    {
        SG_ASSERT(! Contains(elt));
        SgListOf<T>::Append(elt);
    }

    void Push(T* elt)
    {
        SG_ASSERT(! Contains(elt));
        SgListOf<T>::Push(elt);
    }

    void Clear()
    {
        DeleteAll();
        SgListOf<T>::Clear();
    }

    void DeleteAll()
    {
        SG_ASSERT(SgListOf<T>::UniqueElements());
        for (SgListIteratorOf<T> it(*this); it; ++it)
            delete *it;
    }
};

//----------------------------------------------------------------------------

template<typename T>
SgListRec<T>* SgListRecBlock<T>::s_free = 0;

template<typename T>
SgListRecBlock<T>* SgListRecBlock<T>::s_alloc = 0;

template<typename T>
SgListRecBlock<T>::SgListRecBlock()
{
    // Initialize pointers: always point to next record, except for last
    // record, which points to 0.
    for (int i = 0; i < SGLIST_NUM_LIST_REC - 1; ++i)
    {
        m_listRec[i].m_next = &m_listRec[i + 1];
    }
    m_listRec[SGLIST_NUM_LIST_REC - 1].m_next = 0;
    // Add this block at head of allocated blocks list.
    m_nextBlock = s_alloc;
    s_alloc = this;
}

template<typename T>
SgListRecBlock<T>::~SgListRecBlock()
{
    // We know that every element in this block is in the free list, so need
    // to remove every element in it from the free list.
    SgListRec<T>** pp = &s_free;
    int numFree = 0;
    while (*pp)
    {
        if (IsInBlock(*pp))
        {
            *pp = (*pp)->m_next;
            ++numFree;
        }
        else
            pp = &((*pp)->m_next);
    }
    SG_ASSERT(numFree == SGLIST_NUM_LIST_REC);
    // Remove this block from the list of allocated blocks.
    if (this == s_alloc)
        s_alloc = m_nextBlock;
    else
    {
        SgListRecBlock* block = s_alloc;
        while (block && block->m_nextBlock != this)
        {
            block = block->m_nextBlock;
        }
        SG_ASSERT(block != 0);
        SG_ASSERT(block->m_nextBlock == this);
        block->m_nextBlock = block->m_nextBlock->m_nextBlock;
    }
}

template<typename T>
void SgListRecBlock<T>::NewFreeBlock()
{
    // If no more free list records, allocate a block of new ones.
    SgListRecBlock* block = new SgListRecBlock;
    SG_ASSERT(block);
    // New free list starts at first SgListRec.
    SG_ASSERT(s_free == 0);
    s_free = block->FirstSgListRec();
}

template<typename T>
SgListRec<T>* SgListRecBlock<T>::NewSgListRec()
{
    // Allocate new list records if necessary.
    if (s_free == 0)
        NewFreeBlock();
    // Return head of free list, and move head of free list to next.
    SgListRec<T>* p = s_free;
    s_free = s_free->m_next;
    return p;
}

template<typename T>
SgListRec<T>* SgListRecBlock<T>::NewSgListRec(int count)
{
    SG_ASSERT(0 < count);
    // Allocate new list records if necessary.
    if (s_free == 0)
        NewFreeBlock();
    // Remember head of list.
    SgListRec<T>* first = s_free;
    // Move head of free list by count - 1 items.
    for (int i = 0; i < count-1; ++i)
    {
        SgListRec<T>* prev = s_free;
        s_free = s_free->m_next;
        if (s_free == 0)
        {
            NewFreeBlock();
            prev->m_next = s_free;
        }
    }
    // Create circular linked list with last item.
    SgListRec<T>* last = s_free;
    s_free = s_free->m_next;
    last->m_next = first;
    return first;
}

template<typename T>
void SgListRecBlock<T>::FreeSgListRec(SgListRec<T>* head, SgListRec<T>* tail)
{
    tail->m_next = s_free;
    s_free = head;
}

template<typename T>
SgListRecBlock<T>* SgListRecBlock<T>::FindBlock(SgListRec<T>* p)
{
    SgListRecBlock<T>* block = s_alloc;
    while (block && ! block->IsInBlock(p))
    {
        block = block->m_nextBlock;
    }
    return block;
}

template<typename T>
void SgListRecBlock<T>::GetStatistics(int* numAlloc, int* numUsed)
{
    // #allocated = #blocks * records per block.
    *numAlloc = 0;
    SgListRecBlock<T>* block = s_alloc;
    while (block)
    {
        *numAlloc += SGLIST_NUM_LIST_REC;
        block = block->m_nextBlock;
    }
    // #used = #allocated - #free.
    *numUsed = *numAlloc;
    SgListRec<T>* p = s_free;
    while (p)
    {
        (*numUsed)--;
        p = p->m_next;
    }
    SG_ASSERT(*numUsed >= 0);
}

template<typename T>
void SgListRecBlock<T>::CollectGarbage()
{
    // Initialize the count of free SgListRec in each block to zero.
    SgListRecBlock<T>* block = s_alloc;
    while (block)
    {
        block->m_numFree = 0;
        block = block->m_nextBlock;
    }

    // Traverse the free list and count the number of free records in each
    // block.
    SgListRec<T>* p = s_free;
    while (p)
    {
        block = FindBlock(p);
        SG_ASSERT(block != 0); // assert if allocated in a different thread
        if (block)
            ++block->m_numFree;
        p = p->m_next;
    }

    // Dispose every block that contains only free records (none in use).
    // Destructor takes care of fixing up the s_alloc block list; just need
    // to be careful not to point to the item that gets deleted.
    while (s_alloc && s_alloc->AllFree())
        delete s_alloc;
    block = s_alloc;
    while (block)
    {
        SG_ASSERT(! block->AllFree());
        while (block->m_nextBlock && block->m_nextBlock->AllFree())
            delete block->m_nextBlock;
        block = block->m_nextBlock;
    }
}

//----------------------------------------------------------------------------

template<typename T>
SgList<T>::SgList(const T& elt)
    : m_last(SgListRecBlock<T>::NewSgListRec())
{
    m_last->m_next = m_last;
    m_last->m_data = elt;
}

template<typename T>
SgList<T>::SgList(const SgList<T>& list)
    : m_last(0)
{
    if (! list.IsEmpty())
    {
        m_last = SgListRecBlock<T>::NewSgListRec();
        m_last->m_data = list.m_last->m_data;
        SgListRec<T>* source = list.First();
        SgListRec<T>* prev = m_last;
        while (source != list.m_last)
        {
            SgListRec<T>* n = SgListRecBlock<T>::NewSgListRec();
            n->m_data = source->m_data;
            prev->m_next = n;
            prev = n;
            source = source->m_next;
        }
        prev->m_next = m_last;
    }
}

template<typename T>
SgList<T>::~SgList()
{
    // Same as Clear(), but don't need to change m_last since this list
    // will be thrown away.
    // PERF: List destruction happens many times, often with empty lists.
    // However, moving the test for m_last to the header file doesn't
    // speed things up, it slows tactics down by about half a percent.
    if (m_last)
        SgListRecBlock<T>::FreeSgListRec(m_last->m_next, m_last);
}

template<typename T>
SgList<T>& SgList<T>::operator=(const SgList<T>& rhs)
{
    if (this != &rhs)
    {
        Clear();
        if (! rhs.IsEmpty())
        {
            m_last = SgListRecBlock<T>::NewSgListRec();
            m_last->m_data = rhs.m_last->m_data;
            SgListRec<T>* source = rhs.First();
            SgListRec<T>* prev = m_last;
            while (source != rhs.m_last)
            {
                SgListRec<T>* n = SgListRecBlock<T>::NewSgListRec();
                n->m_data = source->m_data;
                prev->m_next = n;
                prev = n;
                source = source->m_next;
            }
            prev->m_next = m_last;
        }
    }
    return *this;
}

template<typename T>
void SgList<T>::Clear()
{
    if (m_last)
        SgListRecBlock<T>::FreeSgListRec(First(), m_last);
    m_last = 0;
}

template<typename T>
void SgList<T>::SetTo(const T& elt)
{
    if (m_last)
        SgListRecBlock<T>::FreeSgListRec(First(), m_last);
    m_last = SgListRecBlock<T>::NewSgListRec();
    m_last->m_next = m_last;
    m_last->m_data = elt;
}

template<typename T>
void SgList<T>::SetTo(const T* array, int count)
{
    if (m_last)
        SgListRecBlock<T>::FreeSgListRec(First(), m_last);
    if (count == 0)
        m_last = 0;
    else
    {
        SgListRec<T>* p = m_last = SgListRecBlock<T>::NewSgListRec(count);
        for (int i = 0; i < count; ++i)
        {
            p = p->m_next;
            p->m_data = array[i];
        }
    }
    SG_ASSERT(IsLength(count));
}

template<typename T>
int SgList<T>::Length() const
{
    int count = 0;
    if (m_last)
    {
        SgListRec<T>* current = m_last;
        do
        {
            ++count;
            current = current->m_next;
        } while (current != m_last);
    }
    return count;
}

template<typename T>
int SgList<T>::Length(int limit) const
{
    int count = 0;
    if (m_last)
    {
        SgListRec<T>* current = m_last;
        do
        {
            ++count;
            current = current->m_next;
        } while (current != m_last && count <= limit);
    }
    return count;
}

template<typename T>
bool SgList<T>::ShorterThan(const SgList<T>& list) const
{
    if (! m_last)
        return false;
    else if (! list.m_last)
        return true;
    SgListRec<T>* p1 = m_last;
    SgListRec<T>* p2 = list.m_last;
    while (true)
    {
        p2 = p2->m_next;
        if (p2 == list.m_last)
            return false;
        else
        {
            p1 = p1->m_next;
            if (p1 == m_last)
                return true;
        }
    }
    SG_ASSERT(false); // cannot get here.
    return false;
}

template<typename T>
T& SgList<T>::operator[](int index)
{
    SG_ASSERT(index > 0);
    int i = index;
    SG_ASSERT(m_last);
    SgListRec<T>* current = m_last;
    while (i > 0)
    {
        current = current->m_next;
        --i;
    }
    return current->m_data;
}

template<typename T>
const T& SgList<T>::operator[](int index) const
{
    SG_ASSERT(index > 0);
    int i = index;
    SG_ASSERT(m_last);
    SgListRec<T>* current = m_last;
    while (i > 0)
    {
        current = current->m_next;
        --i;
    }
    return current->m_data;
}

template<typename T>
bool SgList<T>::operator==(const SgList<T>& rhs) const
{
    SgListRec<T>* p1 = m_last;
    SgListRec<T>* p2 = rhs.m_last;
    if (p1 && p2)
    {
        // Neither list is null, traverse them in parallel.
        do
        {
            // If data is different, lists are different.
            if (p1->m_data != p2->m_data)
                return false;
            p1 = p1->m_next;
            p2 = p2->m_next;
        } while (p1 != m_last && p2 != rhs.m_last);
        // Same if both lists reach the end at the same time.
        return (p1 == m_last && p2 == rhs.m_last);
    }
    else
    {   // If one is null, same only if both are null.
        return (! p1 && ! p2);
    }
}

template<typename T>
void SgList<T>::Append(const T& elt)
{
    if (m_last == 0)
    {
        m_last = SgListRecBlock<T>::NewSgListRec();
        m_last->m_next = m_last;
        m_last->m_data = elt;
    }
    else
    {
        SgListRec<T>* x = SgListRecBlock<T>::NewSgListRec();
        x->m_next = First();
        m_last->m_next = x;
        x->m_data = elt;
        m_last = x; // same as Push, except this instruction
    }
}

template<typename T>
void SgList<T>::Concat(SgList<T>* tail)
{
    SG_ASSERT(tail);
    if (tail->NonEmpty())
    {
        if (NonEmpty())
        {
            SgListRec<T>* first = First();
            m_last->m_next = tail->First();
            tail->m_last->m_next = first;
        }
        m_last = tail->m_last;
        tail->m_last = 0;
    }
}

template<typename T>
void SgList<T>::Union(const SgList<T>& set)
{
    for (SgListIterator<T> iter(set); iter; ++iter)
        Include(*iter);
}

template<typename T>
void SgList<T>::AppendList(const SgList<T>& list)
{
    for (SgListIterator<T> iter(list); iter; ++iter)
        Append(*iter);
}

template<typename T>
bool SgList<T>::LongContains(const T& elt) const
{
    SG_ASSERT(MinLength2());
    // List at least two long: use 'elt' as sentinel to speed up search.
    T saveElt = m_last->m_data;
    m_last->m_data = elt;
    SgListRec<T>* x = m_last->m_next;
    SG_ASSERT(m_last->m_data == elt);
    while (x->m_data != elt)
    {
        SG_ASSERT(m_last->m_data == elt);
        x = x->m_next;
    }
    m_last->m_data = saveElt;
    return (x->m_data == elt);
}

template<typename T>
int SgList<T>::NumContained(const T& elt) const
{
    int num = 0;
    for (SgListIterator<T> it(*this); it; ++it)
        if (*it == elt)
            ++num;
    return num;
}

template<typename T>
bool SgList<T>::RemoveDuplicates()
{
    // AR n^2; could be made much faster; see Modula-code
    SgList<T> uniqueList;
    for (SgListIterator<T> it(*this); it; ++it)
        if (! uniqueList.Contains(*it))
            uniqueList.Append(*it);
    SwapWith(&uniqueList); // avoid copying
    SG_ASSERT(UniqueElements());
    return (uniqueList.Length() != Length());
}

template<typename T>
bool SgList<T>::UniqueElements() const
{
    // AR n^2; could be made much faster; see Modula-code
    if (MinLength2())
    {
        if (IsSorted())
            return IsSortedAndUnique();
        else
            for (SgListIterator<T> it(*this); it; ++it)
            {
                SgListIterator<T> it2(it); // clone iterator
                ++it2; // jump over current element
                for (/* nop */; it2; ++it2)
                if (*it == *it2)
                    return false;
            }
    }
    return true;
}

template<typename T>
bool SgList<T>::Replace(const T& oldElt, const T& newElt)
{   // modeled after Exclude and Contains functions.
    // AR: untested.
    // Special cases for zero and one element lists.
    if (m_last == 0)
        ; // nop
    else if (First() == m_last)
    {
        if (m_last->m_data == oldElt)
        {
            m_last->m_data = newElt;
            return true;
        }
    }
    else
    {
        // List at least two long: use 'oldElt' as sentinel to speed up
        // search.
        int saveElt = m_last->m_data;
        m_last->m_data = oldElt;
        SgListRec<T>* x = First();
        ASSERT(m_last->m_data == oldElt);
        while (x->m_data != oldElt)
        {
            ASSERT(m_last->m_data == oldElt);
            x = x->m_next;
        }
        m_last->m_data = saveElt;
        if (x->m_data == oldElt)
        {
            x->m_data = newElt;
            return true;
        }
    }
    return false;
}

template<typename T>
int SgList<T>::Index(const T& elt) const
{
    // Special cases for zero and one element lists.
    if (m_last == 0)
        return 0;
    else if (First() == m_last)
        return static_cast<int>(m_last->m_data == elt); // false = 0, true = 1
    else
    {
        // List at least two long: use 'elt' as sentinel to speed up search.
        int saveElt = m_last->m_data;
        m_last->m_data = elt;
        SgListRec<T>* x = First();
        SG_ASSERT(m_last->m_data == elt);
        int index = 1;
        while (x->m_data != elt)
        {
            SG_ASSERT(m_last->m_data == elt);
            x = x->m_next;
            ++index;
        }
        m_last->m_data = saveElt;
        SG_ASSERT(x->m_data != elt || (*this)[index] == elt);
        return (x->m_data == elt) ? index : 0;
    }
}

template<typename T>
void SgList<T>::DeleteAt(int index)
{
    SG_ASSERT(0 < index && index <= Length());
    if (First() == m_last)
        Clear(); // single-element list not handled correctly below
    else
    {
        SgListRec<T>* p = m_last;
        SgListRec<T>* t = p->m_next;
        while (--index > 0)
        {
            p = t;
            t = p->m_next;
        }
        p->m_next = t->m_next;
        SgListRecBlock<T>::FreeSgListRec(t, t);
        if (m_last == t)
            m_last = p; // deleted last element
    }
}

template<typename T>
bool SgList<T>::Exclude(const T& elt)
// If 'elt' is in the list, remove the first occurence of 'elt' from the
// list, and return true. Otherwise do nothing and return false.
{
    // Special cases for zero and one element lists.
    if (m_last == 0)
        return false;
    else if (First() == m_last)
    {
        if (m_last->m_data == elt)
        {
            Clear();
            return true;
        }
    }
    else
    {
        SgListRec<T>* p = m_last;
        SgListRec<T>* t = p->m_next;
        while (t != m_last && t->m_data != elt)
        {
            p = t;
            t = p->m_next;
        }
        if (t->m_data == elt)
        {
            p->m_next = t->m_next;
            SgListRecBlock<T>::FreeSgListRec(t, t);
            if (m_last == t)
                m_last = p; // deleted last element
            return true;
        }
    }
    return false;
}

template<typename T>
void SgList<T>::Exclude(const SgList<T>& list)
{
    for (SgListIterator<T> iter(list); iter; ++iter)
        Exclude(*iter);
}

template<typename T>
const T& SgList<T>::Minimum() const
{
    int min = Top();
    for (SgListIterator<T> iter(*this); iter; ++iter)
    {
        if (*iter < min)
            min = *iter;
    }
    return min;
}

template<typename T>
void SgList<T>::Push(const T& elt)
{
    if (m_last == 0)
    {
        m_last = SgListRecBlock<T>::NewSgListRec();
        m_last->m_next = m_last;
        m_last->m_data = elt;
    }
    else
    {
        SgListRec<T>* x = SgListRecBlock<T>::NewSgListRec();
        x->m_next = First();
        m_last->m_next = x;
        x->m_data = elt;
        // same as Append, except missing instruction here to change m_last
    }
}

template<typename T>
T SgList<T>::Pop()
{
    SG_ASSERT(NonEmpty());
    SgListRec<T>* head = First();
    T elt = head->m_data;
    SgListRec<T>* next = head->m_next;
    SgListRecBlock<T>::FreeSgListRec(head, head);
    if (head == m_last)
        m_last = 0;
    else
        m_last->m_next = next;
    return elt;
}

template<typename T>
void SgList<T>::Sort()
// slow, can be optimized
// Orders the elements of the list in ascending order (smallest element at
// the head of the list).
{
    if (MinLength2())
    {
        // Simply sort list by inserting each element in a new sorted list.
        SgList<T> newList;
        for (SgListIterator<T> iter(*this); iter; ++iter)
        {
            newList.Insert(*iter);
        }
        // AR: BUG: this will also throw out duplicates...

        // Throw away old content, and switch old and new lists.
        SgListRecBlock<T>::FreeSgListRec(First(), m_last);
        m_last = newList.m_last;
        newList.m_last = 0;
    }
    if (SGLIST_CHECK_SORTED)
        SG_ASSERT(IsSorted());
}

template<typename T>
bool SgList<T>::Insert(const T& elt)
{
    if (SGLIST_CHECK_SORTED)
        SG_ASSERT(IsSorted());
    if (m_last == 0 || (First()->m_data > elt))
        Push(elt);
    else if (elt > m_last->m_data)
        Append(elt);
    else
    {
        SgListRec<T>* p = First();
        while (true)
        {
            if (p->m_data == elt)
                return false; // already exists, don't insert
            if (p->m_next->m_data > elt)
            {   SgListRec<T>* x = SgListRecBlock<T>::NewSgListRec();
                x->m_data = elt;
                x->m_next = p->m_next;
                p->m_next = x;
                return true;
            }
            p = p->m_next;
        }
    }
    return true;
}

template<typename T>
bool SgList<T>::InsertR(const T& elt)
{
    if (SGLIST_CHECK_SORTED)
        ASSERT(IsSorted(false)); // sorted descending
    if (m_last == 0 || (First()->m_data < elt))
        Push(elt);
    else if (elt < m_last->m_data)
        Append(elt);
    else
    {
        SgListRec<T>* p = First();
        while (true)
        {
            if (p->m_data == elt)
                return false; // already exists, don't insert
            if (p->m_next->m_data < elt)
            {
                SgListRec<T>* x = SgListRecBlock<T>::NewSgListRec();
                x->m_data = elt;
                x->m_next = p->m_next;
                p->m_next = x;
                return true;
            }
            p = p->m_next;
        }
    }
    return true;
}

template<typename T>
bool SgList<T>::Extract(const T& elt)
{
    // slow, can be optimized
    if (SGLIST_CHECK_SORTED)
        ASSERT(IsSorted());
    if (m_last)
    {
        // Sorted list, so must be larger than first and smaller than last.
        if (First()->m_data <= elt && elt <= m_last->m_data)
            return Exclude(elt);
    }
    return false;
}

template<typename T>
void SgList<T>::AppendData(const SgListRec<T>* from, SgListRec<T>** to,
                           bool adjustLast)
{
    SgListRec<T>* x = SgListRecBlock<T>::NewSgListRec();
    x->m_data = from->m_data;
    x->m_next = (*to)->m_next;
    (*to)->m_next = x;
    (*to) = x;
    if (adjustLast)
        m_last = x;
}

template<typename T>
void SgList<T>::LimitListLength(int length)
{
    if (! Last()) // empty list
        return;
    if (length == 0)
    {
        Clear();
        return;
    }
    SgListRec<T>* p = First();
    while (p != Last())
    {
        if (--length == 0)
            break;
        p = p->m_next;
    }
    if (p != Last()) // free elements from after p to last.
    {
        SgListRec<T>* first = First();
        SgListRecBlock<T>::FreeSgListRec(p->m_next, m_last);
        m_last = p;
        m_last->m_next = first;
    }
}

template<typename T>
void SgList<T>::Merge(const SgList<T>& list)
{
    //ASSERT(IsSortedAndUnique(ascending));
    //ASSERT(list.IsSortedAndUnique(ascending));
    if ((this == &list) || list.IsEmpty())
        return;
    else if (IsEmpty())
        operator=(list);
    else if (list.First()->m_data > m_last->m_data)
        // all new elements come after all old elements, just concat lists
        AppendList(list);
    else
    {
        SgListRec<T>* last1 = First();
        SgListRec<T>* curr2 = list.First();
        SgListRec<T>* last2 = list.First();
        SgListRec<T>* prev = m_last;
        SgListRec<T>* curr = First();

        bool isfirst1 = true, isfirst2 = true;
        for (;
             (isfirst1 || (curr != last1)) && (isfirst2 || (curr2 != last2));
             prev = curr, curr = curr->m_next)
        {
            for (;
                (isfirst2 || curr2 != last2)
                    && (curr->m_data > curr2->m_data);
                curr2 = curr2->m_next)
            {
                isfirst2 = false;
                if (prev->m_data != curr2->m_data)
                {
                    AppendData(curr2, &prev, ! isfirst1 && prev==m_last);
                    //ASSERT(IsSortedAndUnique(ascending));
                }
            }
            isfirst1 = false;
        }
        //ASSERT(IsSortedAndUnique(ascending));
        // copy rest of list
        prev = m_last;
        for (; isfirst2 || curr2 != last2; curr2 = curr2->m_next)
        {
            isfirst2 = false;
            if (prev->m_data != curr2->m_data)
            {
                AppendData(curr2, &prev, true);
                //ASSERT(IsSortedAndUnique(ascending));
            }
        }
    }
    //ASSERT(IsSortedAndUnique(ascending));
    //ASSERT(*this == duplicate);
}

template<typename T>
bool SgList<T>::IsSorted(bool ascending) const
{
    if (MinLength2())
    {
        SgListIterator<T> it(*this);
        T previous(*it); // get first element
        for (++it; it; ++it)
        {
            if ((ascending && (*it < previous))
                || (! ascending && (previous < *it)))
                return false;
            previous = *it;
        }
    }
    return true;
}

template<typename T>
bool SgList<T>::IsSortedAndUnique(bool ascending) const
{
    if (MinLength2())
    {
        SgListIterator<T> it(*this);
        T previous(*it); // get first element
        for (++it; it; ++it)
        {
            if (   (ascending && ! (previous < *it))
                || (! ascending && ! (*it < previous)))
                return false;
            previous = *it;
        }
    }
    return true;
}

template<typename T>
bool SgList<T>::SetsAreEqual(const SgList<T>& list2) const
{
    if (! IsLength(list2.Length()))
        return false;

    for (SgListIterator<T> it1(*this); it1; ++it1)
    {
        if (! list2.Contains(*it1))
            return false;
    }
    for (SgListIterator<T> it2(list2); it2; ++it2)
    {
        if (! Contains(*it2))
            return false;
    }
    return true;
}

template<typename T>
void SgList<T>::GetStatistics(int* numAlloc, int* numUsed)
{
    SgListRecBlock<T>::GetStatistics(numAlloc, numUsed);
}

template<typename T>
void SgList<T>::CollectGarbage()
{
    SgListRecBlock<T>::CollectGarbage();
}

template<typename T>
void SgList<T>::MemCheck()
{
    int numAlloc;
    int numUsed;
    SgListRecBlock<T>::CollectGarbage();
    SgListRecBlock<T>::GetStatistics(&numAlloc, &numUsed);
    SG_ASSERT(numAlloc == 0 && numUsed == 0);
}

//----------------------------------------------------------------------------

template<typename T>
SgListPairIterator<T>::SgListPairIterator(const SgList<T>& list)
    : m_list(list),
      m_nextItem(list.m_last ? list.First() : 0),
      m_nextPair(list.m_last ? list.First() : 0)
{
    // No pairs if only one item in list.
    if (m_nextItem == list.m_last)
        m_nextItem = 0;
}

template<typename T>
bool SgListPairIterator<T>::NextPair(T& elt1, T& elt2)
{
    // The outer loop uses 'm_nextItem' which points to the next item1
    // to be returned, or 0 if done. The inner loop uses 'm_nextPair'
    // which points to the item2 returned last.
    if (m_nextItem)
    {
        SG_ASSERT(m_nextPair);
        SG_ASSERT(m_nextPair != m_list.m_last);
        m_nextPair = m_nextPair->m_next;
        elt1 = m_nextItem->m_data;
        elt2 = m_nextPair->m_data;
        if (m_nextPair == m_list.m_last)
        {
            m_nextItem = m_nextItem->m_next;
            if (m_nextItem == m_list.m_last)
                m_nextItem = 0;
            else
                m_nextPair = m_nextItem;
        }
        return true;
    }
    else
        return false;
}

//----------------------------------------------------------------------------

template<class T>
bool SgListOf<T>::ContainsContent(const T& elt) const
{
    for (SgListIteratorOf<T> it(*this); it; ++it)
        if (**it == elt)
            return true;
    return false;
}

template<class T>
void SgListOf<T>::RemoveDuplicateContent()
{
    SgListOf<T> uniqueList;
    for (SgListIteratorOf<T> it(*this); it; ++it)
        if (! uniqueList.ContainsContent(**it))
            uniqueList.Append(*it);
    SwapWith(&uniqueList);
}

//----------------------------------------------------------------------------

#endif // SG_LIST_H
