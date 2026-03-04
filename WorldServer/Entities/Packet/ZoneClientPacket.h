#pragma once

#include "../CMainThread.h"
#include "../CPlayer.h"
#include "../DB_LOAD_AUTOMINE_MACHINE.h"
#include "../DqsDbStructs.h"
#include "../IdaCompat.h"
#include "../InvenKey.h"
#include "../StoragePos.h"
#include "../attack_gen_result_zocl.h"

// Zone -> Client packet structs.

// a_trade_adjust_price_result_zocl.h
#pragma pack(push, 1)
struct _a_trade_adjust_price_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byTaxRate;
  unsigned int dwLeftDalant;
  unsigned int dwTax;
  unsigned __int16 wItemSerial;
  unsigned int dwNewPrice;
  unsigned int dwRegistSerial;
  __int64 size() const;
};
#pragma pack(pop)

// a_trade_clear_item_result_zocl.h
#pragma pack(push, 1)
struct _a_trade_clear_item_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wItemSerial;
  unsigned int dwRegistSerial;
  __int64 size() const;
};
#pragma pack(pop)

// add_lend_item_result_zocl.h
#pragma pack(push, 1)
struct _add_lend_item_result_zocl
{
  unsigned __int8 byTblCode;
  unsigned __int16 wItemIdx;
  unsigned int dwDur;
  unsigned int dwUp;
  unsigned int dwItemSerial;
  unsigned __int8 byCsMethod;
  unsigned int dwT;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)

// alive_char_result_zocl.h
#pragma pack(push, 1)
struct _alive_char_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned int dwSerial;
  _REGED_AVATOR_DB AliveChar;
  _alive_char_result_zocl();
};
#pragma pack(pop)

// alter_action_point_zocl.h
#pragma pack(push, 1)
struct _alter_action_point_zocl
{
  unsigned __int8 byActionCode;
  unsigned int dwActionPoint;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_alter_action_point_zocl));
  }
};
#pragma pack(pop)

// alter_cont_effect_time_zocl.h
#pragma pack(push, 1)
struct _alter_cont_effect_time_zocl
{
  struct __list
  {
    unsigned __int16 wEffectCode;
    __int16 zLeftSec;
  };
  unsigned __int8 byEffectNum;
  __list List[8];
  _alter_cont_effect_time_zocl();
  int size();
};
#pragma pack(pop)

// animus_download_result_zocl.h
#pragma pack(push, 1)
struct _animus_download_result_zocl
{
  struct _list
  {
    char sItemIndex;
    unsigned __int64 dwExp;
    unsigned int dwParam;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned __int8 byAnimusNum;
  _list AnimusList[4];
  _animus_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// atrade_taxrate_result_zocl.h
#pragma pack(push, 1)
struct _atrade_taxrate_result_zocl
{
  unsigned __int8 byTaxRate;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  char wszGuildName[17];
  __int64 size();
};
#pragma pack(pop)

// attack_gen_result_zocl.h
#pragma pack(push, 1)
struct _attack_gen_result_zocl
{
#pragma pack(push, 1)
  struct _dam_list
  {
    unsigned __int8 byDstID;
    unsigned int dwDstSerial;
    unsigned __int16 wDamage;
    bool bActive;
    unsigned __int16 wActiveDamage;
  };
#pragma pack(pop)

  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byAttackPart;
  unsigned __int16 wBulletIndex;
  bool bCritical;
  bool bWPActive;
  unsigned __int8 byListNum;
  _dam_list DamList[32];

