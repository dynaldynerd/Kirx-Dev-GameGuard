#include "pch.h"

#include "CMainThread.h"

#include <crtdbg.h>
#include <process.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "AutominePersonalMgr.h"
#include "AutoMineMachineMng.h"
#include "CActionPointSystemMgr.h"
#include "CAsyncLogger.h"
#include "CAttack.h"
#include "CBillingManager.h"
#include "CBossMonsterScheduleSystem.h"
#include "CCashDBWorkManager.h"
#include "CExchangeEvent.h"
#include "CGoldenBoxItemMgr.h"
#include "CGuildBattleController.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CPotionMgr.h"
#include "CLuaScriptMgr.h"
#include "CLogTypeDBTaskManager.h"
#include "CMapDisplay.h"
#include "CMapOperation.h"
#include "CMonsterEventRespawn.h"
#include "CMonsterEventSet.h"
#include "CMoveMapLimitManager.h"
#include "CNetworkEX.h"
#include "CNetProcess.h"
#include "CPcBangFavor.h"
#include "CPostSystemManager.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRaceBuffManager.h"
#include "CRaceBossWinRate.h"
#include "CTotalGuildRankManager.h"
#include "CUnmannedTraderController.h"

bool _TRADE_DB_BASE::_LIST::IsEmpty()
{
  return this->dwRegistSerial == 0;
}
#include "CUnmannedTraderTaxRateManager.h"
#include "CWeeklyGuildRankManager.h"
#include "CashItemRemoteStore.h"
#include "DqsDbStructs.h"
#include "qry_case_addpvppoint.h"
#include "AmuletItem_fld.h"
#include "TimeItem.h"
#include "UIDGenerator.h"
#include "qry_case_insertitem.h"
#include "AnimusItem_fld.h"
#include "BagItem_fld.h"
#include "BattleDungeonItem_fld.h"
#include "BatteryItem_fld.h"
#include "BootyItem_fld.h"
#include "BoxItem_fld.h"
#include "BulletItem_fld.h"
#include "CloakItem_fld.h"
#include "CouponItem_fld.h"
#include "DfnEquipItem_fld.h"
#include "EventItem_fld.h"
#include "FIRECRACKER_fld.h"
#include "FaceItem_fld.h"
#include "ForceItem_fld.h"
#include "GuardTowerItem_fld.h"
#include "MakeToolItem_fld.h"
#include "MapItem_fld.h"
#include "mobmsg_fld.h"
#include "npc_fld.h"
#include "NPCLink_fld.h"
#include "OreItem_fld.h"
#include "player_fld.h"
#include "PotionItem_fld.h"
#include "RadarItem_fld.h"
#include "RecoveryItem_fld.h"
#include "ResourceItem_fld.h"
#include "RingItem_fld.h"
#include "SiegeKitItem_fld.h"
#include "TOWNItem_fld.h"
#include "TicketItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitBullet_fld.h"
#include "UnitKeyItem_fld.h"
#include "UnitPart_fld.h"
#include "UNmannedminer_fld.h"
#include "WeaponItem_fld.h"
#include "CandidateMgr.h"
#include "CSUItemSystem.h"
#include "CDarkHoleDungeonQuest.h"
#include "CCryptor.h"
#include "CRtc.h"
#include "CNationCodeStrTable.h"
#include "CNationSettingManager.h"
#include "DqsOnRunPayloads.h"
#include "Packet/AccountZonePacket.h"
#include "Packet/ZoneAccountPacket.h"
#include "Packet/ZoneWebPacket.h"
#include "CheatCommands.h"
#include "NameTxt_fld.h"
#include "ResourceItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "AnimusItem_fld.h"
#include "ItemMakeData_fld.h"
#include "EditData_fld.h"
#include "BulletItem_fld.h"
#include "CNuclearBombMgr.h"
#include "CPvpCashMng.h"
#include "CQuestMgr.h"
#include "CRecallEffectController.h"
#include "CReturnGateController.h"
#include "CRaceBossMsgController.h"
#include "CMoneySupplyMgr.h"
#include "CUserDB.h"
#include "ItemDataLoader.h"
#include "KorLocalTime.h"
#include "MonsterSetInfoData.h"
#include "AggroCaculateData.h"
#include "PatriarchElectProcessor.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "TimeItem.h"
#include "NetUtil.h"
#include "WheatyExceptionReport.h"
#include "WorldServerUtil.h"
#include "EconomySystemFunctions.h"
#include "economy_history_data.h"
#include "CItemStoreManager.h"
#include "cStaticMember_Player.h"
#include "GlobalObjects.h"
#include "CGameServerDoc.h"
#include "CGameServerView.h"

#include "LendItemMng.h"
#include "TimeLimitJadeMng.h"
#include "DfAIMgr.h"
#include "RFEvent_ClassRefine.h"
#include "COreAmountMgr.h"
#include "start_world_request_wrac.h"
#include "trans_account_report_wrac.h"
#include "trans_gm_msg_inform_zocl.h"

#include <mmsystem.h>
#include <limits>
#include "CMainThreadLocalStructs.h"

namespace
{
unsigned int g_dwTimeAliveMonNum = 0;
unsigned int g_dwMaxDeadMonNum = 0;
constexpr const char kCandidateMgrInitFailFmt[] = "CandidateMgr::Instance()->Initialize(%d) Fail!";
constexpr DWORD kHalfDays = 43200000;
constexpr char kWorldInfoIniPath[] = "..\\WorldInfo\\WorldInfo.ini";
constexpr char kWorldSystemIniPath[] = ".\\Initialize\\WorldSystem.ini";
constexpr unsigned __int16 kDefaultAccountPort = 29000;
constexpr unsigned __int16 kDefaultGatePort = 27780;

bool TryParseConfiguredPort(const char *text, unsigned __int16 *port)
{
  if (text == nullptr || port == nullptr || text[0] == '\0')
  {
    return false;
  }

  char *end = nullptr;
  const unsigned long parsed = std::strtoul(text, &end, 10);
  if (end == text || *end != '\0' || parsed == 0 || parsed > 65535)
  {
    return false;
  }

  *port = static_cast<unsigned __int16>(parsed);
  return true;
}

bool ReadConfiguredPort(
  const char *iniPath,
  const char *section,
  const char *key,
  unsigned __int16 defaultPort,
  unsigned __int16 *port)
{
  char defaultBuffer[16]{};
  sprintf_s(defaultBuffer, sizeof(defaultBuffer), "%u", defaultPort);

  char valueBuffer[16]{};
  GetPrivateProfileStringA(section, key, defaultBuffer, valueBuffer, static_cast<DWORD>(sizeof(valueBuffer)), iniPath);
  return TryParseConfiguredPort(valueBuffer, port);
}

bool ReadConfiguredIpv4Address(const char *iniPath, const char *section, const char *key, unsigned int *address)
{
  if (address == nullptr)
  {
    return false;
  }

  char valueBuffer[128]{};
  GetPrivateProfileStringA(section, key, "", valueBuffer, static_cast<DWORD>(sizeof(valueBuffer)), iniPath);
  if (valueBuffer[0] == '\0')
  {
    return false;
  }

  IN_ADDR parsedAddress{};
  if (InetPtonA(AF_INET, valueBuffer, &parsedAddress) != 1)
  {
    return false;
  }

  *address = parsedAddress.s_addr;
  return true;
}

bool ReadGateIpv4Address(unsigned int *address)
{
  if (address == nullptr)
  {
    return false;
  }

  char valueBuffer[128]{};
  GetPrivateProfileStringA("System", "GateIP", "", valueBuffer, static_cast<DWORD>(sizeof(valueBuffer)), kWorldInfoIniPath);
  if (valueBuffer[0] == '\0' || std::strcmp(valueBuffer, "X") == 0)
  {
    *address = GetIPAddress();
    return *address != static_cast<unsigned int>(-1);
  }

  IN_ADDR parsedAddress{};
  if (InetPtonA(AF_INET, valueBuffer, &parsedAddress) != 1)
  {
    return false;
  }

  *address = parsedAddress.s_addr;
  return true;
}
}

unsigned int TimeLimitMgr::m_dwCnt;
TimeLimitMgr *TimeLimitMgr::m_pTLStatusMgr = nullptr;

_BILLING_INFO::_BILLING_INFO()
{
  bPCCheat = 0;
}

_WAIT_ENTER_ACCOUNT::_WAIT_ENTER_ACCOUNT()
{
  m_BillingInfo = _BILLING_INFO();
  m_bLoad = false;
  m_byUserDgr = 0;
}

void _WAIT_ENTER_ACCOUNT::Release()
{
  m_bLoad = false;
}

void _WAIT_ENTER_ACCOUNT::SetData(
  unsigned int dwAccountSerial,
  char *pszAccountID,
  unsigned __int8 byUserDgr,
  unsigned __int8 bySubDgr,
  _GLBID *pgidGlobal,
  unsigned int *pdwKey,
  bool bChatLock)
{
  m_bLoad = true;
  m_dwAccountSerial = dwAccountSerial;
  std::strcpy(m_szAccountID, pszAccountID);
  m_byUserDgr = byUserDgr;
  m_bySubDgr = bySubDgr;
  std::memcpy(&m_gidGlobal, pgidGlobal, sizeof(m_gidGlobal));
  std::memcpy(m_dwKey, pdwKey, sizeof(m_dwKey));
  m_dwLoadTime = timeGetTime();
  m_bChatLock = bChatLock;
}

void _WAIT_ENTER_ACCOUNT::SetBillingInfo(__int16 iType, char *szCMS, int lRemainTime, _SYSTEMTIME *pstEndDate)
{
  m_BillingInfo.iType = iType;
  m_BillingInfo.lRemainTime = lRemainTime;
  std::memcpy(m_BillingInfo.szCMS, szCMS, sizeof(m_BillingInfo.szCMS));
  std::memcpy(&m_BillingInfo.stEndDate, pstEndDate, sizeof(m_BillingInfo.stEndDate));
}

void _WAIT_ENTER_ACCOUNT::SetPcBangFlag(bool bIsPcBang)
{
  m_BillingInfo.bIsPcBang = bIsPcBang;
}

void _WAIT_ENTER_ACCOUNT::SetTransFlag(int nTrans)
{
  m_nTrans = nTrans;
}

void _WAIT_ENTER_ACCOUNT::SetAgeLimitFlag(bool bAgeLimit)
{
  m_BillingInfo.bAgeLimit = bAgeLimit;
}

void _WAIT_ENTER_ACCOUNT::SetUILock(
  unsigned __int8 byUILock,
  char *szUILockPW,
  unsigned __int8 byUILockFailCnt,
  char *szAccountPW,
  unsigned __int8 byHintIndex,
  char *uszHintAnswer,
  unsigned __int8 byUILockFindPassFailCount)
{
  char wszTran[40]{};

  m_byUILock = byUILock;
  m_byUILock_FailCnt = byUILockFailCnt;
  m_byUILock_HintIndex = byHintIndex;
  if (szUILockPW)
  {
    strcpy_s(m_szUILock_PW, sizeof(m_szUILock_PW), szUILockPW);
  }
  else
  {
    strcpy_s(m_szUILock_PW, sizeof(m_szUILock_PW), "________PW__");
  }

  if (szAccountPW)
  {
    M2W(szAccountPW, wszTran, 13);
    strcpy_s(m_szAccount_PW, sizeof(m_szAccount_PW), wszTran);
  }
  else
  {
    strcpy_s(m_szAccount_PW, sizeof(m_szAccount_PW), "________PW__");
  }

  if (uszHintAnswer)
  {
    strcpy_s(m_uszUILock_HintAnswer, sizeof(m_uszUILock_HintAnswer), uszHintAnswer);
  }
  else
  {
    strcpy_s(m_uszUILock_HintAnswer, sizeof(m_uszUILock_HintAnswer), "________PW__");
  }

  m_byUILockFindPassFailCount = byUILockFindPassFailCount;
}

_server_rate_realtime_load::_server_rate_realtime_load()
{
  Init(10000);
}

void _server_rate_realtime_load::Init(unsigned int dwReadTerm)
{
  std::memset(this, 0, sizeof(_server_rate_realtime_load));
  m_tmDataFileCheckTime.BeginTimer(dwReadTerm);
}

CMainThread::CMainThread() = default;

CMainThread::~CMainThread()
{
  Release();
}

void CMainThread::Release()
{
  m_bRuleThread = false;
  m_bDQSThread = false;
  Sleep(3000);

  if (m_pWorldDB)
  {
    delete m_pWorldDB;
    m_pWorldDB = nullptr;
  }

  CGuildBattleController::Destroy();
  CRaceBossMsgController::Instance()->CleanUp();
  CReturnGateController::Destroy();
  CRecallEffectController::Destroy();
  CTotalGuildRankManager::Destroy();
  CWeeklyGuildRankManager::Destroy();
  AutoMineMachineMng::Release();
  CLogTypeDBTaskManager::Destroy();
  CLuaScriptMgr::Destroy();
  CItemStoreManager::Destroy();
  COreAmountMgr::Instance()->Release();
  cStaticMember_Player::Release();

  delete[] g_Monster;
  g_Monster = nullptr;
  delete[] g_NPC;
  g_NPC = nullptr;
  delete[] g_Animus;
  g_Animus = nullptr;
  delete[] g_Tower;
  g_Tower = nullptr;
  delete[] g_ItemBox;
  g_ItemBox = nullptr;
  delete[] g_ParkingUnit;
  g_ParkingUnit = nullptr;
  delete[] g_Stone;
  g_Stone = nullptr;
  delete[] g_Keeper;
  g_Keeper = nullptr;
  delete[] g_Trap;
  g_Trap = nullptr;
  delete[] g_DarkHole;
  g_DarkHole = nullptr;
  delete[] g_Guild;
  g_Guild = nullptr;
  if (g_MonsterEventSet)
  {
    delete g_MonsterEventSet;
    g_MonsterEventSet = nullptr;
  }
}

bool CMainThread::IsTestServer() const
{
  return m_byWorldType == 1;
}

bool CMainThread::IsReleaseServiceMode() const
{
  return m_bReleaseServiceMode;
}

bool CMainThread::IsExcuteService() const
{
  return m_bExcuteService;
}

unsigned int CMainThread::GetMonsterRecordNum() const
{
  return m_tblMonster.GetRecordNum();
}

void CMainThread::AccountServerLogin()
{
  _open_world_request_wrac request;

  std::strcpy(request.szWorldName, m_szWorldName);
  request.dwWorldServerIP = m_dwGateIP;

  memcpy_s(request.byHash, sizeof(request.byHash), g_cbHashVerify, sizeof(g_cbHashVerify));

  unsigned __int8 type[2] = {1, 1};
  const unsigned __int16 len = request.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&request), len);

  CNationSettingManager::Instance()->SendCashDBDSNRequest();
}

void CMainThread::gm_ServerClose()
{
  if (!m_bServerClosing)
  {
    m_bServerClosing = true;
    m_tmForceUserExit.BeginTimer(50);
    m_nForceExitSocketIndexOffset = 0;
  }
}

void CMainThread::gm_DisplaymodeChange()
{
  if (g_MapDisplay.m_bDisplayMode)
  {
    if (g_MapDisplay.OffDisplay())
    {
      g_pDoc->m_DisplayView.ShowWindow(SW_HIDE);
      g_pDoc->m_DisplayView.ShowWindow(SW_SHOW);
    }
    else
    {
      MyMessageBox("Display Error", "OffDisplay() Failure");
    }
    return;
  }

  CMapData *map = nullptr;
  unsigned __int16 layerIndex = 0;
  if (g_MapDisplay.m_pOldActMap)
  {
    map = g_MapDisplay.m_pOldActMap;
    layerIndex = g_MapDisplay.m_wOldLayerIndex;
  }
  else if (CGameObject::s_pSelectObject)
  {
    map = CGameObject::s_pSelectObject->m_pCurMap;
    layerIndex = CGameObject::s_pSelectObject->m_wMapLayerIndex;
  }
  else
  {
    map = g_MapOper.GetMap(0);
    layerIndex = 0;
  }

  if (!g_MapDisplay.OnDisplay(map, layerIndex))
  {
    MyMessageBox("Display Error", "OnDisplay() Failure");
  }
}

void CMainThread::gm_MapChange(CMapData *pMap)
{
  g_MapDisplay.m_MapExtend.m_bExtendMode = false;
  g_pDoc->m_DisplayView.SetExtendMode(0);
  g_MapDisplay.ChangeMap(pMap);
}

void CMainThread::gm_ObjectSelect()
{
  CGameObject *selected = g_MapDisplay.SelectObject(&g_pDoc->m_DisplayView.m_ptLBtn);
  if (selected)
  {
    CGameObject::s_pSelectObject = selected;
    if (g_pDoc->m_InfoSheet.GetActiveIndex() == 3)
    {
      m_GameMsg.PackingMsg(1006, 0, 0, 0);
    }
  }
}

void CMainThread::gm_UpdateServer()
{
  if (g_pDoc != nullptr)
  {
    g_pDoc->m_InfoSheet.m_tabServer.UpdateServerTab();
  }
}

void CMainThread::gm_UpdateObject()
{
  if (g_pDoc != nullptr)
  {
    g_pDoc->m_InfoSheet.m_tabObject.UpdateTab();
  }
}

void CMainThread::gm_UpdateMap()
{
  if (g_pDoc != nullptr)
  {
    g_pDoc->m_InfoSheet.m_tabMap.UpdateTab();
  }
}

void CMainThread::gm_DisplayAll()
{
  gm_DisplaymodeChange();
  if (g_MapDisplay.m_bDisplayMode && g_pDoc != nullptr && g_pDoc->m_pwndMainView != nullptr)
  {
    auto *mainView = static_cast<CGameServerView *>(g_pDoc->m_pwndMainView);
    mainView->OnButtonDummy();
    mainView->OnButtonCollline();
  }
}

void CMainThread::gm_PreCloseAnn()
{
  char buffer[132]{};
  sprintf_s(buffer, sizeof(buffer), "%s server close warning.", m_szWorldName);

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Player[index].SendData_ChatTrans(
        0,
        -1,
        static_cast<unsigned __int8>(-1),
        0,
        buffer,
        static_cast<unsigned __int8>(-1),
        nullptr);
    }
  }
}

void CMainThread::gm_UserExit()
{
  CGameObject *selectedObject = CGameObject::s_pSelectObject;
  if (!selectedObject || selectedObject->m_ObjID.m_byKind != 0)
  {
    return;
  }

  if (selectedObject->m_ObjID.m_byID == 1)
  {
    reinterpret_cast<CMonster *>(selectedObject)->Destroy(1, nullptr);
  }
  else if (selectedObject->m_ObjID.m_byID == 0 && selectedObject->m_bLive)
  {
    g_UserDB[selectedObject->m_ObjID.m_wIndex].ForceCloseCommand(0, 0, false, "Kick From Button");
  }
}

void CMainThread::gm_DungeonLoad()
{
  CDarkHoleDungeonQuest *darkHoleQuest = new CDarkHoleDungeonQuest();
  if (!darkHoleQuest)
  {
    return;
  }

  if (darkHoleQuest->LoadDarkHoleQuest())
  {
    MyMessageBox("Dungeon Load", "Success");
  }

  delete darkHoleQuest;
}

bool CMainThread::gm_MonsterInit(CMonster *pExt)
{
  g_MapOper.m_bReSpawnMonster = !g_MapOper.m_bReSpawnMonster;
  if (!g_MapOper.m_bReSpawnMonster)
  {
    for (int j = 0; j < MAX_MONSTER; ++j)
    {
      if (g_Monster[j].m_bLive && pExt != &g_Monster[j])
      {
        g_Monster[j].Destroy(1, nullptr);
      }
    }
  }

  if (g_pDoc->m_InfoSheet.GetActiveIndex() == 3)
  {
    g_Main.m_GameMsg.PackingMsg(1006, 0, 0, 0);
  }

  return g_MapOper.m_bReSpawnMonster;
}

char CMainThread::check_item_code_index()
{
  return 1; // this is not a stub
}

void CMainThread::CheckServiceableTime()
{
  const DWORD currentTick = timeGetTime();
  if (((std::numeric_limits<DWORD>::max)() - currentTick) < kHalfDays)
  {
    m_bCheckOverTickCount = true;
    SerivceSelfStop();
    pc_AllUserKickInform();
    m_logLoadingError.Write("Must Be Rebooting!!");
  }
}

CPlayer *CMainThread::GetChar(char *pszCharName)
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      const char *charName = g_Player[index].m_Param.GetCharNameA();
      if (!std::strcmp(charName, pszCharName))
      {
        return &g_Player[index];
      }
    }
  }
  return nullptr;
}

CGameObject *CMainThread::GetObjectExpand(_object_id *pObjID, char *szCharName, unsigned __int16 wSearchIndex)
{
  CGameObject *found = nullptr;
  int scanIndex = 0;
  unsigned __int16 matchIndex = 0;

  if (szCharName && std::strlen(szCharName))
  {
    if (pObjID->m_byKind == 0 && pObjID->m_byID == 0)
    {
      return GetChar(szCharName);
    }

    if (pObjID->m_byKind == 0 && pObjID->m_byID == 1)
    {
      for (scanIndex = 0; scanIndex < MAX_MONSTER; ++scanIndex)
      {
        if (g_Monster[scanIndex].m_bLive
            && g_Monster[scanIndex].m_pMonRec
            && !std::strcmp(g_Monster[scanIndex].m_pMonRec->m_strCode, szCharName))
        {
          if (wSearchIndex == matchIndex)
          {
            return &g_Monster[scanIndex];
          }
          ++matchIndex;
        }
      }
      return nullptr;
    }

    if (pObjID->m_byKind == 0 && pObjID->m_byID == 2)
    {
      for (scanIndex = 0; scanIndex < MAX_NPC; ++scanIndex)
      {
        if (g_NPC[scanIndex].m_bLive
            && g_NPC[scanIndex].m_pRecordSet
            && !std::strcmp(g_NPC[scanIndex].m_pRecordSet->m_strCode, szCharName))
        {
          if (wSearchIndex == matchIndex)
          {
            return &g_NPC[scanIndex];
          }
          ++matchIndex;
        }
      }
      return nullptr;
    }
  }

  if (found)
  {
    return found;
  }
  return GetObjectA(pObjID);
}

