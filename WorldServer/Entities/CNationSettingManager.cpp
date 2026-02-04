#include "pch.h"

#include "CNationSettingManager.h"

#include "CNationSettingData.h"
#include "NameTxt_fld.h"

int CNationSettingManager::Init(int nationCode, const char *nationCodeStr, bool serviceMode)
{
  (void)nationCode;
  (void)nationCodeStr;
  (void)serviceMode;
  return 0;
}

CBilling *CNationSettingManager::CreateBilling()
{
  if (m_pData == nullptr)
  {
    return nullptr;
  }
  return m_pData->CreateBilling();
}

const char *CNationSettingManager::GetItemName(_NameTxt_fld *pFld)
{
  if (pFld == nullptr)
  {
    return "";
  }
  if (m_pData == nullptr)
  {
    return pFld->m_NameTag[0];
  }
  return m_pData->GetItemName(pFld);
}

const char *CNationSettingManager::GetNoneString()
{
  if (m_pData == nullptr)
  {
    return "";
  }
  return m_pData->GetNoneString();
}

bool CNationSettingManager::IsApplyPcbangPrimium(const CPlayer *pUser)
{
  if (!m_pData)
  {
    return false;
  }
  return m_pData->IsApplyPcbangPrimium(pUser);
}
