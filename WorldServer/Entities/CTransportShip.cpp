#include "pch.h"

#include "CTransportShip.h"

#include <cstring>
#include <mmsystem.h>

#include "CGameStatistics.h"
#include "CMapOperation.h"
#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CUserDB.h"
#include "CMgrAvatorItemHistory.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CTransportShip::__mgr_member::__mgr_member()
{
  pPtr = nullptr;
}

CTransportShip::__mgr_ticket::__mgr_ticket()
{
  init();
}

CTransportShip::CTransportShip()
  : m_tmrCheckState(), m_listLogoffMember()
{
  m_pLinkShipMap = nullptr;
  for (int index = 0; index < 2; ++index)
  {
    m_pLinkPortMap[index] = nullptr;
  }
  m_byRaceCode_Layer = static_cast<unsigned __int8>(-1);

  m_tmrCheckState.BeginTimer(1000);
  m_listLogoffMember.SetList(0x13C8u);
}

CTransportShip::~CTransportShip() = default;

void CTransportShip::__mgr_member::init()
{
  pPtr = nullptr;
}

bool CTransportShip::__mgr_member::is_fill() const
{
  return pPtr != nullptr;
}

void CTransportShip::__mgr_ticket::init()
{
  nCurTicketNum = 100;
  nReserTicketNum = 0;
  dwNextUpdateTime = static_cast<unsigned int>(-1);
}

bool CTransportShip::InitShip(CMapData *pLinkShipMap,
  CMapData *pLinkMainbaseMap,
  CMapData *pLinkPlatformMap,
  int nIndex)
{
  this->m_bAnchor = true;
  this->m_byDirect = 0;
  this->m_bHurry = false;
  this->m_dwNextHurryTime = static_cast<unsigned int>(-1);
  this->m_dwEventCreateTime = timeGetTime();
  this->m_pLinkShipMap = pLinkShipMap;
  this->m_pLinkPortMap[0] = pLinkMainbaseMap;
  this->m_pLinkPortMap[1] = pLinkPlatformMap;
  this->m_byRaceCode_Layer = static_cast<unsigned __int8>(nIndex);

  for (int j = 0; j < 2; ++j)
  {
    this->m_MgrTicket[j].init();
    this->m_MgrTicket[j].pLinkTicketItem = nullptr;

    const int recordNum = g_Main.m_tblItemData[28].GetRecordNum();
    for (int n = 0; n < recordNum; ++n)
    {
      auto *record = reinterpret_cast<_TicketItem_fld *>(g_Main.m_tblItemData[28].GetRecord(n));
      if (record->m_strCivil[2 * this->m_byRaceCode_Layer] != '0'
          && std::strcmp(record->m_strMapCode, this->m_pLinkPortMap[j]->m_pMapSet->m_strCode) == 0)
      {
        this->m_MgrTicket[j].pLinkTicketItem = record;
        break;
      }
    }

    if (!this->m_MgrTicket[j].pLinkTicketItem)
    {
      MyMessageBox("CTransportShip::InitShip", "Ticket Link Error");
      return false;
    }
  }

  memset_0(this->m_NewMember, 0, sizeof(this->m_NewMember));
  memset_0(this->m_OldMember, 0, sizeof(this->m_OldMember));
  return true;
}

void CTransportShip::AlterState(bool bAnchor, unsigned __int8 byDirect, int nPassMin, int nNextSubEventTerm)
{
  m_bAnchor = bAnchor;
  m_byDirect = byDirect;
  m_dwEventCreateTime = timeGetTime();
  m_bHurry = false;
  m_dwNextHurryTime = static_cast<unsigned int>(-1);

  if (m_bAnchor)
  {
    memcpy_0(m_OldMember, m_NewMember, sizeof(m_OldMember));
    memset_0(m_NewMember, 0, sizeof(m_NewMember));
    m_listLogoffMember.ResetList();
    if (nNextSubEventTerm <= nPassMin)
    {
      m_bHurry = true;
    }
    else
    {
      m_dwNextHurryTime =
        60000 * static_cast<unsigned int>(nNextSubEventTerm - nPassMin) + timeGetTime();
    }
  }
  else
  {
    m_MgrTicket[m_byDirect].dwNextUpdateTime = GetLoopTime();
    KickFreeMember();
    memset_0(m_OldMember, 0, sizeof(m_OldMember));
  }

  SendMsg_TransportShipState(-1);
}

void CTransportShip::CheckHurry()
{
  if (m_bAnchor && !m_bHurry && timeGetTime() >= m_dwNextHurryTime)
  {
    m_bHurry = true;
    SendMsg_TransportShipState(-1);
    KickOldMember(m_byDirect);
  }
}

