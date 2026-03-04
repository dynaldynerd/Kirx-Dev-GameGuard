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
static const char aCheatHelpSynta_1[] = "Cheat Help : Syntax = tlinfoset ÇÇ";
static const char aCheatHelpSynta_2[] = "Cheat Help : Syntax = %actpset ÇàÀ";
static const char aCheatHelpSynta_3[] = "Cheat Help : Syntax = %eventset ";
static const char aOne_3[] = "<unknown>";
static const char aCoacE[] = "ÇöÀç È";
static const char aCoacCodeDBoxAc[] = "ÇöÀç [Code_%d Box] ÀÇ ÀÜ";
static const char aCoacE_0[] = "ÇöÀç È";
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

bool __fastcall ct_change_degree(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_change_degree(v4);
}

bool __fastcall ct_respawn_start(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-138h] BYREF
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool started; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 0x20u);
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-138h] BYREF
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool v7; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  v7 = g_MonsterEventRespawn.StopRespawnEvent(szTran, pwszErrCode);
  if ( !pOne )
    return 0;
  if ( v7 )
    sprintf(wszRespon, "Stop %s", s_pwszDstCheat[0]);
  else
    sprintf(wszRespon, "Stop Error %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_circle_mon_kill(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_all_kill();
}

bool __fastcall ct_look_like_boss(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_view_boss();
}

bool __fastcall ct_change_mastery(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  int v5; // eax
  __int64 v6; // [rsp+0h] [rbp-48h] BYREF
  int j; // [rsp+20h] [rbp-28h]
  unsigned __int64 v8; // [rsp+28h] [rbp-20h]
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
        v4 = atoi(s_pwszDstCheat[1]);
        v8 = 16LL * j;
        return pOne->dev_up_mastery(EqSukList[v8 / 0x10].nCode, EqSukList[v8 / 0x10].nIndex, v4);
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
    v5 = atoi(s_pwszDstCheat[0]);
    return pOne->dev_up_mastery(v5, nMasteryIndex, nLv);
  }
}

bool __fastcall ct_all_item_muzi(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_all_item_muzi(v4);
}

bool __fastcall ct_report_cri_hp(CPlayer *pOne)
{
  return pOne != 0LL;
}

bool __fastcall ct_report_position(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-48h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  double v4; // xmm0_8
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF

  if ( !pOne )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  pOne->AlterExp(v4, 1, 0, 0);
  return 1;
}

bool __fastcall ct_all_map(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int RecordNum; // eax
  __int64 v5; // [rsp+0h] [rbp-48h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  float v5; // xmm0_4
  float v6; // xmm0_4
  __int64 v7; // [rsp+0h] [rbp-D8h] BYREF
  char szTran[64]; // [rsp+38h] [rbp-A0h] BYREF
  char pszMapCode[56]; // [rsp+78h] [rbp-60h] BYREF
  float fZ; // [rsp+B0h] [rbp-28h]
  float fY; // [rsp+B4h] [rbp-24h]
  float v12; // [rsp+B8h] [rbp-20h]
  float v13; // [rsp+BCh] [rbp-1Ch]
  float v14; // [rsp+C0h] [rbp-18h]
  float v15; // [rsp+C4h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount == 4 )
  {
    W2M(s_pwszDstCheat[0], szTran, 0x20u);
    fZ = (float)atoi(s_pwszDstCheat[3]);
    fY = (float)atoi(s_pwszDstCheat[2]);
    v4 = (float)atoi(s_pwszDstCheat[1]);
    return pOne->mgr_gotoCoordinates(szTran, v4, fY, fZ);
  }
  else if ( s_nWordCount <= 4 )
  {
    return 0;
  }
  else
  {
    W2M(s_pwszDstCheat[1], pszMapCode, 0x20u);
    if ( !strcmp_0(s_pwszDstCheat[0], "#") )
    {
      v12 = (float)atoi(s_pwszDstCheat[4]);
      v13 = (float)atoi(s_pwszDstCheat[3]);
      v5 = (float)atoi(s_pwszDstCheat[2]);
      return pOne->mgr_gotoDstCoordinates(0LL, pszMapCode, v5, v13, v12);
    }
    else
    {
      v14 = (float)atoi(s_pwszDstCheat[4]);
      v15 = (float)atoi(s_pwszDstCheat[3]);
      v6 = (float)atoi(s_pwszDstCheat[2]);
      return pOne->mgr_gotoDstCoordinates(s_pwszDstCheat[0], pszMapCode, v6, v15, v14);
    }
  }
}

bool __fastcall ct_pass_dungeon(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_dungeon_pass();
}

bool __fastcall ct_tracing_hide(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_tracing(1);
}

bool __fastcall ct_tracing_show(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_tracing(0);
}

bool __fastcall ct_max_attack(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_MaxAttackPoint(1);
}

bool __fastcall ct_min_attack(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_MaxAttackPoint(-1);
}

bool __fastcall ct_mormal_attack(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_MaxAttackPoint(0);
}

bool __fastcall ct_set_matchless(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_matchless(1);
}

bool __fastcall ct_release_matchless(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_matchless(0);
}

bool __fastcall ct_goto_shipport_town(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_goto_shipport(v4, 0);
}

bool __fastcall ct_goto_shipport_eder(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_goto_shipport(v4, 1);
}

bool __fastcall ct_free_ride_ship(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_free_ride_ship();
}

bool __fastcall ct_pass_sch(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_pass_sch_one_step();
}

bool __fastcall ct_start_cri(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_exit_keeper();
}

bool __fastcall ct_start_keeper(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_holykeeper_start(v4);
}

bool __fastcall ct_goto_stone(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned __int8 v4; // al
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->mgr_goto_stone(v4);
}

bool __fastcall ct_exit_stone(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_exit_stone();
}

bool __fastcall ct_set_loot_free(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_loot_free(1);
}

bool __fastcall ct_release_loot_free(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_loot_free(0);
}

bool __fastcall ct_change_class(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-78h] BYREF
  char szTran[56]; // [rsp+28h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  return pOne->dev_change_class(szTran);
}

bool __fastcall ct_kick_player(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_quest_complete();
}

bool __fastcall ct_recall_player(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  return pOne->mgr_recall_player(s_pwszDstCheat[0]);
}

bool __fastcall ct_recall_monster(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-88h] BYREF
  int nCreateNum; // [rsp+20h] [rbp-68h]
  char szTran[56]; // [rsp+38h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nCreateNum = 1;
  if ( s_nWordCount >= 2 )
    nCreateNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  return pOne->mgr_recall_mon(szTran, nCreateNum);
}

bool __fastcall ct_goto_mine(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_goto_mine();
}

bool __fastcall ct_copy_avator(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  return pOne->dev_avator_copy(s_pwszDstCheat[0]);
}

bool __fastcall ct_user_num(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-F8h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_die();
}

bool __fastcall ct_half_gauge(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_half_point();
}

bool __fastcall ct_full_gauge(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_full_point();
}

bool __fastcall ct_alter_inven_dur(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned __int64 v4; // rax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = _strtoui64(s_pwszDstCheat[0], 0LL, 10);
  return pOne->dev_half_inven_amount(v4);
}

bool __fastcall ct_set_never_die(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_never_die(1);
}

bool __fastcall ct_release_never_die(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_never_die(0);
}

bool __fastcall ct_free_sf_by_class(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_free_sf_by_class();
}

bool __fastcall ct_alter_lv(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_lv(v4);
}

bool __fastcall ct_up_forceitem(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_forceitem(v4);
}

bool __fastcall ct_up_skill(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-78h] BYREF
  char szTran[56]; // [rsp+28h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  v4 = atoi(s_pwszDstCheat[1]);
  return pOne->dev_up_skill(szTran, v4);
}

bool __fastcall ct_up_forcemastery(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_forcemastery(v4);
}

bool __fastcall ct_up_allskill(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_all(v4);
}

bool __fastcall ct_alter_pvp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  long double v4; // xmm0_8
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = (double)atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_pvp(v4);
}

bool __fastcall ct_alter_cashbag(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  long double v4; // xmm0_8
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = (double)atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_cashbag(v4);
}

bool __fastcall ct_animus_recall_term(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  bool v5; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v5 = atoi(s_pwszDstCheat[0]) == 0;
  return pOne->dev_animus_recall_time_free(v5);
}

bool __fastcall ct_set_animus_exp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned __int64 v4; // rax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = _strtoui64(s_pwszDstCheat[0], 0LL, 10);
  return pOne->dev_set_animus_exp(v4);
}

bool __fastcall ct_init_monster(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_init_monster();
}

