
#include "pch.h"

#include "CashItemRemoteStore.h"

#include "CashShop_fld.h"
#include "CashShop_str_fld.h"
#include "CouponItem_fld.h"
#include "CAsyncLogger.h"
#include "CCashDBWorkManager.h"
#include "CMainThread.h"
#include "CNationSettingManager.h"
#include "CTSingleton.h"
#include "GlobalObjects.h"
#include "ICsSendInterface.h"
#include "TimeItem.h"
#include "UIDGenerator.h"
#include "WorldServerUtil.h"
#include "param_cash.h"
#include "qry_case_cash_limsale.h"
#include "result_csi_buy_zocl.h"
#include "result_csi_goods_list_zocl.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <new>

namespace
{
  const char kEventTypeDisable[] = " 0 - Event_type_disable : ";
  const char kEventTypeWait[] = " 1 - Event_type_wait : ";
  const char kEventTypeStart[] = " 2 - Event_type_start: ";
  const char kEventType30Before[] = " 3 - Event_type_30_before: ";
  const char kEventType5Before[] = " 4 - Event_type_5_before: ";
  const char kEventTypeEnd[] = " 5 - Event_type_end : ";
  const char kEventTypeExpired[] = " 7 - Event_type_expire : ";
  const char kLimitedSaleDefault[] = "";
}

CashItemRemoteStore *CashItemRemoteStore::_pkInstance;
CashItemRemoteStore::CashMap *CashItemRemoteStore::_phmapTbl;

_cash_discount_ini_::_cash_discount_ini_()
{
  std::memset(this, 0, sizeof(_cash_discount_ini_));
}

_cash_discount_::_cash_discount_()
{
  std::memset(this, 0, sizeof(_cash_discount_));
}

_cash_event_ini::_cash_event_ini()
{
  std::memset(this, 0, sizeof(_cash_event_ini));
}

_cash_event::_cash_event()
{
  std::memset(this, 0, sizeof(_cash_event));
}

_con_event_::_con_event_()
{
  std::memset(this, 0, sizeof(_con_event_));
}

CashItemRemoteStore::CashItemRemoteStore()
{
  _bIsBuyCashItemByGold = false;
}

CashItemRemoteStore::~CashItemRemoteStore() = default;

CashItemRemoteStore::_remain_num_of_good::_remain_num_of_good()
{
  std::memset(this, 0, sizeof(CashItemRemoteStore::_remain_num_of_good));
}

CashItemRemoteStore *CashItemRemoteStore::Instance()
{
  if (!_pkInstance)
  {
    CashItemRemoteStore *instance = new (std::nothrow) CashItemRemoteStore();
    _pkInstance = instance;
  }
  return _pkInstance;
}

bool CashItemRemoteStore::Initialize()
{
  if (!_InitLoggers())
  {
    return false;
  }
  if (!_ReadGoods())
  {
    return false;
  }
  m_TotalEventTimer.BeginTimer(1000);
  Load_Conditional_Event();
  load_cash_discount_event();
  Load_Cash_Event();
  return LoadBuyCashMode();
}

void CashItemRemoteStore::Loop_TatalCashEvent()
{
  if (m_TotalEventTimer.CountingTimer())
  {
    load_cash_discount_event();
    check_loaded_cde_status();
    Load_Cash_Event();
    Load_Conditional_Event();
  }
}

void CashItemRemoteStore::loop_cash_discount_event()
{
  check_cash_discount_ini();
  check_cash_discount_status();
}

void CashItemRemoteStore::Loop_Cash_Event()
{
  for (int j = 0; j < 3; ++j)
  {
    Check_CashEvent_INI(static_cast<unsigned __int8>(j));
    if (g_Main.m_pWorldDB && g_Main.m_pWorldDB->IsConectionActive())
    {
      Check_CashEvent_Status(static_cast<unsigned __int8>(j));
    }
  }
}

void CashItemRemoteStore::Loop_ContEvent()
{
  Check_Conditional_Event_INI();
  Check_Conditional_Event_Status();
}

bool CashItemRemoteStore::GoodsList(unsigned __int16 wSock, char *pPacket)
{
  if (_bIsBuyCashItemByGold)
  {
    return GoodsListBuyByGold(wSock, pPacket) != 0;
  }
  return GoodsListBuyByCash(wSock, pPacket) != 0;
}

bool CashItemRemoteStore::Buy(unsigned __int16 wSock, char *pPacket)
{
  if (_bIsBuyCashItemByGold)
  {
    return BuyByGold(wSock, reinterpret_cast<_request_csi_buy_clzo *>(pPacket)) != 0;
  }
  return BuyByCash(wSock, pPacket) != 0;
}

bool CashItemRemoteStore::Sell(unsigned __int16 wSock, char *pPacket)
{
  (void)wSock;
  (void)pPacket;
  // this is not a stub
  return true;
}

bool CashItemRemoteStore::start_cde(int iBegin_TT, int iB30_TT, int iB5_TT, int iEnd_TT)
{
  if (m_cde.m_ini.m_bUseCashDiscount && is_cde_time())
  {
    m_cde.m_cde_log.Write("Cheat-Command was disregarded. Other Event is on");
    return false;
  }

  __time32_t now[8]{};
  _time32(now);
  __time32_t beginTime[8]{};
  __time32_t endTime[9]{};
  beginTime[0] = iBegin_TT + now[0];
  endTime[0] = iEnd_TT + iB5_TT + iB30_TT + iBegin_TT + now[0];

  m_cde.m_ini.m_bUseCashDiscount = 1;
  m_cde.m_ini.m_cdeTime[0] = beginTime[0];
  m_cde.m_ini.m_cdeTime[1] = endTime[0];
  m_cde.m_cde_inform_before[0] = iB5_TT + iB30_TT;
  m_cde.m_cde_inform_before[1] = iB30_TT;

  std::tm beginTm{};
  std::tm endTm{};
  _localtime32_s(&beginTm, beginTime);
  _localtime32_s(&endTm, endTime);
  m_cde.m_ini.m_wYear[0] = beginTm.tm_year + 1900;
  m_cde.m_ini.m_byMonth[0] = beginTm.tm_mon + 1;
  m_cde.m_ini.m_byDay[0] = beginTm.tm_mday;
  m_cde.m_ini.m_byHour[0] = beginTm.tm_hour;
  m_cde.m_ini.m_byMinute[0] = beginTm.tm_min;
  m_cde.m_ini.m_wYear[1] = endTm.tm_year + 1900;
  m_cde.m_ini.m_byMonth[1] = endTm.tm_mon + 1;
  m_cde.m_ini.m_byDay[1] = endTm.tm_mday;
  m_cde.m_ini.m_byHour[1] = endTm.tm_hour;
  m_cde.m_ini.m_byMinute[1] = endTm.tm_min;
  set_cde_status(1);
  log_about_cash_event("Loaded From Cheat-Args <When Server Running>", &m_cde.m_ini);
  return true;
}

bool CashItemRemoteStore::start_cashevent(
  int iBegin_TT,
  int iB30_TT,
  int iB5_TT,
  int iEnd_TT,
  unsigned __int8 byEventType)
{
  __time32_t now[8]{};
  _time32(now);
  __time32_t beginTime[8]{};
  __time32_t endTime[4]{};
  beginTime[0] = iBegin_TT + now[0];
  endTime[0] = iEnd_TT + iB5_TT + iB30_TT + iBegin_TT + now[0];

  for (int j = 0; j < 3; ++j)
  {
    m_cash_event[j].m_ini.m_bUseCashEvent = (j == byEventType);
  }
  m_cash_event[byEventType].m_ini.m_EventTime[0] = beginTime[0];
  m_cash_event[byEventType].m_ini.m_EventTime[1] = endTime[0];
  m_cash_event[byEventType].m_event_inform_before[0] = iB5_TT + iB30_TT;
  m_cash_event[byEventType].m_event_inform_before[1] = iB30_TT;

  std::tm beginTm{};
  std::tm endTm{};
  _localtime32_s(&beginTm, beginTime);
  _localtime32_s(&endTm, endTime);
  m_cash_event[byEventType].m_ini.m_wYear[0] = beginTm.tm_year + 1900;
  m_cash_event[byEventType].m_ini.m_byMonth[0] = beginTm.tm_mon + 1;
  m_cash_event[byEventType].m_ini.m_byDay[0] = beginTm.tm_mday;
  m_cash_event[byEventType].m_ini.m_byHour[0] = beginTm.tm_hour;
  m_cash_event[byEventType].m_ini.m_byMinute[0] = beginTm.tm_min;
  m_cash_event[byEventType].m_ini.m_wYear[1] = endTm.tm_year + 1900;
  m_cash_event[byEventType].m_ini.m_byMonth[1] = endTm.tm_mon + 1;
  m_cash_event[byEventType].m_ini.m_byDay[1] = endTm.tm_mday;
  m_cash_event[byEventType].m_ini.m_byHour[1] = endTm.tm_hour;
  m_cash_event[byEventType].m_ini.m_byMinute[1] = endTm.tm_min;

  for (int k = 0; k < 3; ++k)
  {
    if (k != byEventType)
    {
      Inform_CashEvent_Status_All(static_cast<unsigned __int8>(k), 5, &m_cash_event[k].m_ini);
      Set_CashEvent_Status(static_cast<unsigned __int8>(k), 1);
    }
  }

  const unsigned __int8 cashEventStatus = Get_CashEvent_Status(byEventType);
  if (cashEventStatus >= 2 && cashEventStatus <= 4)
  {
    Inform_CashEvent_Status_All(byEventType, 5, &m_cash_event[byEventType].m_ini);
  }
  Set_CashEvent_Status(byEventType, 1);
  return true;
}

bool CashItemRemoteStore::start_conevent(int iBegin_TT, int iEnd_TT, unsigned __int8 byEventType)
{
  __time32_t now[8]{};
  _time32(now);
  __time32_t beginTime[8]{};
  __time32_t endTime[9]{};
  beginTime[0] = iBegin_TT + now[0];
  endTime[0] = iEnd_TT + iBegin_TT + now[0];

  m_con_event.m_ini.m_bUseConEvent = byEventType < 3;
  m_con_event.m_bConEvent = true;
  m_con_event.m_eventtime.m_EventTime[0] = beginTime[0];
  m_con_event.m_eventtime.m_EventTime[1] = endTime[0];

  std::tm beginTm{};
  std::tm endTm{};
  _localtime32_s(&beginTm, beginTime);
  _localtime32_s(&endTm, endTime);
  const unsigned __int8 conditionalStatus = Get_Conditional_Event_Status();
  if (conditionalStatus == 2 || conditionalStatus == 3)
  {
    Inform_ConditionalEvent_Status_All(
      m_con_event.m_ini.m_byEventKind,
      4,
      m_con_event.m_ini.m_szEndMsg);
    Set_Conditional_Evnet_Status(1);
  }

  m_con_event.m_bConEvent = true;
  m_con_event.m_ini.m_byEventKind = byEventType;
  Set_Conditional_Evnet_Status(1);
  return true;
}

bool CashItemRemoteStore::CheatBuy(unsigned __int16 wSock, char *szItemCode, char nNum)
{
  int tableCode = GetItemTableCode(szItemCode);
  if (tableCode == -1)
  {
    return false;
  }

  _base_fld *itemRecord = g_Main.m_tblItemData[tableCode].GetRecord(szItemCode, 8);
  if (!itemRecord)
  {
    return false;
  }

  const _CashShop_fld *cashRecord = FindCashRec(static_cast<unsigned int>(tableCode), itemRecord->m_dwIndex);
  if (!cashRecord)
  {
    return false;
  }

  _request_csi_buy_clzo packet{};
  std::memset(&packet, 0, sizeof(packet));
  packet.nNum = 1;
  packet.item[0].nPrice = cashRecord->m_nCsPrice;
  packet.item[0].byDiscount = static_cast<unsigned __int8>(cashRecord->m_nCsDiscount);
  packet.item[0].byTblCode = static_cast<unsigned __int8>(tableCode);
  packet.item[0].wItemIdx = static_cast<unsigned __int16>(itemRecord->m_dwIndex);
  packet.item[0].wStoreIdx = static_cast<unsigned __int16>(cashRecord->m_dwIndex);
  packet.item[0].byOverlapNum = static_cast<unsigned __int8>(nNum);
  return Buy(wSock, reinterpret_cast<char *>(&packet));
}

bool CashItemRemoteStore::CheatLoadCashAmount(unsigned __int16 wSock, int nNum)
{
  if (_kRecGoods.GetRecordNum() <= nNum)
  {
    return false;
  }

  CPlayer *player = &g_Player[wSock];
  if (!player->m_bOper)
  {
    return false;
  }

  _param_cash_select query(player->m_pUserDB->m_dwAccountSerial, player->m_Param.GetCharSerial(), wSock);
  strcpy_s(query.in_szAcc, sizeof(query.in_szAcc), player->m_pUserDB->m_szAccountID);
  const unsigned __int64 querySize = static_cast<unsigned __int64>(query.size());
  CCashDBWorkManager *cashWorkManager = CTSingleton<CCashDBWorkManager>::Instance();
  cashWorkManager->PushTask(0, reinterpret_cast<unsigned __int8 *>(&query), querySize);
  return true;
}

bool CashItemRemoteStore::GoodsListBuyByCash(unsigned __int16 wSock, char *pPacket)
{
  (void)pPacket;

  if (wSock >= MAX_PLAYER)
  {
    return false;
  }

  CPlayer *player = &g_Player[wSock];
  if (!player->m_bLive)
  {
    return true;
  }
  if (!player->m_bOper)
  {
    return true;
  }

  _param_cash_select query(player->m_pUserDB->m_dwAccountSerial, player->m_Param.GetCharSerial(), wSock);
  sprintf_s(query.in_szAcc, sizeof(query.in_szAcc), "%s", player->m_pUserDB->m_szAccountID);

  query.in_bAdjustDiscount = is_cde_time() != 0;
  query.in_bOneN_One = IsEventTime(1) != 0;
  query.in_bSetDiscount = IsEventTime(0) != 0;
  for (int index = 0; index < 4; ++index)
  {
    if (query.in_bSetDiscount)
    {
      query.in_bySetDiscount[index] = GetSetDiscout(static_cast<unsigned __int8>(index));
    }
    else
    {
      query.in_bySetDiscount[index] = 0;
    }
  }

  query.in_bLimited_Sale = IsEventTime(2) != 0;
  if (query.in_bLimited_Sale)
  {
    query.in_byLimDiscount = GetLimDiscout();
  }
  else
  {
    query.in_byLimDiscount = 0;
  }

  const unsigned __int64 querySize = static_cast<unsigned __int64>(query.size());
  CCashDBWorkManager *cashWorkManager = CTSingleton<CCashDBWorkManager>::Instance();
  cashWorkManager->PushTask(0, reinterpret_cast<unsigned __int8 *>(&query), querySize);
  return true;
}

bool CashItemRemoteStore::BuyByCash(unsigned __int16 wSock, char *pPacket)
{
  CPlayer *player = &g_Player[wSock];
  if (!player->m_bOper || !player->m_bLive)
  {
    return true;
  }

  if (player->m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    ICsSendInterface::SendMsg_Error(wSock, 16);
    return true;
  }

  _request_csi_buy_clzo *request = reinterpret_cast<_request_csi_buy_clzo *>(pPacket);
  if (request->nNum > player->m_Param.m_dbInven.GetNumEmptyCon())
  {
    ICsSendInterface::SendMsg_Error(wSock, 16);
    return true;
  }

  if (request->nNum > 20)
  {
    ICsSendInterface::SendMsg_Error(wSock, 4);
    _kLoggers[0].Write("CashItemRemoteStore::Buy() Buy Item Number Error, Num(%d)", request->nNum);
    return true;
  }

  if (request->byCouponNum >= 3)
  {
    ICsSendInterface::SendMsg_Error(wSock, 19);
    return true;
  }

  for (int j = 0; j < request->byCouponNum; ++j)
  {
    if (request->CouponItem[j].byStorageCode >= 8)
    {
      ICsSendInterface::SendMsg_Error(wSock, 19);
      return true;
    }
  }

  int onePlusOneCount = 0;
  for (int k = 0; k < request->nNum; ++k)
  {
    if (request->item[k].byEventType == 3)
    {
      ++onePlusOneCount;
    }
  }

  const int totalRequiredSlots = onePlusOneCount + request->nNum;
  if (totalRequiredSlots > player->m_Param.m_dbInven.GetNumEmptyCon())
  {
    ICsSendInterface::SendMsg_Error(wSock, 16);
    return true;
  }

  _param_cash_update buyList(player->m_pUserDB->m_dwAccountSerial, player->m_Param.GetCharSerial(), wSock);
  const bool adjustDiscount = is_cde_time() != 0;
  const bool oneNOneEvent = IsEventTime(1) != 0;
  const bool setDiscountEvent = IsEventTime(0) != 0;
  const bool limitedSaleEvent = IsEventTime(2) != 0;
  buyList.in_bAdjustDiscount = adjustDiscount;
  buyList.in_bOneN_One = oneNOneEvent;
  buyList.in_bSetDiscount = setDiscountEvent;
  buyList.in_bLimited_Sale = limitedSaleEvent;

  for (int m = 0; m < request->nNum; ++m)
  {
    _request_csi_buy_clzo::__item *srcItem = &request->item[m];
    _param_cash_update::__item *dstItem = &buyList.in_item[static_cast<unsigned __int64>(m)];

    _CashShop_fld *goodsRecord = reinterpret_cast<_CashShop_fld *>(_kRecGoods.GetRecord(srcItem->wStoreIdx));
    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(srcItem->byTblCode, srcItem->wItemIdx);
    if (!timeRec && srcItem->byTblCode >= 37)
    {
      _kLoggers[0].Write(
        "CashItemRemoteStore::Buy() Can not find _TimeItem_fld Data, TableCode(%d), ItemIndex(%d), StoreIndex(%d)",
        srcItem->byTblCode,
        srcItem->wItemIdx,
        srcItem->wStoreIdx);
    }

    if (srcItem->byTblCode >= 37)
    {
      ICsSendInterface::SendMsg_Error(wSock, 11);
      return true;
    }

    const unsigned __int8 raceSexCode = static_cast<unsigned __int8>(player->m_Param.GetRaceSexCode());
    dstItem->byRet = static_cast<unsigned __int8>(_check_buyitem(raceSexCode, srcItem, goodsRecord));
    if (dstItem->byRet)
    {
      ICsSendInterface::SendMsg_Error(wSock, dstItem->byRet);
      return true;
    }

    if (srcItem->byEventType == 1 && !adjustDiscount)
    {
      ICsSendInterface::SendMsg_Error(wSock, 18);
      return true;
    }
    if (srcItem->byEventType == 2 && !setDiscountEvent)
    {
      ICsSendInterface::SendMsg_Error(wSock, 18);
      return true;
    }
    if (srcItem->byEventType == 3 && !oneNOneEvent)
    {
      ICsSendInterface::SendMsg_Error(wSock, 18);
      return true;
    }
    if (srcItem->byEventType == 5 && !limitedSaleEvent)
    {
      ICsSendInterface::SendMsg_Error(wSock, 22);
      return true;
    }

    const unsigned __int8 emptySlots = static_cast<unsigned __int8>(player->m_Param.m_dbInven.GetNumEmptyCon());
    if (srcItem->byEventType == 2)
    {
      if (emptySlots < 3)
      {
        ICsSendInterface::SendMsg_Error(wSock, 16);
        return true;
      }
    }
    else if (srcItem->byEventType == 3 && emptySlots < 2)
    {
      ICsSendInterface::SendMsg_Error(wSock, 16);
      return true;
    }

    dstItem->in_byTblCode = srcItem->byTblCode;
    dstItem->in_wItemIdx = srcItem->wItemIdx;
    dstItem->in_byOverlapNum = srcItem->byOverlapNum;
    dstItem->in_nPrice = srcItem->nPrice;
    dstItem->in_nEventType = srcItem->byEventType;
    if (adjustDiscount && srcItem->byEventType == 1)
    {
      if (srcItem->byDiscount == static_cast<unsigned __int8>(-1))
      {
        dstItem->in_nDiscount = m_cde.m_ini.m_wCsDiscount;
      }
      else
      {
        dstItem->in_nDiscount = srcItem->byDiscount;
      }
    }
    else
    {
      dstItem->in_nDiscount = 0;
    }

    dstItem->in_lnUID = UIDGenerator::getuid(g_Main.m_byWorldCode);
    if (timeRec)
    {
      dstItem->in_nLendType = static_cast<unsigned __int8>(timeRec->m_nCheckType);
      dstItem->in_dwLendTime = timeRec->m_nUseTime;
    }

    strcpy_s(dstItem->in_strItemCode, sizeof(dstItem->in_strItemCode), goodsRecord->m_strCsItemCode);
    if (setDiscountEvent && request->bySetKind && request->bySetKind <= 4)
    {
      dstItem->in_nDiscount = GetSetDiscout(request->bySetKind - 1);
    }
    if (limitedSaleEvent && srcItem->byEventType == 5)
    {
      dstItem->in_nDiscount = GetLimDiscout();
    }
    ++buyList.in_nNum10;
  }

  if (request->byCouponNum)
  {
    if (adjustDiscount || oneNOneEvent)
    {
      ICsSendInterface::SendMsg_Error(wSock, 20);
      return true;
    }

    if (CheckCouponType(request->CouponItem, player, request->byCouponNum) <= 0)
    {
      ICsSendInterface::SendMsg_Error(wSock, 19);
      return true;
    }

    for (int index = 0; index < request->byCouponNum; ++index)
    {
      if (!UseDiscountCoupon(&buyList, request->CouponItem[index], player))
      {
        ICsSendInterface::SendMsg_Error(wSock, 19);
        return true;
      }
      buyList.in_CouponItem[index] = request->CouponItem[index];
    }
  }

  buyList.in_nCouponCnt = request->byCouponNum;
  strcpy_s(buyList.in_szAcc, sizeof(buyList.in_szAcc), player->m_pUserDB->m_szAccountID);
  strcpy_s(buyList.in_szSvrName, sizeof(buyList.in_szSvrName), g_Main.m_szWorldName);
  strcpy_s(buyList.in_szAvatorName, sizeof(buyList.in_szAvatorName), player->m_Param.GetCharNameW());
  buyList.in_nCashAmount = player->GetCashAmount();
  buyList.in_dwIP = player->m_pUserDB->m_dwIP;

  const unsigned __int64 querySize = static_cast<unsigned __int64>(buyList.size());
  CCashDBWorkManager *cashWorkManager = CTSingleton<CCashDBWorkManager>::Instance();
  cashWorkManager->PushTask(1, reinterpret_cast<unsigned __int8 *>(&buyList), querySize);
  return true;
}

