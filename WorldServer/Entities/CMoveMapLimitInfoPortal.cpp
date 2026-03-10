#include "pch.h"

#include "CMoveMapLimitInfoPortal.h"

#include <cstdio>
#include <cstring>
#include <new>

#include "CMapData.h"
#include "CMapOperation.h"
#include "CMoveMapLimitRightInfo.h"
#include "CMerchant.h"
#include "CItemStore.h"
#include "StoreList_fld.h"
#include "CMyTimer.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "notice_move_limit_map_msg_zocl.h"
#include "Packet/ZoneClientPacket.h"

CMoveMapLimitInfoPortal::CMoveMapLimitInfoPortal(unsigned int uiInx, int iType)
  : CMoveMapLimitInfo(uiInx, static_cast<OBJ_TYPE>(iType)),
    m_pkSrcDummy(nullptr),
    m_pkDestDummy(nullptr),
    m_pkRegenDummy(nullptr),
    m_vecAllowDummyCode(),
    m_eNotifyForceMoveHQState(FMHS_WAIT),
    m_pkNotifyForceMoveHQTimer(nullptr),
    m_uiProcNotifyInx(0)
{
}

CMoveMapLimitInfoPortal::~CMoveMapLimitInfoPortal()
{
  m_pStoreNPC = nullptr;
  m_pkSrcDummy = nullptr;
  m_pkDestDummy = nullptr;
  m_pkRegenDummy = nullptr;
  m_vecAllowDummyCode.clear();
  m_uiProcNotifyInx = 0;

  delete m_pkNotifyForceMoveHQTimer;
  m_pkNotifyForceMoveHQTimer = nullptr;
}

bool CMoveMapLimitInfoPortal::Init()
{
  m_pkNotifyForceMoveHQTimer = new (std::nothrow) CMyTimer();
  if (!m_pkNotifyForceMoveHQTimer)
  {
    g_Main.m_logLoadingError.Write(
      "CMoveMapLimitInfoPortal::Init() : m_pkNotifyForceMoveHQTimer = new CMyTimer Fail!");
    return false;
  }
  return LoadINI();
}

bool CMoveMapLimitInfoPortal::LoadINI()
{
  char buffer[1024]{};
  char returned[1024]{};

  const unsigned int inx = GetInx();
  std::sprintf(buffer, "Map%u", inx);
  GetPrivateProfileStringA("MoveMapLimitInfo", buffer, "NULL", returned, 1024, "./Initialize/MoveMapLimit.ini");
  if (!strcmp_0(returned, "NULL"))
  {
    g_Main.m_logLoadingError.Write(
      "CMoveMapLimitInfoPortal::LoadINI() : Inx(%u) Type(%d) %s NULL!",
      GetInx(),
      GetType(),
      buffer);
    return false;
  }

  CMapData *map = g_MapOper.GetMap(returned);
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CMoveMapLimitInfoPortal::LoadINI() : Inx(%u) Type(%d) g_MapOper.GetMap( szData(%s) ) NULL!",
      GetInx(),
      GetType(),
      returned);
    return false;
  }

  m_iMapInx = map->m_nMapCode;
  return true;
}

void CMoveMapLimitInfoPortal::Loop()
{
  switch (m_eNotifyForceMoveHQState)
  {
    case FMHS_WAIT_NOTIFY_FORCE_MOVE:
      if (m_pkNotifyForceMoveHQTimer->CountingTimer())
      {
        m_pkNotifyForceMoveHQTimer->StopTimer();
        m_eNotifyForceMoveHQState = FMHS_NOTIFY_FORCE_MOVE;
      }
      break;
    case FMHS_NOTIFY_FORCE_MOVE:
      SubProcNotifyForceMoveHQ();
      break;
    case FMHS_WAIT_FORCE_MOVE:
      if (m_pkNotifyForceMoveHQTimer->CountingTimer())
      {
        m_pkNotifyForceMoveHQTimer->StopTimer();
        m_eNotifyForceMoveHQState = FMHS_FORCE_MOVE;
      }
      break;
    case FMHS_FORCE_MOVE:
      SubProcForceMoveHQ();
      break;
    default:
      break;
  }
}