bool __fastcall ct_full_force(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_full_force();
}

bool __fastcall ct_loot_material(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_loot_material();
}

bool __fastcall ct_loot_tower(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_loot_tower();
}

bool __fastcall ct_loot_bag(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_loot_bag();
}

bool __fastcall ct_loot_mine(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_loot_mine();
}

bool __fastcall ct_inven_empty(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_inven_empty();
}

bool __fastcall ct_set_make_succ(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_make_succ(1);
}

bool __fastcall ct_release_make_succ(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_make_succ(0);
}

bool __fastcall ct_cont_effet_time(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_cont_effect_time(v4);
}

bool __fastcall ct_cont_effet_clear(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->dev_cont_effect_del();
}

bool __fastcall ct_view_method(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-78h] BYREF
  char szTran[56]; // [rsp+28h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  return pOne->mgr_make_system_tower(szTran);
}

bool __fastcall ct_destroy_system_tower(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_destroy_system_tower();
}

bool __fastcall ct_circle_user_num(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int UseSectorRange; // eax
  __int64 v5; // [rsp+0h] [rbp-188h] BYREF
  _DWORD v6[7]; // [rsp+48h] [rbp-140h] BYREF
  int v7; // [rsp+64h] [rbp-124h]
  _pnt_rect pRect; // [rsp+78h] [rbp-110h] BYREF
  _sec_info *SecInfo; // [rsp+98h] [rbp-F0h]
  int j; // [rsp+A0h] [rbp-E8h]
  int k; // [rsp+A4h] [rbp-E4h]
  unsigned int dwSecIndex; // [rsp+A8h] [rbp-E0h]
  CObjectList *SectorListPlayer; // [rsp+B0h] [rbp-D8h]
  _object_list_point *m_pNext; // [rsp+B8h] [rbp-D0h]
  CGameObject *m_pItem; // [rsp+C0h] [rbp-C8h]
  unsigned int v16; // [rsp+C8h] [rbp-C0h]
  char Buffer[144]; // [rsp+E0h] [rbp-A8h] BYREF
  int nSecNum; // [rsp+170h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( pOne->GetCurSecNum() == -1 )
    return 0;
  memset(v6, 0, 12);
  v7 = 0;
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
          v16 = static_cast<unsigned int>(m_pItem->GetObjRace());
          if ( v16 < 3 )
          {
            ++v6[v16];
            ++v7;
          }
        }
      }
    }
  }
  sprintf(Buffer, "Circle: Total( %d ), B( %d ), C( %d ), A( %d )", v7, v6[0], v6[1], v6[2]);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_boss_sms_cancel(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CRaceBossMsgController *v4; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned int dwMsgID; // [rsp+20h] [rbp-18h]
  int v7; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  dwMsgID = atoi(s_pwszDstCheat[1]);
  v7 = atoi(s_pwszDstCheat[0]);
  v4 = CRaceBossMsgController::Instance();
  return v4->Cancel(static_cast<unsigned __int8>(v7), dwMsgID, pOne) != 0;
}

bool __fastcall ct_trunk_init(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_TrunkInit();
}

bool __fastcall ct_fullset(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
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
    pOne->dev_dalant(0xFFFFFFFF);
  pOne->dev_loot_fullitem(nLv);
  return 1;
}

