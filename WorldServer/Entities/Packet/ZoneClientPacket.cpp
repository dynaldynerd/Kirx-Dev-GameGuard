#include "pch.h"
#include "ZoneClientPacket.h"

_alter_cont_effect_time_zocl::_alter_cont_effect_time_zocl() : byEffectNum(0)
{
}

int _alter_cont_effect_time_zocl::size()
{
  if (byEffectNum > 8u)
  {
    byEffectNum = 0;
  }
  return 33 - 4 * (8 - byEffectNum);
}

_attack_gen_result_zocl::_attack_gen_result_zocl() : byListNum(0)
{
}

int _attack_gen_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 331 - 10 * (32 - byListNum);
}

_buy_store_success_zocl::_buy_store_success_zocl() : byBuyNum(0)
{
}

unsigned __int16 _buy_store_success_zocl::size()
{
  if (byBuyNum > 100)
  {
    byBuyNum = 0;
  }
  return static_cast<unsigned __int16>(750 - 7LL * (100 - byBuyNum));
}

_darkhole_member_info_inform_zocl::_darkhole_member_info_inform_zocl()
{
  wMemberNum = 0;
}

_darkhole_mission_info_inform_zocl::_darkhole_mission_info_inform_zocl()
{
  byJobNum = 0;
}

void _gm_msg_gmcall_list_response_zocl::Init(_gm_msg_gmcall_list_response_zocl *msg)
{
  std::memset(msg, 0, sizeof(_gm_msg_gmcall_list_response_zocl));
}

_limit_item_num_info_zocl::_limit_item_num_info_zocl()
{
  std::memset(this, 0, sizeof(_limit_item_num_info_zocl));
}

unsigned __int16 _limit_item_num_info_zocl::size()
{
  if (byLimitItemNum > 16)
  {
    byLimitItemNum = 0;
  }
  return static_cast<unsigned __int16>(101 - 6LL * (16 - byLimitItemNum));
}

_nuclear_find_rader_result_zocl::_nuclear_find_rader_result_zocl()
{
  byRaceCode = 0;
  byUseClass = 0;
}

_nuclear_position_result_zocl::_nuclear_position_result_zocl()
{
  dwMasterSerial = 0;
  zPos[0] = 0.0f;
  zPos[1] = 0.0f;
  zPos[2] = 0.0f;
  byUseClass = 0;
}

_nuclear_result_code_zocl::_nuclear_result_code_zocl()
{
  byRetCode = 0;
}

_personal_automine_install_zocl::_personal_automine_install_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_pvp_cash_recover_itemlist_result_zocl::_pvp_cash_recover_itemlist_result_zocl()
{
  std::memset(this, 0, sizeof(_pvp_cash_recover_itemlist_result_zocl));
}

unsigned __int16 _pvp_cash_recover_itemlist_result_zocl::size()
{
  if (byItemNum > 14u)
  {
    byItemNum = 0;
  }
  return static_cast<unsigned __int16>(1 + (4 * byItemNum));
}

_result_csi_buy_zocl::_result_csi_buy_zocl()
{
  nCashAmount = 0;
  nNum = 0;
}

_store_list_result_zocl::_store_list_result_zocl() : byStoreNum(0)
{
}

unsigned __int16 _store_list_result_zocl::size()
{
  if (byStoreNum > 100)
  {
    byStoreNum = 0;
  }
  return static_cast<unsigned __int16>(1601 - 16LL * (100 - byStoreNum));
}

_talik_crystal_exchange_zocl::_talik_crystal_exchange_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

__int64 _talik_crystal_exchange_zocl::size()
{
  return 25;
}

_attack_count_result_zocl::_attack_count_result_zocl() : byListNum(0)
{
}

int _attack_count_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 328 - 10 * (32 - byListNum);
}

_attack_force_result_zocl::_attack_force_result_zocl() : byListNum(0)
{
}

int _attack_force_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 335 - 10 * (32 - byListNum);
}

_attack_selfdestruction_result_zocl::_attack_selfdestruction_result_zocl() : byListNum(0)
{
}

int _attack_selfdestruction_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 330 - 10 * (32 - byListNum);
}

_attack_siege_result_zocl::_attack_siege_result_zocl() : byListNum(0)
{
}

