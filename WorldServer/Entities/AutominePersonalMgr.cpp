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
#include "personal_amine_errmsg_zocl.h"
#include "personal_amine_infoui_open_zocl.h"
#include "personal_amine_make_storage_zocl.h"
#include "personal_automine_battery_extract_zocl.h"
#include "personal_automine_battery_insert_zocl.h"
#include "personal_automine_popore_zocl.h"
#include "personal_automine_selore_zocl.h"
#include "personal_automine_stop_zocl.h"
#include "qry_case_make_storage.h"

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

AutominePersonalMgr *AutominePersonalMgr::instance()
{
  static AutominePersonalMgr s_instance;
  return &s_instance;
}

bool AutominePersonalMgr::initialize()
{
  char dest[128]{};
  const unsigned int now = GetKorLocalTime();
  std::memset(dest, 0, sizeof(dest));

  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\SystemLog\\Concession\\ampersonalmgr_%u.log", now);
  m_logService.SetWriteLogFile(dest, 1, 0, 1, 1);

  const unsigned int now2 = GetKorLocalTime();
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

char AutominePersonalMgr::db_load_inven(unsigned int dwSerial, _PERSONALAMINE_INVEN_DB_BASE *pCon)
{
  _personal_amine_inven inven{};
  if (g_Main.m_pWorldDB->select_amine_personal(dwSerial, &inven))
  {
    return 0;
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
      memcpy_0(&pCon->m_List[j], &inven.list[j], 4u);
      pCon->m_List[j].dwDur = inven.list[j].byNum;
    }
  }
  return 1;
}

char AutominePersonalMgr::make_storagebox(int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  if (!player->m_Param.m_pAPM)
  {
    return 0;
  }

  if (player->m_Param.BeHaveBoxOfAMP())
  {
    send_ecode(n, 0x13u);
    return 1;
  }

  if (player->m_bMineMode)
  {
    send_ecode(n, 3u);
    return 1;
  }

  if (!pmsg || *reinterpret_cast<unsigned int *>(pmsg) != player->m_dwObjSerial)
  {
    send_ecode(n, 0x16u);
    return 1;
  }

  if (player->m_Param.GetGold() < 250u)
  {
    send_ecode(n, 0x17u);
    return 1;
  }

  player->AlterGold(-250.0);
  push_dqs_makestorage(n, player->m_dwObjSerial, player->m_Param.GetGold());
  return 1;
}

void AutominePersonalMgr::push_dqs_makestorage(int n, unsigned int dwAvatorSerial, unsigned int dwTotGold)
{
_qry_case_make_storage qry{};
  qry.nSockIdx = n;
  qry.dwAvatorSerial = dwAvatorSerial;

  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x38u, reinterpret_cast<char *>(&qry), size);
}

unsigned __int8 AutominePersonalMgr::request_query(char *pdata)
{
  if (!pdata)
  {
    return 24;
  }

  if (*pdata)
  {
    if (*pdata == 1)
    {
      char buffer[10244]{};
      std::memset(buffer, 0, 10240);
      sprintf(buffer, "update [dbo].[tbl_aminepersonal_inven] set ");

      int len = static_cast<int>(strlen_0(buffer));
      for (int j = 0; j < static_cast<unsigned __int8>(pdata[12]); ++j)
      {
        int num = static_cast<unsigned __int8>(pdata[12 * j + 24]);
        int slot = static_cast<unsigned __int8>(pdata[12 * j + 16]);
        char *dest = &buffer[len];
        sprintf(dest, ",K%d=%d,N%d=%d,", slot, *reinterpret_cast<unsigned int *>(&pdata[12 * j + 20]), slot, num);
      }

      len = static_cast<int>(strlen_0(buffer));
      sprintf(&buffer[len - 1], " where avatorserial = %d", *reinterpret_cast<unsigned int *>(pdata + 8));
      if (g_Main.m_pWorldDB->update_amine_personal(buffer))
      {
        return 0;
      }
      return 24;
    }
    return 24;
  }

  if (g_Main.m_pWorldDB->select_amine_personal(*reinterpret_cast<unsigned int *>(pdata + 8)) != 2
      || g_Main.m_pWorldDB->insert_amine_personal(*reinterpret_cast<unsigned int *>(pdata + 8)))
  {
    return 0;
  }
  return 24;
}