bool __fastcall ct_loot_upgrade(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  signed int Size; // eax
  __int64 v5; // [rsp+0h] [rbp-118h] BYREF
  unsigned int dwIndex; // [rsp+50h] [rbp-C8h]
  _ItemUpgrade_fld *Record; // [rsp+58h] [rbp-C0h]
  unsigned __int16 *RecordByHash; // [rsp+60h] [rbp-B8h]
  _STORAGE_LIST::_db_con pItem; // [rsp+78h] [rbp-A0h] BYREF
  unsigned __int16 *v10; // [rsp+B8h] [rbp-60h]
  _STORAGE_LIST::_db_con v11; // [rsp+C8h] [rbp-50h] BYREF

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
      pItem.m_dwLv = 0xFFFFFFF;
      if ( !CreateItemBox(
              &pItem,
              pOne,
              0xFFFFFFFF,
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
  v10 = (unsigned __int16 *)g_Main.m_tblItemData[11].GetRecordByHash("immmm04", 2, 5);
  if ( !v10 )
    return 1;
  v11 = _STORAGE_LIST::_db_con();
  v11.m_byTableCode = 11;
  v11.m_wItemIndex = *v10;
  v11.m_dwDur = GetItemDurPoint(11, dwIndex);
  v11.m_dwLv = 0xFFFFFFF;
  if ( CreateItemBox(&v11, pOne, 0xFFFFFFFF, 0, 0LL, 2u, pOne->m_pCurMap, pOne->m_wMapLayerIndex, pOne->m_fCurPos, 1) )
    return 1;
  else
    return 1;
}

bool __fastcall ct_loot_dungeon(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int RecordNum; // eax
  __int64 v5; // [rsp+0h] [rbp-B8h] BYREF
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
    pItem.m_dwLv = 0xFFFFFFF;
    if ( !CreateItemBox(
            &pItem,
            pOne,
            0xFFFFFFFF,
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v6; // [rsp+20h] [rbp-18h]
  unsigned __int8 v7; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  v6 = -1;
  if ( !strcmp_0("upper", s_pwszDstCheat[0]) )
  {
    v6 = 0;
  }
  else if ( !strcmp_0("", s_pwszDstCheat[0]) )
  {
    v6 = 1;
  }
  else if ( !strcmp_0("尩", s_pwszDstCheat[0]) )
  {
    v6 = 2;
  }
  else if ( !strcmp_0("shoe", s_pwszDstCheat[0]) )
  {
    v6 = 3;
  }
  else if ( !strcmp_0("helmet", s_pwszDstCheat[0]) )
  {
    v6 = 4;
  }
  else if ( !strcmp_0("shield", s_pwszDstCheat[0]) )
  {
    v6 = 5;
  }
  else if ( !strcmp_0("cloak", s_pwszDstCheat[0]) )
  {
    v6 = 7;
  }
  else if ( !strcmp_0("all", s_pwszDstCheat[0]) )
  {
    v6 = 37;
  }
  v7 = v6;
  if ( v6 > 5u && v7 != 7 && v7 != 37 )
    return 0;
  v4 = atoi(s_pwszDstCheat[1]);
  return pOne->mgr_defense_item_grace(v6, v4);
}

bool __fastcall ct_defense_item_grace_Jp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v6; // [rsp+20h] [rbp-18h]
  unsigned __int8 v7; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  v6 = -1;
  if ( !strcmp_0("upper", s_pwszDstCheat[0]) )
  {
    v6 = 0;
  }
  else if ( !strcmp_0("lower", s_pwszDstCheat[0]) )
  {
    v6 = 1;
  }
  else if ( !strcmp_0("gauntlet", s_pwszDstCheat[0]) )
  {
    v6 = 2;
  }
  else if ( !strcmp_0("shoe", s_pwszDstCheat[0]) )
  {
    v6 = 3;
  }
  else if ( !strcmp_0("helmet", s_pwszDstCheat[0]) )
  {
    v6 = 4;
  }
  else if ( !strcmp_0("shield", s_pwszDstCheat[0]) )
  {
    v6 = 5;
  }
  else if ( !strcmp_0("cloak", s_pwszDstCheat[0]) )
  {
    v6 = 7;
  }
  else if ( !strcmp_0("all", s_pwszDstCheat[0]) )
  {
    v6 = 37;
  }
  v7 = v6;
  if ( v6 > 5u && v7 != 7 && v7 != 37 )
    return 0;
  v4 = atoi(s_pwszDstCheat[1]);
  return pOne->mgr_defense_item_grace(v6, v4);
}

bool __fastcall ct_animus_attack_grade(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->mgr_item_telekinesis();
}

bool __fastcall ct_up_allskill_pt(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_up_all_pt(v4);
}

bool __fastcall ct_complete_quest_other(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  return pOne->dev_quest_complete_other(s_pwszDstCheat[0]);
}

bool __fastcall ct_loot_upgrade_item(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
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
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  W2M(s_pwszDstCheat[2], Str1, 0x20u);
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
  else if ( !strcmp_0(Str1, "ں") )
  {
    strcpy_0(Destination, "irtal13");
  }
  else
  {
    if ( strcmp_0(Str1, "Ȱ") )
      return 0;
    strcpy_0(Destination, "irtal14");
  }
  return pOne->dev_loot_item(szTran, nNum, Destination, nUpNum);
}

bool __fastcall ct_resurrect_player(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-48h] BYREF
  CPvpPointLimiter result; // [rsp+20h] [rbp-28h] BYREF
  __int64 v6; // [rsp+28h] [rbp-20h]
  CPvpPointLimiter *PvpPointLimiter; // [rsp+30h] [rbp-18h]
  CPvpPointLimiter *v8; // [rsp+38h] [rbp-10h]

  v6 = -2LL;
  if ( !pOne )
    return 0;
  if ( s_nWordCount >= 1 )
  {
    if ( strcmp_0(aA_46, s_pwszDstCheat[0]) )
      return pOne->mgr_resurrect_player(s_pwszDstCheat[0]);
    PvpPointLimiter = pOne->GetPvpPointLimiter(&result);
    v8 = PvpPointLimiter;
    PvpPointLimiter->CheatUpdate(pOne->m_Param.m_dbChar.m_dPvPPoint);
  }
  return pOne->pc_Resurrect(0);
}

bool __fastcall ct_add_guild_schedule(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildBattleController *v4; // rax
  __int64 v5; // [rsp+0h] [rbp-118h] BYREF
  char Buffer[136]; // [rsp+50h] [rbp-C8h] BYREF
  CGuild *pSrcGuild; // [rsp+D8h] [rbp-40h]
  CGuild *pDestGuild; // [rsp+E0h] [rbp-38h]
  unsigned int dwStartTime; // [rsp+E8h] [rbp-30h]
  int v10; // [rsp+ECh] [rbp-2Ch]
  unsigned int dwMapInx; // [rsp+F0h] [rbp-28h]
  unsigned __int8 v12; // [rsp+F4h] [rbp-24h]

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
        v10 = atoi(s_pwszDstCheat[3]);
        dwMapInx = atoi(s_pwszDstCheat[4]);
        if ( (dwStartTime & 0x80000000) == 0 )
        {
          v4 = CGuildBattleController::Instance();
          v12 = v4->Add(
            pSrcGuild,
            pDestGuild,
            dwStartTime,
            static_cast<unsigned __int8>(v10),
            dwMapInx);
          sprintf(Buffer, "Add GuildBattle Schedule : %u", v12);
          pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
          return v12 == 0;
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  unsigned int v5; // [rsp+40h] [rbp-C8h]
  char Buffer[144]; // [rsp+60h] [rbp-A8h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  v5 = atoi(s_pwszDstCheat[0]);
  if ( v5 == -1 )
  {
    pOne->m_bInGuildBattle = 0;
    pOne->m_byGuildBattleColorInx = -1;
    sprintf(Buffer, "Clear GuildBattle Color!");
  }
  else if ( v5 > 1 )
  {
    sprintf(Buffer, "Invalid Color Inx! ( 0 or 1 or -1 )");
  }
  else
  {
    pOne->m_bInGuildBattle = 1;
    pOne->m_byGuildBattleColorInx = v5;
    sprintf(Buffer, "Set GuildBattle Color : %d", v5);
  }
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_cur_guildbattle_color(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-F8h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  CTotalGuildRankManager *v4; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned int dwVer; // [rsp+20h] [rbp-18h]
  unsigned __int8 v7; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  dwVer = atoi(s_pwszDstCheat[0]);
  v7 = atoi(s_pwszDstCheat[1]);
  v4 = CTotalGuildRankManager::Instance();
  v4->Send(dwVer, v7, pOne);
  return 1;
}

bool __fastcall ct_recv_pvp_guild_rank(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CWeeklyGuildRankManager *v4; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned int dwVer; // [rsp+20h] [rbp-18h]
  unsigned __int8 v7; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  dwVer = atoi(s_pwszDstCheat[0]);
  v7 = atoi(s_pwszDstCheat[1]);
  v4 = CWeeklyGuildRankManager::Instance();
  {
    unsigned int guildSerial = static_cast<unsigned int>(-1);
    if (pOne->m_Param.m_pGuild)
    {
      guildSerial = pOne->m_Param.m_pGuild->m_dwSerial;
    }
    const unsigned __int8 selfRace = static_cast<unsigned __int8>(pOne->m_Param.GetRaceCode());
    v4->m_kInfo.Send(dwVer, pOne->m_ObjID.m_wIndex, v7, selfRace, guildSerial);
  }
  return 1;
}

bool __fastcall ct_recv_change_atrad_taxrate(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CUnmannedTraderTaxRateManager *v4; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  unsigned int v6; // [rsp+20h] [rbp-18h]
  unsigned int dwNewTaxRate; // [rsp+24h] [rbp-14h]
  char *pCheaterName; // [rsp+28h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  v6 = atoi(s_pwszDstCheat[0]);
  dwNewTaxRate = atoi(s_pwszDstCheat[1]);
  if ( v6 > 2 )
    return 0;
  pCheaterName = pOne->m_Param.GetCharNameW();
  v4 = CUnmannedTraderTaxRateManager::Instance();
  v4->SetPatriarchTaxMoney(static_cast<unsigned __int8>(v6), dwNewTaxRate);
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
  _STORAGE_LIST::_db_con v9; // [rsp+58h] [rbp-1E0h] BYREF
  _personal_amine_mineore_zocl v11; // [rsp+B0h] [rbp-188h] BYREF
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
  v9 = _STORAGE_LIST::_db_con();
  v9.m_byTableCode = ItemTableCode;
  v9.m_wItemIndex = *Record;
  v9.m_dwDur = 99LL;
  while ( pOne->m_Param.m_dbPersonalAmineInven.GetIndexEmptyCon() != 255 )
  {
    v9.m_wSerial = pOne->m_Param.GetNewItemSerial();
    v9.m_dwT = -1;
    if ( pOne->Emb_AddStorage(6u, &v9, 0, 1) )
    {
      v11.clear();
      v11.dwObjSerial = pOne->m_Param.m_pAPM->m_dwObjSerial;
      v11.byChangedNum = 1;
      v11.change[0].wItemIndex = v9.m_wItemIndex;
      v11.change[0].wItemSerial = v9.m_wSerial;
      v11.change[0].dwDur = v9.m_dwDur;
      pbyType[0] = 14;
      pbyType[1] = 55;
      const unsigned __int16 packetSize =
        static_cast<unsigned __int16>(10 + (8 * static_cast<unsigned int>(v11.byChangedNum)));
      g_Network.m_pProcess[0]->LoadSendMsg(pOne->m_id.wIndex, pbyType, reinterpret_cast<char *>(&v11), packetSize);
    }
  }
  return 1;
}

bool __fastcall ct_server_time(CPlayer *pOne)
{
  time_t currentTime = 0;
  tm *v6; // [rsp+58h] [rbp-140h]
  char Buffer[272]; // [rsp+70h] [rbp-128h] BYREF

  if ( !pOne )
    return 0;
  std::time(&currentTime);
  v6 = std::localtime(&currentTime);
  if ( v6 )
    sprintf(
      Buffer,
      "%04d-%02d-%02d %02d:%02d:%02d",
      v6->tm_year + 1900,
      v6->tm_mon + 1,
      v6->tm_mday,
      v6->tm_hour,
      v6->tm_min,
      v6->tm_sec);
  else
    sprintf(Buffer, "::localtime Fail!");
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_party_call(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  if ( (unsigned int)nNum >= 0x64 )
    return 0;
  return 0;
}

bool __fastcall ct_PcBandPrimium(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  float v5; // xmm0_4
  __int64 v6; // [rsp+0h] [rbp-38h] BYREF
  float v7; // [rsp+20h] [rbp-18h]
  float v8; // [rsp+24h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v7 = v4;
  if ( v4 <= 0.0 || v7 > 2.0 )
    return 0;
  v5 = atof(s_pwszDstCheat[1]);
  v8 = v5;
  if ( v5 <= 0.0 || v8 > 2.0 )
    return 0;
  PCBANG_PRIMIUM_FAVOR::MINING_SPEED = v7;
  PCBANG_PRIMIUM_FAVOR::PLAYER_EXP = v7;
  PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP = v7;
  PCBANG_PRIMIUM_FAVOR::BASE_MASTERY = v7;
  PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY = v7;
  PCBANG_PRIMIUM_FAVOR::ITEM_DROP = v8;
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-48h] BYREF
  int v5; // [rsp+20h] [rbp-28h]
  int nRace; // [rsp+24h] [rbp-24h]
  int nPassTime; // [rsp+28h] [rbp-20h]
  int v8; // [rsp+2Ch] [rbp-1Ch]
  int v9; // [rsp+30h] [rbp-18h]
  int v10; // [rsp+34h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0("start", s_pwszDstCheat[0]) )
  {
    if ( s_nWordCount >= 2 )
    {
      v5 = atoi(s_pwszDstCheat[1]);
      g_HolySys.AlterSchedule(1u, static_cast<unsigned __int8>(v5));
      return 1;
    }
    return 0;
  }
  if ( !strcmp_0("end", s_pwszDstCheat[0]) )
    return g_HolySys.ct_StopBattle();
  if ( strcmp_0("Ű", s_pwszDstCheat[0]) )
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
      v10 = atoi(s_pwszDstCheat[2]);
      return g_HolySys.ct_KeeperStart(6, v10, 0);
    }
    return 0;
  }
  if ( s_nWordCount < 3 )
    return 0;
  v8 = atoi(s_pwszDstCheat[2]);
  v9 = 0;
  if ( s_nWordCount >= 4 )
    v9 = atoi(s_pwszDstCheat[3]);
  return g_HolySys.ct_KeeperStart(4, v8, static_cast<unsigned int>(v9));
}

bool __fastcall ct_HolySystem_Jp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-48h] BYREF
  int v5; // [rsp+20h] [rbp-28h]
  int nRace; // [rsp+24h] [rbp-24h]
  int nPassTime; // [rsp+28h] [rbp-20h]
  int v8; // [rsp+2Ch] [rbp-1Ch]
  int v9; // [rsp+30h] [rbp-18h]
  int v10; // [rsp+34h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  if ( !strcmp_0("start", s_pwszDstCheat[0]) )
  {
    if ( s_nWordCount >= 2 )
    {
      v5 = atoi(s_pwszDstCheat[1]);
      g_HolySys.AlterSchedule(1u, static_cast<unsigned __int8>(v5));
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
      v10 = atoi(s_pwszDstCheat[2]);
      return g_HolySys.ct_KeeperStart(6, v10, 0);
    }
    return 0;
  }
  if ( s_nWordCount < 3 )
    return 0;
  v8 = atoi(s_pwszDstCheat[2]);
  v9 = 0;
  if ( s_nWordCount >= 4 )
    v9 = atoi(s_pwszDstCheat[3]);
  return g_HolySys.ct_KeeperStart(4, v8, static_cast<unsigned int>(v9));
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-48h] BYREF
  CPvpPointLimiter result; // [rsp+20h] [rbp-28h] BYREF
  __int64 v6; // [rsp+28h] [rbp-20h]
  CPvpPointLimiter *PvpPointLimiter; // [rsp+30h] [rbp-18h]
  CPvpPointLimiter *v8; // [rsp+38h] [rbp-10h]

  v6 = -2LL;
  PvpPointLimiter = pOne->GetPvpPointLimiter(&result);
  v8 = PvpPointLimiter;
  PvpPointLimiter->CheatUpdate(pOne->m_Param.m_dbChar.m_dPvPPoint);
  return 1;
}

