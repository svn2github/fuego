//----------------------------------------------------------------------------
/** @file SgStack.h
    Stack class.
*/
//----------------------------------------------------------------------------

#ifndef SG_STACK_H
#define SG_STACK_H

//----------------------------------------------------------------------------

/** Stack with up to size objects of class T */
template <class T, int size>
class SgStack
{
public:
    SgStack()
        : m_sp(0)
    { }

    ~SgStack() {}

    void Push(T data)
    {
        SG_ASSERT(m_sp < size - 1);
        m_stack[m_sp++] = data;
    }

    T Pop()
    {
        SG_ASSERT(0 < m_sp);
        return m_stack[--m_sp];
    }

    bool IsEmpty() const
    {
        return m_sp == 0;
    }

    bool NonEmpty() const
    {
        return m_sp != 0;
    }

    int Size() const
    {
        return m_sp;
    }

    const T& Top() const
    {
        SG_ASSERT(0 < m_sp);
        return m_stack[m_sp-1];
    }

    const T& operator[](int index) const
    {
        SG_ASSERT(index < m_sp);
        return m_stack[index];
    }

private:
    int m_sp;

    T m_stack[size];

    /** not implemented */
    SgStack(const SgStack&);

    /** not implemented */
    SgStack& operator=(const SgStack&);
};

//----------------------------------------------------------------------------

#endif // SG_STACK_H
