#pragma once

class CCharacter;

bool __fastcall DE_AttHPtoDstFP(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ContDamageTimeInc(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Recovery(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_HPInc(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_STInc(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ContHelpTimeInc(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_OverHealing(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_LateContHelpSkillRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_LateContHelpForceRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_LateContDamageRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_AllContHelpSkillRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_AllContHelpForceRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_AllContDamageForceRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_OthersContHelpSFRemove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_SkillContHelpTimeInc(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ConvertMonsterTarget(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_TransMonsterHP(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ViewWeakPoint(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ReleaseMonsterTarget(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_MakeGuardTower(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_LayTrap(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_DetectTrap(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_IncHPCircleParty(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Stun(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_SPDec(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_FPDec(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_DamStun(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_TransDestHP(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_RemoveAllContHelp(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_MakePortalReturnBindPositionPartyMember(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ReturnBindPosition(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_IncreaseDP(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_ConvertTargetDest(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_SelfDestruction(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_RecallPartyMember(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_HP_In_Value(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_FP_In_Value(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_SP_In_Value(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_DecHalfSFContDam(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_AllContHelpSkillRemove_Once(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_RemoveAllContinousEffect(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Chaos_Inc_Time(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Chaos_Dec_Time(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Class_Refine(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_RemoveAfterEffect(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_CharReName(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_RecallCommonPlayer(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_TeleportCommonPlayer(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Exp_Increase_Percentage(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Exp_Increase_Absolute(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Revival_Die_Position(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Buf_Extend(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Trunk_Extend(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Race_Debuff_Clear_One(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Race_Debuff_Clear_Two(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Gold_Point(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Recall_After_Stone(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Teleport_After_Stone(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Quick_Revival_Die_Position(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_HFP_Full_Recover(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Cont_Damage_Remove(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_BattleMode_RecallCommonPlayer(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
