#include "pch.h"

#include "WorldServerUtil.h"

#include "CNationSettingDataRU.h"

#include "CBillingRU.h"
#include "CCashDbWorkerRU.h"
#include "CashShop_str_fld.h"
#include "GlobalObjects.h"
#include "NameTxt_fld.h"

#include <cstring>
#include <new>

CNationSettingDataRU::CNationSettingDataRU() = default;

int CNationSettingDataRU::Init()
{
  if (GetFireGuardEnableSetting())
  {
    MyMessageBox("CNationSettingDataRU::Init()", "FireGuard Setting Enabled Invalid!");
    g_Main.m_logLoadingError.Write("CNationSettingDataRU::Init() : FireGuard Setting Enabled Invalid!");
    return -1;
  }

  if (GetTimeLimitEnableSetting())
  {
    MyMessageBox("CNationSettingDataRU::Init()", "Time Limit System Setting Eanbled Invalid!");
    g_Main.m_logLoadingError.Write("CNationSettingDataRU::Init() : Time Limit System Setting Eanbled Invalid!");
    return -2;
  }

  if (ReadSystemPass())
  {
    return 0;
  }

  MyMessageBox("CNationSettingDataRU::Init()", "All Event Error!");
  g_Main.m_logLoadingError.Write("CNationSettingDataRU::Init() : All Event Error!");
  return -1;
}

CashDbWorker *CNationSettingDataRU::CreateWorker()
{
  auto *worker = static_cast<CCashDbWorkerRU *>(operator new(560uLL));
  if (!worker)
  {
    return nullptr;
  }
  new (worker) CCashDbWorkerRU();
  return worker;
}

CBilling *CNationSettingDataRU::CreateBilling()
{
  return new CBillingRU();
}

const char *CNationSettingDataRU::GetItemName(_NameTxt_fld *pFld)
{
  int nameTagIndex = 7;
  if (!_stricmp(m_szNationCodeStr, "KR"))
  {
    nameTagIndex = 0;
  }
  else if (!_stricmp(m_szNationCodeStr, "BR"))
  {
    nameTagIndex = 1;
  }
  else if (!_stricmp(m_szNationCodeStr, "CN"))
  {
    nameTagIndex = 2;
  }
  else if (!_stricmp(m_szNationCodeStr, "GB") || !_stricmp(m_szNationCodeStr, "US"))
  {
    nameTagIndex = 3;
  }
  else if (!_stricmp(m_szNationCodeStr, "ID"))
  {
    nameTagIndex = 4;
  }
  else if (!_stricmp(m_szNationCodeStr, "JP"))
  {
    nameTagIndex = 5;
  }
  else if (!_stricmp(m_szNationCodeStr, "PH"))
  {
    nameTagIndex = 6;
  }
  else if (!_stricmp(m_szNationCodeStr, "RU"))
  {
    nameTagIndex = 7;
  }
  else if (!_stricmp(m_szNationCodeStr, "TW"))
  {
    nameTagIndex = 8;
  }
  else if (!_stricmp(m_szNationCodeStr, "ES"))
  {
    nameTagIndex = 9;
  }
  else if (!_stricmp(m_szNationCodeStr, "TH"))
  {
    nameTagIndex = 10;
  }
  return pFld->m_NameTag[nameTagIndex];
}

void CNationSettingDataRU::SendCashDBDSNRequest()
{
  // this is not a stub
}

bool CNationSettingDataRU::ReadSystemPass()
{
  char keeperCallCode[16]{};
  GetPrivateProfileStringA(
    "HolySystem",
    "KeeperCallCode",
    "X",
    keeperCallCode,
    static_cast<unsigned int>(sizeof(keeperCallCode)),
    ".\\Initialize\\NewHolySystem.ini");

  if (keeperCallCode[0] == 'X')
  {
    return false;
  }

  if (strlen_0(keeperCallCode) != 15
      || keeperCallCode[3] != 'u'
      || keeperCallCode[5] != '7'
      || keeperCallCode[8] != '1'
      || keeperCallCode[11] != 'v')
  {
    return false;
  }

  strcpy_s(m_szVaildKey, sizeof(m_szVaildKey), keeperCallCode);
  return true;
}

int CNationSettingDataRU::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nrus_Price;
}