bool __fastcall ct_set_animus_lv(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( s_nWordCount > 2 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_set_animus_lv(v4);
}

bool __fastcall ct_full_animus_gauge(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return s_nWordCount <= 1 && pOne->dev_full_animus_gauge();
}

bool __fastcall ct_NuAfterEffect(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  bool *v1; // rdi
  __int64 i; // rcx
  bool v4; // [rsp+0h] [rbp-18h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  v4 = !pOne->m_bSFDelayNotCheck;
  pOne->m_bSFDelayNotCheck = v4;
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  CChatStealSystem *v4; // rax
  CChatStealSystem *v5; // rax
  CChatStealSystem *v6; // rax
  CChatStealSystem *v7; // rax
  CChatStealSystem *v8; // rax
  CChatStealSystem *v9; // rax
  CChatStealSystem *v10; // rax
  CChatStealSystem *v11; // rax
  CChatStealSystem *v12; // rax
  __int64 v13; // [rsp+0h] [rbp-38h] BYREF
  int v14; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  v4 = CChatStealSystem::Instance();
  if ( !v4->SetGm(pOne) )
    return 0;
  if ( s_nWordCount >= 1 )
  {
    v14 = atoi(s_pwszDstCheat[1]);
    if ( !strcmp_0("char", s_pwszDstCheat[0]) )
    {
      if ( s_nWordCount >= 3 )
      {
        if ( !strcmp_0(byte_1407AEA8C, s_pwszDstCheat[1]) )
        {
          v5 = CChatStealSystem::Instance();
          return v5->SetTargetInfoFromCharacter(2u, s_pwszDstCheat[2]);
        }
        if ( !strcmp_0(aA_49, s_pwszDstCheat[1]) )
        {
          v6 = CChatStealSystem::Instance();
          return v6->SetTargetInfoFromCharacter(4u, s_pwszDstCheat[2]);
        }
        if ( !strcmp_0(byte_1407AEA9C, s_pwszDstCheat[1]) )
        {
          v7 = CChatStealSystem::Instance();
          return v7->SetTargetInfoFromCharacter(3u, s_pwszDstCheat[2]);
        }
        if ( !strcmp_0(aAi_8, s_pwszDstCheat[1]) )
        {
          v8 = CChatStealSystem::Instance();
          return v8->SetTargetInfoFromCharacter(1u, s_pwszDstCheat[2]);
        }
        if ( !strcmp_0(aAua_0, s_pwszDstCheat[1]) )
        {
          v9 = CChatStealSystem::Instance();
          return v9->SetTargetInfoFromCharacter(5u, s_pwszDstCheat[2]);
        }
      }
    }
    else if ( !strcmp_0("race", s_pwszDstCheat[0]) )
    {
      if ( s_nWordCount >= 2 )
      {
        v10 = CChatStealSystem::Instance();
        return v10->SetTargetInfoFromRace(7u, static_cast<unsigned __int8>(v14));
      }
    }
    else if ( !strcmp_0("boss", s_pwszDstCheat[0]) )
    {
      if ( s_nWordCount >= 2 )
      {
        v11 = CChatStealSystem::Instance();
        return v11->SetTargetInfoFromBoss(6u, static_cast<unsigned __int8>(v14));
      }
    }
    else if ( !strcmp_0("off", s_pwszDstCheat[0]) )
    {
      v12 = CChatStealSystem::Instance();
      return v12->SetGm(nullptr);
    }
  }
  return 0;
}

bool __fastcall ct_drop_jade(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-98h] BYREF
  int nNum; // [rsp+30h] [rbp-68h]
  char szTran[56]; // [rsp+48h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nNum = 1;
  if ( s_nWordCount >= 2 )
    nNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  return pOne->dev_drop_item(szTran, nNum, nullptr);
}

bool __fastcall ct_set_ore_amount(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  COreAmountMgr *v4; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
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
  v4 = COreAmountMgr::Instance();
  return v4->CheatOreAmount(dwTot, dwRemain);
}

bool __fastcall ct_query_remain_ore(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  COreAmountMgr *v4; // rax
  COreAmountMgr *v5; // rax
  unsigned int RemainOre; // eax
  __int64 v7; // [rsp+0h] [rbp-108h] BYREF
  char Buffer[136]; // [rsp+50h] [rbp-B8h] BYREF
  float *MultipleRate; // [rsp+D8h] [rbp-30h]
  int j; // [rsp+E0h] [rbp-28h]
  int k; // [rsp+E4h] [rbp-24h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 128);
  v4 = COreAmountMgr::Instance();
  MultipleRate = v4->GetMultipleRate();
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
  v5 = COreAmountMgr::Instance();
  RemainOre = v5->GetRemainOre();
  sprintf(Buffer, "Remain Ore : %d", RemainOre);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_continue_palytime_inc(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount > 2 )
    return 0;
  v4 = atoi(s_pwszDstCheat[0]);
  return pOne->m_kPcBangCoupon.SetCheetContTime(pOne->m_ObjID.m_wIndex, v4);
}

bool __fastcall ct_server_rate(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-198h] BYREF
  char Buffer[272]; // [rsp+60h] [rbp-138h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 255);
  sprintf_s(
    Buffer,
    0xFFuLL,
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-188h] BYREF
  char Buffer[272]; // [rsp+60h] [rbp-128h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 255);
  sprintf_s(
    Buffer,
    0xFFuLL,
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-138h] BYREF
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool started; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 0x20u);
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-138h] BYREF
  char szTran[72]; // [rsp+48h] [rbp-F0h] BYREF
  char pwszErrCode[132]; // [rsp+90h] [rbp-A8h] BYREF
  bool v7; // [rsp+114h] [rbp-24h]

  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  v7 = g_MonsterEventSet->StopEventSet(szTran, pwszErrCode);
  if ( !pOne )
    return 0;
  if ( v7 )
    sprintf(wszRespon, "Event Set Stop %s", s_pwszDstCheat[0]);
  else
    sprintf(wszRespon, "Event Set Stop Error %s >> %s", s_pwszDstCheat[0], pwszErrCode);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return v7;
}

