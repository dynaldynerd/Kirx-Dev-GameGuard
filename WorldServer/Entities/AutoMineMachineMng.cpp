#include "pch.h"

#include "AutoMineMachineMng.h"

#include "CMainThread.h"
#include "CMapOperation.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CGuild.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "CRFWorldDatabase.h"
#include "qry_case_amine_newowner.h"
#include "qry_case_amine_batterycharge.h"
#include "qry_case_amine_battery_discharge.h"
#include "qry_case_amine_mineore.h"
#include "qry_case_amine_workstate.h"
#include "qry_case_amine_selore.h"
#include "qry_case_amine_moveore.h"

static SYSTEMTIME s_tmLocal{};
namespace
{
  const char *s_szOreKind_160[5] = {
    "ioblu03",
    "iored03",
    "ioyel03",
    "iogre03",
    "iobla03",
  };
}

AutoMineMachineMng *AutoMineMachineMng::m_pInstance = nullptr;

__int64 _pt_automine_charge_money_db_update_fail_zocl::size()
{
  return 4;
}

AutoMineMachine::AutoMineMachine()
{
  m_bInit = false;
  m_bOpenUI = false;
  m_bRunning = false;
  m_bySelectedOre = 0;
  m_pOwnerGuild = nullptr;
  m_dwMiningTime = 0;
  m_dwMiningTerm = 30;
  m_dwDQSPushTime = 0;
  std::memset(&s_tmLocal, 0, sizeof(s_tmLocal));
}

AutoMineMachine::~AutoMineMachine() = default;

void AutoMineMachine::SubChargeCost(unsigned __int8 byRet, char *pdata)
{
  if (pdata == nullptr)
  {
    return;
  }

  if (byRet || pdata[45])
  {
    CPlayer *player = &g_Player[*reinterpret_cast<unsigned __int16 *>(pdata + 11)];
    if (player->m_dwObjSerial != *reinterpret_cast<unsigned int *>(pdata + 13))
    {
      player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, *reinterpret_cast<unsigned int *>(pdata + 13));
    }

    if (player && player->m_bLive)
    {
      _pt_automine_charge_money_db_update_fail_zocl payload{};
      payload.nCharge = *reinterpret_cast<int *>(pdata + 17);

      unsigned __int8 type[36]{};
      type[0] = 14;
      type[1] = 28;

      const unsigned __int16 len = static_cast<unsigned __int16>(payload.size());
      g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), len);
    }
    return;
  }

  m_Battery.m_nCurGage += *reinterpret_cast<int *>(pdata + 17);
  if (m_Battery.m_nCurGage > m_Battery.m_nMaxGage)
  {
    m_Battery.m_nCurGage = m_Battery.m_nMaxGage;
  }

  m_pOwnerGuild->m_byMoneyOutputKind = 7;
  const long double outDalant = static_cast<long double>(-*reinterpret_cast<int *>(pdata + 21));
  CGuild *ownerGuild = m_pOwnerGuild;
  unsigned __int8 *date = reinterpret_cast<unsigned __int8 *>(pdata + 41);
  m_pOwnerGuild->IOMoney(
    "AutoMine Charge",
    ownerGuild->m_MasterData.dwSerial,
    0.0,
    outDalant,
    *reinterpret_cast<long double *>(pdata + 25),
    *reinterpret_cast<long double *>(pdata + 33),
    date,
    0);

  m_Log.Write(
    "Cost : serial:%d, out:%d, total:%.0f)",
    *reinterpret_cast<int *>(pdata + 3),
    *reinterpret_cast<int *>(pdata + 21),
    *reinterpret_cast<double *>(pdata + 33));
}

void AutoMineMachine::ChangeOwner(CGuild *pOwnerGuild)
{
  if (!pOwnerGuild || !m_pOwnerGuild || m_pOwnerGuild->m_dwSerial != pOwnerGuild->m_dwSerial)
  {
    if (m_pOwnerGuild)
    {
      m_Log.Write("Prev Owner:%d", m_pOwnerGuild->m_dwSerial);
    }
    else
    {
      m_Log.Write("Prev Owner:NULL");
    }

    m_bOpenUI = false;
    m_bRunning = false;
    m_bySelectedOre = 0;
    m_pOwnerGuild = pOwnerGuild;
    m_Battery.m_nCurGage = 0;
    m_Inven.clear();
    m_bInit = true;
    push_dqs_newowner();

    if (m_pOwnerGuild)
    {
      m_Log.Write("Changed Owner:%d", m_pOwnerGuild->m_dwSerial);
    }
    else
    {
      m_Log.Write("Changed Owner:NULL");
    }
  }
}

void AutoMineMachine::push_dqs_newowner()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_newowner query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