bool CashItemRemoteStore::BuyByGold(unsigned __int16 wSock, _request_csi_buy_clzo *pPacket)
{
  CPlayer *player = &g_Player[wSock];
  if (!player->m_bOper || !player->m_bLive)
  {
    return true;
  }

  _request_csi_buy_clzo *request = pPacket;
  _param_cashitem_dblog dblogSheet(player->m_Param.GetCharSerial());
  _buybygold_set_cashitem_dblog_sheet(player, &dblogSheet);

  const __int64 errorCode = _buybygold_check_valid(player, request, &dblogSheet);
  if (errorCode)
  {
    ICsSendInterface::SendMsg_Error(wSock, static_cast<int>(errorCode));
    return true;
  }

  _result_csi_buy_zocl sendResult{};
  std::memset(&sendResult, 0, sizeof(sendResult));

  _request_csi_buy_clzo::__item *lastSrcItem = nullptr;
  bool couponUsed = false;
  for (int index = 0; index < request->nNum; ++index)
  {
    lastSrcItem = &request->item[index];
    const __int64 singleBuyError =
      _buybygold_buy_single_item(player, request, lastSrcItem, &dblogSheet, &couponUsed, &sendResult);
    if (singleBuyError)
    {
      ICsSendInterface::SendMsg_Error(wSock, static_cast<int>(singleBuyError));
      return true;
    }
  }

  _buybygold_complete(player, &sendResult, request, lastSrcItem, &dblogSheet, couponUsed);
  return true;
}

int CashItemRemoteStore::CheckCouponType(_STORAGE_POS_INDIV *pCoupon, CPlayer *pOne, unsigned __int8 byCouponNum)
{
  char couponItemPart[32]{};
  char discountType[16]{};
  couponItemPart[0] = static_cast<char>(-1);
  couponItemPart[1] = static_cast<char>(-1);
  couponItemPart[2] = static_cast<char>(-1);
  discountType[0] = static_cast<char>(-1);
  discountType[1] = static_cast<char>(-1);
  discountType[2] = static_cast<char>(-1);

  int validCount = 0;
  for (int index = 0; index < byCouponNum; ++index)
  {
    _STORAGE_LIST *storage = pOne->m_Param.m_pStoragePtr[pCoupon[index].byStorageCode];
    if (!storage)
    {
      continue;
    }

    _STORAGE_LIST::_db_con *couponItem = storage->GetPtrFromSerial(pCoupon[index].wItemSerial);
    if (!couponItem)
    {
      continue;
    }

    _CouponItem_fld *record = static_cast<_CouponItem_fld *>(
      g_Main.m_tblItemData[couponItem->m_byTableCode].GetRecord(couponItem->m_wItemIndex));
    if (!record)
    {
      continue;
    }

    couponItemPart[index] = static_cast<char>(record->m_nCouponItemPart);
    discountType[index] = static_cast<char>(record->m_nDiscount);
    ++validCount;
  }

  if (validCount < 2)
  {
    // narrowing cast for thunk return parity
    return static_cast<int>(validCount);
  }

  if ((couponItemPart[0] == couponItemPart[1] && couponItemPart[1] == couponItemPart[2])
      || (couponItemPart[0] == couponItemPart[2]
          && static_cast<unsigned __int8>(couponItemPart[2]) == static_cast<unsigned __int8>(-1)))
  {
    return 0;
  }

  if ((discountType[0] == discountType[1] || discountType[1] == discountType[2])
      && (discountType[0] == discountType[1]
          || static_cast<unsigned __int8>(discountType[2]) != static_cast<unsigned __int8>(-1)))
  {
    // narrowing cast for thunk return parity
    return static_cast<int>(validCount);
  }
  return 0;
}

bool CashItemRemoteStore::UseDiscountCoupon(_param_cash_update *pBuyList, _STORAGE_POS_INDIV pCoupon, CPlayer *pOne)
{
  _STORAGE_LIST *storage = pOne->m_Param.m_pStoragePtr[pCoupon.byStorageCode];
  if (!storage)
  {
    return false;
  }

  _STORAGE_LIST::_db_con *couponItem = storage->GetPtrFromSerial(pCoupon.wItemSerial);
  if (!couponItem)
  {
    return false;
  }

  _CouponItem_fld *record = static_cast<_CouponItem_fld *>(
    g_Main.m_tblItemData[couponItem->m_byTableCode].GetRecord(couponItem->m_wItemIndex));
  if (!record)
  {
    return false;
  }

  int totalPrice = 0;
  for (int index = 0; index < pBuyList->in_nNum10; ++index)
  {
    totalPrice += pBuyList->in_item[static_cast<unsigned __int64>(index)].in_byOverlapNum
      * pBuyList->in_item[static_cast<unsigned __int64>(index)].in_nPrice;
  }

  const int applyValue = record->m_nPoint;
  const int applyType = record->m_nDiscount;
  const int tenScaledValue = 10 * applyValue;

  if (applyType)
  {
    for (int index = 0; index < pBuyList->in_nNum10; ++index)
    {
      pBuyList->in_item[static_cast<unsigned __int64>(index)].in_nDiscount += static_cast<unsigned __int16>(applyValue);
      pBuyList->in_item[static_cast<unsigned __int64>(index)].in_nEventType = 4;
    }
    return true;
  }

  if (tenScaledValue >= totalPrice)
  {
    return false;
  }

  for (int index = 0; index < pBuyList->in_nNum10; ++index)
  {
    const int itemPrice = pBuyList->in_item[static_cast<unsigned __int64>(index)].in_byOverlapNum
      * pBuyList->in_item[static_cast<unsigned __int64>(index)].in_nPrice;
    if (itemPrice > tenScaledValue
        && !pBuyList->in_item[static_cast<unsigned __int64>(index)].in_bIsApplyCoupon)
    {
      pBuyList->in_item[static_cast<unsigned __int64>(index)].in_nDiscount = static_cast<unsigned __int16>(applyValue);
      pBuyList->in_item[static_cast<unsigned __int64>(index)].in_bIsApplyCoupon = true;
      pBuyList->in_item[static_cast<unsigned __int64>(index)].in_nEventType = 4;
      CPlayer::s_MgrItemHistory.coupon_use_buy_item(
        couponItem,
        pBuyList->in_item[static_cast<unsigned __int64>(index)].in_strItemCode,
        pOne->m_szItemHistoryFileName);
      return true;
    }
  }
  return false;
}

