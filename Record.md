# GetRecord Casting logic Findings

During the implementation of `CMapOperation::Init` and its sub-flows, the following `GetRecord` casting behaviors were observed in the IDA source:

| File | Class::Method | Table Used | Cast Type | Notes |
|---|---|---|---|---|
| `CMapOperation.cpp` | `LoadMaps` | `m_tblMapData` | `_map_fld *` | Direct cast to specific type. |
| `CWorldSchedule.cpp` | `Init` | `m_tblSch` | `_WorldSchedule_fld *` | Decompiled code shows access like `Record[1].m_dwIndex`. This implies `Record` is treated as an array of structs where the base type and specific fields might have known offsets. |
| `CItemStore.cpp` | `CItemStore::GetNpcRaceCode` | `g_Main.m_tblNPC` | `_base_fld *` | Accesses `Record[1].m_strCode[60]` for race code. |
| `CItemStore.cpp` | `CItemStore::Init` | `g_Main.m_tblItemData[nTableCode]` | `_base_fld *` | Uses `Record[1].m_dwIndex` (SLOBYTE) for `bExist`. |
| `WorldServerUtil.cpp` | `GetItemStdPrice / GetItemStdPoint / GetItemProcPoint / GetItemKillPoint / GetItemGoldPoint` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Multiple cases access `Record[N].m_strCode[offset]` and `Record[N].m_dwIndex` depending on table code. |
| `WorldServerUtil.cpp` | `GetItemDurPoint` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Accesses `Record[5].m_strCode`, `Record[9].m_strCode`, `Record[6].m_strCode[28]`, `Record[4].m_strCode[40]`, `Record[4].m_strCode[28]`, `Record[6].m_strCode[40]`, `Record[5].m_strCode[60]` (float), `Record[5].m_dwIndex`, `Record[5].m_strCode[44]`. |
| `CMapData.cpp` | `_LoadStoreDummy` | `CItemStoreManager::m_tblItemStore` | `_StoreList_fld *` | IDA compares `&Record[3].m_strCode[52]` with map code; interpreted as `_StoreList_fld::m_strStore_MAPcode`. |
| `WorldServerUtil.cpp` | `GetDefItemUpgSocketNum` | `g_Main.m_tblItemData[nTableCode]` | `_base_fld *` | Uses `Record[6].m_strCode[52]` and `Record[11].m_strCode[16]`. |
| `WorldServerUtil.cpp` | `GetWeaponClass` | `s_ptblItemData[6]` | `_base_fld *` | Uses `Record[6].m_strCode[8]`. |
| `WorldServerUtil.cpp` | `GetItemGrade` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[3].m_strCode[0]`. |
| `WorldServerUtil.cpp` | `GetItemEquipLevel` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[8]`, `Record[8].m_strCode[8]`, `Record[4].m_strCode[4]`, `Record[4].m_strCode[48]`, `Record[4].m_strCode[60]`, `Record[4].m_strCode[52]` depending on table code. |
| `WorldServerUtil.cpp` | `GetItemEquipUpLevel` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[12]`, `Record[8].m_strCode[12]`, `Record[4].m_strCode[8]`, `Record[4].m_strCode[52]`, `Record[5].m_dwIndex`, `Record[4].m_strCode[56]` depending on table code. |
| `WorldServerUtil.cpp` | `GetItemEquipMastery` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Returns `_EQUIP_MASTERY_LIM *` via `&Record[4].m_strCode[20]` or `&Record[8].m_strCode[20]` depending on table code. |
| `CPlayer.cpp` | `_WEAPON_PARAM::FixWeapon` | `g_Main.m_tblItemData[6]` | `_base_fld *` | Uses `Record[6].m_strCode[8]`, `Record[9].m_strCode[60/56/48/20]`, `Record[10].m_strCode[...]`, `Record[3].m_strCode[4/8]`, `Record[4].m_strCode[4/8]`, `Record[5].m_strCode[12]`. |
| `TimeLimitJade.cpp` | `InsertWaitList` | `g_Main.m_tblItemData[pkItem->m_byTableCode]` | `_base_fld *` | Uses `Record[5].m_strCode[44]` (start hour) and `Record[5].m_strCode[48]` (use time). |
| `CPlayer.cpp` | `SetHaveEffectUseTime` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Uses `Record[6].m_dwIndex` and `Record[6].m_strCode[12*i + {0,4,8}]` for effect data. |
| `CPlayer.cpp` | `CalcDefTol` | `g_Main.m_tblItemData[...]`, `g_Main.m_tblUnitPart` | `_base_fld *` | Uses `Record[5].m_strCode[28]`, `Record[4].m_strCode[52]`, `Record[5].m_strCode[20]` for tolerance values. |
| `CPlayer.cpp` | `CalcEquipSpeed` | `g_Main.m_tblItemData[...]` | `_base_fld *` | Uses `Record[5].m_strCode[4]` and `Record[9].m_strCode[4]` for equip speed. |
| `CPlayer.cpp` | `CalcEquipMaxDP` | `g_Main.m_tblItemData[...]` | `_base_fld *` | Uses `Record[5].m_strCode[52]` for max DP. |
| `CPlayer.cpp` | `SetHaveEffect` | `g_Main.m_tblItemData[...]` | `_base_fld *` | Uses `Record[5].m_strCode[44]`, `Record[5].m_strCode[52]`, `Record[6].m_dwIndex`, `Record[6].m_strCode[12*i + {0,4,8}]`. |
| `CPlayer.cpp` | `_GetItemEffect` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Returns `_ITEM_EFFECT *` via `Record + 51`, `Record + 89`, `Record + 43`, `&Record[5].m_strCode[28]`, or `&Record[6]` depending on table code. |
| `CPlayer.cpp` | `_MASTERY_PARAM::UpdateCumPerMast` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses `Record[4].m_strCode[60]` (skill lv), `Record[1].m_strCode[4]` (skill class). |
| `CPlayer.cpp` | `_MASTERY_PARAM::AlterCumPerMast` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses `Record[4].m_strCode[60]` (skill lv), `Record[1].m_strCode[4]` (skill class). |
| `CPlayer.cpp` | `CPlayer::_check_mastery_lim` | `CPlayer::s_tblLimMastery*` | `_base_fld *` | Uses `Record[1].m_strCode[4*byIndex]`, `Record[1].m_strCode[12/16/20/8]`, `Record[1].m_strCode[4*byIndex+32]`, and `Record[2].m_dwIndex` as mastery limits. |
| `WorldServerUtil.cpp` | `GetUsePcCashType` | `s_ptblItemData[byTblCode]` | `_base_fld *` | Uses `Record[9].m_strCode[16/40]`, `Record[8].m_strCode[12]`, `Record[7].m_strCode[48]`, `Record[5].m_strCode[60]`. |
| `CGoldenBoxItemMgr.cpp` | `CGoldenBoxItemMgr::SetGoldBoxItemIndex` | `g_Main.m_tblItemData[17]` | `_base_fld *` | IDA uses `*(_DWORD *)&Record[3].m_strCode[4]` to filter records; needs review for correct field mapping. |
| `WorldServerUtil.cpp` | `GetAnimusFldFromExp` | `CAnimus::s_tblParameter[nAnimusClass]` | `_base_fld *` | IDA compares `*(_QWORD *)Record[1].m_strCode` to exp; indicates non-base layout usage. |
| `CUnmannedTraderSubClassInfoForceLiverGrade.cpp` | `CUnmannedTraderSubClassInfoForceLiverGrade::GetGroupID` | `g_Main.m_tblEffectData` | `_base_fld *` | Uses `Record[4].m_strCode[60]` for effect data; verify correct field mapping. |
| `DfAIMgr.cpp` | `Action_Attack_OnLoop` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[25].m_strCode[20]` as loop time basis for attack behavior. |
| `DfAIMgr.cpp` | `Action_Change_Handler` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[26].m_strCode[12]` (base), random add; sets loop time. |
| `DfAIMgr.cpp` | `OnDfExternCallFun` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[26].m_strCode[20]` as probability threshold. |
| `DfAIMgr.cpp` | `Mon_SearchStart_OnLoop` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[26].m_strCode[16]` and `Record[3].m_strCode[8]` in search logic. |
| `DfAIMgr.cpp` | `Condition_OnLoop` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[25].m_strCode[4]` and `Record[29].m_strCode[48/52/56]` via raw offsets (1708, 2024, 2028, 2032). |
| `DfAIMgr.cpp` | `CheckEmotionBad` | `CMonster::m_pRecordSet` | `_monster_fld *` | Casts to `_monster_fld` to use `m_fEmoType` (decompiled shows `Record[29].m_strCode[28]`). |
| `CMonster.cpp` | `GetOffensiveType` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[29].m_strCode[36]` for offensive type. |
| `CQuestMgr.cpp` | `CQuestMgr::CheckFailCondition` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `record[32].m_strCode[28]` as quest code for `GetRecord`; offset likely non-base layout. |
| `CPlayer.cpp` | `_MASTERY_PARAM::Init` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses `Record[4].m_strCode[60]` and `Record[1].m_strCode[4]` for skill level/mastery grouping; verify offsets. |
| `CMonster.cpp` | `IsPreAttackAbleMon` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[29].m_strCode[28]` compared to `5.0f`. |
| `CMonster.cpp` | `GeEmotionImpStdTime` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[29].m_strCode[44]` for emotion impulse timing. |
| `CMonster.cpp` | `GetAggroShortTime / GetAggroResetTime` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[3].m_strCode[48]` and `Record[3].m_strCode[52]` (scaled by 1000). |
| `CCharacter.cpp` | `_CheckCumulativeSF` | `g_Main.m_tblEffectData[byEffectCode]` | `_base_fld *` | Uses `Record[9].m_strCode[56/60]`, `Record[10].m_strCode`, `Record[10].m_dwIndex`. |
| `CCharacter.cpp` | `_set_sf_cont` | `g_Main.m_tblEffectData[byEffectCode]` | `_base_fld *` | Uses `Record[12].m_strCode[56]` when effect code 1, else `Record[13].m_strCode[48]` for effect params. |
| `CMonster.cpp` | `GetMaxDMGSFContCount` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[25].m_strCode` as max count. |
| `CMonster.cpp` | `_LootItem_Std` | `g_Main.m_tblItemLoot.m_tblLoot` | `_base_fld *` | Uses `Record[1].m_strCode[4/8]`, `Record[1].m_dwIndex` for loot weights/counts. |
| `CMonster.cpp` | `_LootItem_Std` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[4].m_strCode[8]` as level diff adjust. |
| `CMonster.cpp` | `CreateRepMonster` | `g_Main.m_tblMonster` | `_monster_fld *` | IDA uses `Record[4].m_strCode[4]`; interpreted as `_monster_fld::m_bExpDown` when applying rob-exp field. |
| `CMonster.cpp` | `CreateRespawnMonster` | `g_Main.m_tblMonster` | `_base_fld *` | Uses `Record[4].m_strCode[4]` to set `bRobExp` when applying rob-exp field; verify field mapping. |
| `cStaticMember_Player.cpp` | `cStaticMember_Player::loadLimitExpData` | `g_Main.m_tblExp` | `_base_fld *` | Uses `&Record[1]` as string input to `atof`, implying non-base layout usage. |
| `CPlayerDB.cpp` | `CPlayerDB::CalcCharGrade` | `g_Main.m_tblGrade` | `_base_fld *` | Uses `Record[1].m_dwIndex` as float and `Record[1].m_strCode[0]`; verify field offsets. |
| `CPlayer.cpp` | `CPlayer::SetStaticMember` | `g_Main.m_tblItemData[15]` | `_base_fld *` | Uses `&Record[4].m_strCode[28]` as key into `m_tblEffectData[1]`; likely non-base layout. |
| `CPlayer.cpp` | `CPlayer::SetStaticMember` | `g_Main.m_tblEffectData` | `_base_fld *` | Reads `*reinterpret_cast<unsigned int *>(&Record[1].m_strCode[4])` as mastery index; verify field mapping. |
| `CashItemRemoteStore.cpp` | `CashItemRemoteStore::__CheckGoods` | `CashItemRemoteStore::_kRecGoods` | `_base_fld *` | Uses `reinterpret_cast<char *>(&Record[1])` as item code; writes price to `Record[1].m_strCode[60]`; logs `Record[3].m_dwIndex` and `Record[3].m_strCode[0/4]`. |
| `WorldServerUtil.cpp` | `IsCashItem` | `s_ptblItemData[byTblCode]` | `_base_fld *` | Uses `Record[6].m_strCode[48]`, `Record[9].m_strCode[12/36]`, `Record[5].m_strCode[48/44/60/56]`, `Record[8].m_strCode[8]`, `Record[7].m_strCode[44/32/8]`, `Record[6].m_strCode[0]`. |
> [!NOTE]
> Rule 9: `CRecordData::GetRecord` usually returns `_base_fld`. Re-casters should be careful with structure packing and offsets.
| `WorldServerUtil.cpp` | `InitMasteryFormula` | `pSkillData` / `pForceData` | `_base_fld *` | Uses `Record[1].m_strCode[4]` and `Record[4].m_strCode[60]` to fill mastery-level tables; verify offsets. |
| `CPlayer.cpp` | `CPlayer::DeleteCouponItem` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[48]` to check coupon deletion; verify field mapping. |
| `CBossMonsterScheduleSystem.cpp` | `CBossMonsterScheduleSystem::MakeMap` | `g_Main.m_tblMonster` | `_base_fld *` | Uses `Record[4].m_dwIndex` to filter boss schedules; verify field mapping. |
| `CPlayer.cpp` | `CPlayer::pc_BuyItemStore` | `g_Main.m_tblItemData[28]` | `_base_fld *` | IDA shows `&Record[4].m_strCode[8]` passed to `GetLeftTicketIncludeReserNum`; mapped implementation uses `_TicketItem_fld::m_strMapCode` for ticket target map. |
| `CPlayer.cpp` | `CPlayer::Emb_CheckActForQuest` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[13].m_strCode[60]` and `Record[1].m_strCode[24]` to decide reward vs auto-complete; verify mapping. |
| `CQuestMgr.cpp` | `CQuestMgr::CheckReqAct` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[4*k+1].m_strCode` offsets (`+32/+4/+68/+132/+196/+200/+268`) and `Record[4*m+5].m_strCode[28]`, `Record[1].m_strCode[28]` as raw fields; verify correct struct mapping. |
| `CDarkHoleChannel.cpp` | `CDarkHoleChannel::CheckEvent` | `g_Main.m_tblMonster` | `_base_fld *` | Uses `Record[25].m_strCode[60]` and `Record[26].m_dwIndex` as floats for DH event filtering; verify mapping. |
| `CPlayer_Missing.cpp` | `CPlayer::pc_DarkHoleOpenRequest` | `g_Main.m_tblItemData[23]` | `_base_fld *` | Uses `Record[2].m_strCode` as dark hole code string; verify base layout. |
| `CPlayer_Missing.cpp` | `CPlayer::pc_DarkHoleEnterRequest` | `CDarkHole::m_pRecordSet` | `_base_fld *` | Uses `Record[2].m_strCode` to log dark hole name; verify base layout. |
| `CEquipItemSFAgent.cpp` | `CEquipItemSFAgent::GetBoosterAddSpeed` | `g_Main.m_tblItemData[7]` | `_base_fld *` | Uses `Record[7].m_strCode[60]` as booster add speed; verify mapping. |
| `CPlayer_Missing.cpp` | `CPlayer::_check_mastery_cum_lim` | `CPlayer::s_tblLimMasteryCum / s_tblLimMasteryCumContinue` | `_base_fld *` | Uses `Record[1].m_strCode[4*byIndex]`, `Record[1].m_strCode[12/16/20/8]`, `Record[1].m_strCode[4*byIndex+32]`, `Record[2].m_dwIndex+byIndex` as mastery limits. |
| `CPlayer_Missing.cpp` | `CPlayer::Emb_AlterStat` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses `Record[1].m_strCode[4]` to map skill mastery index for class=3. |
| `WorldServerUtil.cpp` | `IsExchangeItem` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses many raw offsets such as `Record[6].m_strCode[36]`, `Record[11].m_strCode[0]`, `Record[7].m_strCode[60]`, `Record[4].m_strCode[56]`; verify exchange-flag field mapping per table. |
| `AutominePersonal.cpp` | `AutominePersonal::regist_to_map` | `g_Main.m_tblItemData[pInstallItem->m_byTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[52]` (level requirement) and `Record[5].m_strCode[44]` (max HP) via raw offsets; verify mapping for install item table. |
| `AutominePersonal.cpp` | `AutominePersonal::GetDefFC / GetDefGap / GetDefFacing` | `g_Main.m_tblItemData[33]` | `_base_fld *` | Uses `Record[5].m_strCode[16/20/24]` for defense values via raw offsets; verify exact item-field mapping. |
| `CPlayer_Missing.cpp` | `CPlayer::pc_UnitPartTuningRequest` | `g_Main.m_tblUnitBullet / g_Main.m_tblUnitPart[3..4]` | `_base_fld *` | Uses `Record[2].m_strCode[60]` (bullet) and `Record[3].m_strCode[60]` (unit part) for weapon-type matching when validating/removing incompatible bullets. |
| `CPlayer_Missing.cpp` | `CPlayer::pc_UnitBulletFillRequest` | `g_Main.m_tblUnitPart[3..4]` | `_base_fld *` | Uses `Record[3].m_strCode[60]` as weapon-part type and compares it with `_UnitBullet_fld::m_nWPType`; verify field mapping. |
| `CItemLootTable.cpp` | `CItemLootTable::Indexing` | `m_tblLoot`, `itemTables[tableCode]` | `_base_fld *` | Uses `Record[1].m_strCode[8/12+8*j]` for loot item-code list and `RecordByHash[1].m_dwIndex` for existence flag. |
| `CItemUpgradeTable.cpp` | `CItemUpgradeTable::Indexing` | `m_tblItemUpgrade`, `resTable` | `_base_fld *` | Uses `Record->m_strCode` as resource-code key and casts resource record to `unsigned __int16 *` for index extraction. |
| `COreCuttingTable.cpp` | `COreCuttingTable::Indexing` | `m_tblOreCutting`, `oreTable`, `resTable` | `_base_fld *` | Uses `Record[1].m_strCode[60]` for ore cutting rate and `reinterpret_cast<const char*>(&Record[1])` as resource-code key. |
