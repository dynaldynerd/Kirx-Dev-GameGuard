#include "pch.h"

#include "CPotionMgr.h"

#include "CNationSettingManager.h"
#include "CRFWorldDatabase.h"
#include "CLogTypeDBTaskManager.h"
#include "CGuildRoomSystem.h"
#include "CHolyStoneSystem.h"
#include "CMapData.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CRaceBuffManager.h"
#include "CheckPotion_fld.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "NameTxt_fld.h"
#include "skill_fld.h"

#include <mmsystem.h>

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
  if (!m_tblPotionEffectData.IsTableOpen() || !m_tblPotionCheckData.IsTableOpen())
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

namespace
{
bool _CheckPotionData(_CheckPotion_fld::_CheckEffectCode *checkEffectNode, CPlayer *pApplyPlayer)
{
  if (!pApplyPlayer)
  {
    return false;
  }

  const int contParamCode = checkEffectNode->m_nContParamCode;
  if (contParamCode == -1)
  {
    return true;
  }

  if (contParamCode == 0)
  {
    const float effRate = pApplyPlayer->m_EP.GetEff_Rate(checkEffectNode->m_nContParamIndex);
    if (checkEffectNode->m_nContValSt == -1)
    {
      return checkEffectNode->m_fContValue > effRate;
    }
    if (checkEffectNode->m_nContValSt == 0)
    {
      return checkEffectNode->m_fContValue >= effRate;
    }
    if (checkEffectNode->m_nContValSt == 1)
    {
      return checkEffectNode->m_fContValue == effRate;
    }
    if (checkEffectNode->m_nContValSt == 2)
    {
      return effRate >= checkEffectNode->m_fContValue;
    }
    return effRate > checkEffectNode->m_fContValue;
  }

  if (contParamCode == 1)
  {
    const float effPlus = pApplyPlayer->m_EP.GetEff_Plus(checkEffectNode->m_nContParamIndex);
    if (checkEffectNode->m_nContValSt == -1)
    {
      return checkEffectNode->m_fContValue > effPlus;
    }
    if (checkEffectNode->m_nContValSt == 0)
    {
      return checkEffectNode->m_fContValue >= effPlus;
    }
    if (checkEffectNode->m_nContValSt == 1)
    {
      return checkEffectNode->m_fContValue == effPlus;
    }
    if (checkEffectNode->m_nContValSt == 2)
    {
      return effPlus >= checkEffectNode->m_fContValue;
    }
    return effPlus > checkEffectNode->m_fContValue;
  }

  if (contParamCode == 2)
  {
    const bool effState = pApplyPlayer->m_EP.GetEff_State(checkEffectNode->m_nContParamIndex);
    if (checkEffectNode->m_nContValSt == 1)
    {
      const float stateValue = effState ? 1.0f : 0.0f;
      return checkEffectNode->m_fContValue == stateValue;
    }

    const float stateValue = effState ? 0.0f : 1.0f;
    return checkEffectNode->m_fContValue == stateValue;
  }

  return false;
}

bool _GetTempEffectValue(_skill_fld *pEffectFld, int nTempEffectType, float *fValue)
{
  if (!pEffectFld)
  {
    return false;
  }

  if (nTempEffectType == 43)
  {
    *fValue = static_cast<float>(pEffectFld->m_nContEffectSec[0]);
  }
  else
  {
    *fValue = pEffectFld->m_fTempValue[0];
  }
  return true;
}
}

unsigned __int8 CPotionMgr::UsePotion(
  CPlayer *pUsePlayer,
  CCharacter *pTargetCharacter,
  _PotionItem_fld *pfB,
  unsigned int nCurTime)
{
  if (!pfB || !pUsePlayer || !pTargetCharacter)
  {
    return 25;
  }

  _skill_fld *effectFld = reinterpret_cast<_skill_fld *>(m_tblPotionEffectData.GetRecord(pfB->m_strEffCode));
  const bool checkDistance = pfB->m_nUseRange >= 0;
  const unsigned int preCheck =
    PreCheckPotion(pUsePlayer, &pTargetCharacter, pfB, nCurTime, effectFld, checkDistance);
  if (preCheck)
  {
    return static_cast<unsigned __int8>(preCheck);
  }

  _CheckPotion_fld *checkFld =
    reinterpret_cast<_CheckPotion_fld *>(m_tblPotionCheckData.GetRecord(pfB->m_strTargetEff));

  bool pathFlags[72]{};
  CCharacter *targets[33]{};
  unsigned __int8 applyResult[44]{};
  const int targetCount = static_cast<int>(
    pUsePlayer->FindPotionEffectDst(
      pfB->m_nEffArea,
      pfB->m_nEffAreaVal,
      true,
      pTargetCharacter,
      effectFld->m_strActableDst,
      targets,
      pathFlags));

  bool targetApplied = false;
  for (int j = 0; j < targetCount; ++j)
  {
    if (targets[j]->m_ObjID.m_byID != 0)
    {
      continue;
    }

    CPlayer *applyPlayer = static_cast<CPlayer *>(targets[j]);
    _CheckPotion_fld *useCheckFld = (pathFlags[j] && targetApplied) ? nullptr : checkFld;
    applyResult[j] = static_cast<unsigned __int8>(ApplyPotion(
      pUsePlayer,
      applyPlayer,
      effectFld,
      useCheckFld,
      pfB,
      true));

    if (applyPlayer == pTargetCharacter)
    {
      if (applyResult[j])
      {
        if (effectFld->m_nTempEffectType == 45)
        {
          return 18;
        }
        return 25;
      }
      targetApplied = true;
    }
  }

  bool anySuccess = false;
  for (int j = 0; j < targetCount; ++j)
  {
    if (!applyResult[j])
    {
      anySuccess = true;
      break;
    }
  }

  unsigned int result = 0;
  if (!anySuccess)
  {
    result = applyResult[0];
  }
  if (result == 19 && pfB->m_nDelayType == 12)
  {
    return 21;
  }
  return static_cast<unsigned __int8>(result);
}

