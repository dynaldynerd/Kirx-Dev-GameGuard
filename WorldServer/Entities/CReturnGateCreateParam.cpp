#include "pch.h"

#include "CReturnGateCreateParam.h"

#include "CMapData.h"
#include "CPlayer.h"

CReturnGateCreateParam::CReturnGateCreateParam(CPlayer *pkOwner)
{
  m_pkOwner = nullptr;
  if (pkOwner && pkOwner->m_pCurMap)
  {
    m_pkOwner = pkOwner;
    m_nLayerIndex = m_pkOwner->m_wMapLayerIndex;
    m_pMap = m_pkOwner->m_pCurMap;
    m_pMap->GetRandPosInRange(m_pkOwner->m_fCurPos, 10, m_fStartPos);
    m_pRecordSet = nullptr;
  }
}

CReturnGateCreateParam::~CReturnGateCreateParam()
{
// this is not a stub
}
