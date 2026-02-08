#include "pch.h"

#include "CNationSettingFactoryRU.h"

#include "CNationSettingDataRU.h"

#include <new>

CNationSettingFactoryRU::CNationSettingFactoryRU()
  : CNationSettingFactory(643)
{
}

CNationSettingData *CNationSettingFactoryRU::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataRU *>(operator new(0x1C0uLL));
  if (data)
  {
    new (data) CNationSettingDataRU();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 1251;
  created->SetCashDBDSNSetFlag();
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfrusworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#rf%rusworld");

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
