#include "pch.h"

#include "CGoldenBoxItemMgr.h"

#include <ctime>
#include <new>

#include "CAsyncLogger.h"
#include "CHolyStoneSystem.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "OreItem_fld.h"
#include "WorldServerUtil.h"
#include "announ_message_receipt_udp.h"

_goldbox_index::_goldbox_index()
{
  memset_0(this, 0, sizeof(_goldbox_index));
}

_golden_box_item_ini::_golden_box_item_ini()
{
  memset_0(this, 0, sizeof(_golden_box_item_ini));
}

_golden_box_item_event::_golden_box_item_event()
{
  memset_0(this, 0, sizeof(_golden_box_item_event));
}

_golden_box_item_event::~_golden_box_item_event()
{
}

_golden_box_item::_golden_box_item()
{
  memset_0(this, 0, sizeof(_golden_box_item));
}

_db_golden_box_item::_db_golden_box_item()
{
  memset_0(this, 0, sizeof(_db_golden_box_item));
}

CGoldenBoxItemMgr::CGoldenBoxItemMgr()
{
  m_nDBSerial = 0;
}

CGoldenBoxItemMgr::~CGoldenBoxItemMgr()
{
  m_tmLoopTimer.StopTimer();
}

CGoldenBoxItemMgr *CGoldenBoxItemMgr::Instance()
{
  static CGoldenBoxItemMgr s_instance;
  return &s_instance;
}

bool CGoldenBoxItemMgr::Initialize()
{
  _init_loggers();
  m_tmLoopTimer.BeginTimer(0x2710u);
  if (!Load_Golden_Box_Item_Event())
    return false;

  GetLocalTime(&tm1);
  GetLocalTime(&tm);
  m_bInit = false;
  return true;
}

void CGoldenBoxItemMgr::Loop_Event()
{
  if (!g_Main.m_pWorldDB || !g_Main.m_pWorldDB->IsConectionActive())
  {
    return;
  }

  GetLocalTime(&tm);
  if (!tm.wHour && !tm.wMinute)
  {
    if (tm1.wMinute >= tm.wMinute && tm1.wSecond >= tm.wSecond && tm1.wMilliseconds >= tm.wMilliseconds)
    {
      m_bInit = false;
    }

    if (!m_bInit)
    {
      const unsigned __int16 starterBoxCount = Get_StarterBox_Count();
      Set_StarterBox_Count(starterBoxCount, false);
      Set_StarterBox_Count(m_wStarterBoxNum, true);
      Set_ToStruct();
      GetLocalTime(&tm1);
      m_bInit = true;
    }
  }

  if (m_tmLoopTimer.CountingTimer())
  {
    Check_Event_Status();
  }

  if (g_Main.m_bWorldOpen && m_golden_box_event.m_event_timer.CountingTimer() && m_golden_box_event.m_event_status == 2)
  {
    PushDQSUpdate();
  }
}

void CGoldenBoxItemMgr::Check_Event_Status()
{
  const unsigned __int8 eventStatus = Get_Event_Status();
  __time32_t currentTime = 0;
  _time32(&currentTime);

  if (!eventStatus)
  {
    if (!m_golden_box_event.m_ini.m_bUse_event)
    {
      Set_Event_Status(0);
      m_golden_box_item.m_bydck = 0;
      Set_ToStruct();
    }
    return;
  }

  if (eventStatus == 1)
  {
    if (currentTime >= m_golden_box_event.m_ini.m_EventTime[0])
    {
      Set_Event_Status(2);
      m_golden_box_item.m_bydck = 1;
      Set_ToStruct();
    }
    return;
  }

  if (eventStatus == 2 && m_golden_box_event.m_ini.m_EventTime[1] <= currentTime)
  {
    Set_Event_Status(3);
    m_golden_box_item.m_bydck = 0;
    Set_ToStruct();
  }
}

void CGoldenBoxItemMgr::PushDQSUpdate()
{
  constexpr size_t kPayloadSize = 4 + sizeof(_db_golden_box_item) + sizeof(_db_golden_box_item);
  char payload[kPayloadSize]{};
  *reinterpret_cast<int *>(payload) = m_nDBSerial;
  memcpy_0(payload + 4, &m_golden_box_item_New, sizeof(m_golden_box_item_New));
  memcpy_0(
    payload + 4 + sizeof(m_golden_box_item_New),
    &m_golden_box_item_Old,
    sizeof(m_golden_box_item_Old));

  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0xA5u, payload, static_cast<int>(sizeof(payload)));
}