void CTransportShip::CheckTicket()
{
  for (int ticketIndex = 0; ticketIndex < 2; ++ticketIndex)
  {
    __mgr_ticket *ticket = &m_MgrTicket[ticketIndex];
    if (ticket->dwNextUpdateTime != static_cast<unsigned int>(-1) && GetLoopTime() >= ticket->dwNextUpdateTime)
    {
      ticket->init();
    }
  }
}

void CTransportShip::Loop()
{
  if (m_tmrCheckState.CountingTimer())
  {
    CheckTicket();
    CheckHurry();
  }
}

void CTransportShip::KickFreeMember()
{
  const unsigned __int8 kickDirectCode = (m_byDirect == 0);
  KickOldMember(kickDirectCode);

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_NewMember[j];
    if (member->is_fill())
    {
      if (member->pPtr->m_bOper && member->pPtr->m_dwObjSerial == member->dwSerial
          && member->pPtr->m_pCurMap == m_pLinkShipMap)
      {
        Ticketting(member->pPtr);
      }
      else
      {
        member->init();
      }
    }
  }
}

void CTransportShip::KickOldMember(unsigned __int8 byKickDirectCode)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_OldMember[j];
    if (member->is_fill())
    {
      if (member->pPtr->m_bOper && member->pPtr->m_dwObjSerial == member->dwSerial
          && member->pPtr->m_pCurMap == m_pLinkShipMap)
      {
        SendMsg_KickForSail(member->pPtr->m_ObjID.m_wIndex);
        const int raceCode = member->pPtr->m_Param.GetRaceCode();
        const int portalIndex = GetOutPortalIndex(raceCode, byKickDirectCode);
        CheckTicket_Kick(member->pPtr, portalIndex);
      }
      else
      {
        member->init();
      }
    }
  }
}

CTransportShip::__mgr_member *CTransportShip::GetEmptyNewMember()
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_NewMember[j];
    if (!member->is_fill())
    {
      return member;
    }
  }
  return nullptr;
}

void CTransportShip::EnterMember(CPlayer *pEnter)
{
  if (this->m_bAnchor || this->Ticketting(pEnter))
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      __mgr_member *member = &this->m_OldMember[j];
      if (member->is_fill() && member->pPtr == pEnter && member->dwSerial == pEnter->m_dwObjSerial)
      {
        member->init();
      }
    }

    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      __mgr_member *member = &this->m_NewMember[j];
      if (member->is_fill() && member->pPtr == pEnter && member->dwSerial == pEnter->m_dwObjSerial)
      {
        member->init();
      }
    }

    __mgr_member *emptyMember = this->GetEmptyNewMember();
    if (emptyMember)
    {
      emptyMember->pPtr = pEnter;
      emptyMember->dwSerial = pEnter->m_dwObjSerial;
    }
  }
}

void CTransportShip::ReEnterMember(CPlayer *pExiter)
{
  this->m_listLogoffMember.FindNode(pExiter->m_dwObjSerial);
  __mgr_member *emptyMember = this->GetEmptyNewMember();
  if (emptyMember)
  {
    emptyMember->pPtr = pExiter;
    emptyMember->dwSerial = pExiter->m_dwObjSerial;
  }
}

char CTransportShip::RenewOldMember(CPlayer *pMember)
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    __mgr_member *oldMember = &m_OldMember[index];
    if (!oldMember->is_fill() || oldMember->pPtr != pMember || oldMember->dwSerial != pMember->m_dwObjSerial)
    {
      continue;
    }

    __mgr_member *emptyNewMember = GetEmptyNewMember();
    if (!emptyNewMember)
    {
      return 0;
    }

    emptyNewMember->pPtr = pMember;
    emptyNewMember->dwSerial = pMember->m_dwObjSerial;
    oldMember->init();
    return 1;
  }

  return 0;
}

void CTransportShip::ExitMember(CPlayer *pExiter, bool bLogoff)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_NewMember[j];
    if (member->is_fill() && member->pPtr == pExiter && member->dwSerial == pExiter->m_dwObjSerial)
    {
      if (bLogoff && !m_bAnchor)
      {
        m_listLogoffMember.PushNode_Back(pExiter->m_dwObjSerial);
      }
      member->init();
    }
  }

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_OldMember[j];
    if (member->is_fill() && member->pPtr == pExiter && member->dwSerial == pExiter->m_dwObjSerial)
    {
      member->init();
    }
  }
}

