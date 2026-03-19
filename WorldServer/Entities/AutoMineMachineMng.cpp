#include "pch.h"

#include "AutoMineMachineMng.h"

#include "CMapData.h"
#include "CMapOperation.h"
#include "CMainThread.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CRecordData.h"
#include "CGuild.h"
#include "CWeeklyGuildRankManager.h"
#include "DB_LOAD_AUTOMINE_MACHINE.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "Packet/ClientZonePacket.h"
#include "Packet/ZoneClientPacket.h"
#include "StorageList.h"
#include "WorldServerUtil.h"
#include "pt_automine_getoutore_zocl.h"
#include "qry_case_amine_newowner.h"
#include "pt_automine_info_zocl.h"
#include "pt_automine_result_zocl.h"
#include "pt_automine_state_zocl.h"
#include "qry_case_amine_header.h"
#include "qry_case_amine_battery_discharge.h"
#include "qry_case_amine_batterycharge.h"
#include "qry_case_amine_mineore.h"
#include "qry_case_amine_moveore.h"
#include "qry_case_amine_selore.h"
#include "qry_case_amine_workstate.h"

#include <mmsystem.h>

static SYSTEMTIME s_tmLocal;
static const char *s_szOreKind_160[5] = {
  "ioblu04",
  "iored04",
  "ioyel04",
  "iogre04",
  "iobla04",
};

AutoMineMachineMng *AutoMineMachineMng::m_pInstance;

int _pt_automine_charge_money_db_update_fail_zocl::size()
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

void AutoMineMachine::SubChargeCost(unsigned __int8 byRet, const _qry_case_amine_batterycharge *query)
{
  if (query == nullptr)
  {
    return;
  }

  if (byRet || query->byProcRet)
  {
    CPlayer *player = &g_Player[query->wIndex];
    if (player->m_dwObjSerial != query->in_master)
    {
      player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, query->in_master);
    }

    if (player && player->m_bLive)
    {
      _pt_automine_charge_money_db_update_fail_zocl payload{};
      payload.nCharge = query->in_charge;

      unsigned __int8 type[36]{};
      type[0] = 14;
      type[1] = 28;

      const unsigned __int16 len = static_cast<unsigned __int16>(payload.size());
      g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), len);
    }
    return;
  }

  m_Battery.m_nCurGage += query->in_charge;
  if (m_Battery.m_nCurGage > m_Battery.m_nMaxGage)
  {
    m_Battery.m_nCurGage = m_Battery.m_nMaxGage;
  }

  m_pOwnerGuild->m_byMoneyOutputKind = 7;
  const long double outDalant = static_cast<long double>(-query->in_gold);
  CGuild *ownerGuild = m_pOwnerGuild;
  m_pOwnerGuild->IOMoney(
    "AutoMine Charge",
    ownerGuild->m_MasterData.dwSerial,
    0.0,
    outDalant,
    query->out_totaldalant,
    query->out_totalgold,
    const_cast<unsigned __int8 *>(query->byDate),
    0);

  m_Log.Write(
    "Cost : serial:%d, out:%d, total:%.0f)",
    query->dwGuildSerial,
    query->in_gold,
    static_cast<double>(query->out_totalgold));
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
  const unsigned int now = GetKorLocalTime();
  sprintf_s(buffer, sizeof(buffer),
    "..\\ZoneServerLog\\systemlog\\Concession\\AutoMine_R%d_T%d_%d.log",
    m_byRace,
    m_byCollisionType,
    now);
  m_Log.SetWriteLogFile(buffer, 1, 0, 1, 1);

  const unsigned int now2 = GetKorLocalTime();
  sprintf_s(buffer, sizeof(buffer),
    "..\\ZoneServerLog\\systemlog\\Concession\\err_automineR%d_T%d_%d.log",
    m_byRace,
    m_byCollisionType,
    now2);
  m_sysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  if (_InitMineOre())
  {
    if (m_Inven.create(2, 40, MAX_ITEM_OVERLAP))
    {
      return true;
    }
    m_sysLog.Write("Failed AutoMineMachine[%d]::m_Inven.create()", m_byRace);
    return false;
  }

  m_sysLog.Write("Failed AutoMineMachine[%d]::_set_mineore()", m_byRace);
  return false;
}

