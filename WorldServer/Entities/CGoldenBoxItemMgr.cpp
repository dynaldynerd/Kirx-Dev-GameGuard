#include "pch.h"

#include "CGoldenBoxItemMgr.h"

#include <ctime>
#include <new>

#include "CAsyncLogger.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "InvenKey.h"
#include "WorldServerUtil.h"

_goldbox_index::_goldbox_index()
{
  memset_0(this, 0, sizeof(_goldbox_index));
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
  sprintf_s(filePath, sizeof(filePath), "..\\ZoneServerLog\\SystemLog\\GoldenAgeEvent\\%d.his", now);
  _kLogger.SetWriteLogFile(filePath, 1, 0, 1, 1);
  return true;
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
    _base_fld *record = g_Main.m_tblItemData[17].GetRecord(n);
    if (!*reinterpret_cast<const int *>(&record[3].m_strCode[4]))
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

    _base_fld *record = g_Main.m_tblItemData[17].GetRecord(recordIndex);
    if (record)
    {
      if (*reinterpret_cast<const int *>(&record[3].m_strCode[4]))
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

unsigned __int8 CGoldenBoxItemMgr::Get_Event_Status()
{
  return m_golden_box_event.m_event_status;
}
