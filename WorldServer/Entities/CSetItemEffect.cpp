#include "pch.h"

#include "CSetItemEffect.h"

#include "CSUItemSystem.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CSetItemEffect::CSetItemEffect()
{
  Init_Info();
}

void CSetItemEffect::Init_Info()
{
  for (int j = 0; j < 6; ++j)
  {
    Init_Data(static_cast<unsigned __int8>(j));
  }
  SetResetInfo(false, 0, 0, 0);
  m_byTotalSetCount = 0;
}

void CSetItemEffect::Init_Data(unsigned __int8 byIdx)
{
  if (byIdx < 6)
  {
    m_setCount[byIdx].m_bCheckSetEffect = false;
    m_setCount[byIdx].m_dwSetItem = 0;
    m_setCount[byIdx].m_bySetItemNum = 0;
    m_setCount[byIdx].m_bySetEffectNum = 0;
  }
}

__int64 CSetItemEffect::SetOnEffect(
  _AVATOR_DATA *pData,
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum)
{
  if (!pData)
  {
    return 7;
  }

  CSUItemSystem *suSystem = CSUItemSystem::Instance();
  if (!suSystem)
  {
    return 7;
  }

  CRecordData *recordSetItem = &suSystem->m_SUOrigin[0];
  if (!recordSetItem)
  {
    return 7;
  }

  _SetItemEff_fld *setFld = reinterpret_cast<_SetItemEff_fld *>(recordSetItem->GetRecord(dwSetItem));
  if (!setFld)
  {
    return 2;
  }

  CSetItemType *setItemType = suSystem->GetCSetItemType();
  if (!setItemType)
  {
    return 7;
  }

  si_interpret *si = setItemType->Getsi_interpret(static_cast<int>(dwSetItem));
  if (!si)
  {
    return 2;
  }

  const unsigned __int8 equipCount = Check_EquipItem(pData, setFld);
  if (equipCount < bySetItemNum)
  {
    return 3;
  }

  const unsigned __int8 effectTypeCount = si->GetEffectTypeCount();
  unsigned __int8 effectTypeIndex = 0;
  for (; effectTypeIndex < effectTypeCount; ++effectTypeIndex)
  {
    if (si->GetCountOfItem(effectTypeIndex) == bySetItemNum
        && si->GetCountOfEffect(effectTypeIndex) == bySetEffectNum)
    {
      break;
    }
  }
  if (effectTypeIndex == effectTypeCount)
  {
    return 4;
  }

  if (IsSetOn(dwSetItem))
  {
    if (IsSetOnComplete(dwSetItem, bySetItemNum, bySetEffectNum))
    {
      return 5;
    }

    Reset_Set(dwSetItem, bySetItemNum, bySetEffectNum);
    return 8;
  }

  Attach_Set(dwSetItem, bySetItemNum, bySetEffectNum);
  return 0;
}

__int64 CSetItemEffect::SetOffEffect(
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum)
{
  if (!IsSetOnComplete(dwSetItem, bySetItemNum, bySetEffectNum))
  {
    return 6;
  }

  if (Detach_Set(dwSetItem))
  {
    return 1;
  }

  return 6;
}

unsigned __int8 CSetItemEffect::Check_EquipItem(_AVATOR_DATA *pData, _SetItemEff_fld *pSetFld)
{
  if (!pData || !pSetFld)
  {
    return 0;
  }

  const unsigned __int8 baseCount = static_cast<unsigned __int8>(Check_Base_EquipItem(pData, pSetFld));
  const unsigned __int8 otherCount = static_cast<unsigned __int8>(Check_Other_EquipItem(pData, pSetFld));
  return static_cast<unsigned __int8>(baseCount + otherCount);
}

char CSetItemEffect::Check_Base_EquipItem(_AVATOR_DATA *pData, _SetItemEff_fld *pSetFld)
{
  char matchCount = 0;
  char itemCode[32]{};

  for (int j = 0; j < 8; ++j)
  {
    const int itemIndex = pData->dbAvator.m_EquipKey[j].zItemIndex;
    _base_fld *record = g_Main.m_tblItemData[j].GetRecord(itemIndex);
    if (!record)
    {
      continue;
    }

    strcpy_0(itemCode, record->m_strCode);
    switch (j)
    {
      case 0:
        if (!strcmp_0(itemCode, pSetFld->m_strset_upper))
        {
          ++matchCount;
        }
        break;
      case 1:
        if (!strcmp_0(itemCode, pSetFld->m_strset_lower))
        {
          ++matchCount;
        }
        break;
      case 2:
        if (!strcmp_0(itemCode, pSetFld->m_strset_gauntlet))
        {
          ++matchCount;
        }
        break;
      case 3:
        if (!strcmp_0(itemCode, pSetFld->m_strset_shoes))
        {
          ++matchCount;
        }
        break;
      case 4:
        if (!strcmp_0(itemCode, pSetFld->m_strset_head))
        {
          ++matchCount;
        }
        break;
      case 5:
        if (!strcmp_0(itemCode, pSetFld->m_strset_shield))
        {
          ++matchCount;
        }
        break;
      case 6:
        if (!strcmp_0(itemCode, pSetFld->m_strset_weapon))
        {
          ++matchCount;
        }
        break;
      case 7:
        if (!strcmp_0(itemCode, pSetFld->m_strset_cloak))
        {
          ++matchCount;
        }
        break;
      default:
        break;
    }
  }

  return matchCount;
}

