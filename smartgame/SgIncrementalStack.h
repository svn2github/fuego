//----------------------------------------------------------------------------
/** @file SgIncrementalStack.h
    Incremental Update Stack for fast undo during search.
*/
//----------------------------------------------------------------------------

#ifndef SG_INCREMENTALSTACK_H
#define SG_INCREMENTALSTACK_H

#include "SgBoardColor.h"
#include "SgList.h"
#include "SgPoint.h"

class SgPointSet;
class SgBWSet;

//----------------------------------------------------------------------------

/** constants for types of stack entries */
enum SgIncrementalStackEvent
{
    kNewPoints          = 1000,
    kAddEmpty           = 1001,
    kNewSafe            = 1002,
    kNewUnSafe          = 1003,
    kUnsafeToSafe       = 1004,
    kCaptures           = 1005,
    kMove               = 1006,
    kNextMove           = 1007,
    kUnsafeToHalfSafe   = 1008,
    kCaptureHalfSafes   = 1009
};

/** Incremental Update Stack for fast undo during search.
    Used by GoRegionBoard.
*/
class SgIncrementalStack
{
public:
    SgIncrementalStack(){}

    void Clear();

    bool IsEmpty() const {return m_stack.IsEmpty();}

    void PushPts(int type, SgEmptyBlackWhite col, const SgPointSet& pts);

    void PushPt(int type, SgEmptyBlackWhite col, SgPoint pt);
    
    void PushPtr(void* ptr)
    {
        m_stack.Push(IntOrPtr(ptr));
    }
    
    void PushPtrEvent(int type, void* ptr);

    void PushInt(int i)
    {
        m_stack.Push(IntOrPtr(i));
    }

    void StartMoveInfo();

    SgIncrementalStackEvent PopEvent()
    {
        return static_cast<SgIncrementalStackEvent>(m_stack.Pop().m_int);
    }

    void* PopPtr()
    {
        return m_stack.Pop().m_ptr;
    }

    int PopInt()
    {
        return m_stack.Pop().m_int;
    }
    
    void SubtractPoints(SgPointSet* set);

    void AddPoints(SgPointSet* set);

    void SubtractPoints(SgBWSet* set);

    void AddPoints(SgBWSet* set);

    void SubtractAndAddPoints(SgBWSet* subtractset, SgBWSet* addset);

    void SubtractAndAddPoints(SgPointSet* subtractset, SgBWSet* addset);

    void SubtractAndAddPoints(SgBWSet* subtractset, SgPointSet* addset);

private:
    /** Entry for SgIncrementalStack */
    union IntOrPtr
    {
        IntOrPtr()
        {
        }

        IntOrPtr(int i)
        {
            m_int = i;
        }

        IntOrPtr(void* ptr)
        {
            m_ptr = ptr;
        }

        int m_int;

        void* m_ptr;
    };

    /** Stores incremental state changes for execute/undo moves */
    SgList<IntOrPtr> m_stack;
};

//----------------------------------------------------------------------------

#endif // SG_INCREMENTALSTACK_H
