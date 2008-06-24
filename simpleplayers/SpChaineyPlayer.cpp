//----------------------------------------------------------------------------
/** @file SpChaineyPlayer.cpp
    See SpChaineyPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpChaineyPlayer.h"

#include "GoInfluence.h"
#include "GoLadder.h"
#include "SgConnCompIterator.h"
#include "SgDebug.h"
#include "SgPoint.h"

typedef SgListIterator<SpChain*> ChainIterator;
typedef SgListIterator<SpBlock*> BlockIterator;

//----------------------------------------------------------------------------

namespace {

int GetWeight(int libs)
{
    switch (libs)
    {
        case 1: return 0;
        case 2: return 20;
        case 3: return 50;
        case 4: return 75;
        default: return 95 + libs;
    }
}

}
/** Average number of liberties of blocks of color, multiplied by 10 */
int SpChaineyMoveGenerator::LibertyAveragex10(const GoBoard& board,
                                              SgBlackWhite color)
{
    int nuLibs = 0, nuBlocks = 0;
    for (ChainIterator it(m_chains); it; ++it)
    {
        if ((*it)->Color() == color)
        {
            ++nuBlocks;
            nuLibs += (*it)->NuAllLiberties(board);
        }
    }
    return (nuBlocks == 0) ? 0 : 10 * nuLibs / nuBlocks;
}

int SpChaineyMoveGenerator::ChainStrength(const GoBoard& board,
                                          SgBlackWhite color)
{
    int total = 0;
    for (BlockIterator it(m_blocks); it; ++it)
    {
        int weight = 0; // for unsettled
        if ((*it)->LadderStatus() == GO_LADDER_CAPTURED)
            weight = -100;
        else if ((*it)->LadderStatus() == GO_LADDER_ESCAPED)
        {
            const SpChain* c = ChainOf(*it);
            weight = GetWeight(c->NuAllLiberties(board));
        }
        weight *= (*it)->Points().Size();
        if ((*it)->Color() != color)
            weight = -weight;
        total += weight;
    }
    return total;
}

void SpChaineyMoveGenerator::DeleteChains()
{
    for (ChainIterator it(m_chains); it; ++it)
        delete *it;
    m_chains.Clear();
    for (BlockIterator it(m_blocks); it; ++it)
        delete *it;
    m_blocks.Clear();
}

namespace{

int InfluenceScore(const GoBoard& board, SgBlackWhite color)
{
    SgBWSet result;
    GoInfluence::FindInfluence(board, 7, 3, &result);
    return result[color].Size() - result[SgOppBW(color)].Size();
}

bool HasShared(const GoBoard& bd, const SpChain* c1, const SpChain* c2,
               const SgPointSet& chainPts, SgPointSet& shared)
{
    SG_ASSERT(shared.IsEmpty());
    SgPointSet set =   c1->Liberties(bd)
                     & c2->Liberties(bd);
    if (set.MinSetSize(2))
    {
        set -= chainPts; // cannot reuse marked
        if (set.MinSetSize(2))
        {
            SgSetIterator it(set);
            shared.Include(*it);
            ++it;
            shared.Include(*it);
            return true;
        }
    }
    return false;
}
} // namespace

void SpChaineyMoveGenerator::MergeSharedLibChains(SgBWSet& chainPts)
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        SpChain* c1;
        SpChain* c2;
        for (SgListPairIterator<SpChain*> it(m_chains); it.NextPair(c1, c2); )
        {
            SgPointSet shared;
            SgBlackWhite color = c1->Color();
            if (   color == c2->Color()
                && HasShared(m_board, c1, c2, chainPts[color], shared)
               )
            {
                SgList<SpChain*> chains;
                chains.Append(c1);
                chains.Append(c2);
                MergeChains(chains, shared, color);
                chainPts[color]|= shared;
                changed = true;
                break;
            }
        }
    }
}

void SpBlock::ComputeAnchor()
{
    const SgPoint p = m_points.PointOf();
    m_anchor = m_bd.Anchor(p);
    SG_ASSERT(p == m_anchor);
}

void SpBlock::ComputeLadderStatus()
{
    SgPoint defender = Anchor();
    SgPoint toCapture;
    SgPoint toEscape;
    m_status = GoLadderUtil::LadderStatus(m_bd, defender, true,
                                          &toCapture, &toEscape);
    SG_ASSERT(   m_status == GO_LADDER_CAPTURED
              || m_status == GO_LADDER_ESCAPED
              || m_status == GO_LADDER_UNSETTLED);
}

void SpChaineyMoveGenerator::ComputeChains()
{
    SG_ASSERT(m_blocks.IsEmpty());
    SG_ASSERT(m_chains.IsEmpty());

    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color = *it;
        for (SgConnCompIterator it(m_board.All(color), m_board.Size());
             it; ++it)
        {
            SpBlock* b = new SpBlock(*it, color, m_board);
            m_blocks.Append(b);
            m_chains.Append(new SpChain(*it, color));
        }
    }
    SgBWSet chainPts(m_board.All(SG_BLACK), m_board.All(SG_WHITE));
   
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (SgSetIterator it(m_board.AllEmpty() - chainPts.Both()); it; ++it)
        {
            const SgPoint p(*it);
            if (m_board.NumEmptyNeighbors(p) < 3)
            {
                if (   m_board.NumNeighbors(p, SG_BLACK) >= 2
                    && MergeNeighboringChains(p, SG_BLACK, chainPts)
                   )
                    changed = true;
                if (   m_board.NumNeighbors(p, SG_WHITE) >= 2
                    && MergeNeighboringChains(p, SG_WHITE, chainPts)
                   )
                    changed = true;
            }
        }
    }
    
    const int v = m_player->Variant();
    if (v != 2)
    {
        MergeSharedLibChains(chainPts);
        if (v != 1)
            MergeNeighborChainsOfDeadBlocks();
    }
}

