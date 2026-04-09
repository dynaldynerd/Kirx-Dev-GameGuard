#include "pch.h"

#include "CUnmannedTraderUserInfo.h"
#include "CNetworkEX.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CLogFile.h"
#include "CMoneySupplyMgr.h"
#include "CRecordData.h"
#include "class_fld.h"
#include "force_fld.h"
#include "CHolyStoneSystem.h"
#include "CHonorGuild.h"
#include "CLogTypeDBTaskManager.h"
#include "DqsOnRunPayloads.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderItemState.h"
#include "CUnmannedTraderSortType.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CMainThread.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "WorldServerUtil.h"
#include "a_trade_adjust_price_request_clzo.h"
#include "a_trade_adjust_price_result_zocl.h"
#include "a_trade_clear_item_request_clzo.h"
#include "a_trade_clear_item_result_zocl.h"
#include "a_trade_reg_item_request_clzo.h"
#include "lt_qry_case_unmandtrader_select_list.h"
#include "qry_case_unmandtrader_cancelitem.h"
#include "qry_case_unmandtrader_buy_get_info.h"
#include "qry_case_unmandtrader_cheat_updateregisttime.h"
#include "qry_case_unmandtrader_log_in_proc_update_complete.h"
#include "qry_case_unmandtrader_re_registsingleitem.h"
#include "qry_case_unmandtrader_re_registsingleitem_roll_back.h"
#include "qry_case_unmandtrader_registsingleitem.h"
#include "qry_case_unmandtrader_update_reprice.h"
#include "unmannedtrader_Regist_item_inform_zocl.h"
#include "unmannedtrader_buy_item_request_clzo.h"
#include "unmannedtrader_buy_item_result_zocl.h"

#include <ctime>
#include "unmannedtrader_close_item_inform_zocl.h"
#include "unmannedtrader_continue_item_inform_zocl.h"
#include "unmannedtrader_re_regist_request_clzo.h"
#include "unmannedtrader_re_regist_result_zocl.h"
#include "unmannedtrader_regist_item_success_result_zocl.h"
#include "unmannedtrader_search_list_request_clzo.h"
#include "unmannedtrader_waiting_register_item_notify_register_success_zocl.h"

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <ctime>

CUnmannedTraderUserInfo CUnmannedTraderUserInfo::ms_kNull;

namespace
{
unsigned __int16 GetSellWaitInformSize(const _unmannedtrader_Sell_Wait_item_inform_zocl &msg)
{
  unsigned __int8 clamped = msg.byNum;
  if (clamped > 10)
  {
    clamped = 0;
  }

  return static_cast<unsigned __int16>(
    sizeof(msg)
    - sizeof(msg.List[0]) * (10 - clamped));
}
}

CUnmannedTraderUserInfo::CUnmannedTraderUserInfo()
  : m_eState(LOG_IN_STATE::UTUI_NONE),
    m_wInx(static_cast<unsigned short>(-1)),
    m_dwUserSerial(0),
    m_byRegistCnt(0),
    m_byMaxRegistCnt(0),
    m_kRequestState(),
    m_vecRegistItemInfo(),
    m_vecLoadItemInfo()
{
  Clear();
}

CUnmannedTraderUserInfo::~CUnmannedTraderUserInfo()
{
  m_wInx = static_cast<unsigned short>(-1);
  m_byMaxRegistCnt = 0;
  Clear();
}

bool CUnmannedTraderUserInfo::Init(unsigned short wInx)
{
  this->m_wInx = wInx;

  CUnmannedTraderRegistItemInfo regInfo;
  this->m_vecRegistItemInfo.assign(10u, regInfo);
  if (this->m_vecRegistItemInfo.size() != 10u)
  {
    return false;
  }

  CUnmannedTraderRegistItemInfo loadInfo;
  this->m_vecLoadItemInfo.assign(20u, loadInfo);
  if (this->m_vecLoadItemInfo.size() != 20u)
  {
    return false;
  }

  this->m_kRequestState.ClearRequset();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
  return true;
}

bool CUnmannedTraderUserInfo::Load(
  unsigned __int8 byType,
  unsigned __int16 wInx,
  unsigned int dwSerial,
  _TRADE_DB_BASE *kInfo,
  CLogFile *pkLogger)
{
  if (wInx >= 2532)
  {
    return false;
  }

  CPlayer *player = &g_Player[wInx];
  unsigned int maxRegistCount = 0;
  if (player->m_Param.GetPtrCurClass()->m_nClass == 3)
  {
    maxRegistCount = 10;
  }
  else
  {
    maxRegistCount = CUnmannedTraderEnvironmentValue::Unmanned_Trader_Nomal_Max_Regist_Item_Cnt;
  }
  this->m_byMaxRegistCnt = static_cast<unsigned __int8>(maxRegistCount);
  return SetLoadInfo(byType, dwSerial, kInfo, pkLogger);
}

bool CUnmannedTraderUserInfo::SetLoadInfo(
  unsigned __int8 byType,
  unsigned int dwSerial,
  _TRADE_DB_BASE *kInfo,
  CLogFile *pkLogger)
{
  if (IsNull() || byType >= 2u)
  {
    return false;
  }

  Clear();
  for (int j = 0; j < 20; ++j)
  {
    m_vecLoadItemInfo[j].SetState(0);
  }

  for (unsigned int uiInx = 0; uiInx < 20 && !kInfo->m_List[uiInx].IsEmpty(); ++uiInx)
  {
    CUnmannedTraderRegistItemInfo &item = m_vecLoadItemInfo[uiInx];
    if (!item.Set(
          m_wInx,
          kInfo->m_List[uiInx].byInvenIndex,
          uiInx,
          kInfo,
          pkLogger))
    {
      CUnmannedTraderItemState::PushUpdateState(
        byType,
        kInfo->m_List[uiInx].dwRegistSerial,
        7u,
        dwSerial,
        65535,
        255,
        65535);
      if (pkLogger)
      {
        const char *charName = g_Player[m_wInx].m_Param.GetCharNameA();
        pkLogger->Write(
          "CUnmannedTraderUserInfo::SetLoadInfo( byType(%u), dwSerial(%u), _TRADE_DB_BASE & kInfo, CLogFile * pkLogger )\r\n"
          "\t\tCUnmannedTraderItemState::PushUpdateState( byType(%u)\r\n"
          "\t\t, kInfo.m_List[i].dwRegistSerial(%u)\r\n"
          "\t\t, CUnmannedTraderItemState::CANCELREGISTFORSERVERINTERNALERROR, dwSerial )\r\n"
          "\t\tName(%s) Serial(%u) Invalid DB Data!\r\n",
          byType,
          dwSerial,
          byType,
          kInfo->m_List[uiInx].dwRegistSerial,
          charName,
          g_Player[m_wInx].m_id.dwSerial);
      }
      item.ClearToWaitState();
    }
  }

  m_dwUserSerial = dwSerial;
  m_kRequestState.ClearRequset();
  return true;
}

void CUnmannedTraderUserInfo::Clear()
{
  if (IsNull())
  {
    return;
  }

  for (CUnmannedTraderRegistItemInfo &item : this->m_vecRegistItemInfo)
  {
    item.Clear();
  }

  ClearLoadItemInfo();
  this->m_dwUserSerial = 0;
  this->m_byRegistCnt = 0;
}

void CUnmannedTraderUserInfo::ClearLoadItemInfo()
{
  for (CUnmannedTraderRegistItemInfo &item : this->m_vecLoadItemInfo)
  {
    item.Clear();
  }
}

bool CUnmannedTraderUserInfo::IsNull()
{
  return this == &CUnmannedTraderUserInfo::ms_kNull;
}

void CUnmannedTraderUserInfo::ClearRequest()
{
  this->m_kRequestState.ClearRequset();
}

void CUnmannedTraderUserInfo::Regist(
  unsigned __int8 byType,
  _a_trade_reg_item_request_clzo *pRequest,
  CLogFile *pkLogger)
{
  if (byType >= 2u)
  {
    return;
  }

  unsigned __int8 tempSlotIndex = 255;
  unsigned __int8 division = 0;
  unsigned __int8 classCode = 0;
  unsigned __int8 subClassCode = 0;
  unsigned int listIndex = 0;
  unsigned int retParam = 0;

  CountRegistItem();
  unsigned __int8 ret = CheckRegist(
    byType,
    pRequest,
    pkLogger,
    &tempSlotIndex,
    &division,
    &classCode,
    &subClassCode,
    &listIndex,
    &retParam);
  if (!ret)
  {
    ret = RegistItem(byType, pRequest, tempSlotIndex, division, classCode, subClassCode, listIndex, retParam);
  }

  if (ret)
  {
    SendRegistItemErrorResult(this->m_wInx, ret, pRequest->wItemSerial, retParam);
  }
}

unsigned __int8 CUnmannedTraderUserInfo::CheckRegist(
  unsigned __int8 byType,
  _a_trade_reg_item_request_clzo *pRequest,
  CLogFile *pkLogger,
  unsigned __int8 *byTempSlotIndex,
  unsigned __int8 *byDivision,
  unsigned __int8 *byClass,
  unsigned __int8 *bySubClass,
  unsigned int *dwListIndex,
  unsigned int *dwTax)
{
if (this->m_wInx >= 0x9E4u || byType >= 2u)
  {
    return 99;
  }

  CPlayer *owner = &g_Player[this->m_wInx];
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(owner->m_id.wIndex) == 99)
  {
    return static_cast<unsigned __int8>(-52);
  }
  if (owner->m_dwObjSerial != this->m_dwUserSerial)
  {
    return 99;
  }
  if (!this->m_kRequestState.IsEmpty())
  {
    return 95;
  }
  if (!pRequest->bUseNpcLink && !IsBeNearStore(owner, -1))
  {
    return 2;
  }
  if (g_Main.IsReleaseServiceMode() && owner->m_byUserDgr)
  {
    return 21;
  }

  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(pRequest->wItemSerial);
  if (!inventoryItem)
  {
    return 8;
  }
  if (inventoryItem->m_bLock)
  {
    return 15;
  }
  if (this->m_byRegistCnt >= this->m_byMaxRegistCnt)
  {
    return 6;
  }
  if (inventoryItem->m_byCsMethod)
  {
    return 14;
  }

  *dwTax = pRequest->dwPrice / 1000u;
  if (*dwTax > owner->m_Param.GetDalant())
  {
    return static_cast<unsigned __int8>(-55);
  }
  if (pRequest->dwPrice > MAX_DALANT)
  {
    return 22;
  }
  if (inventoryItem->m_byTableCode != pRequest->byItemTableCode || inventoryItem->m_wItemIndex != pRequest->wItemIndex)
  {
    return 9;
  }
  if (!IsExchangeItem(inventoryItem->m_byTableCode, inventoryItem->m_wItemIndex))
  {
    return 3;
  }
  if (IsOverLapItem(inventoryItem->m_byTableCode) && pRequest->byAmount > inventoryItem->m_dwDur)
  {
    return 17;
  }

  if (IsOverLapItem(inventoryItem->m_byTableCode) && inventoryItem->m_dwDur > pRequest->byAmount)
  {
    *byTempSlotIndex = static_cast<unsigned __int8>(owner->m_Param.m_dbInven.GetIndexEmptyCon());
    if (*byTempSlotIndex == 255)
    {
      return 13;
    }
  }

  if (inventoryItem->m_byTableCode == 15
      && !g_Main.m_tblEffectData[1].GetRecord(CPlayer::s_pnLinkForceItemToEffect[inventoryItem->m_wItemIndex]))
  {
    return 23;
  }

  CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
  if (!groupTable->GetGroupID(
        inventoryItem->m_byTableCode,
        inventoryItem->m_wItemIndex,
        byDivision,
        byClass,
        bySubClass,
        dwListIndex))
  {
    return 3;
  }

  return 0;
}