int _attack_siege_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 334 - 10 * (32 - byListNum);
}

_attack_skill_result_zocl::_attack_skill_result_zocl() : byListNum(0)
{
}

int _attack_skill_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 339 - 10 * (32 - byListNum);
}

_attack_unit_result_zocl::_attack_unit_result_zocl() : byListNum(0)
{
}

int _attack_unit_result_zocl::size()
{
  if (byListNum > 32)
  {
    byListNum = 0;
  }
  return 332 - 10 * (32 - byListNum);
}

__int64 _a_trade_adjust_price_result_zocl::size() const
{
  if (byRetCode)
  {
    return 6;
  }
  return 20;
}

__int64 _a_trade_clear_item_result_zocl::size() const
{
  if (byRetCode)
  {
    return 1;
  }
  return 7;
}

_alive_char_result_zocl::_alive_char_result_zocl()
{
}

unsigned __int16 _chat_far_failure_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_far_failure_zocl));
}

unsigned __int16 _chat_guild_failure_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_guild_failure_zocl));
}

__int64 _chat_lock_inform_zocl::size() const
{
  return 1;
}

unsigned __int16 _chat_multi_far_failure_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_multi_far_failure_zocl));
}

_chat_multi_far_trans_zocl::_chat_multi_far_trans_zocl()
{
  wSize = 0;
}

unsigned __int16 _chat_multi_far_trans_zocl::size()
{
  if (wSize > 500)
  {
    wSize = 0;
  }
  return static_cast<unsigned __int16>(502 - (500 - wSize));
}

unsigned __int16 _darkhole_answer_reenter_result_zocl::size() const
{
  return 1;
}

unsigned __int16 _darkhole_ask_reenter_inform_zocl::size() const
{
  return 6;
}

unsigned __int16 _darkhole_channel_close_inform_zocl::size() const
{
  return 1;
}

unsigned __int16 _darkhole_clear_out_result_zocl::size() const
{
  return 9;
}

_darkhole_create_zocl::_darkhole_create_zocl()
{
  std::memset(this, 0, sizeof(_darkhole_create_zocl));
}

unsigned __int16 _darkhole_create_zocl::size() const
{
  return 36;
}

unsigned __int16 _darkhole_destroy_zocl::size() const
{
  return 6;
}

unsigned __int16 _darkhole_enter_result_zocl::size() const
{
  return 12;
}

_darkhole_fixpositon_zocl::_darkhole_fixpositon_zocl()
{
  std::memset(this, 0, sizeof(_darkhole_fixpositon_zocl));
}

unsigned __int16 _darkhole_fixpositon_zocl::size() const
{
  return 36;
}

unsigned __int16 _darkhole_giveup_out_result_zocl::size() const
{
  return 9;
}

unsigned __int16 _darkhole_job_pass_inform_zocl::size() const
{
  return 1;
}

unsigned __int16 _darkhole_member_info_inform_zocl::size()
{
  if (wMemberNum > 32)
  {
    wMemberNum = 32;
  }
  return static_cast<unsigned __int16>(678 - 21 * (32 - wMemberNum));
}

unsigned __int16 _darkhole_mission_info_inform_zocl::size()
{
  if (byJobNum > 128)
  {
    byJobNum = 0;
  }
  return static_cast<unsigned __int16>(3483 - 27 * (128 - byJobNum));
}

unsigned __int16 _darkhole_mission_pass_inform_zocl::size() const
{
  return 17;
}

unsigned __int16 _darkhole_mission_quest_inform_zocl::size() const
{
  return 1;
}

unsigned __int16 _darkhole_new_member_inform_zocl::size() const
{
  return 22;
}

unsigned __int16 _darkhole_new_mission_inform_zocl::size()
{
  if (byJobNum > 128u)
  {
    byJobNum = 0;
  }
  constexpr unsigned __int16 kHeaderSize = 23;
  constexpr unsigned __int16 kJobSize = 24;
  return static_cast<unsigned __int16>(kHeaderSize + kJobSize * byJobNum);
}

unsigned __int16 _darkhole_open_all_portal_by_result_inform_zocl::size() const
{
  return static_cast<unsigned __int16>(byCnt + 1);
}

unsigned __int16 _darkhole_open_portal_by_react_inform_zocl::size() const
{
  return 2;
}

