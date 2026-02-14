#include "pch.h"

#include "CMgrAvatorQuestHistory.h"

#include <cstdio>
#include <cstring>
#include <process.h>

#include "WorldServerUtil.h"

namespace
{
constexpr char kHistoryIniSection[] = "System";
constexpr char kHistoryPathKey[] = "HistoryPath";
constexpr char kHistoryPathDefault[] = "c:\\History";
constexpr char kHistoryIniFile[] = "..\\WorldInfo\\WorldInfo.ini";
constexpr char kHistoryQuestSubdirFmt[] = "%s\\Quest";
}

CMgrAvatorQuestHistory::CMgrAvatorQuestHistory()
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

  sprintf(m_szStdPath, kHistoryQuestSubdirFmt, returnedString);
  CreateDirectoryA(m_szStdPath, nullptr);

  _strtime(m_szCurTime);
  m_szCurTime[5] = 0;

  m_tmrUpdateTime.BeginTimer(0xEA60u);
  m_listLogData.SetList(0x9E4u);
  m_listLogDataEmpty.SetList(0x9E4u);

  for (unsigned int index = 0; index < 2532; ++index)
  {
    m_listLogDataEmpty.PushNode_Back(index);
  }

  m_bIOThread = true;
  _beginthread(&CMgrAvatorQuestHistory::IOThread, 0, this);
}

CMgrAvatorQuestHistory::~CMgrAvatorQuestHistory()
{
  m_bIOThread = false;
}

void __cdecl CMgrAvatorQuestHistory::IOThread(void *pv)
{
  CMgrAvatorQuestHistory *const history = static_cast<CMgrAvatorQuestHistory *>(pv);
  unsigned int outIndex[5]{};
  while (history->m_bIOThread)
  {
    history->m_FrameRate.CalcSpeedPerFrame();
    while (history->m_listLogData.PopNode_Front(outIndex))
    {
      __LOG_DATA &entry = history->m_LogData[outIndex[0]];
      IOFileWrite_2(entry.szFileName, static_cast<unsigned int>(entry.nLen), entry.sData);
      history->m_listLogDataEmpty.PushNode_Back(outIndex[0]);
      Sleep(0);
    }
    Sleep(1u);
  }

  _endthreadex(0);
}
