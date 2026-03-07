#include "pch.h"

#include "UIDGenerator.h"

#include <ctime>
#include <cstdio>

#include "WorldServerUtil.h"

namespace
{
  __time32_t g_t32New = 0;
  __time32_t g_t32Old = 0;
  unsigned int g_uidIndex = 0;
}

CNetCriticalSection UIDGenerator::_cs;

unsigned __int64 UIDGenerator::getuid(unsigned __int8 n)
{
  UIDGenerator::_cs.Lock();

  _time32(&g_t32New);
  if (g_t32Old != g_t32New)
  {
    g_t32Old = g_t32New;
    g_uidIndex = 0;
  }

  const unsigned __int64 base = static_cast<unsigned __int64>(g_t32Old) << 16;
  unsigned __int64 uid = (g_uidIndex % 16777215) | base;
  reinterpret_cast<unsigned char *>(&uid)[7] = n;
  ++g_uidIndex;

  UIDGenerator::_cs.Unlock();
  return uid;
}

unsigned __int64 UIDGenerator::getuid(unsigned __int8 n, unsigned __int8 ncode)
{
  if (!IsItemSerialNum(ncode))
  {
    return 0;
  }

  UIDGenerator::_cs.Lock();

  _time32(&g_t32New);
  if (g_t32Old != g_t32New)
  {
    g_t32Old = g_t32New;
    g_uidIndex = 0;
  }

  const unsigned __int64 base = static_cast<unsigned __int64>(g_t32Old) << 24;
  unsigned __int64 uid = (g_uidIndex % 16777215) | base;
  reinterpret_cast<unsigned char *>(&uid)[7] = n;
  ++g_uidIndex;

  UIDGenerator::_cs.Unlock();
  return uid;
}

void UIDGenerator::tmuid(unsigned __int64 uid, char *szBuf)
{
  if (!szBuf)
  {
    return;
  }

  struct tm tmValue{};
  __time32_t timeValue[4]{};
  timeValue[0] = static_cast<__time32_t>(uid >> 24);

  if (_localtime32_s(&tmValue, timeValue) != 0)
  {
    sprintf_s(
      szBuf,
      40,
      "[%03d] %04d-%02d-%02d %02d:%02d:%02d %d",
      static_cast<int>(reinterpret_cast<unsigned char *>(&uid)[7]),
      tmValue.tm_year + 1900,
      tmValue.tm_mon + 1,
      tmValue.tm_mday,
      tmValue.tm_hour,
      tmValue.tm_min,
      tmValue.tm_sec,
      static_cast<int>(uid & 0xFFFFFF));
  }
  else
  {
    sprintf_s(szBuf, 8u, "errno : %d", 0);
  }
}

