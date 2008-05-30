//----------------------------------------------------------------------------
/** @file SgTimer.h
    Class SgTimer.
*/
//----------------------------------------------------------------------------

#ifndef SG_TIMER_H
#define SG_TIMER_H

#include "SgTime.h"

//----------------------------------------------------------------------------

/** Timer.
    For checking the elapsed time, without calling SgTime::Get each time.
*/
class SgTimer
{
public:
    /** Constructor.
        Also starts the timer.
    */
    SgTimer();

    /** Get elapsed time.
        Returns time since last start or between last start and stop if timer
        is stopped.
    */
    double GetTime() const;

    bool IsStopped() const;

    /** Check for timeout.
        Compares elapsed time with maxTime, but the comparison is done
        only every 16 calls for efficiency.
    */
    bool IsTimeOut(double maxTime);

    /** Reset timer. */
    void Start();

    /** Stop timer. */
    void Stop();

private:
    bool m_isStopped;

    /* For managing the frequency of calling SgTime::Get(). */
    unsigned int m_counter;

    /* Time when we start searching. */
    double m_timeStart;

    /* Elapsed time when timer was stopped. */
    double m_timeStop;

    /* Frequency to call SgTime::Get(). */
    static const int GET_TIME_FREQUENCY = 16;
};

inline SgTimer::SgTimer()
    : m_isStopped(false),
      m_counter(0)
{
    Start();
}

inline double SgTimer::GetTime() const
{
    if (m_isStopped)
        return m_timeStop;
    return (SgTime::Get() -  m_timeStart);
}

inline bool SgTimer::IsStopped() const
{
    return m_isStopped;
}

inline bool SgTimer::IsTimeOut(double maxTime)
{
    ++m_counter;
    if (m_counter % GET_TIME_FREQUENCY == 0)
    {
        double timeNow = SgTime::Get();
        if (timeNow - m_timeStart > maxTime)
        {
            --m_counter;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

inline void SgTimer::Start()
{
    m_timeStart = SgTime::Get();
    m_isStopped = false;
}

inline void SgTimer::Stop()
{
    SG_ASSERT(! IsStopped());
    m_timeStop = (SgTime::Get() -  m_timeStart);
    m_isStopped = true;
}

//----------------------------------------------------------------------------

#endif // SG_TIMER_H
