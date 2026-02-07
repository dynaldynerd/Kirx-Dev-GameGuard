#include "pch.h"

#include "CGuardTower.h"

#include "WorldServerUtil.h"

int CGuardTower::s_nLiveNum = 0;
unsigned int CGuardTower::s_dwSerialCnt = 0;
__TEMP_WAIT_TOWER CGuardTower::s_Temp[MAX_PLAYER]{};

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

bool CGuardTower::Destroy(unsigned __int8 byDesType, bool bSystemBack)
{
  m_dwLastDestroyTime = timeGetTime();
  if (m_pMasterTwr && byDesType == 1)
  {
    m_pMasterTwr->_TowerDestroy(this);
  }
  SendMsg_Destroy(byDesType);

  if (!m_bSystemStruct || bSystemBack)
  {
    m_pMasterTwr = nullptr;
    m_dwObjSerial = static_cast<unsigned int>(-1);
    --CGuardTower::s_nLiveNum;
    return CCharacter::Destroy(this);
  }

  m_nHP = *reinterpret_cast<int *>(&m_pRecordSet[6].m_strCode[40]);
  m_dwStartMakeTime = timeGetTime();
  m_bComplete = false;
  m_pTarget = nullptr;
  m_pMasterSetTarget = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(GetNewSerial());
  SendMsg_Create();
  return true;
}

void CGuardTower::SendMsg_Destroy(unsigned __int8 byDesType)
{
#pragma pack(push, 1)
  struct GuardTowerDestroyMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    unsigned __int8 byDesType;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

  GuardTowerDestroyMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  msg.byDesType = byDesType;
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {3, 27};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CGuardTower::SendMsg_Create()
{
#pragma pack(push, 1)
  struct GuardTowerCreateMsg
  {
    unsigned __int16 wIndex;
    unsigned __int16 wRecordIndex;
    unsigned int dwObjSerial;
    short pos[3];
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

  GuardTowerCreateMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.pos, 3);
  msg.dwMasterSerial = m_dwMasterSerial;

  unsigned __int8 type[2] = {3, 19};
  CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

__int64 CGuardTower::GetNewSerial()
{
  return s_dwSerialCnt++;
}

