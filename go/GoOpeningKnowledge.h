//----------------------------------------------------------------------------
/** @file GoOpeningKnowledge.h
 Simple knowledge about corner moves and side extensions on a GoBoard.

 Used for priorknowledge and move features in Fuego.
 */
//----------------------------------------------------------------------------

#ifndef GO_OPENING_KNOWLEDGE_H
#define GO_OPENING_KNOWLEDGE_H

#include <vector>
#include "GoBoard.h"
#include "SgPoint.h"

namespace GoOpeningKnowledge
{
    typedef std::pair<SgPoint,int> MoveBonusPair;

    std::vector<SgPoint> FindCornerMoves(const GoBoard& bd);
    std::vector<MoveBonusPair> FindSideExtensions(const GoBoard& bd);

} // namespace GoOpeningKnowledge

#endif // GO_OPENING_KNOWLEDGE_H
