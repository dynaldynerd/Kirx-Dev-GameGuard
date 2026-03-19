#include "pch.h"

#include "CRaceBuffHolyQuestResultInfo.h"

#include "CMainThread.h"
#include "CLogFile.h"

#include <cstdio>
#include <cstring>

CRaceBuffHolyQuestResultInfo::CRaceBuffHolyQuestResultInfo()
  : m_bSetBuff(false),
    m_byCurWinRace(static_cast<unsigned __int8>(-1)),
    m_byCurFailRace(static_cast<unsigned __int8>(-1)),
    m_byCurLoseRace(static_cast<unsigned __int8>(-1)),
    m_byOldWinRace(static_cast<unsigned __int8>(-1)),
    m_byOldFailRace(static_cast<unsigned __int8>(-1)),
    m_byOldLoseRace(static_cast<unsigned __int8>(-1)),
    m_uiContinueWinCnt(0),
    m_uiContinueFailCnt(0),
    m_uiContinueLoseCnt(0)
{
}

CRaceBuffHolyQuestResultInfo::~CRaceBuffHolyQuestResultInfo()
{
  // this is not a stub
}

bool CRaceBuffHolyQuestResultInfo::Load()
{
  LoadINI();
  if (m_byOldWinRace == 255 || m_byOldFailRace == 255 || m_byOldLoseRace == 255
    || (m_byOldWinRace != m_byOldFailRace
      && m_byOldFailRace != m_byOldLoseRace
      && m_byOldWinRace != m_byOldLoseRace))
  {
    return SaveINI();
  }

  g_Main.m_logLoadingError.Write(
    "CRaceBuffHolyQuestResultInfo::Load() : \r\n"
    "\t\tm_byOldWinRace(%u) m_byOldFailRace(%u) m_byOldFailRace(%u) Invalid!\r\n",
    m_byOldWinRace,
    m_byOldFailRace,
    m_byOldLoseRace);
  return false;
}

bool CRaceBuffHolyQuestResultInfo::Save()
{
  SaveINI();
  return true;
}

bool CRaceBuffHolyQuestResultInfo::LoadINI()
{
  char returnedString[1024]{};
  GetPrivateProfileStringA(
    "OldHolyQuestResult",
    "beSetBuff",
    "NULL",
    returnedString,
    static_cast<DWORD>(sizeof(returnedString)),
    "..\\SystemSave\\ServerState.ini");
  if (strcmp(returnedString, "NULL") == 0)
  {
    m_bSetBuff = false;
  }
  else if (_stricmp(returnedString, "true") == 0)
  {
    m_bSetBuff = true;
  }
  else
  {
    m_bSetBuff = false;
  }

  m_byCurWinRace = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("OldHolyQuestResult", "CurWinRace", 255, "..\\SystemSave\\ServerState.ini"));
  m_byCurFailRace = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("OldHolyQuestResult", "CurFailRace", 255, "..\\SystemSave\\ServerState.ini"));
  m_byCurLoseRace = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("OldHolyQuestResult", "CurLoseRace", 255, "..\\SystemSave\\ServerState.ini"));
  m_byOldWinRace = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("OldHolyQuestResult", "OldWinRace", 255, "..\\SystemSave\\ServerState.ini"));
  m_byOldFailRace = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("OldHolyQuestResult", "OldFailRace", 255, "..\\SystemSave\\ServerState.ini"));
  m_byOldLoseRace = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("OldHolyQuestResult", "OldLoseRace", 255, "..\\SystemSave\\ServerState.ini"));
  m_uiContinueWinCnt = static_cast<unsigned int>(
    GetPrivateProfileIntA("OldHolyQuestResult", "ContinueWinCnt", 0, "..\\SystemSave\\ServerState.ini"));
  m_uiContinueFailCnt = static_cast<unsigned int>(
    GetPrivateProfileIntA("OldHolyQuestResult", "ContinueFailCnt", 0, "..\\SystemSave\\ServerState.ini"));
  m_uiContinueLoseCnt = static_cast<unsigned int>(
    GetPrivateProfileIntA("OldHolyQuestResult", "ContinueLoseCnt", 0, "..\\SystemSave\\ServerState.ini"));
  return true;
}

