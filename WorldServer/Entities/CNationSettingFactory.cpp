#include "pch.h"

#include "CNationSettingFactory.h"
#include "CheatCommands.h"
#include "CNationSettingData.h"

#include <algorithm>

namespace
{
void SortCheatCommands(std::vector<CHEAT_COMMAND> &commands)
{
  CHEAT_COMMAND comparator{};
  std::sort(
    commands.begin(),
    commands.end(),
    [&comparator](const CHEAT_COMMAND &lhs, const CHEAT_COMMAND &rhs)
    {
      return comparator(&lhs, &rhs);
    });
}
} // namespace

CNationSettingFactory::CNationSettingFactory(int iType)
  : m_iType(iType)
{
}

bool CNationSettingFactory::IsNULL()
{
  // this is not a stub
  return false;
}

int CNationSettingFactory::GetKey()
{
  return m_iType;
}

bool CNationSettingFactory::RegistCheatTable(CNationSettingData *pkData)
{
  if (!RegistCheatTableUnion(pkData))
  {
    return false;
  }
  if (!RegistCheatTableOnlyInternal(pkData))
  {
    return false;
  }
  RegistCheatEndRecord(pkData);
  return true;
}

bool CNationSettingFactory::RegistCheatTableUnion(CNationSettingData *pkData)
{
  if (!RegistCheat(pkData, "change degree", ct_change_degree, 31, 7))
    return false;
  if (!RegistCheat(pkData, "respawn start", ct_respawn_start, 52, 7))
    return false;
  if (!RegistCheat(pkData, "respawn stop", ct_respawn_stop, 52, 7))
    return false;
  if (!RegistCheat(pkData, "cimoki", ct_circle_mon_kill, 20, 6))
    return false;
  if (!RegistCheat(pkData, "lolibo", ct_look_like_boss, 4, 6))
    return false;
  if (!RegistCheat(pkData, "chma", ct_change_mastery, 28, 7))
    return false;
  if (!RegistCheat(pkData, "alitmu", ct_all_item_muzi, 28, 6))
    return false;
  if (!RegistCheat(pkData, "allmuzi", ct_all_item_muzi, 28, 6))
    return false;
  if (!RegistCheat(pkData, "recrhp", ct_report_cri_hp, 28, 7))
    return false;
  if (!RegistCheat(pkData, "repo", ct_report_position, 28, 7))
    return false;
  if (!RegistCheat(pkData, "xyz", ct_report_position, 28, 7))
      return false;
  if (!RegistCheat(pkData, "debug", ct_debug, 28, 7))
      return false;
  if (!RegistCheat(pkData, "alex", ct_alter_exp, 28, 7))
    return false;
  if (!RegistCheat(pkData, "altexp", ct_alter_exp, 28, 7))
    return false;
  if (!RegistCheat(pkData, "allmap", ct_all_map, 28, 7))
    return false;
  if (!RegistCheat(pkData, "jump", ct_jump_to_pos, 28, 7))
    return false;
  if (!RegistCheat(pkData, "padu", ct_pass_dungeon, 20, 7))
    return false;
  if (!RegistCheat(pkData, "pass dungeon", ct_pass_dungeon, 20, 7))
    return false;
  if (!RegistCheat(pkData, "trhi", ct_tracing_hide, 28, 7))
    return false;
  if (!RegistCheat(pkData, "transparent", ct_tracing_hide, 28, 7))
    return false;
  if (!RegistCheat(pkData, "trsh", ct_tracing_show, 28, 7))
    return false;
  if (!RegistCheat(pkData, "no transparent", ct_tracing_show, 28, 7))
    return false;
  if (!RegistCheat(pkData, "max attack point", ct_max_attack, 28, 7))
    return false;
  if (!RegistCheat(pkData, "min attack point", ct_min_attack, 28, 7))
    return false;
  if (!RegistCheat(pkData, "normal attack point", ct_mormal_attack, 28, 7))
    return false;
  if (!RegistCheat(pkData, "sema", ct_set_matchless, 28, 7))
    return false;
  if (!RegistCheat(pkData, "matchless", ct_set_matchless, 28, 7))
    return false;
  if (!RegistCheat(pkData, "rema", ct_release_matchless, 28, 7))
    return false;
  if (!RegistCheat(pkData, "no matchless", ct_release_matchless, 28, 7))
    return false;
  if (!RegistCheat(pkData, "goshto", ct_goto_shipport_town, 28, 7))
    return false;
  if (!RegistCheat(pkData, "port", ct_goto_shipport_town, 28, 7))
    return false;
  if (!RegistCheat(pkData, "goshed", ct_goto_shipport_eder, 28, 7))
    return false;
  if (!RegistCheat(pkData, "eder", ct_goto_shipport_eder, 28, 7))
    return false;
  if (!RegistCheat(pkData, "frrish", ct_free_ride_ship, 28, 7))
    return false;
  if (!RegistCheat(pkData, "goto ship", ct_free_ride_ship, 28, 7))
    return false;
  if (!RegistCheat(pkData, "pasc", ct_pass_sch, 52, 4))
    return false;
  if (!RegistCheat(pkData, "stcr", ct_start_cri, 52, 6))
    return false;
  if (!RegistCheat(pkData, "start holy", ct_start_cri, 52, 6))
    return false;
  if (!RegistCheat(pkData, "stone", ct_start_cri, 52, 6))
    return false;
  if (!RegistCheat(pkData, "exke", ct_exip_keeper, 52, 6))
    return false;
  if (!RegistCheat(pkData, "stone bye", ct_exip_keeper, 52, 6))
    return false;
  if (!RegistCheat(pkData, "keeper bye", ct_exip_keeper, 52, 6))
    return false;
  if (!RegistCheat(pkData, "stke", ct_start_keeper, 52, 6))
    return false;
  if (!RegistCheat(pkData, "servant", ct_start_keeper, 52, 6))
    return false;
  if (!RegistCheat(pkData, "start keeper", ct_start_keeper, 52, 6))
    return false;
  if (!RegistCheat(pkData, "gost", ct_goto_stone, 28, 7))
    return false;
  if (!RegistCheat(pkData, "goto stone", ct_goto_stone, 28, 7))
    return false;
  if (!RegistCheat(pkData, "exst", ct_exit_stone, 52, 6))
    return false;
  if (!RegistCheat(pkData, "selofr", ct_set_loot_free, 20, 7))
    return false;
  if (!RegistCheat(pkData, "beggar", ct_set_loot_free, 20, 7))
    return false;
  if (!RegistCheat(pkData, "relofr", ct_release_loot_free, 20, 7))
    return false;
  if (!RegistCheat(pkData, "no beggar", ct_release_loot_free, 20, 7))
    return false;
  if (!RegistCheat(pkData, "#", ct_change_class, 28, 7))
    return false;
  if (!RegistCheat(pkData, "kick", ct_kick_player, 52, 7))
    return false;
  if (!RegistCheat(pkData, "coqu", ct_complete_quest, 28, 7))
    return false;
  if (!RegistCheat(pkData, "pass quest", ct_complete_quest, 28, 7))
    return false;
  if (!RegistCheat(pkData, "charcall", ct_recall_player, 22, 7))
    return false;
  if (!RegistCheat(pkData, "moncall", ct_recall_monster, 28, 7))
    return false;
  if (!RegistCheat(pkData, "gomi", ct_goto_mine, 28, 7))
    return false;
  if (!RegistCheat(pkData, "goto mine", ct_goto_mine, 28, 7))
    return false;
  if (!RegistCheat(pkData, "coav", ct_copy_avator, 28, 4))
    return false;
  if (!RegistCheat(pkData, "copy", ct_copy_avator, 28, 4))
    return false;
  if (!RegistCheat(pkData, "usnu", ct_user_num, 28, 7))
    return false;
  if (!RegistCheat(pkData, "total", ct_user_num, 28, 7))
    return false;
  if (!RegistCheat(pkData, "die", ct_die, 28, 7))
    return false;
  if (!RegistCheat(pkData, "halfgauge", ct_half_gauge, 28, 7))
    return false;
  if (!RegistCheat(pkData, "miracle", ct_half_gauge, 28, 7))
    return false;
  if (!RegistCheat(pkData, "fuga", ct_full_gauge, 28, 7))
    return false;
  if (!RegistCheat(pkData, "fullmiracle", ct_full_gauge, 28, 7))
    return false;
  if (!RegistCheat(pkData, "alindu", ct_alter_inven_dur, 28, 7))
    return false;
  if (!RegistCheat(pkData, "senedi", ct_set_never_die, 28, 7))
    return false;
  if (!RegistCheat(pkData, "neverdie", ct_set_never_die, 28, 7))
    return false;
  if (!RegistCheat(pkData, "renedi", ct_release_never_die, 28, 7))
    return false;
  if (!RegistCheat(pkData, "no neverdie", ct_release_never_die, 28, 7))
    return false;
  if (!RegistCheat(pkData, "frsfbycl", ct_free_sf_by_class, 28, 7))
    return false;
  if (!RegistCheat(pkData, "lv", ct_alter_lv, 30, 7))
    return false;
  if (!RegistCheat(pkData, "upfcitem", ct_up_forceitem, 28, 4))
    return false;
  if (!RegistCheat(pkData, "upskill", ct_up_skill, 28, 4))
    return false;
  if (!RegistCheat(pkData, "upforce", ct_up_forcemastery, 28, 4))
    return false;
  if (!RegistCheat(pkData, "allskill", ct_up_allskill, 28, 7))
    return false;
  if (!RegistCheat(pkData, "alpv", ct_alter_pvp, 28, 4))
    return false;
  if (!RegistCheat(pkData, "contribute_p", ct_alter_pvp, 28, 4))
    return false;
  if (!RegistCheat(pkData, "alca", ct_alter_cashbag, 28, 4))
    return false;
  if (!RegistCheat(pkData, "contribute_m", ct_alter_cashbag, 28, 4))
    return false;
  if (!RegistCheat(pkData, "recalltime", ct_animus_recall_term, 28, 4))
    return false;
  if (!RegistCheat(pkData, "recallexp", ct_set_animus_exp, 28, 7))
    return false;
  if (!RegistCheat(pkData, "monset", ct_init_monster, 52, 4))
    return false;
  if (!RegistCheat(pkData, "fufo", ct_full_force, 28, 4))
    return false;
  if (!RegistCheat(pkData, "full force", ct_full_force, 28, 4))
    return false;
  if (!RegistCheat(pkData, "loma", ct_loot_material, 28, 4))
    return false;
  if (!RegistCheat(pkData, "loto", ct_loot_tower, 28, 4))
    return false;
  if (!RegistCheat(pkData, "loba", ct_loot_bag, 28, 4))
    return false;
  if (!RegistCheat(pkData, "threeseven", ct_loot_bag, 28, 4))
    return false;
  if (!RegistCheat(pkData, "lomi", ct_loot_mine, 28, 4))
    return false;
  if (!RegistCheat(pkData, "inem", ct_inven_empty, 28, 7))
    return false;
  if (!RegistCheat(pkData, "clear inven", ct_inven_empty, 28, 7))
    return false;
  if (!RegistCheat(pkData, "semasu", ct_set_make_succ, 28, 7))
    return false;
  if (!RegistCheat(pkData, "god hand", ct_set_make_succ, 28, 7))
    return false;
  if (!RegistCheat(pkData, "remasu", ct_release_make_succ, 28, 7))
    return false;
  if (!RegistCheat(pkData, "chicken hand", ct_release_make_succ, 28, 7))
    return false;
  if (!RegistCheat(pkData, "coefti", ct_cont_effet_time, 28, 4))
    return false;
  if (!RegistCheat(pkData, "effect time", ct_cont_effet_time, 28, 4))
    return false;
  if (!RegistCheat(pkData, "coefcl", ct_cont_effet_clear, 28, 7))
    return false;
  if (!RegistCheat(pkData, "effect clear", ct_cont_effet_clear, 28, 7))
    return false;
  if (!RegistCheat(pkData, "vime", ct_view_method, 28, 7))
    return false;
  if (!RegistCheat(pkData, "masyto", ct_make_system_tower, 16, 7))
    return false;
  if (!RegistCheat(pkData, "desyto", ct_destroy_system_tower, 16, 7))
    return false;
  if (!RegistCheat(pkData, "ciusnu", ct_circle_user_num, 28, 7))
    return false;
  if (!RegistCheat(pkData, "circle", ct_circle_user_num, 28, 7))
    return false;
  if (!RegistCheat(pkData, "bosmca", ct_boss_sms_cancel, 36, 7))
    return false;
  if (!RegistCheat(pkData, "trin", ct_trunk_init, 36, 7))
    return false;
  if (!RegistCheat(pkData, "fullset", ct_fullset, 30, 7))
    return false;
  if (!RegistCheat(pkData, "loup", ct_loot_upgrade, 28, 4))
    return false;
  if (!RegistCheat(pkData, "lodu", ct_loot_dungeon, 28, 6))
    return false;
  if (!RegistCheat(pkData, "deitgr", ct_defense_item_grace, 28, 7))
    return false;
  if (!RegistCheat(pkData, "dfgrace", ct_defense_item_grace_Jp, 28, 7))
    return false;
  if (!RegistCheat(pkData, "recallattack", ct_animus_attack_grade, 28, 7))
    return false;
  if (!RegistCheat(pkData, "te", ct_telekinesis, 28, 7))
    return false;
  if (!RegistCheat(pkData, "allskillpt", ct_up_allskill_pt, 28, 7))
    return false;
  if (!RegistCheat(pkData, "coquot", ct_complete_quest_other, 30, 7))
    return false;
  if (!RegistCheat(pkData, "loupit", ct_loot_upgrade_item, 28, 7))
    return false;
  if (!RegistCheat(pkData, "resurrectkr", ct_resurrect_player, 28, 7))
    return false;
  if (!RegistCheat(pkData, "resurrect", ct_resurrect_player, 28, 7))
    return false;
  if (!RegistCheat(pkData, "adgusc", ct_add_guild_schedule, 8, 7))
    return false;
  if (!RegistCheat(pkData, "seguco", ct_set_guildbattle_color, 8, 7))
    return false;
  if (!RegistCheat(pkData, "cuguco", ct_cur_guildbattle_color, 8, 7))
    return false;
  if (!RegistCheat(pkData, "createfield", ct_create_guildbattle_field_object, 8, 7))
    return false;
  if (!RegistCheat(pkData, "destroyfield", ct_destroy_guildbattle_field_object, 8, 7))
    return false;
  if (!RegistCheat(pkData, "regenstone", ct_regen_gravitystone, 8, 7))
    return false;
  if (!RegistCheat(pkData, "destroystone", ct_destroy_gravitystone, 8, 7))
    return false;
  if (!RegistCheat(pkData, "takestone", ct_take_gravitystone, 8, 7))
    return false;
  if (!RegistCheat(pkData, "getstone", ct_get_gravitystone, 8, 7))
    return false;
  if (!RegistCheat(pkData, "dropstone", ct_drop_gravitystone, 8, 7))
    return false;
  if (!RegistCheat(pkData, "forcestone", ct_guild_battle_force_stone, 8, 7))
    return false;
  if (!RegistCheat(pkData, "checkgoal", ct_check_guild_batlle_goal, 8, 7))
    return false;
  if (!RegistCheat(pkData, "recvschedule", ct_recv_reserved_schedulelist, 8, 7))
    return false;
  if (!RegistCheat(pkData, "recvbattleinfo", ct_recv_current_battle_info, 8, 7))
    return false;
  if (!RegistCheat(pkData, "adondagusc", ct_add_one_day_guild_schedule, 8, 7))
    return false;
  if (!RegistCheat(pkData, "gusu", ct_guild_suggest, 8, 7))
    return false;
  if (!RegistCheat(pkData, "guin", ct_guild_info, 8, 7))
    return false;
  if (!RegistCheat(pkData, "recvtotalguildrank", ct_recv_total_guild_rank, 8, 7))
    return false;
  if (!RegistCheat(pkData, "recvpvpguildrank", ct_recv_pvp_guild_rank, 8, 7))
    return false;
  if (!RegistCheat(pkData, "changetaxrate", ct_recv_change_atrad_taxrate, 8, 7))
    return false;
  if (!RegistCheat(pkData, "coexre", ct_combine_ex_result, 8, 7))
    return false;
  if (!RegistCheat(pkData, "amp-set", ct_amp_set, 52, 7))
    return false;
  if (!RegistCheat(pkData, "amp-full", ct_amp_full, 52, 7))
    return false;
  if (!RegistCheat(pkData, "servertime", ct_server_time, 52, 7))
    return false;
  if (!RegistCheat(pkData, "partycall", ct_party_call, 52, 7))
    return false;
  if (!RegistCheat(pkData, "guildcall", ct_guild_call, 52, 7))
    return false;
  if (!RegistCheat(pkData, "cashamount", ct_loadcashamount, 20, 7))
    return false;
  if (!RegistCheat(pkData, "csbuy", ct_cashitembuy, 20, 7))
    return false;
  if (!RegistCheat(pkData, "primium", ct_PcBandPrimium, 20, 7))
    return false;
  if (!RegistCheat(pkData, "classevent", ct_ClassRefineEvent, 20, 7))
    return false;
  if (!RegistCheat(pkData, "takeholymental", ct_takeholymental, 52, 7))
    return false;
  if (g_Main.IsReleaseServiceMode())
  {
    if (!RegistCheat(pkData, "holy", ct_HolySystem, 20, 7))
      return false;
    if (!RegistCheat(pkData, "war", ct_HolySystem_Jp, 52, 7))
      return false;
    if (!RegistCheat(pkData, "keeperattack", ct_HolyKeeperAttack, 20, 7))
      return false;
  }
  else
  {
    if (!RegistCheat(pkData, "hosyde", ct_HolySystem, 52, 7))
      return false;
    if (!RegistCheat(pkData, "war_dev", ct_HolySystem_Jp, 52, 7))
      return false;
    if (!RegistCheat(pkData, "hokeatde", ct_HolyKeeperAttack, 52, 7))
      return false;
  }
  if (!RegistCheat(pkData, "sepaau", ct_SetPatriarchAuto, 4, 7))
    return false;
  if (!RegistCheat(pkData, "sepapr", ct_SetPatriarchProcessor, 4, 7))
    return false;
  if (!RegistCheat(pkData, "inpapr", ct_InformPatriarchProcessor, 4, 7))
    return false;
  if (!RegistCheat(pkData, "pvliin", ct_PvpLimitInit, 52, 7))
    return false;
  if (!RegistCheat(pkData, "recalllv", ct_set_animus_lv, 28, 7))
    return false;
  if (!RegistCheat(pkData, "fuanga", ct_full_animus_gauge, 28, 7))
    return false;
  if (!RegistCheat(pkData, "AniPow", ct_full_animus_gauge, 28, 7))
    return false;
  if (!RegistCheat(pkData, "nuafef", ct_NuAfterEffect, 52, 7))
    return false;
  if (!RegistCheat(pkData, "AfterEff", ct_NuAfterEffect, 52, 7))
    return false;
  if (!RegistCheat(pkData, "cden", ct_CdeEndup, 28, 7))
    return false;
  if (!RegistCheat(pkData, "removesfdelay", ct_remove_sf_delay, 28, 7))
    return false;
  if (!RegistCheat(pkData, "utcancellogout", ct_ut_cancel_registlogout, 28, 7))
    return false;
  if (!RegistCheat(pkData, "utcancel", ct_ut_cancel_regist, 28, 7))
    return false;
  if (!RegistCheat(pkData, "caevst", ct_CashEventStart, 28, 7))
    return false;
  if (!RegistCheat(pkData, "chatsave", ct_chatsave, 28, 7))
    return false;
  if (!RegistCheat(pkData, "drop", ct_drop_jade, 28, 7))
    return false;
  if (!RegistCheat(pkData, "oreamount", ct_set_ore_amount, 28, 4))
    return false;
  if (!RegistCheat(pkData, "remainore", ct_query_remain_ore, 28, 7))
    return false;
  if (!RegistCheat(pkData, "coplin", ct_continue_palytime_inc, 28, 7))
    return false;
  if (!RegistCheat(pkData, "serverrate", ct_server_rate, 28, 7))
    return false;
  if (!RegistCheat(pkData, "premiumrate", ct_premium_rate, 28, 7))
    return false;
  if (!RegistCheat(pkData, "trapattack", ct_trap_attack_grade, 28, 7))
    return false;
  if (!RegistCheat(pkData, "hitme", ct_set_damage_part, 28, 7))
    return false;
  if (!RegistCheat(pkData, "eventset start", ct_eventset_start, 52, 7))
    return false;
  if (!RegistCheat(pkData, "eventset stop", ct_eventset_stop, 52, 7))
    return false;
  if (!RegistCheat(pkData, "temp point", ct_set_temp_cash_point, 28, 7))
    return false;
  if (!RegistCheat(pkData, "new killerlist", ct_set_kill_list_init, 28, 7))
    return false;
  if (!RegistCheat(pkData, "buf", ct_buf_potion_use, 28, 7))
    return false;
  if (!RegistCheat(pkData, "lua", ct_lua_command, 20, 7))
    return false;
  if (!RegistCheat(pkData, "userchatban", ct_userchatban, 20, 7))
    return false;
  if (!RegistCheat(pkData, "itemloot", ct_itemloot, 16, 7))
    return false;
  if (!RegistCheat(pkData, "minespeed", ct_minespeed, 16, 7))
    return false;
  if (!RegistCheat(pkData, "sfmastery", ct_sfmastery, 16, 7))
    return false;
  if (!RegistCheat(pkData, "basemastery", ct_basemastery, 16, 7))
    return false;
  if (!RegistCheat(pkData, "animuexp", ct_animusexp, 16, 7))
    return false;
  if (!RegistCheat(pkData, "playerexp", ct_playerexp, 16, 7))
    return false;
  if (!RegistCheat(pkData, "darkholereward", ct_darkholereward, 16, 7))
    return false;
  if (!RegistCheat(pkData, "pcitemloot", ct_pcitemloot, 16, 7))
    return false;
  if (!RegistCheat(pkData, "pcminespeed", ct_pcminespeed, 16, 7))
    return false;
  if (!RegistCheat(pkData, "pcsfmastery", ct_pcsfmastery, 16, 7))
    return false;
  if (!RegistCheat(pkData, "pcbasemastery", ct_pcbasemastery, 16, 7))
    return false;
  if (!RegistCheat(pkData, "pcanimuexp", ct_pcanimusexp, 16, 7))
    return false;
  if (!RegistCheat(pkData, "pcplayerexp", ct_pcplayerexp, 16, 7))
    return false;
  if (!RegistCheat(pkData, "mepcbang", ct_mepcbang, 28, 7))
    return false;
  if (!RegistCheat(pkData, "pcbangitemget", ct_pcbangitemget, 28, 7))
    return false;
  if (!RegistCheat(pkData, "expirepcbang", ct_expire_pcbang, 28, 7))
    return false;
  if (!RegistCheat(pkData, "voteenable", ct_vote_enable, 28, 7))
    return false;
  if (!RegistCheat(pkData, "voteinfo", ct_elect_info_player, 28, 7))
    return false;
  if (!RegistCheat(pkData, "playerset", ct_elect_set_player, 28, 7))
    return false;
  if (!RegistCheat(pkData, "envset", ct_elect_set_env, 28, 7))
    return false;
  if (!RegistCheat(pkData, "timeset", ct_period_time_set, 28, 7))
    return false;
  if (!RegistCheat(pkData, "tlinfoset", ct_tl_info_set, 28, 7))
    return false;
  if (!RegistCheat(pkData, "tlinfo", ct_tl_info_view, 28, 7))
    return false;
  if (!RegistCheat(pkData, "tlsysset", ct_tl_system_setting, 28, 7))
    return false;
  if (!RegistCheat(pkData, "actpset", ct_action_point_set, 28, 7))
    return false;
  if (!RegistCheat(pkData, "wira", ct_Win_RaceWar, 28, 7))
    return false;
  if (!RegistCheat(pkData, "eventst", ct_Gold_Age_Event_Status, 28, 7))
    return false;
  if (!RegistCheat(pkData, "setevent", ct_Gold_Age_Set_Event_Status, 28, 7))
    return false;
  if (RegistCheat(pkData, "getboxcnt", ct_Gold_Age_Get_Box_Cnt, 28, 7))
  {
    return RegistCheat(pkData, "sethp", ct_set_hp, 28, 7)
        && RegistCheat(pkData, "hfs full", ct_set_hfs_full, 28, 7);
  }
  return false;
}