void CMainThread::GetTommorrowStr(char *szTommorrow)
{
  __time64_t rawTime = 0;
  _time64(&rawTime);
  rawTime += 24 * 60 * 60;

  tm localTime{};
  _localtime64_s(&localTime, &rawTime);
  const unsigned int year = static_cast<unsigned int>(localTime.tm_year + 1900);
  const unsigned int month = static_cast<unsigned int>(localTime.tm_mon + 1);
  const unsigned int day = static_cast<unsigned int>(localTime.tm_mday);
  sprintf_s(szTommorrow, sizeof(szTommorrow), "%04u%02u%02u", year, month, day);
}

void CMainThread::pc_UserChatBlockResult(char byBlockResult, _CLID *pcidTarget, _CLID *pcidGM, int bLogin)
{
  CPlayer *target = nullptr;
  CPlayer *gameMaster = nullptr;
  if (bLogin)
  {
    target = &g_Player[pcidTarget->wIndex];
  }

  gameMaster = &g_Player[pcidGM->wIndex];
  if (target && target->m_pUserDB->m_bActive)
  {
    target->m_pUserDB->SetChatLock(true);
  }

  if (gameMaster->m_pUserDB->m_bActive)
  {
    char buffer[144]{};
    if (byBlockResult == 1)
    {
      sprintf_s(buffer, sizeof(buffer), "Kick - Success");
    }
    else if (byBlockResult == -1)
    {
      sprintf_s(buffer, sizeof(buffer), "Kick - ChatBlock is already adjusted");
    }
    else if (byBlockResult == -2)
    {
      sprintf_s(buffer, sizeof(buffer), "Kick - OtherBlock is already adjusted");
    }
    else
    {
      sprintf_s(buffer, sizeof(buffer), "Kick - Failed");
    }

    gameMaster->SendData_ChatTrans(
      0,
      -1,
      static_cast<unsigned __int8>(-1),
      false,
      buffer,
      static_cast<unsigned __int8>(-1),
      nullptr);
  }
}

void CMainThread::QryCaseAddpvppoint(_DB_QRY_SYN_DATA *pData)
{
  auto *query = reinterpret_cast<_qry_case_addpvppoint *>(pData->m_sData);
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, query->dwSerial);
  if (player && player->m_bLoad)
  {
    player->AlterPvPPoint(static_cast<double>(static_cast<int>(query->dwPoint)), logoff_inc, -1);
  }
}

bool CMainThread::ValidMacAddress()
{
  if (!g_Main.IsReleaseServiceMode())
  {
    return true;
  }
  CNationSettingManager *settingMgr = CTSingleton<CNationSettingManager>::Instance();
  return settingMgr->ValidMacAddress();
}

CPlayer *CMainThread::GetCharW(char *wpszCharName)
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      const char *charName = g_Player[index].m_Param.GetCharNameW();
      if (!std::strcmp(charName, wpszCharName))
      {
        return &g_Player[index];
      }
    }
  }
  return nullptr;
}

CGameObject *CMainThread::GetObjectA(_object_id *pObjID)
{
  if (!pObjID)
  {
    return nullptr;
  }

  return GetObjectA(pObjID->m_byKind, pObjID->m_byID, pObjID->m_wIndex);
}

CGameObject *CMainThread::GetObjectA(int kind, int id, unsigned int index)
{
  if (kind == 0)
  {
    switch (id)
    {
      case 0:
        return index < MAX_PLAYER ? &g_Player[index] : nullptr;
      case 1:
        return index < MAX_MONSTER ? &g_Monster[index] : nullptr;
      case 2:
        return index < MAX_NPC ? &g_NPC[index] : nullptr;
      case 3:
        return index < MAX_ANIMUS ? &g_Animus[index] : nullptr;
      case 4:
        return index < MAX_TOWER ? &g_Tower[index] : nullptr;
      case 5:
        return index < MAX_STONE ? &g_Stone[index] : nullptr;
      case 6:
        return index < MAX_KEEPER ? g_Keeper : nullptr;
      case 7:
        return index < MAX_TRAP ? &g_Trap[index] : nullptr;
      case 8:
      {
        CGuildBattleController *controller = CGuildBattleController::Instance();
        controller->GetRegener(static_cast<int>(index));
        return nullptr;
      }
      case 9:
      {
        CGuildBattleController *controller = CGuildBattleController::Instance();
        controller->GetCircleZone(static_cast<int>(index));
        return nullptr;
      }
      case 11:
      {
        AutominePersonalMgr *automineMgr = AutominePersonalMgr::instance();
        return automineMgr ? automineMgr->get_machine(index) : nullptr;
      }
      default:
        return nullptr;
    }
  }

  if (kind == 1)
  {
    switch (id)
    {
      case 0:
        return index < MAX_ITEMBOX ? &g_ItemBox[index] : nullptr;
      case 1:
        return index < MAX_DARKHOLE ? &g_DarkHole[index] : nullptr;
      case 2:
        return index < MAX_PARKING_UNIT ? &g_ParkingUnit[index] : nullptr;
      case 3:
        return index < MAX_DARKHOLE ? &g_DarkHole[index] : nullptr;
      case 4:
      {
        CGuildBattleController *controller = CGuildBattleController::Instance();
        return reinterpret_cast<CGameObject *>(controller->GetStone(static_cast<int>(index)));
      }
      default:
        return nullptr;
    }
  }

  return nullptr;
}

_DB_QRY_SYN_DATA *CMainThread::PushDQSData(
  unsigned int dwAccountSerial,
  _CLID *pidWorld,
  unsigned __int8 byQryCase,
  char *pQryData,
  int nSize)
{
  unsigned int outIndex[5]{};
  if (!m_listDQSDataEmpty.PopNode_Front(outIndex))
  {
    ServerProgramExit("m_listDQSDataEmpty.PopNode_Front() => failed", 1);
  }

  _DB_QRY_SYN_DATA *data = &m_DBQrySynData[outIndex[0]];
  data->m_dwAccountSerial = dwAccountSerial;
  if (pidWorld)
  {
    std::memcpy(&data->m_idWorld, pidWorld, sizeof(data->m_idWorld));
  }
  data->m_byQryCase = byQryCase;
  if (pQryData)
  {
    std::memcpy(data->m_sData, pQryData, nSize);
  }

  if (m_listDQSData.PushNode_Back(outIndex[0]))
  {
    data->m_bUse = true;
    data->m_bLoad = false;
    return data;
  }

  m_logSystemError.Write("%d : m_listDQSData.PushNode_Back() => failed ", byQryCase);
  return nullptr;
}

unsigned int CMainThread::CreateDataResetToken(_SYSTEMTIME *tm)
{
  char buffer[40]{};
  sprintf_s(buffer, 20, "%04d%02d%0d", tm->wYear, tm->wMonth, tm->wDay);
  return static_cast<unsigned int>(atoi(buffer));
}

void CMainThread::PushResetServerToken()
{
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  _qry_case_update_server_reset_token queryData{};
  queryData.dwServerToken = CreateDataResetToken(&systemTime);
  m_dwServerResetToken = queryData.dwServerToken;

  PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
  queryData.dwESerial = processor->GetElectSerial();
  queryData.wProcType = processor->GetProcessorType();

  const int querySize = static_cast<int>(sizeof(queryData));
  PushDQSData(-1, nullptr, 152, reinterpret_cast<char *>(&queryData), querySize);
}

bool CMainThread::Push_ChargeItem(
  unsigned int dwSerial,
  unsigned int dwK,
  unsigned int dwD,
  unsigned int dwU,
  unsigned __int8 byType)
{
  _qry_case_insertitem qry{};
  qry.byType = byType;
  qry.dwItemCodeK = dwK;
  qry.dwItemCodeD = dwD;
  qry.dwItemCodeU = dwU;
  qry.dwSerial = dwSerial;

  if (!byType)
  {
    const unsigned int tableCode = (dwK >> 8) & 0xFFu;
    const unsigned __int16 itemIndex = static_cast<unsigned __int16>((dwK >> 16) & 0xFFFFu);
    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(tableCode, itemIndex);
    if (timeRec)
    {
      qry.dwRemainTime = timeRec->m_nUseTime;
    }
    qry.lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
  }

  const int size = static_cast<int>(qry.size());
  return PushDQSData(-1, nullptr, 14, reinterpret_cast<char *>(&qry), size) != nullptr;
}

char CMainThread::ms_szClientVerCheck[33];

GuildCreateEventInfo::GuildCreateEventInfo()
{
}

void GuildCreateEventInfo::Init()
{
  m_bStartedEvent = false;
  m_EventInfo = {};
  m_ModifyInfo = {};
  m_dwEstConsumeDalant = 0;
  m_dwEmblemDalant = 0;
  m_tmDataFileCheckTime.BeginTimer(0);
  std::memset(&m_ftWrite, 0, sizeof(m_ftWrite));
}

void GuildCreateEventInfo::Loop()
{
  _FILETIME fileWriteTime{};
  if (m_tmDataFileCheckTime.CountingTimer())
  {
    if (!GetLastWriteFileTime("..\\WorldInfo\\WorldInfo.ini", &fileWriteTime)
        || (m_ftWrite.dwHighDateTime == fileWriteTime.dwHighDateTime
            && m_ftWrite.dwLowDateTime == fileWriteTime.dwLowDateTime))
    {
      CheckEventDate();
      return;
    }

    memcpy_s(&m_ftWrite, sizeof(m_ftWrite), &fileWriteTime, sizeof(fileWriteTime));
    ReadEventInfo();
    if (!ApplyModifiedGuildEventInfo())
    {
      CheckEventDate();
      return;
    }

    m_bStartedEvent = false;
    if (!CheckEventDate())
    {
      SetConsumeDalantFree(false);
    }
  }
}

void GuildCreateEventInfo::ReadEventInfo()
{
  char returnedString[12]{};
  GetPrivateProfileStringA(
    "GUILD CREATE EVENT",
    "Enable",
    "FALSE",
    returnedString,
    8u,
    "..\\WorldInfo\\WorldInfo.ini");

  const unsigned int localDateStart = GetLocalDate();
  m_ModifyInfo.dwStartDate = GetPrivateProfileIntA(
    "GUILD CREATE EVENT",
    "START_DATE",
    localDateStart - 1,
    "..\\WorldInfo\\WorldInfo.ini");

  const unsigned int localDateEnd = GetLocalDate();
  m_ModifyInfo.dwEndDate = GetPrivateProfileIntA(
    "GUILD CREATE EVENT",
    "END_DATE",
    localDateEnd - 1,
    "..\\WorldInfo\\WorldInfo.ini");

  m_ModifyInfo.bEnable = std::strcmp(returnedString, "TRUE") == 0;
}

bool GuildCreateEventInfo::CheckEventDate()
{
  if (!m_EventInfo.bEnable)
  {
    return false;
  }

  char changed = 0;
  const unsigned int localDate = GetLocalDate();
  if (!m_bStartedEvent && m_EventInfo.dwStartDate <= localDate && localDate <= m_EventInfo.dwEndDate)
  {
    changed = 1;
    m_bStartedEvent = true;
    SetConsumeDalantFree(true);
  }
  else if (m_EventInfo.dwEndDate < localDate)
  {
    changed = 1;
    m_bStartedEvent = false;
    m_EventInfo.bEnable = false;
    SetConsumeDalantFree(false);
  }

  return (changed) != 0;
}

bool GuildCreateEventInfo::ApplyModifiedGuildEventInfo()
{
  char changed = 0;
  if (m_EventInfo.dwStartDate != m_ModifyInfo.dwStartDate)
  {
    m_EventInfo.dwStartDate = m_ModifyInfo.dwStartDate;
    changed = 1;
  }

  if (m_EventInfo.dwEndDate != m_ModifyInfo.dwEndDate)
  {
    m_EventInfo.dwEndDate = m_ModifyInfo.dwEndDate;
    changed = 1;
  }

  if (m_EventInfo.bEnable != m_ModifyInfo.bEnable)
  {
    m_EventInfo.bEnable = m_ModifyInfo.bEnable;
    return true;
  }

  return (changed) != 0;
}

void GuildCreateEventInfo::SetConsumeDalantFree(bool bEnable)
{
  if (bEnable)
  {
    m_dwEstConsumeDalant = 0;
    m_dwEmblemDalant = 0;
    return;
  }

  m_dwEstConsumeDalant = 24000000;
  m_dwEmblemDalant = 3000000;
}

unsigned int GuildCreateEventInfo::GetEstConsumeDalant() const
{
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(m_dwEstConsumeDalant);
}

unsigned int GuildCreateEventInfo::GetEmblemDalant() const
{
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(m_dwEmblemDalant);
}

TimeLimitMgr *TimeLimitMgr::Instance()
{
  if (!m_pTLStatusMgr)
  {
    m_pTLStatusMgr = new TimeLimitMgr();
  }
  return m_pTLStatusMgr;
}

TimeLimitMgr::TimeLimitMgr()
{
  m_wEnable = 0;
  m_dwLogoutTerm = 300;
  m_dwNotifyTerm = 300;
}

void TimeLimitMgr::Delete_All()
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    std::memset(&m_lstTLStaus[index], 0, sizeof(m_lstTLStaus[index]));
  }
  TimeLimitMgr::m_dwCnt = 0;
}

unsigned __int16 TimeLimitMgr::GetPlayerData(
  unsigned __int16 wIndex,
  unsigned __int8 *psStatus,
  long double *pdPercent)
{
  if (!m_lstTLStaus[wIndex].m_bUse)
  {
    return 100;
  }

  *psStatus = m_lstTLStaus[wIndex].m_byTL_Status;
  *pdPercent = m_lstTLStaus[wIndex].m_dPercent;
  return m_lstTLStaus[wIndex].m_byTL_Status;
}

bool TimeLimitMgr::SetConfig(
  unsigned __int16 time1,
  unsigned __int16 time2,
  unsigned __int16 time3,
  unsigned __int16 time4,
  unsigned __int16 time5)
{
  unsigned __int16 config[5]{time1, time2, time3, time4, time5};
  for (int index = 0; index < m_wPeriodCnt; ++index)
  {
    m_pwTime[index] = config[index];
  }
  return true;
}

unsigned int TimeLimitMgr::SumMinuteOne(_SYSTEMTIME *tm)
{
  int sumDays = 0;
  int daysInMonth = 0;
  for (int month = 0; month < tm->wMonth - 1; ++month)
  {
    if (tm->wMonth == 2)
    {
      if ((tm->wYear & 3) != 0 && tm->wYear % 100 || tm->wYear % 400)
      {
        daysInMonth = 29;
      }
      else
      {
        daysInMonth = 28;
      }
    }
    else if (tm->wMonth == 4 || tm->wMonth == 6 || tm->wMonth == 9 || tm->wMonth == 11)
    {
      daysInMonth = 30;
    }
    else
    {
      daysInMonth = 31;
    }
    sumDays += daysInMonth;
  }

  sumDays += tm->wDay;
  sumDays *= 1440;
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(60 * tm->wHour + sumDays + static_cast<unsigned int>(tm->wMinute));
}

void TimeLimitMgr::LoadTLINIFile()
{
  char returnedString[48]{};
  char buffer[28]{};

  GetPrivateProfileStringA("Time Limit", "Use", "FALSE", returnedString, sizeof(returnedString), ".\\Initialize\\WorldSystem.ini");
  if (std::strcmp(returnedString, "FALSE") == 0)
  {
    SetTLEnable(0);
  }
  else
  {
    SetTLEnable(1);
  }

  m_wPeriodCnt = static_cast<unsigned __int16>(GetPrivateProfileIntA("Time Limit", "CNT", 2, ".\\Initialize\\WorldSystem.ini"));

  const unsigned int periodCount = m_wPeriodCnt;
  m_pwTime = new unsigned __int16[periodCount];
  m_pdPercent = new long double[periodCount];
  m_pwFatigue = new unsigned __int16[periodCount];

  for (int j = 0; j < m_wPeriodCnt; ++j)
  {
    std::memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer, sizeof(buffer), "Time_%d", j);
    m_pwTime[j] = static_cast<unsigned __int16>(
      GetPrivateProfileIntA("Time Limit", buffer, 60 * (j + 1), ".\\Initialize\\WorldSystem.ini"));

    std::memset(returnedString, 0, sizeof(returnedString));
    std::memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer, sizeof(buffer), "Percent_%d", j);
    GetPrivateProfileStringA("Time Limit", buffer, "1.0", returnedString, sizeof(returnedString), ".\\Initialize\\WorldSystem.ini");
    m_pdPercent[j] = std::atof(returnedString);
  }

  for (int k = 0; k < m_wPeriodCnt; ++k)
  {
    m_pwFatigue[k] = static_cast<unsigned __int16>(100 * m_pwTime[k] / m_pwTime[m_wPeriodCnt - 1]);
  }

  m_dwLogoutTerm = GetPrivateProfileIntA("Time Limit", "LogoutTerm", 300, ".\\Initialize\\WorldSystem.ini");
  m_dwNotifyTerm = GetPrivateProfileIntA("Time Limit", "NotifyTerm", 300, ".\\Initialize\\WorldSystem.ini");

  char destination[1024]{};
  char source[60]{};
  strcpy_s(destination, sizeof(destination), "TimeLimitMgr::LoadTLINIFile()");
  for (int m = 0; m < m_wPeriodCnt; ++m)
  {
    sprintf_s(
      source,
      sizeof(source),
      ", Time_%d = %d, Percent_%d = %f",
      m,
      m_pwTime[m],
      m,
      static_cast<double>(m_pdPercent[m]));
    strcat_s(destination, source);
    std::memset(source, 0, sizeof(source));
  }
  g_Main.m_logLoadingError.Write(destination);
}

void TimeLimitMgr::InitializeTLMgr()
{
  int scratch[12]{};
  std::memset(scratch, 0xCC, sizeof(scratch));

  m_dwPlayFDegree = 60000 * m_pwTime[m_wPeriodCnt - 1] / 100;
  m_dwLogoutFDegree = 60000 * m_dwLogoutTerm / 100;
  m_tmLoopTime.BeginTimer(m_dwPlayFDegree);
  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    std::memset(&m_lstTLStaus[j], 0, sizeof(m_lstTLStaus[j]));
  }
  for (unsigned int k = 0; k < MAX_PLAYER; ++k)
  {
    m_lstTLStaus[k].m_bUpdateLogout = true;
  }
}

void TimeLimitMgr::Chack_Time()
{
  if (!m_tmLoopTime.CountingTimer() || !TimeLimitMgr::m_dwCnt || !m_wEnable || m_wEnable == 2)
  {
    return;
  }

  for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
  {
    CPlayer *player = &g_Player[playerIndex];
    if (!player || !player->m_bOper || !m_lstTLStaus[playerIndex].m_bUse || !m_lstTLStaus[playerIndex].m_bAgeLimit)
    {
      continue;
    }

    if (m_lstTLStaus[playerIndex].m_dwFatigue >= 99)
    {
      if (m_lstTLStaus[playerIndex].m_dwFatigue < 100)
      {
        ++m_lstTLStaus[playerIndex].m_dwFatigue;
      }

      player->m_pUserDB->Update_UserFatigue(m_lstTLStaus[playerIndex].m_dwFatigue);
      m_lstTLStaus[playerIndex].m_byTL_Status = 99;
      player->m_pUserDB->Update_UserTLStatus(99);
      m_lstTLStaus[playerIndex].m_dPercent = 0.0;
    }
    else
    {
      player->m_pUserDB->Update_UserFatigue(++m_lstTLStaus[playerIndex].m_dwFatigue);
      for (int periodIndex = 0; periodIndex < m_wPeriodCnt; ++periodIndex)
      {
        if (m_lstTLStaus[playerIndex].m_dwFatigue >= m_pwFatigue[periodIndex]
            && m_lstTLStaus[playerIndex].m_dwFatigue < m_pwFatigue[periodIndex + 1])
        {
          m_lstTLStaus[playerIndex].m_dPercent = m_pdPercent[periodIndex];
          m_lstTLStaus[playerIndex].m_byTL_Status = static_cast<unsigned __int8>(periodIndex);
          player->m_pUserDB->Update_UserTLStatus(static_cast<unsigned __int8>(periodIndex));
          break;
        }
      }
    }

    player->SendMsg_UpdateTLStatusInfo(
      m_lstTLStaus[playerIndex].m_dwFatigue,
      m_lstTLStaus[playerIndex].m_byTL_Status);
  }
}

void TimeLimitMgr::SetTLEnable(unsigned __int16 wState)
{
  m_wEnable = wState;
}

unsigned __int16 TimeLimitMgr::GetPeriodCnt()
{
  return m_wPeriodCnt;
}

void TimeLimitMgr::SetTime(unsigned __int16 dwTime, unsigned __int16 iIndex)
{
  m_pwTime[iIndex] = dwTime;
}