bool AutoMineMachine::_InitMineOre()
{
  for (int j = 0; j < 5; ++j)
  {
    const int tableCode = GetItemTableCode(s_szOreKind_160[j]);
    if (tableCode >= 37)
    {
      m_sysLog.Write("ERR::Invalid value of table code.(%d)", tableCode);
      return false;
    }

    unsigned __int16 *record =
      reinterpret_cast<unsigned __int16 *>(g_Main.m_tblItemData[tableCode].GetRecord(s_szOreKind_160[j]));
    if (!record)
    {
      m_sysLog.Write("ERR::Is not exist ore(%s)", s_szOreKind_160[j]);
      return false;
    }

    m_OreKind[j].bySlotIndex = 0;
    m_OreKind[j].byTableCode = static_cast<unsigned __int8>(tableCode);
    m_OreKind[j].wItemIndex = *record;
  }
  return true;
}

int AutoMineMachine::_Convert_GoldToGage(int nGold)
{
  const int gage = 86400 * nGold / 5000;
  if (gage <= 86400)
  {
    return gage;
  }
  return 86400;
}

void AutoMineMachine::_Mining()
{
  int nP[1]{};
  int nS[1]{};

  if (m_Battery.m_nCurGage >= 30 && m_Inven.find_empty(&m_OreKind[m_bySelectedOre], 1, nP, nS))
  {
    _INVENKEY item{};
    std::memcpy(&item, &m_OreKind[m_bySelectedOre], sizeof(item));
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
        if (member)
        {
          CPlayer *player = member->pPlayer;
          if (player && player->m_bOper)
          {
            _pt_automine_state_zocl state{};
            state.bySelectOre = m_bySelectedOre;
            state.dwGage = m_Battery.m_nCurGage;
            state.byPage = static_cast<unsigned __int8>(nP[0]);

            TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP[0], nS[0]);
            _INVENKEY *pItem = slot ? slot->get_pitem() : nullptr;
            if (pItem)
            {
              std::memcpy(&state.item, pItem, sizeof(state.item));
            }

            unsigned __int8 type[2] = {14, 25};
            const unsigned __int16 len = static_cast<unsigned __int16>(state.size());
            g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&state), len);
          }
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
      if (member)
      {
        CPlayer *player = member->pPlayer;
        if (player && player->m_bOper)
        {
          _pt_automine_result_zocl result{};
          result.byType = 19;
          result.byRetCode = (m_Battery.m_nCurGage >= 30) ? 11 : 10;
          unsigned __int8 type[2] = {14, 27};
          const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
          g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&result), len);
          m_Log.Write("STOP : %d", result.byRetCode);
        }
      }
    }
  }
}

bool AutoMineMachine::LoadDatabase(_DB_LOAD_AUTOMINE_MACHINE *pdata)
{
  if (m_byRace != pdata->byRace)
  {
    m_sysLog.Write(
      "[ERR-Load DB]::Invalid value of race code.(%d-%d)",
      m_byRace,
      pdata->byRace);
    return false;
  }

  m_pOwnerGuild = GetGuildDataFromSerial(g_Guild, 500, pdata->dwGuildSerial);
  m_bRunning = pdata->bWorking;
  m_bySelectedOre = pdata->bySelectedOre;
  m_dwMiningTime = timeGetTime();
  if (m_byCollisionType == 0)
  {
    m_dwMiningTerm = 30000;
  }
  else if (m_byCollisionType == 1)
  {
    m_dwMiningTerm = 60000;
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
        _INVENKEY *item = slot->get_pitem();
        if (item)
        {
          pInfo->slot[pInfo->bySlotCnt].nLumpIndex = nP;
          std::memcpy(&pInfo->slot[pInfo->bySlotCnt].item, item, sizeof(pInfo->slot[pInfo->bySlotCnt].item));
          pInfo->slot[pInfo->bySlotCnt++].nOverlapNum = slot->get_overlapnum();
        }
      }
    }
  }
}

