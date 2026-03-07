#include "pch.h"

#include "CheatCommands.h"
#include "CHEAT_COMMAND.h"
#include "GlobalObjects.h"
#include "CPlayer.h"
#include "CChatStealSystem.h"
#include "COreAmountMgr.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CGuildBattleController.h"
#include "CWorldSchedule.h"
#include "CUnmannedTraderController.h"
#include "CHolyStoneSystem.h"
#include "CRaceBossMsgController.h"
#include "CMonsterEventRespawn.h"
#include "CMonsterEventSet.h"
#include "CRecordData.h"
#include "CMapData.h"
#include "CMerchant.h"
#include "CMonster.h"
#include "CItemStore.h"
#include "CItemUpgradeTable.h"
#include "ItemUpgrade_fld.h"
#include "CTotalGuildRankManager.h"
#include "CWeeklyGuildRankManager.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CHonorGuild.h"
#include "CashItemRemoteStore.h"
#include "CPcBangFavor.h"
#include "CGoldenBoxItemMgr.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "combine_ex_item_result_zocl.h"
#include "GuildBattle.h"
#include "GuildBattleTypes.h"
#include "CGravityStone.h"
#include "CGravityStoneRegener.h"
#include "ObjectCreateSetData.h"
#include "PatriarchElectProcessor.h"
#include "CandidateMgr.h"
#include "AutoMineMachineMng.h"
#include "CCouponMgr.h"
#include "RFEvent_ClassRefine.h"
#include "CLuaScriptMgr.h"
#include "CLuaCommand.h"
#include "CCashDBWorkManager.h"
#include "CTSingleton.h"
#include "CNationSettingManager.h"
#include "cStaticMember_Player.h"
#include "guild_manage_request_clzo.h"
#include "param_cash.h"
#include "pnt_rect.h"
#include "eq_suk_list.h"
#include "CLogFile.h"
#include "WorldServerUtil.h"

#include <ctime>

namespace {
constexpr int kCheatWordCount = 6;
constexpr int kCheatWordSize = 32;
char g_CheatWordBuf[kCheatWordCount][kCheatWordSize] = {};
}

int s_nWordCount = 0;
char *s_pwszDstCheat[kCheatWordCount] = {
  g_CheatWordBuf[0],
  g_CheatWordBuf[1],
  g_CheatWordBuf[2],
  g_CheatWordBuf[3],
  g_CheatWordBuf[4],
  g_CheatWordBuf[5],
};

char wszRespon[256] = {};
CLogFile s_logCheat;

bool CHEAT_COMMAND::operator()(const CHEAT_COMMAND *lhs, const CHEAT_COMMAND *rhs)
{
  return lhs->uiCmdLen > rhs->uiCmdLen;
}

bool AuthorityFilter(CHEAT_COMMAND *pCmd, CPlayer *pOne)
{
  if (pOne)
  {
    if(pOne->m_byUserDgr == 0) return false;
    if(pOne->m_byUserDgr == 255) return true;
    if (g_Main.IsReleaseServiceMode())
    {
      const unsigned int useMask = 1u << pOne->m_byUserDgr;
      if ((useMask & static_cast<unsigned int>(pCmd->nUseDegree)) == 0)
      {
        return false;
      }

      if (pOne->m_byUserDgr == 2)
      {
        const unsigned int mgrMask = 1u << pOne->m_bySubDgr;
        if ((mgrMask & static_cast<unsigned int>(pCmd->nMgrDegree)) == 0)
        {
          return false;
        }
      }
    }
  }
  else if ((pCmd->nUseDegree & 0x20) == 0)
  {
    return false;
  }

  return true;
}

bool ProcessCheatCommand(CPlayer *pOne, char *pwszCommand)
{
  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  CHEAT_COMMAND *cheatTable = nationSetting->GetCheatTable();

  if (strchr(pwszCommand, '%'))
  {
    sprintf(wszRespon, "%s >> FAIL(grammar or logic)", pwszCommand);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
    return true;
  }

  for (int j = 0; ; ++j)
  {
    CHEAT_COMMAND *command = &cheatTable[j];
    if (!command->pwszCommand)
    {
      break;
    }

    if (!_strnicmp(command->pwszCommand, pwszCommand, command->uiCmdLen))
    {
      if (AuthorityFilter(command, pOne))
      {
        s_nWordCount = ParsingCommandW(&pwszCommand[command->uiCmdLen], 6, s_pwszDstCheat, 32);
        if (command->fn(pOne))
        {
          WriteCheatLog(pwszCommand, pOne);
          if (pOne)
          {
            sprintf(wszRespon, "%s >> OK", pwszCommand);
            pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
          }
          return true;
        }

        if (pOne)
        {
          sprintf(wszRespon, "%s >> FAIL(grammar or logic)", pwszCommand);
          pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
        }
        return false;
      }

      sprintf(wszRespon, "%s >> ERROR (authority)", pwszCommand);
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
      return false;
    }
  }

  if (pOne)
  {
    sprintf(wszRespon, "%s >> ERROR (command)", pwszCommand);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
  }

  return false;
}

void WriteCheatLog(char *pwszCommand, CPlayer *pOne)
{
  char buffer[1284]{};
  memset_0(buffer, 0, 1280);

  if (pOne)
  {
    const char *charName = pOne->m_Param.GetCharNameA();
    sprintf(buffer, "[ %s ] >> ", charName);
  }
  else
  {
    sprintf(buffer, "[ GM tool ] >> ");
  }

  const int offset = static_cast<int>(strlen_0(buffer));
  W2M(pwszCommand, &buffer[offset], 1280 - offset);
  s_logCheat.Write(buffer);
}

void InitCheatCommand(CHEAT_COMMAND *pCmdList, unsigned __int8 *byCommandSizeList)
{
  for (int j = 0; ; ++j)
  {
    CHEAT_COMMAND *command = &pCmdList[j];
    if (!command->pwszCommand)
    {
      break;
    }
    byCommandSizeList[j] = static_cast<unsigned __int8>(strlen_0(command->pwszCommand));
  }

  const unsigned int korLocalTime = GetKorLocalTime();
  char buffer[144]{};
  sprintf(buffer, "..\\ZoneServerLog\\ServiceLog\\Cheat%u.log", korLocalTime);
  s_logCheat.SetWriteLogFile(buffer, 1, 0, 1, 1);
}

static const char aA_44[] = "<unknown>";
static const char aA_45[] = "<unknown>";
static const char aA_46[] = "<unknown>";
static const char aA_47[] = "<unknown>";
static const char aA_48[] = "<unknown>";
static const char aA_49[] = "<unknown>";
static const char aA_50[] = "<unknown>";
static const char aCheatHelpSynta_0[] = "Cheat Help : Syntax = timeset ";
static const char aCheatHelpSynta_1[] = "Cheat Help : Syntax = tlinfoset Ãƒâ€¡Ãƒâ€¡";
static const char aCheatHelpSynta_2[] = "Cheat Help : Syntax = %actpset Ãƒâ€¡ÃƒÂ Ãƒâ‚¬";
static const char aCheatHelpSynta_3[] = "Cheat Help : Syntax = %eventset ";
static const char aOne_3[] = "<unknown>";
static const char aCoacE[] = "Ãƒâ€¡ÃƒÂ¶Ãƒâ‚¬ÃƒÂ§ ÃƒË†";
static const char aCoacCodeDBoxAc[] = "Ãƒâ€¡ÃƒÂ¶Ãƒâ‚¬ÃƒÂ§ [Code_%d Box] Ãƒâ‚¬Ãƒâ€¡ Ãƒâ‚¬ÃƒÅ“";
static const char aCoacE_0[] = "Ãƒâ€¡ÃƒÂ¶Ãƒâ‚¬ÃƒÂ§ ÃƒË†";
static const char aE_2[] = "<unknown>";
static const char aAi_8[] = "<unknown>";
static const char aAo_6[] = "<unknown>";
static const char aAo_7[] = "<unknown>";
static const char aAua_0[] = "<unknown>";
static const char byte_1407AE5D0[] = "<unknown>";
static const char byte_1407AE5E8[] = "<unknown>";
static const char byte_1407AE5F8[] = "<unknown>";
static const char byte_1407AE600[] = "<unknown>";
static const char byte_1407AE608[] = "<unknown>";
static const char byte_1407AE660[] = "<unknown>";
static const char byte_1407AE680[] = "<unknown>";
static const char byte_1407AE6E0[] = "<unknown>";
static const char byte_1407AE6F0[] = "<unknown>";
static const char byte_1407AE700[] = "<unknown>";
static const char byte_1407AE980[] = "<unknown>";
static const char byte_1407AE998[] = "<unknown>";
static const char byte_1407AE9A0[] = "<unknown>";
static const char byte_1407AE9B0[] = "<unknown>";
static const char byte_1407AEA8C[] = "<unknown>";
static const char byte_1407AEA9C[] = "<unknown>";
static const char byte_1407AF0F8[] = "<unknown>";
static const char byte_1407AF104[] = "<unknown>";
static const char byte_1407AF110[] = "<unknown>";
static char unk_1407AE840[] = "<unknown>";
static char unk_1407AE858[] = "<unknown>";
static char unk_1407AE868[] = "<unknown>";
static char unk_1407AE878[] = "<unknown>";
static char unk_1407AE888[] = "<unknown>";
static constexpr float FLOAT_100_0 = 100.0f;

static const _eq_suk_list EqSukList[4] = {
  {"weapon", 0, 0},
  {"defense", 1, 0},
  {"shield", 2, 0},
  {"skill", 3, 0},
};

static GUILD_BATTLE::CNormalGuildBattleField *GetBattleFieldFromPlayer(
  CPlayer *player,
  GUILD_BATTLE::CNormalGuildBattle **battleOut = nullptr)
{
  if (battleOut)
  {
    *battleOut = nullptr;
  }
  if (!player || !player->m_Param.m_pGuild)
  {
    return nullptr;
  }

  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  if (!manager)
  {
    return nullptr;
  }

  GUILD_BATTLE::CNormalGuildBattle *battle =
    manager->GetBattleByGuildSerial(player->m_Param.m_pGuild->m_dwSerial);
  if (battleOut)
  {
    *battleOut = battle;
  }
  return battle ? battle->m_pkField : nullptr;
}

static bool CheatRegenStoneOnField(
  GUILD_BATTLE::CNormalGuildBattleField *field,
  CPlayer *player,
  int regenIndex,
  int *portalInx)
{
  if (portalInx)
  {
    *portalInx = -1;
  }
  if (!field || !field->m_pkBall || !field->m_pkMap || !field->m_pkRegenPos || field->m_uiRegenPosCnt == 0)
  {
    return false;
  }

  if (regenIndex < 0)
  {
    regenIndex = 0;
  }
  if (regenIndex >= static_cast<int>(field->m_uiRegenPosCnt))
  {
    return false;
  }

  CGravityStoneRegener *regen = &field->m_pkRegenPos[regenIndex];
  if (!regen || !regen->m_pkRegenPos)
  {
    return false;
  }

  _object_create_setdata createData{};
  createData.m_pRecordSet = nullptr;
  createData.m_pMap = field->m_pkMap;
  createData.m_nLayerIndex = player ? player->m_wMapLayerIndex : 0;
  createData.m_fStartPos[0] = regen->m_pkRegenPos->m_fCenterPos[0];
  createData.m_fStartPos[1] = regen->m_pkRegenPos->m_fCenterPos[1];
  createData.m_fStartPos[2] = regen->m_pkRegenPos->m_fCenterPos[2];

  field->m_pkBall->Clear();
  if (!field->m_pkBall->Regen(&createData))
  {
    return false;
  }

  if (portalInx)
  {
    *portalInx = regen->GetPortalInx();
  }
  return true;
}

static bool CheatAssignStoneToPlayer(GUILD_BATTLE::CNormalGuildBattleField *field, CPlayer *player)
{
  if (!field || !field->m_pkBall || !player || !field->m_pkBall->m_bLive)
  {
    return false;
  }
  field->m_pkBall->SetOwner(player);
  return true;
}

static bool CheatHolyStopBattle()
{
  if (g_HolySys.m_SaveData.m_nSceneCode != 1)
  {
    return false;
  }
  g_HolySys.m_dwCheckTime[1] = GetLoopTime();
  return true;
}

static bool CheatHolyKeeperStart(int nKeeperState, int nRace, unsigned int nPassTime)
{
  if (g_HolySys.m_SaveData.m_nSceneCode < 2 || g_HolySys.m_SaveData.m_nSceneCode >= 6)
  {
    return false;
  }
  g_HolySys.m_SaveData.m_nHolyMasterRace = nRace;
  g_HolySys.SetScene(g_HolySys.m_SaveData.m_byNumOfTime, nKeeperState, nPassTime, 1);
  return true;
}

static bool CheatHolyState(CPlayer *player)
{
  if (!player)
  {
    return false;
  }

  char chat[256]{};
  sprintf(
    chat,
    "Holy scene=%d, schedule=%u, master=%d, destroyer=%u",
    g_HolySys.GetSceneCode(),
    static_cast<unsigned int>(g_HolySys.GetNumOfTime()),
    g_HolySys.GetHolyMasterRace(),
    g_HolySys.GetDestroyerSerial());
  player->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, chat, 0xFFu, 0LL);

  sprintf(
    chat,
    "Start %04u-%02u-%02u %02u:%02u",
    static_cast<unsigned int>(g_HolySys.GetStartYear()),
    static_cast<unsigned int>(g_HolySys.GetStartMonth()),
    static_cast<unsigned int>(g_HolySys.GetStartDay()),
    static_cast<unsigned int>(g_HolySys.GetStartHour()),
    static_cast<unsigned int>(g_HolySys.GetStartMin()));
  player->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, chat, 0xFFu, 0LL);
  return true;
}

static bool ParsePercentCheatValue(float *outPercent)
{
  if (s_nWordCount < 1)
  {
    return false;
  }

  float percent = atof(s_pwszDstCheat[0]);
  if (percent <= 0.0f)
  {
    return false;
  }
  if (percent > 100.0f)
  {
    percent = 100.0f;
  }

  *outPercent = percent;
  return true;
}

bool __fastcall ct_change_degree(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  const int degree = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_change_degree(degree);
}

