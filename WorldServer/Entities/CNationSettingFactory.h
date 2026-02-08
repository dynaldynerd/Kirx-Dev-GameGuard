#pragma once

#include "IdaCompat.h"

class CNationSettingData;

class __cppobj __declspec(align(8)) CNationSettingFactory
{
public:
  CNationSettingFactory(int iType);

  virtual CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) = 0;

  bool IsNULL();
  int GetKey();

  bool RegistCheatTable(CNationSettingData *pkData);
  bool RegistCheatTableUnion(CNationSettingData *pkData);
  bool RegistCheatTableOnlyInternal(CNationSettingData *pkData);
  void RegistCheatEndRecord(CNationSettingData *pkData);
  bool RegistCheat(CNationSettingData *pkData, const char *szCheat, int nType, int nUse, int nMgr);
  bool IsExistCheat(const char *szCheat, CNationSettingData *pkData);

  int m_iType;
};