AutoMineMachineMng::AutoMineMachineMng() = default;

AutoMineMachineMng *AutoMineMachineMng::Instance()
{
  if (!m_pInstance)
  {
    m_pInstance = new AutoMineMachineMng();
  }
  return m_pInstance;
}

unsigned __int8 AutoMineMachineMng::request_db_query(char *pdata)
{
  if (pdata == nullptr)
  {
    return 24;
  }

  switch (*pdata)
  {
    case 0:
      return _db_qry_insert_newowner(pdata);
    case 1:
      return _db_qry_update_battery_charge(pdata);
    case 2:
      return _db_qry_update_mineore(pdata);
    case 3:
      return _db_qry_update_workstate(pdata);
    case 4:
      return _db_qry_update_selore(pdata);
    case 5:
      return _db_qry_update_battery_discharge(pdata);
    case 6:
      return _db_qry_update_moveore(pdata);
    default:
      return 24;
  }
}

void AutoMineMachineMng::result_db_query(unsigned __int8 byRet, char *pdata)
{
  if (pdata == nullptr)
  {
    return;
  }

  if (*pdata == 1)
  {
    AutoMineMachine *machine = GetMachine(pdata[2], pdata[1]);
    machine->SubChargeCost(byRet, pdata);
  }
}

AutoMineMachine *AutoMineMachineMng::GetMachine(unsigned __int8 byRace, unsigned __int8 byCollisionType)
{
  return &m_Machine[byRace][byCollisionType];
}

void AutoMineMachineMng::ChangeOwner(int nRaceCode, CGuild *pGuild, unsigned __int8 byCollisionType)
{
  m_Machine[nRaceCode][byCollisionType].ChangeOwner(pGuild);
}

unsigned __int8 AutoMineMachineMng::_db_qry_insert_newowner(char *pdata)
{
  g_Main.m_pWorldDB->SetAutoCommitMode(false);
  if (g_Main.m_pWorldDB->update_amine_dck(pdata[1], pdata[2], *reinterpret_cast<unsigned int *>(pdata + 3))
      && g_Main.m_pWorldDB->insert_amine_newowner(pdata[1], pdata[2], *reinterpret_cast<unsigned int *>(pdata + 3)))
  {
    g_Main.m_pWorldDB->CommitTransaction();
    g_Main.m_pWorldDB->SetAutoCommitMode(true);
    return 0;
  }

  g_Main.m_pWorldDB->RollbackTransaction();
  g_Main.m_pWorldDB->SetAutoCommitMode(true);
  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_battery_charge(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_battery(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        *reinterpret_cast<unsigned int *>(pdata + 7)))
  {
    return g_Main.db_output_guild_money(
      *reinterpret_cast<unsigned int *>(pdata + 13),
      *reinterpret_cast<unsigned int *>(pdata + 3),
      0,
      *reinterpret_cast<unsigned int *>(pdata + 21),
      reinterpret_cast<long double *>(pdata + 25),
      reinterpret_cast<long double *>(pdata + 33),
      reinterpret_cast<unsigned __int8 *>(pdata + 41),
      "AutoMine Charge",
      reinterpret_cast<unsigned __int8 *>(pdata + 45));
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_mineore(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_mineore(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7],
        *reinterpret_cast<unsigned int *>(pdata + 8),
        pdata[12],
        *reinterpret_cast<unsigned int *>(pdata + 13)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_workstate(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_workstate(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7]))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_selore(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_selore(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7]))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_battery_discharge(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_battery(
        pdata[2],
        pdata[1],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        *reinterpret_cast<unsigned int *>(pdata + 7)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_moveore(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_moveore(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7],
        *reinterpret_cast<unsigned int *>(pdata + 8),
        pdata[12],
        pdata[13],
        *reinterpret_cast<unsigned int *>(pdata + 14),
        pdata[18]))
  {
    return 0;
  }

  return 24;
}

__int64 AutoMineMachine::_Convert_GoldToGage(int nGold)
{
  int value = 86400 * nGold / 5000;
  if (value <= 86400)
  {
    return static_cast<unsigned int>(value);
  }
  return 86400;
}

bool AutoMineMachine::_InitMineOre()
{
  for (int j = 0; j < 5; ++j)
  {
    const unsigned __int8 tableCode = static_cast<unsigned __int8>(GetItemTableCode(s_szOreKind_160[j]));
    if (tableCode >= 0x25u)
    {
      m_sysLog.Write("ERR::Invalid value of table code.(%d)", tableCode);
      return false;
    }

    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[tableCode], s_szOreKind_160[j]);
    if (!record)
    {
      m_sysLog.Write("ERR::Is not exist ore(%s)", s_szOreKind_160[j]);
      return false;
    }

    m_OreKind[j].bySlotIndex = 0;
    m_OreKind[j].byTableCode = tableCode;
    m_OreKind[j].wItemIndex = *reinterpret_cast<unsigned __int16 *>(record);
  }
  return true;
}