unsigned int CPotionMgr::PreCheckPotion(
  CPlayer *pUsePlayer,
  CCharacter **pTargetCharacter,
  const _PotionItem_fld *pfB,
  unsigned int nCurTime,
  _skill_fld *pFld,
  bool bCheckDist)
{
  if (!pUsePlayer || !*pTargetCharacter || !pfB || !pFld)
  {
    return static_cast<unsigned int>(-1);
  }

  if (Major_Cash_Item)
  {
    if (pFld->m_nTempEffectType == 48 || pFld->m_nTempEffectType == 49)
    {
      return 25;
    }
    if (pFld->m_nTempEffectType == 70 || pFld->m_nTempEffectType == 71)
    {
      return 25;
    }
  }

  if (pUsePlayer->m_EP.GetEff_Have(58) == 0.0f
      && !pUsePlayer->m_PotionParam.IsUsableActDelay(static_cast<unsigned __int8>(pfB->m_nDelayType), nCurTime))
  {
    return 9;
  }

  if (!pUsePlayer->IsEffectableDst(pFld->m_strActableDst, *pTargetCharacter))
  {
    if (pFld->m_strActableDst[0] != '1')
    {
      return 19;
    }
    *pTargetCharacter = pUsePlayer;
  }

  if (!(*pTargetCharacter)->m_ObjID.m_byID
      && pFld->m_nTempEffectType != 70
      && pFld->m_nTempEffectType != 71
      && pFld->m_nTempEffectType != 48
      && pFld->m_nTempEffectType != 49)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(*pTargetCharacter);
    if (pUsePlayer != targetPlayer && targetPlayer->IsRidingUnit())
    {
      return 19;
    }
  }

  if (!(*pTargetCharacter)->m_ObjID.m_byID)
  {
    if ((pFld->m_nTempEffectType == 48 || pFld->m_nTempEffectType == 70)
        && static_cast<CPlayer *>(*pTargetCharacter)->IsSiegeMode())
    {
      return 19;
    }
    if ((pFld->m_nTempEffectType == 49 || pFld->m_nTempEffectType == 71) && pUsePlayer->IsSiegeMode())
    {
      return 19;
    }
  }

  if (bCheckDist && pUsePlayer != *pTargetCharacter)
  {
    const float distance = GetSqrt(pUsePlayer->m_fCurPos, (*pTargetCharacter)->m_fCurPos);
    if (distance > static_cast<float>(pfB->m_nUseRange))
    {
      return 20;
    }
  }

  if (pfB->m_nDelayType == 9 && !(*pTargetCharacter)->m_ObjID.m_byID)
  {
    _sf_continous *guardAura = &(*pTargetCharacter)->m_SFContAura[0][6];
    if (guardAura->m_bExist && guardAura->m_dwStartSec + guardAura->m_wDurSec > nCurTime)
    {
      return 17;
    }
  }

  if (pfB->m_nDelayType == 14 && !(*pTargetCharacter)->m_ObjID.m_byID)
  {
    if (!(*pTargetCharacter)->m_SFContAura[0][5].m_wDurSec)
    {
      return 23;
    }
  }

  if (pfB->m_nDelayType == 35
      && !(*pTargetCharacter)->m_ObjID.m_byID
      && g_HolySys.GetDestroyerState() == 2
      && g_HolySys.GetDestroyerSerial() == pUsePlayer->m_dwObjSerial)
  {
    return 45;
  }

  if (pfB->m_nUseState)
  {
    if (pfB->m_nUseState == 1 && pUsePlayer->Is_Battle_Mode())
    {
      return 28;
    }
    if (pfB->m_nUseState == 2 && !pUsePlayer->Is_Battle_Mode())
    {
      return 27;
    }
  }

  if (pUsePlayer->m_bInGuildBattle && pUsePlayer->m_bTakeGravityStone)
  {
    if (pFld->m_nContEffectType != -1)
    {
      return 14;
    }
    if (pFld->m_nTempEffectType != -1 && pFld->m_nTempEffectType < 150 && pFld->m_nTempEffectType != 37)
    {
      return 14;
    }
  }

  if (pfB->m_nUseState == 3 && !pUsePlayer->m_bCorpse)
  {
    return 29;
  }

  if (pUsePlayer->m_PotionBufUse.IsExtPotionUse())
  {
    if (pFld->m_nTempEffectType == 53)
    {
      return 30;
    }

    for (int j = 0; j < 2; ++j)
    {
      _ContPotionData *contData = &pUsePlayer->m_PotionParam.m_ContCommonPotionData[j];
      if (!contData->IsLive())
      {
        continue;
      }

      const int effectIndex = static_cast<int>(contData->GetEffectIndex());
      _base_fld *effectRecord = m_tblPotionEffectData.GetRecord(effectIndex);
      _base_fld *itemRecord = g_Main.m_tblItemData[13].GetRecord(effectIndex);

      const bool matchEffLim1 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) == pFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != -1
        && pFld->m_nEffLimType != -1;
      const bool matchEffLim2 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) == pFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != -1
        && pFld->m_nEffLimType2 != -1;
      const bool matchEffLim3 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) == pFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) != -1
        && pFld->m_nEffLimType != -1;
      const bool matchEffLim4 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) != pFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) == pFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) != -1
        && pFld->m_nEffLimType2 != -1;

      if ((matchEffLim1 || matchEffLim2 || matchEffLim3 || matchEffLim4)
          && itemRecord
          && pfB->m_nPotionCheck < static_cast<int>(itemRecord[5].m_dwIndex))
      {
        return 31;
      }
    }
  }

  if (pFld->m_nTempEffectType == 54 && !(*pTargetCharacter)->m_ObjID.m_byID)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(*pTargetCharacter);
    if (!targetPlayer->m_Param.GetTrunkSlotNum())
    {
      return 33;
    }
    if (targetPlayer->m_Param.GetExtTrunkSlotNum() >= 0x28u)
    {
      return 34;
    }
  }

  if (pFld->m_nTempEffectType == 55 && !(*pTargetCharacter)->m_ObjID.m_byID)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(*pTargetCharacter);
    CRaceBuffManager *raceBuffMgr = CRaceBuffManager::Instance();
    const unsigned int level = raceBuffMgr->m_kBuffByHolyQuest.GetRaceBuffLevel(targetPlayer);
    const int cancelResult = raceBuffMgr->m_kBuffByHolyQuest.CancelPlayerRaceBuff(
      targetPlayer,
      CRaceBuffInfoByHolyQuestfGroup::RESULT_FAIL,
      level);
    if (level > 2)
    {
      return 35;
    }
    if (cancelResult == -3 || static_cast<float>(static_cast<int>(level + 1)) > pFld->m_fTempValue[0])
    {
      return 36;
    }
  }

  if (pFld->m_nTempEffectType == 56 && !(*pTargetCharacter)->m_ObjID.m_byID)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(*pTargetCharacter);
    CRaceBuffManager *raceBuffMgr = CRaceBuffManager::Instance();
    const unsigned int level = raceBuffMgr->m_kBuffByHolyQuest.GetRaceBuffLevel(targetPlayer);
    const int cancelResult = raceBuffMgr->m_kBuffByHolyQuest.CancelPlayerRaceBuff(
      targetPlayer,
      CRaceBuffInfoByHolyQuestfGroup::RESULT_LOSE,
      level);
    if (level > 2)
    {
      return 35;
    }
    if (cancelResult == -3 || static_cast<float>(static_cast<int>(level + 1)) > pFld->m_fTempValue[0])
    {
      return 36;
    }
  }

  if (pFld->m_nTempEffectType == 48
      || pFld->m_nTempEffectType == 49
      || pFld->m_nTempEffectType == 70
      || pFld->m_nTempEffectType == 71)
  {
    if (!CheckPotionUsableMap(pfB, (*pTargetCharacter)->m_pCurMap))
    {
      return 37;
    }

    if (pFld->m_nTempEffectType == 48 || pFld->m_nTempEffectType == 70)
    {
      CPlayer *targetPlayer = static_cast<CPlayer *>(*pTargetCharacter);
      if (targetPlayer->m_Param.m_pGuild)
      {
        const int raceCode = targetPlayer->m_Param.GetRaceCode();
        CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
        CMapData *raceMap = roomSystem->GetMapData(static_cast<unsigned __int8>(raceCode), 0);
        CMapData *raceMap2 = roomSystem->GetMapData(static_cast<unsigned __int8>(raceCode), 1);
        if (targetPlayer->m_pCurMap == raceMap || targetPlayer->m_pCurMap == raceMap2)
        {
          roomSystem->RoomOut(
            targetPlayer->m_Param.m_pGuild->m_dwSerial,
            targetPlayer->m_ObjID.m_wIndex,
            targetPlayer->m_pUserDB->m_dwSerial);
        }
      }
    }
    else if ((pFld->m_nTempEffectType == 49 || pFld->m_nTempEffectType == 71) && pUsePlayer->m_Param.m_pGuild)
    {
      const int raceCode = pUsePlayer->m_Param.GetRaceCode();
      CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
      CMapData *raceMap = roomSystem->GetMapData(static_cast<unsigned __int8>(raceCode), 0);
      CMapData *raceMap2 = roomSystem->GetMapData(static_cast<unsigned __int8>(raceCode), 1);
      if (pUsePlayer->m_pCurMap == raceMap || pUsePlayer->m_pCurMap == raceMap2)
      {
        roomSystem->RoomOut(
          pUsePlayer->m_Param.m_pGuild->m_dwSerial,
          pUsePlayer->m_ObjID.m_wIndex,
          pUsePlayer->m_pUserDB->m_dwSerial);
      }
    }
  }

  if (!CheckPotionUsableMap(pfB, pUsePlayer->m_pCurMap))
  {
    return 37;
  }

  if (!(*pTargetCharacter)->m_ObjID.m_byID && pFld->m_nTempEffectType == 73)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(*pTargetCharacter);
    if (!targetPlayer)
    {
      return 25;
    }

    if (targetPlayer->GetHP() == targetPlayer->GetMaxHP()
        && targetPlayer->GetFP() == targetPlayer->GetMaxFP()
        && targetPlayer->GetSP() == targetPlayer->GetMaxSP())
    {
      return 46;
    }
  }

  if ((*pTargetCharacter)->m_ObjID.m_byID || pFld->m_nTempEffectType != 74)
  {
    return 0;
  }

  int contCount = 0;
  for (int k = 0; k < 8; ++k)
  {
    if ((*pTargetCharacter)->m_SFCont[0][k].m_bExist)
    {
      ++contCount;
    }
  }
  if (!contCount)
  {
    return 47;
  }

  return 0;
}

