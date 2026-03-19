#include "pch.h"

#include "AutominePersonalMgr.h"

#include "CMainThread.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CMapData.h"
#include "COreAmountMgr.h"
#include "CRFWorldDatabase.h"
#include "CUserDB.h"
#include "KorLocalTime.h"
#include "GlobalObjects.h"
#include "DqsDbStructs.h"
#include "CMgrAvatorItemHistory.h"
#include "Packet/ClientZonePacket.h"
#include "personal_amine_errmsg_zocl.h"
#include "personal_amine_infoui_open_zocl.h"
#include "personal_amine_make_storage_zocl.h"
#include "personal_automine_battery_extract_zocl.h"
#include "personal_automine_battery_insert_zocl.h"
#include "personal_automine_popore_zocl.h"
#include "personal_automine_selore_zocl.h"
#include "personal_automine_stop_zocl.h"
#include "qry_case_make_storage.h"
#include "qry_case_make_storage_update.h"

#include <cstdio>
#include <cstring>

namespace
{
  const char *s_szAMPOreKind[5] = {
    "ioblu03",
    "iored03",
    "ioyel03",
    "iogre03",
    "iobla03",
  };
}

AutominePersonalMgr *AutominePersonalMgr::m_pInstance = nullptr;

AutominePersonalMgr::AutominePersonalMgr()
{
  m_dwObjSerial = 0;
}

AutominePersonalMgr *AutominePersonalMgr::instance()
{
  if (!m_pInstance)
  {
    m_pInstance = new AutominePersonalMgr();
  }
  return m_pInstance;
}

void AutominePersonalMgr::release()
{
  if (m_pInstance)
  {
    if (m_pInstance->m_pMachine)
    {
      delete[] m_pInstance->m_pMachine;
      m_pInstance->m_pMachine = nullptr;
    }

    delete m_pInstance;
    m_pInstance = nullptr;
  }
}

bool AutominePersonalMgr::initialize()
{
  char dest[128]{};
  const unsigned int now = static_cast<unsigned int>(GetKorLocalTime());
  std::memset(dest, 0, sizeof(dest));

  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\SystemLog\\Concession\\ampersonalmgr_%u.log", now);
  m_logService.SetWriteLogFile(dest, 1, 0, 1, 1);

  const unsigned int now2 = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\systemlog\\log_ampersonalmgr_%u.log", now2);
  m_logError.SetWriteLogFile(dest, 1, 0, 1, 1);

  for (int j = 0; j < 5; ++j)
  {
    unsigned __int16 *record =
      reinterpret_cast<unsigned __int16 *>(g_Main.m_tblItemData[17].GetRecord(s_szAMPOreKind[j]));
    if (record == nullptr)
    {
      return false;
    }
    m_wOreIndex[j] = *record;
  }

  if (init_objects())
  {
    return true;
  }

  m_logError.Write("AutominePersonalMgr::initialize() >> init_objects() failed.");
  return false;
}

bool AutominePersonalMgr::init_objects()
{
  m_pMachine = new AutominePersonal[MAX_PLAYER];
  if (m_pMachine == nullptr)
  {
    m_logError.Write(
      "AutominePersonalMgr::init_Objects() >> Failed allocate memory of AutoMinePersonal[MAX_PLAYER].");
    return false;
  }

  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    if (!m_pMachine[j].initialize(static_cast<unsigned __int16>(j)))
    {
      m_logError.Write(
        "AutominePersonalMgr::init_Objects() >> m_Machine[%d].initialize(%d) failed",
        j,
        j);
      return false;
    }
  }

  return true;
}

bool AutominePersonalMgr::CreateDBTable()
{
  if (g_Main.m_pWorldDB)
  {
    return g_Main.m_pWorldDB->TableExist("tbl_aminepersonal_inven")
        || g_Main.m_pWorldDB->create_amine_personal();
  }

  m_logError.Write("AutominePersonalMgr::CreateDBTable() >> g_Main.m_pWorldDB is NULL.");
  return false;
}

