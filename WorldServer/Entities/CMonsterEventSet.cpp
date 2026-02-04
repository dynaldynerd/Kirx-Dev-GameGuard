#include "pch.h"

#include "CMonsterEventSet.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CLogFile.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CMonsterEventSet g_MonsterEventSet;

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
