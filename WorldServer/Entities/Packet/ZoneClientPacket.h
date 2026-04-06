#pragma once

#include "../CPlayer.h"
#include "../AHNHS_TRANS_BUFFER.h"
#include "../DB_LOAD_AUTOMINE_MACHINE.h"
#include "../DqsDbStructs.h"
#include "../IdaCompat.h"
#include "../InvenKey.h"
#include "../REGED_AVATOR_DB.h"
#include "../StoragePos.h"

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
  int size() const;
};
#pragma pack(pop)

// a_trade_clear_item_result_zocl.h
#pragma pack(push, 1)
struct _a_trade_clear_item_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wItemSerial;
  unsigned int dwRegistSerial;
  int size() const;
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
  int size() const
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
  int size() const
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
  int size();
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
  int size();
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
  unsigned int dwHP;
  unsigned int dwSP;
  unsigned int dwFP;
  long double dPvpPoint;
  long double dPvpCashBag;
  long double dPvpTempCash;
  unsigned int dwPvpRank;
  unsigned __int8 byPvpClass;
  unsigned __int8 byPlusKey;
  unsigned __int16 wXorKey;
  unsigned int dwMaxHP;
  unsigned int dwMaxSP;
  unsigned int dwMaxFP;
  unsigned __int16 wMapIndex;
  float fPos[3];
  _list_equip EquipList[8];
  _list_embellish EmbellishList[7];
  unsigned __int8 byHolyMasterState;
  unsigned int dwGuildSerial;
  unsigned __int8 byGuildGrade;
  unsigned __int8 byEffectValue[2];
  unsigned __int8 byUseTrunkSlotNum;
  unsigned int dwMaxDP;
  unsigned int dwDP;
  unsigned __int8 byHonorGuildRank;
  int size();
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
  int size();
};
#pragma pack(pop)

// chat_far_failure_zocl.h
#pragma pack(push, 1)
struct _chat_far_failure_zocl
{
  bool bBlock;
  int size() const;
};
#pragma pack(pop)

// chat_guild_failure_zocl.h
#pragma pack(push, 1)
struct _chat_guild_failure_zocl
{
  unsigned __int8 byErrCode;
  int size() const;
};
#pragma pack(pop)

// chat_lock_inform_zocl.h
#pragma pack(push, 1)
struct _chat_lock_inform_zocl
{
  bool bLock;
  int size() const;
};
#pragma pack(pop)

// chat_multi_far_failure_zocl.h
#pragma pack(push, 1)
struct _chat_multi_far_failure_zocl
{
  char wszDstName[17];
  unsigned __int8 byFailCode;
  int size() const;
};
#pragma pack(pop)

// chat_multi_far_trans_zocl.h
#pragma pack(push, 1)
struct _chat_multi_far_trans_zocl
{
  unsigned __int16 wSize;
  char sData[500];
  _chat_multi_far_trans_zocl();
  int size();
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
  int size() const;
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
  int size() const
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
  int size();
};
#pragma pack(pop)

// darkhole_answer_reenter_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_answer_reenter_result_zocl
{
  unsigned __int8 byRetCode;
  int size() const;
};
#pragma pack(pop)

// darkhole_ask_reenter_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_ask_reenter_inform_zocl
{
  unsigned __int16 wChannelIndex;
  unsigned int dwChannelSerial;
  int size() const;
};
#pragma pack(pop)

// darkhole_channel_close_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_channel_close_inform_zocl
{
  char sDum;
  int size() const;
};
#pragma pack(pop)

// darkhole_clear_out_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_clear_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wTarMapIndex;
  __int16 zTarPos[3];
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// darkhole_destroy_zocl.h
#pragma pack(push, 1)
struct _darkhole_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  int size() const;
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// darkhole_giveup_out_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_giveup_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wTarMapIndex;
  __int16 zTarPos[3];
  int size() const;
};
#pragma pack(pop)

// darkhole_job_count_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_job_count_inform_zocl
{
  unsigned __int8 byJobIndex;
  unsigned __int16 wJobCount;
  int size() const;
};
#pragma pack(pop)

// darkhole_job_pass_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_job_pass_inform_zocl
{
  unsigned __int8 byJobIndex;
  int size() const;
};
#pragma pack(pop)

// darkhole_leader_change_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_leader_change_inform_zocl
{
  unsigned int dwNewLeaderSerial;
  int size() const;
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
  int size();
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
  int size();
};
#pragma pack(pop)

// darkhole_mission_pass_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_mission_pass_inform_zocl
{
  char szCompleteMsgCode[17];
  int size() const;
};
#pragma pack(pop)

// darkhole_mission_quest_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_mission_quest_inform_zocl
{
  char sDum;
  int size() const;
};
#pragma pack(pop)

// darkhole_new_member_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_new_member_inform_zocl
{
  unsigned int dwNewMemberSerial;
  char wszNewMemberName[17];
  bool bReconnect;
  int size() const;
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
  int size();
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
  int size() const;
};
#pragma pack(pop)

// darkhole_open_portal_by_react_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_open_portal_by_react_inform_zocl
{
  unsigned __int16 wPortalIndex;
  int size() const;
};
#pragma pack(pop)

// darkhole_open_result_zocl.h
#pragma pack(push, 1)
struct _darkhole_open_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wHoleIndex;
  unsigned int dwHoleSerial;
  int size() const;
};
#pragma pack(pop)

// darkhole_outof_member_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_outof_member_inform_zocl
{
  unsigned int dwMemberSerial;
  bool bDisconnect;
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// darkhole_real_add_time_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_real_add_time_inform_zocl
{
  int nAddSec;
  unsigned int dwTotalLimSec;
  char szMsg[17];
  int size() const;
};
#pragma pack(pop)

// darkhole_real_msg_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_real_msg_inform_zocl
{
  char szMsg[17];
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// darkhole_timeout_inform_zocl.h
#pragma pack(push, 1)
struct _darkhole_timeout_inform_zocl
{
  char sDum;
  int size() const;
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
  int size() const
  {
    return 29LL;
  }
};
struct _exchange_lend_item_result_zocl
{
  unsigned __int8 byErrorCode;
  _combine_lend_item_result_zocl::_lend_item Item;
  int size() const;
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
  int size();
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
  int size() const
  {
    return static_cast<unsigned __int16>(sizeof(_gm_msg_gmcall_list_response_zocl));
  }
};
#pragma pack(pop)
static_assert(sizeof(_gm_msg_gmcall_list_response_zocl) == 226, "gm call list packet size mismatch");

// moved from ZoneServer-GU.h (4975)
struct _gm_msg_gmcall_response_zocl
{
  int m_bCall;
};

// moved from ZoneServer-GU.h (4976)
struct _gm_msg_gm_accept_response_zocl
{
  int m_nResultCode;
};

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
  int size();
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
  int size();
};
#pragma pack(pop)

// guild_battle_get_gravity_stone_result_zocl.h
#pragma pack(push, 1)
struct _guild_battle_notify_destroy_ball_zocl
{
  unsigned int dwOwnerObjSerial;
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
  int size() const
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
  unsigned int dwLeftRedScore;
  unsigned int dwRightBlueScore;
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwRightBlueGoalCnt;
  unsigned __int8 byLeftHour;
  unsigned __int8 byLeftMin;
  unsigned __int8 byLeftSec;
  int size() const
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
  int size() const;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4537)
struct _ask_join_guild_battle_request_zocl
{
  char wszDestGuildName[17];
};

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
  int size();
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
  int size();
};
#pragma pack(pop)

// holy_keeper_attackable_inform_zocl.h
#pragma pack(push, 1)
struct _holy_keeper_attackable_inform_zocl
{
  unsigned int dwKeeperSerial;
  unsigned __int8 byMasterRace;
  bool bAttackAbleState;
  int size() const
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
  int size();
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
  int size() const;
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
  int size() const;
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
  int size();
};
#pragma pack(pop)

// limitedsale_event_inform_zocl.h
#pragma pack(push, 1)
struct _limitedsale_event_inform_zocl
{
  unsigned __int8 byTableCode;
  unsigned int dwIndex;
  unsigned __int16 wNum;
  int size();
};
#pragma pack(pop)

// move_to_own_stonemap_inform_zocl.h
#pragma pack(push, 1)
struct _move_to_own_stonemap_inform_zocl
{
  unsigned __int8 byStoneMapMoveInfo;
  int size() const;
};
#pragma pack(pop)

// move_to_own_stonemap_result_zocl.h
#pragma pack(push, 1)
struct _move_to_own_stonemap_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byMapIndex;
  __int16 sNewPos[3];
  int size() const;
};
#pragma pack(pop)

// not_arranged_char_inform_zocl.h
#pragma pack(push, 1)
struct _not_arranged_char_inform_zocl
{
  unsigned __int8 byCharNum;
  _NOT_ARRANGED_AVATOR_DB CharList[50];
  _not_arranged_char_inform_zocl();
  int size();
};
#pragma pack(pop)

// notice_move_limit_map_msg_zocl.h
#pragma pack(push, 1)
struct _notice_move_limit_map_msg_zocl
{
  unsigned __int8 byType;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4660)
struct _move_limit_map_zone_result_zocl
{
  char byRet;
};

// notify_cont_play_time_zocl.h
#pragma pack(push, 1)
struct _notify_cont_play_time_zocl
{
  unsigned __int8 byContTime;
  unsigned __int8 byContMin;
  int size() const
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
  int size() const
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
  int size() const
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
  int size() const;
};
#pragma pack(pop)

// notify_not_use_premium_cashitem_zocl.h
#pragma pack(push, 1)
struct _notify_not_use_premium_cashitem_zocl
{
  unsigned __int16 wItemSerial;
  int size() const
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
  int size() const
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
  int size() const;
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
  int size() const
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// personal_amine_infoui_open_zocl.h
#pragma pack(push, 1)
struct _personal_amine_infoui_open_zocl
{
  unsigned int dwObjSerial;
  unsigned int dwBattery[2];
  _personal_amine_infoui_open_zocl();
  int size() const;
};
#pragma pack(pop)

// personal_amine_make_storage_zocl.h
#pragma pack(push, 1)
struct _personal_amine_make_storage_zocl
{
  unsigned int dwAvatorSerial;
  unsigned int dwTotGold;
  _personal_amine_make_storage_zocl();
  int size() const;
};
#pragma pack(pop)

// personal_automine_alter_dur_zocl.h
#pragma pack(push, 1)
struct _personal_automine_alter_dur_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wHPRate;
  _personal_automine_alter_dur_zocl();
  int size() const;
};
#pragma pack(pop)

// personal_automine_attacked_zocl.h
#pragma pack(push, 1)
struct _personal_automine_attacked_zocl
{
  unsigned __int16 wItemSerial;
  _personal_automine_attacked_zocl();
  int size() const;
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// personal_automine_delbattery_zocl.h
#pragma pack(push, 1)
struct _personal_automine_delbattery_zocl
{
  unsigned int dwObjSerial;
  unsigned __int8 bySlot;
  _personal_automine_delbattery_zocl();
  int size() const;
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
  int size();
};
#pragma pack(pop)

// personal_automine_install_zocl.h
#pragma pack(push, 1)
struct _personal_automine_install_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wObjIndex;
  unsigned int dwOwnerSerial;
  unsigned __int16 wItemTblIndex;
  unsigned __int16 wItemSerial;
  float fPos[3];
  unsigned __int8 byFilledSlotCnt;
  _personal_automine_install_zocl();
  int size() const
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
  int size() const;
};
#pragma pack(pop)

