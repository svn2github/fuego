//----------------------------------------------------------------------------
/** @file GoUctGammaMoveGenerator.h */
//----------------------------------------------------------------------------

#ifndef GOUCTGAMMAMOVEGENERATOR_H
#define GOUCTGAMMAMOVEGENERATOR_H

#include <iostream>
#include "GoBoardUtil.h"
#include "GoUctPatterns.h"
#include "GoUctUtil.h"
#include "SgWrite.h"

template<class BOARD>
class GoUctGammaMoveGenerator
{
public:
    GoUctGammaMoveGenerator(const BOARD& bd,
            float biasPatternGammaThreshold,
            GoUctPatterns<BOARD>& patterns,
            SgRandom& rand);

    /** Generate move with probability according to gamma values */
    SgPoint GenerateBiasedPatternMove();

    float GetGammaValueForMoveAt(const int index) const;
    
    SgPoint GetGammaMoveAt(const int index) const;
    
    int NuMoves() const;

    void WriteMovesAndGammas(std::ostream& stream) const;

private:
    /** generate pattern moves with gamma values */
    bool GenerateAllPatternMoves();
    
    void GeneratePatternMove(SgPoint p);
    
    void GeneratePatternMove2(SgPoint p, SgPoint lastMove);
    
    SgPoint SelectAccordingToGammas();

    const BOARD& m_bd;

    float m_biasPatternGammaThreshold;

    GoUctPatterns<BOARD>& m_patterns;

    /** Copy of GoUctPlayoutPolicy::m_moves */
    GoPointList m_moves; 
    
    SgRandom& m_random;
    
    /** gamma values for each pattern move */
    SgArrayList<float, SG_MAX_ONBOARD + 1> m_gammas; 
    
    /** partial sums of gamma values. 
        m_gammasSums[i] = sum(m_gammas[0....i]) */
    SgArrayList<float,SG_MAX_ONBOARD + 1> m_gammaSums; 
    
    /** subset of m_moves, filtered by GoUctUtil::GeneratePoint */
    GoPointList m_movesGammas; 
};

template<class BOARD>
inline SgPoint GoUctGammaMoveGenerator<BOARD>
::GetGammaMoveAt(const int index) const
{
    SG_ASSERT(index < m_movesGammas.Length());
    return m_movesGammas[index];
}

template<class BOARD>
inline float GoUctGammaMoveGenerator<BOARD>
::GetGammaValueForMoveAt(const int index) const
{
    if (m_gammaSums.Length() == 0)
        return 0;
    SG_ASSERT(index < m_gammaSums.Length());
    return index == 0 ? 
           m_gammaSums[0] :
           m_gammaSums[index] - m_gammaSums[index - 1];
}

template<class BOARD>
inline int GoUctGammaMoveGenerator<BOARD>::NuMoves() const
{
    return m_movesGammas.Length();
}

template<class BOARD>
GoUctGammaMoveGenerator<BOARD>::GoUctGammaMoveGenerator(
                                    const BOARD& bd,
                                    float biasPatternGammaThreshold,
                                    GoUctPatterns<BOARD>& patterns, 
                                    SgRandom& random) 
	: m_bd(bd),
      m_biasPatternGammaThreshold(biasPatternGammaThreshold),
      m_patterns(patterns),
      m_random(random)
{ }

