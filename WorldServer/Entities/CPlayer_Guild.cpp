#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "pnt_rect.h"
#include "CMainThread.h"
#include "CItemStore.h"
#include "CNetworkEX.h"
#include "StoreList_fld.h"
#include "CActionPointSystemMgr.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "CGuild.h"
#include "CUserDB.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CNationSettingManager.h"
#include "CNationSettingData.h"
#include "GlobalObjects.h"
#include "CGuildBattleController.h"
#include "CGuildMasterEffect.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CHolyStoneSystem.h"
#include "CPotionMgr.h"
#include "guild_honor_set_request_clzo.h"
#include "CMgrGuildHistory.h"
#include "CMgrAvatorItemHistory.h"
#include "CPostSystemManager.h"
#include "CPartyPlayer.h"
#include "LendItemMng.h"
#include "CUnmannedTraderController.h"
#include "cStaticMember_Player.h"
#include "CTransportShip.h"
#include "CLogFile.h"
#include "CItemUpgradeTable.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "COreCuttingTable.h"
#include "CDarkHole.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CDarkHoleDungeonQuestSetup.h"
#include "TicketItem_fld.h"
#include "darkhole_create_setdata.h"
#include "ENTER_DUNGEON_NEW_POS.h"
#include "QuestHappenEvent_fld.h"
#include "announ_message_receipt_udp.h"
#include "equip_up_item_lv_limit_zocl.h"
#include "sf_delay_download_result_zocl.h"
#include "darkhole_ask_reenter_inform_zocl.h"
#include "darkhole_answer_reenter_result_zocl.h"
#include "darkhole_clear_out_result_zocl.h"
#include "darkhole_enter_result_zocl.h"
#include "darkhole_giveup_out_result_zocl.h"
#include "darkhole_open_result_zocl.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "move_to_own_stonemap_inform_zocl.h"
#include "move_to_own_stonemap_result_zocl.h"
#include "pt_result_change_tax_rate_zocl.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "economy_history_data.h"
#include "DqsDbStructs.h"
#include "EconomySystemFunctions.h"
#include "TimeItem.h"
#include "trans_ship_renew_ticket_result_zocl.h"
#include "trans_gm_msg_inform_zocl.h"
#include "trunk_change_passwd_result_zocl.h"
#include "trunk_download_result_zocl.h"
#include "trunk_est_result_zocl.h"
#include "trunk_extend_result_zocl.h"
#include "trunk_hint_answer_result_zocl.h"
#include "trunk_io_money_result_zocl.h"
#include "trunk_io_result_zocl.h"
#include "trunk_potionsocket_division_result_zocl.h"
#include "trunk_pw_hint_index_result_zocl.h"
#include "trunk_res_division_result_zocl.h"
#include "guildroom_rent_request_clzo.h"
#include "guildroom_enter_request_clzo.h"
#include "guildroom_out_request_clzo.h"
#include "guildroom_resttime_request_clzo.h"
#include "GuardTowerItem_fld.h"
#include "UnitKeyItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "BulletItem_fld.h"
#include "ItemCombine_exp_fld.h"
#include "combine_ex_item_result_zocl.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_accept_result_zocl.h"
#include "qry_case_disjointguild.h"
#include "WorldServerUtil.h"
#include "NetCheckPackets.h"
#include "GlobalObjectDefs.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace
{
struct _qry_case_joinacguild_local
{
  unsigned int in_guildindex;
  unsigned int in_guildserial;
  unsigned int in_applierindex;
  unsigned int in_applierserial;
  unsigned int in_accepterserial;
  int in_Grade;
  int in_MemberNum;

  __int64 size() const
  {
    return sizeof(*this);
  }
};

struct   _qry_case_insertguild_local
{
  unsigned int in_guildindex;
  char in_w_guildName[17];
  unsigned __int8 in_guildRace;
  unsigned __int8 in_membernum;
  unsigned __int8 in_padding;
  unsigned int in_memberindex[8];
  unsigned int in_memberserial[8];
  char tmp_w_membername[136];
  unsigned __int8 tmp_lv[8];
  unsigned __int8 tmp_grade[8];
  unsigned int tmp_pvp[8];
  unsigned int out_guildserial;
  unsigned int tmp_Esterindex;
  unsigned int tmp_Esterserial;

  _qry_case_insertguild_local()
  {
    memset_0(this, 0, sizeof(*this));
    in_guildindex = static_cast<unsigned int>(-1);
    out_guildserial = static_cast<unsigned int>(-1);
    tmp_Esterindex = static_cast<unsigned int>(-1);
    tmp_Esterserial = static_cast<unsigned int>(-1);
  }

  __int64 size() const
  {
    return sizeof(*this);
  }
};

struct _qry_case_inputgmoney_local
{
  unsigned int in_pusherserial;
  char in_w_pushername[17];
  unsigned int tmp_guildindex;
  unsigned int in_guildserial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 in_date[4];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned __int8 byProcRet;

  __int64 size() const
  {
    return sizeof(*this);
  }
};

bool IsEconomyFeeLevel(int level)
{
  return level == 30 || level == 40 || level == 50 || level == 60;
}
} // namespace


