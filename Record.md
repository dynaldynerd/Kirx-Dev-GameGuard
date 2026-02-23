# GetRecord Casting logic Findings

During the implementation of `CMapOperation::Init` and its sub-flows, the following `GetRecord` casting behaviors were observed in the IDA source:

| File | Class::Method | Table Used | Cast Type | Notes |
|---|---|---|---|---|
| `CMapOperation.cpp` | `LoadMaps` | `m_tblMapData` | `_map_fld *` | Direct cast to specific type. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CWorldSchedule.cpp` | `Init` | `m_tblSch` | `_WorldSchedule_fld *` | Uses _WorldSchedule_fld typed fields (m_nHour/m_nMin/m_nEventCode/etc.). |
| `CItemStore.cpp` | `CItemStore::GetNpcRaceCode` | `g_Main.m_tblNPC` | `_base_fld *` | Uses _npc_fld::m_nRace. |
| `CItemStore.cpp` | `CItemStore::Init` | `g_Main.m_tblItemData[nTableCode]` | `_base_fld *` | Uses `Record[1].m_dwIndex` (SLOBYTE) for `bExist`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetItemStdPrice / GetItemStdPoint / GetItemProcPoint / GetItemKillPoint / GetItemGoldPoint` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Multiple cases access `Record[N].m_strCode[offset]` and `Record[N].m_dwIndex` depending on table code. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetItemDurPoint` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Accesses `Record[5].m_strCode`, `Record[9].m_strCode`, `Record[6].m_strCode[28]`, `Record[4].m_strCode[40]`, `Record[4].m_strCode[28]`, `Record[6].m_strCode[40]`, `Record[5].m_strCode[60]` (float), `Record[5].m_dwIndex`, `Record[5].m_strCode[44]`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CMapData.cpp` | `_LoadStoreDummy` | `CItemStoreManager::m_tblItemStore` | `_StoreList_fld *` | Uses _StoreList_fld::m_strStore_MAPcode. |
| `WorldServerUtil.cpp` | `GetDefItemUpgSocketNum` | `g_Main.m_tblItemData[nTableCode]` | `_base_fld *` | Uses `Record[6].m_strCode[52]` and `Record[11].m_strCode[16]`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetWeaponClass` | `s_ptblItemData[6]` | `_base_fld *` | Uses _WeaponItem_fld::m_nType. |
| `WorldServerUtil.cpp` | `GetItemGrade` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[3].m_strCode[0]`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetItemEquipLevel` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[8]`, `Record[8].m_strCode[8]`, `Record[4].m_strCode[4]`, `Record[4].m_strCode[48]`, `Record[4].m_strCode[60]`, `Record[4].m_strCode[52]` depending on table code. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetItemEquipUpLevel` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[12]`, `Record[8].m_strCode[12]`, `Record[4].m_strCode[8]`, `Record[4].m_strCode[52]`, `Record[5].m_dwIndex`, `Record[4].m_strCode[56]` depending on table code. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetItemEquipMastery` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Returns `_EQUIP_MASTERY_LIM *` via `&Record[4].m_strCode[20]` or `&Record[8].m_strCode[20]` depending on table code. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `_WEAPON_PARAM::FixWeapon` | `g_Main.m_tblItemData[6]` | `_base_fld *` | Uses _WeaponItem_fld typed fields (m_nType, m_fGADst, m_nActiveType, etc.). |
| `TimeLimitJade.cpp` | `InsertWaitList` | `g_Main.m_tblItemData[pkItem->m_byTableCode]` | `_base_fld *` | Uses `Record[5].m_strCode[44]` (start hour) and `Record[5].m_strCode[48]` (use time). [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `SetHaveEffectUseTime` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Uses `Record[6].m_dwIndex` and `Record[6].m_strCode[12*i + {0,4,8}]` for effect data. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `CalcDefTol` | `g_Main.m_tblItemData[...]`, `g_Main.m_tblUnitPart` | `_base_fld *` | Uses `Record[5].m_strCode[28]`, `Record[4].m_strCode[52]`, `Record[5].m_strCode[20]` for tolerance values. |
| `CPlayer.cpp` | `CalcEquipSpeed` | `g_Main.m_tblItemData[...]` | `_base_fld *` | Uses `Record[5].m_strCode[4]` and `Record[9].m_strCode[4]` for equip speed. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `CalcEquipMaxDP` | `g_Main.m_tblItemData[...]` | `_base_fld *` | Uses `Record[5].m_strCode[52]` for max DP. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `SetHaveEffect` | `g_Main.m_tblItemData[...]` | `_base_fld *` | Uses `Record[5].m_strCode[44]`, `Record[5].m_strCode[52]`, `Record[6].m_dwIndex`, `Record[6].m_strCode[12*i + {0,4,8}]`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `_GetItemEffect` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Returns `_ITEM_EFFECT *` via `Record + 51`, `Record + 89`, `Record + 43`, `&Record[5].m_strCode[28]`, or `&Record[6]` depending on table code. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `_MASTERY_PARAM::UpdateCumPerMast` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses _skill_fld::m_nLv and _skill_fld::m_nMastIndex. |
| `CPlayer.cpp` | `_MASTERY_PARAM::AlterCumPerMast` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses _skill_fld::m_nLv and _skill_fld::m_nMastIndex. |
| `CPlayer.cpp` | `CPlayer::_check_mastery_lim` | `CPlayer::s_tblLimMastery*` | `_base_fld *` | Uses `Record[1].m_strCode[4*byIndex]`, `Record[1].m_strCode[12/16/20/8]`, `Record[1].m_strCode[4*byIndex+32]`, and `Record[2].m_dwIndex` as mastery limits. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `GetUsePcCashType` | `s_ptblItemData[byTblCode]` | `_base_fld *` | Uses `Record[9].m_strCode[16/40]`, `Record[8].m_strCode[12]`, `Record[7].m_strCode[48]`, `Record[5].m_strCode[60]`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CGoldenBoxItemMgr.cpp` | `CGoldenBoxItemMgr::SetGoldBoxItemIndex` | `g_Main.m_tblItemData[17]` | `_base_fld *` | Uses _OreItem_fld::m_dwOreProbability. |
| `WorldServerUtil.cpp` | `GetAnimusFldFromExp` | `CAnimus::s_tblParameter[nAnimusClass]` | `_base_fld *` | Uses _animus_fld::m_nForLvUpExp. |
| `CUnmannedTraderSubClassInfoForceLiverGrade.cpp` | `CUnmannedTraderSubClassInfoForceLiverGrade::GetGroupID` | `g_Main.m_tblEffectData` | `_base_fld *` | Uses _force_fld::m_nLv. |
| `DfAIMgr.cpp` | `Action_Attack_OnLoop` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[25].m_strCode[20]` as loop time basis for attack behavior. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `DfAIMgr.cpp` | `Action_Change_Handler` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[26].m_strCode[12]` (base), random add; sets loop time. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `DfAIMgr.cpp` | `OnDfExternCallFun` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[26].m_strCode[20]` as probability threshold. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `DfAIMgr.cpp` | `Mon_SearchStart_OnLoop` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[26].m_strCode[16]` and `Record[3].m_strCode[8]` in search logic. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `DfAIMgr.cpp` | `Condition_OnLoop` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses `Record[25].m_strCode[4]` and `Record[29].m_strCode[48/52/56]` via raw offsets (1708, 2024, 2028, 2032). [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `DfAIMgr.cpp` | `CheckEmotionBad` | `CMonster::m_pRecordSet` | `_monster_fld *` | Casts to `_monster_fld` to use `m_fEmoType` (decompiled shows `Record[29].m_strCode[28]`). [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CMonster.cpp` | `GetOffensiveType` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses _monster_fld::m_nOffensiveType. |
| `CQuestMgr.cpp` | `CQuestMgr::CheckFailCondition` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `record[32].m_strCode[28]` as quest code for `GetRecord`; offset likely non-base layout. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `_MASTERY_PARAM::Init` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses _skill_fld::m_nLv and _skill_fld::m_nMastIndex. |
| `CMonster.cpp` | `IsPreAttackAbleMon` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses _monster_fld::m_fEmoType. |
| `CMonster.cpp` | `GeEmotionImpStdTime` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses _monster_fld::m_fEmoImpStdTime. |
| `CMonster.cpp` | `GetAggroShortTime / GetAggroResetTime` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses _monster_fld::m_nAPTime and _monster_fld::m_nAPReset. |
| `CMonster.cpp` | `GetMaxDMGSFContCount` | `CMonster::m_pRecordSet` | `_base_fld *` | Uses _monster_fld::m_nInjuryLimit. |
| `CMonster.cpp` | `_LootItem_Std` | `g_Main.m_tblItemLoot.m_tblLoot` | `_ItemLooting_fld *` | Uses typed fields `_ItemLooting_fld::m_nOperationCount`, `m_nLootRate`, `m_nLootListCount` (IDA parity). |
| `CMonster.cpp` | `_LootItem_Std` | `CMonster::m_pRecordSet` | `_monster_fld *` | Uses typed field `_monster_fld::m_nUpLooting` for level-diff loot adjustment (IDA parity). |
| `CMonster.cpp` | `CreateRepMonster` | `g_Main.m_tblMonster` | `_monster_fld *` | Uses _monster_fld::m_bExpDown. |
| `CMonster.cpp` | `CreateRespawnMonster` | `g_Main.m_tblMonster` | `_base_fld *` | Uses _monster_fld::m_bExpDown. |
| `cStaticMember_Player.cpp` | `cStaticMember_Player::loadLimitExpData` | `g_Main.m_tblExp` | `_base_fld *` | Uses `&Record[1]` as string input to `atof`, implying non-base layout usage. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayerDB.cpp` | `CPlayerDB::CalcCharGrade` | `g_Main.m_tblGrade` | `_base_fld *` | Uses `Record[1].m_dwIndex` as float and `Record[1].m_strCode[0]`; verify field offsets. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `CPlayer::SetStaticMember` | `g_Main.m_tblItemData[15]` | `_base_fld *` | Uses _ForceItem_fld::m_strForce_Codekey. |
| `CPlayer.cpp` | `CPlayer::SetStaticMember` | `g_Main.m_tblEffectData` | `_base_fld *` | Uses _skill_fld::m_nClass and _skill_fld::m_nMastIndex. |
| `CashItemRemoteStore.cpp` | `CashItemRemoteStore::__CheckGoods` | `CashItemRemoteStore::_kRecGoods` | `_base_fld *` | Uses _CashShop_fld::m_strCsItemCode, m_nCsPrice, m_nCsDiscount, m_nCsEvent. |
| `CashItemRemoteStore.cpp` | `CheckCouponType / UseDiscountCoupon / IsUsableCoupon / _buybygold_buy_single_item_calc_price_coupon` | `g_Main.m_tblItemData[couponItem->m_byTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[0/4/8]` as coupon class/type/value fields via raw offsets; verify coupon-table mapping. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CMgrAvatorItemHistory.cpp` | `CMgrAvatorItemHistory::coupon_use_buy_item` | `g_Main.m_tblItemData[pCouponItem->m_byTableCode]` | `_base_fld *` | Uses _CouponItem_fld::m_nPoint (expected coupon table path). |
| `WorldServerUtil.cpp` | `IsCashItem` | `s_ptblItemData[byTblCode]` | `_base_fld *` | Uses `Record[6].m_strCode[48]`, `Record[9].m_strCode[12/36]`, `Record[5].m_strCode[48/44/60/56]`, `Record[8].m_strCode[8]`, `Record[7].m_strCode[44/32/8]`, `Record[6].m_strCode[0]`. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
> [!NOTE]
> Rule 9: `CRecordData::GetRecord` usually returns `_base_fld`. Re-casters should be careful with structure packing and offsets.
| `WorldServerUtil.cpp` | `InitMasteryFormula` | `pSkillData` / `pForceData` | `_base_fld *` | Uses `Record[1].m_strCode[4]` and `Record[4].m_strCode[60]` to fill mastery-level tables; verify offsets. |
| `CPlayer.cpp` | `CPlayer::DeleteCouponItem` | `g_Main.m_tblItemData[pItem->m_byTableCode]` | `_base_fld *` | Uses `Record[4].m_strCode[48]` to check coupon deletion; verify field mapping. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer.cpp` | `CPlayer::pc_BuyItemStore` | `g_Main.m_tblItemData[28]` | `_base_fld *` | Uses _TicketItem_fld::m_strMapCode for ticket map target. |
| `CPlayer.cpp` | `CPlayer::Emb_CheckActForQuest` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[13].m_strCode[60]` and `Record[1].m_strCode[24]` to decide reward vs auto-complete; verify mapping. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CQuestMgr.cpp` | `CQuestMgr::CheckReqAct` | `CQuestMgr::s_tblQuest` | `_base_fld *` | Uses `Record[4*k+1].m_strCode` offsets (`+32/+4/+68/+132/+196/+200/+268`) and `Record[4*m+5].m_strCode[28]`, `Record[1].m_strCode[28]` as raw fields; verify correct struct mapping. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer_Missing.cpp` | `CPlayer::pc_DarkHoleOpenRequest` | `g_Main.m_tblItemData[23]` | `_base_fld *` | Uses _BattleDungeonItem_fld::m_strName for dark-hole item name logging. |
| `CPlayer_Missing.cpp` | `CPlayer::pc_DarkHoleEnterRequest` | `CDarkHole::m_pRecordSet` | `_base_fld *` | Uses _BattleDungeonItem_fld::m_strName for dark-hole dungeon name logging. |
| `CEquipItemSFAgent.cpp` | `CEquipItemSFAgent::GetBoosterAddSpeed` | `g_Main.m_tblItemData[7]` | `_base_fld *` | Uses _CloakItem_fld::m_fBoosterSpeed. |
| `CPlayer_Missing.cpp` | `CPlayer::_check_mastery_cum_lim` | `CPlayer::s_tblLimMasteryCum / s_tblLimMasteryCumContinue` | `_base_fld *` | Uses `Record[1].m_strCode[4*byIndex]`, `Record[1].m_strCode[12/16/20/8]`, `Record[1].m_strCode[4*byIndex+32]`, `Record[2].m_dwIndex+byIndex` as mastery limits. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer_Missing.cpp` | `CPlayer::Emb_AlterStat` | `_MASTERY_PARAM::s_pSkillData` | `_base_fld *` | Uses `Record[1].m_strCode[4]` to map skill mastery index for class=3. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `WorldServerUtil.cpp` | `IsExchangeItem` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Uses many raw offsets such as `Record[6].m_strCode[36]`, `Record[11].m_strCode[0]`, `Record[7].m_strCode[60]`, `Record[4].m_strCode[56]`; verify exchange-flag field mapping per table. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `AutominePersonal.cpp` | `AutominePersonal::regist_to_map` | `g_Main.m_tblItemData[pInstallItem->m_byTableCode]` | `_base_fld *` | Uses _UNmannedminer_fld::m_nLevelLim and m_nMaxHP. |
| `AutominePersonal.cpp` | `AutominePersonal::GetDefFC / GetDefGap / GetDefFacing` | `g_Main.m_tblItemData[33]` | `_base_fld *` | Uses _UNmannedminer_fld::m_nDefFc, m_fDefGap, m_fDefFacing. |
| `CPlayer_Missing.cpp` | `CPlayer::pc_UnitPartTuningRequest` | `g_Main.m_tblUnitBullet / g_Main.m_tblUnitPart[3..4]` | `_base_fld *` | Uses `Record[2].m_strCode[60]` (bullet) and `Record[3].m_strCode[60]` (unit part) for weapon-type matching when validating/removing incompatible bullets. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer_Missing.cpp` | `CPlayer::pc_UnitBulletFillRequest` | `g_Main.m_tblUnitPart[3..4]` | `_base_fld *` | Uses `Record[3].m_strCode[60]` as weapon-part type and compares it with `_UnitBullet_fld::m_nWPType`; verify field mapping. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CItemLootTable.cpp` | `CItemLootTable::Indexing` | `m_tblLoot`, `itemTables[tableCode]` | `_ItemLooting_fld *` | Uses `_ItemLooting_fld::m_nLootListCount` and `_ItemLooting_fld::m_itmLootCodeKey[j]`; item existence flag uses `_ItemExist_fld::m_bExist` low byte (IDA-compatible). |
| `CItemUpgradeTable.cpp` | `CItemUpgradeTable::Indexing` | `m_tblItemUpgrade`, `resTable` | `_base_fld *` | Uses `_base_fld::m_strCode` key lookup in `resTable` and stores `_base_fld::m_dwIndex` to `m_pwResIndex`. |
| `COreCuttingTable.cpp` | `COreCuttingTable::Indexing` | `m_tblOreCutting`, `oreTable`, `resTable` | `_base_fld *` | Uses `Record[1].m_strCode[60]` for ore cutting rate and `reinterpret_cast<const char*>(&Record[1])` as resource-code key. [UNRESOLVED: see _reports/record_field_unresolved.tsv] |
| `CPlayer_Mining.cpp` | `CPlayer::pc_MineComplete` | `g_Main.m_tblItemData[17]` | `_base_fld *` | Uses _OreItem_fld::m_dwOreProbability and _OreItem_fld::m_nOre_Level. |
| `CPlayer.cpp` | `CPlayer::GetDamageLevel` | `g_Main.m_tblItemData[nAttackPart]` | `_base_fld *` | Uses _DfnEquipItem_fld::m_nLevelLim. |
| `CPlayer.cpp` | `CPlayer::GetDamageDP` | `g_Main.m_tblItemData[nAttackPart]` | `_base_fld *` | Uses _DfnEquipItem_fld::m_nDefence_DP. |