void AutoMineMachine::_Mining()
{
  int nP[8]{};
  int nS[4]{};

  if (m_Battery.m_nCurGage >= 30
      && m_Inven.find_empty(&m_OreKind[m_bySelectedOre], 1, nP, nS))
  {
    _INVENKEY item{};
    memcpy_0(&item, &m_OreKind[m_bySelectedOre], sizeof(item));
    item.bySlotIndex = static_cast<unsigned __int8>(nS[0]);

    if (m_Inven.push(nP[0], nS[0], &item, 1))
    {
      m_sysLog.Write(
        "ERR::m_Inven.push(%d,%d,[%d/%d/%d],1)_%d",
        nP[0],
        nS[0],
        item.bySlotIndex,
        item.byTableCode,
        item.wItemIndex,
        570);
    }
    else
    {
      push_dqs_mineore(nP[0], nS[0]);
      if (m_pOwnerGuild && m_bOpenUI)
      {
        const unsigned int masterSerial = m_pOwnerGuild->m_MasterData.dwSerial;
        _guild_member_info *member = m_pOwnerGuild->GetMemberFromSerial(masterSerial);
        if (member && member->pPlayer && member->pPlayer->m_bOper)
        {
          CPlayer *player = member->pPlayer;
          _pt_automine_state_zocl state{};
          state.bySelectOre = m_bySelectedOre;
          state.dwGage = m_Battery.m_nCurGage;
          state.byPage = static_cast<unsigned __int8>(nP[0]);

          TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP[0], nS[0]);
          _INVENKEY *slotItem = slot ? reinterpret_cast<_INVENKEY *>(slot->get_pitem()) : nullptr;
          if (slotItem)
          {
            memcpy_0(&state.item, slotItem, sizeof(state.item));
          }

          unsigned __int8 type[2] = {14, 25};
          const unsigned __int16 len = static_cast<unsigned __int16>(state.size());
          g_Network.m_pProcess[0]->LoadSendMsg(player->m_id.wIndex, type, reinterpret_cast<char *>(&state), len);
        }
      }
    }
  }
  else
  {
    Stop(-1);
    if (m_pOwnerGuild)
    {
      const unsigned int masterSerial = m_pOwnerGuild->m_MasterData.dwSerial;
      _guild_member_info *member = m_pOwnerGuild->GetMemberFromSerial(masterSerial);
      if (member && member->pPlayer && member->pPlayer->m_bOper)
      {
        CPlayer *player = member->pPlayer;
        _pt_automine_result_zocl result{};
        result.byType = 19;
        result.byRetCode = (m_Battery.m_nCurGage >= 30) ? 11 : 10;

        unsigned __int8 type[2] = {14, 27};
        const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
        g_Network.m_pProcess[0]->LoadSendMsg(player->m_id.wIndex, type, reinterpret_cast<char *>(&result), len);
        m_Log.Write("STOP : %d", result.byRetCode);
      }
    }
  }
}

bool AutoMineMachine::Initialize(unsigned __int8 byRace, unsigned __int8 byCollisionType)
{
  CreateDirectoryA("..\\ZoneServerLog\\systemlog\\Concession", nullptr);

  m_byRace = byRace;
  m_byCollisionType = byCollisionType;
  if (!m_byCollisionType)
  {
    m_dwMiningTerm = 30000;
  }
  if (m_byCollisionType == 1)
  {
    m_dwMiningTerm = 60000;
  }

  char buffer[1040]{};
  std::memset(buffer, 0, 1024);

  unsigned int now = GetKorLocalTime();
  sprintf(
    buffer,
    "..\\ZoneServerLog\\systemlog\\Concession\\AutoMine_R%d_T%d_%d.log",
    m_byRace,
    m_byCollisionType,
    now);
  m_Log.SetWriteLogFile(buffer, 1, 0, 1, 1);

  now = GetKorLocalTime();
  sprintf(
    buffer,
    "..\\ZoneServerLog\\systemlog\\Concession\\err_automineR%d_T%d_%d.log",
    m_byRace,
    m_byCollisionType,
    now);
  m_sysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  if (_InitMineOre())
  {
    if (m_Inven.create(2, 40, 99))
    {
      return true;
    }
    m_sysLog.Write("Failed AutoMineMachine[%d]::m_Inven.create()", m_byRace);
    return false;
  }

  m_sysLog.Write("Failed AutoMineMachine[%d]::_set_mineore()", m_byRace);
  return false;
}

