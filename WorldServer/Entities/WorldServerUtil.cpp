#include "pch.h"

#include "WorldServerUtil.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <intrin.h>
#include <atlstr.h>

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "CMerchant.h"
#include "CRecordData.h"
#include "CashItemRemoteStore.h"
#include "GlobalObjectDefs.h"
#include "GlobalObjects.h"
#include "ItemDataLoader.h"
#include "R3EngineState.h"
#include "R3EngineGlobals.h"
#include "CMergeFileManager.h"
#include "base_fld.h"
#include "EQUIP_MASTERY_LIM.h"
#include "CAnimus.h"
#include "animus_fld.h"

const char *dayofweek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static char szDefItemName[] = "UNKNOWN";
static float sR[4] = {200.0f, 200.0f, 200.0f, 200.0f};
static int s_nSkillLvPerMastery[8] = {};
static int s_nForceLvPerMastery[24] = {};

static __int64 D3DXCreateTextureFromFileInMemory_0(IDirect3DDevice8 *device, const void *data, unsigned int size, void *outTex);
static __int64 D3DXCreateTextureFromFileExA_0(IDirect3DDevice8 *device, const char *path, __int64 flags);
static __int64 sub_1404FFFB0(__int64 a1);
static __int64 sub_1404FFF30(unsigned int *a1);
static unsigned __int16 sub_1405005A0(unsigned __int16 a1);
static int sub_140501380(char *a1, const char *a2, const char *a3);
static void sub_140502610(char *a1);
static int sub_140502690(FILE *Stream, __int64 a2, int a3);
static void sub_140503190(char *a1, unsigned int *a2);
static void LoadR3X(char *a1);
static _LIGHTMAP **LoadR3TLightMap(struct R3Texture *a1, D3DFORMAT a2);
static void RestoreSystemTexture();
static IDirect3DTexture8 *GetD3DTextureFromBuffer(unsigned __int8 *a1, unsigned int a2);
static int GetMipMapSkipSize(_DDSURFACEDESC2 *a1, unsigned int a2, unsigned int a3, unsigned int a4);
static void MakeMipMap(unsigned short a1, unsigned short a2, unsigned short *a3, unsigned short *a4);
static void MakeMipMap(unsigned short a1, unsigned short a2, unsigned short *a3, unsigned char *a4);

unsigned int GetLoopTime()
{
  return timeGetTime();
}

bool CanAddMoneyForMaxLimMoney(unsigned __int64 ui64AddMoney, unsigned __int64 ui64HasMoney)
{
  if (ui64AddMoney > 0x77359400ULL)
  {
    return false;
  }
  if (ui64HasMoney <= 0x77359400ULL)
  {
    return ui64AddMoney <= 2000000000ULL - ui64HasMoney;
  }
  return false;
}

void MakeBinaryStr(const unsigned __int8 *pBuff, unsigned __int64 tBufSize, char *pOut, rsize_t tOutSize)
{
  char temp[32]{};

  sprintf_s(temp, "0x%0.2X", pBuff[0]);
  strcat_s(pOut, tOutSize, temp);
  for (unsigned __int64 i = 1; i < tBufSize; ++i)
  {
    sprintf_s(temp, "%0.2X", pBuff[i]);
    strcat_s(pOut, tOutSize, temp);
  }
}

unsigned __int64 SplitString(char *strSrc, const char *_Delim, std::vector<std::string> *stringlist)
{
  for (char *token = strtok(strSrc, _Delim); token; token = strtok(nullptr, _Delim))
  {
    stringlist->push_back(std::string(token));
  }
  return stringlist->size();
}

CMerchant *FindEmptyNPC(CMerchant *pNPC, int nMax)
{
    unsigned int dwTime = timeGetTime();
    for (int i = 0; i < nMax; ++i)
    {
        if (!pNPC[i].m_bLive)
        {
            if (dwTime - pNPC[i].m_dwLastDestroyTime > 30000)
                return &pNPC[i];
        }
    }
    return nullptr;
}

void NetTrace(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    char Buffer[272]{};
    vsprintf_s(Buffer, sizeof(Buffer), fmt, va);
    va_end(va);
    strcat_s(Buffer, sizeof(Buffer), "\n");
    OutputDebugStringA(Buffer);
}

void __trace(const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  char buffer[272]{};
  vsprintf_s(buffer, sizeof(buffer), fmt, va);
  va_end(va);
  strcat_s(buffer, sizeof(buffer), "\n");
  OutputDebugStringA(buffer);
}

static unsigned __int64 dw64Cnt = 0;

void OutputDebugLog(const char *szFormat, ...)
{
  if (CMainThread::IsReleaseServiceMode(&g_Main))
  {
    return;
  }

  va_list args;
  va_start(args, szFormat);
  CStringA message;
  message.FormatV(szFormat, args);
  va_end(args);
  message += "\r\n";

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned __int64 logIndex = dw64Cnt++;
  CStringA line;
  line.Format(
    "%u\t%04d-%02d-%02d %02d:%02d:%02d.%03d : %s",
    static_cast<unsigned int>(logIndex),
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond,
    systemTime.wMilliseconds,
    static_cast<const char *>(message));
  OutputDebugStringA(line);
}
int GetCurrentDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

int GetCurrentYear()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_year + 1900;
}

int GetCurrentMonth()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mon + 1;
}

void GetTodayStr(char *szToday)
{
  if (!szToday)
  {
    return;
  }

  char month[32]{};
  char day[16]{};
  const unsigned __int16 year = static_cast<unsigned __int16>(GetCurrentYear());
  const unsigned __int16 curMonth = static_cast<unsigned __int16>(GetCurrentMonth());
  const unsigned __int16 curDay = static_cast<unsigned __int16>(GetCurrentDay());

  if (curMonth > 9)
  {
    sprintf(month, "%d", curMonth);
  }
  else
  {
    sprintf(month, "0%d", curMonth);
  }

  if (curDay > 9)
  {
    sprintf(day, "%d", curDay);
  }
  else
  {
    sprintf(day, "0%d", curDay);
  }

  sprintf(szToday, "%d%s%s", year, month, day);
}

unsigned int GetLocalDate()
{
  const int year = GetCurrentYear();
  const int month = GetCurrentMonth();
  const int day = GetCurrentDay();
  return static_cast<unsigned int>(10000 * year + 100 * month + day);
}

unsigned int eGetLocalDate()
{
  const int year = GetCurrentYear();
  const int month = GetCurrentMonth();
  const int day = GetCurrentDay();
  return static_cast<unsigned int>(10000 * year + 100 * month + day);
}

int GetCurwDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_wday;
}

void GetNowDateTime(char *szDateTime)
{
  if (!szDateTime)
  {
    return;
  }

  char month[32]{};
  char day[32]{};
  char hour[32]{};
  char minute[16]{};

  const unsigned __int16 year = static_cast<unsigned __int16>(GetCurrentYear());
  const unsigned __int16 curMonth = static_cast<unsigned __int16>(GetCurrentMonth());
  const unsigned __int16 curDay = static_cast<unsigned __int16>(GetCurrentDay());
  const unsigned __int16 curHour = static_cast<unsigned __int16>(GetCurrentHour());
  const unsigned __int16 curMin = static_cast<unsigned __int16>(GetCurrentMin());

  if (curMonth > 9)
    sprintf_s(month, "%d", curMonth);
  else
    sprintf_s(month, "0%d", curMonth);

  if (curDay > 9)
    sprintf_s(day, "%d", curDay);
  else
    sprintf_s(day, "0%d", curDay);

  if (curHour > 9)
    sprintf_s(hour, "%d", curHour);
  else
    sprintf_s(hour, "0%d", curHour);

  if (curMin > 9)
    sprintf_s(minute, "%d", curMin);
  else
    sprintf_s(minute, "0%d", curMin);

  sprintf_s(szDateTime, 128, "%d-%s-%s %s:%s", year, month, day, hour, minute);
}

unsigned __int8 GetItemKindCode(int nTableCode)
{
  if (nTableCode == 19)
  {
    return 2;
  }
  return nTableCode == 24;
}

char *DisplayItemUpgInfo(int nTableCode, int dwLvBit)
{
  static char g_szLv[32] = "00000000";
  static char g_szUPT[32] = "f";
  static char szBufUpt[32]{};

  if (!dwLvBit)
  {
    return g_szUPT;
  }

  if (GetItemKindCode(nTableCode) || dwLvBit == 0x0FFFFFFF)
  {
    return g_szUPT;
  }

  strcpy_s(g_szLv, "00000000");
  _itoa_s(dwLvBit, szBufUpt, 16);
  const size_t len = strlen_0(szBufUpt);
  memcpy_0(&g_szLv[8 - len], szBufUpt, len);
  g_szLv[8] = '\0';
  if (!strcmp_0(&g_szLv[1], "fffffff"))
  {
    g_szLv[2] = '\0';
  }
  return g_szLv;
}

void IOFileWrite_0(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

void IOFileWrite_1(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

void IOFileWrite(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

__time64_t time_20(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t time_18(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t time_17(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t mktime_3(tm *_Tm)
{
  return _mktime64(_Tm);
}

tm *localtime_5(const __int64 *_Time)
{
  return _localtime64(_Time);
}

tm *localtime_2(const __int64 *_Time)
{
  return _localtime64(_Time);
}

int GetCurDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

int GetNextDay()
{
  std::time_t now = std::time(nullptr);
  now += 86400;
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

bool GetDateStrAfterDay(char *szDate, int iBuffSize, unsigned __int16 wDayAfter)
{
  if (!szDate)
  {
    return false;
  }

  std::time_t now = std::time(nullptr);
  now += static_cast<std::time_t>(86400) * static_cast<std::time_t>(wDayAfter);

  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }

  sprintf_s(szDate, iBuffSize, "%04d%02d%02d", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
  return true;
}

bool IsDayChanged(int *iOldDay)
{
  const int curDay = GetCurDay();
  if (curDay < 0)
  {
    return false;
  }
  if (curDay == *iOldDay)
  {
    return false;
  }
  *iOldDay = curDay;
  return true;
}

bool GetDateTimeStr(char *szTime)
{
  if (szTime == nullptr)
  {
    return false;
  }

  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }

  return std::strftime(szTime, 0x80, "%y-%m-%d_%H-%M-%S", &local) != 0;
}

bool GetLastWriteFileTime(const char *szFileName, _FILETIME *ftWrite)
{
  if (!szFileName || !ftWrite)
  {
    return false;
  }

  HANDLE hFile = CreateFileA(szFileName, 0x80000000, 1u, nullptr, 3u, 0x80u, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  _FILETIME creationTime{};
  _FILETIME lastAccessTime{};
  const BOOL ok = GetFileTime(hFile, &creationTime, &lastAccessTime, ftWrite);
  CloseHandle(hFile);
  return ok != FALSE;
}

void FloatToShort(float *pFloat, short *pShort, int size)
{
  for (int j = 0; j < size; ++j)
  {
    pShort[j] = static_cast<short>(static_cast<int>(pFloat[j]));
  }
}

int GetItemTableCode(const char *psItemCode)
{
  if (psItemCode == nullptr)
  {
    return -1;
  }

  char prefix[3] = {};
  strncpy_s(prefix, sizeof(prefix), psItemCode, 2);

  if (strcmp(prefix, "iu") == 0) return 0;
  if (strcmp(prefix, "il") == 0) return 1;
  if (strcmp(prefix, "ig") == 0) return 2;
  if (strcmp(prefix, "is") == 0) return 3;
  if (strcmp(prefix, "ih") == 0) return 4;
  if (strcmp(prefix, "id") == 0) return 5;
  if (strcmp(prefix, "iw") == 0) return 6;
  if (strcmp(prefix, "im") == 0) return 11;
  if (strcmp(prefix, "ie") == 0) return 12;
  if (strcmp(prefix, "ip") == 0) return 13;
  if (strcmp(prefix, "ib") == 0) return 10;
  if (strcmp(prefix, "if") == 0) return 14;
  if (strcmp(prefix, "ic") == 0) return 15;
  if (strcmp(prefix, "it") == 0) return 16;
  if (strcmp(prefix, "io") == 0) return 17;
  if (strcmp(prefix, "ir") == 0) return 18;
  if (strcmp(prefix, "in") == 0) return 19;
  if (strcmp(prefix, "iy") == 0) return 20;
  if (strcmp(prefix, "ik") == 0) return 7;
  if (strcmp(prefix, "ii") == 0) return 8;
  if (strcmp(prefix, "ia") == 0) return 9;
  if (strcmp(prefix, "iz") == 0) return 21;
  if (strcmp(prefix, "iq") == 0) return 22;
  if (strcmp(prefix, "ix") == 0) return 23;
  if (strcmp(prefix, "ij") == 0) return 24;
  if (strcmp(prefix, "gt") == 0) return 25;
  if (strcmp(prefix, "tr") == 0) return 26;
  if (strcmp(prefix, "sk") == 0) return 27;
  if (strcmp(prefix, "ti") == 0) return 28;
  if (strcmp(prefix, "ev") == 0) return 29;
  if (strcmp(prefix, "re") == 0) return 30;
  if (strcmp(prefix, "bx") == 0) return 31;
  if (strcmp(prefix, "fi") == 0) return 32;
  if (strcmp(prefix, "un") == 0) return 33;
  if (strcmp(prefix, "rd") == 0) return 34;
  if (strcmp(prefix, "lk") == 0) return 35;
  if (strcmp(prefix, "cu") == 0) return 36;

  return -1;
}

int IsOverLapItem(int nTableCode)
{
  return nTableCode == 13
      || nTableCode == 17
      || nTableCode == 18
      || nTableCode == 20
      || nTableCode == 22
      || nTableCode == 31
      || nTableCode == 23
      || nTableCode == 30
      || nTableCode == 26
      || nTableCode == 32
      || nTableCode == 34
      || nTableCode == 35;
}

int IsItemSerialNum(int nTableCode)
{
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 6:
    case 8:
    case 9:
    case 15:
    case 24:
    case 33:
      return 1;
    default:
      return 0;
  }
}

int IsStorageRange(unsigned __int8 byStorageCode, unsigned __int8 byStorageIndex)
{
  switch (byStorageCode)
  {
    case 0u:
      return byStorageIndex < 0x64u;
    case 1u:
      return byStorageIndex < 8u;
    case 2u:
      return byStorageIndex < 7u;
    case 3u:
      return byStorageIndex < 0x58u;
    case 4u:
      return byStorageIndex < 4u;
    case 5u:
      return byStorageIndex < 0x64u;
    case 6u:
      return byStorageIndex < 0x28u;
    case 7u:
      return byStorageIndex < 0x28u;
    default:
      return 0;
  }
}

unsigned __int8 GetItemUpgedLv(unsigned int dwLvBit)
{
  if (!dwLvBit)
  {
    return 0;
  }

  unsigned __int8 level = 0;
  for (int j = 0; j < 7; ++j)
  {
    const unsigned __int8 talik = static_cast<unsigned __int8>((dwLvBit >> (4 * j)) & 0xF);
    if (talik == 0xF)
    {
      break;
    }
    ++level;
  }
  return level;
}

unsigned __int8 GetTalikFromSocket(unsigned int dwLvBit, unsigned __int8 bySocketIndex)
{
  if (dwLvBit)
  {
    return static_cast<unsigned __int8>((dwLvBit >> (4 * bySocketIndex)) & 0xF);
  }
  return 15;
}

unsigned __int8 GetDefItemUpgSocketNum(int nTableCode, int nItemIndex)
{
  if (nTableCode < 0)
  {
    return 0;
  }

  CRecordData *table = &s_ptblItemData[nTableCode];
  unsigned __int8 socketNum = 0;
  if (nTableCode <= 5 || nTableCode == 7)
  {
    _base_fld *record = table->GetRecord(nItemIndex);
    if (record)
    {
      socketNum = record[6].m_strCode[52];
    }
  }
  else if (nTableCode == 6)
  {
    _base_fld *record = table->GetRecord(nItemIndex);
    if (record)
    {
      socketNum = record[11].m_strCode[16];
    }
  }

  if (socketNum > 7)
  {
    char buffer[144]{};
    sprintf_s(buffer, "tbl:%d, idx:%d => slot: %d\n", nTableCode, nItemIndex, socketNum);
    OutputDebugStringA(buffer);
    return 0;
  }
  return socketNum;
}

unsigned int GetBitAfterSetLimSocket(unsigned __int8 byLimSocketNum)
{
  return (static_cast<unsigned int>(byLimSocketNum) << 28) | 0x0FFFFFFF;
}

_animus_fld *GetAnimusFldFromExp(int nAnimusClass, unsigned __int64 dwExp)
{

  CRecordData *table = &CAnimus::s_tblParameter[nAnimusClass];
  for (int n = 0; n < 65; ++n)
  {
    _base_fld *record = table->GetRecord(n);
    if (!record)
      return nullptr;
    if (*reinterpret_cast<unsigned __int64 *>(record[1].m_strCode) > dwExp)
      return reinterpret_cast<_animus_fld *>(record);
  }
  const int recordNum = static_cast<int>(table->GetRecordNum());
  return reinterpret_cast<_animus_fld *>(table->GetRecord(recordNum - 1));
}

unsigned int GetMaxParamFromExp(int nAnimusClass, unsigned __int64 dwExp)
{

  _animus_fld *record = GetAnimusFldFromExp(nAnimusClass, dwExp);
  if (!record)
    return 0;
  const unsigned int maxHP = static_cast<unsigned int>(record->m_nMaxHP) & 0xFFFFu;
  const unsigned int maxFP = static_cast<unsigned int>(record->m_nMaxFP) & 0xFFFFu;
  return (maxFP << 16) | maxHP;
}

int GetMaxResKind()
{
  return static_cast<int>(g_Main.m_tblItemData[18].GetRecordNum());
}

unsigned __int8 GetItemGrade(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  if (nTableCode >= 0)
  {
    if (nTableCode <= 5 || nTableCode == 7)
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (record)
      {
        return record[3].m_strCode[0];
      }
    }
    else if (nTableCode == 6)
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (record)
      {
        return record[3].m_strCode[0];
      }
    }
  }
  return 0;
}

int GetItemEquipLevel(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[8]);
    }
    case 6:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[8].m_strCode[8]);
    }
    case 8:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[4]);
    }
    case 9:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[4]);
    }
    case 25:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[48]);
    }
    case 26:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[60]);
    }
    case 27:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[8]);
    }
    case 33:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[52]);
    }
    default:
      return 1;
  }
}