bool CGoldenBoxItemMgr::_init_loggers()
{
  auto *logger = CAsyncLogger::Instance();
  logger->Regist(
    ALT_GETGOLDBAR_LOG,
    "..\\ZoneServerLog\\ServiceLog\\GoldBoxSystem",
    "GetGoldBarHistory",
    true,
    0x5265C00u);

  logger = CAsyncLogger::Instance();
  logger->Regist(
    ALT_GETEVENTCOUPON_LOG,
    "..\\ZoneServerLog\\ServiceLog\\GoldBoxSystem",
    "GetCouponHistory",
    true,
    0x5265C00u);

  CreateDirectoryA("..\\ZoneServerLog\\SystemLog\\GoldenAgeEvent", nullptr);

  char filePath[256];
  memset(filePath, 0, sizeof(filePath));
  const unsigned int now = GetKorLocalTime();
  sprintf_s(filePath, sizeof(filePath), "..\\ZoneServerLog\\SystemLog\\GoldenAgeEvent\\%u.his", now);
  _kLogger.SetWriteLogFile(filePath, 1, 0, 1, 1);
  return true;
}

void CGoldenBoxItemMgr::BoxItemDataCopy()
{
  _db_golden_box_item temp{};
  memcpy_0(&temp, &m_golden_box_item_New, sizeof(m_golden_box_item_New));
  memcpy_0(&m_golden_box_item_Old, &temp, sizeof(m_golden_box_item_Old));
}

_db_golden_box_item *CGoldenBoxItemMgr::GetGodBoxItemInfoPtr()
{
  return &m_temp_db;
}

void CGoldenBoxItemMgr::SetDBSerial(int nDBSerial)
{
  m_nDBSerial = nDBSerial;
}

char CGoldenBoxItemMgr::SynchINIANDDB()
{
  for (int j = 0; j < 2; ++j)
  {
    if (!m_temp_db.bygolden_item_num[j])
    {
      return 0;
    }
  }

  for (int k = 0; k < 2; ++k)
  {
    if (m_temp_db.nBoxcode[k] <= 0)
    {
      return 0;
    }
  }

  for (int m = 0; m < 2; ++m)
  {
    for (int n = 0; n < m_temp_db.bygolden_item_num[m]; ++n)
    {
      if (m_temp_db.List[m][n].ncode <= 0)
      {
        return 0;
      }
    }
  }

  if (!m_temp_db.bydck)
  {
    return 0;
  }

  m_golden_box_item.m_bydck = m_temp_db.bydck;
  m_golden_box_item.m_dwStarterBoxCnt = m_temp_db.dwStarterBoxCnt;

  for (int ii = 0; ii < 2; ++ii)
  {
    m_golden_box_item.m_wBoxMax[ii] = m_temp_db.wBoxMax[ii];
  }

  for (int jj = 0; jj < 2; ++jj)
  {
    m_golden_box_item.m_byBoxTableCode[jj] = static_cast<unsigned __int8>((m_temp_db.nBoxcode[jj] >> 8) & 0xFF);
    m_golden_box_item.m_dwBoxIndex[jj] = static_cast<unsigned __int16>((m_temp_db.nBoxcode[jj] >> 16) & 0xFFFF);
    m_golden_box_item.m_wBoxMax[jj] = m_temp_db.wBoxMax[jj];
    m_golden_box_item.m_bygolden_item_num[jj] = m_temp_db.bygolden_item_num[jj];

    for (int kk = 0; kk < m_temp_db.bygolden_item_num[jj]; ++kk)
    {
      _db_golden_box_item::_db_golden_box_item_List *listEntry = &m_temp_db.List[jj][kk];
      m_golden_box_item.m_golden_box_item_info[jj][kk].m_byTableCode = static_cast<unsigned __int8>((listEntry->ncode >> 8) & 0xFF);
      m_golden_box_item.m_golden_box_item_info[jj][kk].m_dwIndex = static_cast<unsigned __int16>((listEntry->ncode >> 16) & 0xFFFF);
      m_golden_box_item.m_golden_box_item_info[jj][kk].m_wNum = m_temp_db.List[jj][kk].wcount;
    }
  }

  Set_ToStruct();

  unsigned __int8 temp[sizeof(m_golden_box_item_New)]{};
  memcpy_0(temp, &m_golden_box_item_New, sizeof(m_golden_box_item_New));
  memcpy_0(&m_golden_box_item_Old, temp, sizeof(m_golden_box_item_Old));
  return 1;
}

bool CGoldenBoxItemMgr::Load_Golden_Box_Item_Event()
{
  m_golden_box_event.m_event_timer.BeginTimer(0x1D4C0u);
  m_golden_box_event.m_event_log.Write("Server Start and LogFile Loading");
  Set_Event_Status(0);
  if (Load_Event_INI(&m_golden_box_event.m_ini))
  {
    Check_Loaded_Event_Status();
    return !Get_Event_Status() || SetGoldBoxItemIndex();
  }

  MyMessageBox("CGoldenBoxItemMgr::Load_Event_INI() : ", "Load_Golden_Box_Item_Event() Fail!");
  return false;
}