bool __fastcall ct_set_temp_cash_point(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CPvpOrderView *PvpOrderView; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
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

  W2M(s_pwszDstCheat[1], scriptName, 0x20u);
  if ( s_nWordCount >= 3 )
    W2M(s_pwszDstCheat[2], commandArg, 0x20u);

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-198h] BYREF
  char _Dest[128]; // [rsp+50h] [rbp-148h] BYREF
  char pwszMessage[128]; // [rsp+F0h] [rbp-A8h] BYREF
  int iPeriod; // [rsp+174h] [rbp-24h]

  if ( !pOne )
    return 0;
  if ( !pOne->m_byUserDgr )
    return 0;
  if ( s_nWordCount == 3 )
  {
    if ( strlen_0(s_pwszDstCheat[2]) && strlen_0(s_pwszDstCheat[2]) <= 0x7F )
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  ITEM_ROOT_RATE = v6;
  return 1;
}

bool __fastcall ct_minespeed(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  MINE_SPEED_RATE = v6;
  return 1;
}

bool __fastcall ct_sfmastery(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  FORCE_LIVER_ACCUM_RATE = v6;
  return 1;
}

bool __fastcall ct_basemastery(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  MASTERY_GET_RATE = v6;
  return 1;
}

bool __fastcall ct_animusexp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  ANIMUS_EXP_RATE = v6;
  return 1;
}

bool __fastcall ct_playerexp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PLAYER_EXP_RATE = v6;
  return 1;
}

bool __fastcall ct_darkholereward(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  TSVR_ADD_DARKHOLE_REWARD_RATE = v6;
  return 1;
}

bool __fastcall ct_pcitemloot(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PCBANG_PRIMIUM_FAVOR::ITEM_DROP = v6;
  return 1;
}

bool __fastcall ct_pcminespeed(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PCBANG_PRIMIUM_FAVOR::MINING_SPEED = v6;
  return 1;
}

bool __fastcall ct_pcsfmastery(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY = v6;
  return 1;
}

bool __fastcall ct_pcbasemastery(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PCBANG_PRIMIUM_FAVOR::BASE_MASTERY = v6;
  return 1;
}

bool __fastcall ct_pcanimusexp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PCBANG_PRIMIUM_FAVOR::ANIMUS_EXP = v6;
  return 1;
}

bool __fastcall ct_pcplayerexp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 )
    return 0;
  if ( v6 > 100.0 )
    v6 = FLOAT_100_0;
  PCBANG_PRIMIUM_FAVOR::PLAYER_EXP = v6;
  return 1;
}

bool __fastcall ct_mepcbang(CPlayer *pOne)
{
  unsigned int v6;
  _AVATOR_DATA *pData; // [rsp+48h] [rbp-10h]
  CPcBangFavor *pcBang;

  if ( !pOne )
    return 0;
  pData = &pOne->m_pUserDB->m_AvatorData;
  pcBang = CPcBangFavor::Instance();
  if ( !pcBang )
    return 0;
  v6 = pcBang->ClassCodePasing(pData, pOne);
  if ( v6 == -1 )
    return 0;
  sprintf(wszRespon, "you pcbang reward list index : %u", v6);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_pcbangitemget(CPlayer *pOne)
{
  CPcBangFavor *pcBang;
  unsigned int v6; // [rsp+30h] [rbp-78h]
  char v7; // [rsp+44h] [rbp-64h]
  char v8; // [rsp+45h] [rbp-63h]
  char v9; // [rsp+46h] [rbp-62h]
  char v10; // [rsp+47h] [rbp-61h]
  char v11; // [rsp+48h] [rbp-60h]
  unsigned int dwRecIndex; // [rsp+68h] [rbp-40h]
  unsigned __int8 bySeletItemIndex[36]; // [rsp+6Ch] [rbp-3Ch] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 2 )
    return 0;
  v6 = atoi(s_pwszDstCheat[0]);
  v7 = *s_pwszDstCheat[1];
  v8 = s_pwszDstCheat[1][1];
  v9 = s_pwszDstCheat[1][2];
  v10 = s_pwszDstCheat[1][3];
  v11 = s_pwszDstCheat[1][4];
  dwRecIndex = v6;
  bySeletItemIndex[0] = v7 - 48;
  bySeletItemIndex[1] = v8 - 48;
  bySeletItemIndex[2] = v9 - 48;
  bySeletItemIndex[3] = v10 - 48;
  bySeletItemIndex[4] = v11 - 48;
  pcBang = CPcBangFavor::Instance();
  return pcBang && pcBang->PcBangGiveItem(pOne, dwRecIndex, bySeletItemIndex, 5);
}

bool __fastcall ct_expire_pcbang(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  pOne->Billing_Logout();
  pOne->m_pUserDB->m_BillingInfo.iType = 1;
  return 1;
}

bool __fastcall ct_vote_enable(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  pOne->PushDQSCheatPlyerVoteInfo();
  return 1;
}

bool __fastcall ct_elect_info_player(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  const char *v4; // rax
  const char *CharNameW; // rax
  __int64 v6; // [rsp+0h] [rbp-2E8h] BYREF
  unsigned int dwAccumPlayTime; // [rsp+50h] [rbp-298h]
  unsigned int dwLastResetDate; // [rsp+54h] [rbp-294h]
  unsigned __int16 wScanerCnt; // [rsp+58h] [rbp-290h]
  unsigned __int8 VoteEnable; // [rsp+5Ch] [rbp-28Ch]
  BOOL m_bOverlapVote; // [rsp+60h] [rbp-288h]
  char Destination[56]; // [rsp+78h] [rbp-270h] BYREF
  char Buffer[520]; // [rsp+B0h] [rbp-238h] BYREF
  CPlayer *v14; // [rsp+2B8h] [rbp-30h]
  int j; // [rsp+2C0h] [rbp-28h]

  dwAccumPlayTime = 0;
  dwLastResetDate = 0;
  wScanerCnt = 0;
  VoteEnable = 0;
  m_bOverlapVote = 0;
  memset(Destination, 0, 17);
  memset(Buffer, 0, 512);
  if ( s_nWordCount > 0 )
  {
    if ( s_nWordCount >= 1 )
    {
      v14 = 0LL;
      for ( j = 0; j < 2532; ++j )
      {
        v14 = &g_Player[j];
        CharNameW = v14->m_Param.GetCharNameW();
        if ( !strcmp_0(CharNameW, s_pwszDstCheat[0]) )
        {
          dwAccumPlayTime = v14->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime;
          wScanerCnt = v14->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt;
          VoteEnable = v14->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable;
          m_bOverlapVote = v14->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote;
          dwLastResetDate = pOne->m_pUserDB->m_AvatorData.dbSupplement.dwLastResetDate;
          strcpy_0(Destination, s_pwszDstCheat[0]);
          sprintf_s(
            Buffer,
            0x200uLL,
            "Player Vote Info : Name = %s, ResetTime = %d, AccTime = %d, Scaner = %d, Enable = %d, Over = %d",
            Destination,
            dwLastResetDate,
            dwAccumPlayTime,
            wScanerCnt,
            VoteEnable,
            m_bOverlapVote);
          goto $RESULT_128;
        }
      }
    }
    sprintf_s(Buffer, 0x200uLL, "Player Search Fail!! -> Name = %s", s_pwszDstCheat[0]);
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
    v4 = pOne->m_Param.GetCharNameW();
    strcpy_0(Destination, v4);
    sprintf_s(
      Buffer,
      0x200uLL,
      "Player Vote Info : Name = %s, Token = %d, AccTime = %d, Scaner = %d, Enable = %d, Over = %d",
      Destination,
      dwLastResetDate,
      dwAccumPlayTime,
      wScanerCnt,
      VoteEnable,
      m_bOverlapVote);
  }
$RESULT_128:
  s_logCheat.Write( Buffer);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_elect_set_player(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned int v5; // [rsp+20h] [rbp-18h]
  unsigned __int16 v6; // [rsp+24h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  v5 = atoi(s_pwszDstCheat[0]);
  v6 = atoi(s_pwszDstCheat[1]);
  pOne->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime = v5;
  pOne->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt = v6;
  pOne->m_pUserDB->m_AvatorData.dbSupplement.dwScanerGetDate = 199001011;
  pOne->PushDQSUpdatePlyerVoteInfo();
  return 1;
}

bool __fastcall ct_elect_set_env(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned __int16 PeriodCnt; // ax
  unsigned int v5; // eax
  unsigned __int16 v6; // ax
  int v7; // eax
  unsigned int PlayFDegree; // eax
  unsigned int v9; // eax
  __int64 v10; // [rsp+0h] [rbp-F8h] BYREF
  char Buffer[132]; // [rsp+50h] [rbp-A8h] BYREF
  int j; // [rsp+D4h] [rbp-24h]

  memset(Buffer, 0, 125);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 0x7DuLL, aCheatHelpSynta_0);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else if ( s_nWordCount <= 0 || s_nWordCount < g_Main.m_pTimeLimitMgr->GetPeriodCnt() )
  {
    return 0;
  }
  else
  {
    for ( j = 0; ; ++j )
    {
      PeriodCnt = g_Main.m_pTimeLimitMgr->GetPeriodCnt();
      if ( j >= PeriodCnt )
        break;
      v5 = atoi(s_pwszDstCheat[j]);
      g_Main.m_pTimeLimitMgr->SetTime(static_cast<unsigned __int16>(v5), static_cast<unsigned __int16>(j));
    }
    g_Main.m_pTimeLimitMgr->ReInitFatigue();
    v6 = g_Main.m_pTimeLimitMgr->GetPeriodCnt();
    v7 = atoi(s_pwszDstCheat[v6 - 1]);
    g_Main.m_pTimeLimitMgr->SetPlayFDegree(60000 * v7 / 100);
    PlayFDegree = static_cast<unsigned int>(g_Main.m_pTimeLimitMgr->GetPlayFDegree());
    g_Main.m_pTimeLimitMgr->SetLogoutFDegree(PlayFDegree);
    g_Main.m_pTimeLimitMgr->m_tmLoopTime.StopTimer();
    v9 = static_cast<unsigned int>(g_Main.m_pTimeLimitMgr->GetPlayFDegree());
    g_Main.m_pTimeLimitMgr->m_tmLoopTime.BeginTimer(v9);
    return 1;
  }
}

bool __fastcall ct_tl_info_set(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-128h] BYREF
  char Buffer[152]; // [rsp+50h] [rbp-D8h] BYREF
  _qry_case_insert_timelimit_info v7; // [rsp+E8h] [rbp-40h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 125);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 0x7DuLL, aCheatHelpSynta_1);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else if ( s_nWordCount <= 1 )
  {
    return 0;
  }
  else
  {
    v7.dwFatigue = atoi(s_pwszDstCheat[0]);
    v7.wStatus = atoi(s_pwszDstCheat[1]);
    v7.dwAccSerial = pOne->m_pUserDB->m_dwAccountSerial;
    v7.wIndex = pOne->m_id.wIndex;
    v4 = static_cast<int>(v7.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x9Bu, reinterpret_cast<char *>(&v7), v4);
    return 1;
  }
}