unsigned __int8 CUnmannedTraderUserInfo::RegistItem(
  unsigned __int8 byType,
  _a_trade_reg_item_request_clzo *pRequest,
  unsigned __int8 byTempSlotIndex,
  unsigned __int8 byDivision,
  unsigned __int8 byClass,
  unsigned __int8 bySubClass,
  unsigned int dwListIndex,
  unsigned int dwTax)
{
  CPlayer *owner = &g_Player[this->m_wInx];
  _STORAGE_LIST::_db_con *sourceItem = owner->m_Param.m_dbInven.GetPtrFromSerial(pRequest->wItemSerial);
  if (!sourceItem)
  {
    return 8;
  }

  const unsigned __int16 sourceItemSerial = sourceItem->m_wSerial;
  const unsigned __int8 sourceStorageIndex = sourceItem->m_byStorageIndex;
  unsigned __int16 splitItemSerial = 65535;
  _STORAGE_LIST::_db_con *registTarget = sourceItem;

  if (byTempSlotIndex != 255)
  {
    owner->Emb_AlterDurPoint(0, sourceItem->m_byStorageIndex, -static_cast<int>(pRequest->byAmount), false, false);

    _STORAGE_LIST::_db_con splitItem;
    splitItem = *sourceItem;
    splitItem.m_dwDur = pRequest->byAmount;
    splitItem.m_wSerial = owner->m_Param.GetNewItemSerial();
    splitItemSerial = splitItem.m_wSerial;
    registTarget = owner->Emb_AddStorage(0, &splitItem, false, false);
    if (!registTarget)
    {
      owner->Emb_AlterDurPoint(0, sourceItem->m_byStorageIndex, pRequest->byAmount, false, false);
      return 8;
    }
  }

  registTarget->lock(true);

  owner->SubDalant(dwTax);
  const int level = owner->m_Param.GetLevel();
  if (level == 30 || level == 40 || level == 50 || level == 60)
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(owner->m_Param.GetRaceCode()),
      level,
      dwTax);
  }

  _qry_case_unmandtrader_registsingleitem queryData{};
  queryData.wInx = this->m_wInx;
  queryData.wItemSerialOld = sourceItemSerial;
  queryData.dwLeftDalant = owner->m_Param.GetDalant();
  queryData.wSepaSerial = splitItemSerial;
  queryData.bySepaAmount = pRequest->byAmount;
  queryData.byOldStorageIndex = sourceStorageIndex;
  queryData.byStorageIndex = registTarget->m_byStorageIndex;
  queryData.byType = byType;
  queryData.bySellTurm = CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Sell_Turm;
  queryData.byRace = static_cast<unsigned __int8>(owner->m_Param.GetRaceCode());
  queryData.dwRegedSerial = 0;
  queryData.bInserted = false;
  queryData.dwOwnerSerial = owner->m_dwObjSerial;
  queryData.dwPrice = pRequest->dwPrice;
  queryData.byInveninx = registTarget->m_byStorageIndex;

  _INVENKEY itemKey(0, registTarget->m_byTableCode, registTarget->m_wItemIndex);
  queryData.dwK = static_cast<unsigned int>(itemKey.CovDBKey());
  queryData.dwD = registTarget->m_dwDur;
  queryData.dwU = registTarget->m_dwLv;

  if (registTarget->m_byTableCode == 15)
  {
    _force_fld *effectRecord = static_cast<_force_fld *>(
      g_Main.m_tblEffectData[1].GetRecord(CPlayer::s_pnLinkForceItemToEffect[registTarget->m_wItemIndex]));
    queryData.byLv = effectRecord ? static_cast<unsigned __int8>(effectRecord->m_nLv) : 0;
  }
  else
  {
    queryData.byLv = GetItemEquipLevel(registTarget->m_byTableCode, registTarget->m_wItemIndex);
  }
  queryData.byGrade = GetItemGrade(registTarget->m_byTableCode, registTarget->m_wItemIndex);
  queryData.byClass1 = byDivision;
  queryData.byClass2 = byClass;
  queryData.byClass3 = bySubClass;
  queryData.dwT = registTarget->m_dwT;
  queryData.lnUID = registTarget->m_lnUID;
  queryData.dwTax = dwTax;
  queryData.dwListIndex = dwListIndex;

  this->m_kRequestState.SetRequest(static_cast<CUnmannedTraderRequestLimiter::REQUEST_TYPE>(0));
  g_Main.PushDQSData(-1, nullptr, 60, reinterpret_cast<char *>(&queryData), sizeof(queryData));
  return 0;
}

void CUnmannedTraderUserInfo::ModifyPrice(
  unsigned __int8 byType,
  _a_trade_adjust_price_request_clzo *pRequest,
  CLogFile *pkLogger)
{
  if (byType >= 2u)
  {
    return;
  }

  unsigned int oldPrice = 0;
  unsigned int taxDalant = 0;
  const unsigned __int8 ret = CheckModifyPrice(byType, pRequest, &oldPrice, pkLogger, &taxDalant);
  if (ret)
  {
    SendRepriceErrorResult(&g_Player[this->m_wInx], ret);
    return;
  }

  _qry_case_unmandtrader_update_reprice queryData{};
  queryData.dwOldPrice = oldPrice;
  queryData.wInx = this->m_wInx;
  queryData.dwOwnerSerial = this->m_dwUserSerial;
  queryData.wItemSerial = pRequest->wItemSerial;
  queryData.dwTax = taxDalant;
  queryData.byProcRet = 0;
  queryData.byType = byType;
  queryData.dwRegistSerial = pRequest->dwRegistSerial;
  queryData.dwNewPrice = pRequest->dwNewPrice;

  g_Player[this->m_wInx].SubDalant(taxDalant);
  const int level = g_Player[this->m_wInx].m_Param.GetLevel();
  if (level == 30 || level == 40 || level == 50 || level == 60)
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(g_Player[this->m_wInx].m_Param.GetRaceCode()),
      level,
      taxDalant);
  }

  this->m_kRequestState.SetRequest(static_cast<CUnmannedTraderRequestLimiter::REQUEST_TYPE>(1));
  g_Main.PushDQSData(-1, nullptr, 63, reinterpret_cast<char *>(&queryData), sizeof(queryData));
}

unsigned __int8 CUnmannedTraderUserInfo::CheckModifyPrice(
  unsigned __int8 byType,
  _a_trade_adjust_price_request_clzo *pRequest,
  unsigned int *dwOldPrice,
  CLogFile *pkLogger,
  unsigned int *pdwTax)
{
if (this->m_wInx >= 2532)
  {
    return 99;
  }

  CPlayer *owner = &g_Player[this->m_wInx];
  if (owner->m_dwObjSerial != this->m_dwUserSerial)
  {
    return 99;
  }
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(owner->m_id.wIndex) == 99)
  {
    return static_cast<unsigned __int8>(-52);
  }

  *pdwTax = pRequest->dwNewPrice / 1000u;
  if (*pdwTax > owner->m_Param.GetDalant())
  {
    return static_cast<unsigned __int8>(-55);
  }
  if (!this->m_kRequestState.IsEmpty())
  {
    return 95;
  }
  if (!owner->m_Param.m_dbInven.GetPtrFromSerial(pRequest->wItemSerial))
  {
    return 14;
  }
  if (!pRequest->dwRegistSerial)
  {
    return 70;
  }

  auto it = Find(pRequest->dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return 14;
  }

  *dwOldPrice = it->GetPrice();
  return it->GetItemSerial() == pRequest->wItemSerial ? 0 : 25;
}

void CUnmannedTraderUserInfo::CancelRegist(
  unsigned __int8 byType,
  _a_trade_clear_item_request_clzo *pRequest,
  CLogFile *pkLogger)
{
  if (byType >= 2u)
  {
    return;
  }

  const unsigned __int8 ret = CheckCancelRegist(byType, pRequest, pkLogger);
  if (ret)
  {
    SendCancelRegistErrorResult(this->m_wInx, ret);
    return;
  }

  _qry_case_unmandtrader_cancelitem queryData{};
  queryData.wInx = this->m_wInx;
  queryData.wItemSerial = pRequest->wItemSerial;
  queryData.dwOwnerSerial = this->m_dwUserSerial;
  strcpy_s(queryData.szAccount, 13, g_Player[this->m_wInx].m_pUserDB->m_szAccountID);
  strcpy_s(queryData.wszName, 17, g_Player[this->m_wInx].m_Param.GetCharNameW());
  queryData.byType = byType;
  queryData.dwRegistSerial = pRequest->dwRegistSerial;
  queryData.byState = 5;
  queryData.byProcRet = 0;

  this->m_kRequestState.SetRequest(static_cast<CUnmannedTraderRequestLimiter::REQUEST_TYPE>(2));
  g_Main.PushDQSData(-1, nullptr, 61, reinterpret_cast<char *>(&queryData), sizeof(queryData));
}

unsigned __int8 CUnmannedTraderUserInfo::CheckCancelRegist(
  unsigned __int8 byType,
  _a_trade_clear_item_request_clzo *pRequest,
  CLogFile *pkLogger)
{
if (this->m_wInx >= 2532)
  {
    return 99;
  }

  CPlayer *owner = &g_Player[this->m_wInx];
  if (owner->m_dwObjSerial != this->m_dwUserSerial)
  {
    return 99;
  }
  if (!this->m_kRequestState.IsEmpty())
  {
    return 95;
  }
  if (!owner->m_Param.m_dbInven.GetPtrFromSerial(pRequest->wItemSerial))
  {
    return 14;
  }
  if (!pRequest->dwRegistSerial)
  {
    return 70;
  }

  auto it = Find(pRequest->dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return 14;
  }

  return it->GetItemSerial() == pRequest->wItemSerial ? 0 : 25;
}

void CUnmannedTraderUserInfo::Buy(
  unsigned __int8 byType,
  _unmannedtrader_buy_item_request_clzo *pRequest,
  CLogFile *pkLogger)
{
  (void)pkLogger;

  if (byType >= 2u)
  {
    return;
  }

  CPlayer *buyer = nullptr;
  const unsigned __int8 ret = CheckBuy(byType, pRequest, &buyer);
  if (ret)
  {
    SendBuyErrorResult(this->m_wInx, ret);
    return;
  }

  _qry_case_unmandtrader_buy_get_info queryData{};
  queryData.wInx = this->m_wInx;
  queryData.dwBuyer = this->m_dwUserSerial;
  queryData.byRace = static_cast<unsigned __int8>(buyer->m_Param.GetRaceCode());
  queryData.byUserGrade = buyer->m_byUserDgr;
  queryData.byDivision = pRequest->byDivision;
  queryData.byClass = pRequest->byClass;
  queryData.bySubClass = pRequest->bySubClass;
  queryData.byType = byType;
  queryData.byNum = pRequest->byNum;

  for (int index = 0; index < pRequest->byNum; ++index)
  {
    queryData.List[index].dwRegistSerial = pRequest->List[index].dwRegistSerial;
    queryData.List[index].dwPrice = pRequest->List[index].dwPrice;
  }

  this->m_kRequestState.SetRequest(static_cast<CUnmannedTraderRequestLimiter::REQUEST_TYPE>(3));
  g_Main.PushDQSData(-1, nullptr, 64, reinterpret_cast<char *>(&queryData), sizeof(queryData));
}

unsigned __int8 CUnmannedTraderUserInfo::CheckBuy(
  unsigned __int8 byType,
  _unmannedtrader_buy_item_request_clzo *pRequest,
  CPlayer **pkBuyer)
{
  if (this->m_wInx >= 2532)
  {
    return 99;
  }

  *pkBuyer = &g_Player[this->m_wInx];
  if ((*pkBuyer)->m_dwObjSerial != this->m_dwUserSerial)
  {
    return 99;
  }
  if (!this->m_kRequestState.IsEmpty())
  {
    return 95;
  }
  if (!pRequest->bUseNpcLink && !IsBeNearStore(*pkBuyer, -1))
  {
    return 2;
  }

  unsigned int curVersion = 0;
  CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
  if (!groupTable->GetVersion(pRequest->byDivision, pRequest->byClass, &curVersion))
  {
    return 51;
  }
  return curVersion == pRequest->dwVer ? 0 : 52;
}

void CUnmannedTraderUserInfo::Search(
  unsigned __int8 byType,
  _unmannedtrader_search_list_request_clzo *pRequest,
  CLogFile *pkLogger)
{
  (void)pkLogger;

  if (byType >= 2u)
  {
    return;
  }

  unsigned int listIndex = 0;
  unsigned int currentVersion = 0;
  const unsigned __int8 ret = CheckSearch(byType, pRequest, &listIndex, &currentVersion);
  if (ret)
  {
    SendSearchErrorResult(this->m_wInx, ret);
    return;
  }

  CUnmannedTraderSortType *sortType =
    CUnmannedTraderGroupItemInfoTable::Instance()->GetSortType(pRequest->byDivision, pRequest->bySortType);
  const char *queryText = sortType ? sortType->GetQuery() : nullptr;

  _lt_qry_case_unmandtrader_select_list queryData{};
  queryData.wInx = this->m_wInx;
  queryData.dwOwnerSerial = this->m_dwUserSerial;
  queryData.byType = byType;
  queryData.byRace = static_cast<unsigned __int8>(g_Player[this->m_wInx].m_Param.GetRaceCode());
  queryData.dwListIndex = listIndex;
  queryData.byClass1 = pRequest->byDivision;
  queryData.byClass2 = pRequest->byClass;
  queryData.byClass3 = pRequest->bySubClass;
  queryData.bySortType = pRequest->bySortType;
  queryData.dwVer = currentVersion;
  queryData.byPage = pRequest->byPage;
  if (queryText)
  {
    std::strcpy(queryData.szSortQuery, queryText);
  }

  this->m_kRequestState.SetRequest(static_cast<CUnmannedTraderRequestLimiter::REQUEST_TYPE>(4));
  CLogTypeDBTaskManager::Instance()->Push(1u, reinterpret_cast<char *>(&queryData), 888);
}

unsigned __int8 CUnmannedTraderUserInfo::CheckSearch(
  unsigned __int8 byType,
  _unmannedtrader_search_list_request_clzo *pRequest,
  unsigned int *dwListIndex,
  unsigned int *dwCurVer)
{
  if (this->m_wInx >= 2532)
  {
    return 99;
  }

  CPlayer *owner = &g_Player[this->m_wInx];
  if (owner->m_dwObjSerial != this->m_dwUserSerial)
  {
    return 99;
  }
  if (!this->m_kRequestState.IsEmpty())
  {
    return 95;
  }
  if (!pRequest->bUseNpcLink && !IsBeNearStore(owner, -1))
  {
    return 2;
  }

  CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
  if (!groupTable->IsExistGroupID(
        pRequest->byDivision,
        pRequest->byClass,
        pRequest->bySubClass,
        pRequest->bySortType,
        dwListIndex))
  {
    return 3;
  }
  if (!groupTable->GetVersion(pRequest->byDivision, pRequest->byClass, dwCurVer))
  {
    return 51;
  }
  return *dwCurVer == pRequest->dwVer ? 10 : 0;
}