bool CRaceBuffHolyQuestResultInfo::SaveINI()
{
  const char *setBuff = m_bSetBuff ? "true" : "false";
  if (!WritePrivateProfileStringA("OldHolyQuestResult", "beSetBuff", setBuff, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, beSetBuff, %s, %s )",
      "OldHolyQuestResult",
      setBuff,
      "..\\SystemSave\\ServerState.ini");
    return false;
  }

  return SaveINISubProcSaveNum("CurWinRace", m_byCurWinRace)
    && SaveINISubProcSaveNum("CurFailRace", m_byCurFailRace)
    && SaveINISubProcSaveNum("CurLoseRace", m_byCurLoseRace)
    && SaveINISubProcSaveNum("OldWinRace", m_byOldWinRace)
    && SaveINISubProcSaveNum("OldFailRace", m_byOldFailRace)
    && SaveINISubProcSaveNum("OldLoseRace", m_byOldLoseRace)
    && SaveINISubProcSaveNum("ContinueWinCnt", m_uiContinueWinCnt)
    && SaveINISubProcSaveNum("ContinueFailCnt", m_uiContinueFailCnt)
    && SaveINISubProcSaveNum("ContinueLoseCnt", m_uiContinueLoseCnt);
}

bool CRaceBuffHolyQuestResultInfo::SaveINISubProcSaveNum(const char *szItem, unsigned int value)
{
  char buffer[256]{};
  if (_itoa_s(static_cast<int>(value), buffer, sizeof(buffer), 10) != 0)
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : _itoa_s( %s(%u), szBuff, sizeof(szBuff)(%u), 10 ) Fail!",
      szItem,
      value,
      static_cast<unsigned int>(sizeof(buffer)));
    return false;
  }

  if (!WritePrivateProfileStringA("OldHolyQuestResult", szItem, buffer, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, %s, %s, %s )",
      "OldHolyQuestResult",
      szItem,
      buffer,
      "..\\SystemSave\\ServerState.ini");
    return false;
  }
  return true;
}

void CRaceBuffHolyQuestResultInfo::ClearResult()
{
  m_bSetBuff = false;
  m_byOldWinRace = m_byCurWinRace;
  m_byOldFailRace = m_byCurFailRace;
  m_byOldLoseRace = m_byCurLoseRace;
  m_byCurWinRace = static_cast<unsigned __int8>(-1);
  m_byCurFailRace = static_cast<unsigned __int8>(-1);
  m_byCurLoseRace = static_cast<unsigned __int8>(-1);
  SaveINI();
}

void CRaceBuffHolyQuestResultInfo::SetBuffFlag()
{
  m_bSetBuff = true;
  const char *flag = m_bSetBuff ? "true" : "false";
  if (!WritePrivateProfileStringA("OldHolyQuestResult", "beSetBuff", flag, "..\\SystemSave\\ServerState.ini"))
  {
    g_Main.m_logSystemError.Write(
      "CRaceBuffOldHistoryInfo::SaveINI() : WritePrivateProfileString( %s, beSetBuff, %s, %s )",
      "OldHolyQuestResult",
      flag,
      "..\\SystemSave\\ServerState.ini");
  }
}

void CRaceBuffHolyQuestResultInfo::SetResult(
  unsigned __int8 byWinRace,
  unsigned __int8 byLoseRace,
  unsigned int uiThMax)
{
  if (!SetResultSubProcSetRace(byWinRace, byLoseRace, uiThMax))
  {
    m_byCurWinRace = static_cast<unsigned __int8>(-1);
    m_byCurFailRace = static_cast<unsigned __int8>(-1);
    m_byCurLoseRace = static_cast<unsigned __int8>(-1);
  }
  SaveINI();
}

