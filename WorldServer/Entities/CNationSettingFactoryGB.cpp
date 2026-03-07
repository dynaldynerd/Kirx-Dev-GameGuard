#include "pch.h"

#include "CNationSettingFactoryGB.h"

#include "CNationSettingDataGB.h"

#include <new>

CNationSettingFactoryGB::CNationSettingFactoryGB()
  : CNationSettingFactory(826)
{
}

CNationSettingData *CNationSettingFactoryGB::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingData *created = nullptr;
  auto *data = static_cast<CNationSettingDataGB *>(operator new(448));
  if (data)
  {
    new (data) CNationSettingDataGB();
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
  strcpy_s(created->m_szWorldDBID, sizeof(created->m_szWorldDBID), "rfbrtworld");
  strcpy_s(created->m_szWorldDBPW, sizeof(created->m_szWorldDBPW), "rfbrtworld");

  if (RegistCheatTable(created))
  {
    return created;
  }
  return nullptr;
}