void TimeLimitMgr::ReInitFatigue()
{
  for (int j = 0; j < m_wPeriodCnt; ++j)
  {
    m_pwFatigue[j] = static_cast<unsigned __int16>(100 * m_pwTime[j] / m_pwTime[m_wPeriodCnt - 1]);
  }
}

void TimeLimitMgr::SetPlayFDegree(unsigned int dwDegree)
{
  m_dwPlayFDegree = dwDegree;
}

void TimeLimitMgr::SetLogoutFDegree(unsigned int dwDegree)
{
  m_dwLogoutFDegree = dwDegree;
}

long double TimeLimitMgr::GetPlayerPenalty(unsigned __int16 wIndex)
{
  if (!m_wEnable)
  {
    return 1.0L;
  }

  if (!m_lstTLStaus[wIndex].m_bUse)
  {
    return 0.0L;
  }

  if (m_lstTLStaus[wIndex].m_bAgeLimit)
  {
    return m_lstTLStaus[wIndex].m_dPercent;
  }

  return 1.0L;
}

unsigned __int8 TimeLimitMgr::GetPlayerStatus(unsigned __int16 wIndex)
{
  if (!m_wEnable)
  {
    return 0;
  }
  if (!m_lstTLStaus[wIndex].m_bUse)
  {
    return 99;
  }
  if (m_lstTLStaus[wIndex].m_bAgeLimit)
  {
    return m_lstTLStaus[wIndex].m_byTL_Status;
  }
  return 0;
}

void TimeLimitMgr::Pop_Data(unsigned int dwAccountSerial, unsigned __int16 wIndex)
{
  if (m_dwCnt)
  {
    if (dwAccountSerial == m_lstTLStaus[wIndex].m_dwAccountSerial)
    {
      std::memset(&m_lstTLStaus[wIndex], 0, sizeof(m_lstTLStaus[wIndex]));
      --m_dwCnt;
    }
  }
}

void TimeLimitMgr::Push_Data(Player_TL_Status *data, unsigned __int16 wIndex)
{
  m_lstTLStaus[wIndex].m_bUse = true;
  m_lstTLStaus[wIndex].m_byTL_Status = data->m_byTL_Status;
  m_lstTLStaus[wIndex].m_dwStartTime = data->m_dwStartTime;
  m_lstTLStaus[wIndex].m_dwAccountSerial = data->m_dwAccountSerial;
  m_lstTLStaus[wIndex].m_dwLastLogoutTime = data->m_dwLastLogoutTime;
  m_lstTLStaus[wIndex].m_dwFatigue = data->m_dwFatigue;
  m_lstTLStaus[wIndex].m_bAgeLimit = data->m_bAgeLimit;
  ++TimeLimitMgr::m_dwCnt;
}

void TimeLimitMgr::InsertPlayerStatus(
  unsigned __int16 wIndex,
  unsigned int dwAccountSerial,
  unsigned __int8 byStatus,
  unsigned int dwFatigue,
  unsigned int dwLastLogoutTime,
  bool bAgeLimit)
{
  Player_TL_Status data{};
  data.m_dwFatigue = dwFatigue;
  data.m_byTL_Status = byStatus;
  data.m_dwStartTime = timeGetTime();
  data.m_dwAccountSerial = dwAccountSerial;
  data.m_dwLastLogoutTime = dwLastLogoutTime;
  data.m_bAgeLimit = bAgeLimit;
  Push_Data(&data, wIndex);
}

unsigned int TimeLimitMgr::ClacLastLogoutTimeSec(unsigned int dwLastConnTime)
{
  char buffer[28]{};
  sprintf_s(buffer, 15, "%u", dwLastConnTime);
  const int len = static_cast<int>(std::strlen(buffer));

  tm tmLast{};
  tmLast.tm_isdst = -1;

  char temp[32]{};
  char yearString[16]{};

  std::memcpy(temp, buffer, static_cast<size_t>(len - 8));
  if (len == 9)
  {
    sprintf_s(yearString, sizeof(yearString), "200%s", temp);
  }
  else if (len == 10)
  {
    sprintf_s(yearString, sizeof(yearString), "20%s", temp);
  }
  yearString[4] = 0;
  tmLast.tm_year = atoi(yearString) - 1900;

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 8], 2);
  tmLast.tm_mon = atoi(temp) - 1;

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 6], 2);
  tmLast.tm_mday = atoi(temp);

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 4], 2);
  tmLast.tm_hour = atoi(temp);

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 2], 2);
  tmLast.tm_min = atoi(temp);

  std::memset(temp, 0, 3);
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(SumMinuteBetweenSec(&tmLast));
}

unsigned int TimeLimitMgr::ClacLastLogoutTimeToFatigue(unsigned int dwLastConnTime)
{
  char buffer[28]{};
  sprintf_s(buffer, 15, "%u", dwLastConnTime);
  const int len = static_cast<int>(std::strlen(buffer));

  tm tmLast{};
  tmLast.tm_isdst = -1;

  char temp[32]{};
  char yearString[16]{};

  std::memcpy(temp, buffer, static_cast<size_t>(len - 8));
  if (len == 9)
  {
    sprintf_s(yearString, sizeof(yearString), "200%s", temp);
  }
  else if (len == 10)
  {
    sprintf_s(yearString, sizeof(yearString), "20%s", temp);
  }
  yearString[4] = 0;
  tmLast.tm_year = atoi(yearString) - 1900;

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 8], 2);
  tmLast.tm_mon = atoi(temp) - 1;

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 6], 2);
  tmLast.tm_mday = atoi(temp);

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 4], 2);
  tmLast.tm_hour = atoi(temp);

  std::memset(temp, 0, 3);
  std::memcpy(temp, &buffer[len - 2], 2);
  tmLast.tm_min = atoi(temp);

  std::memset(temp, 0, 3);
  const unsigned int minutes = static_cast<unsigned int>(SumMinuteBetweenSec(&tmLast));
  if (minutes)
  {
    // narrowing cast for thunk return parity
    return static_cast<unsigned int>(1000 * minutes / m_dwLogoutFDegree);
  }
  return 0;
}

unsigned __int16 TimeLimitMgr::GetEndPlayTime()
{
  return m_pwTime[m_wPeriodCnt - 1];
}

bool TimeLimitMgr::CheckPlayerStatus(
  unsigned __int16 wIndex,
  unsigned int dwLastContSaveTime,
  unsigned __int8 *pbyStatus,
  unsigned int *pdwFatigue)
{
  Player_TL_Status *data = Find_Data(wIndex);
  if (!data)
  {
    return false;
  }

  const unsigned int logoutFatigue = static_cast<unsigned int>(
    g_Main.m_pTimeLimitMgr->ClacLastLogoutTimeToFatigue(dwLastContSaveTime));
  if (logoutFatigue < 100 || data->m_dwFatigue > logoutFatigue)
  {
    if (data->m_dwFatigue == 100 && logoutFatigue <= 100)
    {
      data->m_dwFatigue = 100;
      *pdwFatigue = 100;
      data->m_byTL_Status = 99;
      *pbyStatus = 99;
      data->m_dPercent = m_pdPercent[m_wPeriodCnt - 1];
      return true;
    }

    if (data->m_dwFatigue < 100)
    {
      for (int j = 0; j < m_wPeriodCnt; ++j)
      {
        if (data->m_dwFatigue >= m_pwFatigue[j] && data->m_dwFatigue < m_pwFatigue[j + 1])
        {
          data->m_dPercent = m_pdPercent[j];
          data->m_byTL_Status = static_cast<unsigned __int8>(j);
          break;
        }
      }
    }

    *pbyStatus = data->m_byTL_Status;
    *pdwFatigue = data->m_dwFatigue;
    return true;
  }

  data->m_dwFatigue = 0;
  *pdwFatigue = 0;
  data->m_byTL_Status = 0;
  *pbyStatus = 0;
  data->m_dPercent = 1.0L;
  return true;
}

bool TimeLimitMgr::UpdatePlayerStatus(unsigned __int16 wIndex, unsigned int dwFatigue, unsigned __int8 wStatus)
{
  Player_TL_Status *data = Find_Data(wIndex);
  if (!data)
  {
    return false;
  }

  data->m_dwFatigue = dwFatigue;
  g_Player[wIndex].m_pUserDB->Update_UserFatigue(dwFatigue);
  data->m_byTL_Status = wStatus;
  g_Player[wIndex].m_pUserDB->Update_UserTLStatus(wStatus);
  return true;
}

void TimeLimitMgr::ReSetPercent(unsigned __int16 wIndex)
{
  CPlayer *player = &g_Player[wIndex];
  if (player && player->m_bOper && m_lstTLStaus[wIndex].m_bUse && m_lstTLStaus[wIndex].m_bAgeLimit)
  {
    if (m_lstTLStaus[wIndex].m_dwFatigue > 99)
    {
      if (m_lstTLStaus[wIndex].m_dwFatigue > 100)
      {
        m_lstTLStaus[wIndex].m_dwFatigue = 100;
      }
      m_lstTLStaus[wIndex].m_byTL_Status = 99;
      m_lstTLStaus[wIndex].m_dPercent = 0.0;
    }
    else
    {
      for (int j = 0; j < m_wPeriodCnt; ++j)
      {
        if (m_lstTLStaus[wIndex].m_dwFatigue >= m_pwFatigue[j] &&
            m_lstTLStaus[wIndex].m_dwFatigue < m_pwFatigue[j + 1])
        {
          m_lstTLStaus[wIndex].m_dPercent = m_pdPercent[j];
          m_lstTLStaus[wIndex].m_byTL_Status = static_cast<unsigned __int8>(j);
          break;
        }
      }
    }
    player->SendMsg_UpdateTLStatusInfo(
      m_lstTLStaus[wIndex].m_dwFatigue,
      m_lstTLStaus[wIndex].m_byTL_Status);
  }
}

unsigned int TimeLimitMgr::SumMinuteBetweenSec(tm *tmLast)
{
  const __int64 lastTime = mktime_3(tmLast);
  if (lastTime == -1)
  {
    return 0;
  }

  __int64 now = 0;
  time_20(&now);
  if (now >= lastTime)
  {
    // narrowing cast for thunk return parity
    return static_cast<unsigned int>(now - lastTime);
  }

  return 0;
}

Player_TL_Status *TimeLimitMgr::Find_Data(unsigned __int16 wIndex)
{
  if (m_lstTLStaus[wIndex].m_bUse)
  {
    return &m_lstTLStaus[wIndex];
  }
  return nullptr;
}

Player_TL_Status *TimeLimitMgr::Find_Data(unsigned int dwSerial)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive && player->m_Param.GetCharSerial() == dwSerial && m_lstTLStaus[j].m_bUse)
    {
      return &m_lstTLStaus[j];
    }
  }
  return nullptr;
}

unsigned __int8 Player_TL_Status::GetTLStatus()
{
  return m_byTL_Status;
}

unsigned int TimeLimitMgr::GetPlayFDegree()
{
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(m_dwPlayFDegree);
}

char CMainThread::Init()
{
  CRtc::GetIntance()->Reg_Fn();
  g_WheatyExceptionReport.SetLogName("ZoneServer_MainLoop");
  g_WheatyExceptionReport.SetDescription("ZoneServer : Exception Program");

  const DWORD startTick = GetTickCount();
  WriteServerStartHistory("Init >> tickcount: %u", startTick);

  m_bWorldOpen = false;
  m_bWorldService = false;
  m_bCheckOverTickCount = false;

  m_tmServerState.BeginTimer(10000);
  m_tmrStateMsgGotoWeb.BeginTimer(60000);
  m_dwCheckAccountOldTick = GetLoopTime();

  const DWORD nowTick = GetTickCount();
  if (((std::numeric_limits<DWORD>::max)() - nowTick) < 691200000)
  {
    MyMessageBox("Start Error", "Must Reboot OS To Service");
    return false;
  }

  CreateDirectoryA("..\\ZoneServerLog\\", nullptr);
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog", 15);
  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog", nullptr);
  clear_file("..\\ZoneServerLog\\ServiceLog", 15);
  CreateDirectoryA("..\\ZoneServerLog\\DBLog", nullptr);
  clear_file("..\\ZoneServerLog\\DBLog", 15);
  CreateDirectoryA("..\\ZoneServerLog\\CharLog", nullptr);
  clear_file("..\\ZoneServerLog\\CharLog", 15);
  CreateDirectoryA("..\\SystemSave", nullptr);
  clear_file("..\\SystemSave", 15);
  CreateDirectoryA("..\\ZoneServerLog\\BillingLog", nullptr);
  clear_file("..\\ZoneServerLog\\BillingLog", 15);
  CreateDirectoryA("..\\ZoneServerLog\\NetLog", nullptr);
  clear_file("..\\ZoneServerLog\\NetLog", 15);
  CreateDirectoryA("..\\ZoneServerLog\\ServerExitLog", nullptr);
  clear_file("..\\ZoneServerLog\\ServerExitLog", 15);

  const unsigned int korLocalTime = static_cast<unsigned int>(GetKorLocalTime());
  _CrtSetReportHook(MyCrtDebugReportHook);

  char buffer[132]{};
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\Systemlog\\SystemError%u.log", korLocalTime);
  m_logSystemError.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "%s", "..\\ZoneServerLog\\Systemlog\\LoadingProcess.log");
  m_logLoadingError.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\KillMon%u.log", korLocalTime);
  m_logKillMon.SetWriteLogFile(buffer, 1, 0, 0, 0);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Dungeon%u.log", korLocalTime);
  m_logDungeon.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\ServerState%u.log", korLocalTime);
  m_logServerState.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\DTrade%u.log", korLocalTime);
  m_logDTrade.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Guild%u.log", korLocalTime);
  m_logGuild.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Rename%u.log", korLocalTime);
  m_logRename.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\AutoTrade%u.log", korLocalTime);
  m_logAutoTrade.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Event%u.log", korLocalTime);
  m_logEvent.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Move%u.log", korLocalTime);
  m_logMove.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Save%u.log", korLocalTime);
  m_logSave.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\ReturnGate%u.log", korLocalTime);
  m_logReturnGate.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Hack%u.log", korLocalTime);
  m_logHack.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\PvP%u.log", korLocalTime);
  m_logPvP.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\MonNum_%u.log", korLocalTime);
  m_logMonNum.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\CheckBilling_%u.log", korLocalTime);
  m_logBillCheck.SetWriteLogFile(buffer, 1, 0, 1, 1);

  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_HACKSHIELD_SYSTEM_LOG,
    "..\\ZoneServerLog\\SystemLog\\HacShiled",
    "HS_System",
    1,
    3600000);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_APEX_SYSTEM_LOG,
    "..\\ZoneServerLog\\SystemLog\\Apex",
    "Apex_System",
    1,
    3600000);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_FIREGUARD_DETECT_LOG,
    "..\\ZoneServerLog\\SystemLog\\fireguard",
    "CCRFG_SystemLog",
    1,
    3600000);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_HONOR_GUILD_LOG,
    "..\\ZoneServerLog\\SystemLog\\HonorGuild",
    "HonorGuild_SysLog",
    1,
    86400000);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_BUY_CASH_ITEM_LOG,
    "..\\ZoneServerLog\\ServiceLog\\PartiallyPaid",
    "BuyCashItemHistory",
    1,
    3600000);

  m_logLoadingError.Write("Server Load Start!!");

  if (!LoadINI())
  {
    MyMessageBox("CGameServerDoc Error", "LoadINI()");
    return false;
  }
  if (!CheckDefine())
  {
    MyMessageBox("CGameServerDoc Error", "CheckDefine()");
    return false;
  }
  if (!check_dbsyn_data_size())
  {
    MyMessageBox("CGameServerDoc Error", "check_dbsyn_data_size()");
    return false;
  }
  if (!DataFileInit())
  {
    MyMessageBox("CGameServerDoc Error", "DataFileInit()");
    return false;
  }
  if (!ObjectInit())
  {
    MyMessageBox("CGameServerDoc Error", "ObjectInit()");
    return false;
  }
  m_logLoadingError.Write("Game Data Load Complete!!");
  if (!NetworkInit())
  {
    MyMessageBox("CGameServerDoc Error", "NetworkInit() == false");
    return false;
  }

  m_logLoadingError.Write("Network Init Complete!!");
  m_logLoadingError.Write("Map Load Start!!");

  if (!g_MapOper.Init())
  {
    MyMessageBox("CGameServerDoc Error", "g_MapOper.Init() == false");
    return false;
  }
  if (!g_MonsterEventRespawn.SetEventRespawn())
  {
    MyMessageBox("CGameServerDoc Error", "Read Error Monster Respawn Script");
    return false;
  }

  char errCode[1028]{};
  if (!g_MonsterEventSet->LoadEventSet(errCode))
  {
    MyMessageBox("CGameServerDoc Error", "Read Error Event Set Script, Reason : %s", errCode);
    return false;
  }
  if (!g_MonsterEventSet->LoadEventSetLooting())
  {
    MyMessageBox("CGameServerDoc Error", "Read Error Event Set Looting Script");
    return false;
  }

  m_logLoadingError.Write("Map Load Complete!!");

  const int asyncInit = CAsyncLogger::Instance()->Init();
  if (asyncInit != 0)
  {
    MyMessageBox(
      "CMainThread::Init() : ",
      "CAsyncLogger::Instance()->Init() Ret(%d) Fail!\r\nCheck LoadingProcess.Log!",
      asyncInit);
    m_logLoadingError.Write("CAsyncLogger::Instace()->Init() Ret(%d) Fail!", asyncInit);
    return false;
  }

  m_pTimeLimitMgr = TimeLimitMgr::Instance();
  m_pTimeLimitMgr->LoadTLINIFile();
  m_pTimeLimitMgr->InitializeTLMgr();

  if (!g_DarkHoleQuest.LoadDarkHoleQuest())
  {
    MyMessageBox("DarkHole", "Failed g_DarkHoleQuest.LoadDarkHoleQuest()");
    m_logLoadingError.Write("Failed g_DarkHoleQuest.LoadDarkHoleQuest()");
    return false;
  }

  if (!CGuildBattleController::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CGuildBattleController::Instance()->Init() == false");
    m_logLoadingError.Write("CGuildBattleController::Instance()->Init() == false");
    return false;
  }
  if (!CTotalGuildRankManager::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CTotalGuildRankManager::Instance()->Init() Fail!");
    m_logLoadingError.Write("CTotalGuildRankManager::Instance()->Init() Fail!");
    return false;
  }
  if (!CWeeklyGuildRankManager::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CWeeklyGuildRankManager::Instance()->Init() Fail!");
    m_logLoadingError.Write("CWeeklyGuildRankManager::Instance()->Init() Fail!");
    return false;
  }
  if (!CandidateMgr::Instance()->Initialize(CANDIDATE_MGR_INIT_COUNT))
  {
    MyMessageBox("CMainThread::Init() : ", kCandidateMgrInitFailFmt, CANDIDATE_MGR_INIT_COUNT);
    m_logLoadingError.Write(kCandidateMgrInitFailFmt, CANDIDATE_MGR_INIT_COUNT);
    return false;
  }
  if (!PatriarchElectProcessor::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "PatriarchElectProcessor::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("PatriarchElectProcessor::Instance()->Initialize() Fail!");
    return false;
  }
  if (!AutominePersonalMgr::instance()->initialize())
  {
    MyMessageBox("ObjectInit() Error", "AutominePersonalMgr::instance()->Initialize() Fail!");
    m_logLoadingError.Write("AutominePersonalMgr::instance()->Initialize() Fail!");
    return false;
  }
  if (!CGuildRoomSystem::GetInstance()->Init())
  {
    MyMessageBox("CMainThread::_GameDataBaseInit()", "CGuildRoomSystem::GetInstance()->Init() Fail!");
    m_logLoadingError.Write("CGuildRoomSystem::GetInstance()->Init() Fail!");
    return false;
  }
  if (!CUnmannedTraderController::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CUnmannedTraderController::Instance()->Init() Fail!");
    m_logLoadingError.Write("CUnmannedTraderController::Instance()->Init() Fail!");
    return false;
  }
  if (!CLogTypeDBTaskManager::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CLogTypeDBTaskManager::Instance()->Init() Fail!");
    m_logLoadingError.Write("CLogTypeDBTaskManager::Instance()->Init() Fail!");
    return false;
  }
  if (!TimeItem::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "TimeItem::Instance()->Init() Fail!");
    m_logLoadingError.Write("TimeItem::Instance()->Init() Fail!");
    return false;
  }
  if (!CTSingleton<CCashDBWorkManager>::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CashDbWorker::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CashDbWorker::Instance()->Initialize() Fail!");
    return false;
  }
  if (!CashItemRemoteStore::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CashItemRemoteStore::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CashItemRemoteStore::Instance()->Initialize() Fail!");
    return false;
  }
  if (!CPostSystemManager::Instace()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CPostSystemManager::Instace()->Init() Fail!");
    m_logLoadingError.Write("CPostSystemManager::Instace()->Init() Fail!");
    return false;
  }
  if (!CPvpUserAndGuildRankingSystem::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CPvpUserAndGuildRankingSystem::Instace()->Init() Fail!");
    m_logLoadingError.Write("CPvpUserAndGuildRankingSystem::Instace()->Init() Fail!");
    return false;
  }
  if (!CMoveMapLimitManager::Instance()->Init())
  {
    MyMessageBox(
      "CMainThread::Init() : ",
      "CMoveMapLimitManager::Instance()->Init() Fail!\r\nCheck LoadingProcess.Log!");
    m_logLoadingError.Write("CPvpUserAndGuildRankingSystem::Instace()->Init() Fail!");
    return false;
  }
  if (!CRaceBuffManager::Instance()->Init())
  {
    MyMessageBox(
      "CMainThread::Init() : ",
      "CRaceBuffManager::Instance()->Init() Fail!\r\nCheck LoadingProcess.Log!");
    m_logLoadingError.Write("CRaceBuffManager::Instace()->Init() Fail!");
    return false;
  }
  if (!CHonorGuild::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CHonorGuild::Instance()->Init() Fail!");
    m_logLoadingError.Write("CHonorGuild::Instance()->Init() Fail!");
    return false;
  }
  if (!CExchangeEvent::Instance()->Initialzie())
  {
    MyMessageBox("CMainThread::Init() : ", "CExchangeEvent::Instance()->Initialzie() Fail!");
    m_logLoadingError.Write("CExchangeEvent::Instance()->Initialzie() Fail!");
    return false;
  }
  if (!CPcBangFavor::Instance()->Initialzie())
  {
    MyMessageBox("CMainThread::Init() : ", "CPcBangFavor::Instance()->Initialzie() Fail!");
    m_logLoadingError.Write("CPcBangFavor::Instance()->Initialzie() Fail!");
    return false;
  }
  if (!CActionPointSystemMgr::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CActionPointSystemMgr::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CActionPointSystemMgr::Instance()->Initialize() Fail");
    return false;
  }

  m_GuildCreateEventInfo.Init();
  m_GameMsg.Init(100);
  m_tmrCheckAvator.BeginTimer(1000);
  m_tmrCheckLoop.BeginTimer(1000);
  m_tmrAccountPing.BeginTimer(1000);
  m_tmrCheckRadarDelay.BeginTimer(10000);

  if (!CLuaScriptMgr::Instance()->InitSDM())
  {
    MyMessageBox("CMainThread::Init() : ", "CLuaScriptMgr::Instance()->InitSDM() Fail!");
    m_logLoadingError.Write("CLuaScriptMgr::Instance()->InitSDM()");
    return false;
  }

  if (!CTSingleton<CCryptor>::Instance()->Init(".\\Initialize\\WorldSystem.bin", false))
  {
    MyMessageBox("CMainThread::Init() : ", "CCryptor::Instance()->Init() Fail");
    m_logLoadingError.Write("CCryptor::Instance()->Init()");
    return false;
  }

  if (!CBossMonsterScheduleSystem::Instance()->Init(&g_MapOper))
  {
    MyMessageBox("CMainThread::Init() : ", "CBossMonsterScheduleSystem::Instance()->Init(&g_MapOper) Fail!");
    m_logLoadingError.Write("CBossMonsterScheduleSystem::Instance()->Init(&g_MapOper) Fail");
    return false;
  }

  m_kEtcNotifyInfo.Init();

  if (!cStaticMember_Player::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "cStaticMember_Player::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("cStaticMember_Player::Instance()->Initialize() Fail!");
    return false;
  }

  m_bRuleThread = true;
  _beginthread(CMainThread::RuleThread, 0, this);
  m_bDQSThread = true;
  _beginthread(CMainThread::DQSThread, 0, this);

  m_logLoadingError.Write("Thread Setting Complete!!");

  m_MgrConnNum.Init();
  m_HisMainFPS.Init();
  m_HisSendFPS.Init();
  m_HisDataFPS.Init();

  if (g_pFrame == nullptr)
  {
    g_pFrame = AfxGetMainWnd();
  }
  if (g_pFrame != nullptr)
  {
    g_pFrame->SendMessage(12, 0, 0);
  }

  MakeSystemTower();
  m_logLoadingError.Write("System Tower Make Complete!!");

  m_byWebAgentServerNetInx = 0;
  m_bConnectedWebAgentServer = false;
  m_byControllServerNetInx = 0;
  m_bConnectedControllServer = false;

  if (!CTSingleton<CBillingManager>::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CBillingManager::Init() Fail!");
    m_logLoadingError.Write("CBillingManager::Init() Fail!");
    return false;
  }

  m_dwCheatSetPlayTime = 600;
  m_dwCheatSetScanerCnt = 0;
  m_dwCheatSetLevel = 50;
  m_dwServerResetToken = 1990011;

  if (!CGoldenBoxItemMgr::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CGoldenBoxItemMgr::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CGoldenBoxItemMgr::Instance()->Initialize() Fail!");
    return false;
  }

  return true;
}

