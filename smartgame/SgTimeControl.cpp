//----------------------------------------------------------------------------
/** @file SgTimeControl.cpp
    See SgTimeControl.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgTimeControl.h"

#include "SgIO.h"
#include "SgTimeRecord.h"

using namespace std;

//----------------------------------------------------------------------------

SgTimeControl::~SgTimeControl()
{
}

//----------------------------------------------------------------------------

SgDefaultTimeControl::SgDefaultTimeControl()
    : m_fastOpenFactor(0.25),
      m_fastOpenMoves(0),
      m_minTime(0),
      m_remainingConstant(1.0),
      m_reserveMovesConstant(0.0)
{
}

double SgDefaultTimeControl::FastOpenFactor() const
{
    return m_fastOpenFactor;
}

int SgDefaultTimeControl::FastOpenMoves() const
{
    return m_fastOpenMoves;
}

double SgDefaultTimeControl::RemainingConstant() const
{
    return m_remainingConstant;
}

double SgDefaultTimeControl::ReserveMovesConstant() const
{
    return m_reserveMovesConstant;
}

void SgDefaultTimeControl::SetRemainingConstant(double value)
{
    m_remainingConstant = value;
}

void SgDefaultTimeControl::SetReserveMovesConstant(double value)
{
    m_reserveMovesConstant = value;
}

void SgDefaultTimeControl::SetFastOpenFactor(double factor)
{
    m_fastOpenFactor = factor;
}

void SgDefaultTimeControl::SetFastOpenMoves(int nummoves)
{
    m_fastOpenMoves = nummoves;
}

void SgDefaultTimeControl::SetMinTime(double mintime)
{
    m_minTime = mintime;
}

double SgDefaultTimeControl::TimeForCurrentMove(const SgTimeRecord& time,
                                                bool quiet)
{
    SgBlackWhite toPlay;
    int estimatedRemainingMoves;
    int movesPlayed;
    GetPositionInfo(toPlay, movesPlayed, estimatedRemainingMoves);
    const bool useOvertime = time.UseOvertime();
    const bool isInOvertime = (useOvertime && time.MovesLeft(toPlay) > 0);
    int remainingMoves = 0;
    if (isInOvertime)
        remainingMoves = time.MovesLeft(toPlay);
    else
    {
        int estimatedTotalMoves = movesPlayed + estimatedRemainingMoves;
        int remainingMovesLimit =
            static_cast<int>(m_remainingConstant * estimatedTotalMoves);
        remainingMoves = min(estimatedRemainingMoves, remainingMovesLimit);
        if (! useOvertime)
        {
            int reserveMoves =
                static_cast<int>(m_reserveMovesConstant
                                 * estimatedTotalMoves);
            reserveMoves = max(reserveMoves, 1);
            remainingMoves += reserveMoves;
        }
    }
    remainingMoves = max(remainingMoves, 1);

    double timeLeft = time.TimeLeft(toPlay);
    double timeForMove = timeLeft / remainingMoves - time.Overhead();

    // Don't use quite as much time for the first few moves, makes no real
    // difference in the quality of the moves.
    if (m_fastOpenMoves > 0 && movesPlayed <= m_fastOpenMoves)
        timeForMove *= m_fastOpenFactor;

    timeForMove = max(timeForMove, m_minTime);

    if (! quiet)
    {
        SgDebug() << "SgDefaultTimeControl: timeLeft=" << timeLeft;
        if (useOvertime)
            SgDebug() << " movesLeft=" << time.MovesLeft(toPlay);
        SgDebug() << " remainingMoves=" << remainingMoves << " timeForMove="
                  << timeForMove << '\n';
    }

    return timeForMove;
}

//----------------------------------------------------------------------------

SgObjectWithDefaultTimeControl::~SgObjectWithDefaultTimeControl()
{
}

//----------------------------------------------------------------------------
