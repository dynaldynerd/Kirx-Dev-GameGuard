#include "pch.h"

#include "CNationSettingData.h"

#include "CPlayer.h"
#include "CUserDB.h"
#include "NameTxt_fld.h"

CBilling *CNationSettingData::CreateBilling()
{
  return nullptr;
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