bool AutoMineMachine::LoadDatabase(_DB_LOAD_AUTOMINE_MACHINE *pdata)
{
  if (m_byRace != pdata->byRace)
  {
    m_sysLog.Write("[ERR-Load DB]::Invalid value of race code.(%d-%d)", m_byRace, pdata->byRace);
    return false;
  }

  m_pOwnerGuild = GetGuildDataFromSerial(g_Guild, MAX_GUILD, pdata->dwGuildSerial);
  m_bRunning = pdata->bWorking;
  m_bySelectedOre = pdata->bySelectedOre;
  m_dwMiningTime = timeGetTime();

  if (m_byCollisionType)
  {
    if (m_byCollisionType == 1)
    {
      m_dwMiningTerm = 60000;
    }
  }
  else
  {
    m_dwMiningTerm = 30000;
  }

  m_Battery.m_nCurGage = pdata->dwBatteryGage;
  m_Battery.m_nMaxGage = 86400;

  m_Log.Write(
    "[Load DB]GuildSerial:%d Run:%d, Race:%d, Ore:%d, Gage:%d, SlotCnt:%d",
    pdata->dwGuildSerial,
    m_bRunning,
    m_byRace,
    m_bySelectedOre,
    m_Battery.m_nCurGage,
    pdata->bySlotCnt);

  for (int j = 0; j < pdata->bySlotCnt; ++j)
  {
    const int nP = pdata->slot[j].nLumpIndex;
    const int nS = pdata->slot[j].item.bySlotIndex;
    const int nNum = pdata->slot[j].nOverlapNum;
    if (m_Inven.push(nP, nS, &pdata->slot[j].item, nNum))
    {
      m_sysLog.Write(
        "[ERR-Load DB]::m_Inven.push(%d,%d,[%d/%d/%d],%d)",
        nP,
        nS,
        pdata->slot[j].item.bySlotIndex,
        pdata->slot[j].item.byTableCode,
        pdata->slot[j].item.wItemIndex,
        nNum);
      return false;
    }

    const char *itemName = GetItemKorName(pdata->slot[j].item.byTableCode, pdata->slot[j].item.wItemIndex);
    m_Log.Write(
      "[Load DB]Page:%d,Slot:%d [ORE Pos:%d,name_%s, num_%d]",
      nP,
      nS,
      pdata->slot[j].item.bySlotIndex,
      itemName,
      nNum);
  }

  return true;
}

void AutoMineMachine::Loop()
{
  if (m_pOwnerGuild && m_bRunning)
  {
    const DWORD now = timeGetTime();
    if (m_dwMiningTerm < now - m_dwMiningTime)
    {
      m_dwMiningTime = now;
      m_Battery.m_nCurGage -= 30;
      if (m_Battery.m_nCurGage < 0)
      {
        m_Battery.m_nCurGage = 0;
      }
      _Mining();
    }
  }
}

void AutoMineMachine::GetMachineInfo(_DB_LOAD_AUTOMINE_MACHINE *pInfo)
{
  pInfo->byCollisionType = m_byCollisionType;
  pInfo->byRace = m_byRace;
  pInfo->dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  pInfo->dwGMasterSerial = m_pOwnerGuild->m_MasterData.dwSerial;
  pInfo->bWorking = m_bRunning;
  pInfo->bySelectedOre = m_bySelectedOre;
  pInfo->dwBatteryGage = m_Battery.m_nCurGage;
  pInfo->bySlotCnt = 0;

  for (int nP = 0; nP < 2; ++nP)
  {
    for (int nS = 0; nS < 40; ++nS)
    {
      TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP, nS);
      if (slot && !slot->is_empty())
      {
        _INVENKEY *src = reinterpret_cast<_INVENKEY *>(slot->get_pitem());
        if (src)
        {
          pInfo->slot[pInfo->bySlotCnt].nLumpIndex = nP;
          memcpy_0(&pInfo->slot[pInfo->bySlotCnt].item, src, sizeof(pInfo->slot[pInfo->bySlotCnt].item));
          pInfo->slot[pInfo->bySlotCnt++].nOverlapNum = slot->get_overlapnum();
        }
      }
    }
  }
}