unsigned __int16 _darkhole_open_result_zocl::size() const
{
  return 7;
}

unsigned __int16 _darkhole_quest_info_inform_zocl::size() const
{
  return 41;
}

_darkhole_state_change_zocl::_darkhole_state_change_zocl()
{
  std::memset(this, 0, sizeof(_darkhole_state_change_zocl));
}

unsigned __int16 _darkhole_state_change_zocl::size() const
{
  return 7;
}

unsigned __int16 _darkhole_timeout_inform_zocl::size() const
{
  return 1;
}

unsigned __int16 _guild_battle_suggest_request_result_zocl::size() const
{
  return byRet ? 1 : 18;
}

unsigned __int16 _move_to_own_stonemap_inform_zocl::size() const
{
  return 1;
}

unsigned __int16 _move_to_own_stonemap_result_zocl::size() const
{
  return 8;
}

_not_arranged_char_inform_zocl::_not_arranged_char_inform_zocl() : byCharNum(0)
{
}

__int64 _not_arranged_char_inform_zocl::size()
{
  if (byCharNum > 50)
  {
    byCharNum = 0;
  }
  return 3451 - 69LL * (50 - byCharNum);
}

__int64 _notify_local_time_result_zocl::size() const
{
  return byRet ? 1LL : 37LL;
}

_personal_amine_errmsg_zocl::_personal_amine_errmsg_zocl()
{
  byErrCode = static_cast<unsigned __int8>(-1);
}

__int64 _personal_amine_errmsg_zocl::size() const
{
  return 1;
}

_personal_amine_fixpos_zocl::_personal_amine_fixpos_zocl()
{
  std::memset(this, 0, sizeof(_personal_amine_fixpos_zocl));
}

unsigned __int16 _personal_amine_fixpos_zocl::size() const
{
  return 25;
}

_personal_amine_infoui_open_zocl::_personal_amine_infoui_open_zocl()
{
  std::memset(this, 0, sizeof(_personal_amine_infoui_open_zocl));
}

__int64 _personal_amine_infoui_open_zocl::size() const
{
  return 12;
}

_personal_amine_make_storage_zocl::_personal_amine_make_storage_zocl()
{
  dwTotGold = 0;
}

__int64 _personal_amine_make_storage_zocl::size() const
{
  return 8;
}

_personal_automine_alter_dur_zocl::_personal_automine_alter_dur_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_alter_dur_zocl));
}

unsigned __int16 _personal_automine_alter_dur_zocl::size() const
{
  return 6;
}

_personal_automine_attacked_zocl::_personal_automine_attacked_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_attacked_zocl));
}

unsigned __int16 _personal_automine_attacked_zocl::size() const
{
  return 2;
}

_personal_automine_battery_extract_zocl::_personal_automine_battery_extract_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_battery_extract_zocl));
}

__int64 _personal_automine_battery_extract_zocl::size() const
{
  return 11;
}

_personal_automine_battery_insert_zocl::_personal_automine_battery_insert_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_battery_insert_zocl));
}

__int64 _personal_automine_battery_insert_zocl::size() const
{
  return 10;
}

_personal_automine_delbattery_zocl::_personal_automine_delbattery_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_delbattery_zocl));
}

unsigned __int16 _personal_automine_delbattery_zocl::size() const
{
  return 5;
}

_personal_automine_popore_zocl::_personal_automine_popore_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_popore_zocl));
}

__int64 _personal_automine_popore_zocl::size() const
{
  return 3;
}

_personal_automine_selore_zocl::_personal_automine_selore_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_selore_zocl));
}

__int64 _personal_automine_selore_zocl::size() const
{
  return 5;
}

_personal_automine_stop_zocl::_personal_automine_stop_zocl()
{
  std::memset(this, 0, sizeof(_personal_automine_stop_zocl));
}

__int64 _personal_automine_stop_zocl::size() const
{
  return 11;
}

_post_content_result_zocl::_post_content_result_zocl()
{
  std::memset(this, 0, sizeof(_post_content_result_zocl));
}

unsigned __int16 _post_content_result_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_post_content_result_zocl));
}

unsigned __int16 _post_recv_delivery_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_post_recv_delivery_zocl));
}

_post_result_zocl::_post_result_zocl() : byErrCode(0), dwGold(0)
{
}