void AutoMineMachine::Loop()
{
  if (!m_pOwnerGuild || !m_bRunning)
  {
    return;
  }

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
      SendMsg_ResultCode(n, 18u, 10u);
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
      SendMsg_ResultCode(n, 19u, 0);
    }
  }
  else
  {
    m_sysLog.Write("[ERR-Stop]:Have not owner guild");
  }
}

void AutoMineMachine::SelectOre(int n, unsigned __int8 byOre)
{
  if (byOre < 5)
  {
    m_bySelectedOre = byOre;
    SendMsg_ResultCode(n, 20u, 0);
    push_dqs_selore();
    m_Log.Write(
      "[SELECT ORE] Owner:%d Master:%d [Ore:%d]",
      m_pOwnerGuild->m_dwSerial,
      m_pOwnerGuild->m_MasterData.dwSerial,
      m_bySelectedOre);
  }
  else
  {
    SendMsg_ResultCode(n, 20u, 5u);
    m_sysLog.Write("[ERR-SelectOre]:max_kind_automine_ore(%d)", byOre);
  }
}

void AutoMineMachine::Charge(int n, int nCharge, int nGold)
{
  if (m_pOwnerGuild)
  {
    if (m_pOwnerGuild->m_bIOWait)
    {
      SendMsg_ResultCode(n, 24u, 13u);
      m_sysLog.Write("[ERR-Charge]:automine_guild_db_io_wait");
    }
    else if (nGold > 0 && static_cast<double>(nGold) <= m_pOwnerGuild->GetTotalGold())
    {
      if (nCharge == static_cast<int>(_Convert_GoldToGage(nGold)))
      {
        SendMsg_ResultCode(n, 24u, 0);
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
        SendMsg_ResultCode(n, 24u, 9u);
        m_sysLog.Write("[ERR-Charge]:utomine_nonmatch_charge_value");
      }
    }
    else
    {
      SendMsg_ResultCode(n, 24u, 7u);
      const long double totalDalant = m_pOwnerGuild->GetTotalDalant();
      const unsigned int totalDalantValue = static_cast<unsigned int>(totalDalant);
      m_sysLog.Write(
        "[ERR-Charge]:automine_not_enough_gold(T:%u, Cost:%d)",
        totalDalantValue,
        nGold);
    }
  }
  else
  {
    SendMsg_ResultCode(n, 24u, 5u);
    m_sysLog.Write("[ERR-Charge]:Have not owner guild");
  }
}

void AutoMineMachine::Discharge()
{
  // this is not a stub
}