void CPlayer::SendMsg_GuildJoinApplyResult(char byRetCode, CGuild *pApplyGuild)
{
  _guild_join_apply_result_zocl packet{};
  packet.byRetCode = byRetCode;
  if (pApplyGuild)
  {
    packet.dwGuildSerial = pApplyGuild->m_dwSerial;
    strcpy_0(packet.wszGuildName, pApplyGuild->m_wszName);
  }
  else
  {
    packet.dwGuildSerial = static_cast<unsigned int>(-1);
    packet.wszGuildName[0] = 0;
  }

  unsigned __int8 type[2] = {27, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildJoinApplyCancelResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_GuildJoinApplyRejectInform()
{
  char payload = 0;
  unsigned __int8 type[2] = {27, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &payload, 1u);
}

void CPlayer::SendMsg_GuildJoinAcceptFail(char byRetCode, unsigned int dwApplierSerial)
{
  _guild_join_accept_fail_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.dwApplierSerial = dwApplierSerial;

  unsigned __int8 type[2] = {27, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_VoteResult(unsigned int dwMatterVoteSynKey, unsigned __int8 byRetCode)
{
  _guild_vote_result_zocl packet{};
  packet.dwMatterVoteSynKey = dwMatterVoteSynKey;
  packet.byRetCode = static_cast<char>(byRetCode);

  unsigned __int8 type[2] = {27, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}



void CPlayer::SendMsg_CancelSuggestResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_GuildSetHonorResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 114};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_GuildPushMoneyResult(char byRetCode)
{
  _guild_push_money_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.dwLeftDalant = this->m_Param.GetDalant();
  packet.dwLeftGold = this->m_Param.GetGold();

  unsigned __int8 type[2] = {27, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}


void CPlayer::SendMsg_OfferSuggestResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}


void CPlayer::SendMsg_GuildRoomRentResult(unsigned __int8 byRetCode, unsigned __int8 bySubRetCode, unsigned __int8 byRoomType)
{
  _guildroom_rent_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySubRetCode = static_cast<char>(bySubRetCode);
  packet.byRoomType = byRoomType;

  unsigned __int8 type[2] = {27, 103};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildRoomEnterResult(unsigned __int8 byRetCode, unsigned __int8 bySubRetCode, unsigned __int8 byMapIndex, unsigned __int16 wMapLayer, float *pPos, int nRestTime)
{
  _guildroom_enter_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.byRetSubCode = static_cast<char>(bySubRetCode);
  packet.byMapIndex = byMapIndex;
  packet.wMapLayerIndex = wMapLayer;
  FloatToShort(pPos, packet.sNewPos, 3);
  packet.restTime = nRestTime;

  unsigned __int8 type[2] = {27, 105};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildRoomRestTimeResult()
{
  // this is not a stub
}


void CPlayer::SendMsg_GuildEstablishFail(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {27, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_GuildJoinOtherInform()
{
  char msg[10]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_id.dwSerial;
  *reinterpret_cast<unsigned int *>(msg + 4) = this->m_Param.m_pGuild ? this->m_Param.m_pGuild->m_dwSerial
                                                                     : static_cast<unsigned int>(-1);
  *reinterpret_cast<unsigned __int16 *>(msg + 8) = this->m_wVisualVer;

  unsigned __int8 type[2] = {27, 39};
  CircleReport(type, msg, 10, false);
}


#if 0 // duplicate implementation exists in CPlayer.cpp

#endif




void CPlayer::Guild_Insert_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int EstConsumeDalant; // eax
  unsigned int v4; // eax
  CPlayer::CashChangeStateFlag *v5; // rax
  CGuildBattleController *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-4C8h] BYREF
  int v8; // [rsp+30h] [rbp-498h]
  unsigned int dwSerial; // [rsp+34h] [rbp-494h]
  char Destination[28]; // [rsp+48h] [rbp-480h] BYREF
  unsigned __int8 v11; // [rsp+64h] [rbp-464h]
  unsigned __int8 v12; // [rsp+65h] [rbp-463h]
  unsigned int v13[16]; // [rsp+78h] [rbp-450h] BYREF
  unsigned int dwMemberSerial[18]; // [rsp+B8h] [rbp-410h] BYREF
  char Source[168]; // [rsp+100h] [rbp-3C8h] BYREF
  _BYTE v16[32]; // [rsp+1A8h] [rbp-320h] BYREF
  _BYTE v17[32]; // [rsp+1C8h] [rbp-300h] BYREF
  _DWORD v18[11]; // [rsp+1E8h] [rbp-2E0h] BYREF
  int v19; // [rsp+214h] [rbp-2B4h]
  int v20; // [rsp+218h] [rbp-2B0h]
  char *m_sData; // [rsp+220h] [rbp-2A8h]
  CGuild *v22; // [rsp+228h] [rbp-2A0h]
  int j; // [rsp+230h] [rbp-298h]
  CPlayer *v24; // [rsp+238h] [rbp-290h]
  CPlayer *v25; // [rsp+240h] [rbp-288h]
  char szTran[136]; // [rsp+260h] [rbp-268h] BYREF
  CPlayer *v27; // [rsp+2E8h] [rbp-1E0h]
  _guild_member_info __t[8]; // [rsp+300h] [rbp-1C8h] BYREF
  CPlayer *pPtr; // [rsp+488h] [rbp-40h]
  _guild_member_info *pMem; // [rsp+490h] [rbp-38h]
  CPlayer::CashChangeStateFlag v31{0}; // [rsp+4A0h] [rbp-28h] BYREF
  char *pszFileName; // [rsp+4A8h] [rbp-20h]
  unsigned int dwLeftDalant; // [rsp+4B0h] [rbp-18h]

  v8 = -1;
  dwSerial = -1;
  v11 = 0;
  v12 = 0;
  v19 = -1;
  v20 = -1;
  m_sData = pData->m_sData;
  v8 = *(_DWORD *)pData->m_sData;
  dwSerial = *(_DWORD *)&pData->m_sData[272];
  v19 = *(_DWORD *)&pData->m_sData[276];
  v20 = *(_DWORD *)&pData->m_sData[280];
  v11 = pData->m_sData[21];
  v12 = pData->m_sData[22];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(v13, m_sData + 24, 0x20uLL);
  memcpy_0(dwMemberSerial, m_sData + 56, 0x20uLL);
  memcpy_0(v18, m_sData + 240, 0x20uLL);
  memcpy_0(v16, m_sData + 224, 8uLL);
  memcpy_0(v17, m_sData + 232, 8uLL);
  memcpy_0(Source, m_sData + 88, 0x88uLL);
  v22 = &g_Guild[v8];
  v22->ReleaseTemp();
  for ( j = 0; j < v12; ++j )
  {
    v24 = &g_Player[v13[j]];
    if ( v24->m_bLive && v24->m_dwObjSerial == dwMemberSerial[j] )
      v24->m_Param.m_bGuildLock = 0;
  }
  if ( pData->m_byResult || v22->IsFill() )
  {
    v25 = &g_Player[v19];
    if ( v25->m_bLive && v25->m_dwObjSerial == v20 )
    {
      v25->SendMsg_GuildEstablishFail( 0xFFu);
      EstConsumeDalant = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
      v25->AddDalant( EstConsumeDalant, 0);
      W2M(Destination, szTran, 0x80u);
      pszFileName = v25->m_szItemHistoryFileName;
      dwLeftDalant = v25->m_Param.GetDalant();
      v4 = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
      CPlayer::s_MgrItemHistory.guild_est_money_rollback(
        v25->m_ObjID.m_wIndex,
        szTran,
        v4,
        dwLeftDalant,
        pszFileName);
    }
  }
  else
  {
    v27 = &g_Player[v19];
    if ( v27->m_bLive && v27->m_dwObjSerial == v20 )
      v27->SendMsg_AlterMoneyInform( 0);
    for ( j = 0; j < v12; ++j )
    {
      __t[j].dwSerial = dwMemberSerial[j];
      strcpy_0(__t[j].wszName, &Source[17 * j]);
      __t[j].byLv = v16[j];
      __t[j].dwPvpPoint = v18[j];
      __t[j].byClassInGuild = 0;
      __t[j].byGrade = v17[j];
    }
    v22->EstGuild( dwSerial, Destination, v11, v12, __t);
    for ( j = 0; j < v12; ++j )
    {
      pPtr = &g_Player[v13[j]];
      if ( pPtr->m_bLive && pPtr->m_dwObjSerial == dwMemberSerial[j] )
      {
        pMem = v22->LoginMember( dwMemberSerial[j], pPtr);
        v22->SendMsg_DownPacket( 0, pMem);
        pPtr->m_pUserDB->m_AvatorData.dbAvator.m_dwGuildSerial = dwSerial;
        pPtr->m_Param.m_pGuild = v22;
        pPtr->m_Param.m_pGuildMemPtr = pMem;
        pPtr->m_Param.SetClassInGuild( 0);
        pPtr->UpdateVisualVer(v31);
        pPtr->SendMsg_GuildJoinOtherInform();
      }
    }
    v6 = CGuildBattleController::Instance();
    v6->UpdatePossibleBattleGuildList();
  }
}

void CPlayer::Guild_Join_Accept_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  const char *CharNameW; // rax
  CPlayer::CashChangeStateFlag *v4; // rax
  char *CharNameA; // rax
  CGuildBattleController *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-198h] BYREF
  int v8; // [rsp+40h] [rbp-158h]
  int v9; // [rsp+44h] [rbp-154h]
  int v10; // [rsp+48h] [rbp-150h]
  unsigned int dwApplierSerial; // [rsp+4Ch] [rbp-14Ch]
  unsigned int dwMemberSerial; // [rsp+50h] [rbp-148h]
  char *m_sData; // [rsp+58h] [rbp-140h]
  CPlayer *pPlayer; // [rsp+60h] [rbp-138h]
  CGuild *v15; // [rsp+68h] [rbp-130h]
  _guild_member_info *MemberFromSerial; // [rsp+70h] [rbp-128h]
  _guild_applier_info *ApplierFromSerial; // [rsp+78h] [rbp-120h]
  _guild_member_info pSheet; // [rsp+88h] [rbp-110h] BYREF
  _guild_member_info *p; // [rsp+C8h] [rbp-D0h]
  char szTran[144]; // [rsp+E0h] [rbp-B8h] BYREF
  CPlayer::CashChangeStateFlag v21{0}; // [rsp+170h] [rbp-28h] BYREF
  char *pszFileName; // [rsp+178h] [rbp-20h]
  int n; // [rsp+180h] [rbp-18h]

  v8 = -1;
  v9 = -1;
  v10 = -1;
  dwApplierSerial = -1;
  dwMemberSerial = -1;
  m_sData = pData->m_sData;
  v8 = *(_DWORD *)pData->m_sData;
  v9 = *(_DWORD *)&pData->m_sData[4];
  v10 = *(_DWORD *)&pData->m_sData[8];
  dwApplierSerial = *(_DWORD *)&pData->m_sData[12];
  dwMemberSerial = *(_DWORD *)&pData->m_sData[16];
  pPlayer = &g_Player[v10];
  if ( pPlayer->m_bLive && pPlayer->m_dwObjSerial == dwApplierSerial )
    pPlayer->m_Param.m_bGuildLock = 0;
  v15 = &g_Guild[v8];
  if ( v15->m_dwSerial == v9 )
  {
    --v15->m_nTempMemberNum;
    if ( v15->m_nMemberNum < 50 && !pData->m_byResult )
    {
      MemberFromSerial = v15->GetMemberFromSerial( dwMemberSerial);
      if ( MemberFromSerial )
      {
        ApplierFromSerial = v15->GetApplierFromSerial( dwApplierSerial);
        if ( ApplierFromSerial )
        {
          pPlayer = ApplierFromSerial->pPlayer;
          pPlayer->m_Param.SetClassInGuild( 0);
          pPlayer->m_Param.m_pApplyGuild = 0LL;
          v15->PopApplier( dwApplierSerial, 0);
          pSheet.dwSerial = pPlayer->m_dwObjSerial;
          CharNameW = pPlayer->m_Param.GetCharNameW();
          strcpy_0(pSheet.wszName, CharNameW);
          pSheet.byLv = pPlayer->GetLevel();
          pSheet.dwPvpPoint = (int)pPlayer->m_Param.GetPvPPoint();
          pSheet.byClassInGuild = pPlayer->m_Param.GetClassInGuild();
          pSheet.pPlayer = pPlayer;
          p = v15->PushMember( &pSheet);
          if ( p )
            v15->SendMsg_GuildJoinAcceptInform( p, dwMemberSerial);
          pPlayer->m_Param.m_pGuild = v15;
          pPlayer->m_Param.m_pGuildMemPtr = p;
          pPlayer->UpdateVisualVer(v21);
          pPlayer->SendMsg_GuildJoinOtherInform();
          
            v15->SendMsg_GuildMemberLogin(
            pPlayer->m_dwObjSerial,
            pPlayer->m_wRegionMapIndex,
            pPlayer->m_wRegionIndex);
          W2M(MemberFromSerial->wszName, szTran, 0x80u);
          pszFileName = v15->m_szHistoryFileName;
          CharNameA = pPlayer->m_Param.GetCharNameA();
          CGuild::s_MgrHistory.join_member(
            CharNameA,
            pPlayer->m_dwObjSerial,
            szTran,
            MemberFromSerial->dwSerial,
            v15->m_nMemberNum,
            pszFileName);
          n = pPlayer->m_ObjID.m_wIndex;
          v6 = CGuildBattleController::Instance();
          v6->JoinGuild(n, v15->m_dwSerial, pPlayer->m_dwObjSerial);
        }
      }
    }
  }
}

void CPlayer::Guild_Self_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildRoomSystem *Instance; // rax
  CGuildRoomSystem *v4; // rax
  CGuildBattleController *v5; // rax
  CGuildMasterEffect *v6; // rax
  CPlayer::CashChangeStateFlag *v7; // rax
  char *CharNameA; // rax
  int v9; // eax
  __int64 v10; // [rsp+0h] [rbp-E8h] BYREF
  char v11; // [rsp+40h] [rbp-A8h]
  CPlayer *pkPlayer; // [rsp+48h] [rbp-A0h]
  CGuild *v13; // [rsp+50h] [rbp-98h]
  int v14; // [rsp+58h] [rbp-90h]
  unsigned int dwMemberSerial; // [rsp+5Ch] [rbp-8Ch]
  int v16; // [rsp+60h] [rbp-88h]
  int v17; // [rsp+64h] [rbp-84h]
  char *m_sData; // [rsp+68h] [rbp-80h]
  char v19; // [rsp+70h] [rbp-78h]
  _qry_case_disjointguild v20; // [rsp+88h] [rbp-60h] BYREF
  CPlayer::CashChangeStateFlag v21{0}; // [rsp+94h] [rbp-54h] BYREF
  CUserDB *m_pUserDB; // [rsp+98h] [rbp-50h]
  int n; // [rsp+A0h] [rbp-48h]
  CGuild *m_pGuild; // [rsp+A8h] [rbp-40h]
  CUserDB *v25; // [rsp+B0h] [rbp-38h]
  int m_wIndex; // [rsp+B8h] [rbp-30h]
  CGuild *v27; // [rsp+C0h] [rbp-28h]
  unsigned __int8 Grade; // [rsp+C8h] [rbp-20h]
  char *pszFileName; // [rsp+D0h] [rbp-18h]

  v11 = 0;
  pkPlayer = 0LL;
  v13 = 0LL;
  v14 = -1;
  dwMemberSerial = -1;
  v16 = -1;
  v17 = -1;
  m_sData = pData->m_sData;
  v14 = *(_DWORD *)&pData->m_sData[8];
  dwMemberSerial = *(_DWORD *)pData->m_sData;
  v16 = *(_DWORD *)&pData->m_sData[12];
  v17 = *(_DWORD *)&pData->m_sData[16];
  v19 = 0;
  pkPlayer = &g_Player[v14];
  if ( pkPlayer->m_bLive && pkPlayer->m_dwObjSerial == dwMemberSerial )
  {
    pkPlayer->m_Param.m_bGuildLock = 0;
    v19 = 1;
  }
  if ( !v19 )
    pkPlayer = 0LL;
  v19 = 0;
  v13 = &g_Guild[v16];
  if ( v13->IsFill() && v13->m_dwSerial == v17 )
    v19 = 1;
  if ( v19 )
  {
    if ( !pkPlayer )
      goto LABEL_22;
    if ( !pkPlayer->m_pUserDB )
      goto LABEL_22;
    if ( !pkPlayer->m_Param.m_pGuild )
      goto LABEL_22;
    m_pUserDB = pkPlayer->m_pUserDB;
    n = pkPlayer->m_ObjID.m_wIndex;
    m_pGuild = pkPlayer->m_Param.m_pGuild;
    Instance = CGuildRoomSystem::GetInstance();
    if (!Instance->IsGuildRoomMemberIn(m_pGuild->m_dwSerial, n, m_pUserDB->m_dwSerial))
      goto LABEL_22;
    v25 = pkPlayer->m_pUserDB;
    m_wIndex = pkPlayer->m_ObjID.m_wIndex;
    v27 = pkPlayer->m_Param.m_pGuild;
    v4 = CGuildRoomSystem::GetInstance();
    if (v4->SetPlayerOut(v27->m_dwSerial, m_wIndex, v25->m_dwSerial))
    {
      pkPlayer->SendMsg_GuildSelfLeaveResult( 0xFFu);
    }
    else
    {
LABEL_22:
      v5 = CGuildBattleController::Instance();
      v5->LeaveGuild(pkPlayer);
      if ( pkPlayer && pkPlayer->m_Param.m_byClassInGuild == 2 )
      {
        Grade = v13->GetGrade();
        v6 = CGuildMasterEffect::GetInstance();
        v6->out_player(pkPlayer, Grade);
      }
      v13->SendMsg_LeaveMember( dwMemberSerial, 1, 0);
      v13->PopMember( dwMemberSerial);
      if ( pkPlayer )
      {
        pkPlayer->m_Param.m_pGuild = 0LL;
        pkPlayer->SendMsg_GuildSelfLeaveResult( 0);
        pkPlayer->UpdateVisualVer(v21);
        pkPlayer->SendMsg_GuildJoinOtherInform();
        pkPlayer->SetLastAttBuff( 0);
        pszFileName = v13->m_szHistoryFileName;
        CharNameA = pkPlayer->m_Param.GetCharNameA();
        CGuild::s_MgrHistory.leave_member(
          CharNameA,
          pkPlayer->m_dwObjSerial,
          1,
          v13->m_nMemberNum,
          pszFileName,
          0);
      }
      if ( v13->GetMemberNum() <= 0 )
      {
        v20.in_guildserial = v13->m_dwSerial;
        v20.tmp_guildindex = v13->m_nIndex;
        v9 = v20.size();
        g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x16u, (char *)&v20, v9);
      }
    }
  }
  else if ( pkPlayer )
  {
    pkPlayer->SendMsg_GuildSelfLeaveResult( 0xFFu);
  }
}

void CPlayer::Guild_Force_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildBattleController *v3; // rax
  CGuildRoomSystem *Instance; // rax
  CGuildRoomSystem *v5; // rax
  CPlayer::CashChangeStateFlag *v6; // rax
  char *CharNameA; // rax
  __int64 v8; // [rsp+0h] [rbp-C8h] BYREF
  CGuild *v9; // [rsp+40h] [rbp-88h]
  unsigned int dwMemberSerial; // [rsp+48h] [rbp-80h]
  int v11; // [rsp+4Ch] [rbp-7Ch]
  int v12; // [rsp+50h] [rbp-78h]
  char *m_sData; // [rsp+58h] [rbp-70h]
  char v14; // [rsp+60h] [rbp-68h]
  _guild_member_info *MemberFromSerial; // [rsp+68h] [rbp-60h]
  CPlayer *pPlayer; // [rsp+70h] [rbp-58h]
  CPlayer::CashChangeStateFlag v17{0}; // [rsp+78h] [rbp-50h] BYREF
  CUserDB *m_pUserDB; // [rsp+80h] [rbp-48h]
  int n; // [rsp+88h] [rbp-40h]
  CGuild *m_pGuild; // [rsp+90h] [rbp-38h]
  CUserDB *v21; // [rsp+98h] [rbp-30h]
  int m_wIndex; // [rsp+A0h] [rbp-28h]
  CGuild *v23; // [rsp+A8h] [rbp-20h]
  char *pszFileName; // [rsp+B0h] [rbp-18h]
  int nMemNum; // [rsp+B8h] [rbp-10h]

  v9 = 0LL;
  dwMemberSerial = -1;
  v11 = -1;
  v12 = -1;
  m_sData = pData->m_sData;
  dwMemberSerial = *(_DWORD *)pData->m_sData;
  v11 = *(_DWORD *)&pData->m_sData[4];
  v12 = *(_DWORD *)&pData->m_sData[8];
  v14 = 0;
  v9 = &g_Guild[v11];
  if ( v9->IsFill() && v9->m_dwSerial == v12 )
    v14 = 1;
  if ( v14 )
  {
    MemberFromSerial = v9->GetMemberFromSerial( dwMemberSerial);
    if ( MemberFromSerial )
    {
      if ( MemberFromSerial->pPlayer )
      {
        v3 = CGuildBattleController::Instance();
        v3->LeaveGuild(MemberFromSerial->pPlayer);
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer->m_Param.m_pGuild )
        {
          m_pUserDB = pPlayer->m_pUserDB;
          n = pPlayer->m_ObjID.m_wIndex;
          m_pGuild = pPlayer->m_Param.m_pGuild;
          Instance = CGuildRoomSystem::GetInstance();
          if (Instance->IsGuildRoomMemberIn(m_pGuild->m_dwSerial, n, m_pUserDB->m_dwSerial))
          {
            v21 = pPlayer->m_pUserDB;
            m_wIndex = pPlayer->m_ObjID.m_wIndex;
            v23 = pPlayer->m_Param.m_pGuild;
            v5 = CGuildRoomSystem::GetInstance();
            v5->SetPlayerOut(v23->m_dwSerial, m_wIndex, v21->m_dwSerial);
          }
        }
        MemberFromSerial->pPlayer->m_Param.m_bGuildLock = 0;
        MemberFromSerial->pPlayer->m_Param.m_pGuild = 0LL;
        MemberFromSerial->pPlayer->m_Param.m_pGuildMemPtr = 0LL;
        MemberFromSerial->pPlayer->UpdateVisualVer(v17);
        MemberFromSerial->pPlayer->SendMsg_GuildJoinOtherInform();
        MemberFromSerial->pPlayer->SetLastAttBuff( 0);
        if ( m_sData[24] )
          MemberFromSerial->pPlayer->SendMsg_GuildForceLeaveBoradori();
        pszFileName = v9->m_szHistoryFileName;
        nMemNum = v9->m_nMemberNum - 1;
        CharNameA = MemberFromSerial->pPlayer->m_Param.GetCharNameA();
        CGuild::s_MgrHistory.leave_member(
          CharNameA,
          MemberFromSerial->dwSerial,
          0,
          nMemNum,
          pszFileName,
          m_sData[24]);
      }
      v9->SendMsg_LeaveMember( dwMemberSerial, 0, m_sData[24]);
      v9->PopMember( dwMemberSerial);
    }
  }
}

