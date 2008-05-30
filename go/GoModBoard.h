//----------------------------------------------------------------------------
/** @file GoModBoard.h */
//----------------------------------------------------------------------------

#ifndef GO_MODBOARD_H
#define GO_MODBOARD_H

#include "GoAssertBoardRestored.h"

//----------------------------------------------------------------------------

/** Make a const board temporarily modifiable.
    Allows functions to use a const board for performing temporary operations
    on the board (e.g. searches), as long as the board is in the same state
    after the function is finished. This class facilitates const-correctness
    and encapsulation, because it allows the owner of a board, which is the
    only one who is allowed to do persistent changes on the board, to hand out
    only a const reference to other code. The other code can still use the
    board for temporary operations without needing a copy of the board.
    GoModBoard does a const_cast from the const reference to a non-const
    reference in its constructor and checks with GoAssertBoardRestored in its
    destructor that the board is returned in the same state.

    Example:
    @code
    // myFunction is not supposed to do persistent changes on the board
    // and therefore gets a const-reference. However it wants to use
    // the board temporarily
    void myFunction(const GoBoard& constBoard)
    {
        GoModBoard modBoard(constBoard);
        GoBoard& bd = modBoard.Board(); // get a nonconst-reference

        // ... play some moves and undo them

        // end of lifetime for modBoard, GoAssertBoardRestored is
        // automatically called in the destructor of modBoard
    }
    @endcode
*/
class GoModBoard
{
public:
    /** Constructor for later explicit call of Init() */
    GoModBoard();

    /** Constructor.
        Remembers the current board state.
    */
    GoModBoard(const GoBoard& bd);

    /** Destructor.
        Checks with assertions that the board state is restored.
    */
    ~GoModBoard();

    /** Automatic conversion to non-const reference.
        Allows to pass GoModBoard to functions that expect a non-const GoBoard
        reference without explicitely calling GoModBoard.Board().
    */
    operator GoBoard&() const;

    /** Explicit conversion to non-const reference. */
    GoBoard& Board() const;

    /** Explicitely remember current board state.
        Can be used together with AssertRestored() in cases where the
        beginning and end of the temporary changes on the board are in
        different function calls and therefore cannot easily by coupled to
        the lifetime of a GoModBoard instance.
    */
    void Init(GoBoard& bd);

    /** Explicitely check with assertions that the board state is restored.
        See Init()
    */
    void AssertRestored();

private:
    GoBoard* m_bd;

    GoAssertBoardRestored m_assertRestored;
};

inline GoModBoard::GoModBoard()
    : m_bd(0)
{
}

inline GoModBoard::GoModBoard(const GoBoard& bd)
    : m_bd(const_cast<GoBoard*>(&bd)),
      m_assertRestored(bd)
{
}

inline GoModBoard::~GoModBoard()
{
}

inline GoModBoard::operator GoBoard&() const
{
    return Board();
}

inline void GoModBoard::AssertRestored()
{
    m_assertRestored.AssertRestored();
}

inline GoBoard& GoModBoard::Board() const
{
    SG_ASSERT(m_bd != 0);
    return *m_bd;
}

inline void GoModBoard::Init(GoBoard& bd)
{
    m_bd = &bd;
    m_assertRestored.Init(bd);
}

//----------------------------------------------------------------------------

#endif // GO_MODBOARD_H
