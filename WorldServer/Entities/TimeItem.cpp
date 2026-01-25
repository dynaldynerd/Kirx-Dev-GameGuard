#include "pch.h"

#include "TimeItem.h"

#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>

TimeItem *TimeItem::Instance()
{
  static TimeItem s_instance;
  return &s_instance;
}

bool TimeItem::Init()
{
  CreateDirectoryA("..\\ZoneServerLog\\SystemLog", nullptr);

  char buffer[256]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(buffer, sizeof(buffer), "%s\\TimeItem_%d.log", "..\\ZoneServerLog\\SystemLog", now);
  _kLogger.SetWriteLogFile(buffer, 1, 0, 1, 1);

  return ReadGoods();
}

bool TimeItem::ReadGoods()
{
  char errMsg[144]{};
  if (!_kRecTimeItem.ReadRecord(".\\Script\\TimerItem.dat", 0x8C, errMsg))
  {
    MyMessageBox("TimeItem", errMsg);
    return false;
  }

  if (!MakeLinkTable(errMsg, static_cast<int>(sizeof(errMsg))))
  {
    MyMessageBox("TimeItem", errMsg);
    g_Main.m_logLoadingError.Write("Failed TimeItem::MakeLinkTable()");
    return false;
  }

  return CheckGoods();
}

bool TimeItem::MakeLinkTable(char *szMsg, int nSize)
{
  (void)szMsg;
  (void)nSize;
  return true;
}

bool TimeItem::CheckGoods()
{
  return true;
}
