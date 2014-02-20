//----------------------------------------------------------------------------
/** @file GoUctGlobalSearch.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctGlobalSearch.h"

//----------------------------------------------------------------------------

GoUctGlobalSearchStateParam::GoUctGlobalSearchStateParam()
    : m_mercyRule(true),
      m_territoryStatistics(false),
      m_lengthModification(0),
      m_scoreModification(0.02f),
      m_useTreeFilter(true),
      m_useDefaultPriorKnowledge(true),
      m_useFeaturePriorKnowledge(false)
{ }

GoUctGlobalSearchStateParam::~GoUctGlobalSearchStateParam()
{ }

//----------------------------------------------------------------------------
