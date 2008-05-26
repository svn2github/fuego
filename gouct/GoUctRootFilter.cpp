//----------------------------------------------------------------------------
/** @file GoUctRootFilter.cpp
    See GoUctRootFilter.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctRootFilter.h"

#include "GoBoard.h"
#include "GoBoardUtil.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctRootFilter::GoUctRootFilter(const GoBoard& bd)
    : m_bd(bd)
{
}

bool GoUctRootFilter::FilterMove(SgMove p) const
{
    return     GoBoardUtil::SelfAtari(m_bd, p)
            //&& ! PossiblyGoodThrowInMove(m_bd, p)
            && ! GoBoardUtil::IsCapturingMove(m_bd, p)
            ;
}

vector<SgPoint> GoUctRootFilter::Get()
{
    vector<SgPoint> rootFilter;

    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsLegal(*it) && FilterMove(*it))
            rootFilter.push_back(*it);

    return rootFilter;
}

//----------------------------------------------------------------------------