void CMoveMapLimitInfoPortal::SubProcNotifyForceMoveHQ()
{
  unsigned int sentCount = 0;
  unsigned __int8 type[2]{};
  type[0] = 59;
  type[1] = 1;

  _notice_move_limit_map_msg_zocl msg{};
  msg.byType = 0;

  for (unsigned int index = m_uiProcNotifyInx; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive && m_iMapInx == g_Player[index].m_pCurMap->m_nMapCode)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        g_Player[index].m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
      if (++sentCount >= 100)
      {
        m_uiProcNotifyInx = index + 1;
        break;
      }
    }
  }

  if (m_uiProcNotifyInx >= MAX_PLAYER)
  {
    m_uiProcNotifyInx = 0;
    m_pkNotifyForceMoveHQTimer->BeginTimer(10000);
    m_eNotifyForceMoveHQState = FMHS_WAIT_FORCE_MOVE;
  }
}

void CMoveMapLimitInfoPortal::SubProcForceMoveHQ()
{
  unsigned int sentCount = 0;
  CMapData *intoMap = nullptr;
  float outPos[3]{};

  unsigned __int8 type[2]{};
  type[0] = 59;
  type[1] = 1;

  _notice_move_limit_map_msg_zocl msg{};
  msg.byType = 1;

  for (unsigned int index = m_uiProcNotifyInx; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive && m_iMapInx == g_Player[index].m_pCurMap->m_nMapCode)
    {
      const unsigned __int8 race = static_cast<unsigned __int8>(g_Player[index].m_Param.GetRaceCode());
      intoMap = g_MapOper.GetPosStartMap(race, false, outPos);
      if (intoMap)
      {
        g_Player[index].pc_Resurrect(false);
        g_Player[index].ForcePullUnit(false);
        g_Player[index].OutOfMap(intoMap, 0, 3u, outPos);
        g_Player[index].SendMsg_GotoBasePortalResult(0);
        g_Network.m_pProcess[0]->LoadSendMsg(
          g_Player[index].m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          static_cast<unsigned __int16>(sizeof(msg)));
        if (++sentCount >= 100)
        {
          m_uiProcNotifyInx = index + 1;
          break;
        }
      }
    }
  }

  if (m_uiProcNotifyInx >= MAX_PLAYER)
  {
    m_uiProcNotifyInx = 0;
    m_pkNotifyForceMoveHQTimer->StopTimer();
    m_eNotifyForceMoveHQState = FMHS_WAIT;
  }
}

unsigned __int8 CMoveMapLimitInfoPortal::Request(
  int iUserInx,
  int iRequetType,
  char *pRequest,
  CMoveMapLimitRightInfo *pkRight)
{
  if (iRequetType == 0)
  {
    return ProcForceMoveHQ(iUserInx, pRequest, pkRight);
  }
  if (iRequetType == 1)
  {
    return ProcUseMoveScroll(iUserInx, pRequest, pkRight);
  }
  if (iRequetType == 2)
  {
    return ProcGotoLimitZone(static_cast<unsigned int>(iUserInx), pRequest, pkRight);
  }
  return 1;
}

void CMoveMapLimitInfoPortal::Load(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
  CMapData *map = g_MapOper.GetMap(static_cast<int>(pkPlayer->m_Param.m_dbChar.m_sStartMapCode));
  if (!map)
  {
    return;
  }
  if (m_iMapInx != map->m_nMapCode)
  {
    return;
  }

  const int type = GetType();
  if (!pkRight->IsHaveRight(type))
  {
    float newPos[3]{};
    if (map->GetRandPosInDummy(m_pkRegenDummy, newPos, true))
    {
      pkPlayer->m_Param.SetCurPos(newPos);
      pkRight->SetFlag(type, 0, true);
    }
  }
}