__int64 _post_result_zocl::size() const
{
  return 5;
}

unsigned __int16 _post_return_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_post_return_zocl));
}

_pt_automine_getoutore_zocl::_pt_automine_getoutore_zocl()
{
  std::memset(this, 0, sizeof(_pt_automine_getoutore_zocl));
}

__int64 _pt_automine_getoutore_zocl::size() const
{
  return 3;
}

__int64 _pt_automine_result_zocl::size() const
{
  return 2;
}

_pt_automine_state_zocl::_pt_automine_state_zocl()
{
  item = _INVENKEY();
}

__int64 _pt_automine_state_zocl::size() const
{
  return 10;
}

_pt_inform_appoint_zocl::_pt_inform_appoint_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

__int64 _pt_inform_appoint_zocl::size() const
{
  return 18;
}

unsigned __int16 _pt_inform_commission_income_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_pt_inform_commission_income_zocl));
}

_pt_inform_tax_rate_zocl::_pt_inform_tax_rate_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

__int64 _pt_inform_tax_rate_zocl::size() const
{
  return 2;
}

_pt_result_appoint_zocl::_pt_result_appoint_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

__int64 _pt_result_appoint_zocl::size() const
{
  return 27;
}

_pt_result_change_tax_rate_zocl::_pt_result_change_tax_rate_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

__int64 _pt_result_change_tax_rate_zocl::size() const
{
  return 2;
}

_sf_delay_download_result_zocl::_eff_list::_eff_list()
{
  byEffectCode = static_cast<unsigned __int8>(-1);
}

_sf_delay_download_result_zocl::_mas_list::_mas_list()
{
  byEffectCode = static_cast<unsigned __int8>(-1);
}

_sf_delay_download_result_zocl::_sf_delay_download_result_zocl() = default;

_trans_ship_renew_ticket_result_zocl::_trans_ship_renew_ticket_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_change_passwd_result_zocl::_trunk_change_passwd_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_download_result_zocl::_trunk_download_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
  byListNum = 0;
  byExtListNum = 0;
}

unsigned __int16 _trunk_download_result_zocl::size()
{
  if (byRetCode)
  {
    return 1;
  }
  if (byListNum > 100u)
  {
    byListNum = 0;
  }
  return static_cast<unsigned __int16>(3381 - 24 * (100 - byListNum));
}

_trunk_est_result_zocl::_trunk_est_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_extend_result_zocl::_trunk_extend_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_hint_answer_result_zocl::_trunk_hint_answer_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_io_money_result_zocl::_trunk_io_money_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_io_result_zocl::_trunk_io_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_potionsocket_division_result_zocl::_trunk_potionsocket_division_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_pw_hint_index_result_zocl::_trunk_pw_hint_index_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_trunk_res_division_result_zocl::_trunk_res_division_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

_unmannedtrader_buy_item_result_zocl::_unmannedtrader_buy_item_result_zocl()
{
  byRetCode = static_cast<unsigned __int8>(-1);
  byNum = 0;
}

__int64 _unmannedtrader_buy_item_result_zocl::size() const
{
  if (byRetCode && byRetCode != 255)
  {
    return 1;
  }
  if (byNum <= 10)
  {
    return 230 - 22LL * (10 - byNum);
  }
  return 0;
}

__int64 _unmannedtrader_close_item_inform_zocl::size() const
{
  return 11;
}

__int64 _unmannedtrader_continue_item_inform_zocl::size() const
{
  if (byNum > 10)
  {
    const_cast<_unmannedtrader_continue_item_inform_zocl *>(this)->byNum = 0;
  }
  return 102 - 10LL * (10 - byNum);
}

__int64 _unmannedtrader_re_regist_result_zocl::size()
{
  if (byNum > 10)
  {
    byNum = 0;
  }
  return 205 - 20LL * (10 - byNum);
}

_unmannedtrader_Regist_item_inform_zocl::_unmannedtrader_Regist_item_inform_zocl()
{
  byNum = 0;
}

__int64 _unmannedtrader_Regist_item_inform_zocl::size()
{
  if (byNum > 10)
  {
    byNum = 0;
  }
  return 191 - 19LL * (10 - byNum);
}

__int64 _unmannedtrader_regist_item_success_result_zocl::size() const
{
  return 25;
}
