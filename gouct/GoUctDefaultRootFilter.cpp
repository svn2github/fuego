//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.cpp
    See GoUctDefaultRootFilter.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultRootFilter.h"

#include "GoBoard.h"
#include "GoBoardUtil.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctDefaultRootFilter::GoUctDefaultRootFilter(const GoBoard& bd)
    : m_bd(bd)
{
}

bool GoUctDefaultRootFilter::FilterMove(SgPoint p) const
{
    return     GoBoardUtil::SelfAtari(m_bd, p)
            //&& ! PossiblyGoodThrowInMove(m_bd, p)
            && ! GoBoardUtil::IsCapturingMove(m_bd, p)
            ;
}

vector<SgPoint> GoUctDefaultRootFilter::Get()
{
    vector<SgPoint> rootFilter;
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsLegal(*it) && FilterMove(*it))
            rootFilter.push_back(*it);
    return rootFilter;
}

//----------------------------------------------------------------------------
