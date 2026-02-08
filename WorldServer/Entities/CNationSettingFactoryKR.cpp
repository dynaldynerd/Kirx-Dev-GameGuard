#include "pch.h"

#include "CNationSettingFactoryKR.h"

#include "CNationSettingDataKR.h"

#include <new>

namespace
{
constexpr char kNationSettingKrNoneString[] = "None";
}

CNationSettingFactoryKR::CNationSettingFactoryKR()
  : CNationSettingFactory(410)
{
}

CNationSettingData *CNationSettingFactoryKR::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataKR *>(operator new(0x1C0uLL));
  if (data)
  {
    new (data) CNationSettingDataKR();
    created = data;
  }
  if (!created)
  {
    return nullptr;
  }

  created->m_bServiceMode = bServiceMode;
  created->m_iNationCode = iNationCode;
  strcpy_s(created->m_szNationCodeStr, sizeof(created->m_szNationCodeStr), szNationCodeStr);
  created->m_iANSICodePage = 949;
  if (created->m_bServiceMode)
  {
    strcpy_s(created->m_szCashDBIP, sizeof(created->m_szCashDBIP), "10.1.60.206");
    strcpy_s(created->m_szCashDBName, sizeof(created->m_szCashDBName), "pbill");
    strcpy_s(created->m_szCashDBID, sizeof(created->m_szCashDBID), "novus_cash");
    strcpy_s(created->m_szCashDBPW, sizeof(created->m_szCashDBPW), "qhdndi^gkfnalcl&");
    strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "novus_zone");
    strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "dhemfl@ehlf*Qjs#");
  }
  else
  {
    strcpy_s(created->m_szCashDBIP, sizeof(created->m_szCashDBIP), "192.168.1.185");
    strcpy_s(created->m_szCashDBName, sizeof(created->m_szCashDBName), "COIN_TEST");
    strcpy_s(created->m_szCashDBID, sizeof(created->m_szCashDBID), "coin_test");
    strcpy_s(created->m_szCashDBPW, sizeof(created->m_szCashDBPW), "rftest");
    strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "odinworld");
    strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "#dnsehd%qnwhr");
  }

  created->SetCashDBDSNSetFlag();
  if (!RegistCheatTable(created))
  {
    return nullptr;
  }

  strcpy_s(created->m_szNoneString, sizeof(created->m_szNoneString), kNationSettingKrNoneString);
  return created;
}