bool __fastcall ct_respawn_start(CPlayer *pOne)
{
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool started; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 32);
  started = g_MonsterEventRespawn.StartRespawnEvent(szTran, pwszErrCode);
  if ( !pOne )
    return 0;
  if ( started )
    sprintf(wszRespon, "Start %s", s_pwszDstCheat[0]);
  else
    sprintf(wszRespon, "Start Error %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_respawn_stop(CPlayer *pOne)
{
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool stopped; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 32);
  stopped = g_MonsterEventRespawn.StopRespawnEvent(szTran, pwszErrCode);
  if ( !pOne )
    return 0;
  if ( stopped )
    sprintf(wszRespon, "Stop %s", s_pwszDstCheat[0]);
  else
    sprintf(wszRespon, "Stop Error %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_circle_mon_kill(CPlayer *pOne)
{

  return pOne && pOne->dev_all_kill();
}

bool __fastcall ct_look_like_boss(CPlayer *pOne)
{

  return pOne && pOne->dev_view_boss();
}

bool __fastcall ct_change_mastery(CPlayer *pOne)
{
  int j; // [rsp+20h] [rbp-28h]
  int nLv; // [rsp+38h] [rbp-10h]
  int nMasteryIndex; // [rsp+3Ch] [rbp-Ch]

  if ( !pOne )
    return 0;
  if ( s_nWordCount == 2 )
  {
    for ( j = 0; j < 4; ++j )
    {
      if ( !strcmp_0(EqSukList[j].pwszEpSuk, s_pwszDstCheat[0]) )
      {
        const int masteryLevel = atoi(s_pwszDstCheat[1]);
        return pOne->dev_up_mastery(EqSukList[j].nCode, EqSukList[j].nIndex, masteryLevel);
      }
    }
    return 0;
  }
  else if ( s_nWordCount < 3 )
  {
    return 0;
  }
  else
  {
    nLv = atoi(s_pwszDstCheat[2]);
    nMasteryIndex = atoi(s_pwszDstCheat[1]);
    const int masteryCode = atoi(s_pwszDstCheat[0]);
    return pOne->dev_up_mastery(masteryCode, nMasteryIndex, nLv);
  }
}

bool __fastcall ct_all_item_muzi(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  const int level = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_all_item_muzi(level);
}

bool __fastcall ct_report_cri_hp(CPlayer *pOne)
{
  return pOne != 0LL;
}

bool __fastcall ct_report_position(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  sprintf(
    wszRespon,
    "map: %s x: %d, y: %d, z: %d",
    pOne->m_pCurMap->m_pMapSet->m_strCode,
    (int)pOne->m_fCurPos[0],
    (int)pOne->m_fCurPos[1],
    (int)pOne->m_fCurPos[2]);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_alter_exp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  const double alterExp = atof(s_pwszDstCheat[0]);
  pOne->AlterExp(alterExp, 1, 0, 0);
  return 1;
}

bool __fastcall ct_all_map(CPlayer *pOne)
{
  int RecordNum; // eax
  int n; // [rsp+30h] [rbp-18h]
  _base_fld *Record; // [rsp+38h] [rbp-10h]

  if ( !pOne )
    return 0;
  for ( n = 0; ; ++n )
  {
    RecordNum = g_Main.m_tblItemData[21].GetRecordNum();
    if ( n >= RecordNum )
      break;
    Record = g_Main.m_tblItemData[21].GetRecord(n);
    pOne->dev_loot_item(Record->m_strCode, 1, 0LL, 0);
  }
  return 1;
}

bool __fastcall ct_jump_to_pos(CPlayer *pOne)
{
  char szTran[64]; // [rsp+38h] [rbp-A0h] BYREF
  char pszMapCode[56]; // [rsp+78h] [rbp-60h] BYREF
  float fZ; // [rsp+B0h] [rbp-28h]
  float fY; // [rsp+B4h] [rbp-24h]
  float dstZ; // [rsp+B8h] [rbp-20h]
  float dstY; // [rsp+BCh] [rbp-1Ch]
  float targetZ; // [rsp+C0h] [rbp-18h]
  float targetY; // [rsp+C4h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount == 4 )
  {
    W2M(s_pwszDstCheat[0], szTran, 32);
    fZ = (float)atoi(s_pwszDstCheat[3]);
    fY = (float)atoi(s_pwszDstCheat[2]);
    const float x = (float)atoi(s_pwszDstCheat[1]);
    return pOne->mgr_gotoCoordinates(szTran, x, fY, fZ);
  }
  else if ( s_nWordCount <= 4 )
  {
    return 0;
  }
  else
  {
    W2M(s_pwszDstCheat[1], pszMapCode, 32);
    if ( !strcmp_0(s_pwszDstCheat[0], "#") )
    {
      dstZ = (float)atoi(s_pwszDstCheat[4]);
      dstY = (float)atoi(s_pwszDstCheat[3]);
      const float dstX = (float)atoi(s_pwszDstCheat[2]);
      return pOne->mgr_gotoDstCoordinates(0LL, pszMapCode, dstX, dstY, dstZ);
    }
    else
    {
      targetZ = (float)atoi(s_pwszDstCheat[4]);
      targetY = (float)atoi(s_pwszDstCheat[3]);
      const float targetX = (float)atoi(s_pwszDstCheat[2]);
      return pOne->mgr_gotoDstCoordinates(s_pwszDstCheat[0], pszMapCode, targetX, targetY, targetZ);
    }
  }
}

bool __fastcall ct_pass_dungeon(CPlayer *pOne)
{

  return pOne && pOne->mgr_dungeon_pass();
}

bool __fastcall ct_tracing_hide(CPlayer *pOne)
{

  return pOne && pOne->mgr_tracing(1);
}

bool __fastcall ct_tracing_show(CPlayer *pOne)
{

  return pOne && pOne->mgr_tracing(0);
}

bool __fastcall ct_max_attack(CPlayer *pOne)
{

  return pOne && pOne->mgr_MaxAttackPoint(1);
}

bool __fastcall ct_min_attack(CPlayer *pOne)
{

  return pOne && pOne->mgr_MaxAttackPoint(-1);
}

bool __fastcall ct_mormal_attack(CPlayer *pOne)
{

  return pOne && pOne->mgr_MaxAttackPoint(0);
}

bool __fastcall ct_set_matchless(CPlayer *pOne)
{

  return pOne && pOne->mgr_matchless(1);
}

bool __fastcall ct_release_matchless(CPlayer *pOne)
{

  return pOne && pOne->mgr_matchless(0);
}

bool __fastcall ct_goto_shipport_town(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int shipPortIndex = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_goto_shipport(shipPortIndex, 0);
}

bool __fastcall ct_goto_shipport_eder(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int shipPortIndex = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_goto_shipport(shipPortIndex, 1);
}

bool __fastcall ct_free_ride_ship(CPlayer *pOne)
{

  return pOne && pOne->mgr_free_ride_ship();
}

bool __fastcall ct_pass_sch(CPlayer *pOne)
{

  return pOne && pOne->mgr_pass_sch_one_step();
}

bool __fastcall ct_start_cri(CPlayer *pOne)
{
  int nNumOfTime; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  nNumOfTime = 0;
  if ( s_nWordCount >= 1 )
    nNumOfTime = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_holystone_start(nNumOfTime);
}

bool __fastcall ct_exip_keeper(CPlayer *pOne)
{

  return pOne && pOne->mgr_exit_keeper();
}

bool __fastcall ct_start_keeper(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int raceCode = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_holykeeper_start(raceCode);
}

bool __fastcall ct_goto_stone(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const unsigned __int8 stoneIndex = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_goto_stone(stoneIndex);
}

bool __fastcall ct_exit_stone(CPlayer *pOne)
{

  return pOne && pOne->mgr_exit_stone();
}

bool __fastcall ct_set_loot_free(CPlayer *pOne)
{

  return pOne && pOne->dev_loot_free(1);
}

bool __fastcall ct_release_loot_free(CPlayer *pOne)
{

  return pOne && pOne->dev_loot_free(0);
}

bool __fastcall ct_change_class(CPlayer *pOne)
{
  char szTran[56]; // [rsp+28h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  W2M(s_pwszDstCheat[0], szTran, 32);
  return pOne->dev_change_class(szTran);
}

bool __fastcall ct_kick_player(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0(s_pwszDstCheat[0], "#") )
    return pOne->mgr_kick(0LL);
  return pOne->mgr_kick(s_pwszDstCheat[0]);
}

bool __fastcall ct_complete_quest(CPlayer *pOne)
{

  return pOne && pOne->dev_quest_complete();
}

bool __fastcall ct_recall_player(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  return pOne->mgr_recall_player(s_pwszDstCheat[0]);
}

bool __fastcall ct_recall_monster(CPlayer *pOne)
{
  int nCreateNum; // [rsp+20h] [rbp-68h]
  char szTran[56]; // [rsp+38h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nCreateNum = 1;
  if ( s_nWordCount >= 2 )
    nCreateNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 32);
  return pOne->mgr_recall_mon(szTran, nCreateNum);
}

bool __fastcall ct_goto_mine(CPlayer *pOne)
{

  return pOne && pOne->mgr_goto_mine();
}

bool __fastcall ct_copy_avator(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  return pOne->dev_avator_copy(s_pwszDstCheat[0]);
}

bool __fastcall ct_user_num(CPlayer *pOne)
{
  char Buffer[144]; // [rsp+50h] [rbp-A8h] BYREF

  if ( !pOne )
    return 0;
  memset(Buffer, 0, 128);
  sprintf(
    Buffer,
    "ConnectUserNum : %d (Field: %d) (B:%d, C:%d, A:%d)",
    CUserDB::s_nLoginNum,
    CPlayer::s_nLiveNum,
    CPlayer::s_nRaceNum[0],
    CPlayer::s_nRaceNum[1],
    CPlayer::s_nRaceNum[2]);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_die(CPlayer *pOne)
{

  return pOne && pOne->dev_die();
}

bool __fastcall ct_half_gauge(CPlayer *pOne)
{

  return pOne && pOne->dev_half_point();
}

bool __fastcall ct_full_gauge(CPlayer *pOne)
{

  return pOne && pOne->dev_full_point();
}

bool __fastcall ct_alter_inven_dur(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const unsigned __int64 durability = _strtoui64(s_pwszDstCheat[0], 0LL, 10);
  return pOne->dev_half_inven_amount(durability);
}

bool __fastcall ct_set_never_die(CPlayer *pOne)
{

  return pOne && pOne->dev_never_die(1);
}

bool __fastcall ct_release_never_die(CPlayer *pOne)
{

  return pOne && pOne->dev_never_die(0);
}

bool __fastcall ct_free_sf_by_class(CPlayer *pOne)
{

  return pOne && pOne->dev_free_sf_by_class();
}

bool __fastcall ct_alter_lv(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int level = atoi(s_pwszDstCheat[0]);
  return pOne->dev_lv(level);
}

bool __fastcall ct_up_forceitem(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int upgradeValue = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_forceitem(upgradeValue);
}

bool __fastcall ct_up_skill(CPlayer *pOne)
{
  char szTran[56]; // [rsp+28h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  W2M(s_pwszDstCheat[0], szTran, 32);
  const int level = atoi(s_pwszDstCheat[1]);
  return pOne->dev_up_skill(szTran, level);
}

bool __fastcall ct_up_forcemastery(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int level = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_forcemastery(level);
}

bool __fastcall ct_up_allskill(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int level = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_all(level);
}

bool __fastcall ct_alter_pvp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const double pvpPoint = (double)atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_pvp(pvpPoint);
}

bool __fastcall ct_alter_cashbag(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const double cashBag = (double)atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_cashbag(cashBag);
}

bool __fastcall ct_animus_recall_term(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const bool disableRecallDelay = atoi(s_pwszDstCheat[0]) == 0;
  return pOne->dev_animus_recall_time_free(disableRecallDelay);
}

bool __fastcall ct_set_animus_exp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const unsigned __int64 animusExp = _strtoui64(s_pwszDstCheat[0], 0LL, 10);
  return pOne->dev_set_animus_exp(animusExp);
}

bool __fastcall ct_init_monster(CPlayer *pOne)
{

  return pOne && pOne->dev_init_monster();
}

bool __fastcall ct_full_force(CPlayer *pOne)
{

  return pOne && pOne->dev_full_force();
}

bool __fastcall ct_loot_material(CPlayer *pOne)
{

  return pOne && pOne->dev_loot_material();
}

bool __fastcall ct_loot_tower(CPlayer *pOne)
{

  return pOne && pOne->dev_loot_tower();
}

bool __fastcall ct_loot_bag(CPlayer *pOne)
{

  return pOne && pOne->dev_loot_bag();
}

bool __fastcall ct_loot_mine(CPlayer *pOne)
{

  return pOne && pOne->dev_loot_mine();
}

bool __fastcall ct_inven_empty(CPlayer *pOne)
{

  return pOne && pOne->dev_inven_empty();
}

bool __fastcall ct_set_make_succ(CPlayer *pOne)
{

  return pOne && pOne->dev_make_succ(1);
}

bool __fastcall ct_release_make_succ(CPlayer *pOne)
{

  return pOne && pOne->dev_make_succ(0);
}

bool __fastcall ct_cont_effet_time(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const unsigned int effectTime = atoi(s_pwszDstCheat[0]);
  return pOne->dev_cont_effect_time(effectTime);
}

bool __fastcall ct_cont_effet_clear(CPlayer *pOne)
{

  return pOne && pOne->dev_cont_effect_del();
}

bool __fastcall ct_view_method(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount >= 1 )
    return pOne->dev_view_method(s_pwszDstCheat[0]);
  if ( s_nWordCount )
    return 0;
  return pOne->dev_view_method(0LL);
}

bool __fastcall ct_make_system_tower(CPlayer *pOne)
{
  char szTran[56]; // [rsp+28h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  W2M(s_pwszDstCheat[0], szTran, 32);
  return pOne->mgr_make_system_tower(szTran);
}

bool __fastcall ct_destroy_system_tower(CPlayer *pOne)
{

  return pOne && pOne->mgr_destroy_system_tower();
}