bool __fastcall ct_tl_info_view(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  const char *v4; // rax
  const char *CharNameW; // rax
  const char *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-2C8h] BYREF
  unsigned int dwFatigue; // [rsp+40h] [rbp-288h]
  unsigned int dwLastLogoutTime; // [rsp+44h] [rbp-284h]
  __int16 byTLStatus; // [rsp+48h] [rbp-280h]
  char Destination[56]; // [rsp+58h] [rbp-270h] BYREF
  char Buffer[520]; // [rsp+90h] [rbp-238h] BYREF
  CPlayer *v13; // [rsp+298h] [rbp-30h]
  int j; // [rsp+2A0h] [rbp-28h]

  dwFatigue = 0;
  dwLastLogoutTime = 0;
  byTLStatus = 0;
  memset(Destination, 0, 17);
  memset(Buffer, 0, 512);
  if ( s_nWordCount > 0 )
  {
    if ( s_nWordCount >= 1 )
    {
      v13 = 0LL;
      for ( j = 0; j < 2532; ++j )
      {
        v13 = &g_Player[j];
        CharNameW = v13->m_Param.GetCharNameW();
        if ( !strcmp_0(CharNameW, s_pwszDstCheat[0]) )
        {
          dwFatigue = v13->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
          dwLastLogoutTime = v13->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime;
          byTLStatus = v13->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;
          v6 = v13->m_Param.GetCharNameW();
          strcpy_0(Destination, v6);
          sprintf_s(
            Buffer,
            0x200uLL,
            "Player Time Limit Info : Name = %s, Fatigue = %d, Status = %d, LogoutTime = %d",
            Destination,
            dwFatigue,
            byTLStatus,
            dwLastLogoutTime);
          goto $RESULT_129;
        }
      }
    }
    sprintf_s(Buffer, 0x200uLL, "Player Search Fail!! -> Name = %s", s_pwszDstCheat[0]);
  }
  else
  {
    if ( !pOne || !pOne->m_bOper )
      return 0;
    dwFatigue = pOne->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwFatigue;
    dwLastLogoutTime = pOne->m_pUserDB->m_AvatorData.dbTimeLimitInfo.dwLastLogoutTime;
    byTLStatus = pOne->m_pUserDB->m_AvatorData.dbTimeLimitInfo.byTLStatus;
    v4 = pOne->m_Param.GetCharNameW();
    strcpy_0(Destination, v4);
    sprintf_s(
      Buffer,
      0x200uLL,
      "Player Time Limit Info : Name = %s, Fatigue = %d, Status = %d, LogoutTime = %d",
      Destination,
      dwFatigue,
      byTLStatus,
      dwLastLogoutTime);
  }
$RESULT_129:
  s_logCheat.Write( Buffer);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_tl_system_setting(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-148h] BYREF
  char Buffer[152]; // [rsp+50h] [rbp-F8h] BYREF
  char v6[60]; // [rsp+E8h] [rbp-60h] BYREF
  unsigned __int16 v7; // [rsp+124h] [rbp-24h]
  int v8; // [rsp+130h] [rbp-18h]

  memset(Buffer, 0, 125);
  memset(v6, 0, 50);
  if ( s_nWordCount <= 0 )
    return 0;
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 0x7DuLL, "Cheat Help : Syntax = tlsysset 0 ~ 2,   0 = TL Disable, 1 = TL Enable, 2 = Suspend");
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else
  {
      v7 = atoi(s_pwszDstCheat[0]);
      v8 = v7;
      if ( v7 )
      {
        if ( v8 == 1 )
        {
        g_Main.m_pTimeLimitMgr->SetTLEnable(1u);
          sprintf_s(v6, 0x32uLL, "Time Limit System Enable");
        }
        else if ( v8 == 2 )
        {
        g_Main.m_pTimeLimitMgr->SetTLEnable(2u);
          sprintf_s(v6, 0x32uLL, "Time Limit System Suspend");
        }
      }
      else
      {
      g_Main.m_pTimeLimitMgr->SetTLEnable(0);
        sprintf_s(v6, 0x32uLL, "Time Limit System Disable");
      }
    sprintf_s(Buffer, 0x7DuLL, "Time Limit Info : %s", v6);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
}

bool __fastcall ct_action_point_set(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-F8h] BYREF
  char Buffer[132]; // [rsp+50h] [rbp-A8h] BYREF
  unsigned __int8 v6; // [rsp+D4h] [rbp-24h]
  unsigned int dwPoint; // [rsp+D8h] [rbp-20h]
  int j; // [rsp+DCh] [rbp-1Ch]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(Buffer, 0, 125);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 0x7DuLL, aCheatHelpSynta_2);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  else if ( s_nWordCount <= 1 )
  {
    return 0;
  }
  else
  {
    v6 = atoi(s_pwszDstCheat[0]);
    dwPoint = atoi(s_pwszDstCheat[1]);
    if ( v6 == 99 )
    {
      for ( j = 0; j < 3; ++j )
      {
        pOne->m_pUserDB->Update_User_Action_Point(static_cast<unsigned __int8>(j), dwPoint);
        pOne->SendMsg_Alter_Action_Point(j, dwPoint);
      }
    }
    else
    {
      pOne->m_pUserDB->Update_User_Action_Point(v6, dwPoint);
      pOne->SendMsg_Alter_Action_Point(v6, dwPoint);
    }
    return 1;
  }
}

bool __fastcall ct_Win_RaceWar(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  int RaceCode; // eax
  __int64 v5; // [rsp+0h] [rbp-28h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  RaceCode = pOne->m_Param.GetRaceCode();
  g_HolySys.m_SaveData.m_nHolyMasterRace = RaceCode;
  return 1;
}