void CGoldenBoxItemMgr::Set_Event_Status(unsigned __int8 byStatus)
{
  const int oldStatus = m_golden_box_event.m_event_status;
  m_golden_box_event.m_event_log.Write("[EventStateChange : %d -> %d ]", oldStatus, byStatus);
  m_golden_box_event.m_event_status = byStatus;
}

bool CGoldenBoxItemMgr::Load_Event_INI(_golden_box_item_ini *pIni)
{
  if (!pIni)
    return false;

  UINT ret = 0;
  char keyItem[20];
  char keyNum[20];
  char keyRate[20];
  std::tm tmValue{};
  char buffer[60];

  memset_0(keyItem, 0, sizeof(keyItem));
  memset_0(keyNum, 0, sizeof(keyNum));
  memset_0(keyRate, 0, sizeof(keyRate));

  pIni->m_bUse_event = 0;
  pIni->m_EventTime[0] = 0;
  pIni->m_EventTime[1] = 0;

  ret = GetPrivateProfileIntA("GoldBox_Item", "USE", 1, "./initialize/GoldenBox_Item.ini");
  if (ret == 1)
  {
    pIni->m_bUse_event = 0;
    return true;
  }

  pIni->m_bUse_event = 1;
  pIni->m_dwStarterBoxCnt =
    GetPrivateProfileIntA("GoldBox_Item", "StarterBox", 1000, "./initialize/GoldenBox_Item.ini");
  m_wStarterBoxNum = pIni->m_dwStarterBoxCnt;

  ret = GetPrivateProfileStringA(
    "GoldBox_Item",
    "StarterBoxCode_0",
    "NULL",
    pIni->m_szStarterBoxCode[0],
    0x40u,
    "./initialize/GoldenBox_Item.ini");
  if (!strcmp_0(pIni->m_szStarterBoxCode[0], "NULL"))
  {
    pIni->m_bUse_event = 0;
    return false;
  }

  ret = GetPrivateProfileStringA(
    "GoldBox_Item",
    "StarterBoxCode_1",
    "NULL",
    pIni->m_szStarterBoxCode[1],
    0x40u,
    "./initialize/GoldenBox_Item.ini");
  if (!strcmp_0(pIni->m_szStarterBoxCode[1], "NULL"))
  {
    pIni->m_bUse_event = 0;
    return false;
  }

  int loopCount = GetPrivateProfileIntA("GoldBox_Item", "GoldBox_Num", 0, "./initialize/GoldenBox_Item.ini");
  if (loopCount <= 0 || loopCount > 255)
    return false;

  pIni->m_byLoopCnt = static_cast<unsigned __int8>(loopCount);
  ret = GetPrivateProfileIntA("GoldBox_Item", "BEGIN_YEAR", 0, "./initialize/GoldenBox_Item.ini");
  if (!ret)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_wYear[0] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "BEGIN_MONTH", 0, "./initialize/GoldenBox_Item.ini");
  if (!ret)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byMonth[0] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "BEGIN_DAY", 0, "./initialize/GoldenBox_Item.ini");
  if (!ret)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byDay[0] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "BEGIN_HOUR", 0, "./initialize/GoldenBox_Item.ini");
  if (ret >= 0x18)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byHour[0] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "BEGIN_MINUTE", 0, "./initialize/GoldenBox_Item.ini");
  if (ret >= 0x3C)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byMinute[0] = ret;

  memset_0(&tmValue, 0, sizeof(tmValue));
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
    pIni->m_bUse_event = 0;
    return false;
  }

  ret = GetPrivateProfileIntA("GoldBox_Item", "END_YEAR", 0, "./initialize/GoldenBox_Item.ini");
  if (!ret)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_wYear[1] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "END_MONTH", 0, "./initialize/GoldenBox_Item.ini");
  if (!ret)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byMonth[1] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "END_DAY", 0, "./initialize/GoldenBox_Item.ini");
  if (!ret)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byDay[1] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "END_HOUR", 0, "./initialize/GoldenBox_Item.ini");
  if (ret >= 0x18)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byHour[1] = ret;

  ret = GetPrivateProfileIntA("GoldBox_Item", "END_MINUTE", 0, "./initialize/GoldenBox_Item.ini");
  if (ret >= 0x3C)
  {
    pIni->m_bUse_event = 0;
    return false;
  }
  pIni->m_byMinute[1] = ret;

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
    pIni->m_bUse_event = 0;
    return false;
  }

  memset(buffer, 0, 50);
  for (int j = 0; j < pIni->m_byLoopCnt; ++j)
  {
    memset_0(buffer, 0, 0x32uLL);
    sprintf_s(buffer, 0x32uLL, "BoxItem_Code_%d", j);
    ret = GetPrivateProfileStringA(
      "GoldBox_Item",
      buffer,
      "NULL",
      pIni->m_szGoldenBoxcode[static_cast<__int64>(j)],
      0x40u,
      "./initialize/GoldenBox_Item.ini");
    if (!strcmp_0(pIni->m_szGoldenBoxcode[static_cast<__int64>(j)], "NULL"))
    {
      pIni->m_bUse_event = 0;
      return false;
    }

    memset_0(buffer, 0, 0x32uLL);
    sprintf_s(buffer, 0x32uLL, "Box_Max_%d", j);
    ret = GetPrivateProfileIntA("GoldBox_Item", buffer, 0, "./initialize/GoldenBox_Item.ini");
    if (ret > 0xEA60)
    {
      pIni->m_bUse_event = 0;
      return false;
    }
    pIni->m_wGoldenBoxmax[j] = static_cast<unsigned __int16>(ret);

    memset_0(buffer, 0, 0x32uLL);
    sprintf_s(buffer, 0x32uLL, "GoldBox_Item_Out_List_%d", j);
    ret = GetPrivateProfileIntA(buffer, "List_Count", 0, "./initialize/GoldenBox_Item.ini");
    if (ret > 0x64)
    {
      pIni->m_bUse_event = 0;
      return false;
    }
    pIni->m_bygolden_item_num[j] = static_cast<unsigned __int8>(ret);

    for (int k = 0; k < pIni->m_bygolden_item_num[j]; ++k)
    {
      sprintf_s(keyItem, sizeof(keyItem), "Item%d", k);
      sprintf_s(keyNum, sizeof(keyNum), "Num%d", k);
      sprintf_s(keyRate, sizeof(keyRate), "Rate%d", k);

      memset_0(buffer, 0, 0x32uLL);
      sprintf_s(buffer, 0x32uLL, "GoldBox_Item_Out_List_%d", j);
      GetPrivateProfileStringA(
        buffer,
        keyItem,
        "NULL",
        pIni->m_golden_box_item_list[j][k].m_szLimcode,
        0x40u,
        "./initialize/GoldenBox_Item.ini");
      if (!strcmp_0(pIni->m_golden_box_item_list[j][k].m_szLimcode, "NULL"))
      {
        pIni->m_bUse_event = 0;
        return false;
      }

      ret = GetPrivateProfileIntA(buffer, keyNum, 0, "./initialize/GoldenBox_Item.ini");
      if (ret > 0xEA60)
      {
        pIni->m_bUse_event = 0;
        return false;
      }
      pIni->m_golden_box_item_list[j][k].m_wLimcount = static_cast<unsigned __int16>(ret);

      ret = GetPrivateProfileIntA(buffer, keyRate, 0, "./initialize/GoldenBox_Item.ini");
      if (ret >= 0x2710)
      {
        pIni->m_bUse_event = 0;
        return false;
      }
      pIni->m_golden_box_item_list[j][k].m_wRate = static_cast<unsigned __int16>(ret);
    }
  }

  Set_FromINIToStruct(pIni);
  return true;
}