void CPlayer::Guild_Push_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int Dalant; // eax
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  int v5; // [rsp+50h] [rbp-B8h]
  int v6; // [rsp+54h] [rbp-B4h]
  unsigned int dwIOerSerial; // [rsp+58h] [rbp-B0h]
  char Destination[28]; // [rsp+68h] [rbp-A0h] BYREF
  unsigned int dwPush; // [rsp+84h] [rbp-84h]
  unsigned int dwPopDalant; // [rsp+88h] [rbp-80h]
  long double dTotalGold; // [rsp+90h] [rbp-78h]
  long double dTotalDalant; // [rsp+98h] [rbp-70h]
  unsigned __int8 pbyDate[20]; // [rsp+A4h] [rbp-64h] BYREF
  char *m_sData; // [rsp+B8h] [rbp-50h]
  CGuild *v15; // [rsp+C0h] [rbp-48h]
  _guild_member_info *MemberFromSerial; // [rsp+C8h] [rbp-40h]
  CPlayer *pPlayer; // [rsp+D0h] [rbp-38h]
  char *pszFileName; // [rsp+E0h] [rbp-28h]
  unsigned int dwLeftGold; // [rsp+E8h] [rbp-20h]

  v5 = -1;
  v6 = -1;
  dwIOerSerial = -1;
  dwPush = 0;
  dwPopDalant = 0;
  dTotalGold = 0.0;
  dTotalDalant = 0.0;
  m_sData = pData->m_sData;
  v5 = *(_DWORD *)&pData->m_sData[24];
  v6 = *(_DWORD *)&pData->m_sData[28];
  dwIOerSerial = *(_DWORD *)pData->m_sData;
  dwPopDalant = *(_DWORD *)&pData->m_sData[36];
  dwPush = *(_DWORD *)&pData->m_sData[32];
  dTotalDalant = *(long double *)&pData->m_sData[56];
  dTotalGold = *(long double *)&pData->m_sData[48];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(pbyDate, m_sData + 40, 4uLL);
  v15 = &g_Guild[v5];
  if ( v15->m_dwSerial == v6 )
  {
    v15->m_bIOWait = 0;
    if ( pData->m_byResult )
    {
      MemberFromSerial = v15->GetMemberFromSerial( dwIOerSerial);
      if ( MemberFromSerial )
      {
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer )
        {
          pPlayer->AddGold( dwPush, 1);
          pPlayer->AddDalant( dwPopDalant, 1);
          pszFileName = pPlayer->m_szItemHistoryFileName;
          dwLeftGold = pPlayer->m_Param.GetGold();
          Dalant = pPlayer->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.guild_pop_money_rollback(
            pPlayer->m_ObjID.m_wIndex,
            v15->m_aszName,
            dwPopDalant,
            dwPush,
            Dalant,
            dwLeftGold,
            pszFileName);
        }
      }
    }
    else
    {
      v15->m_byMoneyOutputKind = 0;
      
        v15->IOMoney(
        Destination,
        dwIOerSerial,
        (double)(int)dwPopDalant,
        (double)(int)dwPush,
        dTotalDalant,
        dTotalGold,
        pbyDate,
        1);
    }
  }
}