bool __fastcall ct_circle_user_num(CPlayer *pOne)
{
  int UseSectorRange; // eax
  _DWORD raceCounts[7]; // [rsp+48h] [rbp-140h] BYREF
  int totalCount; // [rsp+64h] [rbp-124h]
  _pnt_rect pRect; // [rsp+78h] [rbp-110h] BYREF
  _sec_info *SecInfo; // [rsp+98h] [rbp-F0h]
  int j; // [rsp+A0h] [rbp-E8h]
  int k; // [rsp+A4h] [rbp-E4h]
  unsigned int dwSecIndex; // [rsp+A8h] [rbp-E0h]
  CObjectList *SectorListPlayer; // [rsp+B0h] [rbp-D8h]
  _object_list_point *m_pNext; // [rsp+B8h] [rbp-D0h]
  CGameObject *m_pItem; // [rsp+C0h] [rbp-C8h]
  unsigned int raceIndex; // [rsp+C8h] [rbp-C0h]
  char Buffer[144]; // [rsp+E0h] [rbp-A8h] BYREF
  int nSecNum; // [rsp+170h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( pOne->GetCurSecNum() == -1 )
    return 0;
  memset(raceCounts, 0, 12);
  totalCount = 0;
  SecInfo = pOne->m_pCurMap->GetSecInfo();
  nSecNum = static_cast<int>(pOne->GetCurSecNum());
  UseSectorRange = static_cast<int>(pOne->GetUseSectorRange());
  pOne->m_pCurMap->GetRectInRadius(&pRect, UseSectorRange, nSecNum);
  for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
  {
    for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
    {
      dwSecIndex = SecInfo->m_nSecNumW * j + k;
      SectorListPlayer = pOne->m_pCurMap->GetSectorListPlayer(pOne->m_wMapLayerIndex, dwSecIndex);
      if ( SectorListPlayer )
      {
        m_pNext = SectorListPlayer->m_Head.m_pNext;
        while ( m_pNext != &SectorListPlayer->m_Tail )
        {
          m_pItem = m_pNext->m_pItem;
          m_pNext = m_pNext->m_pNext;
          raceIndex = static_cast<unsigned int>(m_pItem->GetObjRace());
          if ( raceIndex < 3 )
          {
            ++raceCounts[raceIndex];
            ++totalCount;
          }
        }
      }
    }
  }
  sprintf(Buffer, "Circle: Total( %d ), B( %d ), C( %d ), A( %d )", totalCount, raceCounts[0], raceCounts[1], raceCounts[2]);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_boss_sms_cancel(CPlayer *pOne)
{
  unsigned int dwMsgID; // [rsp+20h] [rbp-18h]
  int raceCode; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  dwMsgID = atoi(s_pwszDstCheat[1]);
  raceCode = atoi(s_pwszDstCheat[0]);
  CRaceBossMsgController *controller = CRaceBossMsgController::Instance();
  return controller->Cancel(static_cast<unsigned __int8>(raceCode), dwMsgID, pOne) != 0;
}

bool __fastcall ct_trunk_init(CPlayer *pOne)
{

  return pOne && pOne->mgr_TrunkInit();
}

bool __fastcall ct_fullset(CPlayer *pOne)
{
  int nLv; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nLv = atoi(s_pwszDstCheat[0]);
  if ( nLv < 2 || nLv > 35 )
    return 0;
  pOne->dev_lv(nLv);
  pOne->dev_up_all(400000000);
  if ( !g_Main.IsReleaseServiceMode() )
    pOne->dev_dalant(-1);
  pOne->dev_loot_fullitem(nLv);
  return 1;
}

bool __fastcall ct_loot_upgrade(CPlayer *pOne)
{
  signed int Size; // eax
  unsigned int dwIndex; // [rsp+50h] [rbp-C8h]
  _ItemUpgrade_fld *Record; // [rsp+58h] [rbp-C0h]
  unsigned __int16 *RecordByHash; // [rsp+60h] [rbp-B8h]
  _STORAGE_LIST::_db_con pItem; // [rsp+78h] [rbp-A0h] BYREF
  unsigned __int16 *upgradeBoxItem; // [rsp+B8h] [rbp-60h]
  _STORAGE_LIST::_db_con upgradeBox; // [rsp+C8h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  for ( dwIndex = 0; ; ++dwIndex )
  {
    Size = g_Main.m_tblItemUpgrade.m_tblItemUpgrade.GetRecordNum();
    if ( (int)dwIndex >= Size )
      break;
    Record = g_Main.m_tblItemUpgrade.GetRecord(dwIndex);
    if ( !Record )
      continue;
    RecordByHash = (unsigned __int16 *)g_Main.m_tblItemData[18].GetRecordByHash(Record->m_strCode, 2, 5);
    if ( RecordByHash )
    {
      pItem = _STORAGE_LIST::_db_con();
      pItem.m_byTableCode = 18;
      pItem.m_wItemIndex = *RecordByHash;
      pItem.m_dwDur = 99LL;
      pItem.m_dwLv = 268435455;
      if ( !CreateItemBox(
              &pItem,
              pOne,
              -1,
              0,
              0LL,
              2u,
              pOne->m_pCurMap,
              pOne->m_wMapLayerIndex,
              pOne->m_fCurPos,
              1) )
        return 1;
    }
  }
  upgradeBoxItem = (unsigned __int16 *)g_Main.m_tblItemData[11].GetRecordByHash("immmm04", 2, 5);
  if ( !upgradeBoxItem )
    return 1;
  upgradeBox = _STORAGE_LIST::_db_con();
  upgradeBox.m_byTableCode = 11;
  upgradeBox.m_wItemIndex = *upgradeBoxItem;
  upgradeBox.m_dwDur = GetItemDurPoint(11, dwIndex);
  upgradeBox.m_dwLv = 268435455;
  if ( CreateItemBox(&upgradeBox, pOne, -1, 0, 0LL, 2u, pOne->m_pCurMap, pOne->m_wMapLayerIndex, pOne->m_fCurPos, 1) )
    return 1;
  else
    return 1;
}

bool __fastcall ct_loot_dungeon(CPlayer *pOne)
{
  int RecordNum; // eax
  int n; // [rsp+50h] [rbp-68h]
  unsigned __int16 *Record; // [rsp+58h] [rbp-60h]
  _STORAGE_LIST::_db_con pItem; // [rsp+68h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  for ( n = 0; ; ++n )
  {
    RecordNum = g_Main.m_tblItemData[23].GetRecordNum();
    if ( n >= RecordNum )
      break;
    Record = (unsigned __int16 *)g_Main.m_tblItemData[23].GetRecord(n);
    if ( !Record )
      break;
    pItem = _STORAGE_LIST::_db_con();
    pItem.m_byTableCode = 23;
    pItem.m_wItemIndex = *Record;
    pItem.m_dwDur = 1LL;
    pItem.m_dwLv = 268435455;
    if ( !CreateItemBox(
            &pItem,
            pOne,
            -1,
            0,
            0LL,
            2u,
            pOne->m_pCurMap,
            pOne->m_wMapLayerIndex,
            pOne->m_fCurPos,
            1) )
      return 1;
  }
  return 1;
}

bool __fastcall ct_defense_item_grace(CPlayer *pOne)
{
  unsigned __int8 equipPartCode; // [rsp+20h] [rbp-18h]
  unsigned __int8 equipPartCodeCopy; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  equipPartCode = static_cast<unsigned __int8>(-1);
  if ( !strcmp_0("upper", s_pwszDstCheat[0]) )
  {
    equipPartCode = 0;
  }
  else if ( !strcmp_0("", s_pwszDstCheat[0]) )
  {
    equipPartCode = 1;
  }
  else if ( !strcmp_0("Ã¥Â°Â©", s_pwszDstCheat[0]) )
  {
    equipPartCode = 2;
  }
  else if ( !strcmp_0("shoe", s_pwszDstCheat[0]) )
  {
    equipPartCode = 3;
  }
  else if ( !strcmp_0("helmet", s_pwszDstCheat[0]) )
  {
    equipPartCode = 4;
  }
  else if ( !strcmp_0("shield", s_pwszDstCheat[0]) )
  {
    equipPartCode = 5;
  }
  else if ( !strcmp_0("cloak", s_pwszDstCheat[0]) )
  {
    equipPartCode = 7;
  }
  else if ( !strcmp_0("all", s_pwszDstCheat[0]) )
  {
    equipPartCode = 37;
  }
  equipPartCodeCopy = equipPartCode;
  if ( equipPartCode > 5u && equipPartCodeCopy != 7 && equipPartCodeCopy != 37 )
    return 0;
  const int graceLevel = atoi(s_pwszDstCheat[1]);
  return pOne->mgr_defense_item_grace(equipPartCode, graceLevel);
}

bool __fastcall ct_defense_item_grace_Jp(CPlayer *pOne)
{
  unsigned __int8 equipPartCode; // [rsp+20h] [rbp-18h]
  unsigned __int8 equipPartCodeCopy; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  equipPartCode = static_cast<unsigned __int8>(-1);
  if ( !strcmp_0("upper", s_pwszDstCheat[0]) )
  {
    equipPartCode = 0;
  }
  else if ( !strcmp_0("lower", s_pwszDstCheat[0]) )
  {
    equipPartCode = 1;
  }
  else if ( !strcmp_0("gauntlet", s_pwszDstCheat[0]) )
  {
    equipPartCode = 2;
  }
  else if ( !strcmp_0("shoe", s_pwszDstCheat[0]) )
  {
    equipPartCode = 3;
  }
  else if ( !strcmp_0("helmet", s_pwszDstCheat[0]) )
  {
    equipPartCode = 4;
  }
  else if ( !strcmp_0("shield", s_pwszDstCheat[0]) )
  {
    equipPartCode = 5;
  }
  else if ( !strcmp_0("cloak", s_pwszDstCheat[0]) )
  {
    equipPartCode = 7;
  }
  else if ( !strcmp_0("all", s_pwszDstCheat[0]) )
  {
    equipPartCode = 37;
  }
  equipPartCodeCopy = equipPartCode;
  if ( equipPartCode > 5u && equipPartCodeCopy != 7 && equipPartCodeCopy != 37 )
    return 0;
  const int graceLevel = atoi(s_pwszDstCheat[1]);
  return pOne->mgr_defense_item_grace(equipPartCode, graceLevel);
}

bool __fastcall ct_animus_attack_grade(CPlayer *pOne)
{
  int nPoint; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nPoint = 0;
  if ( !strcmp_0("max", s_pwszDstCheat[0]) )
  {
    nPoint = 1;
  }
  else if ( !strcmp_0("min", s_pwszDstCheat[0]) )
  {
    nPoint = -1;
  }
  else
  {
    if ( strcmp_0("normal", s_pwszDstCheat[0]) )
      return 0;
    nPoint = 0;
  }
  return pOne->mgr_set_animus_attack_point(nPoint);
}

bool __fastcall ct_telekinesis(CPlayer *pOne)
{

  return pOne && pOne->mgr_item_telekinesis();
}

bool __fastcall ct_up_allskill_pt(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  const int point = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_all_pt(point);
}

bool __fastcall ct_complete_quest_other(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  return pOne->dev_quest_complete_other(s_pwszDstCheat[0]);
}

bool __fastcall ct_loot_upgrade_item(CPlayer *pOne)
{
  char szTran[64]; // [rsp+38h] [rbp-D0h] BYREF
  char Str1[64]; // [rsp+78h] [rbp-90h] BYREF
  char Destination[44]; // [rsp+B8h] [rbp-50h] BYREF
  int nNum; // [rsp+E4h] [rbp-24h]
  int nUpNum; // [rsp+E8h] [rbp-20h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 3 )
    return 0;
  nUpNum = 1;
  if ( s_nWordCount >= 4 )
    nUpNum = atoi(s_pwszDstCheat[3]);
  nNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 32);
  W2M(s_pwszDstCheat[2], Str1, 32);
  if ( !strcmp_0(Str1, byte_1407AE660) )
  {
    strcpy_0(Destination, "irtal01");
  }
  else if ( !strcmp_0(Str1, aA_44) )
  {
    strcpy_0(Destination, "irtal02");
  }
  else if ( !strcmp_0(Str1, byte_1407AE680) )
  {
    strcpy_0(Destination, "irtal03");
  }
  else if ( !strcmp_0(Str1, aE_2) )
  {
    strcpy_0(Destination, "irtal04");
  }
  else if ( !strcmp_0(Str1, aAo_7) )
  {
    strcpy_0(Destination, "irtal05");
  }
  else if ( !strcmp_0(Str1, "") )
  {
    strcpy_0(Destination, "irtal06");
  }
  else if ( !strcmp_0(Str1, "") )
  {
    strcpy_0(Destination, "irtal07");
  }
  else if ( !strcmp_0(Str1, aA_45) )
  {
    strcpy_0(Destination, "irtal08");
  }
  else if ( !strcmp_0(Str1, byte_1407AE6E0) )
  {
    strcpy_0(Destination, "irtal09");
  }
  else if ( !strcmp_0(Str1, byte_1407AE6F0) )
  {
    strcpy_0(Destination, "irtal10");
  }
  else if ( !strcmp_0(Str1, byte_1407AE700) )
  {
    strcpy_0(Destination, "irtal11");
  }
  else if ( !strcmp_0(Str1, "") )
  {
    strcpy_0(Destination, "irtal12");
  }
  else if ( !strcmp_0(Str1, "ÃšÂº") )
  {
    strcpy_0(Destination, "irtal13");
  }
  else
  {
    if ( strcmp_0(Str1, "ÃˆÂ°") )
      return 0;
    strcpy_0(Destination, "irtal14");
  }
  return pOne->dev_loot_item(szTran, nNum, Destination, nUpNum);
}

bool __fastcall ct_resurrect_player(CPlayer *pOne)
{
  CPvpPointLimiter result; // [rsp+20h] [rbp-28h] BYREF
  CPvpPointLimiter *PvpPointLimiter; // [rsp+30h] [rbp-18h]
  if ( !pOne )
    return 0;
  if ( s_nWordCount >= 1 )
  {
    if ( strcmp_0(aA_46, s_pwszDstCheat[0]) )
      return pOne->mgr_resurrect_player(s_pwszDstCheat[0]);
    PvpPointLimiter = pOne->GetPvpPointLimiter(&result);
    PvpPointLimiter->CheatUpdate(pOne->m_Param.m_dbChar.m_dPvPPoint);
  }
  return pOne->pc_Resurrect(0);
}

bool __fastcall ct_add_guild_schedule(CPlayer *pOne)
{
  char Buffer[136]; // [rsp+50h] [rbp-C8h] BYREF
  CGuild *pSrcGuild; // [rsp+D8h] [rbp-40h]
  CGuild *pDestGuild; // [rsp+E0h] [rbp-38h]
  unsigned int dwStartTime; // [rsp+E8h] [rbp-30h]
  int battleType; // [rsp+ECh] [rbp-2Ch]
  unsigned int dwMapInx; // [rsp+F0h] [rbp-28h]
  unsigned __int8 addResult; // [rsp+F4h] [rbp-24h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 5 )
    return 0;
  pSrcGuild = GetGuildPtrFromName(g_Guild, 500, s_pwszDstCheat[0]);
  if ( pSrcGuild )
  {
    pDestGuild = GetGuildPtrFromName(g_Guild, 500, s_pwszDstCheat[1]);
    if ( pDestGuild )
    {
      dwStartTime = atoi(s_pwszDstCheat[2]) - 1;
      if ( (int)dwStartTime > 0 )
      {
        battleType = atoi(s_pwszDstCheat[3]);
        dwMapInx = atoi(s_pwszDstCheat[4]);
        if ( (dwStartTime & 0x80000000) == 0 )
        {
          CGuildBattleController *controller = CGuildBattleController::Instance();
          addResult = controller->Add(
            pSrcGuild,
            pDestGuild,
            dwStartTime,
            static_cast<unsigned __int8>(battleType),
            dwMapInx);
          sprintf(Buffer, "Add GuildBattle Schedule : %u", addResult);
          pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
          return addResult == 0;
        }
        else
        {
          return 0;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
      sprintf(Buffer, "Invalid Dest Guild : %s", s_pwszDstCheat[1]);
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
      return 0;
    }
  }
  else
  {
    sprintf(Buffer, "Invalid Src Guild : %s", s_pwszDstCheat[0]);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 0;
  }
}

bool __fastcall ct_set_guildbattle_color(CPlayer *pOne)
{
  unsigned int colorIndex; // [rsp+40h] [rbp-C8h]
  char Buffer[144]; // [rsp+60h] [rbp-A8h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  colorIndex = atoi(s_pwszDstCheat[0]);
  if ( colorIndex == static_cast<unsigned int>(-1) )
  {
    pOne->m_bInGuildBattle = 0;
    pOne->m_byGuildBattleColorInx = -1;
    sprintf(Buffer, "Clear GuildBattle Color!");
  }
  else if ( colorIndex > 1 )
  {
    sprintf(Buffer, "Invalid Color Inx! ( 0 or 1 or -1 )");
  }
  else
  {
    pOne->m_bInGuildBattle = 1;
    pOne->m_byGuildBattleColorInx = colorIndex;
    sprintf(Buffer, "Set GuildBattle Color : %d", colorIndex);
  }
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_cur_guildbattle_color(CPlayer *pOne)
{
  char Buffer[144]; // [rsp+50h] [rbp-A8h] BYREF

  if ( !pOne )
    return 0;
  sprintf(Buffer, "Guild Battle Currect Color : %d", pOne->m_byGuildBattleColorInx);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_create_guildbattle_field_object(CPlayer *pOne)
{
  if (!pOne)
    return false;
  CGuildBattleController *controller = CGuildBattleController::Instance();
  return controller->CheatCreateFieldObject(pOne);
}

bool __fastcall ct_destroy_guildbattle_field_object(CPlayer *pOne)
{
  if (!pOne)
    return false;
  CGuildBattleController *controller = CGuildBattleController::Instance();
  return controller->CheatDestroyFieldObject(pOne);
}

bool __fastcall ct_regen_gravitystone(CPlayer *pOne)
{
  int portalInx = -1;
  int regenPos = -1;
  char Buffer[144]{};

  if (!pOne)
  {
    return false;
  }
  if (s_nWordCount > 1)
  {
    return false;
  }
  if (s_nWordCount == 1)
  {
    regenPos = atoi(s_pwszDstCheat[0]);
  }

  GUILD_BATTLE::CNormalGuildBattleField *field = GetBattleFieldFromPlayer(pOne);
  if (!CheatRegenStoneOnField(field, pOne, regenPos, &portalInx))
  {
    return false;
  }

  if (s_nWordCount == 1)
  {
    sprintf(Buffer, "Regen Stone(%d) PortalInx : %d", regenPos, portalInx);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  }
  return true;
}

bool __fastcall ct_destroy_gravitystone(CPlayer *pOne)
{
  if (!pOne)
    return false;
  if (s_nWordCount)
    return false;
  CGuildBattleController *controller = CGuildBattleController::Instance();
  return controller->CheatDestroyStone(pOne);
}

bool __fastcall ct_take_gravitystone(CPlayer *pOne)
{
  int iPortalInx;
  int regenInx = -1;
  GUILD_BATTLE::CNormalGuildBattleField *field;

  if (!pOne || s_nWordCount != 1)
  {
    return false;
  }

  iPortalInx = atoi(s_pwszDstCheat[0]);
  field = GetBattleFieldFromPlayer(pOne);
  if (!field || !field->m_pkBall || !field->m_pkRegenPos)
  {
    return false;
  }

  for (unsigned int j = 0; j < field->m_uiRegenPosCnt; ++j)
  {
    if (field->m_pkRegenPos[j].GetPortalInx() == iPortalInx)
    {
      regenInx = static_cast<int>(j);
      break;
    }
  }
  if (regenInx < 0)
  {
    return false;
  }

  if (!field->m_pkBall->m_bLive && !CheatRegenStoneOnField(field, pOne, regenInx, nullptr))
  {
    return false;
  }
  return CheatAssignStoneToPlayer(field, pOne);
}

bool __fastcall ct_get_gravitystone(CPlayer *pOne)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = GetBattleFieldFromPlayer(pOne);
  if (!field || !field->m_pkBall || s_nWordCount)
  {
    return false;
  }
  if (!field->m_pkBall->m_bLive && !CheatRegenStoneOnField(field, pOne, 0, nullptr))
  {
    return false;
  }
  return CheatAssignStoneToPlayer(field, pOne);
}

bool __fastcall ct_drop_gravitystone(CPlayer *pOne)
{
  if (!pOne)
    return false;
  if (s_nWordCount)
    return false;
  CGuildBattleController *controller = CGuildBattleController::Instance();
  return controller->CheatDropStone(pOne);
}

bool __fastcall ct_guild_battle_force_stone(CPlayer *pOne)
{
  if (!pOne)
    return false;
  CGuildBattleController *controller = CGuildBattleController::Instance();
  return controller->CheatForceTakeStone(pOne);
}

bool __fastcall ct_check_guild_batlle_goal(CPlayer *pOne)
{
  GUILD_BATTLE::CNormalGuildBattle *battle = nullptr;
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  int iPortalInx; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  iPortalInx = atoi(s_pwszDstCheat[0]);
field = GetBattleFieldFromPlayer(pOne, &battle);
  if (!field || !field->m_pkBall || !battle)
  {
    return false;
  }
  if (field->m_pkBall->Drop(pOne) != 0)
  {
    return false;
  }
  battle->NotifyDestoryBall(pOne->m_dwObjSerial);
  return true;
}

bool __fastcall ct_recv_reserved_schedulelist(CPlayer *pOne)
{
  int byDay; // [rsp+40h] [rbp-28h]
  int byPage; // [rsp+44h] [rbp-24h]
  unsigned int dwGuildSerial; // [rsp+48h] [rbp-20h]
  unsigned int uiMapID; // [rsp+4Ch] [rbp-1Ch]
  int n; // [rsp+50h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  byDay = atoi(s_pwszDstCheat[0]);
  byPage = atoi(s_pwszDstCheat[1]);
  dwGuildSerial = -1;
  if ( pOne->m_Param.m_pGuild )
    dwGuildSerial = pOne->m_Param.m_pGuild->m_dwSerial;
  uiMapID = pOne->m_Param.GetRaceCode();
  n = pOne->m_ObjID.m_wIndex;

  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *listManager =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  if (!listManager || uiMapID >= listManager->m_uiMapCnt || (byDay != 0 && byDay != 1))
  {
    return false;
  }

  GUILD_BATTLE::CReservedGuildScheduleDayGroup *dayGroup =
    (byDay == 0) ? listManager->m_pkToday : listManager->m_pkTomorrow;
  if (!dayGroup || uiMapID >= dayGroup->m_uiMapCnt)
  {
    return false;
  }

  GUILD_BATTLE::CReservedGuildScheduleMapGroup *mapGroup = &dayGroup->m_pkList[uiMapID];
  if (byPage < 0 || byPage >= mapGroup->m_byMaxPage)
  {
    return false;
  }

  GUILD_BATTLE::CReservedGuildSchedulePage *page = &mapGroup->m_kList[byPage];
  if (!page->m_pkList)
  {
    return false;
  }

  page->m_pkList->bySelfScheduleInx = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 5; ++j)
  {
    if (page->m_dw1PGuildSerial[j] == dwGuildSerial || page->m_dw2PGuildSerial[j] == dwGuildSerial)
    {
      page->m_pkList->bySelfScheduleInx = static_cast<unsigned __int8>(j);
      break;
    }
  }

  unsigned __int8 type[2] = {27, 51};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(page->m_pkList),
    static_cast<unsigned __int16>(sizeof(_guild_battle_reserved_schedule_result_zocl)));
  return true;
}

bool __fastcall ct_recv_current_battle_info(CPlayer *pOne)
{
  unsigned int uiMapID; // [rsp+20h] [rbp-18h]
  int n; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  uiMapID = pOne->m_Param.GetRaceCode();
  n = pOne->m_ObjID.m_wIndex;

  GUILD_BATTLE::CCurrentGuildBattleInfoManager *infoManager =
    GUILD_BATTLE::CCurrentGuildBattleInfoManager::Instance();
  if (!infoManager || uiMapID >= infoManager->m_uiMapCnt || !infoManager->m_pkInfo)
  {
    return false;
  }

  unsigned __int8 type[2] = {27, 52};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&infoManager->m_pkInfo[uiMapID]),
    static_cast<unsigned __int16>(sizeof(_guild_battle_current_battle_info_result_zocl)));
  return true;
}

