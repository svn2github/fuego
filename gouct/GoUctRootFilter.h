//----------------------------------------------------------------------------
/** @file GoUctRootFilter.h */
//----------------------------------------------------------------------------

#ifndef GOUCTROOTFILTER_H
#define GOUCTROOTFILTER_H

#include <vector>
#include "SgPoint.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Compute a list of moves to filter at the root node of the current
    search.
*/
class GoUctRootFilter
{
public:
    GoUctRootFilter(const GoBoard& bd);

    /** Get moves to filter in the current position.
        This function is invoked by the player before the search, it does not
        need to be thread-safe.
    */
    std::vector<SgPoint> Get();

private:
    const GoBoard& m_bd;

    /** Should move be filtered? */
    bool FilterMove(SgMove move) const;
};

//----------------------------------------------------------------------------

#endif // GOUCTROOTFILTER_H