bool __fastcall ct_Gold_Age_Event_Status(CPlayer *pOne)
{
  CGoldenBoxItemMgr *goldenBox;
  unsigned __int8 Event_Status; // [rsp+40h] [rbp-188h]
  char Buffer[72]; // [rsp+58h] [rbp-170h] BYREF
  char pwszMessage[272]; // [rsp+A0h] [rbp-128h] BYREF
  unsigned __int8 v9; // [rsp+1B0h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  goldenBox = CGoldenBoxItemMgr::Instance();
  if ( !goldenBox )
    return 0;
  Event_Status = goldenBox->Get_Event_Status();
  memset(Buffer, 0, 30);
  memset(pwszMessage, 0, 256);
  v9 = Event_Status;
  if ( Event_Status )
  {
    switch ( v9 )
    {
      case 1u:
        sprintf_s(Buffer, 0x1EuLL, byte_1407AF104);
        break;
      case 2u:
        sprintf_s(Buffer, 0x1EuLL, byte_1407AF0F8);
        break;
      case 3u:
        sprintf_s(Buffer, 0x1EuLL, aA_50);
        break;
      default:
        sprintf_s(Buffer, 0x1EuLL, byte_1407AF110);
        break;
    }
  }
  else
  {
    sprintf_s(Buffer, 0x1EuLL, aOne_3);
  }
  sprintf_s(pwszMessage, 0x100uLL, aCoacE, Buffer);
  pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, pwszMessage, 0xFFu, 0LL);
  return 1;
}

bool __fastcall ct_Gold_Age_Set_Event_Status(CPlayer *pOne)
{
  CGoldenBoxItemMgr *goldenBox;
  char Buffer[260]; // [rsp+50h] [rbp-128h] BYREF
  char v9; // [rsp+154h] [rbp-24h]
  char v10; // [rsp+160h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  goldenBox = CGoldenBoxItemMgr::Instance();
  if ( !goldenBox )
    return 0;
  memset(Buffer, 0, 256);
  if ( !strcmp_0(s_pwszDstCheat[0], "?") )
  {
    sprintf_s(Buffer, 0x100uLL, aCheatHelpSynta_3);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
  if ( s_nWordCount < 1 )
    return 0;
  v9 = atoi(s_pwszDstCheat[0]);
  v10 = v9;
  if ( v9 )
  {
    if ( v10 == 1 )
    {
      goldenBox->Set_Event_Status(2u);
      return 1;
    }
    if ( v10 == 2 )
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
      memset_0(Buffer, 0, 0x100uLL);
      bFilter = goldenBox->Get_Box_Count(static_cast<unsigned __int8>(j));
      sprintf_s(Buffer, 0x100uLL, aCoacCodeDBoxAc, (unsigned int)j, bFilter);
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    }
    return 1;
  }
  else
  {
    sprintf_s(Buffer, 0x100uLL, aCoacE_0);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, Buffer, 0xFFu, 0LL);
    return 1;
  }
}

bool __fastcall ct_set_hp(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  float v6; // [rsp+20h] [rbp-18h]
  float prob; // [rsp+24h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v6 = v4;
  if ( v4 <= 0.0 || v6 > 100.0 )
    return 0;
  prob = v6 / 100.0;
  pOne->dev_set_hp(v6 / 100.0);
  return 1;
}

bool __fastcall ct_set_hfs_full(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

  return pOne && pOne->m_bOper && pOne->SF_HFSInc_Once(pOne);
}

bool __fastcall ct_alter_dalant(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-48h] BYREF
  char *EndPtr; // [rsp+28h] [rbp-20h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return pOne->dev_dalant(0xFFFFFFFF);
  v4 = strtoul(s_pwszDstCheat[0], &EndPtr, 10);
  return pOne->dev_dalant(v4);
}

bool __fastcall ct_alter_gold(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int v4; // eax
  __int64 v5; // [rsp+0h] [rbp-48h] BYREF
  char *EndPtr; // [rsp+28h] [rbp-20h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return pOne->dev_gold(0xFFFFFFFF);
  v4 = strtoul(s_pwszDstCheat[0], &EndPtr, 10);
  return pOne->dev_gold(v4);
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  int v5; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v5 = atoi(s_pwszDstCheat[1]);
  g_HolySys.AlterSchedule(0, static_cast<unsigned __int8>(v5));
  return 1;
}

bool __fastcall ct_SetSettleOwnerGuild(CPlayer *pOne)
{
  AutoMineMachineMng *machineMgr;
  AutoMineMachine *machine;
  unsigned int v10; // [rsp+20h] [rbp-38h]
  CGuild *pGuild; // [rsp+28h] [rbp-30h]
  CGuild *ownerGuild;
  int nRaceCode; // [rsp+40h] [rbp-18h]
  unsigned int dw2ThGuildSerial; // [rsp+44h] [rbp-14h]
  unsigned int dw1ThGuildSerial; // [rsp+48h] [rbp-10h]

  if ( !pOne || s_nWordCount < 2 )
    return 0;
  v10 = atoi(s_pwszDstCheat[0]);
  if ( v10 >= 2 )
    return 0;
  pGuild = GetGuildPtrFromName(g_Guild, 500, s_pwszDstCheat[1]);
  if ( !pGuild )
    return 0;
  nRaceCode = pGuild->m_byRace;
  machineMgr = AutoMineMachineMng::Instance();
  machineMgr->ChangeOwner(nRaceCode, pGuild, static_cast<unsigned __int8>(v10));
  if ( v10 )
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
  int v13; // [rsp+28h] [rbp-30h]
  unsigned int v14; // [rsp+2Ch] [rbp-2Ch]
  CGuild *ownerGuild; // [rsp+30h] [rbp-28h]
  unsigned int dw2ThGuildSerial; // [rsp+40h] [rbp-18h]
  unsigned int dw1ThGuildSerial; // [rsp+44h] [rbp-14h]

  if ( s_nWordCount )
  {
    if ( s_nWordCount == 2 )
    {
      v13 = atoi(s_pwszDstCheat[0]);
      v14 = atoi(s_pwszDstCheat[0]);
      if ( (unsigned int)v13 <= 2 )
      {
        if ( v14 < 2 )
        {
          machineMgr = AutoMineMachineMng::Instance();
          machineMgr->ChangeOwner(v13, nullptr, static_cast<unsigned __int8>(v14));
          if ( v14 )
          {
            machine = machineMgr->GetMachine(static_cast<unsigned __int8>(v13), 0);
            ownerGuild = machine ? machine->m_pOwnerGuild : nullptr;
            if ( ownerGuild )
              dw1ThGuildSerial = ownerGuild->m_dwSerial;
            else
              dw1ThGuildSerial = 0;
            g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(static_cast<unsigned __int8>(v13), dw1ThGuildSerial, 0);
          }
          else
          {
            machine = machineMgr->GetMachine(static_cast<unsigned __int8>(v13), 1u);
            ownerGuild = machine ? machine->m_pOwnerGuild : nullptr;
            if ( ownerGuild )
              dw2ThGuildSerial = ownerGuild->m_dwSerial;
            else
              dw2ThGuildSerial = 0;
            g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(static_cast<unsigned __int8>(v13), 0, dw2ThGuildSerial);
          }
          g_Main.m_kEtcNotifyInfo.Notify(static_cast<unsigned __int8>(v13));
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
    g_Main.m_kEtcNotifyInfo.Notify(0xFFu);
    return 1;
  }
}

bool __fastcall ct_ReqPunishment(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v5; // [rsp+20h] [rbp-18h]

  if ( s_nWordCount != 3 )
    return 0;
  v5 = atoi(s_pwszDstCheat[0]);
  if ( v5 >= 3u )
    return 0;
  pOne->pc_RequestPatriarchPunishment(v5, s_pwszDstCheat[1], s_pwszDstCheat[2]);
  return 1;
}

bool __fastcall ct_ReqChangeHonorGuild(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  char *CharNameW; // rax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
  CUserDB *v6; // [rsp+20h] [rbp-18h]
  CPlayer *v7; // [rsp+28h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v6 = SearchAvatorWithName(g_UserDB, 2532, s_pwszDstCheat[0]);
  if ( !v6 )
    return 0;
  v7 = &g_Player[v6->m_idWorld.wIndex];
  if ( !v7->m_bLive )
    return 0;
  CharNameW = pOne->m_Param.GetCharNameW();
  return v7->mgr_recall_player(CharNameW);
}

bool __fastcall ct_goto_monster(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  int v5; // [rsp+20h] [rbp-18h]
  CMonster *pMon; // [rsp+28h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v5 = atoi(s_pwszDstCheat[0]);
  pMon = &g_Monster[v5];
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-48h] BYREF
  char _Dest[2]; // [rsp+24h] [rbp-24h] BYREF

  if ( !pOne || !pOne->m_bOper )
    return 0;
  memset(_Dest, 0, sizeof(_Dest));
  sprintf_s(_Dest, sizeof(_Dest), "%s", "Test");
  return 1;
}

bool __fastcall ct_pcroom_premium(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-28h] BYREF

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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v5; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  v5 = 0;
  v5 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_SetGuildGrade(v5);
}

bool __fastcall ct_SetGuildGradeByName(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v5; // [rsp+20h] [rbp-18h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  v5 = 0;
  v5 = atoi(s_pwszDstCheat[1]);
  return pOne->dev_SetGuildGradeByName(s_pwszDstCheat[0], v5);
}

bool __fastcall ct_SetGuildGradeByGuildSerial(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned int dwGuildSerial; // [rsp+20h] [rbp-18h]
  unsigned __int8 v6; // [rsp+24h] [rbp-14h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 2 )
    return 0;
  dwGuildSerial = 0;
  dwGuildSerial = atoi(s_pwszDstCheat[0]);
  v6 = 0;
  v6 = atoi(s_pwszDstCheat[1]);
  return pOne->dev_SetGuildGradeByGuildSerial(dwGuildSerial, v6);
}

bool __fastcall ct_SetGuildMaster(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int CharSerial; // eax
  __int64 v5; // [rsp+0h] [rbp-38h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v5; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  v5 = 0;
  v5 = atoi(s_pwszDstCheat[0]);
  return pOne->dev_max_level_ext(v5);
}

bool __fastcall ct_set_exp_rate(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  float v4; // xmm0_4
  cStaticMember_Player *v5; // rax
  long double LimitExp; // xmm0_8
  long double Exp; // xmm0_8
  __int64 v8; // [rsp+0h] [rbp-58h] BYREF
  float v9; // [rsp+30h] [rbp-28h]
  long double dAlterExp; // [rsp+38h] [rbp-20h]
  int lv; // [rsp+40h] [rbp-18h]
  double v12; // [rsp+48h] [rbp-10h]

  if ( !pOne )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  v4 = atof(s_pwszDstCheat[0]);
  v9 = v4;
  if ( v4 < 0.0 || v9 >= 100.0 )
    return 0;
  lv = pOne->m_Param.GetLevel();
  v5 = cStaticMember_Player::Instance();
  LimitExp = v5->GetLimitExp(lv);
  v12 = LimitExp * (float)(v9 / 100.0);
  Exp = pOne->m_Param.GetExp();
  dAlterExp = v12 - Exp;
  pOne->AlterExp(v12 - Exp, 1, 0, 0);
  return 1;
}

bool __fastcall ct_goto_npc(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-B8h] BYREF
  char Destination[72]; // [rsp+30h] [rbp-88h] BYREF
  CMerchant *pNpc; // [rsp+78h] [rbp-40h]
  __int64 v7; // [rsp+80h] [rbp-38h]
  int j; // [rsp+88h] [rbp-30h]
  char *Str1; // [rsp+90h] [rbp-28h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  strcpy_0(Destination, s_pwszDstCheat[0]);
  pNpc = 0LL;
  v7 = 0LL;
  for ( j = 0; j < CMerchant::s_nLiveNum; ++j )
  {
    Str1 = g_NPC[j].m_pItemStore->GetNpcCode();
    if ( Str1 && !strcmp_0(Str1, Destination) )
    {
      pNpc = &g_NPC[j];
      return pNpc && pOne->dev_goto_npc(pNpc);
    }
  }
  return pNpc && pOne->dev_goto_npc(pNpc);
}

