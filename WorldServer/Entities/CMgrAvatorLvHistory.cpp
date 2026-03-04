#include "pch.h"

#include "CMgrAvatorLvHistory.h"

#include <cstdio>
#include <cstring>
#include <process.h>

#include "CPartyPlayer.h"
#include "WorldServerUtil.h"

namespace
{
  char sData_0[10000]{};
  char sBuf_0[512]{};
  constexpr char kHistoryIniSection[] = "System";
  constexpr char kHistoryPathKey[] = "HistoryPath";
  constexpr char kHistoryPathDefault[] = "C:\\History";
  constexpr char kHistoryIniFile[] = "..\\WorldInfo\\WorldInfo.ini";
  constexpr char kHistoryLvSubdirFmt[] = "%s\\Lv";
}

CMgrAvatorLvHistory::CMgrAvatorLvHistory()
{
  m_dwLastLocalDate = 0;
  m_dwLastLocalHour = 0;

  char returnedString[132]{};
  GetPrivateProfileStringA(
    kHistoryIniSection,
    kHistoryPathKey,
    kHistoryPathDefault,
    returnedString,
    0x80u,
    kHistoryIniFile);
  CreateDirectoryA(returnedString, nullptr);

  sprintf(m_szStdPath, kHistoryLvSubdirFmt, returnedString);
  CreateDirectoryA(m_szStdPath, nullptr);

  _strdate(m_szCurDate);
  m_szCurDate[5] = 0;
  _strtime(m_szCurTime);
  m_szCurTime[5] = 0;

  m_tmrUpdateTime.BeginTimer(0xEA60u);

  m_listLogData_2K.SetList(0xFEu);
  m_listLogDataEmpty_2K.SetList(0xFEu);
  for (unsigned int index = 0; index < 254; ++index)
  {
    m_listLogDataEmpty_2K.PushNode_Back(index);
  }

  m_listLogData_1K.SetList(0xFEu);
  m_listLogDataEmpty_1K.SetList(0xFEu);
  for (unsigned int index = 0; index < 254; ++index)
  {
    m_listLogDataEmpty_1K.PushNode_Back(index);
  }

  m_listLogData_200.SetList(0x9E4u);
  m_listLogDataEmpty_200.SetList(0x9E4u);
  for (unsigned int index = 0; index < 2532; ++index)
  {
    m_listLogDataEmpty_200.PushNode_Back(index);
  }

  m_bIOThread = true;
  _beginthread(&CMgrAvatorLvHistory::IOThread, 0, this);
}

CMgrAvatorLvHistory::~CMgrAvatorLvHistory()
{
  m_bIOThread = false;
}

void __cdecl CMgrAvatorLvHistory::IOThread(void *pv)
{
  CMgrAvatorLvHistory *const history = static_cast<CMgrAvatorLvHistory *>(pv);
  unsigned int outIndex[5]{};
  while (history->m_bIOThread)
  {
    history->m_FrameRate.CalcSpeedPerFrame();

    while (history->m_listLogData_2K.PopNode_Front(outIndex))
    {
      __LOG_DATA_2K &entry = history->m_LogData_2K[outIndex[0]];
      IOFileWrite_1(entry.szFileName, static_cast<unsigned int>(entry.nLen), entry.sData);
      history->m_listLogDataEmpty_2K.PushNode_Back(outIndex[0]);
      Sleep(0);
    }

    while (history->m_listLogData_1K.PopNode_Front(outIndex))
    {
      __LOG_DATA_1K &entry = history->m_LogData_1K[outIndex[0]];
      IOFileWrite_1(entry.szFileName, static_cast<unsigned int>(entry.nLen), entry.sData);
      history->m_listLogDataEmpty_1K.PushNode_Back(outIndex[0]);
      Sleep(0);
    }

    while (history->m_listLogData_200.PopNode_Front(outIndex))
    {
      __LOG_DATA_200 &entry = history->m_LogData_200[outIndex[0]];
      IOFileWrite_1(entry.szFileName, static_cast<unsigned int>(entry.nLen), entry.sData);
      history->m_listLogDataEmpty_200.PushNode_Back(outIndex[0]);
      Sleep(0);
    }

    Sleep(1u);
  }

  _endthreadex(0);
}