AutominePersonal *AutominePersonalMgr::get_machine(unsigned int nIdx)
{
  if (nIdx >= MAX_PLAYER)
  {
    return nullptr;
  }
  return &m_pMachine[nIdx];
}

bool AutominePersonalMgr::db_load_inven(unsigned int dwSerial, _PERSONALAMINE_INVEN_DB_BASE *pCon)
{
  if (!pCon)
  {
    return false;
  }

  pCon->Init();

  _personal_amine_inven inven{};
  if (g_Main.m_pWorldDB->select_amine_personal(dwSerial, &inven))
  {
    return false;
  }

  for (int j = 0; j < 40; ++j)
  {
    if (inven.list[j].lK == -1)
    {
      pCon->m_List[j].Key.SetRelease();
      pCon->m_List[j].dwDur = 0;
    }
    else
    {
      std::memcpy(&pCon->m_List[j], &inven.list[j], 4u);
      pCon->m_List[j].dwDur = inven.list[j].byNum;
    }
  }
  return true;
}

bool AutominePersonalMgr::make_storagebox(int n, const _personal_amine_make_storage_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (!player->m_Param.m_pAPM)
  {
    return false;
  }

  if (player->m_Param.BeHaveBoxOfAMP())
  {
    send_ecode(n, 19u);
    return true;
  }

  if (player->IsMineMode())
  {
    send_ecode(n, 3u);
    return true;
  }

  if (!request || request->dwAvatorSerial != player->m_dwObjSerial)
  {
    send_ecode(n, 22u);
    return true;
  }

  if (player->m_Param.GetGold() < 250u)
  {
    send_ecode(n, 23u);
    return true;
  }

  player->AlterGold(-250.0);
  push_dqs_makestorage(n, player->m_dwObjSerial, player->m_Param.GetGold());
  return true;
}

void AutominePersonalMgr::push_dqs_makestorage(int n, unsigned int dwAvatorSerial, unsigned int dwTotGold)
{
_qry_case_make_storage qry{};
  qry.nSockIdx = n;
  qry.dwAvatorSerial = dwAvatorSerial;

  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 56u, reinterpret_cast<char *>(&qry), size);
}

int AutominePersonalMgr::request_query(const _qry_case_make_storage *query)
{
  if (!query)
  {
    return 24;
  }

  if (query->bySubQry)
  {
    if (query->bySubQry == 1)
    {
      const auto *updateQuery = reinterpret_cast<const _qry_case_make_storage_update_header *>(query);
      const auto *entries = updateQuery->entries();
      char buffer[10244]{};
      std::memset(buffer, 0, 10240);
      sprintf_s(buffer, sizeof(buffer), "update [dbo].[tbl_aminepersonal_inven] set ");

      int len = static_cast<int>(std::strlen(buffer));
      for (int j = 0; j < updateQuery->byChangeCount; ++j)
      {
        const _qry_case_make_storage_update_entry &entry = entries[j];
        char *dest = &buffer[len];
        const size_t remaining = sizeof(buffer) - static_cast<size_t>(len);
        sprintf_s(dest, remaining, ",K%d=%d,N%d=%d,", entry.bySlot, entry.dwItemKey, entry.bySlot, entry.byAmount);
      }

      len = static_cast<int>(std::strlen(buffer));
      if (len > 0)
      {
        const size_t remaining = sizeof(buffer) - static_cast<size_t>(len - 1);
        sprintf_s(&buffer[len - 1], remaining, " where avatorserial = %d", updateQuery->base.dwAvatorSerial);
      }
      if (g_Main.m_pWorldDB->update_amine_personal(buffer))
      {
        return 0;
      }
      return 24;
    }
    return 24;
  }

  if (g_Main.m_pWorldDB->select_amine_personal(query->dwAvatorSerial) != 2
      || g_Main.m_pWorldDB->insert_amine_personal(query->dwAvatorSerial))
  {
    return 0;
  }
  return 24;
}