void AutominePersonalMgr::result_query(unsigned __int8 byRet, char *pdata)
{
  if (!pdata)
  {
    return;
  }

  if (!*pdata)
  {
    pop_dqs_makestorage(byRet, pdata);
  }
}

void AutominePersonalMgr::pop_dqs_makestorage(unsigned __int8 byRet, char *pdata)
{
  CPlayer *player = &g_Player[*reinterpret_cast<int *>(pdata + 4)];
  if (player->m_bOper)
  {
    unsigned int charSerial = player->m_Param.GetCharSerial();
    if (charSerial == *reinterpret_cast<unsigned int *>(pdata + 8))
    {
      if (byRet == 24)
      {
        player->AlterGold(250.0);
        send_ecode(player->m_ObjID.m_wIndex, 0x11u);
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

char AutominePersonalMgr::check_machine(
  int n,
  unsigned int dwSerial,
  AutominePersonal *pMachine,
  _STORAGE_LIST::_db_con *pcitem)
{
  if (pMachine == nullptr)
  {
    m_logError.Write("AutominePersonalMgr::check_machine() >> not exist machine");
    return 0;
  }

  if (!pMachine->is_installed())
  {
    send_ecode(n, 4u);
    return 0;
  }

  if (pMachine->get_objserial() != dwSerial)
  {
    send_ecode(n, 0x12u);
    return 0;
  }

  if (pcitem && pMachine->get_item() != pcitem)
  {
    send_ecode(n, 5u);
    return 0;
  }

  return 1;
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

char AutominePersonalMgr::install(int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(player->m_id.wIndex) == 99)
  {
    send_ecode(n, 0x21u);
    return 1;
  }

  if (!player->m_Param.BeHaveBoxOfAMP())
  {
    send_ecode(n, 0x14u);
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (machine == nullptr)
  {
    m_logError.Write("!CRITICAL! AutominePersonalMgr::install() [LINE:%d]", 227);
    return 0;
  }

  if (machine->is_installed())
  {
    send_ecode(n, 3u);
    return 1;
  }

  if (!COreAmountMgr::Instance()->IsOreRemain())
  {
    send_ecode(n, 0x1Bu);
    return 1;
  }

  if (pmsg == nullptr || !check_dummy(player->m_pCurMap, static_cast<unsigned __int8>(pmsg[0]), player->m_fCurPos))
  {
    send_ecode(n, 1u);
    return 1;
  }

  _res_dummy *resDummy = &player->m_pCurMap->m_pResDummy[static_cast<unsigned __int8>(pmsg[0])];
  if (resDummy->GetQualityGrade() != 2 && !player->IsMiningByMinigTicket())
  {
    send_ecode(n, 0x1Au);
    return 1;
  }

  _STORAGE_LIST::_db_con *installItem = get_localitem(player, *reinterpret_cast<unsigned __int16 *>(pmsg + 1));
  if (installItem == nullptr || installItem->m_byTableCode != 33)
  {
    send_ecode(n, 5u);
    return 1;
  }

  const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(player->m_id.wIndex);
  const float effDelay = player->m_EP.GetEff_Have(7) * player->m_EP.GetEff_Have(37);
  const float delayProf = effDelay * static_cast<float>(playerPenalty);

  if (machine->regist_to_map(
        player,
        installItem,
        static_cast<unsigned __int8>(pmsg[0]),
        m_dwObjSerial,
        delayProf))
  {
    ++m_dwObjSerial;
  }
  else
  {
    send_ecode(n, 0xFu);
  }

  return 1;
}

char AutominePersonalMgr::uninstall(int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  _STORAGE_LIST::_db_con *item = get_localitem(player, *(reinterpret_cast<unsigned __int16 *>(pmsg) + 2));
  if (!check_machine(n, *reinterpret_cast<unsigned int *>(pmsg), machine, item))
  {
    return 1;
  }

  if (!machine->unregist_from_map(0))
  {
    send_ecode(n, 0x10u);
    m_logError.Write("AutominePersonalMgr::uninstall() [LINE:%d]", 314);
  }
  return 1;
}

char AutominePersonalMgr::uninstall(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (machine == nullptr)
  {
    return 0;
  }

  const unsigned __int16 objSerial16 = static_cast<unsigned __int16>(machine->get_objserial());
  _STORAGE_LIST::_db_con *item = get_localitem(player, objSerial16);
  if (!check_machine(n, machine->get_objserial(), machine, item))
  {
    return 1;
  }

  if (!machine->unregist_from_map(0))
  {
    send_ecode(n, 0x10u);
    m_logError.Write("AutominePersonalMgr::uninstall() [LINE:%d]", 548);
  }
  return 1;
}

char AutominePersonalMgr::selectore(unsigned int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), *reinterpret_cast<unsigned int *>(pmsg), machine, nullptr))
  {
    return 1;
  }

  const unsigned __int8 oreSlot = static_cast<unsigned __int8>(pmsg[4]);
  if (oreSlot >= 5u)
  {
    send_ecode(n, 9u);
    return 1;
  }

  machine->set_selore(m_wOreIndex[oreSlot]);

  _personal_automine_selore_zocl msg{};
  msg.dwObjSerial = machine->get_objserial();
  msg.bySelectOre = oreSlot;

  unsigned __int8 type[2] = {14, 49};
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));
  return 1;
}

