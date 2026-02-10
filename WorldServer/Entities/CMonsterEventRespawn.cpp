#include "pch.h"

#include "CMonsterEventRespawn.h"

#include <cstdio>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>

#include "CMapOperation.h"
#include "CMapData.h"
#include "CLogFile.h"
#include "CMonster.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CMonsterEventRespawn g_MonsterEventRespawn;

namespace
{
  static bool s_iniExtLenInit = false;
  static size_t s_iniExtLen = 0;
  const char *pos_key[3] = {"x", "y", "z"};
}

void _event_respawn::_state::init()
{
  memset_0(this, 0, sizeof(*this));
}

bool CMonsterEventRespawn::StartRespawnEvent(char *pszEventCode, char *pwszErrCode)
{
  _event_respawn *eventRespawn = nullptr;
  for (int j = 0; j < m_nLoadEventRespawn; ++j)
  {
    _event_respawn *candidate = &m_EventRespawn[j];
    if (candidate->bLoad && !strcmp_0(candidate->szScriptName, pszEventCode))
    {
      eventRespawn = candidate;
      break;
    }
  }

  if (!eventRespawn)
  {
    return false;
  }

  if (eventRespawn->bActive)
  {
    if (pwszErrCode)
    {
      sprintf(pwszErrCode, "now actived");
    }
    return false;
  }

  eventRespawn->State.init();
  int respawnCount = 0;
  for (int k = 0; k < eventRespawn->wMonSetNum; ++k)
  {
    _event_respawn::_mon *monSet = &eventRespawn->MonSet[k];
    for (int m = 0; m < monSet->wNum; ++m)
    {
      CMonster *monster = CreateRepMonster(
        eventRespawn->pMap,
        0,
        eventRespawn->fPos,
        monSet->pMonsterFld->m_strCode,
        nullptr,
        eventRespawn->Option.bExpPenalty,
        eventRespawn->Option.bExpReward,
        0,
        0,
        0);
      if (monster)
      {
        eventRespawn->State.MonInfo[respawnCount].pMon = monster;
        eventRespawn->State.MonInfo[respawnCount].dwSerial = monster->m_dwObjSerial;
        eventRespawn->State.MonInfo[respawnCount].pMonFld = monSet->pMonsterFld;
        ++respawnCount;

        if (!eventRespawn->Option.bItemLoot)
        {
          monster->DisableStdItemLoot();
        }
        monster->LinkEventRespawn(eventRespawn);
      }
    }
  }

  eventRespawn->State.nRespawnNum = respawnCount;
  eventRespawn->State.dwLastUpdateTime = timeGetTime();
  eventRespawn->bActive = true;
  return true;
}

bool CMonsterEventRespawn::StopRespawnEvent(char *pszEventCode, char *pwszErrCode)
{
  _event_respawn *eventRespawn = nullptr;
  for (int j = 0; j < m_nLoadEventRespawn; ++j)
  {
    _event_respawn *candidate = &m_EventRespawn[j];
    if (candidate->bLoad && !strcmp_0(candidate->szScriptName, pszEventCode))
    {
      eventRespawn = candidate;
      break;
    }
  }

  if (!eventRespawn)
  {
    return false;
  }

  if (!eventRespawn->bActive)
  {
    if (pwszErrCode)
    {
      sprintf(pwszErrCode, "now stoped");
    }
    return false;
  }

  if (eventRespawn->Option.bKillAfterStop)
  {
    for (int k = 0; k < eventRespawn->State.nRespawnNum; ++k)
    {
      _event_respawn::_state::_mon *info = &eventRespawn->State.MonInfo[k];
      if (info->pMon && info->pMon->m_bLive && info->pMon->m_dwObjSerial == info->dwSerial)
      {
        info->pMon->Destroy(1u, nullptr);
      }
    }
  }

  eventRespawn->bActive = false;
  return true;
}

