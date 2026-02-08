#include "pch.h"

#include "CNationSettingFactoryBR.h"

#include "CNationSettingDataBR.h"

#include <new>

CNationSettingFactoryBR::CNationSettingFactoryBR()
  : CNationSettingFactory(76)
{
}

CNationSettingData *CNationSettingFactoryBR::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataBR *>(operator new(0x1C0uLL));
  if (data)
  {
    new (data) CNationSettingDataBR();
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
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfbraworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#rf%braworld");
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
