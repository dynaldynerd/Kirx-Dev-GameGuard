#include "pch.h"

#include "CNationSettingData.h"

#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "NameTxt_fld.h"
#include "wrac_packets.h"

CBilling *CNationSettingData::CreateBilling()
{
  return nullptr;
}

CashDbWorker *CNationSettingData::CreateWorker()
{
  // this is not a stub
  return nullptr;
}

int CNationSettingData::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  (void)pFld;
  // this is not a stub
  return 0;
}

void CNationSettingData::SendCashDBDSNRequest()
{
  _cashdb_setting_request_wrac msg{};
  unsigned __int8 type[2] = {1, 31};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&msg), len);
}

void CNationSettingData::NetClose(CPlayer *pOne)
{
  (void)pOne;
  // this is not a stub
}

const char *CNationSettingData::GetItemName(_NameTxt_fld *pFld)
{
  if (pFld == nullptr)
  {
    return "";
  }
  return pFld->m_NameTag[0];
}

bool CNationSettingData::IsApplyPcbangPrimium(const CPlayer *pUser)
{
  if (!pUser)
  {
    return false;
  }
  return pUser->m_pUserDB && pUser->m_pUserDB->m_BillingInfo.IsPcBangType();
}

const char *CNationSettingData::GetNoneString()
{
  return m_szNoneString;
}

bool CNationSettingData::IsCashDBUseExtRef()
{
  return m_eCashDBFlag == CDPS_USE_REF;
}

bool CNationSettingData::IsCashDBInit()
{
  return m_eCashDBFlag == CDPS_INIT;
}

bool CNationSettingData::IsCashDBDSNSetted()
{
  return m_eCashDBFlag == CDPS_SET;
}

INationGameGuardSystem *CNationSettingData::GetGameGuardSystem()
{
  return m_pGameGuardSystem;
}

void CNationSettingData::SetCashDBInitFlag()
{
  m_eCashDBFlag = CDPS_INIT;
}