bool CTransportShip::Ticketting(CPlayer *pExiter)
{
  _STORAGE_LIST::_db_con *sourceItem = nullptr;
  _bag_db_load *inventory = &pExiter->m_Param.m_dbInven;
  int foundIndex = -1;

  for (int j = 0; j < inventory->m_nUsedNum; ++j)
  {
    _STORAGE_LIST::_db_con *entry = &inventory->m_pStorageList[j];
    if (entry->m_bLoad && entry->m_byTableCode == 28)
    {
      auto *ticket = reinterpret_cast<_TicketItem_fld *>(g_Main.m_tblItemData[28].GetRecord(entry->m_wItemIndex));
      if (GetCurRideShipThisTicket(ticket))
      {
        sourceItem = entry;
        foundIndex = j;
        break;
      }
    }
  }

  if (sourceItem)
  {
    _STORAGE_LIST::_db_con itemCopy;
    memcpy_0(&itemCopy, sourceItem, sizeof(itemCopy));

    if (pExiter->Emb_DelStorage(0, foundIndex, 0, 1, "CTransportShip::Ticketting()"))
    {
      CPlayer::s_MgrItemHistory.consume_del_item(
        pExiter->m_ObjID.m_wIndex,
        &itemCopy,
        pExiter->m_szItemHistoryFileName);
      SendMsg_TicketCheck(pExiter->m_ObjID.m_wIndex, 1, itemCopy.m_wSerial);

      const int raceCode = pExiter->m_Param.GetRaceCode();
      const int outPortalIndex = GetOutPortalIndex(raceCode, m_byDirect);
      CheckTicket_Pass(pExiter, outPortalIndex);

      bool countStat = true;
      if (g_Main.IsReleaseServiceMode() && pExiter->m_byUserDgr)
      {
        countStat = false;
      }
      if (countStat)
      {
        CGameStatistics::_DAY *day = g_GameStatistics.CurWriteData();
        ++day->dwEderEnter_Evt;
      }
      return true;
    }

    SendMsg_TicketCheck(pExiter->m_ObjID.m_wIndex, 0, 0xFFFF);
    const unsigned __int8 kickDirect = (m_byDirect == 0);
    const int raceCode = pExiter->m_Param.GetRaceCode();
    const int portalIndex = GetOutPortalIndex(raceCode, kickDirect);
    CheckTicket_Kick(pExiter, portalIndex);
    return true;
  }

  SendMsg_TicketCheck(pExiter->m_ObjID.m_wIndex, 0, 0xFFFF);
  const unsigned __int8 kickDirect = (m_byDirect == 0);
  const int raceCode = pExiter->m_Param.GetRaceCode();
  const int portalIndex = GetOutPortalIndex(raceCode, kickDirect);
  CheckTicket_Kick(pExiter, portalIndex);
  return true;
}

void CTransportShip::SendMsg_KickForSail(unsigned int n)
{
  unsigned __int8 type[2]{};
  type[0] = 33;
  type[1] = 2;
  char message[1]{};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, message, 1);
}

void CTransportShip::SendMsg_TicketCheck(unsigned int n, char bPass, unsigned __int16 wTicketSerial)
{
  unsigned __int8 type[2]{};
  type[0] = 33;
  type[1] = 3;

  _trans_ship_ticket_pass_inform_zocl message{};
  message.bPass = bPass != 0;
  message.wTicketSerial = wTicketSerial;
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&message), sizeof(message));
}

void CTransportShip::SendMsg_TransportShipState(int n)
{
  unsigned __int8 type[2]{};
  type[0] = 33;
  type[1] = 1;

  unsigned __int8 message[7]{};
  message[0] = static_cast<unsigned __int8>(m_bAnchor);
  message[1] = static_cast<unsigned __int8>(m_bHurry);
  message[2] = m_byDirect;

  DWORD passSec = 0;
  if (n != -1)
  {
    passSec = timeGetTime() - m_dwEventCreateTime;
  }
  memcpy_0(&message[3], &passSec, sizeof(passSec));

  if (n == -1)
  {
    for (unsigned int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *player = &g_Player[index];
      if (player->m_bLive && static_cast<unsigned int>(player->m_Param.GetRaceCode()) == m_byRaceCode_Layer)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(index, type, reinterpret_cast<char *>(message), sizeof(message));
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      static_cast<unsigned int>(n),
      type,
      reinterpret_cast<char *>(message),
      sizeof(message));
  }
}

void CTransportShip::CheckTicket_Pass(CPlayer *pPtr, int nPortalIndex)
{
  _portal_dummy *portal = m_pLinkShipMap->GetPortal(nPortalIndex);
  if (!portal || !portal->m_pPortalRec)
  {
    return;
  }

  CMapData *targetMap = g_MapOper.GetMap(portal->m_pPortalRec->m_strLinkMapCode);
  if (!targetMap)
  {
    return;
  }

  _portal_dummy *targetPortal = targetMap->GetPortal(portal->m_pPortalRec->m_strLinkPortalCode);
  if (!targetPortal)
  {
    return;
  }

  float newPos[3]{};
  if (!targetMap->GetRandPosInDummy(targetPortal->m_pDumPos, newPos, 1))
  {
    memcpy_0(newPos, targetPortal->m_pDumPos->m_fCenterPos, sizeof(newPos));
  }

  if (pPtr->m_pUserDB)
  {
    pPtr->m_pUserDB->Update_Map(targetMap->m_pMapSet->m_dwIndex, newPos);
  }
}