bool __fastcall ct_add_one_day_guild_schedule(CPlayer *pOne)
{
  char Buffer[132]{};
  unsigned int dwMapInx[3] = {0, 1, 3};
  char *guildNames[6] = {
    unk_1407AE840,
    unk_1407AE858,
    unk_1407AE868,
    unk_1407AE878,
    unk_1407AE888,
    unk_1407AE840,
  };
  unsigned __int8 result = 0;

  if (!pOne)
  {
    return false;
  }

  CGuildBattleController *controller = CGuildBattleController::Instance();
  for (unsigned int dwStartTime = 0; dwStartTime < 22; ++dwStartTime)
  {
    for (int j = 0; j < 3; ++j)
    {
      CGuild *pSrcGuild = GetGuildPtrFromName(g_Guild, MAX_GUILD, guildNames[j * 2]);
      if (!pSrcGuild)
      {
        sprintf(Buffer, "Invalid Src Guild : %s", guildNames[j * 2]);
        pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
        return false;
      }

      CGuild *pDestGuild = GetGuildPtrFromName(g_Guild, MAX_GUILD, guildNames[j * 2 + 1]);
      if (!pDestGuild)
      {
        sprintf(Buffer, "Invalid Dest Guild : %s", guildNames[j * 2 + 1]);
        pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
        return false;
      }

      result = controller->Add(pSrcGuild, pDestGuild, dwStartTime, static_cast<unsigned __int8>(0), dwMapInx[j]);
      if (result)
      {
        break;
      }
    }
  }

  sprintf(Buffer, "Add GuildBattle Schedule : %u", result);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return result == 0;
}

bool __fastcall ct_guild_suggest(CPlayer *pOne)
{
  return pOne != 0LL;
}

bool __fastcall ct_guild_info(CPlayer *pOne)
{
  return pOne != 0LL;
}

bool __fastcall ct_recv_total_guild_rank(CPlayer *pOne)
{
  unsigned int dwVer; // [rsp+20h] [rbp-18h]
  unsigned __int8 pageIndex; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  dwVer = atoi(s_pwszDstCheat[0]);
  pageIndex = atoi(s_pwszDstCheat[1]);
  CTotalGuildRankManager *rankManager = CTotalGuildRankManager::Instance();
  rankManager->Send(dwVer, pageIndex, pOne);
  return 1;
}

bool __fastcall ct_recv_pvp_guild_rank(CPlayer *pOne)
{
  unsigned int dwVer; // [rsp+20h] [rbp-18h]
  unsigned __int8 pageIndex; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  dwVer = atoi(s_pwszDstCheat[0]);
  pageIndex = atoi(s_pwszDstCheat[1]);
  CWeeklyGuildRankManager *rankManager = CWeeklyGuildRankManager::Instance();
  {
    unsigned int guildSerial = static_cast<unsigned int>(-1);
    if (pOne->m_Param.m_pGuild)
    {
      guildSerial = pOne->m_Param.m_pGuild->m_dwSerial;
    }
    const unsigned __int8 selfRace = static_cast<unsigned __int8>(pOne->m_Param.GetRaceCode());
    rankManager->m_kInfo.Send(dwVer, pOne->m_ObjID.m_wIndex, pageIndex, selfRace, guildSerial);
  }
  return 1;
}

bool __fastcall ct_recv_change_atrad_taxrate(CPlayer *pOne)
{
  unsigned int raceCode; // [rsp+20h] [rbp-18h]
  unsigned int dwNewTaxRate; // [rsp+24h] [rbp-14h]
  char *pCheaterName; // [rsp+28h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  raceCode = atoi(s_pwszDstCheat[0]);
  dwNewTaxRate = atoi(s_pwszDstCheat[1]);
  if ( raceCode > 2 )
    return 0;
  pCheaterName = pOne->m_Param.GetCharNameW();
  CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
  taxRateManager->SetPatriarchTaxMoney(static_cast<unsigned __int8>(raceCode), dwNewTaxRate);
return 1;
}

bool __fastcall ct_combine_ex_result(CPlayer *pOne)
{
  _combine_ex_item_result_zocl pSend{};

  if ( !pOne )
    return 0;
  pSend.byErrCode = 0;
  pSend.byDlgType = 1;
  pSend.ItemBuff.byItemListNum = 1;
  pSend.ItemBuff.RewardItemList[0].Key.byRewardIndex = 0;
  pSend.ItemBuff.RewardItemList[0].Key.byTableCode = 6;
  pSend.ItemBuff.RewardItemList[0].Key.wItemIndex = 30;
  pSend.ItemBuff.RewardItemList[0].dwDur = 1;
  pSend.ItemBuff.RewardItemList[0].dwUpt = 2147483632;
  pOne->SendMsg_CombineItemExResult(&pSend);
  return 1;
}

bool __fastcall ct_amp_set(CPlayer *pOne)
{
  unsigned int dwDelay; // [rsp+20h] [rbp-18h]
  unsigned int dwDS; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( !pOne->m_bOper )
    return 0;
  if ( !pOne->m_Param.m_pAPM || !pOne->m_Param.m_pAPM->is_installed() )
    return 0;
  dwDelay = atoi(s_pwszDstCheat[0]);
  dwDS = atoi(s_pwszDstCheat[1]);
  if ( dwDelay )
    pOne->m_Param.m_pAPM->m_dwDelay = dwDelay;
  if ( dwDS )
    pOne->m_Param.m_pAPM->m_dwDelaySec = static_cast<unsigned __int8>(dwDS);
  return 1;
}

bool __fastcall ct_amp_full(CPlayer *pOne)
{
  unsigned __int8 ItemTableCode; // [rsp+30h] [rbp-208h]
  unsigned __int16 *Record; // [rsp+38h] [rbp-200h]
  _STORAGE_LIST::_db_con storageItem; // [rsp+58h] [rbp-1E0h] BYREF
  _personal_amine_mineore_zocl updatePacket; // [rsp+B0h] [rbp-188h] BYREF
  unsigned __int8 pbyType[2]; // [rsp+214h] [rbp-24h] BYREF

  if ( !pOne )
    return 0;
  if ( !pOne->m_bOper )
    return 0;
  if ( !pOne->m_Param.m_pAPM || !pOne->m_Param.m_pAPM->is_installed() )
    return 0;
  ItemTableCode = GetItemTableCode(s_pwszDstCheat[0]);
  if ( ItemTableCode != 17 )
    return 0;
  Record = (unsigned __int16 *)g_Main.m_tblItemData[17].GetRecord(s_pwszDstCheat[0]);
  if ( !Record )
    return 0;
  storageItem = _STORAGE_LIST::_db_con();
  storageItem.m_byTableCode = ItemTableCode;
  storageItem.m_wItemIndex = *Record;
  storageItem.m_dwDur = 99LL;
  while ( pOne->m_Param.m_dbPersonalAmineInven.GetIndexEmptyCon() != 255 )
  {
    storageItem.m_wSerial = pOne->m_Param.GetNewItemSerial();
    storageItem.m_dwT = -1;
    if ( pOne->Emb_AddStorage(6u, &storageItem, 0, 1) )
    {
      updatePacket.clear();
      updatePacket.dwObjSerial = pOne->m_Param.m_pAPM->m_dwObjSerial;
      updatePacket.byChangedNum = 1;
      updatePacket.change[0].wItemIndex = storageItem.m_wItemIndex;
      updatePacket.change[0].wItemSerial = storageItem.m_wSerial;
      updatePacket.change[0].dwDur = storageItem.m_dwDur;
      pbyType[0] = 14;
      pbyType[1] = 55;
      const unsigned __int16 packetSize =
        static_cast<unsigned __int16>(10 + (8 * static_cast<unsigned int>(updatePacket.byChangedNum)));
      g_Network.m_pProcess[0]->LoadSendMsg(
        pOne->m_id.wIndex,
        pbyType,
        reinterpret_cast<char *>(&updatePacket),
        packetSize);
    }
  }
  return 1;
}

bool __fastcall ct_server_time(CPlayer *pOne)
{
  time_t currentTime = 0;
  tm *localTime; // [rsp+58h] [rbp-140h]
  char Buffer[272]; // [rsp+70h] [rbp-128h] BYREF

  if ( !pOne )
    return 0;
  std::time(&currentTime);
  localTime = std::localtime(&currentTime);
  if ( localTime )
    sprintf(
      Buffer,
      "%04d-%02d-%02d %02d:%02d:%02d",
      localTime->tm_year + 1900,
      localTime->tm_mon + 1,
      localTime->tm_mday,
      localTime->tm_hour,
      localTime->tm_min,
      localTime->tm_sec);
  else
    sprintf(Buffer, "::localtime Fail!");
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_party_call(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( !pOne->m_bOper )
    return 0;
  if ( s_nWordCount >= 1 )
    return pOne->mgr_recall_party_player(s_pwszDstCheat[0]);
  return 0;
}

bool __fastcall ct_guild_call(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( !pOne->m_bOper )
    return 0;
  if ( s_nWordCount >= 1 )
    return pOne->mgr_recall_guild_player(s_pwszDstCheat[0]);
  return 0;
}

bool __fastcall ct_loadcashamount(CPlayer *pOne)
{
  int nNum;
  _param_cash_select selectParam(
    pOne->m_pUserDB->m_dwAccountSerial,
    pOne->m_Param.GetCharSerial(),
    pOne->m_ObjID.m_wIndex);
  CCashDBWorkManager *workMgr;

  if ( !pOne || !pOne->m_bOper )
    return 0;
  nNum = atoi(s_pwszDstCheat[0]);
  if ( nNum < 0 || nNum >= CashItemRemoteStore::Instance()->_kRecGoods.GetRecordNum() )
    return 0;
  strcpy_s(selectParam.in_szAcc, sizeof(selectParam.in_szAcc), pOne->m_pUserDB->m_szAccountID);
  workMgr = CTSingleton<CCashDBWorkManager>::Instance();
  return workMgr && workMgr->PushTask(0, reinterpret_cast<unsigned __int8 *>(&selectParam), selectParam.size());
}

bool __fastcall ct_cashitembuy(CPlayer *pOne)
{
  int nNum;

  if ( !pOne || !pOne->m_bOper )
    return 0;
  nNum = atoi(s_pwszDstCheat[1]);
  if ( (unsigned int)nNum >= 100 )
    return 0;
  return 0;
}

bool __fastcall ct_PcBandPrimium(CPlayer *pOne)
{
  if ( !pOne || !pOne->m_bOper )
    return 0;
  const float sharedRate = atof(s_pwszDstCheat[0]);
  if ( sharedRate <= 0.0 || sharedRate > 2.0 )
    return 0;
  const float itemDropRate = atof(s_pwszDstCheat[1]);
  if ( itemDropRate <= 0.0 || itemDropRate > 2.0 )
    return 0;
  PCBANG_PRIMIUM_FAVOR::MINING_SPEED = sharedRate;
  PCBANG_PRIMIUM_FAVOR::PLAYER_EXP = sharedRate;
  PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP = sharedRate;
  PCBANG_PRIMIUM_FAVOR::BASE_MASTERY = sharedRate;
  PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY = sharedRate;
  PCBANG_PRIMIUM_FAVOR::ITEM_DROP = itemDropRate;
  return 1;
}

bool __fastcall ct_ClassRefineEvent(CPlayer *pOne)
{
  _event_config_classrefine eventConfig{};

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 5 )
    return 0;
  eventConfig.bEnable = strncmp(s_pwszDstCheat[0], "true", 4uLL) == 0;
  eventConfig.nMaxRefineCnt = atoi(s_pwszDstCheat[1]);
  if ( eventConfig.nMaxRefineCnt > 256 )
    return 0;
  eventConfig.nStartDate = atoi(s_pwszDstCheat[2]);
  if ( eventConfig.nStartDate < GetLocalDate() )
    return 0;
  eventConfig.nEndDate = atoi(s_pwszDstCheat[3]);
  if ( eventConfig.nEndDate <= GetLocalDate() )
    return 0;
  if ( eventConfig.nStartDate > eventConfig.nEndDate )
    return 0;
  const bool resetUserData = strncmp(s_pwszDstCheat[4], "true", 4uLL) == 0;
  return static_cast<RFEvent_ClassRefine *>(g_Main.m_pRFEvent_ClassRefine)
         ->SetEvent(reinterpret_cast<const char *>(&eventConfig), sizeof(eventConfig), resetUserData)
      != 0;
}

bool __fastcall ct_takeholymental(CPlayer *pOne)
{
  if (!pOne || !pOne->m_bOper)
    return false;
  const char *holyMentalString = g_HolySys.GetHolyMentalString();
  pOne->CheckMentalTakeAndUpdateLastMetalTicket(holyMentalString);
  return true;
}

