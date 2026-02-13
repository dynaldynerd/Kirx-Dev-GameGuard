#include "pch.h"

#include "CNationSettingDataRU.h"

#include "CBillingRU.h"
#include "CCashDbWorkerRU.h"
#include "CashShop_str_fld.h"
#include "GlobalObjects.h"
#include "NameTxt_fld.h"

#include <new>

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
  auto *worker = static_cast<CCashDbWorkerRU *>(operator new(0x230uLL));
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
  return pFld->m_NameTag[7];
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