int GetItemEquipUpLevel(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[12]);
    }
    case 6:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[8].m_strCode[12]);
    }
    case 8:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[8]);
    }
    case 9:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[8]);
    }
    case 25:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[52]);
    }
    case 26:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return static_cast<int>(record[5].m_dwIndex);
    }
    case 27:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[12]);
    }
    case 33:
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (!record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&record[4].m_strCode[56]);
    }
    default:
      return 1;
  }
}

_EQUIP_MASTERY_LIM *GetItemEquipMastery(int nTableCode, int nItemIndex, int *pnLimNum)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  if (nTableCode >= 0)
  {
    if (nTableCode <= 5 || nTableCode == 7)
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (record)
      {
        *pnLimNum = 2;
        return reinterpret_cast<_EQUIP_MASTERY_LIM *>(&record[4].m_strCode[20]);
      }
    }
    else if (nTableCode == 6)
    {
      _base_fld *record = table->GetRecord(nItemIndex);
      if (record)
      {
        *pnLimNum = 2;
        return reinterpret_cast<_EQUIP_MASTERY_LIM *>(&record[8].m_strCode[20]);
      }
    }
  }
  return nullptr;
}

unsigned __int8 GetWeaponClass(int nItemIndex)
{
  _base_fld *record = s_ptblItemData[6].GetRecord(nItemIndex);
  if (!record)
  {
    return 0;
  }

  const int weaponType = *reinterpret_cast<int *>(&record[6].m_strCode[8]);
  switch (weaponType)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 9:
      return 0;
    case 5:
    case 6:
    case 7:
    case 8:
    case 11:
      return 1;
    default:
      return 0;
  }
}

int CalcRoundUp(float fVal)
{
  int rounded = static_cast<int>(fVal);
  const float remainder = fVal - static_cast<float>(rounded);
  if (remainder > 0.0f)
  {
    ++rounded;
  }
  return rounded;
}

int CalcMastery(int nMasteryCode, int nMasteryIndex, int dwMasteryCum, unsigned int nRaceCode)
{
  float mastery = 0.0f;
  switch (nMasteryCode)
  {
    case 0:
    {
      const float v16 = (static_cast<float>(dwMasteryCum) + 1.0f) / 1000.0f;
      const float v6 = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
      mastery = std::sqrt(v16 + v6);
      break;
    }
    case 1:
    {
      const float v17 = (static_cast<float>(dwMasteryCum) + 1.0f) / 1000.0f;
      const float v7 = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
      mastery = std::sqrt(v17 + v7);
      break;
    }
    case 2:
    {
      const float v18 = (static_cast<float>(dwMasteryCum) + 1.0f) / 100.0f;
      const float v8 = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
      mastery = std::sqrt(v18 + v8);
      break;
    }
    case 3:
    {
      const float v9 = std::sqrt((static_cast<float>(dwMasteryCum) + 1.0f) * 10.0f);
      mastery = std::sqrt(v9);
      break;
    }
    case 4:
    {
      const float v10 = std::sqrt((static_cast<float>(dwMasteryCum) + 1.0f) * 14.0f);
      mastery = std::sqrt(v10);
      break;
    }
    case 5:
    {
      if (nMasteryIndex == 0 || nMasteryIndex == 1)
      {
        mastery = std::sqrt(((static_cast<float>(dwMasteryCum) / 1.1f) * 3.0f) + 1.0f);
      }
      else if (nMasteryIndex == 2)
      {
        mastery = std::sqrt(((static_cast<float>(dwMasteryCum) / 10.0f) * 3.0f) + 1.0f);
      }
      break;
    }
    case 6:
    {
      if (nRaceCode > 1)
      {
        const float v20 = (static_cast<float>(dwMasteryCum) + 1.0f) / 1000.0f;
        const float v11 = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
        mastery = std::sqrt(v20 + v11);
      }
      else
      {
        mastery = std::sqrt(static_cast<float>(dwMasteryCum) / 15000.0f) + 1.0f;
      }
      break;
    }
    default:
      break;
  }

  if (mastery <= 99.0f)
  {
    if (mastery < 1.0f)
    {
      mastery = FLOAT_1_0;
    }
  }
  else
  {
    mastery = 99.0f;
  }
  return static_cast<int>(mastery);
}

int GetSFLevel(int nLv, unsigned int dwHitCount)
{
  const float ratio =
    std::sqrt(static_cast<float>(static_cast<int>(dwHitCount + 1)) / sR[nLv]);
  const int level = static_cast<int>(std::sqrt(ratio) + 0.9999f);
  if (level <= 7)
  {
    if (level < 1)
    {
      return 1;
    }
  }
  else
  {
    return 7;
  }
  return level;
}

int GetStaffMastery(unsigned int *pdwForceLvCum)
{
  const float weightedSum = (static_cast<float>(static_cast<int>(pdwForceLvCum[0])) * 1.125f)
                            + (static_cast<float>(static_cast<int>(pdwForceLvCum[1])) * 2.25f)
                            + (static_cast<float>(static_cast<int>(pdwForceLvCum[2])) * 3.375f)
                            + (static_cast<float>(static_cast<int>(pdwForceLvCum[3])) * 4.5f);
  const float v7 = (weightedSum + 1.0f) / 1000.0f;
  const float v3 = std::sqrt(weightedSum + 1.0f);
  const int mastery = static_cast<int>(std::sqrt(v7 + v3));
  if (mastery <= 99)
  {
    if (mastery < 1)
    {
      return 1;
    }
  }
  else
  {
    return 99;
  }
  return mastery;
}

void InitMasteryFormula(CRecordData *pSkillData, CRecordData *pForceData)
{
  memset_0(s_nSkillLvPerMastery, -1, sizeof(s_nSkillLvPerMastery));
  memset_0(s_nForceLvPerMastery, -1, sizeof(s_nForceLvPerMastery));
  for (int n = 0; ; ++n)
  {
    const int recordNum = pSkillData->GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }
    _base_fld *record = pSkillData->GetRecord(n);
    if (*reinterpret_cast<unsigned int *>(&record[1].m_strCode[4]) < 8u)
    {
      s_nSkillLvPerMastery[*reinterpret_cast<int *>(&record[1].m_strCode[4])] =
        *reinterpret_cast<int *>(&record[4].m_strCode[60]);
    }
  }
  for (int n = 0; ; ++n)
  {
    const int recordNum = pForceData->GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }
    _base_fld *record = pForceData->GetRecord(n);
    if (*reinterpret_cast<unsigned int *>(&record[1].m_strCode[4]) < 0x18u)
    {
      s_nForceLvPerMastery[*reinterpret_cast<int *>(&record[1].m_strCode[4])] =
        *reinterpret_cast<int *>(&record[4].m_strCode[60]);
    }
  }
}

int GetItemStdPrice(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[40]);
    case 6:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[8].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[8].m_strCode[40]);
    case 8:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 9:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 10:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[4]);
    case 11:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 12:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[4]);
    case 13:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[48]);
    case 15:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[4]);
    case 16:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[4]);
    case 17:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[3].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[3].m_strCode[16]);
    case 18:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[12]);
    case 19:
      return 0;
    case 20:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[4]);
    case 21:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[5].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(Record[5].m_strCode);
    case 22:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[60];
      return static_cast<unsigned int>(Record[6].m_dwIndex);
    case 23:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[6].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(Record[6].m_strCode);
    case 24:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[4];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[8]);
    case 25:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[48]);
    case 26:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[7].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[52]);
    case 27:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[32];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[36]);
    case 28:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[4]);
    case 30:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 31:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 32:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 33:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[52]);
    case 34:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[52]);
    case 35:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[40]);
    case 36:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    default:
      return 0;
  }
}

int GetItemStdPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[44]);
    case 6:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[8].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[8].m_strCode[44]);
    case 8:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 9:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 10:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[8]);
    case 11:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 12:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[8]);
    case 13:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[52]);
    case 15:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[8]);
    case 16:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[8]);
    case 17:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[3].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[3].m_strCode[20]);
    case 18:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[8];
      switch (nRace)
      {
        case 0:
          return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[12]);
        case 1:
          return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[16]);
        case 2:
          return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[20]);
        default:
          return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[24]);
      }
    case 19:
      return 0;
    case 20:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[8]);
    case 21:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[5].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[4]);
    case 22:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[60];
      return *reinterpret_cast<unsigned int *>(Record[6].m_strCode);
    case 23:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[6].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[4]);
    case 24:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[4];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 25:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[52]);
    case 26:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[7].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[56]);
    case 27:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[32];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[40]);
    case 28:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[8]);
    case 30:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 31:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 32:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 33:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[56]);
    case 34:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[56]);
    case 36:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    default:
      return 0;
  }
}

int GetItemProcPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[56]);
    case 6:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[8].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[8].m_strCode[56]);
    case 8:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[36]);
    case 9:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[36]);
    case 10:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[20]);
    case 11:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 12:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 13:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[6].m_strCode[44];
      return static_cast<unsigned int>(Record[7].m_dwIndex);
    case 15:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 16:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 17:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[3].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[3].m_strCode[32]);
    case 18:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[5].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[36]);
    case 19:
      return 0;
    case 20:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 21:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = static_cast<unsigned __int8>(Record[5].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[16]);
    case 22:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[60];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[12]);
    case 23:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[6].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[16]);
    case 24:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[4];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 25:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return static_cast<unsigned int>(Record[7].m_dwIndex);
    case 26:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[7].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(Record[8].m_strCode);
    case 27:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[32];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[52]);
    case 28:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[20]);
    case 30:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 31:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[32]);
    case 32:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 33:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(Record[6].m_strCode);
    case 34:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(Record[5].m_strCode);
    case 36:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[32]);
    default:
      return 0;
  }
}

int GetItemKillPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[52]);
    case 6:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[8].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[8].m_strCode[52]);
    case 8:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[32]);
    case 9:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[32]);
    case 10:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[16]);
    case 11:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 12:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 13:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[60]);
    case 15:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 16:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 17:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[3].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[3].m_strCode[28]);
    case 18:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[32]);
    case 19:
      return 0;
    case 20:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 21:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[5].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[12]);
    case 22:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[60];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[8]);
    case 23:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[6].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[12]);
    case 24:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[4];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 25:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[60]);
    case 26:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[7].m_strCode[48];
      return static_cast<unsigned int>(Record[8].m_dwIndex);
    case 27:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[32];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[48]);
    case 28:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[16]);
    case 30:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 31:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 32:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 33:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[48];
      return static_cast<unsigned int>(Record[6].m_dwIndex);
    case 34:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[48];
      return static_cast<unsigned int>(Record[5].m_dwIndex);
    case 36:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    default:
      return 0;
  }
}

int GetItemGoldPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[48]);
    case 6:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[8].m_strCode[36];
      return *reinterpret_cast<unsigned int *>(&Record[8].m_strCode[48]);
    case 8:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 9:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[16];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 10:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[12]);
    case 11:
      Record = table->GetRecord(nItemIndex);
      if (Record)
        *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 12:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 13:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[56]);
    case 15:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 16:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 17:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[3].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[3].m_strCode[24]);
    case 18:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[28]);
    case 19:
      return 0;
    case 20:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[12]);
    case 21:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[5].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[8]);
    case 22:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[60];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[4]);
    case 23:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(Record[6].m_dwIndex);
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[8]);
    case 24:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[4];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[16]);
    case 25:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[44];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[56]);
    case 26:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[7].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[60]);
    case 27:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[32];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[44]);
    case 28:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[6].m_strCode[0];
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[12]);
    case 30:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 31:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    case 32:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[8];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[20]);
    case 33:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[5].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[60]);
    case 34:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[48];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[60]);
    case 36:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return 0;
      *pbyMoneyKind = Record[4].m_strCode[12];
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[24]);
    default:
      return 0;
  }
}

int GetUsePcCashType(unsigned __int8 byTblCode, int nIndex)
{
  CRecordData *table = &s_ptblItemData[byTblCode];
  if (!CashItemRemoteStore::FindCashRec(byTblCode, nIndex))
  {
    return 0;
  }

  if (byTblCode == 10)
  {
    _base_fld *record = table->GetRecord(nIndex);
    if (record)
    {
      return *reinterpret_cast<unsigned int *>(&record[9].m_strCode[16]);
    }
  }
  else
  {
    switch (byTblCode)
    {
      case 13:
      {
        _base_fld *record = table->GetRecord(nIndex);
        if (record)
        {
          return *reinterpret_cast<unsigned int *>(&record[9].m_strCode[40]);
        }
        break;
      }
      case 18:
      {
        _base_fld *record = table->GetRecord(nIndex);
        if (record)
        {
          return *reinterpret_cast<unsigned int *>(&record[8].m_strCode[12]);
        }
        break;
      }
      case 22:
      {
        _base_fld *record = table->GetRecord(nIndex);
        if (record)
        {
          return *reinterpret_cast<unsigned int *>(&record[7].m_strCode[48]);
        }
        break;
      }
      case 31:
      {
        _base_fld *record = table->GetRecord(nIndex);
        if (record)
        {
          return *reinterpret_cast<unsigned int *>(&record[5].m_strCode[60]);
        }
        break;
      }
      default:
        break;
    }
  }
  return 0;
}

int IsCashItem(unsigned __int8 byTblCode, int dwIndex)
{
  CRecordData *table = &s_ptblItemData[byTblCode];
  switch (byTblCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      return 0;
    case 8:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[6].m_strCode[48]);
    }
    case 9:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[6].m_strCode[48]);
    }
    case 10:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[9].m_strCode[12]);
    }
    case 13:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[9].m_strCode[36]);
    }
    case 16:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[5].m_strCode[48]);
    }
    case 18:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[8].m_strCode[8]);
    }
    case 20:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[5].m_strCode[44]);
    }
    case 22:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[7].m_strCode[44]);
    }
    case 30:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[5].m_strCode[60]);
    }
    case 31:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[5].m_strCode[56]);
    }
    case 33:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[7].m_strCode[32]);
    }
    case 35:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&record[7].m_strCode[8]);
    }
    case 36:
    {
      _base_fld *record = table->GetRecord(dwIndex);
      if (!record)
        return 0;
      return *reinterpret_cast<unsigned int *>(record[6].m_strCode);
    }
    default:
      return 0;
  }
}

unsigned int GetItemDurPoint(int nTableCode, int nIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(Record[5].m_strCode);
    case 6:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(Record[9].m_strCode);
    case 10:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[28]);
    case 11:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[40]);
    case 15:
      return 0;
    case 16:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&Record[4].m_strCode[28]);
    case 24:
      return 0;
    case 25:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[40]);
    case 26:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return static_cast<unsigned int>(static_cast<int>(*reinterpret_cast<float *>(&Record[5].m_strCode[60])));
    case 27:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return static_cast<unsigned int>(Record[5].m_dwIndex);
    case 33:
      Record = table->GetRecord(nIndex);
      if (!Record)
        return 1;
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[44]);
    default:
      return 1;
  }
}

char *GetItemKorName(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  _base_fld *Record = nullptr;

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 6:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 8:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 9:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 10:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 11:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 12:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 13:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 14:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return reinterpret_cast<char *>(&Record[2]);
    case 15:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 16:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 17:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 18:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 19:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 20:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 21:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 22:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 23:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 24:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return &Record[2].m_strCode[60];
    case 25:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return &Record[2].m_strCode[56];
    case 26:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return reinterpret_cast<char *>(&Record[3]);
    case 27:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 28:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 29:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[1].m_strCode;
    case 30:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 31:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 32:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 33:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return &Record[2].m_strCode[60];
    case 34:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 35:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    case 36:
      Record = table->GetRecord(nItemIndex);
      if (!Record)
        return szDefItemName;
      return Record[2].m_strCode;
    default:
      return szDefItemName;
  }
}

int IsTimeItem(unsigned __int8 byTblCode, int dwIndex)
{
  CRecordData *table = &s_ptblItemData[byTblCode];
  _base_fld *Record = nullptr;

  switch (byTblCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[56]);
    case 6:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[12].m_strCode[20]);
    case 7:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[56]);
    case 8:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[52]);
    case 9:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[52]);
    case 10:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[9].m_strCode[20]);
    case 11:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[8]);
    case 12:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[44]);
    case 13:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[9].m_strCode[44]);
    case 16:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[52]);
    case 17:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[4]);
    case 18:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[8].m_strCode[16]);
    case 20:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[5].m_strCode[48]);
    case 21:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[40]);
    case 22:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[52]);
    case 23:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(Record[10].m_strCode);
    case 25:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[11].m_strCode[32]);
    case 26:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[9].m_strCode[40]);
    case 27:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[20]);
    case 28:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[44]);
    case 31:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return static_cast<int>(Record[6].m_dwIndex);
    case 33:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[36]);
    case 34:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[7].m_strCode[40]);
    case 36:
      Record = table->GetRecord(dwIndex);
      if (!Record)
        return 0;
      return *reinterpret_cast<unsigned int *>(&Record[6].m_strCode[4]);
    default:
      return 0;
  }
}

int ParsingCommandA(char *pszSrc, int nMaxWordNum, char **ppszDst, int nMaxWordSize)
{
  char *src = pszSrc;
  for (int j = 0; j < nMaxWordNum; ++j)
  {
    char *dst = ppszDst[j];
    int len = 0;

    while (*src == ' ' || *src == '\t')
    {
      ++src;
    }

    while (*src != '\0' && *src != ' ' && *src != '\t')
    {
      *dst++ = *src++;
      if (len++ >= nMaxWordSize - 1)
      {
        return 0;
      }
    }
    *dst = '\0';

    if (*src == '\0')
    {
      if (std::strlen(ppszDst[j]) != 0)
      {
        return j + 1;
      }
      return j;
    }

    if (dst == ppszDst[j])
    {
      return j;
    }
    ++src;
  }

  return 0;
}

int ParsingCommandW(char *pwszSrc, int nMaxWordNum, char **ppwszDst, int nMaxWordSize)
{
  char *src = pwszSrc;
  for (int j = 0; ; ++j)
  {
    if (j >= nMaxWordNum)
    {
      return 0;
    }

    char *dst = ppszDst[j];
    int len = 0;

    while (*src == ' ' || *src == '\t')
    {
      ++src;
    }

    while (*src != '\0' && *src != ' ' && *src != '\t')
    {
      *dst++ = *src++;
      if (len++ >= nMaxWordSize - 1)
      {
        return 0;
      }
    }
    *dst = '\0';

    if (*src == '\0')
    {
      return strlen_0(ppszDst[j]) ? (j + 1) : j;
    }

    if (dst == ppszDst[j])
    {
      return j;
    }

    ++src;
  }
}

void WriteServerStartHistory(const char *format, ...)
{
  char buffer[512]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
}

int GetCurrentHour()
{
    return CTime::GetCurrentTime().GetHour();
}

int GetCurrentMin()
{
    return CTime::GetCurrentTime().GetMinute();
}

int GetCurrentSec()
{
    return CTime::GetCurrentTime().GetSecond();
}

void clear_file(const char *directory, int keepCount)
{
  (void)directory;
  (void)keepCount;
}

int MyMessageBox(const char *title, const char *format, ...)
{
  char buffer[1024]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  MessageBoxA(nullptr, buffer, title, MB_OK | MB_ICONERROR);
  return 0;
}

int MyCrtDebugReportHook(int reportType, char *message, int *returnValue)
{
  (void)reportType;
  (void)message;
  if (returnValue != nullptr)
  {
    *returnValue = 0;
  }
  return 0;
}

void ServerProgramExit(const char *source, int reason)
{
  char buffer[256]{};
  _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "ServerProgramExit: %s (%d)", source, reason);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  ExitProcess(static_cast<unsigned int>(reason));
}

void StripEXT(char *szPath)
{
  int idx = static_cast<int>(std::strlen(szPath)) - 1;
  for (int i = idx; i >= 0; --i)
  {
    if (szPath[i] == '.')
    {
      szPath[i] = '\0';
      return;
    }
  }
}

void StripPath(char *szPath)
{
  const int len = static_cast<int>(std::strlen(szPath));
  int slash = len - 2;
  for (int i = len - 2; i >= 0; --i)
  {
    if (szPath[i] == '\\')
      break;
    --slash;
  }
  const int start = slash + 1;
  if (start < len - 1)
  {
    const int count = len - 1 - start;
    char temp[256]{};
    memcpy_0(temp, &szPath[start], static_cast<unsigned int>(count));
    temp[count] = '\0';
    strcpy_s(szPath, 256, temp);
  }
}

void StripName(char *szPath)
{
  int idx = static_cast<int>(std::strlen(szPath)) - 1;
  for (char *cur = &szPath[idx]; *cur != '\\'; --cur)
    --idx;
  if (idx)
    szPath[idx + 1] = '\0';
}

char W2M(char *lpwStr, char *szTran, unsigned int wTranBuffSize)
{
  if (!wTranBuffSize)
  {
    return 0;
  }

  if (strlen_0(lpwStr) + 1 > wTranBuffSize)
  {
    return 0;
  }

  strcpy_s(szTran, wTranBuffSize, lpwStr);
  return 1;
}

char M2W(char *lpStr, char *wszTran, unsigned int wTranBuffSize)
{
  if (!wTranBuffSize)
  {
    return 0;
  }

  if (strlen_0(lpStr) + 1 > wTranBuffSize)
  {
    return 0;
  }

  strcpy_0(wszTran, lpStr);
  return 1;
}

bool IsServerMode()
{
    return dword_184A77F38 != 0;
}

bool IsInitR3Engine() { return dword_184A77F3C != 0; }
float R3GetTime() { return *reinterpret_cast<float *>(&dword_184A77EE8); }
float R3GetLoopTime() { return *reinterpret_cast<float *>(&dword_184A77EF0); }

IDirect3DDevice8 *GetD3dDevice()
{
  return qword_184A7B2C8;
}

void ResetTexMemSize()
{
  dword_184A79D60 = 0;
}
bool IsExistFile(char *szFileName)
{
  FILE *fp = nullptr;
  if (fopen_s(&fp, szFileName, "rb") == 0 && fp)
  {
    fclose(fp);
    return true;
  }
  return false;
}
unsigned int GetFileSizeAndMergeFile(char *szFileName)
{
  if (!szFileName || !*szFileName)
    return 0;
  if (!qword_184A7B208)
    return GetFileSize(szFileName);
  unsigned int result = qword_184A7B208->GetFileSize(szFileName);
  if (!result)
    return GetFileSize(szFileName);
  return result;
}

__int64 IsExistFileAndMergeFile(char *szFileName)
{
  if (szFileName && *szFileName)
  {
    if (qword_184A7B208 && qword_184A7B208->IsExistFile(szFileName))
      return 1;
    FILE *fp = fopen(szFileName, "rb");
    if (fp)
    {
      fclose(fp);
      return 1;
    }
  }
  return 0;
}

__int64 CalcFileSize(char *pszFileName)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x80000000, 1u, nullptr, 3u, 0x80u, nullptr);
  if (hFile == reinterpret_cast<HANDLE>(-1))
    return 0;

  DWORD v6 = SetFilePointer(hFile, 0, nullptr, 0);
  DWORD v7 = SetFilePointer(hFile, 0, nullptr, 2u);
  CloseHandle(hFile);
  return static_cast<__int64>(v7 - v6);
}
_R3MATERIAL *LoadMainR3M(char *szFileName)
{
  char *v1 = szFileName;
  char v5[256]{};

  reinterpret_cast<unsigned int *>(&qword_184A79818)[1] = 0xFF000000u;
  dword_184A79920 = 1;
  dword_184A798D0 = -1;
  dword_184A79808 = 0;
  dword_184A79828 = 0;
  dword_184A7980C = 0;
  dword_184A79810 = 0;
  dword_184A79A24 = 0;
  dword_184A79A28 = 0;

  char *dst = v5;
  while ((*dst++ = *szFileName++) != '\0')
    ;
  StripEXT(v5);
  strcat(v5, ".r3x");
  LoadR3X(v5);
  if (qword_184A79DA8)
    ReleaseMainMaterial();

  _R3MATERIAL *result = LoadIndependenceR3M(v1);
  qword_184A79DA8 = result;
  if (result)
    dword_184A79DB0 = *reinterpret_cast<unsigned int *>(result);
  return result;
}
_R3MATERIAL *LoadSubR3M(char *szFileName)
{
  return LoadIndependenceR3M(szFileName);
}