void AutoMineMachine::SendMsg_MachineInfo(unsigned int n)
{
  _pt_automine_info_zocl msg{};
  msg.INFO.byCollisionType = m_byCollisionType;
  msg.INFO.byRace = m_byRace;
  msg.INFO.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  msg.INFO.dwGMasterSerial = m_pOwnerGuild->m_MasterData.dwSerial;
  msg.INFO.bWorking = m_bRunning;
  msg.INFO.bySelectedOre = m_bySelectedOre;
  msg.INFO.dwBatteryGage = m_Battery.m_nCurGage;
  msg.INFO.bySlotCnt = 0;

  for (int nP = 0; nP < 2; ++nP)
  {
    for (int nS = 0; nS < 40; ++nS)
    {
      TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP, nS);
      if (slot && !slot->is_empty())
      {
        _INVENKEY *src = reinterpret_cast<_INVENKEY *>(slot->get_pitem());
        if (src)
        {
          msg.INFO.slot[msg.INFO.bySlotCnt].nLumpIndex = nP;
          memcpy_0(&msg.INFO.slot[msg.INFO.bySlotCnt].item, src, sizeof(msg.INFO.slot[msg.INFO.bySlotCnt].item));
          msg.INFO.slot[msg.INFO.bySlotCnt++].nOverlapNum = slot->get_overlapnum();
        }
      }
    }
  }

  unsigned __int8 type[2] = {14, 16};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), len);
}

void AutoMineMachine::SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode)
{
  _pt_automine_result_zocl msg{};
  msg.byType = byType;
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2] = {14, 27};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), len);
}

void AutoMineMachine::SetOpenUI(bool bOpen)
{
  m_bOpenUI = bOpen;
}

bool AutoMineMachine::SetOwner(unsigned __int8 byRace, unsigned __int8 byCollisionType, CGuild *pGuild)
{
  if (pGuild && m_pOwnerGuild && m_pOwnerGuild->m_dwSerial == pGuild->m_dwSerial)
  {
    m_bInit = true;
    return true;
  }

  if (m_pOwnerGuild)
  {
    m_Log.Write("Prev Owner:%d", m_pOwnerGuild->m_dwSerial);
  }
  else
  {
    m_Log.Write("Prev Owner:NULL");
  }

  m_bOpenUI = false;
  m_bRunning = false;
  m_bySelectedOre = 0;
  m_pOwnerGuild = pGuild;
  m_Battery.m_nCurGage = 0;
  m_Inven.clear();
  m_bInit = true;

  if (!m_pOwnerGuild
      || g_Main.m_pWorldDB->insert_amine_newowner(m_byCollisionType, m_byRace, m_pOwnerGuild->m_dwSerial))
  {
    if (m_pOwnerGuild)
    {
      m_Log.Write("Set Owner:%d", m_pOwnerGuild->m_dwSerial);
    }
    else
    {
      m_Log.Write("Set Owner:NULL");
    }
    return true;
  }

  return false;
}

void AutoMineMachine::GetState(unsigned int *dwState)
{
  if (m_pOwnerGuild)
  {
    *dwState = m_bySelectedOre;
    *dwState = m_Battery.m_nCurGage | (*dwState << 24);
  }
}

bool AutoMineMachine::IsMaster(CPlayer *pUser)
{
  if (!m_pOwnerGuild)
  {
    return false;
  }

  const unsigned int serial = CPlayerDB::GetCharSerial(&pUser->m_Param);
  return m_pOwnerGuild->m_MasterData.dwSerial == serial;
}

CGuild *AutoMineMachine::GetOwnerGuild()
{
  return m_pOwnerGuild;
}

void AutoMineMachine::Start(int n)
{
  if (m_pOwnerGuild)
  {
    if (m_Battery.m_nCurGage >= 30)
    {
      m_bRunning = true;
      push_dqs_workingstate();
      m_Log.Write(
        "[START] Owner:%d Master:%d",
        m_pOwnerGuild->m_dwSerial,
        m_pOwnerGuild->m_MasterData.dwSerial);
    }
    else
    {
      SendMsg_ResultCode(n, 0x12u, 0xAu);
    }
  }
  else
  {
    m_sysLog.Write("[ERR-Start]:Have not owner guild");
  }
}

void AutoMineMachine::Stop(int n)
{
  if (m_pOwnerGuild)
  {
    m_bRunning = false;
    if (n == -1)
    {
      push_dqs_workingstate();
      m_Log.Write(
        "[STOP] Owner:%d Master:%d",
        m_pOwnerGuild->m_dwSerial,
        m_pOwnerGuild->m_MasterData.dwSerial);
    }
    else
    {
      SendMsg_ResultCode(n, 0x13u, 0);
    }
  }
  else
  {
    m_sysLog.Write("[ERR-Stop]:Have not owner guild");
  }
}

void AutoMineMachine::SelectOre(int n, unsigned __int8 byOre)
{
  if (byOre < 5u)
  {
    m_bySelectedOre = byOre;
    SendMsg_ResultCode(n, 0x14u, 0);
    push_dqs_selore();
    m_Log.Write(
      "[SELECT ORE] Owner:%d Master:%d [Ore:%d]",
      m_pOwnerGuild->m_dwSerial,
      m_pOwnerGuild->m_MasterData.dwSerial,
      m_bySelectedOre);
  }
  else
  {
    SendMsg_ResultCode(n, 0x14u, 5u);
    m_sysLog.Write("[ERR-SelectOre]:max_kind_automine_ore(%d)", byOre);
  }
}