void AutoMineMachine::GetOutOreInAutoMine(CPlayer *pUser, const _pt_automine_getoutore_clzo *request)
{
  if (m_pOwnerGuild)
  {
    _INVENKEY item{};
    item.LoadDBKey(static_cast<int>(request->dwSrcK));
    const int nNum = static_cast<unsigned __int8>(request->byOverlapNum);
    const int nPage = static_cast<unsigned __int8>(request->bySrcPage);
    const int nSlot = static_cast<unsigned __int8>(item.bySlotIndex);

    const int popRet = m_Inven.pop(nPage, nSlot, &item, nNum);
    if (popRet == -1)
    {
      SendMsg_ResultCode(pUser->m_ObjID.m_wIndex, 21u, 12u);
      m_sysLog.Write(
        "[ERR-GetOutOreInAutoMine]:automine_invalid_values(%d,%d,%d,%d)",
        nPage,
        item.bySlotIndex,
        static_cast<int>(item.CovDBKey()),
        nNum);
    }
    else
    {
      _STORAGE_LIST::_storage_con pCon{};
      pCon.m_byClientIndex = item.bySlotIndex;
      pCon.m_byTableCode = item.byTableCode;
      pCon.m_wItemIndex = item.wItemIndex;
      pCon.m_dwDur += static_cast<unsigned int>(nNum);
      pCon.m_wSerial = pUser->m_Param.GetNewItemSerial();

      if (pUser->Emb_AddStorage(0, &pCon, 0, 1))
      {
        _pt_automine_getoutore_zocl packet{};
        packet.wItemSerial = pCon.m_wSerial;

        unsigned __int8 type[2] = {14, 22};
        const unsigned __int16 len = static_cast<unsigned __int16>(packet.size());
        g_Network.m_pProcess[0]->LoadSendMsg(pUser->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), len);

        TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nPage, item.bySlotIndex);
        if (slot)
        {
          const unsigned __int8 overlapnum = static_cast<unsigned __int8>(slot->get_overlapnum());
          push_dqs_getore(static_cast<char>(nPage), static_cast<char>(item.bySlotIndex), overlapnum);
        }
        else
        {
          m_sysLog.Write("ERR - get_slot(%d, %d) is NULL", nPage, item.bySlotIndex);
        }

        const char *itemName = GetItemKorName(item.byTableCode, item.wItemIndex);
        m_Log.Write(
          "[POP ORE] GuildSerial%d MasterSerial:%d [Page:%d_Slot:%d_Ore:%s], Num:%d",
          m_pOwnerGuild->m_dwSerial,
          m_pOwnerGuild->m_MasterData.dwSerial,
          nPage,
          item.bySlotIndex,
          itemName,
          nNum);
      }
      else
      {
        if (m_Inven.push(nPage, item.bySlotIndex, &item, nNum))
        {
          m_sysLog.Write(
            "[ERR-GetOutOreInAutoMine]::m_Inven.push(%d, %d, [%d/%d/%d], %d)_LINE:%d",
            nPage,
            item.bySlotIndex,
            item.bySlotIndex,
            item.byTableCode,
            item.wItemIndex,
            nNum,
            466);
        }
        SendMsg_ResultCode(pUser->m_ObjID.m_wIndex, 21u, 12u);
      }
    }
  }
  else
  {
    SendMsg_ResultCode(pUser->m_ObjID.m_wIndex, 21u, 5u);
    m_sysLog.Write("[ERR-GetOutOreInAutoMine]:automine_isnot_owner_guild");
  }
}

void AutoMineMachine::MoveOreInAutoMine(
  int n,
  unsigned __int8 sl,
  unsigned __int8 ss,
  unsigned __int8 dl,
  unsigned __int8 ds)
{
  if (m_pOwnerGuild)
  {
    if (sl == dl && ss == ds)
    {
      SendMsg_ResultCode(n, 23u, 0);
    }
    else
    {
      TInvenSlot<_INVENKEY> *pSrc = m_Inven.get_slot(sl, ss);
      TInvenSlot<_INVENKEY> *pDst = m_Inven.get_slot(dl, ds);
      if (pSrc && pDst)
      {
        m_Inven.swap(pSrc, pDst);

        _INVENKEY *pSKey = pSrc->get_pitem();
        if (pSKey)
        {
          pSKey->bySlotIndex = ss;
        }

        _INVENKEY *pDKey = pDst->get_pitem();
        if (pDKey)
        {
          pDKey->bySlotIndex = ds;
        }

        SendMsg_ResultCode(n, 23u, 0);

        const unsigned __int8 byDNum = static_cast<unsigned __int8>(pDst->get_overlapnum());
        const int nDBSlot_D = ds + 40 * dl;
        const unsigned __int8 bySNum = static_cast<unsigned __int8>(pSrc->get_overlapnum());
        push_dqs_moveore(
          static_cast<unsigned __int8>(ss + 40 * sl),
          pSKey,
          bySNum,
          static_cast<unsigned __int8>(nDBSlot_D),
          pDKey,
          byDNum);
      }
      else
      {
        SendMsg_ResultCode(n, 23u, 12u);
        m_sysLog.Write(
          "[ERR-MoveOreInAutoMine]:automine_invalid_values(%d,%d,%d,%d)",
          sl,
          ss,
          dl,
          ds);
      }
    }
  }
  else
  {
    SendMsg_ResultCode(n, 23u, 5u);
    m_sysLog.Write("[ERR-MoveOreInAutoMine]:automine_isnot_owner_guild");
  }
}