void CPlayer::Guild_Pop_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int Dalant; // eax
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  __int64 dTotalDalant; // [rsp+28h] [rbp-E0h]
  __int64 dTotalGold; // [rsp+30h] [rbp-D8h]
  __int64 bInPut; // [rsp+40h] [rbp-C8h]
  __int64 v8; // [rsp+48h] [rbp-C0h]
  int v9; // [rsp+50h] [rbp-B8h]
  unsigned int v10; // [rsp+54h] [rbp-B4h]
  unsigned int dwIOerSerial; // [rsp+58h] [rbp-B0h]
  char Destination[28]; // [rsp+68h] [rbp-A0h] BYREF
  unsigned int dwPopGold; // [rsp+84h] [rbp-84h]
  unsigned int dwPush; // [rsp+88h] [rbp-80h]
  long double v15; // [rsp+90h] [rbp-78h]
  long double v16; // [rsp+98h] [rbp-70h]
  unsigned __int8 pbyDate[20]; // [rsp+A4h] [rbp-64h] BYREF
  char *m_sData; // [rsp+B8h] [rbp-50h]
  CGuild *v19; // [rsp+C0h] [rbp-48h]
  _guild_member_info *MemberFromSerial; // [rsp+C8h] [rbp-40h]
  CPlayer *pPlayer; // [rsp+D0h] [rbp-38h]
  char *pszFileName; // [rsp+E0h] [rbp-28h]
  unsigned int dwLeftGold; // [rsp+E8h] [rbp-20h]

  v9 = -1;
  v10 = -1;
  dwIOerSerial = -1;
  dwPopGold = 0;
  dwPush = 0;
  v15 = 0.0;
  v16 = 0.0;
  m_sData = pData->m_sData;
  v9 = *(_DWORD *)&pData->m_sData[24];
  v10 = *(_DWORD *)&pData->m_sData[28];
  dwIOerSerial = *(_DWORD *)pData->m_sData;
  dwPush = *(_DWORD *)&pData->m_sData[36];
  dwPopGold = *(_DWORD *)&pData->m_sData[32];
  v16 = *(double *)&pData->m_sData[56];
  v15 = *(double *)&pData->m_sData[48];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(pbyDate, m_sData + 40, 4uLL);
  v19 = &g_Guild[v9];
  if ( v19->m_dwSerial == v10 )
  {
    v19->m_bIOWait = 0;
	    if ( m_sData[64] )
	    {
	      const unsigned __int8 qryRet = static_cast<unsigned __int8>(m_sData[64]);
	      const unsigned int ioerSerialForLog = *reinterpret_cast<unsigned int *>(m_sData);
	      const unsigned int subGold = dwPopGold;
	      const unsigned int subDalant = dwPush;
	      g_Main.m_logSystemError.Write(
	        
	        "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
	        "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%u) SubG(%u) %s(%u)\r\n"
	        "\t\t_qry_case_outputgmoney Ret(%u) Fail!",
	        v10,
	        v16,
	        v15,
	        subDalant,
	        subGold,
	        Destination,
	        ioerSerialForLog,
	        qryRet);
	    }
    else if ( !pData->m_byResult )
    {
      
        v19->IOMoney(
        Destination,
        dwIOerSerial,
        -0.0 - (double)(int)dwPush,
        -0.0 - (double)(int)dwPopGold,
        v16,
        v15,
        pbyDate,
        0);
      MemberFromSerial = v19->GetMemberFromSerial( dwIOerSerial);
      if ( MemberFromSerial )
      {
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer )
        {
          pPlayer->AddDalant( dwPush, 1);
          pPlayer->AddGold( dwPopGold, 1);
          pszFileName = pPlayer->m_szItemHistoryFileName;
          dwLeftGold = pPlayer->m_Param.GetGold();
          Dalant = pPlayer->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.guild_pop_money(
            pPlayer->m_ObjID.m_wIndex,
            v19->m_aszName,
            dwPush,
            dwPopGold,
            Dalant,
            dwLeftGold,
            pszFileName);
          pPlayer->SendMsg_AlterMoneyInform( 0);
        }
        else
        {
          if (dwPush)
            g_Main.Push_ChargeItem(dwIOerSerial, 0xFFFFFFFF, dwPush, 0xFFFFFFFu, 1u);
          if (dwPopGold)
            g_Main.Push_ChargeItem(dwIOerSerial, 0xFFFFFFFF, dwPopGold, 0xFFFFFFFu, 2u);
          g_Main.m_logSystemError.Write(
            "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
            "\t\tPush Charge Money BECAUSE Poper Connection Closed : Poper Serial(%u) Pop Dalant(%u) Pop Gold(%u)\r\n",
            dwIOerSerial,
            dwPush,
            dwPopGold);
        }
      }
    }
  }
}

