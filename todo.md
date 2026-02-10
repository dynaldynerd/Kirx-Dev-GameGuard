# Build status (2026-10-02 15:01:35)
Errors: 138
Warnings: 35
Log: build.log

**WorldServer/Entities/CheatCommands.cpp**
- CMonsterEventRespawn::StartRespawnEvent signature mismatch (called with 2 string args)
- CMonsterEventRespawn::StopRespawnEvent signature mismatch (called with 2 string args)
- CRecordData member calls used statically: GetRecordNum, GetRecord, GetRecordByHash
- CGameObject member calls used statically: GetCurSecNum, GetUseSectorRange, GetObjRace
- CMapData member calls used statically: GetSecInfo, GetRectInRadius, GetSectorListPlayer
- CMainThread::IsReleaseServiceMode called statically (signature mismatch)
- CPlayerDB::GetRaceCode and CPlayerDB::GetCharNameW called statically (signature mismatch)
- CRaceBossMsgController::Cancel missing
- CItemUpgradeTable::GetSize missing
- CItemUpgradeTable::GetRecord signature mismatch
- _ItemUpgrade_fld type undefined (missing include/definition)
- _STORAGE_LIST::_db_con ctor mismatch (lines 1117/1139/1170)
- strcmp_0 argument type mismatch with const char (*)[10] (lines 1205, 1217, 1225, 1229, 1233, 1384, 1392, 1416, 1420, 1424)
- aE_2 undefined identifier
- unk_1407AE840/unk_1407AE858/unk_1407AE868/unk_1407AE878/unk_1407AE888 undefined identifiers
- CPvpPointLimiter::CheatUpdate signature mismatch
- CPvpPointLimiter::~CPvpPointLimiter called with argument
- CGuildBattleController::Add overload mismatch
- CGuildBattleController missing: CheatCreateFieldObject, CheatDestroyFieldObject, CheatRegenStone, CheatDestroyStone, CheatTakeStone, CheatGetStone, CheatDropStone, CheatForceTakeStone, CheckGoal, SendReservedScheduleList, SendCurrentBattleInfoRequest
- CTotalGuildRankManager undefined (Instance/Send missing)
- CWeeklyGuildRankManager undefined (Instance/Send missing)
- CUnmannedTraderTaxRateManager undefined (Instance/CheatChangeTaxRate missing)
- _combine_ex_item_result_zocl type undefined

**WorldServer/Entities/CPlayer_Missing.cpp**
- CGuild missing: ReleaseTemp, EstGuild, SendMsg_DownPacket, GetApplierFromSerial, PushMember, SendMsg_GuildJoinAcceptInform, SendMsg_LeaveMember, PopMember, UpdateEmblem, SendMsg_GuildDisjointInform, Release, DB_Update_GuildMaster_Complete
- GuildCreateEventInfo::GetEstConsumeDalant missing
- CGuildBattleController missing: UpdatePossibleBattleGuildList, JoinGuild, LeaveGuild
- CGuildRoomSystem missing: IsGuildRoomMemberIn, SetPlayerOut
- CMgrGuildHistory missing: join_member, leave_member
- CMgrAvatorItemHistory missing: guild_pop_money, guild_pop_money_rollback
- LODWORD assignments not lvalues (lines 1625-1628, 1744-1745)

**WorldServer/Entities/CUnmannedTraderController.cpp**
- CUnmannedTraderUserInfoTable::Load not declared/implemented