void CUnmannedTraderUserInfo::ReRegist(
  unsigned __int8 byType,
  _unmannedtrader_re_regist_request_clzo *pRequest,
  CLogFile *pkLogger)
{
  if (byType >= 2u || pRequest->byRegedNum == 0)
  {
    return;
  }

  CountRegistItem();
  unsigned int totalTax = 0;

  _qry_case_unmandtrader_re_registsingleitem queryData{};
  queryData.byType = 0;
  queryData.wInx = this->m_wInx;
  queryData.byNum = pRequest->byRegedNum;
  queryData.dwOwnerSerial = this->m_dwUserSerial;

  for (int index = 0; index < pRequest->byRegedNum; ++index)
  {
    _qry_case_unmandtrader_re_registsingleitem::__list &entry = queryData.List[index];
    entry.bRegist = pRequest->List[index].bRegist;
    entry.wItemSerial = pRequest->List[index].wItemSerial;
    entry.dwPrice = pRequest->List[index].dwPrice;
    entry.dwRegistSerial = pRequest->List[index].dwRegistSerial;
    entry.byUpdateState = 11;

    if (!entry.bRegist)
    {
      continue;
    }

    entry.byProcRet = CheckReRegist(
      byType,
      pkLogger,
      pRequest->List[index].wItemSerial,
      pRequest->List[index].byAmount,
      pRequest->List[index].byItemTableCode,
      pRequest->List[index].wItemIndex,
      pRequest->List[index].dwRegistSerial,
      pRequest->List[index].dwPrice,
      &entry.byClass1,
      &entry.byClass2,
      &entry.byClass3,
      &entry.dwTax,
      &entry.dwListIndex);
    if (!entry.byProcRet)
    {
      entry.byUpdateState = 1;
      totalTax += entry.dwTax;
    }
  }

  if (totalTax)
  {
    g_Player[this->m_wInx].SubDalant(totalTax);
    const int level = g_Player[this->m_wInx].m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
        static_cast<unsigned __int8>(g_Player[this->m_wInx].m_Param.GetRaceCode()),
        level,
        totalTax);
    }
  }

  this->m_kRequestState.SetRequest(static_cast<CUnmannedTraderRequestLimiter::REQUEST_TYPE>(5));
  g_Main.PushDQSData(
    -1,
    nullptr,
    140,
    reinterpret_cast<char *>(&queryData),
    static_cast<int>(queryData.size()));
}

unsigned __int8 CUnmannedTraderUserInfo::CheckReRegist(
  unsigned __int8 byType,
  CLogFile *pkLogger,
  unsigned __int16 wItemSerial,
  unsigned __int8 byAmount,
  unsigned __int8 byItemTableCode,
  unsigned __int16 wItemIndex,
  unsigned int dwRegistSerial,
  unsigned int dwPrice,
  unsigned __int8 *pbyDivision,
  unsigned __int8 *pbyClass,
  unsigned __int8 *pbySubClass,
  unsigned int *pdwTax,
  unsigned int *pdwListIndex)
{
if (this->m_wInx >= 0x9E4u || byType >= 2u)
  {
    return 99;
  }

  CPlayer *owner = &g_Player[this->m_wInx];
  if (owner->m_dwObjSerial != this->m_dwUserSerial)
  {
    return 99;
  }
  if (!this->m_kRequestState.IsEmpty())
  {
    return 95;
  }
  if (g_Main.IsReleaseServiceMode() && owner->m_byUserDgr)
  {
    return 21;
  }

  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
  if (!inventoryItem)
  {
    return 8;
  }

  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return 25;
  }
  if (!it->IsWaitNoitfyClose())
  {
    return static_cast<unsigned __int8>(-53);
  }
  if (this->m_byRegistCnt >= this->m_byMaxRegistCnt)
  {
    return 6;
  }

  *pdwTax = dwPrice / 1000u;
  if (*pdwTax > owner->m_Param.GetDalant())
  {
    return static_cast<unsigned __int8>(-55);
  }
  if (dwPrice > MAX_DALANT)
  {
    return 22;
  }
  if (inventoryItem->m_byTableCode != byItemTableCode || inventoryItem->m_wItemIndex != wItemIndex)
  {
    return 9;
  }

  if (IsOverLapItem(inventoryItem->m_byTableCode))
  {
    if (byAmount > inventoryItem->m_dwDur)
    {
      return 17;
    }
    if (byAmount < inventoryItem->m_dwDur)
    {
      return 9;
    }
  }

  if (inventoryItem->m_byTableCode == 15
      && !g_Main.m_tblEffectData[1].GetRecord(CPlayer::s_pnLinkForceItemToEffect[inventoryItem->m_wItemIndex]))
  {
    return 23;
  }

  CUnmannedTraderGroupItemInfoTable *groupTable = CUnmannedTraderGroupItemInfoTable::Instance();
  if (!groupTable->GetGroupID(
        inventoryItem->m_byTableCode,
        inventoryItem->m_wItemIndex,
        pbyDivision,
        pbyClass,
        pbySubClass,
        pdwListIndex))
  {
    return 3;
  }
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(g_Player[this->m_wInx].m_id.wIndex) == 99)
  {
    return static_cast<unsigned __int8>(-52);
  }
  return 0;
}

CPlayer *CUnmannedTraderUserInfo::FindOwner()
{
  CPlayer *player = nullptr;
  if (g_Player[this->m_wInx].m_id.dwSerial == this->m_dwUserSerial && g_Player[this->m_wInx].m_bLive)
  {
    player = &g_Player[this->m_wInx];
  }
  else
  {
    CPlayer *candidate = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, this->m_dwUserSerial);
    if (candidate && candidate->m_bOper)
    {
      player = candidate;
    }
  }

  return player ? player : nullptr;
}

unsigned __int16 CUnmannedTraderUserInfo::GetIndex()
{
  return this->m_wInx;
}

bool CUnmannedTraderUserInfo::IsLogInState()
{
  return this->m_eState == LOG_IN_STATE::UTUI_LOGIN;
}

unsigned int CUnmannedTraderUserInfo::GetSerial()
{
  return this->m_dwUserSerial;
}

unsigned __int8 CUnmannedTraderUserInfo::GetMaxRegistCnt()
{
  return this->m_byMaxRegistCnt;
}

const CUnmannedTraderRegistItemInfo *CUnmannedTraderUserInfo::GetRegItemInfo()
{
  if (IsNull() || this->m_vecRegistItemInfo.empty())
  {
    return nullptr;
  }

  return &this->m_vecRegistItemInfo[0];
}

void CUnmannedTraderUserInfo::SendSearchErrorResult(unsigned __int16 wInx, char byRet)
{
  _unmannedtrader_search_list_result_zocl msg{};
  msg.byRet = byRet;

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 30;
  pbyType[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CUnmannedTraderUserInfo::LogOut(unsigned int dwSerial, CLogFile *pkLogger)
{
  if (this->m_dwUserSerial != dwSerial && pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::LogOut( DWORD dwSerial )\r\n\t\tm_wInx(%u) m_dwSerial(%u) != dwSerial(%u)\r\n",
      this->m_wInx,
      this->m_dwUserSerial,
      dwSerial);
  }
  Clear();
  this->m_eState = LOG_IN_STATE::UTUI_EMPTY;
}

bool CUnmannedTraderUserInfo::CheatCancelRegist(unsigned __int8 byNth)
{
  if (byNth == static_cast<unsigned __int8>(-1))
  {
    return CheatCancelRegistAll();
  }

  if (m_vecLoadItemInfo.size() <= byNth)
  {
    return false;
  }

  return CheatCancelRegistSingle(byNth);
}

bool CUnmannedTraderUserInfo::CheatCancelRegistAll()
{
  _qry_case_unmandtrader_cheat_updateregisttime qryData{};
  qryData.byType = 0;
  qryData.byNum = 0;
  qryData.wInx = m_wInx;
  qryData.dwOwnerSerial = m_dwUserSerial;

  auto it = m_vecRegistItemInfo.begin();
  for (unsigned __int8 index = 0; index < static_cast<unsigned __int8>(m_vecRegistItemInfo.size()); ++index)
  {
    it = FindRegist(it);
    if (it == m_vecRegistItemInfo.end())
    {
      break;
    }

    qryData.List[qryData.byNum].dwRegistSerial = it->GetRegistSerial();
    ++qryData.byNum;
    ++it;
  }

  if (qryData.byNum)
  {
    g_Main.PushDQSData(
      -1,
      nullptr,
      141,
      reinterpret_cast<char *>(&qryData),
      static_cast<int>(qryData.size()));
  }

  return true;
}

bool CUnmannedTraderUserInfo::CheatCancelRegistSingle(unsigned __int8 byNth)
{
  CPlayer *owner = FindOwner();
  if (!owner)
  {
    return false;
  }

  auto it = m_vecRegistItemInfo.begin();
  unsigned __int8 registCount = 0;
  unsigned __int8 registIndex = 0;
  bool found = false;
  for (unsigned __int8 index = 0; index < static_cast<unsigned __int8>(m_vecRegistItemInfo.size()); ++index)
  {
    it = FindRegist(it);
    if (it == m_vecRegistItemInfo.end())
    {
      break;
    }

    ++registCount;
    if (byNth <= registCount)
    {
      registIndex = index;
      found = true;
      break;
    }

    ++it;
  }

  if (found)
  {
    if (!it->IsRegist())
    {
      char message[272]{};
      const CUnmannedTraderItemState::STATE state = it->GetState();
      sprintf_s(message, "(%u)th : %u State(%d) Not Regist Item!", byNth, registIndex, static_cast<int>(state));
      owner->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, message, 0xFFu, 0LL);
      return false;
    }

    _qry_case_unmandtrader_cheat_updateregisttime qryData{};
    qryData.byType = 0;
    qryData.byNum = 1;
    qryData.wInx = m_wInx;
    qryData.dwOwnerSerial = m_dwUserSerial;
    qryData.List[0].dwRegistSerial = it->GetRegistSerial();
    g_Main.PushDQSData(
      -1,
      nullptr,
      141,
      reinterpret_cast<char *>(&qryData),
      static_cast<int>(qryData.size()));
    return true;
  }

  char buffer[288]{};
  sprintf_s(buffer, "Not Exist (%u)th Index %u!", byNth, static_cast<unsigned int>(registCount));
  owner->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, 0LL);
  return false;
}

void CUnmannedTraderUserInfo::SendSearchResult(
  unsigned __int16 wInx,
  _lt_qry_case_unmandtrader_select_list *pLoadData)
{
  _unmannedtrader_search_list_result_zocl msg{};
  msg.byRet = 0;

  msg.dwListIndex = pLoadData->dwListIndex;
  msg.byDivision = static_cast<char>(pLoadData->byClass1);
  msg.byClass = static_cast<char>(pLoadData->byClass2);
  msg.bySubClass = static_cast<char>(pLoadData->byClass3);
  msg.bySortType = static_cast<char>(pLoadData->bySortType);
  msg.dwVer = pLoadData->dwVer;
  msg.byPage = static_cast<char>(pLoadData->byPage);
  msg.dwMaxPage = pLoadData->dwMaxPage;
  const unsigned __int8 itemCount = static_cast<unsigned __int8>(pLoadData->dwCnt);
  msg.byCnt = static_cast<char>(itemCount);

  __time64_t now{};
  time_18(reinterpret_cast<__int64 *>(&now));

  _INVENKEY key;
  for (unsigned int index = 0; index < itemCount; ++index)
  {
    msg.List[index].dwRegistSerial = pLoadData->list[index].dwSerial;
    key.LoadDBKey(static_cast<int>(pLoadData->list[index].dwK));
    msg.List[index].byTableCode = static_cast<char>(key.byTableCode);
    msg.List[index].wItemIndex = key.wItemIndex;
    msg.List[index].dwD = pLoadData->list[index].dwD;
    msg.List[index].dwU = pLoadData->list[index].dwU;
    msg.List[index].dwPrice = pLoadData->list[index].dwPrice;
    msg.List[index].dwOwner = pLoadData->list[index].dwOwner;

    const char *source = pLoadData->list[index].wszOwnerName[0] ? pLoadData->list[index].wszOwnerName : "NULL";
    strcpy_s(msg.List[index].wszOwnerName, sizeof(msg.List[index].wszOwnerName), source);

    const __int64 regTime =
      pLoadData->list[index].tRegdate +
      3600 * static_cast<unsigned __int8>(pLoadData->list[index].bySellturm);
    msg.List[index].dwLeftSec = static_cast<unsigned int>(regTime - now);
  }

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 30;
  pbyType[1] = 33;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

bool CUnmannedTraderUserInfo::operator==(unsigned int dwSerial)
{
  return this->m_dwUserSerial && this->m_dwUserSerial == dwSerial;
}

CUnmannedTraderUserInfo &CUnmannedTraderUserInfo::operator=(const CUnmannedTraderUserInfo &rhs)
{
  this->m_eState = rhs.m_eState;
  this->m_wInx = rhs.m_wInx;
  this->m_dwUserSerial = rhs.m_dwUserSerial;
  this->m_byRegistCnt = rhs.m_byRegistCnt;
  this->m_byMaxRegistCnt = rhs.m_byMaxRegistCnt;
  this->m_kRequestState.m_eState = rhs.m_kRequestState.m_eState;
  this->m_vecRegistItemInfo = rhs.m_vecRegistItemInfo;
  this->m_vecLoadItemInfo = rhs.m_vecLoadItemInfo;
  return *this;
}

std::vector<CUnmannedTraderRegistItemInfo>::iterator CUnmannedTraderUserInfo::Find(unsigned int dwRegistSerial)
{
  for (unsigned __int8 index = 0; index < this->m_byMaxRegistCnt; ++index)
  {
    if (this->m_vecRegistItemInfo[index].GetRegistSerial() == dwRegistSerial)
    {
      return this->m_vecRegistItemInfo.begin() + index;
    }
  }
  return this->m_vecRegistItemInfo.end();
}

std::vector<CUnmannedTraderRegistItemInfo>::iterator CUnmannedTraderUserInfo::FindRegist(
  std::vector<CUnmannedTraderRegistItemInfo>::iterator iterStart)
{
  while (iterStart != m_vecRegistItemInfo.end())
  {
    const CUnmannedTraderItemState::STATE state = iterStart->GetState();
    if (state == CUnmannedTraderItemState::STATE::WATEREL || state == CUnmannedTraderItemState::STATE::SOILEL)
    {
      return iterStart;
    }
    ++iterStart;
  }
  return m_vecRegistItemInfo.end();
}

std::vector<CUnmannedTraderRegistItemInfo>::iterator CUnmannedTraderUserInfo::FindEmpty()
{
  for (unsigned __int8 index = 0; index < this->m_byMaxRegistCnt; ++index)
  {
    if (this->m_vecRegistItemInfo[index].IsEmpty())
    {
      return this->m_vecRegistItemInfo.begin() + index;
    }
  }
  return this->m_vecRegistItemInfo.end();
}

void CUnmannedTraderUserInfo::CountRegistItem()
{
  this->m_byRegistCnt = 0;
  const size_t maxCount =
    std::min<size_t>(this->m_byMaxRegistCnt, this->m_vecRegistItemInfo.size());
  for (size_t index = 0; index < maxCount; ++index)
  {
    if (this->m_vecRegistItemInfo[index].IsRegist())
    {
      ++this->m_byRegistCnt;
    }
  }
}

void CUnmannedTraderUserInfo::GetCurrentRegItemStateStr(char *szStateStr, int iBuffSize)
{
  char szTran[1024]{};
  char buffer[1024]{};
  int index = 0;

  for (auto it = this->m_vecRegistItemInfo.begin(); it != this->m_vecRegistItemInfo.end(); ++it, ++index)
  {
    const CUnmannedTraderItemState::STATE state = it->GetState();
    wchar_t *stateStrW = CUnmannedTraderItemState::GetStateStrW(static_cast<unsigned int>(state));
    W2M(reinterpret_cast<char *>(stateStrW), szTran, 1024);
    sprintf_s(buffer, 1024, "%d:(%d)%s ", index, static_cast<int>(state), szTran);
    strcat_s(szStateStr, iBuffSize, buffer);
  }
}

CUnmannedTraderItemState::STATE CUnmannedTraderUserInfo::GetCloseItemForPassTimeUpdateInfo(
  unsigned int dwRegistSerial,
  CPlayer **pkOwner)
{
  if (IsNull())
  {
    return static_cast<CUnmannedTraderItemState::STATE>(6);
  }

  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return static_cast<CUnmannedTraderItemState::STATE>(-1);
  }

  *pkOwner = FindOwner();
  if (!*pkOwner)
  {
    return static_cast<CUnmannedTraderItemState::STATE>(6);
  }

  if (it->IsRegist())
  {
    if (it->IsOverRegistTime())
    {
      return static_cast<CUnmannedTraderItemState::STATE>(13);
    }
    return static_cast<CUnmannedTraderItemState::STATE>(-1);
  }

  return static_cast<CUnmannedTraderItemState::STATE>(-1);
}

