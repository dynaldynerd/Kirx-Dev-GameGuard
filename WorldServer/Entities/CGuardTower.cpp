#include "pch.h"

#include "CGuardTower.h"

void CGuardTower::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_bSystemStruct = false;
  m_nHP = 0;
  m_pMasterTwr = nullptr;
  m_dwMasterSerial = 0;
  m_byRaceCode = static_cast<unsigned __int8>(-1);
  m_pItem = nullptr;
  m_wItemSerial = static_cast<unsigned __int16>(-1);
  m_bSystemStruct = false;
  m_nIniIndex = -1;
  m_dwStartMakeTime = 0;
  m_bComplete = false;
  m_bQuick = false;
  m_pTarget = nullptr;
  m_pMasterSetTarget = nullptr;
}

