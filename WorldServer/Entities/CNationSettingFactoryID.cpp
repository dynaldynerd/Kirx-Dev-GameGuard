#include "pch.h"

#include "CNationSettingFactoryID.h"

#include "CNationSettingDataID.h"

#include <new>

CNationSettingFactoryID::CNationSettingFactoryID()
  : CNationSettingFactory(360)
{
}

CNationSettingData *CNationSettingFactoryID::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataID *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataID();
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
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfindoworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#rf%wjfeo*anwjr");
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