unsigned int CPotionMgr::ApplyPotion(
  CPlayer *pUsePlayer,
  CPlayer *pApplyPlayer,
  _skill_fld *pEffecFld,
  _CheckPotion_fld *pCheckFld,
  const _PotionItem_fld *pfB,
  bool bCommonPotion)
{
  if (!pUsePlayer || !pApplyPlayer || !pEffecFld)
  {
    return static_cast<unsigned int>(-1);
  }

  if (pCheckFld)
  {
    for (int j = 0; j < 5; ++j)
    {
      if (!_CheckPotionData(&pCheckFld->m_CheckEffectCode[j], pApplyPlayer))
      {
        return 19;
      }
    }
  }

  unsigned int insertResult = static_cast<unsigned int>(-1);
  int tempResult = -1;
  if (pEffecFld->m_nContEffectType != -1 && bCommonPotion)
  {
    unsigned int deleteIndex = 0;
    bool hasMatchBuff = false;
    int matchIndex = 0;
    for (int k = 0; k < 2; ++k)
    {
      const int effectIndex = static_cast<int>(pUsePlayer->m_PotionParam.m_ContCommonPotionData[k].GetEffectIndex());
      _base_fld *itemRecord = g_Main.m_tblItemData[13].GetRecord(effectIndex);
      _base_fld *effectRecord = m_tblPotionEffectData.GetRecord(effectIndex);

      const bool matchEffLim1 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) == pEffecFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != -1
        && pEffecFld->m_nEffLimType != -1;
      const bool matchEffLim2 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pEffecFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) == pEffecFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != -1
        && pEffecFld->m_nEffLimType2 != -1;
      const bool matchEffLim3 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pEffecFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pEffecFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) == pEffecFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) != -1
        && pEffecFld->m_nEffLimType != -1;
      const bool matchEffLim4 =
        effectRecord
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pEffecFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[36]) != pEffecFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) != pEffecFld->m_nEffLimType
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) == pEffecFld->m_nEffLimType2
        && *reinterpret_cast<int *>(&effectRecord[13].m_strCode[40]) != -1
        && pEffecFld->m_nEffLimType2 != -1;

      if ((matchEffLim1 || matchEffLim2 || matchEffLim3 || matchEffLim4)
          && itemRecord
          && static_cast<int>(itemRecord[5].m_dwIndex) <= pfB->m_nPotionCheck)
      {
        hasMatchBuff = true;
        matchIndex = k;
      }
    }

    if (pApplyPlayer->m_PotionBufUse.IsExtPotionUse())
    {
      deleteIndex = hasMatchBuff ? static_cast<unsigned int>(matchIndex) : SelectDeleteBuf(pApplyPlayer, true, false);
    }
    else
    {
      deleteIndex = SelectDeleteBuf(pApplyPlayer, false, false);
    }

    if (deleteIndex > 2)
    {
      return 25;
    }

    insertResult = InsertPotionContEffect(
      pApplyPlayer,
      &pApplyPlayer->m_PotionParam.m_ContCommonPotionData[deleteIndex],
      pEffecFld,
      static_cast<unsigned int>(pEffecFld->m_nContEffectSec[0]));
  }

  unsigned __int8 effectResult[32]{};
  effectResult[0] = static_cast<unsigned __int8>(-1);
  if (pEffecFld->m_nTempEffectType != -1)
  {
    if (pEffecFld->m_nTempEffectType >= 150)
    {
      tempResult = -1;
    }
    else
    {
      float fValue[5]{};
      if (_GetTempEffectValue(pEffecFld, pEffecFld->m_nTempEffectType, fValue))
      {
        auto tempFunc = g_TempEffectFunc[pEffecFld->m_nTempEffectType];
        if (tempFunc && tempFunc(pUsePlayer, pApplyPlayer, fValue[0], effectResult))
        {
          tempResult = 0;
        }
        else
        {
          tempResult = effectResult[0];
        }
      }
      else
      {
        tempResult = -1;
      }
    }
  }

  if (insertResult && tempResult)
  {
    return insertResult;
  }
  return 0;
}

