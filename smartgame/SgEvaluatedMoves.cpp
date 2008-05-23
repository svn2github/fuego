//----------------------------------------------------------------------------
/** @file SgEvaluatedMoves.cpp
    @see SgEvaluatedMoves.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgEvaluatedMoves.h"

#include <iomanip>
#include "SgIO.h"
#include "SgRandom.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

void SgEvaluatedMoves::AddMove(SgPoint move, int value)
{
    if (SgPointUtil::InBoardRange(move) && m_relevant[move])
    {
        if (value > m_bestValue)
        {
            m_bestValue = value;
            m_moveList.Clear();
        }
        if (value >= m_bestValue)
            m_moveList.Append(move);
    }
}

void SgEvaluatedMoves::AddMoves(const SgPointSet& moves, int value)
{
    for (SgSetIterator it(moves); it; ++it)
        AddMove(*it, value);
}

void SgEvaluatedMoves::AddMoves(const SgList<SgPoint>& moves, int value)
{
    for (SgListIterator<SgPoint> it(moves); it; ++it)
        AddMove(*it, value);
}

SgPoint SgEvaluatedMoves::BestMove()
{
    if (m_moveList.IsEmpty())
        return SG_PASS;
    else
        return m_moveList[SgRandom::Global().Int(m_moveList.Length()) + 1];
}

void SgEvaluatedMoves::BestMoves(SgList<SgPoint>& best, int nuMoves) const
{
    SG_UNUSED(nuMoves);
    best = m_moveList; // AR: cut off at 'nuMoves'??
}

//----------------------------------------------------------------------------

SgEvaluatedMovesArray::SgEvaluatedMovesArray(const SgPointSet& relevant,
                                             int boardSize)
    : SgEvaluatedMoves(relevant),
      m_boardSize(boardSize)
{
    for (int i = 0; i < SG_MAXPOINT; ++i)
        m_value[i] = 0;
}

void SgEvaluatedMovesArray::AddMove(SgPoint move, int value)
{
    if (SgPointUtil::InBoardRange(move) && m_relevant[move])
    {
        m_value[move] += value;
        SgEvaluatedMoves::AddMove(move, m_value[move]);
    }
}

void SgEvaluatedMovesArray::ReduceMove(SgPoint move, int value)
{
    if (SgPointUtil::InBoardRange(move) && m_relevant[move])
    {
        m_value[move] -= value;
        SgEvaluatedMoves::AddMove(move, m_value[move]);
    }
}

SgPoint SgEvaluatedMovesArray::SelectNextBest(SgList<SgPoint>& bestSoFar)
    const
{
    int bestValue = s_minValue; SgPoint best = 0;
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        if ((m_value[p] > bestValue) && ! bestSoFar.Contains(p))
        {
            bestValue = m_value[p];
            best = p;
        }
    }
    return best;
}

void SgEvaluatedMovesArray::BestMoves(SgList<SgPoint>& best, int nuMoves)
    const
{
    best.Clear();
    while (--nuMoves >= 0)
    {
        int nextBest = SelectNextBest(best);
        best.Append(nextBest);
    }
}

void SgEvaluatedMovesArray::Write() const
{
    int i, j;
    SgDebug() << "      ";
    for (j=1; j <= m_boardSize; ++j)
    {
        SgDebug() << SgPointUtil::Letter(j) << "    ";
    }

    for (i = 1; i <= m_boardSize; ++i)
    {
        SgDebug() << '\n' << setw(2) << i;
        for (j = 1; j <= m_boardSize; ++j)
            SgDebug() << setw(5) << m_value[SgPointUtil::Pt(j, i)];
        SgDebug() << '\n';
    }
}

void SgEvaluatedMovesArray::Clear()
{
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
        m_value[p] = 0;
}

//----------------------------------------------------------------------------