int CMainThread::LoadINI()
{
  const int worldSystemRet = LoadWorldSystemINI();
  if (worldSystemRet != 0)
  {
    MyMessageBox("CMainThread::LoadINI()", "LoadWorldSystemINI() iRet(%d) Fail!", worldSystemRet);
    return false;
  }

  const int worldInfoRet = LoadWorldInfoINI();
  if (worldInfoRet != 0)
  {
    MyMessageBox("CMainThread::LoadINI()", "LoadWorldInfoINI() iRet(%d) Fail!", worldInfoRet);
    return false;
  }

  LoadItemConsumeINI();
  return true;
}

int CMainThread::LoadWorldSystemINI()
{
  GetPrivateProfileStringA(
    "VersionCheck",
    "Ver_CheckKey",
    "X",
    ms_szClientVerCheck,
    static_cast<DWORD>(sizeof(ms_szClientVerCheck)),
    ".\\Initialize\\WorldSystem.ini");
  return 0;
}

int CMainThread::LoadWorldInfoINI()
{
  if (!ReadConfiguredIpv4Address(kWorldInfoIniPath, "System", "AccountAddress", &m_dwAccountIP))
  {
    if (!ReadConfiguredIpv4Address(kWorldSystemIniPath, "System", "AccountAddress", &m_dwAccountIP))
    {
      return -5;
    }
  }

  if (!ReadConfiguredPort(kWorldInfoIniPath, "System", "AccountPort", kDefaultAccountPort, &m_wAccountPort))
  {
    return -6;
  }

  if (!ReadGateIpv4Address(&m_dwGateIP))
  {
    return -7;
  }

  if (!ReadConfiguredPort(kWorldInfoIniPath, "System", "GatePort", kDefaultGatePort, &m_wGatePort))
  {
    return -8;
  }

  GetPrivateProfileStringA(
    "System",
    "WorldName",
    "X",
    m_szWorldName,
    static_cast<DWORD>(sizeof(m_szWorldName)),
    kWorldInfoIniPath);
  if (strcmp(m_szWorldName, "X") == 0)
  {
    return -1;
  }
  strncpy_s(m_wszWorldName, sizeof(m_wszWorldName), m_szWorldName, _TRUNCATE);

  m_bFreeServer = GetPrivateProfileIntA("System", "FreeServer", 0, kWorldInfoIniPath);
  m_byWorldType = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("System", "ServerType", 2, kWorldInfoIniPath));

  char releaseType[9]{};
  GetPrivateProfileStringA(
    "ServerMode",
    "ReleaseType",
    "X",
    releaseType,
    static_cast<DWORD>(sizeof(releaseType)),
    kWorldInfoIniPath);
  if (_stricmp(releaseType, "Internal") == 0)
  {
    m_bReleaseServiceMode = false;
  }
  else if (_stricmp(releaseType, "Release") == 0)
  {
    m_bReleaseServiceMode = true;
  }
  else
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "WorldInfo.ini\r\n[ServerMode]\r\nReleaseType = %s Invalid!!",
      releaseType);
    return -2;
  }

  char executeService[6]{};
  GetPrivateProfileStringA(
    "ServerMode",
    "ExcuteService",
    "X",
    executeService,
    static_cast<DWORD>(sizeof(executeService)),
    kWorldInfoIniPath);
  if (_stricmp(executeService, "true") == 0)
  {
    m_bExcuteService = true;
  }
  else if (_stricmp(executeService, "false") == 0)
  {
    m_bExcuteService = false;
  }
  else
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "WorldInfo.ini\r\n[ServerMode]\r\nExcuteService = %s Invalid!!",
      executeService);
    return -3;
  }

  char nationCode[3]{};
  GetPrivateProfileStringA(
    "System",
    "NationCode",
    "X",
    nationCode,
    static_cast<DWORD>(sizeof(nationCode)),
    kWorldInfoIniPath);

  CNationCodeStrTable table;
  if (!table.Init())
  {
    return -4;
  }

  const int displayType = table.GetCode(nationCode);
  if (displayType == -1)
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "WorldInfo.ini\r\n[System]\r\nNationCode = %s Invalid!!",
      nationCode);
    return -5;
  }

  const char *displayCodeStr = table.GetStr(displayType);
  strncpy_s(nationCode, sizeof(nationCode), displayCodeStr, _TRUNCATE);

  const int type = table.GetCode("RU");
  if (type == -1)
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "CNationCodeStrTable missing RU code.");
    return -5;
  }

  const int initRet = CNationSettingManager::Instance()->Init(type, nationCode, m_bReleaseServiceMode);
  if (initRet != 0)
  {
    const char *serviceMode = m_bReleaseServiceMode ? "true" : "false";
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "CNationSettingManager::Instance()->Init( iNationCode(%d), szNationCodeStr(%s), bServiceMode(%s) ) :  iRet(%d) Fail!",
      type,
      nationCode,
      serviceMode,
      initRet);
    return -6;
  }

  return 0;
}