bool CUnmannedTraderUserInfo::CompleteUpdateState(
  unsigned int dwRegistSerial,
  unsigned __int8 byState,
  bool bReCountRegist)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    return false;
  }

  if (it->SetState(byState))
  {
    if (bReCountRegist)
    {
      CountRegistItem();
    }
    return true;
  }

  return false;
}

void CUnmannedTraderUserInfo::SetAllItemState(unsigned __int8 byState, unsigned __int8 byMaxCnt)
{
  if (m_vecRegistItemInfo.size() >= byMaxCnt)
  {
    for (unsigned __int8 index = 0; index < byMaxCnt; ++index)
    {
      m_vecRegistItemInfo[index].SetState(byState);
    }
  }
}

void CUnmannedTraderUserInfo::PrcoSellUpdateWaitItem(
  _qry_case_unmandtrader_log_in_proc_update_complete *pkResult,
  unsigned __int8 byGroupType,
  CLogFile *pkLogger)
{
  (void)byGroupType;
  CPlayer *owner = FindOwner();
  if (owner && owner->m_bOper)
  {
    for (int index = 0; index < 20; ++index)
    {
      pkResult->List[index].byProcUpdate = 255;
      CUnmannedTraderRegistItemInfo &loadItem = m_vecLoadItemInfo[index];
      if (!loadItem.IsSellUpdateWait())
      {
        continue;
      }

      _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(loadItem.GetItemSerial());
      if (inventoryItem)
      {
        pkResult->List[pkResult->wNum].wItemSerial = inventoryItem->m_wSerial;
        pkResult->List[pkResult->wNum].bySellTurm = loadItem.GetSellTurm();
        pkResult->List[pkResult->wNum].byProcUpdate = 92;
        pkResult->List[pkResult->wNum].dwBuyer = 0;
        pkResult->List[pkResult->wNum].dwRegistSerial = loadItem.GetRegistSerial();
        pkResult->List[pkResult->wNum++].byUpdateState = 1;
        loadItem.ClearBuyerInfo();
        inventoryItem->lock(true);
        loadItem.SetState(14u);
      }
      else
      {
        pkResult->List[pkResult->wNum].byProcUpdate = 83;
        pkResult->List[pkResult->wNum].dwBuyer = 0;
        pkResult->List[pkResult->wNum].dwRegistSerial = loadItem.GetRegistSerial();
        pkResult->List[pkResult->wNum++].byUpdateState = 5;
        loadItem.ClearToWaitState();
      }
    }
  }
  else if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::PrcoSellUpdateWaitItem()\r\n\t\t( 0 == pkOwner || !pkOwner->m_bOper(%d) )\r\n",
      owner ? owner->m_bOper : -1);
  }
}

void CUnmannedTraderUserInfo::SetCompleteInfo(CLogFile *pkLogger)
{
  SetAllItemState(0, this->m_byMaxRegistCnt);

  int setItemCount = 0;
  for (int index = 0; index < 20; ++index)
  {
    CUnmannedTraderRegistItemInfo &loadItem = this->m_vecLoadItemInfo[index];
    if (!loadItem.IsRegist()
      && !loadItem.IsWaitNoitfyClose()
      && !loadItem.IsWaitingRegistingSellUpdateItemWhenLogIn())
    {
      continue;
    }

    if (this->m_byRegistCnt <= this->m_byMaxRegistCnt)
    {
      this->m_vecRegistItemInfo[setItemCount] = loadItem;
      ++setItemCount;
      if (loadItem.IsRegist())
      {
        ++this->m_byRegistCnt;
      }
    }
    else
    {
      CUnmannedTraderItemState::PushUpdateState(
        0,
        loadItem.GetRegistSerial(),
        7u,
        this->m_dwUserSerial,
        65535,
        255,
        65535);
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::SetCompleteInfo( ... )\r\n"
          "\t\tm_byRegistCnt(%u) > m_byMaxRegistCnt(%u)\r\n"
          "\t\tCUnmannedTraderItemState::PushUpdateState( CUnmannedTraderItemState::SINGLE\r\n"
          "\t\t, m_vecLoadItemInfo[i].GetRegistSerial()(%u)\r\n"
          "\t\t, CUnmannedTraderItemState::CANCELREGISTFORSERVERINTERNALERROR, dwSerial )\r\n"
          "\t\tName(%s) Serial(%u) !\r\n",
          this->m_byRegistCnt,
          this->m_byMaxRegistCnt,
          loadItem.GetRegistSerial(),
          g_Player[this->m_wInx].m_Param.GetCharNameA(),
          g_Player[this->m_wInx].m_id.dwSerial);
      }
    }
  }

  if (this->m_byRegistCnt > this->m_byMaxRegistCnt)
  {
    this->m_byRegistCnt = this->m_byMaxRegistCnt;
  }
}

void CUnmannedTraderUserInfo::CompleteCreate(CLogFile *pkLogger)
{
  _qry_case_unmandtrader_log_in_proc_update_complete updateInfo{};
  std::memset(&updateInfo, 0, sizeof(updateInfo));
  updateInfo.wInx = this->m_wInx;
  updateInfo.dwSeller = this->m_dwUserSerial;
  updateInfo.byType = 0;
  updateInfo.wNum = 0;

  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCreate()\r\n\t\t( 0 == pkOwner(%p) || !pkOwner->m_bOper(%d) )\r\n",
        owner,
        owner ? owner->m_bOper : -1);
    }
    return;
  }

  CPlayer::s_MgrItemHistory.ClearLogBuffer();
  PrcoSellUpdateWaitItem(&updateInfo, 0, pkLogger);
  SetCompleteInfo(pkLogger);
  NotifyRegistItem();
  ProcSellWaitItem(&updateInfo, 0, pkLogger);
  NotifyCloseItem(&updateInfo, pkLogger);
  ClearLoadItemInfo();
  CPlayer::s_MgrItemHistory.have_auto_item(owner->m_ObjID.m_wIndex, &this->m_vecRegistItemInfo[0], 10u);
  CPlayer::s_MgrItemHistory.WriteLog(owner->m_szItemHistoryFileName);

  if (updateInfo.wNum)
  {
    g_Main.PushDQSData(
      -1,
      nullptr,
      111,
      reinterpret_cast<char *>(&updateInfo),
      static_cast<int>(sizeof(updateInfo)));
  }
  this->m_eState = LOG_IN_STATE::UTUI_LOGIN;
}

void CUnmannedTraderUserInfo::ProcSellWaitItem(
  _qry_case_unmandtrader_log_in_proc_update_complete *pkResult,
  unsigned __int8 byGroupType,
  CLogFile *pkLogger)
{
  (void)byGroupType;

  _unmannedtrader_Sell_Wait_item_inform_zocl inform{};
  std::memset(&inform, 0, sizeof(inform));

  unsigned __int8 deleteStorageSlot[10]{};
  unsigned __int8 byProcResult = 0;
  CPlayer *owner = FindOwner();
  if (!owner)
  {
    return;
  }

  for (int index = 0; index < 20; ++index)
  {
    CUnmannedTraderRegistItemInfo &loadItem = this->m_vecLoadItemInfo[index];
    if (!loadItem.IsSellWait())
    {
      continue;
    }

    unsigned __int8 *storageIndex = &deleteStorageSlot[inform.byNum];
    byProcResult = loadItem.SellWaitItem(this->m_wInx, pkLogger, loadItem.GetResultTime(), storageIndex);
    if (byProcResult == 37 || byProcResult == 94 || byProcResult == 34)
    {
      pkResult->List[pkResult->wNum].byProcUpdate = 37;
      pkResult->List[pkResult->wNum].dwBuyer = loadItem.GetBuyerSerial();
      pkResult->List[pkResult->wNum].dwRegistSerial = loadItem.GetRegistSerial();
      pkResult->List[pkResult->wNum++].byUpdateState = 8;
      loadItem.Clear();
      loadItem.SetState(8u);
      continue;
    }

    if (byProcResult)
    {
      loadItem.ClearToWaitState();
      continue;
    }

    inform.List[inform.byNum].wItemSerial = loadItem.GetItemSerial();
    inform.List[inform.byNum].dwSellDalant = loadItem.GetPrice();
    inform.List[inform.byNum].dwTax = loadItem.GetTax();
    inform.List[inform.byNum].dwSellDalant -= inform.List[inform.byNum].dwTax;
    inform.dwTotalSellDalant += inform.List[inform.byNum].dwSellDalant;
    inform.dwTotalTaxDalant += inform.List[inform.byNum++].dwTax;

    pkResult->List[pkResult->wNum].byProcUpdate = 90;
    pkResult->List[pkResult->wNum].dwBuyer = loadItem.GetBuyerSerial();
    pkResult->List[pkResult->wNum].dwRegistSerial = loadItem.GetRegistSerial();
    pkResult->List[pkResult->wNum++].byUpdateState = 3;

    loadItem.Clear();
    loadItem.SetState(3u);
  }

  inform.dwCurInvenDalant = owner->m_Param.GetDalant();
  unsigned __int8 type[2] = {30, 26};
  const unsigned __int16 length = GetSellWaitInformSize(inform);
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, type, reinterpret_cast<char *>(&inform), length);

  for (unsigned __int8 index = 0; index < inform.byNum; ++index)
  {
    owner->Emb_DelStorage(0, deleteStorageSlot[index], false, true, "CUnmannedTraderUserInfo::ProcSellWaitItem()");
  }
}

void CUnmannedTraderUserInfo::NotifyCloseItem(
  _qry_case_unmandtrader_log_in_proc_update_complete *pkResult,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (owner && owner->m_bOper)
  {
    _STORAGE_LIST::_db_con *regItem = nullptr;
    _unmannedtrader_continue_item_inform_zocl inform{};
    std::memset(&inform, 0, sizeof(inform));
    inform.byTaxRate = 1;

    for (int index = 0; index < 20; ++index)
    {
      CUnmannedTraderRegistItemInfo &loadItem = this->m_vecLoadItemInfo[index];
      if (!loadItem.IsWaitNoitfyClose())
      {
        continue;
      }

      regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(loadItem.GetItemSerial());
      if (regItem)
      {
        CPlayer::s_MgrItemHistory.login_cancel_auto_trade(
          owner->m_ObjID.m_wIndex,
          loadItem.GetRegistSerial(),
          regItem,
          loadItem.GetResultTime());

        inform.List[inform.byNum].dwPrice = loadItem.GetPrice();
        inform.List[inform.byNum].wItemSerial = loadItem.GetItemSerial();
        inform.List[inform.byNum].dwRegistSerial = loadItem.GetRegistSerial();
        ++inform.byNum;

        loadItem.Clear();
        loadItem.SetState(11);
      }
      else
      {
        pkResult->List[pkResult->wNum].byProcUpdate = 82;
        pkResult->List[pkResult->wNum].dwBuyer = loadItem.GetBuyerSerial();
        pkResult->List[pkResult->wNum].dwRegistSerial = loadItem.GetRegistSerial();
        pkResult->List[pkResult->wNum++].byUpdateState = 7;

        loadItem.Clear();
        loadItem.SetState(7u);
      }
    }

    unsigned __int8 type[2] = {30, 38};
    if (inform.byNum)
    {
      const unsigned __int16 length = static_cast<unsigned __int16>(inform.size());
      g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, type, reinterpret_cast<char *>(&inform), length);
    }
  }
  else
  {
    const int ownerOper = owner ? owner->m_bOper : -1;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::NotifyCloseItem()\r\n\t\t( 0 == pkOwner || !pkOwner->m_bOper(%d) )\r\n",
        ownerOper);
    }
  }
}