void CMgrAvatorLvHistory::OnLoop()
{
  if (m_tmrUpdateTime.CountingTimer())
  {
    _strdate(m_szCurDate);
    m_szCurDate[5] = 0;
    _strtime(m_szCurTime);
    m_szCurTime[5] = 0;
  }
}

unsigned int CMgrAvatorLvHistory::GetTotalWaitSize()
{
  const unsigned int wait2K = m_listLogData_2K.size();
  const unsigned int wait1K = m_listLogData_1K.size();
  const unsigned int wait200 = m_listLogData_200.size();
  return wait2K + wait1K + wait200;
}

void CMgrAvatorLvHistory::WriteFile(char *pszFileName, char *pszLog)
{
  const int logLen = static_cast<int>(strlen_0(pszLog));
  unsigned int outIndex = 0;

  if (logLen >= 0xC8)
  {
    if (logLen >= 0x3E8)
    {
      if (logLen < 0x7D0 && m_listLogDataEmpty_2K.PopNode_Front(&outIndex))
      {
        __LOG_DATA_2K &entry = m_LogData_2K[outIndex];
        strcpy_0(entry.szFileName, pszFileName);
        entry.nLen = logLen;
        memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
        entry.sData[logLen] = '\0';
        m_listLogData_2K.PushNode_Back(outIndex);
        return;
      }
    }
    else if (m_listLogDataEmpty_1K.PopNode_Front(&outIndex))
    {
      __LOG_DATA_1K &entry = m_LogData_1K[outIndex];
      strcpy_0(entry.szFileName, pszFileName);
      entry.nLen = logLen;
      memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
      entry.sData[logLen] = '\0';
      m_listLogData_1K.PushNode_Back(outIndex);
      return;
    }
  }
  else if (m_listLogDataEmpty_200.PopNode_Front(&outIndex))
  {
    __LOG_DATA_200 &entry = m_LogData_200[outIndex];
    strcpy_0(entry.szFileName, pszFileName);
    entry.nLen = logLen;
    memcpy_0(entry.sData, pszLog, static_cast<unsigned int>(logLen));
    entry.sData[logLen] = '\0';
    m_listLogData_200.PushNode_Back(outIndex);
    return;
  }

  IOFileWrite_1(pszFileName, static_cast<unsigned int>(logLen), pszLog);
}

void CMgrAvatorLvHistory::GetNewFileName(unsigned int dwAvatorSerial, char *pszFileName)
{
  const unsigned int localDate = GetLocalDate();

  char dateDir[132]{};
  sprintf(dateDir, "%s\\%d", m_szStdPath, localDate);
  CreateDirectoryA(dateDir, nullptr);
  m_dwLastLocalDate = localDate;

  const unsigned int currentHour = GetCurrentHour();
  char hourDir[148]{};
  sprintf(hourDir, "%s\\%d\\%d", m_szStdPath, m_dwLastLocalDate, currentHour);
  CreateDirectoryA(hourDir, nullptr);
  m_dwLastLocalHour = currentHour;

  char hourText[32]{};
  if (currentHour <= 9)
  {
    sprintf(hourText, "0%d", currentHour);
  }
  else
  {
    sprintf(hourText, "%d", currentHour);
  }

  const unsigned int currentMinute = GetCurrentMin();
  char minuteText[32]{};
  if (currentMinute <= 9)
  {
    sprintf(minuteText, "0%d", currentMinute);
  }
  else
  {
    sprintf(minuteText, "%d", currentMinute);
  }

  const unsigned int currentSecond = GetCurrentSec();
  char secondText[16]{};
  if (currentSecond <= 9)
  {
    sprintf(secondText, "0%d", currentSecond);
  }
  else
  {
    sprintf(secondText, "%d", currentSecond);
  }

  char hmsText[32]{};
  sprintf(hmsText, "%s%s%s", hourText, minuteText, secondText);
  sprintf(
    pszFileName,
    "%s\\%d\\%d\\%d_%s.lhi",
    m_szStdPath,
    m_dwLastLocalDate,
    m_dwLastLocalHour,
    dwAvatorSerial,
    hmsText);
}

