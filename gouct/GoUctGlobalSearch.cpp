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
      m_defaultPriorWeight(0.15f),
      m_additiveKnowledgeScale(0.03f)
{ }

GoUctGlobalSearchStateParam::~GoUctGlobalSearchStateParam()
{ }

//----------------------------------------------------------------------------
GoUctGlobalSearchAllParam::GoUctGlobalSearchAllParam(
      const GoUctGlobalSearchStateParam& searchStateParam,
      const GoUctPlayoutPolicyParam& policyParam,
      const GoUctDefaultMoveFilterParam& moveFilterParam,
      const GoUctFeatureKnowledgeParam& featureParam,
      const SgBWSet& safe,
      const SgPointArray<bool>& allSafe)
    :
    m_searchStateParam(searchStateParam),
    m_policyParam(policyParam),
    m_moveFilterParam(moveFilterParam),
    m_featureParam(featureParam),
    m_safe(safe),
    m_allSafe(allSafe)
{ }