void CGoldenBoxItemMgr::Check_Loaded_Event_Status()
{
  if (!m_golden_box_event.m_ini.m_bUse_event)
  {
    Set_Event_Status(0);
    return;
  }

  __time32_t timeValue[4];
  _time32(timeValue);
  if (timeValue[0] > m_golden_box_event.m_ini.m_EventTime[1])
  {
    Set_Event_Status(3u);
    return;
  }

  if (timeValue[0] < m_golden_box_event.m_ini.m_EventTime[0]
      || timeValue[0] > m_golden_box_event.m_ini.m_EventTime[1])
  {
    if (timeValue[0] < m_golden_box_event.m_ini.m_EventTime[0])
    {
      Set_Event_Status(1u);
      m_golden_box_event.m_event_log.Write(
        "[golden_box_item_event Loaded] [EventState : %d] [EventTime : %d/%d/%d %d:%d  ~ %d/%d/%d %d:%d ]",
        m_golden_box_event.m_event_status,
        m_golden_box_event.m_ini.m_wYear[0],
        m_golden_box_event.m_ini.m_byMonth[0],
        m_golden_box_event.m_ini.m_byDay[0],
        m_golden_box_event.m_ini.m_byHour[0],
        m_golden_box_event.m_ini.m_byMinute[0],
        m_golden_box_event.m_ini.m_wYear[1],
        m_golden_box_event.m_ini.m_byMonth[1],
        m_golden_box_event.m_ini.m_byDay[1],
        m_golden_box_event.m_ini.m_byHour[1],
        m_golden_box_event.m_ini.m_byMinute[1]);
    }
    return;
  }

  const int remain = m_golden_box_event.m_ini.m_EventTime[1] - timeValue[0];
  if (remain <= 0)
  {
    Set_Event_Status(3u);
    return;
  }

  Set_Event_Status(2u);
}

