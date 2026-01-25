#include "pch.h"

#include "CHolyStone.h"

void CHolyStone::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_bOper = false;
  m_nHP = 0;
  m_nMaxHP = 0;
  m_pRec = nullptr;
  m_pDum = nullptr;
  m_byMasterRaceCode = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 5; ++j)
  {
    m_nDefPart[j] = 0;
  }
  m_nOldRate = 0;
  m_dwLastRecoverTime = 0;
  m_nCurrLootIndex = -1;
  m_nEndLootIndex = -1;
  m_nCurrDropIndex = -1;
  m_wMagnifications = 0;
  m_wRange = 0;
  m_wDropCntOnce = 0;
  m_wAddCountWithPlayer = 0;
}
