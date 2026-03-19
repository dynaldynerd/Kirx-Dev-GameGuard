#pragma once

#include "../CHRID.h"
#include "../IdaCompat.h"
#include "../StoragePos.h"
#include "../tuning_data.h"

// Client -> Zone packet structs.

// a_trade_adjust_price_request_clzo.h
#pragma pack(push, 1)
struct _a_trade_adjust_price_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwNewPrice;
  unsigned int dwRegistSerial;
  unsigned __int8 byTaxRate;
};
#pragma pack(pop)

// a_trade_clear_item_request_clzo.h
#pragma pack(push, 1)
struct _a_trade_clear_item_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwRegistSerial;
};
#pragma pack(pop)

// a_trade_reg_item_request_clzo.h
#pragma pack(push, 1)
struct _a_trade_reg_item_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned __int8 byAmount;
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemIndex;
  unsigned int dwPrice;
  unsigned __int8 byTax;
  int bUseNpcLink;
};
#pragma pack(pop)

// alter_item_slot_request_clzo.h
#pragma pack(push, 1)
struct _alter_item_slot_request_clzo
{
  struct __list
  {
    unsigned __int8 byStorageIndex;
    unsigned int dwItemSerial;
    unsigned __int8 byClientSlotIndex;
  };
  unsigned __int8 byNum;
  __list list[100];
};
#pragma pack(pop)

// alter_link_slot_request_clzo.h
#pragma pack(push, 1)
struct _alter_link_slot_request_clzo
{
  struct __list
  {
    unsigned __int8 bySlotIndex;
    unsigned __int8 byLinkCode;
    unsigned __int16 wIndex;
  };
  unsigned __int8 byLinkLock;
  unsigned __int8 byNum;
  __list list[50];
};
#pragma pack(pop)

// alter_party_loot_share_request_clzo.h
#pragma pack(push, 1)
struct _alter_party_loot_share_request_clzo
{
  unsigned __int8 byLootShareMode;
};
#pragma pack(pop)

// buy_store_request_clzo.h
#pragma pack(push, 1)
struct _buy_store_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned int dwGoodSerial;
    unsigned __int8 byAmount;
  };
  unsigned int dwStoreIndex;
  unsigned __int8 byBuyNum;
  int bUseNPCLinkIntem;
  _list OfferList[100];
};
#pragma pack(pop)

// combine_ex_item_accept_request_clzo.h
#pragma pack(push, 2)
#pragma pack(push, 1)
struct _combine_ex_item_accept_request_clzo
{
  struct _Select_ItemList_buff
  {
    unsigned __int8 bySelectNum;
    unsigned __int8 bySelectIndexList[24];
  };
  unsigned __int8 byDlgType;
  unsigned int dwCheckKey;
  _Select_ItemList_buff SelectItemBuff;
};
#pragma pack(pop)
#pragma pack(pop)

// combine_ex_item_request_clzo.h
#pragma pack(push, 1)
struct _combine_ex_item_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned __int16 wItemSerial;
    unsigned __int8 byAmount;
  };
  unsigned int wManualIndex;
  unsigned __int8 byCombineSlotNum;
  _list iCombineSlot[5];
  int bUseNpcLink;
};
#pragma pack(pop)

// combine_item_request_clzo.h
#pragma pack(push, 1)
struct _combine_item_request_clzo
{
  unsigned __int16 wManualIndex;
  unsigned __int16 wOverlapItemSerial;
  unsigned __int8 byMaterialNum;
  _STORAGE_POS_INDIV ipMaterials[100];
};
#pragma pack(pop)

// guild_honor_set_request_clzo.h
#pragma pack(push, 1)
struct _guild_honor_set_request_clzo
{
  struct __list
  {
    char wszGuildName[17];
    unsigned __int8 byTaxRate;
  };
  unsigned __int8 byListNum;
  __list GuildList[5];
};
#pragma pack(pop)

// guild_manage_request_clzo.h
#pragma pack(push, 1)
struct _guild_manage_request_clzo
{
  _guild_manage_request_clzo();
  int size();
  unsigned __int8 byManageType;
  unsigned int dwManageDst;
  unsigned int dwManageObj1;
  unsigned int dwManageObj2;
  unsigned int dwManageObj3;
};
#pragma pack(pop)

// guildroom_enter_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_enter_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

// guildroom_out_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_out_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

// guildroom_rent_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_rent_request_clzo
{
  unsigned int dwGuildSerial;
  unsigned __int8 byRoomType;
};
#pragma pack(pop)

// guildroom_resttime_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_resttime_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