bool CGoldenBoxItemMgr::SetGoldBoxItemIndex()
{
  unsigned __int16 zeroCount = 0;
  const unsigned __int8 loopCount = GetLoopCount();
  const size_t count = loopCount;

  _goldbox_index *items = nullptr;
  if (count != 0)
    items = new (std::nothrow) _goldbox_index[count];
  else
    items = new (std::nothrow) _goldbox_index[0];

  m_pItemIndex = items;

  for (int n = 0;; ++n)
  {
    const int recordNum = g_Main.m_tblItemData[17].GetRecordNum();
    if (n >= recordNum)
      break;
    _OreItem_fld *record = reinterpret_cast<_OreItem_fld *>(g_Main.m_tblItemData[17].GetRecord(n));
    if (!record->m_dwOreProbability)
      ++zeroCount;
  }

  const int totalExpected = static_cast<int>(GetLoopCount()) + zeroCount;
  if (totalExpected != g_Main.m_tblItemData[17].GetRecordNum())
    return false;

  int recordIndex = 0;
  int itemIndex = 0;
  while (true)
  {
    const int recordNum = g_Main.m_tblItemData[17].GetRecordNum();
    if (recordIndex >= recordNum)
      break;

    _OreItem_fld *record = reinterpret_cast<_OreItem_fld *>(g_Main.m_tblItemData[17].GetRecord(recordIndex));
    if (record)
    {
      if (record->m_dwOreProbability)
      {
        strcpy_0(m_pItemIndex[itemIndex].szItemCode, record->m_strCode);
        m_pItemIndex[itemIndex++].wItemIndex = record->m_dwIndex;
      }

      if (itemIndex > GetLoopCount())
        return false;
    }

    ++recordIndex;
  }

  return true;
}

unsigned __int8 CGoldenBoxItemMgr::GetLoopCount()
{
  return m_golden_box_event.m_ini.m_byLoopCnt;
}

unsigned __int16 CGoldenBoxItemMgr::GetGoldBoxItemIndex(unsigned __int16 wIndex)
{
  return m_pItemIndex[wIndex].wItemIndex;
}

unsigned __int16 CGoldenBoxItemMgr::Get_Box_Count(unsigned __int8 byIndex)
{
  return m_golden_box_item.m_wBoxMax[byIndex];
}

void CGoldenBoxItemMgr::Set_Box_Count(unsigned __int8 byIndex)
{
  if (m_golden_box_item.m_wBoxMax[byIndex] != 0)
  {
    --m_golden_box_item.m_wBoxMax[byIndex];
    Set_ToStruct();
  }
}

unsigned __int16 CGoldenBoxItemMgr::Get_BoxItem_Count(unsigned __int8 byIndex, unsigned int dwIndex)
{
  for (int j = 0; j < m_golden_box_item.m_bygolden_item_num[byIndex]; ++j)
  {
    if (m_golden_box_item.m_golden_box_item_info[byIndex][j].m_dwIndex == dwIndex)
    {
      return m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wNum;
    }
  }
  return 0;
}

void CGoldenBoxItemMgr::RateCheck(unsigned __int8 byIndex)
{
  unsigned __int16 zeroRateTotal = 0;
  int zeroCount = 0;

  for (int j = 0; j < m_golden_box_item.m_bygolden_item_num[byIndex]; ++j)
  {
    if (!Get_BoxItem_Count(byIndex, m_golden_box_item.m_golden_box_item_info[byIndex][j].m_dwIndex))
    {
      zeroRateTotal += m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wRate;
      m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wRate = 0;
      ++zeroCount;
    }
  }

  if (zeroCount >= m_golden_box_item.m_bygolden_item_num[byIndex])
  {
    zeroCount = 0;
  }
  const __int16 addRate = static_cast<__int16>(
    zeroRateTotal / (m_golden_box_item.m_bygolden_item_num[byIndex] - zeroCount));

  for (int j = 0; j < m_golden_box_item.m_bygolden_item_num[byIndex]; ++j)
  {
    if (m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wRate)
    {
      m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wRate += addRate;
    }
  }
}

char *CGoldenBoxItemMgr::GetGoldBoxItemPtr()
{
  return m_golden_box_event.m_ini.m_szGoldenBoxcode[0];
}