bool CUnmannedTraderUserInfo::CheckIsUpdatedTaxRate(unsigned __int8 byTax, CLogFile *pkLogger)
{
  (void)pkLogger;

  if (this->m_wInx >= MAX_PLAYER)
  {
    return true;
  }

  CPlayer *owner = &g_Player[this->m_wInx];
  if (owner->m_dwObjSerial != this->m_dwUserSerial)
  {
    return true;
  }

  const int raceCode = static_cast<int>(owner->m_Param.GetRaceCode());
  const unsigned __int8 currentTaxRate =
    static_cast<unsigned __int8>(CUnmannedTraderTaxRateManager::Instance()->GetTaxRate(raceCode) * 100.0f);

  if (currentTaxRate == byTax)
  {
    return false;
  }

  _unmannedtrader_taxrate_inform_zocl msg{};
  msg.byTaxRate = static_cast<char>(currentTaxRate);

  unsigned __int8 type[2] = {30, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, type, reinterpret_cast<char *>(&msg), sizeof(msg));
  return true;
}

void CUnmannedTraderUserInfo::CompleteRegist(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_registsingleitem *pLoadData,
  CLogFile *pkLogger)
{
  const unsigned __int16 sepaSerial = pLoadData->wSepaSerial;
  const unsigned __int16 itemSerial = sepaSerial == 65535 ? pLoadData->wItemSerialOld : sepaSerial;
  const unsigned int registSerial = pLoadData->dwRegedSerial;
  const unsigned int price = pLoadData->dwPrice;
  const unsigned int taxDalant = pLoadData->dwTax;
  const unsigned int ownerSerial = pLoadData->dwOwnerSerial;
  const unsigned int dbKey = pLoadData->dwK;
  const unsigned __int64 dbDur = pLoadData->dwD;
  const unsigned int dbLv = pLoadData->dwU;
  const unsigned __int8 sellTurm = pLoadData->bySellTurm;
  const unsigned __int8 storageIndex = pLoadData->byStorageIndex;
  const bool inserted = pLoadData->bInserted;
  const unsigned __int8 byType = pLoadData->byType;
  const unsigned __int8 class1 = pLoadData->byClass1;
  const unsigned __int8 class2 = pLoadData->byClass2;

  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    return;
  }

  _STORAGE_LIST::_storage_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
  if (!regItem)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegist( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
        "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
        itemSerial);
    }
    return;
  }

  if (byRet)
  {
    if (sepaSerial != 65535)
    {
      owner->Emb_AlterDurPoint(
        0,
        pLoadData->byOldStorageIndex,
        static_cast<int>(pLoadData->bySepaAmount),
        false,
        false);
      owner->Emb_DelStorage(
        0,
        pLoadData->byStorageIndex,
        false,
        false,
        "CUnmannedTraderUserInfo::CompleteRegist()");
    }

    regItem->lock(false);
    owner->AddDalant(static_cast<int>(taxDalant), true);
    const unsigned int leftDalant = owner->m_Param.GetDalant();
    SendRegistItemErrorResult(owner->m_ObjID.m_wIndex, 24, itemSerial, leftDalant);
    return;
  }

  _INVENKEY invenKey;
  invenKey.LoadDBKey(dbKey);
  if (CompleteRegistItem(
        registSerial,
        itemSerial,
        regItem->m_dwETSerialNumber,
        price,
        sellTurm,
        invenKey.byTableCode,
        invenKey.wItemIndex,
        storageIndex,
        dbDur,
        dbLv,
        inserted))
  {
    const unsigned int leftDalant = owner->m_Param.GetDalant();
    CPlayer::s_MgrItemHistory.reg_auto_trade(
      owner->m_ObjID.m_wIndex,
      registSerial,
      reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
      price,
      taxDalant,
      leftDalant,
      owner->m_szItemHistoryFileName);

    SendRegistItemSuccessResult(owner->m_Param.GetDalant(), owner->m_ObjID.m_wIndex, pLoadData);

    if (g_HolySys.GetHolyMasterRace() == -1)
    {
      const unsigned int totalTax = 75 * taxDalant / 100;
      const unsigned int halfTax = totalTax / 2;
      const int race = static_cast<int>(owner->m_Param.GetRaceCode());
      CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(race, halfTax);
      CHonorGuild::Instance()->SetGuildMaintainMoney(race, totalTax, owner->m_dwObjSerial);
    }
    else
    {
      const int ownerRace = static_cast<int>(owner->m_Param.GetRaceCode());
      const int holyMasterRace = g_HolySys.GetHolyMasterRace();
      unsigned int totalTax = 0;
      if (ownerRace == holyMasterRace)
      {
        totalTax = 75 * taxDalant / 100;
      }
      else
      {
        totalTax = 50 * taxDalant / 100;
      }
      const unsigned int halfTax = totalTax / 2;
      CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(holyMasterRace, halfTax);
      CHonorGuild::Instance()->SetGuildMaintainMoney(holyMasterRace, totalTax, owner->m_dwObjSerial);
    }

    if (!CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(
          invenKey.byTableCode,
          invenKey.wItemIndex,
          class1,
          class2))
    {
      const char *ownerName = owner->m_Param.GetCharNameA();
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderController::ComleteRegistItem( BYTE byRet(%u), char * pLoadData )\r\n"
          "\t\tOwner : (%u)(%u)%s\r\n"
          "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(\r\n"
          "\t\tkInvenKey.byTableCode(%u), kInvenKey.wItemIndex(%u), pkQuery->byClass1(%u), pkQuery->byClass2(%u) ) Fail!\r\n",
          0,
          this->m_dwUserSerial,
          owner->m_dwObjSerial,
          ownerName,
          invenKey.byTableCode,
          invenKey.wItemIndex,
          class1,
          class2);
      }
    }
    return;
  }

  char stateStr[2048]{};
  GetCurrentRegItemStateStr(stateStr, 2048);
  const char *ownerName = owner->m_Param.GetCharNameA();
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::ComleteRegistItem( BYTE byRet(%u), char * pLoadData )\r\n"
      "\t\tOwner : (%u)(%u)%s\r\n"
      "\t\tCurrent Reg Item State %s\r\n"
      "\t\tCompleteRegistItem( pkQuery->dwRegedSerial(%u), wItemSerial(%u), pRegItem->m_dwETSerialNumber(%u), pkQuery->dwPrice(%u), pkQuery->dwTax(%u)\r\n"
      "\t\t, pkQuery->bySellTurm(%u), kInvenKey.byTableCode(%u), kInvenKey.wItemIndex(%u)\r\n"
      "\t\t, pkQuery->bInserted, pkLogger ) Fail!\r\n",
      0,
      this->m_dwUserSerial,
      owner->m_dwObjSerial,
      ownerName,
      stateStr,
      registSerial,
      itemSerial,
      regItem->m_dwETSerialNumber,
      price,
      taxDalant,
      sellTurm,
      invenKey.byTableCode,
      invenKey.wItemIndex);
  }

  if (sepaSerial != 65535)
  {
    owner->Emb_AlterDurPoint(
      0,
      pLoadData->byOldStorageIndex,
      static_cast<int>(pLoadData->bySepaAmount),
      false,
      false);
    owner->Emb_DelStorage(
      0,
      pLoadData->byStorageIndex,
      false,
      false,
      "CUnmannedTraderUserInfo::CompleteRegist()");
  }
  regItem->lock(false);
  owner->AddDalant(static_cast<int>(taxDalant), true);
  const unsigned int leftDalant = owner->m_Param.GetDalant();
  SendRegistItemErrorResult(owner->m_ObjID.m_wIndex, 6u, itemSerial, leftDalant);
  CUnmannedTraderItemState::PushUpdateState(
    byType,
    registSerial,
    12,
    ownerSerial,
    itemSerial,
    255,
    65535);
}

bool CUnmannedTraderUserInfo::CompleteRegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  unsigned int dwETSerialNumber,
  unsigned int dwPrice,
  unsigned __int8 bySellTurm,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex,
  unsigned __int8 byStorageIndex,
  unsigned __int64 dwD,
  unsigned int dwU,
  bool bInserted)
{
  auto it = FindEmpty();
  if (it == this->m_vecRegistItemInfo.end())
  {
    return false;
  }

  it->RegistItem(
    dwRegistSerial,
    dwItemSerial,
    dwETSerialNumber,
    dwPrice,
    bySellTurm,
    byTableCode,
    wItemIndex,
    byStorageIndex,
    dwD,
    dwU,
    bInserted);
  CountRegistItem();
  return true;
}

void CUnmannedTraderUserInfo::CompleteCancelRegist(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_cancelitem *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bLive)
  {
    return;
  }

  const unsigned __int8 procRet = pLoadData->byProcRet;
  if (byRet || procRet)
  {
    SendCancelRegistErrorResult(owner->m_ObjID.m_wIndex, procRet);
    return;
  }

  const unsigned __int16 itemSerial = pLoadData->wItemSerial;
  _STORAGE_LIST::_storage_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
  if (!regItem)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCancelRegist( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
        "\t\tpkQuery->byType(%u), pkQuery->dwRegistSerial(%u)\r\n"
        "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
        pLoadData->byType,
        pLoadData->dwRegistSerial,
        itemSerial);
    }
    SendCancelRegistErrorResult(owner->m_ObjID.m_wIndex, 8u);
    return;
  }

  regItem->lock(false);
  const unsigned int registSerial = pLoadData->dwRegistSerial;
  if (CompleteCancelRegistItem(registSerial, itemSerial, pkLogger))
  {
    if (!CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(
          regItem->m_byTableCode,
          regItem->m_wItemIndex))
    {
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteCancelRegist( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
          "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion( pRegItem->m_byTableCode(%u), pRegItem->m_wItemIndex(%u) )\r\n",
          regItem->m_byTableCode,
          regItem->m_wItemIndex);
      }
    }

    SendCancelRegistSuccessResult(owner->m_ObjID.m_wIndex, itemSerial, registSerial);
    CPlayer::s_MgrItemHistory.self_cancel_auto_trade(
      owner->m_ObjID.m_wIndex,
      registSerial,
      reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
      owner->m_szItemHistoryFileName);
  }
  else
  {
    SendCancelRegistErrorResult(owner->m_ObjID.m_wIndex, 29);
  }
}

bool CUnmannedTraderUserInfo::CompleteCancelRegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  CLogFile *pkLogger)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCancelRegistItem( DWORD dwRegistSerial(%u), WORD dwItemSerial(%u) )\r\n"
        "\t\t Find( dwRegistSerial(%u) ) NULL!\r\n",
        dwRegistSerial,
        dwItemSerial,
        dwRegistSerial);
    }
    return false;
  }

  if (it->GetItemSerial() != dwItemSerial)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteCancelRegistItem( DWORD dwRegistSerial(%u), WORD dwItemSerial(%u) )\r\n"
        "\t\t (*iFind).GetItemSerial()(%u) != dwItemSerial(%u) Invalid!\r\n",
        dwRegistSerial,
        dwItemSerial,
        it->GetItemSerial(),
        dwRegistSerial);
    }
  }

  it->ClearRegist();
  CountRegistItem();
  return true;
}

void CUnmannedTraderUserInfo::CompleteTimeOutClear(unsigned int dwRegistSerial, CLogFile *pkLogger)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteTimeOutClear(...)\r\n\t\tFind( dwRegistSerial(%u) ) Item Fail!\r\n",
        dwRegistSerial);
    }
    return;
  }

  const unsigned __int16 itemSerial = it->GetItemSerial();
  it->SetState(13);
  CountRegistItem();

  CPlayer *owner = FindOwner();
  if (owner && owner->m_bOper)
  {
    _STORAGE_LIST::_db_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (regItem)
    {
      CPlayer::s_MgrItemHistory.time_out_cancel_auto_trade(
        owner->m_ObjID.m_wIndex,
        dwRegistSerial,
        regItem,
        owner->m_szItemHistoryFileName);

      const int race = static_cast<int>(owner->m_Param.GetRaceCode());
      const unsigned __int8 taxRate =
        static_cast<unsigned __int8>(CUnmannedTraderTaxRateManager::Instance()->GetTaxRate(race) * 100.0f);
      const unsigned int price = it->GetPrice();
      SendNotifyCloseItem(owner->m_ObjID.m_wIndex, itemSerial, dwRegistSerial, price, taxRate);
    }
    else
    {
      const char *ownerName = owner->m_Param.GetCharNameA();
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteTimeOutClear(...)\r\n"
          "\t\tOnwer : Serial(%u) Name(%s)\r\n"
          "\t\tdwRegistSerial(%u)\r\n"
          "\t\tpkOwner->m_Param.m_dbInven.GetPtrFromSerial( (*i).GetItemSerial(%u) ) NULL!\r\n",
          owner->m_dwObjSerial,
          ownerName,
          dwRegistSerial,
          itemSerial);
      }
      NotifyRegistItem();
    }
    return;
  }

  const int ownerOper = owner ? owner->m_bOper : -1;
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteTimeOutClear( dwRegistSerial(%u) )\r\n"
      "\t\t( 0 == pkOwner || !pkOwner->m_bOper(%d) )\r\n",
      dwRegistSerial,
      ownerOper);
  }
}