unsigned int CPotionMgr::RemovePotionContEffect(CPlayer *pApplyPlayer, _ContPotionData *contPotionData)
{
  if (!contPotionData->IsLive())
  {
    return static_cast<unsigned int>(-1);
  }

  _base_fld *effectRecord = m_tblPotionEffectData.GetRecord(contPotionData->m_dwPotionEffectIndex);
  if (!effectRecord)
  {
    return static_cast<unsigned int>(-1);
  }

  char *contParam = &effectRecord[13].m_strCode[48];
  for (int j = 0; j < 5; ++j)
  {
    char *node = &contParam[36 * j];
    if (*reinterpret_cast<int *>(node) == -1)
    {
      break;
    }

    const int contParamCode = *reinterpret_cast<int *>(node);
    const int contParamIndex = *(reinterpret_cast<int *>(node) + 1);
    const float contValue = *(reinterpret_cast<float *>(node) + 2);
    if (contParamCode == 0)
    {
      pApplyPlayer->m_EP.SetEff_Rate(contParamIndex, contValue, false);
    }
    else if (contParamCode == 1)
    {
      pApplyPlayer->m_EP.SetEff_Plus(contParamIndex, contValue, false);
    }
    else if (contParamCode == 2)
    {
      pApplyPlayer->m_EP.SetEff_State(contParamIndex, false);
    }
  }

  const unsigned __int16 effectIndex = static_cast<unsigned __int16>(contPotionData->GetEffectIndex());
  pApplyPlayer->m_PotionBufUse.SednMsg_RemovePotionContEffect(effectIndex, pApplyPlayer->m_ObjID.m_wIndex);
  contPotionData->Init();
  return 0;
}