void AutoMineMachine::Charge(int n, int nCharge, int nGold)
{
  if (m_pOwnerGuild)
  {
    if (m_pOwnerGuild->m_bIOWait)
    {
      SendMsg_ResultCode(n, 0x18u, 0xDu);
      m_sysLog.Write("[ERR-Charge]:automine_guild_db_io_wait");
    }
    else if (nGold > 0 && static_cast<double>(nGold) <= static_cast<double>(m_pOwnerGuild->GetTotalGold()))
    {
      if (nCharge == _Convert_GoldToGage(nGold))
      {
        SendMsg_ResultCode(n, 0x18u, 0);
        push_dqs_battery_charge(static_cast<unsigned __int16>(n), nCharge, nGold);
        m_Log.Write(
          "Owner:%d Charge:%d GuildGold:%.0f - Cost:%d",
          m_pOwnerGuild->m_dwSerial,
          nCharge,
          static_cast<double>(m_pOwnerGuild->m_dTotalGold),
          nGold);
      }
      else
      {
        SendMsg_ResultCode(n, 0x18u, 9u);
        m_sysLog.Write("[ERR-Charge]:utomine_nonmatch_charge_value");
      }
    }
    else
    {
      SendMsg_ResultCode(n, 0x18u, 7u);
      const long double totalDalant = m_pOwnerGuild->GetTotalDalant();
      m_sysLog.Write("[ERR-Charge]:automine_not_enough_gold(T:%u, Cost:%d)", static_cast<unsigned int>(totalDalant), nGold);
    }
  }
  else
  {
    SendMsg_ResultCode(n, 0x18u, 5u);
    m_sysLog.Write("[ERR-Charge]:Have not owner guild");
  }
}

void AutoMineMachine::Discharge()
{
  // this is not a stub
}

void AutoMineMachine::GetOutOreInAutoMine(CPlayer *pUser, char *pMsg)
{
  unsigned __int8 *payload = reinterpret_cast<unsigned __int8 *>(pMsg);
  if (!m_pOwnerGuild)
  {
    SendMsg_ResultCode(pUser->m_id.wIndex, 0x15u, 5u);
    m_sysLog.Write("[ERR-GetOutOreInAutoMine]:automine_isnot_owner_guild");
    return;
  }

  _INVENKEY *pItem = reinterpret_cast<_INVENKEY *>(payload + 1);
  const int nNum = payload[10];
  const int nS = payload[1];
  const int nP = payload[0];

  const int popResult = m_Inven.pop(nP, nS, reinterpret_cast<_INVENKEY *>(payload + 1), nNum);
  if (popResult == -1)
  {
    SendMsg_ResultCode(pUser->m_id.wIndex, 0x15u, 0xCu);
    m_sysLog.Write(
      "[ERR-GetOutOreInAutoMine]:automine_invalid_values(%d,%d,%d,%d)",
      nP,
      pItem->bySlotIndex,
      *reinterpret_cast<int *>(pItem),
      nNum);
    return;
  }

  _STORAGE_LIST::_storage_con con{};
  con.m_byClientIndex = pItem->bySlotIndex;
  con.m_byTableCode = pItem->byTableCode;
  con.m_wItemIndex = pItem->wItemIndex;
  con.m_dwDur += payload[10];
  con.m_wSerial = CPlayerDB::GetNewItemSerial(&pUser->m_Param);

  if (CPlayer::Emb_AddStorage(pUser, 0, &con, 0, 1))
  {
    _pt_automine_getoutore_zocl result{};
    result.wItemSerial = con.m_wSerial;

    unsigned __int8 type[2] = {14, 22};
    const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(pUser->m_id.wIndex, type, reinterpret_cast<char *>(&result), len);

    TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP, pItem->bySlotIndex);
    if (slot)
    {
      const unsigned __int8 overlap = static_cast<unsigned __int8>(slot->get_overlapnum());
      push_dqs_getore(static_cast<char>(nP), static_cast<char>(pItem->bySlotIndex), overlap);
    }
    else
    {
      m_sysLog.Write("ERR - get_slot(%d, %d) is NULL", nP, pItem->bySlotIndex);
    }

    const char *itemName = GetItemKorName(pItem->byTableCode, pItem->wItemIndex);
    m_Log.Write(
      "[POP ORE] GuildSerial%d MasterSerial:%d [Page:%d_Slot:%d_Ore:%s], Num:%d",
      m_pOwnerGuild->m_dwSerial,
      m_pOwnerGuild->m_MasterData.dwSerial,
      nP,
      pItem->bySlotIndex,
      itemName,
      payload[10]);
  }
  else
  {
    if (m_Inven.push(nP, pItem->bySlotIndex, pItem, payload[10]))
    {
      m_sysLog.Write(
        "[ERR-GetOutOreInAutoMine]::m_Inven.push(%d, %d, [%d/%d/%d], %d)_LINE:%d",
        nP,
        pItem->bySlotIndex,
        pItem->bySlotIndex,
        pItem->byTableCode,
        pItem->wItemIndex,
        payload[10],
        466);
    }
    SendMsg_ResultCode(pUser->m_id.wIndex, 0x15u, 0xCu);
  }
}