_R3MATERIAL *LoadIndependenceR3M(char *szFileName)
{
  FILE *fp = fopenMFM(szFileName, "rb");
  if (!fp)
  {
    Warning(szFileName, aAiAaiai_2);
    return nullptr;
  }

  float version = 0.0f;
  fread(&version, 4, 1, fp);
  if (version != 1.1f)
  {
    Warning(szFileName, aAiAaiai_5);
    fclose(fp);
    return nullptr;
  }

  int matNum = 0;
  fread(&matNum, 4, 1, fp);
  auto *mat = static_cast<unsigned int *>(Dmalloc(470 * matNum));
  memset_0(mat, 0, 470LL * matNum);

  int startId = static_cast<int>(GetNowR3D3DTexCnt()) - 1;
  *mat = matNum;
  if (matNum > 0)
  {
    unsigned int *v9 = mat + 34;
    for (int i = 0; i < *mat; ++i)
    {
      const __int64 base = 470LL * i;
      fread(reinterpret_cast<char *>(mat) + base + 144, 4, 1, fp);
      fread(reinterpret_cast<char *>(mat) + base + 132, 4, 1, fp);
      fread(reinterpret_cast<char *>(mat) + base + 136, 4, 1, fp);
      fread(reinterpret_cast<char *>(mat) + base + 140, 4, 1, fp);
      fread(reinterpret_cast<char *>(mat) + base + 4, 0x80, 1, fp);
      if (*v9 != 0xFFFFFFFFu)
        *v9 += startId;
      if (static_cast<int>(v9[2]) > 0)
      {
        unsigned int *v12 = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(v9) + 14);
        for (int j = 0; j < static_cast<int>(v9[2]); ++j)
        {
          fread(reinterpret_cast<char *>(mat) + 46LL * j + base + 148, 0x2E, 1, fp);
          *v12 += startId;
          v12 = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(v12) + 46);
        }
      }
      if (*v9 != 0xFFFFFFFFu)
        *(v9 - 1) |= 2u;
      v9 = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(v9) + 470);
    }
  }

  fclose(fp);
  return reinterpret_cast<_R3MATERIAL *>(mat);
}

_R3MATERIAL *LoadSubMaterial(char *szFileName)
{
  char String[256]{};
  char v62[256]{};

  char *src = szFileName;
  char *dst = aTexture;
  while ((*dst++ = *src++) != '\0')
    ;
  strcpy_s(v62, szFileName);
  strcat(v62, "MainMaterial.mst");

  FILE *fp = fopenMFM(v62, "rt");
  if (!fp)
  {
    Warning(v62, aAiAaiai_3);
    return nullptr;
  }

  unsigned int *mat = nullptr;
  int matNum = 0;
  if (fscanf(fp, "%s", String) != -1)
  {
    do
    {
      if (!strcmp(String, "end"))
        break;

      _strupr(String);
      if (!strcmp(String, "*MATERIAL_NUM"))
      {
        fscanf(fp, "%s", String);
        matNum = atoi(String);
        if (matNum >= 1024)
          Error(aAo_2, byte_140883769);
        mat = static_cast<unsigned int *>(Dmalloc(470 * matNum));
        memset_0(mat, 0, 470LL * matNum);
        fscanf(fp, "%s", String);
        if (matNum > 0)
        {
          char *block = reinterpret_cast<char *>(mat);
          for (int i = 0; i < matNum; ++i)
          {
            sub_140502610(block);
            block += 470;
          }
          for (int i = 0; i < matNum; ++i)
          {
            strcpy_s(v62, szFileName);
            fscanf(fp, "%s", String);
            strcat(v62, String);
            strcpy_s(reinterpret_cast<char *>(mat) + 470LL * i + 4, 256, String);
            strcat(v62, ".mst");
            fscanf(fp, "%s", String);
            int matIdx = atoi(String);
            sub_140503190(v62, reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(mat) + 470LL * matIdx));
          }
        }
      }

      if (!strcmp(String, "*FOG_START"))
      {
        dword_184A79808 = 1;
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79844 = *reinterpret_cast<unsigned int *>(&v);
        float v2 = static_cast<float>(atof(String));
        dword_184A79814 = *reinterpret_cast<unsigned int *>(&v2);
        dword_184A7982C = dword_184A79814;
      }

      if (!strcmp(String, "*FOG_END"))
      {
        dword_184A79808 = 1;
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        *reinterpret_cast<float *>(&qword_184A79848) = v;
        float v2 = static_cast<float>(atof(String));
        *reinterpret_cast<float *>(&qword_184A79818) = v2;
        dword_184A79834 = *reinterpret_cast<unsigned int *>(&v2);
        dword_184A79778 = *reinterpret_cast<unsigned int *>(&v2);
      }

      if (!strcmp(String, "*FOG_COLOR"))
      {
        dword_184A79808 = 1;
        if (*reinterpret_cast<float *>(&qword_184A79818) == 0.0f)
        {
          *reinterpret_cast<float *>(&qword_184A79818) = 5000.0f;
          *reinterpret_cast<float *>(&qword_184A79848) = 5000.0f;
        }
        if (*reinterpret_cast<float *>(&dword_184A79814) == 0.0f)
        {
          *reinterpret_cast<float *>(&dword_184A79814) = 5.0f;
          *reinterpret_cast<float *>(&dword_184A79844) = 5.0f;
        }
        fscanf(fp, "%s", String);
        int r = atoi(String);
        fscanf(fp, "%s", String);
        int g = atoi(String);
        fscanf(fp, "%s", String);
        unsigned int color = static_cast<unsigned int>(atoi(String) | ((g | ((r | 0xFFFFFF00) << 8)) << 8));
        dword_184A7981C = color;
        dword_184A7984C = color;
        dword_184A7983C = color;
      }

      if (!strcmp(String, "*FOG_COLOR2"))
      {
        fscanf(fp, "%s", String);
        int r = atoi(String);
        fscanf(fp, "%s", String);
        int g = atoi(String);
        fscanf(fp, "%s", String);
        dword_184A79840 = static_cast<unsigned int>(((g | ((r | 0xFFFFFF00) << 8)) << 8) | atoi(String));
      }

      if (!strcmp(String, "*FOG_START2"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79830 = *reinterpret_cast<unsigned int *>(&v);
      }

      if (!strcmp(String, "*FOG_END2"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79838 = *reinterpret_cast<unsigned int *>(&v);
      }

      if (!strcmp(String, "*NO_FOG_SKY"))
        dword_184A7980C = 1;

      if (!strcmp(String, "*FOG_RANGE"))
      {
        fscanf(fp, "%s", String);
        if (!strcmp(String, "TRUE"))
          dword_184A79810 = 1;
      }

      if (!strcmp(String, "*LENS_FLARE_SCALE"))
      {
        float *dstScale = unk_184A798D4;
        float *end = reinterpret_cast<float *>(&dword_184A79914);
        while (dstScale < end)
        {
          fscanf(fp, "%s", String);
          *dstScale++ = static_cast<float>(atof(String));
        }
      }

      if (!strcmp(String, "*LENS_FLARE"))
      {
        fscanf(fp, "%s", String);
        strcpy_s(v62, szFileName);
        strcat(v62, String);
        dword_184A798D0 = R3GetPreTextureId(v62);
        char *state = reinterpret_cast<char *>(&stState);
        size_t idx = 0;
        do
        {
          state[idx + 2143] = v62[idx];
        } while (v62[idx++]);
      }

      if (!strcmp(String, "*LENS_FLARE_POS"))
      {
        fscanf(fp, "%s", String);
        *reinterpret_cast<float *>(&dword_184A79914) = static_cast<float>(atof(String));
        fscanf(fp, "%s", String);
        *reinterpret_cast<float *>(&dword_184A7991C) = static_cast<float>(atof(String));
        fscanf(fp, "%s", String);
        *reinterpret_cast<float *>(&dword_184A79918) = static_cast<float>(atof(String));
      }

      if (!strcmp(String, "*ENVIRONMENT"))
      {
        fscanf(fp, "%s", String);
        char *state = reinterpret_cast<char *>(&stState);
        size_t idx = 0;
        do
        {
          state[idx + 2355] = String[idx];
        } while (String[idx++]);
      }

      if (!strcmp(String, "*ENVIRONMENT_ID"))
      {
        fscanf(fp, "%s", String);
        dword_184A79A24 = atoi(String);
      }

      if (!strcmp(String, "*ENVIRONMENT2"))
      {
        fscanf(fp, "%s", String);
        char *state = reinterpret_cast<char *>(&stState);
        size_t idx = 0;
        do
        {
          state[idx + 2483] = String[idx];
        } while (String[idx++]);
      }

      if (!strcmp(String, "*ENVIRONMENT_ID2"))
      {
        fscanf(fp, "%s", String);
        dword_184A79A28 = atoi(String);
      }

      _strlwr(String);
      if (!strcmp(String, "*weather"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79820 &= ~0xFFu;
        dword_184A79820 |= static_cast<unsigned int>(v * 255.0f);
      }
      if (!strcmp(String, "*haze"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79820 &= ~(0xFFu << 8);
        dword_184A79820 |= static_cast<unsigned int>(v * 255.0f) << 8;
      }
      if (!strcmp(String, "*special_weather"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79820 &= ~(0xFFu << 16);
        dword_184A79820 |= static_cast<unsigned int>(v * 255.0f) << 16;
      }
      if (!strcmp(String, "*no_sun"))
        dword_184A79824 |= 1u;
      if (!strcmp(String, "*no_sky_color"))
        dword_184A79824 |= 4u;
      if (!strcmp(String, "*use_fog_fog2"))
        dword_184A79824 |= 2u;
    } while (fscanf(fp, "%s", String) != -1);
  }

  if (mat)
    *mat = static_cast<unsigned int>(matNum);
  fclose(fp);
  return reinterpret_cast<_R3MATERIAL *>(mat);
}

void ReleaseMainMaterial()
{
  if (qword_184A79DA8)
    Dfree(qword_184A79DA8);
  dword_184A79DB0 = 0;
  qword_184A79DA8 = nullptr;
}

void ReleaseSubMaterial(struct _R3MATERIAL *mat)
{
  if (mat)
    Dfree(mat);
}

void AdjustIndependenceR3M(struct _R3MATERIAL *mat, int startId, int newStartId)
{
  if (!mat)
    return;
  int matNum = *reinterpret_cast<int *>(mat);
  if (matNum <= 0)
    return;
  char *base = reinterpret_cast<char *>(mat) + 136;
  for (int i = 0; i < matNum; ++i)
  {
    int *detail = reinterpret_cast<int *>(base);
    if (*detail != -1)
      *detail = newStartId + *detail - startId;
    if (detail[2] > 0)
    {
      char *layer = base + 14;
      for (int j = 0; j < detail[2]; ++j)
      {
        *reinterpret_cast<int *>(layer - 46) += (newStartId - startId);
        layer += 46;
      }
    }
    base += 470;
  }
}
struct R3Texture *R3GetTexInfoR3T(char *szFileName, int a2)
{
  unsigned int *v2 = static_cast<unsigned int *>(qword_184A79D70);
  if (!qword_184A79D70)
  {
    qword_184A79D70 = Dmalloc(144 * dword_140978968);
    memset_0(qword_184A79D70, 0, 144LL * dword_140978968);
    qword_184A79D68 = Dmalloc(16 * dword_140978960);
    memset_0(qword_184A79D68, 0, 16LL * dword_140978960);
    v2 = static_cast<unsigned int *>(qword_184A79D70);
  }

  int v5 = dword_140978964;
  if (dword_140978964 >= dword_140978968)
  {
    v2 = static_cast<unsigned int *>(ReAlloc(v2, 144 * dword_140978968, 16 * (9 * dword_140978968 + 144)));
    int v7 = dword_140978968;
    qword_184A79D70 = v2;
    memset_0(&v2[36 * dword_140978968], 0, 0x900uLL);
    v5 = dword_140978964;
    dword_140978968 = v7 + 16;
  }

  if ((stR3TexManageFlag & 1) != 0 || v5 <= 1)
  {
    if (std::strlen(szFileName) < 0x7F)
    {
      FILE *fp = fopenMFM(szFileName, "rb");
      if (fp)
      {
        char Buffer[12]{};
        int texNum = 0;
        fread(Buffer, 4, 1, fp);
        fread(&texNum, 4, 1, fp);
        fclose(fp);

        unsigned int *entry = v2 + 36 * dword_140978964;
        entry[34] = static_cast<unsigned int>(a2);
        entry[32] = dword_14097895C;
        entry[33] = static_cast<unsigned int>(texNum);
        char *dst = reinterpret_cast<char *>(entry);
        while ((*dst++ = *szFileName++) != '\0')
          ;

        if (dword_14097895C + static_cast<unsigned int>(texNum) >= dword_140978960)
        {
          unsigned int *newArr =
            static_cast<unsigned int *>(ReAlloc(qword_184A79D68, 16 * dword_140978960, 16 * (dword_140978960 + texNum)));
          int old = dword_140978960;
          qword_184A79D68 = newArr;
          memset_0(&newArr[4 * old], 0, 16LL * texNum);
          dword_140978960 = old + texNum;
        }
        dword_14097895C += texNum;
        ++dword_140978964;
        return reinterpret_cast<R3Texture *>(entry);
      }

      char String[256]{};
      strcpy_s(String, szFileName);
      _strlwr(String);
      StripEXT(String);
      size_t len = std::strlen(String);
      if (len < 3 || String[len - 3] != 'l' || String[len - 2] != 'g' || String[len - 1] != 't')
        Warning(szFileName, aAiAaiai_4);
      return nullptr;
    }
    Warning(aA_3, byte_140883769);
    return nullptr;
  }

  for (int idx = 1; idx < v5; ++idx)
  {
    char *name = reinterpret_cast<char *>(&v2[36 * idx]);
    if (std::strcmp(name, szFileName) == 0)
    {
      ++reinterpret_cast<unsigned int *>(name)[35];
      return reinterpret_cast<R3Texture *>(name);
    }
  }

  if (std::strlen(szFileName) < 0x7F)
  {
    FILE *fp = fopenMFM(szFileName, "rb");
    if (fp)
    {
      char Buffer[12]{};
      int texNum = 0;
      fread(Buffer, 4, 1, fp);
      fread(&texNum, 4, 1, fp);
      fclose(fp);

      unsigned int *entry = v2 + 36 * dword_140978964;
      entry[34] = static_cast<unsigned int>(a2);
      entry[32] = dword_14097895C;
      entry[33] = static_cast<unsigned int>(texNum);
      char *dst = reinterpret_cast<char *>(entry);
      while ((*dst++ = *szFileName++) != '\0')
        ;

      if (dword_14097895C + static_cast<unsigned int>(texNum) >= dword_140978960)
      {
        unsigned int *newArr =
          static_cast<unsigned int *>(ReAlloc(qword_184A79D68, 16 * dword_140978960, 16 * (dword_140978960 + texNum)));
        int old = dword_140978960;
        qword_184A79D68 = newArr;
        memset_0(&newArr[4 * old], 0, 16LL * texNum);
        dword_140978960 = old + texNum;
      }
      dword_14097895C += texNum;
      ++dword_140978964;
      return reinterpret_cast<R3Texture *>(entry);
    }

    char String[256]{};
    strcpy_s(String, szFileName);
    _strlwr(String);
    StripEXT(String);
    size_t len = std::strlen(String);
    if (len < 3 || String[len - 3] != 'l' || String[len - 2] != 'g' || String[len - 1] != 't')
      Warning(szFileName, aAiAaiai_4);
    return nullptr;
  }
  Warning(aA_3, byte_140883769);
  return nullptr;
}

int R3GetPreTextureId(char *szFileName)
{
  unsigned int *v1 = static_cast<unsigned int *>(qword_184A79D70);
  unsigned int *v5 = nullptr;
  int v4 = 0;

  if (qword_184A79D70)
  {
    v5 = static_cast<unsigned int *>(qword_184A79D68);
    v4 = dword_140978960;
  }
  else
  {
    qword_184A79D70 = Dmalloc(144 * dword_140978968);
    memset_0(qword_184A79D70, 0, 144LL * dword_140978968);
    v5 = static_cast<unsigned int *>(Dmalloc(16 * dword_140978960));
    qword_184A79D68 = v5;
    memset_0(v5, 0, 16LL * dword_140978960);
    v1 = static_cast<unsigned int *>(qword_184A79D70);
    v4 = dword_140978960;
  }

  int v6 = dword_140978964;
  if (dword_140978964 >= dword_140978968)
  {
    v1 = static_cast<unsigned int *>(ReAlloc(v1, 144 * dword_140978968, 16 * (9 * dword_140978968 + 144)));
    int v8 = dword_140978968;
    qword_184A79D70 = v1;
    memset_0(&v1[36 * dword_140978968], 0, 0x900uLL);
    v5 = static_cast<unsigned int *>(qword_184A79D68);
    v4 = dword_140978960;
    v6 = dword_140978964;
    dword_140978968 = v8 + 16;
  }

  if (std::strlen(szFileName) >= 0x7F)
  {
    Warning(aA_3, byte_140883769);
    return 0;
  }

  if ((stR3TexManageFlag & 1) == 0 && v6 > 1)
  {
    for (int i = 1; i < v6; ++i)
    {
      char *name = reinterpret_cast<char *>(&v1[36 * i]);
      if (std::strcmp(name, szFileName) == 0)
      {
        ++reinterpret_cast<unsigned int *>(name)[35];
        return reinterpret_cast<unsigned int *>(name)[32];
      }
    }
  }

  char *dst = reinterpret_cast<char *>(v1) + 144LL * v6;
  while ((*dst++ = *szFileName++) != '\0')
    ;

  if ((stR3TexManageFlag & 1) != 0 || v6 <= 1)
  {
    v1[36 * v6 + 34] = 40960;
    v1[36 * v6 + 32] = dword_14097895C;
    v1[36 * v6 + 33] = 1;
    int id = dword_14097895C + 1;
    dword_14097895C = id;
    dword_140978964 = v6 + 1;
    if (id >= v4)
    {
      unsigned int *v20 = static_cast<unsigned int *>(ReAlloc(v5, 16 * v4, 16 * (v4 + 16)));
      qword_184A79D68 = v20;
      memset_0(&v20[4 * dword_140978960], 0, 0x100uLL);
      dword_140978960 += 16;
      id = dword_14097895C;
    }
    return id - 1;
  }

  int v16 = 1;
  unsigned int *v17 = v1 + 36;
  while (v17[33] || *reinterpret_cast<unsigned char *>(v17))
  {
    ++v16;
    v17 += 36;
    if (v16 >= v6)
      break;
  }
  if (v16 >= v6)
  {
    v1[36 * v6 + 34] = 40960;
    v1[36 * v6 + 32] = dword_14097895C;
    v1[36 * v6 + 33] = 1;
    int id = dword_14097895C + 1;
    dword_14097895C = id;
    dword_140978964 = v6 + 1;
    if (id >= v4)
    {
      unsigned int *v20 = static_cast<unsigned int *>(ReAlloc(v5, 16 * v4, 16 * (v4 + 16)));
      qword_184A79D68 = v20;
      memset_0(&v20[4 * dword_140978960], 0, 0x100uLL);
      dword_140978960 += 16;
      id = dword_14097895C;
    }
    return id - 1;
  }

  unsigned int *entry = v1 + 36 * v16;
  entry[34] = 40960;
  entry[32] = v16;
  entry[33] = 1;
  return v16;
}

void R3GetPreAniTextureId(char *path, char *name, int *startId, int *num)
{
  int v6 = 0;
  __int64 v7 = 0;
  char *v9 = name;
  char v18[128]{};
  char v17[32]{};
  char Buffer[256]{};

  do
  {
    if (*v9 == '.')
      break;
    char v10 = 0;
    if (*v9 != '$')
      v10 = *v9;
    ++v7;
    ++v9;
    v18[v6++] = v10;
  } while (v7 < 128);

  v17[0] = '.';
  v17[1] = name[v6 + 1];
  v17[2] = name[v6 + 2];
  v17[3] = name[v6 + 3];
  v17[4] = 0;

  *num = sub_140501380(path, v18, v17);
  for (int i = 1; i < *num + 1; ++i)
  {
    strcpy_s(Buffer, path);
    sprintf(Buffer, "%s%s%04d%s", path, v18, i, v17);
    int preId = R3GetPreTextureId(Buffer);
    if (i == 1)
      *startId = preId;
  }
}

void R3ReleasePreTextures()
{
  if (qword_184A79D70)
  {
    Dfree(qword_184A79D70);
    qword_184A79D70 = nullptr;
  }
  dword_140978964 = 1;
  if (qword_184A79D68)
  {
    Dfree(qword_184A79D68);
    qword_184A79D68 = nullptr;
  }
  dword_14097895C = 1;
}

__int64 IsAniTex(char *name)
{
  if (!name)
    return 0;
  unsigned int idx = 0;
  const unsigned __int64 len = std::strlen(name) + 1;
  if (len == 1)
    return 0;
  while (name[idx] != '$')
  {
    if (++idx >= len - 1)
      return 0;
  }
  return 1;
}
void SetNoLodTextere()
{
  if (!dword_184A79DB0)
    return;
  unsigned int v0 = 0;
  char *v1 = reinterpret_cast<char *>(qword_184A79DA8);
  __int64 v2 = 0;
  do
  {
    if ((v1[v2 + 132] & 2) != 0)
    {
      SetR3D3DTexture(*reinterpret_cast<unsigned int *>(&v1[v2 + 136]), 0x80000000);
      v1 = reinterpret_cast<char *>(qword_184A79DA8);
      if (*reinterpret_cast<unsigned int *>(v1 + v2 + 144))
      {
        unsigned int v3 = 0;
        __int64 v4 = v2;
        do
        {
          SetR3D3DTexture(*reinterpret_cast<unsigned int *>(&v1[v4 + 150]), 0x80000000);
          v1 = reinterpret_cast<char *>(qword_184A79DA8);
          ++v3;
          v4 += 46;
        } while (v3 < *reinterpret_cast<unsigned int *>(v1 + v2 + 144));
      }
    }
    ++v0;
    v2 += 470;
  } while (v0 < dword_184A79DB0);
}
void LoadR3T(struct R3Texture *pTex)
{
  if (!pTex)
    return;
  FILE *fp = fopenMFM(reinterpret_cast<char *>(pTex), "rb");
  if (!fp)
  {
    Warning(reinterpret_cast<char *>(pTex), aAiAaiai);
    return;
  }
  float version = 0.0f;
  unsigned int texNum = 0;
  fread(&version, 4, 1, fp);
  fread(&texNum, 4, 1, fp);
  if (version >= 1.1f)
  {
    fseek(fp, static_cast<long>(texNum << 7), SEEK_CUR);
    for (unsigned int i = 0; i < texNum; ++i)
    {
      size_t size = 0;
      fread(&size, 4, 1, fp);
      unsigned int *flags = static_cast<unsigned int *>(qword_184A79D68);
      if (_bittest(reinterpret_cast<const long *>(flags + 4 * i + 4 * pTex->mStartID + 2), 0x1F))
        reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * i + 2 * pTex->mStartID] =
          reinterpret_cast<unsigned long long>(R3LoadDDSFromFP(fp, size, 0, 0x800, 0x800));
      else
        reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * i + 2 * pTex->mStartID] =
          reinterpret_cast<unsigned long long>(R3LoadDDSFromFP(fp, size, dword_184A797D0, 0x800, 0x800));
    }
    fclose(fp);
  }
  else
  {
    Warning(reinterpret_cast<char *>(pTex), aAiAaiaiAiau);
  }
}
unsigned int GetNowTexMemSize()
{
  return dword_184A79D60;
}
__int64 GetNowR3D3DTexCnt()
{
  return dword_14097895C;
}

void SetNowR3D3DTexCnt(int cnt)
{
  if (cnt < dword_14097895C)
    memset_0(reinterpret_cast<char *>(qword_184A79D68) + 16 * cnt, 0, 16LL * (dword_14097895C - cnt));
  dword_14097895C = cnt;
}

__int64 GetNowR3TexCnt()
{
  return dword_140978964;
}

void SetNowR3TexCnt(int cnt)
{
  if (cnt < dword_140978964)
  {
    char *v1 = reinterpret_cast<char *>(qword_184A79D70) + 144 * cnt;
    __int64 v2 = dword_140978964 - cnt;
    do
    {
      v1 += 144;
      --v2;
      reinterpret_cast<unsigned int *>(v1)[-36] = 0;
      reinterpret_cast<unsigned int *>(v1)[-35] = 0;
      reinterpret_cast<unsigned int *>(v1)[-34] = 0;
      reinterpret_cast<unsigned int *>(v1)[-33] = 0;
    } while (v2);
  }
  dword_140978964 = cnt;
}

void SetR3TexManageFlag(unsigned int flag)
{
  stR3TexManageFlag = flag;
}

__int64 GetR3TexManageFlag()
{
  return stR3TexManageFlag;
}

void R3LoadTextureMem(int id)
{
  if (!reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id] && dword_140978964 > 1)
  {
    int v2 = 1;
    unsigned int *v3 = reinterpret_cast<unsigned int *>(qword_184A79D70) + 68;
    do
    {
      if (*v3 > static_cast<unsigned int>(id))
        break;
      ++v2;
      v3 += 36;
    } while (v2 < dword_140978964);

    if (_bittest(reinterpret_cast<const long *>(qword_184A79D70) + 36 * v2 - 2, 0xF))
    {
      if (!reinterpret_cast<unsigned int *>(qword_184A79D70)[36 * v2 - 1])
      {
        char *name = reinterpret_cast<char *>(qword_184A79D70) + 144 * v2 - 144;
        unsigned int mip = _bittest(reinterpret_cast<const long *>(qword_184A79D68) + 4 * (2 * id) + 2, 0x1F)
                             ? 0
                             : dword_184A797D0;
        reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id] =
          reinterpret_cast<unsigned long long>(R3LoadDDS(name, mip, 0x800, 0x800));
      }
    }
    else
    {
      Warning(aR3tAaia_0, byte_140883769);
    }
  }
}

