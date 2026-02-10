#include "pch.h"

#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"

#include "CGuild.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CNationSettingManager.h"
#include "GlobalObjects.h"

CNotifyNotifyRaceLeaderSownerUTaxrate::CNotifyNotifyRaceLeaderSownerUTaxrate()
{
  // this is not a stub
}

void CNotifyNotifyRaceLeaderSownerUTaxrate::Init()
{
  for (int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    for (int leaderIndex = 0; leaderIndex < 9; ++leaderIndex)
    {
      auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
      const char *noneString = nationSetting->GetNoneString();
      strcpy_s(m_Send[raceIndex].wszRaceLeaderName[leaderIndex], 0x11u, noneString);
    }

    auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    const char *noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[raceIndex].wszSettlement1OwnerGuildName, 0x11u, noneString);

    nationSetting = CTSingleton<CNationSettingManager>::Instance();
    noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[raceIndex].wszSettlement1OwnerGuildMasterName, 0x11u, noneString);

    nationSetting = CTSingleton<CNationSettingManager>::Instance();
    noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[raceIndex].wszSettlement2OwnerGuildName, 0x11u, noneString);

    nationSetting = CTSingleton<CNationSettingManager>::Instance();
    noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[raceIndex].wszSettlement2OwnerGuildMasterName, 0x11u, noneString);

    m_Send[raceIndex].byTaxRate = 5;
  }
}

void CNotifyNotifyRaceLeaderSownerUTaxrate::UpdateTaxRate(unsigned __int8 byRace, unsigned __int8 byTaxRate)
{
  if (byRace < 3u)
  {
    m_Send[byRace].byTaxRate = byTaxRate;
  }
}

void CNotifyNotifyRaceLeaderSownerUTaxrate::UpdateRaceLeader(
  unsigned __int8 byRace,
  unsigned __int8 byNth,
  char *wszLeaderName)
{
  const char *source = nullptr;
  if (*wszLeaderName)
  {
    source = wszLeaderName;
  }
  else
  {
    auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    source = nationSetting->GetNoneString();
  }

  strcpy_s(m_Send[byRace].wszRaceLeaderName[byNth], 0x11u, source);
}

void CNotifyNotifyRaceLeaderSownerUTaxrate::UpdateSettlementOwner(
  unsigned __int8 byRace,
  unsigned int dw1ThGuildSerial,
  unsigned int dw2ThGuildSerial)
{
  if (byRace >= 3u)
  {
    return;
  }

  CGuild *guild = GetGuildDataFromSerial(g_Guild, 500, dw1ThGuildSerial);
  if (guild)
  {
    strcpy_s(m_Send[byRace].wszSettlement1OwnerGuildName, 0x11u, guild->m_wszName);
    const char *source = nullptr;
    if (guild->m_MasterData.pMember)
    {
      source = guild->m_MasterData.pMember->wszName;
    }
    else
    {
      auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
      source = nationSetting->GetNoneString();
    }
    strcpy_s(m_Send[byRace].wszSettlement1OwnerGuildMasterName, 0x11u, source);
  }
  else
  {
    auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    const char *noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[byRace].wszSettlement1OwnerGuildName, 0x11u, noneString);
    nationSetting = CTSingleton<CNationSettingManager>::Instance();
    noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[byRace].wszSettlement1OwnerGuildMasterName, 0x11u, noneString);
  }

  guild = GetGuildDataFromSerial(g_Guild, 500, dw2ThGuildSerial);
  if (guild)
  {
    strcpy_s(m_Send[byRace].wszSettlement2OwnerGuildName, 0x11u, guild->m_wszName);
    const char *source = nullptr;
    if (guild->m_MasterData.pMember)
    {
      source = guild->m_MasterData.pMember->wszName;
    }
    else
    {
      auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
      source = nationSetting->GetNoneString();
    }
    strcpy_s(m_Send[byRace].wszSettlement2OwnerGuildMasterName, 0x11u, source);
  }
  else
  {
    auto *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    const char *noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[byRace].wszSettlement2OwnerGuildName, 0x11u, noneString);
    nationSetting = CTSingleton<CNationSettingManager>::Instance();
    noneString = nationSetting->GetNoneString();
    strcpy_s(m_Send[byRace].wszSettlement2OwnerGuildMasterName, 0x11u, noneString);
  }
}

void CNotifyNotifyRaceLeaderSownerUTaxrate::Notify(unsigned __int8 byRace)
{
  unsigned __int8 pbyType[20]{};
  pbyType[0] = 13;
  pbyType[1] = 101;

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (!player->m_bLive)
    {
      continue;
    }

    if (byRace < 3u && static_cast<unsigned __int8>(player->m_Param.GetRaceCode()) != byRace)
    {
      continue;
    }

    const unsigned __int8 raceCode = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      pbyType,
      reinterpret_cast<char *>(&m_Send[raceCode]),
      0xDEu);
  }
}

void CNotifyNotifyRaceLeaderSownerUTaxrate::Notify(unsigned __int8 byRace, unsigned __int16 wIndex)
{
  if (byRace >= 3u || wIndex >= MAX_PLAYER)
  {
    return;
  }

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 13;
  pbyType[1] = 101;

  g_Network.m_pProcess[0]->LoadSendMsg(
    wIndex,
    pbyType,
    reinterpret_cast<char *>(&m_Send[byRace]),
    0xDEu);
}