unsigned int CPotionMgr::InsertPotionContEffect(
  CPlayer *pApplyPlayer,
  _ContPotionData *contPotionData,
  _skill_fld *pEffecFld,
  unsigned int dwDurTime)
{
  if (!pApplyPlayer || !pEffecFld)
  {
    return static_cast<unsigned int>(-1);
  }

  RemovePotionContEffect(pApplyPlayer, contPotionData);
  for (int j = 0; j < 5; ++j)
  {
    _cont_param_list *paramNode = &pEffecFld->m_ContParamList[j];
    if (paramNode->m_nContParamCode == -1)
    {
      break;
    }

    if (paramNode->m_nContParamCode == 0)
    {
      pApplyPlayer->m_EP.SetEff_Rate(paramNode->m_nContParamIndex, paramNode->m_fContValue[0], true);
    }
    else if (paramNode->m_nContParamCode == 1)
    {
      pApplyPlayer->m_EP.SetEff_Plus(paramNode->m_nContParamIndex, paramNode->m_fContValue[0], true);
    }
    else if (paramNode->m_nContParamCode == 2)
    {
      pApplyPlayer->m_EP.SetEff_State(paramNode->m_nContParamIndex, true);
    }
  }

  const unsigned int now = _sf_continous::GetSFContCurTime();
  contPotionData->Set(pEffecFld->m_dwIndex, now, static_cast<unsigned __int16>(dwDurTime));
  return 0;
}

