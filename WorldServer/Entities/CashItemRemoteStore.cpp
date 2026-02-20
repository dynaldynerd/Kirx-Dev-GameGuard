
#include "pch.h"

#include "CashItemRemoteStore.h"

#include "CashShop_fld.h"
#include "CashShop_str_fld.h"
#include "CCashDBWorkManager.h"
#include "CMainThread.h"
#include "CNationSettingManager.h"
#include "CTSingleton.h"
#include "GlobalObjects.h"
#include "ICsSendInterface.h"
#include "TimeItem.h"
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

CashItemRemoteStore::CashItemRemoteStore()
{
  _bIsBuyCashItemByGold = false;
}

CashItemRemoteStore::~CashItemRemoteStore() = default;

CashItemRemoteStore::_remain_num_of_good::_remain_num_of_good()
{
  memset_0(this, 0, sizeof(CashItemRemoteStore::_remain_num_of_good));
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
  m_TotalEventTimer.BeginTimer(0x3E8u);
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

char CashItemRemoteStore::GoodsListBuyByGold(unsigned __int16 wSock, char *pPacket)
{
  (void)pPacket;

  if (wSock >= MAX_PLAYER)
  {
    return 0;
  }

  CPlayer *player = &g_Player[wSock];
  if (!player->m_bLive || !player->m_bOper)
  {
    return 1;
  }

  _result_csi_goods_list_zocl msg{};
  msg.nCashAmount = player->m_Param.GetGold();
  msg.bAdjustDiscountRate = is_cde_time() != 0;
  msg.bOneNOne = IsEventTime(1u) != 0;
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

  msg.bLimSale = IsEventTime(2u) != 0;
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
  return 1;
}

unsigned __int8 CashItemRemoteStore::Get_CashEvent_Status(unsigned __int8 byEventType)
{
  return m_cash_event[byEventType].m_event_status;
}

char CashItemRemoteStore::ChangeEventTime(unsigned __int8 byEventType)
{
  if (!m_cash_event[byEventType].m_ini.m_bRepeat)
  {
    return 0;
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
    return 0;
  }

  m_cash_event[byEventType].m_ini.m_wYear[0] = tmValue.tm_year + 1900;
  m_cash_event[byEventType].m_ini.m_byMonth[0] = tmValue.tm_mon + 1;
  m_cash_event[byEventType].m_ini.m_byDay[0] = tmValue.tm_mday;
  m_cash_event[byEventType].m_ini.m_byHour[0] = tmValue.tm_hour;
  m_cash_event[byEventType].m_ini.m_byMinute[0] = tmValue.tm_min;

  memset_0(&tmValue, 0, sizeof(tmValue));
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
    return 0;
  }

  m_cash_event[byEventType].m_ini.m_wYear[1] = tmValue.tm_year + 1900;
  m_cash_event[byEventType].m_ini.m_byMonth[1] = tmValue.tm_mon + 1;
  m_cash_event[byEventType].m_ini.m_byDay[1] = tmValue.tm_mday;
  m_cash_event[byEventType].m_ini.m_byHour[1] = tmValue.tm_hour;
  m_cash_event[byEventType].m_ini.m_byMinute[1] = tmValue.tm_min;
  return 1;
}

void CashItemRemoteStore::Set_LimitedSale_DCK(unsigned __int8 /*byEventType*/, unsigned __int8 byDCK)
{
  m_lim_event.DCK = byDCK;
}