bool CMonsterEventRespawn::SetEventRespawn()
{
  if (!s_iniExtLenInit)
  {
    s_iniExtLenInit = true;
    s_iniExtLen = strlen_0(".ini");
  }

  int fileCount = 0;
  char fileBaseNames[2080]{};
  WIN32_FIND_DATAA findData{};
  HANDLE hFindFile = FindFirstFileA(".\\EventRespawn\\*.ini", &findData);
  if (hFindFile != INVALID_HANDLE_VALUE)
  {
    do
    {
      const int fileNameLen = static_cast<int>(strlen_0(findData.cFileName));
      const int baseNameLen = fileNameLen - static_cast<int>(s_iniExtLen);
      strncpy(&fileBaseNames[64 * fileCount], findData.cFileName, baseNameLen);
      fileBaseNames[64 * fileCount + baseNameLen] = '\0';
      ++fileCount;
    } while (fileCount < 32 && FindNextFileA(hFindFile, &findData));
    FindClose(hFindFile);
  }

  int loadedCount = 0;
  for (int fileIndex = 0; fileIndex < fileCount; ++fileIndex)
  {
    char buffer[136]{};
    sprintf(buffer, ".\\EventRespawn\\%s.ini", &fileBaseNames[64 * fileIndex]);

    _event_respawn *eventRespawn = &m_EventRespawn[loadedCount];
    strcpy_0(eventRespawn->szScriptName, &fileBaseNames[64 * fileIndex]);

    const int monSetNum = GetPrivateProfileIntA("MONSTER", "set", -1, buffer);
    if (monSetNum == -1)
    {
      g_Main.m_logLoadingError.Write(
        "Monster Respawn Load Error : %s >> mon set num error",
        &fileBaseNames[64 * fileIndex]);
      return false;
    }
    if (monSetNum > 640)
    {
      g_Main.m_logLoadingError.Write(
        "Monster Respawn Load Error : %s >> mon set num error : %d > %d",
        &fileBaseNames[64 * fileIndex],
        monSetNum,
        640);
      return false;
    }

    eventRespawn->wMonSetNum = static_cast<unsigned __int16>(monSetNum);
    int totalMonNum = 0;
    for (int k = 0; k < monSetNum; ++k)
    {
      char keyName[132]{};
      char returnedString[72]{};
      sprintf(keyName, "code%d", k);
      GetPrivateProfileStringA("MONSTER", keyName, "X", returnedString, 0x40u, buffer);
      if (!strcmp_0(returnedString, "X"))
      {
        g_Main.m_logLoadingError.Write(
          "Monster Respawn Load Error : %s >> mon code error : %d) %s",
          &fileBaseNames[64 * fileIndex],
          k,
          returnedString);
        return false;
      }

      _base_fld *record = g_Main.m_tblMonster.GetRecord(returnedString);
      if (!record)
      {
        g_Main.m_logLoadingError.Write(
          "Monster Respawn Load Error : %s >> mon code error : %d) %s",
          &fileBaseNames[64 * fileIndex],
          k,
          returnedString);
        return false;
      }

      eventRespawn->MonSet[k].pMonsterFld = record;

      sprintf(keyName, "num%d", k);
      const unsigned int num = GetPrivateProfileIntA("MONSTER", keyName, -1, buffer);
      if (num == static_cast<unsigned int>(-1))
      {
        g_Main.m_logLoadingError.Write(
          "Monster Respawn Load Error : %s >> mon num error : %d) %s num: %d",
          &fileBaseNames[64 * fileIndex],
          k,
          returnedString,
          -1);
        return false;
      }

      eventRespawn->MonSet[k].wNum = static_cast<unsigned __int16>(num);
      totalMonNum += static_cast<int>(num);
    }

    if (totalMonNum > 640)
    {
      g_Main.m_logLoadingError.Write(
        "Monster Respawn Load Error : %s >> total mon num error : %d > %d",
        &fileBaseNames[64 * fileIndex],
        totalMonNum,
        640);
      return false;
    }

    char mapCode[136]{};
    GetPrivateProfileStringA("POSITION", "map", "X", mapCode, 0x80u, buffer);
    CMapData *mapData = g_MapOper.GetMap(mapCode);
    if (!mapData)
    {
      g_Main.m_logLoadingError.Write(
        "Monster Respawn Load Error : %s >> map code error : %s",
        &fileBaseNames[64 * fileIndex],
        mapCode);
      return false;
    }
    eventRespawn->pMap = mapData;

    for (int m = 0; m < 3; ++m)
    {
      const int pos = GetPrivateProfileIntA("POSITION", pos_key[m], 0xFFFF, buffer);
      if (pos == 0xFFFF)
      {
        g_Main.m_logLoadingError.Write(
          "Monster Respawn Load Error : %s >> xyz error : %d",
          &fileBaseNames[64 * fileIndex],
          0xFFFF);
        return false;
      }
      eventRespawn->fPos[m] = static_cast<float>(pos);
    }

    if (!mapData->IsMapIn(eventRespawn->fPos))
    {
      g_Main.m_logLoadingError.Write(
        "Monster Respawn Load Error : %s >> xyz range error : %d %d %d",
        &fileBaseNames[64 * fileIndex],
        static_cast<int>(eventRespawn->fPos[0]),
        static_cast<int>(eventRespawn->fPos[1]),
        static_cast<int>(eventRespawn->fPos[2]));
      return false;
    }

    const unsigned int termHour = GetPrivateProfileIntA("TERM", "hour", 0, buffer);
    const unsigned int termMin = GetPrivateProfileIntA("TERM", "min", 0, buffer);
    const unsigned int termSec = GetPrivateProfileIntA("TERM", "sec", 0, buffer);
    eventRespawn->dwTermMSec = 1000 * (3600 * termHour + termSec + 60 * termMin);

    const unsigned int optKillAfterStop = GetPrivateProfileIntA("OPTION", "kill after stop", 0, buffer);
    const unsigned int optExpPenalty = GetPrivateProfileIntA("OPTION", "exp penalty", 0, buffer);
    const unsigned int optExpReward = GetPrivateProfileIntA("OPTION", "exp reward", 1, buffer);
    const unsigned int optItemLoot = GetPrivateProfileIntA("OPTION", "item loot", 1, buffer);

    eventRespawn->nUseRewardItemNum = 0;
    for (int n = 0; n < 128; ++n)
    {
      char keyName[132]{};
      char itemCode[132]{};
      sprintf(keyName, "item code %d", n + 1);
      GetPrivateProfileStringA("REWARD ITEM", keyName, "X", itemCode, 0x80u, buffer);
      if (itemCode[0] != 'X')
      {
        const unsigned __int8 itemTableCode = GetItemTableCode(itemCode);
        if (itemTableCode == 0xFF)
        {
          g_Main.m_logLoadingError.Write(
            "Monster Respawn Load Error : %s >> reward item : item code error : %s",
            &fileBaseNames[64 * fileIndex],
            itemCode);
          return false;
        }

        _base_fld *itemRecord = g_Main.m_tblItemData[itemTableCode].GetRecordByHash(itemCode, 2, 5);
        if (!itemRecord)
        {
          g_Main.m_logLoadingError.Write(
            "Monster Respawn Load Error : %s >> reward item : item code error : %s",
            &fileBaseNames[64 * fileIndex],
            itemCode);
          return false;
        }

        char monCode[136]{};
        sprintf(keyName, "monster %d", n + 1);
        GetPrivateProfileStringA("REWARD ITEM", keyName, "ALL", monCode, 0x80u, buffer);
        _base_fld *dstMonRecord = nullptr;
        if (strcmp_0(monCode, "ALL"))
        {
          dstMonRecord = g_Main.m_tblMonster.GetRecord(monCode);
          if (!dstMonRecord)
          {
            g_Main.m_logLoadingError.Write(
              "Monster Respawn Load Error : %s >> reward item : monster code error : %s",
              &fileBaseNames[64 * fileIndex],
              monCode);
            return false;
          }

          bool matchFound = false;
          for (int ii = 0; ii < monSetNum; ++ii)
          {
            if (eventRespawn->MonSet[ii].pMonsterFld == dstMonRecord)
            {
              matchFound = true;
              break;
            }
          }
          if (!matchFound)
          {
            g_Main.m_logLoadingError.Write(
              "Monster Respawn Load Error : %s >> reward item : monster code match error : %s",
              &fileBaseNames[64 * fileIndex],
              monCode);
            return false;
          }
        }

        sprintf(keyName, "item %% %d", n + 1);
        const unsigned int prob = GetPrivateProfileIntA("REWARD ITEM", keyName, 0, buffer);

        _event_respawn::_reward_item *rewardItem = &eventRespawn->RewardItem[eventRespawn->nUseRewardItemNum];
        rewardItem->byItemTableCode = itemTableCode;
        rewardItem->pItemFld = itemRecord;
        rewardItem->pDstMonFld = reinterpret_cast<_monster_fld *>(dstMonRecord);
        rewardItem->nProb = static_cast<int>(prob);
        ++eventRespawn->nUseRewardItemNum;
      }
    }

    eventRespawn->Option.bKillAfterStop = optKillAfterStop != 0;
    eventRespawn->Option.bExpPenalty = optExpPenalty != 0;
    eventRespawn->Option.bExpReward = optExpReward != 0;
    eventRespawn->Option.bItemLoot = optItemLoot != 0;
    eventRespawn->bLoad = true;
    ++loadedCount;
    g_Main.m_logLoadingError.Write("Monster Respawn Load >> %s", &fileBaseNames[64 * fileIndex]);
  }

  m_nLoadEventRespawn = loadedCount;
  return true;
}