void CPotionMgr::UpdatePotionContEffect(CPlayer *pPlayer)
{
  if (!pPlayer)
  {
    return;
  }

  const unsigned int now = _sf_continous::GetSFContCurTime();
  for (int j = 0; j < 2; ++j)
  {
    _ContPotionData *contData = &pPlayer->m_PotionParam.m_ContCommonPotionData[j];
    if (contData->IsLive())
    {
      const unsigned int elapsed = now - contData->m_dwStartSec;
      if (elapsed >= contData->m_wDurCapSec)
      {
        RemovePotionContEffect(pPlayer, contData);
      }
    }
  }

  _ContPotionData *stoneData = &pPlayer->m_PotionParam.m_StoneOfMovePotionData;
  if (stoneData->IsLive())
  {
    const unsigned int elapsed = now - stoneData->m_dwStartSec;
    if (elapsed >= stoneData->m_wDurCapSec)
    {
      RemovePotionContEffect(pPlayer, stoneData);
    }
  }
}

unsigned int CPotionMgr::SelectDeleteBuf(CPlayer *pOne, bool bUse, bool bRemove)
{
  int liveCount = 0;
  unsigned int selectedIndex = 0;
  for (int j = 0; j < 2; ++j)
  {
    if (bUse)
    {
      if (pOne->m_PotionParam.m_ContCommonPotionData[j].IsLive())
      {
        ++liveCount;
      }
      else
      {
        selectedIndex = static_cast<unsigned int>(j);
      }
    }
    else
    {
      ++liveCount;
      selectedIndex = static_cast<unsigned int>(j);
    }
  }

  if (liveCount == 0)
  {
    selectedIndex = 0;
  }
  else if (liveCount == 2)
  {
    unsigned int oldestStartSec = static_cast<unsigned int>(-1);
    for (int k = 0; k < 2; ++k)
    {
      if (pOne->m_PotionParam.m_ContCommonPotionData[k].m_dwStartSec < oldestStartSec)
      {
        oldestStartSec = pOne->m_PotionParam.m_ContCommonPotionData[k].m_dwStartSec;
        selectedIndex = static_cast<unsigned int>(k);
      }
    }
  }

  if (bRemove)
  {
    RemovePotionContEffect(pOne, &pOne->m_PotionParam.m_ContCommonPotionData[selectedIndex]);
  }
  return selectedIndex;
}

char CPotionMgr::CheckPotionUsableMap(const _PotionItem_fld *pPotionFld, CMapData *pMap)
{
  if (!pPotionFld || !pMap)
  {
    return 0;
  }

  if (!pPotionFld->m_nPotionLim)
  {
    for (int j = 0; j < 5; ++j)
    {
      if (pPotionFld->m_nMapCode[j] == pMap->m_pMapSet->m_nPotionLim)
      {
        return 0;
      }
    }
    return 1;
  }

  if (pPotionFld->m_nPotionLim == 1)
  {
    for (int j = 0; j < 5; ++j)
    {
      if (pPotionFld->m_nMapCode[j] == pMap->m_pMapSet->m_nPotionLim)
      {
        return 1;
      }
    }
    return 0;
  }

  if (pPotionFld->m_nPotionLim == 2)
  {
    if (pMap->m_pMapSet->m_nMapType == 1)
    {
      return 0;
    }
    return 1;
  }

  if (pPotionFld->m_nPotionLim == 3 && pMap->m_pMapSet->m_nMapType != 1)
  {
    return 0;
  }

  return 1;
}

void CPotionMgr::InsertMovePotionStoneEffect(CPlayer *pApplyPlayer)
{
  if (!pApplyPlayer)
  {
    return;
  }

  pApplyPlayer->m_EP.SetEff_State(28, true);
  const unsigned int now = _sf_continous::GetSFContCurTime();
  pApplyPlayer->m_PotionParam.m_StoneOfMovePotionData.Set(0x1B8u, now, 0xAu);
}

bool CPotionMgr::InsertRenamePotion(CRFWorldDatabase *pkWorldDB, char *pData)
{
  return pkWorldDB->Insert_RenamePotionLog(
    *reinterpret_cast<unsigned int *>(pData),
    pData + 4,
    pData + 21);
}

bool CPotionMgr::IsPotionDelayUseIndex(unsigned int nIndex)
{
  return nIndex < 0x26 && m_PotionInnerData.m_bPotionDelayIndexList[nIndex];
}