void R3ReleaseTextureMem(int id)
{
  if (!qword_184A79D68 || id == -1)
    return;
  if (!reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id])
    return;

  char *v4 = reinterpret_cast<char *>(qword_184A79D70);
  int v5 = 1;
  if (dword_140978964 > 1)
  {
    unsigned int *v6 = reinterpret_cast<unsigned int *>(qword_184A79D70) + 68;
    do
    {
      if (*v6 > static_cast<unsigned int>(id))
        break;
      ++v5;
      v6 += 36;
    } while (v5 < dword_140978964);
  }

  __int64 v7 = 18LL * v5;
  int refCnt = reinterpret_cast<unsigned int *>(qword_184A79D70)[36 * v5 - 1];
  if (refCnt)
  {
    reinterpret_cast<unsigned int *>(qword_184A79D70)[36 * v5 - 1] = refCnt - 1;
  }
  else
  {
    if (!_bittest(reinterpret_cast<const long *>(qword_184A79D70) + 36 * v5 - 2, 0xF))
    {
      Warning(aR3tAaia, byte_140883769);
      v4 = reinterpret_cast<char *>(qword_184A79D70);
    }
    v4[8 * v7 - 144] = 0;
    if (reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id])
    {
      auto *tex = reinterpret_cast<IDirect3DTexture8 *>(reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id]);
      if (tex && tex->__vftable && tex->__vftable->Release)
        tex->__vftable->Release(reinterpret_cast<IDA_IUnknown *>(tex));
      reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id] = 0;
    }
  }
}

struct IDirect3DTexture8 *R3GetSurface(int id)
{
  if (id <= 0 || id > static_cast<int>(dword_14097895C))
    return nullptr;
  return reinterpret_cast<IDirect3DTexture8 *>(reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id]);
}

void SetR3D3DTexture(unsigned int id, int flag)
{
  if (id && dword_14097895C > id)
    reinterpret_cast<unsigned int *>(qword_184A79D68)[4 * id + 2] = static_cast<unsigned int>(flag);
}

struct IDirect3DTexture8 *R3LoadDDS(char *name, unsigned int mipmap, unsigned int maxX, unsigned int maxY)
{
  unsigned int size = GetFileSizeAndMergeFile(name);
  FILE *fp = fopenMFM(name, "rb");
  if (!fp)
    return nullptr;
  IDirect3DTexture8 *tex = R3LoadDDSFromFP(fp, size, mipmap, maxX, maxY);
  if (!tex)
    tex = R3LoadDDSAndTextureMem(name);
  fclose(fp);
  return tex;
}

struct IDirect3DTexture8 *R3LoadDDSFromFP(FILE *fp, size_t size, unsigned int mipmap, unsigned int maxX, unsigned int maxY)
{
  unsigned int skip = 0;
  auto *buf = static_cast<unsigned int *>(std::malloc(size));
  if (!buf)
    return nullptr;

  fread(buf, 0x90, 1, fp);
  if (*buf != 542327876)
  {
    sub_1404FFFB0(reinterpret_cast<__int64>(buf));
    if (*buf != 542327876)
    {
      std::free(buf);
      return nullptr;
    }
  }

  if (buf[8])
  {
    skip = static_cast<unsigned int>(GetMipMapSkipSize(reinterpret_cast<_DDSURFACEDESC2 *>(buf + 2), mipmap, maxX, maxY));
    if (skip)
      fseek(fp, skip, SEEK_CUR);
  }

  int dataSize = static_cast<int>(size) - static_cast<int>(skip) - 144;
  fread(buf + 36, dataSize, 1, fp);
  sub_1404FFF30(buf);

  IDirect3DDevice8 *dev = GetD3dDevice();
  unsigned long long outTex = 0;
  int hr = static_cast<int>(D3DXCreateTextureFromFileInMemory_0(dev, buf, static_cast<unsigned int>(dataSize + 144), &outTex));
  std::free(buf);
  if (hr < 0)
    return nullptr;
  dword_184A79D60 += static_cast<unsigned int>(dataSize);
  return reinterpret_cast<IDirect3DTexture8 *>(outTex);
}

