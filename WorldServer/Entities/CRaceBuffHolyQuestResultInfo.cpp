#include "pch.h"

#include "CRaceBuffHolyQuestResultInfo.h"

#include "CMainThread.h"
#include "CLogFile.h"

#include <cstdio>
#include <cstring>

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