void CPotionMgr::Complete_RenameChar_DB_Select(unsigned __int8 byRet, char *p)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, *reinterpret_cast<unsigned int *>(p));
  if (player && player->m_bOper)
  {
    if (byRet)
    {
      player->SendMsg_CharacterRenameCashResult(0, 0x1Au);
    }
    else if (*reinterpret_cast<unsigned int *>(p + 4) == static_cast<unsigned int>(-1))
    {
      memcpy_s(&player->m_ReNamePotionUseInfo, 4u, p + 8, 4u);
      strcpy_s(player->m_ReNamePotionUseInfo.wszChangeName, 0x11u, p + 12);
      player->SendMsg_CharacterRenameCashResult(0, 0);
    }
    else
    {
      player->SendMsg_CharacterRenameCashResult(0, 0xAu);
    }
  }
}

void CPotionMgr::Complete_RenameChar_DB_Update(unsigned __int8 byRet, char *p)
{
  unsigned __int8 resultCode = 0;
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, *reinterpret_cast<unsigned int *>(p));

  if (byRet)
  {
    if (player && player->m_bOper)
    {
      _STORAGE_LIST *storageList = player->m_Param.m_pStoragePtr[static_cast<unsigned __int8>(p[8])];
      _STORAGE_LIST::_storage_con *storage = storageList
        ? storageList->GetPtrFromSerial(*reinterpret_cast<unsigned __int16 *>(p + 9))
        : nullptr;
      if (storage)
      {
        storage->lock(false);
      }
    }
    resultCode = 26;
  }
  else
  {
    if (player && player->m_bOper)
    {
      _STORAGE_LIST *storageList = player->m_Param.m_pStoragePtr[static_cast<unsigned __int8>(p[8])];
      _STORAGE_LIST::_db_con *useItem =
        storageList ? storageList->GetPtrFromSerial(*reinterpret_cast<unsigned __int16 *>(p + 9)) : nullptr;

      if (!useItem)
      {
        player->m_ReNamePotionUseInfo.Init();
        const char *charName = player->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "CPotionMgr::Complete_RenameChar_DB_Update : %u(%s) pUseItem NULL!",
          player->m_dwObjSerial,
          charName);
        return;
      }

      _base_fld *record = g_Main.m_tblItemData[13].GetRecord(useItem->m_wItemIndex);
      if (!record)
      {
        const char *charName = player->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "CPotionMgr::Complete_RenameChar_DB_Update : %u(%s) pPotionFld NULL!",
          player->m_dwObjSerial,
          charName);
      }

      const unsigned __int8 adjustResult =
        player->Emb_AlterDurPoint(0, useItem->m_byStorageIndex, -1, 0, 0);
      player->SendMsg_AdjustAmountInform(0, useItem->m_wSerial, adjustResult);

      if (record && *reinterpret_cast<unsigned int *>(&record[9].m_strCode[36]) == 1)
      {
        CPlayer::s_MgrItemHistory.cash_item_use(
          player->m_ObjID.m_wIndex,
          useItem,
          player->m_szItemHistoryFileName);
      }

      if (record)
      {
        const unsigned int actDelay = static_cast<unsigned int>(*reinterpret_cast<float *>(record[6].m_strCode));
        const DWORD now = timeGetTime();
        player->SetPotionActDelay(record[6].m_strCode[4], now / 1000, actDelay);
      }

      strcpy_0(player->m_Param.m_dbChar.m_wszCharID, p + 12);
      strcpy_0(player->m_pUserDB->m_wszAvatorName, p + 12);
      CPlayer::CashChangeStateFlag changeFlag(1);
      player->UpdateVisualVer(changeFlag);

      unsigned int objSerial = player->m_dwObjSerial;
      char nameBuffer[38]{};
      strcpy_0(nameBuffer, player->m_pUserDB->m_wszAvatorName);
      unsigned __int8 type[2]{3, 61};
      unsigned __int8 report[23]{};
      memcpy_0(report + 2, &objSerial, sizeof(objSerial));
      memcpy_0(report + 6, nameBuffer, 17);
      player->CircleReport(type, reinterpret_cast<char *>(report), 23, false);

      if (player->m_pPartyMgr->IsPartyMode())
      {
        player->pc_PartyLeaveSelfReqeuest();
      }
      if (player->m_Param.m_pGuild)
      {
        player->pc_GuildSelfLeaveRequest();
      }
      player->m_NameChangeBuddyInfo.bNameChange = 1;
    }

    PushRenamePotionDBLog(p);
  }

  if (player && player->m_bOper)
  {
    player->m_ReNamePotionUseInfo.Init();
    player->SendMsg_CharacterRenameCashResult(1, byRet);
  }
}

