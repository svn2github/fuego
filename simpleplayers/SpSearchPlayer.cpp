//----------------------------------------------------------------------------
/** @file SpSearchPlayer.cpp
    See SpSearchPlayer.h
*/
//----------------------------------------------------------------------------
#include "SgSystem.h"
#include "SpSearchPlayer.h"

#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoSearch.h"
#include "SgHashTable.h"
#include "SgSearch.h"
#include "SgPointSet.h"
#include "SpUtil.h"

class ChaineySearch: public GoSearch
{
public:
    ChaineySearch(SgSearchHashTable* hash,
                  int depthLimit,
                  GoBoard& bd,
                  const SgPointSet& relevant);
    /**  */
    void Generate(SgList<SgPoint>* moves, int depth);

    /**  */
    int Evaluate(SgList<SgPoint>* sequence, bool* isExact, int depth);

    /**  */
    bool Execute(SgPoint move, int* delta, int depth)
    {
        SG_UNUSED(delta);
        SG_UNUSED(depth);
        return GoBoardUtil::PlayIfLegal(m_bd, move);
    }
    
    SgHashCode GetHashCode() const
    {
        return m_bd.GetHashCodeInclToPlay();
    }

    /** Takes back the most recent move successfully executed by Execute. */
    void TakeBack()
    {
        m_bd.Undo();
    }

    /** Return the current player. */
    SgBlackWhite GetToPlay() const
    {
        return m_bd.ToPlay();
    }
    
private:
    int m_depthLimit;
    
    GoBoard& m_bd;
    
    const SgPointSet m_relevant;
};

ChaineySearch::ChaineySearch(SgSearchHashTable* hash,
              int depthLimit,
              GoBoard& bd,
              const SgPointSet& relevant)
    : GoSearch(bd, hash),
      m_depthLimit(depthLimit),
      m_bd(bd),
      m_relevant(relevant)
     { }
     
void ChaineySearch::Generate(SgList<SgPoint>* moves, int depth)
{
    SG_UNUSED(moves);
    SG_UNUSED(depth);
}

int ChaineySearch::Evaluate(SgList<SgPoint>* sequence,
                            bool* isExact, int depth)
{
    SG_UNUSED(sequence);
    SG_UNUSED(isExact);
    SG_UNUSED(depth);
    
    return 0;
}

SgPoint SpSearchPlayer::GenMove(const SgTimeRecord& time, SgBlackWhite toPlay)
{
    SG_UNUSED(time);
    SgPointSet relevant =
        SpUtil::GetRelevantMoves(Board(), toPlay, true);

    Board().SetToPlay(toPlay);
    ChaineySearch s(m_hash, 3, Board(), relevant);
    
    return SG_NULLMOVE;
}

SpSearchPlayer::SpSearchPlayer(GoBoard& board) : GoPlayer(board),
    m_generator(board, this),
    m_hash(0)
{ }


