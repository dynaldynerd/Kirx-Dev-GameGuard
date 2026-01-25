#include "pch.h"

#include "CNationSettingData.h"

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