void CPotionMgr::PushRenamePotionDBLog(char *pInfo)
{
  char data[40]{};
  *reinterpret_cast<unsigned int *>(data) = *reinterpret_cast<unsigned int *>(pInfo);
  strcpy_s(data + 4, 0x11u, pInfo + 29);
  strcpy_s(data + 21, 0x11u, pInfo + 12);

  CLogTypeDBTaskManager *mgr = CLogTypeDBTaskManager::Instance();
  mgr->Push(2, data, 0x28u);
  g_Main.m_logRename.Write("CPotionMgr::PushRenamePotionDBLog()!");
}

bool __fastcall DE_Potion_CharReName(
  CCharacter *pActChar,
  CCharacter * /*pTargetChar*/,
  float /*fEffectValue*/,
  unsigned __int8 * /*byRet*/)
{
  if (!pActChar)
  {
    return false;
  }

  if (pActChar->m_ObjID.m_byID != 0)
  {
    return false;
  }

  auto *player = reinterpret_cast<CPlayer *>(pActChar);
  _qry_case_character_rename query{};
  query.dwCharSerial = player->m_pUserDB->m_dwSerial;
  query.dwAlreadySerial = static_cast<unsigned int>(-1);
  memcpy_0(&query.ItemInfo, &player->m_ReNamePotionUseInfo.ItemInfo, sizeof(query.ItemInfo));
  strcpy_s(query.wszCharName, sizeof(query.wszCharName), player->m_ReNamePotionUseInfo.wszChangeName);
  strcpy_s(query.wszOldName, sizeof(query.wszOldName), player->m_Param.GetCharNameW());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x92u, reinterpret_cast<char *>(&query), sizeof(query));
  return true;
}

bool __fastcall DE_Potion_Class_Refine(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float /*fEffectValue*/,
  unsigned __int8 *byRet)
{
  if (!pTargetChar)
  {
    return false;
  }
  if (pActChar->m_ObjID.m_byID || pTargetChar->m_ObjID.m_byID)
  {
    return false;
  }
  if (pActChar != pTargetChar)
  {
    return false;
  }

  CPlayer *targetPlayer = static_cast<CPlayer *>(pTargetChar);
  char byResult = targetPlayer->pc_InitClass();
  unsigned __int8 pbyType[2]{11, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(targetPlayer->m_ObjID.m_wIndex, pbyType, &byResult, 1u);
  if (!byResult)
  {
    return true;
  }

  *byRet = 18;
  return false;
}

bool __fastcall DE_Potion_Race_Debuff_Clear_One(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 * /*byRet*/)
{
  if (!pActChar)
  {
    return false;
  }
  if (pActChar->m_ObjID.m_byID)
  {
    return false;
  }
  if (pActChar != pTargetChar)
  {
    return false;
  }

  CPlayer *player = static_cast<CPlayer *>(pActChar);
  if (player->IsUseReleaseRaceBuffPotion())
  {
    return false;
  }

  const unsigned int uiContinueCnt = static_cast<unsigned int>(static_cast<int>(fEffectValue));
  CRaceBuffManager *raceBuffManager = CRaceBuffManager::Instance();
  const int nResult = raceBuffManager->m_kBuffByHolyQuest.CancelPlayerRaceBuff(
    player,
    CRaceBuffInfoByHolyQuestfGroup::RESULT_FAIL,
    uiContinueCnt);
  if (nResult <= 0)
  {
    char szMsg[4]{};
    *reinterpret_cast<int *>(szMsg) = nResult;
    unsigned __int8 pbyType[2]{17, 37};
    g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, szMsg, 4u);
  }
  player->SetUseReleaseRaceBuffPotion();
  return true;
}

bool __fastcall DE_Potion_Race_Debuff_Clear_Two(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 * /*byRet*/)
{
  if (!pActChar)
  {
    return false;
  }
  if (pActChar->m_ObjID.m_byID)
  {
    return false;
  }
  if (pActChar != pTargetChar)
  {
    return false;
  }

  CPlayer *player = static_cast<CPlayer *>(pActChar);
  if (player->IsUseReleaseRaceBuffPotion())
  {
    return false;
  }

  const unsigned int uiContinueCnt = static_cast<unsigned int>(static_cast<int>(fEffectValue));
  CRaceBuffManager *raceBuffManager = CRaceBuffManager::Instance();
  const int nResult = raceBuffManager->m_kBuffByHolyQuest.CancelPlayerRaceBuff(
    player,
    CRaceBuffInfoByHolyQuestfGroup::RESULT_LOSE,
    uiContinueCnt);
  if (nResult <= 0)
  {
    char szMsg[4]{};
    *reinterpret_cast<int *>(szMsg) = nResult;
    unsigned __int8 pbyType[2]{17, 37};
    g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, szMsg, 4u);
  }
  player->SetUseReleaseRaceBuffPotion();
  return true;
}
