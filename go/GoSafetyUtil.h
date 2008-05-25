//----------------------------------------------------------------------------
/** @file GoSafetyUtil.h
    Utility functions for the static and search-based safety solvers.
*/
//----------------------------------------------------------------------------

#ifndef GOSAFETYUTIL_H
#define GOSAFETYUTIL_H

#include "GoBoard.h"
#include "GoRegionBoard.h"
#include "SgBlackWhite.h"
#include "SgList.h"
#include "SgMiaiStrategy.h"
#include "SgPoint.h"
#include "SgPointSet.h"

//----------------------------------------------------------------------------
/** Add pts to *safe[color] */
void AddToSafe(const GoBoard& board, const SgPointSet& pts,
               SgBlackWhite color, SgBWSet* safe, const char* reason,
               int depth, bool addBoundary);

/** Stronger version of IsTerritory that uses region information
    This version checks for opponent nakade inside the area.
    Useful for proving safe semeai test cases after resolving semeai.
*/
bool ExtendedIsTerritory(const GoBoard& board, GoRegionBoard* regions, 
                 const SgPointSet& pts,
                 const SgPointSet& safe, SgBlackWhite color);

/** Simple static territory check for surrounded area */
bool IsTerritory(const GoBoard& board, const SgPointSet& pts,
                 const SgPointSet& safe, SgBlackWhite color);

/** Given set of stones, reduce to block anchors */
void ReduceToAnchors(const GoBoard& board, const SgPointSet& stones,
                     SgList<SgPoint>* anchors);

/** helper function for 1-vitality test
    try to find two matching liberties for point p, subtract them from libs
    if found.
*/
bool Find2Libs(SgPoint p, SgPointSet* libs);

/** helper function for 1-vitality test, similar to Find2Libs(), but
    try to find miaiPair of two best liberties 
    (not shared with other interior points).
*/
bool Find2BestLibs(SgPoint p, const SgPointSet& libs,
                   SgPointSet interior, SgMiaiPair* miaiPair);

/** Extended version of MightMakeLife. Recognizes some nakade shapes
    as dead. Useful mostly for semeai solver.
*/
bool ExtendedMightMakeLife(const GoBoard& board, GoRegionBoard* regions,
                   const SgPointSet& area, 
                   const SgPointSet& safe,
                   SgBlackWhite color);

/** Test whether color can make 2 eyes inside a surrounded area.
    Precondition: area surrounded by safe stones of opponent.
    Basic test, handles 1 and 2 point eyes only.
 */
bool MightMakeLife(const GoBoard& board, 
                   const SgPointSet& area, 
                   const SgPointSet& safe,
                   SgBlackWhite color);
//----------------------------------------------------------------------------

#endif // GOSAFETYUTIL_H