bool CNationSettingFactory::RegistCheatTableOnlyInternal(CNationSettingData *pkData)
{
  g_Main.IsReleaseServiceMode();

  if (!RegistCheat(pkData, "chdein", ct_change_degree, 28, 7))
    return false;
  if (!RegistCheat(pkData, "show me the dalant", ct_alter_dalant, 30, 7))
    return false;
  if (!RegistCheat(pkData, "show me the gold", ct_alter_gold, 30, 7))
    return false;
  if (!RegistCheat(pkData, "sepacl", ct_SetPatriarchClear, 52, 7))
    return false;
  if (!RegistCheat(pkData, "sepagr", ct_SetPatriarchGroup, 52, 7))
    return false;
  if (!RegistCheat(pkData, "BossMe", ct_SetPatriarchGroup, 52, 7))
    return false;
  if (!RegistCheat(pkData, "incrbabeho", ct_InformCristalBattleBeforeAnHour, 12, 7))
    return false;
  if (!RegistCheat(pkData, "OneHourAfter", ct_InformCristalBattleBeforeAnHour, 12, 7))
    return false;
  if (!RegistCheat(pkData, "seseowgu", ct_SetSettleOwnerGuild, 52, 7))
    return false;
  if (!RegistCheat(pkData, "Set Settle Owner Guild", ct_SetSettleOwnerGuild, 52, 7))
    return false;
  if (!RegistCheat(pkData, "clseowgu", ct_ClearSettleOwnerGuild, 52, 7))
    return false;
  if (!RegistCheat(pkData, "Clear Settle Owner Guild", ct_ClearSettleOwnerGuild, 52, 7))
    return false;
  if (!RegistCheat(pkData, "repu", ct_ReqPunishment, 52, 7))
    return false;
  if (!RegistCheat(pkData, "rechhogu", ct_ReqChangeHonorGuild, 52, 7))
    return false;
  if (!RegistCheat(pkData, "gotochar", ct_goto_char, 52, 7))
    return false;
  if (!RegistCheat(pkData, "gotomon", ct_goto_monster, 28, 7))
    return false;
  if (!RegistCheat(pkData, "cdst", ct_CdeStart, 28, 7))
    return false;
  if (!RegistCheat(pkData, "exception", ct_exception, 16, 0))
    return false;
  if (!RegistCheat(pkData, "premium", ct_pcroom_premium, 28, 7))
    return false;
  if (!RegistCheat(pkData, "segugr", ct_SetGuildGrade, 12, 7))
    return false;
  if (!RegistCheat(pkData, "Set Guild Grade", ct_SetGuildGrade, 12, 7))
    return false;
  if (!RegistCheat(pkData, "segugrbyna", ct_SetGuildGradeByName, 12, 7))
    return false;
  if (!RegistCheat(pkData, "Set Guild Grade By Name", ct_SetGuildGradeByName, 12, 7))
    return false;
  if (!RegistCheat(pkData, "segugrbygu", ct_SetGuildGradeByGuildSerial, 12, 7))
    return false;
  if (!RegistCheat(
        pkData,
        "Set Guild Grade By GuildSerial",
        ct_SetGuildGradeByGuildSerial,
        12,
        7))
  {
    return false;
  }
  if (!RegistCheat(pkData, "seguma", ct_SetGuildMaster, 12, 7))
    return false;
  if (!RegistCheat(pkData, "MasterMe", ct_SetGuildMaster, 12, 7))
    return false;
  if (!RegistCheat(pkData, "semaleli", ct_SetMaxLevelLimit, 12, 7))
    return false;
  if (!RegistCheat(pkData, "altlv", ct_SetMaxLevelLimit, 12, 7))
    return false;
  if (!RegistCheat(pkData, "setexprate", ct_set_exp_rate, 28, 7))
    return false;
  if (!RegistCheat(pkData, "gotonpc", ct_goto_npc, 28, 7))
    return false;
  if (!RegistCheat(pkData, "npcquest", ct_request_npc_quest, 28, 7))
    return false;
  if (!RegistCheat(pkData, "delquest", ct_request_delete_quest, 28, 7))
    return false;
  if (!RegistCheat(pkData, "manageguild", ct_manage_guild, 28, 7))
    return false;
  if (!RegistCheat(pkData, "masterelect", ct_change_master_elect, 28, 7))
    return false;
  if (!RegistCheat(pkData, "relpun", ct_release_punishment, 28, 7))
    return false;
  if (RegistCheat(pkData, "conevent", ct_ConEventStart, 28, 7))
    return RegistCheat(pkData, "*", ct_loot_item, 28, 7);
  return false;
}