void AutoMineMachine::MoveOreInAutoMine(
  int n,
  unsigned __int8 sl,
  unsigned __int8 ss,
  unsigned __int8 dl,
  unsigned __int8 ds)
{
  if (!m_pOwnerGuild)
  {
    SendMsg_ResultCode(n, 0x17u, 5u);
    m_sysLog.Write("[ERR-MoveOreInAutoMine]:automine_isnot_owner_guild");
    return;
  }

  if (sl == dl && ss == ds)
  {
    SendMsg_ResultCode(n, 0x17u, 0);
    return;
  }

  TInvenSlot<_INVENKEY> *src = m_Inven.get_slot(sl, ss);
  TInvenSlot<_INVENKEY> *dst = m_Inven.get_slot(dl, ds);
  if (src && dst)
  {
    m_Inven.swap(src, dst);

    _INVENKEY *sKey = reinterpret_cast<_INVENKEY *>(src->get_pitem());
    if (sKey)
    {
      sKey->bySlotIndex = ss;
    }
    _INVENKEY *dKey = reinterpret_cast<_INVENKEY *>(dst->get_pitem());
    if (dKey)
    {
      dKey->bySlotIndex = ds;
    }

    SendMsg_ResultCode(n, 0x17u, 0);

    const unsigned __int8 dstOverlap = static_cast<unsigned __int8>(dst->get_overlapnum());
    const int dstIndex = ds + 40 * dl;
    const unsigned __int8 srcOverlap = static_cast<unsigned __int8>(src->get_overlapnum());
    push_dqs_moveore(
      static_cast<unsigned __int8>(ss + 40 * sl),
      sKey,
      srcOverlap,
      static_cast<unsigned __int8>(dstIndex),
      dKey,
      dstOverlap);
  }
  else
  {
    SendMsg_ResultCode(n, 0x17u, 0xCu);
    m_sysLog.Write("[ERR-MoveOreInAutoMine]:automine_invalid_values(%d,%d,%d,%d)", sl, ss, dl, ds);
  }
}

void AutoMineMachine::OreMerge(int n, char *pMsg)
{
  const unsigned __int8 page = static_cast<unsigned __int8>(pMsg[0]);
  const unsigned __int8 slot = static_cast<unsigned __int8>(pMsg[1]);
  const unsigned __int8 num = static_cast<unsigned __int8>(pMsg[5]);

  const int popResult = m_Inven.pop(page, slot, reinterpret_cast<_INVENKEY *>(pMsg + 1), num);
  if (popResult == -1)
  {
    SendMsg_ResultCode(n, 0x1Au, 0xCu);
    m_sysLog.Write(
      "[ERR-OreMerge]:automine_invalid_values(%d,%d,%d,%d)",
      page,
      slot,
      *reinterpret_cast<int *>(pMsg + 1),
      num);
  }
  else
  {
    SendMsg_ResultCode(n, 0x1Au, 0);
  }
}

void AutoMineMachine::push_dqs_battery_charge(unsigned __int16 wIndex, int ncharge, int ncost)
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_batterycharge query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.dwBattery = m_Battery.m_nCurGage;
  query.wIndex = wIndex;
  query.in_master = m_pOwnerGuild->m_MasterData.dwSerial;
  query.in_charge = ncharge;
  query.in_gold = ncost;
  query.out_totalgold = 0.0;
  query.out_totaldalant = 0.0;
  query.byProcRet = 0;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

void AutoMineMachine::push_dqs_battery_discharge()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_battery_discharge query{};
  query.byRace = m_byRace;
  query.byCollisionType = m_byCollisionType;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.dwBattery = m_Battery.m_nCurGage;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

void AutoMineMachine::push_dqs_mineore(int nP, int nS)
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_mineore query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.byColmID = static_cast<unsigned __int8>(nS + 40 * nP);

  TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP, nS);
  query.dwK = static_cast<unsigned int>(*reinterpret_cast<_INVENKEY *>(slot->get_pitem()));

  slot = m_Inven.get_slot(nP, nS);
  query.byOverlapNum = static_cast<unsigned __int8>(slot->get_overlapnum());
  query.dwGage = m_Battery.m_nCurGage;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