void CPlayer::Guild_Buy_Emblem_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-F8h] BYREF
  __int64 dTotalDalant; // [rsp+28h] [rbp-D0h]
  unsigned __int8 *pbyDate; // [rsp+38h] [rbp-C0h]
  bool bInPut[16]; // [rsp+40h] [rbp-B8h]
  int v7; // [rsp+50h] [rbp-A8h]
  unsigned int v8; // [rsp+54h] [rbp-A4h]
  unsigned int dwEmblemBack; // [rsp+58h] [rbp-A0h]
  unsigned int dwEmblemMark; // [rsp+5Ch] [rbp-9Ch]
  unsigned int dwIOerSerial; // [rsp+60h] [rbp-98h]
  char Destination[28]; // [rsp+78h] [rbp-80h] BYREF
  int v13; // [rsp+94h] [rbp-64h]
  int v14; // [rsp+98h] [rbp-60h]
  long double dTotalGold; // [rsp+A0h] [rbp-58h]
  long double v16; // [rsp+A8h] [rbp-50h]
  unsigned __int8 v17[20]; // [rsp+B4h] [rbp-44h] BYREF
  char *m_sData; // [rsp+C8h] [rbp-30h]
  CGuild *v19; // [rsp+D0h] [rbp-28h]

  v7 = -1;
  v8 = -1;
  dwEmblemBack = -1;
  dwEmblemMark = -1;
  dwIOerSerial = -1;
  v13 = 0;
  v14 = 0;
  dTotalGold = 0.0;
  v16 = 0.0;
  memset(v17, 0, 4);
  m_sData = pData->m_sData;
  v7 = *(_DWORD *)&pData->m_sData[24];
  v8 = *(_DWORD *)pData->m_sData;
  dwEmblemBack = *(_DWORD *)&pData->m_sData[4];
  dwEmblemMark = *(_DWORD *)&pData->m_sData[8];
  dwIOerSerial = *(_DWORD *)&pData->m_sData[16];
  v14 = *(_DWORD *)&pData->m_sData[12];
  v16 = *(double *)&pData->m_sData[56];
  dTotalGold = *(double *)&pData->m_sData[48];
  memcpy_0(v17, &pData->m_sData[20], 4uLL);
  strcpy_0(Destination, m_sData + 28);
  v19 = &g_Guild[v7];
  if ( v19->m_dwSerial == v8 )
  {
    v19->m_bIOWait = 0;
	    if ( m_sData[64] )
	    {
	      const unsigned __int8 qryRet = static_cast<unsigned __int8>(m_sData[64]);
	      const unsigned int ioerSerialForLog = *((_DWORD *)m_sData + 4);
	      const int subDalant = v14;
	      g_Main.m_logSystemError.Write(
	        
	        "CPlayer::Guild_Buy_Emblem_Complete(...) : \r\n"
	        "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%d) %s(%u)\r\n"
	        "\t\tqry_case_buyemblem Ret(%u) Fail!",
	        v8,
	        v16,
	        dTotalGold,
	        subDalant,
	        m_sData + 28,
	        ioerSerialForLog,
	        qryRet);
	    }
    else if ( !pData->m_byResult )
    {
      v19->m_byMoneyOutputKind = 1;
      v19->IOMoney( Destination, dwIOerSerial, (double)v14, 0.0, v16, dTotalGold, v17, 0);
      v19->UpdateEmblem( dwEmblemBack, dwEmblemMark);
    }
  }
}

void CPlayer::Guild_Disjoint_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-48h] BYREF
  int v4; // [rsp+20h] [rbp-28h]
  int v5; // [rsp+24h] [rbp-24h]
  char *m_sData; // [rsp+28h] [rbp-20h]
  CGuild *v7; // [rsp+30h] [rbp-18h]

  v4 = -1;
  v5 = -1;
  m_sData = pData->m_sData;
  v4 = *(_DWORD *)&pData->m_sData[4];
  v5 = *(_DWORD *)pData->m_sData;
  v7 = &g_Guild[v4];
  if ( v7 && v7->m_dwSerial == v5 && !pData->m_byResult )
  {
    if ( v7->m_nApplierNum > 0 )
      v7->SendMsg_GuildDisjointInform();
    v7->Release();
  }
}

void CPlayer::Guild_Update_GuildMater_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-68h] BYREF
  int v4; // [rsp+30h] [rbp-38h]
  int v5; // [rsp+34h] [rbp-34h]
  unsigned int in_guild_prev_masterSerial; // [rsp+38h] [rbp-30h]
  unsigned __int8 v7; // [rsp+3Ch] [rbp-2Ch]
  unsigned int in_guild_new_masterSerial; // [rsp+40h] [rbp-28h]
  unsigned __int8 in_guild_new_masterPrevGrade; // [rsp+44h] [rbp-24h]
  char *m_sData; // [rsp+48h] [rbp-20h]
  CGuild *v11; // [rsp+50h] [rbp-18h]

  v4 = -1;
  v5 = -1;
  m_sData = pData->m_sData;
  v4 = *(_DWORD *)&pData->m_sData[4];
  v5 = *(_DWORD *)pData->m_sData;
  in_guild_prev_masterSerial = *(_DWORD *)&pData->m_sData[8];
  v7 = pData->m_sData[12];
  in_guild_new_masterSerial = *(_DWORD *)&pData->m_sData[16];
  in_guild_new_masterPrevGrade = pData->m_sData[20];
  v11 = &g_Guild[v4];
  if ( v11 && v11->m_dwSerial == v5 && !pData->m_byResult )
    
      v11->DB_Update_GuildMaster_Complete(
      in_guild_prev_masterSerial,
      v7,
      in_guild_new_masterSerial,
      in_guild_new_masterPrevGrade);
}


void CPlayer::pc_GuildBattleBlock(bool bBlock)
{
  this->m_bBlockGuildBattleMsg = bBlock;
}


