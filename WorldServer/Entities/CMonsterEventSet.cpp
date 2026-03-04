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

CMonsterEventSet *g_MonsterEventSet = nullptr;

_event_set::_monster_set::_state::_state()
{
  init();
}

void _event_set::_monster_set::_state::init()
{
  memset_0(this, 0, sizeof(*this));
}

_event_set::_monster_set::_monster_set()
  : m_State()
{
}

_event_set::_event_set()
{
  init();
}

void _event_set::init()
{
  memset_0(this, 0, sizeof(*this));
}

_event_set_looting::_event_set_looting()
{
  init();
}

void _event_set_looting::init()
{
  memset_0(this, 0, sizeof(*this));
}

CMonsterEventSet::CMonsterEventSet()
{
  m_bLoadEventLooting = false;
}

CMonsterEventSet::~CMonsterEventSet()
{
}

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

      monsterSet->m_State.init();

      if (monsterSet->bUnknownMap)
      {
        monsterSet->pMap = pOne->m_pCurMap;
        memcpy_0(monsterSet->fPos, pOne->m_fCurPos, sizeof(monsterSet->fPos));
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
        repMonster->LinkEventSet(eventSet);
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

      monsterSet->m_State.init();
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

void CMonsterEventSet::CheckEventSetRespawn()
{
  const DWORD now = timeGetTime();
  for (int eventIndex = 0; eventIndex < 10; ++eventIndex)
  {
    _event_set *eventSet = &m_EventSet[eventIndex];
    if (!eventSet->m_bOper)
    {
      continue;
    }

    bool hasRunningMonsterSet = false;
    for (int setIndex = 0; setIndex < 10; ++setIndex)
    {
      _event_set::_monster_set *monsterSet = &eventSet->m_MonSet[setIndex];
      if (!monsterSet->bIsSet || !monsterSet->m_State.bOper)
      {
        continue;
      }

      hasRunningMonsterSet = true;
      if (monsterSet->dwDuring && now - monsterSet->m_State.dwStartTime >= monsterSet->dwDuring)
      {
        for (int monIndex = 0; monIndex < monsterSet->m_State.nRespawnNum; ++monIndex)
        {
          _event_set::_monster_set::_state::_mon *monInfo = &monsterSet->m_State.MonInfo[monIndex];
          if (monInfo->pMon && monInfo->pMon->m_bLive && monInfo->pMon->m_dwObjSerial == monInfo->dwSerial)
          {
            monInfo->pMon->Destroy(1u, nullptr);
          }
        }

        monsterSet->m_State.init();
        g_Main.m_logEvent.Write("Stop Event Monster Set (by during) >> %s", monsterSet->pMonsterFld->m_strCode);
      }
      else if (now - monsterSet->m_State.dwLastUpdateTime >= monsterSet->dwRegenTerm)
      {
        int regenRange = 20 * monsterSet->m_State.nRespawnNum;
        if (regenRange >= 500)
        {
          regenRange = 500;
        }

        for (int monIndex = 0; monIndex < monsterSet->m_State.nRespawnNum; ++monIndex)
        {
          _event_set::_monster_set::_state::_mon *monInfo = &monsterSet->m_State.MonInfo[monIndex];
          if (monInfo->pMon && monInfo->pMon->m_bLive && monInfo->pMon->m_dwObjSerial == monInfo->dwSerial)
          {
            continue;
          }

          if (monsterSet->byRegenProb < static_cast<unsigned __int8>(rand() % 100))
          {
            continue;
          }

          float spawnPos[3]{};
          if (!monsterSet->pMap->GetRandPosVirtualDumExcludeStdRange(monsterSet->fPos, regenRange, 0, spawnPos))
          {
            memcpy_0(spawnPos, monsterSet->fPos, sizeof(spawnPos));
          }

          CMonster *monster = CreateRepMonster(
            monsterSet->pMap,
            0,
            spawnPos,
            monsterSet->pMonsterFld->m_strCode,
            nullptr,
            false,
            true,
            false,
            false,
            false);
          if (monster)
          {
            monInfo->pMon = monster;
            monInfo->dwSerial = monster->m_dwObjSerial;
            monInfo->pMonFld = monsterSet->pMonsterFld;
            monster->DisableStdItemLoot();
            monster->LinkEventSet(eventSet);
          }
          else
          {
            monInfo->pMon = nullptr;
          }
        }

        monsterSet->m_State.dwLastUpdateTime = now;
      }
    }

    if (!hasRunningMonsterSet)
    {
      eventSet->m_bOper = false;
      g_Main.m_logEvent.Write("Stop Event Set (by during) >> %s", eventSet->m_strId);
    }
  }

  if (IsINIFileChanged(".\\Initialize\\EventSetLooting.ini", m_ftLootingWrite) && !LoadEventSetLooting())
  {
    g_Main.m_logEvent.Write(
      "Reload Event set looting INI file fail >> %s",
      ".\\Initialize\\EventSetLooting.ini");
  }
}

_event_set *CMonsterEventSet::GetEmptyEventSet()
{
  for (int index = 0; index < 10; ++index)
  {
    if (m_EventSet[index].m_strId[0] == '\0')
    {
      return &m_EventSet[index];
    }
  }
  return nullptr;
}

_event_set::_monster_set *CMonsterEventSet::GetMonsterSet(_event_set *eventSet)
{
  if (!eventSet)
  {
    return nullptr;
  }

  for (int index = 0; index < 10; ++index)
  {
    if (!eventSet->m_MonSet[index].bIsSet)
    {
      return &eventSet->m_MonSet[index];
    }
  }
  return nullptr;
}

bool CMonsterEventSet::LoadEventSet(char *errBuffer)
{
  _FILETIME writeTime{};
  if (!GetLastWriteFileTime(".\\Initialize\\EventSet.ini", &writeTime))
  {
    sprintf(errBuffer, "Event Set Load Error >> can't find .ini file : %s", ".\\Initialize\\EventSet.ini");
    g_Main.m_logLoadingError.Write(errBuffer);
    return false;
  }

  m_ftWrite = writeTime;

  FILE *stream = nullptr;
  if (fopen_s(&stream, ".\\Initialize\\EventSet.ini", "r+t"))
  {
    return false;
  }

  char tokenStorage[10][64]{};
  char *token0 = tokenStorage[0];
  char *token1 = tokenStorage[1];
  char *token2 = tokenStorage[2];
  char *token3 = tokenStorage[3];
  char *token4 = tokenStorage[4];
  char *token5 = tokenStorage[5];
  char *token6 = tokenStorage[6];
  char *token7 = tokenStorage[7];
  char *token8 = tokenStorage[8];
  char *token9 = tokenStorage[9];
  char *tokens[10] = {token0, token1, token2, token3, token4, token5, token6, token7, token8, token9};
  char buffer[1056]{};

  while (fgets(buffer, 1024, stream))
  {
    if (buffer[0] == ';' || buffer[0] == '\n' || buffer[0] == '\r')
    {
      continue;
    }

    memset_0(tokenStorage, 0, sizeof(tokenStorage));
    const int parsedCount = ParsingCommandA(buffer, 10, tokens, 64);
    if (parsedCount == 1)
    {
      continue;
    }

    if (parsedCount != 10)
    {
      sprintf(errBuffer, "Event Set Load Error >> event parameter count error : %d", parsedCount);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    _event_set *eventSet = GetEmptyEventSet();
    if (!eventSet)
    {
      sprintf(errBuffer, "Event Set Load Error >> over max event set error : %d", 10);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    if (eventSet->m_strId[0] == '\0')
    {
      strcpy_0(eventSet->m_strId, token0);
    }

    _event_set::_monster_set *monsterSet = GetMonsterSet(eventSet);

    char mapCodeUpper[72]{};
    strcpy_0(mapCodeUpper, token1);
    if (!strcmp_0(_strupr(mapCodeUpper), "UNKNOWN"))
    {
      monsterSet->pMap = nullptr;
      monsterSet->fPos[0] = -1.0f;
      monsterSet->fPos[1] = -1.0f;
      monsterSet->fPos[2] = -1.0f;
      monsterSet->bUnknownMap = true;
    }
    else
    {
      CMapData *map = g_MapOper.GetMap(token1);
      if (!map)
      {
        sprintf(errBuffer, "Event Set Load Error : %s >> map code error : %s", token0, token1);
        g_Main.m_logLoadingError.Write(errBuffer);
        fclose(stream);
        return false;
      }

      monsterSet->pMap = map;
      monsterSet->fPos[0] = static_cast<float>(atoi(token2));
      monsterSet->fPos[1] = static_cast<float>(atoi(token3));
      monsterSet->fPos[2] = static_cast<float>(atoi(token4));
      if (!map->IsMapIn(monsterSet->fPos))
      {
        sprintf(
          errBuffer,
          "Event Set Load Error : %s >> xyz range error : %d %d %d",
          token0,
          static_cast<int>(monsterSet->fPos[0]),
          static_cast<int>(monsterSet->fPos[1]),
          static_cast<int>(monsterSet->fPos[2]));
        g_Main.m_logLoadingError.Write(errBuffer);
        fclose(stream);
        return false;
      }
      monsterSet->bUnknownMap = false;
    }

    _base_fld *record = g_Main.m_tblMonster.GetRecord(token5);
    if (!record)
    {
      sprintf(errBuffer, "Event Set Load Error : %s >> mon code error : %s", token0, token5);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    monsterSet->pMonsterFld = record;
    monsterSet->wNum = static_cast<unsigned __int16>(atoi(token6));
    if (!monsterSet->wNum || monsterSet->wNum > 100)
    {
      sprintf(errBuffer, "Event Set Load Error : %s >> mon num error : %d", token0, monsterSet->wNum);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    monsterSet->dwRegenTerm = 1000 * static_cast<unsigned int>(atol(token7));
    if (!monsterSet->dwRegenTerm)
    {
      sprintf(errBuffer, "Event Set Load Error : %s >> mon regen term error : %d", token0, monsterSet->dwRegenTerm);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    monsterSet->byRegenProb = static_cast<unsigned __int8>(atoi(token8));
    if (!monsterSet->byRegenProb || monsterSet->byRegenProb > 100)
    {
      sprintf(errBuffer, "Event Set Load Error : %s >> mon regen prob error : %d", token0, monsterSet->byRegenProb);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    monsterSet->dwDuring = static_cast<unsigned int>(atoi(token9));
    if (monsterSet->dwDuring > 7)
    {
      sprintf(errBuffer, "Event Set Load Error : %s >> event during error : %d", token0, monsterSet->dwDuring);
      g_Main.m_logLoadingError.Write(errBuffer);
      fclose(stream);
      return false;
    }

    monsterSet->dwDuring *= 86400000;
    monsterSet->bIsSet = true;
  }

  fclose(stream);
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
