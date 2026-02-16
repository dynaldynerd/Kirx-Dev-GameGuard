# CMainThread::OnDQSRun DB Query Audit (manual, case-by-case)

Scope:
- Source switch: `WorldServer/Entities/CMainThread_DQS.cpp`
- SQL sources: `WorldServer/Entities/CMainThread_DB*.cpp`, `WorldServer/Entities/CRFWorldDatabase*.cpp`, and case-specific managers
- Reference: `ida_static_data.csv`

Status legend:
- `ok`: SQL string(s) match IDA CSV value(s)
- `mismatch`: SQL string differs from IDA CSV
- `dynamic`: query built dynamically; compared by format/prefix where possible
- `n/a`: no DB query in this case
- `pending`: not checked yet

| case | handler in OnDQSRun | db/query entry points | status | notes |
|---|---|---|---|---|
| 0 | `db_Reged_Avator` | `Select_CharacterBaseInfoBySerial`, `Select_NotArrangeCharacter` | ok | matched CSV: `pSelect_CharacterBaseInfoBySerial_20061115`, `pSelect_NotArrangeCharacter` |
| 1 | `db_Insert_Avator` | `Select_Equal_Name`, `Select_Equal_DeleteName_NoArranged`, `Insert_CharacterData`, `Insert_MacroData` | ok | matched CSV for all `pInsert_*` calls + macro insert format |
| 2 | `db_Delete_Avator` | `Delete_CharacterData` | ok | matched CSV: `pDelete_Base`, `pUpdate_DeleteName_Step1`, `pUpdate_DeleteName_Step2` |
| 3 | `db_Load_Avator` | many `_db_Load_*` + `Select_CheckSumValue`, `Select_ItemCharge`, `Select_AccountItemCharge` | ok | manual check: key login SQL matched IDA (`select FirstConnTime...`, `pSelect_ItemCharge_20070130`, trunk item-charge procs) |
| 5 | `db_Update_Avator`, `db_Update_PostStorage`, `_db_update_event_classrefine`, `_db_Update_SFDelayData` | `Update_CharacterData` + dynamic update builders | dynamic | matched IDA for `Update_CharacterData` path + post/classrefine/SFDelay literals; per-field update SQL is builder-based |
| 6 | `db_Update_Avator`, `db_Update_PostStorage`, `_db_update_event_classrefine`, `_db_Update_SFDelayData` | same as case 5 | dynamic | same SQL path as case 5 |
| 7 | `db_Log_AvatorLevel` | `Insert_Level_Log` | ok | matched CSV: `{ CALL pInsert_Level_Log( %d, %d, %d ) }` |
| 8 | `db_Log_UserNum` | `Insert_UserNum_Log` | ok | matched CSV: `{ CALL pInsert_UserNum_Log( %d, %d ) }` |
| 9 | `db_Insert_Economy_History` | `Insert_Economy_History` | ok | matched CSV insert/update economy history call format |
| 10 | `db_Insert_CharacSelect_Log` | `InsertCharacterSelectLog` | ok | matched CSV insert into `tbl_characterselect_log_%04d%02d` |
| 11 | `db_Update_PvpInfo` | `Update_PvpPointInfo` | ok | matched CSV: `{ CALL pUpdate_PvpPointInfo( %d, %d, %d, %d, %f ) }` |
| 12 | `db_Update_Avator` (+ refine/poststorage) | same as case 5 | dynamic | same SQL path as case 5 |
| 13 | `db_Add_PvpPoint` | `Add_PvpPoint` | ok | matched CSV: `{ CALL pAdd_PvpPoint( %d, %d, %d ) }` |
| 14 | `db_Insert_Item` | `Insert_ItemChargeInGame` | ok | matched CSV: `{ CALL pInsert_ItemChargeInGame( %u, %d, %d, %d, %u ) }` |
| 15 | `db_Insert_guild` | `Insert_Guild`, `Select_GuildSerial`, `Update_UserGuildData_20070614`, `Update_GuildMemberCount` | ok | matched CSV call/update strings |
| 16 | `db_update_guildmember_add` | `Update_UserGuildData_20070614`, `Update_GuildMemberCount` | ok | matched CSV call/update strings |
| 17 | `db_update_guildmember_del` | `Update_UserGuildData_20070614`, `Update_GuildMemberCount` | ok | matched CSV call/update strings |
| 18 | `db_update_guildmember_del` | same as case 17 | ok | same SQL as case 17 |
| 19 | `db_input_guild_money` | `Update_InputGuildMoney`, `Select_GuildData`, `Insert_GuildMoneyHistory_Log` | ok | matched CSV for guild money update/history |
| 20 | `db_output_guild_money` | `Update_SetGuildMoney`, `Select_GuildData`, `Insert_GuildMoneyHistory_Log` | ok | matched CSV for guild money set/history |
| 21 | `db_buy_emblem` | `Update_GuildEmblem`, `Select_GuildData`, `Insert_GuildMoneyHistory_Log` | ok | matched CSV for emblem update/history |
| 22 | `db_disjoint_guild` | `Delete_Guild` | ok | matched CSV: `{ CALL pUpdate_DeleteGuild(%d) }` |
| 23 | `db_char_set_alive` | `Select_CharNumInWorld`, `Select_ArrangeInfo`, `Select_Equal_Name`, `Select_Equal_DeleteName_NoArranged`, `Update_SetActive`, `Update_CharSlot`, `Select_CharacterBaseInfo` | ok | matched CSV for all listed calls + `Update_CharSlot` inline selects/updates |
| 24 | `db_Insert_ChangeClass_AfterInitClass` | `InsertChangeClassLogAfterInitClass` | ok | matched CSV class-log call format |
| 25 | `db_sendwebracebosssms` | `Select_ChracterSerialRace` | ok | matched CSV: `{ CALL pSelect_CharacterSerialRace('%s') }` |
| 30 | `CGuildBattleController::AddSchedule` | guild battle DB methods | ok | matched CSV: `pUpdate_ReservedGuildBattleInfo`, `pUpdate_ScheduleInfo` |
| 31 | `CGuildBattleController::UpdateWinLose` | guild battle DB methods | ok | matched CSV: `pUpdate_WinGuildBattleRank`, `pUpdate_LoseGuildBattleRank`, plus record check SQL |
| 32 | `CGuildBattleController::UpdateDraw` | guild battle DB methods | ok | matched CSV: `pUpdate_DrawGuildBattleRank` (+ rank-record check SQL) |
| 33 | `CGuildBattleController::UpdateRank` | guild battle DB methods | ok | matched CSV: `pSelect_GuildBattleRankList(%u)` |
| 34 | `CreateGuildBattleRankTable` | `CreateGuildBattleRankTable` | ok | matched CSV `select identity ... into [dbo].[tbl_GuildBattleRank%s] ...` |
| 35 | `UpdateClearGuildBattleRank` | `UpdateClearGuildBattleRank` | ok | matched CSV: `{ CALL pUpdate_ClearGuildBattleRank }` |
| 36 | `CGuildBattleController::UpdateClearRerservedDayInfo` | guild battle DB methods | ok | matched CSV: `pUpdate_ClearGuildBattleScheduleInfo`, `pUpdate_ClearGuildBattleInfo` |
| 37 | `db_input_guild_money` | same as case 19 | ok | same SQL as case 19 |
| 38 | `db_output_guild_money` | same as case 20 | ok | same SQL as case 20 |
| 39 | `db_input_guild_money` | same as case 19 | ok | same SQL as case 19 |
| 40 | set result 0 | none | n/a | no DB operation |
| 41 | `CGuildBattleController::UpdateReservedGuildBattleSchedule` | guild battle DB methods | ok | matched CSV: `pSelect_GuildBattleReservedSchedule` via reserved-list update path |
| 43 | `CTotalGuildRankManager::Update` | `Select_TotalGuildRank` | ok | matched CSV select on `tbl_GuildRank%s` (plus `TableExist` guard) |
| 44 | `CWeeklyGuildRankManager::UpdateTodayRank` | `Select_PvpPointGuildRank` | ok | matched CSV select + rank/update/create table SQL for weekly flow |
| 45 | `CWeeklyGuildRankManager::UpdateWeeklyOwner` | `Select_WeeklyGuildRankOwnerGuild` | ok | matched CSV owner select + `Update_PvpPointGuildRankSumLv` |
| 46 | `Update_ClearWeeklyPvpPointSum` | `Update_ClearWeeklyPvpPointSum` | ok | matched CSV update on `[tbl_WeeklyGuildPVPPointSum]` |
| 47 | `Update_IncreaseWeeklyGuildGuildBattlePvpPointSum` | `Update_IncreaseWeeklyGuildGuildBattlePvpPointSum` | ok | matched CSV `{ CALL pUpdate_WeeklyGuildGuildBattlePVPPoint(%u, %f) }` |
| 49 | `db_update_guildmaster` | `Update_GuildMaster` | ok | matched CSV: `{ CALL pUpdate_GuildMaster(%d, %d, %d) }` |
| 50 | `_db_Update_MacroData` | `Update_MacroData` | ok | matched CSV macro update fragments (`Update tbl_Macro Set ... where serial=%d and belt=%d`) |
| 51 | `TRC_AutoTrade::_insert_info` | atrade DB methods | ok | matched CSV atrade insert (`Insert [dbo].[tbl_atradetaxrate] ...`) |
| 52 | `db_input_guild_money_atradetax` | atrade tax update + guild money history | ok | matched CSV guild money update/history SQL used in flow |
| 53 | `AutoMineMachineMng::request_db_query` | `insert_amine_newowner`, `update_amine_battery`, `update_amine_mineore`, `update_amine_workstate`, `update_amine_selore`, `update_amine_moveore` | ok | matched CSV all automine machine query formats |
| 56 | `AutominePersonalMgr::request_query` | personal automine DB methods | ok | matched CSV: `select_amine_personal`, `insert_amine_personal`; update path is dynamic SQL builder |
| 57 | `Select_GuildBattleRecord` | `Select_GuildBattleRecord` | ok | matched CSV: `{ CALL pSelect_GuildBattleTotalRecord(%u) }` |
| 58 | `Select_UnmannedTraderReservedSchedule` | `Select_UnmannedTraderReservedSchedule` | ok | matched CSV: `select top %u ... tbl_utsellinfo ...` |
| 59 | `CUnmannedTraderController::UpdateItemState` | unmanned trader DB methods | ok | matched CSV `pUpdate_utitemstate` |
| 60 | `CUnmannedTraderController::UpdateRegistItem` | unmanned trader DB methods | ok | matched CSV regist/update item-state SQL set |
| 61 | `CUnmannedTraderController::UpdateCancelRegist` | unmanned trader DB methods | ok | matched CSV `Select_UnmannedTraderBuySingleItemInfo` + `Update_UnmannedTraderItemState` |
| 62 | `CUnmannedTraderController::UpdateTimeOutCancelRegist` | unmanned trader DB methods | ok | matched CSV same core queries as case 61 |
| 63 | `CUnmannedTraderController::UpdateRePrice` | unmanned trader DB methods | ok | matched CSV `pUpdate_utsellinfoprice` |
| 64 | `CUnmannedTraderController::SelectBuy` | `Select_UnmannedTraderBuySingleItemInfo` | ok | matched CSV `pSelect_utsellinfo` + `pSelect_utsellwaititems_salestotals` |
| 65 | `CUnmannedTraderController::UpdateBuy` | unmanned trader DB methods | ok | matched CSV `Select_UnmannedTraderBuySingleItemInfo` + `pUpdate_utresutlinfobuy` |
| 66 | `CUnmannedTraderController::UpdateBuyRollBack` | unmanned trader DB methods | ok | matched CSV `pUpdate_utresutlinfobuy` |
| 67 | `CUnmannedTraderController::UpdateLazyClean` | unmanned trader DB methods | ok | matched CSV lazy-clean path queries (state/result updates) |
| 68 | `CUnmannedTraderController::UpdateBuyComplete` | unmanned trader DB methods | ok | matched CSV result/state update query set |
| 71 | `_db_GuildRoom_Insert` | `Insert_GuidRoom` | ok | matched CSV guild-room insert SQL |
| 72 | `_db_GuildRoom_Update` | `Update_GuildRoom` | ok | matched CSV guild-room update SQL |
| 74 | `db_GM_GreetingMsg` | `Update_GmGreet` | ok | matched CSV update on `tbl_GreetMsg` `useType=255` |
| 75 | `db_RACE_GreetingMsg` | `Update_RaceGreet` | ok | matched CSV update on `tbl_GreetMsg` by race |
| 76 | `db_GUILD_GreetingMsg` | `Update_GuildGreet` | ok | matched CSV update on `tbl_guild` greeting |
| 77 | `CPostSystemManager::UpdateRegist` | `Update_PostRegistry` | ok | matched CSV full `update tbl_PostRegistry set ...` |
| 78 | `CPostSystemManager::PostSend` | post send DB methods | ok | matched CSV post send chain (`Select_PostRecvStorageCheck`, `Select_PostStorageRecordCheck`, `Insert_PostStorageRecord`, `Select_PostStorageEmptyRecordSerial`, `Update_PostStorageSendToRecver`) |
| 79 | `db_Load_PostStorage` | `Select_PostStorageList` | ok | matched CSV `select top %d ... from tbl_PostStorage` |
| 80 | `db_Load_ReturnPost` | `Select_ReturnPost` | ok | matched CSV count+list queries on `tbl_PostStorage` by return state |
| 82 | `db_Load_Content` | `Select_PostContent` | ok | matched CSV `select content from tbl_PostStorage where serial=%d` |
| 83 | `_db_init_classrefine_count` | literal update on `tbl_event` | ok | matched CSV exact literal `update [dbo].[tbl_event] set ClassRefineCnt=0,ClassRefineDate=0` |
| 84 | `Insert_GuildBatlleResultLog` | `Insert_GuildBatlleResultLog` | ok | matched CSV: `pInsert_guildbattleresultlog`, `pSelect_guildbattleresulloglatest`, `pUpdate_guildbattleresultlog` |
| 85 | `UpdateRaceRankStep1` | `Update_RaceRank_Step1` | ok | matched IDA race-rank step1 temp-table/rate SQL strings |
| 86 | `UpdateRaceRankStep2` | `Update_RaceRank_Step2` | ok | matched IDA race-rank step2 temp-table/rate SQL strings |
| 87 | `UpdateRaceRankStep3` | `Update_RaceRank_Step3` | ok | matched IDA race-rank step3 temp-table/rate SQL strings |
| 88 | `UpdateRaceRankStep4` | `Update_RaceRank_Step4` | ok | matched IDA create/drop/union SQL for `tbl_PvpRank%s` |
| 89 | `UpdateRaceRankStep5` | `Update_RaceRank_Step5` | ok | matched IDA grade-assignment SQL set |
| 90 | `UpdateRaceRankStep6` | `Update_RaceRank_Step6` | ok | matched IDA temp-table cleanup SQL |
| 91 | `UpdateRaceRankStep7` | `Update_RaceRank_Step7` | ok | matched IDA rank update SQL |
| 92 | `UpdateRaceRankStep8` | `Update_RaceRank_Step8` | ok | matched IDA guild-name sync SQL |
| 93 | `UpdateRaceRankStep9` | `Update_RaceRank_Step9` | ok | matched IDA today-table refresh SQL |
| 94 | `UpdateRaceRankStep10` | `Select_PvpRankInfo` (step10 flow) | ok | matched IDA `select top 100 ... from tbl_PvpRank%s ...` |
| 95 | `UpdateRaceRankStep11` | `Select_PvpRate` (step11 flow) | ok | matched IDA `select Rank, Rate from tbl_PvpRank%s ...` |
| 97 | `UpdateGuildRankStep1` | `Update_GuildRank_Step1` | ok | matched IDA guild-rank step1 SQL strings |
| 98 | `UpdateGuildRankStep2` | `Update_GuildRank_Step2` | ok | matched IDA guild-rank step2 SQL strings |
| 99 | `UpdateGuildRankStep3` | `Update_GuildRank_Step3` | ok | matched IDA guild-rank step3 SQL strings |
| 100 | `UpdateGuildRankStep4` | `Update_RankInGuild_Step1` | ok | matched IDA rank-in-guild step1 SQL |
| 101 | `UpdateRankinGuildStep1` | `Update_RankInGuild_Step2` | ok | matched IDA rank-in-guild step2 grade/rate SQL set |
| 102 | `UpdateRankinGuildStep2` | `Update_RankInGuild_Step3` | ok | matched IDA rank-in-guild step3 temp-table SQL |
| 103 | `UpdateRankinGuildStep3` | `Update_RankInGuild_Step4` | ok | matched IDA apply-to-`tbl_general` SQL |
| 104 | `UpdateRankinGuildStep4` | `Update_RankInGuild_Step5` | ok | matched IDA select/return-member-rank SQL |
| 105 | `UpdateRankinGuildStep5` | `Update_RankInGuild_Step6` | ok | matched IDA cleanup SQL |
| 106 | `UpdateRankinGuildStep6` | `Update_RankInGuild_Step7/8/9` flow | ok | matched IDA step7/8/9 temp-table cleanup SQL strings |
| 107 | `UpdateAndSelectGuildGrade` | `Update_GuildGrade` | ok | matched IDA `update tbl_Guild set Grade ...` and serial/grade select flow |
| 111 | `CUnmannedTraderController::UpdateLogInComplete` | unmanned trader DB methods | ok | matched CSV unmanned trader item-state/result update queries |
| 112 | `CItemStoreManager::UpdateStoreLimitItem` | limit item DB methods | ok | matched CSV for limit-item chain (`Select_LimitItemUsedRecord`, `Select_LimitItemEmptyRecord`, `Insert_LimitItemRecord`, `Update_LimitItemNum`) |
| 113 | `CItemStoreManager::UpdateDisableInstanceStore` | `Update_DisableInstanceStore` | ok | matched CSV `Update tbl_StoreLimitItem_061212 set dck=1 where serial=%d` |
| 114 | `CandidateMgr::Update_RegistCandidate_2st` | candidate DB methods | ok | matched IDA update-builder fragments (`tbl_patriarch_candidate`, `ASerial=%d or`) |
| 115 | `CandidateMgr::Update_Score` | candidate DB methods | ok | matched IDA `{ CALL pUpdate_Patriarch_Score ... }` |
| 116 | `CandidateMgr::Update_ClassType` | candidate DB methods | ok | matched IDA `{ CALL pUpdate_Patriarch_ClassType ... }` |
| 117 | `CandidateMgr::Insert_Candidate` | `Select_PatriarchWinCnt` + candidate insert/update | ok | matched IDA candidate insert call + win-count select path |
| 118 | `db_output_guild_money` | same as case 20 | ok | same SQL as case 20 |
| 119 | `PatriarchElectProcessor::Insert_Elect` | `Insert` elect query | ok | matched IDA `{ CALL pInsert_Patriarch_Elect ... }` |
| 120 | `PatriarchElectProcessor::Update_Elect` | elect update query | ok | matched IDA `{ CALL pUpdate_Patriarch_Elect_20080611 ... }` |
| 121 | `CandidateMgr::Update_Refund` | candidate DB methods | ok | matched IDA `{ CALL pUpdate_Patriarch_Refund ... }` |
| 122 | `PatriarchElectProcessor::LoadElectState` | `Select_PatriarchElectState` | ok | matched IDA `{ CALL pSelect_Patriarch_Elect_20080708 ( '%s' ) }` |
| 123 | `PatriarchElectProcessor::Request_Refund` | refund DB methods | ok | matched IDA `{ CALL pUpdate_Patriarch_PayRefund ... }` |
| 124 | `PatriarchElectProcessor::Insert_PatrirchItemChargeRefund` | refund insert DB methods | ok | matched IDA insert into `tbl_itemcharge` literal |
| 125 | `CandidateMgr::Insert_Candidate` | same as case 117 path | ok | same SQL path as case 117 |
| 126 | `CandidateMgr::Update_DischargePatriarch` | candidate DB methods | ok | matched IDA `{ CALL pUpdate_Patriarch_Status ... }` |
| 127 | `Update_Punishment` | `Update_Punishment` | ok | matched IDA `pSelect_PunishmentCount` + `pInsert/Update_Punishment` calls |
| 128 | `Select_GetCharSerialByNameRace` | `Select_GetCharSerialByNameRace` | ok | matched IDA `{ CALL pSelect_GetCharSerialByNameRace ( '%s', %d, %d ) }` |
| 129 | `CPostSystemManager::PostReceiverCheck` | post receiver DB methods | ok | matched IDA `Update_PostRegistryDisable`, `Select_PostRecvSerialFromName`, `Select_PlayerTimeLimitInfo` queries |
| 130 | `_db_Load_PatriarchComm` | `Select_PatriarchComm`, `Delete_PatriarchComm` | ok | matched IDA `pSelect_PatriarchComm` and `pDelete_PatriarchComm` |
| 131 | `Insert_PatriarchComm` | `Insert_PatriarchComm` | ok | matched IDA `Select_PatriarchCommCount` + `pInsert_PatriarchComm` |
| 134 | `CHonorGuild::UpdateNextHonorGuild` | honor guild DB methods | ok | matched IDA `pUpdate_HonorGuild` + `pInsert_HonorGuild` |
| 135 | `CHonorGuild::UpdateChangeHonorGuild` | honor guild change/update SQL | ok | matched IDA direct `UPDATE tbl_honor_guild ...` + `pUpdate_HonorGuild` + `pUpdate_ChangeHonorGuild` |
| 136 | `db_input_guild_money_atradetax` | same as case 52 | ok | same SQL as case 52 |
| 137 | `Insert_RaceBattleLog` | `Insert_RaceBattleLog` | ok | matched IDA `{ CALL pInsert_RaceBattleLog_070427 ... }` |
| 138 | `CandidateMgr::CheckDBValidCharacter` | `Select_IsValidChar` | ok | matched IDA `{ CALL pSelect_IsValidChar( %d ) }` |
| 139 | `CandidateMgr::Update_VoteTime` | candidate vote time update | ok | matched IDA `update [dbo].[tbl_general] set tmRaceBossVote=...` |
| 140 | `CUnmannedTraderController::UpdateReRegist` | `Select_UnmannedTraderRegister`, `Update_UnmannedTraderReRegist` | ok | matched IDA `pSelect_utsellinfo_owner` + `pUpdate_utsellinfo_reregist` |
| 141 | `CUnmannedTraderController::UpdateCheatRegistTime` | `Update_UnmannedTraderCheatUpdateRegistDate` | ok | matched IDA `{ CALL pUpdate_utcheatregdate( %u, %u ) }` |
| 142 | `CRaceBossWinRate::LoadBossAccmulationWinRate` | `Select_RaceBossAccumulationWinRate` | ok | matched IDA `{ CALL pSelect_BossAccWinRate(%d, %d, '%s') }` |
| 143 | `Update_UserGuildData` | `Update_UserGuildData_20070614` | ok | matched IDA `{ CALL pUpdate_UserGuildData_20070614( %d, %d, %d ) }` |
| 144 | `Select_GuildMasterLastConn` | `Select_GuildMasterLastConn` | ok | matched IDA `{ CALL pSelect_GuildMasterLastConn( %d, %d ) }` |
| 145 | `Select_CharacterReName` | `Select_CharacterReName` | ok | matched IDA `{ CALL pSelect_CharacterSerial ( '%s' ) }` |
| 146 | `Update_CharacterReName` | `Update_CharacterReName` | ok | matched IDA `{ CALL pUpdate_CharacterReName(N'%s', %d ) }` |
| 147 | `CUnmannedTraderController::UpdateReRegistRollBack` | unmanned trader DB methods | ok | matched IDA rollback path query family (`pUpdate_utitemstate`, state checks) |
| 149 | `Updatet_Account_Vote_Available` | `Updatet_Account_Vote_Available` | ok | matched IDA `declare @out_amount ... pUpdatet_Account_Vote_Available ...` |
| 150 | `Update_Player_Vote_Info` | `Update_Player_Vote_Info` | ok | matched IDA `{ CALL pUpdate_Supplement_Ex_20080609( ... ) }` |
| 151 | `UpdateVotedReset_Cheat` | `UpdateVotedReset_Supplement`, `UpdateVotedReset_General` | ok | matched IDA both update literals (`tbl_Supplement`, `tbl_general`) |
| 152 | `UpdateServerResetToken` | `UpdateServerResetToken` | ok | matched IDA `{ CALL pUpdate_Patriarch_Elect_ResetTime( %d, %d, %d ) }` |
| 153 | `Select_Player_Last_LogoutTime` (+ optional `Insert_PlayerTimeLimitInfo`) | time-limit select/insert | ok | matched IDA `Select LastLogoutTime ...` + `Insert into [tbl_TimeLimitInfo]` |
| 155 | `Update_Player_TimeLimit_Info` | `Update_Player_TimeLimit_Info` | ok | matched IDA `{ CALL pUpdate_TimeLimit_Info( %d, %d, %d) }` |
| 157 | `_db_Load_Cash_LimSale` | `Select_CashLimSale`, `Insert_CashLimSale` | ok | matched IDA `pSelect_Cash_LimSale` + `pInsert_CashLimSale` |
| 158 | `_db_Update_Cash_LimSale` | dynamic `UPDATE tbl_Cash_LimSale ...` | dynamic | matched IDA field-fragment literals (`DCK=`, `LimSaleNum=`, `Code_K`, `Num`, `WHERE [index] = 1`) |
| 161 | `_db_Update_Set_Limit_Run` | `Select_Limit_Run_Record`, `Insert_Set_Limit_Run`, `Update_Set_Limit_Run` | ok | matched IDA `tbl_sf_delay` select/insert/update binary-query literals |
| 165 | `_db_Update_GoldBoxItem` | dynamic `UPDATE tbl_GoldBoxItem ...` | dynamic | matched IDA field-fragment literals (`StarterBoxMax`, `BoxItemK_`, `BoxItemMax_`, `LimItemNum_`, `Kx_y`, `Nx_y`) |
| 171 | `_db_Select_RegeAvator_For_Lobby_Logout` | `Select_RegeAvator_For_Lobby_Logout` | ok | matched IDA `select Serial,Name,Slot,Lv,Dalant,Gold from tbl_base ...` |
| 172 | `Insert_OreReset_Log` | `Insert_OreReset_Log` | ok | matched IDA `{CALL pInsert_RemainOre_Log (... )}` |
| 175 | `_db_Update_Data_For_Post_Send` | dynamic update query for post send settlement | dynamic | uses `Update_Gold` + `_db_Update_Inven` builder; matched IDA `_db_Update_Inven` SQL fragments |
| 176 | `_db_Update_Data_For_Trade` | dynamic update query for trade settlement | dynamic | uses `Update_Dalant`/`Update_Gold` + `_db_Update_Inven` builder; matched IDA `_db_Update_Inven` SQL fragments |
