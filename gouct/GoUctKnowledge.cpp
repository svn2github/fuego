//----------------------------------------------------------------------------
/** @file GoUctKnowledge.cpp
    See GoUctKnowledge.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctKnowledge.h"
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
    for (std::size_t i = 0; i < moves.size(); ++i) 
    {
        SgMove p = moves[i].m_move;
        if (m_values[p].IsDefined())
        {
            moves[i].m_count = m_values[p].Count();
            moves[i].m_value =
                 SgUctSearch::InverseEstimate(m_values[p].Mean());
            moves[i].m_raveCount = m_values[p].Count();
            moves[i].m_raveValue = m_values[p].Mean();
        }
    }
}

//----------------------------------------------------------------------------
