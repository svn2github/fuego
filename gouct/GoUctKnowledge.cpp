//----------------------------------------------------------------------------
/** @file GoUctKnowledge.cpp
    See GoUctKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctKnowledge.h"
#include "SgDebug.h"
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

GoUctKnowledge::GoUctKnowledge(const GoBoard& bd)
    : m_bd(bd)
{ }

GoUctKnowledge::~GoUctKnowledge()
{ }

void GoUctKnowledge::Add(SgPoint p, SgUctValue value, SgUctValue count)
{
    m_values[p].Add(value, count);
}

void
GoUctKnowledge::Get(SgPoint p, SgUctValue& value, SgUctValue& count) const
{
    value = m_values[p].Mean();
    count = m_values[p].Count();
}

void GoUctKnowledge::Initialize(SgPoint p, SgUctValue value, SgUctValue count)
{
    m_values[p].Initialize(value, count);
}

void GoUctKnowledge::ClearValues()
{
    for (int i = 0; i < SG_PASS + 1; ++i)
        m_values[i].Clear();
}

void GoUctKnowledge::TransferValues(std::vector<SgUctMoveInfo>& moves) const
{
    static const bool WARN_OVERWRITE = false;
    for (std::size_t i = 0; i < moves.size(); ++i)
    {
        const SgMove p = moves[i].m_move;
        if (m_values[p].IsDefined())
        {
            if (WARN_OVERWRITE && moves[i].m_count != 0)
                SgDebug() << "WARNING - overwriting moves[i].m_count "
                          << moves[i].m_count << " by "
                          << m_values[p].Count() << '\n';
            moves[i].m_count = m_values[p].Count();
            moves[i].m_value =
            SgUctSearch::InverseEstimate(m_values[p].Mean());
            moves[i].m_raveCount = m_values[p].Count();
            moves[i].m_raveValue = m_values[p].Mean();
        }
    }
}

inline void AddToMoveValue(
                const SgStatisticsBase<SgUctValue, SgUctValue>& value,
                struct SgUctMoveInfo& moveInfo)
{
    moveInfo.m_count += value.Count();
    SgUctValue v1 = SgUctSearch::InverseEstimate(moveInfo.m_value);
    SgUctValue v2 = value.Mean();
    v2 -= v1; // same formula for weighted mean as in SgStatisticsBase::Add
    v1 += (v2 * value.Count()) / moveInfo.m_count;
    moveInfo.m_value = SgUctSearch::InverseEstimate(v1);
    moveInfo.m_raveCount = moveInfo.m_count;
    moveInfo.m_raveValue = v1;
}

void GoUctKnowledge::AddValuesTo(std::vector<SgUctMoveInfo>& moves) const
{
    for (std::size_t i = 0; i < moves.size(); ++i)
    {
        const SgMove p = moves[i].m_move;
        if (m_values[p].IsDefined())
        {
            AddToMoveValue(m_values[p], moves[i]);
        }
    }
}

//----------------------------------------------------------------------------
