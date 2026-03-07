#include "pch.h"

#include "CNationSettingFactoryTH.h"

#include "CNationSettingDataTH.h"

#include <new>

CNationSettingFactoryTH::CNationSettingFactoryTH()
  : CNationSettingFactory(764)
{
}

CNationSettingData *CNationSettingFactoryTH::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataTH *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataTH();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 874;
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfthaiworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#sh%anwjr*xornr");
  created->m_wBillingForceCloseDelay = 30;
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
