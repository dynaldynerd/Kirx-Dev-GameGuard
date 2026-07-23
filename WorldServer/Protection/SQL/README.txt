================================================================================
 PROTECTION SYSTEM - SQL MIGRATION SCRIPTS
 Reference: 6.23_Guard 2/Server/[SqlAddon]
================================================================================

EXECUTION ORDER:
  Run scripts in numerical order. Each script is idempotent (safe to re-run).
  Always BACKUP your database before running migration scripts.

================================================================================
 SCRIPT INDEX
================================================================================

  001_protection_system_tables.sql   [RF_World] Protection System core tables
    - tbl_protect_hwid               HWID tracking & blocking
    - tbl_protect_cheat_log          Cheat detection event log
    - tbl_protect_transaction_log    Anti-dupe transaction audit
    - tbl_protect_config             DB-driven config overrides

  002_auction_and_economy_fixes.sql  [RF_World] Currency overflow fixes
    - BIGINT migration for Gold/Dalant/TrunkGold/TrunkDalant
    - CHECK constraints (non-negative currency)
    - Cleanup existing negative values

  003_stored_procedures.sql          [RF_World] Protection System procedures
    - sp_protect_hwid_register       Register/update player HWID
    - sp_protect_hwid_block          Block/unblock HWID
    - sp_protect_log_cheat           Log cheat detection event
    - sp_protect_get_blocked_hwids   Load blocked list at startup
    - sp_protect_cheat_report        GM cheat summary report
    - sp_protect_cleanup_logs        Purge old logs (retention)

--- Guard23 Reference Scripts (for future feature development) ---

  004_save_hwid.sql                  [RF_User] HWID on account table
    - lastconnecthwid column on tbl_UserAccount & tbl_StaffAccount
    - pUpdate_HWID_Update procedure
    - pInsert_UserLogoutLog_Daily20070115 (with HWID in daily logs)

  005_new_fix_2022_bigint.sql        [RF_World] Y2038 BIGINT migration
    - tbl_base.LastConnTime
    - tbl_supplement.cristalbattledate, BufEndTime
    - tbl_racebattle_log.endtime
    - tbl_TimeLimitInfo.LastLogoutTime
    - tbl_npc_quest_history.Time1-Time70 (all 70 columns)

  006_auction_fix.sql                [RF_World] Auction/UMT fixes
    - Remove CK_tbl_utsellinfo constraint
    - pUpdate_utitemstate (cancel/expire state reset)
    - pSelect_utsellinfo (interracial auction support)

  007_potion_save.sql                [RF_World] Potion persistence
    - tbl_base.PI0, PD0, PI1, PD1 columns

  008_autocure.sql                   [RF_World] Auto Cure on Macro
    - tbl_base.MCI (Macro Cure Item index)
    - tbl_base.MCV (Macro Cure Value)

  009_guild_point.sql                [RF_World] Guild Point system
    - tbl_Guild.Point column (BIGINT)

  010_instant_revival.sql            [RF_World] Instant Revival cooldown
    - tbl_base.IR0, IR1 columns (BIGINT)

  011_7day_login.sql                 [RF_World] 7-Day Login Rewards
    - tbl_7daylogin table + pSelect_7daylogin procedure

  012_guild_skills.sql               [RF_World] Guild Skill system
    - tbl_guild_skills table (attack/defense/hp_fp/drop_rate/speed)

  013_season_pass.sql                [RF_World] Season/Battle Pass
    - tbl_season_pass table (200 tiers x 2 tracks)
    - pSelect_SeasonPass procedure

  014_billing.sql                    [BILLING] Cash shop procedures
    - RF_AddBalance, RF_ReduceBalance, RF_AddPremium

================================================================================
 DATABASE TARGETS
================================================================================

  RF_World  = Game world database (characters, guilds, items)
  RF_User   = Account database (login, HWID tracking)
  BILLING   = Cash shop / premium billing database

================================================================================
 GUARD23 ORIGINAL FILES MAPPING
================================================================================

  Guard23 File                    ->  Our Script
  -----------------------------------------------
  SAVE_HWID.sql                   ->  004_save_hwid.sql
  NEW_FIX_2022.sql                ->  005_new_fix_2022_bigint.sql
  AUCTION_FIX.sql                 ->  006_auction_fix.sql
  POTION_SAVE.sql                 ->  007_potion_save.sql
  AUTOCURE.sql                    ->  008_autocure.sql
  GUILD_POINT.sql                 ->  009_guild_point.sql
  INSTAN_REVIVAL.sql              ->  010_instant_revival.sql
  7DAY_LOGIN/7DAY_LOGIN.sql       ->  011_7day_login.sql
  7DAY_LOGIN/7DAY_LOGIN_RESET.sql ->  011_7day_login.sql (reset commented)
  GUILD_SKILLS/GUILD_SKILLS.sql   ->  012_guild_skills.sql
  GUILD_SKILLS/GUILD_SKILLS_RESET ->  012_guild_skills.sql (reset commented)
  SEASON_PASS/SEASON_PASS.sql     ->  013_season_pass.sql
  SEASON_PASS/SEASON_PASS_RESET   ->  013_season_pass.sql (reset commented)
  BILLING/BILLING_TYPE_0_*.sql    ->  014_billing.sql
  BILLING/BILLING_TYPE_1_*.sql    ->  014_billing.sql (Type 1 uses same procs)