char *CGoldenBoxItemMgr::BoxItemOpen(unsigned __int8 byIndex)
{
  const unsigned int randomRate = rand() % 10000;
  unsigned int accRate = 0;

  RateCheck(byIndex);
  const unsigned __int64 count = m_golden_box_item.m_bygolden_item_num[byIndex];
  m_pBoxItemOpen = static_cast<_BoxItemOpen_output *>(operator new[](saturated_mul(count, 0x10uLL)));
  for (int j = 0; j < m_golden_box_item.m_bygolden_item_num[byIndex]; ++j)
  {
    m_pBoxItemOpen[j].m_dwProb = m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wRate;
    strcpy_0(
      m_pBoxItemOpen[j].m_itmPdOutput,
      m_golden_box_event.m_ini.m_golden_box_item_list[byIndex][j].m_szLimcode);
    m_pBoxItemOpen[j].m_nPdProCnt = 1;

    char *itemCode = m_pBoxItemOpen[j].m_itmPdOutput;
    if (!itemCode)
    {
      return nullptr;
    }
    if (strlen_0(itemCode) < 2 || !strncmp(itemCode, "-", 1u) || !strncmp(itemCode, "-1", 2u))
    {
      return nullptr;
    }

    accRate += m_pBoxItemOpen[j].m_dwProb;
    if (Get_BoxItem_Count(byIndex, m_golden_box_item.m_golden_box_item_info[byIndex][j].m_dwIndex)
        && randomRate < accRate)
    {
      return itemCode;
    }
  }

  return nullptr;
}

void CGoldenBoxItemMgr::Set_BoxItem_Count(unsigned __int8 byIndex, unsigned int dwIndex)
{
  for (int j = 0; j < m_golden_box_item.m_bygolden_item_num[byIndex]; ++j)
  {
    if (m_golden_box_item.m_golden_box_item_info[byIndex][j].m_dwIndex == dwIndex)
    {
      if (!m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wNum)
      {
        return;
      }
      --m_golden_box_item.m_golden_box_item_info[byIndex][j].m_wNum;
      break;
    }
  }
  Set_ToStruct();
}

void CGoldenBoxItemMgr::BoxItemOpenEffectType(
  char *szUseItem,
  char *szNewItem,
  unsigned __int8 *pbyType,
  bool *bCircle)
{
  *bCircle = true;
  if (!strcmp_0(szUseItem, "bxgol01"))
  {
    *pbyType = 9;
  }
  else if (!strcmp_0(szUseItem, "bxgol02"))
  {
    *pbyType = 10;
  }
  else if (!strcmp_0(szUseItem, "bxgol03"))
  {
    *pbyType = 11;
  }
  else if (!strcmp_0(szUseItem, "bxgol05"))
  {
    *pbyType = 10;
  }
  else if (!strcmp_0(szUseItem, "iogld01"))
  {
    if (!strcmp_0(szNewItem, "iygld01") || !strcmp_0(szNewItem, "iygld02"))
    {
      *pbyType = 8;
      *bCircle = false;
    }
  }
  else if (!strcmp_0(szUseItem, "iogld02"))
  {
    *pbyType = 12;
  }
  else if (!strcmp_0(szUseItem, "bxgol04"))
  {
    if (!strcmp_0(szNewItem, "iygld11"))
    {
      *pbyType = 6;
      *bCircle = false;
    }
    else if (!strcmp_0(szNewItem, "iygld12"))
    {
      *pbyType = 7;
    }
  }
  else
  {
    *pbyType = 0;
  }
}

void CGoldenBoxItemMgr::Set_FromINIToStruct(_golden_box_item_ini *pIni)
{
  if (!pIni)
    return;

  m_golden_box_item.m_dwStarterBoxCnt = pIni->m_dwStarterBoxCnt;
  for (int j = 0; j < m_golden_box_event.m_ini.m_byLoopCnt; ++j)
  {
    unsigned __int8 tableCode = GetItemTableCode(pIni->m_szGoldenBoxcode[static_cast<__int64>(j)]);
    if (tableCode == 255)
      return;

    _base_fld *record =
      g_Main.m_tblItemData[tableCode].GetRecord(pIni->m_szGoldenBoxcode[static_cast<__int64>(j)]);
    if (!record)
      return;

    m_golden_box_item.m_byBoxTableCode[j] = tableCode;
    m_golden_box_item.m_dwBoxIndex[j] = record->m_dwIndex;
    m_golden_box_item.m_wBoxMax[j] = pIni->m_wGoldenBoxmax[j];
    m_golden_box_item.m_bygolden_item_num[j] = pIni->m_bygolden_item_num[j];

    for (int k = 0; k < m_golden_box_item.m_bygolden_item_num[j]; ++k)
    {
      m_golden_box_item.m_golden_box_item_info[j][k].m_byTableCode = 0xFF;
      m_golden_box_item.m_golden_box_item_info[j][k].m_dwIndex = 255;
      m_golden_box_item.m_golden_box_item_info[j][k].m_wNum = pIni->m_golden_box_item_list[j][k].m_wLimcount;
      m_golden_box_item.m_golden_box_item_info[j][k].m_wRate = pIni->m_golden_box_item_list[j][k].m_wRate;

      tableCode = GetItemTableCode(pIni->m_golden_box_item_list[j][k].m_szLimcode);
      if (tableCode == 255)
        return;

      _base_fld *itemRecord =
        g_Main.m_tblItemData[tableCode].GetRecord(pIni->m_golden_box_item_list[j][k].m_szLimcode);
      if (!itemRecord)
        return;

      m_golden_box_item.m_golden_box_item_info[j][k].m_byTableCode = tableCode;
      m_golden_box_item.m_golden_box_item_info[j][k].m_dwIndex = itemRecord->m_dwIndex;
    }
  }

  Set_ToStruct();
}

