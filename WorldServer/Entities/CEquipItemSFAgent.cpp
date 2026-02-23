#include "pch.h"

#include "CEquipItemSFAgent.h"

#include "CRecordData.h"
#include "GlobalObjects.h"
#include "base_fld.h"
#include "CPlayer.h"
#include "CloakItem_fld.h"

CEquipItemSFAgent::CEquipItemSFAgent()
{
  m_pMaster = nullptr;
  AllEndContSF();
}

CEquipItemSFAgent::~CEquipItemSFAgent()
{
  // this is not a stub
}

_sf_continous *CEquipItemSFAgent::GetEquipSFCont(unsigned int nEquipTblIndex)
{
  if (nEquipTblIndex >= 8)
  {
    return nullptr;
  }
  return m_pContSF[nEquipTblIndex];
}

_STORAGE_LIST::_db_con *CEquipItemSFAgent::GetEquip(unsigned int nEquipTblIndex)
{
  if (!m_pMaster)
  {
    return nullptr;
  }

  _STORAGE_LIST::_db_con *equip = nullptr;
  if (nEquipTblIndex < 8)
  {
    equip = &m_pMaster->m_Param.m_dbEquip.m_pStorageList[nEquipTblIndex];
  }

  if (!equip)
  {
    return nullptr;
  }

  return equip->m_bLoad ? equip : nullptr;
}

bool CEquipItemSFAgent::IsUseBooster()
{
  return GetEquipSFCont(7) != nullptr;
}

float CEquipItemSFAgent::GetBoosterAddSpeed()
{
  _sf_continous *equipCont = GetEquipSFCont(7);
  if (!equipCont || equipCont->m_byEffectCode)
  {
    return 0.0f;
  }

  _base_fld *record = g_Main.m_tblEffectData[0].GetRecord(equipCont->m_wEffectIndex);
  if (!record)
  {
    return 0.0f;
  }

  _STORAGE_LIST::_db_con *equip = GetEquip(7);
  if (!equip)
  {
    return 0.0f;
  }

  _CloakItem_fld *itemRecord = reinterpret_cast<_CloakItem_fld *>(g_Main.m_tblItemData[7].GetRecord(equip->m_wItemIndex));
  if (!itemRecord)
  {
    return 0.0f;
  }

  return itemRecord->m_fBoosterSpeed;
}

void CEquipItemSFAgent::AllEndContSF()
{
  for (int j = 0; j < 8; ++j)
  {
    this->m_pContSF[j] = nullptr;
  }
}

void CEquipItemSFAgent::Init(CPlayer *pMaster)
{
  if (pMaster)
  {
    this->m_pMaster = pMaster;
    this->AllEndContSF();
  }
}

void CEquipItemSFAgent::ReleaseSFCont(int nEquipTblIndex)
{
  if (!m_pMaster)
  {
    return;
  }

  _sf_continous *equipCont = GetEquipSFCont(static_cast<unsigned int>(nEquipTblIndex));
  if (!equipCont)
  {
    return;
  }

  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 8; ++k)
    {
      _sf_continous *cont = &m_pMaster->m_SFCont[j][k];
      if (cont->m_bExist && equipCont == cont)
      {
        m_pMaster->RemoveSFContEffect(static_cast<unsigned __int8>(j), static_cast<unsigned __int16>(k), false, false);
        return;
      }
    }
  }
}

CEquipItemSFAgent::_requireSlot::_requireSlot()
{
  this->Init();
}

void CEquipItemSFAgent::_requireSlot::Init()
{
  memset_0(this, 0, sizeof(CEquipItemSFAgent::_requireSlot));
}

char CEquipItemSFAgent::GetRequireSFSlot(_requireSlot *pSlot, _skill_fld *pSkillFld)
{
  if (!pSlot || !pSkillFld)
  {
    return 0;
  }

  if (pSkillFld->m_strFixWeapon[12] == '1')
  {
    pSlot->m_SlotIndex[7] = true;
  }

  return 1;
}

unsigned __int8 CEquipItemSFAgent::IsEnableSkill(_skill_fld *pSkill)
{
  if (!pSkill)
  {
    return 0;
  }

  if (pSkill->m_strFixWeapon[12] == '1')
  {
    return IsEnableSkill(7, pSkill);
  }

  return 0;
}

unsigned __int8 CEquipItemSFAgent::IsEnableSkill(int nEquipTblIndex, _skill_fld *pSkill)
{
  (void)pSkill;

  if (!GetEquip(static_cast<unsigned int>(nEquipTblIndex)))
  {
    return 22;
  }

  if (nEquipTblIndex == 7 && m_pMaster && m_pMaster->m_byModeType)
  {
    return 13;
  }

  return 0;
}

void CEquipItemSFAgent::SetSFCont(unsigned int nEquipTblIndex, _sf_continous *pSF)
{
  if (nEquipTblIndex >= 8)
  {
    return;
  }

  _sf_continous *previous = this->m_pContSF[nEquipTblIndex];
  this->m_pContSF[nEquipTblIndex] = pSF;

  if (nEquipTblIndex != 7)
  {
    return;
  }

  if (pSF)
  {
    this->m_pMaster->m_byMoveType = 2;
    this->m_pMaster->SenseState();
    this->m_pMaster->UpdateVisualVer(CPlayer::CashChangeStateFlag(0));
    return;
  }

  this->m_pMaster->m_byMoveType = 1;
  this->m_pMaster->SenseState();
  this->m_pMaster->UpdateVisualVer(CPlayer::CashChangeStateFlag(0));
  const bool byDirect = this->m_pMaster->m_byMoveDirect == 0;
  this->m_pMaster->pc_MoveNext(1u, this->m_pMaster->m_fCurPos, this->m_pMaster->m_fTarPos, byDirect);
  if (previous)
  {
    this->m_pMaster->SendMsg_Circle_DelEffect(previous->m_byEffectCode, previous->m_wEffectIndex, previous->m_byLv, 0);
  }
}

void CEquipItemSFAgent::EndContSF(_sf_continous *pSF_Cont)
{
  if (!pSF_Cont)
  {
    return;
  }

  for (int nEquipTblIndex = 0; nEquipTblIndex < 8; ++nEquipTblIndex)
  {
    if (this->m_pContSF[nEquipTblIndex] == pSF_Cont)
    {
      this->SetSFCont(static_cast<unsigned int>(nEquipTblIndex), nullptr);
    }
  }
}

void CEquipItemSFAgent::StartContSF(_sf_continous *pSF_Cont)
{
  if (!pSF_Cont)
  {
    return;
  }

  const unsigned __int8 effectCode = pSF_Cont->m_byEffectCode;
  const int effectIndex = pSF_Cont->m_wEffectIndex;
  if (effectCode)
  {
    return;
  }

  _skill_fld *skillRecord = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(effectIndex));
  if (!skillRecord || skillRecord->m_nClass != 2)
  {
    return;
  }

  _requireSlot requiredSlot{};
  if (!this->GetRequireSFSlot(&requiredSlot, skillRecord))
  {
    return;
  }

  for (int nEquipTblIndex = 0; nEquipTblIndex < 8; ++nEquipTblIndex)
  {
    if (requiredSlot.m_SlotIndex[nEquipTblIndex] && !this->m_pContSF[nEquipTblIndex])
    {
      this->SetSFCont(static_cast<unsigned int>(nEquipTblIndex), pSF_Cont);
    }
  }
}