void AutominePersonalMgr::result_query(unsigned __int8 byRet, const _qry_case_make_storage *query)
{
  if (!query)
  {
    return;
  }

  if (!query->bySubQry)
  {
    pop_dqs_makestorage(byRet, query);
  }
}

void AutominePersonalMgr::pop_dqs_makestorage(unsigned __int8 byRet, const _qry_case_make_storage *query)
{
  CPlayer *player = &g_Player[query->nSockIdx];
  if (player->m_bOper)
  {
    unsigned int charSerial = player->m_Param.GetCharSerial();
    if (charSerial == query->dwAvatorSerial)
    {
      if (byRet == 24)
      {
        player->AlterGold(250.0);
        send_ecode(player->m_ObjID.m_wIndex, 17u);
      }
      else
      {
        player->m_Param.SetHaveBoxOfAMP(true);
        _personal_amine_make_storage_zocl msg{};
        msg.dwAvatorSerial = player->m_Param.GetCharSerial();
        msg.dwTotGold = player->m_Param.GetGold();
        unsigned __int8 pbyType[2] = {14, 43};
        const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
        g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), nLen);
      }
    }
  }
}

void AutominePersonalMgr::send_ecode(unsigned int n, unsigned __int8 byCode)
{
  _personal_amine_errmsg_zocl msg{};
  msg.byErrCode = byCode;
  unsigned __int8 pbyType[2] = {14, 62};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&msg), nLen);
}

bool AutominePersonalMgr::check_dummy(CMapData *pMap, unsigned __int8 byCurDummyIndex, float *pfCurPos)
{
  if (pMap == nullptr)
  {
    return false;
  }

  if (byCurDummyIndex >= pMap->m_nResDumNum)
  {
    return false;
  }

  return pMap->GetResDummySector(byCurDummyIndex, pfCurPos) != -1;
}

bool AutominePersonalMgr::check_machine(
  int n,
  unsigned int dwSerial,
  AutominePersonal *pMachine,
  _STORAGE_LIST::_db_con *pcitem)
{
  if (pMachine == nullptr)
  {
    m_logError.Write("AutominePersonalMgr::check_machine() >> not exist machine");
    return false;
  }

  if (!pMachine->is_installed())
  {
    send_ecode(n, 4u);
    return false;
  }

  if (pMachine->get_objserial() != dwSerial)
  {
    send_ecode(n, 18u);
    return false;
  }

  if (pcitem && pMachine->get_item() != pcitem)
  {
    send_ecode(n, 5u);
    return false;
  }

  return true;
}

_STORAGE_LIST::_db_con *AutominePersonalMgr::get_localitem(CPlayer *pOne, unsigned __int16 wItemSerial)
{
  if (pOne == nullptr || pOne->m_Param.m_pStoragePtr[0] == nullptr)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *item = pOne->m_Param.m_pStoragePtr[0]->GetPtrFromSerial(wItemSerial);
  if (item == nullptr || item->m_byTableCode != 33)
  {
    return nullptr;
  }
  return item;
}

bool AutominePersonalMgr::install(int n, const _personal_automine_install_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(player->m_id.wIndex) == 99)
  {
    send_ecode(n, 33u);
    return true;
  }

  if (!player->m_Param.BeHaveBoxOfAMP())
  {
    send_ecode(n, 20u);
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (machine == nullptr)
  {
    m_logError.Write("!CRITICAL! AutominePersonalMgr::install() [LINE:%d]", 227);
    return false;
  }

  if (machine->is_installed())
  {
    send_ecode(n, 3u);
    return true;
  }

  if (!COreAmountMgr::Instance()->IsOreRemain())
  {
    send_ecode(n, 27u);
    return true;
  }

  if (request == nullptr || !check_dummy(player->m_pCurMap, static_cast<unsigned __int8>(request->byMineDummyIndex), player->m_fCurPos))
  {
    send_ecode(n, 1u);
    return true;
  }

  _res_dummy *resDummy = &player->m_pCurMap->m_pResDummy[static_cast<unsigned __int8>(request->byMineDummyIndex)];
  if (resDummy->GetQualityGrade() != 2 && !player->IsMiningByMinigTicket())
  {
    send_ecode(n, 26u);
    return true;
  }

  _STORAGE_LIST::_db_con *installItem = get_localitem(player, request->wItemSerial);
  if (installItem == nullptr || installItem->m_byTableCode != 33)
  {
    send_ecode(n, 5u);
    return true;
  }

  const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(player->m_id.wIndex);
  const float effDelay = player->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_7) * player->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_37);
  const float delayProf = effDelay * static_cast<float>(playerPenalty);

  if (machine->regist_to_map(
        player,
        installItem,
        static_cast<unsigned __int8>(request->byMineDummyIndex),
        m_dwObjSerial,
        delayProf))
  {
    ++m_dwObjSerial;
  }
  else
  {
    send_ecode(n, 15u);
  }

  return true;
}