unsigned __int8 CMoveMapLimitInfoPortal::ProcUseMoveScroll(
  int iUserInx,
  char *pRequest,
  CMoveMapLimitRightInfo *pkRight)
{
  for (char *dummyCode : m_vecAllowDummyCode)
  {
    if (!strcmp_0(dummyCode, pRequest))
    {
      return 0;
    }
  }

  if (pkRight && pkRight->IsHaveRight(m_eType))
  {
    return 0;
  }
  return 10;
}

unsigned __int8 CMoveMapLimitInfoPortal::ProcGotoLimitZone(
  unsigned int iUserInx,
  char *pRequest,
  CMoveMapLimitRightInfo *pkRight)
{
  const unsigned __int8 err = SubProcGotoLimitZone(iUserInx, pRequest, pkRight);
  if (err)
  {
    unsigned __int8 type[2]{};
    type[0] = 4;
    type[1] = 40;
    _move_limit_map_zone_result_zocl msg{};
    msg.byRet = static_cast<char>(err);
    g_Network.m_pProcess[0]->LoadSendMsg(
      iUserInx,
      type,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }
  return 0;
}

unsigned __int8 CMoveMapLimitInfoPortal::SubProcGotoLimitZone(
  unsigned int iUserInx,
  char *pRequest,
  CMoveMapLimitRightInfo *pkRight)
{
  if (iUserInx >= MAX_PLAYER)
  {
    return 20;
  }

  CPlayer *player = &g_Player[iUserInx];
  const unsigned int requestedStoreIndex = *reinterpret_cast<const unsigned int *>(pRequest);
  CMapData *currentMap = player->m_pCurMap;

  if (!m_pStoreNPC || !m_pkDestDummy)
  {
    return 5;
  }
  if (m_iMapInx != currentMap->m_nMapCode)
  {
    return 21;
  }
  if (m_pStoreNPC->m_pItemStore->m_pRec->m_dwIndex != requestedStoreIndex)
  {
    return 22;
  }
  if (player->GetCurSecNum() == -1 || player->m_bMapLoading)
  {
    return 23;
  }
  if (player->m_bCorpse)
  {
    return 24;
  }
  if (currentMap->m_pMapSet->m_nMapType)
  {
    return 25;
  }
  if (player->m_byStandType == 1)
  {
    return 26;
  }
  if (player->IsSiegeMode())
  {
    return 27;
  }
  if (player->IsRidingUnit())
  {
    return 28;
  }
  if (player->m_EP.GetEff_State(EFF_STATE_INSUPERABLE))
  {
    return 29;
  }
  if (player->m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
  {
    return 29;
  }
  if (GetSqrt(m_pStoreNPC->m_pItemStore->m_pDum->m_pDumPos->m_fCenterPos, player->m_fCurPos) > 100.0)
  {
    return 30;
  }
  const int limitType = GetType();
  if (!pkRight->IsHaveRight(limitType))
  {
    return 31;
  }

  float newPos[3]{};
  if (!currentMap->GetRandPosInDummy(m_pkDestDummy, newPos, true))
  {
    return 32;
  }

  player->OutOfMap(currentMap, 0, 3u, newPos);
  player->SendMsg_GotoBasePortalResult(0);
  return 0;
}

unsigned __int8 CMoveMapLimitInfoPortal::ProcForceMoveHQ(
  int iUserInx,
  char *pRequest,
  CMoveMapLimitRightInfo *pkRight)
{
m_uiProcNotifyInx = 0;
  m_pkNotifyForceMoveHQTimer->BeginTimer(4000);
  m_eNotifyForceMoveHQState = FMHS_WAIT_NOTIFY_FORCE_MOVE;
  return 0;
}
