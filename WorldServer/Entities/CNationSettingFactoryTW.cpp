#include "pch.h"

#include "CNationSettingFactoryTW.h"

#include "CNationSettingDataTW.h"

#include <new>

CNationSettingFactoryTW::CNationSettingFactoryTW()
  : CNationSettingFactory(158)
{
}

CNationSettingData *CNationSettingFactoryTW::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataTW *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataTW();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 950;
  if (created->m_bServiceMode)
  {
    strcpy_s(created->m_szCashDBIP, sizeof(created->m_szCashDBIP), "192.168.4.16");
    strcpy_s(created->m_szCashDBName, sizeof(created->m_szCashDBName), "RF_DB_CHG");
    strcpy_s(created->m_szCashDBID, sizeof(created->m_szCashDBID), "ccr");
    strcpy_s(created->m_szCashDBPW, sizeof(created->m_szCashDBPW), "ji3g4zo4j4");
  }
  else
  {
    strcpy_s(created->m_szCashDBIP, sizeof(created->m_szCashDBIP), "192.168.1.185");
    strcpy_s(created->m_szCashDBName, sizeof(created->m_szCashDBName), "COIN_TEST");
    strcpy_s(created->m_szCashDBID, sizeof(created->m_szCashDBID), "coin_test");
    strcpy_s(created->m_szCashDBPW, sizeof(created->m_szCashDBPW), "rftest");
  }
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "e1eoaks*dnjfem");

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