bool AutominePersonalMgr::uninstall(int n, const _personal_automine_uninstall_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  _STORAGE_LIST::_db_con *item = get_localitem(player, request->wItemSerial);
  if (!check_machine(n, request->dwObjSerial, machine, item))
  {
    return true;
  }

  if (!machine->unregist_from_map(0))
  {
    send_ecode(n, 16u);
    m_logError.Write("AutominePersonalMgr::uninstall() [LINE:%d]", 314);
  }
  return true;
}

bool AutominePersonalMgr::uninstall(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (machine == nullptr)
  {
    return false;
  }

  const unsigned __int16 objSerial16 = static_cast<unsigned __int16>(machine->get_objserial());
  _STORAGE_LIST::_db_con *item = get_localitem(player, objSerial16);
  if (!check_machine(n, machine->get_objserial(), machine, item))
  {
    return true;
  }

  if (!machine->unregist_from_map(0))
  {
    send_ecode(n, 16u);
    m_logError.Write("AutominePersonalMgr::uninstall() [LINE:%d]", 548);
  }
  return true;
}

bool AutominePersonalMgr::selectore(unsigned int n, const _personal_automine_selore_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), request->dwObjSerial, machine, nullptr))
  {
    return true;
  }

  const unsigned __int8 oreSlot = static_cast<unsigned __int8>(request->bySelectOre);
  if (oreSlot >= 5u)
  {
    send_ecode(n, 9u);
    return true;
  }

  machine->set_selore(static_cast<unsigned __int8>(m_wOreIndex[oreSlot]));

  _personal_automine_selore_zocl msg{};
  msg.dwObjSerial = machine->get_objserial();
  msg.bySelectOre = oreSlot;

  unsigned __int8 type[2] = {14, 49};
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));
  return true;
}

bool AutominePersonalMgr::insert_battery(unsigned int n, const _personal_automine_battery_insert_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), request->dwObjSerial, machine, nullptr))
  {
    return true;
  }

  if (!machine->insert_battery(
        static_cast<unsigned __int8>(request->byInPos),
        request->wItemSerial))
  {
    send_ecode(n, 24u);
    return true;
  }

  _personal_automine_battery_insert_zocl msg{};
  msg.dwObjSerial = machine->get_objserial();
  msg.dwOwnerSerial = machine->get_ownerserial();
  msg.wItemSerial = machine->get_itemserial();

  unsigned __int8 type[2] = {14, 51};
  machine->CircleReport(
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()),
    player->m_dwObjSerial,
    false);
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));
  return true;
}

