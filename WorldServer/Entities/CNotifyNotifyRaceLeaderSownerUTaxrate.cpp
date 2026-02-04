#include "pch.h"

#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"

#include "CNationSettingManager.h"

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