SpChain* SpChaineyMoveGenerator::ChainAt(SgPoint p, SgBlackWhite color) const
{
    for (ChainIterator it(m_chains); it; ++it)
    {
        if ((*it)->Color() == color
            && (*it)->Points().Contains(p)
           )
            return *it;
    }
    return 0;
}

void SpChaineyMoveGenerator::NbChains(SgPoint p,
                        SgBlackWhite color,
                        SgList<SpChain*>& chains)
{
    for (SgNb4Iterator it(p); it; ++it)
    {
        if (m_board.IsColor(*it, color))
        {
            SpChain* c = ChainAt(*it, color);
            SG_ASSERT(c);
            chains.Include(c);
        }
    }
}

void SpChaineyMoveGenerator::MergeChains(SgList<SpChain*> chains,
            SgPoint p, SgBlackWhite color)
{
    SgPointSet pts;
    pts.Include(p);
    MergeChains(chains, pts, color);
}

void SpChaineyMoveGenerator::MergeChains(SgList<SpChain*> chains,
            const SgPointSet& chainPts, SgBlackWhite color)
{
    SG_UNUSED(chainPts);
    SgPointSet pts;
    for (ChainIterator it(chains); it; ++it)
        pts |= (*it)->Points();
    if (m_player->Variant() == 3)
        pts |= chainPts;
    for (ChainIterator it(chains); it; ++it)
        delete *it;
    m_chains.Exclude(chains);
    m_chains.Append(new SpChain(pts, color));
}

bool SpChaineyMoveGenerator::MergeNeighboringChains(SgPoint p, 
            SgBlackWhite color, SgBWSet& chainPts)
{
    SgList<SpChain*> chains;
    NbChains(p, color, chains);
    if (chains.MaxLength(1))
        return false;
    
    bool byLdr, ko;
    if (GoLadderUtil::IsProtectedLiberty(m_board, p, color, byLdr, ko)
        && ! ko)
    {
        MergeChains(chains, p, color);
        chainPts[color].Include(p);
        return true;
    }
    return false;
}

bool SpChaineyMoveGenerator::SomeBlocksDead(const SpChain* c) const
{
    const SgBlackWhite color = c->Color();
    for (BlockIterator it(m_blocks); it; ++it)
    {
        const SpBlock* block = *it;
        if (   block->LadderStatus() == GO_LADDER_CAPTURED
            && block->Color() == color
            && ChainOf(block) == c
           )
        {
            return true;
        }
    }
    return false;
}

void SpChaineyMoveGenerator::MergeNeighborChainsOfDeadBlocks()
{
    for (SgBWIterator it; it; ++it)
    {
        const SgBlackWhite color = *it;
        for (BlockIterator it(m_blocks); it; ++it)
        {
            const SpBlock* block = *it;
            if (   block->LadderStatus() == GO_LADDER_CAPTURED
                && block->Color() == color
               )
            {
                // changed to only merge non-dead chains because of
                // bug with a killing but dead single stone being merged
                // to the outside, setting everything to dead later on.
                
                SgList<SpChain*> nonDeadAdj;
                const SpChain* c = ChainOf(block);
                SgList<SpChain*> adj;
                AdjChains(c, &adj);
                for (ChainIterator it(adj); it; ++it)
                {
                    if (! SomeBlocksDead(*it))
                        nonDeadAdj.Append(*it);
                }
                
                if (nonDeadAdj.MinLength(2))
                {
                    MergeChains(nonDeadAdj, block->Points(), SgOppBW(color));
                }
            }
        }
    }
}

int SpChaineyMoveGenerator::Evaluate()
{
    ComputeChains();
    // We are Opponent since this is after executing our move
    SgBlackWhite player = m_board.Opponent();
    int score;
    if (m_player->Variant() != 4)
        score = ChainStrength(m_board, player);
    else
        score = LibertyAveragex10(m_board, player)
              - LibertyAveragex10(m_board, SgOppBW(player));
    DeleteChains();
    int influenceScore = InfluenceScore(m_board, player);
    score = 100 * score + influenceScore;
    return score;
}

int SpChain::NuLiberties(const GoBoard& bd) const
{
    return Liberties(bd).Size();
}

int SpChain::NuAllLiberties(const GoBoard& bd) const
{
    return AllLiberties(bd).Size();
}

SgPointSet SpChain::AllLiberties(const GoBoard& bd) const
{
    const int size = bd.Size();
    SgPointSet pts = m_points & bd.All(m_color);
    return  (  pts.Border(size)
             & bd.AllEmpty()
            );
}

SgPointSet SpChain::Liberties(const GoBoard& bd) const
{
    return  AllLiberties(bd) - m_points;
}

void SpChaineyMoveGenerator::AdjChains(const SpChain* c,
                            SgList<SpChain*>* adj) const
{
    const SgBlackWhite color = SgOppBW(c->Color());
    const SgPointSet points = c->Points().Border(m_board.Size());
    for (ChainIterator it(m_chains); it; ++it)
    {
        if ((*it)->Color() == color
            && (*it)->Points().Overlaps(points)
           )
            adj->Append(*it);
    }
}