char CSetItemEffect::Check_Other_EquipItem(_AVATOR_DATA *pData, _SetItemEff_fld *pSetFld)
{
  char matchCount = 0;
  char itemCode[32]{};

  bool amulet1Matched = false;
  bool amulet2Matched = false;
  bool ring1Matched = false;
  bool ring2Matched = false;

  for (int j = 0; j < 7; ++j)
  {
    const unsigned __int8 tableCode = pData->dbEquip.m_EmbellishList[j].Key.byTableCode;
    const int itemIndex = pData->dbEquip.m_EmbellishList[j].Key.wItemIndex;
    if (tableCode >= 37)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(itemIndex);
    if (!record)
    {
      continue;
    }

    strcpy_0(itemCode, record->m_strCode);
    if (!amulet1Matched && !strcmp_0(itemCode, pSetFld->m_strset_amulet1))
    {
      ++matchCount;
      amulet1Matched = true;
      continue;
    }
    if (!amulet2Matched && !strcmp_0(itemCode, pSetFld->m_strset_amulet2))
    {
      ++matchCount;
      amulet2Matched = true;
      continue;
    }
    if (!ring1Matched && !strcmp_0(itemCode, pSetFld->m_strset_ring1))
    {
      ++matchCount;
      ring1Matched = true;
      continue;
    }
    if (!ring2Matched && !strcmp_0(itemCode, pSetFld->m_strset_ring2))
    {
      ++matchCount;
      ring2Matched = true;
      continue;
    }
  }

  return matchCount;
}

char CSetItemEffect::IsSetOn(unsigned int dwSetItem)
{
  for (int j = 0; j < 6; ++j)
  {
    if (m_setCount[j].m_bCheckSetEffect && m_setCount[j].m_dwSetItem == dwSetItem)
    {
      return 1;
    }
  }

  return 0;
}

char CSetItemEffect::IsSetOnComplete(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum)
{
  for (int j = 0; j < 6; ++j)
  {
    if (m_setCount[j].m_bCheckSetEffect
        && m_setCount[j].m_dwSetItem == dwSetItem
        && m_setCount[j].m_bySetItemNum == bySetItemNum
        && m_setCount[j].m_bySetEffectNum == bySetEffectNum)
    {
      return 1;
    }
  }

  return 0;
}

char CSetItemEffect::Attach_Set(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum)
{
  if (m_byTotalSetCount > 6u)
  {
    return 0;
  }
  if (IsSetOnComplete(dwSetItem, bySetItemNum, bySetEffectNum))
  {
    return 0;
  }

  for (int j = 0; j < 6; ++j)
  {
    if (!m_setCount[j].m_bCheckSetEffect)
    {
      m_setCount[j].m_bCheckSetEffect = true;
      m_setCount[j].m_dwSetItem = dwSetItem;
      m_setCount[j].m_bySetItemNum = bySetItemNum;
      m_setCount[j].m_bySetEffectNum = bySetEffectNum;
      ++m_byTotalSetCount;
      return 1;
    }
  }

  return 0;
}

char CSetItemEffect::Detach_Set(unsigned int dwSetItem)
{
  if (!m_byTotalSetCount)
  {
    return 0;
  }
  if (!IsSetOn(dwSetItem))
  {
    return 0;
  }

  for (int j = 0; j < 6; ++j)
  {
    if (m_setCount[j].m_bCheckSetEffect && m_setCount[j].m_dwSetItem == dwSetItem)
    {
      Init_Data(static_cast<unsigned __int8>(j));
      --m_byTotalSetCount;
      return 1;
    }
  }

  return 0;
}

bool CSetItemEffect::Reset_Set(unsigned int dwSetItem, unsigned __int8 bySetItemNum, unsigned __int8 bySetEffectNum)
{
  for (int j = 0; j < 6; ++j)
  {
    if (m_setCount[j].m_bCheckSetEffect && m_setCount[j].m_dwSetItem == dwSetItem)
    {
      SetResetInfo(
        true,
        dwSetItem,
        m_setCount[j].m_bySetItemNum,
        m_setCount[j].m_bySetEffectNum);
      return Detach_Set(dwSetItem) && Attach_Set(dwSetItem, bySetItemNum, bySetEffectNum);
    }
  }

  return Detach_Set(dwSetItem) && Attach_Set(dwSetItem, bySetItemNum, bySetEffectNum);
}

void CSetItemEffect::SetResetInfo(
  bool bSet,
  unsigned int dwSetItem,
  unsigned __int8 bySetItemNum,
  unsigned __int8 bySetEffectNum)
{
  if (bSet)
  {
    m_resetInfo.m_bCheckSetEffect = true;
    m_resetInfo.m_dwSetItem = dwSetItem;
    m_resetInfo.m_bySetItemNum = bySetItemNum;
    m_resetInfo.m_bySetEffectNum = bySetEffectNum;
  }
  else
  {
    m_resetInfo.m_bCheckSetEffect = false;
    m_resetInfo.m_dwSetItem = 0;
    m_resetInfo.m_bySetItemNum = 0;
    m_resetInfo.m_bySetEffectNum = 0;
  }
}

unsigned __int8 CSetItemEffect::GetResetEffectNum()
{
  if (m_resetInfo.m_bCheckSetEffect)
  {
    return m_resetInfo.m_bySetEffectNum;
  }

  return 0;
}

unsigned __int8 CSetItemEffect::GetResetItemNum()
{
  if (m_resetInfo.m_bCheckSetEffect)
  {
    return m_resetInfo.m_bySetItemNum;
  }

  return 0;
}

__int64 CSetItemEffect::GetResetIdx()
{
  if (m_resetInfo.m_bCheckSetEffect)
  {
    return m_resetInfo.m_dwSetItem;
  }

  return 0;
}