void CPlayer::pc_GuildEstablishRequest(char *pwszGuildName)
{
  unsigned __int8 resultCode = 0;
  int selectedGuildIndex = -1;
  unsigned __int8 memberCount = 0;
  int memberIndices[8] = {};
  unsigned int memberSerials[8] = {};
  unsigned int memberPvpPoints[8] = {};
  unsigned __int8 memberLevels[8] = {};
  unsigned __int8 memberGrades[8] = {};
  char memberNames[136] = {};

  std::memset(memberIndices, 0xFF, sizeof(memberIndices));
  std::memset(memberSerials, 0xFF, sizeof(memberSerials));

  CPartyPlayer *party = this->m_pPartyMgr;
  CPartyPlayer **partyMembers = party ? party->GetPtrPartyMember() : nullptr;
  CGuild *reservedGuild = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    resultCode = 106;
    goto SEND_RESULT;
  }

  for (char *cursor = pwszGuildName; *cursor; ++cursor)
  {
    if (*cursor == ' ' || *cursor == '\'')
    {
      resultCode = 15;
      goto SEND_RESULT;
    }
  }

  if (!CTSingleton<CNationSettingManager>::Instance()->IsNormalString(pwszGuildName))
  {
    resultCode = 15;
    goto SEND_RESULT;
  }

  if (!IsSQLValidString(pwszGuildName))
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::pc_GuildEstablishRequest() : %u(%s) ::IsSQLValidString(pwszGuildName(%s)) Invalid!",
      this->m_dwObjSerial,
      this->m_Param.GetCharNameA(),
      pwszGuildName);
    resultCode = 15;
    goto SEND_RESULT;
  }

  if (this->IsPunished(1u, true))
  {
    return;
  }

  if (!party || !party->IsPartyMode())
  {
    resultCode = 1;
    goto SEND_RESULT;
  }

  if (!party->IsPartyBoss())
  {
    resultCode = 2;
    goto SEND_RESULT;
  }

  if (g_Main.IsReleaseServiceMode() && this->m_byUserDgr)
  {
    resultCode = 13;
    goto SEND_RESULT;
  }

  if (this->m_Param.GetDalant() < g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant())
  {
    resultCode = 12;
    goto SEND_RESULT;
  }

  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    if (!g_Guild[guildIndex].IsFill() && !g_Guild[guildIndex].m_bDBWait)
    {
      reservedGuild = &g_Guild[guildIndex];
      break;
    }
  }

  if (!reservedGuild)
  {
    resultCode = 10;
    goto SEND_RESULT;
  }

  if (!partyMembers)
  {
    resultCode = 1;
    goto SEND_RESULT;
  }

  for (int partySlot = 0; partySlot < 8 && partyMembers[partySlot]; ++partySlot)
  {
    CPlayer *member = &g_Player[partyMembers[partySlot]->m_wZoneIndex];
    if (!member->m_bLive)
    {
      resultCode = 9;
      goto SEND_RESULT;
    }

    if (member->IsPunished(1u, true))
    {
      resultCode = 70;
      goto SEND_RESULT;
    }

    if (member->m_Param.m_pGuild || member->m_Param.m_bGuildLock)
    {
      resultCode = 7;
      goto SEND_RESULT;
    }

    if (member->m_Param.m_pApplyGuild)
    {
      resultCode = 8;
      goto SEND_RESULT;
    }

    if (!member->m_Param.m_pClassHistory[0])
    {
      resultCode = 11;
      goto SEND_RESULT;
    }

    if (member->m_Param.GetRaceCode() != this->m_Param.GetRaceCode())
    {
      resultCode = 70;
      goto SEND_RESULT;
    }

    memberIndices[partySlot] = member->m_id.wIndex;
    memberSerials[partySlot] = member->m_id.dwSerial;
    memberPvpPoints[partySlot] = static_cast<unsigned int>(static_cast<int>(member->m_Param.GetPvPPoint()));
    memberLevels[partySlot] = static_cast<unsigned __int8>(member->m_Param.GetLevel());
    memberGrades[partySlot] = member->m_Param.m_byPvPGrade;
    strcpy_0(&memberNames[17 * partySlot], member->m_Param.GetCharNameW());
    ++memberCount;
  }

  if (g_Main.IsReleaseServiceMode())
  {
    if (this->m_byUserDgr)
    {
      resultCode = 13;
      goto SEND_RESULT;
    }

    if (memberCount < 8u)
    {
      resultCode = 9;
      goto SEND_RESULT;
    }
  }
  else if (!this->m_byUserDgr && memberCount < 8u)
  {
    resultCode = 9;
    goto SEND_RESULT;
  }

  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    if ((g_Guild[guildIndex].IsFill() || g_Guild[guildIndex].m_bDBWait)
        && !strcmp_0(pwszGuildName, g_Guild[guildIndex].m_wszName))
    {
      resultCode = 4;
      goto SEND_RESULT;
    }
  }

  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    if (!g_Guild[guildIndex].IsFill() && !g_Guild[guildIndex].m_bDBWait)
    {
      g_Guild[guildIndex].m_nIndex = guildIndex;
      g_Guild[guildIndex].m_bDBWait = true;
      strcpy_0(g_Guild[guildIndex].m_wszName, pwszGuildName);
      selectedGuildIndex = guildIndex;
      break;
    }
  }

  if (selectedGuildIndex == -1)
  {
    resultCode = 10;
    goto SEND_RESULT;
  }

SEND_RESULT:
  if (resultCode)
  {
    this->SendMsg_GuildEstablishFail(static_cast<char>(resultCode));
    return;
  }

  _qry_case_insertguild_local query;
  query.in_guildindex = static_cast<unsigned int>(selectedGuildIndex);
  query.tmp_Esterindex = this->m_id.wIndex;
  query.tmp_Esterserial = this->m_id.dwSerial;
  query.in_guildRace = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
  query.in_membernum = memberCount;
  strcpy_0(query.in_w_guildName, pwszGuildName);
  std::memcpy(query.in_memberindex, memberIndices, sizeof(query.in_memberindex));
  std::memcpy(query.in_memberserial, memberSerials, sizeof(query.in_memberserial));
  std::memcpy(query.tmp_pvp, memberPvpPoints, sizeof(query.tmp_pvp));
  std::memcpy(query.tmp_lv, memberLevels, sizeof(query.tmp_lv));
  std::memcpy(query.tmp_grade, memberGrades, sizeof(query.tmp_grade));
  std::memcpy(query.tmp_w_membername, memberNames, sizeof(query.tmp_w_membername));

  if (!g_Main.PushDQSData(
        0xFFFFFFFFu,
        nullptr,
        0x0Fu,
        reinterpret_cast<char *>(&query),
        static_cast<int>(query.size())))
  {
    return;
  }

  for (int partySlot = 0; partySlot < memberCount; ++partySlot)
  {
    CPlayer *member = &g_Player[partyMembers[partySlot]->m_wZoneIndex];
    member->m_Param.m_bGuildLock = true;
  }

  const unsigned int consumeDalant = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
  this->SubDalant(consumeDalant);

  const int level = this->m_Param.GetLevel();
  if (IsEconomyFeeLevel(level))
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(this->m_Param.GetRaceCode()),
      level,
      consumeDalant);
  }
}

void CPlayer::pc_GuildDownLoadRequest()
{
  if (!this->m_Param.m_pGuild)
  {
    return;
  }

  this->m_bGuildDownload = true;
  if (this->m_Param.m_pGuildMemPtr)
  {
    this->m_Param.m_pGuild->SendMsg_DownPacket(1u, this->m_Param.m_pGuildMemPtr);
  }
}

void CPlayer::pc_GuildJoinApplyRequest(char *pwszGuildName)
{
  unsigned __int8 result = 0;
  CGuild *applyGuild = GetGuildPtrFromName(g_Guild, MAX_GUILD, pwszGuildName);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else
  {
    if (this->IsPunished(1u, true))
    {
      return;
    }

    if (!applyGuild)
    {
      result = static_cast<unsigned __int8>(-56);
    }
    else if (this->m_Param.m_bGuildLock)
    {
      result = static_cast<unsigned __int8>(-51);
    }
    else if (this->m_Param.m_pGuild)
    {
      result = 22;
    }
    else if (this->m_Param.m_pApplyGuild)
    {
      result = 23;
    }
    else if (applyGuild->m_nMemberNum >= 50)
    {
      result = 24;
    }
    else if (applyGuild->m_nApplierNum >= 32)
    {
      result = 25;
    }
    else if (this->m_Param.GetRaceCode() != static_cast<int>(applyGuild->m_byRace))
    {
      result = 70;
    }
    else if (g_Main.IsReleaseServiceMode() && this->m_byUserDgr)
    {
      result = static_cast<unsigned __int8>(-1);
    }
  }

  if (!result)
  {
    applyGuild->PushApplier(this);
    this->m_Param.m_pApplyGuild = applyGuild;
  }

  this->SendMsg_GuildJoinApplyResult(static_cast<char>(result), applyGuild);
}