bool __fastcall ct_HolySystem(CPlayer *pOne)
{
  int scheduleIndex; // [rsp+20h] [rbp-28h]
  int nRace; // [rsp+24h] [rbp-24h]
  int nPassTime; // [rsp+28h] [rbp-20h]
  int keeperRace; // [rsp+2Ch] [rbp-1Ch]
  int keeperPassTime; // [rsp+30h] [rbp-18h]
  int chaosRace; // [rsp+34h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0("start", s_pwszDstCheat[0]) )
  {
    if ( s_nWordCount >= 2 )
    {
      scheduleIndex = atoi(s_pwszDstCheat[1]);
      g_HolySys.AlterSchedule(1u, static_cast<unsigned __int8>(scheduleIndex));
      return 1;
    }
    return 0;
  }
  if ( !strcmp_0("end", s_pwszDstCheat[0]) )
    return g_HolySys.ct_StopBattle();
  if ( strcmp_0("Ã…Â°", s_pwszDstCheat[0]) )
  {
    if ( s_nWordCount >= 1 && !strcmp_0("state", s_pwszDstCheat[0]) )
      return g_HolySys.ct_State(pOne);
    return 0;
  }
  if ( s_nWordCount < 2 )
    return 0;
  if ( !strcmp_0("neutral", s_pwszDstCheat[1]) )
  {
    if ( s_nWordCount >= 3 )
    {
      nRace = atoi(s_pwszDstCheat[2]);
      nPassTime = 0;
      if ( s_nWordCount >= 4 )
        nPassTime = atoi(s_pwszDstCheat[3]);
      return g_HolySys.ct_KeeperStart(3, nRace, static_cast<unsigned int>(nPassTime));
    }
    return 0;
  }
  if ( strcmp_0("invincible", s_pwszDstCheat[1]) )
  {
    if ( !strcmp_0("chaos", s_pwszDstCheat[1]) && s_nWordCount >= 3 )
    {
      chaosRace = atoi(s_pwszDstCheat[2]);
      return g_HolySys.ct_KeeperStart(6, chaosRace, 0);
    }
    return 0;
  }
  if ( s_nWordCount < 3 )
    return 0;
  keeperRace = atoi(s_pwszDstCheat[2]);
  keeperPassTime = 0;
  if ( s_nWordCount >= 4 )
    keeperPassTime = atoi(s_pwszDstCheat[3]);
  return g_HolySys.ct_KeeperStart(4, keeperRace, static_cast<unsigned int>(keeperPassTime));
}

bool __fastcall ct_HolySystem_Jp(CPlayer *pOne)
{
  int scheduleIndex; // [rsp+20h] [rbp-28h]
  int nRace; // [rsp+24h] [rbp-24h]
  int nPassTime; // [rsp+28h] [rbp-20h]
  int keeperRace; // [rsp+2Ch] [rbp-1Ch]
  int keeperPassTime; // [rsp+30h] [rbp-18h]
  int chaosRace; // [rsp+34h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0("start", s_pwszDstCheat[0]) )
  {
    if ( s_nWordCount >= 2 )
    {
      scheduleIndex = atoi(s_pwszDstCheat[1]);
      g_HolySys.AlterSchedule(1u, static_cast<unsigned __int8>(scheduleIndex));
      return 1;
    }
    return 0;
  }
  if ( !strcmp_0("end", s_pwszDstCheat[0]) )
    return g_HolySys.ct_StopBattle();
  if ( strcmp_0("keeper", s_pwszDstCheat[0]) )
  {
    if ( s_nWordCount >= 1 && !strcmp_0("state", s_pwszDstCheat[0]) )
      return g_HolySys.ct_State(pOne);
    return 0;
  }
  if ( s_nWordCount < 2 )
    return 0;
  if ( !strcmp_0("neutral", s_pwszDstCheat[1]) )
  {
    if ( s_nWordCount >= 3 )
    {
      nRace = atoi(s_pwszDstCheat[2]);
      nPassTime = 0;
      if ( s_nWordCount >= 4 )
        nPassTime = atoi(s_pwszDstCheat[3]);
      return g_HolySys.ct_KeeperStart(3, nRace, static_cast<unsigned int>(nPassTime));
    }
    return 0;
  }
  if ( strcmp_0("invincible", s_pwszDstCheat[1]) )
  {
    if ( !strcmp_0("chaos", s_pwszDstCheat[1]) && s_nWordCount >= 3 )
    {
      chaosRace = atoi(s_pwszDstCheat[2]);
      return g_HolySys.ct_KeeperStart(6, chaosRace, 0);
    }
    return 0;
  }
  if ( s_nWordCount < 3 )
    return 0;
  keeperRace = atoi(s_pwszDstCheat[2]);
  keeperPassTime = 0;
  if ( s_nWordCount >= 4 )
    keeperPassTime = atoi(s_pwszDstCheat[3]);
  return g_HolySys.ct_KeeperStart(4, keeperRace, static_cast<unsigned int>(keeperPassTime));
}

bool __fastcall ct_HolyKeeperAttack(CPlayer *pOne)
{
  unsigned int damage;

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  damage = atoi(s_pwszDstCheat[0]);
  g_Keeper->SetDamage(static_cast<int>(damage), pOne, 1, false, -1, 0, true);
  return 1;
}

bool __fastcall ct_SetPatriarchAuto(CPlayer *pOne)
{
  PatriarchElectProcessor *processor;
  int value;

  if ( !pOne || !pOne->m_bOper )
    return 0;
  value = atoi(s_pwszDstCheat[0]);
  processor = PatriarchElectProcessor::Instance();
  if ( value )
  {
    if ( value != 1 )
      return 0;
    processor->SetTimeCheck(true);
  }
  else
  {
    processor->SetTimeCheck(false);
  }
  return 1;
}

bool __fastcall ct_SetPatriarchProcessor(CPlayer *pOne)
{
  PatriarchElectProcessor *processor;
  ElectProcessor::ProcessorType procType;
  int inputProc;

  if ( !pOne || !pOne->m_bOper )
    return 0;
  inputProc = atoi(s_pwszDstCheat[0]);
  if ( inputProc < 0 || inputProc >= 6 )
    return 0;
  procType = static_cast<ElectProcessor::ProcessorType>(inputProc);
  processor = PatriarchElectProcessor::Instance();
  return processor->ForceChangeProcessor(procType);
}

bool __fastcall ct_InformPatriarchProcessor(CPlayer *pOne)
{
  static const char *kProcessorNames[6] = {
    "CandidateRegister",
    "SecondCandidateCrystallizer",
    "Voter",
    "FinalDecisionProcessor",
    "FinalDecisionApplyer",
    "ClassOrderProcessor",
  };
  PatriarchElectProcessor *processor;
  ElectProcessor::ProcessorType processorType;
  char buffer[144];

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(buffer, 0, sizeof(buffer));
  processor = PatriarchElectProcessor::Instance();
  processorType = processor->GetProcessorType();
  if ( processorType == ElectProcessor::_eNonProcessor )
  {
    sprintf(buffer, "Current active processor : %s", "None");
  }
  else
  {
    sprintf(buffer, "Current active processor : %s", kProcessorNames[static_cast<int>(processorType)]);
  }
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_PvpLimitInit(CPlayer *pOne)
{
  CPvpPointLimiter result; // [rsp+20h] [rbp-28h] BYREF
  CPvpPointLimiter *PvpPointLimiter; // [rsp+30h] [rbp-18h]
  PvpPointLimiter = pOne->GetPvpPointLimiter(&result);
  PvpPointLimiter->CheatUpdate(pOne->m_Param.m_dbChar.m_dPvPPoint);
  return 1;
}

bool __fastcall ct_set_animus_lv(CPlayer *pOne)
{
  if ( s_nWordCount > 2 )
    return 0;
  const int animusLevel = atoi(s_pwszDstCheat[0]);
  return pOne->dev_set_animus_lv(animusLevel);
}

bool __fastcall ct_full_animus_gauge(CPlayer *pOne)
{

  return s_nWordCount <= 1 && pOne->dev_full_animus_gauge();
}

bool __fastcall ct_NuAfterEffect(CPlayer *pOne)
{

  if ( !pOne->m_bOper && pOne->m_bCorpse )
    return 0;
  pOne->dev_after_effect();
  return 1;
}

bool __fastcall ct_CdeEndup(CPlayer *pOne)
{
  CashItemRemoteStore *store;
store = CashItemRemoteStore::Instance();
  if ( store->m_cde.m_ini.m_bUseCashDiscount )
  {
    store->m_cde.m_cde_status = 5;
    store->m_cde.m_ini.m_bUseCashDiscount = 0;
    store->m_cde.m_cde_log.Write("A Event which was Registated is ended up by perforce");
  }
  return 1;
}

bool __fastcall ct_remove_sf_delay(CPlayer *pOne)
{
  if ( !pOne || !pOne->m_bOper )
    return 0;
  pOne->m_bSFDelayNotCheck = !pOne->m_bSFDelayNotCheck;
  return 1;
}

bool __fastcall ct_ut_cancel_registlogout(CPlayer *pOne)
{
  if ( !ct_ut_cancel_regist(pOne) )
    return 0;
  g_Network.Close(0, pOne->m_ObjID.m_wIndex, false, nullptr);
  return 1;
}

bool __fastcall ct_ut_cancel_regist(CPlayer *pOne)
{
  CUnmannedTraderController *controller;
  unsigned __int8 nth;

  nth = static_cast<unsigned __int8>(-1);
  if ( s_nWordCount == 1 )
  {
    nth = static_cast<unsigned __int8>(atoi(s_pwszDstCheat[0]));
  }
  else if ( s_nWordCount )
  {
    return 0;
  }
  controller = CUnmannedTraderController::Instance();
  return controller->CheatCancelRegist(pOne->m_ObjID.m_wIndex, pOne->m_dwObjSerial, nth);
}

bool __fastcall ct_CashEventStart(CPlayer *pOne)
{
  CashItemRemoteStore *store;
  std::time_t now;
  std::tm beginTm{};
  std::tm endTm{};
  int iBegin_TT; // [rsp+38h] [rbp-30h]
  int iB30_TT[3]; // [rsp+3Ch] [rbp-2Ch] BYREF
  unsigned __int8 byEventType; // [rsp+54h] [rbp-14h]

  if ( !pOne )
    return 0;
  iBegin_TT = 0;
  memset(iB30_TT, 0, sizeof(iB30_TT));
  iBegin_TT = atoi(s_pwszDstCheat[0]);
  iB30_TT[0] = atoi(s_pwszDstCheat[1]);
  iB30_TT[1] = atoi(s_pwszDstCheat[2]);
  iB30_TT[2] = atoi(s_pwszDstCheat[3]);
  byEventType = atoi(s_pwszDstCheat[4]);
  if ( iBegin_TT <= 0 || iB30_TT[0] <= 0 || iB30_TT[1] <= 0 || iB30_TT[2] <= 0 || byEventType > 1u )
    return 0;
  store = CashItemRemoteStore::Instance();
  now = std::time(nullptr);

  for ( int j = 0; j < 3; ++j )
    store->m_cash_event[j].m_ini.m_bUseCashEvent = (j == byEventType);

  store->m_cash_event[byEventType].m_ini.m_EventTime[0] = static_cast<int>(now) + iBegin_TT;
  store->m_cash_event[byEventType].m_ini.m_EventTime[1] = static_cast<int>(now) + iBegin_TT + iB30_TT[0] + iB30_TT[1] + iB30_TT[2];
  store->m_cash_event[byEventType].m_event_inform_before[0] = iB30_TT[0] + iB30_TT[1];
  store->m_cash_event[byEventType].m_event_inform_before[1] = iB30_TT[0];
  std::time_t beginTime = static_cast<std::time_t>(store->m_cash_event[byEventType].m_ini.m_EventTime[0]);
  std::time_t endTime = static_cast<std::time_t>(store->m_cash_event[byEventType].m_ini.m_EventTime[1]);
  beginTm = *std::localtime(&beginTime);
  endTm = *std::localtime(&endTime);
  store->m_cash_event[byEventType].m_ini.m_wYear[0] = beginTm.tm_year + 1900;
  store->m_cash_event[byEventType].m_ini.m_byMonth[0] = beginTm.tm_mon + 1;
  store->m_cash_event[byEventType].m_ini.m_byDay[0] = beginTm.tm_mday;
  store->m_cash_event[byEventType].m_ini.m_byHour[0] = beginTm.tm_hour;
  store->m_cash_event[byEventType].m_ini.m_byMinute[0] = beginTm.tm_min;
  store->m_cash_event[byEventType].m_ini.m_wYear[1] = endTm.tm_year + 1900;
  store->m_cash_event[byEventType].m_ini.m_byMonth[1] = endTm.tm_mon + 1;
  store->m_cash_event[byEventType].m_ini.m_byDay[1] = endTm.tm_mday;
  store->m_cash_event[byEventType].m_ini.m_byHour[1] = endTm.tm_hour;
  store->m_cash_event[byEventType].m_ini.m_byMinute[1] = endTm.tm_min;
  store->m_cash_event[byEventType].m_event_status = 1;
  return 1;
}

bool __fastcall ct_chatsave(CPlayer *pOne)
{
  if ( !pOne || !pOne->m_bOper )
    return 0;
  CChatStealSystem *chatStealSystem = CChatStealSystem::Instance();
  if ( !chatStealSystem->SetGm(pOne) )
    return 0;
  if ( s_nWordCount >= 1 )
  {
    const int raceOrBossCode = atoi(s_pwszDstCheat[1]);
    if ( !strcmp_0("char", s_pwszDstCheat[0]) )
    {
      if ( s_nWordCount >= 3 )
      {
        if ( !strcmp_0(byte_1407AEA8C, s_pwszDstCheat[1]) )
          return chatStealSystem->SetTargetInfoFromCharacter(2u, s_pwszDstCheat[2]);
        if ( !strcmp_0(aA_49, s_pwszDstCheat[1]) )
          return chatStealSystem->SetTargetInfoFromCharacter(4u, s_pwszDstCheat[2]);
        if ( !strcmp_0(byte_1407AEA9C, s_pwszDstCheat[1]) )
          return chatStealSystem->SetTargetInfoFromCharacter(3u, s_pwszDstCheat[2]);
        if ( !strcmp_0(aAi_8, s_pwszDstCheat[1]) )
          return chatStealSystem->SetTargetInfoFromCharacter(1u, s_pwszDstCheat[2]);
        if ( !strcmp_0(aAua_0, s_pwszDstCheat[1]) )
          return chatStealSystem->SetTargetInfoFromCharacter(5u, s_pwszDstCheat[2]);
      }
    }
    else if ( !strcmp_0("race", s_pwszDstCheat[0]) )
    {
      if ( s_nWordCount >= 2 )
        return chatStealSystem->SetTargetInfoFromRace(7u, static_cast<unsigned __int8>(raceOrBossCode));
    }
    else if ( !strcmp_0("boss", s_pwszDstCheat[0]) )
    {
      if ( s_nWordCount >= 2 )
        return chatStealSystem->SetTargetInfoFromBoss(6u, static_cast<unsigned __int8>(raceOrBossCode));
    }
    else if ( !strcmp_0("off", s_pwszDstCheat[0]) )
    {
      return chatStealSystem->SetGm(nullptr);
    }
  }
  return 0;
}

bool __fastcall ct_drop_jade(CPlayer *pOne)
{
  int nNum; // [rsp+30h] [rbp-68h]
  char szTran[56]; // [rsp+48h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nNum = 1;
  if ( s_nWordCount >= 2 )
    nNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 32);
  return pOne->dev_drop_item(szTran, nNum, nullptr);
}

bool __fastcall ct_set_ore_amount(CPlayer *pOne)
{
  unsigned int dwTot; // [rsp+20h] [rbp-18h]
  unsigned int dwRemain; // [rsp+24h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  dwTot = 0;
  dwRemain = 0;
  if ( s_nWordCount > 2 )
    return 0;
  if ( s_nWordCount == 1 )
  {
    dwRemain = atoi(s_pwszDstCheat[0]);
  }
  else
  {
    if ( s_nWordCount != 2 )
      return 0;
    dwTot = atoi(s_pwszDstCheat[0]);
    dwRemain = atoi(s_pwszDstCheat[1]);
  }
  COreAmountMgr *oreAmountManager = COreAmountMgr::Instance();
  return oreAmountManager->CheatOreAmount(dwTot, dwRemain);
}

