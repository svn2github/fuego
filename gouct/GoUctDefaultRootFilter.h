//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTROOTFILTER_H
#define GOUCT_DEFAULTROOTFILTER_H

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

    /** Should move be filtered? */
    bool FilterMove(SgPoint move) const;
};

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTROOTFILTER_H
