#include "pch.h"

#include "CTrap.h"

#include <cstring>

void CTrap::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_dwMasterSerial = static_cast<unsigned int>(-1);
  m_nHP = 0;
  m_pMaster = nullptr;
  m_byRaceCode = static_cast<unsigned __int8>(-1);
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
  std::memset(m_aszMasterName, 0, sizeof(m_aszMasterName));
  m_dMasterPvPPoint = 0.0;
  m_dwStartMakeTime = 0;
  m_bComplete = false;
  m_bBreakTransparBuffer = false;
  m_nTrapMaxAttackPnt = 0;
}

void CTrap::MasterNetClose(long double dPvPPoint)
{
  m_pMaster = nullptr;
  m_dMasterPvPPoint = dPvPPoint;
}

void CTrap::MasterReStart(CPlayer *pMaster)
{
  m_pMaster = pMaster;
  m_dMasterPvPPoint = pMaster->m_Param.GetPvPPoint();
}

