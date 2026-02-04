#include "pch.h"

#include "CRaceBuffInfoByHolyQuestfGroup.h"

#include "CMainThread.h"
#include "CLogFile.h"

const char *CRaceBuffInfoByHolyQuestfGroup::TYPE_NAME[4] = {"Win", "Fail", "Lose", "Chaos"};

CRaceBuffInfoByHolyQuestfGroup::CRaceBuffInfoByHolyQuestfGroup(unsigned int nth)
  : m_uiNTh(nth)
{
}

bool CRaceBuffInfoByHolyQuestfGroup::Init()
{
  m_vecInfo.assign(4, nullptr);
  if (m_vecInfo.size() != 4)
  {
    g_Main.m_logLoadingError.Write(
      "CRaceBuffInfoByHolyQuestfGroup::Init() : MAX_TYPE_NUM(%u) != m_vecInfo.size()(%d)",
      4,
      static_cast<int>(m_vecInfo.size()));
    return false;
  }

  for (unsigned int i = 0; i < 4; ++i)
  {
    CRaceBuffInfoByHolyQuest *info =
      CRaceBuffInfoByHolyQuest::Create(m_uiNTh, const_cast<char *>(TYPE_NAME[i]));
    m_vecInfo[i] = info;
    if (info == nullptr)
    {
      g_Main.m_logLoadingError.Write(
        "CRaceBuffInfoByHolyQuestfGroup::Init() : CRaceBuffInfoByHolyQuest::Create( m_uiNTh(%u), TYPE_NAME[i](%s) ) NULL!",
        i,
        TYPE_NAME[i]);
    }
  }

  return true;
}

bool CRaceBuffInfoByHolyQuestfGroup::Apply(unsigned int iResultType, CPlayer *pkDest)
{
  if (iResultType >= 4 || !pkDest)
  {
    return false;
  }
  if (iResultType >= m_vecInfo.size())
  {
    return false;
  }
  return m_vecInfo[iResultType]->Apply(pkDest);
}

bool CRaceBuffInfoByHolyQuestfGroup::Release(unsigned int iResultType, CPlayer *pkDest)
{
  if (iResultType >= 4 || !pkDest)
  {
    return false;
  }
  if (iResultType >= m_vecInfo.size())
  {
    return false;
  }
  return m_vecInfo[iResultType]->Release(pkDest);
}

bool CRaceBuffInfoByHolyQuestfGroup::CreateComplete(unsigned int iResultType, CPlayer *pkDest)
{
  if (iResultType >= 4 || !pkDest)
  {
    return false;
  }
  if (iResultType >= m_vecInfo.size())
  {
    return false;
  }
  return m_vecInfo[iResultType]->CreateComplete(pkDest);
}