void CGoldenBoxItemMgr::Set_ToStruct()
{
  m_golden_box_item_New.bydck = m_golden_box_item.m_bydck;
  m_golden_box_item_New.dwStarterBoxCnt = m_golden_box_item.m_dwStarterBoxCnt;

  for (int j = 0; j < m_golden_box_event.m_ini.m_byLoopCnt; ++j)
  {
    _INVENKEY boxKey(0, m_golden_box_item.m_byBoxTableCode[j], m_golden_box_item.m_dwBoxIndex[j]);
    m_golden_box_item_New.nBoxcode[j] = boxKey.CovDBKey();
    m_golden_box_item_New.wBoxMax[j] = m_golden_box_item.m_wBoxMax[j];
    m_golden_box_item_New.bygolden_item_num[j] = m_golden_box_item.m_bygolden_item_num[j];

    for (int k = 0; k < m_golden_box_item.m_bygolden_item_num[j]; ++k)
    {
      _INVENKEY itemKey(
        0,
        m_golden_box_item.m_golden_box_item_info[j][k].m_byTableCode,
        m_golden_box_item.m_golden_box_item_info[j][k].m_dwIndex);
      m_golden_box_item_New.List[j][k].ncode = itemKey.CovDBKey();
      m_golden_box_item_New.List[j][k].wcount = m_golden_box_item.m_golden_box_item_info[j][k].m_wNum;
    }
  }
}

unsigned __int8 CGoldenBoxItemMgr::IsBuyRaceBossGoldBox(CPlayer *pOne)
{
  const int raceCode = pOne->m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (pOne->m_dwObjSerial != rankingSystem->GetCurrentRaceBossSerial(raceCode, 0))
  {
    return 29;
  }
  if (raceCode != g_HolySys.GetHolyMasterRace())
  {
    return 30;
  }
  if (!g_HolySys.GetGoldBoxConsumable())
  {
    return 31;
  }
  if (Get_Event_Status() == 2)
  {
    return 0;
  }
  return 17;
}

unsigned __int8 CGoldenBoxItemMgr::Get_Event_Status()
{
  return m_golden_box_event.m_event_status;
}

unsigned __int16 CGoldenBoxItemMgr::Get_StarterBox_Count()
{
  return static_cast<unsigned __int16>(m_golden_box_item.m_dwStarterBoxCnt);
}

char *CGoldenBoxItemMgr::GetStarterBoxCode(unsigned __int16 wIndex)
{
  return m_golden_box_event.m_ini.m_szStarterBoxCode[wIndex];
}

bool CGoldenBoxItemMgr::StarterBox_InsertToInven(CPlayer *pOne, char *szItemCode)
{
  const unsigned __int8 itemTableCode = GetItemTableCode(szItemCode);
  if (itemTableCode == 0xFF)
  {
    return false;
  }

  unsigned __int16 *record =
    reinterpret_cast<unsigned __int16 *>(g_Main.m_tblItemData[itemTableCode].GetRecord(szItemCode));
  return record && _insert_to_inven(pOne, itemTableCode, *record);
}