void CUnmannedTraderUserInfo::CompleteReprice(
  unsigned __int8 byRet,
  _qry_case_unmandtrader_update_reprice *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *receiver = FindOwner();
  if (receiver && receiver->m_bLive)
  {
    const unsigned __int8 procRet = pLoadData->byProcRet;
    if (byRet || procRet)
    {
      SendRepriceErrorResult(receiver, procRet);
      receiver->AddDalant(static_cast<int>(pLoadData->dwTax), true);
      return;
    }

    const unsigned __int16 itemSerial = pLoadData->wItemSerial;
    _STORAGE_LIST::_db_con *regItem = receiver->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (regItem)
    {
      const unsigned int registSerial = pLoadData->dwRegistSerial;
      const unsigned int newPrice = pLoadData->dwNewPrice;
      if (CompleteRepriceItem(registSerial, itemSerial, newPrice))
      {
        if (!CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(
              regItem->m_byTableCode,
              regItem->m_wItemIndex))
        {
          if (pkLogger)
          {
            pkLogger->Write(
              "CUnmannedTraderUserInfo::CompleteReprice( BYTE byRet(%u), char * pLoadData, CLogFile * pkLogger )\r\n"
              "\t\tpkQuery->dwRegistSerial(%u), pkQuery->dwNewPrice(%u)\r\n"
              "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion( pRegItem->m_byTableCode(%u), pRegItem->m_wItemIndex(%u) )\r\n",
              0,
              registSerial,
              newPrice,
              regItem->m_byTableCode,
              regItem->m_wItemIndex);
          }
        }

        SendRepriceSuccessResult(
          receiver,
          itemSerial,
          newPrice,
          registSerial,
          pLoadData->dwTax);
        CPlayer::s_MgrItemHistory.price_auto_trade(
          receiver->m_ObjID.m_wIndex,
          registSerial,
          regItem,
          pLoadData->dwTax,
          pLoadData->dwOldPrice,
          newPrice,
          receiver->m_szItemHistoryFileName);
      }
      else
      {
        if (pkLogger)
        {
          pkLogger->Write(
            "CUnmannedTraderUserInfo::CompleteReprice( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
            "\t\tCompleteRepriceItem( pkQuery->dwRegistSerial(%u), pkQuery->wItemSerial(%u), pkQuery->dwNewPrice(%u), pkLogger ) Fail!\r\n",
            registSerial,
            itemSerial,
            newPrice);
        }
        SendRepriceErrorResult(receiver, 28);
      }
    }
    else
    {
      if (pkLogger)
      {
        pkLogger->Write(
          "CCUnmannedTraderUserInfo::CompleteReprice( BYTE byRet, char * pLoadData, CLogFile * pkLogger )\r\n"
          "\t\tpkQuery->dwRegistSerial(%u), pkQuery->dwNewPrice(%u)\r\n"
          "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
          pLoadData->dwRegistSerial,
          pLoadData->dwNewPrice,
          itemSerial);
      }
      SendRepriceErrorResult(receiver, 8u);
    }
    return;
  }

  if (byRet || pLoadData->byProcRet)
  {
    g_Main.Push_ChargeItem(
      pLoadData->dwOwnerSerial,
      -1,
      pLoadData->dwTax,
      268435455,
      1u);
  }
}

bool CUnmannedTraderUserInfo::CompleteRepriceItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  unsigned int dwPrice)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end() || it->GetItemSerial() != dwItemSerial)
  {
    return false;
  }

  it->RepriceItem(dwPrice);
  return true;
}

void CUnmannedTraderUserInfo::CompleteReRegist(
  _qry_case_unmandtrader_re_registsingleitem *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner)
  {
    return;
  }

  _qry_case_unmandtrader_re_registsingleitem_roll_back updateComplete{};
  updateComplete.byType = pLoadData->byType;
  updateComplete.wInx = this->m_wInx;
  updateComplete.dwOwnerSerial = this->m_dwUserSerial;

  _unmannedtrader_re_regist_result_zocl result{};
  result.byNum = pLoadData->byNum;

  for (unsigned __int8 j = 0; j < pLoadData->byNum; ++j)
  {
    _qry_case_unmandtrader_re_registsingleitem::__list &entry = pLoadData->List[j];
    const unsigned __int8 procRet = entry.byProcRet;
    const bool registFlag = entry.bRegist;
    const unsigned __int16 itemSerial = entry.wItemSerial;
    const unsigned int taxDalant = entry.dwTax;
    const unsigned int listIndex = entry.dwListIndex;
    const unsigned int price = entry.dwPrice;
    const unsigned int registSerial = entry.dwRegistSerial;

    result.List[j].byRet = procRet;
    result.List[j].bRegist = registFlag;
    result.List[j].wItemSerial = itemSerial;
    result.List[j].dwPrice = price;
    result.List[j].dwRegedSerial = registSerial;
    result.List[j].dwListIndex = listIndex;
    result.List[j].dwTax = taxDalant;

    _STORAGE_LIST::_storage_con *regItem = owner->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
    if (regItem)
    {
      if (registFlag)
      {
        if (procRet)
        {
          if (procRet == 202 || procRet == 24)
          {
            owner->AddDalant(static_cast<int>(taxDalant), true);
          }
          if (CompleteCancelRegistItem(registSerial, itemSerial, pkLogger))
          {
            regItem->lock(false);
          }
          else if (pkLogger)
          {
            pkLogger->Write(
              "CUnmannedTraderUserInfo::CompleteReRegist(...) : if( pkQuery->List[i].bRegist && 0 != pkQuery->List[i].byProcRet(%u) )"
              "CompleteCancelRegistItem( pkQuery->List[%u].dwRegedSerial(%u), pkQuery->List[%u].wItemSerial(%u) Fail!",
              procRet,
              j,
              registSerial,
              itemSerial);
          }
        }
        else if (CompleteReRegistItem(registSerial, itemSerial, price, pkLogger, &entry.byProcRet))
        {
          const unsigned int leftDalant = owner->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.re_reg_auto_trade(
            owner->m_ObjID.m_wIndex,
            registSerial,
            reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
            price,
            taxDalant,
            leftDalant,
            owner->m_szItemHistoryFileName);

          if (g_HolySys.GetHolyMasterRace() == -1)
          {
            const unsigned int totalTax = 75 * taxDalant / 100;
            const unsigned int halfTax = totalTax / 2;
            const int race = static_cast<int>(owner->m_Param.GetRaceCode());
            CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(race, halfTax);
            CHonorGuild::Instance()->SetGuildMaintainMoney(race, totalTax, owner->m_dwObjSerial);
          }
          else
          {
            const int ownerRace = static_cast<int>(owner->m_Param.GetRaceCode());
            const int holyMasterRace = g_HolySys.GetHolyMasterRace();
            unsigned int totalTax = 0;
            if (ownerRace == holyMasterRace)
            {
              totalTax = 75 * taxDalant / 100;
            }
            else
            {
              totalTax = 50 * taxDalant / 100;
            }
            const unsigned int halfTax = totalTax / 2;
            CUnmannedTraderTaxRateManager::Instance()->SetPatriarchTaxMoney(holyMasterRace, halfTax);
            CHonorGuild::Instance()->SetGuildMaintainMoney(holyMasterRace, totalTax, owner->m_dwObjSerial);
          }
        }
        else
        {
          result.List[j].byRet = entry.byProcRet;
          owner->AddDalant(static_cast<int>(taxDalant), true);
          updateComplete.List[updateComplete.byNum].byState = 12;
          updateComplete.List[updateComplete.byNum].dwRegistSerial = registSerial;
          ++updateComplete.byNum;
        }
      }
      else if (CompleteCancelRegistItem(registSerial, itemSerial, pkLogger))
      {
        regItem->lock(false);
        CPlayer::s_MgrItemHistory.self_cancel_auto_trade(
          owner->m_ObjID.m_wIndex,
          registSerial,
          reinterpret_cast<_STORAGE_LIST::_db_con *>(regItem),
          owner->m_szItemHistoryFileName);
      }
      else if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteReRegist(...) : if( !pkQuery->List[i].bRegist )CompleteCancelRegistItem( pkQuery->List[%u].dwRegedSerial(%u), pkQuery->List[%u].wItemSerial(%u) Fail!",
          j,
          registSerial,
          itemSerial);
      }
    }
    else
    {
      entry.byProcRet = 25;
      result.List[j].byRet = 25;
      const char *ownerName = owner->m_Param.GetCharNameA();
      if (pkLogger)
      {
        pkLogger->Write(
          "CUnmannedTraderUserInfo::CompleteReRegist(...)\r\n"
          "\t\tOnwer : Serial(%u) Name(%s)\r\n"
          "\t\tdwRegistSerial(%u)\r\n"
          "\t\tpkRegister->m_Param.m_dbInven.GetPtrFromSerial( wItemSerial(%u) ) NULL!\r\n",
          owner->m_dwObjSerial,
          ownerName,
          registSerial,
          itemSerial);
      }
      if (registFlag)
      {
        owner->AddDalant(static_cast<int>(taxDalant), true);
      }
      updateComplete.List[updateComplete.byNum].byState = 11;
      updateComplete.List[updateComplete.byNum].dwRegistSerial = registSerial;
      ++updateComplete.byNum;
    }
  }

  if (updateComplete.byNum)
  {
    g_Main.PushDQSData(-1, nullptr, 147, reinterpret_cast<char *>(&updateComplete), sizeof(updateComplete));
  }

  result.dwLeftDalant = owner->m_Param.GetDalant();
  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 37;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

bool CUnmannedTraderUserInfo::CompleteReRegistItem(
  unsigned int dwRegistSerial,
  unsigned __int16 dwItemSerial,
  unsigned int dwPrice,
  CLogFile *pkLogger,
  unsigned __int8 *pbyProcRet)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    *pbyProcRet = 25;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteReRegistItem(...)\r\n\t\t Find( dwRegistSerial(%u) ) Not Exist!\r\n",
        dwRegistSerial);
    }
    return false;
  }

  if (it->GetItemSerial() == dwItemSerial)
  {
    it->ReRegistItem(dwPrice);
    CountRegistItem();
    return true;
  }

  *pbyProcRet = 25;
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteReRegistItem( DWORD dwRegistSerial(%u), WORD dwItemSerial(%u) )\r\n"
      "\t\t (*iFind).GetItemSerial()(%u) != dwItemSerial(%u) Invalid!\r\n",
      dwRegistSerial,
      it->GetItemSerial(),
      dwItemSerial,
      dwRegistSerial);
  }
  return false;
}

void CUnmannedTraderUserInfo::CompleteUpdateCheatRegistTime(
  _qry_case_unmandtrader_cheat_updateregisttime *pLoadData)
{
  for (unsigned __int8 j = 0; j < pLoadData->byNum; ++j)
  {
    if (!pLoadData->List[j].byProcRet)
    {
      auto it = Find(pLoadData->List[j].dwRegistSerial);
      if (it != this->m_vecRegistItemInfo.end())
      {
        it->SetOverRegistTime();
      }
    }
  }
}

void CUnmannedTraderUserInfo::CompleteReRegistRollBack(
  _qry_case_unmandtrader_re_registsingleitem_roll_back *pData,
  CLogFile *pkLogger)
{
  if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteReRegistRollBack( char * pLoadData, CLogFile * pkLogger )\r\n"
      "\t\tType(%u) wInx(%u) Owner(%u)\r\n",
      pData->byType,
      pData->wInx,
      pData->dwOwnerSerial);
  }

  for (unsigned __int8 j = 0; j < pData->byNum; ++j)
  {
    if (!pData->List[j].byProcRet &&
        !CompleteUpdateState(pData->List[j].dwRegistSerial, pData->List[j].byState, false))
    {
      if (pkLogger)
      {
        pkLogger->Write(
          "\t\t(%d)Nth RegistSerial(%u) CompleteUpdateState(..) State(%u) Fail!\r\n",
          j,
          pData->List[j].dwRegistSerial,
          pData->List[j].byProcRet);
      }
    }
    if (pkLogger)
    {
      pkLogger->Write(
        "\t\t(%d)Nth RegistSerial(%u) byProcRet(%u)\r\n",
        j,
        pData->List[j].dwRegistSerial,
        pData->List[j].byProcRet);
    }
  }

  CountRegistItem();
}

void CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInRegisterSuccess(
  _qry_case_unmandtrader_log_in_proc_update_complete::__list *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInRegisterSuccess()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Owner Log out!\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
    return;
  }

  bool success = true;
  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(pLoadData->wItemSerial);
  if (!inventoryItem)
  {
    success = false;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Item None\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }

  auto regInfo = Find(pLoadData->dwRegistSerial);
  if (regInfo == this->m_vecRegistItemInfo.end())
  {
    success = false;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo None\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }
  else if (regInfo->GetItemSerial() != pLoadData->wItemSerial)
  {
    success = false;
    if (inventoryItem)
    {
      inventoryItem->lock(false);
    }
    regInfo->Clear();
    regInfo->SetState(7u);
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo ItemSerial different!\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }

  if (success)
  {
    regInfo->SetState(1u);
    CountRegistItem();

    CPlayer::s_MgrItemHistory.reg_ut_trade_waitingsellupdateitem_RegisterSuccess(
      owner->m_ObjID.m_wIndex,
      pLoadData->dwRegistSerial,
      inventoryItem,
      regInfo->GetPrice(),
      regInfo->GetTax(),
      owner->m_Param.GetDalant(),
      owner->m_szItemHistoryFileName);

    unsigned __int8 byDivision = 0;
    unsigned __int8 byClass = 0;
    unsigned __int8 bySubClass = 0;
    unsigned int dwListIndex = 0;
    if (CUnmannedTraderGroupItemInfoTable::Instance()->GetGroupID(
          inventoryItem->m_byTableCode,
          inventoryItem->m_wItemIndex,
          &byDivision,
          &byClass,
          &bySubClass,
          &dwListIndex))
    {
      _unmannedtrader_regist_item_success_result_zocl result{};
      result.wItemSerial = pLoadData->wItemSerial;
      result.dwPrice = pLoadData->dwPrice;
      result.dwRegedSerial = pLoadData->dwRegistSerial;
      result.dwListIndex = dwListIndex;
      result.dwTax = pLoadData->dwTax;
      result.dwLeftDalant = owner->m_Param.GetDalant();
      result.wSepaSerial = static_cast<unsigned __int16>(-1);
      result.bySepaAmount = 0;

      unsigned __int8 type[16]{};
      type[0] = 30;
      type[1] = 28;
      g_Network.m_pProcess[0]->LoadSendMsg(
        this->m_wInx,
        type,
        reinterpret_cast<char *>(&result),
        static_cast<unsigned __int16>(result.size()));

      CUnmannedTraderGroupItemInfoTable::Instance()->IncreaseVersion(byDivision, byClass);
    }
    else if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderController::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner : (%u)%s\r\n"
        "\t\tCUnmannedTraderGroupItemInfoTable::Instance()->GetGroupID()\r\n"
        "\t\tTableCode(%u), ItemIndex(%u) It's not Exist!\r\n"
        "\t\tIt's Ignored Sending Result and Increasing Version\r\n",
        this->m_dwUserSerial,
        owner->m_Param.GetCharNameA(),
        inventoryItem->m_byTableCode,
        inventoryItem->m_wItemIndex);
    }
  }
  else
  {
    CUnmannedTraderItemState::PushUpdateState(0, pLoadData->dwRegistSerial, 7u, this->m_dwUserSerial, 0xFFFFu, 0xFFu, 0xFFFFu);
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Clear Regist Info, State Update -> CANCELREGISTFORSERVERINTERNALERROR\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }
}

void CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInCancelRegist(
  _qry_case_unmandtrader_log_in_proc_update_complete::__list *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInCancelRegist()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Owner Log out!\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
    return;
  }

  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(pLoadData->wItemSerial);
  if (inventoryItem)
  {
    inventoryItem->lock(false);
  }
  else if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
      "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Item None\r\n",
      this->m_dwUserSerial,
      pLoadData->dwRegistSerial,
      pLoadData->wItemSerial);
  }

  auto regInfo = Find(pLoadData->dwRegistSerial);
  if (regInfo == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo None\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
    return;
  }

  if (regInfo->GetItemSerial() == pLoadData->wItemSerial)
  {
    regInfo->Clear();
    regInfo->SetState(7u);
    if (inventoryItem)
    {
      CPlayer::s_MgrItemHistory.reg_ut_trade_waitingsellupdateitem_RegisterFailed_CancelItem(
        pLoadData->dwRegistSerial,
        inventoryItem,
        owner->m_szItemHistoryFileName);
    }
    SendCancelRegistSuccessResult(owner->m_ObjID.m_wIndex, pLoadData->wItemSerial, pLoadData->dwRegistSerial);
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Cancel Regist Success\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }
  else if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogIn()\r\n"
      "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo ItemSerial different!\r\n",
      this->m_dwUserSerial,
      pLoadData->dwRegistSerial,
      pLoadData->wItemSerial);
  }
}

void CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInInvalidStateItem(
  _qry_case_unmandtrader_log_in_proc_update_complete::__list *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInInvalidStateItem()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Owner Log out!\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
    return;
  }

  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(pLoadData->wItemSerial);
  if (inventoryItem)
  {
    inventoryItem->lock(false);
  }
  else if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInInvalidStateItem()\r\n"
      "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Item None\r\n",
      this->m_dwUserSerial,
      pLoadData->dwRegistSerial,
      pLoadData->wItemSerial);
  }

  auto regInfo = Find(pLoadData->dwRegistSerial);
  if (regInfo == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInInvalidStateItem()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo None\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
    return;
  }

  if (regInfo->GetItemSerial() == pLoadData->wItemSerial)
  {
    regInfo->Clear();
    regInfo->SetState(7u);
    if (inventoryItem)
    {
      CPlayer::s_MgrItemHistory.reg_ut_trade_waitingsellupdateitem_InvalidState_CancelItem(
        pLoadData->dwRegistSerial,
        inventoryItem,
        owner->m_szItemHistoryFileName);
    }
    SendCancelRegistSuccessResult(owner->m_ObjID.m_wIndex, pLoadData->wItemSerial, pLoadData->dwRegistSerial);
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInInvalidStateItem()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Cancel Regist Success\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }
  else if (pkLogger)
  {
    pkLogger->Write(
      "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInInvalidStateItem()\r\n"
      "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo ItemSerial different!\r\n",
      this->m_dwUserSerial,
      pLoadData->dwRegistSerial,
      pLoadData->wItemSerial);
  }
}

void CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInSoldItem(
  int nthIndex,
  _qry_case_unmandtrader_log_in_proc_update_complete *pLoadData,
  CLogFile *pkLogger)
{
  _qry_case_unmandtrader_log_in_proc_update_complete::__list *result = &pLoadData->List[nthIndex];
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInSoldItem()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Owner Log out!\r\n",
        this->m_dwUserSerial,
        result->dwRegistSerial,
        result->wItemSerial);
    }
    return;
  }

  auto regInfo = Find(result->dwRegistSerial);
  if (regInfo == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInSoldItem()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo None\r\n",
        this->m_dwUserSerial,
        result->dwRegistSerial,
        result->wItemSerial);
    }
    return;
  }

  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(result->wItemSerial);
  if (!inventoryItem)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::CompleteRegistWaitingSellUpdateItemWhenLogInSoldItem()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Item None\r\n",
        this->m_dwUserSerial,
        result->dwRegistSerial,
        result->wItemSerial);
    }
    return;
  }

  regInfo->SellComplete(
    result->dwPrice,
    result->dwBuyer,
    result->dwTax,
    result->dbresulttime,
    result->wszName,
    result->szAccountID);
  CountRegistItem();

  const unsigned int realSellDalant = result->dwPrice - result->dwTax;
  owner->AddDalant(static_cast<int>(realSellDalant), true);
  CUnmannedTraderItemState::PushUpdateState(0, result->dwRegistSerial, 3u, this->m_dwUserSerial, result->wItemSerial, 0xFFu, 0xFFFFu);

  _unmannedtrader_Sell_Wait_item_inform_zocl inform{};
  inform.List[0].wItemSerial = result->wItemSerial;
  inform.List[0].dwSellDalant = realSellDalant;
  inform.List[0].dwTax = result->dwTax;
  inform.dwTotalSellDalant += inform.List[0].dwSellDalant;
  inform.dwTotalTaxDalant += inform.List[0].dwTax;
  inform.byNum = 1;
  inform.dwCurInvenDalant = owner->m_Param.GetDalant();

  unsigned __int8 type[16]{};
  type[0] = 30;
  type[1] = 26;
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_wInx,
    type,
    reinterpret_cast<char *>(&inform),
    GetSellWaitInformSize(inform));

  if (!owner->Emb_DelStorage(0, inventoryItem->m_byStorageIndex, false, true, "CUnmannedTraderUserInfo::SellComplete()"))
  {
    g_Main.m_logSystemError.Write(
      "CUnmannedTraderUserInfo::SellComplete() : User(%u) pkSellPlayer->Emb_DelStorage() Fail!",
      this->m_dwUserSerial);
  }

  const int level = owner->m_Param.GetLevel();
  if (level == 30 || level == 40 || level == 50 || level == 60)
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(owner->m_Param.GetRaceCode()),
      level,
      result->dwTax);
  }

  char resultDate[64]{};
  std::time_t resultTime = static_cast<std::time_t>(result->dbresulttime);
  std::tm *localTime = std::localtime(&resultTime);
  if (localTime)
  {
    sprintf_s(
      resultDate,
      sizeof(resultDate),
      "%04d-%02d-%02d %02d:%02d:%02d",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec);
  }
  else
  {
    sprintf_s(resultDate, sizeof(resultDate), "Invalid(%u)", static_cast<unsigned int>(result->dbresulttime));
  }

  CPlayer::s_MgrItemHistory.reg_ut_trade_waitingsellupdateitem_Sold(
    result->wszName,
    result->dwBuyer,
    result->szAccountID,
    result->dwRegistSerial,
    inventoryItem,
    resultDate,
    realSellDalant,
    result->dwTax,
    owner->m_Param.GetDalant(),
    owner->m_Param.GetGold(),
    owner->m_szItemHistoryFileName);

  _base_fld *record = g_Main.m_tblItemData[inventoryItem->m_byTableCode].GetRecord(inventoryItem->m_wItemIndex);
  g_Main.m_logAutoTrade.Write(
    "%s %s >> sell (%u) user(%u)%s -> user(%u)%s price(%u) tax(%u)",
    record->m_strCode,
    resultDate,
    result->dwRegistSerial,
    result->dwBuyer,
    result->wszName,
    owner->m_dwObjSerial,
    owner->m_Param.GetCharNameA(),
    result->dwPrice,
    result->dwTax);
}

unsigned __int8 CUnmannedTraderUserInfo::CheckSellComplete(
  CPlayer *pkSellPlayer,
  CPlayer *pkBuyer,
  unsigned int dwRegistSerial,
  unsigned int dwRealPrice,
  CLogFile *pkLogger)
{
  auto it = Find(dwRegistSerial);
  if (it == this->m_vecRegistItemInfo.end())
  {
    if (pkLogger)
    {
      const char *sellerName = pkSellPlayer ? pkSellPlayer->m_Param.GetCharNameA() : "NULL";
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::CheckSellComplete(...)\r\n"
        "\t\terror_used_invaled_item_local_sellinfo_lack!\r\n"
        "\t\tName(%s) Serial(%u) Invalid DB Data!\r\n"
        "\t\tFind( dwRegistSeria(%u) ) NULL!\r\n",
        sellerName,
        pkSellPlayer ? pkSellPlayer->m_id.dwSerial : 0,
        dwRegistSerial);
    }
    return 37;
  }

  if (!pkSellPlayer || !pkBuyer || !pkLogger)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderRegistItemInfo::CheckSellComplete(...)\r\n"
        "\t\t( NULL == pkSellPlayer(%p) || NULL == pkBuyer(%p) || NULL == pkLogger(%p) )\r\n"
        "\t\tdwRegistSeria(%u) error_used_internal_error!\r\n",
        pkSellPlayer,
        pkBuyer,
        pkLogger,
        dwRegistSerial);
    }
    it->ClearToWaitState();
    if (g_Player[this->m_wInx].m_bOper)
    {
      NotifyRegistItem();
    }
    return 99;
  }

  const unsigned __int16 itemSerial = it->GetItemSerial();
  if (pkSellPlayer->m_Param.m_dbInven.GetPtrFromSerial(itemSerial))
  {
    const unsigned int dalant = pkSellPlayer->m_Param.GetDalant();
    if (CanAddMoneyForMaxLimMoney(dwRealPrice, dalant))
    {
      return 0;
    }
    return 34;
  }

  const char *sellerName = pkSellPlayer->m_Param.GetCharNameA();
  pkLogger->Write(
    "CUnmannedTraderRegistItemInfo::CheckSellComplete(...)\r\n"
    "\t\terror_used_invaled_item_local_sellinfo_lack!\r\n"
    "\t\tName(%s) Serial(%u) Invalid DB Data!\r\n"
    "\t\tdwRegistSerial(%u)\r\n"
    "\t\tpkPlayer->m_Param.m_dbInven.GetPtrFromSerial( m_wItemSerial(%u) ) == NULL!\r\n",
    sellerName,
    pkSellPlayer->m_id.dwSerial,
    dwRegistSerial,
    itemSerial);
  it->ClearToWaitState();
  if (g_Player[this->m_wInx].m_bOper)
  {
    NotifyRegistItem();
  }
  return 37;
}

