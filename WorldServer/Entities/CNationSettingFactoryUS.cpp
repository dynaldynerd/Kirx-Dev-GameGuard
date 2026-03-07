#include "pch.h"

#include "CNationSettingFactoryUS.h"

#include "CNationSettingDataUS.h"

#include <new>

CNationSettingFactoryUS::CNationSettingFactoryUS()
  : CNationSettingFactory(840)
{
}

CNationSettingData *CNationSettingFactoryUS::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataUS *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataUS();
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
