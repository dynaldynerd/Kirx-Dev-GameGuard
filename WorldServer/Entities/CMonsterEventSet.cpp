#include "pch.h"

#include "CMonsterEventSet.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mmsystem.h>

#include "CMapData.h"
#include "CMonster.h"
#include "CPlayer.h"
#include "CLogFile.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CMonsterEventSet g_MonsterEventSet;

namespace
{
void ResetMonsterState(_event_set::_monster_set::_state *state)
{
  if (state)
  {
    memset_0(state, 0, sizeof(*state));
  }
}
} // namespace

bool CMonsterEventSet::IsINIFileChanged(const char *pszFileName, _FILETIME ftCurr)
{
  _FILETIME ftWrite{};
  if (!GetLastWriteFileTime(pszFileName, &ftWrite))
  {
    return false;
  }

  return ftCurr.dwLowDateTime != ftWrite.dwLowDateTime || ftCurr.dwHighDateTime != ftWrite.dwHighDateTime;
}

bool CMonsterEventSet::StartEventSet(char *pszEventCode, char *pwszErrCode, CPlayer *pOne)
{
  char loadMessage[516]{};
  strcpy_0(loadMessage, "without reload ini file");

  if (!m_bLoadEventLooting)
  {
    sprintf(pwszErrCode, "Event Set Looting File Not Loaded : %s", ".\\Initialize\\EventSetLooting.ini");
    g_Main.m_logEvent.Write("%s", pwszErrCode);
    return false;
  }

  if (IsINIFileChanged(".\\Initialize\\EventSet.ini", m_ftWrite))
  {
    memset_0(m_EventSet, 0, sizeof(m_EventSet));
    if (!LoadEventSet(loadMessage))
    {
      sprintf(pwszErrCode, "Reload INI file fail!!! : %s", loadMessage);
      g_Main.m_logEvent.Write("Reload INI file fail!!! : %s >> %s", ".\\Initialize\\EventSet.ini", loadMessage);
      return false;
    }

    sprintf(loadMessage, "with reload ini file : %s", ".\\Initialize\\EventSet.ini");
    g_Main.m_logEvent.Write("Reload INI file for Monster Event Set >> %s", ".\\Initialize\\EventSet.ini");
  }

  int matchedEventCount = 0;
  for (int j = 0; j < 10; ++j)
  {
    _event_set *eventSet = &m_EventSet[j];
    if (strcmp_0(eventSet->m_strId, pszEventCode) != 0)
    {
      continue;
    }

    if (eventSet->m_bOper)
    {
      if (pwszErrCode)
      {
        sprintf(pwszErrCode, "now actived");
      }
      return false;
    }

    ++matchedEventCount;
    for (int k = 0; k < 10; ++k)
    {
      _event_set::_monster_set *monsterSet = &eventSet->m_MonSet[k];
      if (!monsterSet->bIsSet)
      {
        continue;
      }

      ResetMonsterState(&monsterSet->m_State);

      if (monsterSet->bUnknownMap)
      {
        if (!pOne || !pOne->m_pCurMap)
        {
          continue;
        }

        monsterSet->pMap = pOne->m_pCurMap;
        memcpy_0(monsterSet->fPos, pOne->m_fCurPos, sizeof(monsterSet->fPos));
      }

      if (!monsterSet->pMap || !monsterSet->pMonsterFld)
      {
        continue;
      }

      int spawnRange = static_cast<int>(monsterSet->wNum) * 20;
      if (spawnRange >= 500)
      {
        spawnRange = 500;
      }

      int spawnedCount = 0;
      for (int m = 0; m < monsterSet->wNum; ++m)
      {
        if (monsterSet->byRegenProb < static_cast<unsigned __int8>(rand() % 100))
        {
          continue;
        }

        float pos[3]{};
        if (!monsterSet->pMap->GetRandPosVirtualDumExcludeStdRange(monsterSet->fPos, spawnRange, 0, pos))
        {
          memcpy_0(pos, monsterSet->fPos, sizeof(pos));
        }

        CMonster *repMonster = CreateRepMonster(
          monsterSet->pMap,
          0,
          pos,
          monsterSet->pMonsterFld->m_strCode,
          nullptr,
          false,
          true,
          false,
          false,
          false);
        if (!repMonster)
        {
          continue;
        }

        monsterSet->m_State.MonInfo[spawnedCount].pMon = repMonster;
        monsterSet->m_State.MonInfo[spawnedCount].dwSerial = repMonster->m_dwObjSerial;
        monsterSet->m_State.MonInfo[spawnedCount].pMonFld = monsterSet->pMonsterFld;
        ++spawnedCount;

        repMonster->DisableStdItemLoot();
        repMonster->m_pEventSet = eventSet;
      }

      monsterSet->m_State.nRespawnNum = spawnedCount;
      monsterSet->m_State.dwLastUpdateTime = timeGetTime();
      monsterSet->m_State.dwStartTime = timeGetTime();
      monsterSet->m_State.bOper = true;
    }

    eventSet->m_bOper = true;
  }

  if (matchedEventCount)
  {
    strcpy_0(pwszErrCode, loadMessage);
    g_Main.m_logEvent.Write("Start Event Set (by cheat) >> %s", pszEventCode);
    return true;
  }

  if (pwszErrCode)
  {
    sprintf(pwszErrCode, "can't find event set id");
  }
  return false;
}

