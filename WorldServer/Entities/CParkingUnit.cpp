#include "pch.h"

#include "CParkingUnit.h"
#include "ObjectCreateSetData.h"
#include "WorldServerUtil.h"

#include <mmsystem.h>

int CParkingUnit::s_nLiveNum = 0;
unsigned int CParkingUnit::s_dwSerialCounter = 0;

void CParkingUnit::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_dwLastDestroyTime = 0;
  m_byFrame = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 6; ++j)
  {
    m_byPartCode[j] = static_cast<unsigned __int8>(-1);
  }
  m_byCreateType = static_cast<unsigned __int8>(-1);
  m_byTransDistCode = static_cast<unsigned __int8>(-1);
  m_wHPRate = 0;
}

bool CParkingUnit::Create(_parkingunit_create_setdata *pParam)
{
  if (!CGameObject::Create(pParam))
  {
    return false;
  }

  m_dwObjSerial = s_dwSerialCounter++;
  m_pOwner = pParam->pOwner;
  m_dwOwnerSerial = pParam->pOwner->m_dwObjSerial;
  m_byFrame = pParam->byFrame;
  memcpy_0(m_byPartCode, pParam->byPartCode, sizeof(m_byPartCode));
  m_byCreateType = pParam->byCreateType;
  m_byTransDistCode = pParam->byTransDistCode;
  m_dwParkingStartTime = timeGetTime();
  m_wHPRate = pParam->wHPRate;
  ++s_nLiveNum;
  SendMsg_Create();
  return true;
}

bool CParkingUnit::Destroy(unsigned __int8 byDestoryType)
{
  m_pOwner = nullptr;
  m_dwOwnerSerial = static_cast<unsigned int>(-1);
  m_dwParkingStartTime = 0;
  m_dwLastDestroyTime = timeGetTime();
  SendMsg_Destroy(byDestoryType);
  --s_nLiveNum;
  return CGameObject::Destroy();
}

void CParkingUnit::SendMsg_Create()
{
#pragma pack(push, 1)
  struct ParkingUnitCreateMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    unsigned __int8 byCreateType;
    unsigned __int8 byFrame;
    unsigned __int8 byPartCode[6];
    unsigned int dwOwnerSerial;
    __int16 zPos[3];
    unsigned __int8 byTransDistCode;
  };
#pragma pack(pop)

  ParkingUnitCreateMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byCreateType = m_byCreateType;
  msg.byFrame = m_byFrame;
  memcpy_0(msg.byPartCode, m_byPartCode, sizeof(msg.byPartCode));
  msg.dwOwnerSerial = m_dwOwnerSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);
  msg.byTransDistCode = m_byTransDistCode;

  unsigned __int8 type[2]{3, 21};
  CircleReport(type, reinterpret_cast<char *>(&msg), 25, false);
}

void CParkingUnit::SendMsg_Destroy(unsigned __int8 byDestoryType)
{
#pragma pack(push, 1)
  struct DestroyMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    unsigned __int8 byDestroyType;
  };
#pragma pack(pop)

  DestroyMsg msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.byDestroyType = byDestoryType;

  unsigned __int8 type[2] = {3, 29};
  CircleReport(type, reinterpret_cast<char *>(&msg), 7, false);
}

CParkingUnit *FindEmptyParkingUnit(CParkingUnit *pItem, int nMax)
{
  const DWORD now = timeGetTime();
  for (int j = 0; j < nMax; ++j)
  {
    if (!pItem[j].m_bLive)
    {
      const unsigned int elapsed = now - pItem[j].m_dwLastDestroyTime;
      if (elapsed > 0x7530)
      {
        return &pItem[j];
      }
    }
  }
  return nullptr;
}

