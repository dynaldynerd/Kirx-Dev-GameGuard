#include "pch.h"

#include "CReturnGate.h"
#include "CMapData.h"
#include "CNetProcess.h"
#include "CPartyPlayer.h"
#include "CPlayer.h"
#include "CReturnGateCreateParam.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "CMainThread.h"

#include <cstring>

unsigned int CReturnGate::ms_dwSerialCnt = 1;

CReturnGate::CReturnGate(_object_id *pID)
{
  m_eState = CUnmannedTraderSchedule::STATE::EMPTY;
  m_pkOwner = nullptr;
  m_dwOwnerSerial = 0;
  m_pDestMap = nullptr;
  m_fBindPos[0] = 0.0f;
  m_fBindPos[1] = 0.0f;
  m_fBindPos[2] = 0.0f;
  CGameObject::Init(pID);
}

CReturnGate::~CReturnGate() = default;

void CReturnGate::Clear()
{
  m_pkOwner = nullptr;
  m_dwOwnerSerial = 0;
  m_dwCloseTime = 0;
}

unsigned __int16 CReturnGate::GetIndex()
{
  return m_ObjID.m_wIndex;
}

CPlayer *CReturnGate::GetOwner()
{
  return m_pkOwner;
}

bool CReturnGate::IsOpen()
{
  return m_eState == CUnmannedTraderSchedule::STATE::REG_WAIT;
}

bool CReturnGate::IsClose()
{
  if (!m_dwCloseTime)
  {
    return false;
  }
  return !IsValidOwner() || GetLoopTime() > m_dwCloseTime;
}

bool CReturnGate::IsValidOwner()
{
  return m_pkOwner && m_dwOwnerSerial == m_pkOwner->m_dwObjSerial;
}

bool CReturnGate::IsValidPosition(float *pfCurPos)
{
  return GetSqrt(m_fCurPos, pfCurPos) <= 100.0;
}

void CReturnGate::GetInfo(_open_return_gate_inform_zocl *Info)
{
  Info->wGateInx = m_ObjID.m_wIndex;
  Info->dwOpenerSerial = m_dwOwnerSerial;
  const char *charName = m_pkOwner->m_Param.GetCharNameW();
  std::strcpy(Info->wszOpenerName, charName);
  FloatToShort(m_fCurPos, Info->zPos, 3);
}

bool CReturnGate::Open(CReturnGateCreateParam *pParam)
{
  if (!pParam || !pParam->m_pkOwner)
  {
    g_Main.m_logReturnGate.Write("CReturnGate::Open 0 == pParam->GetOwner()");
    return false;
  }

  m_pkOwner = pParam->m_pkOwner;
  m_dwOwnerSerial = m_pkOwner->m_dwObjSerial;
  m_pDestMap = m_pkOwner->GetBindMap(m_fBindPos, true);
  if (!m_pDestMap)
  {
    g_Main.m_logReturnGate.Write("CReturnGate::Open 0 == m_pkOwner->GetBindMap");
    return false;
  }

  if (!Create(pParam))
  {
    g_Main.m_logReturnGate.Write("CReturnGate::Open CGameObject::Create() Failed!");
    return false;
  }

  m_dwObjSerial = ms_dwSerialCnt++;
  m_dwCloseTime = GetLoopTime() + 50000;
  m_eState = CUnmannedTraderSchedule::STATE::REG_WAIT;
  SendMsg_Create();
  return true;
}

int CReturnGate::Enter(CPlayer *pkObj)
{
  if (!IsValidPosition(pkObj->m_fCurPos))
  {
    return 3;
  }

  if (m_pkOwner != pkObj && !m_pkOwner->m_pPartyMgr->IsPartyMember(pkObj))
  {
    return 4;
  }

  if (!m_pDestMap)
  {
    return 5;
  }

  pkObj->OutOfMap(m_pDestMap, 0, 3u, m_fBindPos);
  SendMsg_MovePortal(pkObj);
  return 0;
}

void CReturnGate::Close()
{
  Clear();
  m_eState = CUnmannedTraderSchedule::STATE::WAIT_CANCEL;
  SendMsg_Destroy();
  m_dwObjSerial = 0;
  Destroy();
}

void CReturnGate::SendMsg_Create()
{
  _open_return_gate_inform_zocl payload{};
  payload.wGateInx = m_ObjID.m_wIndex;
  payload.dwObjSerial = m_dwObjSerial;
  payload.dwOpenerSerial = m_dwOwnerSerial;
  std::strcpy(payload.wszOpenerName, m_pkOwner->m_Param.GetCharNameW());
  FloatToShort(m_fCurPos, payload.zPos, 3);

  unsigned __int8 type[2] = {8, 7};
  CircleReport(type, reinterpret_cast<char *>(&payload), static_cast<unsigned __int16>(sizeof(payload)), false);
}

void CReturnGate::SendMsg_FixPosition(int n)
{
  _return_gate_fix_position_zocl msg{};
  msg.wGateInx = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  msg.dwOpenerSerial = m_dwOwnerSerial;
  std::strcpy(msg.wszOpenerName, m_pkOwner->m_Param.GetCharNameW());
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 packetType[2] = {4, static_cast<unsigned __int8>(-86)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    packetType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CReturnGate::SendMsg_Destroy()
{
  _return_gate_destroy_inform_zocl msg{};
  msg.dwObjSerial = m_dwObjSerial;
  unsigned __int8 pbyType[2];
  std::memset(pbyType, 8, sizeof(pbyType));
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}

void CReturnGate::SendMsg_MovePortal(CPlayer *pkObj)
{
  _move_potal_result_zocl msg{};
  msg.byRet = 0;
  msg.byMapIndex = static_cast<char>(m_pDestMap->m_pMapSet->m_dwIndex);
  std::memcpy(msg.fStartPos, m_fBindPos, sizeof(msg.fStartPos));
  msg.byZoneCode = 2;
  unsigned __int8 pbyType[2] = {8, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkObj->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}
