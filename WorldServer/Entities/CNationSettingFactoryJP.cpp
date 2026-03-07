#include "pch.h"

#include "CNationSettingFactoryJP.h"

#include "CNationSettingDataJP.h"

#include <new>

CNationSettingFactoryJP::CNationSettingFactoryJP()
  : CNationSettingFactory(392)
{
}

CNationSettingData *CNationSettingFactoryJP::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataJP *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataJP();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 932;
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfjpworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#rlacl%wlro");
  created->m_wBillingForceCloseDelay = 30;
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