bool __fastcall ct_query_remain_ore(CPlayer *pOne)
{
  unsigned int RemainOre; // eax
  char Buffer[136]; // [rsp+50h] [rbp-B8h] BYREF
  float *MultipleRate; // [rsp+D8h] [rbp-30h]
  int j; // [rsp+E0h] [rbp-28h]
  int k; // [rsp+E4h] [rbp-24h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 128);
  COreAmountMgr *oreAmountManager = COreAmountMgr::Instance();
  MultipleRate = oreAmountManager->GetMultipleRate();
  if ( MultipleRate )
  {
    for ( j = 0; j < 7; ++j )
    {
      for ( k = 0; k < 3; ++k )
      {
        if ( *MultipleRate != 1.0 )
        {
          sprintf(Buffer, "%s_%d : %0.f", dayofweek[j], k + 1, (float)(*MultipleRate * 100.0));
          pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
        }
        ++MultipleRate;
      }
    }
  }
  RemainOre = oreAmountManager->GetRemainOre();
  sprintf(Buffer, "Remain Ore : %d", RemainOre);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_continue_palytime_inc(CPlayer *pOne)
{
  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount > 2 )
    return 0;
  const int continuePlayTime = atoi(s_pwszDstCheat[0]);
  return pOne->m_kPcBangCoupon.SetCheetContTime(pOne->m_ObjID.m_wIndex, continuePlayTime);
}

bool __fastcall ct_server_rate(CPlayer *pOne)
{
  char Buffer[272]; // [rsp+60h] [rbp-138h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 255);
  sprintf_s(
    Buffer,
    255,
    "Item Looting: %f Percent\n"
    "Mine Speed: %f Percent\n"
    "Force Liver: %f Percent\n"
    "Mastery: %f Percent\n"
    "Animus Exp: %f Percent\n"
    "Character Exp: %f Percent\n"
    "Darkhole Reward: %f Percent",
    (float)(ITEM_ROOT_RATE * 100.0),
    (float)(MINE_SPEED_RATE * 100.0),
    (float)(FORCE_LIVER_ACCUM_RATE * 100.0),
    (float)(MASTERY_GET_RATE * 100.0),
    (float)(ANIMUS_EXP_RATE * 100.0),
    (float)(PLAYER_EXP_RATE * 100.0),
    (float)(TSVR_ADD_DARKHOLE_REWARD_RATE * 100.0));
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_premium_rate(CPlayer *pOne)
{
  char Buffer[272]; // [rsp+60h] [rbp-128h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 255);
  sprintf_s(
    Buffer,
    255,
    "PC Item Looting: %f Percent\n"
    "PC Mine Speed: %f Percent\n"
    "PC Force Liver: %f Percent\n"
    "PC Mastery: %f Percent\n"
    "PC Animus Exp: %f Percent\n"
    "PC Character Exp: %f Percent\n",
    (float)(PCBANG_PRIMIUM_FAVOR::ITEM_DROP * 100.0),
    (float)(PCBANG_PRIMIUM_FAVOR::MINING_SPEED * 100.0),
    (float)(PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY * 100.0),
    (float)(PCBANG_PRIMIUM_FAVOR::BASE_MASTERY * 100.0),
    (float)(PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP * 100.0),
    (float)(PCBANG_PRIMIUM_FAVOR::PLAYER_EXP * 100.0));
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_trap_attack_grade(CPlayer *pOne)
{
  int nPoint; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nPoint = 0;
  if ( !strcmp_0("max", s_pwszDstCheat[0]) )
  {
    nPoint = 1;
  }
  else if ( !strcmp_0("min", s_pwszDstCheat[0]) )
  {
    nPoint = -1;
  }
  else
  {
    if ( strcmp_0("normal", s_pwszDstCheat[0]) )
      return 0;
    nPoint = 0;
  }
  return pOne->dev_trap_attack_grade(nPoint);
}

bool __fastcall ct_set_damage_part(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0("body", s_pwszDstCheat[0]) )
  {
    pOne->m_byDamagePart = 0;
  }
  else if ( !strcmp_0("leg", s_pwszDstCheat[0]) )
  {
    pOne->m_byDamagePart = 1;
  }
  else if ( !strcmp_0("hand", s_pwszDstCheat[0]) )
  {
    pOne->m_byDamagePart = 2;
  }
  else if ( !strcmp_0("foot", s_pwszDstCheat[0]) )
  {
    pOne->m_byDamagePart = 3;
  }
  else if ( !strcmp_0("head", s_pwszDstCheat[0]) )
  {
    pOne->m_byDamagePart = 4;
  }
  else
  {
    if ( strcmp_0("random", s_pwszDstCheat[0]) )
      return 0;
    pOne->m_byDamagePart = -1;
  }
  return 1;
}

bool __fastcall ct_eventset_start(CPlayer *pOne)
{
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool started; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 32);
  if ( !pOne )
    return 0;
  started = g_MonsterEventSet->StartEventSet(szTran, pwszErrCode, pOne);
  if ( started )
    sprintf(wszRespon, "Event Set Start Success %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  else
    sprintf(wszRespon, "Event Set Start Error %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return started;
}

bool __fastcall ct_eventset_stop(CPlayer *pOne)
{
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool stopped; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 32);
  stopped = g_MonsterEventSet->StopEventSet(szTran, pwszErrCode);
  if ( !pOne )
    return 0;
  if ( stopped )
    sprintf(wszRespon, "Event Set Stop %s", s_pwszDstCheat[0]);
  else
    sprintf(wszRespon, "Event Set Stop Error %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return stopped;
}

bool __fastcall ct_set_temp_cash_point(CPlayer *pOne)
{
  CPvpOrderView *PvpOrderView; // rax
  long double dTempPvpCash; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount > 2 )
    return 0;
  dTempPvpCash = (double)atoi(s_pwszDstCheat[0]);
  PvpOrderView = pOne->GetPvpOrderView();
  PvpOrderView->Update_PvpTempCash(pOne->m_ObjID.m_wIndex, static_cast<double>(dTempPvpCash));
  return 1;
}

bool __fastcall ct_set_kill_list_init(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  pOne->m_kPvpCashPoint.KillerListInit();
  return 1;
}

bool __fastcall ct_buf_potion_use(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  if ( s_nWordCount > 3 )
    return 0;
  if ( !strcmp_0("end", s_pwszDstCheat[0]) )
    pOne->Cheet_BufEffectEnd();
  return 1;
}

bool __fastcall ct_lua_command(CPlayer *pOne)
{
  CLuaScriptMgr *scriptMgr;
  CLuaScript *script;
  CLuaCommand attachCommand;
  char scriptName[64];
  char commandArg[72];

  if ( !pOne || !pOne->m_bOper )
    return 0;

  scriptMgr = CLuaScriptMgr::Instance();
  if ( !scriptMgr || s_nWordCount < 2 )
    return 0;

  attachCommand.Init();
  memset(scriptName, 0, sizeof(scriptName));
  memset(commandArg, 0, sizeof(commandArg));

  W2M(s_pwszDstCheat[1], scriptName, 32);
  if ( s_nWordCount >= 3 )
    W2M(s_pwszDstCheat[2], commandArg, 32);

  if ( !strcmp_0("attach", s_pwszDstCheat[0]) )
  {
    script = scriptMgr->NewScript();
    if ( !script )
      return 0;
    script->SetName(scriptName);
    if ( s_nWordCount == 3 )
      attachCommand.SetCmd(2u, commandArg);
    else
      attachCommand.Init();
    return scriptMgr->AttachLuaScript(script, &attachCommand);
  }

  if ( !strcmp_0("dettach", s_pwszDstCheat[0]) )
  {
    script = scriptMgr->SearchScript(scriptName);
    if ( !script )
      return 0;
    return scriptMgr->DetackLuaScript(script);
  }

  if ( !strcmp_0("cmdfile", s_pwszDstCheat[0]) )
  {
    script = scriptMgr->SearchScript(scriptName);
    if ( !script )
      return 0;
    if ( s_nWordCount == 3 )
      attachCommand.SetCmd(2u, commandArg);
    else
      attachCommand.Init();
    return script->RunCommand(&attachCommand);
  }

  if ( strcmp_0("cmdstr", s_pwszDstCheat[0]) )
    return 0;

  script = scriptMgr->SearchScript(scriptName);
  if ( !script )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  if ( s_nWordCount == 3 )
    attachCommand.SetCmd(1u, commandArg);
  else
    attachCommand.Init();
  return script->RunCommand(&attachCommand);
}

bool __fastcall ct_userchatban(CPlayer *pOne)
{
  char _Dest[128]; // [rsp+50h] [rbp-148h] BYREF
  char pwszMessage[128]; // [rsp+F0h] [rbp-A8h] BYREF
  int iPeriod; // [rsp+174h] [rbp-24h]

  if ( !pOne )
    return 0;
  if ( !pOne->m_byUserDgr )
    return 0;
  if ( s_nWordCount == 3 )
  {
    if ( strlen_0(s_pwszDstCheat[2]) && strlen_0(s_pwszDstCheat[2]) <= 127 )
    {
      iPeriod = 0;
      iPeriod = atoi(s_pwszDstCheat[1]);
      return pOne->mgr_user_ban(s_pwszDstCheat[0], iPeriod, s_pwszDstCheat[2], 1u);
    }
    else
    {
      memset(pwszMessage, 0, sizeof(pwszMessage));
      sprintf_s(pwszMessage, sizeof(pwszMessage), "Invaled Reason Str!");
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, pwszMessage, 0xFFu, 0LL);
      return 0;
    }
  }
  else
  {
    memset(_Dest, 0, sizeof(_Dest));
    sprintf_s(_Dest, sizeof(_Dest), "Invaled Parameter Count!");
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, _Dest, 0xFFu, 0LL);
    return 0;
  }
}

bool __fastcall ct_itemloot(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  ITEM_ROOT_RATE = percent;
  return 1;
}

bool __fastcall ct_minespeed(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  MINE_SPEED_RATE = percent;
  return 1;
}

bool __fastcall ct_sfmastery(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  FORCE_LIVER_ACCUM_RATE = percent;
  return 1;
}

bool __fastcall ct_basemastery(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  MASTERY_GET_RATE = percent;
  return 1;
}

bool __fastcall ct_animusexp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  ANIMUS_EXP_RATE = percent;
  return 1;
}

bool __fastcall ct_playerexp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PLAYER_EXP_RATE = percent;
  return 1;
}

bool __fastcall ct_darkholereward(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  TSVR_ADD_DARKHOLE_REWARD_RATE = percent;
  return 1;
}

bool __fastcall ct_pcitemloot(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PCBANG_PRIMIUM_FAVOR::ITEM_DROP = percent;
  return 1;
}

bool __fastcall ct_pcminespeed(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PCBANG_PRIMIUM_FAVOR::MINING_SPEED = percent;
  return 1;
}

bool __fastcall ct_pcsfmastery(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY = percent;
  return 1;
}

bool __fastcall ct_pcbasemastery(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PCBANG_PRIMIUM_FAVOR::BASE_MASTERY = percent;
  return 1;
}

bool __fastcall ct_pcanimusexp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP = percent;
  return 1;
}

bool __fastcall ct_pcplayerexp(CPlayer *pOne)
{
  if ( !pOne )
    return 0;
  float percent = 0.0f;
  if ( !ParsePercentCheatValue(&percent) )
    return 0;
  PCBANG_PRIMIUM_FAVOR::PLAYER_EXP = percent;
  return 1;
}

bool __fastcall ct_mepcbang(CPlayer *pOne)
{
  unsigned int rewardListIndex;
  _AVATOR_DATA *avatorData; // [rsp+48h] [rbp-10h]
  CPcBangFavor *pcBang;

  if ( !pOne )
    return 0;
  avatorData = &pOne->m_pUserDB->m_AvatorData;
  pcBang = CPcBangFavor::Instance();
  if ( !pcBang )
    return 0;
  rewardListIndex = pcBang->ClassCodePasing(avatorData, pOne);
  if ( rewardListIndex == static_cast<unsigned int>(-1) )
    return 0;
  sprintf(wszRespon, "you pcbang reward list index : %u", rewardListIndex);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_pcbangitemget(CPlayer *pOne)
{
  CPcBangFavor *pcBang;
  unsigned int dwRecIndex; // [rsp+68h] [rbp-40h]
  unsigned __int8 bySeletItemIndex[36]; // [rsp+6Ch] [rbp-3Ch] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  dwRecIndex = atoi(s_pwszDstCheat[0]);
  for ( int itemSlot = 0; itemSlot < 5; ++itemSlot )
    bySeletItemIndex[itemSlot] = static_cast<unsigned __int8>(s_pwszDstCheat[1][itemSlot] - '0');
  pcBang = CPcBangFavor::Instance();
  return pcBang && pcBang->PcBangGiveItem(pOne, dwRecIndex, bySeletItemIndex, 5);
}

bool __fastcall ct_expire_pcbang(CPlayer *pOne)
{

  if ( !pOne || !pOne->m_bOper )
    return 0;
  pOne->Billing_Logout();
  pOne->m_pUserDB->m_BillingInfo.iType = 1;
  return 1;
}

bool __fastcall ct_vote_enable(CPlayer *pOne)
{

  if ( !pOne || !pOne->m_bOper )
    return 0;
  pOne->PushDQSCheatPlyerVoteInfo();
  return 1;
}

bool __fastcall ct_elect_info_player(CPlayer *pOne)
{
  unsigned int dwAccumPlayTime; // [rsp+50h] [rbp-298h]
  unsigned int dwLastResetDate; // [rsp+54h] [rbp-294h]
  unsigned __int16 wScanerCnt; // [rsp+58h] [rbp-290h]
  unsigned __int8 VoteEnable; // [rsp+5Ch] [rbp-28Ch]
  BOOL m_bOverlapVote; // [rsp+60h] [rbp-288h]
  char Destination[56]; // [rsp+78h] [rbp-270h] BYREF
  char Buffer[520]; // [rsp+B0h] [rbp-238h] BYREF

  dwAccumPlayTime = 0;
  dwLastResetDate = 0;
  wScanerCnt = 0;
  VoteEnable = 0;
  m_bOverlapVote = 0;
  memset(Destination, 0, 17);
  memset(Buffer, 0, 512);
  if ( s_nWordCount > 0 )
  {
    bool foundPlayer = false;
    for ( int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex )
    {
      CPlayer *targetPlayer = &g_Player[playerIndex];
      const char *targetName = targetPlayer->m_Param.GetCharNameW();
      if ( !strcmp_0(targetName, s_pwszDstCheat[0]) )
      {
        dwAccumPlayTime = targetPlayer->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime;
        wScanerCnt = targetPlayer->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt;
        VoteEnable = targetPlayer->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable;
        m_bOverlapVote = targetPlayer->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote;
        dwLastResetDate = pOne->m_pUserDB->m_AvatorData.dbSupplement.dwLastResetDate;
        strcpy_0(Destination, s_pwszDstCheat[0]);
        sprintf_s(
          Buffer,
          512,
          "Player Vote Info : Name = %s, ResetTime = %d, AccTime = %d, Scaner = %d, Enable = %d, Over = %d",
          Destination,
          dwLastResetDate,
          dwAccumPlayTime,
          wScanerCnt,
          VoteEnable,
          m_bOverlapVote);
        foundPlayer = true;
        break;
      }
    }
    if ( !foundPlayer )
      sprintf_s(Buffer, 512, "Player Search Fail!! -> Name = %s", s_pwszDstCheat[0]);
  }
  else
  {
    if ( !pOne || !pOne->m_bOper )
      return 0;
    dwAccumPlayTime = pOne->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime;
    wScanerCnt = pOne->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt;
    VoteEnable = pOne->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable;
    m_bOverlapVote = pOne->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote;
    dwLastResetDate = pOne->m_pUserDB->m_AvatorData.dbSupplement.dwLastResetDate;
    const char *charName = pOne->m_Param.GetCharNameW();
    strcpy_0(Destination, charName);
    sprintf_s(
      Buffer,
      512,
      "Player Vote Info : Name = %s, Token = %d, AccTime = %d, Scaner = %d, Enable = %d, Over = %d",
      Destination,
      dwLastResetDate,
      dwAccumPlayTime,
      wScanerCnt,
      VoteEnable,
      m_bOverlapVote);
  }
  s_logCheat.Write( Buffer);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_elect_set_player(CPlayer *pOne)
{
  unsigned int accumPlayTime; // [rsp+20h] [rbp-18h]
  unsigned __int16 scanerCount; // [rsp+24h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  accumPlayTime = atoi(s_pwszDstCheat[0]);
  scanerCount = atoi(s_pwszDstCheat[1]);
  pOne->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime = accumPlayTime;
  pOne->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt = scanerCount;
  pOne->m_pUserDB->m_AvatorData.dbSupplement.dwScanerGetDate = 199001011;
  pOne->PushDQSUpdatePlyerVoteInfo();
  return 1;
}

bool __fastcall ct_elect_set_env(CPlayer *pOne)
{

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 3 )
    return 0;
  g_Main.m_dwCheatSetPlayTime = atoi(s_pwszDstCheat[0]);
  g_Main.m_dwCheatSetScanerCnt = atoi(s_pwszDstCheat[1]);
  g_Main.m_dwCheatSetLevel = atoi(s_pwszDstCheat[2]);
  return 1;
}

bool __fastcall ct_period_time_set(CPlayer *pOne)
{
  char Buffer[132]; // [rsp+50h] [rbp-A8h] BYREF

  memset(Buffer, 0, 125);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 125, aCheatHelpSynta_0);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else if ( s_nWordCount <= 0 || s_nWordCount < g_Main.m_pTimeLimitMgr->GetPeriodCnt() )
  {
    return 0;
  }
  else
  {
    for ( unsigned __int16 periodIndex = 0; ; ++periodIndex )
    {
      const unsigned __int16 periodCount = g_Main.m_pTimeLimitMgr->GetPeriodCnt();
      if ( periodIndex >= periodCount )
        break;
      const unsigned int periodTime = atoi(s_pwszDstCheat[periodIndex]);
      g_Main.m_pTimeLimitMgr->SetTime(static_cast<unsigned __int16>(periodTime), periodIndex);
    }
    g_Main.m_pTimeLimitMgr->ReInitFatigue();
    const unsigned __int16 periodCount = g_Main.m_pTimeLimitMgr->GetPeriodCnt();
    const int fatiguePercent = atoi(s_pwszDstCheat[periodCount - 1]);
    g_Main.m_pTimeLimitMgr->SetPlayFDegree(60000 * fatiguePercent / 100);
    const unsigned int playFatigueDegree = static_cast<unsigned int>(g_Main.m_pTimeLimitMgr->GetPlayFDegree());
    g_Main.m_pTimeLimitMgr->SetLogoutFDegree(playFatigueDegree);
    g_Main.m_pTimeLimitMgr->m_tmLoopTime.StopTimer();
    const unsigned int loopStart = static_cast<unsigned int>(g_Main.m_pTimeLimitMgr->GetPlayFDegree());
    g_Main.m_pTimeLimitMgr->m_tmLoopTime.BeginTimer(loopStart);
    return 1;
  }
}