// personal_automine_selore_zocl.h
#pragma pack(push, 1)
struct _personal_automine_selore_zocl
{
  unsigned int dwObjSerial;
  unsigned __int8 bySelectOre;
  _personal_automine_selore_zocl();
  int size() const;
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
  int size() const;
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// post_result_zocl.h
#pragma pack(push, 1)
struct _post_result_zocl
{
  unsigned __int8 byErrCode;
  unsigned int dwGold;
  _post_result_zocl();
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// pt_automine_getoutore_zocl.h
#pragma pack(push, 1)
struct _pt_automine_getoutore_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wItemSerial;
  _pt_automine_getoutore_zocl();
  int size() const;
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// pt_inform_appoint_zocl.h
#pragma pack(push, 1)
struct _pt_inform_appoint_zocl
{
  unsigned __int8 byClassType;
  char wszAvatorName[17];
  _pt_inform_appoint_zocl();
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// pt_inform_punishment_zocl.h
#pragma pack(push, 1)
struct _pt_inform_punishment_zocl
{
  unsigned __int8 byType;
  int nRemainMin;
  int size() const
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// pt_result_change_tax_rate_zocl.h
#pragma pack(push, 1)
struct _pt_result_change_tax_rate_zocl
{
  unsigned __int8 byRet;
  unsigned __int8 byNextTax;
  _pt_result_change_tax_rate_zocl();
  int size() const;
};
#pragma pack(pop)

// pt_result_code_zocl.h
#pragma pack(push, 1)
struct _pt_result_code_zocl
{
  unsigned __int8 byRetCode;
  int size() const
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
  int size();
};
#pragma pack(pop)

// pvp_cash_recover_itemlist_result_zocl.h
#pragma pack(push, 1)
struct _pvp_cash_recover_itemlist_result_zocl
{
  unsigned __int8 byItemNum;
  int nTalikInfo[14];
  _pvp_cash_recover_itemlist_result_zocl();
  int size();
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
  int size();
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4822)
struct _pvp_rank_list_result_nodata_zocl
{
  char byRace;
  char byPage;
  char byRet;
};

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
  int size();
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
  int size();
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
  int size();
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
  int size() const
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
  int size() const
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
  int size() const
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
  int size();
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
  int size();
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
  int size();
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
  int size();
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
  int size();
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

// moved from ZoneServer-GU.h (7452)
struct _Exttrunk_extend_result_zocl
{
  char byRetCode;
  char byPackNum;
  char byExtendNum;
};

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
  int size();
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
  int size() const;
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
  int size() const;
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
  int size() const;
};
#pragma pack(pop)

// unmannedtrader_waiting_register_item_notify_register_success_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_waiting_register_item_notify_register_success_zocl
{
  unsigned int dwRegedSerial;
  unsigned __int16 wItemSerial;
  int size() const;
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
  int size();
};
#pragma pack(pop)

// unmannedtrader_Regist_item_inform_zocl.h
#pragma pack(push, 1)
struct _unmannedtrader_Regist_item_inform_zocl
{
  struct __list
  {
    char bWaitingRegister;
    unsigned __int16 wItemSerial;
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
    unsigned int dwLeftSec;
    unsigned int dwListIndex;
  };
  unsigned __int8 byNum;
  __list List[10];
  _unmannedtrader_Regist_item_inform_zocl();
  int size();
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
  int size() const;
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


// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_amine_mineore_zocl
{
  #pragma pack(push, 1)
  struct __changed
  {
    unsigned __int16 wItemIndex;
    unsigned int dwDur;
    unsigned __int16 wItemSerial;
  };
  #pragma pack(pop)

  unsigned int dwObjSerial;
  unsigned __int8 m_byUseBattery;
  unsigned int dwBattery;
  unsigned __int8 byChangedNum;
  __changed change[40];

  void clear();
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_uninstall_zocl
{
  #pragma pack(push, 1)
  struct __battery
  {
    unsigned int dwDur;
    unsigned __int16 wSerial;
  };
  #pragma pack(pop)

  unsigned int dwObjSerial;
  unsigned __int8 byActType;
  unsigned int dwOwnerSerial;
  unsigned __int16 wItemSerial;
  unsigned __int8 byCnt;
  __battery battery[2];

  _personal_automine_uninstall_zocl();
  int size() const;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_uninstall_circle_zocl
{
  unsigned int dwObjSerial;
  unsigned __int8 byActType;

  _personal_automine_uninstall_circle_zocl();
  int size() const;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_current_state_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int8 byFilledSlotCnt;
  unsigned int dwBatteryGage;

  _personal_automine_current_state_zocl();
  int size() const;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_member_download_zocl
{
  unsigned __int8 byDownType;
  unsigned int dwGuildSerial;
  unsigned __int8 byGuildGrade;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  long double dDalant;
  long double dGold;
  unsigned __int8 byGuildRoomType;
  int GuildRoomRestTime;
  unsigned __int8 byCurTax;
  unsigned int dwTotWin;
  unsigned int dwTotDraw;
  unsigned int dwTotLose;
  bool bPossibleElectMaster;
  unsigned __int16 wDataSize;
  char sData[10000];
};
#pragma pack(pop)

// moved from CMainThread.h
struct  _guild_applier_download_zocl
{
  unsigned __int16 wDataSize;
  char sData[10000];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_query_info_result_zocl
{
  unsigned int dwGuildSerial;
  char wszGuildName[17];
  unsigned __int8 byGrade;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  unsigned int dwTotWin;
  unsigned int dwTotDraw;
  unsigned int dwTotLose;
  unsigned __int8 byCurTax;
};
#pragma pack(pop)

// moved from CMainThread.h
struct  _guild_money_io_download_zocl
{
  unsigned __int16 wDataSize;
  char sData[10000];
};

// moved from CMainThread.h
struct  _guild_member_buddy_download_zocl
{
  unsigned __int16 wDataSize;
  char sData[900];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _party_join_joiner_result_zocl
{
  #pragma pack(push, 1)
  struct _list
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    char wszAvatorName[17];
  };
  #pragma pack(pop)

  char byLootShareMode;
  char byListNum;
  _party_join_joiner_result_zocl::_list List[8];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _cash_event_inform_zocl
{
  #pragma pack(push, 1)
  struct __lim_sale
  {
    #pragma pack(push, 1)
    struct __Item
    {
      char byTableCode;
      unsigned int dwIndex;
      unsigned __int16 wNum;
    };
    #pragma pack(pop)

    char byCount;
    char byDiscount;
    __Item item[20];
  };
  #pragma pack(pop)

  char byEventType;
  char byEventStatus;
  unsigned __int16 wYear[2];
  char byMonth[2];
  char byDay[2];
  char byHour[2];
  char byMinute[2];
  __lim_sale LimitedSale;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _cash_discount_event_inform_zocl
{
  char byEventType;
  unsigned __int16 wCsDiscount;
  unsigned __int16 wYear[2];
  char byMonth[2];
  char byDay[2];
  char byHour[2];
  char byMinute[2];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (2775)
#pragma pack(push, 1)
struct _conditional_event_inform_zocl
{
  char byEventType;
  char byEventStatus;
  char szMsgCode[8];
  unsigned __int16 wCsDiscount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_keeper_inform_zocl
{
  unsigned int dwAtterSerial;
  bool bCritical;
  char byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_update_player_time_limit_Info_zocl
{
  unsigned int dwFatigue;
  unsigned __int16 wStatus;
  unsigned int dwRemainTime;
  unsigned int dwAccumPlayTime;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (2682)
struct _notify_time_limit_panalty_status_zocl
{
  char byRetCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guildroom_enter_result_zocl
{
  char byRetCode;
  char byRetSubCode;
  char byMapIndex;
  unsigned __int16 wMapLayerIndex;
  __int16 sNewPos[3];
  int restTime;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _weekly_guild_rank_result_zocl
{
  #pragma pack(push, 1)
  struct _list
  {
    char byRank;
    char wszGuildName[17];
    char byGrade;
    unsigned int dwPvpPoint;
  };
  #pragma pack(pop)

  unsigned int dwVer;
  char byRace;
  char byExistSelfRankInfo;
  char byCnt;
  _weekly_guild_rank_result_zocl::_list list[11];

  int size();
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_pack_fill_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  unsigned int dwComsumMoney[7];
  unsigned int dwLeftMoney[7];
  char byFillNum;
  _unit_pack_fill_request_clzo::__list List[8];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _throw_skill_result_one_zocl
{
  #pragma pack(push, 1)
  struct __effected_list
  {
    char byObjID;
    unsigned int dwObjSerial;
    char byRetCode;
    unsigned __int16 wEffectValue;
  };
  #pragma pack(pop)

  char byErrCode;
  char byAttackSerial;
  _CHRID idDster;
  char byEffectedNum;
  _throw_skill_result_one_zocl::__effected_list list[30];
};
#pragma pack(pop)

// moved from CMainThread.h
struct _throw_unit_result_one_zocl
{
  char byErrCode;
  char byAttackSerial;
  _CHRID idDster;
  char byEffectedNum;
  _throw_skill_result_one_zocl::__effected_list list[30];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _npc_quest_list_result_zocl
{
  char byQuestNum;
  unsigned int QuestIndexList[30];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_trap_inform_zocl
{
  unsigned int dwAtterSerial;
  bool bCritical;
  char byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_atrade_tax_zocl
{
  unsigned int dwGSerial;
  char byTax;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_effect_for_get_gold_box_item_zocl
{
  char byBoxType;
  char byTableCode;
  unsigned __int16 wItemIndex;
  char byBoxDur;
  unsigned int dwCharSerial;
  char szCharacterName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _set_item_check_result_zocl
{
  char byResult;
  unsigned int dwSetIndex;
  char bySetEffectNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _nuclear_bomb_drop_result_zocl
{
  char byRaceCode;
  char byItemTableCode;
  unsigned __int16 wItemRecIndex;
  char byUseClass;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (2466)
struct _nuclear_explosion_success_zocl
{
  char byRaceCode;
  char byUseClass;
};

// moved from ZoneServer-GU.h (2735)
struct _nuclear_bomb_destruction_zocl
{
  char byRaceCode;
  char byUseClass;
};

// moved from ZoneServer-GU.h (2773)
struct _nuclear_bomb_explosion_result_zocl
{
  char byRaceCode;
  char byUseClass;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _throw_unit_result_other_zocl
{
  char byRetCode;
  _CHRID idPerformer;
  unsigned __int16 wBulletIndex;
  char byAttackSerial;
  _CHRID idDster;
  char byEffectedNum;
  _throw_skill_result_one_zocl::__effected_list list[30];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _started_vote_inform_zocl
{
  int nVoteSerial;
  char byLimGrade;
  unsigned __int16 wLeftSec;
  bool bActed;
  unsigned __int16 wPoint[3];
  bool bHurry;
  unsigned __int16 wContentSize;
  char wszContent[1280];

  _started_vote_inform_zocl();
  int size();
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _other_shape_change_zocl
{
  #pragma pack(push, 1)
  struct _model
  {
    unsigned __int16 wPartIndex;
    char byLv;
  };
  #pragma pack(pop)

  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  char byPartIndex;
  _other_shape_change_zocl::_model ModelPerPart;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _combine_item_result_zocl
{
  #pragma pack(push, 1)
  struct __item
  {
    char byTableCode;
    unsigned __int16 wItemIndex;
    unsigned int dwDurPoint;
    unsigned __int16 wSerial;
    unsigned int dwLv;
  };
  #pragma pack(pop)

  char byErrCode;
  unsigned int dwFee;
  unsigned int dwLeftDalant;
  _combine_item_result_zocl::__item NewItem;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pvp_order_view_point_inform_zocl
{
  unsigned int dwSerial;
  long double dPvpPoint;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_player_time_limit_Info_zocl
{
  unsigned int dwFatigue;
  unsigned __int16 wStatus;
  unsigned int dwRemainTime;
  unsigned int dwAccumPlayTime;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_active_jade_result_zocl
{
  int nErrorCode;
  unsigned __int16 wSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trans_ship_ticket_inform_zocl
{
  struct __list
  {
    char byDirectCode;
    unsigned __int16 wLeftTicketNum;
  };

  unsigned int dwNpcSerial;
  _trans_ship_ticket_inform_zocl::__list TicketList[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unmannedtrader_Sell_Wait_item_inform_zocl
{
  struct __list
  {
    unsigned __int16 wItemSerial;
    unsigned int dwSellDalant;
    unsigned int dwTax;
  };

  char byNum;
  unsigned int dwTotalSellDalant;
  unsigned int dwTotalTaxDalant;
  unsigned int dwCurInvenDalant;
  _unmannedtrader_Sell_Wait_item_inform_zocl::__list List[10];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _nuclear_bomb_position_inform_zocl
{
  char byRaceCode;
  char byUseClass;
  float zPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_gold_box_event_status_zocl
{
  char byBoxType;
  char byTableCode;
  unsigned __int16 wItemIndex;
  char byBoxDur;
  unsigned int dwCharSerial;
  char szCharacterName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unmannedtrader_search_list_result_zocl
{
  struct __list
  {
    unsigned int dwRegistSerial;
    char byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int64 dwD;
    unsigned int dwU;
    unsigned int dwPrice;
    unsigned int dwOwner;
    char wszOwnerName[17];
    unsigned int dwLeftSec;
  };

  char byRet;
  unsigned int dwListIndex;
  char byDivision;
  char byClass;
  char bySubClass;
  char bySortType;
  unsigned int dwVer;
  char byPage;
  unsigned int dwMaxPage;
  char byCnt;
  __list List[10];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pvp_cash_point_recover_result_zocl
{
  char byRetCode;
  int nRecvrPoint;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4455)
struct _notify_pvp_cash_point_error_zocl
{
  char byRetCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_fin_pvppoint_result_zocl
{
  char byWin;
  long double dPvpPoint;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_notify_committee_member_position_zocl
{
  __int16 zPos[2];
  char byColor;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_restart_zocl
{
  char byColorInx;
  unsigned int dwLeftRedScore;
  unsigned int dwRightBlueScore;
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwRightBlueGoalCnt;
  char byLeftHour;
  char byLeftMin;
  char byLeftSec;
  int iRedPortalInx;
  int iBluePortalInx;
  int iRegenPortalInx[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_start_zocl
{
  char byColorInx;
  unsigned int dwLeftRedScore;
  unsigned int dwRightBlueScore;
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwRightBlueGoalCnt;
  char byLeftHour;
  char byLeftMin;
  char byLeftSec;
  int iRedPortalInx;
  int iBluePortalInx;
  int iRegenPortalInx[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_notify_ball_position_zocl
{
  __int16 zPos[2];
  char byColor;
  unsigned int dwOwnerObjSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_pvp_inform_zocl
{
  unsigned int dwLeftRedScore;
  unsigned int dwRightBlueScore;
  char byLeftHour;
  char byLeftMin;
  char byLeftSec;
};
#pragma pack(pop)

// moved from CMainThread.h
struct _guild_battle_notify_before_start_zocl
{
  unsigned __int8 byLeftMinutes;
  char wszGuildNameRed[17];
  char wszGuildNameBlue[17];
};

// moved from ZoneServer-GU.h (4538)
struct _guild_battle_left_minute_before_start_zocl
{
  char byLeftMinute;
};

// moved from ZoneServer-GU.h (4539)
struct _guild_battle_notify_regen_ball_zocl
{
  int iPortalInx;
};

// moved from CMainThread.h
struct _guild_battle_notify_battle_result_zocl
{
  unsigned __int8 byResult;
  char wszGuildNameRed[17];
  char wszGuildNameBlue[17];
};

// moved from ZoneServer-GU.h (4546)
struct _guild_battle_notify_pass_gravity_stone_take_limit_time_zocl
{
  char byDummy;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_rank_list_result_no_data_zocl
{
  char byRace;
  unsigned int dwCurVer;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4577)
struct _join_guild_battle_result_zocl
{
  char byRet;
};

// moved from ZoneServer-GU.h (4580)
struct _possible_battle_guild_list_error_result_zocl
{
  char byRet;
};

// moved from ZoneServer-GU.h (4582)
struct _guild_battle_reserved_schedule_same_result_zocl
{
  unsigned int dwCurVer;
};

// moved from ZoneServer-GU.h (4583)
struct _guild_battle_reserved_schedule_no_data_result_zocl
{
  char byToday;
  char byTommorow;
  char byDayId;
};

// moved from ZoneServer-GU.h (4584)
struct _guild_battle_current_battle_info_error_result_zocl
{
  char byRet;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _effect_start_inform_zocl
{
  char byLv;
  unsigned __int16 wEffectCode;
  unsigned __int16 wDurSec;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_set_race_buff_by_holy_quest_zocl
{
  char byLv;
  unsigned __int16 wEffectIndex;
  unsigned int dwDestSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4590)
struct _notify_release_race_buff_by_holy_quest_zocl
{
  int nError;
};

// moved from ZoneServer-GU.h (4666)
struct _remain_potion_buf_use_time_inform_zocl
{
  bool bUse;
  char byDay;
  char byHour;
  char byMin;
};

// moved from ZoneServer-GU.h (4667)
struct _remove_potion_effect_inform_zocl
{
  unsigned __int16 wPotionIndex;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _other_shape_cash_rename_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  char wszName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_unmanned_trader_weekly_daily_income_zocl
{
  unsigned __int64 ui64TotalIncome;
  char wszRaceLeaderName[17];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4924)
struct _unmannedtrader_taxrate_inform_zocl
{
  char byTaxRate;
};

// moved from ZoneServer-GU.h (4216)
struct _hs_msg_make_crc_req_msg_zocl
{
  unsigned __int16 wSeq;
  char byReqMsg[160];
};

// moved from ZoneServer-GU.h (4217)
struct _hs_msg_make_guid_req_msg_zocl
{
  unsigned __int16 wSeq;
  char byGuidReqMsg[20];
};

// moved from ZoneServer-GU.h (4218)
struct _hs_msg_ack_guid_ack_msg_zocl
{
  unsigned __int16 wSeq;
};

// moved from ZoneServer_Regular_1_0_x64.h
#pragma pack(push, 1)
struct _hs_msg_req_qry_zocl
{
  _AHNHS_TRANS_BUFFER AuthKey;
};
static_assert(sizeof(_hs_msg_req_qry_zocl) == 402);
#pragma pack(pop)

// moved from ZoneServer_Regular_1_0_x64.h
#pragma pack(push, 1)
struct _hs_msg_notify_zocl
{
  char byRet;
};
static_assert(sizeof(_hs_msg_notify_zocl) == 1);
#pragma pack(pop)

// moved from ZoneServer_Regular_1_0_x64.h
#pragma pack(push, 1)
struct _npr_msg_req_qry_zocl
{
  char packet[4096];
  unsigned int packetSize;
};
static_assert(sizeof(_npr_msg_req_qry_zocl) == 4100);
#pragma pack(pop)

// moved from ZoneServer_Regular_1_0_x64.h
#pragma pack(push, 1)
struct _npr_msg_notify_zocl
{
  unsigned int byRet;
};
static_assert(sizeof(_npr_msg_notify_zocl) == 4);
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unmannedtrader_regist_item_error_result_zocl
{
  char byRet;
  unsigned __int16 wItemSerial;
  unsigned int dwRetParam1;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _a_trade_sell_inform_zocl
{
  unsigned __int16 wItemSerial;
  unsigned int dwAddDalant;
  unsigned int dwTaxDalant;
  unsigned int dwTotalDalant;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _ore_cutting_result_zocl
{
  #pragma pack(push, 1)
  struct _list
  {
    unsigned __int16 wResIndex;
    char byAddAmount;
  };
  #pragma pack(pop)

  char byErrCode;
  char byLeftNum;
  unsigned int dwLeftDalant;
  unsigned int dwConsumDalant;
  char byCuttingNum;
  _ore_cutting_result_zocl::_list ResList[100];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _weekly_guild_rank_result_no_data_zocl
{
  unsigned int dwVer;
  char byRace;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_result_fcandidacy_list_zocl
{
  #pragma pack(push, 1)
  struct __candi_info
  {
    char byGrade;
    long double dPvpPoint;
    unsigned int dwWinCnt;
    char wszAvatorName[17];
    char wszGuildName[17];
  };
  #pragma pack(pop)

  char byCnt;
  _pt_result_fcandidacy_list_zocl::__candi_info Candidacy[500];

  _pt_result_fcandidacy_list_zocl();
  int size() const;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _complete_vote_inform_zocl
{
  int nVoteSerial;
  unsigned __int16 wPoint[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _progress_vote_inform_zocl
{
  int nVoteSerial;
  unsigned __int16 wPoint[3];
  bool bHurry;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _boss_sms_cancel_result_zocl
{
  char byRetCode;
  unsigned int dwMsgID;
  char wszBossName[17];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4985)
struct _boss_sms_msg_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (4988)
struct _boss_sms_send_inform_zocl
{
  char byRemainCnt;
};

// moved from ZoneServer-GU.h (4991)
struct _boss_sms_refuse_inform_zocl
{
  char sDum;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _quest_return_item_after_quest_zocl
{
  char byQuestDBSlot;
  unsigned __int16 wSerial;
  char byNum;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7435)
struct _quest_complete_inform_zocl
{
  char byQuestDBSlot;
};

// moved from ZoneServer-GU.h (7436)
struct _quest_process_inform_zocl
{
  char byQuestDBSlot;
  char byActIndex;
  unsigned __int16 wCount;
};

// moved from ZoneServer-GU.h (7437)
struct _quest_fail_inform_zocl
{
  char byFailCode;
  char byQuestDBSlot;
};

// moved from ZoneServer-GU.h (7438)
struct _quest_give_up_result_zocl
{
  char byQuestDBSlot;
};

// moved from ZoneServer-GU.h (7439)
struct _quest_select_reward_inform_zocl
{
  char byQuestDBSlot;
};

// moved from ZoneServer-GU.h (5019)
struct _quest_no_complete_from_npc_zocl
{
  char byQuestDBSlot;
};

// moved from ZoneServer-GU.h (5020)
struct _quest_no_have_return_item_zocl
{
  char byQuestDBSlot;
};

// moved from ZoneServer-GU.h (5021)
struct _quest_no_have_give_item_zocl
{
  char byQuestDBSlot;
};

// moved from CMainThread.h
struct _holy_stone_hp_inform_zocl
{
  unsigned __int16 wHPRate[3];
};

// moved from ZoneServer-GU.h (5024)
struct _notify_holy_keeper_attack_time_be_keep_keeper_zocl
{
  bool bKeepKeeper;
};

// moved from ZoneServer-GU.h (5029)
struct _stone_exit_inform_zocl
{
  char byMasterRace;
};

// moved from ZoneServer-GU.h (5030)
struct _notice_end_quest_inform_zocl
{
  char byWinRace;
  char byLoseRace;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _keeper_enter_inform_zocl
{
  bool bNow;
  unsigned __int16 wTotalSecTime;
  __int16 zHurrySecTime;
  __int16 zExitSecTime;
  char byMasterRace;
  bool bChaos;
  char byAttackAbleType;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (5033)
struct _kepper_wait_inform_zocl
{
  char byWaitType;
  char byMasterRace;
};

// moved from ZoneServer-GU.h (5034)
struct _stone_wait_inform_zocl
{
  char sDum;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _stone_enter_inform_zocl
{
  bool bNow;
  unsigned __int16 wTotalSecTime;
  __int16 zHurrySecTime;
  __int16 zExitSecTime;
  unsigned __int16 wElapseTime;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notice_next_quest_inform_zocl
{
  unsigned __int16 wLeftSec;
  char byStoneMapMoveInfo;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (5036)
struct _notify_holy_stone_destroyed_zocl
{
  char byDummy;
};

// moved from ZoneServer-GU.h (5037)
struct _notice_start_quest_inform_zocl
{
  char sDum;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _darkhole_reward_message_inform_zocl
{
  int bIsRewarded;
  unsigned int dwPartyMemberIndex;
  char byTableCode;
  unsigned __int16 wItemIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trans_ship_ticket_pass_inform_zocl
{
  bool bPass;
  unsigned __int16 wTicketSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (5090)
struct _trans_ship_kick_inform_zocl
{
  char sDum;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trans_ship_state_inform_zocl
{
  bool bAnchor;
  bool bHurry;
  char byDirect;
  unsigned int dwPassSec;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_info_update_inform_zocl
{
  unsigned int dwGuildSerial;
  char byGrade;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  unsigned int dwTotWin;
  unsigned int dwTotDraw;
  unsigned int dwTotLose;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_leave_inform_zocl
{
  bool bSelf;
  bool bPunised;
  unsigned int dwMemberSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_vote_complete_zocl
{
  unsigned int dwMatterVoteSynKey;
  char byApprPoint;
  char byOppoPoint;
  bool bPassed;
};
#pragma pack(pop)

// moved from CMainThread.h
struct _guild_vote_state_zocl
{
  unsigned int dwMatterVoteSynKey;
  unsigned __int8 byApprPoint;
  unsigned __int8 byOppoPoint;
  unsigned __int8 byLoginSeniorNum;
  unsigned __int8 byTotalSeniorNum;
};

// moved from ZoneServer-GU.h (5102)
struct _guild_cancel_suggest_inform_zocl
{
  unsigned int dwMatterVoteSynKey;
};

// moved from ZoneServer-GU.h (5103)
struct _guild_vote_stop_zocl
{
  unsigned int dwMatterVoteSynKey;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_add_join_applier_inform_zocl
{
  unsigned int dwApplierSerial;
  char wszName[17];
  char byLv;
  unsigned int dwPvpPoint;
  unsigned int dwApplyTime;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_del_join_applier_inform_zocl
{
  unsigned int dwApplierSerial;
  char byDelCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_join_accept_inform_zocl
{
  unsigned int dwAccepterSerial;
  unsigned int dwApplierSerial;
  char wszName[17];
  char byClassInGuild;
  char byLv;
  unsigned int dwPvpPoint;
  char byGuildRank;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_io_money_inform_zocl
{
  unsigned int dwIOerSerial;
  char byKind;
  bool bInPut;
  long double dIODalant;
  long double dIOGold;
  long double dTotalDalant;
  long double dTotalGold;
  char byDate[4];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (5109)
struct _guild_pop_money_fail_zocl
{
  unsigned int dwIOerSerial;
};

// moved from ZoneServer-GU.h (5110)
struct _guild_disjoint_inform_zocl
{
  unsigned int dwGuildSerial;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_member_login_inform_zocl
{
  unsigned int dwSerial;
  unsigned __int16 wMapCode;
  char byRegionIndex;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (5112)
struct _guild_member_logoff_inform_zocl
{
  unsigned int dwSerial;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_member_pos_inform_zocl
{
  unsigned int dwSerial;
  unsigned __int16 wMapCode;
  char byRegionIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
struct _guild_battle_apply_build_battle_result_zocl
{
  unsigned __int8 byRet;
  char wszDestGuildName[17];
};

// moved from CMainThread.h
struct _guild_manage_committee_inform_zocl
{
  bool bAppoint;
  char wszName[17];
};

// moved from ZoneServer-GU.h (7487)
struct _guild_manage_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7462)
struct _guild_self_leave_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7499)
struct _guild_force_leave_boradori
{
  char sDum;
};

// moved from ZoneServer-GU.h (5116)
struct _guild_master_election_possible_zocl
{
  bool bPossible;
};

// moved from ZoneServer-GU.h (5117)
struct _guild_battle_propose_notify_zocl
{
  char wszGuildName[17];
};

// moved from ZoneServer-GU.h (5118)
struct _guild_battle_refuse_notify_zocl
{
  char wszGuildName[17];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _open_return_gate_inform_zocl
{
  unsigned __int16 wGateInx;
  unsigned int dwObjSerial;
  unsigned int dwOpenerSerial;
  char wszOpenerName[17];
  __int16 zPos[3];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (5121)
struct _enter_return_gate_result_zocl
{
  char byResult;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_recall_request_zocl
{
  unsigned __int16 wRequestID;
  char wszPerformerName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
struct _recall_request_result_zocl
{
  unsigned __int8 byRet;
  char wszDestName[17];
};

// moved from ZoneServer-GU.h (7055)
#pragma pack(push, 1)
struct _object_server_pos_result_zocl
{
  char byErrCode;
  char byObjKind;
  char byObjID;
  unsigned __int16 wObjIndex;
  unsigned int dwObjSerial;
  float fServerPos[3];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7190)
struct _object_real_fixpositon_zocl
{
  char byObjKind;
  char byObjID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _stun_inform_zocl
{
  _CHRID idStun;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7032)
struct _init_class_result_zocl
{
  char byRet;
};

// moved from ZoneServer-GU.h (7034)
struct _init_class_cost_result_zocl
{
  unsigned int m_dwCostGold;
};

// moved from ZoneServer-GU.h (7035)
struct _can_select_class_result_zocl
{
  char byRet;
  char byIsRealClassUp;
};

// moved from ZoneServer-GU.h (7371)
struct _change_class_command_zocl
{
  unsigned __int16 wCurClassIndex;
};

// moved from ZoneServer-GU.h (7372)
struct _select_class_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7004)
struct _trunk_create_cost_is_free_result_zocl
{
  char byRet;
};

// moved from ZoneServer-GU.h (7082)
struct _use_recover_lossexp_item_result_zocl
{
  char cRet;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _decide_recall_error_result_zocl
{
  char byErr;
  int nMapCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_fire_cracker_item_result_zocl
{
  char cRet;
  unsigned __int16 wSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_fire_cracker_item_inform_zocl
{
  unsigned int dwUserObjSerial;
  unsigned __int16 wItemIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_soccer_ball_item_result_zocl
{
  char byRet;
  unsigned __int16 wSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_soccer_ball_item_inform_zocl
{
  unsigned int dwUserObjSerial;
  unsigned __int16 wItemIndex;
  bool bTakeSoccerBall;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _break_stop_result_zocl
{
  char byObjID;
  unsigned int dwObjSerial;
  __int16 zCur[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _last_effect_change_inform_zocl
{
  char byObjID;
  unsigned int dwSerial;
  unsigned __int16 wLastContEffect;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _count_succ_inform_zocl
{
  char byActEffectCode;
  char byAtterID;
  unsigned int dwAtterSerial;
  char byDamerID;
  unsigned int dwDamerSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _robed_hp_inform_zocl
{
  _CHRID idDster;
  _CHRID idPerformer;
  unsigned __int16 wRobedHP;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _party_mode_monsterkillexp_inform_zocl
{
  float fExp;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _return_gate_fix_position_zocl
{
  unsigned __int16 wGateInx;
  unsigned int dwObjSerial;
  unsigned int dwOpenerSerial;
  char wszOpenerName[17];
  __int16 zPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _move_potal_result_zocl
{
  char byRet;
  char byMapIndex;
  float fStartPos[3];
  char byZoneCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _close_return_gate_inform_zocl
{
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _parkingunit_create_zocl
{
  unsigned __int16 wObjIndex;
  unsigned int dwObjSerial;
  char byCreateType;
  char byFrame;
  char byPart[6];
  unsigned int dwOwerSerial;
  __int16 zPos[3];
  char byTransDistCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _parkingunit_destroy_zocl
{
  unsigned __int16 wObjIndex;
  unsigned int dwObjSerial;
  char byDestroyCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _parkingunit_change_owner_zocl
{
  unsigned __int16 wObjIndex;
  unsigned int dwObjSerial;
  unsigned int dwOldOwnerSerial;
  unsigned int dwNewOwnerSerial;
  char byNewOwnerUnitSlotIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _parkingunit_fixpositon_zocl
{
  unsigned __int16 wObjIndex;
  unsigned int dwObjSerial;
  char byFrame;
  char byPart[6];
  __int16 zPos[3];
  unsigned int dwMasterSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _itembox_create_zocl
{
  char byItemTableCode;
  unsigned __int16 wItemRecIndex;
  char byAmount;
  unsigned __int16 wBoxIndex;
  unsigned int dwOwerSerial;
  _CHRID idDumber;
  char byState;
  __int16 zPos[3];
  char byThrowerRace;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _itembox_fixpositon_zocl
{
  char byItemTableCode;
  unsigned __int16 wItemRecIndex;
  char byAmount;
  unsigned __int16 wItemBoxIndex;
  unsigned int dwOwerSerial;
  __int16 zPos[3];
  char byState;
  char byThrowerRace;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _itembox_state_change_zocl
{
  unsigned __int16 wItemBoxIndex;
  unsigned int dwOwerSerial;
  char byState;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _gravity_stone_fix_position_zocl
{
  unsigned __int16 wObjIndex;
  unsigned __int16 wRecIndex;
  unsigned int dwObjSerial;
  __int16 zPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _create_gravity_stone_zocl
{
  unsigned __int16 wIndex;
  unsigned __int16 wRecIndex;
  unsigned int dwObjSerial;
  __int16 zPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _destroy_gravity_stone_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _class_skill_result_other_zocl
{
  char byRetCode;
  _CHRID idPerformer;
  _CHRID idDster;
  unsigned __int16 wSkillIndex;
  char byAttackSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_create_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
  char bYAngle;
  unsigned __int16 wStateInfo;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wLastEffectCode;
  __int16 zCur[3];
  char bYAngle;
  unsigned __int16 wStateInfo;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_real_move_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wLastEffectCode;
  __int16 zCur[3];
  __int16 zTar[2];
  unsigned __int16 wStateInfo;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_change_state_zocl
{
  unsigned int dwSerial;
  unsigned __int16 wMonsterState;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_change_rotate_zocl
{
  unsigned int dwSerial;
  char byAngle;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_change_target_player_zocl
{
  unsigned int dwSerial;
  unsigned int dwTargetSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_present_emotion_zocl
{
  unsigned int dwSerial;
  char byType;
  unsigned __int16 wIndex;
  unsigned __int16 wRIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  char byDestroyCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _monster_move_zocl
{
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trap_create_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
  unsigned int dwMasterSerial;
  char byRaceCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trap_destroy_zocl
{
  unsigned int dwSerial;
  char byDestroyCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trap_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
  unsigned __int16 wCompLeftSec;
  unsigned int dwMasterSerial;
  bool bTranspar;
  char byRaceCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trap_alter_transpar_inform_zocl
{
  unsigned int dwTrapSerial;
  bool bTranspar;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trap_complete_inform_zocl
{
  unsigned int dwTrapSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _npc_create_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _npc_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _npc_move_zocl
{
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
};
#pragma pack(pop)

// moved from CMainThread.h
struct _npc_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wLastEffectCode;
  __int16 zCur[3];
};

// moved from CMainThread.h
struct _npc_real_move_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wLastEffectCode;
  __int16 zCur[3];
  __int16 zTar[2];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _stone_create_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
  char byMasterRace;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _stone_destroy_zocl
{
  unsigned int dwSerial;
  char byDestroyCode;
  unsigned int dwDestroyerSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _stone_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
  char byMasterRace;
  bool bOper;
  bool bChip;
  int nControlLeftSec;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _holystone_alter_oper_circle_inform_zocl
{
  unsigned int dwSerial;
  bool bOper;
  unsigned __int16 wCurHPRate;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _keeper_create_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _keeper_destroy_zocl
{
  unsigned int dwSerial;
  char byDestroyCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _keeper_move_zocl
{
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
  char byMoveType;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _keeper_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _keeper_real_move_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _tower_create_zocl
{
  unsigned __int16 wIndex;
  unsigned __int16 wRecIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
  unsigned int dwMasterSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _tower_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  char byDestroyCode;
  unsigned int dwMasterSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
struct _tower_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
  unsigned __int16 wCompLeftSec;
  unsigned int dwMasterSerial;
};

// moved from CMainThread.h
struct _tower_complete_inform_zocl
{
  unsigned int dwTowerObjSerial;
  unsigned int dwMasterSerial;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_tower_inform_zocl
{
  unsigned int dwAtterSerial;
  char byAttackPart;
  bool bCritical;
  char byDstID;
  unsigned int dwDstSerial;
  unsigned __int16 wDamage;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _gravity_stone_regener_fix_position_zocl
{
  int iPortalInx;
  char byState;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _gravity_stone_regener_inform_zocl
{
  int iPortalInx;
  char byState;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _create_circle_zone_stone_zocl
{
  int iPortalInx;
  char byColor;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _circle_zone_fix_position_zocl
{
  int iPortalInx;
  char byColor;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _circle_zone_goal_inform_zocl
{
  int iPortalInx;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_create_zocl
{
  unsigned __int16 wIndex;
  unsigned __int16 wRecIndex;
  unsigned int dwSerial;
  __int16 zPos[3];
  char byLv;
  unsigned int dwMasterSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  char byDestroyCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_move_zocl
{
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
  float fSpeed;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_fixpositon_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
  char byLv;
  unsigned __int16 wLastEffectCode;
  unsigned int dwMasterSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_real_move_zocl
{
  unsigned __int16 wRecIndex;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
  char byLv;
  unsigned __int16 wLastEffectCode;
  unsigned int dwMasterSerial;
  float fSpeed;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_lvup_inform_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  char byLv;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_act_heal_inform_zocl
{
  unsigned int dwAnimusSerial;
  unsigned int dwDstSerial;
  unsigned __int16 wAddHP;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _current_billing_type_inform_zocl
{
  __int16 iType;
  char byPaymentType;
  int lRemainMin;
  _SYSTEMTIME stEndDate;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _other_new_view_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  __int16 zPos[3];
  char byRaceCode;
  char byViewType;
  unsigned __int64 dwStateFlag;
  unsigned __int64 dwStateFlagEx;
  unsigned __int16 wLastEffectCode;
  char byColor;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_test_result_zocl
{
  unsigned int dwAtterSerial;
  unsigned __int16 wBulletIndex;
  char byEffectCode;
  char byEffectIndex;
  char byEffectLv;
  char byWeaponPart;
  __int16 zTar[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _shield_equip_dur_dec_zocl
{
  char byPartIndex;
  unsigned __int16 wSerial;
  unsigned __int16 wLeftDurPoint;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7264)
struct _attack_error_result_zocl
{
  char sFailureCode;
};

// moved from ZoneServer-GU.h (2218)
struct _inform_revival_jade_effect_zocl
{
  char dummy[1];
};

// moved from ZoneServer-GU.h (2249)
struct _uilock_init_result_zocl
{
  char byRet;
  char byUILock_HintIndex;
};

// moved from ZoneServer-GU.h (2549)
struct _uilock_user_certify_result_zocl
{
  char byRet;
  char byFailCount;
};

// moved from ZoneServer-GU.h (2725)
struct _uilock_update_info_result_zocl
{
  char byRet;
};

// moved from ZoneServer-GU.h (3119)
struct _uilock_find_pw_result_zocl
{
  char byRet;
  char byFindPassFailCount;
  char uszUILockPW[13];
};

// moved from ZoneServer-GU.h (2396)
struct _uilock_inform_request_zocl
{
  char byInformType;
  char byFailCount;
  char byHintIndex;
  char byFindPassFailCount;
};

// moved from ZoneServer-GU.h (3105)
struct _storage_refresh_inform_zocl
{
  char byStorageCode;
  char byItemNum;
  unsigned __int16 wSerial[100];
};

// moved from ZoneServer-GU.h (7337)
struct _force_inven_change_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (4451)
struct _notify_max_pvp_point_zocl
{
  int nMaxPoint;
};

// moved from ZoneServer-GU.h (7511)
struct _newmovepotion_stone_result_zocl
{
  bool bStone;
};

// moved from ZoneServer-GU.h (4452)
struct _notify_jade_effect_error_zocl
{
  char byErrorCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _other_unit_ride_change_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  bool bTake;
  unsigned int dwUnitSerial;
  __int16 zNewPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _other_shape_failure_zocl
{
  unsigned __int16 wOtherIndex;
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  char byState;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_die_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_fixpositon_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  char byRaceCode;
  __int16 zCur[3];
  unsigned __int16 wLastEffectCode;
  unsigned __int64 dwStateFlag;
  unsigned __int64 dwStateFlagEx;
  char byColor;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_real_move_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 dwEquipVer;
  char byRaceCode;
  __int16 zCur[3];
  __int16 zTar[2];
  unsigned __int16 wLastEffectCode;
  unsigned __int64 dwStateFlag;
  unsigned __int64 dwStateFlagEx;
  __int16 nAddSpeed;
  char byDirect;
  char byColor;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_move_zocl
{
  unsigned int dwSerial;
  __int16 zCur[3];
  __int16 zTar[2];
  __int16 nAddSpeed;
  char byDirect;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_revival_zocl
{
  char byRet;
  unsigned __int16 wStartMapIndex;
  __int16 zPos[3];
  char byLevel;
  unsigned __int16 wCurHP;
  unsigned __int16 wCurFP;
  unsigned __int16 wCurST;
  char byZoneCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_resurrect_zocl
{
  char byRet;
  char byLevel;
  unsigned __int16 wCurHP;
  unsigned __int16 wCurFP;
  unsigned __int16 wCurST;
  bool bQuickPotion;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_stop_zocl
{
  unsigned int dwSerial;
  __int16 zCur[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _move_error_result_zocl
{
  char byErrCode;
  float fCur[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _level_up_zocl
{
  unsigned int dwSerial;
  char byLevel;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7280)
struct _max_hfsp_zocl
{
  unsigned __int16 wMaxHP;
  unsigned __int16 wMaxFP;
  unsigned __int16 wMaxST;
};

// moved from ZoneServer-GU.h (7281)
struct _recover_zocl
{
  unsigned __int16 wHP;
  unsigned __int16 wFP;
  unsigned __int16 wST;
  unsigned __int16 wDP;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_booster_charge_inform_zocl
{
  char bySlotIndex;
  unsigned __int16 wBoosterGauge;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _other_map_out_zocl
{
  char byMapOutType;
  unsigned int dwSerial;
  char byNextMap;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _state_inform_zocl
{
  unsigned int dwSerial;
  unsigned __int64 dwState;
};
#pragma pack(pop)

// moved from ZoneServer_Regular_1_0_x64.h (8520)
#pragma pack(push, 1)
struct _state_inform_ex_zocl
{
  unsigned int dwSerial;
  unsigned __int64 dwStateEx;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _goto_avator_result_zocl
{
  char byRet;
  char byMapCode;
  float fStartPos[3];
  char byMapInType;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7286)
struct _goto_baseportal_result_zocl
{
  char byRet;
  char byMapCode;
  __int16 zPos[3];
};

// moved from ZoneServer-GU.h (7288)
struct _exit_world_result_zocl
{
  char dummy[1];
};

// moved from ZoneServer-GU.h (7335)
struct _new_pos_start_result_zocl
{
  char byMapInMode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _stat_inform_zocl
{
  char byStatIndex;
  unsigned int dwNewStat;
  char byReason;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _fcitem_inform_zocl
{
  unsigned __int16 wItemSerial;
  unsigned int dwNewStat;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _exchange_money_result_zocl
{
  char byErrCode;
  unsigned int dwLeftGold;
  unsigned int dwLeftDalant;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _economy_history_inform_zocl
{
  unsigned __int16 wEconomyGuide[3][12];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _economy_rate_inform_zocl
{
  bool bStart;
  float fPayExgRate;
  float fTexRate;
  unsigned __int16 wMgrValue;
  unsigned __int16 wEconomyGuide[3];
  float fOreSellRate;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_potion_result_zocl
{
  char byErrCode;
  unsigned __int16 wPotionSerial;
  unsigned __int16 wHP;
  unsigned __int16 wFP;
  unsigned __int16 wSP;
  char byLeftNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_potion_result_other_zocl
{
  char byRetCode;
  unsigned __int16 wPotionIndex;
  _CHRID idPerformer;
  _CHRID idDster;
  unsigned __int16 wHP;
  unsigned __int16 wFP;
  unsigned __int16 wSP;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _potionsocket_separation_result_zocl
{
  char sErrorCode;
  unsigned __int16 wParentSerial;
  char byParentAmount;
  unsigned __int16 wChildSerial;
  char byChildAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _potionsocket_division_result_zocl
{
  char sErrorCode;
  unsigned __int16 wSerial;
  char byParentAmount;
  unsigned __int16 wChildSerial;
  char byChildAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _linkboard_download_result_zocl
{
  unsigned __int16 wLinkBoard[50];
  char byLinkLock;
  unsigned int dwSkill[2];
  unsigned int dwForce[2];
  unsigned int dwCharacter[2];
  unsigned int dwAnimus[2];
  unsigned int dwInven;
  unsigned int dwInvenWindow[5];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _gesture_inform_zocl
{
  unsigned int dwActorSerial;
  char byGestureType;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_frame_buy_result_zocl
{
  char byRetCode;
  char byFrameCode;
  char byAddUnitSlot;
  char byKeyIndex;
  unsigned __int16 wKeySerial;
  unsigned int dwLeftMoney[7];
  unsigned int dwConsumMoney[7];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_frame_repair_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  unsigned int dwNewGauge;
  unsigned int dwConsumDalant;
  unsigned int dwLeftDalant;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_bullet_fill_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  unsigned __int16 wBulletIndex[2];
  unsigned int dwComsumMoney[7];
  unsigned int dwLeftMoney[7];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_delivery_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  char byTransDistCode;
  unsigned int dwParkingUnitSerial;
  unsigned int dwPayDalant;
  unsigned int dwLeftDalant;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_return_result_zocl
{
  char byRetCode;
  unsigned int dwPayDalant;
  unsigned int dwNewDalant;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7300)
struct _equip_part_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7301)
struct _embellish_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7302)
struct _off_part_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7303)
struct _make_item_cheatcode_setui_zocl
{
  bool bEnableMakeUiButton;
};

// moved from ZoneServer-GU.h (7304)
struct _make_item_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7305)
struct _exchange_item_result_zocl
{
  char byErrCode;
  _combine_item_result_zocl::__item NewItem;
};

// moved from ZoneServer-GU.h (7306)
struct _upgrade_item_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7307)
struct _downgrade_item_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7341)
struct _add_bag_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7320)
struct _unit_take_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7321)
struct _unit_leave_result_zocl
{
  char byRetCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_alter_fee_inform_zocl
{
  char bySlotIndex;
  unsigned int dwPullingFee;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_force_return_inform_zocl
{
  char bySlotIndex;
  unsigned int dwDebt;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7323)
struct _unit_bullet_replace_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7324)
struct _unit_destroy_inform_zocl
{
  char bySlotIndex;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alter_unit_hp_inform_zocl
{
  char bySlotIndex;
  unsigned int dwLeftHP;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_recall_result_zocl
{
  char byResultCode;
  unsigned int dwAnimusSerial;
  unsigned __int16 wAnimusHP;
  unsigned __int16 wAnimusFP;
  unsigned __int64 dwAnimusExp;
  unsigned __int16 wLeftFP;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_return_result_zocl
{
  char byResultCode;
  unsigned __int16 wAnimusItemSerial;
  char byReturnType;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7329)
struct _animus_target_result_zocl
{
  char byResultCode;
};

// moved from ZoneServer-GU.h (7330)
struct _animus_hp_inform_zocl
{
  unsigned __int16 wAnimusItemSerial;
  unsigned __int16 wLeftHP;
};

// moved from ZoneServer-GU.h (7331)
struct _animus_fp_inform_zocl
{
  unsigned __int16 wAnimusItemSerial;
  unsigned __int16 wLeftFP;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_exp_inform_zocl
{
  unsigned __int16 wAnimusItemSerial;
  unsigned __int64 dwExp;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7333)
struct _animus_command_inform_zocl
{
  char byStateCode;
};

// moved from ZoneServer-GU.h (7334)
struct _animus_recall_wait_time_free_inform_zocl
{
  bool bFree;
};

// moved from ZoneServer-GU.h (7338)
struct _animus_inven_change_result_zocl
{
  char byErrCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _breakdown_equip_item_zocl
{
  unsigned __int16 wPlayerIndex;
  unsigned int dwPlayerSerial;
  unsigned __int16 dwEquipVer;
  char byPartIndex;
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _res_separation_result_zocl
{
  char byErrCode;
  unsigned __int16 wParentSerial;
  char byParentAmount;
  unsigned __int16 wChildSerial;
  char byChildAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _res_division_result_zocl
{
  char byErrCode;
  unsigned __int16 wParentSerial;
  char byParentAmount;
  unsigned __int16 wChildSerial;
  char byChildAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _delete_storage_inform_zocl
{
  char byStorageCode;
  unsigned __int16 wSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _adjust_amount_inform_zocl
{
  char byStorageCode;
  unsigned __int16 wSerial;
  unsigned int dwNewDur;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _mine_complete_result_zocl
{
  char byErrCode;
  unsigned __int16 wEquipLeftDur;
  unsigned __int16 wBatteryLeftDur;
  char byOreIndex;
  unsigned __int16 wOreSerial;
  char byOreDur;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7345)
struct _mine_start_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7346)
struct _mine_cancle_result_zocl
{
  char sDum;
};

// moved from ZoneServer-GU.h (7283)
struct _throw_storage_result_zocl
{
  char byErrCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _ore_into_bag_result_zocl
{
  char byErrCode;
  unsigned __int16 wNewSerial;
  char byCsMethod;
  unsigned int dwT;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _cutting_complete_result_zocl
{
  unsigned int dwLeftGold;
  char byRet;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _party_leave_self_result_zocl
{
  unsigned int dwExiterSerial;
  bool bWorldExit;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _take_loot_item_inform_zocl
{
  unsigned int dwTakerSerial;
  char byItemTableCode;
  unsigned __int16 wItemIndex;
  char byNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alter_item_dur_zocl
{
  char byStorageCode;
  unsigned __int16 wItemSerial;
  unsigned __int64 dwDur;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _change_class_other_zocl
{
  unsigned int dwPlayerSerial;
  unsigned __int16 wClassIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _reward_add_item_zocl
{
  char byTableCode;
  unsigned __int16 wItemIndex;
  unsigned __int64 dwDur;
  unsigned int dwLv;
  unsigned __int16 wItemSerial;
  char byReason;
  char byCsMethod;
  unsigned int dwT;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alter_unit_bullet_inform_zocl
{
  char byPart;
  unsigned __int16 wLeftNum;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7375)
struct _alter_weapon_bullet_inform_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int16 wLeftNum;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alter_pvp_cash_zocl
{
  long double dNewPoint;
  char byIOCode;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7377)
struct _alter_pvp_point_zocl
{
  long double dNewPoint;
};

// moved from ZoneServer-GU.h (4469)
struct _alter_pvp_temp_cash_zocl
{
  long double dNewTempPoint;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alter_ranking_inform_zocl
{
  unsigned __int16 wRankRate;
  unsigned int dwRank;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alter_money_inform_zocl
{
  char byReason;
  unsigned int dwGold;
  unsigned int dwDalant;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _add_holyquest_actcum_inform_zocl
{
  int nPvpPoint;
  unsigned __int16 wKillPoint;
  char byHolyMentalCount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _force_result_one_zocl
{
  char byErrCode;
  unsigned int dwItemCum;
  char byAttackSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _insert_item_inform_zocl
{
  char byStorageCode;
  char byTableCode;
  unsigned __int16 wItemIndex;
  unsigned int dwDurPoint;
  unsigned __int16 wSerial;
  unsigned int dwLv;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _insert_quest_item_inform_zocl
{
  char byTableCode;
  unsigned __int16 wItemIndex;
  unsigned int dwDurPoint;
  unsigned int dwLv;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _make_tower_result_zocl
{
  char byErrCode;
  unsigned int dwTowerObjSerial;
  unsigned __int16 wLeftFP;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _back_tower_result_zocl
{
  char byErrCode;
  unsigned __int16 wItemSerial;
  unsigned __int16 wLeftHP;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _effect_add_inform_zocl
{
  char byLv;
  unsigned __int16 wEffectCode;
  unsigned __int16 wDurSec;
  unsigned int dwPlayerSerial;
  char wszPlayerName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _effect_remove_inform_zocl
{
  unsigned __int16 wEffectCode;
  unsigned int dwPlayerSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7407)
struct _made_trap_num_inform_zocl
{
  char byNum;
};

// moved from ZoneServer-GU.h (7408)
#pragma pack(push, 1)
struct _continue_tower_inform
{
  unsigned __int16 wItemSerial;
  unsigned __int16 wTwrRecIndex;
  unsigned __int16 wTwrIndex;
  unsigned int dwTwrSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7409)
struct _back_trap_result_zocl
{
  char byErrCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _make_trap_result_zocl
{
  char byErrCode;
  unsigned int dwTrapObjSerial;
  unsigned __int16 wLeftFP;
};
#pragma pack(pop)

// AOP uses the `_clzo` suffix for this zone->client packet name.
#pragma pack(push, 1)
struct _d_trade_ask_result_clzo
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_ask_inform_zocl
{
  _CLID idAsker;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_answer_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_start_inform_zocl
{
  _CLID idAsker;
  char byAskerEmptyNum;
  _CLID idAnswer;
  char byAnswerEmptyNum;
  unsigned int dwKey[4];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_cancle_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_cancle_inform_zocl
{
  char dummy[1];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_lock_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_lock_inform_zocl
{
  char dummy[1];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_add_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_add_inform_zocl
{
  char bySlotIndex;
  char byTableCode;
  unsigned __int16 wItemIndex;
  unsigned __int64 dwDurPoint;
  unsigned int dwUdtInfo;
  char byAmount;
  char byEmptyInvenNum;
  char byCsMethod;
  unsigned int dwT;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_del_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_del_inform_zocl
{
  char bySlotIndex;
  char byEmptyInvenNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_bet_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_bet_inform_zocl
{
  char byMoneyUnit;
  unsigned int dwBetAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_ok_result_zocl
{
  char byErrCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_ok_inform_zocl
{
  char dummy[1];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_accomplish_inform_zocl
{
  unsigned int dwDalant;
  unsigned int dwGold;
  unsigned __int16 wStartSerial;
  bool bSucc;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_close_inform_zocl
{
  char byCloseCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_unit_info_inform_zocl
{
  char byTradeSlotIndex;
  char byFrame;
  unsigned int dwGauge;
  char byPart[6];
  unsigned int dwBullet[2];
  unsigned int dwSpare[8];
  int nDebtFee;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_unit_add_inform_zocl
{
  unsigned __int16 wUnitKeySerial;
  char bySlotIndex;
  char byFrame;
  unsigned int dwGauge;
  char byPart[6];
  unsigned int dwBullet[2];
  unsigned int dwSpare[8];
  int nPullingFee;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _select_waited_quest_command_zocl
{
  char byEventType;
  unsigned int dwEventIndex;
  char byEventNodeIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _insert_quest_failure_zocl
{
  char byEventType;
  unsigned int dwEventIndex;
  char byEventNodeIndex;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7389)
struct _holyquest_succ_inform_zocl
{
  char byQuestCode;
  bool bSucc;
};

// moved from ZoneServer-GU.h (7390)
struct _npc_quest_result_zocl
{
  bool bSuccess;
};

// moved from ZoneServer-GU.h (7391)
struct _npc_quest_history_inform_zocl
{
  char byIndex;
  char szQuestCode[8];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _recv_hsk_quest_inform_zocl
{
  char byHSKQuestCode;
  int nPvpPoint;
  unsigned __int16 wKillPoint;
  char byHolyMentalCount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _set_target_object_result_zocl
{
  char byRetCode;
  char byKind;
  char byID;
  unsigned int dwSerial;
  unsigned __int16 wHPRate;
  char byGrade;
  bool bForce;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _refresh_group_target_position_zocl
{
  char byGroupType;
  char byMapCode;
  char byID;
  unsigned int dwSerial;
  float fPos[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _cast_vote_result_zocl
{
  char byRetCode;
  int nVoteSerial;
  unsigned __int16 wPoint[3];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7448)
struct _propose_vote_result_zocl
{
  char byRetCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_join_apply_result_zocl
{
  char byRetCode;
  unsigned int dwRemainingTime;
  unsigned int dwGuildSerial;
  char wszGuildName[17];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7461)
struct _guild_join_apply_cancel_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7463)
struct _guild_offer_suggest_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7464)
struct _guild_cancel_suggest_result_zocl
{
  char byRetCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_vote_result_zocl
{
  unsigned int dwMatterVoteSynKey;
  char byRetCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_join_accept_fail_zocl
{
  char byRetCode;
  unsigned int dwApplierSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7467)
struct _guild_join_apply_reject_inform_zocl
{
  char sDum;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_push_money_result_zocl
{
  char byRetCode;
  unsigned int dwLeftDalant;
  unsigned int dwLeftGold;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7459)
struct _guild_establish_fail_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7498)
struct _guild_honor_set_result_zocl
{
  char byRetCode;
};

// moved from CMainThread.h
struct _group_target_inform_zocl
{
  char byGroupType;
  char wszTargetName[65];
};

// moved from CMainThread.h
struct _guildroom_rent_result_zocl
{
  char byRetCode;
  char bySubRetCode;
  char byRoomType;
};

// moved from ZoneServer-GU.h (2221)
struct _guildroom_rented_inform_zocl
{
  char byRoomType;
};

// moved from CMainThread.h
struct _alter_tower_hp_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int16 wLeftHP;
};

// moved from CMainThread.h
struct _alter_equip_sp_inform_zocl
{
  float fEquipUseSP;
};

// moved from ZoneServer-GU.h (7379)
struct _alter_sp_inform_zocl
{
  unsigned __int16 wNewSP;
};

// moved from ZoneServer-GU.h (7381)
struct _alter_hp_inform_zocl
{
  unsigned __int16 wHP;
};

// moved from ZoneServer-GU.h (7382)
struct _alter_grade_inform_zocl
{
  char byGrade;
};

// moved from ZoneServer-GU.h (7385)
struct _alter_tol_inform_zocl
{
  __int16 zTol[4];
};

// moved from ZoneServer-GU.h (7386)
struct _race_top_inform_zocl
{
  bool bTop;
};

// moved from ZoneServer-GU.h (7441)
struct _pvp_rank_list_version_up_zocl
{
  char byVersion;
};

// moved from ZoneServer-GU.h (7395)
struct _set_fp_inform_zocl
{
  unsigned __int16 wFP;
};

// moved from ZoneServer-GU.h (7396)
struct _set_hp_inform_zocl
{
  unsigned __int16 wHP;
};

// moved from ZoneServer-GU.h (7397)
struct _set_sp_inform_zocl
{
  unsigned __int16 wSP;
};

// moved from CMainThread.h
struct _set_dp_inform_zocl
{
  unsigned __int16 wDP;
};

// moved from CMainThread.h
struct _max_dp_zocl
{
  unsigned __int16 wMaxDP;
};

// moved from CMainThread.h
struct _buddy_off_reged_player_inform_zocl
{
  unsigned int dwSerial;
};

// moved from CMainThread.h
struct _buddy_add_fail_zocl
{
  char byRetCode;
  char wszDstName[17];
};

// moved from ZoneServer-GU.h (7502)
struct _notice_buddha_event_msg_zocl
{
  char byErrorCode;
};

// moved from ZoneServer-GU.h (7504)
struct _character_rename_cash_result_zocl
{
  bool bChange;
  char byErrorCode;
};

// moved from ZoneServer-GU.h (7512)
struct _notify_buy_cashitem_mode
{
  char byMode;
};

// moved from ZoneServer-GU.h (7291)
struct _town_or_field_inform_zocl
{
  char byTown;
};

// moved from ZoneServer-GU.h (7292)
struct _alter_region_inform_zocl
{
  unsigned __int16 wRegionIndex;
};

// moved from ZoneServer-GU.h (7506)
struct _mine_ore_deposit_inform_zocl
{
  char byDepositRate;
};

// moved from ZoneServer-GU.h (7507)
struct _ore_carry_forword_count_zocl
{
  char byOreTransferCount;
};

// moved from ZoneServer-GU.h (7508)
struct _pc_room_charclass_info_zocl
{
  unsigned int dwPcRoomIndex;
};

// moved from ZoneServer-GU.h (7509)
struct _pc_room_favor_item_error_zocl
{
  char byErrorCode;
};

// moved from CMainThread.h
struct _party_join_invitation_question_zocl
{
  _CLID idBoss;
};

// moved from CMainThread.h
struct _party_join_application_question_zocl
{
  _CLID idApplicant;
};

// moved from ZoneServer-GU.h (7354)
struct _party_join_fail_level_inform_zocl
{
  char sDum;
};

// moved from CMainThread.h
struct _party_join_member_result_zocl
{
  unsigned int dwJoinerSerial;
  char wszJoinerName[17];
  char byLootShareMode;
  unsigned __int16 wIndex;
};

// moved from CMainThread.h
struct _party_leave_compulsion_result_zocl
{
  unsigned int dwExiterSerial;
};

// moved from ZoneServer-GU.h (7357)
struct _party_disjoint_result_zocl
{
  char bySuccess;
};

// moved from ZoneServer-GU.h (7358)
struct _party_succession_result_zocl
{
  unsigned int dwSuccessorSerial;
};

// moved from ZoneServer-GU.h (7359)
struct _party_lock_result_zocl
{
  char byLock;
};

// moved from ZoneServer-GU.h (7360)
#pragma pack(push, 1)
struct _party_member_hp_upd
{
  unsigned int dwMemSerial;
  unsigned __int16 wHPRate;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7361)
#pragma pack(push, 1)
struct _party_member_fp_upd
{
  unsigned int dwMemSerial;
  unsigned __int16 wFPRate;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7362)
#pragma pack(push, 1)
struct _party_member_sp_upd
{
  unsigned int dwMemSerial;
  unsigned __int16 wSPRate;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7363)
#pragma pack(push, 1)
struct _party_member_lv_upd
{
  unsigned int dwMemSerial;
  char byLv;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7365)
#pragma pack(push, 1)
struct _party_member_max_hfsp_upd
{
  unsigned int dwMemSerial;
  unsigned __int16 wMaxHP;
  unsigned __int16 wMaxFP;
  unsigned __int16 wMaxSP;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7366)
#pragma pack(push, 1)
struct _party_member_effect_upd
{
  unsigned int dwMemSerial;
  char byAlterCode;
  unsigned __int16 wEffectCode;
  char byEffectLv;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7369)
struct _exp_alter_zocl
{
  unsigned int dwExpRate;
};

// moved from ZoneServer-GU.h (7367)
struct _alter_party_loot_share_result_zocl
{
  char byLootShareMode;
};

// moved from CMainThread.h
struct _away_party_join_invitation_question_zocl
{
  _CLID idBoss;
  char wszCharName[17];
};

// moved from ZoneServer-GU.h (7495)
struct _away_party_invitation_result_zocl
{
  char byRetCode;
};

// moved from ZoneServer-GU.h (7494)
struct _notify_raceboss_cry_msg_request_zocl
{
  char wszCryMsg[10][65];
};

// moved from CMainThread.h
struct _guild_master_info_zocl
{
  char byState;
  char byGrade;
  char byEffectValue[4];
};

// moved from CMainThread.h
struct _set_group_target_object_result_zocl
{
  char byRetCode;
  char byGroupType;
  char byMapCode;
  char byID;
  unsigned int dwSerial;
  float fPos[3];
};

// moved from ZoneServer-GU.h (7445)
struct _release_group_target_object_request_zocl
{
  char byGroupType;
};

// moved from CMainThread.h
struct _set_group_map_point_result_zocl
{
  char byRetCode;
  char byGroupType;
  char byMapCode;
  char byRemain;
  float zPos[2];
};

// moved from CMainThread.h
struct _buddy_download_result_zocl
{
  unsigned __int16 wDataSize;
  char sData[10000];
};

// moved from CMainThread.h
struct _unit_sell_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  unsigned __int16 wKeySerial;
  unsigned int dwNonPayDalant;
  int nAddMoney[7];
  unsigned int dwLeftMoney[7];
};

// moved from CMainThread.h
struct _unit_part_tuning_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  char byPart[6];
  unsigned int dwBullet[2];
  int nCost[7];
  unsigned int dwLeftMoney[7];
};

// moved from CMainThread.h
struct _skill_result_one_zocl
{
  char byErrCode;
  char byAttackSerial;
};

// moved from CMainThread.h
struct _skill_result_other_zocl
{
  char byRetCode;
  _CHRID idPerformer;
  _CHRID idDster;
  char bySkillIndex;
  char bySkillLv;
  char byAttackSerial;
};

// moved from CMainThread.h
struct _class_skill_result_one_zocl
{
  char byErrCode;
  char byAttackSerial;
};

// moved from CMainThread.h
struct _force_result_other_zocl
{
  char byRetCode;
  char byForceIndex;
  char byForceLv;
  _CHRID idPerformer;
  _CHRID idDster;
  char byAttackSerial;
};

// moved from CMainThread.h
struct _potion_delay_time_information_zocl
{
  int nMaxNum;
  unsigned int dwPotionDelayTime[38];
};

// moved from CMainThread.h
struct _guild_list_result_zocl
{
  struct __list
  {
    char byGrade;
    char wszGuildName[17];
    char wszMasterName[17];
  };

  char byPage;
  char byMaxPage;
  char byListCnt;
  _guild_list_result_zocl::__list GuildList[4];
};

// moved from CMainThread.h
struct _throw_skill_result_other_zocl
{
  char byRetCode;
  _CHRID idPerformer;
  char bySkillIndex;
  char byAttackSerial;
  _CHRID idDster;
  char byEffectedNum;
  _throw_skill_result_one_zocl::__effected_list list[30];
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_join_other_inform_zocl
{
  unsigned int dwAvatorSerial;
  unsigned int dwGuildSerial;
  unsigned __int16 wVisualVersion;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_notify_gravity_stone_owner_die_zocl
{
  char byObjID;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _transform_siege_mode_other_inform_zocl
{
  unsigned int dwAvatorSerial;
  unsigned __int16 wWeaponItemIndex;
  char bySiegeItemIndex;
  unsigned __int16 wVisualVer;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7474)
struct _transform_siege_mode_result_zocl
{
  char byErrCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _release_siege_mode_other_inform_zocl
{
  unsigned int dwAvatorSerial;
  unsigned __int16 wVisualVer;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _remaintime_inform_zocl
{
  __int16 iType;
  char byPaymentType;
  int lRemainMin;
  _SYSTEMTIME stEndDate;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _change_billing_type_inform_zocl
{
  __int16 iCurrentType;
  __int16 iChangeType;
  int lRemainMin;
  _SYSTEMTIME stEndDate;
  char byReason;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _billing_expire_inform_zocl
{
  char byKind;
  unsigned __int16 wWaitSec;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_in_reged_player_inform_zocl
{
  unsigned int dwSerial;
  char byMapIndex;
  char byPositionCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_add_ask_zocl
{
  unsigned __int16 wAskerIndex;
  unsigned int dwAskerSerial;
  char wszAskerName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_add_result_zocl
{
  char byRetCode;
  bool bAccept;
  unsigned int dwAskerSerial;
  unsigned __int16 wAdderIndex;
  unsigned int dwAdderSerial;
  char wszAdderName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_pos_inform_zocl
{
  unsigned int dwSerial;
  char byMapIndex;
  char byPositionCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _map_env_code_inform_zocl
{
  char byMapCode;
  unsigned int dwEnvCode;
  int long_time;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _use_radar_result_zocl
{
  char byErrCode;
  unsigned __int16 wRadarSerial;
  unsigned int dwDelay;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7489)
struct _radar_delay_inform_zocl
{
  unsigned int dwDelay;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _post_delete_reply_zocl
{
  unsigned int dwPostSerial;
  char byErrCode;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (7475)
struct _release_siege_mode_result_zocl
{
  char byErrCode;
};

// moved from ZoneServer-GU.h (7491)
struct _post_itemgold_reply_zocl
{
  char byErrCode;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _post_return_confirm_result_zocl
{
  char byErrCode;
  unsigned int dwPostSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _notify_race_battle_penelty_zocl
{
  char byLoseType;
  int nAlterPoint;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_honorguild_mark_zocl
{
  char byHonorRank;
  unsigned int dwSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _teleport_error_result_zocl
{
  char byErrorCode;
  unsigned int dwMapIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_renewal_zocl
{
  unsigned int dwSerial;
  char wszBuddyName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _lend_item_time_expired_zocl
{
  char byStorageCode;
  unsigned __int16 wSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h (4454)
struct _notify_excepted_from_raceranking
{
  int bExcepted;
};

// moved from CMainThread.h
#pragma pack(push, 1)
struct _start_trade_inform_zocl
{
  char s;
};
#pragma pack(pop)

// moved from AutoMineMachineMng.h
struct _pt_automine_charge_money_db_update_fail_zocl
{
  int nCharge;

  int size();
};

// moved from CHonorGuild.h
#pragma pack(push, 1)
struct _guild_honor_list_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    unsigned int dwGuildSerial;
    unsigned int dwEmblemBack;
    unsigned int dwEmblemMark;
    char wszGuildName[17];
    char wszMasterName[17];
    unsigned __int8 byTaxRate;
  };
  #pragma pack(pop)

  unsigned __int8 byListNum;
  unsigned __int8 byUI;
  __list GuildList[5];

  _guild_honor_list_result_zocl();
  int size() const;
};
#pragma pack(pop)

// moved from CNuclearBombMgr.h
#pragma pack(push, 1)
struct _nuclear_bomb_current_state_zocl
{
  #pragma pack(push, 1)
  struct __nuclear
  {
    unsigned __int8 byRaceCode;
    unsigned __int8 byUseClass;
    float zPos[3];
  };
  #pragma pack(pop)

  unsigned __int8 nNum;
  __nuclear nuclear[9];

  int size() const;
};
#pragma pack(pop)

// moved from CPlayer.h
#pragma pack(push, 1)
struct _target_monster_contsf_allinform_zocl
{
  #pragma pack(push, 1)
  struct _moncontsf
  {
    unsigned __int16 wSfcode;
  };
  #pragma pack(pop)

  unsigned int dwSerial;
  unsigned __int8 byContCount;
  _moncontsf m_MonContSf[8];

  _target_monster_contsf_allinform_zocl();
  void Init();
};
#pragma pack(pop)

// moved from CPlayer.h
#pragma pack(push, 1)
struct _target_player_damage_contsf_allinform_zocl
{
  #pragma pack(push, 1)
  struct _playercontsf
  {
    unsigned __int16 wSfcode;
    unsigned __int8 byContCount;
  };
  #pragma pack(pop)

  unsigned int dwSerial;
  unsigned __int8 byContCount;
  _playercontsf m_PlayerContSf[8];

  _target_player_damage_contsf_allinform_zocl();
  void Init();
};
#pragma pack(pop)

// moved from CPlayer.h
#pragma pack(push, 1)
struct _other_shape_all_zocl
{
  #pragma pack(push, 1)
  struct _model
  {
    unsigned __int16 wPartIndex;
    unsigned __int8 byLv;
  };
  #pragma pack(pop)

  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  unsigned __int8 byCashChangeStateFlag;
  unsigned __int8 byRecIndex;
  unsigned __int8 byFaceIndex;
  unsigned __int8 byHairIndex;
  _model ModelPerPart[8];
  unsigned __int8 byUserGrade;
  unsigned int dwGuildSerial;
  char wszName[17];
  unsigned __int8 byColor;
  unsigned __int8 byHonorGuildRank;
  unsigned __int8 bySpecialPart;
  unsigned __int8 byFrameIndex;
  unsigned __int8 byUnitPartIndex[6];

  _other_shape_all_zocl();
  int size();
};
#pragma pack(pop)

// moved from CPlayer.h
#pragma pack(push, 1)
struct _other_shape_part_zocl
{
  #pragma pack(push, 1)
  struct _model
  {
    unsigned __int16 wPartIndex;
    unsigned __int8 byLv;
  };
  #pragma pack(pop)

  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  unsigned __int8 byCashChangeStateFlag;
  _model ModelPerPart[8];
  unsigned __int8 byHonorGuildRank;
  unsigned __int8 bySpecialPart;
  unsigned __int8 byFrameIndex;
  unsigned __int8 byUnitPartIndex[6];

  _other_shape_part_zocl();
  int size();
};
#pragma pack(pop)

// moved from CPvpOrderView.h
#pragma pack(push, 1)
struct _pvp_order_view_inform_zocl
{
  int nKillCnt;
  int nDeahtCnt;
  long double dTodayPvpPoint;
  long double dOriginalPvpPoint;

  _pvp_order_view_inform_zocl();
  int size() const;
};
#pragma pack(pop)

// moved from CTotalGuildRankInfo.h
#pragma pack(push, 1)
struct _total_guild_rank_result_zocl
{
  #pragma pack(push, 1)
  struct _list
  {
    unsigned __int8 byRank;
    char wszGuildName[17];
    unsigned __int8 byGrade;
    char wszMasterName[17];
  };
  #pragma pack(pop)

  unsigned int dwVer;
  unsigned __int8 byRace;
  unsigned __int8 byExistSelfRankInfo;
  unsigned __int8 byCnt;
  _list list[11];

  int size();
};
#pragma pack(pop)

// moved from ElectPacketTypes.h
#pragma pack(push, 1)
struct _pt_trans_votepaper_zocl
{
  #pragma pack(push, 1)
  struct __body
  {
    unsigned __int8 byRank;
    char wszAvatorName[17];
    char wszGuildName[17];
    unsigned int dwWinCnt;
  };
  #pragma pack(pop)

  _pt_trans_votepaper_zocl();
  int size() const;

  unsigned __int8 byCnt;
  __body body[8];
};
#pragma pack(pop)

// moved from ElectPacketTypes.h
#pragma pack(push, 1)
struct _pt_notify_vote_score_zocl
{
  #pragma pack(push, 1)
  struct __body
  {
    unsigned __int8 byRank;
    char wszAvatorName[17];
    unsigned __int8 byScoreRate;
  };
  #pragma pack(pop)

  _pt_notify_vote_score_zocl();
  int size() const;

  unsigned __int8 byRace;
  unsigned __int8 byVoteRate;
  unsigned __int8 byNonvoteRate;
  unsigned __int8 byCnt;
  __body body[8];
};
#pragma pack(pop)

// moved from ElectPacketTypes.h
#pragma pack(push, 1)
struct _pt_appoint_inform_request_zocl
{
  #pragma pack(push, 1)
  struct __body
  {
    unsigned __int8 byLevel;
    unsigned __int8 byClassType;
    long double dPvpPoint;
    char wszAvatorName[17];
  };
  #pragma pack(pop)

  __body body[4];
};
#pragma pack(pop)

// moved from ElectPacketTypes.h
#pragma pack(push, 1)
struct _pt_propose_appoint_zocl
{
  unsigned __int8 byClassType;
};
#pragma pack(pop)

// moved from ElectPacketTypes.h
#pragma pack(push, 1)
struct _pt_query_appoint_zocl
{
  unsigned __int8 byClassType;
  unsigned __int8 byRet;
  char wszAvatorName[17];

  _pt_query_appoint_zocl();
  int size();
};
#pragma pack(pop)

// moved from GuildBattleTypes.h
#pragma pack(push, 1)
struct _possible_battle_guild_list_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    char wszName[17];
    unsigned __int8 byGrade;
    unsigned __int8 byRace;
    unsigned int dwGuildSerial;
  };
  #pragma pack(pop)

  unsigned __int8 byPage;
  unsigned __int8 byMaxPage;
  unsigned int dwCurVer;
  unsigned int dwBattleCost;
  unsigned __int8 byMapCnt;
  unsigned __int8 byMapInx[100];
  unsigned __int8 byCount;
  __list DestGuild[4];

  int size();
};
#pragma pack(pop)

// moved from GuildBattleTypes.h
#pragma pack(push, 2)
struct _guild_battle_rank_list_result_zocl_list
{
  int nRank;
  unsigned __int8 byGrade;
  char wszName[17];
  unsigned int dwWin;
  unsigned int dwLose;
  unsigned int dwDraw;
  unsigned int dwScore;
};
#pragma pack(pop)

// moved from GuildBattleTypes.h
#pragma pack(push, 1)
struct _guild_battle_rank_list_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    int nRank;
    char byGrade;
    char wszName[17];
    unsigned int dwWin;
    unsigned int dwLose;
    unsigned int dwDraw;
    unsigned int dwScore;
  };
  #pragma pack(pop)

  unsigned int dwCurVer;
  unsigned __int8 byRace;
  unsigned __int8 byCurPage;
  unsigned __int8 byMaxPage;
  unsigned __int8 byExistSelfGuildInfo;
  unsigned __int8 byCnt;
  _guild_battle_rank_list_result_zocl_list list[11];

  int size();
};
#pragma pack(pop)

// moved from GuildBattleTypes.h
#pragma pack(push, 1)
struct _guild_battle_reserved_schedule_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    char wsz1PName[17];
    unsigned __int8 by1PRace;
    char wsz2PName[17];
    unsigned __int8 by2PRace;
    unsigned __int8 byStartHour;
    unsigned __int8 byStartMin;
    unsigned __int8 byEndHour;
    unsigned __int8 byEndMin;
  };
  #pragma pack(pop)

  unsigned int dwVer;
  unsigned __int8 byToDay;
  unsigned __int8 byTomorrow;
  unsigned __int8 byDate;
  unsigned __int8 byPage;
  unsigned __int8 byMaxPage;
  unsigned __int8 bySelfScheduleInx;
  unsigned __int8 byCnt;
  __list List[5];

  int size();
};
#pragma pack(pop)

// moved from GuildBattleTypes.h
#pragma pack(push, 1)
struct _guild_battle_current_battle_info_result_zocl
{
  char wszLeftRedName[17];
  char wszRightBlueName[17];
  unsigned int dwLeftRedScore;
  unsigned int dwRighBluetScore;
  unsigned int dwLeftRedGoalCnt;
  unsigned int dwRighBluetGoalCnt;
  unsigned __int8 byLeftHour;
  unsigned __int8 byLeftMin;
  unsigned __int8 byLeftSec;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _guild_battle_notify_start_zocl
{
  char byDummy;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _guild_battle_notify_position_member_delete_zocl
{
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _guild_battle_fin_draw_result_zocl
{
  char wsz1PGuildName[17];
  char wsz2PGuildName[17];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _guild_battle_fin_win_result_zocl
{
  char wszWinGuildName[17];
  char wszLoseGuildName[17];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _guild_battle_fin_lose_result_zocl
{
  char wszWinGuildName[17];
  char wszLoseGuildName[17];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _guildroom_resttime_result_zocl
{
  char byReturnCode;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _itembox_destroy_zocl
{
  unsigned __int16 wIndex;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _messenger_enter_key_inform_zocl
{
  unsigned int dwAvatorSerial;
  unsigned int dwKey[4];
  unsigned int dwMessengerIP;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _notify_golden_dig_event_status_zocl
{
  char byRetCode;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _player_resurrect_inform_zocl
{
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _pvp_order_view_end_zocl
{
  char dummy[1];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _regist_bind_result_zocl
{
  char byRetCode;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _result_csi_sell_zocl
{
  char dummy[1];
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _unmannedtrader_re_regist_taxrate_update_inform_zocl
{
  char byTaxRate;
};
#pragma pack(pop)

// moved from ZoneServer-GU.h
#pragma pack(push, 1)
struct _use_guildroom_entrance_item_result_zocl
{
  char dummy[1];
};
#pragma pack(pop)