  _attack_gen_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_gen_result_zocl::_dam_list) == 10, "_dam_list packing mismatch");
static_assert(sizeof(_attack_gen_result_zocl) == 331, "_attack_gen_result_zocl packing mismatch");

// base_download_result_zocl.h
#pragma pack(push, 1)
struct _base_download_result_zocl
{
  struct _list_equip
  {
    char sTableCode;
    unsigned __int16 wItemIndex;
    unsigned int dwUptInfo;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  struct _list_embellish
  {
    char sTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int16 wNum;
    char sClientIndex;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned __int8 byRetCode;
  unsigned __int16 wZoneIndex;
  unsigned int dwExpRate;
  unsigned __int16 wClassHistory[3];
  unsigned __int16 wHP;
  unsigned __int16 wSP;
  unsigned __int16 wFP;
  long double dPvpPoint;
  long double dPvpCashBag;
  long double dPvpTempCash;
  unsigned int dwPvpRank;
  unsigned __int8 byPvpClass;
  unsigned __int8 byPlusKey;
  unsigned __int16 wXorKey;
  unsigned __int16 wMaxHP;
  unsigned __int16 wMaxSP;
  unsigned __int16 wMaxFP;
  unsigned __int16 wMapIndex;
  float fPos[3];
  _list_equip EquipList[8];
  _list_embellish EmbellishList[7];
  unsigned __int8 byHolyMasterState;
  unsigned int dwGuildSerial;
  unsigned __int8 byGuildGrade;
  unsigned __int8 byEffectValue[2];
  unsigned __int8 byUseTrunkSlotNum;
  unsigned __int16 wMaxDP;
  unsigned __int16 wDP;
  unsigned __int8 byHonorGuildRank;
  unsigned __int64 size();
};
#pragma pack(pop)

// buy_store_failure_zocl.h
#pragma pack(push, 1)
struct _buy_store_failure_zocl
{
  unsigned int dwDalant;
  unsigned int dwGold;
  long double dPoint;
  unsigned int dwActPoint[3];
  unsigned __int8 byRetCode;
};
#pragma pack(pop)

// buy_store_success_zocl.h
#pragma pack(push, 1)
struct _buy_store_success_zocl
{
  struct _list
  {
    unsigned __int16 wSerial;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned int dwLeftDalant;
  unsigned int dwLeftGold;
  unsigned int dwConsumDanlant;
  unsigned int dwConsumGold;
  unsigned int dwLeftPoint;
  unsigned int dwConsumPoint;
  unsigned int dwLeftActPoint[3];
  unsigned int dwConsumActPoint[3];
  unsigned __int8 byDiscountRate;
  unsigned __int8 byBuyNum;
  _list OfferList[100];
  _buy_store_success_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// chat_far_failure_zocl.h
#pragma pack(push, 1)
struct _chat_far_failure_zocl
{
  bool bBlock;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// chat_guild_failure_zocl.h
#pragma pack(push, 1)
struct _chat_guild_failure_zocl
{
  unsigned __int8 byErrCode;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// chat_lock_inform_zocl.h
#pragma pack(push, 1)
struct _chat_lock_inform_zocl
{
  bool bLock;
  __int64 size() const;
};
#pragma pack(pop)

// chat_multi_far_failure_zocl.h
#pragma pack(push, 1)
struct _chat_multi_far_failure_zocl
{
  char wszDstName[17];
  unsigned __int8 byFailCode;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// chat_multi_far_trans_zocl.h
#pragma pack(push, 1)
struct _chat_multi_far_trans_zocl
{
  unsigned __int16 wSize;
  char sData[500];
  _chat_multi_far_trans_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// chat_steal_message_gm_zocl.h
#pragma pack(push, 1)
struct _chat_steal_message_gm_zocl
{
  unsigned __int8 byMessageType;
  unsigned int dwSenderSerial;
  char wszSenderName[17];
  unsigned __int8 byRaceCode;
  bool bFiltering;
  unsigned __int8 byPvpGrade;
  unsigned __int8 bySize;
  char wszChatData[256];
  _chat_steal_message_gm_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// combine_ex_item_accept_result_zocl.h
#pragma pack(push, 1)
struct _combine_ex_item_accept_result_zocl
{
  unsigned __int8 byErrCode;
};
#pragma pack(pop)

// combine_ex_item_result_zocl.h
#pragma pack(push, 1)
struct _combine_ex_item_result_zocl
{
  struct __item
  {
    _COMBINEKEY Key;
    unsigned int dwDur;
    unsigned int dwUpt;
    __item();
    void Init();
  };
  struct _Result_ItemList_Buff
  {
    unsigned __int8 byItemListNum;
    __item RewardItemList[24];
    _Result_ItemList_Buff();
    void Init();
  };
  unsigned __int8 byErrCode;
  unsigned __int8 byDlgType;
  unsigned int dwDalant;
  unsigned int dwCheckKey;
  unsigned __int8 bySelectItemCount;
  _Result_ItemList_Buff ItemBuff;
  unsigned int dwResultEffectType;
  unsigned int dwResultEffectMsgCode;
  _combine_ex_item_result_zocl();
};
#pragma pack(pop)

// create_holy_master_zocl.h
#pragma pack(push, 1)
struct _create_holy_master_zocl
{
  unsigned __int8 byHolyStoneRaceCode;
  unsigned __int8 byPlayerRaceCode;
  unsigned __int8 byNameLen;
  char wszName[17];
  int nControlSec;
  unsigned int dwObjSerial;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_create_holy_master_zocl));
  }
};
#pragma pack(pop)

// cum_download_result_zocl.h
#pragma pack(push, 1)
struct _cum_download_result_zocl
{
  unsigned __int8 byRetCode;
  _STAT_DB_BASE Stat;
  unsigned __int8 byLeftCuttingResNum;
  unsigned __int16 wleftResList[20];
  _cum_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// darkhole_answer_reenter_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_answer_reenter_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_ask_reenter_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_ask_reenter_inform_zocl
{
  unsigned __int16 wChannelIndex;
  unsigned int dwChannelSerial;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_channel_close_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_channel_close_inform_zocl
{
  char sDum;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_clear_out_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_clear_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wTarMapIndex;
  __int16 zTarPos[3];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_create_zocl.h
#pragma pack(push, 1)
struct _darkhole_create_zocl
{
  unsigned __int16 wQuestIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  short zPos[3];
  unsigned __int8 bHurry;
  char wszOpenerName[17];
  unsigned int dwOpenerSerial;
  _darkhole_create_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_destroy_zocl.h
#pragma pack(push, 1)
struct _darkhole_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_enter_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_enter_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned int dwHoleSerial;
  unsigned __int8 byMapCode;
  __int16 zPos[3];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_fixpositon_zocl.h
#pragma pack(push, 1)
struct _darkhole_fixpositon_zocl
{
  unsigned __int16 wQuestIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  short zPos[3];
  unsigned __int8 bHurry;
  char wszOpenerName[17];
  unsigned int dwOpenerSerial;
  _darkhole_fixpositon_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_giveup_out_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_giveup_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wTarMapIndex;
  __int16 zTarPos[3];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_job_count_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_job_count_inform_zocl
{
  unsigned __int8 byJobIndex;
  unsigned __int16 wJobCount;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_job_pass_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_job_pass_inform_zocl
{
  unsigned __int8 byJobIndex;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_leader_change_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_leader_change_inform_zocl
{
  unsigned int dwNewLeaderSerial;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_member_info_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_member_info_inform_zocl
{
  struct __list
  {
    char wszName[17];
    unsigned int dwSerial;
  };
  unsigned int dwLeaderSerial;
  unsigned __int16 wMemberNum;
  __list List[32];
  _darkhole_member_info_inform_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// darkhole_mission_info_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_mission_info_inform_zocl
{
  struct __job
  {
    unsigned __int8 byType;
    unsigned __int8 byTableCode;
    unsigned __int16 wRecordIndex;
    __int16 zNeedCount;
    char szDescirptCode[17];
    bool bPass;
    unsigned __int16 wCurCount;
    bool bDisable;
  };
  char szDescirptCode[17];
  unsigned int dwLimTimeSec;
  unsigned int dwPassTimeSec;
  unsigned __int8 byOrder;
  unsigned __int8 byJobNum;
  __job Job[128];
  _darkhole_mission_info_inform_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// darkhole_mission_pass_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_mission_pass_inform_zocl
{
  char szCompleteMsgCode[17];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_mission_quest_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_mission_quest_inform_zocl
{
  char sDum;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_new_member_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_new_member_inform_zocl
{
  unsigned int dwNewMemberSerial;
  char wszNewMemberName[17];
  bool bReconnect;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_new_mission_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_new_mission_inform_zocl
{
  struct __job
  {
    unsigned __int8 byType;
    unsigned __int8 byTableCode;
    unsigned __int16 wRecordIndex;
    __int16 zNeedCount;
    char szDescirptCode[17];
    bool bDisable;
  };
  char szDescirptCode[17];
  unsigned int dwLimTimeSec;
  unsigned __int8 byOrder;
  unsigned __int8 byJobNum;
  __job Job[128];
  _darkhole_new_mission_inform_zocl() : szDescirptCode{}, dwLimTimeSec(0), byOrder(0), byJobNum(0), Job{}
  {
  }
  unsigned __int16 size();
};
#pragma pack(pop)

// darkhole_open_all_portal_by_result_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_open_all_portal_by_result_inform_zocl
{
  unsigned __int8 byCnt;
  unsigned __int8 byPotalIndex[128];
  _darkhole_open_all_portal_by_result_inform_zocl() : byCnt(0), byPotalIndex{}
  {
  }
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_open_portal_by_react_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_open_portal_by_react_inform_zocl
{
  unsigned __int16 wPortalIndex;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_open_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_open_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wHoleIndex;
  unsigned int dwHoleSerial;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_outof_member_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_outof_member_inform_zocl
{
  unsigned int dwMemberSerial;
  bool bDisconnect;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_quest_info_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_quest_info_inform_zocl
{
  char szDescirptCode[17];
  unsigned int dwLimTimeSec;
  unsigned int dwPassTimeSec;
  long double dRwExp;
  unsigned int dwRwPvp;
  unsigned int dwRwDalant;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_real_add_time_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_real_add_time_inform_zocl
{
  int nAddSec;
  unsigned int dwTotalLimSec;
  char szMsg[17];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_real_msg_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_real_msg_inform_zocl
{
  char szMsg[17];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_state_change_zocl.h
#pragma pack(push, 1)
struct _darkhole_state_change_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int8 bHurry;
  _darkhole_state_change_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// darkhole_timeout_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_timeout_inform_zocl
{
  char sDum;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// equip_up_item_lv_limit_zocl.h
#pragma pack(push, 1)
struct _equip_up_item_lv_limit_zocl
{
  unsigned __int8 byEquipItemNum[8];
  _equip_up_item_lv_limit_zocl();
};
#pragma pack(pop)

// exchange_lend_item_result_zocl.h
#pragma pack(push, 1)
struct _combine_lend_item_result_zocl
{
  struct _lend_item
  {
    unsigned __int8 byTblCode;
    unsigned __int16 wItemIdx;
    unsigned int dwDur;
    unsigned int dwUp;
    unsigned int dwItemSerial;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned __int8 byErrorCode;
  unsigned int dwFee;
  unsigned int dwLeftDalant;
  _lend_item Item;
  __int64 size() const
  {
    return 29LL;
  }
};
struct _exchange_lend_item_result_zocl
{
  unsigned __int8 byErrorCode;
  _combine_lend_item_result_zocl::_lend_item Item;
  __int64 size() const;
};
#pragma pack(pop)

// force_download_result_zocl.h
#pragma pack(push, 1)
struct _force_download_result_zocl
{
  struct _list
  {
    unsigned __int16 wItemIndex;
    unsigned int dwCum;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotNum;
  _list ItemSlotInfo[88];
  _force_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// gm_msg_gmcall_list_response_zocl.h
#pragma pack(push, 1)
struct _gm_msg_gmcall_list_response_zocl
{
  struct _call_node
  {
    unsigned int dwSerial;
    char m_wszUserName[17];
  };
  int m_nCurrPageUserNum;
  int m_nCurrPageIndex;
  int m_MaxPage;
  int m_nMaxUser;
  _call_node m_arCallNode[10];
  static void Init(_gm_msg_gmcall_list_response_zocl *msg);
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_gm_msg_gmcall_list_response_zocl));
  }
};
#pragma pack(pop)
static_assert(sizeof(_gm_msg_gmcall_list_response_zocl) == 0xE2, "gm call list packet size mismatch");

// guild_alter_member_grade_inform_zocl.h
#pragma pack(push, 1)
struct _guild_alter_member_grade_inform_zocl
{
  struct __list
  {
    unsigned int dwMemberSerial;
    unsigned __int8 byRank;
    unsigned __int8 byGrade;
  };
  unsigned __int8 byAlterMemberNum;
  __list MemberList[50];
  _guild_alter_member_grade_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// guild_alter_member_state_inform_zocl.h
#pragma pack(push, 1)
struct _guild_alter_member_state_inform_zocl
{
  struct __list
  {
    unsigned int dwMemberSerial;
    unsigned __int8 byLv;
    unsigned int dwPvpPoint;
  };
  unsigned __int8 byAlterMemberNum;
  __list MemberList[50];
  _guild_alter_member_state_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// guild_battle_get_gravity_stone_result_zocl.h
#pragma pack(push, 1)
struct _guild_battle_get_gravity_stone_result_zocl
{
  unsigned __int8 byRet;
  char wszGuildName[17];
  char wszCharName[17];
  int iTakePortalInx;
  unsigned __int8 byColor;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_guild_battle_get_gravity_stone_result_zocl));
  }
};
#pragma pack(pop)

// guild_battle_goal_result_zocl.h
#pragma pack(push, 1)
struct _guild_battle_goal_result_zocl
{
  unsigned __int8 byRet;
  char wszGuildName[17];
  unsigned int dwObjSerial;
  char wszCharName[17];
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwLeftRedScore;
  unsigned int dwRightBlueGoalCnt;
  unsigned int dwRightBlueScore;
  unsigned __int8 byLeftHour;
  unsigned __int8 byLeftMin;
  unsigned __int8 byLeftSec;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_guild_battle_goal_result_zocl));
  }
};
#pragma pack(pop)
static_assert(sizeof(_guild_battle_goal_result_zocl) == 58, "guild battle goal packet size mismatch");

// guild_battle_suggest_request_result_zocl.h
#pragma pack(push, 1)
struct _guild_battle_suggest_request_result_zocl
{
  unsigned __int8 byRet;
  char wszDestGuildName[17];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// guild_vote_process_inform_zocl.h
#pragma pack(push, 1)
struct _guild_vote_process_inform_zocl
{
  bool bStart;
  unsigned __int8 byMatterType;
  unsigned int dwMatterDst;
  unsigned int dwMatterObj1;
  unsigned int dwMatterObj2;
  unsigned int dwMatterObj3;
  unsigned int dwMatterVoteSynKey;
  unsigned int dwSuggesterSerial;
  unsigned __int8 byLoginSeniorNum;
  unsigned __int8 byTotalSeniorNum;
  bool bActed;
  unsigned __int8 byApprPoint;
  unsigned __int8 byOppoPoint;
  char wszDestGuildName[17];
  unsigned __int8 byDestGuildGrade;
  unsigned __int8 byDestGuildRace;
  unsigned __int8 byCommentLen;
  char wszComment[65];
  _guild_vote_process_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// guildroom_out_result_zocl.h
#pragma pack(push, 1)
struct _guildroom_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byMapIndex;
  unsigned __int16 wMapLayerIndex;
  __int16 sNewPos[3];
  __int64 size();
};
#pragma pack(pop)

// holy_keeper_attackable_inform_zocl.h
#pragma pack(push, 1)
struct _holy_keeper_attackable_inform_zocl
{
  unsigned int dwKeeperSerial;
  unsigned __int8 byMasterRace;
  bool bAttackAbleState;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_holy_keeper_attackable_inform_zocl));
  }
};
#pragma pack(pop)

// Init_action_point_zocl.h
#pragma pack(push, 1)
struct _Init_action_point_zocl
{
  unsigned int dwActionPoint[3];
  _Init_action_point_zocl();
};
#pragma pack(pop)

// insert_new_quest_inform_zocl.h
#pragma pack(push, 1)
struct _insert_new_quest_inform_zocl
{
  unsigned __int8 byQuestDBSlot;
  _QUEST_DB_BASE::_LIST NewQuestData;
};
#pragma pack(pop)

// insert_next_quest_inform_zocl.h
#pragma pack(push, 1)
struct _insert_next_quest_inform_zocl
{
  unsigned __int8 byQuestDBSlot;
  _QUEST_DB_BASE::_LIST NewQuestData;
  _insert_next_quest_inform_zocl();
};
#pragma pack(pop)

// inven_download_result_zocl.h
#pragma pack(push, 1)
struct _inven_download_result_zocl
{
  struct _list
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDurPoint;
    char sClientIndex;
    unsigned int dwUptInfo;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned __int8 byRetCode;
  unsigned __int8 byBagNum;
  unsigned __int8 bySlotNum;
  _list ItemSlotInfo[100];
  _inven_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// item_fanfare_zocl.h
#pragma pack(push, 1)
struct _item_fanfare_zocl
{
  struct _SubType
  {
    unsigned __int8 bitParmaType : 2;
    unsigned __int8 bitGrade : 6;
  };
  unsigned __int8 byTakeType;
  _SubType bySubType;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;
  unsigned __int16 wMonsterIndex;
  unsigned __int16 wMapIndex;
  unsigned int dwCharacterSerial;
  char strCharacterName[17];
  bool bAllSend;
  _item_fanfare_zocl();
  void Init();
};
#pragma pack(pop)

// itembox_take_add_result_zocl.h
#pragma pack(push, 1)
struct _itembox_take_add_result_zocl
{
  char sErrorCode;
  unsigned __int16 wItemSerial;
  unsigned __int8 byAmount;
  __int64 size() const;
};
#pragma pack(pop)

// itembox_take_new_result_zocl.h
#pragma pack(push, 1)
struct _itembox_take_new_result_zocl
{
  char sErrorCode;
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemIndex;
  unsigned __int64 dwCurDurPoint;
  unsigned int dwUptInfo;
  unsigned __int16 wItemSerial;
  unsigned __int8 byCsMethod;
  unsigned int dwT;
  __int64 size() const;
};
#pragma pack(pop)

// limit_item_num_info_zocl.h
#pragma pack(push, 1)
struct _limit_item_num_info_zocl
{
  struct _limit_item_info
  {
    unsigned int dwLimitItemIndex;
    unsigned __int16 wLimitNum;
  };
  unsigned int dwStoreIndex;
  unsigned __int8 byLimitItemNum;
  _limit_item_info LimitItemInfo[16];
  _limit_item_num_info_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// limitedsale_event_inform_zocl.h
#pragma pack(push, 1)
struct _limitedsale_event_inform_zocl
{
  unsigned __int8 byTableCode;
  unsigned int dwIndex;
  unsigned __int16 wNum;
  __int64 size();
};
#pragma pack(pop)

// move_to_own_stonemap_inform_zocl.h
#pragma pack(push, 1)
struct _move_to_own_stonemap_inform_zocl
{
  unsigned __int8 byStoneMapMoveInfo;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// move_to_own_stonemap_result_zocl.h
#pragma pack(push, 1)
struct _move_to_own_stonemap_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byMapIndex;
  __int16 sNewPos[3];
  unsigned __int16 size() const;
};
#pragma pack(pop)

// not_arranged_char_inform_zocl.h
#pragma pack(push, 1)
struct _not_arranged_char_inform_zocl
{
  unsigned __int8 byCharNum;
  _NOT_ARRANGED_AVATOR_DB CharList[50];
  _not_arranged_char_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// notice_move_limit_map_msg_zocl.h
#pragma pack(push, 1)
struct _notice_move_limit_map_msg_zocl
{
  unsigned __int8 byType;
};
#pragma pack(pop)

// notify_cont_play_time_zocl.h
#pragma pack(push, 1)
struct _notify_cont_play_time_zocl
{
  unsigned __int8 byContTime;
  unsigned __int8 byContMin;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)

// notify_coupon_ensure_time_zocl.h
#pragma pack(push, 1)
struct _notify_coupon_ensure_time_zocl
{
  unsigned __int8 byCouponTime;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)

// notify_coupon_error_zocl.h
#pragma pack(push, 1)
struct _notify_coupon_error_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)

// notify_local_time_result_zocl.h
#pragma pack(push, 1)
struct _notify_local_time_result_zocl
{
  unsigned __int8 byRet;
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  __int64 size() const;
};
#pragma pack(pop)

// notify_not_use_premium_cashitem_zocl.h
#pragma pack(push, 1)
struct _notify_not_use_premium_cashitem_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_notify_not_use_premium_cashitem_zocl));
  }
};
#pragma pack(pop)

// notify_race_boss_winrate_zocl.h
#pragma pack(push, 2)
#pragma pack(push, 1)
struct _notify_race_boss_winrate_zocl
{
  unsigned __int8 byTotalCnt;
  unsigned __int8 byCurWinCnt;
  unsigned int dwAccTotalCnt;
  unsigned int dwAccWinCnt;
  int size() const
  {
    return 10;
  }
};
#pragma pack(pop)
#pragma pack(pop)

// notify_remain_coupon_zocl.h
#pragma pack(push, 1)
struct _notify_remain_coupon_zocl
{
  unsigned __int8 byRemainCoupon;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)

// npclink_check_item_result_zocl.h
#pragma pack(push, 1)
struct _npclink_check_item_result_zocl
{
  unsigned __int8 byRet;
  _STORAGE_POS_INDIV storage;
  __int64 size() const;
};
#pragma pack(pop)

// nuclear_find_rader_result_zocl.h
#pragma pack(push, 1)
struct _nuclear_find_rader_result_zocl
{
  unsigned __int8 byRaceCode;
  unsigned __int8 byUseClass;
  _nuclear_find_rader_result_zocl();
};
#pragma pack(pop)

// nuclear_position_result_zocl.h
#pragma pack(push, 1)
struct _nuclear_position_result_zocl
{
  unsigned int dwMasterSerial;
  float zPos[3];
  unsigned __int8 byUseClass;
  _nuclear_position_result_zocl();
};
#pragma pack(pop)

// nuclear_result_code_zocl.h
#pragma pack(push, 1)
struct _nuclear_result_code_zocl
{
  unsigned __int8 byRetCode;
  _nuclear_result_code_zocl();
  unsigned __int16 size() const
  {
    return 1;
  }
};
#pragma pack(pop)

// personal_amine_errmsg_zocl.h
#pragma pack(push, 1)
struct _personal_amine_errmsg_zocl
{
  unsigned __int8 byErrCode;
  _personal_amine_errmsg_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_amine_fixpos_zocl.h
#pragma pack(push, 1)
struct _personal_amine_fixpos_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wObjIndex;
  float fFixPos[3];
  unsigned __int8 byAct;
  unsigned __int16 wItemTblIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwOwnerSeiral;
  _personal_amine_fixpos_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// personal_amine_infoui_open_zocl.h
#pragma pack(push, 1)
struct _personal_amine_infoui_open_zocl
{
  unsigned int dwObjSerial;
  unsigned int dwBattery[2];
  _personal_amine_infoui_open_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_amine_make_storage_zocl.h
#pragma pack(push, 1)
struct _personal_amine_make_storage_zocl
{
  unsigned int dwAvatorSerial;
  unsigned int dwTotGold;
  _personal_amine_make_storage_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_automine_alter_dur_zocl.h
#pragma pack(push, 1)
struct _personal_automine_alter_dur_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wHPRate;
  _personal_automine_alter_dur_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// personal_automine_attacked_zocl.h
#pragma pack(push, 1)
struct _personal_automine_attacked_zocl
{
  unsigned __int16 wItemSerial;
  _personal_automine_attacked_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// personal_automine_battery_extract_zocl.h
#pragma pack(push, 1)
struct _personal_automine_battery_extract_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wSerial;
  unsigned int dwDur;
  unsigned __int8 byRetCode;
  _personal_automine_battery_extract_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_automine_battery_insert_zocl.h
#pragma pack(push, 1)
struct _personal_automine_battery_insert_zocl
{
  unsigned int dwObjSerial;
  unsigned int dwOwnerSerial;
  unsigned __int16 wItemSerial;
  _personal_automine_battery_insert_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_automine_delbattery_zocl.h
#pragma pack(push, 1)
struct _personal_automine_delbattery_zocl
{
  unsigned int dwObjSerial;
  unsigned __int8 bySlot;
  _personal_automine_delbattery_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// personal_automine_download_zocl.h
#pragma pack(push, 1)
struct _personal_automine_download_zocl
{
  struct __list
  {
    unsigned __int8 byClientSlotIndex;
    unsigned __int8 byTblCode;
    unsigned __int16 wItemTblIndex;
    unsigned __int8 byDur;
  };
  unsigned __int8 byHaveBag;
  unsigned __int8 byCnt;
  __list list[40];
  unsigned __int64 size();
};
#pragma pack(pop)

// personal_automine_install_zocl.h
#pragma pack(push, 1)
struct _personal_automine_install_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wObjIndex;
  unsigned int dwOwnerSerial;
  unsigned __int16 wItemSerial;
  unsigned __int16 wItemTblIndex;
  float fPos[3];
  unsigned __int8 byFilledSlotCnt;
  _personal_automine_install_zocl();
  unsigned int size() const
  {
    return sizeof(*this);
  }
};
#pragma pack(pop)

// personal_automine_popore_zocl.h
#pragma pack(push, 1)
struct _personal_automine_popore_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int8 byNum;
  _personal_automine_popore_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_automine_selore_zocl.h
#pragma pack(push, 1)
struct _personal_automine_selore_zocl
{
  unsigned int dwObjSerial;
  unsigned __int8 bySelectOre;
  _personal_automine_selore_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// personal_automine_stop_zocl.h
#pragma pack(push, 1)
struct _personal_automine_stop_zocl
{
  unsigned int dwObjSerial;
  unsigned int dwOwnerSerial;
  unsigned __int8 byStopType;
  unsigned __int16 wItemSerial;
  _personal_automine_stop_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// player_macro_result_zocl.h
#pragma pack(push, 1)
struct _player_macro_result_zocl
{
  unsigned int potion[3];
  unsigned int potionvalue[3];
  unsigned int behavior[30];
  char chatting[10][81];
};
#pragma pack(pop)

// post_content_result_zocl.h
#pragma pack(push, 1)
struct _post_content_result_zocl
{
  struct _post_item
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDur;
    unsigned int dwLv;
  };
  unsigned __int8 byErrCode;
  unsigned int dwPostSerial;
  char wszContent[201];
  _post_item Item;
  unsigned int dwGold;
  _post_content_result_zocl();
  unsigned __int16 size() const;
};
#pragma pack(pop)

// post_recv_delivery_zocl.h
#pragma pack(push, 1)
struct _post_recv_delivery_zocl
{
  unsigned __int8 byIndex;
  unsigned int dwPostSerial;
  char wszSendName[17];
  char wszTitle[21];
  bool bItem;
  bool bGold;
  unsigned __int8 byState;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// post_result_zocl.h
#pragma pack(push, 1)
struct _post_result_zocl
{
  unsigned __int8 byErrCode;
  unsigned int dwGold;
  _post_result_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// post_return_zocl.h
#pragma pack(push, 1)
struct _post_return_zocl
{
  struct _post_item
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDur;
    unsigned int dwLv;
  };
  unsigned __int8 byErrCode;
  unsigned int dwPostSerial;
  char wszRecvName[17];
  char wszTitle[21];
  char wszContent[201];
  _post_item Item;
  unsigned int dwGold;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// pt_automine_getoutore_zocl.h
#pragma pack(push, 1)
struct _pt_automine_getoutore_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wItemSerial;
  _pt_automine_getoutore_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// pt_automine_info_zocl.h
#pragma pack(push, 1)
struct _pt_automine_info_zocl
{
  _DB_LOAD_AUTOMINE_MACHINE INFO;
  _pt_automine_info_zocl();
  int size();
};
#pragma pack(pop)

// pt_automine_result_zocl.h
#pragma pack(push, 1)
struct _pt_automine_result_zocl
{
  unsigned __int8 byType;
  unsigned __int8 byRetCode;
  __int64 size() const;
};
#pragma pack(pop)

// pt_automine_state_zocl.h
#pragma pack(push, 1)
struct _pt_automine_state_zocl
{
  unsigned __int8 bySelectOre;
  unsigned int dwGage;
  unsigned __int8 byPage;
  _INVENKEY item;
  _pt_automine_state_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// pt_inform_appoint_zocl.h
#pragma pack(push, 1)
struct _pt_inform_appoint_zocl
{
  unsigned __int8 byClassType;
  char wszAvatorName[17];
  _pt_inform_appoint_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// pt_inform_commission_income_zocl.h
#pragma pack(push, 1)
struct _pt_inform_commission_income_zocl
{
  unsigned __int8 byRet;
  unsigned __int8 byMonth;
  unsigned __int8 byDay;
  unsigned int dwCommission;
  unsigned __int16 size() const;
};
#pragma pack(pop)

// pt_inform_punishment_zocl.h
#pragma pack(push, 1)
struct _pt_inform_punishment_zocl
{
  unsigned __int8 byType;
  int nRemainMin;
  __int64 size() const
  {
    return 5;
  }
};
#pragma pack(pop)

// pt_inform_tax_rate_zocl.h
#pragma pack(push, 1)
struct _pt_inform_tax_rate_zocl
{
  unsigned __int8 byCurrTax;
  unsigned __int8 byNextTax;
  _pt_inform_tax_rate_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// pt_result_appoint_zocl.h
#pragma pack(push, 1)
struct _pt_result_appoint_zocl
{
  unsigned __int8 byLevel;
  unsigned __int8 byClassType;
  long double dPvpPoint;
  char wszAvatorName[17];
  _pt_result_appoint_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// pt_result_change_tax_rate_zocl.h
#pragma pack(push, 1)
struct _pt_result_change_tax_rate_zocl
{
  unsigned __int8 byRet;
  unsigned __int8 byNextTax;
  _pt_result_change_tax_rate_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// pt_result_code_zocl.h
#pragma pack(push, 1)
struct _pt_result_code_zocl
{
  unsigned __int8 byRetCode;
  __int64 size() const
  {
    return 1;
  }
};
#pragma pack(pop)

// pt_result_punishment_zocl.h
#pragma pack(push, 1)
struct _pt_result_punishment_zocl
{
  unsigned __int8 byType;
  char wszCharName[17];
  __int64 size();
};
#pragma pack(pop)

// pvp_cash_recover_itemlist_result_zocl.h
#pragma pack(push, 1)
struct _pvp_cash_recover_itemlist_result_zocl
{
  unsigned __int8 byItemNum;
  int nTalikInfo[14];
  _pvp_cash_recover_itemlist_result_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// pvp_rank_list_result_data_zocl.h
#pragma pack(push, 1)
struct _pvp_rank_list_result_data_zocl
{
  unsigned __int8 byRace;
  unsigned __int8 byVersion;
  unsigned __int8 byPage;
  unsigned __int16 wRankDataLen;
  char szPvpRankData[4000];
  _pvp_rank_list_result_data_zocl();
  __int64 size();
};
#pragma pack(pop)

// quest_download_result_zocl.h
#pragma pack(push, 1)
struct _quest_download_result_zocl
{
  struct __list
  {
    unsigned __int8 byDBSlotIndex;
    _QUEST_DB_BASE::_LIST cont;
    __list();
  };
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotNum;
  __list SlotInfo[30];
  _quest_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// quest_history_download_result_zocl.h
#pragma pack(push, 1)
struct _quest_history_download_result_zocl
{
  struct __list
  {
    unsigned __int8 byIndex;
    char szQuestCode[8];
  };
  unsigned __int8 bySlotNum;
  __list SlotInfo[70];
  _quest_history_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// radar_char_list_result_zocl.h
#pragma pack(push, 1)
struct _radar_char_list_result_zocl
{
  struct _char_info
  {
    unsigned __int8 m_byCharType;
    float m_fPos[2];
  };
  bool bEnd;
  unsigned __int8 byListNum;
  _char_info CharInfo[50];
  _radar_char_list_result_zocl();
  __int64 size();
};
#pragma pack(pop)

// result_csi_buy_zocl.h
#pragma pack(push, 1)
struct _result_csi_buy_zocl
{
  struct __item
  {
    unsigned __int8 byTblCode;
    unsigned __int16 wItemIdx;
    unsigned int dwDur;
    unsigned int dwUp;
    unsigned int dwItemSerial;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  int nCashAmount;
  unsigned __int8 nNum;
  bool bAdjustDiscount;
  __item item[40];
  _result_csi_buy_zocl();
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(806 - 20LL * (40 - nNum));
  }
};
#pragma pack(pop)

// result_csi_error_zocl.h
#pragma pack(push, 1)
struct _result_csi_error_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_result_csi_error_zocl));
  }
};
#pragma pack(pop)

// result_csi_goods_list_zocl.h
#pragma pack(push, 1)
struct _result_csi_goods_list_zocl
{
  int nCashAmount;
  bool bAdjustDiscountRate;
  bool bOneNOne;
  bool bSetDiscount;
  unsigned __int8 bySetDiscount[4];
  bool bLimSale;
  unsigned __int8 byLimDiscount;
  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_result_csi_goods_list_zocl));
  }
};
#pragma pack(pop)

// sell_store_result_zocl.h
#pragma pack(push, 1)
struct _sell_store_result_zocl
{
  unsigned __int8 bSucc;
  unsigned int dwLeftDalant;
  unsigned int dwLeftGold;
  unsigned int dwProfitDanlant;
  unsigned int dwProfitGold;
  unsigned __int8 byDiscountRate;
  unsigned __int8 byRetCode;
};
#pragma pack(pop)

// sf_delay_download_result_zocl.h
#pragma pack(push, 1)
struct _sf_delay_download_result_zocl
{
  struct _eff_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int16 wEffectIndex;
    unsigned int dwRemainTime;
    _eff_list();
  };
  struct _mas_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int8 byMastery;
    unsigned int dwRemainTime;
    _mas_list();
  };
  _eff_list EFF[10];
  _mas_list MAS[10];
  _sf_delay_download_result_zocl();
};
#pragma pack(pop)

// starting_vote_inform_zocl.h
#pragma pack(push, 1)
struct _starting_vote_inform_zocl
{
  int nVoteSerial;
  unsigned __int8 byLimGrade;
  unsigned __int16 wLeftSec;
  unsigned __int16 wContentSize;
  char wszContent[1280];
  _starting_vote_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// store_list_result_zocl.h
#pragma pack(push, 1)
struct _store_list_result_zocl
{
  struct _store_pos
  {
    unsigned int dwStoreIndex;
    float fPos[3];
  };
  unsigned __int8 byStoreNum;
  _store_pos StorePos[100];
  _store_list_result_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// talik_crystal_exchange_zocl.h
#pragma pack(push, 1)
struct _talik_crystal_exchange_zocl
{
  struct _remain_item
  {
    unsigned __int8 byRemainNum;
  };
  unsigned __int8 byErrorCode;
  _remain_item RemainItem[24];
  _talik_crystal_exchange_zocl();
  __int64 size();
};
#pragma pack(pop)

// target_monster_aggro_inform_zocl.h
#pragma pack(push, 1)
struct _target_monster_aggro_inform_zocl
{
  struct _aggro_node
  {
    unsigned __int8 m_IsData;
    char m_Name[32];
    int m_nAggroData;
    int m_nDamageData;
    int m_nKingPowerDamage;
  };
  unsigned int dwSerial;
  _aggro_node m_AggroNode[10];
  float fTimer;
  _target_monster_aggro_inform_zocl();
  void Init();
};
#pragma pack(pop)

// time_limit_accum_logouttime_result_zocl.h
struct _time_limit_accum_logouttime_result_zocl
{
  int bEnable;
  unsigned int dwAccumLogoutTime;
};

static_assert(sizeof(_time_limit_accum_logouttime_result_zocl) == 8);

// trans_gm_msg_inform_zocl.h
#pragma pack(push, 1)
struct _trans_gm_msg_inform_zocl
{
  unsigned __int16 wMsgSize;
  char wszChatData[1281];
  _trans_gm_msg_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// trans_ship_renew_ticket_result_zocl.h
#pragma pack(push, 1)
struct _trans_ship_renew_ticket_result_zocl
{
  unsigned __int8 byErrCode;
  _trans_ship_renew_ticket_result_zocl();
};
#pragma pack(pop)

// trunk_change_passwd_result_zocl.h
#pragma pack(push, 1)
struct _trunk_change_passwd_result_zocl
{
  unsigned __int8 byRetCode;
  _trunk_change_passwd_result_zocl();
};
#pragma pack(pop)

// trunk_download_result_zocl.h
#pragma pack(push, 1)
struct _trunk_download_result_zocl
{
  struct _list
  {
    unsigned __int16 wSerial;
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDurPoint;
    unsigned __int8 byClientIndex;
    unsigned int dwUptInfo;
    unsigned __int8 byRace;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };
  unsigned __int8 byRetCode;
  long double dDalant;
  long double dGold;
  unsigned __int8 byListNum;
  unsigned __int8 byPackNum;
  unsigned __int8 byExtListNum;
  unsigned __int8 byExtPackNum;
  _list ExtList[40];
  _list List[100];
  _trunk_download_result_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

// trunk_est_result_zocl.h
#pragma pack(push, 1)
struct _trunk_est_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned int dwLeftDalant;
  char szPW[16];
  _trunk_est_result_zocl();
};
#pragma pack(pop)

// trunk_extend_result_zocl.h
#pragma pack(push, 1)
struct _trunk_extend_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byPackNum;
  unsigned int dwLeftDalant;
  unsigned int dwConsumDanlant;
  _trunk_extend_result_zocl();
};
#pragma pack(pop)

// trunk_hint_answer_result_zocl.h
#pragma pack(push, 1)
struct _trunk_hint_answer_result_zocl
{
  unsigned __int8 byRetCode;
  char wszPassword[13];
  _trunk_hint_answer_result_zocl();
};
#pragma pack(pop)

// trunk_io_money_result_zocl.h
#pragma pack(push, 1)
struct _trunk_io_money_result_zocl
{
  unsigned __int8 byRetCode;
  long double dTrunkDalant;
  long double dTrunkGold;
  unsigned int dwCharDalant;
  unsigned int dwCharGold;
  unsigned int dwFeeDalant;
  _trunk_io_money_result_zocl();
};
#pragma pack(pop)

// trunk_io_result_zocl.h
#pragma pack(push, 1)
struct _trunk_io_result_zocl
{
  unsigned __int8 byCase;
  unsigned __int8 byRetCode;
  unsigned int dwLeftDalant;
  unsigned int dwConsumDanlant;
  _trunk_io_result_zocl();
};
#pragma pack(pop)

// trunk_potionsocket_division_result_zocl.h
#pragma pack(push, 1)
struct _trunk_potionsocket_division_result_zocl
{
  char sErrorCode;
  unsigned __int16 wSerial;
  unsigned __int16 wParentAmount;
  unsigned __int16 wChildSerial;
  unsigned __int16 wChildAmount;
  _trunk_potionsocket_division_result_zocl();
};
#pragma pack(pop)

// trunk_pw_hint_index_result_zocl.h
#pragma pack(push, 1)
struct _trunk_pw_hint_index_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byHintIndex;
  _trunk_pw_hint_index_result_zocl();
};
#pragma pack(pop)

// trunk_res_division_result_zocl.h
#pragma pack(push, 1)
struct _trunk_res_division_result_zocl
{
  unsigned __int8 byErrCode;
  unsigned __int16 wParentSerial;
  unsigned __int16 wParentAmount;
  unsigned __int16 wChildSerial;
  unsigned __int16 wChildAmount;
  _trunk_res_division_result_zocl();
};
#pragma pack(pop)

// unit_download_result_zocl.h
#pragma pack(push, 1)
struct _unit_download_result_zocl
{
  struct _list
  {
    unsigned __int8 bySlotIndex;
    _UNIT_DB_BASE::_LIST UnitData;
    _list();
  };
  unsigned __int8 byUnitNum;
  _list UnitList[4];
  _unit_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)

// unmannedtrader_buy_item_result_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_buy_item_result_zocl
{
  struct __list
  {
    unsigned __int8 byRet;
    unsigned int dwPrice;
    unsigned __int16 dwNewItemSerial;
    unsigned __int8 byItemTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwDur;
    unsigned int dwLv;
  };
  unsigned __int8 byRetCode;
  unsigned int dwPayDalant;
  unsigned int dwLeftDalant;
  unsigned __int8 byNum;
  __list List[10];
  _unmannedtrader_buy_item_result_zocl();
  __int64 size() const;
};
#pragma pack(pop)

// unmannedtrader_close_item_inform_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_close_item_inform_zocl
{
  unsigned __int8 byTaxRate;
  unsigned __int16 wItemSerial;
  unsigned int dwRegistSerial;
  unsigned int dwPrice;
  __int64 size() const;
};
#pragma pack(pop)

// unmannedtrader_continue_item_inform_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_continue_item_inform_zocl
{
  struct __list
  {
    unsigned __int16 wItemSerial;
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
  };
  unsigned __int8 byTaxRate;
  unsigned __int8 byNum;
  __list List[10];
  __int64 size() const;
};
#pragma pack(pop)

// unmannedtrader_re_regist_result_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_re_regist_result_zocl
{
  struct __list
  {
    unsigned __int8 byRet;
    bool bRegist;
    unsigned __int16 wItemSerial;
    unsigned int dwPrice;
    unsigned int dwRegedSerial;
    unsigned int dwListIndex;
    unsigned int dwTax;
  };
  unsigned int dwLeftDalant;
  unsigned __int8 byNum;
  __list List[10];
  __int64 size();
};
#pragma pack(pop)

// unmannedtrader_Regist_item_inform_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_Regist_item_inform_zocl
{
  struct __list
  {
    unsigned __int16 wItemSerial;
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
    unsigned int dwLeftSec;
    unsigned int dwListIndex;
  };
  unsigned __int8 byNum;
  __list List[10];
  _unmannedtrader_Regist_item_inform_zocl();
  __int64 size();
};
#pragma pack(pop)

// unmannedtrader_regist_item_success_result_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_regist_item_success_result_zocl
{
  unsigned __int16 wItemSerial;
  unsigned int dwPrice;
  unsigned int dwRegedSerial;
  unsigned int dwListIndex;
  unsigned int dwTax;
  unsigned int dwLeftDalant;
  unsigned __int16 wSepaSerial;
  unsigned __int8 bySepaAmount;
  __int64 size() const;
};
#pragma pack(pop)

// attack_count_result_zocl.h
#pragma pack(push, 1)
struct _attack_count_result_zocl
{
  unsigned int dwAtterSerial;
  unsigned __int8 byAttackPart;
  bool bCritical;
  bool bWPActive;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_count_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_count_result_zocl) == 328, "_attack_count_result_zocl packing mismatch");

// attack_force_result_zocl.h
#pragma pack(push, 1)
struct _attack_force_result_zocl
{
  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byForceIndex;
  unsigned __int8 byForceLv;
  __int16 zAreaPos[2];
  unsigned __int8 byAttackPart;
  bool bCritical;
  bool bWPActive;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_force_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_force_result_zocl) == 335, "_attack_force_result_zocl packing mismatch");

// attack_selfdestruction_result_zocl.h
#pragma pack(push, 1)
struct _attack_selfdestruction_result_zocl
{
  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byAttackPart;
  unsigned __int16 wBulletIndex;
  bool bCritical;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_selfdestruction_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_selfdestruction_result_zocl) == 330, "_attack_selfdestruction_result_zocl packing mismatch");

// attack_siege_result_zocl.h
#pragma pack(push, 1)
struct _attack_siege_result_zocl
{
  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byAttackPart;
  unsigned __int16 wBulletIndex;
  bool bCritical;
  __int16 zTar[2];
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_siege_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_siege_result_zocl) == 334, "_attack_siege_result_zocl packing mismatch");

// attack_skill_result_zocl.h
#pragma pack(push, 1)
struct _attack_skill_result_zocl
{
  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byEffectCode;
  unsigned __int16 wSkillIndex;
  unsigned __int8 bySkillLv;
  unsigned __int8 byAttackPart;
  unsigned __int16 wBulletIndex;
  bool bCritical;
  __int16 zAttackPos[2];
  bool bWPActive;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_skill_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_skill_result_zocl) == 339, "_attack_skill_result_zocl packing mismatch");

// attack_unit_result_zocl.h
#pragma pack(push, 1)
struct _attack_unit_result_zocl
{
  unsigned int dwAtterSerial;
  unsigned __int8 byWeaponPart;
  unsigned __int16 wWeaponIndex;
  unsigned __int16 wBulletIndex;
  unsigned __int8 byAttackPart;
  bool bCritical;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_unit_result_zocl();

  int size();
};
#pragma pack(pop)

static_assert(sizeof(_attack_unit_result_zocl) == 332, "_attack_unit_result_zocl packing mismatch");
