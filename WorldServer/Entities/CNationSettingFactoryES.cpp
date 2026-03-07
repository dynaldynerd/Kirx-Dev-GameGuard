#include "pch.h"

#include "CNationSettingFactoryES.h"

#include "CNationSettingDataES.h"

#include <new>

CNationSettingFactoryES::CNationSettingFactoryES()
  : CNationSettingFactory(724)
{
}

CNationSettingData *CNationSettingFactoryES::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataES *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataES();
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
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "odinworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#dnsehd%qnwhr");
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
