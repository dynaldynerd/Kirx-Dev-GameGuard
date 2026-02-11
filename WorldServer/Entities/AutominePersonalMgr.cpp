#include "pch.h"

#include "AutominePersonalMgr.h"

#include "CMainThread.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "COreAmountMgr.h"
#include "CRFWorldDatabase.h"
#include "KorLocalTime.h"
#include "GlobalObjects.h"
#include "DqsDbStructs.h"
#include "personal_amine_errmsg_zocl.h"
#include "personal_amine_make_storage_zocl.h"
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

bool AutominePersonalMgr::Is_MineRun(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  return machine != nullptr && machine->is_run();
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

  if (CPlayerDB::BeHaveBoxOfAMP(&player->m_Param))
  {
    send_ecode(n, 0x13u);
    return 1;
  }

  if (CPlayer::IsMineMode(player))
  {
    send_ecode(n, 3u);
    return 1;
  }

  const unsigned int objSerial = *reinterpret_cast<unsigned int *>(pmsg);
  if (objSerial == player->m_dwObjSerial)
  {
    if (CPlayerDB::GetGold(&player->m_Param) >= 0xFAu)
    {
      player->AlterGold(-250.0);
      const unsigned int gold = CPlayerDB::GetGold(&player->m_Param);
      push_dqs_makestorage(n, player->m_dwObjSerial, gold);
    }
    else
    {
      send_ecode(n, 0x17u);
    }
    return 1;
  }

  send_ecode(n, 0x16u);
  return 1;
}

char AutominePersonalMgr::install(int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  if (TimeLimitMgr::GetPlayerStatus(g_Main.m_pTimeLimitMgr, player->m_id.wIndex) == 99)
  {
    send_ecode(n, 0x21u);
    return 1;
  }

  if (!CPlayerDB::BeHaveBoxOfAMP(&player->m_Param))
  {
    send_ecode(n, 0x14u);
    return 1;
  }

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (!machine)
  {
    m_logError.Write("!CRITICAL! AutominePersonalMgr::install() [LINE:%d]", 227);
    return 0;
  }

  if (machine->is_installed())
  {
    send_ecode(n, 3u);
    return 1;
  }

  COreAmountMgr *oreMgr = COreAmountMgr::Instance();
  if (!COreAmountMgr::IsOreRemain(oreMgr))
  {
    send_ecode(n, 0x1Bu);
    return 1;
  }

  const unsigned __int8 dummyIndex = static_cast<unsigned __int8>(pmsg[0]);
  if (!check_dummy(player->m_pCurMap, dummyIndex, player->m_fCurPos))
  {
    send_ecode(n, 1u);
    return 1;
  }

  _res_dummy *resDummy = &player->m_pCurMap->m_pResDummy[dummyIndex];
  if (_res_dummy::GetQualityGrade(resDummy) != 2 && !CPlayer::IsMiningByMinigTicket(player))
  {
    send_ecode(n, 0x1Au);
    return 1;
  }

  _STORAGE_LIST::_db_con *dstItem = get_localitem(player, *reinterpret_cast<unsigned __int16 *>(pmsg + 1));
  if (!dstItem || dstItem->m_byTableCode != 33)
  {
    send_ecode(n, 5u);
    return 1;
  }

  const double playerPenalty = TimeLimitMgr::GetPlayerPenalty(g_Main.m_pTimeLimitMgr, player->m_id.wIndex);
  float effectRate = _effect_parameter::GetEff_Have(&player->m_EP, 7);
  effectRate *= _effect_parameter::GetEff_Have(&player->m_EP, 37);
  float delayProfile = effectRate * static_cast<float>(playerPenalty);

  if (machine->regist_to_map(player, dstItem, dummyIndex, m_dwObjSerial, delayProfile))
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

  const unsigned int objSerial = *reinterpret_cast<unsigned int *>(pmsg);
  AutominePersonal *machine = player->m_Param.m_pAPM;
  _STORAGE_LIST::_db_con *item = get_localitem(player, *reinterpret_cast<unsigned __int16 *>(pmsg + 4));
  if (!check_machine(n, objSerial, machine, item))
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

  const unsigned __int8 oreIndex = static_cast<unsigned __int8>(pmsg[4]);
  if (oreIndex < 5u)
  {
    machine->set_selore(m_wOreIndex[oreIndex]);
    _personal_automine_selore_zocl packet{};
    packet.dwObjSerial = machine->get_objserial();
    packet.bySelectOre = oreIndex;
    unsigned __int8 pbyType[2] = {14, 49};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(packet.size());
    g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&packet), nLen);
  }
  else
  {
    send_ecode(n, 9u);
  }

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

  if (machine->insert_battery(static_cast<unsigned __int8>(pmsg[4]), *reinterpret_cast<unsigned __int16 *>(pmsg + 5)))
  {
    _personal_automine_battery_insert_zocl packet{};
    packet.dwObjSerial = machine->get_objserial();
    packet.dwOwnerSerial = machine->get_ownerserial();
    packet.wItemSerial = machine->get_itemserial();

    unsigned __int8 pbyType[2] = {14, 51};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(packet.size());
    machine->CircleReport(pbyType, reinterpret_cast<char *>(&packet), nLen, player->m_dwObjSerial, false);
    g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&packet), nLen);
  }
  else
  {
    send_ecode(n, 0x18u);
  }

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
  if (added)
  {
    _personal_automine_battery_extract_zocl packet{};
    packet.dwObjSerial = machine->get_objserial();
    packet.wSerial = added->m_wSerial;
    packet.dwDur = added->m_dwDur;
    packet.byRetCode = 0;
    unsigned __int8 pbyType[2] = {14, 53};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(packet.size());
    g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&packet), nLen);

    if (!machine->get_battery())
    {
      machine->set_work(false);
      _personal_automine_stop_zocl stopPacket{};
      stopPacket.dwObjSerial = machine->get_objserial();
      stopPacket.dwOwnerSerial = machine->get_ownerserial();
      stopPacket.byStopType = 1;
      stopPacket.wItemSerial = machine->get_itemserial();
      unsigned __int8 stopType[2] = {14, 58};
      const unsigned __int16 stopLen = static_cast<unsigned __int16>(stopPacket.size());
      machine->CircleReport(stopType, reinterpret_cast<char *>(&stopPacket), stopLen, player->m_dwObjSerial, false);
      g_Network.m_pProcess[0]->LoadSendMsg(n, stopType, reinterpret_cast<char *>(&stopPacket), stopLen);
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

  _personal_automine_battery_extract_zocl errorPacket{};
  errorPacket.dwObjSerial = machine->get_objserial();
  errorPacket.byRetCode = 1;
  unsigned __int8 errorType[2] = {14, 53};
  const unsigned __int16 errorLen = static_cast<unsigned __int16>(errorPacket.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, errorType, reinterpret_cast<char *>(&errorPacket), errorLen);
  return 1;
}