void CNationSettingFactory::RegistCheatEndRecord(CNationSettingData *pkData)
{
  CHEAT_COMMAND record{};
  record.pwszCommand = nullptr;
  record.uiCmdLen = 0;
  record.fn = nullptr;
  record.nUseDegree = 0;
  record.nMgrDegree = 0;
  pkData->m_vecCheatData.push_back(record);
  SortCheatCommands(pkData->m_vecCheatData);
}

bool CNationSettingFactory::RegistCheat(
  CNationSettingData *pkData,
  const char *szCheat,
  bool(__fastcall *pCheatCommandFn)(CPlayer *),
  int iUseDegree,
  int iMgrDegree)
{
  if (pkData && szCheat && pCheatCommandFn && iUseDegree)
  {
    if (IsExistCheat(szCheat, pkData))
    {
      MyMessageBox(
        "CNationSettingFactory::RegistChest : ",
        "Nation(%s) Cheat(%s) Already Exist!",
        pkData->m_szNationCodeStr,
        szCheat);
      return false;
    }

    CHEAT_COMMAND record{};
    record.pwszCommand = szCheat;
    record.uiCmdLen = static_cast<unsigned int>(strlen_0(szCheat));
    record.fn = pCheatCommandFn;
    record.nUseDegree = iUseDegree;
    record.nMgrDegree = iMgrDegree;
    pkData->m_vecCheatData.push_back(record);
    SortCheatCommands(pkData->m_vecCheatData);
    return true;
  }

  MyMessageBox(
    "CNationSettingFactory::RegistChest : ",
    "0 == pkData(%p) || 0 == szCheat(%s) || 0 == pCheatCommandFn(%p) Invalid!",
    pkData,
    szCheat,
    pCheatCommandFn);
  return false;
}

bool CNationSettingFactory::IsExistCheat(const char *szCheat, CNationSettingData *pkData)
{
  size_t maxCount = 0;
  const size_t cheatLen = strlen_0(szCheat);

  for (const auto &entry : pkData->m_vecCheatData)
  {
    maxCount = entry.uiCmdLen <= cheatLen ? cheatLen : entry.uiCmdLen;
    if (!_strnicmp(entry.pwszCommand, szCheat, maxCount))
    {
      return true;
    }
  }
  return false;
}
