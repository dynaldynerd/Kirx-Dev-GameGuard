#include "pch.h"

#include "CMgrAvatorLvHistory.h"

#include <cstdio>
#include <cstring>

#include "CPartyPlayer.h"
#include "WorldServerUtil.h"

namespace
{
  char sData_0[10000]{};
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