char CGoldenBoxItemMgr::_insert_to_inven(
  CPlayer *pOne,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex)
{
  _base_fld *record = g_Main.m_tblItemData[byTableCode].GetRecord(wItemIndex);
  if (!record)
  {
    return 0;
  }

  _STORAGE_LIST::_db_con item{};
  item.m_byTableCode = byTableCode;
  item.m_wItemIndex = wItemIndex;
  if (IsOverLapItem(byTableCode))
  {
    item.m_dwDur = 1;
  }
  else
  {
    item.m_dwDur = GetItemDurPoint(byTableCode, wItemIndex);
  }

  const unsigned int upSocketNum = GetDefItemUpgSocketNum(byTableCode, wItemIndex);
  item.m_dwLv = GetBitAfterSetLimSocket(upSocketNum);
  item.m_wSerial = pOne->m_Param.GetNewItemSerial();

  _STORAGE_LIST::_db_con *addedItem = pOne->Emb_AddStorage(0, &item, 0, 1);
  if (!addedItem)
  {
    const char *itemName = GetItemKorName(byTableCode, wItemIndex);
    _kLogger.Write("Failed _insert_to_inven() >> %s", itemName);
    return 0;
  }

  pOne->SendMsg_RewardAddItem(addedItem, 0xAu);
  char *charName = pOne->m_Param.GetCharNameA();
  const unsigned int charSerial = pOne->m_Param.GetCharSerial();
  pOne->SendMsg_Notify_Get_Golden_Box(2u, charSerial, charName, addedItem, 0);
  return 1;
}

void CGoldenBoxItemMgr::Set_StarterBox_Count(unsigned int dwNum, bool bAdd)
{
  if (bAdd)
  {
    m_golden_box_item.m_dwStarterBoxCnt += dwNum;
  }
  else
  {
    if (!m_golden_box_item.m_dwStarterBoxCnt)
    {
      return;
    }

    if (m_golden_box_item.m_dwStarterBoxCnt == dwNum)
    {
      m_golden_box_item.m_dwStarterBoxCnt = 0;
    }
    else
    {
      m_golden_box_item.m_dwStarterBoxCnt -= dwNum;
    }
  }

  Set_ToStruct();
}

void CGoldenBoxItemMgr::WriteGetGoldBarLog(CPlayer *pOne, _STORAGE_LIST::_db_con *pItem)
{
  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  char *itemKorName = GetItemKorName(pItem->m_byTableCode, pItem->m_wItemIndex);
  char *itemCode = record->m_strCode;
  CUserDB *userDB = pOne->m_pUserDB;
  char *accountID = pOne->m_pUserDB->m_szAccountID;
  const unsigned int charSerial = pOne->m_Param.GetCharSerial();
  char *charName = pOne->m_Param.GetCharNameA();

  CAsyncLogger::Instance()->FormatLog(
    ALT_GETGOLDBAR_LOG,
    "Name: %s(%d), AccountID: %s(%d), Item: %s(%s) ",
    charName,
    charSerial,
    accountID,
    userDB->m_dwAccountSerial,
    itemCode,
    itemKorName);
}

void CGoldenBoxItemMgr::WriteEventCouponLog(CPlayer *pOne, _STORAGE_LIST::_db_con *pItem)
{
  _base_fld *record = g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
  char *itemKorName = GetItemKorName(pItem->m_byTableCode, pItem->m_wItemIndex);
  char *itemCode = record->m_strCode;
  CUserDB *userDB = pOne->m_pUserDB;
  char *accountID = pOne->m_pUserDB->m_szAccountID;
  const unsigned int charSerial = pOne->m_Param.GetCharSerial();
  char *charName = pOne->m_Param.GetCharNameA();

  CAsyncLogger::Instance()->FormatLog(
    ALT_GETEVENTCOUPON_LOG,
    "Name: %s(%d), AccountID: %s(%d), Item: %s(%s) ",
    charName,
    charSerial,
    accountID,
    userDB->m_dwAccountSerial,
    itemCode,
    itemKorName);
}

void CGoldenBoxItemMgr::SendMsg_RaceChat(CPlayer *pOne, char *pwszChatData)
{
  _announ_message_receipt_udp msg{};
  msg.byMessageType = 4;
  msg.bySenderRace = static_cast<unsigned __int8>(pOne->m_Param.GetRaceCode());
  msg.dwSenderSerial = pOne->m_dwObjSerial;
  strcpy_0(msg.wszSenderName, pOne->m_Param.GetCharNameW());
  msg.bySize = static_cast<unsigned __int8>(strlen_0(pwszChatData));
  memcpy_0(msg.wszChatData, pwszChatData, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;
  msg.byPvpGrade = static_cast<unsigned __int8>(-1);

  unsigned __int8 type[2]{};
  type[0] = 2;
  type[1] = 11;
  const int length = msg.size();

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *receiver = &g_Player[index];
    if (!receiver->m_bLive)
    {
      continue;
    }

    const bool isAllowedUserDgr = receiver->m_byUserDgr >= 2u;
    const bool isSameRace = receiver->m_Param.GetRaceCode() == pOne->m_Param.GetRaceCode();
    if (isAllowedUserDgr || isSameRace)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(receiver->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), length);
    }
  }
}
