//----------------------------------------------------------------------------
/** @file SgUctBiasTermPrecomp.h */
//----------------------------------------------------------------------------

#ifndef SGUCTBIASTERMPRECOMP_H
#define SGUCTBIASTERMPRECOMP_H

#include <cmath>
#include <cstddef>
#include <vector>

//----------------------------------------------------------------------------

/** Precomputes part of the bias term formula for small counts. */
class SgUctBiasTermPrecomp
{
public:
    SgUctBiasTermPrecomp(std::size_t maxPosCount, std::size_t maxMoveCount);

    /** Get sqrt(log(posCount) / moveCount).
        Uses the lookup table for small counts, computes the result otherwise.
        @param posCount The visit count of the position
        @param logPosCount The logarithm of posCount for speeding up the
        computation, if table lookup fails. This logarithm can usually be
        computed only once for all moves in a position.
        @param moveCount The count of the move
    */
    float Get(std::size_t posCount, float logPosCount,
              std::size_t moveCount) const;

private:
    std::size_t m_maxPosCount;

    std::size_t m_maxMoveCount;

    std::vector<float> m_values;

    float Compute(float logPosCount, std::size_t moveCount) const;

    float& Value(std::size_t posCount, std::size_t moveCount);

    float Value(std::size_t posCount, std::size_t moveCount) const;
};

inline float SgUctBiasTermPrecomp::Compute(float logPosCount,
                                           std::size_t moveCount) const
{
    return (std::sqrt(logPosCount / moveCount));
}

inline float SgUctBiasTermPrecomp::Get(std::size_t posCount,
                                       float logPosCount,
                                       std::size_t moveCount) const
{
    if (posCount < m_maxPosCount && moveCount < m_maxMoveCount)
        return Value(posCount, moveCount);
    else
        return Compute(logPosCount, moveCount);
}

inline float& SgUctBiasTermPrecomp::Value(std::size_t posCount,
                                          std::size_t moveCount)
{
    SG_ASSERT(posCount < m_maxPosCount);
    SG_ASSERT(moveCount < m_maxMoveCount);
    return m_values[posCount * m_maxMoveCount + moveCount];
}

inline float SgUctBiasTermPrecomp::Value(std::size_t posCount,
                                         std::size_t moveCount) const
{
    SG_ASSERT(posCount < m_maxPosCount);
    SG_ASSERT(moveCount < m_maxMoveCount);
    return m_values[posCount * m_maxMoveCount + moveCount];
}

//----------------------------------------------------------------------------

#endif // SGUCTBIASTERMPRECOMP_H