void CPlayer::pc_GuildJoinApplyCancelRequest()
{
  unsigned __int8 result = 0;
  CGuild *applyGuild = this->m_Param.m_pApplyGuild;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else if (this->m_Param.m_bGuildLock)
  {
    result = static_cast<unsigned __int8>(-51);
  }
  else if (!applyGuild || !applyGuild->GetApplierFromSerial(this->m_dwObjSerial))
  {
    result = static_cast<unsigned __int8>(-52);
  }

  if (!result)
  {
    applyGuild->PopApplier(this->m_dwObjSerial, 2u);
    this->m_Param.m_pApplyGuild = nullptr;
  }

  this->SendMsg_GuildJoinApplyCancelResult(static_cast<char>(result));
}

void CPlayer::pc_GuildJoinAcceptRequest(unsigned int dwApplierSerial, bool bAccept)
{
  unsigned __int8 result = 0;
  CGuild *guild = this->m_Param.m_pGuild;
  _guild_applier_info *applierInfo = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else if (!guild)
  {
    result = static_cast<unsigned __int8>(-54);
  }
  else
  {
    applierInfo = guild->GetApplierFromSerial(dwApplierSerial);
    if (!applierInfo)
    {
      result = static_cast<unsigned __int8>(-52);
    }
    else if (!applierInfo->pPlayer)
    {
      result = 26;
    }
    else if (this->m_Param.m_pGuildMemPtr->byClassInGuild != 1
      && this->m_Param.m_pGuildMemPtr->byClassInGuild != 2)
    {
      result = static_cast<unsigned __int8>(-53);
    }
    else if (static_cast<int>(guild->GetMemberNum()) + guild->m_nTempMemberNum >= 50)
    {
      result = static_cast<unsigned __int8>(-50);
    }
    else if (applierInfo->pPlayer->m_Param.GetRaceCode() != static_cast<int>(guild->m_byRace))
    {
      result = 70;
    }
    else if (guild->m_bRankWait)
    {
      result = 71;
    }
  }

  if (!result)
  {
    if (bAccept)
    {
      g_Main.m_logGuild.Write(
        "[%s] Join accept: approver=%s applicant=%s",
        guild->m_aszName,
        this->m_Param.GetCharNameA(),
        applierInfo->pPlayer->m_Param.GetCharNameA());

      _qry_case_joinacguild_local query{};
      query.in_guildindex = guild->m_nIndex;
      query.in_guildserial = guild->m_dwSerial;
      query.in_applierindex = applierInfo->pPlayer->m_ObjID.m_wIndex;
      query.in_applierserial = applierInfo->pPlayer->m_dwObjSerial;
      query.in_accepterserial = this->m_dwObjSerial;
      query.in_Grade = 0;
      query.in_MemberNum = static_cast<int>(guild->GetMemberNum()) + 1;

      if (g_Main.PushDQSData(0xFFFFFFFFu, &this->m_id, 0x10u, reinterpret_cast<char *>(&query), static_cast<int>(query.size())))
      {
        applierInfo->pPlayer->m_Param.m_bGuildLock = true;
        ++guild->m_nTempMemberNum;
      }
      else
      {
        result = static_cast<unsigned __int8>(-1);
      }
    }
    else
    {
      applierInfo->pPlayer->SendMsg_GuildJoinApplyRejectInform();
      applierInfo->pPlayer->m_Param.m_pApplyGuild = nullptr;
      guild->PopApplier(dwApplierSerial, 1u);
    }
  }

  if (result)
  {
    this->SendMsg_GuildJoinAcceptFail(static_cast<char>(result), dwApplierSerial);
  }
}

void CPlayer::pc_GuildPushMoneyRequest(unsigned int dwPushDalant, unsigned int dwPushGold)
{
  unsigned __int8 result = 0;
  CGuild *guild = this->m_Param.m_pGuild;

  if (!guild || !guild->GetMemberFromSerial(this->m_dwObjSerial))
  {
    result = static_cast<unsigned __int8>(-54);
  }
  else if (guild->m_bIOWait)
  {
    result = 103;
  }
  else if (dwPushDalant > 0xB2D05E00u || dwPushGold > 0xB2D05E00u)
  {
    result = 101;
  }
  else if (this->m_Param.GetDalant() < dwPushDalant || this->m_Param.GetGold() < dwPushGold)
  {
    result = 102;
  }
  else
  {
    const double totalDalant = static_cast<double>(guild->GetTotalDalant()) + static_cast<double>(dwPushDalant);
    const double totalGold = static_cast<double>(guild->GetTotalGold()) + static_cast<double>(dwPushGold);
    if (totalDalant > 1000000000.0 || totalGold > 500000.0)
    {
      result = 104;
    }
  }

  if (!result)
  {
    _qry_case_inputgmoney_local query{};
    query.in_pusherserial = this->m_id.dwSerial;
    query.tmp_guildindex = guild->m_nIndex;
    query.in_guildserial = guild->m_dwSerial;
    query.dwAddGold = dwPushGold;
    query.dwAddDalant = dwPushDalant;
    query.in_date[0] = GetCurrentMonth();
    query.in_date[1] = GetCurrentDay();
    query.in_date[2] = GetCurrentHour();
    query.in_date[3] = GetCurrentMin();
    strcpy_0(query.in_w_pushername, this->m_Param.GetCharNameW());

    if (g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x13u, reinterpret_cast<char *>(&query), static_cast<int>(query.size())))
    {
      guild->m_bIOWait = true;
      this->SubDalant(dwPushDalant);
      this->SubGold(dwPushGold);
      this->SendMsg_AlterMoneyInform(0);
    }
    else
    {
      result = static_cast<unsigned __int8>(-1);
    }
  }

  this->SendMsg_GuildPushMoneyResult(static_cast<char>(result));
}