void CashItemRemoteStore::Set_LimitedSale_Event()
{
  _cash_lim_sale tmp{};
  memcpy_0(&tmp, &m_lim_event_New, sizeof(tmp));
  memcpy_0(&m_lim_event, &tmp, sizeof(m_lim_event));
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

  auto moveToEnd = [&]() { Set_CashEvent_Status(byEventType, 5u); };
  switch (eventStatus)
  {
    case 0u:
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

      Set_CashEvent_Status(byEventType, 1u);
      break;
    }

    case 1u:
    {
      if (m_cash_event[byEventType].m_ini.m_bRepeat
          && m_cash_event[byEventType].m_ini.m_EventTime[2] <= now[0])
      {
        moveToEnd();
      }
      else if (now[0] >= m_cash_event[byEventType].m_ini.m_EventTime[0])
      {
        Set_CashEvent_Status(byEventType, 2u);
        if (byEventType == 2)
        {
          Set_LimitedSale_DCK(byEventType, 1u);
          g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x9Du, nullptr, 0);
          Set_DB_LimitedSale_Event();
        }
        Inform_CashEvent_Status_All(byEventType, 2u, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 2u:
    {
      if (byEventType == 2 && m_lim_event.DCK != 1)
      {
        g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x9Du, nullptr, 0);
        Set_LimitedSale_Event();
      }

      if (m_cash_event[byEventType].m_ini.m_EventTime[1] - now[0]
          <= m_cash_event[byEventType].m_event_inform_before[0])
      {
        Set_CashEvent_Status(byEventType, 3u);
        Inform_CashEvent_Status_All(byEventType, 3u, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 3u:
    {
      if (byEventType == 2 && m_lim_event.DCK != 1)
      {
        g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x9Du, nullptr, 0);
        Set_LimitedSale_Event();
      }

      if (m_cash_event[byEventType].m_ini.m_EventTime[1] - now[0]
          <= m_cash_event[byEventType].m_event_inform_before[1])
      {
        Set_CashEvent_Status(byEventType, 4u);
        Inform_CashEvent_Status_All(byEventType, 4u, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 4u:
    {
      if (byEventType == 2 && m_lim_event.DCK != 1)
      {
        g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x9Du, nullptr, 0);
        Set_LimitedSale_Event();
      }

      if (now[0] >= m_cash_event[byEventType].m_ini.m_EventTime[1])
      {
        Set_CashEvent_Status(byEventType, 5u);
        if (byEventType == 2)
        {
          Set_LimitedSale_DCK(byEventType, 0);
          Set_DB_LimitedSale_Event();
        }
        Inform_CashEvent_Status_All(byEventType, 5u, &m_cash_event[byEventType].m_ini);
      }
      break;
    }

    case 5u:
    {
      if (!m_cash_event[byEventType].m_ini.m_bUseCashEvent || !m_cash_event[byEventType].m_ini.m_bRepeat)
      {
        Set_CashEvent_Status(byEventType, 7u);
        break;
      }

      if (m_cash_event[byEventType].m_ini.m_EventTime[2] <= now[0])
      {
        m_cash_event[byEventType].m_ini.m_bUseCashEvent = 0;
        m_cash_event[byEventType].m_ini.m_bRepeat = 0;
        Set_CashEvent_Status(byEventType, 7u);
        return;
      }

      if (SetNextEventTime(byEventType))
      {
        Set_CashEvent_Status(byEventType, 0);
      }
      else
      {
        Set_CashEvent_Status(byEventType, 7u);
      }
      break;
    }

    default:
      return;
  }
}

const _CashShop_fld *CashItemRemoteStore::FindCashRec(unsigned int nTbl, int nIdx)
{
  if (nTbl > 0x24)
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
            || static_cast<unsigned int>(timeValue[0] - m_cde.m_ini.m_cdeTime[1]) > 0x1C20))
    {
      for (int j = 0; j < 3; ++j)
      {
        if (timeValue[0] > m_cash_event[j].m_ini.m_EventTime[1]
            && static_cast<unsigned int>(timeValue[0] - m_cash_event[j].m_ini.m_EventTime[1]) <= 0x1C20)
        {
          return;
        }
      }

      if ((timeValue[0] <= m_con_event.m_eventtime.m_EventTime[1]
           || static_cast<unsigned int>(timeValue[0] - m_con_event.m_eventtime.m_EventTime[1]) > 0x1C20)
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
        Set_Conditional_Evnet_Status(1u);
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
  unsigned int logTime = GetKorLocalTime();
  sprintf_s(buffer, 0x100u, "%s\\Shop_%d.sys", logDir, logTime);
  _kLoggers[0].SetWriteLogFile(buffer, 1, 0, 1, 1);

  memset_0(buffer, 0, 0x100u);
  logTime = GetKorLocalTime();
  sprintf_s(buffer, 0x100u, "%s\\Shop_%d.prc", logDir, logTime);
  _kLoggers[1].SetWriteLogFile(buffer, 1, 0, 1, 1);

  memset_0(buffer, 0, 0x100u);
  logTime = GetKorLocalTime();
  sprintf_s(buffer, 0x100u, "%s\\DiscountRate_%d.log", logDir, logTime);
  m_cde.m_cde_log.SetWriteLogFile(buffer, 1, 0, 1, 1);

  for (int j = 0; j < 3; ++j)
  {
    memset_0(buffer, 0, 0x100u);
    logTime = GetKorLocalTime();
    sprintf_s(buffer, 0x100u, "%s\\CashEvent_%dType_%d.log", logDir, j, logTime);
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

  memset_0(buffer, 0, 0x100u);
  logTime = GetKorLocalTime();
  sprintf_s(buffer, 0x100u, "%s\\ConditionalEvent_%d.log", logDir, logTime);
  m_con_event.m_conevent_log.SetWriteLogFile(buffer, 1, 0, 1, 1);

  return true;
}

bool CashItemRemoteStore::_ReadGoods()
{
  char errorMessage[160];
  if (_kRecGoods.ReadRecord(".\\Script\\CashShop.dat", 0xF0u, errorMessage))
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
  if (krecPrice->ReadRecord(".\\script\\CashShop_str.dat", 0x70u, errorMessage))
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

  _base_fld *record = nullptr;
  for (int n = 0; ; ++n)
  {
    const int recordCount = _kRecGoods.GetRecordNum();
    if (n >= recordCount)
    {
      return true;
    }

    record = _kRecGoods.GetRecord(n);
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

    const int itemTableCode = GetItemTableCode(reinterpret_cast<char *>(&record[1]));
    if (itemTableCode == -1)
    {
      if (g_Main.IsExcuteService())
      {
        _kLoggers[0].Write(
          "CashShop Data Load Error : Invalid Table Code, CashShop Item Code(%s)",
          reinterpret_cast<const char *>(&record[1]));
      }
      else
      {
        MyMessageBox(
          "CashITemRemoteStore",
          "CashShop Data Load Error : Invalid Table Code, CashShop Item Code(%s)",
          reinterpret_cast<const char *>(&record[1]));
      }
      return false;
    }

    _base_fld *itemRecord = g_Main.m_tblItemData[itemTableCode].GetRecord(
      reinterpret_cast<const char *>(&record[1]),
      7);
    if (!itemRecord)
    {
      if (g_Main.IsExcuteService())
      {
        _kLoggers[0].Write(
          "CashShop Data Load Error : Null Pointer Exception (pRec), Not Exist Item! Cash Shop ItemCode(%s)",
          reinterpret_cast<const char *>(&record[1]));
      }
      else
      {
        MyMessageBox(
          "CashITemRemoteStore",
          "CashShop Data Load Error : Null Pointer Exception (pRec), Not Exist Item! Cash Shop ItemCode(%s)",
          reinterpret_cast<const char *>(&record[1]));
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
        reinterpret_cast<const char *>(&record[1]));
      return false;
    }

    CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    const int price = nationSetting->GetCashItemPrice(priceRecord);
    *reinterpret_cast<int *>(&record[1].m_strCode[60]) = price;

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
      *reinterpret_cast<int *>(&record[1].m_strCode[60]),
      checkType,
      useTime,
      record[3].m_dwIndex,
      *reinterpret_cast<int *>(record[3].m_strCode),
      *reinterpret_cast<int *>(&record[3].m_strCode[4]));
  }

  if (g_Main.IsExcuteService())
  {
    _kLoggers[0].Write("Is not cash item : %s", reinterpret_cast<const char *>(&record[1]));
  }
  else
  {
    MyMessageBox("CashITemRemoteStore", "Is not cash item : %s", reinterpret_cast<const char *>(&record[1]));
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
            8u,
            "./initialize/condistional_event.ini");
          if (!strcmp_0(pIni->m_szStartMsg, "FALSE"))
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
              8u,
              "./initialize/condistional_event.ini");
            if (!strcmp_0(pIni->m_szMiddletMsg, "FALSE"))
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
                8u,
                "./initialize/condistional_event.ini");
              if (!strcmp_0(pIni->m_szEndMsg, "FALSE"))
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

void CashItemRemoteStore::Set_Conditional_Evnet_Status(unsigned __int8 byStatus)
{
  const int currentStatus = m_con_event.m_conevent_status;
  m_con_event.m_conevent_log.Write("[EventStateChange : %d -> %d ]", currentStatus, byStatus);
  m_con_event.m_conevent_status = byStatus;
}

void CashItemRemoteStore::load_cash_discount_event()
{
  m_cde.m_cde_log.Write("Loading Cash Discount-Rate Event");
  m_cde.m_cde_timer.BeginTimer(0x3E8u);
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
            if (static_cast<unsigned int>(value) < 0x18)
            {
              pIni->m_byHour[0] = static_cast<unsigned __int8>(value);
              value =
                GetPrivateProfileIntA("Cash", "BEGIN_MINUTE", 0, "./initialize/cash_discount.ini");
              if (static_cast<unsigned int>(value) < 0x3C)
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
                        if (static_cast<unsigned int>(value) < 0x18)
                        {
                          pIni->m_byHour[1] = static_cast<unsigned __int8>(value);
                          value = GetPrivateProfileIntA(
                            "Cash",
                            "END_MINUTE",
                            0,
                            "./initialize/cash_discount.ini");
                          if (static_cast<unsigned int>(value) < 0x3C)
                          {
                            pIni->m_byMinute[1] = static_cast<unsigned __int8>(value);
                            memset_0(&tmValue, 0, sizeof(tmValue));
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
                              if (static_cast<unsigned int>(value) < 0x18)
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
                              if (static_cast<unsigned int>(value) < 0x3C)
                              {
                                pIni->m_byMinute[2] = static_cast<unsigned __int8>(value);
                              }
                              else
                              {
                                pIni->m_bRepeat = 0;
                              }
                              memset_0(&tmValue, 0, sizeof(tmValue));
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
      set_cde_status(1u);
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
          set_cde_status(2u);
        }
        else
        {
          set_cde_status(3u);
        }
      }
      else
      {
        set_cde_status(4u);
      }
    }
    else
    {
      set_cde_status(0);
    }
  }

  SetNextDiscountEventTime();
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

char CashItemRemoteStore::is_cde_time()
{
  if (!m_con_event.m_bConEvent && !m_cde.m_ini.m_bUseCashDiscount)
  {
    return 0;
  }

  __time32_t now[4]{};
  _time32(now);

  if (m_con_event.m_bConEvent)
  {
    if (m_con_event.m_ini.m_byEventKind == 2
        && now[0] > m_con_event.m_eventtime.m_EventTime[0]
        && now[0] < m_con_event.m_eventtime.m_EventTime[1])
    {
      return 1;
    }
  }
  else if (now[0] > m_cde.m_ini.m_cdeTime[0] && now[0] < m_cde.m_ini.m_cdeTime[1])
  {
    return 1;
  }

  return 0;
}

char CashItemRemoteStore::IsEventTime(unsigned __int8 byEventType)
{
  if (!m_con_event.m_bConEvent && !m_cash_event[byEventType].m_ini.m_bUseCashEvent)
  {
    return 0;
  }

  __time32_t now[4]{};
  _time32(now);

  if (m_con_event.m_bConEvent)
  {
    if (m_con_event.m_ini.m_byEventKind == byEventType
        && now[0] > m_con_event.m_eventtime.m_EventTime[0]
        && now[0] < m_con_event.m_eventtime.m_EventTime[1])
    {
      return 1;
    }
  }
  else if (
    now[0] > m_cash_event[byEventType].m_ini.m_EventTime[0]
    && now[0] < m_cash_event[byEventType].m_ini.m_EventTime[1])
  {
    return 1;
  }

  return 0;
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

  memset_0(&tmValue, 0, sizeof(tmValue));
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
    m_cash_event[j].m_event_timer.BeginTimer(0x3E8u);
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

  if (!strcmp_0(pEventType, "NULL"))
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

  if (!strcmp_0(pEventType, "Limit Sale"))
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
              if (value < 0x18)
              {
                pIni->m_byHour[0] = static_cast<unsigned __int8>(value);
                value = GetPrivateProfileIntA(
                  pEventType,
                  "BEGIN_MINUTE",
                  0,
                  "./Initialize/cash_discount_limitsale.ini");
                if (value < 0x3C)
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
                          if (value < 0x18)
                          {
                            pIni->m_byHour[1] = static_cast<unsigned __int8>(value);
                            value = GetPrivateProfileIntA(
                              pEventType,
                              "END_MINUTE",
                              0,
                              "./Initialize/cash_discount_limitsale.ini");
                            if (value < 0x3C)
                            {
                              pIni->m_byMinute[1] = static_cast<unsigned __int8>(value);
                              memset_0(&tmValue, 0, sizeof(tmValue));
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
                                if (value < 0x18)
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
                                if (value < 0x3C)
                                {
                                  pIni->m_byMinute[2] = static_cast<unsigned __int8>(value);
                                }
                                else
                                {
                                  pIni->m_bRepeat = 0;
                                }
                                memset_0(&tmValue, 0, sizeof(tmValue));
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
    if (value >= 0x18)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byHour[0] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "BEGIN_MINUTE", 0, "./initialize/cash_discount.ini");
    if (value >= 0x3C)
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
    if (value >= 0x18)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byHour[1] = static_cast<unsigned __int8>(value);
    value = GetPrivateProfileIntA(pEventType, "END_MINUTE", 0, "./initialize/cash_discount.ini");
    if (value >= 0x3C)
    {
      pIni->m_bUseCashEvent = 0;
      return;
    }
    pIni->m_byMinute[1] = static_cast<unsigned __int8>(value);

    if (!strcmp_0(pEventType, "Set"))
    {
      value = GetPrivateProfileIntA(pEventType, "SetA_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 0x64)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[0] = static_cast<unsigned __int8>(value);
      value = GetPrivateProfileIntA(pEventType, "SetB_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 0x64)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[1] = static_cast<unsigned __int8>(value);
      value = GetPrivateProfileIntA(pEventType, "SetC_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 0x64)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[2] = static_cast<unsigned __int8>(value);
      value = GetPrivateProfileIntA(pEventType, "SetAll_Discout", 0, "./initialize/cash_discount.ini");
      if (value >= 0x64)
      {
        pIni->m_bUseCashEvent = 0;
        return;
      }
      pIni->m_byDiscout[3] = static_cast<unsigned __int8>(value);
    }

    memset_0(&tmValue, 0, sizeof(tmValue));
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
      if (value < 0x18)
      {
        pIni->m_byHour[2] = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      value = GetPrivateProfileIntA(pEventType, "EXPIRE_MINUTE", 0, "./initialize/cash_discount.ini");
      if (value < 0x3C)
      {
        pIni->m_byMinute[2] = static_cast<unsigned __int8>(value);
      }
      else
      {
        pIni->m_bRepeat = 0;
      }
      memset_0(&tmValue, 0, sizeof(tmValue));
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
  if (pIni && pft && strcmp_0(pEventType, "NULL"))
  {
    char codeKey[20];
    char countKey[20];
    memset_0(codeKey, 0, sizeof(codeKey));
    memset_0(countKey, 0, sizeof(countKey));

    if (!strcmp_0(pEventType, "Limit Sale"))
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
          0x40u,
          "./Initialize/cash_discount_limitsale.ini");
        int value = GetPrivateProfileIntA(
          pEventType,
          countKey,
          0,
          "./Initialize/cash_discount_limitsale.ini");
        if (value > 0x7D0)
        {
          pIni->m_bUseCashEvent = 0;
        }
        pIni->m_Limited_sale[j].m_wLimcount = static_cast<unsigned __int16>(value);
      }
      int value = GetPrivateProfileIntA(pEventType, "Discout", 0, "./Initialize/cash_discount_limitsale.ini");
      if (value >= 0x64)
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
  memcpy_0(&m_lim_event_New, &m_lim_event, sizeof(m_lim_event_New));

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
  if (g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x9Eu, reinterpret_cast<char *>(&pQryData), nSize))
  {
    memcpy_0(&m_lim_event_Old, &m_lim_event_New, sizeof(m_lim_event_Old));
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
  memcpy_0(&tmp, &m_lim_event_New, sizeof(tmp));
  memcpy_0(&m_lim_event_Old, &tmp, sizeof(m_lim_event_Old));
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

char CashItemRemoteStore::LimitedSale_check_count(unsigned __int8 byTableCode, unsigned int dwIndex)
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
  return 1;
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
      Set_CashEvent_Status(byEventType, 1u);

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
          Set_CashEvent_Status(byEventType, 2u);
        }
        else
        {
          Set_CashEvent_Status(byEventType, 3u);
        }
      }
      else
      {
        Set_CashEvent_Status(byEventType, 4u);
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

  memset_0(&tmValue, 0, sizeof(tmValue));
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
    6u,
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