void CMgrAvatorLvHistory::start_mastery(
  int /*n*/,
  char *pszAvatorName,
  unsigned int dwLv,
  long double dExp,
  unsigned int dwExpRate,
  int nGrade,
  int *pnMaxPoint,
  _MASTERY_PARAM *pData,
  char *pszFileName)
{
  sData_0[0] = 0;
  sprintf(
    sBuf_0,
    "%s lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
    pszAvatorName,
    dwLv,
    static_cast<double>(dExp),
    dwExpRate,
    nGrade,
    pnMaxPoint[0],
    pnMaxPoint[1],
    pnMaxPoint[2],
    m_szCurDate,
    m_szCurTime);
  strcat_0(sData_0, sBuf_0);

  for (int index = 0; index < 2; ++index)
  {
    if (pData->GetCumPerMast(0, index) > 0)
    {
      const int mastery = pData->GetMasteryPerMast(0, index);
      const int cum = pData->GetCumPerMast(0, index);
      sprintf(sBuf_0, "\tW%d: %d (%d)\r\n", index, cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
  }

  if (pData->GetCumPerMast(1u, 0) > 0)
  {
    const int mastery = pData->GetMasteryPerMast(1u, 0);
    const int cum = pData->GetCumPerMast(1u, 0);
    sprintf(sBuf_0, "\tD: %d (%d)\r\n", cum, mastery);
    strcat_0(sData_0, sBuf_0);
  }

  if (pData->GetCumPerMast(2u, 0) > 0)
  {
    const int mastery = pData->GetMasteryPerMast(2u, 0);
    const int cum = pData->GetCumPerMast(2u, 0);
    sprintf(sBuf_0, "\tP: %d (%d)\r\n", cum, mastery);
    strcat_0(sData_0, sBuf_0);
  }

  for (int index = 0; index < 48; ++index)
  {
    if (pData->m_BaseCum.m_dwSkillCum[index])
    {
      const int skillLv = pData->GetSkillLv(static_cast<unsigned __int8>(index));
      sprintf(sBuf_0, "\tS%d: %d (%d)\r\n", index, pData->m_BaseCum.m_dwSkillCum[index], skillLv);
      strcat_0(sData_0, sBuf_0);
    }
  }

  for (int index = 0; index < 24; ++index)
  {
    if (pData->GetCumPerMast(4u, index) > 0)
    {
      const int mastery = pData->GetMasteryPerMast(4u, index);
      const int cum = pData->GetCumPerMast(4u, index);
      sprintf(sBuf_0, "\tF%d: %d (%d)\r\n", index, cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
  }

  for (int index = 0; index < 3; ++index)
  {
    if (pData->GetCumPerMast(5u, index) > 0)
    {
      const int mastery = pData->GetMasteryPerMast(5u, index);
      const int cum = pData->GetCumPerMast(5u, index);
      sprintf(sBuf_0, "\tM%d: %d (%d)\r\n", index, cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
  }

  if (pData->GetCumPerMast(6u, 0) > 0)
  {
    const int mastery = pData->GetMasteryPerMast(6u, 0);
    const int cum = pData->GetCumPerMast(6u, 0);
    sprintf(sBuf_0, "\tR: %d (%d)\r\n", cum, mastery);
    strcat_0(sData_0, sBuf_0);
  }

  sprintf(sBuf_0, "\r\n");
  strcat_0(sData_0, sBuf_0);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::close(int n, char *pCloseCode, char *pszFileName)
{
  (void)n;
  sprintf(sData_0, "\r\nCLOSE %s [%s %s]\r\n\r\n", pCloseCode, m_szCurDate, m_szCurTime);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::die(int n, char *pszDstName, char *pszDeathName, char *pszFileName)
{
  (void)n;
  sprintf(
    sData_0,
    "DIE %s, <CORPSE: %s> [%s %s]\r\n\r\n",
    pszDstName,
    pszDeathName,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::down_exp(
  int n,
  long double dOldExp,
  unsigned __int16 wOldExpRate,
  long double dNewExp,
  unsigned __int16 wNewExpRate,
  char *pCause,
  char *pszFileName)
{
  (void)n;
  sprintf(
    sData_0,
    "EXP DOWN %.0f(%d) -> %.0f(%d) : %s [%s %s]\r\n\r\n",
    static_cast<double>(dOldExp),
    wOldExpRate,
    static_cast<double>(dNewExp),
    wNewExpRate,
    pCause,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::char_copy(
  int /*n*/,
  char *pszDstName,
  unsigned int dwDstSerial,
  char *pszFileName)
{
  sprintf(sData_0, "Char Copy: dst(%s:%d)\r\n", pszDstName, dwDstSerial);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::alter_pvp(
  int /*n*/,
  long double dPvpVariation,
  CPartyPlayer *pParty,
  char *pszFileName)
{
  char buffer[132]{};
  const bool isPartyMode = pParty->IsPartyMode();

  sprintf_s(sData_0, 0x2710u, "Pvp_Variation: %.0f, Party: %d, ", static_cast<double>(dPvpVariation), isPartyMode);

  if (isPartyMode)
  {
    sprintf_s(buffer, 0x80u, "Boss: %s, Member: ", pParty->m_pPartyBoss->m_wszName);
    strcat_s(sData_0, 0x2710u, buffer);
    for (int j = 0; j < 8; ++j)
    {
      if (pParty->m_pPartyBoss->m_pPartyMember[j])
      {
        sprintf_s(buffer, 0x80u, "%s, ", pParty->m_pPartyBoss->m_pPartyMember[j]->m_wszName);
        strcat_s(sData_0, 0x2710u, buffer);
      }
    }
  }

  sprintf_s(buffer, 0x80u, "[%s %s]\r\n\r\n", m_szCurDate, m_szCurTime);
  strcat_s(sData_0, 0x2710u, buffer);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::adjust_pvpcash(
  bool bAdjust,
  long double dPvpCash,
  long double dPvpTempCash,
  char *pszFileName)
{
  if (bAdjust)
  {
    sprintf(
      sData_0,
      "PVP Cash Adjust After  : PvpCash ( %.0f ), PvpTempCash ( %.0f )\r\n\r\n",
      static_cast<double>(dPvpCash),
      static_cast<double>(dPvpTempCash));
  }
  else
  {
    sprintf(
      sData_0,
      "PVP Cash Adjust Before : PvpCash ( %.0f ), PvpTempCash ( %.0f )\r\n\r\n",
      static_cast<double>(dPvpCash),
      static_cast<double>(dPvpTempCash));
  }
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::recovery_exp(
  int /*n*/,
  long double dOldExp,
  unsigned __int16 wOldExpRate,
  long double dNewExp,
  unsigned __int16 wNewExpRate,
  long double dLossExp,
  int nProbPro,
  char *pCause,
  char *pszFileName)
{
  sprintf(
    sData_0,
    "EXP RECOV %.0f(%d) -> %.0f(%d) : %s (%.0f * %d / 100)[%s %s]\r\n\r\n",
    static_cast<double>(dOldExp),
    wOldExpRate,
    static_cast<double>(dNewExp),
    wNewExpRate,
    pCause,
    static_cast<double>(dLossExp),
    nProbPro,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::update_mastery(
  int /*n*/,
  unsigned __int8 byUserDgr,
  unsigned int dwLv,
  long double dExp,
  unsigned int dwExpRate,
  int nGrade,
  int *pnMaxPoint,
  _MASTERY_PARAM *pData,
  unsigned int *pdwAlter,
  char *pszFileName,
  unsigned __int8 byLogType,
  char *pszTitle)
{
  sData_0[0] = 0;
  if (byLogType)
  {
    switch (byLogType)
    {
      case 1:
        sprintf(
          sBuf_0,
          "CLOSE lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
          dwLv,
          static_cast<double>(dExp),
          dwExpRate,
          nGrade,
          pnMaxPoint[0],
          pnMaxPoint[1],
          pnMaxPoint[2],
          m_szCurDate,
          m_szCurTime);
        break;
      case 2:
        if (pszTitle)
        {
          sprintf(
            sBuf_0,
            "QUEST (%s) lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
            pszTitle,
            dwLv,
            static_cast<double>(dExp),
            dwExpRate,
            nGrade,
            pnMaxPoint[0],
            pnMaxPoint[1],
            pnMaxPoint[2],
            m_szCurDate,
            m_szCurTime);
        }
        else
        {
          sprintf(
            sBuf_0,
            "QUEST lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
            dwLv,
            static_cast<double>(dExp),
            dwExpRate,
            nGrade,
            pnMaxPoint[0],
            pnMaxPoint[1],
            pnMaxPoint[2],
            m_szCurDate,
            m_szCurTime);
        }
        break;
      case 3:
        if (pszTitle)
        {
          sprintf(
            sBuf_0,
            "CLASS (%s) lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
            pszTitle,
            dwLv,
            static_cast<double>(dExp),
            dwExpRate,
            nGrade,
            pnMaxPoint[0],
            pnMaxPoint[1],
            pnMaxPoint[2],
            m_szCurDate,
            m_szCurTime);
        }
        else
        {
          sprintf(
            sBuf_0,
            "CLASS lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
            dwLv,
            static_cast<double>(dExp),
            dwExpRate,
            nGrade,
            pnMaxPoint[0],
            pnMaxPoint[1],
            pnMaxPoint[2],
            m_szCurDate,
            m_szCurTime);
        }
        break;
      default:
        return;
    }
  }
  else
  {
    sprintf(
      sBuf_0,
      "UPDATE lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n",
      dwLv,
      static_cast<double>(dExp),
      dwExpRate,
      nGrade,
      pnMaxPoint[0],
      pnMaxPoint[1],
      pnMaxPoint[2],
      m_szCurDate,
      m_szCurTime);
  }

  strcat_0(sData_0, sBuf_0);
  if (byLogType == 1)
  {
    for (int j = 0; j < 2; ++j)
    {
      if (pData->GetCumPerMast(0, j) > 0)
      {
        const int mastery = pData->GetMasteryPerMast(0, j);
        const int cum = pData->GetCumPerMast(0, j);
        sprintf(sBuf_0, "\tW%d: %d (%d)\r\n", j, cum, mastery);
        strcat_0(sData_0, sBuf_0);
      }
    }
    if (pData->GetCumPerMast(1u, 0) > 0)
    {
      const int mastery = pData->GetMasteryPerMast(1u, 0);
      const int cum = pData->GetCumPerMast(1u, 0);
      sprintf(sBuf_0, "\tD: %d (%d)\r\n", cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
    if (pData->GetCumPerMast(2u, 0) > 0)
    {
      const int mastery = pData->GetMasteryPerMast(2u, 0);
      const int cum = pData->GetCumPerMast(2u, 0);
      sprintf(sBuf_0, "\tP: %d (%d)\r\n", cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
    for (int j = 0; j < 48; ++j)
    {
      if (pData->m_BaseCum.m_dwSkillCum[j])
      {
        const int skillLv = pData->GetSkillLv(j);
        sprintf(sBuf_0, "\tS%d: %d (%d)\r\n", j, pData->m_BaseCum.m_dwSkillCum[j], skillLv);
        strcat_0(sData_0, sBuf_0);
      }
    }
    for (int j = 0; j < 24; ++j)
    {
      if (pData->GetCumPerMast(4u, j) > 0)
      {
        const int mastery = pData->GetMasteryPerMast(4u, j);
        const int cum = pData->GetCumPerMast(4u, j);
        sprintf(sBuf_0, "\tF%d: %d (%d)\r\n", j, cum, mastery);
        strcat_0(sData_0, sBuf_0);
      }
    }
    for (int j = 0; j < 3; ++j)
    {
      if (pData->GetCumPerMast(5u, j) > 0)
      {
        const int mastery = pData->GetMasteryPerMast(5u, j);
        const int cum = pData->GetCumPerMast(5u, j);
        sprintf(sBuf_0, "\tM%d: %d (%d)\r\n", j, cum, mastery);
        strcat_0(sData_0, sBuf_0);
      }
    }
    if (pData->GetCumPerMast(6u, 0) > 0)
    {
      const int mastery = pData->GetMasteryPerMast(6u, 0);
      const int cum = pData->GetCumPerMast(6u, 0);
      sprintf(sBuf_0, "\tR: %d (%d)\r\n", cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
  }
  else
  {
    for (int k = 0; k < 2; ++k)
    {
      if (pdwAlter[k])
      {
        const int mastery = pData->GetMasteryPerMast(0, k);
        const int cum = pData->GetCumPerMast(0, k);
        sprintf(sBuf_0, "\tW%d: %d (%d)\r\n", k, cum, mastery);
        strcat_0(sData_0, sBuf_0);
      }
    }
    if (pdwAlter[2])
    {
      const int mastery = pData->GetMasteryPerMast(1u, 0);
      const int cum = pData->GetCumPerMast(1u, 0);
      sprintf(sBuf_0, "\tD: %d (%d)\r\n", cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
    if (pdwAlter[3])
    {
      const int mastery = pData->GetMasteryPerMast(2u, 0);
      const int cum = pData->GetCumPerMast(2u, 0);
      sprintf(sBuf_0, "\tP: %d (%d)\r\n", cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
    for (int k = 0; k < 48; ++k)
    {
      if (pdwAlter[k + 4])
      {
        const int skillLv = pData->GetSkillLv(k);
        sprintf(sBuf_0, "\tS%d: %d (%d)\r\n", k, pData->m_BaseCum.m_dwSkillCum[k], skillLv);
        strcat_0(sData_0, sBuf_0);
      }
    }
    for (int k = 0; k < 24; ++k)
    {
      if (pdwAlter[k + 52])
      {
        const int mastery = pData->GetMasteryPerMast(4u, k);
        const int cum = pData->GetCumPerMast(4u, k);
        sprintf(sBuf_0, "\tF%d: %d (%d)\r\n", k, cum, mastery);
        strcat_0(sData_0, sBuf_0);
      }
    }
    for (int k = 0; k < 3; ++k)
    {
      if (pdwAlter[k + 76])
      {
        const int mastery = pData->GetMasteryPerMast(5u, k);
        const int cum = pData->GetCumPerMast(5u, k);
        sprintf(sBuf_0, "\tM%d: %d (%d)\r\n", k, cum, mastery);
        strcat_0(sData_0, sBuf_0);
      }
    }
    if (pdwAlter[79])
    {
      const int mastery = pData->GetMasteryPerMast(6u, 0);
      const int cum = pData->GetCumPerMast(6u, 0);
      sprintf(sBuf_0, "\tR: %d (%d)\r\n", cum, mastery);
      strcat_0(sData_0, sBuf_0);
    }
  }

  sprintf(sBuf_0, "\r\n");
  strcat_0(sData_0, sBuf_0);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::down_animus_exp(
  unsigned __int64 dw64OldExp,
  unsigned __int64 dw64NewExp,
  __int64 i64Alter,
  char *pszFileName)
{
  sprintf(
    sData_0,
    "ANIMUS EXP DOWN %I64u -> %I64u : Alter(%I64d) [%s %s]\r\n\r\n",
    dw64OldExp,
    dw64NewExp,
    i64Alter,
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::upgrade_lv(int /*n*/, unsigned int dwLv, int nGrade, int *pnMaxPoint, char *pszFileName)
{
  sprintf(
    sData_0,
    "LV UP %d grade:%d max:%d/%d/%d [%s %s]\r\n\r\n",
    dwLv,
    nGrade,
    pnMaxPoint[0],
    pnMaxPoint[1],
    pnMaxPoint[2],
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData_0);
}

void CMgrAvatorLvHistory::downgrade_lv(int /*n*/, unsigned int dwLv, int nGrade, int *pnMaxPoint, char *pszFileName)
{
  sprintf(
    sData_0,
    "LV DOWN %d grade:%d max:%d/%d/%d [%s %s]\r\n\r\n",
    dwLv,
    nGrade,
    pnMaxPoint[0],
    pnMaxPoint[1],
    pnMaxPoint[2],
    m_szCurDate,
    m_szCurTime);
  WriteFile(pszFileName, sData_0);
}