void AutoMineMachine::OreMerge(int n, const _pt_automine_merge_clzo *request)
{
  const unsigned __int8 page = static_cast<unsigned __int8>(request->byPage);
  const unsigned __int8 slot = static_cast<unsigned __int8>(request->item.bySlotIndex);
  const int num = static_cast<unsigned __int8>(request->byOverlapNum);

  const int popRet = m_Inven.pop(page, slot, &request->item, num);
  if (popRet == -1)
  {
    _INVENKEY item = request->item;
    SendMsg_ResultCode(n, 26u, 12u);
    m_sysLog.Write(
      "[ERR-OreMerge]:automine_invalid_values(%d,%d,%d,%d)",
      page,
      slot,
      static_cast<int>(item.CovDBKey()),
      num);
  }
  else
  {
    SendMsg_ResultCode(n, 26u, 0);
  }
}

void AutoMineMachine::SendMsg_MachineInfo(unsigned int n)
{
  _pt_automine_info_zocl info{};
  info.INFO.byCollisionType = m_byCollisionType;
  info.INFO.byRace = m_byRace;
  info.INFO.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  info.INFO.dwGMasterSerial = m_pOwnerGuild->m_MasterData.dwSerial;
  info.INFO.bWorking = m_bRunning;
  info.INFO.bySelectedOre = m_bySelectedOre;
  info.INFO.dwBatteryGage = m_Battery.m_nCurGage;
  info.INFO.bySlotCnt = 0;

  for (int nP = 0; nP < 2; ++nP)
  {
    for (int nS = 0; nS < 40; ++nS)
    {
      TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP, nS);
      if (slot && !slot->is_empty())
      {
        _INVENKEY *item = slot->get_pitem();
        if (item)
        {
          info.INFO.slot[info.INFO.bySlotCnt].nLumpIndex = nP;
          std::memcpy(&info.INFO.slot[info.INFO.bySlotCnt].item, item, sizeof(info.INFO.slot[info.INFO.bySlotCnt].item));
          info.INFO.slot[info.INFO.bySlotCnt++].nOverlapNum = slot->get_overlapnum();
        }
      }
    }
  }

  unsigned __int8 type[2] = {14, 16};
  const unsigned __int16 len = static_cast<unsigned __int16>(info.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&info), len);
}

void AutoMineMachine::SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode)
{
  _pt_automine_result_zocl result{};
  result.byType = byType;
  result.byRetCode = byRetCode;

  unsigned __int8 type[2] = {14, 27};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), len);
}

void AutoMineMachine::GetState(unsigned int *dwState)
{
  if (m_pOwnerGuild)
  {
    *dwState = m_bySelectedOre;
    *dwState = m_Battery.m_nCurGage | (*dwState << 24);
  }
}

CGuild *AutoMineMachine::GetOwnerGuild()
{
  return m_pOwnerGuild;
}

bool AutoMineMachine::IsMaster(CPlayer *pUser)
{
  if (!m_pOwnerGuild)
  {
    return false;
  }
  const unsigned int serial = pUser->m_Param.GetCharSerial();
  return m_pOwnerGuild->m_MasterData.dwSerial == serial;
}

void AutoMineMachine::SetOpenUI(bool bOpen)
{
  m_bOpenUI = bOpen;
}

void AutoMineMachine::push_dqs_workingstate()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_workstate qry{};
  qry.byCollisionType = m_byCollisionType;
  qry.byRace = m_byRace;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.bWorking = m_bRunning;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
}

void AutoMineMachine::push_dqs_selore()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_selore qry{};
  qry.byCollisionType = m_byCollisionType;
  qry.byRace = m_byRace;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.byOreIdx = m_bySelectedOre;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
}

