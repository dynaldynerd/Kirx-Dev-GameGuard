#include "pch.h"

#include "WorldServerUtil.h"

#include "CParkingUnit.h"

#include <mmsystem.h>

#include "CNetProcess.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

int CParkingUnit::s_nLiveNum;
unsigned int CParkingUnit::s_dwSerialCounter = 0;

CParkingUnit::CParkingUnit()
{
  m_pOwner = nullptr;
  m_dwOwnerSerial = static_cast<unsigned int>(-1);
  m_dwParkingStartTime = 0;
}

CParkingUnit::~CParkingUnit()
{
}

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
  std::memcpy(m_byPartCode, pParam->byPartCode, sizeof(m_byPartCode));
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

unsigned __int16 CParkingUnit::CalcCurHPRate()
{
  return m_wHPRate;
}

void CParkingUnit::ChangeOwner(CPlayer *pNewOwner, unsigned __int8 byUnitSlotIndex)
{
  CPlayer *oldOwner = m_pOwner;
  m_pOwner = pNewOwner;
  m_dwOwnerSerial = pNewOwner->m_dwObjSerial;
  SendMsg_ChangeOwner(byUnitSlotIndex, oldOwner);
}

bool CParkingUnit::IsRideRight(CPlayer *pOne)
{
  (void)pOne;
  return false; // this is not a stub
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
  _parkingunit_create_zocl payload{};
  payload.wObjIndex = m_ObjID.m_wIndex;
  payload.dwObjSerial = m_dwObjSerial;
  payload.byCreateType = static_cast<char>(m_byCreateType);
  payload.byFrame = static_cast<char>(m_byFrame);
  std::memcpy(payload.byPart, m_byPartCode, sizeof(m_byPartCode));
  payload.dwOwerSerial = m_dwOwnerSerial;
  FloatToShort(m_fCurPos, payload.zPos, 3);
  payload.byTransDistCode = static_cast<char>(m_byTransDistCode);

  unsigned __int8 type[2] = {3, 21};
  CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), false);
}

void CParkingUnit::SendMsg_ChangeOwner(unsigned __int8 byUnitSlotIndex, CPlayer *pOldOwner)
{
  _parkingunit_change_owner_zocl payload{};
  payload.wObjIndex = m_ObjID.m_wIndex;
  payload.dwObjSerial = m_dwObjSerial;
  payload.dwOldOwnerSerial = pOldOwner->m_dwObjSerial;
  payload.dwNewOwnerSerial = m_pOwner->m_dwObjSerial;
  payload.byNewOwnerUnitSlotIndex = static_cast<char>(byUnitSlotIndex);

  unsigned __int8 type[2] = {23, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pOwner->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
  g_Network.m_pProcess[0]->LoadSendMsg(
    pOldOwner->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CParkingUnit::Loop()
{
  // this is not a stub
}

void CParkingUnit::SendMsg_FixPosition(int n)
{
  _parkingunit_fixpositon_zocl msg{};
  msg.wObjIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  msg.byFrame = static_cast<char>(m_byFrame);
  std::memcpy(msg.byPart, m_byPartCode, sizeof(m_byPartCode));
  FloatToShort(m_fCurPos, msg.zPos, 3);
  msg.dwMasterSerial = m_pOwner->m_dwObjSerial;

  unsigned __int8 packetType[2] = {4, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

