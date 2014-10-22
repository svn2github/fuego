//----------------------------------------------------------------------------
/** @file GoUctDefaultMoveFilter.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTROOTFILTER_H
#define GOUCT_DEFAULTROOTFILTER_H

#include "GoLadder.h"
#include "GoUctMoveFilter.h"

class GoBoard;

//----------------------------------------------------------------------------

class GoUctDefaultMoveFilterParam
{
 public:

    GoUctDefaultMoveFilterParam();

    /** Prune unsuccesful ladder defense moves (unless the ladder would be
        short). */
    bool CheckLadders() const;

    /** See CheckLadders() */
    void SetCheckLadders(bool enable);

    /** Prune unsuccessful ladder chasing moves */
    bool CheckOffensiveLadders() const;

    /** See CheckOffensiveLadders() */
    void SetCheckOffensiveLadders(bool enable);

    /** Minimum ladder length necessary to prune loosing ladder defense moves.
        @see m_checkLadders */
    int MinLadderLength() const;

    /** See MinLadderLength() */
    void SetMinLadderLength(int length);

    /** Prune moves on the first line if there is no stone within a
        manhattan distance of 4 */
    bool FilterFirstLine() const;

    /** See FilterFirstLine() */
    void SetFilterFirstLine(bool enable);

    /** Prune moves based on group safety */
    bool CheckSafety() const;
    
    /** See CheckSafety() */
    void SetCheckSafety(bool enable);

 public:

    /** See CheckLadders() */
    bool m_checkLadders;

    /** See CheckOffensiveLadders() */
    bool m_checkOffensiveLadders;

    /** See MinLadderLength() */
    int m_minLadderLength;

    /** See FilterFiltLine() */
    bool m_filterFirstLine;

    /** See CheckSafety() */
    bool m_checkSafety;
};

inline bool GoUctDefaultMoveFilterParam::CheckLadders() const
{
    return m_checkLadders;
}

inline void GoUctDefaultMoveFilterParam::SetCheckLadders(bool enable)
{
    m_checkLadders = enable;
}

inline bool GoUctDefaultMoveFilterParam::CheckOffensiveLadders() const
{
    return m_checkOffensiveLadders;
}

inline void GoUctDefaultMoveFilterParam::SetCheckOffensiveLadders(bool enable)
{
    m_checkOffensiveLadders = enable;
}

inline bool GoUctDefaultMoveFilterParam::FilterFirstLine() const
{
    return m_filterFirstLine;
}

inline void GoUctDefaultMoveFilterParam::SetFilterFirstLine(bool flag)
{
    m_filterFirstLine = flag;
}

inline bool GoUctDefaultMoveFilterParam::CheckSafety() const
{
    return m_checkSafety;
}

inline void GoUctDefaultMoveFilterParam::SetCheckSafety(bool flag)
{
    m_checkSafety = flag;
}

inline int GoUctDefaultMoveFilterParam::MinLadderLength() const
{
    return m_minLadderLength;
}

inline void GoUctDefaultMoveFilterParam::SetMinLadderLength(int length)
{
    m_minLadderLength = length;
}

//----------------------------------------------------------------------------

/** Default root filter used by GoUctPlayer. */
class GoUctDefaultMoveFilter
    : public GoUctMoveFilter
{
public:
    GoUctDefaultMoveFilter(const GoBoard& bd, const GoUctDefaultMoveFilterParam &param);

    /** @name Pure virtual functions of GoUctMoveFilter */
    // @{

    /** Get moves to filter in the current position.
        This function is invoked by the player before the search, it does not
        need to be thread-safe. */
    std::vector<SgPoint> Get();

    // @} // @name

private:
    const GoBoard& m_bd;

    const GoUctDefaultMoveFilterParam &m_param;

    GoLadder m_ladder;

    /** Local variable in Get().
        Reused for efficiency. */
    mutable SgVector<SgPoint> m_ladderSequence;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTROOTFILTER_H