void AutoMineMachine::push_dqs_mineore(int nP, int nS)
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_mineore qry{};
  qry.byCollisionType = m_byCollisionType;
  qry.byRace = m_byRace;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.byColmID = static_cast<unsigned __int8>(nS + 40 * nP);
  TInvenSlot<_INVENKEY> *slot = m_Inven.get_slot(nP, nS);
  qry.dwK = *reinterpret_cast<unsigned int *>(slot->get_pitem());
  qry.byOverlapNum = static_cast<unsigned __int8>(slot->get_overlapnum());
  qry.dwGage = m_Battery.m_nCurGage;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
}

void AutoMineMachine::push_dqs_getore(char nP, char nS, unsigned __int8 byNum)
{
  _qry_case_amine_mineore qry{};
  qry.byCollisionType = m_byCollisionType;
  qry.byRace = m_byRace;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.byColmID = static_cast<unsigned __int8>(nS + 40 * nP);
  qry.dwK = static_cast<unsigned int>(-1);
  qry.byOverlapNum = byNum;
  qry.dwGage = m_Battery.m_nCurGage;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
}

void AutoMineMachine::push_dqs_moveore(
  unsigned __int8 nDBSlot_S,
  _INVENKEY *pSKey,
  unsigned __int8 bySNum,
  unsigned __int8 nDBSlot_D,
  _INVENKEY *pDKey,
  unsigned __int8 byDNum)
{
  _qry_case_amine_moveore qry{};
  qry.byCollisionType = m_byCollisionType;
  qry.byRace = m_byRace;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.bySColmID = nDBSlot_S;
  qry.dwSK = pSKey ? *reinterpret_cast<unsigned int *>(pSKey) : static_cast<unsigned int>(-1);
  qry.bySOverlapNum = bySNum;
  qry.byDColmID = nDBSlot_D;
  qry.dwDK = pDKey ? *reinterpret_cast<unsigned int *>(pDKey) : static_cast<unsigned int>(-1);
  qry.byDOverlapNum = byDNum;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
}

void AutoMineMachine::push_dqs_battery_charge(unsigned __int16 wIndex, int ncharge, int ncost)
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_batterycharge qry{};
  qry.byCollisionType = m_byCollisionType;
  qry.byRace = m_byRace;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.dwBattery = m_Battery.m_nCurGage;
  qry.wIndex = wIndex;
  qry.in_master = m_pOwnerGuild->m_MasterData.dwSerial;
  qry.in_charge = ncharge;
  qry.in_gold = ncost;
  qry.out_totalgold = 0.0;
  qry.out_totaldalant = 0.0;
  qry.byProcRet = 0;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
}

void AutoMineMachine::push_dqs_battery_discharge()
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_amine_battery_discharge qry{};
  qry.byRace = m_byRace;
  qry.byCollisionType = m_byCollisionType;
  qry.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  qry.dwBattery = m_Battery.m_nCurGage;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&qry), size);
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
  g_Main.PushDQSData(-1, nullptr, 53u, reinterpret_cast<char *>(&query), size);
}

bool AutoMineMachineMng::Initialzie()
{
  if (!g_Main.m_pWorldDB->create_automine_table())
  {
    return false;
  }

  _DB_LOAD_AUTOMINE_MACHINE pdata{};
  for (int j = 0; j < 3; ++j)
  {
    for (int k = 0; k < 2; ++k)
    {
      if (!m_Machine[j][k].Initialize(static_cast<unsigned __int8>(j), static_cast<unsigned __int8>(k)))
      {
        return false;
      }

      std::memset(&pdata, 0, sizeof(pdata));
      pdata.byRace = static_cast<unsigned __int8>(j);
      pdata.byCollisionType = static_cast<unsigned __int8>(k);
      if (g_Main.m_pWorldDB->select_automine(&pdata) == 1)
      {
        return false;
      }

      if (!m_Machine[j][k].LoadDatabase(&pdata))
      {
        return false;
      }

      CGuild *pGuild = CWeeklyGuildRankManager::Instance()->GetPrevOwnerGuild(
        static_cast<unsigned __int8>(j),
        static_cast<unsigned __int8>(k));
      if (!m_Machine[j][k].SetOwner(static_cast<unsigned __int8>(j), static_cast<unsigned __int8>(k), pGuild))
      {
        return false;
      }
    }
  }
  return true;
}

