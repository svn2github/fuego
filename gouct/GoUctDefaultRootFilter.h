//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTROOTFILTER_H
#define GOUCT_DEFAULTROOTFILTER_H

#include "GoLadder.h"
#include "GoUctRootFilter.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Default root filter used by GoUctGlobalSearchPlayer. */
class GoUctDefaultRootFilter
    : public GoUctRootFilter
{
public:
    GoUctDefaultRootFilter(const GoBoard& bd);

    /** Get moves to filter in the current position.
        This function is invoked by the player before the search, it does not
        need to be thread-safe.
    */
    std::vector<SgPoint> Get();

private:
    const GoBoard& m_bd;

    GoLadder m_ladder;

    /* Prune loosing ladder defense moves. */
    bool m_checkLadders;

    /** Minimum ladder length necessary to prune loosing ladder defense moves.
        @see m_checkLadders
    */
    int m_minLadderLength;

    /** Local variable in Get().
        Reused for efficiency.
    */
    mutable SgList<SgPoint> m_ladderSequence;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTROOTFILTER_H
