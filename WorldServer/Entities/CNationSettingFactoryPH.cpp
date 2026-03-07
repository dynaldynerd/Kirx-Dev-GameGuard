#include "pch.h"

#include "CNationSettingFactoryPH.h"

#include "CNationSettingDataPH.h"

#include <new>

CNationSettingFactoryPH::CNationSettingFactoryPH()
  : CNationSettingFactory(608)
{
}

CNationSettingData *CNationSettingFactoryPH::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataPH *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataPH();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 1252;
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfppworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#rf%vlfflvls*eoqkr");
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