void AutoMineMachineMng::Loop()
{
  if (!g_Main.m_bWorldOpen || !g_Main.m_bWorldService)
  {
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    for (int k = 0; k < 2; ++k)
    {
      m_Machine[j][k].Loop();
    }
  }
}

bool AutoMineMachineMng::OpenUI(int n)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type != static_cast<unsigned __int8>(-1))
  {
    CMapData *settlementMap = g_MapOper.m_SettlementMapData[raceCode][type];
    if (settlementMap && settlementMap == pUser->m_pCurMap)
    {
      m_Machine[raceCode][type].SetOpenUI(true);
      m_Machine[raceCode][type].SendMsg_MachineInfo(n);
      return true;
    }
  }

  SendMsg_ResultCode(n, 15u, 6u);
  return true;
}

bool AutoMineMachineMng::CloseUI(int n)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 17u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].SetOpenUI(false);
  }
  return true;
}

bool AutoMineMachineMng::StartWorkMachine(int n)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 18u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].Start(n);
  }
  return true;
}

bool AutoMineMachineMng::StopWorkMachine(int n)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 19u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].Stop(n);
  }
  return true;
}

bool AutoMineMachineMng::SelectOreType(int n, const _pt_automine_selectore_clzo *request)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 20u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].SelectOre(n, static_cast<unsigned __int8>(request->bySelected));
  }
  return true;
}

bool AutoMineMachineMng::GetOutOre(int n, const _pt_automine_getoutore_clzo *request)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 21u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].GetOutOreInAutoMine(pUser, request);
  }
  return true;
}

bool AutoMineMachineMng::MoveOrePos(int n, const _pt_automine_moveore_clzo *request)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 23u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].MoveOreInAutoMine(
      n,
      static_cast<unsigned __int8>(request->bySrcPage),
      static_cast<unsigned __int8>(request->bySrcSlot),
      static_cast<unsigned __int8>(request->byDstPage),
      static_cast<unsigned __int8>(request->byDstSlot));
  }
  return true;
}

bool AutoMineMachineMng::BatteryCharge(int n, const _pt_automine_charge_clzo *request)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 24u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].Charge(n, request->nCharge, request->nGold);
  }
  return true;
}

bool AutoMineMachineMng::OreMerge(int n, const _pt_automine_merge_clzo *request)
{
  CPlayer *pUser = &g_Player[n];
  if (!pUser->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = pUser->m_Param.GetRaceCode();
  const unsigned __int8 type = get_type(pUser, raceCode);
  if (type == static_cast<unsigned __int8>(-1))
  {
    SendMsg_ResultCode(n, 26u, 6u);
  }
  else
  {
    m_Machine[raceCode][type].OreMerge(n, request);
  }
  return true;
}

bool AutoMineMachineMng::Command(int n, int nType, char *pMsg)
{
// this is not a stub
  return false;
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
  return static_cast<unsigned __int8>(-1);
}

void AutoMineMachineMng::SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode)
{
  _pt_automine_result_zocl result{};
  result.byType = byType;
  result.byRetCode = byRetCode;

  unsigned __int8 type[2] = {14, 27};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), len);
}

