//----------------------------------------------------------------------------
/** @file SpChaineyPlayer.h
    Average liberty of simple chains player
*/
//----------------------------------------------------------------------------

#ifndef SPCHAINEYPLAYER_H
#define SPCHAINEYPLAYER_H

#include <string>
#include "GoBoard.h"
#include "GoLadder.h"
#include "SgList.h"
#include "SgPoint.h"
#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** A simple block implementation for the Chainey player */
class SpBlock
{
public:
    SpBlock(const SgPointSet& points, SgBlackWhite color, GoBoard& bd) :
        m_anchor(SG_NULLPOINT),
        m_points(points),
        m_color(color),
        m_bd(bd),
        m_status(GO_LADDER_UNKNOWN)
    {
        ComputeAnchor();
        ComputeLadderStatus();
    }
    
    const SgPointSet& Points() const
    {
        return m_points;
    }
    
    SgBlackWhite Color() const
    {
        return m_color;
    }
    
    int NuLiberties(const GoBoard& board) const;
    
    SgPointSet Liberties(const GoBoard& board) const;
    
    void ComputeAnchor();
    
    void ComputeLadderStatus();

    GoLadderStatus LadderStatus() const
    {
        return m_status;
    }

    SgPoint Anchor() const
    {
        return m_anchor;
    }
    
private:

    SgPoint m_anchor;
    
    SgPointSet m_points;
    
    SgBlackWhite m_color;
    
    GoBoard& m_bd;
    
    GoLadderStatus m_status;
};

//----------------------------------------------------------------------------

/** A simple chain implementation for the Chainey player */
class SpChain
{
public:
    SpChain(const SgPointSet& points, SgBlackWhite color) :
        m_points(points),
        m_color(color)
    { }
    
    const SgPointSet& Points() const
    {
        return m_points;
    }
    
    SgBlackWhite Color() const
    {
        return m_color;
    }
    
    int NuLiberties(const GoBoard& board) const;

    int NuAllLiberties(const GoBoard& board) const;
    
    /** Liberties external to chain (no chain points) */
    SgPointSet Liberties(const GoBoard& board) const;
    
    /** Liberties including internal empty points in chain */
    SgPointSet AllLiberties(const GoBoard& board) const;

private:

    SgPointSet m_points;
    
    SgBlackWhite m_color;
};

//----------------------------------------------------------------------------

/** Tries to maximize liberty average of own minus opponent chains.
    Tends to build long chains with many liberties and some eyes.
    The strongest of the simple players.
*/
class SpChaineyMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    SpChaineyMoveGenerator(GoBoard& board, GoPlayer* player)
        : Sp1PlyMoveGenerator(board), m_player(player)
    { }

    int Evaluate();
    
    int LibertyAveragex10(const GoBoard& board, SgBlackWhite color);
    int ChainStrength(const GoBoard& board, SgBlackWhite color);

private:
    void ComputeChains();

    void DeleteChains();

    SpChain* ChainAt(SgPoint p, SgBlackWhite color) const;
    
    SpChain* ChainOf(const SpBlock* b) const
    {
        return ChainAt(b->Anchor(), b->Color());
    }

    void NbChains(SgPoint p, SgBlackWhite color, SgList<SpChain*>& chains);

    void MergeChains(SgList<SpChain*> chains,
                const SgPointSet& chainPts, SgBlackWhite color);

    void MergeChains(SgList<SpChain*> chains,
                SgPoint p, SgBlackWhite color);

    bool MergeNeighboringChains(SgPoint p, SgBlackWhite color,
                                SgBWSet& chainPts);
    
    void MergeNeighborChainsOfDeadBlocks();

    SgPointSet ProtectedLibs(SgBlackWhite color) const;

    void MergeSharedLibChains(SgBWSet& chainPts);

    bool SomeBlocksDead(const SpChain* c) const;

    void AdjChains(const SpChain* c, SgList<SpChain*>* adj) const;
    
    SgList<SpBlock*> m_blocks;
    
    SgList<SpChain*> m_chains;
    
    GoPlayer* m_player;
};

//----------------------------------------------------------------------------

/** A player similar to average liberty player, works on chains not blocks */
class SpChaineyPlayer
    : public SpSimplePlayer
{
public:
    SpChaineyPlayer(GoBoard& board, bool atarigo = false)
        : SpSimplePlayer(board, new SpChaineyMoveGenerator(board, this),
                         atarigo)
    { }

    std::string Name() const
    {
        return "Chainey";
    }
    
    bool UseFilter() const
    { 
        return ! PlaysAtariGo(); 
    }
};

//----------------------------------------------------------------------------

#endif // SPCHAINEYPLAYER_H

