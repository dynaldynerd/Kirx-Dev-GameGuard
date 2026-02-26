#include "pch.h"

#include "GlobalObjects.h"

#include "CCRFG_SEND_BUFFER.h"
#include "CMapDisplay.h"
#include "CGameServerDoc.h"
#include "RFAcc.h"
#include "TempEffectFunctions.h"

#include <cstdarg>

CUserDB g_UserDB[MAX_PLAYER]{};
CPartyPlayer g_PartyPlayer[MAX_PLAYER]{};
CPlayer g_Player[MAX_PLAYER]{};
CPlayer sPlayerDum{};

CMonster *g_Monster = nullptr;
CMerchant *g_NPC = nullptr;
CAnimus *g_Animus = nullptr;
CGuardTower *g_Tower = nullptr;
CHolyStone *g_Stone = nullptr;
CHolyKeeper *g_Keeper = nullptr;
CTrap *g_Trap = nullptr;
CItemBox *g_ItemBox = nullptr;
CParkingUnit *g_ParkingUnit = nullptr;
CDarkHole *g_DarkHole = nullptr;
CGuild *g_Guild = nullptr;
AggroCaculateData g_AggroCaculateData{};
MonsterSetInfoData g_MonsterSetInfoData{};

_R3ENGINE_STATE stState{};
int dword_184A79820 = 0;

CTransportShip g_TransportShip[3]{};
CLogFile g_logSchedule{};
CHolyStoneSystem g_HolySys{};
CWorldSchedule g_WorldSch{};
CGameStatistics g_GameStatistics{};
_ECONOMY_SYSTEM e_EconomySystem{};
_economy_history_data e_EconomyHistory[12]{};
unsigned int e_dwMinCount = 0;
unsigned int e_dwUserCumCount[3]{};
int e_nOreDft = 0;
int e_nTaxDft = 0;
int e_nMgrDft = 0;
CVoteSystem g_VoteSys[3]{};
CMainThread g_Main{};
CRFAcc g_RFAcc{};
CNetworkEX g_Network{};
CMapDisplay g_MapDisplay{};
int gAsci[256]{};
CWnd *g_pFrame = nullptr;
CGameServerDoc *g_pDoc = nullptr;
CLogFile g_FgLogFile{};
_CCRFG_SEND_BUFFER g_FGSendData{};
_CCRFG_RECV_BUFFER g_FGRecvData{};
_CCRFG_CALLBACK g_pfnCallBack{};
unsigned __int8 g_cbHashVerify[32] = {
  0xB1, 0x77, 0xCC, 0xCA, 0x32, 0x1F, 0x34, 0x80, 0x99, 0x07, 0xF9, 0x42, 0x92, 0x5C, 0x0C, 0x4C,
  0x23, 0x1B, 0x8E, 0xBC, 0xBB, 0x42, 0xCD, 0xFC, 0x9A, 0x24, 0x0B, 0x2C, 0x55, 0x28, 0xA1, 0xF8};
unsigned __int8 g_key[16] = {
  0x49, 0x79, 0xC2, 0x04, 0x32, 0xC2, 0x3D, 0xD4, 0x3D, 0x86, 0xF2, 0x7E, 0x33, 0xDE, 0x43, 0x9D};
unsigned __int8 g_iv[16] = {
  0x08, 0x90, 0xCC, 0x90, 0x2D, 0x69, 0x45, 0x04, 0x10, 0x6C, 0xE4, 0xF9, 0xAA, 0x96, 0xD3, 0x79};
float g_fPoint[2]{};