// make_item_request_clzo.h
#pragma pack(push, 1)
struct _make_item_request_clzo
{
  _STORAGE_POS_INDIV ipMakeTool;
  unsigned __int16 wManualIndex;
  unsigned __int8 byMaterialNum;
  _STORAGE_POS_INDIV ipMaterials[100];
};
#pragma pack(pop)

// make_tower_request_clzo.h
#pragma pack(push, 1)
struct _make_tower_request_clzo
{
  struct __material
  {
    unsigned __int16 wItemSerial;
    unsigned __int8 byMaterSlotIndex;
    unsigned __int8 byAmount;
  };
  unsigned __int16 wSkillIndex;
  __int16 zPos[3];
  unsigned __int16 wTowerItemSerial;
  unsigned __int16 wConsumeItemSerial[3];
  unsigned __int8 byMaterialNum;
  __material Material[30];
};
#pragma pack(pop)

// other_shape_request_clzo.h
#pragma pack(push, 1)
struct _other_shape_request_clzo
{
  enum : unsigned __int8
  {
    SHAPE_ALL = 0,
    SHAPE_PART = 1
  };
  unsigned __int16 wIndex;
  unsigned __int8 byReqType;
  unsigned __int8 byStateFlag;
};
#pragma pack(pop)

// request_csi_buy_clzo.h
#pragma pack(push, 1)
struct _request_csi_buy_clzo
{
  struct __item
  {
    int nPrice;
    unsigned __int8 byDiscount;
    unsigned __int8 byTblCode;
    unsigned __int16 wItemIdx;
    unsigned __int16 wStoreIdx;
    unsigned __int8 byOverlapNum;
    int nEvent[8];
    unsigned __int8 byEventType;
  };
  unsigned __int8 nNum;
  unsigned __int8 bySetKind;
  unsigned __int8 byCouponNum;
  _STORAGE_POS_INDIV CouponItem[3];
  __item item[20];
};
#pragma pack(pop)

// sell_store_request_clzo.h
#pragma pack(push, 1)
struct _sell_store_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned __int16 wSerial;
    unsigned __int8 byAmount;
  };
  unsigned int dwStoreIndex;
  unsigned __int8 bySellNum;
  int bUseNPCLinkIntem;
  _list Item[100];
};
#pragma pack(pop)

// talik_crystal_exchange_clzo.h
#pragma pack(push, 1)
struct _talik_crystal_exchange_clzo
{
  struct _list
  {
    unsigned __int16 wSerial;
    unsigned __int8 byItemCount;
  };
  unsigned __int8 byExchangeNum;
  _list Item[24];
};
#pragma pack(pop)

// unit_pack_fill_request_clzo.h
#pragma pack(push, 1)
struct _unit_pack_fill_request_clzo
{
  struct __list
  {
    unsigned __int8 bySpareIndex;
    unsigned __int16 wBulletIndex;
  };
  unsigned __int8 bySlotIndex;
  unsigned __int8 byFillNum;
  int bUseNPCLinkIntem;
  __list List[8];
};
#pragma pack(pop)

// unmannedtrader_buy_item_request_clzo.h
#pragma pack(push, 1)
struct _unmannedtrader_buy_item_request_clzo
{
  struct __list
  {
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
  };
  unsigned __int8 byDivision;
  unsigned __int8 byClass;
  unsigned __int8 bySubClass;
  unsigned int dwVer;
  unsigned __int8 byStoreIndex;
  unsigned __int8 byNum;
  __list List[10];
  int bUseNpcLink;
};
#pragma pack(pop)

// unmannedtrader_re_regist_request_clzo.h
#pragma pack(push, 1)
struct _unmannedtrader_re_regist_request_clzo
{
  struct __list
  {
    bool bRegist;
    unsigned __int16 wItemSerial;
    unsigned __int8 byAmount;
    unsigned __int8 byItemTableCode;
    unsigned __int16 wItemIndex;
    unsigned int dwPrice;
    unsigned int dwRegistSerial;
  };
  unsigned __int8 byTaxRate;
  unsigned __int8 byRegedNum;
  __list List[10];
};
#pragma pack(pop)

// unmannedtrader_search_list_request_clzo.h
#pragma pack(push, 1)
struct _unmannedtrader_search_list_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int8 byDivision;
  unsigned __int8 byClass;
  unsigned __int8 bySubClass;
  unsigned __int8 bySortType;
  unsigned int dwVer;
  unsigned __int8 byPage;
  int bUseNpcLink;
};
#pragma pack(pop)

// add_char_request_clzo.h
#pragma pack(push, 1)
struct _add_char_request_clzo
{
  unsigned __int8 bySlotIndex;
  char wszCharName[17];
  unsigned __int8 byRaceSexCode;
  char wszClassCode[5];
  unsigned int dwMakeCharKey;
};
#pragma pack(pop)