bool CashItemRemoteStore::IsUsableCoupon(
  _request_csi_buy_clzo *pBuyList,
  _STORAGE_POS_INDIV pCoupon,
  CPlayer *pOne,
  bool *bCheck)
{
  _STORAGE_LIST *storage = pOne->m_Param.m_pStoragePtr[pCoupon.byStorageCode];
  if (!storage)
  {
    return false;
  }

  _STORAGE_LIST::_db_con *couponItem = storage->GetPtrFromSerial(pCoupon.wItemSerial);
  if (!couponItem)
  {
    return false;
  }

  _CouponItem_fld *record = static_cast<_CouponItem_fld *>(
    g_Main.m_tblItemData[couponItem->m_byTableCode].GetRecord(couponItem->m_wItemIndex));
  if (!record)
  {
    return false;
  }

  int totalPrice = 0;
  for (int index = 0; index < pBuyList->nNum; ++index)
  {
    totalPrice += pBuyList->item[index].byOverlapNum * pBuyList->item[index].nPrice;
  }

  const int applyValue = record->m_nPoint;
  const int applyType = record->m_nDiscount;
  const int tenScaledValue = 10 * applyValue;
  if (applyType)
  {
    return true;
  }
  if (tenScaledValue >= totalPrice)
  {
    return false;
  }

  for (int index = 0; index < pBuyList->nNum; ++index)
  {
    const int itemPrice = pBuyList->item[index].byOverlapNum * pBuyList->item[index].nPrice;
    if (itemPrice > tenScaledValue && !bCheck[index])
    {
      bCheck[index] = true;
      return true;
    }
  }
  return false;
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_check_buyitem(
  unsigned __int8 byRaceSex,
  const _request_csi_buy_clzo::__item *pCsItem,
  const _CashShop_fld *pFld)
{
  if (!pCsItem || !pFld || pCsItem->byTblCode >= 37)
  {
    return 4;
  }

  char *itemEquipCivil = GetItemEquipCivil(pCsItem->byTblCode, pCsItem->wItemIdx);
  if (itemEquipCivil[byRaceSex] != '1')
  {
    return 12;
  }

  _base_fld *itemRecord = g_Main.m_tblItemData[pCsItem->byTblCode].GetRecord(pCsItem->wItemIdx);
  if (!itemRecord || !pFld)
  {
    return 11;
  }
  if (!pFld->m_bView)
  {
    return 11;
  }
  if (strncmp(pFld->m_strCsItemCode, itemRecord->m_strCode, 7))
  {
    return 11;
  }
  if (pCsItem->byOverlapNum > MAX_ITEM_OVERLAP)
  {
    return 6;
  }
  if (pCsItem->byOverlapNum > 1 && !IsOverLapItem(pCsItem->byTblCode))
  {
    return 6;
  }
  if (pFld->m_nCsPrice != pCsItem->nPrice)
  {
    return 5;
  }
  if (pCsItem->byEventType == 5)
  {
    if (pCsItem->byDiscount != GetLimDiscout())
    {
      return 7;
    }
  }
  else if (static_cast<unsigned __int8>(pFld->m_nCsDiscount) != pCsItem->byDiscount)
  {
    return 7;
  }

  for (int index = 0; index < 8; ++index)
  {
    if (pFld->m_nCsEvent[index] != pCsItem->nEvent[index])
    {
      return 18;
    }
  }

  if (pCsItem->byEventType != 5 || BuyLimSale(pCsItem->byTblCode, pCsItem->wItemIdx))
  {
    return 0;
  }
  return 21;
}

void CashItemRemoteStore::_buybygold_set_cashitem_dblog_sheet(CPlayer *pOne, _param_cashitem_dblog *pSheet)
{
  pSheet->byLv = static_cast<unsigned __int8>(pOne->m_Param.GetLevel());
  pSheet->in_bAdjustDiscount = is_cde_time() != 0;
  pSheet->in_bOneN_One = IsEventTime(1) != 0;
  pSheet->in_bSetDiscount = IsEventTime(0) != 0;
  pSheet->in_bLimited_Sale = IsEventTime(2) != 0;
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_check_valid(
  CPlayer *pOne,
  _request_csi_buy_clzo *pRecv,
  _param_cashitem_dblog *pSheet)
{
  if (pOne->m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    return 16;
  }

  pOne->m_Param.m_dbInven.GetNumEmptyCon();
  int onePlusOneCount = 0;
  for (int index = 0; index < pRecv->nNum; ++index)
  {
    if (pRecv->item[index].byEventType == 3)
    {
      ++onePlusOneCount;
    }
  }

  const int totalRequiredSlots = pRecv->nNum + onePlusOneCount;
  if (totalRequiredSlots > pOne->m_Param.m_dbInven.GetNumEmptyCon())
  {
    return 16;
  }

  const __int64 couponError = _buybygold_check_coupon(pOne, pRecv, pSheet);
  if (couponError)
  {
    return static_cast<CashItemRemoteStore::CS_RCODE>(couponError);
  }
  return 0;
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_check_coupon(
  CPlayer *pOne,
  _request_csi_buy_clzo *pRecv,
  _param_cashitem_dblog *pSheet)
{
  if (pRecv->byCouponNum)
  {
    if (pSheet->in_bAdjustDiscount || pSheet->in_bOneN_One)
    {
      return 20;
    }

    if (CheckCouponType(pRecv->CouponItem, pOne, pRecv->byCouponNum) <= 0)
    {
      return 19;
    }

    bool couponCheck[20]{};
    for (int index = 0; index < pRecv->byCouponNum; ++index)
    {
      if (!IsUsableCoupon(pRecv, pRecv->CouponItem[index], pOne, couponCheck))
      {
        return 19;
      }
      pSheet->in_CouponItem[index] = pRecv->CouponItem[index];
    }
  }
  return 0;
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_buy_single_item(
  CPlayer *pOne,
  _request_csi_buy_clzo *pRecv,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  bool *bCouponUse,
  _result_csi_buy_zocl *Send)
{
  _CashShop_fld *cashField = nullptr;
  __int64 errorCode = _buybygold_buy_single_item_check_item(pOne, pSrc, pSheet, &cashField);
  if (errorCode)
  {
    return static_cast<CashItemRemoteStore::CS_RCODE>(errorCode);
  }

  unsigned int pushedPrice = 0;
  unsigned int discountRate = 0;
  bool couponUseCheck[3]{};
  errorCode = _buybygold_buy_single_item_proc_price(
    pOne,
    pRecv,
    pSrc,
    pSheet,
    cashField,
    couponUseCheck,
    Send,
    &pushedPrice,
    &discountRate);
  if (errorCode)
  {
    return static_cast<CashItemRemoteStore::CS_RCODE>(errorCode);
  }

  _STORAGE_LIST::_db_con giveItem;
  errorCode = _buybygold_buy_single_item_give_item(pOne, pSrc, &giveItem);
  if (errorCode)
  {
    pOne->AddGold(static_cast<int>(pushedPrice), true);
    return static_cast<CashItemRemoteStore::CS_RCODE>(errorCode);
  }

  errorCode = _buybygold_buy_single_item_additional_process(pOne, pSrc, pSheet, Send);
  if (errorCode)
  {
    return static_cast<CashItemRemoteStore::CS_RCODE>(errorCode);
  }

  _buybygold_buy_single_item_proc_complete(
    pOne,
    pSrc,
    pSheet,
    cashField,
    &giveItem,
    Send,
    pushedPrice,
    discountRate,
    couponUseCheck,
    bCouponUse);
  return 0;
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_buy_single_item_check_item(
  CPlayer *pOne,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  _CashShop_fld **pCsFld)
{
  if (pSrc->byEventType == 1 && !pSheet->in_bAdjustDiscount)
  {
    return 18;
  }
  if (pSrc->byEventType == 2 && !pSheet->in_bSetDiscount)
  {
    return 18;
  }
  if (pSrc->byEventType == 3 && !pSheet->in_bOneN_One)
  {
    return 18;
  }
  if (pSrc->byEventType == 5 && !pSheet->in_bLimited_Sale)
  {
    return 22;
  }

  *pCsFld = reinterpret_cast<_CashShop_fld *>(_kRecGoods.GetRecord(pSrc->wStoreIdx));
  if (!*pCsFld)
  {
    return 11;
  }

  if (pSheet->in_bLimited_Sale && pSrc->byEventType == 5)
  {
    const int discountPrice = pSrc->byDiscount * pSrc->nPrice / 100;
    const unsigned int needGold = static_cast<unsigned int>(pSrc->nPrice - discountPrice);
    if (pOne->m_Param.GetGold() < needGold)
    {
      return 5;
    }
  }
  else if (pOne->m_Param.GetGold() < static_cast<unsigned int>((*pCsFld)->m_nCsPrice))
  {
    return 5;
  }

  const unsigned __int8 raceSexCode = static_cast<unsigned __int8>(pOne->m_Param.GetRaceSexCode());
  const __int64 itemError = _check_buyitem(raceSexCode, pSrc, *pCsFld);
  if (itemError)
  {
    return static_cast<CashItemRemoteStore::CS_RCODE>(itemError);
  }
  return 0;
}

unsigned int CashItemRemoteStore::_buybygold_buy_single_item_calc_price(
  CPlayer *pOne,
  _request_csi_buy_clzo *pRecv,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  _CashShop_fld *pCsFld,
  bool *bCouponUseCheck,
  _result_csi_buy_zocl *Send,
  unsigned int *dwDiscount)
{
  (void)Send;

  const unsigned int basePrice = pSrc->byOverlapNum * pCsFld->m_nCsPrice;
  if (pRecv->byCouponNum)
  {
    return _buybygold_buy_single_item_calc_price_coupon(
      pOne,
      pRecv,
      pSrc->byOverlapNum,
      pCsFld->m_nCsPrice,
      bCouponUseCheck,
      dwDiscount);
  }
  if (pSheet->in_bAdjustDiscount && pSrc->byEventType == 1)
  {
    // narrowing cast for thunk return parity
    return static_cast<unsigned int>(_buybygold_buy_single_item_calc_price_discount(pCsFld, pSrc->byOverlapNum));
  }
  if (pSheet->in_bSetDiscount && pRecv->bySetKind && pRecv->bySetKind <= 4)
  {
    // narrowing cast for thunk return parity
    return static_cast<unsigned int>(_buybygold_buy_single_item_calc_price_one_n_one(pRecv->bySetKind, pCsFld->m_nCsPrice, pSrc->byOverlapNum));
  }
  if ((!pSheet->in_bOneN_One || pSrc->byEventType != 3) && pSheet->in_bLimited_Sale && pSrc->byEventType == 5)
  {
    // narrowing cast for thunk return parity
    return static_cast<unsigned int>(_buybygold_buy_single_item_calc_price_limitsale(pCsFld->m_nCsPrice, pSrc->byOverlapNum));
  }
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(basePrice);
}

unsigned int CashItemRemoteStore::_buybygold_buy_single_item_calc_price_discount(
  _CashShop_fld *pCsFld,
  unsigned __int8 byOverlapNum)
{
  unsigned __int16 discountRate = 0;
  if (pCsFld->m_nCsDiscount == -1)
  {
    discountRate = m_cde.m_ini.m_wCsDiscount;
  }

  const int discountPrice =
    10 * static_cast<int>((static_cast<double>((static_cast<unsigned int>(discountRate) * pCsFld->m_nCsPrice) / 100) * 0.1) + 0.5);
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(byOverlapNum * static_cast<unsigned int>(pCsFld->m_nCsPrice - discountPrice));
}

unsigned int CashItemRemoteStore::_buybygold_buy_single_item_calc_price_one_n_one(
  unsigned __int8 bySetKind,
  int nCsPrice,
  unsigned __int8 byOverlapNum)
{
  const unsigned __int8 setDiscount = GetSetDiscout(bySetKind - 1);
  const int discountPrice =
    10 * static_cast<int>((static_cast<double>((static_cast<unsigned int>(setDiscount) * nCsPrice) / 100) * 0.1) + 0.5);
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(byOverlapNum * static_cast<unsigned int>(nCsPrice - discountPrice));
}

unsigned int CashItemRemoteStore::_buybygold_buy_single_item_calc_price_limitsale(int nCsPrice, unsigned __int8 byOverlapNum)
{
  const int discountPrice = 10 * static_cast<int>((static_cast<double>((GetLimDiscout() * nCsPrice) / 100) * 0.1) + 0.5);
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(byOverlapNum * static_cast<unsigned int>(nCsPrice - discountPrice));
}

unsigned int CashItemRemoteStore::_buybygold_buy_single_item_calc_price_coupon(
  CPlayer *pOne,
  _request_csi_buy_clzo *pRecv,
  unsigned __int8 byOverlapNum,
  int nCsPrice,
  bool *bCouponUseCheck,
  unsigned int *dwDiscount)
{
  unsigned int basePrice = byOverlapNum * nCsPrice;
  int totalDiscountPrice = 0;
  for (int index = 0; index < pRecv->byCouponNum; ++index)
  {
    _STORAGE_LIST *storage = pOne->m_Param.m_pStoragePtr[pRecv->CouponItem[index].byStorageCode];
    if (!storage)
    {
      continue;
    }

    _STORAGE_LIST::_db_con *couponItem = storage->GetPtrFromSerial(pRecv->CouponItem[index].wItemSerial);
    if (!couponItem)
    {
      continue;
    }

    _CouponItem_fld *record = static_cast<_CouponItem_fld *>(
      g_Main.m_tblItemData[couponItem->m_byTableCode].GetRecord(couponItem->m_wItemIndex));
    if (!record)
    {
      continue;
    }

    const int applyValue = record->m_nPoint;
    const int applyType = record->m_nDiscount;
    const unsigned int tenScaledValue = 10 * applyValue;
    if (applyType)
    {
      int rawDiscount = applyValue * nCsPrice / 100;
      rawDiscount = 10 * static_cast<int>((static_cast<double>(rawDiscount) * 0.1) + 0.5);
      totalDiscountPrice += rawDiscount;
      *dwDiscount += applyValue;
      bCouponUseCheck[index] = true;
    }
    else if (tenScaledValue < basePrice && !bCouponUseCheck[index])
    {
      basePrice -= applyValue;
      *dwDiscount += applyValue;
      bCouponUseCheck[index] = true;
    }
  }

  if (totalDiscountPrice)
  {
    // narrowing cast for thunk return parity
    return static_cast<unsigned int>(byOverlapNum * static_cast<unsigned int>(nCsPrice - totalDiscountPrice));
  }
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(basePrice);
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_buy_single_item_proc_price(
  CPlayer *pOne,
  _request_csi_buy_clzo *pRecv,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  _CashShop_fld *pCsFld,
  bool *bCouponUseCheck,
  _result_csi_buy_zocl *Send,
  unsigned int *dwPrice,
  unsigned int *dwDiscountRate)
{
  *dwPrice =
    static_cast<unsigned int>(_buybygold_buy_single_item_calc_price(
      pOne,
      pRecv,
      pSrc,
      pSheet,
      pCsFld,
      bCouponUseCheck,
      Send,
      dwDiscountRate));
  if (pOne->m_Param.GetGold() < *dwPrice)
  {
    return 13;
  }

  pOne->SubGold(*dwPrice);
  return 0;
}

void CashItemRemoteStore::_buybygold_buy_single_item_setsenddata(
  _STORAGE_LIST::_db_con *GiveItem,
  _result_csi_buy_zocl *Send)
{
  Send->item[Send->nNum].byTblCode = GiveItem->m_byTableCode;
  Send->item[Send->nNum].wItemIdx = GiveItem->m_wItemIndex;
  Send->item[Send->nNum].dwDur = static_cast<unsigned int>(GiveItem->m_dwDur);
  Send->item[Send->nNum].dwUp = GiveItem->m_dwLv;
  Send->item[Send->nNum].dwItemSerial = GiveItem->m_wSerial;
  Send->item[Send->nNum].byCsMethod = GiveItem->m_byCsMethod;
  Send->item[Send->nNum++].dwT = GiveItem->m_dwT;
}

void CashItemRemoteStore::_buybygold_buy_single_item_setbuydblog(
  _param_cashitem_dblog *pSheet,
  _STORAGE_LIST::_db_con *GiveItem,
  unsigned int dwPrice,
  unsigned int dwDiscountRate)
{
  pSheet->data[pSheet->nBuyNum].byRet = 0;
  pSheet->data[pSheet->nBuyNum].byTblCode = GiveItem->m_byTableCode;
  pSheet->data[pSheet->nBuyNum].wItemIndex = GiveItem->m_wItemIndex;
  pSheet->data[pSheet->nBuyNum].byOverlapNum = static_cast<unsigned __int8>(GiveItem->m_dwDur);
  pSheet->data[pSheet->nBuyNum].dwCost = dwPrice;
  pSheet->data[pSheet->nBuyNum++].iCashDiscount = static_cast<int>(dwDiscountRate);
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_buy_single_item_give_item(
  CPlayer *pOne,
  _request_csi_buy_clzo::__item *pSrc,
  _STORAGE_LIST::_db_con *GiveItem)
{
  GiveItem->m_byTableCode = pSrc->byTblCode;
  GiveItem->m_wItemIndex = pSrc->wItemIdx;
  if (IsOverLapItem(GiveItem->m_byTableCode))
  {
    GiveItem->m_dwDur = pSrc->byOverlapNum;
  }
  else
  {
    GiveItem->m_dwDur = GetItemDurPoint(pSrc->byTblCode, pSrc->wItemIdx);
  }

  GiveItem->m_dwLv = GetDefItemUpgSocketNum(pSrc->byTblCode, pSrc->wItemIdx);
  GiveItem->m_dwLv = GetBitAfterSetLimSocket(static_cast<unsigned __int8>(GiveItem->m_dwLv));
  GiveItem->m_wSerial = pOne->m_Param.GetNewItemSerial();

  const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(pSrc->byTblCode, pSrc->wItemIdx);
  if (timeRec && timeRec->m_nCheckType)
  {
    GiveItem->m_byCsMethod = static_cast<unsigned __int8>(timeRec->m_nCheckType);
    __time32_t currentTime[5]{};
    _time32(currentTime);
    GiveItem->m_dwT = static_cast<unsigned int>(timeRec->m_nUseTime + currentTime[0]);
    GiveItem->m_dwLendRegdTime = static_cast<unsigned int>(currentTime[0]);
  }

  if (pOne->Emb_AddStorage(0, GiveItem, false, true))
  {
    return 0;
  }
  return 15;
}

void CashItemRemoteStore::_buybygold_buy_single_item_proc_complete(
  CPlayer *pOne,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  _CashShop_fld *pCsFld,
  _STORAGE_LIST::_db_con *GiveItem,
  _result_csi_buy_zocl *Send,
  unsigned int dwPrice,
  unsigned int dwDiscountRate,
  bool *bCouponUseCheck,
  bool *bCouponUse)
{
  _buybygold_buy_single_item_setsenddata(GiveItem, Send);

  CPlayer::s_MgrItemHistory.buy_to_inven_cashitem(
    GiveItem->m_byTableCode,
    GiveItem->m_wItemIndex,
    pCsFld->m_nCsPrice,
    pCsFld->m_nCsDiscount,
    pSrc->byOverlapNum,
    pCsFld->m_nCsPrice,
    static_cast<int>(pOne->m_Param.GetGold()),
    pOne->m_szItemHistoryFileName,
    GiveItem->m_lnUID,
    pSrc->byEventType);
  _buybygold_buy_single_item_setbuydblog(pSheet, GiveItem, dwPrice, dwDiscountRate);

  for (int index = 0; index < 3; ++index)
  {
    if (bCouponUseCheck[index])
    {
      *bCouponUse = true;
      return;
    }
  }
}

CashItemRemoteStore::CS_RCODE CashItemRemoteStore::_buybygold_buy_single_item_additional_process(
  CPlayer *pOne,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  _result_csi_buy_zocl *Send)
{
  if (pSheet->in_bOneN_One && pSrc->byEventType == 3)
  {
    _STORAGE_LIST::_db_con extraItem;
    extraItem.m_byTableCode = pSrc->byTblCode;
    extraItem.m_wItemIndex = pSrc->wItemIdx;
    if (IsOverLapItem(extraItem.m_byTableCode))
    {
      extraItem.m_dwDur = pSrc->byOverlapNum;
    }
    else
    {
      extraItem.m_dwDur = GetItemDurPoint(pSrc->byTblCode, pSrc->wItemIdx);
    }
    extraItem.m_dwLv = GetDefItemUpgSocketNum(pSrc->byTblCode, pSrc->wItemIdx);
    extraItem.m_wSerial = pOne->m_Param.GetNewItemSerial();

    if (!pOne->Emb_AddStorage(0, &extraItem, false, true))
    {
      return 15;
    }

    _buybygold_buy_single_item_setsenddata(&extraItem, Send);
    _base_fld *record = g_Main.m_tblItemData[extraItem.m_byTableCode].GetRecord(extraItem.m_wItemIndex);
    if (record)
    {
      CAsyncLogger *logger = CAsyncLogger::Instance();
      logger->FormatLog(
        5,
        "[Name: %s AccountID: %d] [Insert 1+1 Event CASTITEM] : %s(%s) [UID: %I64u] [Num:%d Event: %d]",
        pOne->m_Param.GetCharNameA(),
        pOne->m_pUserDB->m_dwAccountSerial,
        record->m_strCode,
        GetItemKorName(extraItem.m_byTableCode, extraItem.m_wItemIndex),
        extraItem.m_lnUID,
        pSrc->byOverlapNum,
        pSrc->byEventType);
    }
  }
  return 0;
}

bool CashItemRemoteStore::GoodsListBuyByGold(unsigned __int16 wSock, char *pPacket)
{
  (void)pPacket;

  if (wSock >= MAX_PLAYER)
  {
    return false;
  }

  CPlayer *player = &g_Player[wSock];
  if (!player->m_bLive || !player->m_bOper)
  {
    return true;
  }

  _result_csi_goods_list_zocl msg{};
  msg.nCashAmount = player->m_Param.GetGold();
  msg.bAdjustDiscountRate = is_cde_time() != 0;
  msg.bOneNOne = IsEventTime(1) != 0;
  msg.bSetDiscount = IsEventTime(0) != 0;

  for (int index = 0; index < 4; ++index)
  {
    if (IsEventTime(0))
    {
      msg.bySetDiscount[index] = GetSetDiscout(static_cast<unsigned __int8>(index));
    }
    else
    {
      msg.bySetDiscount[index] = 0;
    }
  }

  msg.bLimSale = IsEventTime(2) != 0;
  if (msg.bLimSale)
  {
    msg.byLimDiscount = GetLimDiscout();
  }
  else
  {
    msg.byLimDiscount = 0;
  }

  unsigned __int8 type[2]{};
  type[0] = 57;
  type[1] = 2;

  g_Network.m_pProcess[0]->LoadSendMsg(wSock, type, reinterpret_cast<char *>(&msg), msg.size());
  return true;
}

unsigned __int8 CashItemRemoteStore::Get_CashEvent_Status(unsigned __int8 byEventType)
{
  return m_cash_event[byEventType].m_event_status;
}

bool CashItemRemoteStore::ChangeEventTime(unsigned __int8 byEventType)
{
  if (!m_cash_event[byEventType].m_ini.m_bRepeat)
  {
    return false;
  }

  std::tm tmValue{};
  tmValue.tm_year = m_cash_event[byEventType].m_ini.m_wYear[0] - 1900;
  tmValue.tm_mon = m_cash_event[byEventType].m_ini.m_byMonth[0] - 1;
  tmValue.tm_mday = m_cash_event[byEventType].m_ini.m_byDay[0] + m_cash_event[byEventType].m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cash_event[byEventType].m_ini.m_byHour[0];
  tmValue.tm_min = m_cash_event[byEventType].m_ini.m_byMinute[0];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cash_event[byEventType].m_ini.m_EventTime[0] = _mktime32(&tmValue);
  if (m_cash_event[byEventType].m_ini.m_EventTime[0] == -1)
  {
    m_cash_event[byEventType].m_ini.m_bRepeat = 0;
    m_cash_event[byEventType].m_event_log.Write("ChangeEventTime() : Fail When Calculate Event Begin Time");
    return false;
  }

  m_cash_event[byEventType].m_ini.m_wYear[0] = tmValue.tm_year + 1900;
  m_cash_event[byEventType].m_ini.m_byMonth[0] = tmValue.tm_mon + 1;
  m_cash_event[byEventType].m_ini.m_byDay[0] = tmValue.tm_mday;
  m_cash_event[byEventType].m_ini.m_byHour[0] = tmValue.tm_hour;
  m_cash_event[byEventType].m_ini.m_byMinute[0] = tmValue.tm_min;

  std::memset(&tmValue, 0, sizeof(tmValue));
  tmValue.tm_year = m_cash_event[byEventType].m_ini.m_wYear[1] - 1900;
  tmValue.tm_mon = m_cash_event[byEventType].m_ini.m_byMonth[1] - 1;
  tmValue.tm_mday = m_cash_event[byEventType].m_ini.m_byDay[1] + m_cash_event[byEventType].m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cash_event[byEventType].m_ini.m_byHour[1];
  tmValue.tm_min = m_cash_event[byEventType].m_ini.m_byMinute[1];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cash_event[byEventType].m_ini.m_EventTime[1] = _mktime32(&tmValue);
  if (m_cash_event[byEventType].m_ini.m_EventTime[1] == -1)
  {
    m_cash_event[byEventType].m_ini.m_bRepeat = 0;
    m_cash_event[byEventType].m_event_log.Write("ChangeEventTime() : Fail When Calculate Event End Time");
    return false;
  }

  m_cash_event[byEventType].m_ini.m_wYear[1] = tmValue.tm_year + 1900;
  m_cash_event[byEventType].m_ini.m_byMonth[1] = tmValue.tm_mon + 1;
  m_cash_event[byEventType].m_ini.m_byDay[1] = tmValue.tm_mday;
  m_cash_event[byEventType].m_ini.m_byHour[1] = tmValue.tm_hour;
  m_cash_event[byEventType].m_ini.m_byMinute[1] = tmValue.tm_min;
  return true;
}

void CashItemRemoteStore::Set_LimitedSale_DCK(unsigned __int8 /*byEventType*/, unsigned __int8 byDCK)
{
  m_lim_event.DCK = byDCK;
}

void CashItemRemoteStore::Set_LimitedSale_Event()
{
  _cash_lim_sale tmp{};
  std::memcpy(&tmp, &m_lim_event_New, sizeof(tmp));
  std::memcpy(&m_lim_event, &tmp, sizeof(m_lim_event));
}

void CashItemRemoteStore::Inform_CashEvent_Status_All(
  unsigned __int8 byEventType,
  unsigned __int8 byStatus,
  _cash_event_ini *pIni)
{
  for (unsigned __int16 index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bOper && player->m_bLive)
    {
      ICsSendInterface::SendMsg_CashEventInform(
        player->m_ObjID.m_wIndex,
        byEventType,
        byStatus,
        pIni,
        &m_lim_event);
    }
  }

  char eventName[80]{};
  Get_CashEvent_Name(byEventType, eventName);
  m_cash_event[byEventType].m_event_log.Write(
    "[ %s CashEvent Inform when Event status change] [EventState : %d] [EventTime : %d/%d/%d %d:%d  ~ %d/%d/%d %d:%d ]",
    eventName,
    byStatus,
    pIni->m_wYear[0],
    pIni->m_byMonth[0],
    pIni->m_byDay[0],
    pIni->m_byHour[0],
    pIni->m_byMinute[0],
    pIni->m_wYear[1],
    pIni->m_byMonth[1],
    pIni->m_byDay[1],
    pIni->m_byHour[1],
    pIni->m_byMinute[1]);
}

void CashItemRemoteStore::Check_CashEvent_Status(unsigned __int8 byEventType)
{
  const unsigned __int8 eventStatus = Get_CashEvent_Status(byEventType);
  __time32_t now[4]{};
  _time32(now);

  auto moveToEnd = [&]() { Set_CashEvent_Status(byEventType, 5); };
  switch (eventStatus)
  {
    case 0:
    {
      if (!m_cash_event[byEventType].m_ini.m_bUseCashEvent || !m_cash_event[byEventType].m_ini.m_bRepeat)
      {
        moveToEnd();
        break;
      }

      const bool changed = ChangeEventTime(byEventType) != 0;
      if (m_cash_event[byEventType].m_ini.m_NextEventTime[1] <= now[0] || !changed)
      {
        moveToEnd();
        break;
      }

      Set_CashEvent_Status(byEventType, 1);
      break;
    }

    case 1:
    {
      if (m_cash_event[byEventType].m_ini.m_bRepeat
          && m_cash_event[byEventType].m_ini.m_EventTime[2] <= now[0])
      {
        moveToEnd();
      }
      else if (now[0] >= m_cash_event[byEventType].m_ini.m_EventTime[0])
      {
        Set_CashEvent_Status(byEventType, 2);
        if (byEventType == 2)
        {
          Set_LimitedSale_DCK(byEventType, 1);
          g_Main.PushDQSData(-1, nullptr, 157, nullptr, 0);
          Set_DB_LimitedSale_Event();
        }
        Inform_CashEvent_Status_All(byEventType, 2, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 2:
    {
      if (byEventType == 2 && m_lim_event.DCK != 1)
      {
        g_Main.PushDQSData(-1, nullptr, 157, nullptr, 0);
        Set_LimitedSale_Event();
      }

      if (m_cash_event[byEventType].m_ini.m_EventTime[1] - now[0]
          <= m_cash_event[byEventType].m_event_inform_before[0])
      {
        Set_CashEvent_Status(byEventType, 3);
        Inform_CashEvent_Status_All(byEventType, 3, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 3:
    {
      if (byEventType == 2 && m_lim_event.DCK != 1)
      {
        g_Main.PushDQSData(-1, nullptr, 157, nullptr, 0);
        Set_LimitedSale_Event();
      }

      if (m_cash_event[byEventType].m_ini.m_EventTime[1] - now[0]
          <= m_cash_event[byEventType].m_event_inform_before[1])
      {
        Set_CashEvent_Status(byEventType, 4);
        Inform_CashEvent_Status_All(byEventType, 4, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 4:
    {
      if (byEventType == 2 && m_lim_event.DCK != 1)
      {
        g_Main.PushDQSData(-1, nullptr, 157, nullptr, 0);
        Set_LimitedSale_Event();
      }

      if (now[0] >= m_cash_event[byEventType].m_ini.m_EventTime[1])
      {
        Set_CashEvent_Status(byEventType, 5);
        if (byEventType == 2)
        {
          Set_LimitedSale_DCK(byEventType, 0);
          Set_DB_LimitedSale_Event();
        }
        Inform_CashEvent_Status_All(byEventType, 5, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 5:
    {
      if (!m_cash_event[byEventType].m_ini.m_bUseCashEvent || !m_cash_event[byEventType].m_ini.m_bRepeat)
      {
        Set_CashEvent_Status(byEventType, 7);
        break;
      }

      if (m_cash_event[byEventType].m_ini.m_EventTime[2] <= now[0])
      {
        m_cash_event[byEventType].m_ini.m_bUseCashEvent = 0;
        m_cash_event[byEventType].m_ini.m_bRepeat = 0;
        Set_CashEvent_Status(byEventType, 7);
        return;
      }

      if (SetNextEventTime(byEventType))
      {
        Set_CashEvent_Status(byEventType, 0);
      }
      else
      {
        Set_CashEvent_Status(byEventType, 7);
      }
      break;
    }

    default:
      return;
  }
}

const _CashShop_fld *CashItemRemoteStore::FindCashRec(unsigned int nTbl, int nIdx)
{
  if (nTbl > 36)
  {
    return nullptr;
  }

  CashMap &map = _phmapTbl[nTbl];
  CashMap::iterator it = map.find(nIdx);
  if (it == map.end())
  {
    return nullptr;
  }
  return it->second;
}

int CashItemRemoteStore::GetRemainNumOfGood(unsigned __int16 wStoreIndex)
{
  if (!_pkRemainInfo)
  {
    return 0;
  }

  const int storeIndex = wStoreIndex;
  const int recordNum = _kRecGoods.GetRecordNum();
  if (storeIndex < recordNum)
  {
    // narrowing cast for thunk return parity
    return static_cast<int>(static_cast<int>(static_cast<unsigned int>(_pkRemainInfo[wStoreIndex].nRemainNum)));
  }
  return 0;
}

int CashItemRemoteStore::GetRemainNumOfGood(char *strCode)
{
  if (!_pkRemainInfo)
  {
    return 0;
  }

  for (int j = 0;; ++j)
  {
    const int recordNum = _kRecGoods.GetRecordNum();
    if (j >= recordNum)
    {
      break;
    }
    if (!strncmp(_pkRemainInfo[j].strCode, strCode, 8))
    {
      return static_cast<unsigned int>(_pkRemainInfo[j].nRemainNum);
    }
  }
  return 0;
}

void CashItemRemoteStore::Inform_CashEvent(unsigned __int16 wIndex)
{
  for (int j = 0; j < 3; ++j)
  {
    if (m_cash_event[j].m_ini.m_bUseCashEvent)
    {
      if (IsEventTime(static_cast<unsigned __int8>(j)))
      {
        ICsSendInterface::SendMsg_CashEventInform(
          wIndex,
          static_cast<unsigned __int8>(j),
          m_cash_event[j].m_event_status,
          &m_cash_event[j].m_ini,
          &m_lim_event);
      }
    }
  }
}

void CashItemRemoteStore::Check_Grosssales(unsigned int dwTotalSellCash)
{
  __time32_t timeValue[4]{};
  _time32(timeValue);

  if (m_cde.m_ini.m_cdeTime[0] > timeValue[0] || m_cde.m_ini.m_cdeTime[1] < timeValue[0])
  {
    for (int j = 0; j < 3; ++j)
    {
      if (m_cash_event[j].m_ini.m_EventTime[0] <= timeValue[0] && m_cash_event[j].m_ini.m_EventTime[1] >= timeValue[0])
      {
        return;
      }
    }

    if ((m_con_event.m_eventtime.m_EventTime[0] < timeValue[0] || m_con_event.m_eventtime.m_EventTime[1] < timeValue[0])
        && !m_con_event.m_bConEvent
        && (timeValue[0] <= m_cde.m_ini.m_cdeTime[1]
            || static_cast<unsigned int>(timeValue[0] - m_cde.m_ini.m_cdeTime[1]) > 7200))
    {
      for (int j = 0; j < 3; ++j)
      {
        if (timeValue[0] > m_cash_event[j].m_ini.m_EventTime[1]
            && static_cast<unsigned int>(timeValue[0] - m_cash_event[j].m_ini.m_EventTime[1]) <= 7200)
        {
          return;
        }
      }

      if ((timeValue[0] <= m_con_event.m_eventtime.m_EventTime[1]
           || static_cast<unsigned int>(timeValue[0] - m_con_event.m_eventtime.m_EventTime[1]) > 7200)
          && dwTotalSellCash < m_con_event.m_ini.m_dwCashMin)
      {
        tm *timeInfo = _localtime32(timeValue);
        ++timeInfo->tm_min;
        m_con_event.m_eventtime.m_EventTime[0] = _mktime32(timeInfo);
        m_con_event.m_eventtime.m_nYear[0] = timeInfo->tm_year;
        m_con_event.m_eventtime.m_nMonth[0] = timeInfo->tm_mon;
        m_con_event.m_eventtime.m_nDay[0] = timeInfo->tm_mday;
        m_con_event.m_eventtime.m_nHour[0] = timeInfo->tm_hour;
        m_con_event.m_eventtime.m_nMinute[0] = timeInfo->tm_min;
        timeInfo->tm_min += m_con_event.m_ini.m_iEventTime;
        m_con_event.m_eventtime.m_EventTime[1] = _mktime32(timeInfo);
        m_con_event.m_eventtime.m_nYear[1] = timeInfo->tm_year;
        m_con_event.m_eventtime.m_nMonth[1] = timeInfo->tm_mon;
        m_con_event.m_eventtime.m_nDay[1] = timeInfo->tm_mday;
        m_con_event.m_eventtime.m_nHour[1] = timeInfo->tm_hour;
        m_con_event.m_eventtime.m_nMinute[1] = timeInfo->tm_min;
        m_con_event.m_bConEvent = 1;
        Set_Conditional_Evnet_Status(1);
      }
    }
  }
}

void CashItemRemoteStore::Check_Total_Selling()
{
  if (m_con_event.m_ini.m_bUseConEvent == 1 && !m_con_event.m_bConEvent)
  {
    _param_cash_total_selling info{};
    const unsigned __int64 size = static_cast<unsigned __int64>(info.size());
    CCashDBWorkManager *manager = CTSingleton<CCashDBWorkManager>::Instance();
    manager->PushTask(4, reinterpret_cast<unsigned __int8 *>(&info), size);
  }
}

void CashItemRemoteStore::Loop_Check_Total_Selling()
{
  // this is not a stub
}

bool CashItemRemoteStore::_InitLoggers()
{
  const char *unusedSystem = "System.sys";
  const char *unusedProcess = "Process.his";
const char *logDir = "..\\ZoneServerLog\\SystemLog\\PartiallyPaid";
  CreateDirectoryA(logDir, nullptr);

  char buffer[260];
  std::memset(buffer, 0, 256);
  unsigned int logTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(buffer, 256, "%s\\Shop_%u.sys", logDir, logTime);
  _kLoggers[0].SetWriteLogFile(buffer, 1, 0, 1, 1);

  std::memset(buffer, 0, 256);
  logTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(buffer, 256, "%s\\Shop_%u.prc", logDir, logTime);
  _kLoggers[1].SetWriteLogFile(buffer, 1, 0, 1, 1);

  std::memset(buffer, 0, 256);
  logTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(buffer, 256, "%s\\DiscountRate_%u.log", logDir, logTime);
  m_cde.m_cde_log.SetWriteLogFile(buffer, 1, 0, 1, 1);

  for (int j = 0; j < 3; ++j)
  {
    std::memset(buffer, 0, 256);
    logTime = static_cast<unsigned int>(GetKorLocalTime());
    sprintf_s(buffer, 256, "%s\\CashEvent_%dType_%u.log", logDir, j, logTime);
    m_cash_event[j].m_event_log.SetWriteLogFile(buffer, 1, 0, 1, 1);
    m_cash_event[j].m_event_log.Write("Event State");
    m_cash_event[j].m_event_log.Write(kEventTypeDisable);
    m_cash_event[j].m_event_log.Write(kEventTypeWait);
    m_cash_event[j].m_event_log.Write(kEventTypeStart);
    m_cash_event[j].m_event_log.Write(kEventType30Before);
    m_cash_event[j].m_event_log.Write(kEventType5Before);
    m_cash_event[j].m_event_log.Write(kEventTypeEnd);
    m_cash_event[j].m_event_log.Write(kEventTypeExpired);
  }

  std::memset(buffer, 0, 256);
  logTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(buffer, 256, "%s\\ConditionalEvent_%u.log", logDir, logTime);
  m_con_event.m_conevent_log.SetWriteLogFile(buffer, 1, 0, 1, 1);

  return true;
}

bool CashItemRemoteStore::_ReadGoods()
{
  char errorMessage[160];
  if (_kRecGoods.ReadRecord(
        ".\\Script\\CashShop.dat",
        static_cast<int>(sizeof(_CashShop_fld)),
        errorMessage))
  {
    if (_MakeLinkTable(errorMessage, 128))
    {
      CRecordData priceTable;
      if (LoadNationalPrice(&priceTable))
      {
        if (__CheckGoods(&priceTable))
        {
          return true;
        }
        return false;
      }
      MyMessageBox("CashItemRemoteStore::_ReadGoods()", "LoadNationalPrice() Failed!");
      g_Main.m_logLoadingError.Write("CashItemRemoteStore::_ReadGoods() LoadNationalPrice() Failed!");
      return false;
    }
    MyMessageBox("CashItemRemoteStore", errorMessage);
    g_Main.m_logLoadingError.Write("Failed CashItemRemoteStore::_MakeLinkTable()");
    return false;
  }

  MyMessageBox("CashItemRemoteStore", errorMessage);
  return false;
}

bool CashItemRemoteStore::_MakeLinkTable(char *szMsg, int nSize)
{
  CashMap *tables = new (std::nothrow) CashMap[37];
  _phmapTbl = tables;

  for (int n = 0; ; ++n)
  {
    const int recordNum = _kRecGoods.GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }

    _CashShop_fld *record = reinterpret_cast<_CashShop_fld *>(_kRecGoods.GetRecord(n));
    if (!record)
    {
      strcpy_s(szMsg, nSize, "Wroing index of cash item\r\n");
      return false;
    }

    const int itemTableCode = GetItemTableCode(record->m_strCsItemCode);
    if (itemTableCode == -1)
    {
      strcpy_s(szMsg, nSize, "Wroing string code[_CashShop_fld::m_strCsItemCode]\r\n");
      return false;
    }

    _base_fld *itemRecord =
      g_Main.m_tblItemData[itemTableCode].GetRecord(record->m_strCsItemCode, 7);
    if (!itemRecord)
    {
      strcpy_s(szMsg, nSize, "Wroing string code[_CashShop_fld::m_strCsItemCode]-NonExist\r\n");
      return false;
    }

    _phmapTbl[itemTableCode].insert(std::make_pair(static_cast<int>(itemRecord->m_dwIndex), record));
  }

  return true;
}

bool CashItemRemoteStore::LoadNationalPrice(CRecordData *krecPrice)
{
  char errorMessage[144];
  std::memset(errorMessage, 0, 128);
  if (krecPrice->ReadRecord(".\\script\\CashShop_str.dat", static_cast<int>(sizeof(_CashShop_str_fld)), errorMessage))
  {
    return true;
  }

  MyMessageBox(
    "CashDbWorker::LoadNationalPrice()",
    "ReadRecord( %s ) ErrorMsg(%s)",
    ".\\script\\CashShop_str.dat",
    errorMessage);
  g_Main.m_logLoadingError.Write(
    "CashDbWorker::LoadNationalPrice() ReadRecord(%s) ErrorMsg(%s) Failed!",
    ".\\script\\CashShop_str.dat",
    errorMessage);
  return false;
}

bool CashItemRemoteStore::__CheckGoods(CRecordData *krecPrice)
{
  const unsigned __int64 recordNum = _kRecGoods.GetRecordNum();
  _remain_num_of_good *remainInfo =
    new (std::nothrow) _remain_num_of_good[static_cast<size_t>(recordNum)];
  _pkRemainInfo = remainInfo;
  if (!_pkRemainInfo)
  {
    return false;
  }

  _CashShop_fld *record = nullptr;
  for (int n = 0; ; ++n)
  {
    const int recordCount = _kRecGoods.GetRecordNum();
    if (n >= recordCount)
    {
      return true;
    }

    record = reinterpret_cast<_CashShop_fld *>(_kRecGoods.GetRecord(n));
    if (!record)
    {
      if (g_Main.IsExcuteService())
      {
        _kLoggers[0].Write(
          "CashShop Data Load Error : Null Pointer Exception (pCsData), CashShop Index(%d)",
          n);
      }
      else
      {
        MyMessageBox(
          "CashITemRemoteStore",
          "CashShop Data Load Error : Null Pointer Exception (pCsData), CashShop Index(%d)",
          n);
      }
      return false;
    }

    const int itemTableCode = GetItemTableCode(record->m_strCsItemCode);
    if (itemTableCode == -1)
    {
      if (g_Main.IsExcuteService())
      {
        _kLoggers[0].Write(
          "CashShop Data Load Error : Invalid Table Code, CashShop Item Code(%s)",
          record->m_strCsItemCode);
      }
      else
      {
        MyMessageBox(
          "CashITemRemoteStore",
          "CashShop Data Load Error : Invalid Table Code, CashShop Item Code(%s)",
          record->m_strCsItemCode);
      }
      return false;
    }

    _base_fld *itemRecord = g_Main.m_tblItemData[itemTableCode].GetRecord(
      record->m_strCsItemCode,
      7);
    if (!itemRecord)
    {
      if (g_Main.IsExcuteService())
      {
        _kLoggers[0].Write(
          "CashShop Data Load Error : Null Pointer Exception (pRec), Not Exist Item! Cash Shop ItemCode(%s)",
          record->m_strCsItemCode);
      }
      else
      {
        MyMessageBox(
          "CashITemRemoteStore",
          "CashShop Data Load Error : Null Pointer Exception (pRec), Not Exist Item! Cash Shop ItemCode(%s)",
          record->m_strCsItemCode);
      }
      return false;
    }

    const _TimeItem_fld *timeRecord = TimeItem::FindTimeRec(itemTableCode, itemRecord->m_dwIndex);
    if (!IsCashItem(itemTableCode, itemRecord->m_dwIndex))
    {
      break;
    }

    _CashShop_str_fld *priceRecord =
      reinterpret_cast<_CashShop_str_fld *>(krecPrice->GetRecord(record->m_strCode));
    if (!priceRecord)
    {
      MyMessageBox(
        "CashItemRemoteStore::__CheckGoods",
        "CashShop_str.dat : Code(%s) Item(%s) Price Not Exist!",
        record->m_strCode,
        record->m_strCsItemCode);
      return false;
    }

    CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    const int price = nationSetting->GetCashItemPrice(priceRecord);
    record->m_nCsPrice = price;

    int checkType = 0;
    int useTime = 0;
    if (timeRecord)
    {
      checkType = timeRecord->m_nCheckType;
      useTime = timeRecord->m_nUseTime;
    }

    char *itemKorName = GetItemKorName(itemTableCode, itemRecord->m_dwIndex);
    _kLoggers[1].Write(
      "[%s(%s)], price:%d, method:%d, lentime:%d, discount:%d, event1: %d, event2: %d",
      itemKorName,
      itemRecord->m_strCode,
      record->m_nCsPrice,
      checkType,
      useTime,
      record->m_nCsDiscount,
      record->m_nCsEvent[0],
      record->m_nCsEvent[1]);
  }

  if (g_Main.IsExcuteService())
  {
    _kLoggers[0].Write("Is not cash item : %s", record->m_strCsItemCode);
  }
  else
  {
    MyMessageBox("CashITemRemoteStore", "Is not cash item : %s", record->m_strCsItemCode);
  }
  return false;
}

void CashItemRemoteStore::Load_Conditional_Event()
{
  m_con_event.m_conevent_log.Write("Loading Conditional Event");
  m_con_event.m_conevent_status = 0;
  Set_Conditional_Evnet_Status(0);
  load_con_event_ini(&m_con_event.m_ini, &m_con_event.m_conevent_ini_file_time);
  m_con_event.m_conevent_log.Write("Complete For Conditional Event");
}

unsigned __int8 CashItemRemoteStore::Get_Conditional_Event_Status()
{
  return m_con_event.m_conevent_status;
}

void CashItemRemoteStore::Get_Conditional_Event_Name(unsigned __int8 byEventType, char *szEventName)
{
  if (byEventType)
  {
    if (byEventType == 1)
    {
      wsprintfA(szEventName, "%s", "Set");
    }
    else if (byEventType == 2)
    {
      wsprintfA(szEventName, "%s", "One&One");
    }
    else
    {
      wsprintfA(szEventName, "%s", "NULL");
    }
  }
  else
  {
    wsprintfA(szEventName, "%s", "Discount");
  }
}

bool CashItemRemoteStore::Check_CashEvent_INI(unsigned __int8 byEventType)
{
  _cash_event_ini newIni{};
  _FILETIME newFileTime{};
  char eventName[260]{};
  Get_CashEvent_Name(byEventType, eventName);
  Load_Event_INI(&newIni, &newFileTime, eventName);
  if (newFileTime.dwHighDateTime == m_cash_event[byEventType].m_event_ini_file_time.dwHighDateTime
      && newFileTime.dwLowDateTime == m_cash_event[byEventType].m_event_ini_file_time.dwLowDateTime)
  {
    return false;
  }

  m_cash_event[byEventType].m_event_ini_file_time = newFileTime;
  m_cash_event[byEventType].m_event_log.Write("Event INI File Changed (Server Runing)");

  unsigned __int8 status = 0;
  __time32_t now[4]{};
  _time32(now);
  if (newIni.m_bUseCashEvent)
  {
    if (now[0] >= newIni.m_EventTime[0])
    {
      if (now[0] > newIni.m_EventTime[1])
      {
        status = 5;
        if (m_cash_event[byEventType].m_ini.m_bUseCashEvent && IsEventTime(byEventType))
        {
          Inform_CashEvent_Status_All(byEventType, status, &newIni);
        }
      }
      else
      {
        const unsigned int remain = static_cast<unsigned int>(newIni.m_EventTime[1] - now[0]);
        if (newIni.m_EventTime[1] == now[0])
        {
          status = 0;
        }
        else if (remain && remain <= static_cast<unsigned int>(m_cash_event[byEventType].m_event_inform_before[1]))
        {
          status = 4;
        }
        else if (
          remain <= static_cast<unsigned int>(m_cash_event[byEventType].m_event_inform_before[1])
          || remain > static_cast<unsigned int>(m_cash_event[byEventType].m_event_inform_before[0]))
        {
          status = static_cast<unsigned __int8>(
            remain > static_cast<unsigned int>(m_cash_event[byEventType].m_event_inform_before[0]));
        }
        else
        {
          status = 3;
        }

        if (m_cash_event[byEventType].m_ini.m_bUseCashEvent && IsEventTime(byEventType) && status)
        {
          Inform_CashEvent_Status_All(byEventType, status, &newIni);
        }
      }
    }
    else
    {
      status = 1;
      if (m_cash_event[byEventType].m_ini.m_bUseCashEvent && IsEventTime(byEventType))
      {
        Inform_CashEvent_Status_All(byEventType, status, &newIni);
      }
    }
  }
  else if (m_cash_event[byEventType].m_ini.m_bUseCashEvent && byEventType == 0)
  {
    status = 5;
    Inform_CashEvent_Status_All(0, 5, &newIni);
  }

  Set_CashEvent_Status(byEventType, status);
  Update_INI(&newIni, byEventType);
  return true;
}

void CashItemRemoteStore::Update_INI(_cash_event_ini *pNewIni, unsigned __int8 byEventType)
{
  if (!pNewIni)
  {
    return;
  }

  m_cash_event[byEventType].m_event_log.Write(
    "Updated INI File : [%d/%d/%d %d:%d ~ %d/%d/%d %d:%d] -> [%d/%d/%d %d:%d ~ %d/%d/%d %d:%d",
    m_cash_event[byEventType].m_ini.m_wYear[0],
    m_cash_event[byEventType].m_ini.m_byMonth[0],
    m_cash_event[byEventType].m_ini.m_byDay[0],
    m_cash_event[byEventType].m_ini.m_byHour[0],
    m_cash_event[byEventType].m_ini.m_byMinute[0],
    m_cash_event[byEventType].m_ini.m_wYear[1],
    m_cash_event[byEventType].m_ini.m_byMonth[1],
    m_cash_event[byEventType].m_ini.m_byDay[1],
    m_cash_event[byEventType].m_ini.m_byHour[1],
    m_cash_event[byEventType].m_ini.m_byMinute[1],
    pNewIni->m_wYear[0],
    pNewIni->m_byMonth[0],
    pNewIni->m_byDay[0],
    pNewIni->m_byHour[0],
    pNewIni->m_byMinute[0],
    pNewIni->m_wYear[1],
    pNewIni->m_byMonth[1],
    pNewIni->m_byDay[1],
    pNewIni->m_byHour[1],
    pNewIni->m_byMinute[1]);

  m_cash_event[byEventType].m_event_log.Write(
    "Repeat Information >> Repeat:%d , RepeatDay:%d",
    m_cash_event[byEventType].m_ini.m_bRepeat,
    m_cash_event[byEventType].m_ini.m_byRepeatDay);
  m_cash_event[byEventType].m_event_log.Write(
    "Expire Date >> [%d/%d/%d %d:%d]",
    m_cash_event[byEventType].m_ini.m_wYear[2],
    m_cash_event[byEventType].m_ini.m_byMonth[2],
    m_cash_event[byEventType].m_ini.m_byDay[2],
    m_cash_event[byEventType].m_ini.m_byHour[2],
    m_cash_event[byEventType].m_ini.m_byMinute[2]);

  if (m_cash_event[byEventType].m_ini.m_bUseCashEvent != pNewIni->m_bUseCashEvent)
  {
    m_cash_event[byEventType].m_event_log.Write(
      "Updated Use of INI File : [%d -> %d]",
      m_cash_event[byEventType].m_ini.m_bUseCashEvent,
      pNewIni->m_bUseCashEvent);
  }

  m_cash_event[byEventType].m_ini.m_bUseCashEvent = pNewIni->m_bUseCashEvent;
  m_cash_event[byEventType].m_ini.m_bRepeat = pNewIni->m_bRepeat;
  m_cash_event[byEventType].m_ini.m_byRepeatDay = pNewIni->m_byRepeatDay;
  for (int j = 0; j < 3; ++j)
  {
    m_cash_event[byEventType].m_ini.m_wYear[j] = pNewIni->m_wYear[j];
    m_cash_event[byEventType].m_ini.m_byMonth[j] = pNewIni->m_byMonth[j];
    m_cash_event[byEventType].m_ini.m_byDay[j] = pNewIni->m_byDay[j];
    m_cash_event[byEventType].m_ini.m_byHour[j] = pNewIni->m_byHour[j];
    m_cash_event[byEventType].m_ini.m_byMinute[j] = pNewIni->m_byMinute[j];
    m_cash_event[byEventType].m_ini.m_EventTime[j] = pNewIni->m_EventTime[j];
  }

  if (byEventType == 0)
  {
    m_cash_event[0].m_ini.m_byDiscout[0] = pNewIni->m_byDiscout[0];
    m_cash_event[0].m_ini.m_byDiscout[1] = pNewIni->m_byDiscout[1];
    m_cash_event[0].m_ini.m_byDiscout[2] = pNewIni->m_byDiscout[2];
    m_cash_event[0].m_ini.m_byDiscout[3] = pNewIni->m_byDiscout[3];
  }
  SetNextEventTime(byEventType);
}

void CashItemRemoteStore::GetEvnetTime(_cash_event_time *pETime, int itime)
{
  __time32_t now[5]{};
  _time32(now);
  std::tm *tmValue = _localtime32(now);
  tmValue->tm_min += 5;
  pETime->m_EventTime[0] = _mktime32(tmValue);
  __time32_t firstEventTime = static_cast<__time32_t>(pETime->m_EventTime[0]);
  tmValue = _localtime32(&firstEventTime);
  pETime->m_nYear[0] = tmValue->tm_year;
  pETime->m_nMonth[0] = tmValue->tm_mon;
  pETime->m_nDay[0] = tmValue->tm_mday;
  pETime->m_nHour[0] = tmValue->tm_hour;
  pETime->m_nMinute[0] = tmValue->tm_min;
  tmValue->tm_min += itime;
  pETime->m_EventTime[1] = _mktime32(tmValue);
  __time32_t secondEventTime = static_cast<__time32_t>(pETime->m_EventTime[1]);
  tmValue = _localtime32(&secondEventTime);
  pETime->m_nYear[1] = tmValue->tm_year;
  pETime->m_nMonth[1] = tmValue->tm_mon;
  pETime->m_nDay[1] = tmValue->tm_mday;
  pETime->m_nHour[1] = tmValue->tm_hour;
  pETime->m_nMinute[1] = tmValue->tm_min;
}

bool CashItemRemoteStore::isConEventTime()
{
  if (!m_con_event.m_ini.m_bUseConEvent)
  {
    return false;
  }

  __time32_t now[9]{};
  _time32(now);
  return m_con_event.m_eventtime.m_EventTime[0] <= now[0] && m_con_event.m_eventtime.m_EventTime[1] >= now[0];
}

void CashItemRemoteStore::load_con_event_ini(_con_event_ini *pIni, _FILETIME *pft)
{
  if (!pIni || !pft)
  {
    return;
  }

  pIni->m_bUseConEvent = 0;
  if (GetLastWriteFileTime("./initialize/condistional_event.ini", pft))
  {
    int useFlag = GetPrivateProfileIntA(
      "CONDITIONAL_EVENT",
      "USE",
      1,
      "./initialize/condistional_event.ini");
    if (useFlag == 1)
    {
      pIni->m_bUseConEvent = 0;
    }
    else
    {
      pIni->m_bUseConEvent = 1;
      useFlag = GetPrivateProfileIntA(
        "CONDITIONAL_EVENT",
        "CHSH_MIN",
        0,
        "./initialize/condistional_event.ini");
      if (useFlag)
      {
        pIni->m_dwCashMin = 10000 * useFlag;
        useFlag = GetPrivateProfileIntA(
          "CONDITIONAL_EVENT",
          "EVENT_TIME",
          0,
          "./initialize/condistional_event.ini");
        if (useFlag)
        {
          pIni->m_iEventTime = useFlag;
          useFlag = GetPrivateProfileIntA(
            "CONDITIONAL_EVENT",
            "EVENT_KIND",
            -1,
            "./initialize/condistional_event.ini");
          pIni->m_byEventKind = static_cast<unsigned __int8>(useFlag);
          GetPrivateProfileStringA(
            "CONDITIONAL_EVENT",
            "EVENT_START_MSG",
            "FALSE",
            pIni->m_szStartMsg,
            8,
            "./initialize/condistional_event.ini");
          if (!std::strcmp(pIni->m_szStartMsg, "FALSE"))
          {
            pIni->m_bUseConEvent = 0;
          }
          else
          {
            GetPrivateProfileStringA(
              "CONDITIONAL_EVENT",
              "EVENT_MIDDLE_MSG",
              "FALSE",
              pIni->m_szMiddletMsg,
              8,
              "./initialize/condistional_event.ini");
            if (!std::strcmp(pIni->m_szMiddletMsg, "FALSE"))
            {
              pIni->m_bUseConEvent = 0;
            }
            else
            {
              GetPrivateProfileStringA(
                "CONDITIONAL_EVENT",
                "EVENT_END_MSG",
                "FALSE",
                pIni->m_szEndMsg,
                8,
                "./initialize/condistional_event.ini");
              if (!std::strcmp(pIni->m_szEndMsg, "FALSE"))
              {
                pIni->m_bUseConEvent = 0;
              }
            }
          }
        }
        else
        {
          pIni->m_bUseConEvent = 0;
        }
      }
      else
      {
        pIni->m_bUseConEvent = 0;
      }
    }
  }
}

void CashItemRemoteStore::Check_Conditional_Event_INI()
{
  _con_event_ini newIni{};
  _FILETIME newFileTime{};
  load_con_event_ini(&newIni, &newFileTime);
  if (newFileTime.dwHighDateTime != m_con_event.m_conevent_ini_file_time.dwHighDateTime
      || newFileTime.dwLowDateTime != m_con_event.m_conevent_ini_file_time.dwLowDateTime)
  {
    m_con_event.m_conevent_ini_file_time = newFileTime;
    if (m_con_event.m_ini.m_bUseConEvent && isConEventTime())
    {
      m_con_event.m_conevent_log.Write("The New Ini File was disregarded. Other Event is on");
    }
    else
    {
      m_con_event.m_ini.m_bUseConEvent = newIni.m_bUseConEvent;
      m_con_event.m_ini.m_byEventKind = newIni.m_byEventKind;
      m_con_event.m_ini.m_dwCashMin = newIni.m_dwCashMin;
      m_con_event.m_ini.m_iEventTime = newIni.m_iEventTime;
      std::strcpy(m_con_event.m_ini.m_szStartMsg, newIni.m_szStartMsg);
      std::strcpy(m_con_event.m_ini.m_szMiddletMsg, newIni.m_szMiddletMsg);
      std::strcpy(m_con_event.m_ini.m_szEndMsg, newIni.m_szEndMsg);
    }
  }
}

void CashItemRemoteStore::Inform_ConditionalEvent_Status_All(
  unsigned __int8 byEventType,
  unsigned __int8 byStatus,
  char *pszMsg)
{
  for (unsigned __int16 j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bOper && player->m_bLive)
    {
      unsigned __int16 discount = 0;
      if (byEventType == 2)
      {
        discount = m_cde.m_ini.m_wCsDiscount;
      }
      ICsSendInterface::SendMsg_ConditionalEventInform(j, byEventType, discount, byStatus, pszMsg);
    }
  }

  char eventName[80]{};
  Get_Conditional_Event_Name(byEventType, eventName);
  m_con_event.m_conevent_log.Write(
    "[ %s Conditional Event Inform when Event status change] [EventState : %d] [EventTime : %d/%d/%d %d:%d  ~ %d/%d/%d %d:%d ]",
    eventName,
    byStatus,
    m_con_event.m_eventtime.m_nYear[0] + 1900,
    m_con_event.m_eventtime.m_nMonth[0] + 1,
    m_con_event.m_eventtime.m_nDay[0],
    m_con_event.m_eventtime.m_nHour[0],
    m_con_event.m_eventtime.m_nMinute[0],
    m_con_event.m_eventtime.m_nYear[1] + 1900,
    m_con_event.m_eventtime.m_nMonth[1] + 1,
    m_con_event.m_eventtime.m_nDay[1],
    m_con_event.m_eventtime.m_nHour[1],
    m_con_event.m_eventtime.m_nMinute[1]);
}

void CashItemRemoteStore::Check_Conditional_Event_Status()
{
  const unsigned __int8 conditionalStatus = Get_Conditional_Event_Status();

  __time32_t now[4]{};
  _time32(now);
  if (m_cde.m_ini.m_cdeTime[0] > now[0] && m_cde.m_ini.m_cdeTime[0] - now[0] < 300 && m_con_event.m_bConEvent)
  {
    Set_Conditional_Evnet_Status(0);
    Inform_ConditionalEvent_Status_All(m_con_event.m_ini.m_byEventKind, 4, m_con_event.m_ini.m_szEndMsg);
    m_con_event.m_bConEvent = false;
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (m_cash_event[j].m_ini.m_EventTime[0] > now[0]
        && m_cash_event[j].m_ini.m_EventTime[0] - now[0] < 300
        && m_con_event.m_bConEvent)
    {
      Set_Conditional_Evnet_Status(0);
      Inform_ConditionalEvent_Status_All(m_con_event.m_ini.m_byEventKind, 4, m_con_event.m_ini.m_szEndMsg);
      m_con_event.m_bConEvent = false;
      return;
    }
  }

  if (conditionalStatus == 1)
  {
    if (now[0] >= m_con_event.m_eventtime.m_EventTime[0])
    {
      Set_Conditional_Evnet_Status(2);
      Inform_ConditionalEvent_Status_All(m_con_event.m_ini.m_byEventKind, 2, m_con_event.m_ini.m_szStartMsg);
    }
  }
  else if (conditionalStatus == 2)
  {
    const int halfDuration = (m_con_event.m_eventtime.m_EventTime[1] - m_con_event.m_eventtime.m_EventTime[0]) / 2;
    if (halfDuration >= m_con_event.m_eventtime.m_EventTime[1] - now[0])
    {
      Set_Conditional_Evnet_Status(3);
      Inform_ConditionalEvent_Status_All(m_con_event.m_ini.m_byEventKind, 3, m_con_event.m_ini.m_szMiddletMsg);
    }
  }
  else if (conditionalStatus == 3 && m_con_event.m_eventtime.m_EventTime[1] <= now[0])
  {
    Set_Conditional_Evnet_Status(0);
    Inform_ConditionalEvent_Status_All(m_con_event.m_ini.m_byEventKind, 4, m_con_event.m_ini.m_szEndMsg);
    m_con_event.m_bConEvent = false;
  }
}

void CashItemRemoteStore::Inform_ConditionalEvent(unsigned __int16 wIndex)
{
  if (m_con_event.m_ini.m_bUseConEvent && isConEventTime())
  {
    unsigned __int16 discount = 0;
    if (m_con_event.m_ini.m_byEventKind == 2)
    {
      discount = m_cde.m_ini.m_wCsDiscount;
    }

    switch (m_con_event.m_conevent_status)
    {
      case 2:
        ICsSendInterface::SendMsg_ConditionalEventInform(
          wIndex,
          m_con_event.m_ini.m_byEventKind,
          discount,
          m_con_event.m_conevent_status,
          m_con_event.m_ini.m_szStartMsg);
        break;

      case 3:
        ICsSendInterface::SendMsg_ConditionalEventInform(
          wIndex,
          m_con_event.m_ini.m_byEventKind,
          discount,
          m_con_event.m_conevent_status,
          m_con_event.m_ini.m_szMiddletMsg);
        break;

      case 4:
        ICsSendInterface::SendMsg_ConditionalEventInform(
          wIndex,
          m_con_event.m_ini.m_byEventKind,
          0,
          m_con_event.m_conevent_status,
          m_con_event.m_ini.m_szEndMsg);
        break;

      default:
        break;
    }
  }
}

void CashItemRemoteStore::Change_Conditional_Event_Status()
{
  __time32_t now[4]{};
  _time32(now);

  if (!m_con_event.m_ini.m_bUseConEvent)
  {
    Set_Conditional_Evnet_Status(0);
  }

  if (m_con_event.m_eventtime.m_EventTime[1] >= now[0])
  {
    if (m_con_event.m_eventtime.m_EventTime[0] > now[0] || m_con_event.m_eventtime.m_EventTime[1] < now[0])
    {
      if (m_con_event.m_eventtime.m_EventTime[0] > now[0])
      {
        Set_Conditional_Evnet_Status(1);
      }
    }
    else if (m_con_event.m_eventtime.m_EventTime[1] > now[0])
    {
      if (now[0] - m_con_event.m_eventtime.m_EventTime[0] <= 5 || m_con_event.m_eventtime.m_EventTime[1] <= now[0])
      {
        if (now[0] - m_con_event.m_eventtime.m_EventTime[0] <= 5)
        {
          Set_Conditional_Evnet_Status(2);
        }
      }
      else
      {
        Set_Conditional_Evnet_Status(3);
      }
    }
    else
    {
      Set_Conditional_Evnet_Status(4);
      m_con_event.m_bConEvent = false;
    }
  }
  else
  {
    Set_Conditional_Evnet_Status(0);
  }
}

void CashItemRemoteStore::Set_Conditional_Evnet_Status(unsigned __int8 byStatus)
{
  const int currentStatus = m_con_event.m_conevent_status;
  m_con_event.m_conevent_log.Write("[EventStateChange : %d -> %d ]", currentStatus, byStatus);
  m_con_event.m_conevent_status = byStatus;
}

void CashItemRemoteStore::load_cash_discount_event()
{
  m_cde.m_cde_log.Write("Loading Cash Discount-Rate Event");
  m_cde.m_cde_timer.BeginTimer(1000);
  m_cde.m_cde_inform_before[0] = 1800;
  m_cde.m_cde_inform_before[1] = 300;
  m_cde.m_cde_status = 0;
  load_cde_ini(&m_cde.m_ini, &m_cde.m_cde_ini_file_time);
  log_about_cash_event("Loaded From Ini File <When Server Started>", &m_cde.m_ini);
  check_loaded_cde_status();
  m_cde.m_cde_log.Write("Complete For Cash Discount-Rate Event");
}

void CashItemRemoteStore::load_cde_ini(_cash_discount_ini_ *pIni, _FILETIME *pft)
{
  if (!pIni || !pft)
  {
    return;
  }

  pIni->m_bUseCashDiscount = 0;
  pIni->m_cdeTime[0] = 0;
  pIni->m_cdeTime[1] = 0;
  pIni->m_bRepeat = 0;

  _FILETIME ftWrite{};
  if (GetLastWriteFileTime("./initialize/cash_discount.ini", &ftWrite))
  {
    *pft = ftWrite;
    int value = GetPrivateProfileIntA("Cash", "USE", 1, "./initialize/cash_discount.ini");
    if (value == 1)
    {
      pIni->m_bUseCashDiscount = 0;
    }
    else
    {
      pIni->m_bUseCashDiscount = 1;
      pIni->m_bRepeat = GetPrivateProfileIntA("Cash", "REPEAT", 0, "./initialize/cash_discount.ini");
      value = GetPrivateProfileIntA("Cash", "REPEAT_DAY", 0, "./initialize/cash_discount.ini");
      if (value > 0)
      {
        pIni->m_byRepeatDay = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      pIni->m_wCsDiscount =
        static_cast<unsigned __int16>(GetPrivateProfileIntA("Cash", "SALE", 0, "./initialize/cash_discount.ini"));
      value = GetPrivateProfileIntA("Cash", "BEGIN_YEAR", 0, "./initialize/cash_discount.ini");
      if (value)
      {
        pIni->m_wYear[0] = static_cast<unsigned __int16>(value);
        value = GetPrivateProfileIntA("Cash", "BEGIN_MONTH", 0, "./initialize/cash_discount.ini");
        if (value)
        {
          pIni->m_byMonth[0] = static_cast<unsigned __int8>(value);
          value = GetPrivateProfileIntA("Cash", "BEGIN_DAY", 0, "./initialize/cash_discount.ini");
          if (value)
          {
            pIni->m_byDay[0] = static_cast<unsigned __int8>(value);
            value = GetPrivateProfileIntA("Cash", "BEGIN_HOUR", 0, "./initialize/cash_discount.ini");
            if (static_cast<unsigned int>(value) < 24)
            {
              pIni->m_byHour[0] = static_cast<unsigned __int8>(value);
              value =
                GetPrivateProfileIntA("Cash", "BEGIN_MINUTE", 0, "./initialize/cash_discount.ini");
              if (static_cast<unsigned int>(value) < 60)
              {
                pIni->m_byMinute[0] = static_cast<unsigned __int8>(value);
                std::tm tmValue{};
                tmValue.tm_year = pIni->m_wYear[0] - 1900;
                tmValue.tm_mon = pIni->m_byMonth[0] - 1;
                tmValue.tm_mday = pIni->m_byDay[0];
                tmValue.tm_hour = pIni->m_byHour[0];
                tmValue.tm_min = pIni->m_byMinute[0];
                tmValue.tm_sec = 0;
                tmValue.tm_isdst = -1;
                pIni->m_cdeTime[0] = _mktime32(&tmValue);
                if (pIni->m_cdeTime[0] == -1)
                {
                  pIni->m_bUseCashDiscount = 0;
                }
                else
                {
                  value = GetPrivateProfileIntA("Cash", "END_YEAR", 0, "./initialize/cash_discount.ini");
                  if (value)
                  {
                    pIni->m_wYear[1] = static_cast<unsigned __int16>(value);
                    value =
                      GetPrivateProfileIntA("Cash", "END_MONTH", 0, "./initialize/cash_discount.ini");
                    if (value)
                    {
                      pIni->m_byMonth[1] = static_cast<unsigned __int8>(value);
                      value =
                        GetPrivateProfileIntA("Cash", "END_DAY", 0, "./initialize/cash_discount.ini");
                      if (value)
                      {
                        pIni->m_byDay[1] = static_cast<unsigned __int8>(value);
                        value = GetPrivateProfileIntA(
                          "Cash",
                          "END_HOUR",
                          0,
                          "./initialize/cash_discount.ini");
                        if (static_cast<unsigned int>(value) < 24)
                        {
                          pIni->m_byHour[1] = static_cast<unsigned __int8>(value);
                          value = GetPrivateProfileIntA(
                            "Cash",
                            "END_MINUTE",
                            0,
                            "./initialize/cash_discount.ini");
                          if (static_cast<unsigned int>(value) < 60)
                          {
                            pIni->m_byMinute[1] = static_cast<unsigned __int8>(value);
                            std::memset(&tmValue, 0, sizeof(tmValue));
                            tmValue.tm_year = pIni->m_wYear[1] - 1900;
                            tmValue.tm_mon = pIni->m_byMonth[1] - 1;
                            tmValue.tm_mday = pIni->m_byDay[1];
                            tmValue.tm_hour = pIni->m_byHour[1];
                            tmValue.tm_min = pIni->m_byMinute[1];
                            tmValue.tm_sec = 0;
                            tmValue.tm_isdst = -1;
                            pIni->m_cdeTime[1] = _mktime32(&tmValue);
                            if (pIni->m_cdeTime[1] == -1)
                            {
                              pIni->m_bUseCashDiscount = 0;
                            }
                            else
                            {
                              value =
                                GetPrivateProfileIntA("Cash", "EXPIRE_YEAR", 0, "./initialize/cash_discount.ini");
                              if (value > 0)
                              {
                                pIni->m_wYear[2] = static_cast<unsigned __int16>(value);
                              }
                              else
                              {
                                pIni->m_bRepeat = 0;
                              }
                              value = GetPrivateProfileIntA(
                                "Cash",
                                "EXPIRE_MONTH",
                                0,
                                "./initialize/cash_discount.ini");
                              if (value > 0)
                              {
                                pIni->m_byMonth[2] = static_cast<unsigned __int8>(value);
                              }
                              else
                              {
                                pIni->m_bRepeat = 0;
                              }
                              value = GetPrivateProfileIntA(
                                "Cash",
                                "EXPIRE_DAY",
                                0,
                                "./initialize/cash_discount.ini");
                              if (value > 0)
                              {
                                pIni->m_byDay[2] = static_cast<unsigned __int8>(value);
                              }
                              else
                              {
                                pIni->m_bRepeat = 0;
                              }
                              value = GetPrivateProfileIntA(
                                "Cash",
                                "EXPIRE_HOUR",
                                0,
                                "./initialize/cash_discount.ini");
                              if (static_cast<unsigned int>(value) < 24)
                              {
                                pIni->m_byHour[2] = static_cast<unsigned __int8>(value);
                              }
                              else
                              {
                                pIni->m_bRepeat = 0;
                              }
                              value = GetPrivateProfileIntA(
                                "Cash",
                                "EXPIRE_MINUTE",
                                0,
                                "./initialize/cash_discount.ini");
                              if (static_cast<unsigned int>(value) < 60)
                              {
                                pIni->m_byMinute[2] = static_cast<unsigned __int8>(value);
                              }
                              else
                              {
                                pIni->m_bRepeat = 0;
                              }
                              std::memset(&tmValue, 0, sizeof(tmValue));
                              tmValue.tm_year = pIni->m_wYear[2] - 1900;
                              tmValue.tm_mon = pIni->m_byMonth[2] - 1;
                              tmValue.tm_mday = pIni->m_byDay[2];
                              tmValue.tm_hour = pIni->m_byHour[2];
                              tmValue.tm_min = pIni->m_byMinute[2];
                              tmValue.tm_sec = 0;
                              tmValue.tm_isdst = -1;
                              pIni->m_cdeTime[2] = _mktime32(&tmValue);
                              if (pIni->m_cdeTime[2] == -1)
                              {
                                pIni->m_bRepeat = 0;
                              }
                            }
                          }
                          else
                          {
                            pIni->m_bUseCashDiscount = 0;
                          }
                        }
                        else
                        {
                          pIni->m_bUseCashDiscount = 0;
                        }
                      }
                      else
                      {
                        pIni->m_bUseCashDiscount = 0;
                      }
                    }
                    else
                    {
                      pIni->m_bUseCashDiscount = 0;
                    }
                  }
                  else
                  {
                    pIni->m_bUseCashDiscount = 0;
                  }
                }
              }
              else
              {
                pIni->m_bUseCashDiscount = 0;
              }
            }
            else
            {
              pIni->m_bUseCashDiscount = 0;
            }
          }
          else
          {
            pIni->m_bUseCashDiscount = 0;
          }
        }
        else
        {
          pIni->m_bUseCashDiscount = 0;
        }
      }
      else
      {
        pIni->m_bUseCashDiscount = 0;
      }
    }
  }
}

void CashItemRemoteStore::log_about_cash_event(const char *szLoadInfo, _cash_discount_ini_ *pIni)
{
  m_cde.m_cde_log.Write("\t##Begin <%s>", szLoadInfo);
  m_cde.m_cde_log.Write("\t\tStatus : %d", get_cde_status());
  m_cde.m_cde_log.Write("\t\tRepeat : %d", pIni->m_bRepeat);
  m_cde.m_cde_log.Write("\t\tRepeatDay : %d", pIni->m_byRepeatDay);
  if (m_cde.m_ini.m_bUseCashDiscount)
  {
    m_cde.m_cde_log.Write(
      "\t\tStart : %04d-%02d-%02d %02d:%02d",
      pIni->m_wYear[0],
      pIni->m_byMonth[0],
      pIni->m_byDay[0],
      pIni->m_byHour[0],
      pIni->m_byMinute[0]);
    m_cde.m_cde_log.Write(
      "\t\tEnd : %04d-%02d-%02d %02d:%02d",
      pIni->m_wYear[1],
      pIni->m_byMonth[1],
      pIni->m_byDay[1],
      pIni->m_byHour[1],
      pIni->m_byMinute[1]);
    m_cde.m_cde_log.Write(
      "\t\tExpire : %04d-%02d-%02d %02d:%02d",
      pIni->m_wYear[2],
      pIni->m_byMonth[2],
      pIni->m_byDay[2],
      pIni->m_byHour[2],
      pIni->m_byMinute[2]);
  }
  m_cde.m_cde_log.Write("\t##End <Information about loaded Cash Discount-Rate Event>");
}

bool CashItemRemoteStore::ChangeDiscountEventTime()
{
  if (!m_cde.m_ini.m_bRepeat)
  {
    return false;
  }

  std::tm tmValue{};
  tmValue.tm_year = m_cde.m_ini.m_wYear[0] - 1900;
  tmValue.tm_mon = m_cde.m_ini.m_byMonth[0] - 1;
  tmValue.tm_mday = m_cde.m_ini.m_byDay[0] + m_cde.m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cde.m_ini.m_byHour[0];
  tmValue.tm_min = m_cde.m_ini.m_byMinute[0];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cde.m_ini.m_cdeTime[0] = _mktime32(&tmValue);
  if (m_cde.m_ini.m_cdeTime[0] == -1)
  {
    m_cde.m_ini.m_bRepeat = 0;
    m_cde.m_cde_log.Write("ChangeDiscountEventTime() : Fail When Calculate Discount Event Begin Time");
    return false;
  }
  m_cde.m_ini.m_wYear[0] = tmValue.tm_year + 1900;
  m_cde.m_ini.m_byMonth[0] = tmValue.tm_mon + 1;
  m_cde.m_ini.m_byDay[0] = tmValue.tm_mday;
  m_cde.m_ini.m_byHour[0] = tmValue.tm_hour;
  m_cde.m_ini.m_byMinute[0] = tmValue.tm_min;

  std::memset(&tmValue, 0, sizeof(tmValue));
  tmValue.tm_year = m_cde.m_ini.m_wYear[1] - 1900;
  tmValue.tm_mon = m_cde.m_ini.m_byMonth[1] - 1;
  tmValue.tm_mday = m_cde.m_ini.m_byDay[1] + m_cde.m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cde.m_ini.m_byHour[1];
  tmValue.tm_min = m_cde.m_ini.m_byMinute[1];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cde.m_ini.m_cdeTime[1] = _mktime32(&tmValue);
  if (m_cde.m_ini.m_cdeTime[1] == -1)
  {
    m_cde.m_ini.m_bRepeat = 0;
    m_cde.m_cde_log.Write("ChangeDiscountEventTime() : Fail When Calculate Discount Event End Time");
    return false;
  }
  m_cde.m_ini.m_wYear[1] = tmValue.tm_year + 1900;
  m_cde.m_ini.m_byMonth[1] = tmValue.tm_mon + 1;
  m_cde.m_ini.m_byDay[1] = tmValue.tm_mday;
  m_cde.m_ini.m_byHour[1] = tmValue.tm_hour;
  m_cde.m_ini.m_byMinute[1] = tmValue.tm_min;
  return true;
}

void CashItemRemoteStore::check_cash_discount_ini()
{
  _cash_discount_ini_ newIni{};
  _FILETIME newFileTime{};
  load_cde_ini(&newIni, &newFileTime);

  if (m_cde.m_ini.m_wCsDiscount != newIni.m_wCsDiscount)
  {
    m_cde.m_ini.m_wCsDiscount = newIni.m_wCsDiscount;
  }

  if (newFileTime.dwHighDateTime != m_cde.m_cde_ini_file_time.dwHighDateTime
      || newFileTime.dwLowDateTime != m_cde.m_cde_ini_file_time.dwLowDateTime)
  {
    m_cde.m_cde_ini_file_time = newFileTime;
    log_about_cash_event("Loaded From Ini File <When Server Running>", &newIni);
    if (m_cde.m_ini.m_bUseCashDiscount && is_cde_time())
    {
      m_cde.m_cde_log.Write("The New Ini File was disregarded. Other Event is on");
    }
    else
    {
      unsigned __int8 status = 0;
      __time32_t now[4]{};
      _time32(now);
      if (newIni.m_bUseCashDiscount)
      {
        if (now[0] >= newIni.m_cdeTime[0])
        {
          if (now[0] > newIni.m_cdeTime[1])
          {
            status = 5;
          }
          else
          {
            const unsigned int remain = static_cast<unsigned int>(newIni.m_cdeTime[1] - now[0]);
            if (newIni.m_cdeTime[1] == now[0])
            {
              status = 0;
            }
            else if (remain && remain <= static_cast<unsigned int>(m_cde.m_cde_inform_before[1]))
            {
              status = 4;
            }
            else if (
              remain <= static_cast<unsigned int>(m_cde.m_cde_inform_before[1])
              || remain > static_cast<unsigned int>(m_cde.m_cde_inform_before[0]))
            {
              if (remain <= static_cast<unsigned int>(m_cde.m_cde_inform_before[0]))
              {
                status = 0;
              }
              else
              {
                status = 2;
              }
            }
            else
            {
              status = 3;
            }
          }
        }
        else
        {
          status = 1;
        }
      }
      set_cde_status(status);
      update_ini(&newIni);
    }
  }
}

void CashItemRemoteStore::update_ini(_cash_discount_ini_ *pNewIni)
{
  if (!pNewIni)
  {
    return;
  }

  m_cde.m_ini.m_bUseCashDiscount = pNewIni->m_bUseCashDiscount;
  m_cde.m_ini.m_bRepeat = pNewIni->m_bRepeat;
  m_cde.m_ini.m_byRepeatDay = pNewIni->m_byRepeatDay;
  for (unsigned __int8 j = 0; j < 3; ++j)
  {
    m_cde.m_ini.m_wYear[j] = pNewIni->m_wYear[j];
    m_cde.m_ini.m_byMonth[j] = pNewIni->m_byMonth[j];
    m_cde.m_ini.m_byDay[j] = pNewIni->m_byDay[j];
    m_cde.m_ini.m_byHour[j] = pNewIni->m_byHour[j];
    m_cde.m_ini.m_byMinute[j] = pNewIni->m_byMinute[j];
    m_cde.m_ini.m_cdeTime[j] = pNewIni->m_cdeTime[j];
  }
  SetNextDiscountEventTime();
}

void CashItemRemoteStore::check_loaded_cde_status()
{
  if (!m_cde.m_ini.m_bUseCashDiscount)
  {
    set_cde_status(0);
    return;
  }

  __time32_t now[4];
  _time32(now);

  if (now[0] > m_cde.m_ini.m_cdeTime[1])
  {
    set_cde_status(0);
  }
  else if (now[0] < m_cde.m_ini.m_cdeTime[0] || now[0] > m_cde.m_ini.m_cdeTime[1])
  {
    if (now[0] < m_cde.m_ini.m_cdeTime[0])
    {
      set_cde_status(1);
    }
  }
  else
  {
    const int remainTime = m_cde.m_ini.m_cdeTime[1] - now[0];
    if (remainTime > 0)
    {
      if (remainTime > m_cde.m_cde_inform_before[1])
      {
        if (remainTime > m_cde.m_cde_inform_before[0])
        {
          set_cde_status(2);
        }
        else
        {
          set_cde_status(3);
        }
      }
      else
      {
        set_cde_status(4);
      }
    }
    else
    {
      set_cde_status(0);
    }
  }

  SetNextDiscountEventTime();
}

void CashItemRemoteStore::check_cash_discount_status()
{
  const unsigned __int8 currentStatus = get_cde_status();
  m_cde.m_ini.m_bCoEvent = 0;
  if (m_con_event.m_ini.m_byEventKind == 2 && m_con_event.m_bConEvent)
  {
    m_cde.m_ini.m_bCoEvent = 1;
  }

  __time32_t now[4]{};
  _time32(now);
  switch (currentStatus)
  {
    case 0:
    {
      if (!m_cde.m_ini.m_bUseCashDiscount || !m_cde.m_ini.m_bRepeat)
      {
        set_cde_status(5);
        break;
      }
      const bool changed = ChangeDiscountEventTime() != 0;
      if (m_cde.m_ini.m_NextEventTime[1] <= now[0] || !changed)
      {
        set_cde_status(5);
      }
      else
      {
        set_cde_status(1);
      }
      break;
    }

    case 1:
      if (m_cde.m_ini.m_cdeTime[2] > now[0])
      {
        if (now[0] >= m_cde.m_ini.m_cdeTime[0])
        {
          set_cde_status(2);
          inform_cashdiscount_status_all(2, &m_cde.m_ini);
        }
      }
      else
      {
        set_cde_status(5);
      }
      break;

    case 2:
      if (m_cde.m_ini.m_cdeTime[1] - now[0] <= m_cde.m_cde_inform_before[0])
      {
        set_cde_status(3);
        inform_cashdiscount_status_all(3, &m_cde.m_ini);
      }
      break;

    case 3:
      if (m_cde.m_ini.m_cdeTime[1] - now[0] <= m_cde.m_cde_inform_before[1])
      {
        set_cde_status(4);
        inform_cashdiscount_status_all(4, &m_cde.m_ini);
      }
      break;

    case 4:
      if (now[0] >= m_cde.m_ini.m_cdeTime[1])
      {
        set_cde_status(5);
        inform_cashdiscount_status_all(5, &m_cde.m_ini);
      }
      break;

    case 5:
      if (!m_cde.m_ini.m_bUseCashDiscount || !m_cde.m_ini.m_bRepeat)
      {
        set_cde_status(7);
        break;
      }
      if (m_cde.m_ini.m_cdeTime[2] <= now[0])
      {
        m_cde.m_ini.m_bUseCashDiscount = 0;
        m_cde.m_ini.m_bRepeat = 0;
        set_cde_status(7);
      }
      else if (SetNextDiscountEventTime())
      {
        set_cde_status(0);
      }
      else
      {
        set_cde_status(7);
      }
      break;

    default:
      break;
  }
}

void CashItemRemoteStore::inform_cashdiscount_event(unsigned __int16 widx)
{
  ICsSendInterface::SendMsg_CashDiscountEventInform(widx, m_cde.m_cde_status, &m_cde.m_ini);
}

void CashItemRemoteStore::inform_cashdiscount_status_all(unsigned __int8 byType, _cash_discount_ini_ *pIni)
{
  (void)pIni;
  for (unsigned __int16 j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive)
    {
      ICsSendInterface::SendMsg_CashDiscountEventInform(j, byType, &m_cde.m_ini);
    }
  }
}

void CashItemRemoteStore::force_endup_cash_discount_event()
{
  if (m_cde.m_ini.m_bUseCashDiscount)
  {
    set_cde_status(5);
    if (is_cde_time())
    {
      inform_cashdiscount_status_all(5, &m_cde.m_ini);
    }
    else
    {
      m_cde.m_ini.m_bUseCashDiscount = 0;
      m_cde.m_cde_log.Write("A Event which was Registated is ended up by perforce");
    }
  }
}

unsigned __int8 CashItemRemoteStore::get_cde_status()
{
  return m_cde.m_cde_status;
}

void CashItemRemoteStore::set_cde_status(unsigned __int8 byStatus)
{
  m_cde.m_cde_log.Write("Convert Status : %d -> %d", m_cde.m_cde_status, byStatus);
  if (byStatus == 7)
  {
    m_cde.m_cde_log.Write("[Discount Event Expired]");
  }
  m_cde.m_cde_status = byStatus;
}

bool CashItemRemoteStore::is_cde_time()
{
  if (!m_con_event.m_bConEvent && !m_cde.m_ini.m_bUseCashDiscount)
  {
    return false;
  }

  __time32_t now[4]{};
  _time32(now);

  if (m_con_event.m_bConEvent)
  {
    if (m_con_event.m_ini.m_byEventKind == 2
        && now[0] > m_con_event.m_eventtime.m_EventTime[0]
        && now[0] < m_con_event.m_eventtime.m_EventTime[1])
    {
      return true;
    }
  }
  else if (now[0] > m_cde.m_ini.m_cdeTime[0] && now[0] < m_cde.m_ini.m_cdeTime[1])
  {
    return true;
  }

  return false;
}

bool CashItemRemoteStore::IsEventTime(unsigned __int8 byEventType)
{
  if (!m_con_event.m_bConEvent && !m_cash_event[byEventType].m_ini.m_bUseCashEvent)
  {
    return false;
  }

  __time32_t now[4]{};
  _time32(now);

  if (m_con_event.m_bConEvent)
  {
    if (m_con_event.m_ini.m_byEventKind == byEventType
        && now[0] > m_con_event.m_eventtime.m_EventTime[0]
        && now[0] < m_con_event.m_eventtime.m_EventTime[1])
    {
      return true;
    }
  }
  else if (
    now[0] > m_cash_event[byEventType].m_ini.m_EventTime[0]
    && now[0] < m_cash_event[byEventType].m_ini.m_EventTime[1])
  {
    return true;
  }

  return false;
}

unsigned __int8 CashItemRemoteStore::GetSetDiscout(unsigned __int8 bySetKind)
{
  return m_cash_event[0].m_ini.m_byDiscout[bySetKind];
}

unsigned __int8 CashItemRemoteStore::GetLimDiscout()
{
  return m_cash_event[2].m_ini.m_byLimDiscout;
}

bool CashItemRemoteStore::SetNextDiscountEventTime()
{
  if (!m_cde.m_ini.m_bRepeat)
  {
    return false;
  }

  std::tm tmValue{};
  tmValue.tm_year = m_cde.m_ini.m_wYear[0] - 1900;
  tmValue.tm_mon = m_cde.m_ini.m_byMonth[0] - 1;
  tmValue.tm_mday = m_cde.m_ini.m_byDay[0] + m_cde.m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cde.m_ini.m_byHour[0];
  tmValue.tm_min = m_cde.m_ini.m_byMinute[0];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cde.m_ini.m_NextEventTime[0] = _mktime32(&tmValue);
  if (m_cde.m_ini.m_NextEventTime[0] == -1)
  {
    m_cde.m_ini.m_bRepeat = 0;
    m_cde.m_cde_log.Write(
      "SetNextDiscountEventTime() : Fail When Calculate Next Discount Event Begin Time");
    return false;
  }

  std::memset(&tmValue, 0, sizeof(tmValue));
  tmValue.tm_year = m_cde.m_ini.m_wYear[1] - 1900;
  tmValue.tm_mon = m_cde.m_ini.m_byMonth[1] - 1;
  tmValue.tm_mday = m_cde.m_ini.m_byDay[1] + m_cde.m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cde.m_ini.m_byHour[1];
  tmValue.tm_min = m_cde.m_ini.m_byMinute[1];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cde.m_ini.m_NextEventTime[1] = _mktime32(&tmValue);
  if (m_cde.m_ini.m_NextEventTime[1] == -1)
  {
    m_cde.m_ini.m_bRepeat = 0;
    m_cde.m_cde_log.Write("SetNextDiscountEventTime() : Fail When Calculate Next Discount Event End Time");
    return false;
  }

  return true;
}

void CashItemRemoteStore::Load_Cash_Event()
{
  char eventName[260];
  for (int j = 0; j < 3; ++j)
  {
    m_cash_event[j].m_event_timer.BeginTimer(1000);
    m_cash_event[j].m_event_inform_before[0] = 1800;
    m_cash_event[j].m_event_inform_before[1] = 300;
    m_cash_event[j].m_event_log.Write("Server Start and LogFile Loaing");
    Set_CashEvent_Status(static_cast<unsigned __int8>(j), 0);
    Get_CashEvent_Name(static_cast<unsigned __int8>(j), eventName);
    Load_Event_INI(&m_cash_event[j].m_ini, &m_cash_event[j].m_event_ini_file_time, eventName);
    if (j == 2)
    {
      Load_LimitedSale_Event_INI(
        &m_cash_event[2].m_ini,
        &m_cash_event[2].m_event_ini_file_time,
        eventName);
    }
    Check_Loaded_Event_Status(static_cast<unsigned __int8>(j));
  }
}

void CashItemRemoteStore::Set_CashEvent_Status(unsigned __int8 byEventType, unsigned __int8 byStatus)
{
  const int currentStatus = m_cash_event[byEventType].m_event_status;
  m_cash_event[byEventType].m_event_log.Write("[EventStateChange : %d -> %d ]", currentStatus, byStatus);
  if (byStatus == 7)
  {
    m_cash_event[byEventType].m_event_log.Write("[Event Expired]");
  }
  m_cash_event[byEventType].m_event_status = byStatus;
}

void CashItemRemoteStore::Load_Event_INI(_cash_event_ini *pIni, _FILETIME *pft, char *pEventType)
{
  if (!pIni || !pft)
  {
    return;
  }

  if (!std::strcmp(pEventType, "NULL"))
  {
    return;
  }

  pIni->m_bUseCashEvent = 0;
  pIni->m_EventTime[0] = 0;
  pIni->m_EventTime[1] = 0;
  pIni->m_bRepeat = 0;

  _FILETIME ftWrite{};
  if (!GetLastWriteFileTime("./Initialize/cash_discount_limitsale.ini", &ftWrite))
  {
    return;
  }
  *pft = ftWrite;

  if (!std::strcmp(pEventType, "Limit Sale"))
  {
    _FILETIME ftLimit{};
    if (GetLastWriteFileTime("./Initialize/cash_discount_limitsale.ini", &ftLimit))
    {
      *pft = ftLimit;
      int value = GetPrivateProfileIntA(
        pEventType,
        "USE",
        1,
        "./Initialize/cash_discount_limitsale.ini");
      if (value == 1)
      {
        pIni->m_bUseCashEvent = 0;
      }
      else
      {
        pIni->m_bUseCashEvent = 1;
        pIni->m_bRepeat = GetPrivateProfileIntA(
          pEventType,
          "REPEAT",
          0,
          "./Initialize/cash_discount_limitsale.ini");
        value = GetPrivateProfileIntA(
          pEventType,
          "REPEAT_DAY",
          0,
          "./Initialize/cash_discount_limitsale.ini");
        if (value > 0)
        {
          pIni->m_byRepeatDay = static_cast<unsigned __int8>(value);
        }
        else
        {
          pIni->m_bRepeat = 0;
        }
        value = GetPrivateProfileIntA(
          pEventType,
          "BEGIN_YEAR",
          0,
          "./Initialize/cash_discount_limitsale.ini");
        if (value)
        {
          pIni->m_wYear[0] = static_cast<unsigned __int16>(value);
          value = GetPrivateProfileIntA(
            pEventType,
            "BEGIN_MONTH",
            0,
            "./Initialize/cash_discount_limitsale.ini");
          if (value)
          {
            pIni->m_byMonth[0] = static_cast<unsigned __int8>(value);
            value = GetPrivateProfileIntA(
              pEventType,
              "BEGIN_DAY",
              0,
              "./Initialize/cash_discount_limitsale.ini");
            if (value)
            {
              pIni->m_byDay[0] = static_cast<unsigned __int8>(value);
              value = GetPrivateProfileIntA(
                pEventType,
                "BEGIN_HOUR",
                0,
                "./Initialize/cash_discount_limitsale.ini");
              if (value < 24)
              {
                pIni->m_byHour[0] = static_cast<unsigned __int8>(value);
                value = GetPrivateProfileIntA(
                  pEventType,
                  "BEGIN_MINUTE",
                  0,
                  "./Initialize/cash_discount_limitsale.ini");
                if (value < 60)
                {
                  pIni->m_byMinute[0] = static_cast<unsigned __int8>(value);
                  std::tm tmValue{};
                  tmValue.tm_year = pIni->m_wYear[0] - 1900;
                  tmValue.tm_mon = pIni->m_byMonth[0] - 1;
                  tmValue.tm_mday = pIni->m_byDay[0];
                  tmValue.tm_hour = pIni->m_byHour[0];
                  tmValue.tm_min = pIni->m_byMinute[0];
                  tmValue.tm_sec = 0;
                  tmValue.tm_isdst = -1;
                  pIni->m_EventTime[0] = _mktime32(&tmValue);
                  if (pIni->m_EventTime[0] == -1)
                  {
                    pIni->m_bUseCashEvent = 0;
                  }
                  else
                  {
                    value = GetPrivateProfileIntA(
                      pEventType,
                      "END_YEAR",
                      0,
                      "./Initialize/cash_discount_limitsale.ini");
                    if (value)
                    {
                      pIni->m_wYear[1] = static_cast<unsigned __int16>(value);
                      value = GetPrivateProfileIntA(
                        pEventType,
                        "END_MONTH",
                        0,
                        "./Initialize/cash_discount_limitsale.ini");
                      if (value)
                      {
                        pIni->m_byMonth[1] = static_cast<unsigned __int8>(value);
                        value = GetPrivateProfileIntA(
                          pEventType,
                          "END_DAY",
                          0,
                          "./Initialize/cash_discount_limitsale.ini");
                        if (value)
                        {
                          pIni->m_byDay[1] = static_cast<unsigned __int8>(value);
                          value = GetPrivateProfileIntA(
                            pEventType,
                            "END_HOUR",
                            0,
                            "./Initialize/cash_discount_limitsale.ini");
                          if (value < 24)
                          {
                            pIni->m_byHour[1] = static_cast<unsigned __int8>(value);
                            value = GetPrivateProfileIntA(
                              pEventType,
                              "END_MINUTE",
                              0,
                              "./Initialize/cash_discount_limitsale.ini");
                            if (value < 60)
                            {
                              pIni->m_byMinute[1] = static_cast<unsigned __int8>(value);
                              std::memset(&tmValue, 0, sizeof(tmValue));
                              tmValue.tm_year = pIni->m_wYear[1] - 1900;
                              tmValue.tm_mon = pIni->m_byMonth[1] - 1;
                              tmValue.tm_mday = pIni->m_byDay[1];
                              tmValue.tm_hour = pIni->m_byHour[1];
                              tmValue.tm_min = pIni->m_byMinute[1];
                              tmValue.tm_sec = 0;
                              tmValue.tm_isdst = -1;
                              pIni->m_EventTime[1] = _mktime32(&tmValue);
                              if (pIni->m_EventTime[1] == -1)
                              {
                                pIni->m_bUseCashEvent = 0;
                              }
                              else
                              {
                                value = GetPrivateProfileIntA(
                                  pEventType,
                                  "EXPIRE_YEAR",
                                  0,
                                  "./Initialize/cash_discount_limitsale.ini");
                                if (value > 0)
                                {
                                  pIni->m_wYear[2] = static_cast<unsigned __int16>(value);
                                }
                                else
                                {
                                  pIni->m_bRepeat = 0;
                                }
                                value = GetPrivateProfileIntA(
                                  pEventType,
                                  "EXPIRE_MONTH",
                                  0,
                                  "./Initialize/cash_discount_limitsale.ini");
                                if (value > 0)
                                {
                                  pIni->m_byMonth[2] = static_cast<unsigned __int8>(value);
                                }
                                else
                                {
                                  pIni->m_bRepeat = 0;
                                }
                                value = GetPrivateProfileIntA(
                                  pEventType,
                                  "EXPIRE_DAY",
                                  0,
                                  "./Initialize/cash_discount_limitsale.ini");
                                if (value > 0)
                                {
                                  pIni->m_byDay[2] = static_cast<unsigned __int8>(value);
                                }
                                else
                                {
                                  pIni->m_bRepeat = 0;
                                }
                                value = GetPrivateProfileIntA(
                                  pEventType,
                                  "EXPIRE_HOUR",
                                  0,
                                  "./Initialize/cash_discount_limitsale.ini");
                                if (value < 24)
                                {
                                  pIni->m_byHour[2] = static_cast<unsigned __int8>(value);
                                }
                                else
                                {
                                  pIni->m_bRepeat = 0;
                                }
                                value = GetPrivateProfileIntA(
                                  pEventType,
                                  "EXPIRE_MINUTE",
                                  0,
                                  "./Initialize/cash_discount_limitsale.ini");
                                if (value < 60)
                                {
                                  pIni->m_byMinute[2] = static_cast<unsigned __int8>(value);
                                }
                                else
                                {
                                  pIni->m_bRepeat = 0;
                                }
                                std::memset(&tmValue, 0, sizeof(tmValue));
                                tmValue.tm_year = pIni->m_wYear[2] - 1900;
                                tmValue.tm_mon = pIni->m_byMonth[2] - 1;
                                tmValue.tm_mday = pIni->m_byDay[2];
                                tmValue.tm_hour = pIni->m_byHour[2];
                                tmValue.tm_min = pIni->m_byMinute[2];
                                tmValue.tm_sec = 0;
                                tmValue.tm_isdst = -1;
                                pIni->m_EventTime[2] = _mktime32(&tmValue);
                                if (pIni->m_EventTime[2] == -1)
                                {
                                  pIni->m_bRepeat = 0;
                                }
                              }
                            }
                            else
                            {
                              pIni->m_bUseCashEvent = 0;
                            }
                          }
                          else
                          {
                            pIni->m_bUseCashEvent = 0;
                          }
                        }
                        else
                        {
                          pIni->m_bUseCashEvent = 0;
                        }
                      }
                      else
                      {
                        pIni->m_bUseCashEvent = 0;
                      }
                    }
                    else
                    {
                      pIni->m_bUseCashEvent = 0;
                    }
                  }
                }
                else
                {
                  pIni->m_bUseCashEvent = 0;
                }
              }
              else
              {
                pIni->m_bUseCashEvent = 0;
              }
            }
            else
            {
              pIni->m_bUseCashEvent = 0;
            }
          }
          else
          {
            pIni->m_bUseCashEvent = 0;
          }
        }
        else
        {
          pIni->m_bUseCashEvent = 0;
        }
      }
    }
  }
  else
  {
    _FILETIME ftNormal{};
    if (!GetLastWriteFileTime("./initialize/cash_discount.ini", &ftNormal))
    {
      return;
    }
    *pft = ftNormal;

    int value = GetPrivateProfileIntA(pEventType, "USE", 1, "./initialize/cash_discount.ini");
    if (value == 1)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }

    pIni->m_bUseCashEvent = 1;
    pIni->m_bRepeat = GetPrivateProfileIntA(pEventType, "REPEAT", 0, "./initialize/cash_discount.ini");
    value = GetPrivateProfileIntA(pEventType, "REPEAT_DAY", 0, "./initialize/cash_discount.ini");
    if (value > 0)
    {
      pIni->m_byRepeatDay = static_cast<unsigned __int8>(value);
    }
    else
    {
      pIni->m_bRepeat = 0;
    }

    value = GetPrivateProfileIntA(pEventType, "BEGIN_YEAR", 0, "./initialize/cash_discount.ini");
    if (!value)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_wYear[0] = static_cast<unsigned __int16>(value);
    value = GetPrivateProfileIntA(pEventType, "BEGIN_MONTH", 0, "./initialize/cash_discount.ini");
    if (!value)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byMonth[0] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "BEGIN_DAY", 0, "./initialize/cash_discount.ini");
    if (!value)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byDay[0] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "BEGIN_HOUR", 0, "./initialize/cash_discount.ini");
    if (value >= 24)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byHour[0] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "BEGIN_MINUTE", 0, "./initialize/cash_discount.ini");
    if (value >= 60)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byMinute[0] = static_cast<unsigned __int8>(value);

    std::tm tmValue{};
    tmValue.tm_year = pIni->m_wYear[0] - 1900;
    tmValue.tm_mon = pIni->m_byMonth[0] - 1;
    tmValue.tm_mday = pIni->m_byDay[0];
    tmValue.tm_hour = pIni->m_byHour[0];
    tmValue.tm_min = pIni->m_byMinute[0];
    tmValue.tm_sec = 0;
    tmValue.tm_isdst = -1;
    pIni->m_EventTime[0] = _mktime32(&tmValue);
    if (pIni->m_EventTime[0] == -1)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }

    value = GetPrivateProfileIntA(pEventType, "END_YEAR", 0, "./initialize/cash_discount.ini");
    if (!value)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_wYear[1] = static_cast<unsigned __int16>(value);
    value = GetPrivateProfileIntA(pEventType, "END_MONTH", 0, "./initialize/cash_discount.ini");
    if (!value)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byMonth[1] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "END_DAY", 0, "./initialize/cash_discount.ini");
    if (!value)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byDay[1] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "END_HOUR", 0, "./initialize/cash_discount.ini");
    if (value >= 24)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byHour[1] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "END_MINUTE", 0, "./initialize/cash_discount.ini");
    if (value >= 60)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byMinute[1] = static_cast<unsigned __int8>(value);

    if (!std::strcmp(pEventType, "Set"))
    {
      value = GetPrivateProfileIntA(pEventType, "SetA_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 100)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[0] = static_cast<unsigned __int8>(value);
      value = GetPrivateProfileIntA(pEventType, "SetB_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 100)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[1] = static_cast<unsigned __int8>(value);
      value = GetPrivateProfileIntA(pEventType, "SetC_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 100)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[2] = static_cast<unsigned __int8>(value);
      value = GetPrivateProfileIntA(pEventType, "SetAll_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 100)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[3] = static_cast<unsigned __int8>(value);
    }

    std::memset(&tmValue, 0, sizeof(tmValue));
    tmValue.tm_year = pIni->m_wYear[1] - 1900;
    tmValue.tm_mon = pIni->m_byMonth[1] - 1;
    tmValue.tm_mday = pIni->m_byDay[1];
    tmValue.tm_hour = pIni->m_byHour[1];
    tmValue.tm_min = pIni->m_byMinute[1];
    tmValue.tm_sec = 0;
    tmValue.tm_isdst = -1;
    pIni->m_EventTime[1] = _mktime32(&tmValue);
    if (pIni->m_EventTime[1] == -1)
    {
      pIni->m_bUseCashEvent = 0;
    }
    else
    {
      value = GetPrivateProfileIntA(pEventType, "EXPIRE_YEAR", 0, "./initialize/cash_discount.ini");
      if (value > 0)
      {
        pIni->m_wYear[2] = static_cast<unsigned __int16>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      value = GetPrivateProfileIntA(pEventType, "EXPIRE_MONTH", 0, "./initialize/cash_discount.ini");
      if (value > 0)
      {
        pIni->m_byMonth[2] = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      value = GetPrivateProfileIntA(pEventType, "EXPIRE_DAY", 0, "./initialize/cash_discount.ini");
      if (value > 0)
      {
        pIni->m_byDay[2] = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      value = GetPrivateProfileIntA(pEventType, "EXPIRE_HOUR", 0, "./initialize/cash_discount.ini");
      if (value < 24)
      {
        pIni->m_byHour[2] = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      value = GetPrivateProfileIntA(pEventType, "EXPIRE_MINUTE", 0, "./initialize/cash_discount.ini");
      if (value < 60)
      {
        pIni->m_byMinute[2] = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      std::memset(&tmValue, 0, sizeof(tmValue));
      tmValue.tm_year = pIni->m_wYear[2] - 1900;
      tmValue.tm_mon = pIni->m_byMonth[2] - 1;
      tmValue.tm_mday = pIni->m_byDay[2];
      tmValue.tm_hour = pIni->m_byHour[2];
      tmValue.tm_min = pIni->m_byMinute[2];
      tmValue.tm_sec = 0;
      tmValue.tm_isdst = -1;
      pIni->m_EventTime[2] = _mktime32(&tmValue);
      if (pIni->m_EventTime[2] == -1)
      {
        pIni->m_bRepeat = 0;
      }
    }
  }
}

void CashItemRemoteStore::Load_LimitedSale_Event_INI(_cash_event_ini *pIni, _FILETIME *pft, char *pEventType)
{
  if (pIni && pft && std::strcmp(pEventType, "NULL"))
  {
    char codeKey[20];
    char countKey[20];
    std::memset(codeKey, 0, sizeof(codeKey));
    std::memset(countKey, 0, sizeof(countKey));

    if (!std::strcmp(pEventType, "Limit Sale"))
    {
      pIni->m_byLimited_sale_num = static_cast<unsigned __int8>(GetPrivateProfileIntA(
        pEventType,
        "LimItem",
        0,
        "./Initialize/cash_discount_limitsale.ini"));
      for (int j = 0; j < pIni->m_byLimited_sale_num; ++j)
      {
        sprintf_s(codeKey, sizeof(codeKey), "Limcode%d", j);
        sprintf_s(countKey, sizeof(countKey), "Limcount%d", j);
        GetPrivateProfileStringA(
          pEventType,
          codeKey,
          kLimitedSaleDefault,
          pIni->m_Limited_sale[j].m_szLimcode,
          64,
          "./Initialize/cash_discount_limitsale.ini");
        int value = GetPrivateProfileIntA(
          pEventType,
          countKey,
          0,
          "./Initialize/cash_discount_limitsale.ini");
        if (value > 2000)
        {
          pIni->m_bUseCashEvent = 0;
        }
        pIni->m_Limited_sale[j].m_wLimcount = static_cast<unsigned __int16>(value);
      }
      int value = GetPrivateProfileIntA(pEventType, "Discout", 0, "./Initialize/cash_discount_limitsale.ini");
      if (value >= 100)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byLimDiscout = static_cast<unsigned __int8>(value);
    }

    Set_LimitedSale_Event_Ini(pIni);
  }
}

void CashItemRemoteStore::Set_LimitedSale_Event_Ini(_cash_event_ini *pIni)
{
  if (!pIni)
  {
    return;
  }

  m_lim_event.DCK = pIni->m_byDCK;
  m_lim_event.m_byEventNum = pIni->m_byLimited_sale_num;
  for (int j = 0; j < m_lim_event.m_byEventNum; ++j)
  {
    m_lim_event.m_EventItemInfo[j].byTableCode = static_cast<unsigned __int8>(-1);
    m_lim_event.m_EventItemInfo[j].dwIndex = 255;
    m_lim_event.m_EventItemInfo[j].wCount = pIni->m_Limited_sale[j].m_wLimcount;
    const unsigned __int8 itemTableCode = GetItemTableCode(pIni->m_Limited_sale[j].m_szLimcode);
    if (itemTableCode != 255)
    {
      _base_fld *record =
        g_Main.m_tblItemData[itemTableCode].GetRecord(pIni->m_Limited_sale[j].m_szLimcode);
      if (record)
      {
        m_lim_event.m_EventItemInfo[j].byTableCode = itemTableCode;
        m_lim_event.m_EventItemInfo[j].dwIndex = record->m_dwIndex;
      }
    }
  }
}

void CashItemRemoteStore::Set_LimitedSale_count(unsigned __int8 byTableCode, unsigned int dwIndex)
{
  for (int j = 0; j < m_lim_event.m_byEventNum; ++j)
  {
    if (m_lim_event.m_EventItemInfo[j].byTableCode == byTableCode
        && m_lim_event.m_EventItemInfo[j].dwIndex == dwIndex)
    {
      --m_lim_event.m_EventItemInfo[j].wCount;
      return;
    }
  }
}

void CashItemRemoteStore::Set_DB_LimitedSale_Event()
{
  std::memcpy(&m_lim_event_New, &m_lim_event, sizeof(m_lim_event_New));

  qry_case_cash_limsale pQryData;
  pQryData.NewSale.byDck = m_lim_event_New.DCK;
  pQryData.OldSale.byDck = m_lim_event_Old.DCK;
  pQryData.NewSale.byLimited_sale_num = m_lim_event_New.m_byEventNum;
  pQryData.OldSale.byLimited_sale_num = m_lim_event_Old.m_byEventNum;

  for (int j = 0; j < m_lim_event_New.m_byEventNum; ++j)
  {
    _INVENKEY newKey(0, m_lim_event_New.m_EventItemInfo[j].byTableCode, m_lim_event_New.m_EventItemInfo[j].dwIndex);
    pQryData.NewSale.List[j].nLimcode = newKey.CovDBKey();
    pQryData.NewSale.List[j].nLimcount = m_lim_event_New.m_EventItemInfo[j].wCount;

    _INVENKEY oldKey(0, m_lim_event_Old.m_EventItemInfo[j].byTableCode, m_lim_event_Old.m_EventItemInfo[j].dwIndex);
    pQryData.OldSale.List[j].nLimcode = oldKey.CovDBKey();
    pQryData.OldSale.List[j].nLimcount = m_lim_event_Old.m_EventItemInfo[j].wCount;
  }

  const int nSize = static_cast<int>(pQryData.size());
  if (g_Main.PushDQSData(-1, nullptr, 158, reinterpret_cast<char *>(&pQryData), nSize))
  {
    std::memcpy(&m_lim_event_Old, &m_lim_event_New, sizeof(m_lim_event_Old));
  }
}

void CashItemRemoteStore::Set_FROMDB_LimitedSale_Event(_db_cash_limited_sale *Sheet)
{
  m_lim_event_New.DCK = Sheet->byDck;
  m_lim_event_New.m_byEventNum = Sheet->byLimited_sale_num;

  for (int i = 0; i < m_lim_event_New.m_byEventNum; ++i)
  {
    const _db_cash_limited_sale::_db_cash_limited_info *info = &Sheet->List[i];
    m_lim_event_New.m_EventItemInfo[i].byTableCode = static_cast<unsigned __int8>(info->nLimcode >> 8);
    m_lim_event_New.m_EventItemInfo[i].dwIndex = static_cast<unsigned int>(info->nLimcode >> 16);
    m_lim_event_New.m_EventItemInfo[i].wCount = static_cast<unsigned __int16>(info->nLimcount);
  }

  _cash_lim_sale tmp{};
  std::memcpy(&tmp, &m_lim_event_New, sizeof(tmp));
  std::memcpy(&m_lim_event_Old, &tmp, sizeof(m_lim_event_Old));
}

unsigned __int16 CashItemRemoteStore::BuyLimSale(unsigned __int8 byTableCode, unsigned int dwIndex)
{
  for (int j = 0; j < m_lim_event.m_byEventNum; ++j)
  {
    if (m_lim_event.m_EventItemInfo[j].byTableCode == byTableCode
        && m_lim_event.m_EventItemInfo[j].dwIndex == dwIndex)
    {
      return m_lim_event.m_EventItemInfo[j].wCount;
    }
  }
  return 0;
}

bool CashItemRemoteStore::LimitedSale_check_count(unsigned __int8 byTableCode, unsigned int dwIndex)
{
  const unsigned __int16 remainCount = BuyLimSale(byTableCode, dwIndex);
  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bOper && player->m_bLive)
    {
      ICsSendInterface::SendMsg_LimitedsaleEventInform(player->m_ObjID.m_wIndex, byTableCode, dwIndex, remainCount);
    }
  }
  return true;
}

void CashItemRemoteStore::Check_Loaded_Event_Status(unsigned __int8 byEventType)
{
  if (!m_cash_event[byEventType].m_ini.m_bUseCashEvent)
  {
    Set_CashEvent_Status(byEventType, 0);
    return;
  }

  __time32_t now[4];
  _time32(now);

  if (now[0] > m_cash_event[byEventType].m_ini.m_EventTime[1])
  {
    Set_CashEvent_Status(byEventType, 0);
  }
  else if (now[0] < m_cash_event[byEventType].m_ini.m_EventTime[0]
    || now[0] > m_cash_event[byEventType].m_ini.m_EventTime[1])
  {
    if (now[0] < m_cash_event[byEventType].m_ini.m_EventTime[0])
    {
      Set_CashEvent_Status(byEventType, 1);

      char eventName[80];
      Get_CashEvent_Name(byEventType, eventName);
      m_cash_event[byEventType].m_event_log.Write(
        "[ %s CashEvent Loaded] [EventState : %d] [EventTime : %d/%d/%d %d:%d  ~ %d/%d/%d %d:%d ]",
        eventName,
        m_cash_event[byEventType].m_event_status,
        m_cash_event[byEventType].m_ini.m_wYear[0],
        m_cash_event[byEventType].m_ini.m_byMonth[0],
        m_cash_event[byEventType].m_ini.m_byDay[0],
        m_cash_event[byEventType].m_ini.m_byHour[0],
        m_cash_event[byEventType].m_ini.m_byMinute[0],
        m_cash_event[byEventType].m_ini.m_wYear[1],
        m_cash_event[byEventType].m_ini.m_byMonth[1],
        m_cash_event[byEventType].m_ini.m_byDay[1],
        m_cash_event[byEventType].m_ini.m_byHour[1],
        m_cash_event[byEventType].m_ini.m_byMinute[1]);
      m_cash_event[byEventType].m_event_log.Write(
        "Repeat Information >> Repeat:%d , RepeatDay:%d",
        m_cash_event[byEventType].m_ini.m_bRepeat,
        m_cash_event[byEventType].m_ini.m_byRepeatDay);
      m_cash_event[byEventType].m_event_log.Write(
        "Expire Date >> [%d/%d/%d %d:%d]",
        m_cash_event[byEventType].m_ini.m_wYear[2],
        m_cash_event[byEventType].m_ini.m_byMonth[2],
        m_cash_event[byEventType].m_ini.m_byDay[2],
        m_cash_event[byEventType].m_ini.m_byHour[2],
        m_cash_event[byEventType].m_ini.m_byMinute[2]);
    }
  }
  else
  {
    const int remainTime = m_cash_event[byEventType].m_ini.m_EventTime[1] - now[0];
    if (remainTime > 0)
    {
      if (remainTime > m_cash_event[byEventType].m_event_inform_before[1])
      {
        if (remainTime > m_cash_event[byEventType].m_event_inform_before[0])
        {
          Set_CashEvent_Status(byEventType, 2);
        }
        else
        {
          Set_CashEvent_Status(byEventType, 3);
        }
      }
      else
      {
        Set_CashEvent_Status(byEventType, 4);
      }
    }
    else
    {
      Set_CashEvent_Status(byEventType, 0);
    }
  }

  SetNextEventTime(byEventType);
}

bool CashItemRemoteStore::SetNextEventTime(unsigned __int8 byEventType)
{
  if (!m_cash_event[byEventType].m_ini.m_bRepeat)
  {
    return false;
  }

  std::tm tmValue{};
  tmValue.tm_year = m_cash_event[byEventType].m_ini.m_wYear[0] - 1900;
  tmValue.tm_mon = m_cash_event[byEventType].m_ini.m_byMonth[0] - 1;
  tmValue.tm_mday = m_cash_event[byEventType].m_ini.m_byDay[0] + m_cash_event[byEventType].m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cash_event[byEventType].m_ini.m_byHour[0];
  tmValue.tm_min = m_cash_event[byEventType].m_ini.m_byMinute[0];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cash_event[byEventType].m_ini.m_NextEventTime[0] = _mktime32(&tmValue);
  if (m_cash_event[byEventType].m_ini.m_NextEventTime[0] == -1)
  {
    m_cash_event[byEventType].m_ini.m_bRepeat = 0;
    m_cash_event[byEventType].m_event_log.Write(
      "SetNextEventTime() : Fail When Calculate Next Event Begin Time");
    return false;
  }

  std::memset(&tmValue, 0, sizeof(tmValue));
  tmValue.tm_year = m_cash_event[byEventType].m_ini.m_wYear[1] - 1900;
  tmValue.tm_mon = m_cash_event[byEventType].m_ini.m_byMonth[1] - 1;
  tmValue.tm_mday = m_cash_event[byEventType].m_ini.m_byDay[1] + m_cash_event[byEventType].m_ini.m_byRepeatDay;
  tmValue.tm_hour = m_cash_event[byEventType].m_ini.m_byHour[1];
  tmValue.tm_min = m_cash_event[byEventType].m_ini.m_byMinute[1];
  tmValue.tm_sec = 0;
  tmValue.tm_isdst = -1;
  m_cash_event[byEventType].m_ini.m_NextEventTime[1] = _mktime32(&tmValue);
  if (m_cash_event[byEventType].m_ini.m_NextEventTime[1] == -1)
  {
    m_cash_event[byEventType].m_ini.m_bRepeat = 0;
    m_cash_event[byEventType].m_event_log.Write(
      "SetNextEventTime() : Fail When Calculate Next Event End Time");
    return false;
  }

  return true;
}

bool CashItemRemoteStore::LoadBuyCashMode()
{
  char returnedString[28];
  std::memset(returnedString, 0, 6);
  returnedString[16] = 0;
  GetPrivateProfileStringA(
    "ServerMode",
    "BuyCashItemByGold",
    "X",
    returnedString,
    6,
    "..\\WorldInfo\\WorldInfo.ini");

  if (!_stricmp(returnedString, "true"))
  {
    _bIsBuyCashItemByGold = true;
  }
  else
  {
    if (_stricmp(returnedString, "false"))
    {
      MyMessageBox(
        "CashItemRemoteStore::LoadBuyCashMode()",
        "WorldInfo.ini File\r\n[ServerMode]\r\nBuyCashItemByGold = %s Invalid!!",
        returnedString);
      return false;
    }
    _bIsBuyCashItemByGold = false;
  }

  if (g_Main.IsTestServer() && !_bIsBuyCashItemByGold)
  {
    _bIsBuyCashItemByGold = true;
    MyMessageBox(
      "CashItemRemoteStore::LoadBuyCashMode()",
      "WorldInfo.ini File\r\n"
      "[ServerMode]\r\n"
      "RegularTestServerMode  = true\r\n"
      "BuyCashItemByGold = false -> true Force Changed!!");
  }

  return true;
}

void CashItemRemoteStore::_buybygold_complete(
  CPlayer *pOne,
  _result_csi_buy_zocl *Send,
  _request_csi_buy_clzo *pRecv,
  _request_csi_buy_clzo::__item *pSrc,
  _param_cashitem_dblog *pSheet,
  bool bCouponUse)
{
  Send->nCashAmount = pOne->m_Param.GetGold();

  unsigned __int8 type[2]{};
  type[0] = 57;
  type[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pOne->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(Send),
    Send->size());

  const unsigned __int64 taskSize = static_cast<unsigned __int64>(pSheet->size());
  CCashDBWorkManager *cashWorkManager = CTSingleton<CCashDBWorkManager>::Instance();
  cashWorkManager->PushTask(3, reinterpret_cast<unsigned __int8 *>(pSheet), taskSize);

  if (pSheet->in_bLimited_Sale && pSrc->byEventType == 5)
  {
    Set_LimitedSale_count(pSrc->byTblCode, pSrc->wItemIdx);
    Set_DB_LimitedSale_Event();
    LimitedSale_check_count(pSrc->byTblCode, pSrc->wItemIdx);
  }

  if (bCouponUse)
  {
    pOne->DeleteCouponItem(pRecv->CouponItem, pRecv->byCouponNum);
  }
}

bool CashItemRemoteStore::IsBuyCashItemByGold() const
{
  return _bIsBuyCashItemByGold;
}

void Get_CashEvent_Name(unsigned __int8 byEventType, char *szEventName)
{
  if (byEventType)
  {
    if (byEventType == 1)
    {
      wsprintfA(szEventName, "%s", "One&One");
    }
    else if (byEventType == 2)
    {
      wsprintfA(szEventName, "%s", "Limit Sale");
    }
    else
    {
      wsprintfA(szEventName, "%s", "NULL");
    }
  }
  else
  {
    wsprintfA(szEventName, "%s", "Set");
  }
}