bool __fastcall ct_request_npc_quest(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-98h] BYREF
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
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v5; // [rsp+20h] [rbp-18h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  v5 = atoi(s_pwszDstCheat[0]);
  if ( v5 >= 0x1Eu )
    return 0;
  pOne->pc_QuestGiveupRequest(v5);
  return 1;
}

bool __fastcall ct_manage_guild(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  bool result; // al
  __int64 v4; // [rsp+0h] [rbp-68h] BYREF
  _guild_manage_request_clzo dwObj2; // [rsp+38h] [rbp-30h] BYREF
  int byManageType; // [rsp+54h] [rbp-14h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  dwObj2.byManageType = atoi(s_pwszDstCheat[0]);
  if ( dwObj2.byManageType >= 6u )
    return 0;
  byManageType = dwObj2.byManageType;
  switch ( dwObj2.byManageType )
  {
    case 0u:
      if ( s_nWordCount == 5 )
      {
        dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
        dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
        dwObj2.dwManageObj2 = atoi(s_pwszDstCheat[3]);
        dwObj2.dwManageObj3 = atoi(s_pwszDstCheat[4]);
        goto LABEL_28;
      }
      result = 0;
      break;
    case 1u:
      if ( s_nWordCount == 2 )
        goto LABEL_27;
      result = 0;
      break;
    case 2u:
      if ( s_nWordCount == 4 )
        goto LABEL_18;
      result = 0;
      break;
    case 3u:
      if ( s_nWordCount == 4 )
      {
LABEL_18:
        dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
        dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
        dwObj2.dwManageObj2 = atoi(s_pwszDstCheat[3]);
        goto LABEL_28;
      }
      result = 0;
      break;
    case 4u:
      if ( s_nWordCount == 3 )
      {
        dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
        dwObj2.dwManageObj1 = atoi(s_pwszDstCheat[2]);
        goto LABEL_28;
      }
      result = 0;
      break;
    case 5u:
      if ( s_nWordCount == 2 )
      {
LABEL_27:
        dwObj2.dwManageDst = atoi(s_pwszDstCheat[1]);
        goto LABEL_28;
      }
      result = 0;
      break;
    default:
LABEL_28:
      pOne->pc_GuildManageRequest(
        dwObj2.byManageType,
        dwObj2.dwManageDst,
        dwObj2.dwManageObj1,
        dwObj2.dwManageObj2,
        dwObj2.dwManageObj3);
      result = 1;
      break;
  }
  return result;
}

bool __fastcall ct_change_master_elect(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-38h] BYREF
  bool v5; // [rsp+20h] [rbp-18h]
  bool v6; // [rsp+21h] [rbp-17h]

  if ( !pOne || !pOne->m_bOper )
    return 0;
  if ( !pOne->m_Param.m_pGuild )
    return 0;
  if ( s_nWordCount != 1 )
    return 0;
  v6 = atoi(s_pwszDstCheat[0]) != 0;
  v5 = v6;
  pOne->m_Param.m_pGuild->m_bPossibleElectMaster = v6;
  pOne->m_Param.m_pGuild->MakeDownMemberPacket();
  pOne->m_Param.m_pGuild->SendMsg_MasterElectPossible(v5);
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
  int iBegin_TT; // [rsp+28h] [rbp-20h]
  int iEnd_TT; // [rsp+2Ch] [rbp-1Ch] BYREF
  unsigned __int8 v8; // [rsp+34h] [rbp-14h]

  if ( !pOne )
    return 0;
  iBegin_TT = 0;
  memset(&iEnd_TT, 0, sizeof(iEnd_TT));
  iBegin_TT = atoi(s_pwszDstCheat[0]);
  iEnd_TT = atoi(s_pwszDstCheat[1]);
  v8 = atoi(s_pwszDstCheat[2]);
  if ( iBegin_TT <= 0 || iEnd_TT <= 0 || v8 > 3u )
    return 0;
  store = CashItemRemoteStore::Instance();
  now = std::time(nullptr);
  store->m_con_event.m_ini.m_bUseConEvent = (v8 < 3u);
  store->m_con_event.m_ini.m_byEventKind = v8;
  store->m_con_event.m_bConEvent = true;
  store->m_con_event.m_eventtime.m_EventTime[0] = static_cast<int>(now) + iBegin_TT;
  store->m_con_event.m_eventtime.m_EventTime[1] = static_cast<int>(now) + iBegin_TT + iEnd_TT;
  store->m_con_event.m_conevent_status = 1;
  return 1;
}

bool __fastcall ct_loot_item(CPlayer *pOne)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-98h] BYREF
  int nNum; // [rsp+30h] [rbp-68h]
  char szTran[56]; // [rsp+48h] [rbp-50h] BYREF

  if ( !pOne )
    return 0;
  if ( s_nWordCount < 1 )
    return 0;
  nNum = 1;
  if ( s_nWordCount >= 2 )
    nNum = atoi(s_pwszDstCheat[1]);
  W2M(s_pwszDstCheat[0], szTran, 0x20u);
  return pOne->dev_loot_item(szTran, nNum, 0LL, 0);
}



