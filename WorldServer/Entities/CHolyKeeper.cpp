#include "pch.h"

#include "CHolyKeeper.h"

void CHolyKeeper::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_nMasterRace = -1;
  m_pPosCreate = nullptr;
  m_pPosActive = nullptr;
  m_pPosCenter = nullptr;
  m_nHP = 0;
  m_pRec = nullptr;
  m_bExit = false;
  m_bChaos = false;
  m_dwLastStopTime = static_cast<unsigned int>(-1);
  for (int j = 0; j < 5; ++j)
  {
    m_nDefPart[j] = 0;
  }
  m_pLastMoveTarget = nullptr;
  m_at = nullptr;
  m_bDamageAbleState = false;
  m_nCurrLootIndex = -1;
  m_nEndLootIndex = -1;
  m_nCurrDropIndex = 0;
  m_wMagnifications = 0;
  m_wRange = 0;
  m_wDropCntOnce = 0;
}