bool AutominePersonalMgr::extract_battery(unsigned int n, const _personal_automine_battery_extract_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), request->dwObjSerial, machine, nullptr))
  {
    return true;
  }

  _STORAGE_LIST::_db_con battery{};
  machine->extract_battery(static_cast<unsigned __int8>(request->byPos), &battery);
  battery.m_byClientIndex = static_cast<unsigned __int8>(request->bySlotIdx);

  _STORAGE_LIST::_db_con *added = player->Emb_AddStorage(0, &battery, false, false);
  if (added == nullptr)
  {
    _personal_automine_battery_extract_zocl fail{};
    fail.dwObjSerial = machine->get_objserial();
    fail.byRetCode = 1;
    unsigned __int8 type[2] = {14, 53};
    g_Network.m_pProcess[0]->LoadSendMsg(
      static_cast<int>(n),
      type,
      reinterpret_cast<char *>(&fail),
      static_cast<unsigned __int16>(fail.size()));
    return true;
  }

  _personal_automine_battery_extract_zocl ok{};
  ok.dwObjSerial = machine->get_objserial();
  ok.wSerial = added->m_wSerial;
  ok.dwDur = static_cast<unsigned int>(added->m_dwDur);
  ok.byRetCode = 0;

  unsigned __int8 type[2] = {14, 53};
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&ok),
    static_cast<unsigned __int16>(ok.size()));

  if (machine->get_battery() == 0)
  {
    machine->set_work(false);
    _personal_automine_stop_zocl stopMsg{};
    stopMsg.dwObjSerial = machine->get_objserial();
    stopMsg.dwOwnerSerial = machine->get_ownerserial();
    stopMsg.byStopType = 1;
    stopMsg.wItemSerial = machine->get_itemserial();
    unsigned __int8 stopType[2] = {14, 58};
    machine->CircleReport(
      stopType,
      reinterpret_cast<char *>(&stopMsg),
      static_cast<unsigned __int16>(stopMsg.size()),
      player->m_dwObjSerial,
      false);
    g_Network.m_pProcess[0]->LoadSendMsg(
      static_cast<int>(n),
      stopType,
      reinterpret_cast<char *>(&stopMsg),
      static_cast<unsigned __int16>(stopMsg.size()));
  }

  CPlayer *owner = machine->get_owner();
  CPlayer::s_MgrItemHistory.personal_amine_itemlog(
    "MOVE_TO_INVEN",
    added->m_byStorageIndex,
    added->m_byTableCode,
    added->m_wItemIndex,
    static_cast<unsigned int>(added->m_dwDur),
    owner->m_szItemHistoryFileName);
  return true;
}

bool AutominePersonalMgr::extract_battery(unsigned int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), machine->get_objserial(), machine, nullptr))
  {
    return true;
  }

  _STORAGE_LIST::_db_con battery{};
  for (int slot = 0; slot < 2; ++slot)
  {
    if (!machine->get_battery(slot))
    {
      continue;
    }

    machine->extract_battery(static_cast<unsigned __int8>(slot), &battery);
    battery.m_byClientIndex = static_cast<unsigned __int8>(slot);

    _STORAGE_LIST::_db_con *added = player->Emb_AddStorage(0, &battery, false, false);
    if (added == nullptr)
    {
      _personal_automine_battery_extract_zocl fail{};
      fail.dwObjSerial = machine->get_objserial();
      fail.byRetCode = 1;
      unsigned __int8 type[2] = {14, 53};
      g_Network.m_pProcess[0]->LoadSendMsg(
        static_cast<int>(n),
        type,
        reinterpret_cast<char *>(&fail),
        static_cast<unsigned __int16>(fail.size()));
      return true;
    }

    _personal_automine_battery_extract_zocl ok{};
    ok.dwObjSerial = machine->get_objserial();
    ok.wSerial = added->m_wSerial;
    ok.dwDur = static_cast<unsigned int>(added->m_dwDur);
    ok.byRetCode = 0;

    unsigned __int8 type[2] = {14, 53};
    g_Network.m_pProcess[0]->LoadSendMsg(
      static_cast<int>(n),
      type,
      reinterpret_cast<char *>(&ok),
      static_cast<unsigned __int16>(ok.size()));

    if (machine->get_battery() == 0)
    {
      machine->set_work(false);
      _personal_automine_stop_zocl stopMsg{};
      stopMsg.dwObjSerial = machine->get_objserial();
      stopMsg.dwOwnerSerial = machine->get_ownerserial();
      stopMsg.byStopType = 1;
      stopMsg.wItemSerial = machine->get_itemserial();

      unsigned __int8 stopType[2] = {14, 58};
      machine->CircleReport(
        stopType,
        reinterpret_cast<char *>(&stopMsg),
        static_cast<unsigned __int16>(stopMsg.size()),
        player->m_dwObjSerial,
        false);
      g_Network.m_pProcess[0]->LoadSendMsg(
        static_cast<int>(n),
        stopType,
        reinterpret_cast<char *>(&stopMsg),
        static_cast<unsigned __int16>(stopMsg.size()));
    }

    CPlayer *owner = machine->get_owner();
  CPlayer::s_MgrItemHistory.personal_amine_itemlog(
    "MOVE_TO_INVEN",
    added->m_byStorageIndex,
    added->m_byTableCode,
    added->m_wItemIndex,
    static_cast<unsigned int>(added->m_dwDur),
    owner->m_szItemHistoryFileName);
  }
  return true;
}