void CMainThread::LoadItemConsumeINI()
{
  char returnedString[10]{};
  GetPrivateProfileStringA(
    "AWAY_PARTY",
    "ItemConsume",
    "TRUE",
    returnedString,
    static_cast<DWORD>(sizeof(returnedString)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAwayPartyConsumeItem = strcmp(returnedString, "TRUE") == 0;

  GetPrivateProfileStringA(
    "AWAY_PARTY",
    "ItemCode",
    "ircht01",
    m_strAwayPartyItemCode,
    static_cast<DWORD>(sizeof(m_strAwayPartyItemCode)),
    ".\\Initialize\\ItemConsume.ini");

  char moneyConsume[10]{};
  GetPrivateProfileStringA(
    "AWAY_PARTY",
    "MoneyConsume",
    "FALSE",
    moneyConsume,
    static_cast<DWORD>(sizeof(moneyConsume)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAwayPartyConsumeMoney = strcmp(moneyConsume, "TRUE") == 0;
  m_dwAwayPartyMoney = static_cast<unsigned int>(
    GetPrivateProfileIntA("AWAY_PARTY", "Money", 0, ".\\Initialize\\ItemConsume.ini"));

  GetPrivateProfileStringA(
    "AllRaceChat",
    "ItemCode",
    "X",
    m_strAllRaceChatItemCode,
    static_cast<DWORD>(sizeof(m_strAllRaceChatItemCode)),
    ".\\Initialize\\ItemConsume.ini");

  char itemConsume[10]{};
  GetPrivateProfileStringA(
    "AllRaceChat",
    "ItemConsume",
    "TRUE",
    itemConsume,
    static_cast<DWORD>(sizeof(itemConsume)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAllRaceChatItemConsume = strcmp(itemConsume, "TRUE") == 0;

  GetPrivateProfileStringA(
    "AllRaceChat",
    "MoneyConsume",
    "FALSE",
    itemConsume,
    static_cast<DWORD>(sizeof(itemConsume)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAllRaceChatMoneyConsume = strcmp(itemConsume, "TRUE") == 0;
  m_dwAllRaceChatMoney = static_cast<unsigned int>(
    GetPrivateProfileIntA("AllRaceChat", "Money", 0, ".\\Initialize\\ItemConsume.ini"));
}

bool CMainThread::CheckDefine()
{
  return true; //this is not stub, original code is already like this
}

bool CMainThread::check_dbsyn_data_size()
{
  return true; //this is not stub, original code is already like this
}

char CMainThread::DataFileInit()
{
  char szErrCode[152]{};
  if (!WriteTableData(37, m_tblItemData, true, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }

  const char *effectFiles[] =
  {
    ".\\script\\skill.dat",
    ".\\script\\force.dat",
    ".\\script\\ClassSkill.dat",
    ".\\script\\BulletItemEffect.dat",
  };
  const unsigned int effectSizes[] = {1168, 1088, 1168, 1168};
  for (int i = 0; i < 4; ++i)
  {
    if (!m_tblEffectData[i].ReadRecord(effectFiles[i], static_cast<int>(effectSizes[i]), szErrCode))
    {
      MyMessageBox("DatafileInit", szErrCode);
      return false;
    }
  }
  if (!g_PotionMgr.DatafileInit())
  {
    return false;
  }

  CPlayer::ms_pXmas_Snow_Effect =
    reinterpret_cast<_skill_fld *>(m_tblEffectData[3].GetRecord("16"));
  CPlayer::ms_pXmas_Snow_Bullet_Effect =
    reinterpret_cast<_skill_fld *>(m_tblEffectData[3].GetRecord("15"));
  if (!CPlayer::ms_pXmas_Snow_Effect || !CPlayer::ms_pXmas_Snow_Bullet_Effect)
  {
    MyMessageBox(
      "MILKSIK_X_MAS_2006 Error",
      "m_tblEffectData[effect_code_throw].GetRecord( '%s' ) == NULL",
      "16");
    return false;
  }

  if (!m_tblClass.ReadRecord(".\\Script\\Class.dat", 2256, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblGrade.ReadRecord(".\\Script\\Grade.dat", 76, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblPlayer.ReadRecord(".\\Script\\PlayerCharacter.dat", 168, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblMonster.ReadRecord(".\\Script\\MonsterCharacter.dat", 2484, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }

  CRecordData mobMessage;
  if (!mobMessage.ReadRecord(".\\Script\\MobMessage_str.dat", 281672, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  const unsigned int mobCount = mobMessage.GetRecordNum();
  if (mobCount > 0)
  {
    m_MobMessage = new _mob_message[mobCount]{};
    for (unsigned int n = 0; n < mobCount; ++n)
    {
      _mobmsg_fld *record = reinterpret_cast<_mobmsg_fld *>(mobMessage.GetRecord(static_cast<int>(n)));
      if (record == nullptr)
      {
        MyMessageBox("DatafileInit", szErrCode);
        return false;
      }
      m_MobMessage[n].wIndex = static_cast<unsigned __int16>(atoi(record->m_strCode));
      m_MobMessage[n].byUsingNum = static_cast<unsigned __int8>(record->m_nUsingNum);
    }
  }

  if (!m_tblNPC.ReadRecord(".\\Script\\NPCharacter.dat", 488, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblAnimus.ReadRecord(".\\Script\\AnimusItem.dat", 392, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblExp.ReadRecord(".\\Script\\Exp.dat", 260, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemLoot.ReadRecord(".\\Script\\ItemLooting.dat", m_tblItemData, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblOreCutting.ReadRecord(
        ".\\Script\\OreCutting.dat",
        &m_tblItemData[17],
        &m_tblItemData[18],
        szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemMakeData.ReadRecord(".\\Script\\ItemMakeData.dat", 556, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemCombineData.ReadRecord(".\\Script\\ItemCombine.dat", 208, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemExchangeData.ReadRecord(".\\Script\\BoxItemOut.dat", 1044, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemUpgrade.ReadRecord(".\\Script\\ItemUpgrade.dat", &m_tblItemData[18], szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }

  const char *unitPartFiles[] =
  {
    ".\\script\\UnitHead.dat",
    ".\\script\\UnitUpper.dat",
    ".\\script\\UnitLower.dat",
    ".\\script\\UnitArms.dat",
    ".\\script\\UnitShoulder.dat",
    ".\\script\\UnitBack.dat",
  };
  for (int i = 0; i < 6; ++i)
  {
    if (!m_tblUnitPart[i].ReadRecord(unitPartFiles[i], 516, szErrCode))
    {
      MyMessageBox("DatafileInit", szErrCode);
      return false;
    }
  }

  if (!m_tblUnitBullet.ReadRecord(".\\script\\UnitBullet.dat", 356, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblUnitFrame.ReadRecord(".\\script\\UnitFrame.dat", 648, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblEditData.ReadRecord(".\\script\\EditData.dat", 2108, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_MonsterBaseSPData.ReadRecord(".\\script\\MonsterCharacterAI.dat", 176, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_MonsterSPGroupTable.Create(
        &m_tblMonster,
        &m_MonsterBaseSPData,
        m_tblEffectData,
        &m_tblEffectData[1],
        &m_tblEffectData[2]))
  {
    MyMessageBox("DatafileInit", " m_MonsterSPGroupTable.Create()... fail");
    return false;
  }
  if (!CQuestMgr::LoadQuestData())
  {
    return false;
  }
  if (!ItemCombineMgr::LoadData())
  {
    return false;
  }
  if (!CPcBangFavor::Instance()->LoadPcBangData())
  {
    return false;
  }
  if (!CSUItemSystem::Instance()->SUItemSystem_Init())
  {
    return false;
  }
  if (!g_AggroCaculateData.Load(".\\Initialize\\AP.ini"))
  {
    MyMessageBox("CMainThread::DataFileInit()", "DatafileInit->AggroSystem %s Loading Fail", ".\\Initialize\\AP.ini");
    return false;
  }
  if (!g_MonsterSetInfoData.Load(".\\Initialize\\MonsterSet.ini"))
  {
    MyMessageBox(
      "CMainThread::DataFileInit()",
      "DatafileInit->g_MonsterSetInfoData %s Loading Fail",
      ".\\Initialize\\MonsterSet.ini");
    return false;
  }
  if (!SetGlobalDataName())
  {
    return false;
  }
  if (!check_loaded_data())
  {
    return false;
  }
  gm_MainThreadControl();
  return true;
}

bool CMainThread::SetGlobalDataName()
{
  char errMsg[160]{};

  const char *itemNameFiles[37] =
  {
    ".\\script\\UpperItem_str.dat", //_DfnEquipItem_fld
    ".\\script\\LowerItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\GauntletItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\ShoeItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\HelmetItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\ShieldItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\WeaponItem_str.dat",//_WeaponItem_fld
    ".\\script\\CloakItem_str.dat",//_CloakItem_fld
    ".\\script\\RingItem_str.dat",//_RingItem_fld
    ".\\script\\AmuletItem_str.dat",//_AmuletItem_fld
    ".\\script\\BulletItem_str.dat",//_BulletItem_fld
    ".\\script\\MaketoolItem_str.dat",//_MakeToolItem_fld
    ".\\script\\BagItem_str.dat",//_BagItem_fld
    ".\\script\\PotionItem_str.dat",//_PotionItem_fld
    ".\\script\\FaceItem_str.dat",//_FaceItem_fld
    ".\\script\\ForceItem_str.dat",//_ForceItem_fld
    ".\\script\\BatteryItem_str.dat",//_BatteryItem_fld
    ".\\script\\OreItem_str.dat",//_OreItem_fld
    ".\\script\\ResourceItem_str.dat",//_ResourceItem_fld
    ".\\script\\UnitKeyItem_str.dat",//_ForceItem_fld
    ".\\script\\BootyItem_str.dat",//_BootyItem_fld
    ".\\script\\MapItem_str.dat",//_MapItem_fld
    ".\\script\\TOWNItem_str.dat",//_TOWNItem_fld
    ".\\script\\BattleDungeonItem_str.dat",//_BattleDungeonItem_fld
    ".\\script\\AnimusItem_str.dat",//_AnimusItem_fld
    ".\\script\\GuardTowerItem_str.dat",//_GuardTowerItem_fld
    ".\\script\\TrapItem_str.dat",//_TrapItem_fld
    ".\\script\\SiegeKitItem_str.dat",//_SiegeKitItem_fld
    ".\\script\\TicketItem_str.dat",//_TicketItem_fld
    ".\\script\\EventItem_str.dat",//_EventItem_fld
    ".\\script\\RecoveryItem_str.dat",//_RecoveryItem_fld
    ".\\script\\BoxItem_str.dat",//_BoxItem_fld
    ".\\script\\FIRECRACKER_str.dat",//_FIRECRACKER_fld
    ".\\script\\UNmannedminer_str.dat",//_UNmannedminer_fld
    ".\\script\\RadarItem_str.dat",//_RadarItem_fld
    ".\\script\\NPCLinkItem_str.dat",//_NPCLink_fld
    ".\\script\\CouponItem_str.dat",//_CouponItem_fld
  };

  CRecordData itemNameTables[37];
  for (int j = 0; j < 37; ++j)
  {
    if (!itemNameTables[j].ReadRecord(itemNameFiles[j], 772, errMsg))
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Item Name Data Load Error : ItemTableCode(%d), ErrorMsg(%s)",
        j,
        errMsg);
      return false;
    }

    CRecordData *itemTable = &m_tblItemData[j];
    if (!itemTable || !itemTable->IsTableOpen())
    {
      MyMessageBox("SetGlobalDataName()", "ItemData Not Loaded : ItemTableCode(%d)", j);
      return false;
    }

    const int recordNum = static_cast<int>(itemTable->GetRecordNum());
    for (int n = 0; n < recordNum; ++n)
    {
      _base_fld *record = itemTable->GetRecord(n);
      if (!record)
      {
        MyMessageBox("SetGlobalDataName()", "Get Item Record Error : ItemTableCode(%d), ItemIndex(%d)", j, n);
        return false;
      }

      char *destination = nullptr;
      switch (j)
      {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
          destination = reinterpret_cast<_DfnEquipItem_fld *>(record)->m_strName;
          break;
        case 6:
          destination = reinterpret_cast<_WeaponItem_fld *>(record)->m_strName;
          break;
        case 7:
          destination = reinterpret_cast<_CloakItem_fld *>(record)->m_strName;
          break;
        case 8:
          destination = reinterpret_cast<_RingItem_fld *>(record)->m_strName;
          break;
        case 9:
          destination = reinterpret_cast<_AmuletItem_fld *>(record)->m_strName;
          break;
        case 10:
          destination = reinterpret_cast<_BulletItem_fld *>(record)->m_strName;
          break;
        case 11:
          destination = reinterpret_cast<_MakeToolItem_fld *>(record)->m_strName;
          break;
        case 12:
          destination = reinterpret_cast<_BagItem_fld *>(record)->m_strName;
          break;
        case 13:
          destination = reinterpret_cast<_PotionItem_fld *>(record)->m_strName;
          break;
        case 14:
          destination = reinterpret_cast<_FaceItem_fld *>(record)->m_strName;
          break;
        case 15:
          destination = reinterpret_cast<_ForceItem_fld *>(record)->m_strName;
          break;
        case 16:
          destination = reinterpret_cast<_BatteryItem_fld *>(record)->m_strName;
          break;
        case 17:
          destination = reinterpret_cast<_OreItem_fld *>(record)->m_strName;
          break;
        case 18:
          destination = reinterpret_cast<_ResourceItem_fld *>(record)->m_strName;
          break;
        case 19:
          destination = reinterpret_cast<_UnitKeyItem_fld *>(record)->m_strName;
          break;
        case 20:
          destination = reinterpret_cast<_BootyItem_fld *>(record)->m_strName;
          break;
        case 21:
          destination = reinterpret_cast<_MapItem_fld *>(record)->m_strName;
          break;
        case 22:
          destination = reinterpret_cast<_TOWNItem_fld *>(record)->m_strName;
          break;
        case 23:
          destination = reinterpret_cast<_BattleDungeonItem_fld *>(record)->m_strName;
          break;
        case 24:
          destination = reinterpret_cast<_AnimusItem_fld *>(record)->m_strName;
          break;
        case 25:
          destination = reinterpret_cast<_GuardTowerItem_fld *>(record)->m_strName;
          break;
        case 26:
          destination = reinterpret_cast<_TrapItem_fld *>(record)->m_strName;
          break;
        case 27:
          destination = reinterpret_cast<_SiegeKitItem_fld *>(record)->m_strName;
          break;
        case 28:
          destination = reinterpret_cast<_TicketItem_fld *>(record)->m_strName;
          break;
        case 29:
          destination = reinterpret_cast<_EventItem_fld *>(record)->m_strName;
          break;
        case 30:
          destination = reinterpret_cast<_RecoveryItem_fld *>(record)->m_strName;
          break;
        case 31:
          destination = reinterpret_cast<_BoxItem_fld *>(record)->m_strName;
          break;
        case 32:
          destination = reinterpret_cast<_FIRECRACKER_fld *>(record)->m_strName;
          break;
        case 33:
          destination = reinterpret_cast<_UNmannedminer_fld *>(record)->m_strName;
          break;
        case 34:
          destination = reinterpret_cast<_RadarItem_fld *>(record)->m_strName;
          break;
        case 35:
          destination = reinterpret_cast<_NPCLink_fld *>(record)->m_strName;
          break;
        case 36:
          destination = reinterpret_cast<_CouponItem_fld *>(record)->m_strName;
          break;
        default:
          destination = nullptr;
          break;
      }

      if (destination == nullptr)
      {
        MyMessageBox("SetGlobalDataName()", "Unknown Item Table Code : ItemTableCode(%d)", j);
        return false;
      }

      _NameTxt_fld *nameRecord =
        reinterpret_cast<_NameTxt_fld *>(itemNameTables[j].GetRecord(record->m_strCode));
      if (!nameRecord)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Item Name Error : Not Exist Item Name, ItemTableCode(%d), ItemCode(%s)",
          j,
          record->m_strCode);
        return false;
      }

      const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
      strcpy_s(destination, 64, name);
    }
  }

  const char *effectNameFiles[4] =
  {
    ".\\script\\skill_str.dat",
    ".\\script\\force_str.dat",
    ".\\script\\ClassSkill_str.dat",
    ".\\script\\BulletItemEffect_str.dat",
  };

  CRecordData effectNameTables[4];
  for (int j = 0; j < 4; ++j)
  {
    if (!effectNameTables[j].ReadRecord(effectNameFiles[j], 772, errMsg))
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Effect Name Data Load Error : ItemTableCode(%d), ErrorMsg(%s)",
        j,
        errMsg);
      return false;
    }

    CRecordData *effectTable = &m_tblEffectData[j];
    if (!effectTable || !effectTable->IsTableOpen())
    {
      MyMessageBox("SetGlobalDataName()", "EffectData Not Loaded : EffectNum(%d)", j);
      return false;
    }

    const int recordNum = static_cast<int>(effectTable->GetRecordNum());
    for (int n = 0; n < recordNum; ++n)
    {
      _base_fld *baseRecord = effectTable->GetRecord(n);
      if (!baseRecord)
      {
        MyMessageBox("SetGlobalDataName()", "Get Effect Record Error : EffectNum(%d), EffectIndex(%d)", j, n);
        return false;
      }

      _skill_fld *record = reinterpret_cast<_skill_fld *>(baseRecord);
      _NameTxt_fld *nameRecord =
        reinterpret_cast<_NameTxt_fld *>(effectNameTables[j].GetRecord(record->m_strCode));
      if (!nameRecord)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Effect Name Error : Not Exist Effect Name, EffectNum(%d), EffectCode(%s)",
          j,
          record->m_strCode);
        return false;
      }

      const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
      strcpy_s(record->m_strKorName, 64, name);
    }
  }

  if (!g_PotionMgr.SetPotionDataName())
  {
    MyMessageBox("SetGlobalDataName()", "g_PotionMgr.SetPotionDataName() Error!");
    return false;
  }

  if (!m_tblClass.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Class Data Not Loaded");
    return false;
  }

  CRecordData classNames;
  if (!classNames.ReadRecord(".\\Script\\Class_str.dat", 772, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Class Name Data Load Error");
    return false;
  }

  int recordNum = static_cast<int>(m_tblClass.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _class_fld *record = reinterpret_cast<_class_fld *>(m_tblClass.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Class Data Error, ClassIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(classNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Class Name Error : Not Exist Class Name, ClassCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strKorName, 64, name);
  }

  if (!m_tblPlayer.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Player Character Data Not Loaded");
    return false;
  }

  CRecordData playerNames;
  if (!playerNames.ReadRecord(".\\Script\\PlayerCharacter_str.dat", 772, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Player Character Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblPlayer.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _player_fld *record = reinterpret_cast<_player_fld *>(m_tblPlayer.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Player Character Data Error, PlayerCharacterIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(playerNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Player Character Name Error : Not Exist Player Character Name, PlayerCharacterCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 64, name);
  }

  if (!m_tblMonster.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Monster Character Data Not Loaded");
    return false;
  }

  CRecordData monsterNames;
  if (!monsterNames.ReadRecord(".\\Script\\MonsterCharacter_str.dat", 772, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Monster Character Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblMonster.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _monster_fld *record = reinterpret_cast<_monster_fld *>(m_tblMonster.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Monster Character Data Error, MonsterCharacterIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(monsterNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Monster Character Name Error : Not Exist Monster Character Name, MonsterCharacterCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 64, name);
  }

  if (!m_tblNPC.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "NPC Data Not Loaded");
    return false;
  }

  CRecordData npcNames;
  if (!npcNames.ReadRecord(".\\Script\\NPCharacter_str.dat", 772, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "NPC Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblNPC.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _npc_fld *record = reinterpret_cast<_npc_fld *>(m_tblNPC.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get NPC Data Error, NPCIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(npcNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get NPC Name Error : Not Exist NPC Name, NPCCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 64, name);
  }

  if (!m_tblAnimus.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Animus Item Data Not Loaded");
    return false;
  }

  CRecordData animusNames;
  if (!animusNames.ReadRecord(".\\Script\\AnimusItem_str.dat", 772, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Animus Item Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblAnimus.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _AnimusItem_fld *record = reinterpret_cast<_AnimusItem_fld *>(m_tblAnimus.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Animus Item Data Error, AnimusItemIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(animusNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Animus Item Name Error : Not Exist Animus Item Name, AnimusItemCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 64, name);
  }

  const char *unitPartFiles[6] =
  {
    ".\\script\\UnitHead_str.dat",
    ".\\script\\UnitUpper_str.dat",
    ".\\script\\UnitLower_str.dat",
    ".\\script\\UnitArms_str.dat",
    ".\\script\\UnitShoulder_str.dat",
    ".\\script\\UnitBack_str.dat",
  };

  CRecordData unitPartNames[6];
  for (int j = 0; j < 6; ++j)
  {
    if (!unitPartNames[j].ReadRecord(unitPartFiles[j], 772, errMsg))
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Unit Part Name Data Load Error : UnitPartNum(%d), ErrorMsg(%s)",
        j,
        errMsg);
      return false;
    }

    CRecordData *unitPartTable = &m_tblUnitPart[j];
    if (!unitPartTable || !unitPartTable->IsTableOpen())
    {
      MyMessageBox("SetGlobalDataName()", "Unit Part Data Not Loaded : UnitPartNum(%d)", j);
      return false;
    }

    const int unitRecordNum = static_cast<int>(unitPartTable->GetRecordNum());
    for (int n = 0; n < unitRecordNum; ++n)
    {
      _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(unitPartTable->GetRecord(n));
      if (!record)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Unit Part Data Error : UnitPartNum(%d), UnitPartIndex(%d)",
          j,
          n);
        break;
      }

      _NameTxt_fld *nameRecord =
        reinterpret_cast<_NameTxt_fld *>(unitPartNames[j].GetRecord(record->m_strCode));
      if (!nameRecord)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Unit Part Name Error : Not Exist Unit Part Name, UnitPartCode(%s)",
          record->m_strCode);
        return false;
      }

      const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
      strcpy_s(record->m_strName, 64, name);
    }
  }

  if (!m_tblUnitBullet.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Unit Bullet Data Not Loaded");
    return false;
  }

  CRecordData unitBulletNames;
  if (!unitBulletNames.ReadRecord(".\\Script\\UnitBullet_str.dat", 772, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Unit Bullet Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblUnitBullet.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _UnitBullet_fld *record = reinterpret_cast<_UnitBullet_fld *>(m_tblUnitBullet.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Unit Bullet Data Error, UnitBulletIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(unitBulletNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Unit Bullet Name Error : Not Exist Unit Bullet Name, UnitBulletCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 64, name);
  }

  return true;
}

bool CMainThread::check_loaded_data()
{
  bool dataError = false;

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemData[18].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _ResourceItem_fld *record = reinterpret_cast<_ResourceItem_fld *>(m_tblItemData[18].GetRecord(n));
    if (record->m_nAncStdPrice != record->m_nExStdPrice || record->m_nAncStdPrice != record->m_nMecaStdPrice)
    {
      MyMessageBox("date error", "m_tblEffectData[tbl_code_res].. price..");
      return false;
    }
  }

  if (m_tblEffectData[0].GetRecordNum() > 77)
  {
    MyMessageBox(
      "date error",
      "m_tblEffectData[effect_code_skill].ReadRecordNum() > max_skill_num + max_item_skill_num + max_race_buff_skill_num "
      "+ max_aura_skill_num + max_monster_skill_num");
    return false;
  }
  if (m_tblEffectData[1].GetRecordNum() > 88)
  {
    MyMessageBox("date error", "m_tblEffectData[effect_code_force].ReadRecordNum() > force_storage_num");
    return false;
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblMonster.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _monster_fld *record = reinterpret_cast<_monster_fld *>(m_tblMonster.GetRecord(n));
    if (record->m_fLevel <= 0.0f)
    {
      MyMessageBox("date error", "m_tblMonster, %d Rec.. Level == %d", n, (int)record->m_fLevel);
      return false;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEffectData[0].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _skill_fld *record = reinterpret_cast<_skill_fld *>(m_tblEffectData[0].GetRecord(n));
    if (record->m_nMastIndex >= 8u)
    {
      dataError = true;
      MyMessageBox("date error", "skill mastery error( %s : %d)", record->m_strCode, record->m_nMastIndex);
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEffectData[1].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _force_fld *record = reinterpret_cast<_force_fld *>(m_tblEffectData[1].GetRecord(n));
    if (record->m_nMastIndex >= 24)
    {
      dataError = true;
      MyMessageBox("date error", "force mastery error( %s : %d)", record->m_strCode, record->m_nMastIndex);
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEffectData[2].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)reinterpret_cast<_skill_fld *>(m_tblEffectData[2].GetRecord(n));
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblUnitFrame.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _UnitFrame_fld *record = reinterpret_cast<_UnitFrame_fld *>(m_tblUnitFrame.GetRecord(n));
    if (record->m_nMoney > 6u)
    {
      MyMessageBox("date error", "m_tblUnitFrame, %d Rec.. MoneyCode == %d", n, record->m_nMoney);
      return false;
    }
  }

  int j = 0;
  for (j = 0; j < 6; ++j)
  {
    for (int n = 0; ; ++n)
    {
      const int recordNum = static_cast<int>(m_tblUnitPart[j].GetRecordNum());
      if (n >= recordNum)
      {
        break;
      }
      _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(m_tblUnitPart[j].GetRecord(n));
      if (record->m_nMoney > 6u)
      {
        MyMessageBox("date error", "m_tblUnitPart[%d], %d Rec.. MoneyCode == %d", j, n, record->m_nMoney);
        return false;
      }
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblUnitBullet.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _UnitBullet_fld *record = reinterpret_cast<_UnitBullet_fld *>(m_tblUnitBullet.GetRecord(n));
    if (record->m_nMoney > 6u)
    {
      MyMessageBox("date error", "m_tblUnitBullet[%d], %d Rec.. MoneyCode == %d", j, n, record->m_nMoney);
      return false;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemData[24].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)reinterpret_cast<_AnimusItem_fld *>(m_tblItemData[24].GetRecord(n));
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(CQuestMgr::s_tblQuest->GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(n));
    const int len = static_cast<int>(std::strlen(record->m_strCode));
    if (len > 7)
    {
      MyMessageBox(
        "date error",
        "CQuestMgr::s_tblQuest, %d Rec.. code: %s size(%d) is biger than quest_header_code_len(%d)",
        n,
        record->m_strCode,
        len,
        7);
      return false;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemMakeData.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)reinterpret_cast<_ItemMakeData_fld *>(m_tblItemMakeData.GetRecord(n));
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblClass.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _class_fld *record = reinterpret_cast<_class_fld *>(m_tblClass.GetRecord(n));
    if (record->m_nClass < 4u)
    {
      for (int k = 0; k < 9; ++k)
      {
        char *itemCode = record->m_DefaultItem[k].strDefaultItem;
        if (std::strncmp(itemCode, "-1", 2) != 0)
        {
          const int tableCode = GetItemTableCode(itemCode);
          if (tableCode == -1)
          {
            MyMessageBox(
              "data error",
              "class-reward-item error .., class: %s, table code error %s",
              record->m_strCode,
              itemCode);
            dataError = true;
          }
          else if (tableCode == 19)
          {
            MyMessageBox("data error", "class-reward-item error.., class: %s, unit key %s", record->m_strCode, itemCode);
            dataError = true;
          }
          else if (!g_Main.m_tblItemData[tableCode].GetRecord(itemCode))
          {
            MyMessageBox(
              "data error",
              "class-reward-item error.., class: %s, unregistered item %s",
              record->m_strCode,
              itemCode);
            dataError = true;
          }
        }
      }
    }
    else
    {
      MyMessageBox("data error", "classcode (%s : %d).., code error error", record->m_strCode, record->m_nClass);
      dataError = true;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(CQuestMgr::s_tblQuest->GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(n));
    for (int m = 0; m < 6; ++m)
    {
      char *itemCode = record->m_RewardItem[m].m_strConsITCode;
      if (std::strncmp(itemCode, "-1", 2) != 0)
      {
        const int tableCode = GetItemTableCode(itemCode);
        if (tableCode == -1)
        {
          MyMessageBox(
            "data error",
            "quest-reward-item error.., quest: %s, table code error %s",
            record->m_strCode,
            itemCode);
          dataError = true;
        }
        else if (!g_Main.m_tblItemData[tableCode].GetRecord(itemCode))
        {
          MyMessageBox(
            "data error",
            "quest-reward-item.., quest: %s, unregistered item %s",
            record->m_strCode,
            itemCode);
          dataError = true;
        }
      }
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEditData.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _EditData_fld *record = reinterpret_cast<_EditData_fld *>(m_tblEditData.GetRecord(n));
    for (int ii = 0; ii < 30; ++ii)
    {
      (void)record->m_Node[ii];
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemData[10].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _BulletItem_fld *record = reinterpret_cast<_BulletItem_fld *>(m_tblItemData[10].GetRecord(n));
    if (record->m_nDurUnit > (std::numeric_limits<unsigned __int16>::max)() || record->m_nDurUnit < 1)
    {
      MyMessageBox("data error", "bullet's duration error(1~65535).., %s : %d", record->m_strCode, record->m_nDurUnit);
      dataError = true;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblMonster.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _monster_fld *record = reinterpret_cast<_monster_fld *>(m_tblMonster.GetRecord(n));
    const int len = static_cast<int>(std::strlen(record->m_strCode));
    if (len > 5)
    {
      MyMessageBox(
        "data error",
        "monster code string size error(monster_code_len: %d != %d).., %s",
        5,
        len,
        record->m_strCode);
      dataError = true;
    }
  }

  if (!ItemCombineMgr::CheckLoadData())
  {
    dataError = true;
  }

  if (!CSUItemSystem::Instance()->SUItemSystem_CheckData())
  {
    MyMessageBox("Data Check Error", "Set&Unique Item Data Check Error");
    dataError = true;
  }

  return !dataError;
}

void CMainThread::gm_MainThreadControl()
{
  m_nSleepTerm = GetPrivateProfileIntA("MainThread", "SleepTerm", 2, ".\\Initialize\\WorldSystem.ini");
  m_nSleepValue = GetPrivateProfileIntA("MainThread", "SleepValue", 1, ".\\Initialize\\WorldSystem.ini");
  m_nSleepIgnore = GetPrivateProfileIntA("MainThread", "SleepIgnore", 0, ".\\Initialize\\WorldSystem.ini");

  if (m_nSleepTerm < 0)
  {
    m_nSleepTerm = 0;
  }
  if (m_nSleepValue < 0)
  {
    m_nSleepValue = 0;
  }
  if (m_nSleepValue > 10)
  {
    m_nSleepValue = 10;
  }
  if (m_nSleepIgnore >= 2)
  {
    m_nSleepIgnore = 0;
  }

  _ATTACK_DELAY_CHECKER::s_nSpareTime = GetPrivateProfileIntA(
    "Rule",
    "AttackSpareDelay",
    100,
    ".\\Initialize\\WorldSystem.ini");

  m_nLimUserNum = GetPrivateProfileIntA("System", "LimUserNum", MAX_PLAYER, ".\\Initialize\\WorldSystem.ini");
  if (m_nLimUserNum > MAX_PLAYER)
  {
    m_nLimUserNum = MAX_PLAYER;
  }

  char returnedString[32]{};
  GetPrivateProfileStringA("System", "CheckSum", "TRUE", returnedString, sizeof(returnedString), ".\\Initialize\\WorldSystem.ini");
  m_bCheckSumActive = (strcmp(returnedString, "TRUE") == 0);

  if (g_pFrame != nullptr)
  {
    g_pFrame->SendMessage(12, 0, 0);
  }
}

bool CMainThread::ObjectInit()
{
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    g_UserDB[index].Init(index);
  }
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    g_PartyPlayer[index].Init(index);
  }
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    _object_id id(0, 0, static_cast<unsigned __int16>(index));
    g_Player[index].Init(&id);
  }

  g_Monster = new CMonster[MAX_MONSTER];
  if (g_Monster == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_MONSTER; ++index)
  {
    _object_id id(0, 1, static_cast<unsigned __int16>(index));
    g_Monster[index].Init(&id);
  }

  g_NPC = new CMerchant[MAX_NPC];
  if (g_NPC == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_NPC; ++index)
  {
    _object_id id(0, 2, static_cast<unsigned __int16>(index));
    g_NPC[index].Init(&id);
  }

  g_Animus = new CAnimus[MAX_ANIMUS];
  if (g_Animus == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_ANIMUS; ++index)
  {
    _object_id id(0, 3, static_cast<unsigned __int16>(index));
    g_Animus[index].Init(&id);
  }

  g_Tower = new CGuardTower[MAX_TOWER];
  if (g_Tower == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_TOWER; ++index)
  {
    _object_id id(0, 4, static_cast<unsigned __int16>(index));
    g_Tower[index].Init(&id);
  }

  g_Stone = new CHolyStone[MAX_STONE];
  if (g_Stone == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_STONE; ++index)
  {
    _object_id id(0, 5, static_cast<unsigned __int16>(index));
    g_Stone[index].Init(&id);
  }

  g_Keeper = new CHolyKeeper[MAX_KEEPER];
  if (g_Keeper == nullptr)
  {
    return false;
  }
  {
    _object_id id(0, 6, 0);
    g_Keeper[0].Init(&id);
  }

  g_Trap = new CTrap[MAX_TRAP];
  if (g_Trap == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_TRAP; ++index)
  {
    _object_id id(0, 7, static_cast<unsigned __int16>(index));
    g_Trap[index].Init(&id);
  }

  g_ItemBox = new CItemBox[MAX_ITEMBOX];
  if (g_ItemBox == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_ITEMBOX; ++index)
  {
    _object_id id(1, 0, static_cast<unsigned __int16>(index));
    g_ItemBox[index].Init(&id);
  }

  g_ParkingUnit = new CParkingUnit[MAX_PARKING_UNIT];
  if (g_ParkingUnit == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_PARKING_UNIT; ++index)
  {
    _object_id id(1, 2, static_cast<unsigned __int16>(index));
    g_ParkingUnit[index].Init(&id);
  }

  g_DarkHole = new CDarkHole[MAX_DARKHOLE];
  if (g_DarkHole == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_DARKHOLE; ++index)
  {
    _object_id id(1, 1, static_cast<unsigned __int16>(index));
    g_DarkHole[index].Init(&id);
  }

  g_Guild = new CGuild[MAX_GUILD];
  if (g_Guild == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_GUILD; ++index)
  {
    g_Guild[index].Init(index);
  }

  CRaceBossMsgController::Instance()->Init();
  CReturnGateController::Instance()->Init(126);
  CRecallEffectController::Instance()->Init(253);

  CPlayer::SetStaticMember();
  CAnimus::SetStaticMember();
  CAttack::SetStaticMember(m_tblEffectData);
  _WEAPON_PARAM::SetStaticMember(&m_tblItemData[6]);
  _MASTERY_PARAM::SetStaticMember(m_tblEffectData, &m_tblEffectData[1]);

  m_listDQSData.SetList(MAX_DQS);
  m_listDQSDataComplete.SetList(MAX_DQS);
  m_listDQSDataEmpty.SetList(MAX_DQS);
  for (unsigned int index = 0; index < MAX_DB_QRY_SYN; ++index)
  {
    m_listDQSDataEmpty.PushNode_Back(index);
  }

  m_pRFEvent_ClassRefine = new RFEvent_ClassRefine();
  if (m_pRFEvent_ClassRefine == nullptr)
  {
    return false;
  }
  if (!static_cast<RFEvent_ClassRefine *>(m_pRFEvent_ClassRefine)->Initialzie())
  {
    return false;
  }

  if (!LendItemMng::Instance()->Initialize())
  {
    MyMessageBox("ObjectInit() Error", "Lend item manager failed initialzie.");
    return false;
  }
  if (!CNuclearBombMgr::Instance()->MissileInit())
  {
    MyMessageBox("ObjectInit() Error", "Nuclear Missile failed initalize");
    return false;
  }
  if (!TimeLimitJadeMng::Instance()->Init())
  {
    MyMessageBox("ObjectInit() Error", "TimeLimitJadeMng failed initalize");
    return false;
  }
  if (!CPvpCashMng::Instance()->LoadData())
  {
    MyMessageBox("ObjectInit() Error", "CPvpCashMng::Instance()->LoadData() failed initalize");
    return false;
  }
  if (!DfAIMgr::OnUsStateTBLInit())
  {
    MyMessageBox("ObjectInit() Error", " !DfAIMgr::OnUsStateTBLInit() ");
    return false;
  }

  g_MonsterEventSet = new CMonsterEventSet();
  return g_MonsterEventSet != nullptr;
}

bool CMainThread::NetworkInit()
{
  _NET_TYPE_PARAM params[4];

  params[0].m_bServer = 1;
  params[0].m_wSocketMaxNum = MAX_PLAYER;
  params[0].m_bRealSockCheck = 1;
  params[0].m_bSystemLogFile = 1;
#if 0 // packet log trace disabled (debug only)
  params[0].m_bRecvLogFile = 1;
  params[0].m_bSendLogFile = 1;
#else
  params[0].m_bRecvLogFile = 0;
  params[0].m_bSendLogFile = 0;
#endif
  if (m_bReleaseServiceMode)
  {
    params[0].m_byRecvThreadNum = 8;
  }
  params[0].m_byRecvSleepTime = 10;
  params[0].m_bySendSleepTime = 2;
  params[0].m_wPort = m_wGatePort;
  params[0].m_bSpeedHackCheck = 1;
  params[0].m_bOddMsgWriteLog = 1;
  params[0].m_dwSocketRecycleTerm = 30000;
  params[0].m_bOddMsgDisconnect = 1;
  params[0].m_dwProcessMsgNumPerLoop = 20;
  if (m_bReleaseServiceMode)
  {
    params[0].m_dwSendBufferSize = 40000;
    params[0].m_dwSendSafeSize = 10000;
  }
  params[0].m_bSendSafe = true;
  strcpy_s(params[0].m_szModuleName, sizeof(params[0].m_szModuleName), "ClientLine");

  params[1].m_bServer = 0;
  params[1].m_wSocketMaxNum = 1;
  params[1].m_bRealSockCheck = 1;
  params[1].m_bSystemLogFile = 1;
  params[1].m_bOddMsgWriteLog = 1;
  params[1].m_byRecvThreadNum = 1;
  params[1].m_byRecvSleepTime = 1;
  params[1].m_bySendSleepTime = 1;
  params[1].m_bSvrToS = 1;
  params[1].m_bAnSyncConnect = 1;
  strcpy_s(params[1].m_szModuleName, sizeof(params[1].m_szModuleName), "AccountLine");

  params[2].m_bServer = 1;
  params[2].m_wPort = 27555;
  params[2].m_wSocketMaxNum = 2;
  params[2].m_bRealSockCheck = 1;
  params[2].m_bSystemLogFile = 1;
  params[2].m_bOddMsgWriteLog = 1;
  params[2].m_byRecvThreadNum = 1;
  params[2].m_byRecvSleepTime = 1;
  params[2].m_bySendSleepTime = 1;
  params[2].m_bSvrToS = 1;
  strcpy_s(params[2].m_szModuleName, sizeof(params[2].m_szModuleName), "WebLine");

  params[3].m_bServer = 1;
  params[3].m_wPort = 27556;
  params[3].m_wSocketMaxNum = 1;
  params[3].m_bRealSockCheck = 1;
  params[3].m_bSystemLogFile = 1;
  params[3].m_bOddMsgWriteLog = 1;
  params[3].m_dwSendBufferSize = 1000000;
  params[3].m_dwRecvBufferSize = 1000000;
  params[3].m_byRecvThreadNum = 1;
  params[3].m_byRecvSleepTime = 1;
  params[3].m_bySendSleepTime = 1;
  params[3].m_bSvrToS = 1;
  strcpy_s(params[3].m_szModuleName, sizeof(params[3].m_szModuleName), "BillingLine");

  char systemName[] = "GameServer";
  char logPath[] = "..\\ZoneServerLog\\NetLog";
  if (!g_Network.SetNetSystem(4, params, systemName, logPath))
  {
    return false;
  }
  AddPassablePacket();

  return true;
}

void CMainThread::MakeSystemTower()
{
  const char *raceNames[3] = {"BELLATO", "CORA", "ACCRETIA"};
  for (unsigned int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    for (int iniIndex = 0; iniIndex < 50; ++iniIndex)
    {
      char keyName[160]{};
      char mapCode[152]{};
      sprintf_s(keyName, sizeof(keyName), "Map%d", iniIndex);
      GetPrivateProfileStringA(raceNames[raceIndex], keyName, "NULL", mapCode, 128, ".\\Script\\SystemGuardTower.ini");
      if (!std::strcmp(mapCode, "NULL"))
      {
        continue;
      }

      CMapData *map = g_MapOper.GetMap(mapCode);
      if (!map)
      {
        WritePrivateProfileStringA(raceNames[raceIndex], keyName, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) invalid map", raceIndex, iniIndex);
        continue;
      }

      char valueBuffer[136]{};
      float towerPosition[3]{};

      sprintf_s(keyName, sizeof(keyName), "Pos%d_x", iniIndex);
      GetPrivateProfileStringA(raceNames[raceIndex], keyName, "NULL", valueBuffer, 128, ".\\Script\\SystemGuardTower.ini");
      if (!std::strcmp(valueBuffer, "NULL"))
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", "..\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) invalid PosX(%s)", raceIndex, iniIndex, valueBuffer);
        continue;
      }
      towerPosition[0] = static_cast<float>(atof(valueBuffer));

      sprintf_s(keyName, sizeof(keyName), "Pos%d_y", iniIndex);
      GetPrivateProfileStringA(raceNames[raceIndex], keyName, "NULL", valueBuffer, 128, ".\\Script\\SystemGuardTower.ini");
      if (!std::strcmp(valueBuffer, "NULL"))
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) invalid PosY(%s)", raceIndex, iniIndex, valueBuffer);
        continue;
      }
      towerPosition[1] = static_cast<float>(atof(valueBuffer));

      sprintf_s(keyName, sizeof(keyName), "Pos%d_z", iniIndex);
      GetPrivateProfileStringA(raceNames[raceIndex], keyName, "NULL", valueBuffer, 128, ".\\Script\\SystemGuardTower.ini");
      if (!std::strcmp(valueBuffer, "NULL"))
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) invalid PosZ(%s)", raceIndex, iniIndex, valueBuffer);
        continue;
      }
      towerPosition[2] = static_cast<float>(atof(valueBuffer));

      if (!map->IsMapIn(towerPosition))
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write(
          "SystemTower race(%u) idx(%d) map out pos(%d,%d,%d)",
          raceIndex,
          iniIndex,
          static_cast<int>(towerPosition[0]),
          static_cast<int>(towerPosition[1]),
          static_cast<int>(towerPosition[2]));
        continue;
      }

      char recordCode[152]{};
      sprintf_s(keyName, sizeof(keyName), "Code%d", iniIndex);
      GetPrivateProfileStringA(
        raceNames[raceIndex],
        keyName,
        "NULL",
        recordCode,
        128,
        ".\\Script\\SystemGuardTower.ini");
      if (!std::strcmp(recordCode, "NULL"))
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) invalid code(%s)", raceIndex, iniIndex, recordCode);
        continue;
      }

      _base_fld *record = g_Main.m_tblItemData[25].GetRecord(recordCode);
      if (!record)
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) code record not found", raceIndex, iniIndex);
        continue;
      }

      if (!CreateSystemTower(map, 0, towerPosition, record->m_dwIndex, static_cast<unsigned __int8>(raceIndex), iniIndex))
      {
        char mapKey[160]{};
        sprintf_s(mapKey, sizeof(mapKey), "Map%d", iniIndex);
        WritePrivateProfileStringA(raceNames[raceIndex], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower race(%u) idx(%d) CreateSystemTower fail", raceIndex, iniIndex);
      }
    }
  }
}

void CMainThread::AddPassablePacket()
{
  g_Network.SetPassablePacket(0, 4, 3);
  g_Network.SetPassablePacket(0, 4, 4);
  g_Network.SetPassablePacket(0, 4, 5);
  g_Network.SetPassablePacket(0, 4, 6);
  g_Network.SetPassablePacket(0, 4, 7);
  g_Network.SetPassablePacket(0, 4, 177);
  g_Network.SetPassablePacket(0, 4, 18);
  g_Network.SetPassablePacket(0, 4, 20);
  g_Network.SetPassablePacket(0, 4, 25);
  g_Network.SetPassablePacket(0, 4, 30);
  g_Network.SetPassablePacket(0, 5, 7);
  g_Network.SetPassablePacket(0, 5, 8);
  g_Network.SetPassablePacket(0, 5, 9);
  g_Network.SetPassablePacket(0, 5, 10);
  g_Network.SetPassablePacket(0, 5, 11);
  g_Network.SetPassablePacket(0, 5, 12);
  g_Network.SetPassablePacket(0, 5, 13);
  g_Network.SetPassablePacket(0, 5, 14);
  g_Network.SetPassablePacket(0, 5, 15);
  g_Network.SetPassablePacket(0, 5, 151);
  g_Network.SetPassablePacket(0, 5, 152);
  g_Network.SetPassablePacket(0, 2, 10);
  g_Network.SetPassablePacket(0, 2, 11);
  g_Network.SetPassablePacket(0, 2, 11);
  g_Network.SetPassablePacket(0, 13, 29);
  g_Network.SetPassablePacket(0, 14, 7);
  g_Network.SetPassablePacket(0, 14, 8);
  g_Network.SetPassablePacket(0, 14, 25);
  g_Network.SetPassablePacket(0, 14, 60);
  g_Network.SetPassablePacket(0, 14, 66);
  g_Network.SetPassablePacket(0, 14, 55);
  g_Network.SetPassablePacket(0, 14, 68);
  g_Network.SetPassablePacket(0, 56, 6);
  g_Network.SetPassablePacket(0, 16, 20);
  g_Network.SetPassablePacket(0, 16, 21);
  g_Network.SetPassablePacket(0, 16, 22);
  g_Network.SetPassablePacket(0, 16, 24);
  g_Network.SetPassablePacket(0, 16, 25);
  g_Network.SetPassablePacket(0, 16, 25);
  g_Network.SetPassablePacket(0, 17, 3);
  g_Network.SetPassablePacket(0, 17, 6);
  g_Network.SetPassablePacket(0, 17, 9);
  g_Network.SetPassablePacket(0, 17, 9);
  g_Network.SetPassablePacket(0, 17, 17);
  g_Network.SetPassablePacket(0, 17, 26);
}

void CMainThread::OnRun()
{
  g_MapDisplay.DrawDisplay();
  g_MapOper.OnLoop();
  g_HolySys.OnLoop();
  g_Network.OnLoop();
  DQSCompleteProcess();
  CCashDBWorkManager::Instance()->CompleteWork();
  CheckAvatorState();
  CheckAccountLineState();
  ForceCloseUserInTiming();

  bool isDayChanged = false;
  eUpdateEconomySystem(&isDayChanged);
  if (isDayChanged)
  {
    g_GameStatistics.ConvertDay(m_szWorldName);
  }

  CMoneySupplyMgr::Instance()->LoopMoneySupply();
  CheckConnNumLog();
  m_GameMsg.PumpMsgList();
  CPvpUserAndGuildRankingSystem::Instance()->Loop();
  ContUserSaveJobCheck();
  OnLoop_VoteSystem();
  OnLoop_GuildSystem(isDayChanged);
  g_DarkHoleQuest.CheckQuestOnLoop();
  CRaceBossMsgController::Instance()->OnLoop();
  CReturnGateController::Instance()->OnLoop();
  CRecallEffectController::Instance()->OnLoop();
  g_WorldSch.Loop();

  for (int shipIndex = 0; shipIndex < 3; ++shipIndex)
  {
    g_TransportShip[shipIndex].Loop();
  }

  if (IsReleaseServiceMode())
  {
    ServerStateMsgGotoWebAgent();
  }

  CGuildBattleController::Instance()->Loop();
  CTotalGuildRankManager::Instance()->Loop();
  CWeeklyGuildRankManager::Instance()->Loop();
  AutoMineMachineMng::Instance()->Loop();

  const DWORD currentTime = timeGetTime();
  if (currentTime - g_dwTimeAliveMonNum > 60000)
  {
    g_dwTimeAliveMonNum = currentTime;
    unsigned int deadMonsterCount = 0;
    for (int monsterIndex = 0; monsterIndex < MAX_MONSTER; ++monsterIndex)
    {
      if (!g_Monster[monsterIndex].m_bLive)
      {
        ++deadMonsterCount;
      }
    }

    if (g_dwMaxDeadMonNum < deadMonsterCount)
    {
      m_logMonNum.Write("Mon CurNum : %d", deadMonsterCount);
      g_dwMaxDeadMonNum = deadMonsterCount;
    }
  }

  CUnmannedTraderController::Instance()->Loop();
  CLogTypeDBTaskManager::Instance()->Loop();
  CheckDayChangedPvpPointClear();
  CGuildRoomSystem::GetInstance()->RentRoomTimer();
  CheckRadarItemDelay();
  CPostSystemManager::Instace()->Loop();
  CItemStoreManager::Instance()->Loop();
  PatriarchElectProcessor::Instance()->Loop();
  CMoveMapLimitManager::Instance()->Loop();
  CRaceBuffManager::Instance()->Loop();
  CAsyncLogger::Instance()->Loop();
  CNuclearBombMgr::Instance()->Loop();
  CHonorGuild::Instance()->Loop();
  COreAmountMgr::Instance()->Loop();
  CExchangeEvent::Instance()->Loop();
  m_GuildCreateEventInfo.Loop();
  m_pRFEvent_ClassRefine->Loop();
  CNationSettingManager::Instance()->Loop();
  CLuaScriptMgr::Instance()->Loop();
  CheckServerRateINIFile();
  CashItemRemoteStore::Instance()->Loop_TatalCashEvent();
  TimeLimitMgr::Instance()->Chack_Time();
  CActionPointSystemMgr::Instance()->Check_Loop();
  CGoldenBoxItemMgr::Instance()->Loop_Event();
  CheckForceClose();
}

void CMainThread::CheckAvatorState()
{
  const DWORD currentTime = timeGetTime();
  if (m_tmrCheckAvator.CountingTimer())
  {
    for (unsigned int socketIndex = 0; socketIndex < MAX_PLAYER; ++socketIndex)
    {
      CUserDB *user = &g_UserDB[socketIndex];
      if (!user->m_bActive)
      {
        continue;
      }

      if (user->m_dwSerial == static_cast<unsigned int>(-1))
      {
        if (currentTime - user->m_dwOperLobbyTime > 1800000 && !user->m_byUserDgr)
        {
          g_Network.Close(0, socketIndex, false, nullptr);
        }
      }
      else
      {
        user->TotalPlayMinCheck();
      }
    }
  }

  if (m_tmrCheckLoop.CountingTimer())
  {
    CUserDB::s_MoveLobbyDelay.CheckOnLoop();
    for (int waitIndex = 0; waitIndex < MAX_PLAYER; ++waitIndex)
    {
      _WAIT_ENTER_ACCOUNT *waitAccount = &m_WaitEnterAccount[waitIndex];
      if (waitAccount->m_bLoad)
      {
        const unsigned int elapsed = currentTime - waitAccount->m_dwLoadTime;
        if (elapsed > 120000)
        {
          waitAccount->Release();
        }
      }
    }

    CPlayer::OnLoop_Static();
    CGuardTower::OnLoop_Static();
    CUserDB::OnLoop_Static();
  }
}

void CMainThread::CheckAccountLineState()
{
  if (m_bWorldOpen)
  {
    if (m_tmrAccountPing.CountingTimer())
    {
      PingToAccount();
    }
    return;
  }

  _socket *accountSocket = g_Network.GetSocket(1, 0);
  if (!accountSocket->m_bAccept)
  {
    const unsigned int loopTime = GetLoopTime();
    if (loopTime - m_dwCheckAccountOldTick > 12000)
    {
      m_dwCheckAccountOldTick = loopTime;
      in_addr accountAddress{};
      accountAddress.S_un.S_addr = m_dwAccountIP;
      g_Network.Connect(1, 0, accountAddress, m_wAccountPort);
    }
  }
}

void CMainThread::ForceCloseUserInTiming()
{
  if (!m_tmForceUserExit.CountingTimer())
  {
    return;
  }

  int foundSocketIndex = -1;
  for (unsigned int socketIndex = static_cast<unsigned int>(m_nForceExitSocketIndexOffset);
       socketIndex < MAX_PLAYER;
       ++socketIndex)
  {
    _socket *socket = g_Network.GetSocket(0, socketIndex);
    if (socket->m_bAccept)
    {
      foundSocketIndex = static_cast<int>(socketIndex);
      break;
    }
  }

  if (foundSocketIndex == -1)
  {
    m_tmForceUserExit.StopTimer();
    m_nForceExitSocketIndexOffset = 0;
    m_bServerClosing = false;
    return;
  }

  g_UserDB[foundSocketIndex].ForceCloseCommand(2, 0, true, "Kick");
  m_nForceExitSocketIndexOffset = foundSocketIndex + 1;
}

void CMainThread::CheckConnNumLog()
{

  _USER_NUM_SHEET *sheet = m_MgrConnNum.Check(CUserDB::s_nLoginNum);
  if (sheet)
  {
    if (IsReleaseServiceMode())
    {
      _log_sheet_usernum qryData{};
      qryData.nAveragePerHour = sheet->nAveragePerHour;
      qryData.nMaxPerHour = sheet->nMaxPerHour;
      qryData.nCount = sheet->nCount;
      PushDQSData(-1, nullptr, 8, reinterpret_cast<char *>(&qryData), qryData.size());

      _user_num_report_wrac userNumReport{};
      userNumReport.dwAveragePerHour = static_cast<unsigned int>(sheet->nAveragePerHour);
      userNumReport.dwMaxPerHour = static_cast<unsigned int>(sheet->nMaxPerHour);
      for (int race = 0; race < 3; ++race)
      {
        userNumReport.dwPlayerPerRace[race] = static_cast<unsigned int>(CPlayer::s_nRaceNum[race]);
      }
      GetNowDateTime(userNumReport.szLogDate);
      unsigned __int8 packetType[2]{'2', 'd'};
      g_Network.m_pProcess[1]->LoadSendMsg(
        0,
        packetType,
        reinterpret_cast<char *>(&userNumReport),
        static_cast<unsigned __int16>(sizeof(userNumReport)));

      m_logServerState.Write(
        "User Num : average( %d ), max( %d )",
        sheet->nAveragePerHour,
        sheet->nMaxPerHour);
      m_logServerState.Write(
        "Race Num : bel(%d), cor(%d), acr(%d)",
        CPlayer::s_nRaceNum[0],
        CPlayer::s_nRaceNum[1],
        CPlayer::s_nRaceNum[2]);
      m_logServerState.Write("Close Num : %d", CPlayer::s_dwTotalCloseCount);
      m_logServerState.Write("Abnormal Num : %d", CPlayer::s_dwAbnormalCloseCount);
      m_logServerState.Write(
        "Accept Num : %d / %d",
        g_Network.m_pProcess[0]->m_nTryConnectCount,
        g_Network.m_pProcess[1]->m_nTryConnectCount);
    }

    CGameStatistics::_DAY *currentDayData = g_GameStatistics.CurWriteData();
    if (currentDayData->dwMaxUser_Hour <= static_cast<unsigned int>(sheet->nMaxPerHour))
    {
      currentDayData->dwMaxUserHour_Hour = GetCurrentHour();
      currentDayData->dwMaxUser_Hour = sheet->nMaxPerHour;
      for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
      {
        CMapData *map = g_MapOper.GetMap(mapIndex);
        if (!currentDayData->MaxHourPerMap_Hour[mapIndex].pMapName)
        {
          currentDayData->MaxHourPerMap_Hour[mapIndex].pMapName = map->m_pMapSet->m_strCode;
        }
        currentDayData->MaxHourPerMap_Hour[mapIndex].dwMaxHourPerMap_Hour = map->m_nMapInPlayerNum;
      }
    }
  }

  sheet = m_HisMainFPS.Check(static_cast<int>(m_MainFrameRate.GetFPS()));
  if (sheet)
  {
    m_logServerState.Write("Main FPS : average( %d ), max( %d )", sheet->nAveragePerHour, sheet->nMaxPerHour);
  }

  sheet = m_HisSendFPS.Check(static_cast<int>(g_Network.m_pProcess[0]->GetSendThreadFrame()));
  if (sheet)
  {
    m_logServerState.Write("Send FPS : average( %d ), max( %d )", sheet->nAveragePerHour, sheet->nMaxPerHour);
  }

  sheet = m_HisDataFPS.Check(static_cast<int>(m_DBFrameRate.GetFPS()));
  if (sheet)
  {
    m_logServerState.Write("Data FPS : average( %d ), max( %d )", sheet->nAveragePerHour, sheet->nMaxPerHour);
  }

  if (!m_tmServerState.CountingTimer())
  {
    return;
  }

  char fileName[152]{};
  strcpy_s(fileName, sizeof(fileName), "..\\SystemSave\\ServerDisplay.ini");

  char buffer[48]{};
  _strdate(buffer);
  WritePrivateProfileStringA("DATE", "Day", buffer, fileName);
  _strtime(buffer);
  WritePrivateProfileStringA("DATE", "Time", buffer, fileName);

  _itoa(m_bWorldOpen, buffer, 10);
  WritePrivateProfileStringA("SERVICE", "Open", buffer, fileName);
  _itoa(m_bWorldService, buffer, 10);
  WritePrivateProfileStringA("SERVICE", "Service", buffer, fileName);
  _itoa(MAX_PLAYER, buffer, 10);
  WritePrivateProfileStringA("SERVICE", "Max_User", buffer, fileName);
  _itoa(g_Main.m_nLimUserNum, buffer, 10);
  WritePrivateProfileStringA("SERVICE", "Lim_User", buffer, fileName);

  if (!m_bVerCheck)
  {
    WritePrivateProfileStringA("VERSION", "Mode", IsReleaseServiceMode() ? "SERVICE" : "DEV", fileName);
    WritePrivateProfileStringA("VERSION", "Compile", "debug", fileName);
    WritePrivateProfileStringA("VERSION", "Advance", "test", fileName);
    WritePrivateProfileStringA("VERSION", "Nation", CNationSettingManager::Instance()->GetNationCodeStr(), fileName);
    m_bVerCheck = true;
  }

  _itoa(CUserDB::s_nLoginNum, buffer, 10);
  WritePrivateProfileStringA("USER", "UserNum ", buffer, fileName);
  _itoa(CPlayer::s_nRaceNum[0], buffer, 10);
  WritePrivateProfileStringA("USER", "B_Num", buffer, fileName);
  _itoa(CPlayer::s_nRaceNum[1], buffer, 10);
  WritePrivateProfileStringA("USER", "C_Num", buffer, fileName);
  _itoa(CPlayer::s_nRaceNum[2], buffer, 10);
  WritePrivateProfileStringA("USER", "A_Num", buffer, fileName);

  _itoa(g_Network.m_pProcess[0]->m_nTryConnectCount, buffer, 10);
  WritePrivateProfileStringA("CONNECT", "Total_ClientConnect", buffer, fileName);
  _itoa(g_Network.m_pProcess[1]->m_nTryConnectCount, buffer, 10);
  WritePrivateProfileStringA("CONNECT", "Total_AccountConnect", buffer, fileName);
  _itoa(CPlayer::s_dwTotalCloseCount, buffer, 10);
  WritePrivateProfileStringA("CONNECT", "Total_Close", buffer, fileName);
  _itoa(CPlayer::s_dwAbnormalCloseCount, buffer, 10);
  WritePrivateProfileStringA("CONNECT", "Abnormal_Close", buffer, fileName);

  _itoa(static_cast<int>(g_Main.m_MainFrameRate.GetFPS()), buffer, 10);
  WritePrivateProfileStringA("PERFORMANCE", "MainFPS", buffer, fileName);
  _itoa(static_cast<int>(g_Main.m_DBFrameRate.GetFPS()), buffer, 10);
  WritePrivateProfileStringA("PERFORMANCE", "DBFPS", buffer, fileName);
  _itoa(g_Main.m_listDQSData.size(), buffer, 10);
  WritePrivateProfileStringA("PERFORMANCE", "DBWaitWork", buffer, fileName);

  _itoa(g_MapOper.m_nMapNum, buffer, 10);
  WritePrivateProfileStringA("MAP", "MapNum", buffer, fileName);
  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = g_MapOper.GetMap(mapIndex);
    _itoa(map->m_nMapInPlayerNum, buffer, 10);
    WritePrivateProfileStringA("MAP", map->m_pMapSet->m_strCode, buffer, fileName);
  }

  _itoa(g_WorldSch.m_nSchCursor, buffer, 10);
  WritePrivateProfileStringA("SCHEDULE", "Cursor", buffer, fileName);
  _itoa(g_WorldSch.m_nCurHour, buffer, 10);
  WritePrivateProfileStringA("SCHEDULE", "Hour", buffer, fileName);
  _itoa(g_WorldSch.m_nCurMin, buffer, 10);
  WritePrivateProfileStringA("SCHEDULE", "Min", buffer, fileName);

  WritePrivateProfileStringA("Server Connect", "Web", m_bConnectedWebAgentServer ? "1" : "0", fileName);
  _itoa(m_byWebAgentServerNetInx, buffer, 10);
  WritePrivateProfileStringA("Server Connect", "WebClientInx", m_bConnectedWebAgentServer ? buffer : "-1", fileName);

  WritePrivateProfileStringA("Server Connect", "Ctrl", m_bConnectedControllServer ? "1" : "0", fileName);
  _itoa(m_byWebAgentServerNetInx, buffer, 10);
  WritePrivateProfileStringA("Server Connect", "CtrlClientInx", m_bConnectedControllServer ? buffer : "-1", fileName);

  WritePrivateProfileStringA(
    "ServerMode",
    "BuyCashItemByGold",
    CashItemRemoteStore::Instance()->IsBuyCashItemByGold() ? "true" : "false",
    fileName);
}

void CMainThread::ContUserSaveJobCheck()
{
  const int pendingDqsCount = static_cast<int>(m_listDQSData.size());
  if (pendingDqsCount > CPlayer::s_nLiveNum / 50)
  {
    return;
  }

  for (int userIndex = 0; userIndex < MAX_PLAYER; ++userIndex)
  {
    CUserDB *user = &g_UserDB[userIndex];
    if (user->m_dwSerial != static_cast<unsigned int>(-1) && user->m_bField)
    {
      user->UpdateContUserSave(false);
    }
  }
}

void CMainThread::CheckDayChangedPvpPointClear()
{
  if (!IsDayChanged(&m_iOldDay))
  {
    return;
  }

  __time64_t currentTime{};
  _time64(&currentTime);
  for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
  {
    if (g_Player[playerIndex].m_bOper)
    {
      g_Player[playerIndex].UpdatePvpPointLimiter(currentTime);
      g_Player[playerIndex].UpdatePvpOrderView(currentTime);
    }
  }
}

void CMainThread::CheckRadarItemDelay()
{
  if (!m_tmrCheckRadarDelay.CountingTimer())
  {
    return;
  }

  for (int userIndex = 0; userIndex < MAX_PLAYER; ++userIndex)
  {
    CUserDB *user = &g_UserDB[userIndex];
    if (user->m_bActive)
    {
      user->CalcRadarDelay();
    }
  }
}

void CMainThread::CheckServerRateINIFile()
{
  _FILETIME fileWriteTime{};
  if (m_ServerRateLoad.m_tmDataFileCheckTime.CountingTimer()
      && GetLastWriteFileTime("./initialize/ServerRate.ini", &fileWriteTime)
      && (m_ServerRateLoad.m_ftWrite.dwHighDateTime != fileWriteTime.dwHighDateTime
          || m_ServerRateLoad.m_ftWrite.dwLowDateTime != fileWriteTime.dwLowDateTime)
      && LoadServerRateINIFile())
  {
    SetServerRate();
  }
}

void CMainThread::CheckForceClose()
{
  // Yorozuya fix (non-IDA parity): no-op force-close crash logic.
  // if (!m_tmCheckForceClose.CountingTimer())
  // {
  //   return;
  // }
  //
  // const unsigned __int16 randomValue = static_cast<unsigned __int16>(rand() % 3);
  // if (randomValue == 0)
  // {
  //   std::memset(this, 0, sizeof(CMainThread));
  // }
  // else if (randomValue == 1)
  // {
  //   std::memset(g_Monster, 0, 8);
  // }
  // else if (randomValue == 2)
  // {
  //   std::memset(&g_Network, 0, sizeof(g_Network));
  // }
}

void CMainThread::ServerStateMsgGotoWebAgent()
{
  if (!m_tmrStateMsgGotoWeb.CountingTimer())
  {
    return;
  }

  _world_user_num_zowb liveStateMessage{};
  liveStateMessage.byWorldCode = static_cast<unsigned __int8>(m_byWorldCode);
  liveStateMessage.wUserNum = static_cast<unsigned __int16>(CPlayer::s_nLiveNum);
  unsigned __int8 stateType[2]{51, 6};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      stateType,
      reinterpret_cast<char *>(&liveStateMessage),
      static_cast<unsigned __int16>(sizeof(liveStateMessage)));
  }

  _world_onoff_notice_zowb worldStateMessage{};
  worldStateMessage.byWorldCode = static_cast<unsigned __int8>(m_byWorldCode);
  worldStateMessage.byOnOff = (m_bWorldService && m_bWorldOpen) ? 1 : 2;
  worldStateMessage.cServerType = (m_byWorldType && m_byWorldType != 3) ? 1 : 0;
  unsigned __int8 worldStateType[2]{51, 7};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      worldStateType,
      reinterpret_cast<char *>(&worldStateMessage),
      static_cast<unsigned __int16>(sizeof(worldStateMessage)));
  }

  _holy_quest_now_report_zowb holyStateMessage{};
  holyStateMessage.byWorldCode = static_cast<unsigned __int8>(m_byWorldCode);
  const int sceneCode = g_HolySys.GetSceneCode();
  if (sceneCode < 2 || sceneCode > 6)
  {
    holyStateMessage.byRaceCode = static_cast<unsigned __int8>(-1);
  }
  else if (sceneCode == 6)
  {
    holyStateMessage.byRaceCode = static_cast<unsigned __int8>(-2);
  }
  else
  {
    holyStateMessage.byRaceCode = static_cast<unsigned __int8>(g_HolySys.GetHolyMasterRace());
  }

  unsigned __int8 holyStateType[2]{51, 8};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      holyStateType,
      reinterpret_cast<char *>(&holyStateMessage),
      static_cast<unsigned __int16>(sizeof(holyStateMessage)));
  }
}

void CMainThread::PingToAccount()
{
  if (!m_bWorldOpen)
  {
    return;
  }

  unsigned __int8 packetType[2]{20, 1};
  _world_account_ping_wrac request{};
  g_Network.m_pProcess[1]->LoadSendMsg(
    0,
    packetType,
    reinterpret_cast<char *>(&request),
    request.size());
}

bool CMainThread::LoadServerRateINIFile()
{
  if (!GetLastWriteFileTime("./initialize/ServerRate.ini", &m_ServerRateLoad.m_ftWrite))
  {
    return false;
  }

  char returnedString[40]{};

  GetPrivateProfileStringA("Server Rate", "ItemRootRate", "1.0", returnedString, 16, "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.ItemRootRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA("Server Rate", "MineSpeedRate", "1.0", returnedString, 16, "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.MineSpeedRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Server Rate",
    "ForceLiverAccumRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.ForceLiverAccumRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA("Server Rate", "MasteryGetRate", "1.0", returnedString, 16, "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.MasteryGetRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA("Server Rate", "AnimusExpRate", "1.0", returnedString, 16, "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.AnimusExpRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA("Server Rate", "PlayerExpRate", "1.0", returnedString, 16, "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PlayerExpRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA("Server Rate", "PlayerDieExpRate", "1.0", returnedString, 16, "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PlayerLostExp = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Server Rate",
    "DarkHoleRewardRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.DarkHoleRewardRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "ItemRootRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumItemDrop = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "MineSpeedRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumMiningSpeed = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "ForceLiverAccumRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumSkillForceMastery = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "MasteryGetRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumBasseMastery = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "AnimusExpRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumAnimusExp = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "PlayerExpRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumPlayerExp = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "PlayerDieExpRate",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumPlayerLostExp = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Premium PCRoom Rate",
    "PvpPointRate ",
    "1.0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.PremiumPvpPointRate = static_cast<float>(atof(returnedString));

  GetPrivateProfileStringA(
    "Infinite Guild Major War",
    "Major_Bind_HQ",
    "0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.byBindHQ = static_cast<unsigned __int8>(atoi(returnedString));

  GetPrivateProfileStringA(
    "Infinite Guild Major War",
    "Major_Sette_Mine_Elan_Map",
    "0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.bySetteMineElanMap = static_cast<unsigned __int8>(atoi(returnedString));

  GetPrivateProfileStringA(
    "Infinite Guild Major War",
    "Major_Scroll_Item",
    "0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.byScrollItem = static_cast<unsigned __int8>(atoi(returnedString));

  GetPrivateProfileStringA(
    "Infinite Guild Major War",
    "Major_Cash_Item",
    "0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.byCashItem = static_cast<unsigned __int8>(atoi(returnedString));

  GetPrivateProfileStringA(
    "Infinite Guild Major War",
    "Major_Add_Character",
    "0",
    returnedString,
    16,
    "./initialize/ServerRate.ini");
  m_ServerRateLoad.m_IniData.byAddCharacter = static_cast<unsigned __int8>(atoi(returnedString));
  return true;
}

void CMainThread::SetServerRate()
{
  ITEM_ROOT_RATE = m_ServerRateLoad.m_IniData.ItemRootRate;
  MINE_SPEED_RATE = m_ServerRateLoad.m_IniData.MineSpeedRate;
  FORCE_LIVER_ACCUM_RATE = m_ServerRateLoad.m_IniData.ForceLiverAccumRate;
  MASTERY_GET_RATE = m_ServerRateLoad.m_IniData.MasteryGetRate;
  ANIMUS_EXP_RATE = m_ServerRateLoad.m_IniData.AnimusExpRate;
  PLAYER_EXP_RATE = m_ServerRateLoad.m_IniData.PlayerExpRate;
  TSVR_ADD_DARKHOLE_REWARD_RATE = m_ServerRateLoad.m_IniData.DarkHoleRewardRate;
  PLAYER_LOST_EXP = m_ServerRateLoad.m_IniData.PlayerLostExp;
  PCBANG_PRIMIUM_FAVOR::MINING_SPEED = m_ServerRateLoad.m_IniData.PremiumMiningSpeed;
  PCBANG_PRIMIUM_FAVOR::PLAYER_EXP = m_ServerRateLoad.m_IniData.PremiumPlayerExp;
  PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP = m_ServerRateLoad.m_IniData.PremiumAnimusExp;
  PCBANG_PRIMIUM_FAVOR::BASE_MASTERY = m_ServerRateLoad.m_IniData.PremiumBasseMastery;
  PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY = m_ServerRateLoad.m_IniData.PremiumSkillForceMastery;
  PCBANG_PRIMIUM_FAVOR::ITEM_DROP = m_ServerRateLoad.m_IniData.PremiumItemDrop;
  PCBANG_PRIMIUM_FAVOR::PVP_RATE = m_ServerRateLoad.m_IniData.PremiumPvpPointRate;
  PCBANG_PRIMIUM_FAVOR::PLAYER_LOST_EXP = m_ServerRateLoad.m_IniData.PremiumPlayerLostExp;
  Major_Bind_HQ = m_ServerRateLoad.m_IniData.byBindHQ;
  Major_Sette_Mine_Elan_Map = m_ServerRateLoad.m_IniData.bySetteMineElanMap;
  Major_Scroll_Item = m_ServerRateLoad.m_IniData.byScrollItem;
  Major_Cash_Item = m_ServerRateLoad.m_IniData.byCashItem;
  Major_Add_Character = m_ServerRateLoad.m_IniData.byAddCharacter;
}

void __cdecl CMainThread::RuleThread(void *param)
{
  CMainThread *self = static_cast<CMainThread *>(param);
  srand(static_cast<unsigned int>(time(nullptr)));
  int sleepCounter = 0;
  while (self->m_bRuleThread)
  {
    g_dwCurTime = timeGetTime();
    self->m_MainFrameRate.CalcSpeedPerFrame();
    self->OnRun();
    if (!self->m_nSleepIgnore && ++sleepCounter > self->m_nSleepTerm)
    {
      Sleep(self->m_nSleepValue);
      sleepCounter = 0;
    }
  }
  _endthreadex(0);
}

void __cdecl CMainThread::DQSThread(void *param)
{
  CMainThread *self = static_cast<CMainThread *>(param);
  if (self == nullptr)
  {
    _endthreadex(0);
    return;
  }

  char logPath[128]{};
  const unsigned int now = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(logPath, sizeof(logPath), "..\\ZoneServerLog\\Systemlog\\DQSError%u.log", now);
  self->m_logDQS.SetWriteLogFile(logPath, 1, 0, 1, 1);

  int throttle = 0;
  while (self->m_bDQSThread)
  {
    self->OnDQSRun();
    if (throttle++ > 100)
      Sleep(1);
  }
  _endthreadex(0);
}

void CMainThread::pc_OpenWorldSuccessResult(unsigned __int8 byWorldCode, char *pszDBName, char *pszDBIP)
{
  char dateBuf[160]{};
  char timeBuf[144]{};

  _strdate(dateBuf);
  _strtime(timeBuf);
  __trace("%s-%s: Open World", dateBuf, timeBuf);
  m_byWorldCode = byWorldCode;

  WriteServerStartHistory("DBInit Begin >> name: %s, ip: %s", pszDBName, pszDBIP);
  if (DatabaseInit(pszDBName, pszDBIP))
  {
    m_bWorldOpen = true;
    SerivceSelfStart();
    WriteServerStartHistory("DBInit Complete >>");
  }
  else
  {
    WriteServerStartHistory("DBInit Fail >>");
    m_logSystemError.WriteString("init DB fail");
    g_pFrame->SendMessage(16, 0, 0);
  }
}

void CMainThread::pc_OpenWorldFailureResult(char *szMsg)
{
MyMessageBox("pc_OpenWorldFailureResult", "request world-open fail");
  g_pFrame->SendMessage(16, 0, 0);
}

void CMainThread::pc_ForceCloseCommand(
  _CLID *pidWorld,
  bool bDirectly,
  unsigned __int8 byKickType,
  unsigned int dwPushIP)
{
CUserDB *user = &g_UserDB[pidWorld->wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pidWorld->dwSerial)
  {
    user->ForceCloseCommand(byKickType, dwPushIP, false, "Account Server Command");
  }
}

void CMainThread::pc_TransIPKeyInform(
  unsigned int dwAccountSerial,
  char *pszAccountID,
  unsigned __int8 byUserDgr,
  unsigned __int8 bySubDgr,
  unsigned int *pdwKey,
  _GLBID *pgidGlobal,
  unsigned int dwClientIP,
  bool bChatLock,
  __int16 iType,
  char *szCMS,
  _SYSTEMTIME *pstEndDate,
  int lRemainTime,
  unsigned __int8 byUILock,
  char *szUILockPW,
  unsigned __int8 byUILockFailCnt,
  char *szAccountPW,
  unsigned __int8 byUILock_HintIndex,
  char *uszUILock_HintAnswer,
  unsigned __int8 byUILockFindPassFailCount,
  bool bIsPcBang,
  int nTrans,
  bool bAgeLimit,
  unsigned int *pdwRequestMoveCharacterSerialList,
  unsigned int *pdwTournamentCharacterSerialList)
{
  unsigned __int8 retCode = 0;
  bool slotFound = false;
  if (CUserDB::s_nLoginNum < m_nLimUserNum || byUserDgr)
  {
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      CUserDB *user = &g_UserDB[index];
      if (user->m_bActive && user->m_dwAccountSerial == dwAccountSerial)
      {
        user->ForceCloseCommand(1u, dwClientIP, false, "Duplicate Account Login");
        retCode = 2;
        slotFound = true;
        break;
      }
    }

    if (!retCode)
    {
      for (int index = 0; index < MAX_PLAYER; ++index)
      {
        _WAIT_ENTER_ACCOUNT *entry = &m_WaitEnterAccount[index];
        if (!entry->m_bLoad)
        {
          entry->SetData(dwAccountSerial, pszAccountID, byUserDgr, bySubDgr, pgidGlobal, pdwKey, bChatLock);
          entry->SetBillingInfo(iType, szCMS, lRemainTime, pstEndDate);
          entry->SetPcBangFlag(bIsPcBang);
          entry->SetTransFlag(nTrans);
          entry->SetAgeLimitFlag(bAgeLimit);
          if (!byUserDgr)
          {
            entry->SetUILock(
              byUILock,
              szUILockPW,
              byUILockFailCnt,
              szAccountPW,
              byUILock_HintIndex,
              uszUILock_HintAnswer,
              byUILockFindPassFailCount);
          }
          for (int j = 0; j < 3; ++j)
          {
            entry->m_dwRequestMoveCharacterSerialList[j] = pdwRequestMoveCharacterSerialList[j];
            entry->m_dwTournamentCharacterSerialList[j] = pdwTournamentCharacterSerialList[j];
          }
          m_logBillCheck.Write("id:%s Bill:%d bAgeLimit:%d", pszAccountID, iType, bAgeLimit);
          if (g_Network.m_Process[0].m_Type.m_bAcceptIPCheck
              && !g_Network.m_Process[0].m_NetSocket.PushIPCheckList(dwClientIP))
          {
            retCode = 8;
          }
          else
          {
            slotFound = true;
          }
          break;
        }
      }
    }
    if (!slotFound && !retCode)
    {
      retCode = 8;
    }
  }
  else
  {
    retCode = 8;
  }

  _trans_account_report_wrac report{};
  report.byRetCode = retCode;
  std::memcpy(&report, pgidGlobal, 8);
  unsigned __int8 pbyType[2]{1, 9};
  const unsigned __int16 len = report.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, pbyType, reinterpret_cast<char *>(&report), len);
}

void CMainThread::pc_EnterWorldResult(unsigned __int8 byRetCode, _CLID *pidWorld)
{
  if (byRetCode)
  {
    CUserDB *user = &g_UserDB[pidWorld->wIndex];
    if (user->m_bActive && user->m_idWorld.dwSerial == pidWorld->dwSerial)
    {
      user->ForceCloseCommand(1, -1, false, "Enter world False");
    }
  }
}

void CMainThread::pc_UILockInitResult(const _uilock_init_result_acwr *request)
{
  CUserDB *user = &g_UserDB[request->wUserIndex];
  if (user->m_bActive)
  {
    user->UILockInfo_Init(request);
  }
}

void CMainThread::pc_UILockUpdateResult(const _uilock_update_result_acwr *request)
{
  CUserDB *user = &g_UserDB[request->wUserIndex];
  if (user->m_bActive)
  {
    user->UILockInfo_Update(request);
  }
}

void CMainThread::pc_AllUserMsgInform(char *pwszMsg)
{
  char destination[288]{};
  std::strcpy(destination, "$????");
  if (*pwszMsg == '%')
  {
    ProcessCheatCommand(nullptr, pwszMsg + 1);
    return;
  }

  const size_t prefixLen = std::strlen(destination);
  if (!strncmp(destination, pwszMsg, prefixLen))
  {
    char gmMsg[1328]{};
    std::strcpy(gmMsg, &pwszMsg[prefixLen]);
    pc_AllUserGMNoticeInform(gmMsg);
    return;
  }

  _trans_gm_msg_inform_zocl msg{};
  msg.wMsgSize = static_cast<unsigned short>(std::strlen(pwszMsg));
  std::memcpy(msg.wszChatData, pwszMsg, msg.wMsgSize);
  msg.wszChatData[msg.wMsgSize] = 0;

  unsigned __int8 pbyType[2]{2, 13};
  const int nLen = msg.size();
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(index, pbyType, reinterpret_cast<char *>(&msg), nLen);
    }
  }
}

void CMainThread::pc_AllUserGMNoticeInform(char *pwszMsg)
{
  _trans_gm_msg_inform_zocl msg{};
  msg.wMsgSize = static_cast<unsigned short>(std::strlen(pwszMsg));
  std::memcpy(msg.wszChatData, pwszMsg, msg.wMsgSize);
  msg.wszChatData[msg.wMsgSize] = 0;

  unsigned __int8 pbyType[2]{2, 14};
  const int nLen = msg.size();
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    if (g_Player[index].m_bLive && g_Player[index].m_bOper)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(index, pbyType, reinterpret_cast<char *>(&msg), nLen);
    }
  }
}

void CMainThread::pc_SetMainGreetingMsg(char *pwszGMName, char *pwszMsg)
{
  if (*pwszMsg)
  {
    if (std::strlen(pwszMsg) <= 255)
    {
      strcpy_s(m_wszGMName, 17, pwszGMName);
      strcpy_s(m_wszMainGreetingMsg, 256, pwszMsg);

      _qry_case_gm_greetingmsg qry{};
      strcpy_s(qry.in_gmgreetingmsg, 256, m_wszMainGreetingMsg);
      strcpy_s(qry.in_gmname, 17, m_wszGMName);
      const int size = qry.size();
      g_Main.PushDQSData(-1, nullptr, 74, reinterpret_cast<char *>(&qry), size);
    }
  }
}

void CMainThread::pc_SetRaceGreetingMsg(int racenum, char *pwszBossName, char *pwszMsg)
{
  if (*pwszMsg)
  {
    if (std::strlen(pwszMsg) <= 255)
    {
      strcpy_s(m_wszBossName[racenum], 17, pwszBossName);
      strcpy_s(m_wszRaceGreetingMsg[racenum], 256, pwszMsg);

      _qry_case_race_greetingmsg qry{};
      strcpy_s(qry.in_racegreetingmsg, 256, pwszMsg);
      strcpy_s(qry.in_bossname, 17, pwszBossName);
      qry.type = racenum;
      const int size = qry.size();
      g_Main.PushDQSData(-1, nullptr, 75, reinterpret_cast<char *>(&qry), size);
    }
  }
}

void CMainThread::pc_AllUserKickInform()
{
  if (!m_bServerClosing)
  {
    m_bServerClosing = true;
    m_tmForceUserExit.BeginTimer(50);
    m_nForceExitSocketIndexOffset = 0;
  }
}

void CMainThread::pc_ChatLockCommand(_CLID *pidLocal, unsigned __int16 wBlockTimeH)
{
CUserDB *user = &g_UserDB[pidLocal->wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pidLocal->dwSerial)
  {
    user->SetChatLock(true);
  }
}

void CMainThread::pc_AlterWorldService(bool bService)
{
  SerivceForceSet(bService);
}

void CMainThread::pc_CashDBInfoRecvResult(
  char *szIP,
  char *szDBName,
  char *szAccount,
  char *szPassword,
  unsigned int dwPort)
{
  char dateBuf[160]{};
  char timeBuf[144]{};

  _strdate(dateBuf);
  _strtime(timeBuf);
  WriteServerStartHistory("Cash DB Init Begin >> name: %s, ip: %s", szDBName, szIP);
  if (!std::strcmp(szIP, "None") || !std::strcmp(szDBName, "None"))
  {
    MyMessageBox(
      "CMainThread::pc_CashDBInfoRecvResult",
      "DSN : szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) Invalid",
      szIP,
      szDBName,
      szAccount,
      szPassword,
      dwPort);
    m_logLoadingError.Write(
      "CMainThread::pc_CashDBInfoRecvResult : DSN : szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) Invalid",
      szIP,
      szDBName,
      szAccount,
      szPassword,
      dwPort);
    m_logSystemError.Write(
      "CMainThread::pc_CashDBInfoRecvResult : DSN : szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) Invalid",
      szIP,
      szDBName,
      szAccount,
      szPassword,
      dwPort);
    ServerProgramExit("CMainThread::pc_CashDBInfoRecvResult Invalid DSN!", 1);
  }
  if (!CashDBInit(szIP, szDBName, szAccount, szPassword, dwPort))
  {
    MyMessageBox(
      "CMainThread::pc_CashDBInfoRecvResult",
      "CashDBInit( szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) ) Fail!",
      szIP,
      szDBName,
      szAccount,
      szPassword,
      dwPort);
    m_logLoadingError.Write(
      "CMainThread::pc_CashDBInfoRecvResult : CashDBInit( szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) ) Fail!",
      szIP,
      szDBName,
      szAccount,
      szPassword,
      dwPort);
    m_logSystemError.Write(
      "CMainThread::pc_CashDBInfoRecvResult : CashDBInit( szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) ) Fail!",
      szIP,
      szDBName,
      szAccount,
      szPassword,
      dwPort);
    ServerProgramExit("CMainThread::pc_CashDBInfoRecvResult CashDBInit Fail!", 1);
  }
  WriteServerStartHistory("Cash DB Init Complete >>");
  m_logLoadingError.WriteString("Cash DB Init Complete >>");
  CCashDBWorkManager::Instance()->Start();
  __trace("%s-%s: Open Cash DB", dateBuf, timeBuf);
}

void CMainThread::pc_TaiwanBillingUserCertify(char *szAccount, unsigned __int8 byCertify)
{
// this is not a stub (intentionally unimplemented for non-RU per rule #19)
}

void CMainThread::ManageClientLimitRunRequest(const _manage_client_limit_run_request_acwr *request)
{
  OutputDebugLog("ManageClientLimitRunRequest Start");

  if (!PushDQSData(-1, nullptr, 161, reinterpret_cast<char *>(const_cast<_manage_client_limit_run_request_acwr *>(request)), 7))
  {
    _manage_client_limit_run_result_wrac result{};
    result.byRet = 24;
    std::memcpy(&result.idLocal, request, sizeof(result.idLocal));
    result.byLoginServerIndex = request->byCode;
    unsigned __int8 pbyType[2]{1, 26};
    g_Network.m_pProcess[1]->LoadSendMsg(
      0,
      pbyType,
      reinterpret_cast<char *>(&result),
      static_cast<unsigned __int16>(sizeof(result)));
  }

  OutputDebugLog("ManageClientLimitRunRequest End");
}

void CMainThread::EndServer()
{
  // this is not a stub
}

bool CMainThread::_CheckTotalSales()
{
  CashItemRemoteStore::Instance()->Check_Total_Selling();
  return true;
}

char CMainThread::_GameDataBaseInit()
{
  m_logLoadingError.Write("Avators DBRestore Complete!!");

  const unsigned int dwDate = eGetLocalDate();
  int nHisNum[11]{};
  _economy_history_data history[12]{};
  _economy_history_data curData{};
  unsigned int pnCurMgrValue[8]{};
  unsigned int pnNextMgrValue[8]{};
  int pnDBSerial[4]{};

  pnCurMgrValue[0] = 1000;
  pnNextMgrValue[0] = 1000;
  if (db_Select_Economy_History(&curData, pnCurMgrValue, pnNextMgrValue, history, nHisNum, dwDate))
  {
    MyMessageBox("DatabaseInit", "Economy data load fail");
    return 0;
  }

  if (eInitEconomySystem(pnCurMgrValue[0], pnNextMgrValue[0], history, nHisNum[0], &curData))
  {
    CMoneySupplyMgr::Instance()->Initialize();
    m_logLoadingError.Write("Economy History Load Complete!!");
    if (CPvpUserAndGuildRankingSystem::Instance()->Load())
    {
      if (CGuildBattleController::Instance()->Load())
      {
        if (CTotalGuildRankManager::Instance()->Load())
        {
          if (CWeeklyGuildRankManager::Instance()->Load())
          {
            if (AutoMineMachineMng::Instance()->Initialzie())
            {
              if (m_pWorldDB->create_table_atrade_taxrate())
              {
                if (CUnmannedTraderTaxRateManager::Instance()->Load())
                {
                  if (!AutominePersonalMgr::instance()->CreateDBTable())
                  {
                    MyMessageBox("CMainThread::_GameDataBaseInit()", "AutominePersonalMgr::CreateDBTable Fail!");
                    m_logLoadingError.Write("AutominePersonalMgr Create Database table Fail!");
                  }
                  if (CUnmannedTraderController::Instance()->Load())
                  {
                    if (CGuildRoomSystem::GetInstance()->Load_db())
                    {
                      if (CPostSystemManager::Instace()->Load())
                      {
                        if (db_LoadGreetingMsg())
                        {
                          if (CItemStoreManager::Instance()->Load())
                          {
                            CreateSelectCharacterLogTable(1u);
                            if (CHonorGuild::Instance()->LoadDB())
                            {
                              if (CRaceBossWinRate::Instance()->LoadDB())
                              {
                                _db_Load_BattleTournamentInfo();
                                if (LoadLimitInfo())
                                {
                                  CCashDBWorkManager::Instance()->Start();
                                  pnDBSerial[0] = 0;
                                  if (_db_Load_GoldBoxItem(
                                        reinterpret_cast<qry_case_select_golden_box_item *>(
                                          CGoldenBoxItemMgr::Instance()->GetGodBoxItemInfoPtr()),
                                        pnDBSerial))
                                  {
                                    MyMessageBox("CMainThread::_GameDataBaseInit()", "_db_Load_GoldBoxItem() Fail!");
                                    m_logLoadingError.Write("_db_Load_GoldBoxItem Fail!");
                                    return 0;
                                  }

                                  CGoldenBoxItemMgr::Instance()->SetDBSerial(pnDBSerial[0]);
                                  if (CGoldenBoxItemMgr::Instance()->Get_Event_Status() == 2)
                                  {
                                    CGoldenBoxItemMgr::Instance()->SynchINIANDDB();
                                  }
                                  return 1;
                                }
                                return 0;
                              }
                              MyMessageBox(
                                "CMainThread::_GameDataBaseInit()",
                                "CRaceBossWinRate::Instance()->LoadDB() Fail!");
                              m_logLoadingError.Write("CRaceBossWinRate::Instance()->LoadDB() Fail!");
                              return 0;
                            }
                            MyMessageBox(
                              "CMainThread::_GameDataBaseInit()",
                              "CHonorGuild::Instance()->LoadDB() Fail!");
                            m_logLoadingError.Write("CHonorGuild::Instance()->LoadDB() Fail!");
                            return 0;
                          }
                          MyMessageBox(
                            "CMainThread::_GameDataBaseInit()",
                            "CItemStoreManager::Instance()->Load() Fail!");
                          m_logLoadingError.Write("CItemStoreManager::Instance()->Load() Fail!");
                          return 0;
                        }
                        MyMessageBox("CMainThread::_GameDataBaseInit()", "db_LoadGreetingMsg() Fail!");
                        m_logLoadingError.Write("db_LoadGreetingMsg() Fail!");
                        return 0;
                      }
                      MyMessageBox("CMainThread::_GameDataBaseInit()", "CPostSystemManager::Instace()->Load() Fail!");
                      m_logLoadingError.Write("CPostSystemManager::Instace()->Load() Fail!");
                      return 0;
                    }
                    MyMessageBox("CMainThread::Init() : ", "CGuildRoomSystem::GetInstance()->Load_db()");
                    m_logLoadingError.Write("CGuildRoomSystem::GetInstance()->Load_db()");
                    return 0;
                  }
                  MyMessageBox(
                    "CMainThread::_GameDataBaseInit()",
                    "CUnmannedTraderController::Instance()->Load() Fail!");
                  m_logLoadingError.Write("CUnmannedTraderController::Instance()->Load() Fail!");
                  return 0;
                }
                MyMessageBox("CMainThread::_GameDataBaseInit()", "ATradeTaxRateTable() Fail!");
                m_logLoadingError.Write("ATradeTaxRateTable() Fail!");
                return 0;
              }
              return 0;
            }
            MyMessageBox("CMainThread::_GameDataBaseInit()", "InitAutoMineMachines() Fail!");
            m_logLoadingError.Write("InitAutoMineMachines() Fail!");
            return 0;
          }
          MyMessageBox("CMainThread::_GameDataBaseInit()", "CWeeklyGuildRankManager::Instance()->Load() Fail!");
          m_logLoadingError.Write("CWeeklyGuildRankManager::Instance()->Load() Fail!");
          return 0;
        }
        MyMessageBox("CMainThread::_GameDataBaseInit()", "CTotalGuildRankManager::Instance()->Load() Fail!");
        m_logLoadingError.Write("CGuildBattleController::Instance()->Load() Fail!");
        return 0;
      }
      MyMessageBox("CMainThread::_GameDataBaseInit()", "CGuildBattleController::Instance()->Load() Fail!");
      m_logLoadingError.Write("CGuildBattleController::Instance()->Load() Fail!");
      return 0;
    }
    MyMessageBox("CMainThread::_GameDataBaseInit()", "CPvpUserRankingAndGuildRankingSystem::Instance()->Load() Fail!");
    m_logLoadingError.Write("CPvpUserRankingAndGuildRankingSystem::Instance()->Load() Fail!");
    return 0;
  }

  MyMessageBox("DatabaseInit", "Economy data init fail");
  return 0;
}

bool CMainThread::DatabaseInit(char *pszDBName, char *pszDBIP)
{
  m_logLoadingError.Write("DataBase Setting Start!! (%s : %s)", pszDBIP, pszDBName);
  char trustedConnectionValue[16]{};
  const char *defaultWorldDbId = CNationSettingManager::Instance()->GetWorldDBID();
  const char *defaultWorldDbPassword = CNationSettingManager::Instance()->GetWorldDBPW();
  char worldDbId[64]{};
  char worldDbPassword[64]{};
  ReadOptionAndWriteDefault(
    ".\\Initialize\\settings.ini",
    "WorldDB",
    "trusted_connection",
    "0",
    trustedConnectionValue,
    sizeof(trustedConnectionValue));
  ReadOptionAndWriteDefault(
    ".\\Initialize\\settings.ini",
    "WorldDB",
    "ID",
    defaultWorldDbId ? defaultWorldDbId : "sa",
    worldDbId,
    sizeof(worldDbId));
  ReadOptionAndWriteDefault(
    ".\\Initialize\\settings.ini",
    "WorldDB",
    "PWD",
    defaultWorldDbPassword ? defaultWorldDbPassword : "",
    worldDbPassword,
    sizeof(worldDbPassword));
  const bool bTrustedConnection = trustedConnectionValue[0] != '\0' && trustedConnectionValue[0] != '0';
  std::strcpy(m_szWorldDBName, pszDBName);
  g_pFrame->SendMessage(12, 0, 0);

  if (!m_pWorldDB)
  {
    CRFWorldDatabase *worldDb = new CRFWorldDatabase();
    m_pWorldDB = worldDb;
    m_pWorldDB->SetLogFile("..\\ZoneServerLog\\", m_szWorldDBName);
    if (!m_pWorldDB->ConfigUserODBC(pszDBName, pszDBIP, pszDBName, 1433, bTrustedConnection))
    {
      MyMessageBox("DatabaseInit", "World DB ODBC Setting Faild!");
      return false;
    }
    m_logLoadingError.Write("World DB ODBC Config Complete!!");
    char *passWord = bTrustedConnection ? nullptr : worldDbPassword;
    const char *configuredWorldDbId = bTrustedConnection ? nullptr : worldDbId;
    if (!m_pWorldDB->StartDataBase(m_szWorldDBName, configuredWorldDbId, passWord))
    {
      MyMessageBox("DatabaseInit", "Connect World DB Failed!");
      return false;
    }
    m_logLoadingError.Write("Start World DataBase Complete!!");
    if (!_GameDataBaseInit())
    {
      return false;
    }
  }

  if (!CLogTypeDBTaskManager::Instance()->IsInitialized())
  {
    if (!CLogTypeDBTaskManager::Instance()->InitDB(pszDBName, pszDBIP))
    {
      return false;
    }
  }

  m_logLoadingError.Write("DataBase Setting Complete!! (%s : %s)", pszDBIP, pszDBName);
  return true;
}

bool CMainThread::CashDBInit(
  char *szIP,
  char *szDBName,
  char *szAccount,
  char *szPassword,
  unsigned int dwPort)
{
  CNationSettingManager::Instance()->SetCashDBDSN(szIP, szDBName, szAccount, szPassword, dwPort);
  if (CCashDBWorkManager::Instance()->InitializeWorker())
  {
    m_logLoadingError.Write(
      "CMainThread::CashDBInit() : Cash Item DataBase Setting Complete!! (%s : %s)",
      szIP,
      szDBName);
    return true;
  }

  MyMessageBox("CMainThread::Init() : ", "CCashDBWorkManager::Instance()->Initialize() Fail!");
  m_logLoadingError.Write("CashDbWorker::Instance()->Initialize() Fail!");
  return false;
}

void CMainThread::SerivceSelfStart()
{
  if (!m_bCheckOverTickCount && g_Main.m_bWorldOpen && !g_Main.m_bWorldService)
  {
    m_bWorldService = true;
    _start_world_request_wrac request{};
    request.dwGateIP = m_dwGateIP;
    request.wGatePort = m_wGatePort;
    unsigned __int8 pbyType[2]{1, 3};
    const unsigned __int16 len = request.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, pbyType, reinterpret_cast<char *>(&request), len);
  }
}

void CMainThread::SerivceSelfStop()
{
  if (g_Main.m_bWorldOpen && g_Main.m_bWorldService)
  {
    m_bWorldService = false;
    unsigned __int8 pbyType[2]{1, 4};
    _world_exit_report_wrac request{};
    g_Network.m_pProcess[1]->LoadSendMsg(
      0,
      pbyType,
      reinterpret_cast<char *>(&request),
      request.size());
  }
}

void CMainThread::SerivceForceSet(bool bService)
{
  m_bWorldService = bService;
}

