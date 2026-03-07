#include "pch.h"

#include "CNationSettingFactoryCN.h"

#include "CNationSettingDataCN.h"

#include <new>

CNationSettingFactoryCN::CNationSettingFactoryCN()
  : CNationSettingFactory(156)
{
}

CNationSettingData *CNationSettingFactoryCN::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataCN *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataCN();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 936;
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfchinaworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), ")*ckdlskdnjfem@(");
  created->SetCahsDBUseExtRefFlag();

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