void AutoMineMachine::push_dqs_getore(char nP, char nS, unsigned __int8 byNum)
{
  _qry_case_amine_mineore query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.byColmID = static_cast<unsigned __int8>(nS + 40 * nP);
  query.dwK = static_cast<unsigned int>(-1);
  query.byOverlapNum = byNum;
  query.dwGage = m_Battery.m_nCurGage;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

void AutoMineMachine::push_dqs_workingstate()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_workstate query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.bWorking = m_bRunning;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

void AutoMineMachine::push_dqs_selore()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_selore query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.byOreIdx = m_bySelectedOre;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

void AutoMineMachine::push_dqs_moveore(
  unsigned __int8 nDBSlot_S,
  _INVENKEY *pSKey,
  unsigned __int8 bySNum,
  unsigned __int8 nDBSlot_D,
  _INVENKEY *pDKey,
  unsigned __int8 byDNum)
{
  _qry_case_amine_moveore query{};
  query.byCollisionType = m_byCollisionType;
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.bySColmID = nDBSlot_S;
  query.dwSK = pSKey ? static_cast<unsigned int>(*pSKey) : static_cast<unsigned int>(-1);
  query.bySOverlapNum = bySNum;
  query.byDColmID = nDBSlot_D;
  query.dwDK = pDKey ? static_cast<unsigned int>(*pDKey) : static_cast<unsigned int>(-1);
  query.byDOverlapNum = byDNum;

  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x35u, reinterpret_cast<char *>(&query), size);
}

bool AutoMineMachineMng::OpenUI(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type != 0xFF)
  {
    CMapData *settlement = g_MapOper.GetSettlementMapData(race, type);
    if (settlement && settlement == player->m_pCurMap)
    {
      m_Machine[race][type].SetOpenUI(true);
      m_Machine[race][type].SendMsg_MachineInfo(n);
      return true;
    }
  }

  SendMsg_ResultCode(n, 0xFu, 6u);
  return true;
}

bool AutoMineMachineMng::CloseUI(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x11u, 6u);
  }
  else
  {
    m_Machine[race][type].SetOpenUI(false);
  }
  return true;
}

bool AutoMineMachineMng::StartWorkMachine(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x12u, 6u);
  }
  else
  {
    m_Machine[race][type].Start(n);
  }
  return true;
}

bool AutoMineMachineMng::StopWorkMachine(int n)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x13u, 6u);
  }
  else
  {
    m_Machine[race][type].Stop(n);
  }
  return true;
}

bool AutoMineMachineMng::SelectOreType(int n, char *pMsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x14u, 6u);
  }
  else
  {
    m_Machine[race][type].SelectOre(n, static_cast<unsigned __int8>(*pMsg));
  }
  return true;
}

bool AutoMineMachineMng::GetOutOre(int n, char *pMsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x15u, 6u);
  }
  else
  {
    m_Machine[race][type].GetOutOreInAutoMine(player, pMsg);
  }
  return true;
}

bool AutoMineMachineMng::MoveOrePos(int n, char *pMsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x17u, 6u);
  }
  else
  {
    m_Machine[race][type].MoveOreInAutoMine(
      n,
      static_cast<unsigned __int8>(pMsg[0]),
      static_cast<unsigned __int8>(pMsg[1]),
      static_cast<unsigned __int8>(pMsg[2]),
      static_cast<unsigned __int8>(pMsg[3]));
  }
  return true;
}

bool AutoMineMachineMng::BatteryCharge(int n, char *pMsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x18u, 6u);
  }
  else
  {
    m_Machine[race][type].Charge(
      n,
      *reinterpret_cast<int *>(pMsg),
      *reinterpret_cast<int *>(pMsg + 4));
  }
  return true;
}

bool AutoMineMachineMng::OreMerge(int n, char *pMsg)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
  const unsigned __int8 type = get_type(player, race);
  if (type == 0xFF)
  {
    SendMsg_ResultCode(n, 0x1Au, 6u);
  }
  else
  {
    m_Machine[race][type].OreMerge(n, pMsg);
  }
  return true;
}

unsigned __int8 AutoMineMachineMng::get_type(CPlayer *pUser, unsigned __int8 byRace)
{
  if (m_Machine[byRace][0].IsMaster(pUser))
  {
    return 0;
  }
  if (m_Machine[byRace][1].IsMaster(pUser))
  {
    return 1;
  }
  return 0xFF;
}

void AutoMineMachineMng::SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode)
{
  _pt_automine_result_zocl msg{};
  msg.byType = byType;
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2] = {14, 27};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), len);
}
