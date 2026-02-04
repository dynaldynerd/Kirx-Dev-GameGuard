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
  dwNextUpdateTime = static_cast<unsigned int>(-1);
}

bool CTransportShip::InitShip(
  CTransportShip *pShip,
  CMapData *pLinkShipMap,
  CMapData *pLinkMainbaseMap,
  CMapData *pLinkPlatformMap,
  int nIndex)
{
  pShip->m_bAnchor = true;
  pShip->m_byDirect = 0;
  pShip->m_bHurry = false;
  pShip->m_dwNextHurryTime = static_cast<unsigned int>(-1);
  pShip->m_dwEventCreateTime = timeGetTime();
  pShip->m_pLinkShipMap = pLinkShipMap;
  pShip->m_pLinkPortMap[0] = pLinkMainbaseMap;
  pShip->m_pLinkPortMap[1] = pLinkPlatformMap;
  pShip->m_byRaceCode_Layer = static_cast<unsigned __int8>(nIndex);

  for (int j = 0; j < 2; ++j)
  {
    pShip->m_MgrTicket[j].init();
    pShip->m_MgrTicket[j].pLinkTicketItem = nullptr;

    const int recordNum = g_Main.m_tblItemData[28].GetRecordNum();
    for (int n = 0; n < recordNum; ++n)
    {
      auto *record = reinterpret_cast<_TicketItem_fld *>(g_Main.m_tblItemData[28].GetRecord(n));
      if (record->m_strCivil[2 * pShip->m_byRaceCode_Layer] != '0'
          && std::strcmp(record->m_strMapCode, pShip->m_pLinkPortMap[j]->m_pMapSet->m_strCode) == 0)
      {
        pShip->m_MgrTicket[j].pLinkTicketItem = record;
        break;
      }
    }

    if (!pShip->m_MgrTicket[j].pLinkTicketItem)
    {
      MyMessageBox("CTransportShip::InitShip", "Ticket Link Error");
      return false;
    }
  }

  memset_0(pShip->m_NewMember, 0, sizeof(pShip->m_NewMember));
  memset_0(pShip->m_OldMember, 0, sizeof(pShip->m_OldMember));
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
    CNetIndexList::ResetList(&m_listLogoffMember);
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
        const int raceCode = CPlayerDB::GetRaceCode(&member->pPtr->m_Param);
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

void CTransportShip::ExitMember(CPlayer *pExiter, bool bLogoff)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    __mgr_member *member = &m_NewMember[j];
    if (member->is_fill() && member->pPtr == pExiter && member->dwSerial == pExiter->m_dwObjSerial)
    {
      if (bLogoff && !m_bAnchor)
      {
        CNetIndexList::PushNode_Back(&m_listLogoffMember, pExiter->m_dwObjSerial);
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

      const int raceCode = CPlayerDB::GetRaceCode(&pExiter->m_Param);
      const int outPortalIndex = GetOutPortalIndex(raceCode, m_byDirect);
      CheckTicket_Pass(pExiter, outPortalIndex);

      bool countStat = true;
      if (g_Main.IsReleaseServiceMode() && pExiter->m_byUserDgr)
      {
        countStat = false;
      }
      if (countStat)
      {
        CGameStatistics::_DAY *day = CGameStatistics::CurWriteData(&g_GameStatistics);
        ++day->dwEderEnter_Evt;
      }
      return true;
    }

    SendMsg_TicketCheck(pExiter->m_ObjID.m_wIndex, 0, 0xFFFF);
    const unsigned __int8 kickDirect = (m_byDirect == 0);
    const int raceCode = CPlayerDB::GetRaceCode(&pExiter->m_Param);
    const int portalIndex = GetOutPortalIndex(raceCode, kickDirect);
    CheckTicket_Kick(pExiter, portalIndex);
    return true;
  }

  SendMsg_TicketCheck(pExiter->m_ObjID.m_wIndex, 0, 0xFFFF);
  const unsigned __int8 kickDirect = (m_byDirect == 0);
  const int raceCode = CPlayerDB::GetRaceCode(&pExiter->m_Param);
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
  struct
  {
    char pass;
    unsigned __int16 ticketSerial;
  } message{};
  message.pass = bPass;
  message.ticketSerial = wTicketSerial;
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&message), 3);
}

void CTransportShip::SendMsg_TransportShipState(int n)
{
  unsigned __int8 type[2]{};
  type[0] = 33;
  type[1] = 1;

  unsigned __int8 message[3]{};
  message[0] = static_cast<unsigned __int8>(m_bAnchor);
  message[1] = static_cast<unsigned __int8>(m_bHurry);
  message[2] = m_byDirect;

  if (n == -1)
  {
    for (unsigned int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *player = &g_Player[index];
      if (player->m_bLive && static_cast<unsigned int>(CPlayerDB::GetRaceCode(&player->m_Param)) == m_byRaceCode_Layer)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(index, type, reinterpret_cast<char *>(message), 7);
      }
    }
  }
  else
  {
    const DWORD elapsed = timeGetTime() - m_dwEventCreateTime;
    (void)elapsed;
    g_Network.m_pProcess[0]->LoadSendMsg(static_cast<unsigned int>(n), type, reinterpret_cast<char *>(message), 7);
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
    CUserDB::Update_Map(pPtr->m_pUserDB, targetMap->m_pMapSet->m_dwIndex, newPos);
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
  const unsigned __int8 mapCode = static_cast<unsigned __int8>(CPlayerDB::GetMapCode(&pPtr->m_Param));
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
