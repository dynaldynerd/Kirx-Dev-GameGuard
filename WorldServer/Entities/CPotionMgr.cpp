#include "pch.h"

#include "CPotionMgr.h"

#include "CNationSettingManager.h"
#include "CheckPotion_fld.h"
#include "NameTxt_fld.h"
#include "skill_fld.h"

CPotionMgr g_PotionMgr;

bool CPotionMgr::DatafileInit()
{
    char pszErrMsg[144]; // [rsp+30h] [rbp-A8h] BYREF

    if (m_tblPotionEffectData.ReadRecord(".\\script\\PotionItemEffect.dat", 0x490u, pszErrMsg))
    {
        if (m_tblPotionCheckData.ReadRecord(".\\script\\CheckPotionEffect.dat", 0xDCu, pszErrMsg))
        {
            return true;
        }
        else
        {
            MyMessageBox("DatafileInit", pszErrMsg);
            return false;
        }
    }
    else
    {
        MyMessageBox("DatafileInit", pszErrMsg);
        return false;
    }
}

bool CPotionMgr::SetPotionDataName()
{
  if (!CRecordData::IsTableOpen(&m_tblPotionEffectData) || !CRecordData::IsTableOpen(&m_tblPotionCheckData))
  {
    MyMessageBox("CPotionMgr::SetPotionDataName()", "PotionItemEffectData Or CheckPotionEffectData Not Load!");
    return false;
  }

  char errMsg[160]{};
  CRecordData potionEffectNames;
  if (!potionEffectNames.ReadRecord(".\\script\\PotionItemEffect_str.dat", 0x304, errMsg))
  {
    MyMessageBox("CPotionMgr::SetPotionDataName()", "PotionItemEffect Name Data Load Error, ErrorMsg(%s)", errMsg);
    return false;
  }

  int recordNum = static_cast<int>(m_tblPotionEffectData.GetRecordNum());
  for (int n = 0; n < recordNum; ++n)
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(m_tblPotionEffectData.GetRecord(n));
    if (!record)
    {
      MyMessageBox("CPotionMgr::SetPotionDataName()", "Get Potion Effect Data Error, PotionEffectIndex(%d)", n);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(potionEffectNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "CPotionMgr::SetPotionDataName()",
        "Get Potion Effect Name Data Error, PotionEffectCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strKorName, 0x40, name);
  }

  CRecordData checkEffectNames;
  if (!checkEffectNames.ReadRecord(".\\script\\CheckPotionEffect_str.dat", 0x304, errMsg))
  {
    MyMessageBox(
      "CPotionMgr::SetPotionDataName()",
      "CheckPotionEffect Name Data Load Error, ErrorMsg(%s)",
      errMsg);
    return false;
  }

  recordNum = static_cast<int>(m_tblPotionCheckData.GetRecordNum());
  for (int n = 0; n < recordNum; ++n)
  {
    _CheckPotion_fld *record = reinterpret_cast<_CheckPotion_fld *>(m_tblPotionCheckData.GetRecord(n));
    if (!record)
    {
      MyMessageBox(
        "CPotionMgr::SetPotionDataName()",
        "Get Potion Effect Check Data Error, PotionEffectCheckIndex(%d)",
        n);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(potionEffectNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "CPotionMgr::SetPotionDataName()",
        "Get Potion Effect Check Name Data Error, PotionEffectCheckCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strKOR_name, 0x40, name);
  }

  return true;
}