bool CMonsterEventSet::StopEventSet(char *pszEventCode, char *pwszErrCode)
{
  int matchedEventCount = 0;

  for (int j = 0; j < 10; ++j)
  {
    _event_set *eventSet = &m_EventSet[j];
    if (strcmp_0(eventSet->m_strId, pszEventCode) != 0)
    {
      continue;
    }

    if (!eventSet->m_bOper)
    {
      if (pwszErrCode)
      {
        sprintf(pwszErrCode, "now stoped");
      }
      return false;
    }

    ++matchedEventCount;
    for (int k = 0; k < 10; ++k)
    {
      _event_set::_monster_set *monsterSet = &eventSet->m_MonSet[k];
      if (!monsterSet->bIsSet)
      {
        continue;
      }

      for (int m = 0; m < monsterSet->m_State.nRespawnNum; ++m)
      {
        _event_set::_monster_set::_state::_mon *monInfo = &monsterSet->m_State.MonInfo[m];
        if (monInfo->pMon && monInfo->pMon->m_bLive && monInfo->pMon->m_dwObjSerial == monInfo->dwSerial)
        {
          monInfo->pMon->Destroy(1u, nullptr);
        }
      }

      ResetMonsterState(&monsterSet->m_State);
    }

    eventSet->m_bOper = false;
  }

  if (matchedEventCount)
  {
    g_Main.m_logEvent.Write("Stop Event Set (by cheat) >> %s", pszEventCode);
    return true;
  }

  if (pwszErrCode)
  {
    sprintf(pwszErrCode, "can't find event set id");
  }
  return false;
}

bool CMonsterEventSet::LoadEventSet(char *errBuffer)
{
  if (errBuffer != nullptr)
  {
    errBuffer[0] = '\0';
  }
  return true;
}

bool CMonsterEventSet::LoadEventSetLooting()
{
  _FILETIME ftWrite{};
  if (!GetLastWriteFileTime(".\\Initialize\\EventSetLooting.ini", &ftWrite))
  {
    g_Main.m_logLoadingError.Write(
      "Event Set Looting INI Load Error >> can't find INI file : %s",
      ".\\Initialize\\EventSetLooting.ini");
    m_bLoadEventLooting = false;
    return false;
  }

  m_ftLootingWrite = ftWrite;
  memset_0(m_EventSetLootingList, 0, sizeof(m_EventSetLootingList));

  FILE *stream = nullptr;
  if (fopen_s(&stream, ".\\Initialize\\EventSetLooting.ini", "r+t"))
  {
    g_Main.m_logLoadingError.Write(
      "Event Set Looting INI Load Error >> can't open INI file : %s",
      ".\\Initialize\\EventSetLooting.ini");
    m_bLoadEventLooting = false;
    return false;
  }

  bool inItemSection = false;
  int lootingCount = 0;
  char buffer[1056]{};
  while (fgets(buffer, 1024, stream))
  {
    if (buffer[0] == ';' || buffer[0] == '\n')
    {
      continue;
    }

    const size_t lootingTagLen = strlen_0("[Looting]");
    if (!strncmp_0(buffer, "[Looting]", lootingTagLen))
    {
      inItemSection = false;
      continue;
    }

    const size_t itemTagLen = strlen_0("[Item]");
    if (!strncmp_0(buffer, "[Item]", itemTagLen))
    {
      inItemSection = true;
      continue;
    }

    char token0[1024]{};
    char token1[1024]{};
    char token2[1024]{};
    char token3[1024]{};
    char token4[1024]{};
    char *tokens[5] = {token0, token1, token2, token3, token4};

    if (ParsingCommandA(buffer, 5, tokens, 1023) != 5)
    {
      continue;
    }

    if (inItemSection)
    {
      _event_set_looting *eventLooting = GetEvenSetLooting(token0);
      if (!eventLooting)
      {
        m_bLoadEventLooting = false;
        fclose(stream);
        g_Main.m_logLoadingError.Write(
          "Event Set Looting INI Load Error >> can't find [Looting] data : %s",
          token0);
        return false;
      }

      _event_set_looting::_event_item *item =
        &eventLooting->stEventItemList[eventLooting->nItemCount];
      strcpy_0(item->strCode, token1);
      item->wDropCount = static_cast<unsigned __int16>(std::atoi(token2));
      item->wDuration = static_cast<unsigned __int16>(std::atoi(token3));
      item->byProb = static_cast<unsigned __int8>(std::atoi(token4));
      ++eventLooting->nItemCount;
    }
    else
    {
      _event_set_looting *entry = &m_EventSetLootingList[lootingCount];
      strcpy_0(entry->strCode, token0);
      entry->wMagnifications = static_cast<unsigned __int16>(std::atoi(token1));
      entry->wRange = static_cast<unsigned __int16>(std::atoi(token2));
      entry->bWithHolyScanner = std::atoi(token3);
      entry->byLootAuth = static_cast<unsigned __int8>(std::atoi(token4));
      ++lootingCount;
    }
  }

  fclose(stream);
  m_bLoadEventLooting = true;
  return true;
}

_event_set_looting *CMonsterEventSet::GetEvenSetLooting(const char *pszCode)
{
  for (int j = 0; j < 100; ++j)
  {
    if (!strcmp_0(m_EventSetLootingList[j].strCode, pszCode))
    {
      return &m_EventSetLootingList[j];
    }
  }
  return nullptr;
}