int g_tmpEffectedNum = 0;
_tmp_effected_list g_tmpEffectedList[30]{};
bool (__fastcall *g_TempEffectFunc[150])(CCharacter *, CCharacter *, float, unsigned __int8 *) = {
  // 0..57
  DE_AttHPtoDstFP,
  DE_ContDamageTimeInc,
  DE_Recovery,
  DE_HPInc,
  DE_STInc,
  DE_ContHelpTimeInc,
  DE_OverHealing,
  DE_LateContHelpSkillRemove,
  DE_LateContHelpForceRemove,
  DE_LateContDamageRemove,
  DE_AllContHelpSkillRemove,
  DE_AllContHelpForceRemove,
  DE_AllContDamageForceRemove,
  DE_OthersContHelpSFRemove,
  DE_SkillContHelpTimeInc,
  DE_ConvertMonsterTarget,
  DE_TransMonsterHP,
  DE_ViewWeakPoint,
  DE_ReleaseMonsterTarget,
  DE_MakeGuardTower,
  DE_LayTrap,
  DE_DetectTrap,
  DE_IncHPCircleParty,
  DE_Stun,
  DE_SPDec,
  DE_FPDec,
  DE_DamStun,
  DE_TransDestHP,
  DE_RemoveAllContHelp,
  DE_MakePortalReturnBindPositionPartyMember,
  DE_ReturnBindPosition,
  DE_IncreaseDP,
  DE_ConvertTargetDest,
  DE_RecoverAllReturnStateAnimusHPFull,
  DE_MakeZeroAnimusRecallTimeOnce,
  DE_SelfDestruction,
  DE_RecallPartyMember,
  DE_Potion_HP_In_Value,
  DE_Potion_FP_In_Value,
  DE_Potion_SP_In_Value,
  DE_Potion_DecHalfSFContDam,
  DE_Potion_AllContHelpSkillRemove_Once,
  DE_Potion_RemoveAllContinousEffect,
  DE_Potion_Chaos_Inc_Time,
  DE_Potion_Chaos_Dec_Time,
  DE_Potion_Class_Refine,
  DE_Potion_RemoveAfterEffect,
  DE_Potion_CharReName,
  DE_RecallCommonPlayer,
  DE_TeleportCommonPlayer,
  DE_Potion_Exp_Increase_Percentage,
  DE_Potion_Exp_Increase_Absolute,
  DE_Potion_Revival_Die_Position,
  DE_Potion_Buf_Extend,
  DE_Potion_Trunk_Extend,
  DE_Potion_Race_Debuff_Clear_One,
  DE_Potion_Race_Debuff_Clear_Two,
  DE_Potion_Gold_Point,
  // 58..69
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  // 70..75
  DE_Recall_After_Stone,
  DE_Teleport_After_Stone,
  DE_Quick_Revival_Die_Position,
  DE_Potion_HFP_Full_Recover,
  DE_Potion_Cont_Damage_Remove,
  DE_BattleMode_RecallCommonPlayer,
};

namespace
{
CLogFile g_animusDebugLog{};
bool g_animusDebugLogInitialized = false;

void EnsureAnimusDebugLog()
{
  if (g_animusDebugLogInitialized)
  {
    return;
  }

  CreateDirectoryA(".\\ZoneServerLog", nullptr);
  g_animusDebugLog.SetWriteLogFile(".\\ZoneServerLog\\AnimusDebug.log", 1, false, true, true);
  g_animusDebugLogInitialized = true;
}
} // namespace

void AnimusDebugLog(const char *format, ...)
{
  if (!format)
  {
    return;
  }

  EnsureAnimusDebugLog();
  va_list argList{};
  va_start(argList, format);
  g_animusDebugLog.WriteFromArg(format, argList);
  va_end(argList);
}

float ITEM_ROOT_RATE = 1.0f;
float MINE_SPEED_RATE = 1.0f;
float FORCE_LIVER_ACCUM_RATE = 1.0f;
float MASTERY_GET_RATE = 1.0f;
float ANIMUS_EXP_RATE = 1.0f;
float PLAYER_EXP_RATE = 1.0f;
float PLAYER_LOST_EXP = 1.0f;
float TSVR_ADD_DARKHOLE_REWARD_RATE = 1.0f;
unsigned __int8 Major_Bind_HQ = 0;
unsigned __int8 Major_Sette_Mine_Elan_Map = 0;
unsigned __int8 Major_Scroll_Item = 0;
unsigned __int8 Major_Cash_Item = 0;
unsigned __int8 Major_Add_Character = 0;
unsigned int g_dwCurTime = 0;