struct IDirect3DTexture8 *R3LoadDDSAndTextureMem(char *name)
{
  IDirect3DDevice8 *dev = GetD3dDevice();
  int hr = static_cast<int>(D3DXCreateTextureFromFileExA_0(dev, name, 0xFFFFFFFFLL));
  if (hr < 0)
    return nullptr;
  return nullptr;
}
_R3MATERIAL *LoadMainMaterial(char *szFileName)
{
  reinterpret_cast<unsigned int *>(&qword_184A79818)[1] = 0xFF000000u;
  dword_184A79920 = 1;
  dword_184A798D0 = -1;
  dword_184A79808 = 0;
  dword_184A79828 = 0;
  dword_184A7980C = 0;
  dword_184A79810 = 0;
  dword_184A79A24 = 0;
  dword_184A79A28 = 0;
  if (qword_184A79DA8)
    ReleaseMainMaterial();
  _R3MATERIAL *result = LoadSubMaterial(szFileName);
  qword_184A79DA8 = result;
  dword_184A79DB0 = result ? *reinterpret_cast<unsigned int *>(result) : 0;
  return result;
}

_R3MATERIAL *GetMainMaterial()
{
  return qword_184A79DA8;
}

unsigned int GetMainMaterialNum()
{
  return dword_184A79DB0;
}

void SetMergeFileManager(CMergeFileManager *pMgr)
{
  qword_184A7B208 = pMgr;
}

void LoadLightMap(char *szFileName)
{
  char v10[256]{};
  char *dst = v10;
  while ((*dst++ = *szFileName++) != '\0')
    ;
  qword_184A79DA0 = reinterpret_cast<unsigned long long>(R3GetTexInfoR3T(v10, 0));
  stLightmap = LoadR3TLightMap(reinterpret_cast<R3Texture *>(qword_184A79DA0), static_cast<D3DFORMAT>(D3DFMT_R5G6B5));
  if (stLightmap)
  {
    dword_184A79D88 = *reinterpret_cast<unsigned int *>(qword_184A79DA0 + 132);
    LightmapTexID = static_cast<int *>(Dmalloc(4 * dword_184A79D88));
    if (dword_184A79D88 > 0)
    {
      int v5 = 0;
      for (int i = 0; i < static_cast<int>(dword_184A79D88); ++i)
      {
        LightmapTexID[i] = v5 + *reinterpret_cast<unsigned int *>(qword_184A79DA0 + 128);
        ++v5;
      }
    }
  }
}

void ReleaseLightMap()
{
  if (LightmapTexID)
  {
    Dfree(LightmapTexID);
    LightmapTexID = nullptr;
    qword_184A79DA0 = 0;
  }
  if (stLightmap)
  {
    if (dword_184A79D88 > 0)
    {
      for (int i = 0; i < static_cast<int>(dword_184A79D88); ++i)
      {
        Dfree(reinterpret_cast<void **>(stLightmap)[i * 2 + 1]);
        Dfree(reinterpret_cast<void **>(stLightmap)[i * 2]);
      }
    }
    dword_184A79D88 = 0;
    Dfree(stLightmap);
    stLightmap = nullptr;
  }
}

struct IDirect3DTexture8 *GetLightMapSurface(int id)
{
  if (LightmapTexID)
    return R3GetSurface(LightmapTexID[id]);
  return nullptr;
}

unsigned int GetLightMapTexSize()
{
  return dword_184A79C54;
}

unsigned int GetLightMapColor(float *const uv, int id)
{
  float u = uv[0];
  if (u <= 0.0f)
    u = 0.0f;
  float v = uv[1];
  if (v <= 0.0f)
    v = 0.0f;
  if (u >= 1.0f)
    u = 1.0f;
  if (v >= 1.0f)
    v = 1.0f;
  uv[0] = u;
  uv[1] = v;
  if (!dword_184A79D88)
    return 0xFFFFFFFFu;
  unsigned short *lm = reinterpret_cast<unsigned short **>(stLightmap)[id];
  __int64 idx = static_cast<int>(lm[0] * u) + lm[0] * static_cast<__int64>(static_cast<int>(lm[1] * v));
  unsigned short val = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned short **>(lm)[1]) + 2 * idx);
  return ((val >> 11) << 19) | (8 * (val & 0x1F)) | (((val >> 5) & 0x3F) << 10) | 0xFF000000;
}

void CN_SetEnableSky(int bEnable)
{
  stRealSky = bEnable;
}

void R3RestoreAllTextures()
{
  int v0 = 1;
  if (dword_140978964 > 1)
  {
    char *v1 = reinterpret_cast<char *>(qword_184A79D70);
    __int64 v2 = 144;
    do
    {
      if (_bittest(reinterpret_cast<const long *>(&v1[v2 + 136]), 0xF))
      {
        if (*reinterpret_cast<unsigned int *>(&v1[v2 + 132]) != 1)
        {
          Error(aAo_1, byte_140883769);
          v1 = reinterpret_cast<char *>(qword_184A79D70);
        }
        unsigned int id = *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(qword_184A79D70) + v2 + 128);
        if (!reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id])
        {
          IDirect3DTexture8 *dds =
            _bittest(reinterpret_cast<const long *>(qword_184A79D68) + 4 * id + 2, 0x1F)
              ? R3LoadDDS(&v1[144 * v0], 0, 0x800, 0x800)
              : R3LoadDDS(&v1[144 * v0], dword_184A797D0, 0x800, 0x800);
          v1 = reinterpret_cast<char *>(qword_184A79D70);
          reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * id] = reinterpret_cast<unsigned long long>(dds);
        }
      }
      ++v0;
      v2 += 144;
    } while (v0 < dword_140978964);
  }
  RestoreSystemTexture();
}

void RTMovieCreate(char *szFileName, void *pLevel)
{
  (void)szFileName;
  (void)pLevel;
}

void RTMovieSetState(unsigned int nState)
{
  (void)nState;
}

void R3EnvironmentShakeOff()
{
  stShakeState = 0;
}

void ClearDynamicLight()
{
  stDLightNum = 0;
}
void Error(char *source, char *msg)
{
  const char *v3 = source;
  char Text[256]{};

  char *dst = Text;
  while ((*dst++ = *source++) != '\0')
    ;

  char *v6 = &Text[std::strlen(Text) + 1];
  size_t idx = 0;
  while ((v6[idx++] = msg[idx - 1]) != '\0')
    ;

  FILE *v9 = nullptr;
  if (fopen_s(&v9, "error_message.txt", "wt") == 0 && v9)
  {
    fprintf(v9, "%s%s", v3, msg);
    fclose(v9);
  }

  void (*v10)(char *) = ErrorMessageProc;
  if (!v10)
  {
    MessageBoxA(nullptr, Text, "message", MB_ICONERROR);
    exit(1);
  }

  static const unsigned char asc_140884708[4] = { 0x3C, 0x2D, 0xBF, 0xA1 }; // "<-에"
  char *v11 = &Text[std::strlen(Text) + 1];
  memcpy_0(v11 - 1, asc_140884708, sizeof(asc_140884708));
  *reinterpret_cast<unsigned short *>(v11 + 3) = static_cast<unsigned short>(-20553);
  v11[5] = 0;
  v10(Text);
}

void Warning(char *source, char *msg)
{
  if (No_warning)
    return;

  char Text[256]{};
  char *dst = Text;
  while ((*dst++ = *source++) != '\0')
    ;
  char *v5 = &Text[std::strlen(Text) + 1];
  size_t idx = 0;
  while ((v5[idx++] = msg[idx - 1]) != '\0')
    ;

  void (*v8)(char *) = WarningMessageProc;
  if (v8)
  {
    static const char kSuffix[] = "\x3C\x2D\xBF\xA1\xB7\xAF"; // "<-에러"
    strcat_s(Text, kSuffix);
    v8(Text);
  }
  else
  {
    MessageBoxA(nullptr, Text, "message", 0);
  }
}

void ResetTotalVertexBufferInfo()
{
  dword_184A7B2D8 = 0;
  dword_184A7B2DC = 0;
}

unsigned int GetTotalVertexBufferSize()
{
  return dword_184A7B2D8;
}

unsigned int GetTotalVertexBufferCnt()
{
  return dword_184A7B2DC;
}

void *Dmalloc(int size)
{
  if (!size)
    return nullptr;

  dword_184A78FD4 += static_cast<unsigned int>(size);
  ++dword_184A78FD8;
  auto *v3 = static_cast<unsigned int *>(malloc(static_cast<size_t>(size) + 4));
  if (!v3)
    return nullptr;
  *v3 = static_cast<unsigned int>(size);
  return v3 + 1;
}

void Dfree(void *ptr)
{
  if (!ptr)
    return;

  auto *base = static_cast<unsigned int *>(ptr) - 1;
  unsigned int v1 = *base;
  --dword_184A78FD8;
  dword_184A78FD4 -= v1;
  free(base);
}

void *ReAlloc(void *ptr, unsigned int oldSize, unsigned int newSize)
{
  unsigned int copySize = oldSize;
  if (newSize < copySize)
    copySize = newSize;
  void *dst = Dmalloc(static_cast<int>(newSize));
  if (dst && ptr && copySize)
    memcpy_0(dst, ptr, copySize);
  Dfree(ptr);
  return dst;
}

unsigned int GetDmallocSize()
{
  return dword_184A78FD4;
}

unsigned int GetDmallocCnt()
{
  return dword_184A78FD8;
}

bool IsParticle(char *a1)
{
  _strlwr(a1);
  const size_t len = std::strlen(a1);
  return len >= 3 && *reinterpret_cast<unsigned short *>(&a1[len - 2]) == 29808 && a1[len - 3] == 's';
}

CMergeFileManager *GetMergeFileManager()
{
  return qword_184A7B208;
}

FILE *fopenMFM(char *a1, const char *Mode)
{
  const char *v2 = Mode;
  char *v3 = a1;
  if (qword_184A7B208)
  {
    CMergeFileManager *mgr = GetMergeFileManager();
    FILE *result = mgr ? mgr->LoadFileOffset(v3, v2) : nullptr;
    if (result)
      return result;
    Mode = v2;
    a1 = v3;
  }
  return fopen(a1, Mode);
}

unsigned int GetFileSize(char *a1)
{
  FILE *v2 = fopen(a1, "rb");
  if (v2)
  {
    long v3 = ftell(v2);
    fseek(v2, 0, SEEK_END);
    long v4 = ftell(v2);
    fclose(v2);
    return static_cast<unsigned int>(v4 - v3);
  }
  return 0;
}

__int64 GetTokenFloat(char *a1, float *a2)
{
  int v4 = 0;
  int v5 = 0;
  __int64 v6 = 0;
  int v7 = static_cast<int>(std::strlen(a1));
  __int64 v8 = 0;
  char String[256]{};

  if (v7 <= 0)
  {
    *a2 = 0.0f;
    return static_cast<unsigned int>(v7);
  }

  char v9 = 0;
  while (true)
  {
    v9 = a1[v6];
    if (v9 != '(')
      break;
    v5 = 0;
    v8 = 0;
LABEL_7:
    ++v6;
    ++v4;
    if (v6 >= v7)
    {
      *a2 = 0.0f;
      return static_cast<unsigned int>(v7);
    }
  }
  if (v9 != ',' && v9 != ')')
  {
    String[v8] = v9;
    ++v5;
    ++v8;
    goto LABEL_7;
  }
  String[v5] = 0;
  *a2 = static_cast<float>(std::atof(String));
  return static_cast<unsigned int>(v4 + 1);
}

__int64 GetRandOrNum(FILE *Stream, float *a2, float *a3)
{
  char String[256]{};
  char v17[256]{};

  fscanf(Stream, "%s", String);
  if (String[0] == 'r' && *reinterpret_cast<unsigned short *>(&String[1]) == 0x6E61 && String[3] == 'd')
  {
    int v6 = static_cast<int>(std::strlen(String));
    int v7 = 0;
    while (true)
    {
      if (v6 > 0)
      {
        for (__int64 v8 = 0; v8 < v6; ++v8)
        {
          if (String[v8] == ')')
            v7 = 1;
        }
        if (v7)
          break;
      }
      if (fscanf(Stream, "%s", v17) == -1 || !std::strcmp(String, "end"))
        Error(byte_140884F48, asc_140884F60);
      char *v9 = &String[std::strlen(String) + 1];
      __int64 v10 = 0;
      char v11 = 0;
      do
      {
        v11 = v17[v10];
        v9[v10++ - 1] = v11;
      } while (v11);
      v6 = static_cast<int>(std::strlen(String));
    }
    int TokenFloat = static_cast<int>(GetTokenFloat(String, a2));
    GetTokenFloat(&String[TokenFloat], a3);
    return 1;
  }
  float v15 = static_cast<float>(std::atof(String));
  *a2 = v15;
  *a3 = v15;
  return 0;
}

__int64 IM_LoadWave(char *a1, unsigned int a2)
{
  (void)a1;
  (void)a2;
  return 1;
}

void IM_StopWave(unsigned int a1)
{
  (void)a1;
}

void IM_ReleaseWave(unsigned int a1)
{
  (void)a1;
}

void IM_ReleaseAllWaves()
{
}

static HRESULT __fastcall Texture_QueryInterface(IDA_IUnknown *self, const IDA_GUID *, void **outObject)
{
  (void)self;
  if (outObject)
    *outObject = nullptr;
  return 0;
}

static unsigned int __fastcall Texture_AddRef(IDA_IUnknown *self)
{
  (void)self;
  return 1;
}

static unsigned int __fastcall Texture_Release(IDA_IUnknown *self)
{
  auto *tex = reinterpret_cast<IDirect3DTexture8 *>(self);
  if (tex->m_Data)
    std::free(tex->m_Data);
  std::free(tex);
  return 0;
}

static IDA_IUnknown_vtbl g_TextureVtable = {Texture_QueryInterface, Texture_AddRef, Texture_Release};

static __int64 D3DXCreateTextureFromFileInMemory_0(IDirect3DDevice8 *device, const void *data, unsigned int size, void *outTex)
{
  (void)device;
  if (!outTex)
    return -1;
  auto **outPtr = reinterpret_cast<IDirect3DTexture8 **>(outTex);
  auto *tex = static_cast<IDirect3DTexture8 *>(std::malloc(sizeof(IDirect3DTexture8)));
  if (!tex)
  {
    *outPtr = nullptr;
    return -1;
  }
  std::memset(tex, 0, sizeof(*tex));
  tex->__vftable = &g_TextureVtable;
  if (size && data)
  {
    tex->m_Data = static_cast<unsigned char *>(std::malloc(size));
    if (!tex->m_Data)
    {
      std::free(tex);
      *outPtr = nullptr;
      return -1;
    }
    memcpy_0(tex->m_Data, data, size);
    tex->m_Size = size;
  }
  *outPtr = tex;
  return 0;
}