void AutoMineMachineMng::Release()
{
  if (m_pInstance)
  {
    delete m_pInstance;
    m_pInstance = nullptr;
  }
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

unsigned __int8 AutoMineMachineMng::request_db_query(_qry_case_amine_header *query)
{
  if (!query)
  {
    return 24;
  }

  switch (query->bySubQryCase)
  {
    case 0:
      return _db_qry_insert_newowner(reinterpret_cast<const _qry_case_amine_newowner *>(query));
    case 1:
      return _db_qry_update_battery_charge(reinterpret_cast<_qry_case_amine_batterycharge *>(query));
    case 2:
      return _db_qry_update_mineore(reinterpret_cast<const _qry_case_amine_mineore *>(query));
    case 3:
      return _db_qry_update_workstate(reinterpret_cast<const _qry_case_amine_workstate *>(query));
    case 4:
      return _db_qry_update_selore(reinterpret_cast<const _qry_case_amine_selore *>(query));
    case 5:
      return _db_qry_update_battery_discharge(reinterpret_cast<const _qry_case_amine_battery_discharge *>(query));
    case 6:
      return _db_qry_update_moveore(reinterpret_cast<const _qry_case_amine_moveore *>(query));
    default:
      return 24;
  }
}

void AutoMineMachineMng::result_db_query(unsigned __int8 byRet, const _qry_case_amine_header *query)
{
  if (query == nullptr)
  {
    return;
  }

  switch (query->bySubQryCase)
  {
    case 1:
    {
      auto *batteryChargeQuery = reinterpret_cast<const _qry_case_amine_batterycharge *>(query);
      AutoMineMachine *machine = GetMachine(batteryChargeQuery->byRace, batteryChargeQuery->byCollisionType);
      machine->SubChargeCost(byRet, batteryChargeQuery);
      break;
    }
    default:
      break;
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

unsigned __int8 AutoMineMachineMng::_db_qry_insert_newowner(const _qry_case_amine_newowner *query)
{
  if (!query)
  {
    return 24;
  }

  g_Main.m_pWorldDB->SetAutoCommitMode(false);
  if (g_Main.m_pWorldDB->update_amine_dck(query->byCollisionType, query->byRace, query->dwGuildSerial)
      && g_Main.m_pWorldDB->insert_amine_newowner(query->byCollisionType, query->byRace, query->dwGuildSerial))
  {
    g_Main.m_pWorldDB->CommitTransaction();
    g_Main.m_pWorldDB->SetAutoCommitMode(true);
    return 0;
  }

  g_Main.m_pWorldDB->RollbackTransaction();
  g_Main.m_pWorldDB->SetAutoCommitMode(true);
  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_battery_charge(_qry_case_amine_batterycharge *query)
{
  if (!query)
  {
    return 24;
  }

  if (g_Main.m_pWorldDB->update_amine_battery(
        query->byCollisionType,
        query->byRace,
        query->dwGuildSerial,
        query->dwBattery))
  {
    return g_Main.db_output_guild_money(
      query->in_master,
      query->dwGuildSerial,
      0,
      static_cast<unsigned int>(query->in_gold),
      &query->out_totaldalant,
      &query->out_totalgold,
      query->byDate,
      "AutoMine Charge",
      &query->byProcRet);
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_mineore(const _qry_case_amine_mineore *query)
{
  if (!query)
  {
    return 24;
  }

  if (g_Main.m_pWorldDB->update_amine_mineore(
        query->byCollisionType,
        query->byRace,
        query->dwGuildSerial,
        query->byColmID,
        query->dwK,
        query->byOverlapNum,
        query->dwGage))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_workstate(const _qry_case_amine_workstate *query)
{
  if (!query)
  {
    return 24;
  }

  if (g_Main.m_pWorldDB->update_amine_workstate(
        query->byCollisionType,
        query->byRace,
        query->dwGuildSerial,
        query->bWorking))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_selore(const _qry_case_amine_selore *query)
{
  if (!query)
  {
    return 24;
  }

  if (g_Main.m_pWorldDB->update_amine_selore(
        query->byCollisionType,
        query->byRace,
        query->dwGuildSerial,
        query->byOreIdx))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_battery_discharge(const _qry_case_amine_battery_discharge *query)
{
  if (!query)
  {
    return 24;
  }

  if (g_Main.m_pWorldDB->update_amine_battery(
        query->byCollisionType,
        query->byRace,
        query->dwGuildSerial,
        query->dwBattery))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_moveore(const _qry_case_amine_moveore *query)
{
  if (!query)
  {
    return 24;
  }

  if (g_Main.m_pWorldDB->update_amine_moveore(
        query->byCollisionType,
        query->byRace,
        query->dwGuildSerial,
        query->bySColmID,
        query->dwSK,
        query->bySOverlapNum,
        query->byDColmID,
        query->dwDK,
        query->byDOverlapNum))
  {
    return 0;
  }

  return 24;
}