void CTransportShip::CheckTicket_Kick(CPlayer *pPtr, int nPortalIndex)
{
  _portal_dummy *portal = m_pLinkShipMap->GetPortal(nPortalIndex);
  if (!portal || !portal->m_pPortalRec)
  {
    return;
  }

  CMapData *targetMap = g_MapOper.GetMap(portal->m_pPortalRec->m_strLinkMapCode);
  if (!targetMap)
  {
    return;
  }

  _portal_dummy *targetPortal = targetMap->GetPortal(portal->m_pPortalRec->m_strLinkPortalCode);
  if (!targetPortal)
  {
    return;
  }

  float newPos[3]{};
  if (!targetMap->GetRandPosInDummy(targetPortal->m_pDumPos, newPos, 1))
  {
    memcpy_0(newPos, targetPortal->m_pDumPos->m_fCenterPos, sizeof(newPos));
  }

  pPtr->OutOfMap(targetMap, 0, 1, newPos);
  const unsigned __int8 mapCode = static_cast<unsigned __int8>(pPtr->m_Param.GetMapCode());
  pPtr->SendMsg_GotoRecallResult(0, mapCode, newPos, 4);
}

int CTransportShip::GetOutPortalIndex(int nRaceCode, unsigned __int8 byExitDirect)
{
  return 3 * static_cast<int>(byExitDirect) + nRaceCode;
}

bool CTransportShip::GetCurRideShipThisTicket(_TicketItem_fld *pTicketFld)
{
  if (pTicketFld->m_strCivil[2 * m_byRaceCode_Layer] != '1')
  {
    return false;
  }

  unsigned __int8 direct = m_byDirect;
  if (m_bAnchor)
  {
    direct = (m_byDirect == 0);
  }

  return std::strcmp(m_pLinkPortMap[direct]->m_pMapSet->m_strCode, pTicketFld->m_strMapCode) == 0;
}

CMapData *CTransportShip::GetMapCurDirect()
{
  return m_pLinkPortMap[m_byDirect];
}

int CTransportShip::GetRideLimLevel()
{
  if (m_bAnchor)
  {
    const BOOL oppositeDirection = (m_byDirect == 0);
    return m_MgrTicket[oppositeDirection].pLinkTicketItem->m_nLevelLim;
  }
  return m_MgrTicket[m_byDirect].pLinkTicketItem->m_nLevelLim;
}

int CTransportShip::GetRideUpLimLevel()
{
  if (m_bAnchor)
  {
    const BOOL oppositeDirection = (m_byDirect == 0);
    return m_MgrTicket[oppositeDirection].pLinkTicketItem->m_nUpLevelLim;
  }
  return m_MgrTicket[m_byDirect].pLinkTicketItem->m_nUpLevelLim;
}

void CTransportShip::InitTicketReserver()
{
  for (int j = 0; j < 2; ++j)
  {
    m_MgrTicket[j].nReserTicketNum = 0;
  }
}

void CTransportShip::ApplyTicketReserver()
{
  for (int j = 0; j < 2; ++j)
  {
    m_MgrTicket[j].nCurTicketNum -= m_MgrTicket[j].nReserTicketNum;
    if (m_MgrTicket[j].nCurTicketNum < 0)
    {
      m_MgrTicket[j].nCurTicketNum = 0;
    }
  }
}

int CTransportShip::GetLeftTicketIncludeReserNum(char *pszTarMapCode, int nAdd)
{
  int leftTicket = 0;
  for (int j = 0; j < 2; ++j)
  {
    if (!strcmp_0(m_pLinkPortMap[j]->m_pMapSet->m_strCode, pszTarMapCode))
    {
      leftTicket = m_MgrTicket[j].nCurTicketNum - m_MgrTicket[j].nReserTicketNum;
      if (leftTicket < 0)
      {
        leftTicket = 0;
      }
      m_MgrTicket[j].nReserTicketNum += nAdd;
      return leftTicket;
    }
  }
  return leftTicket;
}

bool CTransportShip::IsMemberBeforeLogoff(unsigned int dwPlayerSerial)
{
  return this->m_listLogoffMember.IsInList(dwPlayerSerial);
}

char CTransportShip::IsOldMember(CPlayer *pMember)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_OldMember[j];
    if (member->is_fill() && member->pPtr == pMember && member->dwSerial == pMember->m_dwObjSerial)
    {
      return 1;
    }
  }

  return 0;
}

void CTransportShip::GetStartPosInShip(float *pfPos)
{
  if (this->m_pLinkShipMap)
  {
    this->m_pLinkShipMap->GetRandPosInDummy(this->m_pLinkShipMap->m_pStartDummy->m_pDumPos, pfPos, true);
  }
}
