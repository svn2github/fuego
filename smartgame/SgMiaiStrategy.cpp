//----------------------------------------------------------------------------
/** @file SgMiaiStrategy.cpp
    See SgMiaiStrategy.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgMiaiStrategy.h"

#include "SgWrite.h"

//----------------------------------------------------------------------------

void SgMiaiStrategy::Write(std::ostream& stream) const
{
    SgStrategy::Write(stream);
    stream << "Miai Strategies: ";
    for (SgListIterator<SgMiaiPair> it(m_miaiStrategies); it; ++it)
        stream << '(' << SgWritePoint((*it).first)
               << "-" << SgWritePoint((*it).second)
               << ") ";
    stream << "\n" << SgWritePointList(m_openThreats, "open threats", false)
    << "Broken: " << m_failed
    << '\n';
}

void SgMiaiStrategy::AddPair(const SgMiaiPair& m)
{
#if _DEBUG // check that points are new.
    SgPointSet dependency = Dependency();
    SG_ASSERT(! dependency.Contains(m.first));
    SG_ASSERT(! dependency.Contains(m.second));
    SG_ASSERT(m.first != m.second);
    // SG_PASS, NullMove are not valid miai Points.
    SG_ASSERT(SgPointUtil::InBoardRange(m.first));
    SG_ASSERT(SgPointUtil::InBoardRange(m.second));
#endif
    m_miaiStrategies.Append(m);
}


void SgMiaiStrategy::Clear()
{
    SgStrategy::Clear();
    m_miaiStrategies.Clear();
    m_openThreats.Clear();
    m_failed = false;
    SG_ASSERT(Dependency().IsEmpty());
}

SgPointSet SgMiaiStrategy::Dependency() const
{
    SgPointSet dependency;
    for (SgListIterator<SgMiaiPair> it(m_miaiStrategies); it; ++it)
    {
        dependency.Include((*it).first);
        dependency.Include((*it).second);
    }
    return dependency;
}

SgStrategyStatus SgMiaiStrategy::Status() const
{
    if (m_failed)
        return strFailed;
    else if (m_openThreats.IsEmpty())
        return strAchieved;
    else if (m_openThreats.IsLength1())
        return strThreatened;
    else
        return strFailed;
}

void SgMiaiStrategy::StrategyFailed()
{
    m_failed = true;
    m_openThreats.Clear();
    m_miaiStrategies.Clear();
}

void SgMiaiStrategy::ExecuteMove(const SgPoint p, SgBlackWhite player)
{
    if (m_failed)
        /* */ return; /* */

    SgList<SgPoint> fixedThreats;
    for (SgListIterator<SgPoint> it(m_openThreats); it; ++it)
        if (p == *it)
        {
            if (player == Player())
                fixedThreats.Append(*it);
            else
            {
                StrategyFailed();
                break;
            }
        }

    if (m_failed)
    {
        /* */ return; /* */
    }
    
    SgList<SgMiaiPair> toChange;
    for (SgListIterator<SgMiaiPair> it(m_miaiStrategies); it; ++it)
        if (p == (*it).first || p == (*it).second)
            toChange.Append(*it);

    m_miaiStrategies.Exclude(toChange);
    if (player == Player())
    {
        m_openThreats.Exclude(fixedThreats);
        // All toChange strategies have been achieved - remove.
    }
    else
        // move other endpoint of toChange to open threats
    for (SgListIterator<SgMiaiPair> it(toChange); it; ++it)
    {
        m_openThreats.Append(Other(*it, p));
    }
}
    
SgPoint Other(const SgMiaiPair& pair, SgPoint p)
{
    SG_ASSERT(pair.first == p || pair.second == p);
    return pair.first == p ? pair.second : pair.first;
}

bool SgMiaiStrategy::HasOverlappingMiaiPairs() const
{
    SgPointSet used;
    for (SgListIterator<SgMiaiPair> it(m_miaiStrategies); it; ++it)
    {
        const SgPoint p1 = (*it).first;
        const SgPoint p2 = (*it).second;
        if (used[p1] || used[p2])
            /* */ return true; /* */
            
        used.Include(p1);
        used.Include(p2);
    }
    return false;
}

const SgList<SgPoint>& SgMiaiStrategy::OpenThreats() const
{
    return m_openThreats;
}

SgPoint SgMiaiStrategy::OpenThreatMove() const
{
    SG_ASSERT(m_openThreats.MaxLength(1));
    return m_openThreats.IsEmpty() ? SG_NULLPOINT : 
                                     m_openThreats.Tail();
}

void SgMiaiStrategy::UndoMove()
{
    SG_ASSERT(false);
}