char AutominePersonalMgr::pop_ore(unsigned int n, char *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  if (_STORAGE_LIST::GetIndexEmptyCon(player->m_Param.m_pStoragePtr[0]) == 255)
  {
    send_ecode(n, 0x15u);
    return 1;
  }

  const unsigned __int16 *serialPtr = reinterpret_cast<unsigned __int16 *>(pmsg);
  _STORAGE_LIST::_storage_con *storageItem =
    _STORAGE_LIST::GetPtrFromSerial(player->m_Param.m_pStoragePtr[6], *serialPtr);
  if (!storageItem)
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Is not exist item(%d)", *serialPtr);
    return 1;
  }

  _STORAGE_LIST::_db_con *added = player->Emb_AddStorage(0, storageItem, false, false);
  if (!added)
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Failed CPlayer::Emb_AddStorage()");
    return 1;
  }

  const unsigned __int8 clientIndex = *(reinterpret_cast<unsigned __int8 *>(pmsg) + 4);
  added->m_byClientIndex = clientIndex;
  if (!CUserDB::Update_ItemSlot(player->m_pUserDB, 0, added->m_byStorageIndex, clientIndex))
  {
    m_logError.Write("AutominePersonalMgr::pop_ore() >> Failed CPlayer::CUserDB::Update_ItemSlot()");
    return 1;
  }

  if (!player->Emb_DelStorage(6u, BYTE3(storageItem[1].m_dwDur), 0, 1, "AutominePersonalMgr::pop_ore()"))
  {
    player->Emb_DelStorage(0, storageItem->m_byClientIndex, 0, 0, "AutominePersonalMgr::pop_ore() - Delete Fail");
    return 1;
  }

  _personal_automine_popore_zocl packet{};
  std::memcpy(&packet, serialPtr, sizeof(packet));
  unsigned __int8 pbyType[2] = {14, 57};
  const unsigned __int16 nLen = static_cast<unsigned __int16>(packet.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&packet), nLen);

  AutominePersonal *machine = player->m_Param.m_pAPM;
  if (machine && machine->is_installed())
  {
    if (machine->get_battery())
    {
      if (!machine->is_run())
      {
        machine->set_work(true);
      }
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
    player->m_Param.m_pAPM->send_changed_packet(n);
  }
  return 1;
}

char AutominePersonalMgr::Open_InfoUI(unsigned int n, bool *pmsg)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    AutominePersonal *machine = player->m_Param.m_pAPM;
    machine->set_openUI_battery(*pmsg);

    _personal_amine_infoui_open_zocl packet{};
    packet.dwObjSerial = machine->get_objserial();
    packet.dwBattery[0] = machine->get_battery(0);
    packet.dwBattery[1] = machine->get_battery(1);

    unsigned __int8 pbyType[2] = {14, 64};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(packet.size());
    g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&packet), nLen);
  }
  return 1;
}

bool AutominePersonalMgr::check_dummy(CMapData *pMap, unsigned __int8 byCurDummyIndex, float *pfCurPos)
{
  if (byCurDummyIndex >= pMap->m_nResDumNum)
  {
    return false;
  }

  const int resDummySector = CMapData::GetResDummySector(pMap, byCurDummyIndex, pfCurPos);
  return resDummySector != -1;
}

char AutominePersonalMgr::check_machine(
  int n,
  unsigned int dwSerial,
  AutominePersonal *pMachine,
  _STORAGE_LIST::_db_con *pcitem)
{
  if (!pMachine)
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

  if (pcitem)
  {
    if (pMachine->get_item() != pcitem)
    {
      send_ecode(n, 5u);
      return 0;
    }
  }

  return 1;
}

_STORAGE_LIST::_db_con *AutominePersonalMgr::get_localitem(CPlayer *pOne, unsigned __int16 wItemSerial)
{
  _STORAGE_LIST::_db_con *item =
    _STORAGE_LIST::GetPtrFromSerial(pOne->m_Param.m_pStoragePtr[0], wItemSerial);
  if (!item)
  {
    return nullptr;
  }

  if (item->m_byTableCode == 33)
  {
    return item;
  }

  return nullptr;
}

void AutominePersonalMgr::push_dqs_makestorage(int n, unsigned int dwAvatorSerial, unsigned int dwTotGold)
{
  _qry_case_make_storage qry{};
  qry.nSockIdx = n;
  qry.dwAvatorSerial = dwAvatorSerial;
  const int nSize = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x38u, reinterpret_cast<char *>(&qry), nSize);
}