bool CRaceBuffHolyQuestResultInfo::SetResultSubProcSetRace(
  unsigned __int8 byWinRace,
  unsigned __int8 byLoseRace,
  unsigned int uiThMax)
{
  unsigned __int8 failRace = static_cast<unsigned __int8>(-1);
  if (!FindFailRace(byWinRace, byLoseRace, &failRace))
  {
    return false;
  }

  if (byWinRace == m_byOldWinRace)
  {
    if (uiThMax <= ++m_uiContinueWinCnt)
    {
      m_uiContinueWinCnt = uiThMax;
    }
  }
  else
  {
    m_uiContinueWinCnt = 0;
  }

  if (failRace == m_byOldFailRace)
  {
    if (uiThMax <= ++m_uiContinueFailCnt)
    {
      m_uiContinueFailCnt = uiThMax;
    }
  }
  else
  {
    m_uiContinueFailCnt = 0;
  }

  if (byLoseRace == m_byOldLoseRace)
  {
    if (uiThMax <= ++m_uiContinueLoseCnt)
    {
      m_uiContinueLoseCnt = uiThMax;
    }
  }
  else
  {
    m_uiContinueLoseCnt = 0;
  }

  m_byCurWinRace = byWinRace;
  m_byCurFailRace = failRace;
  m_byCurLoseRace = byLoseRace;
  return IsValidResult();
}

bool CRaceBuffHolyQuestResultInfo::FindFailRace(
  unsigned __int8 byWinRace,
  unsigned __int8 byLoseRace,
  unsigned __int8 *pbyFailRace)
{
  if (byWinRace >= 3u || byLoseRace >= 3u || byWinRace == byLoseRace)
  {
    return false;
  }

  unsigned __int8 used[3]{};
  used[byWinRace] = 1;
  used[byLoseRace] = 1;
  *pbyFailRace = static_cast<unsigned __int8>(-1);

  for (unsigned __int8 j = 0; j < 3u; ++j)
  {
    if (!used[j])
    {
      *pbyFailRace = j;
      return *pbyFailRace != static_cast<unsigned __int8>(-1);
    }
  }

  return *pbyFailRace != static_cast<unsigned __int8>(-1);
}

bool CRaceBuffHolyQuestResultInfo::IsValidResult()
{
  return m_byCurWinRace != static_cast<unsigned __int8>(-1)
      && m_byCurFailRace != static_cast<unsigned __int8>(-1)
      && m_byCurLoseRace != static_cast<unsigned __int8>(-1)
      && m_byCurWinRace != m_byCurFailRace
      && m_byCurFailRace != m_byCurLoseRace
      && m_byCurWinRace != m_byCurLoseRace;
}

bool CRaceBuffHolyQuestResultInfo::IsChaos()
{
  return m_byCurWinRace == static_cast<unsigned __int8>(-1)
      || m_byCurFailRace == static_cast<unsigned __int8>(-1)
      || m_byCurLoseRace == static_cast<unsigned __int8>(-1);
}

int CRaceBuffHolyQuestResultInfo::GetResultType(unsigned __int8 byRace, bool bGetScanner)
{
  if (!m_bSetBuff)
  {
    return -1;
  }
  if (m_byCurWinRace == byRace && bGetScanner)
  {
    return 0;
  }
  if (m_byCurWinRace == byRace && !bGetScanner)
  {
    return 1;
  }
  if (m_byCurFailRace == byRace)
  {
    return 2;
  }
  if (m_byCurLoseRace == byRace)
  {
    return 3;
  }
  return -2;
}

unsigned int CRaceBuffHolyQuestResultInfo::GetContinueCnt(unsigned int iType)
{
  if (iType <= 1)
  {
    return m_uiContinueWinCnt;
  }
  if (iType == 2)
  {
    return m_uiContinueFailCnt;
  }
  if (iType == 3)
  {
    return m_uiContinueLoseCnt;
  }
  return 0;
}
