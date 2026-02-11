#include "pch.h"

#include "CReturnGate.h"
#include "CMapData.h"
#include "CPartyPlayer.h"
#include "CPlayer.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstring>

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

bool CReturnGate::IsOpen()
{
  return m_eState == CUnmannedTraderSchedule::STATE::REG_WAIT;
}

bool CReturnGate::IsValidOwner()
{
  return m_pkOwner && m_dwOwnerSerial == m_pkOwner->m_dwObjSerial;
}

bool CReturnGate::IsValidPosition(float *pfCurPos)
{
  return GetSqrt(m_fCurPos, pfCurPos) <= 100.0;
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

void CReturnGate::SendMsg_Destroy()
{
  unsigned int objSerial = m_dwObjSerial;
  unsigned __int8 pbyType[2];
  std::memset(pbyType, 8, sizeof(pbyType));
  CircleReport(pbyType, reinterpret_cast<char *>(&objSerial), sizeof(objSerial), false);
}

void CReturnGate::SendMsg_MovePortal(CPlayer *pkObj)
{
  unsigned __int8 msg[15]{};
  msg[0] = 0;
  msg[1] = static_cast<unsigned __int8>(m_pDestMap->m_pMapSet->m_dwIndex);
  memcpy_0(msg + 2, m_fBindPos, 12);
  msg[14] = 2;
  unsigned __int8 pbyType[2] = {8, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(pkObj->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(msg), 15u);
}
