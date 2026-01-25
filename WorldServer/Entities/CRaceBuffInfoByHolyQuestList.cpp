#include "pch.h"

#include "CRaceBuffInfoByHolyQuestList.h"

#include "CMainThread.h"
#include "CLogFile.h"

bool CRaceBuffInfoByHolyQuestList::Init()
{
  const unsigned int count =
    static_cast<unsigned int>(GetPrivateProfileIntA("RaceBuff", "thCnt", 0, ".\\Initialize\\NewHolySystem.ini"));
  if (count == 0)
  {
    return true;
  }

  m_vecInfo.assign(count, nullptr);
  if (m_vecInfo.size() != count)
  {
    g_Main.m_logLoadingError.Write(
      "CRaceBuffInfoByHolyQuestList::Init() : thCnt(%u) != m_vecInfo.size()(%d)",
      count,
      static_cast<int>(m_vecInfo.size()));
    return false;
  }

  for (unsigned int i = 0; i < count; ++i)
  {
    m_vecInfo[i] = new CRaceBuffInfoByHolyQuestfGroup(i);
    if (m_vecInfo[i] == nullptr)
    {
      g_Main.m_logLoadingError.Write(
        "CRaceBuffInfoByHolyQuestList::Init() : new CRaceBuffInfoByHolyQuestfGroup(%u) NULL!",
        i);
      return false;
    }
    if (!m_vecInfo[i]->Init())
    {
      g_Main.m_logLoadingError.Write(
        "CRaceBuffInfoByHolyQuestList::Init() : m_vecInfo[i(%u)]->Init() Fail!",
        i);
      return false;
    }
  }

  return true;
}