template<class BOARD>
bool GoUctGammaMoveGenerator<BOARD>::GenerateAllPatternMoves()
{
    SG_ASSERT(m_moves.IsEmpty());
    SgPoint lastMove = m_bd.GetLastMove();
    SG_ASSERT(! SgIsSpecialMove(lastMove));
    GeneratePatternMove(lastMove + SG_NS - SG_WE);
    GeneratePatternMove(lastMove + SG_NS);
    GeneratePatternMove(lastMove + SG_NS + SG_WE);
    GeneratePatternMove(lastMove - SG_WE);
    GeneratePatternMove(lastMove + SG_WE);
    GeneratePatternMove(lastMove - SG_NS - SG_WE);
    GeneratePatternMove(lastMove - SG_NS);
    GeneratePatternMove(lastMove - SG_NS + SG_WE);

    const SgPoint lastMove2 = m_bd.Get2ndLastMove();
    if (! SgIsSpecialMove(lastMove2))
    {
        GeneratePatternMove2(lastMove2 + SG_NS - SG_WE, lastMove);
        GeneratePatternMove2(lastMove2 + SG_NS,         lastMove);
        GeneratePatternMove2(lastMove2 + SG_NS + SG_WE, lastMove);
        GeneratePatternMove2(lastMove2 - SG_WE,         lastMove);
        GeneratePatternMove2(lastMove2 + SG_WE,         lastMove);
        GeneratePatternMove2(lastMove2 - SG_NS - SG_WE, lastMove);
        GeneratePatternMove2(lastMove2 - SG_NS,         lastMove);
        GeneratePatternMove2(lastMove2 - SG_NS + SG_WE, lastMove);
    }

    m_movesGammas.Clear();
    m_gammaSums.Clear();
    float sum = 0;

    for (int i = 0; i < m_moves.Length(); ++i)
    {
        const SgPoint p = m_moves[i];
        if (! GoUctUtil::GeneratePoint(m_bd, p, m_bd.ToPlay()))
            continue;
        m_movesGammas.PushBack(p);
        if (m_gammas[i] > m_biasPatternGammaThreshold)
            sum += m_gammas[i];
        else
            sum += 1.f;
        m_gammaSums.PushBack(sum);
    }

    if (m_movesGammas.Length() == 0)
        m_moves.Clear();
    return ! m_moves.IsEmpty();
}

template<class BOARD>
inline void GoUctGammaMoveGenerator<BOARD>::GeneratePatternMove(SgPoint p)
{
    float gamma = 0;
    if (  m_bd.IsEmpty(p)
       && m_patterns.MatchAny(p, gamma)
       && ! GoBoardUtil::SelfAtari(m_bd, p)
       )
    {
        m_moves.PushBack(p);
        m_gammas.PushBack(gamma);
    }
}

template<class BOARD>
inline void GoUctGammaMoveGenerator<BOARD>::GeneratePatternMove2(SgPoint p,
        SgPoint lastMove)
{
    float gamma = 0;
    if (  m_bd.IsEmpty(p)
       && ! SgPointUtil::In8Neighborhood(lastMove, p)
       && m_patterns.MatchAny(p, gamma)
       && ! GoBoardUtil::SelfAtari(m_bd, p)
       )
    {
        m_moves.PushBack(p);
        m_gammas.PushBack(gamma);
    }
}

template<class BOARD>
inline SgPoint GoUctGammaMoveGenerator<BOARD>::GenerateBiasedPatternMove()
{
    m_moves.Clear();
    m_gammas.Clear();
    m_gammaSums.Clear();
    m_movesGammas.Clear();
    if (GenerateAllPatternMoves())
        return SelectAccordingToGammas();
    return SG_NULLPOINT;
}

template<class BOARD>
inline SgPoint GoUctGammaMoveGenerator<BOARD>::SelectAccordingToGammas()
{
    SgArrayList<float, SG_MAX_ONBOARD + 1>& gammas = m_gammaSums;
    SG_ASSERT(gammas.Length() == m_movesGammas.Length());
    SG_ASSERT(gammas.Length() > 0);
    float randNum = m_random.Float(gammas.Last());
    for (int i = 0; i < gammas.Length(); ++i)
        if (randNum <= gammas[i])
            return m_movesGammas[i];

    SG_ASSERT(false);
    return SG_NULLPOINT;
}

//----------------------------------------------------------------------------
template<class BOARD>
std::ostream& operator<<(std::ostream& stream, 
                         const GoUctGammaMoveGenerator<BOARD>& gen)
{
    for (int i = 0; i < gen.NuMoves(); ++i)
        stream << ' ' << SgWritePoint(gen.GetGammaMoveAt(i));
    return stream;
}

template<class BOARD>
void GoUctGammaMoveGenerator<BOARD>::
	 WriteMovesAndGammas(std::ostream& stream) const
{
    for (int i = 0; i < NuMoves(); ++i)
    {
        stream << SgWritePoint(GetGammaMoveAt(i)) << ":"
               << GetGammaValueForMoveAt(i) << "  ";
    }
    stream << std::endl;
}
//----------------------------------------------------------------------------

#endif // GOUCTGAMMAMOVEGENERATOR_H
