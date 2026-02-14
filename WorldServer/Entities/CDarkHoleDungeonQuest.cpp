#include "pch.h"

#include "CDarkHoleDungeonQuest.h"

#include <mmsystem.h>

#include "CDarkHole.h"
#include "CMapData.h"
#include "CPartyPlayer.h"
#include "GlobalObjects.h"
namespace
{
  static const char kDarkHoleQuestMatchSection[] = "MATCH";
  static const char kDarkHoleQuestDefaultValue[] = "default";
  static const char kDarkHoleQuestIndexIni[] = ".\\Quest\\Index.ini";
  static const char kDarkHoleQuestNoMatch[] = "X";
  static const char kDarkHoleQuestPathFmt[] = ".\\Quest\\%s.txt";
}

CDarkHoleDungeonQuest g_DarkHoleQuest;

CDarkHoleDungeonQuest::CDarkHoleDungeonQuest()
  : m_bLoad(false)
{
  for (int index = 0; index < 100; ++index)
  {
    m_QuestSetup[index] = nullptr;
  }
  for (int index = 0; index < 128; ++index)
  {
    m_Channel[index].m_wChannelIndex = index;
  }
}

CDarkHoleDungeonQuest::~CDarkHoleDungeonQuest() = default;

bool CDarkHoleDungeonQuest::LoadDarkHoleQuest()
{

  for (int n = 0;; ++n)
  {
    const int recordNum = static_cast<int>(g_Main.m_tblItemData[23].GetRecordNum());
    if (n >= recordNum)
      break;
    _base_fld *record = g_Main.m_tblItemData[23].GetRecord(n);
    char returnedString[160]{};
    GetPrivateProfileStringA(
      kDarkHoleQuestMatchSection,
      record->m_strCode,
      kDarkHoleQuestDefaultValue,
      returnedString,
      0x80u,
      kDarkHoleQuestIndexIni);
    if (!strcmp_0(returnedString, kDarkHoleQuestNoMatch))
      MyMessageBox("Quest Dungeon Load Error", "%s item no match quest", record->m_strCode);
    char buffer[144]{};
    sprintf(buffer, kDarkHoleQuestPathFmt, returnedString);
    if (!SetupQuest(buffer))
    {
      const char *errorMsg = GetErrorMsg();
      MyMessageBox("Quest Dungeon Load Error", "%s", errorMsg);
      return false;
    }
  }
  m_bLoad = true;
  m_dwCheckLastTime = timeGetTime();
  return true;
}

CDarkHoleChannel *CDarkHoleDungeonQuest::GetChannel(unsigned int dwChannelIndex)
{
  return &m_Channel[dwChannelIndex];
}

CDarkHoleChannel *CDarkHoleDungeonQuest::CanOpenChannel(int nQuestIndex)
{
  int layerIndex = SearchEmptyDarkHoleLayer(nQuestIndex);
  if (layerIndex == -1)
  {
    return nullptr;
  }
  const int channelIndex = SearchEmptyDarkHoleChannel();
  if (channelIndex == -1)
  {
    return nullptr;
  }
  return &m_Channel[channelIndex];
}

int CDarkHoleDungeonQuest::SearchEmptyDarkHoleChannel()
{
  for (int j = 0; j < 128; ++j)
  {
    if (!m_Channel[j].IsFill())
    {
      return j;
    }
  }
  return -1;
}

int CDarkHoleDungeonQuest::SearchEmptyDarkHoleLayer(int nQuestIndex)
{
  if (nQuestIndex >= m_nLoadQuest)
  {
    return -1;
  }

  CMapData *useMap = m_QuestSetup[nQuestIndex]->pUseMap;
  for (int j = 0; j < useMap->m_pMapSet->m_nLayerNum; ++j)
  {
    if (!useMap->m_ls[j].IsActiveLayer())
    {
      return j;
    }
  }
  return -1;
}

CDarkHoleChannel *CDarkHoleDungeonQuest::OpenChannel(int nQuestIndex, CPlayer *pOpener, CDarkHole *pHoleObj)
{
  _dh_quest_setup *questSetup = m_QuestSetup[nQuestIndex];
  if (questSetup->bPartyOnly && !pOpener->m_pPartyMgr->IsPartyMode())
  {
    return nullptr;
  }

  const int layerIndex = SearchEmptyDarkHoleLayer(nQuestIndex);
  if (layerIndex == -1)
  {
    return nullptr;
  }

  const int channelIndex = SearchEmptyDarkHoleChannel();
  if (channelIndex == -1)
  {
    return nullptr;
  }

  CDarkHoleChannel *channel = &m_Channel[channelIndex];
  questSetup->SetRealBoss(true);
  channel->OpenDungeon(m_QuestSetup[nQuestIndex], layerIndex, pOpener, pHoleObj);
  return channel;
}

CDarkHoleChannel *CDarkHoleDungeonQuest::SearchOncePlayedChannel(unsigned int dwMemberSerial)
{
  for (int j = 0; j < 128; ++j)
  {
    if (m_Channel[j].IsFill())
    {
      CDarkHoleChannel::__enter_member enterInfo;
      if (m_Channel[j].m_listEnterMember.IsInList(dwMemberSerial, reinterpret_cast<char *>(&enterInfo))
          && enterInfo.bDisnormalClose)
      {
        return &m_Channel[j];
      }
    }
  }
  return nullptr;
}

void CDarkHoleDungeonQuest::CheckQuestOnLoop()
{
  if (!m_bLoad)
  {
    return;
  }

  const DWORD now = timeGetTime();
  if (now - m_dwCheckLastTime < 1000)
  {
    return;
  }

  m_dwCheckLastTime = now;
  for (int channelIndex = 0; channelIndex < 128; ++channelIndex)
  {
    CDarkHoleChannel *channel = &m_Channel[channelIndex];
    if (channel->IsFill())
    {
      channel->OnLoop();
    }
  }
}