void CPlayer::pc_GuildRoomRentRequest(_guildroom_rent_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 0;
  const unsigned __int8 roomType = pProtocol ? pProtocol->byRoomType : 0;

  if (roomType >= 2u)
  {
    retCode = 7;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    retCode = 6;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  if (this->m_Param.m_byClassInGuild != 1 && this->m_Param.m_byClassInGuild != 2)
  {
    retCode = 2;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (guild->m_bIOWait)
  {
    retCode = 11;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (static_cast<long double>(CGuildRoomInfo::sm_RoomInfo[roomType].dwCost) > guild->GetTotalDalant())
  {
    retCode = 8;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (guild->GetGrade() < CGuildRoomInfo::sm_RoomInfo[roomType].byRestrict)
  {
    retCode = 9;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (roomSystem->IsRoomRented(pProtocol->dwGuildSerial))
  {
    retCode = 3;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_pUserDB->m_AvatorData.dbAvator.m_byRaceSexCode >> 1);
  subRetCode = static_cast<unsigned __int8>(
    roomSystem->RentRoom(raceCode, roomType, guild->m_nIndex, guild->m_dwSerial, nullptr, false));
  if (subRetCode)
  {
    retCode = 5;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  retCode = 10;

  _qry_case_outputgmoney query{};
  query.in_poperserial = 0;
  query.tmp_guildindex = guild->m_nIndex;
  query.in_guildserial = guild->m_dwSerial;
  query.dwSubDalant = CGuildRoomInfo::sm_RoomInfo[roomType].dwCost;
  query.dwSubGold = 0;
  std::memset(&query.out_totalgold, 0, 17);
  query.in_date[0] = static_cast<unsigned __int8>(GetCurrentMonth());
  query.in_date[1] = static_cast<unsigned __int8>(GetCurrentDay());
  query.in_date[2] = static_cast<unsigned __int8>(GetCurrentHour());
  query.in_date[3] = static_cast<unsigned __int8>(GetCurrentMin());
  strcpy_0(query.in_w_popername, "GuildRoom Rent");
  g_Main.PushDQSData(
    0xFFFFFFFFu,
    nullptr,
    0x14u,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));

  CMapData *roomMap = roomSystem->GetMapData(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()), roomType);
  if (roomMap)
  {
    CMapItemStoreList *destStoreList = CItemStoreManager::Instance()->GetMapItemStoreListBySerial(roomMap->m_nMapIndex);
    CMapItemStoreList *emptyInstanceStore = CItemStoreManager::Instance()->GetEmptyInstanceItemStore();
    if (destStoreList && emptyInstanceStore && emptyInstanceStore->CopyItemStoreData(destStoreList))
    {
      emptyInstanceStore->SetTypeNSerial(1u, static_cast<int>(guild->m_dwSerial));
    }
  }

  this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
  guild->SendMsg_GuildRoomRented(static_cast<char>(roomType));
}

void CPlayer::pc_GuildRoomEnterRequest(_guildroom_enter_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 3;
  unsigned __int8 mapIndex = 0;
  unsigned __int16 mapLayer = 0;
  float startPos[3] = {0.0f, 0.0f, 0.0f};
  int restTime[5]{};

  if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    retCode = 5;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (this->IsRidingUnit())
  {
    retCode = 6;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (this->m_byStandType == 1)
  {
    retCode = 7;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    retCode = 1;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (!roomSystem->IsRoomRented(guild->m_dwSerial))
  {
    retCode = 2;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  subRetCode = static_cast<unsigned __int8>(
    roomSystem->RoomIn(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial));
  if (subRetCode)
  {
    retCode = 3;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CMapData *mapDummy = nullptr;
  unsigned __int8 roomType = 2;
  if (!roomSystem->GetMapPos(guild->m_dwSerial, startPos, mapDummy, &mapLayer, &roomType))
  {
    roomSystem->RoomOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial);
    retCode = 4;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CMapData *roomMap = roomSystem->GetMapData(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()), roomType);
  if (!roomMap)
  {
    roomSystem->RoomOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial);
    retCode = 4;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  retCode = 10;
  this->OutOfMap(roomMap, mapLayer, 3u, startPos);
  mapIndex = static_cast<unsigned __int8>(roomMap->m_pMapSet->m_dwIndex);
  roomSystem->GetRestTime(guild->m_dwSerial, restTime);
  this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
}

void CPlayer::pc_GuildRoomOutRequest(_guildroom_out_request_clzo *pProtocol)
{
  if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    this->SendMsg_GuildRoomOutResult(4u, 0, 0, nullptr);
    return;
  }

  if (this->IsRidingUnit())
  {
    this->SendMsg_GuildRoomOutResult(5u, 0, 0, nullptr);
    return;
  }

  if (this->m_byStandType == 1)
  {
    this->SendMsg_GuildRoomOutResult(6u, 0, 0, nullptr);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    this->SendMsg_GuildRoomOutResult(1u, 0, 0, nullptr);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (!roomSystem->IsRoomRented(guild->m_dwSerial)
    || !roomSystem->SetPlayerOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial))
  {
    this->SendMsg_GuildRoomOutResult(2u, 0, 0, nullptr);
  }
}

void CPlayer::pc_GuildRoomRestTimeRequest(_guildroom_resttime_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 3;

  if (this->m_Param.m_pGuild && pProtocol && pProtocol->dwGuildSerial == this->m_Param.m_pGuild->m_dwSerial)
  {
    CGuild *guild = this->m_Param.m_pGuild;
    CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
    if (!roomSystem->IsRoomRented(guild->m_dwSerial))
    {
      retCode = 2;
    }
    this->SendMsg_GuildRoomRestTimeResult();
  }
  else
  {
    retCode = 1;
    this->SendMsg_GuildRoomRestTimeResult();
  }

  (void)retCode;
  (void)subRetCode;
}

void CPlayer::pc_GuildSetHonorRequest(_guild_honor_set_request_clzo *pData)
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial =
    CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);

  if (charSerial == raceBossSerial)
  {
    const unsigned __int8 result =
      CHonorGuild::Instance()->SetNextHonorGuild(static_cast<unsigned __int8>(raceCode), pData);
    this->SendMsg_GuildSetHonorResult(static_cast<char>(result));
  }
  else
  {
    this->SendMsg_GuildSetHonorResult(1u);
  }
}


void CPlayer::pc_GuildHonorListRequest(unsigned __int8 byUI)
{
  const int raceCode = this->m_Param.GetRaceCode();
  CHonorGuild::Instance()->SendCurrHonorGuildList(this->m_ObjID.m_wIndex, static_cast<unsigned __int8>(raceCode), byUI);
}





void CPlayer::pc_GuildQueryInfoRequest(unsigned int dwGuildSerial)
{
  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    CGuild &guild = g_Guild[guildIndex];
    if (!guild.IsFill() || guild.m_dwSerial != dwGuildSerial)
    {
      continue;
    }

    guild.SendMsg_QueryPacket_Info(this->m_ObjID.m_wIndex);
    return;
  }
}

void CPlayer::pc_GuildListRequest(unsigned __int8 byPage)
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
    static_cast<unsigned __int8>(raceCode),
    0);

  if (charSerial != raceBossSerial)
  {
    return;
  }

  CGuild::s_GuildList.SendList(
    this->m_ObjID.m_wIndex,
    static_cast<unsigned __int8>(this->m_Param.GetRaceCode()),
    byPage);
}

void CPlayer::pc_GuildNextHonorListRequest()
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
    static_cast<unsigned __int8>(raceCode),
    0);

  if (charSerial != raceBossSerial || raceCode < 0 || raceCode >= 3)
  {
    return;
  }

  CHonorGuild::Instance()->SendNextHonorGuildList(
    this->m_ObjID.m_wIndex,
    static_cast<unsigned __int8>(this->m_Param.GetRaceCode()));
}


void CPlayer::pc_GuildCancelSuggestRequest(unsigned int dwMatterVoteSynKey)
{
  (void)dwMatterVoteSynKey;

  unsigned __int8 resultCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    resultCode = static_cast<unsigned __int8>(-54);
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  else if (guild->m_dwSuggesterSerial != m_dwObjSerial)
  {
    resultCode = static_cast<unsigned __int8>(-53);
  }

  if (!resultCode)
  {
    guild->SendMsg_VoteCancelInform();
    guild->InitVote();
  }

  this->SendMsg_CancelSuggestResult(static_cast<char>(resultCode));
}

void CPlayer::pc_GuildVoteRequest(unsigned int dwMatterVoteSynKey, unsigned __int8 byVoteCode)
{
  unsigned __int8 resultCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    resultCode = static_cast<unsigned __int8>(-54);
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  else if (!guild->m_bNowProcessSgtMter)
  {
    resultCode = 62;
  }
  else if (guild->m_SuggestedMatter.dwMatterVoteSynKey != dwMatterVoteSynKey)
  {
    resultCode = 63;
  }
  else if (!guild->m_SuggestedMatter.IsVotable(m_dwObjSerial))
  {
    resultCode = static_cast<unsigned __int8>(-53);
  }
  else if (m_Param.m_pGuildMemPtr->bVote)
  {
    resultCode = 66;
  }

  if (!resultCode)
  {
    m_Param.m_pGuildMemPtr->bVote = true;
    if (byVoteCode < 2)
    {
      ++guild->m_SuggestedMatter.byVoteState[byVoteCode];
    }
  }

  this->SendMsg_VoteResult(dwMatterVoteSynKey, resultCode);
}


void CPlayer::pc_GuildOfferSuggestRequest(
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *pwszComment,
  unsigned int dwMatterObj1,
  unsigned int dwMatterObj2,
  unsigned int dwMatterObj3)
{
  unsigned __int8 byRetCode = 0;
  CGuild *guild = m_Param.m_pGuild;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    byRetCode = 106;
  }
  else if (!guild)
  {
    byRetCode = static_cast<unsigned __int8>(-54);
  }
  else if (guild->m_bNowProcessSgtMter)
  {
    byRetCode = 61;
  }
  else if (guild->m_bRankWait)
  {
    byRetCode = 71;
  }
  else if (!guild->GetMemberFromSerial(dwMatterDst))
  {
    byRetCode = 63;
  }
  else if (guild->m_MasterData.IsFill() && guild->m_MasterData.dwSerial == dwMatterDst)
  {
    byRetCode = 67;
  }
  else if (!guild->m_bPossibleElectMaster)
  {
    byRetCode = static_cast<unsigned __int8>(-78);
  }

  if (!byRetCode
      && !guild->RegSuggestedMatter(
        m_dwObjSerial,
        byMatterType,
        dwMatterDst,
        pwszComment,
        dwMatterObj1,
        dwMatterObj2,
        dwMatterObj3))
  {
    byRetCode = 64;
  }

  this->SendMsg_OfferSuggestResult(static_cast<char>(byRetCode));
}

