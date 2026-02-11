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
| `CPlayer.cpp` | `pc_AnimusRecallRequest` | `g_Main.m_tblItemData[24]` | `_AnimusItem_fld *` | IDA reads `Record[3].m_strCode[60]` (offset aligns with `m_nAnimusType`) to gate animus usability checks. |
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
| `CPlayer.cpp` | `CPlayer::pc_AnimusInvenChange` | `g_Main.m_tblItemData[24]` | `_base_fld *` | Uses `Record[3].m_strCode[60]` to decide class-specific animus checks. |
| `CashItemRemoteStore.cpp` | `CashItemRemoteStore::__CheckGoods` | `CashItemRemoteStore::_kRecGoods` | `_base_fld *` | Uses `reinterpret_cast<char *>(&Record[1])` as item code; writes price to `Record[1].m_strCode[60]`; logs `Record[3].m_dwIndex` and `Record[3].m_strCode[0/4]`. |
| `WorldServerUtil.cpp` | `IsCashItem` | `s_ptblItemData[byTblCode]` | `_base_fld *` | Uses `Record[6].m_strCode[48]`, `Record[9].m_strCode[12/36]`, `Record[5].m_strCode[48/44/60/56]`, `Record[8].m_strCode[8]`, `Record[7].m_strCode[44/32/8]`, `Record[6].m_strCode[0]`. |
| `WorldServerUtil.cpp` | `IsExchangeItem` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[6].m_strCode[36]`, `Record[11].m_strCode`, `Record[5].m_strCode[36]`, `Record[8].m_dwIndex`, `Record[4].m_strCode[60/32/36/48/44/52/56]`, `Record[8].m_strCode[20/16]`, `Record[5].m_strCode[28]`, `Record[3].m_strCode[48]`, `Record[7].m_dwIndex`, `Record[7].m_strCode[60]`, `Record[10].m_strCode[8]`, `Record[6].m_strCode[32/12/24]`, `Record[6].m_dwIndex`. Verify struct mapping. |
> [!NOTE]
> Rule 9: `CRecordData::GetRecord` usually returns `_base_fld`. Re-casters should be careful with structure packing and offsets.
| `WorldServerUtil.cpp` | `InitMasteryFormula` | `pSkillData` / `pForceData` | `_base_fld *` | Uses `Record[1].m_strCode[4]` and `Record[4].m_strCode[60]` to fill mastery-level tables; verify offsets. |
| `CPlayer.cpp` | `CPlayer::DeleteCouponItem` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[48]` to check coupon deletion; verify field mapping. |
| `CBossMonsterScheduleSystem.cpp` | `CBossMonsterScheduleSystem::MakeMap` | `g_Main.m_tblMonster` | `_base_fld *` | Uses `Record[4].m_dwIndex` to filter boss schedules; verify field mapping. |
| `CPlayer.cpp` | `CPlayer::Emb_CheckActForQuest` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[13].m_strCode[60]` and `Record[1].m_strCode[24]` to decide reward vs auto-complete; verify mapping. |
| `CQuestMgr.cpp` | `CQuestMgr::CheckReqAct` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[4*k+1].m_strCode` offsets (`+32/+4/+68/+132/+196/+200/+268`) and `Record[4*m+5].m_strCode[28]`, `Record[1].m_strCode[28]` as raw fields; verify correct struct mapping. |
| `CDarkHoleChannel.cpp` | `CDarkHoleChannel::CheckEvent` | `g_Main.m_tblMonster` | `_base_fld *` | Uses `Record[25].m_strCode[60]` and `Record[26].m_dwIndex` as floats for DH event filtering; verify mapping. |
| `CPlayer.cpp` | `CPlayer::pc_MineStart` | `g_Main.m_tblItemData[17]` | `_base_fld *` | Uses `Record[3].m_strCode` to gate ore type; verify field offsets. |
| `CPlayer.cpp` | `CPlayer::pc_MineStart` | `g_Main.m_tblItemData[6]` | `_base_fld *` | Uses `Record[6].m_strCode[8]` to validate mining equipment type. |
| `CPlayer.cpp` | `CPlayer::pc_MineComplete` | `g_Main.m_tblItemData[17]` | `_base_fld *` | Uses `Record[3].m_strCode[4]` for golden box chance and `Record[3].m_strCode[0]` for eAddMineOre. |
| `CPlayer.cpp` | `CPlayer::pc_OreCutting` | `g_Main.m_tblItemData[17]` | `_base_fld *` | Uses `Record[4].m_dwIndex`, `Record[4].m_strCode` (min/max), `Record[4].m_strCode[4]` (cost), `Record[3].m_strCode[36]` (action point), `Record[3].m_strCode[0]` (eAddCutOre). |
| `CPlayer.cpp` | `CPlayer::pc_CuttingComplete` | `g_Main.m_tblItemData[18]` | `_base_fld *` | Uses `Record[5].m_strCode[12]` to compute gold payout. |
| `COreCuttingTable.cpp` | `COreCuttingTable::Indexing` | `m_tblOreCutting` | `_base_fld *` | Uses `Record[1].m_strCode[60]` for cut rate and passes `&Record[1]` as the resource code when calling `GetRecord`. |
| `AutominePersonal.cpp` | `AutominePersonal::regist_to_map` | `g_Main.m_tblItemData[pDstItem->m_byTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[52]` (required level) and `Record[5].m_strCode[44]` (max HP) via raw offsets; verify field mapping. |
| `CGuardTower.cpp` | `IsOtherTowerNear` | `CGuardTower::m_pRecordSet` | `_base_fld *` | Uses `Record[5].m_strCode[24]` as tower range when checking nearby towers. |
| `CTrap.cpp` | `IsOtherInvalidObjNear` | `CTrap::m_pRecordSet` | `_base_fld *` | Uses `Record[5].m_strCode[20]` for invalid object range check. |
| `CPlayer.cpp` | `CPlayer::pc_ForceRequest` | `g_Main.m_tblEffectData[1]` | `_base_fld *` | Uses `Record[1].m_strCode[4]`, `Record[4].m_strCode[60]`, `Record[6].m_strCode`, `Record[9].m_strCode[16/52]`, `Record[12].m_strCode[12/48]`; verify offsets. |
| `CPlayer.cpp` | `CPlayer::pc_MakeTowerRequest` | `g_Main.m_tblEffectData[2]` | `_base_fld *` | Uses `Record[9].m_strCode[8/16/52]` and `Record[13].m_dwIndex`; verify offsets. |
| `CPlayer.cpp` | `CPlayer::pc_MakeTowerRequest` | `g_Main.m_tblItemData[25]` | `_base_fld *` | Uses `Record[7+slot].m_strCode[4]` for material codes and `Record[8..10].m_strCode` for required amounts. |
| `CPlayer.cpp` | `CPlayer::pc_MakeTrapRequest` | `g_Main.m_tblEffectData[2]` | `_base_fld *` | Uses `Record[9].m_strCode[8/16/52]` and `Record[13].m_dwIndex`; verify offsets. |
| `CPlayer.cpp` | `CPlayer::pc_ThrowSkillRequest` | `g_Main.m_tblItemData[10]` | `_base_fld *` | Uses `&Record[7]` as string key for effect lookup; verify struct layout. |
| `CPlayer.cpp` | `CPlayer::pc_ThrowSkillRequest` | `g_Main.m_tblEffectData[3]` | `_base_fld *` | Uses `GetRecord` with `&Record[7]` string key; verify mapping. |
| `CPlayer.cpp` | `CPlayer::pc_ThrowUnitRequest` | `g_Main.m_tblUnitBullet` | `_base_fld *` | Uses `Record[3].m_strCode[20]` as effect code string. |
| `CPlayer.cpp` | `CPlayer::pc_ThrowUnitRequest` | `g_Main.m_tblUnitPart[4]` | `_base_fld *` | Uses `Record[4].m_strCode[48]` as attack delay. |
| `CPlayer.cpp` | `CPlayer::skill_process_for_aura` | `g_Main.m_tblEffectData` | `_base_fld *` | Uses `Record[1].m_dwIndex` and `Record[12].m_strCode/Record[12].m_dwIndex` for aura effect data. |
| `WorldServerUtil.cpp` | `gGetUnitKeyMatchFrame` | `g_Main.m_tblItemData[19]` | `_base_fld *` | Uses `Record[3].m_strCode[60]` to match frame code. |
| `CPlayer.cpp` | `pc_UnitFrameBuyRequest` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode[24/28]` for money type/price, `Record[1].m_strCode` for gauge, and treats `Record + 2` as 6x64-byte part code strings. |
| `CPlayer.cpp` | `pc_UnitFrameBuyRequest` | `g_Main.m_tblUnitPart` | `_base_fld *` | Uses `Record->m_dwIndex` as part index after lookup by code string. |
| `CPlayer.cpp` | `pc_UnitSellRequest` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode` (max gauge) and `Record[1].m_strCode[28]` (base sell price). |
| `CPlayer.cpp` | `pc_UnitPartTuningRequest` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode[60]` as tuning flag. |
| `CPlayer.cpp` | `pc_UnitPartTuningRequest` | `g_Main.m_tblUnitPart` | `_base_fld *` | Uses `&Record[3].m_dwIndex + byFrame` for frame compatibility, `Record[4].m_strCode[12/16/20]` for level/mastery, `Record[5].m_strCode[36/40/48/52]` for costs, `Record[3].m_strCode[60]` for bullet type. |
| `CPlayer.cpp` | `pc_UnitPartTuningRequest` | `g_Main.m_tblUnitBullet` | `_base_fld *` | Uses `Record[3].m_strCode[4/8]` for money type/price and `Record[2].m_strCode[60]` for bullet type matching. |
| `CPlayer.cpp` | `pc_UnitFrameRepairRequest` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode[52/60]` for repair costs/flags and `Record[1].m_strCode` for max gauge. |
| `CPlayer.cpp` | `pc_UnitFrameRepairRequest` | `g_Main.m_tblUnitPart` | `_base_fld *` | Uses `Record[5].m_strCode[48/52]` for repair cost additions. |
| `CPlayer.cpp` | `pc_UnitBulletFillRequest` | `g_Main.m_tblUnitBullet` | `_base_fld *` | Uses raw offsets `+200/+208/+212/+216` (type/left/money type/price). |
| `CPlayer.cpp` | `pc_UnitBulletFillRequest` | `g_Main.m_tblUnitPart` | `_base_fld *` | Uses `Record[3].m_strCode[60]` for bullet type match. |
| `CPlayer.cpp` | `pc_UnitPackFillRequest` | `g_Main.m_tblUnitPart[5]` | `_base_fld *` | Uses `Record[6].m_dwIndex` as max spare count. |
| `CPlayer.cpp` | `pc_UnitPackFillRequest` | `g_Main.m_tblUnitBullet` | `_base_fld *` | Uses raw offsets `+208/+212/+216` and record pointer for bullet index. |
| `CPlayer.cpp` | `pc_UnitDeliveryRequest` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode` as max gauge for HP rate. |
| `CPlayer.cpp` | `pc_UnitDeliveryRequest` | `g_Main.m_tblUnitPart[5]` | `_base_fld *` | Uses `Record[5].m_strCode[56]` for booster. |
| `CPlayer.cpp` | `pc_UnitLeaveRequest` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode` as max gauge for HP rate. |
| `CPlayer.cpp` | `_UnitDestroy` | `g_Main.m_tblUnitFrame` | `_base_fld *` | Uses `Record[1].m_strCode[60/56]` to decide destroy vs gauge reset. |
| `CPlayer.cpp` | `pc_SelectQuestReward` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[13].m_strCode[60]` (reward selection flag), `Record[14] + 72*index` for reward slot code, and `Record[22].m_strCode[64*index+44]` for link quest code. |
| `CPlayer.cpp` | `pc_BriefPass` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record->m_strCode` as quest code for `Emb_CheckActForQuest`. |
| `CPlayer.cpp` | `Emb_CreateNPCQuest` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[1].m_strCode[4]` for start-history gating and `Record[1].m_strCode[8]` (double) for end time. |