static __int64 D3DXCreateTextureFromFileExA_0(IDirect3DDevice8 *device, const char *path, __int64 flags)
{
  (void)device;
  (void)path;
  (void)flags;
  return 0;
}

static __int64 sub_1404FFFB0(__int64 a1)
{
  int *v1 = reinterpret_cast<int *>(&pass_word);
  char *v2 = reinterpret_cast<char *>(&unk_1409788D4) - a1;
  __int64 v3 = a1 + 4;
  char *v4 = reinterpret_cast<char *>(&pass_word) - a1;
  char *v5 = reinterpret_cast<char *>(&unk_1409788D8) - a1;
  __int64 v6 = 8;
  int v7 = 0;
  __int64 result = 0;
  do
  {
    v7 = *v1;
    v3 += 16;
    v1 += 4;
    *reinterpret_cast<unsigned int *>(v3 - 20) ^= v7;
    *reinterpret_cast<unsigned int *>(v3 - 16) ^= *reinterpret_cast<unsigned int *>(&v4[v3 - 16]);
    *reinterpret_cast<unsigned int *>(v3 - 12) ^= *reinterpret_cast<unsigned int *>(&v2[v3 - 16]);
    result = *reinterpret_cast<unsigned int *>(&v5[v3 - 16]);
    *reinterpret_cast<unsigned int *>(v3 - 8) ^= static_cast<unsigned int>(result);
    --v6;
  } while (v6);
  return result;
}

static __int64 sub_1404FFF30(unsigned int *a1)
{
  int v1 = a1[23];
  __int64 result = 1;
  if (v1 == 5)
  {
    a1[23] = 4;
    goto LABEL_10;
  }
  if (v1 == 65)
  {
    int v3 = a1[25];
    if (v3 == 16)
    {
      if (a1[29] == 0x8000)
        goto LABEL_10;
      a1[26] = 3840;
    }
    else
    {
      if (v3 != 32)
        goto LABEL_10;
      a1[26] = 16711680;
      a1[27] = 65280;
      a1[28] = 255;
      a1[29] = 0xFF000000;
    }
    result = 0;
  }
LABEL_10:
  if (!a1[22])
  {
    a1[22] = 32;
    a1[23] = 4;
    return 0;
  }
  return result;
}

static unsigned __int16 sub_1405005A0(unsigned __int16 a1)
{
  if (a1 > 4)
    a1 >>= 1;
  return a1;
}

static int sub_140501380(char *a1, const char *a2, const char *a3)
{
  int v6 = 1;
  char Buffer[256]{};
  while (true)
  {
    strcpy_s(Buffer, a1);
    sprintf(Buffer, "%s%s%04d%s", a1, a2, v6, a3);
    FILE *fp = fopen(Buffer, "rb");
    if (!fp)
      break;
    fclose(fp);
    if (++v6 >= 65)
      return 65;
  }
  return v6 - 1;
}

static void sub_140502610(char *a1)
{
  if (a1)
  {
    memset_0(a1, 0, 0x1D6u);
    auto *v2 = reinterpret_cast<int *>(a1 + 154);
    __int64 v3 = 7;
    do
    {
      *(v2 - 1) = 0;
      *v2 = 0;
      v2[1] = -1;
      v2[2] = 0;
      v2 = reinterpret_cast<int *>(reinterpret_cast<char *>(v2) + 46);
      --v3;
    } while (v3);
  }
}

static int sub_140502690(FILE *Stream, __int64 a2, int a3)
{
  __int64 v3 = a3;
  __int64 v6 = 46LL * a3;
  *reinterpret_cast<unsigned int *>(v6 + a2 + 158) = static_cast<unsigned int>(-1);

  char String[256]{};
  char v55[256]{};
  int v53[4]{};

  int result = fscanf(Stream, "%s", String);
  while (strcmp(String, "}"))
  {
    if (!strcmp(String, "map_name") && !dword_184A79DB4)
    {
      fscanf(Stream, "%s", String);
      strcpy_s(v55, aTexture);
      strcat_s(v55, String);
      if (!String[0] || !strcmp(String, "NULL"))
      {
        *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 150) = 0;
      }
      else if (IsAniTex(String))
      {
        *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 162) |= 0x1000u;
        R3GetPreAniTextureId(aTexture, String, reinterpret_cast<int *>(v6 + a2 + 150), v53);
        *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 148) = static_cast<unsigned short>(v53[0]);
      }
      else
      {
        *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 150) = R3GetPreTextureId(v55);
      }
    }
    if (!strcmp(String, "uv_scroll_u"))
    {
      fscanf(Stream, "%s", String);
      __int64 v13 = 46 * v3;
      float v14 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned int *>(v13 + a2 + 162) |= 0x10u;
      *reinterpret_cast<unsigned short *>(v13 + a2 + 170) = static_cast<unsigned short>(v14 * 256.0f);
    }
    if (!strcmp(String, "uv_scroll_v"))
    {
      fscanf(Stream, "%s", String);
      __int64 v16 = 46 * v3;
      float v17 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned int *>(v16 + a2 + 162) |= 0x20u;
      *reinterpret_cast<unsigned short *>(v16 + a2 + 172) = static_cast<unsigned short>(v17 * 256.0f);
    }
    if (!strcmp(String, "uv_scale"))
    {
      fscanf(Stream, "%s", String);
      float v19 = static_cast<float>(atof(String));
      __int64 v20 = 46 * v3;
      *reinterpret_cast<unsigned int *>(v20 + a2 + 162) |= 0x80u;
      *reinterpret_cast<unsigned short *>(v20 + a2 + 180) = 256;
      unsigned short v22 = static_cast<unsigned short>(v19 * 256.0f);
      *reinterpret_cast<unsigned short *>(v20 + a2 + 176) = v22;
      *reinterpret_cast<unsigned short *>(v20 + a2 + 178) = v22;
    }
    if (!strcmp(String, "uv_scale_speed"))
    {
      fscanf(Stream, "%s", String);
      float v23 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 180) = static_cast<unsigned short>(v23 * 256.0f);
    }
    if (!strcmp(String, "uv_scale_end"))
    {
      fscanf(Stream, "%s", String);
      float v24 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 178) = static_cast<unsigned short>(v24 * 256.0f);
    }
    if (!strcmp(String, "uv_rotate"))
    {
      fscanf(Stream, "%s", String);
      __int64 v25 = 46 * v3;
      float v26 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned int *>(v25 + a2 + 162) |= 0x40u;
      *reinterpret_cast<unsigned short *>(v25 + a2 + 174) = static_cast<unsigned short>(v26 * 256.0f);
    }
    if (!strcmp(String, "uv_lava"))
    {
      fscanf(Stream, "%s", String);
      __int64 v28 = 46 * v3;
      if (!*reinterpret_cast<unsigned short *>(46 * v3 + a2 + 166))
        *reinterpret_cast<unsigned short *>(v28 + a2 + 166) = 256;
      float v29 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned int *>(v28 + a2 + 162) |= 4u;
      *reinterpret_cast<unsigned short *>(v28 + a2 + 168) = static_cast<unsigned short>(v29 * 256.0f);
    }
    if (!strcmp(String, "uv_lava_wave"))
    {
      fscanf(Stream, "%s", String);
      float v31 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 166) = static_cast<unsigned short>(v31 * 256.0f);
    }
    if (!strcmp(String, "uv_gradient_alpha_u"))
    {
      fscanf(Stream, "%s", String);
      __int64 v32 = 46 * v3;
      float v33 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(v32 + a2 + 192) |= static_cast<unsigned short>(static_cast<int>((v33 * 25.0f) + 100.0f)) << 8;
      *reinterpret_cast<unsigned int *>(v32 + a2 + 162) |= 0x100u;
    }
    if (!strcmp(String, "uv_gradient_alpha_v"))
    {
      fscanf(Stream, "%s", String);
      __int64 v34 = 46 * v3;
      float v35 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(v34 + a2 + 192) |= static_cast<unsigned short>(static_cast<int>((v35 * 25.0f) + 100.0f));
      *reinterpret_cast<unsigned int *>(v34 + a2 + 162) |= 0x200u;
    }
    if (!strcmp(String, "uv_metal_floor"))
    {
      fscanf(Stream, "%s", String);
      __int64 v36 = 46 * v3;
      float v37 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned int *>(v36 + a2 + 162) |= 2u;
      *reinterpret_cast<unsigned short *>(v36 + a2 + 182) = static_cast<unsigned short>(v37 * 256.0f);
    }
    if (!strcmp(String, "uv_metal_wall"))
    {
      fscanf(Stream, "%s", String);
      __int64 v39 = 46 * v3;
      float v40 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned int *>(v39 + a2 + 162) |= 8u;
      *reinterpret_cast<unsigned short *>(v39 + a2 + 182) = static_cast<unsigned short>(v40 * 256.0f);
    }
    if (!strcmp(String, "ani_alpha_flicker"))
    {
      fscanf(Stream, "%s", String);
      float v42 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * (v3 + 4) + a2) = static_cast<unsigned short>(v42 * 256.0f);
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 162) |= 0x400u;
    }
    if (!strcmp(String, "ani_alpha_flicker_start"))
    {
      fscanf(Stream, "%s", String);
      float v43 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 186) |= static_cast<unsigned short>(static_cast<int>(v43 * 255.0f)) << 8;
    }
    if (!strcmp(String, "ani_alpha_flicker_end"))
    {
      fscanf(Stream, "%s", String);
      float v44 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 186) |= static_cast<unsigned short>(static_cast<int>(v44 * 255.0f));
    }
    if (!strcmp(String, "ani_tex_speed"))
    {
      fscanf(Stream, "%s", String);
      float v45 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 190) = static_cast<unsigned short>(v45 * 256.0f);
    }
    if (!strcmp(String, "ani_tex_frame"))
    {
      fscanf(Stream, "%s", String);
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 162) |= 0x800u;
      float v46 = static_cast<float>(atof(String));
      *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 188) = static_cast<unsigned short>(v46 * 256.0f);
      if (!*reinterpret_cast<unsigned short *>(46 * v3 + a2 + 190))
        *reinterpret_cast<unsigned short *>(46 * v3 + a2 + 190) = 256;
    }
    if (!strcmp(String, "uv_env"))
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 162) |= 1u;
    if (!strcmp(String, "alpha_sort"))
    {
      fscanf(Stream, "%s", String);
      if (!strcmp(_strupr(String), "TRUE"))
      {
        if (a3)
          Warning(aAlphaSortTrue, byte_140883769);
        else
          *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 154) |= 0x80000000;
      }
    }
    if (!strcmp(String, "tex_clamp"))
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 162) |= 0x10000u;
    if (!strcmp(String, "zwrite"))
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 154) |= 0x2000u;
    if (!strcmp(String, "bump_env"))
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 162) |= 0x8000u;
    if (!strcmp(String, "type"))
    {
      fscanf(Stream, "%s", String);
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 154) = atoi(String);
    }
    if (!strcmp(String, "alpha"))
    {
      fscanf(Stream, "%s", String);
      *reinterpret_cast<unsigned char *>(46 * v3 + a2 + 161) = 0;
      *reinterpret_cast<unsigned int *>(46 * v3 + a2 + 158) |= static_cast<unsigned int>(atoi(String)) << 24;
    }
    if (!strcmp(String, "color"))
    {
      fscanf(Stream, "%s", String);
      int r = atoi(String);
      fscanf(Stream, "%s", String);
      int g = atoi(String);
      fscanf(Stream, "%s", String);
      int b = atoi(String);
      __int64 v52 = 46 * v3;
      *reinterpret_cast<unsigned int *>(v52 + a2 + 158) &= 0xFF000000;
      *reinterpret_cast<unsigned int *>(v52 + a2 + 158) |= b | ((g | (r << 8)) << 8);
    }
    result = fscanf(Stream, "%s", String);
  }
  if (!*reinterpret_cast<unsigned short *>(v6 + a2 + 186))
    *reinterpret_cast<unsigned short *>(v6 + a2 + 186) = 255;
  return result;
}

static void sub_140503190(char *a1, unsigned int *a2)
{
  char v4 = 0;
  FILE *fp = fopenMFM(a1, "rt");
  if (fp)
  {
    if (!dword_184A79DB4)
    {
      unsigned int *v6 = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(a2) + 150);
      for (int i = 0; i < 7; ++i)
      {
        *v6 = static_cast<unsigned int>(-1);
        v6 = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(v6) + 46);
      }
      a2[34] = static_cast<unsigned int>(-1);
      a2[33] = 0;
    }

    unsigned int v8 = dword_184A797D0;
    unsigned int v16 = dword_184A797D0;
    char String[256]{};
    if (fscanf(fp, "%s", String) != -1)
    {
      do
      {
        if (!strcmp(String, "end"))
          break;
        if (!strcmp(String, "}"))
        {
          if (!v4)
            break;
          v4 = 0;
        }
        if (!strcmp(String, "light_map"))
        {
          fscanf(fp, "%s", String);
          if (!strcmp(_strupr(String), "TRUE"))
            a2[33] |= 1u;
        }
        if (!strcmp(String, "sky_sun_light"))
          a2[33] |= 4u;
        if (!strcmp(String, "sky_night"))
          a2[33] |= 8u;
        if (!strcmp(String, "sky_day"))
          a2[33] |= 0x10u;
        if (!strcmp(String, "texture_splatting"))
          a2[33] |= 0x20u;
        if (!strcmp(String, "layer_num"))
        {
          fscanf(fp, "%s", String);
          a2[36] = atoi(String);
        }
        if (!strcmp(String, "no_lod_texture"))
        {
          a2[33] |= 2u;
          dword_184A797D0 = 0;
        }
        if (!strcmp(String, "detail_texture"))
        {
          fscanf(fp, "%s", String);
          strcpy_s(a1, 256, aTexture);
          strcat_s(a1, 256, String);
          a2[33] |= 2u;
          dword_184A797D0 = 0;
          if (!String[0] || !strcmp(String, "NULL"))
            a2[34] = 0;
          else
            a2[34] = R3GetPreTextureId(a1);
          a2[35] = 1065353216;
        }
        if (!strcmp(String, "no_collision"))
          *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(a2) + 162) |= 0x20000u;
        if (!strcmp(String, "water"))
          *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(a2) + 162) |= 0x60000u;
        if (!strcmp(String, "detail_texture_scale"))
        {
          fscanf(fp, "%s", String);
          float v14 = static_cast<float>(atof(String));
          reinterpret_cast<float *>(a2)[35] = v14;
        }
        if (!strcmp(String, "layer"))
        {
          fscanf(fp, "%s", String);
          int v15 = atoi(String);
          sub_140502690(fp, reinterpret_cast<__int64>(a2), v15);
          v4 = 1;
        }
      } while (fscanf(fp, "%s", String) != -1);
      v8 = v16;
    }
    dword_184A797D0 = v8;
    fclose(fp);
  }
  else
  {
    Warning(a1, aAiAaiai_3);
  }
}