unsigned __int8 CUnmannedTraderUserInfo::SellComplete(
  CPlayer *pkSellPlayer,
  CPlayer *pkBuyer,
  unsigned int dwOriPrice,
  unsigned int dwRealPrice,
  unsigned int dwTax,
  unsigned int dwRegistSerial,
  __int64 tResultTime,
  CLogFile *pkLogger)
{
auto it = Find(dwRegistSerial);
  const unsigned __int16 itemSerial = it->GetItemSerial();
  _STORAGE_LIST::_db_con *pItem = pkSellPlayer->m_Param.m_dbInven.GetPtrFromSerial(itemSerial);
  if (!pItem)
  {
    return 37;
  }

  if (!pkSellPlayer->Emb_DelStorage(
        0,
        pItem->m_byStorageIndex,
        false,
        true,
        "CUnmannedTraderUserInfo::SellComplete()"))
  {
    g_Main.m_logSystemError.Write(
      "CUnmannedTraderUserInfo::SellComplete() : User(%u) pkSellPlayer->Emb_DelStorage() Fail!",
      this->m_dwUserSerial);
    return static_cast<unsigned __int8>(-105);
  }

  pkSellPlayer->AddDalant(static_cast<int>(dwRealPrice), true);
  const char *buyerAccount = pkBuyer->m_pUserDB->m_szAccountID;
  const char *buyerNameW = pkBuyer->m_Param.GetCharNameW();
  it->SellComplete(dwOriPrice, pkBuyer->m_dwObjSerial, dwTax, tResultTime, buyerNameW, buyerAccount);
  CountRegistItem();

  const int level = pkSellPlayer->m_Param.GetLevel();
  if (level == 30 || level == 40 || level == 50 || level == 60)
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(pkSellPlayer->m_Param.GetRaceCode()),
      level,
      dwTax);
  }

  const unsigned int leftGold = pkSellPlayer->m_Param.GetGold();
  const unsigned int leftDalant = pkSellPlayer->m_Param.GetDalant();
  const char *buyerID = pkBuyer->m_pUserDB->m_szAccountID;
  const char *buyerNameA = pkBuyer->m_Param.GetCharNameA();
  CPlayer::s_MgrItemHistory.auto_trade_sell(
    buyerNameA,
    pkBuyer->m_dwObjSerial,
    buyerID,
    dwRegistSerial,
    pItem,
    dwRealPrice,
    dwTax,
    leftDalant,
    leftGold,
    pkSellPlayer->m_szItemHistoryFileName);

  tm *timeInfo = std::localtime(&tResultTime);
  char buffer[64]{};
  if (timeInfo)
  {
    sprintf_s(
      buffer,
      64,
      "%04d-%02d-%02d %02d:%02d:%02d",
      timeInfo->tm_year + 1900,
      timeInfo->tm_mon + 1,
      timeInfo->tm_mday,
      timeInfo->tm_hour,
      timeInfo->tm_min,
      timeInfo->tm_sec);
  }
  else
  {
    sprintf_s(buffer, 64, "Invalid(%u)", static_cast<unsigned int>(tResultTime));
  }

  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  const char *sellerNameA = pkSellPlayer->m_Param.GetCharNameA();
  const char *buyerNameLog = pkBuyer->m_Param.GetCharNameA();
  g_Main.m_logAutoTrade.Write(
    "%s %s >> sell (%u) user(%u)%s -> user(%u)%s price(%u) tax(%u)",
    record->m_strCode,
    buffer,
    dwRegistSerial,
    pkBuyer->m_dwObjSerial,
    buyerNameLog,
    pkSellPlayer->m_dwObjSerial,
    sellerNameA,
    dwOriPrice,
    dwTax);

  const unsigned int finalDalant = pkSellPlayer->m_Param.GetDalant();
  SendSellInfom(pkSellPlayer->m_ObjID.m_wIndex, pItem->m_wSerial, dwRealPrice, dwTax, finalDalant);
  return 90;
}

unsigned __int8 CUnmannedTraderUserInfo::CheckBuyComplete(CPlayer *pkBuyer, unsigned int dwPrice)
{
  if (dwPrice > pkBuyer->m_Param.GetDalant())
  {
    return 43;
  }
  if (pkBuyer->m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    return 42;
  }
  return 0;
}

unsigned __int8 CUnmannedTraderUserInfo::BuyComplete(
  CPlayer *pkBuyer,
  unsigned int dwSellerSerial,
  const char *wszSellerName,
  const char *szSellerAccountName,
  unsigned int dwRegistSerial,
  unsigned int dwK,
  unsigned __int64 dwD,
  unsigned int dwU,
  unsigned int dwPrice,
  unsigned __int64 lnUID,
  CLogFile *pkLogger,
  unsigned __int16 *wAddItemSerial)
{
_INVENKEY key;
  key.LoadDBKey(dwK);
  _STORAGE_LIST::_db_con item;
  std::memset(&item, 0, sizeof(item));
  item.m_byTableCode = key.byTableCode;
  item.m_wItemIndex = key.wItemIndex;
  item.m_dwDur = dwD;
  item.m_dwLv = dwU;
  item.m_wSerial = pkBuyer->m_Param.GetNewItemSerial();
  *wAddItemSerial = item.m_wSerial;
  item.m_lnUID = lnUID;

  if (pkBuyer->Emb_AddStorage(0, &item, false, true))
  {
    pkBuyer->SubDalant(dwPrice);
    char szTran[17]{};
    W2M(const_cast<char *>(wszSellerName), szTran, 17);
    const unsigned int leftGold = pkBuyer->m_Param.GetGold();
    const unsigned int leftDalant = pkBuyer->m_Param.GetDalant();
    CPlayer::s_MgrItemHistory.auto_trade_buy(
      szTran,
      dwSellerSerial,
      szSellerAccountName,
      dwRegistSerial,
      &item,
      dwPrice,
      leftDalant,
      leftGold,
      pkBuyer->m_szItemHistoryFileName);
    return 0;
  }

  g_Main.m_logSystemError.Write(
    "CUnmannedTraderUserInfo::BuyComplete() : User(%u) pkSellPlayer->Emb_AddStorage() Fail!",
    this->m_dwUserSerial);
  return static_cast<unsigned __int8>(-106);
}

void CUnmannedTraderUserInfo::NotifyRegistItem()
{
  unsigned __int8 byDivision[32]{};
  unsigned __int8 byClass[32]{};
  unsigned __int8 bySubClass[44]{};
  byDivision[0] = static_cast<unsigned __int8>(-1);
  byClass[0] = static_cast<unsigned __int8>(-1);
  bySubClass[0] = static_cast<unsigned __int8>(-1);

  _unmannedtrader_Regist_item_inform_zocl info{};
  for (int j = 0; j < this->m_byMaxRegistCnt; ++j)
  {
    CUnmannedTraderRegistItemInfo &item = this->m_vecRegistItemInfo[j];
    if (item.IsRegist())
    {
      info.List[info.byNum].bWaitingRegister = 0;
      info.List[info.byNum].wItemSerial = item.GetItemSerial();
      info.List[info.byNum].dwRegistSerial = item.GetRegistSerial();
      info.List[info.byNum].dwPrice = item.GetPrice();
      info.List[info.byNum].dwLeftSec = static_cast<unsigned int>(item.GetLeftSec());
      info.List[info.byNum].dwListIndex = 0;

      unsigned int *listIndex = &info.List[info.byNum].dwListIndex;
      const unsigned __int16 itemIndex = item.GetItemIndex();
      const unsigned __int8 tableCode = item.GetTableCode();
      CUnmannedTraderGroupItemInfoTable::Instance()->GetGroupID(
        tableCode,
        itemIndex,
        byDivision,
        byClass,
        bySubClass,
        listIndex);
      ++info.byNum;
    }
  }

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 25;
  const unsigned __int16 len = static_cast<unsigned __int16>(info.size());
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_wInx, pbyType, reinterpret_cast<char *>(&info), len);
}

void CUnmannedTraderUserInfo::SendRegistItemSuccessResult(
  unsigned int dwLeftDalant,
  unsigned __int16 wInx,
  const _qry_case_unmandtrader_registsingleitem *pLoadData)
{
  _unmannedtrader_regist_item_success_result_zocl result{};
  result.wItemSerial = pLoadData->wItemSerialOld;
  result.dwPrice = pLoadData->dwPrice;
  result.dwRegedSerial = pLoadData->dwRegedSerial;
  result.dwListIndex = pLoadData->dwListIndex;
  result.dwTax = pLoadData->dwTax;
  result.dwLeftDalant = dwLeftDalant;
  result.wSepaSerial = pLoadData->wSepaSerial;
  result.bySepaAmount = pLoadData->bySepaAmount;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 28;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendRegistItemErrorResult(
  unsigned __int16 wInx,
  char byRet,
  unsigned __int16 wItemSerial,
  unsigned int dwRetParam1)
{
  _unmannedtrader_regist_item_error_result_zocl msg{};
  msg.byRet = byRet;
  msg.wItemSerial = wItemSerial;
  msg.dwRetParam1 = static_cast<unsigned int>(-1);

  if (byRet == 6 || byRet == 24 || byRet == static_cast<char>(-56))
  {
    msg.dwRetParam1 = dwRetParam1;
  }

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 29;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CUnmannedTraderUserInfo::SendCancelRegistSuccessResult(
  unsigned __int16 wInx,
  unsigned __int16 wItemSerial,
  unsigned int dwRegistSerial)
{
  _a_trade_clear_item_result_zocl result{};
  result.byRetCode = 0;
  result.wItemSerial = wItemSerial;
  result.dwRegistSerial = dwRegistSerial;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 12;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendCancelRegistErrorResult(unsigned __int16 wInx, unsigned __int8 byRet)
{
  _a_trade_clear_item_result_zocl result{};
  result.byRetCode = byRet;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 12;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendRepriceSuccessResult(
  CPlayer *pReceiver,
  unsigned __int16 wItemSerial,
  unsigned int dwNewPrice,
  unsigned int dwRegistSerial,
  unsigned int dwTax)
{
  _a_trade_adjust_price_result_zocl result{};
  result.byRetCode = 0;
  result.wItemSerial = wItemSerial;
  result.dwNewPrice = dwNewPrice;
  result.dwRegistSerial = dwRegistSerial;
  result.dwLeftDalant = pReceiver->m_Param.GetDalant();
  result.byTaxRate = 1;
  result.dwTax = dwTax;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 10;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(pReceiver->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendRepriceErrorResult(CPlayer *pReceiver, unsigned __int8 byRet)
{
  _a_trade_adjust_price_result_zocl result{};
  std::memset(&result.byTaxRate, 0, 19);
  result.byRetCode = byRet;
  result.byTaxRate = 1;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 10;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(pReceiver->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendBuyErrorResult(unsigned __int16 wInx, unsigned __int8 byRet)
{
  _unmannedtrader_buy_item_result_zocl result{};
  result.byRetCode = byRet;

  unsigned __int8 pbyType[2]{30, 31};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&result), len);
}

void CUnmannedTraderUserInfo::SendSellInfom(
  unsigned __int16 wInx,
  unsigned __int16 wItemSerial,
  unsigned int dwAddDalant,
  unsigned int dwTaxDalant,
  unsigned int dwTotalDalant)
{
  _a_trade_sell_inform_zocl msg{};
  msg.wItemSerial = wItemSerial;
  msg.dwAddDalant = dwAddDalant;
  msg.dwTaxDalant = dwTaxDalant;
  msg.dwTotalDalant = dwTotalDalant;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 9;
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CUnmannedTraderUserInfo::SendWaitingRegisterItemNotifyRegisterSuccess(
  unsigned __int16 wInx,
  unsigned int dwRegistSerial,
  unsigned __int16 wItemSerial)
{
  _unmannedtrader_waiting_register_item_notify_register_success_zocl inform{};
  inform.dwRegedSerial = dwRegistSerial;
  inform.wItemSerial = wItemSerial;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 40;
  const unsigned __int16 len = static_cast<unsigned __int16>(inform.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&inform), len);
}

void CUnmannedTraderUserInfo::SendNotifyCloseItem(
  unsigned __int16 wInx,
  unsigned __int16 wItemSerial,
  unsigned int dwRegistSerial,
  unsigned int dwPrice,
  unsigned __int8 byTaxRate)
{
_unmannedtrader_close_item_inform_zocl inform{};
  inform.byTaxRate = 1;
  inform.dwPrice = dwPrice;
  inform.wItemSerial = wItemSerial;
  inform.dwRegistSerial = dwRegistSerial;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 30;
  pbyType[1] = 27;
  const unsigned __int16 len = static_cast<unsigned __int16>(inform.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wInx, pbyType, reinterpret_cast<char *>(&inform), len);
}

void CUnmannedTraderUserInfo::LockedItemSetUnlock(
  const _qry_case_unmandtrader_log_in_proc_update_complete::__list *pLoadData,
  CLogFile *pkLogger)
{
  CPlayer *owner = FindOwner();
  if (!owner || !owner->m_bOper)
  {
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::LockedItemSetUnlock()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Owner Log out!\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
    return;
  }

  bool bSendNotify = true;
  _STORAGE_LIST::_db_con *inventoryItem = owner->m_Param.m_dbInven.GetPtrFromSerial(pLoadData->wItemSerial);
  if (!inventoryItem)
  {
    bSendNotify = false;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::LockedItemSetUnlock()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) Item None\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }

  auto regInfo = Find(pLoadData->dwRegistSerial);
  if (regInfo == this->m_vecRegistItemInfo.end())
  {
    bSendNotify = false;
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::LockedItemSetUnlock()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo None\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }
  else if (regInfo->GetItemSerial() != pLoadData->wItemSerial)
  {
    bSendNotify = false;
    if (inventoryItem)
    {
      inventoryItem->lock(false);
    }
    regInfo->Clear();
    regInfo->SetState(7u);
    if (pkLogger)
    {
      pkLogger->Write(
        "CUnmannedTraderUserInfo::LockedItemSetUnlock()\r\n"
        "\t\tOwner(%u) UTRegisterSerial(%u) wItemSerial(%u) UTRegistInfo ItemSerial different!\r\n",
        this->m_dwUserSerial,
        pLoadData->dwRegistSerial,
        pLoadData->wItemSerial);
    }
  }

  if (bSendNotify)
  {
    SendWaitingRegisterItemNotifyRegisterSuccess(
      owner->m_ObjID.m_wIndex,
      pLoadData->dwRegistSerial,
      pLoadData->wItemSerial);
  }
}