bool AutominePersonalMgr::pop_ore(unsigned int n, const _personal_automine_popore_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (player->m_Param.m_pStoragePtr[0]->GetIndexEmptyCon() == 255)
  {
    send_ecode(n, 21u);
    return true;
  }

  _STORAGE_LIST::_storage_con *sourceOre =
    player->m_Param.m_pStoragePtr[6]->GetPtrFromSerial(request->wItemSerial);
  if (sourceOre == nullptr)
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Is not exist item(%d)", request->wItemSerial);
    return true;
  }

  _STORAGE_LIST::_db_con *added = player->Emb_AddStorage(0, sourceOre, false, false);
  if (added == nullptr)
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Failed CPlayer::Emb_AddStorage()");
    return true;
  }

  added->m_byClientIndex = static_cast<unsigned __int8>(request->byStorageIndex_R);
  if (!player->m_pUserDB->Update_ItemSlot(
        0,
        added->m_byStorageIndex,
        static_cast<unsigned __int8>(request->byStorageIndex_R)))
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Failed CPlayer::CUserDB::Update_ItemSlot()");
    return true;
  }

  if (!player->Emb_DelStorage(6, sourceOre->m_byClientIndex, false, true, "AutominePersonalMgr::pop_ore()"))
  {
    player->Emb_DelStorage(0, sourceOre->m_byClientIndex, false, false, "AutominePersonalMgr::pop_ore() - Delete Fail");
    return true;
  }

  _personal_automine_popore_zocl msg{};
  msg.wItemSerial = request->wItemSerial;
  msg.byNum = request->byNum;
  unsigned __int8 type[2] = {14, 57};
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (machine->is_installed())
  {
    if (machine->get_battery() && !machine->is_run())
    {
      machine->set_work(true);
    }
    machine->sub_filledslot();
    machine->send_current_state();
  }

  CPlayer::s_MgrItemHistory.personal_amine_itemlog(
    "POP_ORE",
    added->m_byStorageIndex,
    added->m_byTableCode,
    added->m_wItemIndex,
    static_cast<unsigned int>(added->m_dwDur),
    player->m_szItemHistoryFileName);
  return true;
}

bool AutominePersonalMgr::Open_InvenUI(int n, const _personal_amine_infoui_open_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (player->m_Param.m_pAPM)
  {
    player->m_Param.m_pAPM->set_openUI_Inven(request->bIsOpen);
    player->m_Param.m_pAPM->send_changed_packet(static_cast<unsigned int>(n));
  }
  return true;
}

bool AutominePersonalMgr::Open_InfoUI(unsigned int n, const _personal_amine_infoui_open_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  machine->set_openUI_battery(request->bIsOpen);

  _personal_amine_infoui_open_zocl msg{};
  msg.dwObjSerial = machine->get_objserial();
  msg.dwBattery[0] = machine->get_battery(0);
  msg.dwBattery[1] = machine->get_battery(1);

  unsigned __int8 type[2] = {14, 64};
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));
  return true;
}

bool AutominePersonalMgr::Is_MineRun(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  return machine && machine->is_run();
}