static void LoadR3X(char *a1)
{
  FILE *fp = fopenMFM(a1, "rb");
  if (!fp)
    return;

  float version = 0.0f;
  fread(&version, 4, 1, fp);
  if (version != 1.1f)
  {
    Warning(a1, aAiAaiai_0);
    fclose(fp);
    return;
  }

  unsigned char raw[0x408]{};
  fread(raw, sizeof(raw), 1, fp);
  fclose(fp);

  int v17 = *reinterpret_cast<int *>(raw);
  int v18 = *reinterpret_cast<int *>(raw + 4);
  __int64 v19 = *reinterpret_cast<__int64 *>(raw + 8);
  int v20 = *reinterpret_cast<int *>(raw + 16);
  int v21 = *reinterpret_cast<int *>(raw + 20);
  unsigned int *v22 = reinterpret_cast<unsigned int *>(raw + 24);

  if ((v17 & 4) != 0)
  {
    dword_184A7983C = static_cast<unsigned int>(v19 >> 32);
    dword_184A79814 = v18;
    qword_184A79818 = static_cast<unsigned long long>(v19);
    dword_184A79778 = static_cast<unsigned int>(v19);
    dword_184A79808 = 1;
    dword_184A79844 = v18;
    qword_184A79848 = static_cast<unsigned long long>(v19);
    dword_184A7982C = v18;
    dword_184A79834 = static_cast<unsigned int>(v19);
  }

  dword_184A79824 = v22[125];
  if ((v17 & 8) != 0)
  {
    dword_184A79840 = v22[0];
    dword_184A79828 = 1;
    dword_184A79830 = v20;
    dword_184A79838 = v21;
  }

  if ((v17 & 0x10) != 0)
  {
    char *state = reinterpret_cast<char *>(&stState);
    size_t idx = 0;
    do
    {
      char v7 = reinterpret_cast<char *>(&v22[23])[idx];
      state[idx + 2144] = v7;
      byte_184A79850[idx] = v7;
      ++idx;
      if (!v7)
        break;
    } while (true);

    dword_184A798D0 = R3GetPreTextureId(byte_184A79850);
    *reinterpret_cast<float *>(&dword_184A79914) = *reinterpret_cast<float *>(&v22[55]);
    dword_184A79918 = v22[56];
    dword_184A7991C = v22[57];
    __int64 v9 = 0;
    do
    {
      int v10 = v22[v9 + 7];
      v9 += 8;
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2244]) = v10;
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2248]) = v22[v9];
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2252]) = v22[v9 + 1];
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2256]) = v22[v9 + 2];
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2260]) = v22[v9 + 3];
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2264]) = v22[v9 + 4];
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2268]) = v22[v9 + 5];
      *reinterpret_cast<unsigned int *>(&state[v9 * 4 + 2272]) = v22[v9 + 6];
    } while (v9 < 16);
  }

  if ((v17 & 0x20) != 0)
  {
    dword_184A79A24 = v22[90];
    char *state = reinterpret_cast<char *>(&stState);
    size_t idx = 0;
    do
    {
      char v12 = reinterpret_cast<char *>(&v22[58])[idx];
      state[idx + 2356] = v12;
      ++idx;
      if (!v12)
        break;
    } while (true);
  }

  if ((v17 & 0x20) != 0)
  {
    dword_184A79A28 = v22[124];
    char *state = reinterpret_cast<char *>(&stState);
    size_t idx = 0;
    do
    {
      char v13 = reinterpret_cast<char *>(&v22[92])[idx];
      state[idx + 2484] = v13;
      ++idx;
      if (!v13)
        break;
    } while (true);
  }

  if ((v17 & 1) != 0)
    dword_184A7980C = 1;
  if ((v17 & 2) != 0)
    dword_184A79810 = 1;
}

static _LIGHTMAP **LoadR3TLightMap(struct R3Texture *a1, D3DFORMAT a2)
{
  (void)a2;
  unsigned int v2 = 0;
  unsigned int v19 = 0;
  dword_184A79C54 = 0;
  if (!a1)
    return nullptr;

  FILE *fp = fopen(reinterpret_cast<const char *>(a1), "rb");
  if (!fp)
    return nullptr;

  float version = 0.0f;
  fread(&version, 4, 1, fp);
  fread(&v19, 4, 1, fp);
  if (version != 1.1f && version != 1.2f)
  {
    Warning(reinterpret_cast<char *>(a1), aAiAaiaiAiau);
    fclose(fp);
    return nullptr;
  }

  auto **list = reinterpret_cast<unsigned short **>(Dmalloc(8 * v19));
  if (!list)
  {
    fclose(fp);
    return nullptr;
  }

  if (v19)
  {
    auto **cur = list;
    while (true)
    {
      size_t elementSize = 0;
      fread(&elementSize, 4, 1, fp);
      auto *buf = static_cast<unsigned int *>(Dmalloc(static_cast<int>(elementSize)));
      fread(buf, static_cast<unsigned int>(elementSize), 1, fp);
      if (*buf != 542327876)
      {
        Error(reinterpret_cast<char *>(a1), aAiR3tAai);
        Dfree(buf);
        fclose(fp);
        return nullptr;
      }
      unsigned short v10 = reinterpret_cast<unsigned short *>(buf)[10];
      unsigned short v11 = reinterpret_cast<unsigned short *>(buf)[8];
      if (buf[24])
      {
        Error(reinterpret_cast<char *>(a1), aAi);
        Dfree(buf);
        fclose(fp);
        return nullptr;
      }
      int v12 = buf[25];
      int v13 = 0;
      if (v12 == 16)
      {
        v13 = 23;
      }
      else
      {
        if (v12 != 32)
        {
          Error(reinterpret_cast<char *>(a1), aAi);
          Dfree(buf);
          fclose(fp);
          return nullptr;
        }
        v13 = 21;
      }
      if (v10 > 0x800u || v11 > 0x800u)
        Error(byte_140883FF8, byte_140883769);
      dword_184A79C54 += v10 * v11 * (buf[25] >> 3);
      int v14 = sub_1405005A0(v10);
      int v15 = sub_1405005A0(v11);
      auto *lm = static_cast<unsigned short *>(Dmalloc(16));
      *cur = lm;
      lm[0] = static_cast<unsigned short>(v14);
      lm[1] = static_cast<unsigned short>(v15);
      *reinterpret_cast<unsigned short **>(reinterpret_cast<char *>(lm) + 8) =
        static_cast<unsigned short *>(Dmalloc(2 * v14 * v15));
      dword_184A79D60 += static_cast<unsigned int>(elementSize);
      if (v13 == 21)
        MakeMipMap(lm[0], lm[1], *reinterpret_cast<unsigned short **>(reinterpret_cast<char *>(lm) + 8),
                   reinterpret_cast<unsigned char *>(buf) + 144);
      else
        MakeMipMap(lm[0], lm[1], *reinterpret_cast<unsigned short **>(reinterpret_cast<char *>(lm) + 8),
                   reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(buf) + 72));

      reinterpret_cast<unsigned long long *>(qword_184A79D68)[2 * v2 + 2 * reinterpret_cast<unsigned int *>(a1)[32]] =
        reinterpret_cast<unsigned long long>(GetD3DTextureFromBuffer(reinterpret_cast<unsigned char *>(buf), elementSize));
      Dfree(buf);
      ++v2;
      ++cur;
      if (v2 >= v19)
        break;
    }
  }

  fclose(fp);
  return reinterpret_cast<_LIGHTMAP **>(list);
}

static void RestoreSystemTexture()
{
  unsigned long long v0 = qword_184A79C20;
  if (qword_184A79C20)
  {
    auto *tex = reinterpret_cast<IDirect3DTexture8 *>(qword_184A79C20);
    if (tex && tex->__vftable && tex->__vftable->Release)
      tex->__vftable->Release(reinterpret_cast<IDA_IUnknown *>(tex));
    v0 = qword_184A79C20;
  }
  if (qword_184A79C18)
  {
    char logoPath[] = ".\\system\\logo.dds";
    qword_184A79C18 = reinterpret_cast<unsigned long long>(R3LoadDDS(logoPath, 2, 0x800, 0x800));
    v0 = qword_184A79C20;
  }
  if (v0)
  {
    char dlightPath[] = ".\\system\\dlight.dds";
    qword_184A79C20 = reinterpret_cast<unsigned long long>(R3LoadDDS(dlightPath, 2, 0x800, 0x800));
  }
}

static IDirect3DTexture8 *GetD3DTextureFromBuffer(unsigned __int8 *a1, unsigned int a2)
{
  unsigned long long outTex = 0;
  IDirect3DDevice8 *dev = GetD3dDevice();
  D3DXCreateTextureFromFileInMemory_0(dev, a1, a2, &outTex);
  return reinterpret_cast<IDirect3DTexture8 *>(outTex);
}

static int GetMipMapSkipSize(_DDSURFACEDESC2 *a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
  unsigned int v4 = 0;
  if (a3 < 0x100 || a4 < 0x100)
    Error(aAi_1, byte_140883769);
  unsigned int i = 0;
  for (i = 0; i < a2 || a1->dwWidth > a3 || a1->dwHeight > a4; ++i)
  {
    if (i >= a1->dwMipMapCount)
      break;
    int lPitch = a1->lPitch;
    unsigned int dwHeight = a1->dwHeight;
    v4 += lPitch;
    if (dwHeight <= 4)
      break;
    a1->dwHeight = dwHeight >> 1;
    unsigned int dwWidth = a1->dwWidth;
    if (dwWidth <= 4)
      break;
    a1->dwWidth = dwWidth >> 1;
    a1->lPitch = lPitch / 4;
  }
  a1->dwMipMapCount -= i;
  return static_cast<int>(v4);
}

static void MakeMipMap(unsigned short a1, unsigned short a2, unsigned short *a3, unsigned short *a4)
{
  __int64 v4 = 0;
  unsigned short *v5 = a3;
  unsigned short v6 = a1;
  int v8 = 2 * a1;
  int v9 = a1;
  int v36 = a1;
  __int64 v32 = 0;
  if (a2)
  {
    __int64 v10 = a2;
    unsigned short *v11 = &a4[v8];
    __int64 v12 = 8LL * a1;
    __int64 v34 = v12;
    __int64 v33 = a2;
    unsigned short *v13 = &a4[v8 + 1];
    do
    {
      if (v9 > 0)
      {
        unsigned short *v14 = v11;
        unsigned short *v15 = a4;
        unsigned short *v16 = a4 + 1;
        __int64 v17 = v4 * v6;
        __int64 v18 = static_cast<unsigned int>(v9);
        unsigned short *v19 = &v5[v17];
        unsigned short *v20 = v13;
        do
        {
          unsigned int v21 = *v15;
          int v22 = (*v16 >> 11) + (v21 >> 11);
          unsigned int v23 = *v16;
          int v24 = v23 & 0x1F;
          int v25 = ((v23 >> 5) & 0x3F) + ((v21 >> 5) & 0x3F);
          ++v19;
          v15 += 2;
          v16 += 2;
          int v26 = (*v14 >> 11) + v22;
          unsigned int v27 = *v14;
          int v28 = (v27 & 0x1F) + v24 + (v21 & 0x1F);
          v14 += 2;
          unsigned int v29 = (((v27 >> 5) & 0x3F) + v25 + ((*v20 >> 5) & 0x3F)) >> 2;
          unsigned int v30 = (unsigned int)(v26 + (*v20 >> 11)) >> 2;
          unsigned int v31 = (v28 + (*v20 & 0x1Fu)) >> 2;
          v20 += 2;
          --v18;
          *(v19 - 1) = static_cast<unsigned short>(v31 | (32 * ((v30 << 6) | v29)));
        } while (v18);
        v12 = v34;
        v4 = v32;
        v9 = v36;
        v10 = v33;
        v6 = a1;
        v5 = a3;
      }
      ++v4;
      v11 = reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(v11) + v12);
      v13 = reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(v13) + v12);
      a4 = reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(a4) + v12);
      --v10;
      v32 = v4;
      v33 = v10;
    } while (v10);
  }
}

static void MakeMipMap(unsigned short a1, unsigned short a2, unsigned short *a3, unsigned char *a4)
{
  unsigned short *v4 = a3;
  int v5 = a1;
  __int64 v6 = 0;
  int v7 = 2 * a1;
  int v30 = v5;
  if (a2)
  {
    unsigned char *v9 = a4 + 2;
    unsigned char *v10 = &a4[4 * v7 + 2];
    __int64 v11 = 8LL * v7;
    __int64 v28 = v11;
    __int64 v12 = a2;
    unsigned char *v13 = &a4[4 * v7 + 5];
    __int64 v27 = a2;
    do
    {
      if (v5 > 0)
      {
        unsigned char *v14 = v9;
        unsigned char *v15 = v13;
        unsigned char *v16 = v9 + 4;
        unsigned short *v17 = &v4[v6 * static_cast<unsigned short>(v5)];
        __int64 v8 = static_cast<unsigned int>(v5);
        unsigned char *v18 = v10;
        do
        {
          int v19 = *(v14 - 1);
          int v20 = *v14;
          int v21 = v15[1];
          int v22 = *(v16 - 2) + *(v14 - 2);
          ++v17;
          v14 += 8;
          int v23 = *(v16 - 1) + v19;
          int v24 = *v16;
          v16 += 8;
          int v25 = v24 + v20;
          int v26 = *(v18 - 2);
          v18 += 8;
          v15 += 8;
          --v8;
          *(v17 - 1) = static_cast<unsigned short>(((v26 + v22 + *(v15 - 9)) >> 5) |
                                                  (32 * (((*(v18 - 9) + v23 + *(v15 - 8)) >> 4) |
                                                          (static_cast<unsigned short>((( *(v18 - 8) + v25 + v21) >> 5)) << 6))));
        } while (v8);
        v11 = v28;
        v5 = v30;
        v12 = v27;
        v4 = a3;
      }
      v10 += v11;
      v13 += v11;
      v9 += v11;
      ++v6;
      v27 = --v12;
    } while (v12);
  }
}