char AutominePersonalMgr::insert_battery(unsigned int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), *reinterpret_cast<unsigned int *>(pmsg), machine, nullptr))
  {
    return 1;
  }

  if (!machine->insert_battery(static_cast<unsigned __int8>(pmsg[4]), *reinterpret_cast<unsigned __int16 *>(pmsg + 5)))
  {
    send_ecode(n, 0x18u);
    return 1;
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
  return 1;
}

char AutominePersonalMgr::extract_battery(unsigned int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), *reinterpret_cast<unsigned int *>(pmsg), machine, nullptr))
  {
    return 1;
  }

  _STORAGE_LIST::_db_con battery{};
  machine->extract_battery(static_cast<unsigned __int8>(pmsg[4]), &battery);
  battery.m_byClientIndex = static_cast<unsigned __int8>(pmsg[5]);

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
    return 1;
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
    added->m_dwDur,
    owner->m_szItemHistoryFileName);
  return 1;
}

char AutominePersonalMgr::extract_battery(unsigned int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!check_machine(static_cast<int>(n), machine->get_objserial(), machine, nullptr))
  {
    return 1;
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
      return 1;
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
      added->m_dwDur,
      owner->m_szItemHistoryFileName);
  }
  return 1;
}

char AutominePersonalMgr::pop_ore(unsigned int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  if (player->m_Param.m_pStoragePtr[0]->GetIndexEmptyCon() == 255)
  {
    send_ecode(n, 0x15u);
    return 1;
  }

  _STORAGE_LIST::_storage_con *sourceOre =
    player->m_Param.m_pStoragePtr[6]->GetPtrFromSerial(*reinterpret_cast<unsigned __int16 *>(pmsg));
  if (sourceOre == nullptr)
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Is not exist item(%d)", *reinterpret_cast<unsigned __int16 *>(pmsg));
    return 1;
  }

  _STORAGE_LIST::_db_con *added = player->Emb_AddStorage(0, sourceOre, false, false);
  if (added == nullptr)
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Failed CPlayer::Emb_AddStorage()");
    return 1;
  }

  added->m_byClientIndex = static_cast<unsigned __int8>(pmsg[4]);
  if (!player->m_pUserDB->Update_ItemSlot(0, added->m_byStorageIndex, static_cast<unsigned __int8>(pmsg[4])))
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Failed CPlayer::CUserDB::Update_ItemSlot()");
    return 1;
  }

  if (!player->Emb_DelStorage(6, sourceOre->m_byClientIndex, false, true, "AutominePersonalMgr::pop_ore()"))
  {
    player->Emb_DelStorage(0, sourceOre->m_byClientIndex, false, false, "AutominePersonalMgr::pop_ore() - Delete Fail");
    return 1;
  }

  _personal_automine_popore_zocl msg{};
  msg = *reinterpret_cast<_personal_automine_popore_zocl *>(pmsg);
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
    added->m_dwDur,
    player->m_szItemHistoryFileName);
  return 1;
}

char AutominePersonalMgr::Open_InvenUI(int n, bool *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  if (player->m_Param.m_pAPM)
  {
    player->m_Param.m_pAPM->set_openUI_Inven(*pmsg);
    player->m_Param.m_pAPM->send_changed_packet(static_cast<unsigned int>(n));
  }
  return 1;
}

char AutominePersonalMgr::Open_InfoUI(unsigned int n, bool *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  machine->set_openUI_battery(*pmsg);

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
  return 1;
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