bool __fastcall ct_tl_info_set(CPlayer *pOne)
{
  char Buffer[152]; // [rsp+50h] [rbp-D8h] BYREF
  _qry_case_insert_timelimit_info timelimitInfo; // [rsp+E8h] [rbp-40h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 125);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 125, aCheatHelpSynta_1);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else if ( s_nWordCount <= 1 )
  {
    return 0;
  }
  else
  {
    timelimitInfo.dwFatigue = atoi(s_pwszDstCheat[0]);
    timelimitInfo.wStatus = atoi(s_pwszDstCheat[1]);
    timelimitInfo.dwAccSerial = pOne->m_pUserDB->m_dwAccountSerial;
    timelimitInfo.wIndex = pOne->m_id.wIndex;
    const int querySize = static_cast<int>(timelimitInfo.size());
    g_Main.PushDQSData(-1, nullptr, 155, reinterpret_cast<char *>(&timelimitInfo), querySize);
    return 1;
  }
}

bool __fastcall ct_tl_info_view(CPlayer *pOne)
{
  unsigned int dwFatigue; // [rsp+40h] [rbp-288h]
  unsigned int dwLastLogoutTime; // [rsp+44h] [rbp-284h]
  __int16 byTLStatus; // [rsp+48h] [rbp-280h]
  char Destination[56]; // [rsp+58h] [rbp-270h] BYREF
  char Buffer[520]; // [rsp+90h] [rbp-238h] BYREF

  dwFatigue = 0;
  dwLastLogoutTime = 0;
  byTLStatus = 0;
  memset(Destination, 0, 17);
  memset(Buffer, 0, 512);
  if ( s_nWordCount > 0 )
  {
    bool foundPlayer = false;
    for ( int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex )
    {
      CPlayer *targetPlayer = &g_Player[playerIndex];
      const char *targetName = targetPlayer->m_Param.GetCharNameW();
      if ( !strcmp_0(targetName, s_pwszDstCheat[0]) )
      {
        dwFatigue = targetPlayer->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
        dwLastLogoutTime = targetPlayer->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime;
        byTLStatus = targetPlayer->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;
        const char *charName = targetPlayer->m_Param.GetCharNameW();
        strcpy_0(Destination, charName);
        sprintf_s(
          Buffer,
          512,
          "Player Time Limit Info : Name = %s, Fatigue = %d, Status = %d, LogoutTime = %d",
          Destination,
          dwFatigue,
          byTLStatus,
          dwLastLogoutTime);
        foundPlayer = true;
        break;
      }
    }
    if ( !foundPlayer )
      sprintf_s(Buffer, 512, "Player Search Fail!! -> Name = %s", s_pwszDstCheat[0]);
  }
  else
  {
    if ( !pOne || !pOne->m_bOper )
      return 0;
    dwFatigue = pOne->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
    dwLastLogoutTime = pOne->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime;
    byTLStatus = pOne->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;
    const char *charName = pOne->m_Param.GetCharNameW();
    strcpy_0(Destination, charName);
    sprintf_s(
      Buffer,
      512,
      "Player Time Limit Info : Name = %s, Fatigue = %d, Status = %d, LogoutTime = %d",
      Destination,
      dwFatigue,
      byTLStatus,
      dwLastLogoutTime);
  }
  s_logCheat.Write( Buffer);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_tl_system_setting(CPlayer *pOne)
{
  char Buffer[152]; // [rsp+50h] [rbp-F8h] BYREF
  char statusMessage[60]; // [rsp+E8h] [rbp-60h] BYREF

  memset(Buffer, 0, 125);
  memset(statusMessage, 0, 50);
  if ( s_nWordCount <= 0 )
    return 0;
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 125, "Cheat Help : Syntax = tlsysset 0 ~ 2,   0 = TL Disable, 1 = TL Enable, 2 = Suspend");
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else
  {
    const unsigned __int16 systemState = atoi(s_pwszDstCheat[0]);
    if ( systemState )
    {
      if ( systemState == 1 )
      {
        g_Main.m_pTimeLimitMgr->SetTLEnable(1u);
        sprintf_s(statusMessage, 50, "Time Limit System Enable");
      }
      else if ( systemState == 2 )
      {
        g_Main.m_pTimeLimitMgr->SetTLEnable(2u);
        sprintf_s(statusMessage, 50, "Time Limit System Suspend");
      }
    }
    else
    {
      g_Main.m_pTimeLimitMgr->SetTLEnable(0);
      sprintf_s(statusMessage, 50, "Time Limit System Disable");
    }
    sprintf_s(Buffer, 125, "Time Limit Info : %s", statusMessage);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
}

bool __fastcall ct_action_point_set(CPlayer *pOne)
{
  char Buffer[132]; // [rsp+50h] [rbp-A8h] BYREF
  unsigned __int8 byPointType; // [rsp+D4h] [rbp-24h]
  unsigned int dwPoint; // [rsp+D8h] [rbp-20h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 125);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 125, aCheatHelpSynta_2);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else if ( s_nWordCount <= 1 )
  {
    return 0;
  }
  else
  {
    byPointType = atoi(s_pwszDstCheat[0]);
    dwPoint = atoi(s_pwszDstCheat[1]);
    if ( byPointType == 99 )
    {
      for ( int actionPointType = 0; actionPointType < 3; ++actionPointType )
      {
        pOne->m_pUserDB->Update_User_Action_Point(static_cast<unsigned __int8>(actionPointType), dwPoint);
        pOne->SendMsg_Alter_Action_Point(actionPointType, dwPoint);
      }
    }
    else
    {
      pOne->m_pUserDB->Update_User_Action_Point(byPointType, dwPoint);
      pOne->SendMsg_Alter_Action_Point(byPointType, dwPoint);
    }
    return 1;
  }
}

bool __fastcall ct_Win_RaceWar(CPlayer *pOne)
{
  int RaceCode; // eax

  if ( !pOne || !pOne->m_bOper )
    return 0;
  RaceCode = pOne->m_Param.GetRaceCode();
  g_HolySys.m_SaveData.m_nHolyMasterRace = RaceCode;
  return 1;
}

bool __fastcall ct_Gold_Age_Event_Status(CPlayer *pOne)
{
  CGoldenBoxItemMgr *goldenBox;
  unsigned __int8 eventStatus; // [rsp+40h] [rbp-188h]
  char Buffer[72]; // [rsp+58h] [rbp-170h] BYREF
  char pwszMessage[272]; // [rsp+A0h] [rbp-128h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  goldenBox = CGoldenBoxItemMgr::Instance();
  if ( !goldenBox )
    return 0;
  eventStatus = goldenBox->Get_Event_Status();
  memset(Buffer, 0, 30);
  memset(pwszMessage, 0, 256);
  if ( eventStatus )
  {
    switch ( eventStatus )
    {
      case 1u:
        sprintf_s(Buffer, 30, byte_1407AF104);
        break;
      case 2u:
        sprintf_s(Buffer, 30, byte_1407AF0F8);
        break;
      case 3u:
        sprintf_s(Buffer, 30, aA_50);
        break;
      default:
        sprintf_s(Buffer, 30, byte_1407AF110);
        break;
    }
  }
  else
  {
    sprintf_s(Buffer, 30, aOne_3);
  }
  sprintf_s(pwszMessage, 256, aCoacE, Buffer);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, pwszMessage, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_Gold_Age_Set_Event_Status(CPlayer *pOne)
{
  CGoldenBoxItemMgr *goldenBox;
  char Buffer[260]; // [rsp+50h] [rbp-128h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  goldenBox = CGoldenBoxItemMgr::Instance();
  if ( !goldenBox )
    return 0;
  memset(Buffer, 0, 256);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 256, aCheatHelpSynta_3);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  if ( s_nWordCount < 1 )
    return 0;
  const char requestedStatus = atoi(s_pwszDstCheat[0]);
  if ( requestedStatus )
  {
    if ( requestedStatus == 1 )
    {
      goldenBox->Set_Event_Status(2u);
      return 1;
    }
    if ( requestedStatus == 2 )
    {
      goldenBox->Set_Event_Status(3u);
      return 1;
    }
  }
  goldenBox->Set_Event_Status(0);
  return 1;
}

bool __fastcall ct_Gold_Age_Get_Box_Cnt(CPlayer *pOne)
{
  CGoldenBoxItemMgr *goldenBox;
  unsigned __int8 LoopCount; // al
  int bFilter; // [rsp+20h] [rbp-158h]
  char Buffer[260]; // [rsp+50h] [rbp-128h] BYREF
  int j; // [rsp+154h] [rbp-24h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 256);
  goldenBox = CGoldenBoxItemMgr::Instance();
  if ( !goldenBox )
    return 0;
  if ( goldenBox->Get_Event_Status() == 2 )
  {
    for ( j = 0; ; ++j )
    {
      LoopCount = goldenBox->GetLoopCount();
      if ( j >= LoopCount )
        break;
      memset_0(Buffer, 0, 256);
      bFilter = goldenBox->Get_Box_Count(static_cast<unsigned __int8>(j));
      sprintf_s(Buffer, 256, aCoacCodeDBoxAc, (unsigned int)j, bFilter);
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    }
    return 1;
  }
  else
  {
    sprintf_s(Buffer, 256, aCoacE_0);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
}

bool __fastcall ct_set_hp(CPlayer *pOne)
{
  float hpPercent; // [rsp+20h] [rbp-18h]
  float probability; // [rsp+24h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  hpPercent = atof(s_pwszDstCheat[0]);
  if ( hpPercent <= 0.0 || hpPercent > 100.0 )
    return 0;
  probability = hpPercent / 100.0;
  pOne->dev_set_hp(probability);
  return 1;
}

bool __fastcall ct_set_hfs_full(CPlayer *pOne)
{

  return pOne && pOne->m_bOper && pOne->SF_HFSInc_Once(pOne);
}

bool __fastcall ct_alter_dalant(CPlayer *pOne)
{
  char *EndPtr; // [rsp+28h] [rbp-20h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return pOne->dev_dalant(-1);
  const unsigned int dalant = strtoul(s_pwszDstCheat[0], &EndPtr, 10);
  return pOne->dev_dalant(dalant);
}

bool __fastcall ct_alter_gold(CPlayer *pOne)
{
  char *EndPtr; // [rsp+28h] [rbp-20h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return pOne->dev_gold(-1);
  const unsigned int gold = strtoul(s_pwszDstCheat[0], &EndPtr, 10);
  return pOne->dev_gold(gold);
}

bool __fastcall ct_SetPatriarchClear(CPlayer *pOne)
{
  if ( !pOne || !pOne->m_bOper )
    return 0;
  return PatriarchElectProcessor::Instance()->CheatClearPatriarch();
}

bool __fastcall ct_SetPatriarchGroup(CPlayer *pOne)
{
  int eClass;

  if ( !pOne || !pOne->m_bOper )
    return 0;
  eClass = atoi(s_pwszDstCheat[0]);
  return PatriarchElectProcessor::Instance()->CheatSetPatriarch(pOne, eClass);
}

bool __fastcall ct_InformCristalBattleBeforeAnHour(CPlayer *pOne)
{
  int scheduleValue; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  scheduleValue = atoi(s_pwszDstCheat[1]);
  g_HolySys.AlterSchedule(0, static_cast<unsigned __int8>(scheduleValue));
  return 1;
}

bool __fastcall ct_SetSettleOwnerGuild(CPlayer *pOne)
{
  AutoMineMachineMng *machineMgr;
  AutoMineMachine *machine;
  unsigned int ownershipSlot; // [rsp+20h] [rbp-38h]
  CGuild *pGuild; // [rsp+28h] [rbp-30h]
  CGuild *ownerGuild;
  int nRaceCode; // [rsp+40h] [rbp-18h]
  unsigned int dw2ThGuildSerial; // [rsp+44h] [rbp-14h]
  unsigned int dw1ThGuildSerial; // [rsp+48h] [rbp-10h]

  if ( !pOne || s_nWordCount < 2 )
    return 0;
  ownershipSlot = atoi(s_pwszDstCheat[0]);
  if ( ownershipSlot >= 2 )
    return 0;
  pGuild = GetGuildPtrFromName(g_Guild, 500, s_pwszDstCheat[1]);
  if ( !pGuild )
    return 0;
  nRaceCode = pGuild->m_byRace;
  machineMgr = AutoMineMachineMng::Instance();
  machineMgr->ChangeOwner(nRaceCode, pGuild, static_cast<unsigned __int8>(ownershipSlot));
  if ( ownershipSlot )
  {
    machine = machineMgr->GetMachine(pGuild->m_byRace, 0);
    ownerGuild = machine ? machine->m_pOwnerGuild : nullptr;
    if ( ownerGuild )
      dw1ThGuildSerial = ownerGuild->m_dwSerial;
    else
      dw1ThGuildSerial = 0;
    g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(pGuild->m_byRace, dw1ThGuildSerial, pGuild->m_dwSerial);
  }
  else
  {
    machine = machineMgr->GetMachine(pGuild->m_byRace, 1u);
    ownerGuild = machine ? machine->m_pOwnerGuild : nullptr;
    if ( ownerGuild )
      dw2ThGuildSerial = ownerGuild->m_dwSerial;
    else
      dw2ThGuildSerial = 0;
    g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(pGuild->m_byRace, pGuild->m_dwSerial, dw2ThGuildSerial);
  }
  g_Main.m_kEtcNotifyInfo.Notify(pGuild->m_byRace);
  return 1;
}

bool __fastcall ct_ClearSettleOwnerGuild(CPlayer *pOne)
{
  AutoMineMachineMng *machineMgr;
  AutoMineMachine *machine;
  int nRaceCode; // [rsp+20h] [rbp-38h]
  int j; // [rsp+24h] [rbp-34h]
  int raceCode; // [rsp+28h] [rbp-30h]
  unsigned int ownershipSlot; // [rsp+2Ch] [rbp-2Ch]
  CGuild *ownerGuild; // [rsp+30h] [rbp-28h]
  unsigned int dw2ThGuildSerial; // [rsp+40h] [rbp-18h]
  unsigned int dw1ThGuildSerial; // [rsp+44h] [rbp-14h]

  if ( s_nWordCount )
  {
    if ( s_nWordCount == 2 )
    {
      raceCode = atoi(s_pwszDstCheat[0]);
      ownershipSlot = atoi(s_pwszDstCheat[0]);
      if ( static_cast<unsigned int>(raceCode) <= 2 )
      {
        if ( ownershipSlot < 2 )
        {
          machineMgr = AutoMineMachineMng::Instance();
          machineMgr->ChangeOwner(raceCode, nullptr, static_cast<unsigned __int8>(ownershipSlot));
          if ( ownershipSlot )
          {
            machine = machineMgr->GetMachine(static_cast<unsigned __int8>(raceCode), 0);
            ownerGuild = machine ? machine->m_pOwnerGuild : nullptr;
            if ( ownerGuild )
              dw1ThGuildSerial = ownerGuild->m_dwSerial;
            else
              dw1ThGuildSerial = 0;
            g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(static_cast<unsigned __int8>(raceCode), dw1ThGuildSerial, 0);
          }
          else
          {
            machine = machineMgr->GetMachine(static_cast<unsigned __int8>(raceCode), 1u);
            ownerGuild = machine ? machine->m_pOwnerGuild : nullptr;
            if ( ownerGuild )
              dw2ThGuildSerial = ownerGuild->m_dwSerial;
            else
              dw2ThGuildSerial = 0;
            g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(static_cast<unsigned __int8>(raceCode), 0, dw2ThGuildSerial);
          }
          g_Main.m_kEtcNotifyInfo.Notify(static_cast<unsigned __int8>(raceCode));
          return 1;
        }
        else
        {
          return 0;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
  }
  else
  {
    for ( nRaceCode = 0; nRaceCode < 3; ++nRaceCode )
    {
      for ( j = 0; j < 2; ++j )
      {
        machineMgr = AutoMineMachineMng::Instance();
        machineMgr->ChangeOwner(nRaceCode, nullptr, static_cast<unsigned __int8>(j));
      }
      g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(static_cast<unsigned __int8>(nRaceCode), 0, 0);
    }
    g_Main.m_kEtcNotifyInfo.Notify(255);
    return 1;
  }
}

bool __fastcall ct_ReqPunishment(CPlayer *pOne)
{
  unsigned __int8 punishmentType; // [rsp+20h] [rbp-18h]

  if ( s_nWordCount != 3 )
    return 0;
  punishmentType = atoi(s_pwszDstCheat[0]);
  if ( punishmentType >= 3u )
    return 0;
  pOne->pc_RequestPatriarchPunishment(punishmentType, s_pwszDstCheat[1], s_pwszDstCheat[2]);
  return 1;
}

bool __fastcall ct_ReqChangeHonorGuild(CPlayer *pOne)
{
  int j; // [rsp+20h] [rbp-18h]

  for ( j = 0; j < 3; ++j )
  {
    CHonorGuild *honorGuild = CHonorGuild::Instance();
    honorGuild->ChangeHonorGuild(j);
  }
  return 1;
}

bool ct_debug(CPlayer* pOne)
{
    if (pOne->m_TargetObject.pObject != NULL)
    {
        if (!pOne)
            return 0;
        sprintf(
            wszRespon,
            "serial: %d",
            pOne->m_TargetObject.pObject->m_dwObjSerial);
        
        pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);

    }
}

bool __fastcall ct_goto_char(CPlayer *pOne)
{
  CUserDB *targetUser; // [rsp+20h] [rbp-18h]
  CPlayer *targetPlayer; // [rsp+28h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  targetUser = SearchAvatorWithName(g_UserDB, MAX_PLAYER, s_pwszDstCheat[0]);
  if ( !targetUser )
    return 0;
  targetPlayer = &g_Player[targetUser->m_idWorld.wIndex];
  if ( !targetPlayer->m_bLive )
    return 0;
  char *charName = pOne->m_Param.GetCharNameW();
  return targetPlayer->mgr_recall_player(charName);
}

bool __fastcall ct_goto_monster(CPlayer *pOne)
{
  int monsterIndex; // [rsp+20h] [rbp-18h]
  CMonster *pMon; // [rsp+28h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  monsterIndex = atoi(s_pwszDstCheat[0]);
  pMon = &g_Monster[monsterIndex];
  return pMon && pMon->m_bLive && pMon->m_bOper && pOne->dev_goto_monster(pMon);
}

bool __fastcall ct_CdeStart(CPlayer *pOne)
{
  CashItemRemoteStore *store;
  std::time_t now;
  std::tm beginTm{};
  std::tm endTm{};
  int iBegin_TT; // [rsp+38h] [rbp-30h]
  int iB30_TT; // [rsp+3Ch] [rbp-2Ch]
  int iB5_TT; // [rsp+40h] [rbp-28h]
  int iEnd_TT; // [rsp+44h] [rbp-24h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 4 )
    return 0;
  iBegin_TT = 0;
  iB30_TT = 0;
  iB5_TT = 0;
  iEnd_TT = 0;
  iBegin_TT = atoi(s_pwszDstCheat[0]);
  iB30_TT = atoi(s_pwszDstCheat[1]);
  iB5_TT = atoi(s_pwszDstCheat[2]);
  iEnd_TT = atoi(s_pwszDstCheat[3]);
  if ( iBegin_TT <= 0 || iB30_TT <= 0 || iB5_TT <= 0 || iEnd_TT <= 0 )
    return 0;
  store = CashItemRemoteStore::Instance();
  now = std::time(nullptr);
  store->m_cde.m_ini.m_bUseCashDiscount = 1;
  store->m_cde.m_ini.m_cdeTime[0] = static_cast<int>(now) + iBegin_TT;
  store->m_cde.m_ini.m_cdeTime[1] = static_cast<int>(now) + iBegin_TT + iB30_TT + iB5_TT + iEnd_TT;
  store->m_cde.m_cde_inform_before[0] = iB5_TT + iB30_TT;
  store->m_cde.m_cde_inform_before[1] = iB30_TT;
  std::time_t beginTime = static_cast<std::time_t>(store->m_cde.m_ini.m_cdeTime[0]);
  std::time_t endTime = static_cast<std::time_t>(store->m_cde.m_ini.m_cdeTime[1]);
  beginTm = *std::localtime(&beginTime);
  endTm = *std::localtime(&endTime);
  store->m_cde.m_ini.m_wYear[0] = beginTm.tm_year + 1900;
  store->m_cde.m_ini.m_byMonth[0] = beginTm.tm_mon + 1;
  store->m_cde.m_ini.m_byDay[0] = beginTm.tm_mday;
  store->m_cde.m_ini.m_byHour[0] = beginTm.tm_hour;
  store->m_cde.m_ini.m_byMinute[0] = beginTm.tm_min;
  store->m_cde.m_ini.m_wYear[1] = endTm.tm_year + 1900;
  store->m_cde.m_ini.m_byMonth[1] = endTm.tm_mon + 1;
  store->m_cde.m_ini.m_byDay[1] = endTm.tm_mday;
  store->m_cde.m_ini.m_byHour[1] = endTm.tm_hour;
  store->m_cde.m_ini.m_byMinute[1] = endTm.tm_min;
  store->m_cde.m_cde_status = 1;
  return 1;
}

bool __fastcall ct_exception(CPlayer *pOne)
{
  char _Dest[2]; // [rsp+24h] [rbp-24h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "%s", "Test");
  return 1;
}

bool __fastcall ct_pcroom_premium(CPlayer *pOne)
{

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0("y", s_pwszDstCheat[0]) )
  {
    pOne->m_pUserDB->m_BillingInfo.bPCCheat = 1;
    pOne->m_pUserDB->m_BillingInfo.bIsPcBang = 1;
    pOne->m_pUserDB->m_BillingInfo.iType = 7;
  }
  else if ( !strcmp_0("n", s_pwszDstCheat[0]) )
  {
    pOne->m_pUserDB->m_BillingInfo.bPCCheat = 0;
    pOne->m_pUserDB->m_BillingInfo.bIsPcBang = 0;
    pOne->m_pUserDB->m_BillingInfo.iType = 1;
  }
  return 1;
}

bool __fastcall ct_SetGuildGrade(CPlayer *pOne)
{
  unsigned __int8 guildGrade; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  guildGrade = 0;
  guildGrade = atoi(s_pwszDstCheat[0]);
  return pOne->dev_SetGuildGrade(guildGrade);
}

bool __fastcall ct_SetGuildGradeByName(CPlayer *pOne)
{
  unsigned __int8 guildGrade; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  guildGrade = 0;
  guildGrade = atoi(s_pwszDstCheat[1]);
  return pOne->dev_SetGuildGradeByName(s_pwszDstCheat[0], guildGrade);
}

bool __fastcall ct_SetGuildGradeByGuildSerial(CPlayer *pOne)
{
  unsigned int dwGuildSerial; // [rsp+20h] [rbp-18h]
  unsigned __int8 guildGrade; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  dwGuildSerial = 0;
  dwGuildSerial = atoi(s_pwszDstCheat[0]);
  guildGrade = 0;
  guildGrade = atoi(s_pwszDstCheat[1]);
  return pOne->dev_SetGuildGradeByGuildSerial(dwGuildSerial, guildGrade);
}

bool __fastcall ct_SetGuildMaster(CPlayer *pOne)
{
  unsigned int CharSerial; // eax
  CGuild *m_pGuild; // [rsp+20h] [rbp-18h]
  _guild_member_info *pNewguildMaster; // [rsp+28h] [rbp-10h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( !pOne->m_Param.m_pGuild )
    return 0;
  m_pGuild = pOne->m_Param.m_pGuild;
  CharSerial = pOne->m_Param.GetCharSerial();
  pNewguildMaster = m_pGuild->GetMemberFromSerial(CharSerial);
  return pNewguildMaster && m_pGuild->DB_Update_GuildMaster(pNewguildMaster);
}

bool __fastcall ct_SetMaxLevelLimit(CPlayer *pOne)
{
  unsigned __int8 maxLevelLimit; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  maxLevelLimit = 0;
  maxLevelLimit = atoi(s_pwszDstCheat[0]);
  return pOne->dev_max_level_ext(maxLevelLimit);
}

bool __fastcall ct_set_exp_rate(CPlayer *pOne)
{
  long double LimitExp; // xmm0_8
  long double Exp; // xmm0_8
  float expPercent; // [rsp+30h] [rbp-28h]
  long double dAlterExp; // [rsp+38h] [rbp-20h]
  int lv; // [rsp+40h] [rbp-18h]
  double targetExp; // [rsp+48h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  expPercent = atof(s_pwszDstCheat[0]);
  if ( expPercent < 0.0 || expPercent >= 100.0 )
    return 0;
  lv = pOne->m_Param.GetLevel();
  cStaticMember_Player *playerStatics = cStaticMember_Player::Instance();
  LimitExp = playerStatics->GetLimitExp(lv);
  targetExp = LimitExp * static_cast<float>(expPercent / 100.0f);
  Exp = pOne->m_Param.GetExp();
  dAlterExp = targetExp - Exp;
  pOne->AlterExp(dAlterExp, 1, 0, 0);
  return 1;
}

bool __fastcall ct_goto_npc(CPlayer *pOne)
{
  char Destination[72]; // [rsp+30h] [rbp-88h] BYREF
  CMerchant *pNpc; // [rsp+78h] [rbp-40h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  strcpy_0(Destination, s_pwszDstCheat[0]);
  pNpc = 0LL;
  for ( int npcIndex = 0; npcIndex < CMerchant::s_nLiveNum; ++npcIndex )
  {
    char *npcCode = g_NPC[npcIndex].m_pItemStore->GetNpcCode();
    if ( npcCode && !strcmp_0(npcCode, Destination) )
    {
      pNpc = &g_NPC[npcIndex];
      return pNpc && pOne->dev_goto_npc(pNpc);
    }
  }
  return pNpc && pOne->dev_goto_npc(pNpc);
}

bool __fastcall ct_request_npc_quest(CPlayer *pOne)
{
  char Destination[80]; // [rsp+30h] [rbp-68h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  strcpy_0(Destination, s_pwszDstCheat[0]);
  return pOne->Emb_CreateQuestEvent(quest_happen_type_npc, Destination);
}

bool __fastcall ct_request_delete_quest(CPlayer *pOne)
{
  unsigned __int8 questIndex; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  questIndex = atoi(s_pwszDstCheat[0]);
  if ( questIndex >= 30 )
    return 0;
  pOne->pc_QuestGiveupRequest(questIndex);
  return 1;
}

bool __fastcall ct_manage_guild(CPlayer *pOne)
{
  _guild_manage_request_clzo dwObj2; // [rsp+38h] [rbp-30h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  dwObj2.byManageType = atoi(s_pwszDstCheat[0]);
  if ( dwObj2.byManageType >= 6u )
    return 0;
  switch ( dwObj2.byManageType )
  {
    case 0u:
      if ( s_nWordCount != 5 )
        return 0;
      dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
      dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
      dwObj2.dwManageObj2 = atoi(s_pwszDstCheat[3]);
      dwObj2.dwManageObj3 = atoi(s_pwszDstCheat[4]);
      break;
    case 1u:
      if ( s_nWordCount != 2 )
        return 0;
      dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
      break;
    case 2u:
      if ( s_nWordCount != 4 )
        return 0;
      dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
      dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
      dwObj2.dwManageObj2 = atoi(s_pwszDstCheat[3]);
      break;
    case 3u:
      if ( s_nWordCount != 4 )
        return 0;
      dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
      dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
      dwObj2.dwManageObj2 = atoi(s_pwszDstCheat[3]);
      break;
    case 4u:
      if ( s_nWordCount != 3 )
        return 0;
      dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
      dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
      break;
    case 5u:
      if ( s_nWordCount != 2 )
        return 0;
      dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
      break;
    default:
      return 0;
  }
  pOne->pc_GuildManageRequest(
    dwObj2.byManageType,
    dwObj2.dwManageDst,
    dwObj2.dwManageObj1,
    dwObj2.dwManageObj2,
    dwObj2.dwManageObj3);
  return 1;
}

bool __fastcall ct_change_master_elect(CPlayer *pOne)
{
  bool electPossible; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( !pOne->m_Param.m_pGuild )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  electPossible = atoi(s_pwszDstCheat[0]) != 0;
  pOne->m_Param.m_pGuild->m_bPossibleElectMaster = electPossible;
  pOne->m_Param.m_pGuild->MakeDownMemberPacket();
  pOne->m_Param.m_pGuild->SendMsg_MasterElectPossible(electPossible);
  return 1;
}

bool __fastcall ct_release_punishment(CPlayer *pOne)
{
  int j; // [rsp+0h] [rbp-18h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  for ( j = 0; j < 3; ++j )
    pOne->m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[j] = static_cast<unsigned int>(-1);
  return 1;
}

bool __fastcall ct_ConEventStart(CPlayer *pOne)
{
  CashItemRemoteStore *store;
  std::time_t now;
  int beginDelay; // [rsp+28h] [rbp-20h]
  int duration; // [rsp+2Ch] [rbp-1Ch] BYREF
  unsigned __int8 eventKind; // [rsp+34h] [rbp-14h]

  if ( !pOne )
    return 0;
  beginDelay = 0;
  memset(&duration, 0, sizeof(duration));
  beginDelay = atoi(s_pwszDstCheat[0]);
  duration = atoi(s_pwszDstCheat[1]);
  eventKind = atoi(s_pwszDstCheat[2]);
  if ( beginDelay <= 0 || duration <= 0 || eventKind > 3u )
    return 0;
  store = CashItemRemoteStore::Instance();
  now = std::time(nullptr);
  store->m_con_event.m_ini.m_bUseConEvent = (eventKind < 3u);
  store->m_con_event.m_ini.m_byEventKind = eventKind;
  store->m_con_event.m_bConEvent = true;
  store->m_con_event.m_eventtime.m_EventTime[0] = static_cast<int>(now) + beginDelay;
  store->m_con_event.m_eventtime.m_EventTime[1] = static_cast<int>(now) + beginDelay + duration;
  store->m_con_event.m_conevent_status = 1;
  return 1;
}

bool __fastcall ct_loot_item(CPlayer *pOne)
{
  int nNum; // [rsp+30h] [rbp-68h]
  char szTran[56]; // [rsp+48h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nNum = 1;
  if ( s_nWordCount >= 2 )
    nNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 32);
  return pOne->dev_loot_item(szTran, nNum, 0LL, 0);
}