// animus_command_request_clzo.h
#pragma pack(push, 1)
struct _animus_command_request_clzo
{
  char byCommandCode;
};
#pragma pack(pop)

// animus_inven_change_request_clzo.h
#pragma pack(push, 1)
struct _animus_inven_change_request_clzo
{
  _STORAGE_POS_INDIV Item;
  unsigned __int16 wReplaceSerial;
};
#pragma pack(pop)

// away_party_invitation_answer_request_clzo.h
#pragma pack(push, 1)
struct _away_party_invitation_answer_request_clzo
{
  char byRetCode;
  unsigned __int16 wBossIndex;
  unsigned int dwBossSerial;
};
#pragma pack(pop)

// boss_sms_msg_request_clzo.h
#pragma pack(push, 1)
struct _boss_sms_msg_request_clzo
{
  char bySize;
  char wszChatData[48];
};
#pragma pack(pop)

// brief_pass_report_clzo.h
#pragma pack(push, 1)
struct _brief_pass_report_clzo
{
  char bySlotIndex;
};
#pragma pack(pop)

// class_skill_request_clzo.h
#pragma pack(push, 1)
struct _class_skill_request_clzo
{
  unsigned __int16 wSkillIndex;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// dtrade_ask_request_clzo.h
#pragma pack(push, 1)
struct _dtrade_ask_request_clzo
{
  unsigned __int16 wDstIndex;
};
#pragma pack(pop)

// dtrade_del_request_clzo.h
#pragma pack(push, 1)
struct _dtrade_del_request_clzo
{
  char bySlotIndex;
};
#pragma pack(pop)

// force_inven_change_request_clzo.h
#pragma pack(push, 1)
struct _force_inven_change_request_clzo
{
  _STORAGE_POS_INDIV Item;
  unsigned __int16 wReplaceSerial;
};
#pragma pack(pop)

// force_request_clzo.h
#pragma pack(push, 1)
struct _force_request_clzo
{
  unsigned __int16 wForceSerial;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// gesture_request_clzo.h
#pragma pack(push, 1)
struct _gesture_request_clzo
{
  char byGestureType;
};
#pragma pack(pop)

// guild_honor_list_request_clzo.h
#pragma pack(push, 1)
struct _guild_honor_list_request_clzo
{
  char byDivision;
};
#pragma pack(pop)

// guild_list_request_clzo.h
#pragma pack(push, 1)
struct _guild_list_request_clzo
{
  char byPage;
};
#pragma pack(pop)

// itembox_take_request_clzo.h
#pragma pack(push, 1)
struct _itembox_take_request_clzo
{
  unsigned __int16 wItemBoxIndex;
  unsigned __int16 wAddSerial;
};
#pragma pack(pop)

// make_trap_request_clzo.h
#pragma pack(push, 1)
struct _make_trap_request_clzo
{
  unsigned __int16 wSkillIndex;
  __int16 zPos[3];
  unsigned __int16 wTrapItemSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// mine_start_request_clzo.h
#pragma pack(push, 1)
struct _mine_start_request_clzo
{
  unsigned __int8 byMineIndex;
  unsigned __int8 byOreIndex;
  unsigned __int16 wOreSerial;
};
#pragma pack(pop)

// mode_change_request_clzo.h
#pragma pack(push, 1)
struct _mode_change_request_clzo
{
  char byModeCode;
  char byStandCode;
};
#pragma pack(pop)

// object_server_pos_request_clzo.h
#pragma pack(push, 1)
struct _object_server_pos_request_clzo
{
  unsigned __int8 byObjKind;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// pvp_rank_list_request_clzo.h
#pragma pack(push, 1)
struct _pvp_rank_list_request_clzo
{
  unsigned __int8 byRace;
  unsigned __int8 byType;
  unsigned __int8 byPage;
};
#pragma pack(pop)

// quest_giveup_request_clzo.h
#pragma pack(push, 1)
struct _quest_giveup_request_clzo
{
  char byQuestDBSlot;
};
#pragma pack(pop)

// quest_select_reward_report_clzo.h
#pragma pack(push, 1)
struct _quest_select_reward_report_clzo
{
  unsigned __int8 byQuestSlot;
  unsigned __int8 bySelectItemSlotIndex;
  unsigned __int8 byLinkQuestIndex;
};
#pragma pack(pop)

// release_group_target_object_request_clzo.h
#pragma pack(push, 1)
struct _release_group_target_object_request_clzo
{
  char byGroupType;
};
#pragma pack(pop)

// request_change_tax_rate_request_clzo.h
#pragma pack(push, 1)
struct _request_change_tax_rate_request_clzo
{
  char byTaxRate;
};
#pragma pack(pop)

// select_waited_quest_report_clzo.h
#pragma pack(push, 1)
struct _select_waited_quest_report_clzo
{
  char bySelectRewardItem;
};
#pragma pack(pop)

// set_group_target_object_request_clzo.h
#pragma pack(push, 1)
struct _set_group_target_object_request_clzo
{
  unsigned __int8 byGroupType;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// set_race_boss_cry_msg_request_clzo.h
#pragma pack(push, 1)
struct _set_race_boss_cry_msg_request_clzo
{
  char bySlot;
  char wszMessage[64];
};
#pragma pack(pop)

// set_target_object_request_clzo.h
#pragma pack(push, 1)
struct _set_target_object_request_clzo
{
  unsigned __int8 byObjKind;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// throw_skill_request_clzo.h
#pragma pack(push, 1)
struct _throw_skill_request_clzo
{
  unsigned __int16 wBulletSerial;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// throw_unit_request_clzo.h
#pragma pack(push, 1)
struct _throw_unit_request_clzo
{
  char byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// trunk_change_passwd_request_clzo.h
#pragma pack(push, 1)
struct _trunk_change_passwd_request_clzo
{
  char wszPrevPassword[13];
  char wszChangedPassword[13];
  unsigned __int8 byHintIndex;
  char wszHintAnswer[17];
};
#pragma pack(pop)

// trunk_download_request_clzo.h
#pragma pack(push, 1)
struct _trunk_download_request_clzo
{
  char wszPassword[13];
};
#pragma pack(pop)

// trunk_est_request_clzo.h
#pragma pack(push, 1)
struct _trunk_est_request_clzo
{
  char wszPassword[13];
  unsigned __int8 byHintIndex;
  char wszHintAnswer[17];
};
#pragma pack(pop)

// trunk_io_merge_request_clzo.h
#pragma pack(push, 1)
struct _trunk_io_merge_request_clzo
{
  unsigned __int8 byStartStorageIndex;
  unsigned __int8 byTarStorageIndex;
  unsigned __int16 wStartSerial;
  unsigned __int16 wTarSerial;
  unsigned __int16 wMoveAmount;
};
#pragma pack(pop)

// trunk_io_swap_request_clzo.h
#pragma pack(push, 1)
struct _trunk_io_swap_request_clzo
{
  unsigned __int8 byStartStorageIndex;
  unsigned __int8 byTarStorageIndex;
  unsigned __int16 wStartSerial;
  unsigned __int16 wTarSerial;
};
#pragma pack(pop)

// uilock_init_request_clzo.h
#pragma pack(push, 1)
struct _uilock_init_request_clzo
{
  char uszUILockPW[13];
  char uszUILockPWConfirm[13];
  unsigned __int8 byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)

// uilock_update_request_clzo.h
#pragma pack(push, 1)
struct _uilock_update_request_clzo
{
  char uszUILockPWOld[13];
  char uszUILockPW[13];
  char uszUILockPWConfirm[13];
  char byHintIndex;
  char uszHintAnswer[17];
};
#pragma pack(pop)

// unit_bullet_replace_request_clzo.h
#pragma pack(push, 1)
struct _unit_bullet_replace_request_clzo
{
  char bySlotIndex;
  char byPackIndex;
  char byBulletPart;
};
#pragma pack(pop)

// cnetworkex_scalar_requests_clzo.h
#pragma pack(push, 1)
struct _apex_result_request_clzo
{
  unsigned int dwResult;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _new_pos_start_request_clzo
{
  unsigned __int8 byMapInMode;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _goto_base_portal_request_clzo
{
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _goto_avator_request_clzo
{
  char wszAvatorName[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _move_portal_request_clzo
{
  char byPortalIndex;
  unsigned __int16 wConsumeSerial[3];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _enter_return_gate_request_clzo
{
  unsigned __int16 wGateIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _move_type_change_request_clzo
{
  char byMoveType;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _init_class_request_clzo
{
  unsigned __int8 byInitType;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _exchange_dalant_for_gold_request_clzo
{
  unsigned int dwDalant;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _exchange_gold_for_dalant_request_clzo
{
  unsigned int dwGold;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _limit_item_num_request_clzo
{
  unsigned int dwStoreIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _darkhole_open_request_clzo
{
  unsigned int dwItemSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _post_index_request_clzo
{
  unsigned int dwPostIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _post_return_confirm_request_clzo
{
  unsigned int dwPostSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _transform_siege_mode_request_clzo
{
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _transship_renew_ticket_request_clzo
{
  unsigned __int16 wTicketSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_join_invitation_request_clzo
{
  unsigned __int16 wDstIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_join_application_request_clzo
{
  unsigned __int16 wBossIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_leave_compulsion_request_clzo
{
  unsigned int dwExiterSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_succession_request_clzo
{
  unsigned int dwSuccessorSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_join_invitation_answer_clzo
{
  _CLID idBoss;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_join_application_answer_clzo
{
  _CLID idApplicant;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_lock_request_clzo
{
  bool bLock;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _away_party_invitation_request_clzo
{
  char wszCharName[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _back_tower_request_clzo
{
  unsigned int dwTowerObjSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _d_trade_answer_request_clzo
{
  _CLID idAsker;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _dtrade_ok_request_clzo
{
  unsigned int dwKey[4];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _animus_recall_request_clzo
{
  unsigned __int16 wAnimusItemSerial;
  unsigned __int16 wUnknown;
  unsigned __int16 wAnimusClientHP;
  unsigned __int16 wAnimusClientFP;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _unit_leave_request_clzo
{
  __int16 zPos[3];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _npc_rec_index_request_clzo
{
  unsigned int dwRecIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _npc_quest_request_clzo
{
  unsigned int dwRecIndex;
  unsigned int dwNPCQuestIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_cancel_suggest_request_clzo
{
  unsigned int dwMatterVoteSynKey;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_query_info_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_push_money_request_clzo
{
  unsigned int dwPushGold;
  unsigned int dwPushDalant;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_battle_take_gravity_stone_request_clzo
{
  int nObjectIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_battle_goal_request_clzo
{
  int nGoalObjectIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_establish_request_clzo
{
  char wszGuildName[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_join_apply_request_clzo
{
  char wszGuildName[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _buddy_del_request_clzo
{
  unsigned int dwBuddySerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _exchange_gold_for_pvp_request_clzo
{
  unsigned int dwGold;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _use_potion_request_clzo
{
  char byReqType;
  unsigned __int16 wTargetIndex;
  unsigned int dwReserved;
  _STORAGE_POS_INDIV Item;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _throw_storage_request_clzo
{
  _STORAGE_POS_INDIV Item;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _equip_part_request_clzo
{
  _STORAGE_POS_INDIV Item;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _embellish_request_clzo
{
  _STORAGE_POS_INDIV Item;
  unsigned __int16 wChangeSerial;
};
#pragma pack(pop)
static_assert(sizeof(_embellish_request_clzo) == 6);

#pragma pack(push, 1)
struct _off_part_request_clzo
{
  _STORAGE_POS_INDIV Item;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _add_bag_request_clzo
{
  unsigned __int16 wBagSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _use_recover_loss_exp_item_request_clzo
{
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _exchange_item_request_clzo
{
  unsigned __int16 wManualIndex;
  unsigned __int16 wTarSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _use_firecracker_item_request_clzo
{
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _use_soccer_ball_item_request_clzo
{
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _use_radar_item_request_clzo
{
  _STORAGE_POS_INDIV Item;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)
static_assert(sizeof(_use_radar_item_request_clzo) == 10);

#pragma pack(push, 1)
struct _use_recall_teleport_item_request_clzo
{
  _STORAGE_POS_INDIV Item;
  char wszRecallName[17];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _character_rename_cash_request_clzo
{
  char bChange;
  _STORAGE_POS_INDIV Item;
  char szNewName[16];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _party_req_block_report_clzo
{
  bool bBlock;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _whisper_block_report_clzo
{
  bool bBlock;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _trade_block_report_clzo
{
  bool bBlock;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _guild_battle_block_report_clzo
{
  bool bBlock;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _trunk_hint_answer_request_clzo
{
  char wszAnswer[17];
};
#pragma pack(pop)

static_assert(sizeof(_new_pos_start_request_clzo) == 1);
static_assert(sizeof(_goto_avator_request_clzo) == 17);
static_assert(sizeof(_init_class_request_clzo) == 1);
static_assert(sizeof(_party_join_invitation_answer_clzo) == 6);
static_assert(sizeof(_party_join_application_answer_clzo) == 6);
static_assert(sizeof(_party_lock_request_clzo) == 1);
static_assert(sizeof(_away_party_invitation_request_clzo) == 17);
static_assert(sizeof(_d_trade_answer_request_clzo) == 6);
static_assert(sizeof(_guild_establish_request_clzo) == 17);
static_assert(sizeof(_guild_join_apply_request_clzo) == 17);
static_assert(sizeof(_throw_storage_request_clzo) == 4);
static_assert(sizeof(_equip_part_request_clzo) == 4);
static_assert(sizeof(_off_part_request_clzo) == 4);
static_assert(sizeof(_use_recall_teleport_item_request_clzo) == 21);
static_assert(sizeof(_party_req_block_report_clzo) == 1);
static_assert(sizeof(_whisper_block_report_clzo) == 1);
static_assert(sizeof(_trade_block_report_clzo) == 1);
static_assert(sizeof(_guild_battle_block_report_clzo) == 1);
static_assert(sizeof(_trunk_hint_answer_request_clzo) == 17);

#pragma pack(push, 1)
struct _char_slot_request_clzo
{
  char bySlotIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _alter_window_info_request_clzo
{
  unsigned int dwSkill[2];
  unsigned int dwForce[2];
  unsigned int dwCharacter[2];
  unsigned int dwAnimus[2];
  unsigned int dwInven;
  unsigned int dwInvenBag[5];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _chat_operator_request_clzo
{
  char byRaceCode;
  char bySize;
  char wszChatData[255];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _chat_message_request_clzo
{
  char bySize;
  char wszChatData[255];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _chat_trade_request_clzo
{
  char byRaceCode;
  char bySize;
  char wszChatData[255];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _chat_far_request_clzo
{
  char wszName[16];
  char byUnknown;
  char bySize;
  char wszChatData[255];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _chat_multi_far_request_clzo
{
  char byRaceCode;
  char byUnknown;
  unsigned __int8 byTransNum;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _chat_toggle_request_clzo
{
  char byRecvType;
};
#pragma pack(pop)


// moved from CMainThread.h
#pragma pack(push, 1)
struct _set_item_check_request_clzo
{
  bool bSet;
  unsigned int dwSetIndex;
  char bySetItemNum;
  char bySetEffectNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_offer_suggest_request_clzo
{
  char byMatterType;
  unsigned int dwMatterDst;
  unsigned int dwMatterObj1;
  unsigned int dwMatterObj2;
  unsigned int dwMatterObj3;
  char byCommentLen;
  char wszComment[65];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_test_request_clzo
{
  char byEffectCode;
  char byEffectIndex;
  unsigned __int16 wBulletSerial;
  char byWeaponPart;
  __int16 zTar[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_request_punishment_clzo
{
  char byType;
  unsigned __int16 wContentSize;
  char wszCharName[17];
  char wszContent[1280];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _chat_guild_request_clzo
{
  unsigned int dwDstSerial;
  char bySize;
  char wszChatData[256];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _propose_vote_request_clzo
{
  char byLimGrade;
  unsigned __int16 wContentSize;
  char wszContent[1280];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_part_tuning_request_clzo
{
  char bySlotIndex;
  char byTuningNum;
  int bUseNPCLinkIntem;
  _tuning_data TuningList[6];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_amine_make_storage_clzo
{
  unsigned int dwAvatorSerial;
};
static_assert(sizeof(_personal_amine_make_storage_clzo) == 4);
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_amine_infoui_open_clzo
{
  bool bIsOpen;
};
static_assert(sizeof(_personal_amine_infoui_open_clzo) == 1);
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_install_clzo
{
  char byMineDummyIndex;
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_uninstall_clzo
{
  unsigned int dwObjSerial;
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_selore_clzo
{
  unsigned int dwObjSerial;
  char bySelectOre;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_battery_insert_clzo
{
  unsigned int dwObjSerial;
  char byInPos;
  unsigned __int16 wItemSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_battery_extract_clzo
{
  unsigned int dwObjSerial;
  char byPos;
  char bySlotIdx;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _personal_automine_popore_clzo
{
  unsigned __int16 wItemSerial;
  char byNum;
  char byStorageIndex_L;
  char byStorageIndex_R;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_automine_selectore_clzo
{
  unsigned __int8 bySelected;
};
static_assert(sizeof(_pt_automine_selectore_clzo) == 1);
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_automine_moveore_clzo
{
  unsigned __int8 bySrcPage;
  unsigned __int8 bySrcSlot;
  unsigned __int8 byDstPage;
  unsigned __int8 byDstSlot;
};
static_assert(sizeof(_pt_automine_moveore_clzo) == 4);
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_automine_charge_clzo
{
  int nCharge;
  int nGold;
};
static_assert(sizeof(_pt_automine_charge_clzo) == 8);
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_automine_merge_clzo
{
  char byPage;
  _INVENKEY item;
  char byOverlapNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pt_automine_getoutore_clzo
{
  char bySrcPage;
  unsigned int dwSrcK;
  char byDstBag;
  unsigned int dwDstK;
  char byOverlapNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pc_room_reward_info_clzo
{
  unsigned int dwPcRoomIndex;
  char bySelect_ItemIndex[5];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _darkhole_answer_reenter_request_clzo
{
  bool bEnter;
  unsigned __int16 wChannelIndex;
  unsigned int dwChannelSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _darkhole_enter_request_clzo
{
  unsigned __int16 wHoleIndex;
  unsigned int dwHoleSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _post_request_clzo
{
  char wszRecvName[17];
  char wszTitle[21];
  char wszContent[201];
  _STORAGE_POS_INDIV Item;
  unsigned int dwGold;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_frame_buy_request_clzo
{
  char byFrameCode;
  int bUseNPCLinkIntem;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_sell_request_clzo
{
  char bySlotIndex;
  int bUseNPCLinkIntem;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_frame_repair_request_clzo
{
  char bySlotIndex;
  int bUseNPCLinkIntem;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_bullet_fill_request_clzo
{
  char bySlotIndex;
  int bUseNPCLinkIntem;
  unsigned __int16 wBulletIndex[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _unit_delivery_request_clzo
{
  char bySlotIndex;
  unsigned int dwStoreIndex;
  __int16 zUnitNewPos[3];
  int bUseNPCLinkIntem;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _res_separation_request_clzo
{
  unsigned __int16 wStartSerial;
  char byMoveAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _res_division_request_clzo
{
  unsigned __int16 wStartSerial;
  unsigned __int16 wTarSerial;
  char byMoveAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _potionsocket_separation_request_clzo
{
  unsigned __int16 wStartSerial;
  char byMoveAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _potionsocket_division_request_clzo
{
  unsigned __int16 wStartSerial;
  unsigned __int16 wTarSerial;
  char byMoveAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _total_guild_rank_request_clzo
{
  unsigned int dwVer;
  char byRace;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _weekly_guild_rank_request_clzo
{
  unsigned int dwVer;
  char byRace;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _set_group_map_point_request_clzo
{
  char byGroupType;
  float zPos[2];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trunk_io_move_request_clzo
{
  char byStartStorageIndex;
  char byTarStorageIndex;
  unsigned __int16 wItemSerial;
  char byClientSlotIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trunk_alter_item_slot_request_clzo
{
  unsigned int dwItemSerial;
  char byClientSlotIndex;
  char byStorageIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trunk_res_division_request_clzo
{
  unsigned __int16 wStartSerial;
  unsigned __int16 wTarSerial;
  unsigned __int16 wMoveAmount;
  char byStorageIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trunk_potionsocket_division_request_clzo
{
  unsigned __int16 wStartSerial;
  unsigned __int16 wTarSerial;
  unsigned __int16 wMoveAmount;
  char byStorageIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _trunk_io_money_request_clzo
{
  char byCase;
  unsigned int dwDalant;
  unsigned int dwGold;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_add_request_clzo
{
  char byStorageCode;
  unsigned int dwSerial;
  char bySlotIndex;
  char byAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _d_trade_bet_request_clzo
{
  char byMoneyUnit;
  unsigned int dwBetAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _pvp_cash_point_recorver_request_clzo
{
  unsigned int dwItemSerial;
  char byItemCnt;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _alive_char_request_clzo
{
  char byCase;
  unsigned int dwSerial;
  char wszNewName[17];
  char bySlotIndex;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _ore_cutting_request_clzo
{
  char byCuttingHouse;
  unsigned __int16 wOreSerial;
  char byProcessNum;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _ore_into_bag_request_clzo
{
  unsigned __int16 wResIndex;
  unsigned __int16 wSerial;
  char byAddAmount;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _animus_target_request_clzo
{
  char byObjectID;
  unsigned __int16 wObjectIndex;
  unsigned int dwObjectSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _select_class_request_clzo
{
  unsigned __int16 wSelClassIndex;
  char bySelectRewardItem;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _cast_vote_request_clzo
{
  int nVoteSerial;
  char byCode;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _move_request_clzo
{
  char byMoveType;
  float fCur[3];
  unsigned int dwSerial;
  __int16 zTar[2];
  char byDirect;
};
#pragma pack(pop)
static_assert(sizeof(_move_request_clzo) == 22);

// real_movpos_request_clzo.h
#pragma pack(push, 1)
struct _real_movpos_request_clzo
{
  unsigned int dwSerial;
  float fCur[3];
};
#pragma pack(pop)
static_assert(sizeof(_real_movpos_request_clzo) == 16);

// player_stop_clzo.h
#pragma pack(push, 1)
struct _player_stop_clzo
{
  unsigned int dwSerial;
  float fCur[3];
};
#pragma pack(pop)
static_assert(sizeof(_player_stop_clzo) == 16);

// moved from CMainThread.h
#pragma pack(push, 1)
struct _move_potal_request_clzo
{
  char byPotalIndex;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_join_accept_request_clzo
{
  bool bAccept;
  unsigned int dwApplierSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_vote_request_clzo
{
  unsigned int dwMatterVoteSynKey;
  char byVoteCode;
};
#pragma pack(pop)

// regist_bind_request_clzo.h
#pragma pack(push, 1)
struct _regist_bind_request_clzo
{
  unsigned __int16 wNPCIndex;
};
#pragma pack(pop)
static_assert(sizeof(_regist_bind_request_clzo) == 2);

// upgrade_item_request_clzo.h
#pragma pack(push, 1)
struct _upgrade_item_request_clzo
{
  _STORAGE_POS_INDIV m_posUpgItem;
  _STORAGE_POS_INDIV m_posTalik;
  _STORAGE_POS_INDIV m_posToolItem;
  unsigned __int8 byJewelNum;
  _STORAGE_POS_INDIV m_posUpgJewel[4];
};
#pragma pack(pop)
static_assert(sizeof(_upgrade_item_request_clzo) == 29);

// downgrade_item_request_clzo.h
#pragma pack(push, 1)
struct _downgrade_item_request_clzo
{
  _STORAGE_POS_INDIV m_posUpgItem;
  _STORAGE_POS_INDIV m_posTalik;
  _STORAGE_POS_INDIV m_posToolItem;
};
#pragma pack(pop)
static_assert(sizeof(_downgrade_item_request_clzo) == 12);

// moved from CMainThread.h
#pragma pack(push, 1)
struct _possible_battle_guild_list_request_clzo
{
  char byPage;
  unsigned int dwCurVer;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_rank_list_request_clzo
{
  unsigned int dwCurVer;
  char byRace;
  char byPage;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_reserved_schedule_request_clzo
{
  unsigned int dwVer;
  char byDay;
  char byPage;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _guild_battle_get_gravity_stone_request_clzo
{
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_add_request_clzo
{
  unsigned __int16 wDstIndex;
  unsigned int dwDstSerial;
  char wszDstName[17];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_add_answer_clzo
{
  bool bAccept;
  unsigned __int16 wAskerIndex;
  unsigned int dwAskerSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_gen_request_clzo
{
  char byID;
  unsigned __int16 wIndex;
  char byAttPart;
  unsigned __int16 wBulletSerial;
  unsigned __int16 wEffBulletSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_skill_request_clzo
{
  char byID;
  unsigned __int16 wIndex;
  char byEffectCode;
  unsigned __int16 wSkillIndex;
  unsigned __int16 wBulletSerial;
  __int16 zAreaPos[2];
  unsigned __int16 wConsumeItemSerial[3];
  unsigned __int16 wEffBulletSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_force_request_clzo
{
  char byID;
  unsigned __int16 wIndex;
  __int16 zAreaPos[2];
  unsigned __int16 wForceSerial;
  unsigned __int16 wConsumeItemSerial[3];
  unsigned __int16 wEffBulletSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_unit_request_clzo
{
  char byID;
  unsigned __int16 wIndex;
  char byWeaponPart;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _attack_siege_request_clzo
{
  char byID;
  unsigned __int16 wIndex;
  __int16 zAttackPos[2];
  char byAttPart;
  unsigned __int16 wBulletSerial;
  unsigned __int16 wEffBulletSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _skill_request_clzo
{
  char bySkillIndex;
  _CHRID idDst;
  char byAttackSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _decide_recall_request_clzo
{
  unsigned __int16 wRequestID;
  char byAgree;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _back_trap_request_clzo
{
  unsigned int dwTrapObjSerial;
  unsigned __int16 wAddSerial;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _force_recall_teleport_request_clzo
{
  unsigned __int16 wForceSerial;
  char wszRecallName[17];
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _skill_recall_teleport_request_clzo
{
  unsigned __int16 wSkillIndex;
  char wszRecallName[17];
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _class_skill_recall_teleport_request_clzo
{
  unsigned __int16 wSkillIndex;
  char wszRecallName[17];
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _player_macro_update_request_clzo
{
  unsigned int potion[3];
  unsigned int potionvalue[3];
  unsigned int behavior[30];
  char chatting[2][5][81];
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _target_object_hp_inform_clzo
{
  char byKind;
  char byID;
  unsigned int dwSerial;
  unsigned __int16 wHPRate;
};
#pragma pack(pop)

// moved from CMainThread.h
#pragma pack(push, 1)
struct _buddy_del_result_clzo
{
  char byRetCode;
  unsigned int dwSerial;
};
#pragma pack(pop)

