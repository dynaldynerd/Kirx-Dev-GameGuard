#include "pch.h"

#include "WorldServerUtil.h"

#include "CParkingUnit.h"

#include <mmsystem.h>

#include "CNetProcess.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

int CParkingUnit::s_nLiveNum;
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

char CParkingUnit::Create(_parkingunit_create_setdata *pParam)
{
  if (!CGameObject::Create(pParam))
  {
    return 0;
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
  return 1;
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

void CParkingUnit::SendMsg_Destroy(unsigned __int8 byDestoryType)
{

  _parkingunit_destroy_zocl msg{};
  msg.wObjIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  msg.byDestroyCode = byDestoryType;

  unsigned __int8 type[2] = {3, 29};
  CircleReport(type, reinterpret_cast<char *>(&msg), 7, false);
}

void CParkingUnit::SendMsg_Create()
{
  char payload[25]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = m_dwObjSerial;
  payload[6] = static_cast<char>(m_byCreateType);
  payload[7] = static_cast<char>(m_byFrame);
  memcpy_0(payload + 8, m_byPartCode, sizeof(m_byPartCode));
  *reinterpret_cast<unsigned int *>(payload + 14) = m_dwOwnerSerial;
  FloatToShort(m_fCurPos, reinterpret_cast<short *>(payload + 18), 3);
  payload[24] = static_cast<char>(m_byTransDistCode);

  unsigned __int8 type[2] = {3, 21};
  CircleReport(type, payload, 25, false);
}

void CParkingUnit::SendMsg_ChangeOwner(unsigned __int8 byUnitSlotIndex, CPlayer *pOldOwner)
{
  char payload[15]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = m_dwObjSerial;
  *reinterpret_cast<unsigned int *>(payload + 6) = pOldOwner->m_dwObjSerial;
  *reinterpret_cast<unsigned int *>(payload + 10) = m_pOwner->m_dwObjSerial;
  payload[14] = static_cast<char>(byUnitSlotIndex);

  unsigned __int8 type[2] = {23, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(m_pOwner->m_ObjID.m_wIndex, type, payload, 0xFu);
  g_Network.m_pProcess[0]->LoadSendMsg(pOldOwner->m_ObjID.m_wIndex, type, payload, 0xFu);
}

void CParkingUnit::Loop()
{
  // this is not a stub
}

void CParkingUnit::SendMsg_FixPosition(int n)
{
  char msg[23]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 2) = m_dwObjSerial;
  msg[6] = static_cast<char>(m_byFrame);
  memcpy_0(msg + 7, m_byPartCode, sizeof(m_byPartCode));
  FloatToShort(m_fCurPos, reinterpret_cast<short *>(msg + 13), 3);
  *reinterpret_cast<unsigned int *>(msg + 19) = m_pOwner->m_dwObjSerial;

  unsigned __int8 packetType[2] = {4, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    msg,
    static_cast<unsigned __int16>(sizeof(msg)));
}

