# Static Data Log
Only unresolved static strings/arrays/data from IDA go here. Remove entries once recovered.

## Recent loop-chain literals (2026-02-12)
- "RaceBossSMSCurTime" / "Time" / "..\\SystemSave\\ServerState.ini" (CRaceBossMsgController::SaveCurTime)
- "cursor:%u hour:%u min:%u msec:%u time:%s" (CWorldSchedule::CheckSch)
- "code:%s event:%u info1:%u info2:%u" (CWorldSchedule::CheckSch)

## UnmannedTrader request literals (2026-02-13)
- "CUnmannedTraderUserInfoTable::Regist(...)" (CUnmannedTraderUserInfoTable::Regist)
- "CUnmannedTraderUserInfoTable::ModifyPrice(...)" (CUnmannedTraderUserInfoTable::ModifyPrice)
- "CUnmannedTraderUserInfoTable::CancelRegist(...)" (CUnmannedTraderUserInfoTable::CancelRegist)
- "CUnmannedTraderUserInfoTable::Buy(...)" (CUnmannedTraderUserInfoTable::Buy)
- "CUnmannedTraderUserInfoTable::Search(...)" (CUnmannedTraderUserInfoTable::Search)
- "CUnmannedTraderUserInfoTable::ReRegist(...)" (CUnmannedTraderUserInfoTable::ReRegist)
- Packet type bytes `{30, 31}` (CUnmannedTraderUserInfo::SendBuyErrorResult)

## `clear_file` literals (2026-02-13)
- "%s\\*.*" (`clear_file`)
- "%s\\%s" (`clear_file`)
- "." / ".." (`clear_file`)
- cutoff high-datetime multiplier `200` (`clear_file`)

## GuildBattle loop-chain literals (2026-02-13)
- "CGuildBattleScheduleManager::Clear() : %d Day!" (`CGuildBattleScheduleManager::Clear`)
- "CGuildBattleScheduleManager::Flip() : %d Day!" (`CGuildBattleScheduleManager::Flip`)
- "CGuildBattleController::Clear() SaveINI() Fail!" (`CGuildBattleController::Clear`)
- "CGuildBattleController::Flip() SaveINI() Fail!" (`CGuildBattleController::Flip`)
- "CGuildBattleReservedSchedule::CheckNextEvent( iRet(%d) ) : m_uiScheduleID(%u) : -1 == m_pkSchedule[%u]->Check() Invalid!" (`CGuildBattleReservedSchedule::CheckNextEvent`)
- DQS case id `0x24` (`CGuildBattleReservedScheduleMapGroup::PushDQSClear`)

## Apex bridge literals (2026-02-13)
- "Send_Trans - %d" (`CChiNetworkEX::Send_Trans`)
- "Send_ClienInform - %d" (`CChiNetworkEX::Send_ClienInform`)
- Apex message ids `0x52` and `0x54` (`CChiNetworkEX::Send_Trans` / `CChiNetworkEX::Send_ClienInform`)

## Unknown string labels (bytes not recovered)
- aRpkAaiAo (CMergeFileManager::InitMergeFile when .rpk count > 255)
- aA_14 / aA_15 / aA_16 (CNuclearBombMgr::LoadIni error messages for zero times)
- aGetyposinleafu / byte_140883769 (CBsp::GetYposInLeafUseEdgeNormal Warning message; old source: "GetYposInLeafUseEdgeNormal()함수는 서버에서 쓸겁니다.", "")
- aPathfinderStac (CPathFinder::PopPathStack error message; current guess: "pathfinder stack error")
- pathfinder stack overflow (CPathFinder::PushPathStack error message)
- L"pMon->m_pRecordSet" (DfAIMgr::Atp_SearchStart_OnLoop / DfAIMgr::CheckEmotionBad _wassert expression)
- L"G:\\00_ZoneServer_Source\\03_Temp_Source\\2009_05_13_Source_Oversea\\zoneserver\\GameMain\\NewMonster\\NewMonsterAI_Df_Handler.cpp" (DfAIMgr::Atp_SearchStart_OnLoop / DfAIMgr::CheckEmotionBad _wassert file path)
- "Birth %s >> %s Map(%s) Pos(%d, %d, %d)" (CMonster::_BossBirthWriteLog)
- "Death %s >> %s Map(%s) Pos(%d, %d, %d)" (CMonster::_BossDieWriteLog_Start)
- " " (CMonster::_BossDieWriteLog_Start separator)
- "%s >> %s CreatTime: %d/%d %d:%d DeadTime: %d/%d %d:%d (Map:%s, Pos:%d %d %d)" (CMonster::_BossDieWriteLog_Start)
- "%d * %d = %.0f" (CMonster::_BossDieWriteLog_Start)
- "%d * %d * 1.2 = %.0f" (CMonster::_BossDieWriteLog_Start)
- "\t system guard tower : %s" (CMonster::_BossDieWriteLog_Start)
- "\t %s (%d) : %s" (CMonster::_BossDieWriteLog_Start)
- "\t disconnect!! (%d) : %s" (CMonster::_BossDieWriteLog_Start)
- "\t <<TAKER>> %s (%d) Sec: %d" (CMonster::_BossDieWriteLog_Start)
- "<<PARTY>> " (CMonster::_BossDieWriteLog_Start)
- " %d " (CMonster::_BossDieWriteLog_Start)
- "\t %s" (CMonster::_BossDieWriteLog_Start)
- "\t NOBODY LOOT" (CMonster::_BossDieWriteLog_Start)
- "\r\n" (CMonster::_BossDieWriteLog_End)
- "\t LootItem : %s" (CMonster::_LootItem_Std/_LootItem_Rwp)
- "Event Set Item >> %s, %d" (CMonster::_LootItem_EventSet)
- "Skill Attack Error : Skill(%s), SIndex(%d), l_fConst(%f), l_nLvConst(%d), nMastery(%d), nMaxAF(%d), nMinAF(%d)" (CMonsterAttack::_CalcMonSkillAttPnt)
- "FlashDamage Error AttackTarget Map : Attack Obj( %s : %s ) Dst Obj( %s : %s )" (CAttack::FlashDamageProc)
- "OpenWorld(Account -> Zone)" (CNetworkEX::OpenWorldSuccessResult)
- "!!Server type is wrong!!(AC:%d)(ZO:%d)" (CNetworkEX::OpenWorldSuccessResult)
- "ServerType is Wrong ==> AccountServer(%d) != ZoneServer(%d)" (CNetworkEX::OpenWorldSuccessResult)
- "_uilock_update_result_acwr ret(%u) Account(%u) Fail!" (CNetworkEX::UILockRefreshResult)
- "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) pUser->m_idWorld.dwSerial(%u) != pRecv->idLocal.dwSerial(%u) Invalid!" (CNetworkEX::CheckIsBlockIPResult)
- "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) Account Server Push DQS Fail!" (CNetworkEX::CheckIsBlockIPResult)
- "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) DB Error!" (CNetworkEX::CheckIsBlockIPResult)
- "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) Invalid Return(%u) Value!" (CNetworkEX::CheckIsBlockIPResult)
- "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) pRecv->idLocal.wIndex(%u) >= MAX_PLAYER(%u) Invalid!" (CNetworkEX::CheckIsBlockIPResult)
- "CLOSE>> ForceCloseCommand Account: %s(%u) Type: %d, IP: %u.%u.%u.%u" (CNetworkEX::CheckIsBlockIPResult)
- "CUnmannedTraderController::InsertStateRecord()\r\n\t\tnew _unmannedtrader_stade_id_info[%u] NULL!\r\n" (CUnmannedTraderController::InsertStateRecord)
- "CUnmannedTraderController::InsertStateRecord()\r\n\t\tg_Main.m_pWorldDB->Insert_UnmannedTraderItemStateRecord( %u, wszStateName ) Fail!\r\n" (CUnmannedTraderController::InsertStateRecord)
- "CUnmannedTraderController::InsertStateRecord()\r\n\t\tg_Main.m_pWorldDB->Truncate_UnmannedTraderItemStateRecord() Fail!\r\n" (CUnmannedTraderController::InsertStateRecord)
- "CUnmannedTraderController::InsertStateRecord()\r\n\t\tg_Main.m_pWorldDB->Select_UnmannedTraderItemStateInfo() Fail!\r\n" (CUnmannedTraderController::InsertStateRecord)
- "CUnmannedTraderController::InsertStateRecord()\r\n\t\tg_Main.m_pWorldDB->Select_UnmannedTraderItemStateInfoCnt() Fail!\r\n" (CUnmannedTraderController::InsertStateRecord)
- "CUnmannedTraderController::UpdateClearDanglingOwnerRecord()\r\n\t\tg_Main.m_pWorldDB->Update_UnmannedTraderClearDanglingOwnerRecord() Fail!\r\n" (CUnmannedTraderController::UpdateClearDanglingOwnerRecord)
- "CUnmannedTraderController::InsertDefalutRecord()\r\n\t\tg_Main.m_pWorldDB->Insert_UnmannedTraderSingleDefaultRecord( %u ) Fail!\r\n" (CUnmannedTraderController::InsertDefalutRecord)
- "CUnmannedTraderController::InsertDefalutRecord()\r\n\t\tiCnt(%d) = g_Main.m_pWorldDB->Select_UnmannedTraderItemEmptyRecordCnt()!\r\n" (CUnmannedTraderController::InsertDefalutRecord)
- "CPostSystemManager::UpdateDisappearOwner\r\n\t\tg_Main.m_pWorldDB->Update_DisappearOwnerRecord() Fail!\r\n" (CPostSystemManager::UpdateDisappearOwnerRecord)
- "CPostSystemManager::InsertDefaultPSRecord\r\n\t\tg_Main.m_pWorldDB->Insert_PSDefaultRecord( %d ) Fail!\r\n" (CPostSystemManager::InsertDefaultPSRecord)
- "CPostSystemManager::InsertDefaultPSRecord\r\n\t\tnCount(%d) = g_Main.m_pWorldDB->Select_PostStorageEmptyRecord() Fail!\r\n" (CPostSystemManager::InsertDefaultPSRecord)
- "CGuildRoomSystem::Load_db() : g_Main.m_pWorldDB->Select_GuildRoomInfo(...) Fail!" (CGuildRoomSystem::Load_db)
- "CGuildRoomSystem::Load_db() : RentRoom(...) %uth Fail!" (CGuildRoomSystem::Load_db)
- "CGuildRoomSystem::Load_db() : CGuildRoomSystem::GetInstance()->GetMapData( byRace(%u), byRoomType(%u) ) %uth NULL!" (CGuildRoomSystem::Load_db)
- "CGuildRoomSystem::Load_db() : CItemStoreManager::Instance()->GetMapItemStoreListBySerial(pMap->m_nMapIndex(%d)) %uth NULL!" (CGuildRoomSystem::Load_db)
- "CGuildRoomSystem::Load_db() : CItemStoreManager::Instance()->GetEmptyInstanceItemStore() %uth NULL!" (CGuildRoomSystem::Load_db)
- "CGuildRoomSystem::Load_db() : pGuildStoreList->CopyItemStoreData(pBaseStoreList) %uth Fail!\r\nMap(%s) GuildRoom Store NPC Not Exist!" (CGuildRoomSystem::Load_db)
- "select gr.guildserial, g.id, gr.roomtype, gr.roomrace, gr.roomrentdate from tbl_GuildRoom as gr inner join tbl_Guild as g on gr.guildserial = g.serial where gr.dck = 0 order by g.serial desc" (CRFWorldDatabase::Select_GuildRoomInfo)
- "CItemStoreManager::SelectUsedRecordNum\r\n\t\tg_Main.m_pWorldDB->Select_UsedLimitItemRecordNum() Fail!\r\n" (CItemStoreManager::SelectUsedRecordNum)
- "CItemStoreManager::SelectTotalRecordNum\r\n\t\tg_Main.m_pWorldDB->Select_TotalRecordNum() Fail!\r\n" (CItemStoreManager::SelectTotalRecordNum)
- "CItemStoreManager::InsertNotEnoughLimitItemRecord\r\n\t\tg_Main.m_pWorldDB->Insert_LimitItemRecord() Fail!\r\n" (CItemStoreManager::InsertNotEnoughLimitItemRecord)
- "CItemStoreManager::SelectStoreLimitItem\r\n\t\tg_Main.m_pWorldDB->Select_StoreLimitItem() Fail!\r\n" (CItemStoreManager::SelectStoreLimitItem)
- "030F1" (CItemStoreManager::SetEnforceInitNormalStore)
- "040F1" (CItemStoreManager::SetEnforceInitNormalStore)
- "050F1" (CItemStoreManager::SetEnforceInitNormalStore)
- "select count(serial) from tbl_StoreLimitItem_061212 where dck=0" (CRFWorldDatabase::Select_UsedLimitItemRecordNum)
- "select count(serial) from tbl_StoreLimitItem_061212" (CRFWorldDatabase::Select_TotalRecordNum)
- "select k0,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,num0,num1,num2,num3,num4,num5,num6,num7,num8,num9,num10,num11,num12,num13,num14,num15,serial,type,typeserial,storeinx,resettime from tbl_StoreLimitItem_061212 where dck=0" (CRFWorldDatabase::Select_StoreLimitItem)
- "Kick By GM-Tool" (CNetworkEX::DisconnectGuildWarCharacterRequest)
- "%s-%s: Open World" (CMainThread::pc_OpenWorldSuccessResult)
- "DBInit Begin >> name: %s, ip: %s" (CMainThread::pc_OpenWorldSuccessResult)
- "DBInit Complete >>" (CMainThread::pc_OpenWorldSuccessResult)
- "DBInit Fail >>" (CMainThread::pc_OpenWorldSuccessResult)
- "init DB fail" (CMainThread::pc_OpenWorldSuccessResult)
- "pc_OpenWorldFailureResult" (CMainThread::pc_OpenWorldFailureResult)
- "request world-open fail" (CMainThread::pc_OpenWorldFailureResult)
- "Account Server Command" (CMainThread::pc_ForceCloseCommand)
- aA_17 (CMainThread::pc_TransIPKeyInform ForceCloseCommand reason; bytes unknown)
- "id:%s Bill:%d bAgeLimit:%d" (CMainThread::pc_TransIPKeyInform)
- "Enter world False" (CMainThread::pc_EnterWorldResult)
- "$????" (CMainThread::pc_AllUserMsgInform GM notice prefix; bytes unknown)
- "%s >> FAIL(grammar or logic)" (ProcessCheatCommand)
- "%s >> OK" (ProcessCheatCommand)
- "%s >> ERROR (authority)" (ProcessCheatCommand)
- "%s >> ERROR (command)" (ProcessCheatCommand)
- "Cash DB Init Begin >> name: %s, ip: %s" (CMainThread::pc_CashDBInfoRecvResult)
- "CMainThread::pc_CashDBInfoRecvResult" (CMainThread::pc_CashDBInfoRecvResult)
- "DSN : szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) Invalid" (CMainThread::pc_CashDBInfoRecvResult)
- "CMainThread::pc_CashDBInfoRecvResult : DSN : szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) Invalid" (CMainThread::pc_CashDBInfoRecvResult)
- "CMainThread::pc_CashDBInfoRecvResult Invalid DSN!" (CMainThread::pc_CashDBInfoRecvResult)
- "CashDBInit( szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) ) Fail!" (CMainThread::pc_CashDBInfoRecvResult)
- "CMainThread::pc_CashDBInfoRecvResult : CashDBInit( szIP(%s), szDBName(%s), szAccount(%s), szPassword(%s) dwPort(%d) ) Fail!" (CMainThread::pc_CashDBInfoRecvResult)
- "CMainThread::pc_CashDBInfoRecvResult CashDBInit Fail!" (CMainThread::pc_CashDBInfoRecvResult)
- "Cash DB Init Complete >>" (CMainThread::pc_CashDBInfoRecvResult)
- "%s-%s: Open Cash DB" (CMainThread::pc_CashDBInfoRecvResult)
- "DataBase Setting Start!! (%s : %s)" (CMainThread::DatabaseInit)
- "World DB ODBC Config Complete!!" (CMainThread::DatabaseInit)
- "Start World DataBase Complete!!" (CMainThread::DatabaseInit)
- "DataBase Setting Complete!! (%s : %s)" (CMainThread::DatabaseInit)
- "DatabaseInit" (CMainThread::DatabaseInit)
- "World DB ODBC Setting Faild!" (CMainThread::DatabaseInit)
- "Connect World DB Failed!" (CMainThread::DatabaseInit)
- "..\\ZoneServerLog\\" (CMainThread::DatabaseInit)
- "CMainThread::CashDBInit() : Cash Item DataBase Setting Complete!! (%s : %s)" (CMainThread::CashDBInit)
- "CMainThread::Init() : " (CMainThread::CashDBInit)
- "CCashDBWorkManager::Instance()->Initialize() Fail!" (CMainThread::CashDBInit)
- "CashDbWorker::Instance()->Initialize() Fail!" (CMainThread::CashDBInit)
- "ManageClientLimitRunRequest Start" (CMainThread::ManageClientLimitRunRequest)
- "odd.. %s: AttackPersonalRequest()..  if(!pDst)" (CNetworkEX::AttackPersonalRequest)
- "odd.. %s: AttackPersonalRequest()..  if(pRecv->byAttPart >= base_fix_num)" (CNetworkEX::AttackPersonalRequest)
- "odd.. %s: AttackSkillRequest()..  if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->wSkillIndex))" (CNetworkEX::AttackSkillRequest)
- "odd.. %s: AttackSkillRequest()..  if(pRecv->byEffectCode != effect_code_skill && pRecv->byEffectCode != effect_code_class)" (CNetworkEX::AttackSkillRequest)
- "odd.. %s: AttackForceRequest()..  if(pRecv->byID != 0xFF || pRecv->wIndex != 0xFFFF)" (CNetworkEX::AttackForceRequest)
- "odd.. %s: AttackUnitRequest()..  if(!pDst)" (CNetworkEX::AttackUnitRequest)
- "odd.. %s: AttackUnitRequest()..  if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)" (CNetworkEX::AttackUnitRequest)
- "odd.. %s: AttackTestRequest()..  if(pRecv->byEffectCode != 0xFF && pRecv->byEffectCode >= EFFECT_CODE_NUM) : %d" (CNetworkEX::AttackTestRequest)
- "odd.. %s: AttackTestRequest()..  if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->byEffectIndex))" (CNetworkEX::AttackTestRequest)
- "odd.. %s: AttackTestRequest()..  if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)" (CNetworkEX::AttackTestRequest)
- "odd.. %s: AttackSiegeRequest()..  if(pRecv->byAttPart >= base_fix_num)" (CNetworkEX::AttackSiegeRequest)
- "odd.. %s: RegistBindRequest()..  if(pRecv->wNPCIndex >= CItemStoreManager::Instance()->m_tblItemStore.GetRecordNum())" (CNetworkEX::RegistBindRequest)
- "CPlayer::pc_PlayAttack_Gen()---0" (CPlayer::pc_PlayAttack_Gen)
- "CPlayer::pc_PlayAttack_Gen()---1" (CPlayer::pc_PlayAttack_Gen)
- "CPlayer::pc_PlayAttack_Skill()---0" (CPlayer::pc_PlayAttack_Skill)
- "CPlayer::pc_PlayAttack_Skill()---1" (CPlayer::pc_PlayAttack_Skill)
- "CPlayer::pc_PlayAttack_Skill()---2" (CPlayer::pc_PlayAttack_Skill)
- "CPlayer::pc_PlayAttack_Force()---0" (CPlayer::pc_PlayAttack_Force)
- "CPlayer::pc_PlayAttack_Siege()---0" (CPlayer::pc_PlayAttack_Siege)
- "CPlayer::pc_PlayAttack_Siege()---1" (CPlayer::pc_PlayAttack_Siege)
- "-1" (CPlayer::make_gen_attack_param/make_force_attack_param/make_siege_attack_param/make_skill_attack_param)
- "1" (CPlayer::_pre_check_skill_attack)
- "Skill Attack Warning : nGaMaxAF(%d), Potion_Inc_Fc(%f), fAddBulletFc(%f)" (CPlayer::make_skill_attack_param)
- "CPlayer::DeleteUseConsumeItem()" (CPlayer::DeleteUseConsumeItem)
- "CPlayer::CalcExp()--0" (CPlayer::CalcExp)
- "CPlayer::CalcExp()--1" (CPlayer::CalcExp)
- "CPlayer::CalcExp()--2" (CPlayer::CalcExp)
- "odd.. %s: ItemboxTakeRequest()..  if(pRecv->wItemBoxIndex >= MAX_ITEMBOX)" (CNetworkEX::ItemboxTakeRequest)
- "odd.. %s: ThrowStorageRequest() : pRecv->Item.byStorageCode(%d)" (CNetworkEX::ThrowStorageRequest)
- "odd.. %s: UsePotionRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::UsePotionRequest)
- "odd.. %s: EquipPartRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::EquipPartRequest)
- "odd.. %s: EmbellishRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::EmbellishRequest)
- "odd.. %s: OffPartRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::EQUIP && pRecv->Item.byStorageCode != _STORAGE_POS::EMBELLISH)" (CNetworkEX::OffPartRequest)
- "odd.. %s: MakeItemRequest()..  if(pRecv->byMaterialNum > _make_item_request_clzo::material_num)" (CNetworkEX::MakeItemRequest)
- "odd.. %s: MakeItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemMakeData.GetRecordNum())" (CNetworkEX::MakeItemRequest)
- "odd.. %s: UpgradeItemRequest()..  if(pRecv->byJewelNum > upgrade_jewel_num)" (CNetworkEX::UpgradeItemRequest)
- "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::UpgradeItemRequest)
- "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posUpgJewel[i].byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::UpgradeItemRequest)
- "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::UpgradeItemRequest)
- "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::DownGradeItemRequest)
- "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::DownGradeItemRequest)
- "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::INVEN && pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::EQUIP)" (CNetworkEX::DownGradeItemRequest)
- "odd.. %s: CombineItemRequest()..  if(pRecv->byMaterialNum > _combine_item_request_clzo::material_num)" (CNetworkEX::CombineItemRequest)
- "odd.. %s: CombineItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemCombineData.GetRecordNum())" (CNetworkEX::CombineItemRequest)
- "odd.. %s: ExchangeItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemExchangeData.GetRecordNum())" (CNetworkEX::ExchangeItemRequest)
- "odd.. %s: CombineExItemRequest()..  if(pRecv->byCombineSlotNum > _combine_ex_item_request_clzo::combineslot_max ))" (CNetworkEX::CombineExItemRequest)
- "odd.. %s: CombineItemRequest()..  if(pRecv->wManualIndex >= ItemCombineMgr::ms_tbl_ItemCombine[eCOMMON].GetRecordNum()))" (CNetworkEX::CombineExItemRequest)
- "odd.. %s: CombineExItemAcceptRequest().. \tIsNotDlgType  " (CNetworkEX::CombineExItemAcceptRequest)
- "odd.. %s: CombineExItemAcceptRequest().. \tif(\tpRecv->SelectItemBuff.bySelectNum >= _combine_ex_item_accept_request_clzo::eMaxSelectItemNum ) " (CNetworkEX::CombineExItemAcceptRequest)
- "odd.. %s: UseRadarRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::UseRadarItemRequest)
- "odd.. %s: CharacterRenameCash()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)" (CNetworkEX::CharacterRenameCash)
- "odd.. %s: TalikCrystalExchangeRequest() : if(pRecv->byExchangeNum == 0)" (CNetworkEX::TalikCrystalExchangeRequest)
- "odd.. %s: TalikCrystalExchangeRequest() : pRecv->byExchangeNum(%d) >= _talik_crystal_exchange_clzo::exchange_zone(%d)" (CNetworkEX::TalikCrystalExchangeRequest)
- "CPlayer::pc_AddBag()" (CPlayer::pc_AddBag)
- "CPlayer::pc_CharacterRenameCash(...) : User %s(%u) Item Table(%u) Index(%u) Not Exist!" (CPlayer::pc_CharacterRenameCash)
- "bxgol04" (CPlayer::pc_CombineItem)
- "CPlayer::pc_CombineItem - Emb_AddStorage() Fail" (CPlayer::pc_CombineItem)
- "CPlayer::pc_DowngradeItem - Emb_AddStorage() Fail" (CPlayer::pc_DowngradeItem)
- "GradeDown Ground Reward" (CPlayer::pc_DowngradeItem)
- "GradeDown Reward" (CPlayer::pc_DowngradeItem)
- "Cauldron01" (CPlayer::pc_MovePortal)
- "resources" (CPlayer::pc_MovePortal)
- "Sette" (CPlayer::pc_MovePortal)
- "elan" (CPlayer::pc_MovePortal)
- "NeutralA" (CPlayer::pc_MovePortal)
- "NeutralB" (CPlayer::pc_MovePortal)
- "NeutralC" (CPlayer::pc_MovePortal)
- "CPlayer::pc_EmbellishPart() -- 0" (CPlayer::pc_EmbellishPart)
- "CPlayer::pc_EmbellishPart() -- 1" (CPlayer::pc_EmbellishPart)
- "CPlayer::pc_EmbellishPart() -- 1 Fail" (CPlayer::pc_EmbellishPart)
- "CPlayer::pc_EquipPart() -- 0" (CPlayer::pc_EquipPart)
- "CPlayer::pc_EquipPart() -- 1" (CPlayer::pc_EquipPart)
- "-" (CPlayer::pc_ExchangeItem)
- "-1" (CPlayer::pc_ExchangeItem)
- "CPlayer::pc_ExchangeItem()" (CPlayer::pc_ExchangeItem)
- "-1" (CPlayer::pc_MakeItem)
- "CPlayer::pc_MakeItem()---0" (CPlayer::pc_MakeItem)
- "CPlayer::pc_MakeItem()---1" (CPlayer::pc_MakeItem)
- "CPlayer::pc_OffPart()" (CPlayer::pc_OffPart)
- "\t Mob: %s Item: %s => %s ( %d ), Sec: %d" (CPlayer::pc_TakeGroundingItem)
- "bxgol03" (CPlayer::pc_TakeGroundingItem)
- "iyhol01" (CPlayer::pc_TakeGroundingItem)
- "pc_TakeGroundingItem() : Error Take Add Item Result : BoxItem TableCode(%u), ItemIndex(%u)" (CPlayer::pc_TakeGroundingItem)
- "pc_TakeGroundingItem() : Error Take New Item Result : BoxItem TableCode(%u), ItemIndex(%u)" (CPlayer::pc_TakeGroundingItem)
- "CPlayer::pc_ThrowStorageItem()" (CPlayer::pc_ThrowStorageItem)
- "CPlayer::pc_UpgradeItem()" (CPlayer::pc_UpgradeItem)
- "CPlayer::pc_UsePotionItem(...) : User %s(%u) Item Table(%u) Index(%u) Not Exist!" (CPlayer::pc_UsePotionItem)
- "ManageClientLimitRunRequest End" (CMainThread::ManageClientLimitRunRequest)
- "%u\t%04d-%02d-%02d %02d:%02d:%02d.%03d : %s" (OutputDebugLog)
- "\r\n" (OutputDebugLog)
- "..\\ZoneServerLog\\ServiceLog\\Cheat%d.log" (InitCheatCommand)
- "[ %s ] >> " (WriteCheatLog)
- "odd.. %s: DarkHoleEnterRequest()..  if(pRecv->wHoleIndex >= MAX_DARKHOLE)" (CNetworkEX::DarkHoleEnterRequest)
- "CNetworkEX::BossSMSMsgRequest() : %u(%s) !::IsSQLValidString( pRecv->wszChatData(%s) ) Invalid!" (CNetworkEX::BossSMSMsgRequest)
- "odd.. %s: BossSMSMsgRequest()..  if(pRecv->bySize > max_boss_sms_msg_size)" (CNetworkEX::BossSMSMsgRequest)
- "OPEN: %s, DARKHOLE:%s" (CPlayer::pc_DarkHoleOpenRequest)
- "ENTER: %s, (OPENER:%s, DARKHOLE:%s)" (CPlayer::pc_DarkHoleEnterRequest)
- "InvalidDungenName" (CPlayer::pc_DarkHoleEnterRequest)
- "REENTER: %s, (OPENER:%s, DARKHOLE:%s)" (CPlayer::pc_DarkHoleAnswerReenterRequest)
- "CPostSystemManager::PostSendRequest() : %u(%s) ::IsSQLValidString( wszRecvName(%s) ) Invalid!" (CPostSystemManager::PostSendRequest)
- "CPostSystemManager::PostSendRequest() : %u(%s) ::IsSQLValidString( wszTitle(%s) ) Invalid!" (CPostSystemManager::PostSendRequest)
- "CPostSystemManager::PostSendRequest() : %u(%s) ::IsSQLValidString( wszContent(%s) ) Invalid!" (CPostSystemManager::PostSendRequest)
- "CPostSystemManager::PostSendRequest()" (CPostSystemManager::PostSendRequest)
- " " (CPostSystemManager::PostSendRequest content default)
- "[ GM tool ] >> " (WriteCheatLog)
- "________PW__" (_WAIT_ENTER_ACCOUNT::SetUILock)
- "System" (CMainThread::SerivceSelfStart GetPrivateProfileStringA section)
- "GateIP" (CMainThread::SerivceSelfStart GetPrivateProfileStringA key)
- "X" (CMainThread::SerivceSelfStart GetPrivateProfileStringA default)
- "..\\WorldInfo\\WorldInfo.ini" (CMainThread::SerivceSelfStart GetPrivateProfileStringA path)
- "KeyCheckError.. Socket( %d ): ( pMsgHeader->m_wSize - sizeof(_MSG_HEADER) ) - sizeof( _enter_world_request_zone ) != 0 " (CNetworkEX::EnterWorldRequest)
- "KeyCheckError.. Socket( %d ): EnterWorldRequest().. if( strncmp( CMainThread::ms_szClientVerCheck, pRecv->szClientVerCheckKey, CMainThread::eClinetCheckMax ) )" (CNetworkEX::EnterWorldRequest)
- "KeyCheckError.. Socket( %d ): EnterWorldRequest().. if( strlen(pRecv->szClientVerCheckKey) != CMainThread::eClinetCheckMax )" (CNetworkEX::EnterWorldRequest)
- "CLOSE>> MoveLobbyRequest ID:%s" (CNetworkEX::MoveLobbyRequest)
- "CLOSE>> RegedCharRequest ID:%s" (CNetworkEX::RegedCharRequest)
- "odd.. id( %s ): AddCharRequest().. if(strlen(pRecv->szCharName) > max_name_len)" (CNetworkEX::AddCharRequest / AliveCharRequest)
- "CLOSE>> AddCharRequest ID:%s" (CNetworkEX::AddCharRequest)
- "odd.. id( %s ): DelCharRequest().. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)" (CNetworkEX::DelCharRequest)
- "CLOSE>> DelCharRequest ID:%s" (CNetworkEX::DelCharRequest)
- "odd.. id( %s ): SelCharRequest().. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)" (CNetworkEX::SelCharRequest)
- "odd.. id( %s ): SelCharRequest().. UI Lock Not Certified!" (CNetworkEX::SelCharRequest)
- "CLOSE>> SelCharRequest ID:%s" (CNetworkEX::SelCharRequest)
- "odd.. AliveCharRequest() .. if(pRecv->byCase != 0 && pRecv->byCase != 1)" (CNetworkEX::AliveCharRequest)
- "odd.. AliveCharRequest() .. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)" (CNetworkEX::AliveCharRequest)
- "odd.. %s: ChatOperatorRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatOperatorRequest)
- "odd.. %s: ChatOperatorRequest()..  if(pRecv->byRaceCode >= RACE_NUM && pRecv->byRaceCode != 0xFF)" (CNetworkEX::ChatOperatorRequest)
- "odd.. %s: ChatCircleRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatCircleRequest)
- "odd.. %s: ChatFarRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatFarRequest)
- "odd.. %s: ChatPartyRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatPartyRequest)
- "odd.. %s: ChatRaceRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatRaceRequest)
- "odd.. %s: ChatCheatRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatCheatRequest)
- "odd.. %s: ChatMgrWhisperRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatMgrWhisperRequest)
- "odd.. %s: ChatMapRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatMapRequest)
- "odd.. %s: ChatRaceBossRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatRaceBossRequest)
- "odd.. %s: ChatGuildEstSenRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatGuildEstSenRequest)
- "odd.. %s: ChatRePresentationRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatRePresentationRequest)
- "odd.. %s: ChatAllRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatAllRequest)
- "odd.. %s: ChatGreetingMsg_GM()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatGreetingMsg_GM)
- "CNetworkEX::ChatGreetingMsg_GM() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!" (CNetworkEX::ChatGreetingMsg_GM)
- "move count hack(start) warning! >> %s (one: %d, recv: %d)" (CNetworkEX::NextPoint)
- "move count hack(start) >> %s (one: %d, recv: %d)" (CNetworkEX::NextPoint)
- "odd.. %s: NextPoint()..  if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run && pRecv->byMoveType != move_type_fly)" (CNetworkEX::NextPoint)
- "move count hack(real) warning! >> %s (one: %d, recv: %d)" (CNetworkEX::RealMovPosRequest)
- "move count hack(real) >> %s (one: %d, recv: %d)" (CNetworkEX::RealMovPosRequest)
- "move count hack(stop) warning! >> %s (one: %d, recv: %d)" (CNetworkEX::Stop)
- "move count hack(stop) >> %s (one: %d, recv: %d)" (CNetworkEX::Stop)
- "odd.. %s: MoveTypeChangeRequeset()..  if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run)" (CNetworkEX::MoveTypeChangeRequeset)
- "odd.. %s: GotoAvatorRequest()..  if(pOne->m_byUserDgr == USER_DEGREE_STD)" (CNetworkEX::GotoAvatorRequest)
- "odd.. %s: GotoAvatorRequest()..  if(strlen(pRecv->szAvatorName) > max_name_len)" (CNetworkEX::GotoAvatorRequest)
- "Elan" (CPlayer::pc_GotoBasePortalRequest map code check)
- "Platform01" (CPlayer::pc_GotoBasePortalRequest map code check)
- "Medicallab" (CPlayer::pc_GotoBasePortalRequest map code check)
- "NeutralA" (CPlayer::pc_GotoBasePortalRequest map code check)
- "NeutralB" (CPlayer::pc_GotoBasePortalRequest map code check)
- "NeutralC" (CPlayer::pc_GotoBasePortalRequest map code check)
- "DELETE NPC QUEST ITEM : %s_%u_[%I64u] [%s %s]\r\n" (CMgrAvatorItemHistory::delete_npc_quest_item)
- "CQuestMgr::CheckReqAct : DeleteQuestItem(%s, %d) : %s" (CQuestMgr::CheckReqAct)
- "-1" (Quest string sentinel in CQuestMgr::CheckReqAct / CheckFailCondition / Emb_CompleteQuest)
- "CQuestMgr::GiveItem() -- 1" (CQuestMgr::DeleteQuestItem)
- "%s : Update_QuestInsert(EXIST) : slot : %d" (CUserDB::Update_QuestInsert)
- "%s : Update_QuestInsert(SlotIndex OVER) : slot : %d" (CUserDB::Update_QuestInsert)
- "%s : Update_QuestDelete(EXIST) : slot : %d" (CUserDB::Update_QuestDelete)
- "%s : Update_QuestDelete(SlotIndex OVER) : slot : %d" (CUserDB::Update_QuestDelete)
- "%s : Update_QuestUpdate(NOTHING) : slot : %d" (CUserDB::Update_QuestUpdate)
- "%s : Update_QuestUpdate(SlotIndex OVER) : slot : %d" (CUserDB::Update_QuestUpdate)
- "%s : Update_NPCQuestHistory(Index OVER) : %d" (CUserDB::Update_NPCQuestHistory)
- "Quest" (CPlayer::_Reward_Quest reward_add_money)
- "Quest G (%s)" (CPlayer::_Reward_Quest item box clause)
- "Quest (%s)" (CPlayer::_Reward_Quest inventory clause)
- "CPlayer::_Reward_Quest() - Emb_AddStorage() Fail" (CPlayer::_Reward_Quest)
- "CPlayer::_Reward_Quest()---0" (CPlayer::_Reward_Quest)
- "CPlayer::_Reward_Quest()---1" (CPlayer::_Reward_Quest)
- "CPlayer::pcChatAllRequest()" (CPlayer::Emb_StartQuest Emb_DelStorage error source)
- "odd.. %s: ChatGreetingMsg_RACE()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatGreetingMsg_RACE)
- "CNetworkEX::ChatGreetingMsg_RACE() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!" (CNetworkEX::ChatGreetingMsg_RACE)
- "odd.. %s: ChatGreetingMsg_GUILD()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatGreetingMsg_GUILD)
- "CNetworkEX::ChatGreetingMsg_GUILD() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!" (CNetworkEX::ChatGreetingMsg_GUILD)
- "odd.. %s: ChatTradeMsg()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatTradeRequestMsg)
- "odd.. %s: ChatGuildRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatGuildRequest)
- "odd.. %s: ChatMultiFarRequest()..  if(byNameLen > max_name_len)" (CNetworkEX::ChatMultiFarRequest)
- "odd.. %s: ChatMultiFarRequest()..  if(byMsgSize >= max_message_size)" (CNetworkEX::ChatMultiFarRequest)
- "odd.. %s: ChatMultiFarRequest()..  if(byTransNum > max_multi_far_msg)" (CNetworkEX::ChatMultiFarRequest)
- "odd.. %s: ChatRaceBossCryRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatRaceBossCryRequest)
- "odd.. %s: ChatGmNoticeRequest()..  if(pRecv->bySize > max_message_size)" (CNetworkEX::ChatGmNoticeRequest)
- "%d.. Enter_Account : m_bActive == true" (CUserDB::Enter_Account)
- "%d.. Enter_Account : if(!m_ss.chk_enter())" (CUserDB::Enter_Account)
- "%d.. Enter_Account : pWaitData == NULL" (CUserDB::Enter_Account)
- "%s.. Enter_Account : if(dwProtocolVer != __PROTOCOL_VER)" (CUserDB::Enter_Account)
- "%s.. Enter_Account : if(g_Player[m_idWorld.wIndex].m_bLoad)" (CUserDB::Enter_Account)
- "%s.. Enter_Account : if(dwAccountSerial == p->m_dwAccountSerial)" (CUserDB::Enter_Account)
- "%s.. Enter_Account Billing Type = 0" (CUserDB::Enter_Account)
- "%s.. Enter_Account Billing lRemainTime = 0" (CUserDB::Enter_Account)
- "CUserDB::Insert_Char_Request() : Account : %s(%u) ::IsSQLValidString(pwszCharName(%s)) Invalid!" (CUserDB::Insert_Char_Request)
- "CPlayer::pcChatAllRequest()" (CPlayer::pc_ChatAllRequest Emb_DelStorage error context)
- "Regist Character Request [%s %s]\r\n" (CMgrAccountLobbyHistory::reged_char_request)
- "Add Character Request [%s %s]\r\n" (CMgrAccountLobbyHistory::add_char_request)
- "Del Character Request [%s %s]\r\nSlot: %d\r\nCharSR: %d\r\n" (CMgrAccountLobbyHistory::del_char_request)
- "Select Character Request [%s %s]\r\nSlot: %d\r\nCharSR: %d\r\n" (CMgrAccountLobbyHistory::sel_char_request)
- "Add Result: %d (%s)\r\n" (CMgrAccountLobbyHistory::add_char_complete)
- "[Slot%d]\r\nNAME: %s\r\nCharSR: %d\r\nLV: %d\r\n$D: %d\r\n$G: %d\r\n\r\n" (CMgrAccountLobbyHistory::add_char_complete)
- "Add Character Complete [%s %s]\r\n" (CMgrAccountLobbyHistory::add_char_complete)
- "Tutorial Process Report Received [%s %s]\r\n" (CMgrAccountLobbyHistory::tutorial_process_report_recv)
- "\r\n\t============\r\n\r\n" (CMgrAccountLobbyHistory::* log separators)
- "%s\\%d\\%d\\%d_%s.his" (CMgrAccountLobbyHistory::GetNewFileName)
- "SQL filter patterns: ';', \"'\", \"--\", \"/*\", \"*/\", \"xp_\"" (IsSQLValidString)
- "odd.. %s: NewPosStartRequest()..  if(pRecv->byMapInMode >= mapin_type_num)" (CNetworkEX::NewPosStartRequest)
- "odd.. %s: BaseDownloadRequest()..  if(pOne->m_bBaseDownload)" (CNetworkEX::BaseDownloadRequest)
- "odd.. %s: InvenDownloadRequest()..  if(pOne->m_bInvenDownload)" (CNetworkEX::InvenDownloadRequest)
- "odd.. %s: CumDownloadRequest()..  if(pOne->m_bCumDownload)" (CNetworkEX::CumDownloadRequest)
- "odd.. %s: ForceDownloadRequest()..  if(pOne->m_bForceDownload)" (CNetworkEX::ForceDownloadRequest)
- "odd.. %s: QuestDownloadRequest()..  if(pOne->m_bQuestDownload)" (CNetworkEX::QuestDownloadRequest)
- "odd.. %s: SpecialDownloadRequest()..  if(pOne->m_bSpecialDownload)" (CNetworkEX::SpecialDownloadRequest)
- "odd.. %s: OtherShapeRequest()..  if(pRecv->wIndex >= MAX_PLAYER)" (CNetworkEX::OtherShapeRequest)
- "odd.. %s: OtherShapeRequest()..  if(pRecv->byReqType != _other_shape_request_clzo::SHAPE_ALL && pRecv->byReqType != _other_shape_request_clzo::SHAPE_PART)" (CNetworkEX::OtherShapeRequest)
- "odd.. %s: AMP_InvenDownloadRequest()..  if(pOne->m_bAMPInvenDownload)" (CNetworkEX::AMP_DownloadRequest)
- "SPY GM !!" (CPlayer::CreateComplete)
- aClose_0 (CPlayer::pc_NewPosStart close message; guessed format: "Close.. %s: MapInMode(%u) Reason(%d)")
- aCloseCreateS (CPlayer::Create close message; guessed format: "Close.. %s: Create() Map(%s) Pos(%d, %d, %d)")
- "%s\\%d" (CMgrAvatorItemHistory::GetNewFileName)
- "[AniMaster:%s]" (CPlayer::SendMsg_MonsterAggroData)
- "%s" (CPlayer::SendMsg_MonsterAggroData format for player name)
- "%s: Emb_AddStorage.. total_storage_num is over. storage: %d, item: %d-%d: " (CPlayer::Emb_AddStorage)
- "%s: Emb_AddStorage.. TransInCon()error storage: %d, item: %d-%d: " (CPlayer::Emb_AddStorage)
- "%s: Emb_AddStorage.. Item is not exist. storage: %d, item: %d-%d: " (CPlayer::Emb_AddStorage)
- "%s: Emb_AddStorage.. InsertLink ()error storage: %d, item: %d-%d: " (CPlayer::Emb_AddStorage)
- "%s: Emb_AlterDurPoint.. TimeLimitJadeMng::DeleteList() error storage: %d, item[%s]: %d-%d: " (CPlayer::Emb_AlterDurPoint)
- "%s: Emb_AlterDurPoint.. AlterCurDur() error storage: %d, slot: %d: " (CPlayer::Emb_AlterDurPoint)
- "%s: Emb_DelStorage.. Storage Code Over storage: %d, slot: %d: CodePos: %s" (CPlayer::Emb_DelStorage)
- "%s: Emb_DelStorage.. Storage Code Over storage: %d, slot: %d: " (CPlayer::Emb_DelStorage)
- "%s: Emb_DelStorage.. EmptyCon()error storage: %d, slot: %d: CodePos: %s" (CPlayer::Emb_DelStorage)
- "%s: Emb_DelStorage.. EmptyCon()error storage: %d, slot: %d: " (CPlayer::Emb_DelStorage)
- "%s: Emb_DelStorage.. TimeLimitJadeMng::DeleteList() error storage: %d, item: %d-%d: " (CPlayer::Emb_DelStorage)
- "CPlayer::_TowerDestroy()" (CPlayer::Emb_DelStorage error source string)
- "CPlayer::NetClose() : Time Limit System Stop!! : ID = %s" (CPlayer::NetClose)
- "Normal" (CPlayer::NetClose item history close)
- "Abnormal" (CPlayer::NetClose item history close)
- "resources" (CPlayer::NetClose map code check)
- "0" (CPlayer::Update_Bind default string)
- "%s\\%d\\%d" (CMgrAvatorItemHistory::GetNewFileName)
- "0%d" (CMgrAvatorItemHistory::GetNewFileName)
- "%d" (CMgrAvatorItemHistory::GetNewFileName)
- "%s%s%s" (CMgrAvatorItemHistory::GetNewFileName)
- "%s\\%d\\%d\\%d_%s.his" (CMgrAvatorItemHistory::GetNewFileName)
- "%s\\%d" (CMgrAvatorLvHistory::GetNewFileName)
- "%s\\%d\\%d" (CMgrAvatorLvHistory::GetNewFileName)
- "0%d" (CMgrAvatorLvHistory::GetNewFileName)
- "%d" (CMgrAvatorLvHistory::GetNewFileName)
- "%s%s%s" (CMgrAvatorLvHistory::GetNewFileName)
- "%s\\%d\\%d\\%d_%s.lhi" (CMgrAvatorLvHistory::GetNewFileName)
- "%01d%02d%02d%02d%02d" (GetConnectTime_AddBySec)
- "%s lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tW%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tD: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tP: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tS%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tF%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tM%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tR: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\r\n" (CMgrAvatorLvHistory::start_mastery)
- "NAME: %s [%s %s]\r\nWORLD: %s\r\nSR: %d\r\nID: %s (%d)\r\nDGR: %d\r\nLV: %d\r\nXP: %.0f (%d)\r\nRACE: %d\r\nSEX: %d\r\nCurClass:%s\r\nOldClass 0:%s, 1:%s, 2:%s\r\nLastClassGrade: %d\r\nInitClassCnt: %d\r\n$D: %u  ( %u push )\r\n$G: %u\r\nPvP: %.0f\r\nCB: %.0f\r\nBAG: %d\r\nIP: %s\r\nTIME: %d\r\n\r\n" (CMgrAvatorItemHistory::have_item)
- "\r\n\t============\r\n\r\n" (CMgrAvatorItemHistory::have_item)
- "LV: %d\r\nXP: %.0f (%d)\r\n$D: %u\r\n$G: %u\r\nPvP: %.0f\r\nCB: %.0f\r\nTIME: %d\r\n\r\n" (CMgrAvatorItemHistory::have_item)
- "EQUIP\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_@%s[%I64u]\r\n" (CMgrAvatorItemHistory::have_item)
- "EMBELL\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s[%I64u]\r\n" (CMgrAvatorItemHistory::have_item)
- "INVEN\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%u_@%s[%I64u]\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%u_@%s[%I64u] \t#push\r\n" (CMgrAvatorItemHistory::have_item)
- "FORCE\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%u[%I64u]\r\n" (CMgrAvatorItemHistory::have_item)
- "RES\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%u\r\n" (CMgrAvatorItemHistory::have_item)
- "ANIMUS\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%I64u[%I64u]\r\n" (CMgrAvatorItemHistory::have_item)
- "UNIT\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%d>fr:%d %d/%d/%d/%d/%d/%d\r\n" (CMgrAvatorItemHistory::have_item)
- "TRUNK (slot:%d, ^D:%.0f, ^G:%.0f) ( ^D:%.0f, ^G:%.0f push )\r\n" (CMgrAvatorItemHistory::have_item)
- "TRUNK (slot:%d, ^D:%.0f, ^G:%.0f)\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%u_@%s[%I64u] %s\r\n" (CMgrAvatorItemHistory::have_item)
- "\t%s_%u_@%s[%I64u] %s\t#push\r\n" (CMgrAvatorItemHistory::have_item)
- "CMgrAvatorItemHistory::have_item() : _CUTTING_DB_BASE::_LIST* pList->Key.wItemIndex(%u) i(%d) Serial(%u)" (CMgrAvatorItemHistory::have_item)
- "-1" (CMgrAvatorItemHistory::have_item class record fallback)

## Unknown data tables (bytes not recovered)
- cvt_string static buffer `sBuffer` size unknown (using 32 bytes)
- UnLockDDS XOR tables (unk_1409788D4 / unk_1409788D8); current code uses old-source constants
- EmotionType table (EMOTYPE[] used by DfAIMgr::CheckEmotionBad; values not recovered, currently zeroed)
- DfAIMgr::Mon_SearchStart_OnLoop static NearMon[_NEAR_DATA x20]
- DfAIMgr::Mv_Go_OnLoop static fNextTarPos[3]
- DfAIMgr::SearchPatrollPath static vTargetPos_0[3]
- DfAIMgr::SearchCharacterPath static vTargetPos[3]
- DfAIMgr::ChangeTargetPos static NewTar[3] / fBeforeTar[3]
- Attack limit tables s_Mon_nLimitDist / s_Mon_nLimitRadius / s_Mon_nLimitAngle (used by CMonsterAttack; values not recovered, currently zeroed)
- Attack limit tables s_nLimitDist / s_nLimitRadius / s_nLimitAngle (used by CPlayerAttack; values not recovered, currently zeroed)
- Effect limit tables s_nLimitRadius_0 / s_nLimitAngle_0 / s_nLimitAngle_1 (used by CCharacter effect targeting; values not recovered, currently zeroed)
- Character name filters (wszNonMakeName_0 / wszNonMakeName_1 / wszNonMakeName / wszGMCmp) used by name validation; current guess: {"GM","ADMIN","OPERATOR"} and prefix "GM"
- CPlayer::pc_UpgradeItem probability tables v30[77], v37[77], v38[77] (see CPlayer__pc_UpgradeItem_1.c assignments)
  v30 = {1000,750,500,250,100,50,0, 1000,750,500,250,100,50,0, 1000,750,500,250,100,50,0, 1000,750,500,250,100,50,0, 328,246,164,82,49,16,0, 640,480,320,160,96,32,0, 512,384,256,128,77,26,0, 410,307,205,102,61,20,0, 800,600,400,200,120,40,0, 262,197,131,66,39,13,0, 210,157,105,52,31,10,0}
  v37 = {0,0,0,0,1600,3200,6400, 0,0,1200,1600,3000,4400,6800, 0,0,0,1000,2250,3500,5750, 0,750,1125,1500,2375,3250,4625, 2400,3940,6170,6790,7470,8220,9040, 7000,7700,8470,9320,10000,10000,10000, 4900,6160,7620,8390,9220,10000,10000, 3430,4930,6860,7550,8300,9130,10000, 10000,10000,10000,10000,10000,10000,10000, 1680,3150,5560,6110,6720,7400,8140, 1180,2520,5000,5500,6050,6660,7320}
  v38 = {0,0,0,0,2000,4000,8000, 0,0,2400,3200,6000,8800,9500, 0,0,0,2000,4500,7000,9500, 0,3000,4500,6000,9500,9500,9500, 0,7300,9500,9500,9500,9500,9500, 0,4300,6400,8600,9500,9500,9500, 0,5100,7600,9500,9500,9500,9500, 0,6100,9100,9500,9500,9500,9500, 0,3600,5400,7200,9500,9500,9500, 0,8700,9500,9500,9500,9500,9500, 0,9500,9500,9500,9500,9500,9500}

- "MAKE(FAIL): [%s %s]\r\n" (CMgrAvatorItemHistory::make_item)
- "MAKE(SUCC): %s_%u_@%s[%I64u] [%s %s]\r\n" (CMgrAvatorItemHistory::make_item)
- "MAKE(QUEST): %s_%u_@%s[%I64u] [%s %s]\r\n" (CMgrAvatorItemHistory::make_item)
- "\t- %s_%d[%I64]\r\n" (CMgrAvatorItemHistory::make_item)
- "MAKE(FAIL): [%s %s] ( CHEAT_MATERIAL ) \r\n" (CMgrAvatorItemHistory::cheat_make_item_no_material)
- "MAKE(SUCC): %s_%u_@%s[%I64u] [%s %s] ( CHEAT_MATERIAL ) \r\n" (CMgrAvatorItemHistory::cheat_make_item_no_material)
- "UPGRADE(FAIL): %s_%u_@%s[%I64u] [%s %s]\r\n" (CMgrAvatorItemHistory::grade_up_item)
- "UPGRADE(ZERO): %s_%u_@%s[%I64u]->%s [%s %s]\r\n" (CMgrAvatorItemHistory::grade_up_item)
- "UPGRADE(LOST): %s_%u_@%s[%I64u] [%s %s]\r\n" (CMgrAvatorItemHistory::grade_up_item)
- "UPGRADE(SUCC): %s_%u_@%s[%I64u]->%s [%s %s]\r\n" (CMgrAvatorItemHistory::grade_up_item)
- "\t- T %s \r\n" (CMgrAvatorItemHistory::grade_up_item)
- "\t- R %s\r\n" (CMgrAvatorItemHistory::grade_up_item)
- "DOWNGRADE: %s_%u_@%s[%I64u] -> %s [%s %s]\r\n" (CMgrAvatorItemHistory::grade_down_item)
- "\t- T %s\r\n" (CMgrAvatorItemHistory::grade_down_item)
- "COMBINE: %s_%u_@%s[%I64u] pay(D:%u) $D:%u [%s %s]\r\n" (CMgrAvatorItemHistory::combine_item)
- "\t- %s_%d\r\n" (CMgrAvatorItemHistory::combine_item)
- "EXCHANGE: %s_%u_@%s[%I64u] [%s %s]\r\n" (CMgrAvatorItemHistory::exchange_item)
- "\t- %s_%u_%s\r\n" (CMgrAvatorItemHistory::exchange_item)
- "Amb_AddStorage ERR - item:[%d-%d], CodePos:(%s) \r\n" (CMgrAvatorItemHistory::add_storage_fail)

## Unknown constants (values not recovered)
- CMoveMapLimitEnviromentValues::ELAN_1TH_LIMIT_NPC_RECORD_INDEX (used by RequestElanMapUserForceMoveHQ)
- Attack scaling constants fR / fRLf / fRMf / fR_0 / fRLf_0 / fRMf_0 / fR_1 / fRLf_1 / fRMf_1 (used by CAttack/CMonsterAttack/CPlayerAttack; values not recovered, currently zeroed)
- "17" (CCharacter::InsertSFContEffect GetRecord key for effect code 3)
- "-1" (CCharacter::InsertSFContEffect compares effectRecord[12].m_strCode)
- e_nTaxDft (economy default tax value; initialization not recovered)
- e_nMgrDft (economy default manager value; initialization not recovered)

## Newly logged (CheatCommands)
- aCheatHelpSynta_0 (ct_period_time_set help syntax string; currently "<unknown>")
- aCheatHelpSynta_1 (ct_tl_info_set help syntax string; currently "<unknown>")
- aCheatHelpSynta_2 (ct_action_point_set help syntax string; currently "<unknown>")
- aCheatHelpSynta_3 (ct_Gold_Age_Set_Event_Status help syntax string; currently "<unknown>")
- aOne_3 (ct_Gold_Age_Event_Status disabled-state text; currently "<unknown>")
- aCoacE (ct_Gold_Age_Event_Status response format; currently "<unknown>")
- aCoacCodeDBoxAc (ct_Gold_Age_Get_Box_Cnt response format; currently "<unknown>")
- aCoacE_0 (ct_Gold_Age_Get_Box_Cnt inactive-state text; currently "<unknown>")
- byte_1407AF104 / byte_1407AF0F8 / byte_1407AF110 (ct_Gold_Age_Event_Status state labels; currently "<unknown>")

## Newly logged (CHonorGuild)
- "Changed Honor Guild (RACE:%d) : (%s:%d%%)" (CHonorGuild::ChangeHonorGuild)
- "Faild Current CHonorGuild::LoadDB(RACE:%d)"
- "Faild Next CHonorGuild::LoadDB(RACE:%d)"
- "Faild Load ClearHonorGuild(RACE:%d)"
- "Success CHonorGuild::LoadDB()"

## Newly logged (CMainThread::_GameDataBaseInit)
- "Avators DBRestore Complete!!"
- "Economy data load fail"
- "Economy data init fail"
- "Economy History Load Complete!!"
- "CMainThread::_GameDataBaseInit()"
- "AutominePersonalMgr::CreateDBTable Fail!"
- "AutominePersonalMgr Create Database table Fail!"
- "CUnmannedTraderController::Instance()->Load() Fail!"
- "CGuildRoomSystem::GetInstance()->Load_db()"
- "CPostSystemManager::Instace()->Load() Fail!"
- "db_LoadGreetingMsg() Fail!"
- "CItemStoreManager::Instance()->Load() Fail!"
- "CHonorGuild::Instance()->LoadDB() Fail!"
- "CRaceBossWinRate::Instance()->LoadDB() Fail!"
- "_db_Load_GoldBoxItem() Fail!"
- "_db_Load_GoldBoxItem Fail!"
- "InitAutoMineMachines() Fail!"
- "ATradeTaxRateTable() Fail!"
- "CWeeklyGuildRankManager::Instance()->Load() Fail!"
- "CTotalGuildRankManager::Instance()->Load() Fail!"
- "CGuildBattleController::Instance()->Load() Fail!"
- "CPvpUserRankingAndGuildRankingSystem::Instance()->Load() Fail!"

## Newly logged (CRFWorldDatabase::create_table_atrade_taxrate)
- "CREATE TABLE [dbo].[tbl_ATradeTaxRate] ( [serial] [int] IDENTITY (1, 1) NOT NULL , [Race] [tinyint] NOT NULL , [GSerial] [int] NOT NULL , [GName] [nvarchar] (24) NOT NULL , [Tax] [tinyint] NOT NULL , [NextTax] [tinyint] NOT NULL , [UpdateTime] [datetime] NOT NULL ) ON [PRIMARY] ALTER TABLE [dbo].[tbl_ATradeTaxRate] WITH NOCHECK ADD CONSTRAINT [PK_tbl_ATradeTaxRate] PRIMARY KEY  CLUSTERED ([serial])  ON [PRIMARY] ALTER TABLE [dbo].[tbl_ATradeTaxRate] WITH NOCHECK ADD CONSTRAINT [DF_tbl_ATradeTaxRate_GSerial] DEFAULT (-1) FOR [GSerial], CONSTRAINT [DF_tbl_ATradeTaxRate_GName] DEFAULT ('*') FOR [GName], CONSTRAINT [DF_tbl_ATradeTaxRate_Tax] DEFAULT (5) FOR [Tax], CONSTRAINT [DF_tbl_ATradeTaxRate_NextTax] DEFAULT (5) FOR [NextTax], CONSTRAINT [DF_tbl_ATradeTaxRate_UpdateTime] DEFAULT (getdate()) FOR [UpdateTime] CREATE  INDEX [IX_Race] ON [dbo].[tbl_ATradeTaxRate]([Race]) ON [PRIMARY]"

## Newly logged (CRFWorldDatabase HonorGuild)
- "{ CALL pSelect_HonorGuild( %d, %d ) }"
- "SQLFetch"
- "SQLGetData"
- "SQLExecDirectA"
- "ReConnectDataBase Fail. Query : %s"
- "Select GuildSerial From tbl_honor_guild Where Race=%d AND GuildSerial=0xFFFFFFFF And DCK=0 AND IsNext=1"

## Newly logged (Ranking System)
- "Race_%d Ranking Start!!"
- "Race_%d Ranking Complete!!"
- "DatabaseInit"
- "load race rank fail"
- "PvpRankData Packing Complete!!"
- "!!! Failed init(Patriarch Elect system.)"
- "tbl_PvpRank%s"
- "create race-rank-table fail"
- "Today Rank Table(%s) Make Complete!!"
- "CUserRankingProcess::Load() : LoadPvpRank(szDate(%s)) Fail!"
- "CUserRankingProcess::Load() : CheckAndCreateTodayPvpRankTable(szDate(%s)) Fail!"
- "Tommorrow Rank Table(%s) Not Exist!!"
- "Guild Rank Table Make Start!!"
- "tbl_GuildRank%s"
- "Create guild-rank-table fail\r\nUpdate_GuildRank Fail!"
- "Guild Rank Table Make Complete!!"
- "update guild-grade fail"
- "bool CGuildRanking::Load() : SetLoadAllGuildInfo(..) Fail!\r\n"
- "bool CGuildRanking::Load() : UpdateRankAndGrade(..) Fail!\r\n"
- "Guild List(%d) Load Complete!!"
- "Select_AllGuildData() fail"
- "Guild(%u, %s) CheckSum Load Fail!"
- "Guild(%u, %s) CheckSum Insert Fail!"
- "Guild(%u, %s) Diff Value  dalant(%f) -> %f, Gold(%f) -> %f"
- "guild_master_data_error and modify Data(guild:%u, user:%u:, ClassInGuild:%d )"
- "Update_GuildMemberCount guild(%u) cnt(%u) Fail!"
- "CGuildRanking::SetLoadAllGuildInfo(...) : %d > g_Main.check_min_max_guild_money(...) Fail!"
- "Error"
- "CGuildRanking::SetLoadAllGuildInfo(...) : \r\n%d > g_Main.check_min_max_guild_money(...) Fail!"
- "Guild List(%d) Setting Complete!!"
- "%d : %s > Guild DALANT (%d) (ADJUST %d)"
- "%d : %s > Guild GOLD (%d) (ADJUST %d)"
- "%s Success"

## Newly logged (Economy System)
- "Economy"
- "Default_OreVal"
- ".\\Initialize\\WorldSystem.ini"
- "Economy Error"
- "Nothing Default MgrValue"
- "number of history(%d) is more than MAX_ECONOMY_HISTORY (%d)"

## Newly logged (CGuild::ManageBuyGuildEmblem)
- "<unknown>"

## Newly logged (CMainThread Greeting/Limit)
- "GM"
- "Welcome to RF World "
- "RaceBoss"
- "There is no Message "
- "tbl_characterselect_log_%s"
- "CMainThread::_GameDataBaseInit()"
- "Create %s Table Failed!"
- "Character Select Log Table(%s) Make Complete!!"
- "CMainThread::LoadLimitInfo() : Set Force Close Time( %u Min %u Sec )"
- "Battle Tournament Winner Info Setting Fail! Winner Info Disable!"
- "Can't Battle Tournament Winner Info Getting From DB!"

## Newly logged (CRFWorldDatabase Greeting/Limit/GoldenBox)
- "select count(useType) from tbl_GreetMsg where useType = %d"
- "{CALL pInsert_GreetingMsgRecord_071119(%d,'%s','%s')}"
- "select GMsg,Name from tbl_GreetMsg where usetype = %d"
- "select [effect] from [dbo].[tbl_sf_delay] where [aserial] = 0"
- "Select top %d CharacterSerial,CharacterName,BattleWinGrade from tbl_battletournament"
- "{ CALL pSelect_GoldBoxItem }"
- "{ CALL pInsert_GoldBoxItem( %d ) }"

## Newly logged (CWeeklyGuildRankManager)
- "CWeeklyGuildRankManager::Load() : LoadPrevOwner() %s"
- "Success"
- "Fail"
- "CWeeklyGuildRankManager::InsertDefaultRecord() : g_Main.m_pWorldDB->Insert_DefaultRecordWeeklyGuildPvpPointSum() Fail!"
- "{ CALL pInsert_DefaultRecordWeeklyGuildPVPPointSum }"
- "tbl_PvpPointGuildRank%s"
- "PvpPointGuildRank"
- "Year"
- "Month"
- "Day"
- "..\\SystemSave\\ServerState.ini"

## GuildBattle additions (2026-02-10)
- "CNormalGuildBattle::Init(%u,%u,%u,%u)GetID() != dwID Fail!"
- "CNormalGuildBattle::Init(%u,%u,%u,%u)byNumber(%u) Invalid!"
- "CNormalGuildBattle::Init( %u, %u, %u, %u, %u, %u ) : %s : Init( %s, %s, %d, %u, pkStateList )"
- "CNormalGuildBattle::Init( %u, %u, %u, %u, %u, %u ) : %s : Init( ... ) Skip!"
- "CNormalGuildBattle::LeaveGuild( %u ) : (%u) m_pkField->DropBall( %s ) Fail(%u)!"
- "CNormalGuildBattle::LeaveGuild( %u ) : %s %s %s %s"
- "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : AddDefaultDBRecord()"
- "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : Load( true, %d, m_ppkTodayBattle ) Fail!"
- "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : Load( false, %d, m_ppkTomorrowBattle ) Fail!"
- "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iTomorrow == iCurDay ) : Load( true, %d, m_ppkTodayBattle ) Fail!"
- "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) AddDefaultDBRecord()"
- "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) CGuildBattleScheduleManager::Instance()->CleanUpDanglingReservedSchedule() Fail!"
- "CNormalGuildBattleManager::LoadDBGuildBattleInfo() : g_Main.m_pWorldDB->LoadGuildBattleInfo() Fail!"
- "CNormalGuildBattleManager::AddDefaultDBRecord() g_Main.m_pWorldDB->InsertGuildBattleDefaultRecord( %u ) Fail!"
- "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : AddDefaultDBTable()"
- "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : m_pkTodaySchedule->Load(true) Fail!"
- "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : m_pkTomorrowSchedule->Load(false) Fail!"
- "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iTomorrow == iCurDay ) : m_pkTodaySchedule->Load(true) Fail!"
- "CGuildBattleScheduleManager::AddDefaultDBTable() :  g_Main.m_pWorldDB->InsertGuildBattleScheduleDefaultRecord( 2 DAY, %u, %u, %u ) Fail!"
- "CGuildBattleReservedScheduleMapGroup::Load() new _worlddb_guild_battle_schedule_list[%u] Fail!"
- "CGuildBattleReservedScheduleMapGroup::Load() g_Main.m_pWorldDB->LoadGuildBattleScheduleInfo( %u, %u, ms_pkDBInfo )"
- "CGuildBattleReservedScheduleMapGroup::Load( %d ) m_ppkReservedSchedule[%u]->Load( ms_pkDBScheduleInfo )"
- "CGuildBattleReservedSchedule::Load()  m_uiScheduleID(%u) != pkInfo->list[%u].uiSLID(%u) Fail!"
- "CGuildBattleReservedSchedule::Load() CGuildBattleSchedulePool::Instance()->Get( %u ) Fail!"
- "CGuildBattleReservedSchedule::Load() : %s : pkSchedule->Load( %u, %u, %04d-%02d-%02d %02d:%02d:%02d, %u ) Load!"
- "CGuildBattleReservedSchedule::Load() : %s : pkSchedule->Load( %u, %u, %04d-%02d-%02d %02d:%02d:%02d, %u ) Skip!"
- "CGuildBattleReservedSchedule::Flip() : SLID(%u) m_pkSchedule[%u]->Clear() Elapased Time!"
- "CGuildBattleReservedSchedule::CleanUpDanglingReservedSchedule() : m_pkSchedule[%u]->Clear() ID(%u)!"
- "CGuildBattleReservedScheduleListManager::Load(%d,%u,%d,%d) : LoadTodaySchedule() Fail!"
- "CGuildBattleReservedScheduleListManager::Load(%d,%u,%d,%d) : LoadTomorrowSchedule() Fail!"
- "CGuildBattleReservedScheduleListManager::LoadTodaySchedule() : UpdateTodaySchedule( %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::LoadTodaySchedule() : UpdateTomorrowSchedule( %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(%u) : m_pkToday->Load( %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(%u) : g_Main.m_pWorldDB->SelectGuildBattleRerservedList( %u, %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(%u) : CGuildBattleScheduler::Instance()->GetTodaySLIDByMap( %u, %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::UpdateTomorrowScheduleLoad(%u) : m_pkTomorrow->Load( %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::UpdateTomorrowSchedule(%u) : g_Main.m_pWorldDB->SelectGuildBattleRerservedList( %u, %u ) Fail!"
- "CGuildBattleReservedScheduleListManager::UpdateTomorrowSchedule(%u) : CGuildBattleScheduler::Instance()->GetTomorrowSLIDRange( %u, %u ) Fail!"
- "CReservedGuildSchedulePage::InitClear(%u) : -1 == iToDay || -1 == iTomorrow Fail!"
- "select count(id) from [dbo].[tbl_ReservedGuildBattleInfo]"
- "select count(id) from [dbo].[tbl_GuildBattleScheduleInfo]"
- "{ CALL pSelect_ReservedGuildBattleInfo(%u,%u) }"
- "{ CALL pSelect_ReservedGuildBattleSchedule( %u, %u ) }"
- "truncate table tbl_ReservedGuildBattleInfo"
- "truncate table [dbo].[tbl_GuildBattleScheduleInfo]"
- "{ CALL pInsert_GuildBattleInfo( %u, 0, 0, 0, 0 ) }"
- "INSERT INTO [dbo].[tbl_GuildBattleScheduleInfo]( [ID], [SLID], [State], [StartTime], [BattleTurm] )VALUES( %d, %d, 0, 0, 0 )"

## Post/UnmannedTrader additions (2026-02-10)
- "update [dbo].[tbl_PostStorage] set [dbo].[tbl_PostStorage].dck=1 from (select p.serial from [dbo].[tbl_PostStorage] as p join [dbo].[tbl_base] as b on p.owner <> 0 and p.owner = b.serial and b.dck=1) as d where [dbo].[tbl_PostStorage].serial = d.serial"
- "select count(serial) from tbl_PostStorage where dck=1"
- "select * from tbl_PostRegistry where dck=0"
- "{ CALL pInsert_PostStorageRecord }"
- "{ CALL pSelect_BossWinRate(%d, N'%s', '%s') }"
- "select count([id]) from [dbo].[tbl_utresultstateid]"
- "select top %u [id], [desc] from [dbo].[tbl_utresultstateid]"
- "truncate table [dbo].[tbl_utresultstateid]"
- "insert into [dbo].[tbl_utresultstateid] ([id], [desc]) values ( %u, N'%s' )"
- "update [dbo].[tbl_utresultinfo] set [dbo].[tbl_utresultinfo].state = 0 from ( select s.type, s.serial, s.owner, r.state from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_utresultinfo] as r on s.owner <> 0 and r.type = s.type and s.serial = r.serial join [dbo].[tbl_base] as b on s.owner = b.serial and b.dck = 1 ) as d where [dbo].[tbl_utresultinfo].type = d.type and [dbo].[tbl_utresultinfo].serial = d.serial"
- "select count(serial) from tbl_utresultinfo where type = 0 and state = 0"
- "SELECT TOP 1 SuggesterName, Tax, NextTax FROM [dbo].[tbl_ATradeTaxRate] WHERE Race=%d ORDER BY serial DESC"
- "OldIncome"
- "CurrentIncome"
- "Init tax rate : Suggester:%s tax: %.2f"

## AutoMine/Personal additions (2026-02-10)
- "..\\ZoneServerLog\\systemlog\\Concession"
- "..\\ZoneServerLog\\systemlog\\Concession\\AutoMine_R%d_T%d_%d.log"
- "..\\ZoneServerLog\\systemlog\\Concession\\err_automineR%d_T%d_%d.log"
- "AutoMine Charge"
- "Failed AutoMineMachine[%d]::m_Inven.create()"
- "Failed AutoMineMachine[%d]::_set_mineore()"
- "[Load DB]GuildSerial:%d Run:%d, Race:%d, Ore:%d, Gage:%d, SlotCnt:%d"
- "[ERR-Load DB]::m_Inven.push(%d,%d,[%d/%d/%d],%d)"
- "[Load DB]Page:%d,Slot:%d [ORE Pos:%d,name_%s, num_%d]"
- "[ERR-Load DB]::Invalid value of race code.(%d-%d)"
- "Prev Owner:%d"
- "Prev Owner:NULL"
- "Changed Owner:%d"
- "Changed Owner:NULL"
- "Set Owner:%d"
- "Set Owner:NULL"
- "[START] Owner:%d Master:%d"
- "[ERR-Start]:Have not owner guild"
- "[STOP] Owner:%d Master:%d"
- "[ERR-Stop]:Have not owner guild"
- "[SELECT ORE] Owner:%d Master:%d [Ore:%d]"
- "[ERR-SelectOre]:max_kind_automine_ore(%d)"
- "[ERR-Charge]:automine_guild_db_io_wait"
- "[ERR-Charge]:utomine_nonmatch_charge_value"
- "[ERR-Charge]:automine_not_enough_gold(T:%u, Cost:%d)"
- "[ERR-Charge]:Have not owner guild"
- "Owner:%d Charge:%d GuildGold:%.0f - Cost:%d"
- "Cost : serial:%d, out:%d, total:%.0f)"
- "ERR::m_Inven.push(%d,%d,[%d/%d/%d],1)_%d"
- "STOP : %d"
- "ERR::Invalid value of table code.(%d)"
- "ERR::Is not exist ore(%s)"
- "[ERR-GetOutOreInAutoMine]:automine_invalid_values(%d,%d,%d,%d)"
- "[ERR-GetOutOreInAutoMine]::m_Inven.push(%d, %d, [%d/%d/%d], %d)_LINE:%d"
- "[ERR-GetOutOreInAutoMine]:automine_isnot_owner_guild"
- "ERR - get_slot(%d, %d) is NULL"
- "[POP ORE] GuildSerial%d MasterSerial:%d [Page:%d_Slot:%d_Ore:%s], Num:%d"
- "[ERR-MoveOreInAutoMine]:automine_invalid_values(%d,%d,%d,%d)"
- "[ERR-MoveOreInAutoMine]:automine_isnot_owner_guild"
- "[ERR-OreMerge]:automine_invalid_values(%d,%d,%d,%d)"
- "tbl_aminepersonal_inven"
- "AutominePersonalMgr::CreateDBTable() >> g_Main.m_pWorldDB is NULL."
- "AutominePersonalMgr::init_Objects() >> Failed allocate memory of AutoMinePersonal[MAX_PLAYER]."
- "AutominePersonalMgr::init_Objects() >> m_Machine[%d].initialize(%d) failed"
- "AutominePersonalMgr::initialize() >> init_objects() failed."
- "AutominePersonalMgr::install() [LINE:%d]"
- "AutominePersonalMgr::uninstall() [LINE:%d]"
- "AutominePersonalMgr::check_machine() >> not exist machine"
- "AutominePersonalMgr::pop_ore() >> Failed CPlayer::CUserDB::Update_ItemSlot()"
- "AutominePersonalMgr::pop_ore() >> Failed CPlayer::Emb_AddStorage()"
- "AutominePersonalMgr::pop_ore() >> Is not exist item(%d)"
- "AutominePersonalMgr::pop_ore()"
- "AutominePersonalMgr::pop_ore() - Delete Fail"
- "[dbo].[tbl_automine_inven]"
- "{ CALL pcreate_automine }"
- "{ CALL pselect_automine_inven(%d, %d) }"
- "s_szOreKind_160 = {\"ioblu03\", \"iored03\", \"ioyel03\", \"iogre03\", \"iobla03\"}"
- "s_szAMPOreKind = {\"ioblu03\", \"iored03\", \"ioyel03\", \"iogre03\", \"iobla03\"}"

## Newly logged (Build fix batch 2026-02-10)
- "Calc Region time : %d" (CPlayer::CheckPos_Region)
- "%s: error stored effect, code: %d, idx: %d: lv: %d" (CPlayer::_set_db_sf_effect)
- "Player Create: %s [%s %s]\r\n" (CMgrAccountLobbyHistory::player_create)
- "WARNNING : First Connect Char Money Wrong : $D(%d), $G(%d)" (CMgrAccountLobbyHistory::player_create)
- "Create Complete Player Money: $D(%d), $G(%d)\r\n" (CMgrAccountLobbyHistory::player_create_complete_money)
- "Player Money FIX: $D(%d -> %d), $G(%d -> %d) [%s %s]\r\n" (CMgrAccountLobbyHistory::player_money_fix)
- "Failed _insert_to_inven() >> %s" (CGoldenBoxItemMgr::_insert_to_inven)
- "ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems()" (ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems)

## Newly logged (CMgrGuildHistory additions 2026-02-11)
- byte_140793818 (CMgrGuildHistory::start_guild header format; current guess: "START GUILD : Name(%s) Serial(%u) Grade(%d) TotalD:%.0f TotalG:%.0f [%s]\r\n")
- byte_140793850 (CMgrGuildHistory::start_guild member count format; current guess: "MEMBER COUNT : %u\r\n")
- asc_140793878 (CMgrGuildHistory::join_member format; current guess: "JOIN MEMBER : Name(%s) Serial(%u) Approver(%s,%u) total(%d) [%s]\r\n")
- aSelfLeaveFmt (CMgrGuildHistory::leave_member self format; original bytes unresolved, current guess: "SELF LEAVE ( %s , %u ) total(%d) [%s]\r\n")
- aCo (CMgrGuildHistory::leave_member punish format; current guess: "FORCED LEAVE ( %s , %u ) total(%d) [%s]\r\n")
- asc_140793908 (CMgrGuildHistory::leave_member format; current guess: "LEAVE MEMBER ( %s , %u ) total(%d) [%s]\r\n")

## Newly logged (UnmannedTrader Load 2026-02-11)
- "CUnmannedTraderRegistItemInfo::Set(...)\r\n\t\tdwRegistSerial(%u)\r\n\t\tm_kState.Set( kInfo.m_List[uiInx(%u)].byState(%u) ) Faili!\r\n"
- "CUnmannedTraderRegistItemInfo::Set(...)\r\n\t\tdwRegistSerial(%u) MaxSlot(%u) byInvenIndex(%u)\r\n\t\t( max_used_trade_num <= uiInx(%u) || 0 == pkItem(%p) ) == false!\r\n"
- "CUnmannedTraderUserInfo::SetLoadInfo( byType(%u), dwSerial(%u), _TRADE_DB_BASE & kInfo, CLogFile * pkLogger )\r\n\t\tCUnmannedTraderItemState::PushUpdateState( byType(%u)\r\n\t\t, kInfo.m_List[i].dwRegistSerial(%u)\r\n\t\t, CUnmannedTraderItemState::CANCELREGISTFORSERVERINTERNALERROR, dwSerial )\r\n\t\tName(%s) Serial(%u) Invalid DB Data!\r\n"

## Newly logged (Build fix batch 2026-02-11)
- "Guild Member Fail: guild:%s, name:%s" (CPlayerDB::ConvertAvatorDB)
- "%s: _STAT_DB_BASE::IsRangePerMastery(%d, %d) == false : CodePos: %s" (CPlayer::Emb_AlterStat)
- "%s: _STAT_DB_BASE::IsRangePerMastery(%d, %d) == false" (CPlayer::Emb_AlterStat)
- "_check_mastery_cum_lim.. pCurFld : NULL" (CPlayer::_check_mastery_cum_lim)
- "_check_mastery_cum_lim.. level : %d, max level : %d" (CPlayer::_check_mastery_cum_lim)
- "_check_mastery_cum_lim.. racecode : %d" (CPlayer::_check_mastery_cum_lim)
- "_check_mastery_cum_lim.. cur_class : %d, base_class : %d" (CPlayer::_check_mastery_cum_lim)
- "17" (CPlayer::_set_db_sf_effect record lookup)

## Newly logged (Post system batch 2026-02-11)
- "NoItem" (CMgrAvatorItemHistory::post_delete / post_getpresent / post_senditem / post_return)
- "[PostSystem : Post Delete] - PostSerial[%u] - No[%d] - Item[%s] - Gold[%u] - Sender[%s] - [%s %s]\r\n" (CMgrAvatorItemHistory::post_delete)
- "[PostSystem : Get Item & Gold In Inven] - PostSerial[%u] - Item[%s] - Gold[%u] - Sender[%s] - [%s %s]\r\n" (CMgrAvatorItemHistory::post_getpresent)
- "[PostSystem : Send Item & Gold] - Item[%s] - Gold[%u] - Receiver[%s] - [%s %s]\r\n" (CMgrAvatorItemHistory::post_senditem)
- "[PostSystem : Get Return Item & Gold In Inven] - PostSerial[%u] - Item[%s] - Gold[%u] - Receiver[%s] - [%s %s]\r\n" (CMgrAvatorItemHistory::post_return)
- "GetItem Wrong DB Data >> Name:%s >> Inx:%d >> Storage(S:%d) : DB(S:%d)" (CPlayer::UpdatePostAddLog)
- "GetItem KeyError >> Name:%s >> INX(%d) >> ORIGIN(%d) : NEW(%d) PS(%u) : OLD(%d) PS(%u)" (CPlayer::UpdatePostAddLog)
- "And Post Serial Is Equal!" (CPlayer::UpdatePostAddLog)
- "And Post DB Base Index Wrong >> Name:%s >> NEW : INX(%d) PS(%u) >> OLD : INX(%d) PS(%u)" (CPlayer::UpdatePostAddLog)
- "And Matched Serial Is None!" (CPlayer::UpdatePostAddLog)
- "CPlayer::UpdatePostAddLog() : pPost Is Null : Index(%u)" (CPlayer::UpdatePostAddLog)
- Packet type bytes {13, 119} (TRC_AutoTrade::SendMsg_PatriarchTaxRate)
- "[Suggest Change Tax Rate]:[SUBPATRIARCH:%s] - %d(%%)" (TRC_AutoTrade::set_suggested)
- "*" default guild name marker (TRC_AutoTrade::PushDQSData)
- Packet type bytes {13, 121} (CPlayer::SendMsg_ResultChangeTaxRate)
- Packet type bytes {13, 105} and fixed payload size 650 (CPlayer::SendMsg_RaceBossCryMsg)
- "resources" map-code gate string (CPlayer::pc_SetGroupMapPointRequest)
- Packet type bytes {17, 11} with payload {effectBit,objSerial} (CPlayer::SendMsg_Circle_DelEffect)

## Newly logged (Trunk/Siege batch 2026-02-11)
- "CPlayer::pc_TrunkEstRequest() : %u(%s) !::IsSQLValidString( pwszPassword(%s) ) || !::IsSQLValidString(pwszHintAnswer(%s) ) Invalid!"
- "CPlayer::pc_TrunkEstRequest() : %u(%s) !::IsSQLValidString( pwszPassword(%s) ) || !::IsSQLValidString(pwszHintAnswer(%s)) Invalid!"
- "CPlayer::pc_TrunkIoMoveRequest()"
- "CPlayer::pc_TrunkIoSwapRequest() -- 0"
- "CPlayer::pc_TrunkIoSwapRequest() -- 1"
- "CPlayer::pc_TrunkIoSwapRequest() -- 1 Fail"
- "CPlayer::pc_TrunkIoSwapRequest() -- 2 Fail"
- Packet type bytes {33, 6} (CPlayer::SendMsg_TransShipRenewTicketResult)
- Packet type bytes {34, 2} (CPlayer::SendMsg_TrunkEstResult)
- Packet type bytes {34, 4} (CPlayer::SendMsg_TrunkDownloadResult)
- Packet type bytes {34, 6} (CPlayer::SendMsg_TrunkChangPasswdResult)
- Packet type bytes {34, 8} (CPlayer::SendMsg_TrunkExtendResult)
- Packet type bytes {34, 11} (CPlayer::SendMsg_TrunkResDivision)
- Packet type bytes {34, 13} (CPlayer::SendMsg_TrunkPotionDivision)
- Packet type bytes {34, 17} (CPlayer::SendMsg_TrunkIoResult)
- Packet type bytes {34, 19} (CPlayer::SendMsg_TrunkIoMoneyResult)
- Packet type bytes {34, 21} (CPlayer::SendMsg_TrunkPwHintIndexResult)
- Packet type bytes {34, 23} (CPlayer::SendMsg_TrunkHintAnswerResult)

## Newly logged (Build fix batch 2026-02-11-2)
- "CGuildBattleReservedScheduleMapGroup::Clear(%u,%u) m_ppkReservedSchedule[%u]->Clear(%u) Fail!" (GUILD_BATTLE::CGuildBattleReservedScheduleMapGroup::Clear)
- "{ CALL pSelect_GuildBattleRank(%u) }" (CRFWorldDatabase::SelectGuildBattleRankRecord)
- "{ CALL pInsert_GuildBattleRank( %u ) }" (CRFWorldDatabase::InsertGuildBattleRankRecord)
- "%s\\%d" (CMgrAvatorLvHistory::GetNewFileName)
- "%s\\%d\\%d" (CMgrAvatorLvHistory::GetNewFileName)
- "%s\\%d\\%d\\%d_%s.lhi" (CMgrAvatorLvHistory::GetNewFileName)
- "%s lv:%d xp:%.0f(%d) grade:%d max:%d/%d/%d [%s %s]\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tW%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tD: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tP: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tS%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tF%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tM%d: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- "\tR: %d (%d)\r\n" (CMgrAvatorLvHistory::start_mastery)
- Packet type bytes {51, 16} (GUILD_BATTLE::CNormalGuildBattle::SendWebAddScheduleInfo)

## Newly logged (Build fix batch 2026-02-11-3)
- Packet type bytes {30, 39} with payload {1} (TRC_AutoTrade::SendMsg_UserLogInNotifyTaxRate)
- "CUnmannedTraderController::SellWaitItem(...) Exceed Tax Price!\r\n\t\t dwRegistSerial(%u) dwSeller(%u) dwBuyer(%u)\r\n\t\t dwPrice(%u) dwTax(%u)\r\n" (CUnmannedTraderRegistItemInfo::SellWaitItem)
- "CUnmannedTraderRegistItemInfo::SellWaitItem(...)\r\n\t\t(0 == m_dwBuyerSerial) m_dwRegistSerial(%u) Serial(%u) State(%d)\r\n" (CUnmannedTraderRegistItemInfo::SellWaitItem)
- "CUnmannedTraderRegistItemInfo::SellWaitItem( WORD wInx(%u), CLogFile * pkLogger )\r\n\t\tName(%s) Serial(%u) Invalid DB Data!\r\n\t\tm_dwRegistSerial(%u)\r\n\t\tpkPlayer->m_Param.m_dbInven.GetPtrFromSerial( m_wItemSerial(%u) ) == NULL!\r\n" (CUnmannedTraderRegistItemInfo::SellWaitItem)
- "CUnmannedTraderRegistItemInfo::SellWaitItem( WORD wInx(%u), DWORD & dwRealSellDalant, CLogFile * pkLogger )\r\n\t\tm_dwRegistSerial(%u) dwRealSellDalant(%u) = m_dwPrice(%u) - m_dwTax(%u)\r\n\t\tName(%s) Serial(%u) Cur(%u) Sell(%u) Exceed Max Money!\r\n" (CUnmannedTraderRegistItemInfo::SellWaitItem)
- "%s %s >> sellwait (%u) user(%u)%s -> user(%u)%s price(%u) tax(%u)" (CUnmannedTraderRegistItemInfo::SellWaitItem)
- "CUnmannedTraderRegistItemInfo::SellWaitItem()" (CUnmannedTraderRegistItemInfo::SellWaitItem Emb_DelStorage reason)
- "CUnmannedTraderUserInfo::SetCompleteInfo( ... )\r\n\t\tm_byRegistCnt(%u) > m_byMaxRegistCnt(%u)\r\n\t\tCUnmannedTraderItemState::PushUpdateState( CUnmannedTraderItemState::SINGLE\r\n\t\t, m_vecLoadItemInfo[i].GetRegistSerial()(%u)\r\n\t\t, CUnmannedTraderItemState::CANCELREGISTFORSERVERINTERNALERROR, dwSerial )\r\n\t\tName(%s) Serial(%u) !\r\n" (CUnmannedTraderUserInfo::CompleteCreate)
- Packet type bytes {30, 26} (CUnmannedTraderUserInfo::CompleteCreate sell-wait notify)
- Packet type bytes {30, 38} (CUnmannedTraderUserInfo::CompleteCreate continue notify)
- DQS op code 0x6F with payload size 332 (CUnmannedTraderUserInfo::CompleteCreate)

## Newly logged (Build fix batch 2026-02-11-4)
- Packet type bytes {13, 7} (CPlayer::SendMsg_ResSeparation)
- Packet type bytes {13, 13} (CPlayer::SendMsg_PotionSeparation)
- Packet type bytes {13, 9} (CPlayer::SendMsg_ResDivision)
- Packet type bytes {13, 15} (CPlayer::SendMsg_PotionDivision)
- Packet type bytes {13, 3} (CPlayer::SendMsg_ForceInvenChange)
- Packet type bytes {13, 5} (CPlayer::SendMsg_AnimusInvenChange)
- "%s : Update_ItemSlot(CODE) : scode : %d, icode : %d  " (CUserDB::Update_ItemSlot)
- "%s:Update_ItemSlot(EMBELL, Idx:%d)" (CUserDB::Update_ItemSlot)
- "%s:Update_ItemSlot(TRUNK, Idx:%d)" (CUserDB::Update_ItemSlot)
- "%s:Update_ItemSlot(EXT_TRUNK, Idx:%d)" (CUserDB::Update_ItemSlot)
- "%s:Update_ItemSlot(INVEN, Idx:%d)" (CUserDB::Update_ItemSlot)

## Newly logged (Guild/Animus handler batch 2026-02-11-5)
- "[%s] Join accept: approver=%s applicant=%s" (CPlayer::pc_GuildJoinAcceptRequest, current guess for IDA log format)
- Packet type bytes {27, 7} (guild join apply result)
- Packet type bytes {27, 11} (guild join apply cancel result)
- Packet type bytes {27, 12} (guild join apply reject inform)
- Packet type bytes {27, 14} (guild join accept fail result)
- Packet type bytes {27, 26} (guild vote result)
- Packet type bytes {27, 36} (guild push money result)
- Packet type bytes {27, 114} (guild set honor result)
- Packet type bytes {22, 8} (animus target result)

## Newly logged (Target/GuildRoom batch 2026-02-11-6)
- Packet type bytes {13, 27} (target object set result)
- Packet type bytes {13, 111} (group target inform)
- Packet type bytes {27, 103} (guild room rent result)
- Packet type bytes {27, 105} (guild room enter result)
- "GuildRoom Rent" (guild room rent money I/O operator name)

## Newly logged (Automine batch 2026-02-11-7)
- "itttt04" (AP_BatterySlot::is_private_item battery code allowlist)
- "itttt05" (AP_BatterySlot::is_private_item battery code allowlist)
- "itcsa01" (AP_BatterySlot::is_private_item battery code allowlist)
- "itcsa02" (AP_BatterySlot::is_private_item battery code allowlist)
- Packet type bytes {14, 45} (automine install inform)
- Packet type bytes {14, 49} (automine ore select result)
- Packet type bytes {14, 51} (automine battery insert result)
- Packet type bytes {14, 53} (automine battery extract result)
- Packet type bytes {14, 57} (automine pop ore result)
- Packet type bytes {14, 58} (automine stop inform)
- Packet type bytes {14, 62} (automine error code)
- Packet type bytes {14, 64} (automine info UI open)

## Newly logged (Build fix batch 2026-02-12-1)
- Packet type bytes {23, 2} (CPlayer::SendUnitFrameBuyResultPacket)
- Packet type bytes {23, 4} (CPlayer::SendUnitSellResultPacket)
- Packet type bytes {23, 6} (CPlayer::SendUnitPartTuningResultPacket)
- Packet type bytes {23, 8} (CPlayer::SendUnitFrameRepairResultPacket)
- Packet type bytes {23, 10} (CPlayer::SendUnitBulletFillResultPacket)
- Packet type bytes {23, 12} (CPlayer::SendUnitPackFillResultPacket)
- Packet type bytes {23, 14} (CPlayer::SendUnitDeliveryResultPacket)
- Packet type bytes {23, 18} (CPlayer::SendUnitTakeResultPacket)
- Packet type bytes {23, 20} (CPlayer::SendUnitLeaveResultPacket)
- Packet type bytes {24, 18} (CPlayer::SendResultNpcQuestPacket)
- Packet type bytes {24, 21} (CPlayer::SendNpcQuestListResultPacket)
- Packet type bytes {27, 20} (CPlayer::SendOfferSuggestResultPacket)
- Packet type bytes {17, 32} (CRecallEffectController::SendRecallReqeustResult)
- Packet type bytes {17, 35} (CRecallEffectController::DecideRecall error result)
- "-1" (CPlayer::pc_UnitFrameBuyRequest default unit-part sentinel)
- Bullet-slot map {-1, -1, -1, 0, 1, -1} (CPlayer::pc_UnitPartTuningRequest)
- Weapon-part map {3, 4} (CPlayer::pc_UnitBulletFillRequest)
- "RewardChangeClass - Emb_AddStorage() Fail" (ItemCombineMgr::MakeNewItems)
- "-", "-1" (ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems output-code sentinels)
- "ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems()" (ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems Emb_DelStorage reason)

## Newly logged (Case 24 batch 2026-02-12-2)
- Packet type bytes {25, 25} (CPlayer::SendMsg_MoveToOwnStoneMapResult)
- Packet type bytes {25, 26} (CPlayer::SendMsg_MoveToOwnStoneMapInform)

## Newly logged (Send helper member-refactor batch 2026-02-12-3)
- Packet type bytes {1, 15} (CPlayer::SendMsg_UILock_Init_Request_ToAccount)
- Packet type bytes {1, 17} (CPlayer::SendMsg_UILock_Update_Request_ToAccount)
- Packet type bytes {13, 27} (CPlayer::SendMsg_SetTargetObjectResult)
- Packet type bytes {13, 111} (CPlayer::SendMsg_GroupTargetInform)
- Packet type bytes {13, 128} (CPlayer::SendMsg_UILock_Init_Result)
- Packet type bytes {13, 130} (CPlayer::SendMsg_UILock_Login_Result)
- Packet type bytes {13, 132} (CPlayer::SendMsg_UILock_Update_Result)
- Packet type bytes {13, 135} (CPlayer::SendMsg_UILock_FindPW_Result)
- Packet type bytes {14, 2} (CPlayer::SendMsg_MineStartResult)
- Packet type bytes {14, 10} (CPlayer::SendMsg_OreCuttingResult)
- Packet type bytes {14, 12} (CPlayer::SendMsg_OreIntoBagResult)
- Packet type bytes {17, 2} (CPlayer::SendMsg_ForceResult self)
- Packet type bytes {17, 3} (CPlayer::SendMsg_ForceResult circle)
- Packet type bytes {17, 5} (CPlayer::SendMsg_SkillResult self)
- Packet type bytes {17, 6} (CPlayer::SendMsg_SkillResult circle)
- Packet type bytes {17, 8} (CPlayer::SendMsg_ClassSkillResult self)
- Packet type bytes {17, 9} (CPlayer::SendMsg_ClassSkillResult circle)
- Packet type bytes {17, 19} (CPlayer::SendMsg_CreateTowerResult)
- Packet type bytes {17, 21} (CPlayer::SendMsg_BackTowerResult)
- Packet type bytes {17, 28} (CPlayer::SendMsg_CreateTrapResult)
- Packet type bytes {17, 39} (CPlayer::SendMsg_BackTrapResult)
- Packet type bytes {17, 100} (CPlayer::SendMsg_ThrowSkillResult self)
- Packet type bytes {17, 101} (CPlayer::SendMsg_ThrowSkillResult circle)
- Packet type bytes {17, 103} (CPlayer::SendMsg_ThrowUnitResult self)
- Packet type bytes {17, 104} (CPlayer::SendMsg_ThrowUnitResult circle)
- Packet type bytes {18, 23} (CPlayer::SendMsg_DTradeOKResult)
- Packet type bytes {18, 24} (CPlayer::SendMsg_DTradeOKInform)
- Packet type bytes {18, 25} (CPlayer::SendMsg_DTradeAccomplishInform)
- Packet type bytes {22, 2} (CPlayer::SendMsg_AnimusRecallResult)
- Packet type bytes {22, 8} (CPlayer::SendMsg_AnimusTargetResult)
- Packet type bytes {23, 2} (CPlayer::SendMsg_UnitFrameBuyResult)
- Packet type bytes {23, 4} (CPlayer::SendMsg_UnitSellResult)
- Packet type bytes {23, 6} (CPlayer::SendMsg_UnitPartTuningResult)
- Packet type bytes {23, 8} (CPlayer::SendMsg_UnitFrameRepairResult)
- Packet type bytes {23, 10} (CPlayer::SendMsg_UnitBulletFillResult)
- Packet type bytes {23, 12} (CPlayer::SendMsg_UnitPackFillResult)
- Packet type bytes {23, 14} (CPlayer::SendMsg_UnitDeliveryResult)
- Packet type bytes {23, 18} (CPlayer::SendMsg_UnitTakeResult)
- Packet type bytes {23, 20} (CPlayer::SendMsg_UnitLeaveResult)
- Packet type bytes {24, 18} (CPlayer::SendMsg_ResultNpcQuest)
- Packet type bytes {24, 21} (CPlayer::SendMsg_NpcQuestListResult)
- Packet type bytes {27, 7} (CPlayer::SendMsg_GuildJoinApplyResult)
- Packet type bytes {27, 11} (CPlayer::SendMsg_GuildJoinApplyCancelResult)
- Packet type bytes {27, 12} (CPlayer::SendMsg_GuildJoinApplyRejectInform)
- Packet type bytes {27, 14} (CPlayer::SendMsg_GuildJoinAcceptFail)
- Packet type bytes {27, 20} (CPlayer::SendMsg_OfferSuggestResult)
- Packet type bytes {27, 22} (CPlayer::SendMsg_CancelSuggestResult)
- Packet type bytes {27, 26} (CPlayer::SendMsg_VoteResult)
- Packet type bytes {27, 36} (CPlayer::SendMsg_GuildPushMoneyResult)
- Packet type bytes {27, 103} (CPlayer::SendMsg_GuildRoomRentResult)
- Packet type bytes {27, 105} (CPlayer::SendMsg_GuildRoomEnterResult)
- Packet type bytes {27, 114} (CPlayer::SendMsg_GuildSetHonorResult)

## Newly logged (CMainThread run parity batch 2026-02-12-4)
- "Current : %.10f Limit : %.10f Rate : %d Init Limit!" (CPvpPointLimiter::Clear debug chat string)

## Newly logged (CMainThread run parity batch 2026-02-12-5)
- "Post Use Count > Total Use: %d, Return Post: %d\\r\\n" (CPostSystemManager::Loop periodic stats log)
- "pass_min:%u" (CWorldSchedule::Init/CheckSch schedule log format)
- "cursor:%u hour:%u min:%u msec:%u time:%s" (CWorldSchedule::Init/CheckSch schedule log format)
- "code:%s event:%u info1:%u info2:%u" (CWorldSchedule::Init/CheckSch schedule log format)
- "PatriarchElectProcessor::TimeCheck() Process Initialize Error (_eProcessType:%d)" (PatriarchElectProcessor::TimeCheck system log)

## Newly logged (todo.md closure batch 2026-02-13-1)
- Packet type bytes {35, 7} with payload `{wPortalIndex}` (CDarkHoleChannel::SendMsg_OpenPortalByReact)
- Packet type bytes {35, 8} with payload `{byCnt, byPotalIndex[128]}` (CDarkHoleChannel::SendMsg_OpenPortalByResult)
- Packet type bytes {35, 9} with payload `{byJobIndex}` (CDarkHoleChannel::SendMsg_JobPass)
- Packet type bytes {35, 10} with payload `{szCompleteMsgCode[17]}` (CDarkHoleChannel::SendMsg_MissionPass)
- Packet type bytes {35, 11} with payload `{char sDum}` (CDarkHoleChannel::SendMsg_QuestPass)
- Packet type bytes {35, 12} with payload `{char sDum}` (CDarkHoleChannel::SendMsg_ChannelClose)
- Packet type bytes {35, 13} with payload `{desc + job array}` (CDarkHoleChannel::SendMsg_NewMission)
- Packet type bytes {35, 16} with payload `{char sDum}` (CDarkHoleChannel::SendMsg_TimeOut)
- Packet type bytes {52, 4} with payload `{remainCount}` (CRaceBossMsgController::SendInfomSender)
- Packet type bytes {51, 14} with payload `{dwWebSendDBID,dwMsgID,worldCode,race}` (CRaceBossMsgController::SendRequestWeb)
- Packet type bytes {51, 10} with payload `{dwMsgID,worldCode,race}` (CRaceBossMsgController::SendRequestWeb non-web sender)
- Packet type bytes {23, 25} with payload `{unitSlotIndex, wBooster}` (CPlayer::SendMsg_AlterBooster)

## Newly logged (todo.md closure batch 2026-02-13-2)
- DQS query case `0x98` payload `_qry_case_update_server_reset_token {dwServerToken,dwESerial,wProcType}` (CMainThread::PushResetServerToken)
- Date token format string `"%04d%02d%0d"` (CMainThread::CreateDataResetToken)

## Newly logged (holy-stone transition batch 2026-02-13-3)
- Static transition table `sTransTBL[6]`: `{1,1,2}`, `{2,2,3}`, `{3,3,4}`, `{4,5,6}`, `{5,5,6}`, `{6,6,0}` (`CHolyStoneSystem::HSKRespawnSystem` / `_GetTransTBL`)
- Packet type bytes `{51, 2}` with payload `{worldCode(u32), hour, min, numOfTime}` (`CHolyStoneSystem::SendSMS_MineTimeExtend`)

## Newly logged (virtual-closure batch 2026-02-13-4)
- Candidate register system-log path format `"..\\ZoneServerLog\\SystemLog\\Patriarch\\CandidateRegister_%d.log"` (`CandidateRegister::Initialize`)
- Candidate list packet type bytes `{56, 2}` (`CandidateRegister::_SendList` / `_Regist`)
- Candidate registration fee constant `0x989680` (`CandidateRegister::_Regist`)
- DQS query case `0x75` payload `_qry_case_insert_candidate {byRace,wIndex,dwElectSerial,dwAvatorSerial}` (`CandidateMgr::Regist`)
- Item history log format `"[RACE BOSS]candidate >> Avator Serial:%d\\t$D:%d \\t^Time:%d\\n"` (`CMgrAvatorItemHistory::raceboss_candidate`)

## Newly logged (virtual-closure batch 2026-02-13-5)
- Packet type bytes `{14, 54}` with payload `{dwObjSerial, bySlot}` (`AutominePersonal::Loop` battery depletion notify)
- Packet type bytes `{14, 59}` with payload `{dwObjSerial,wObjIndex,fFixPos[3],byAct,wItemTblIndex,wItemSerial,dwOwnerSeiral}` (`AutominePersonal::SendMsg_FixPosition`)
- Packet type bytes `{14, 60}` with payload `{dwObjSerial,wHPRate}` (`AutominePersonal::send_attacked`)
- Packet type bytes `{14, 66}` with payload `{wItemSerial}` (`AutominePersonal::send_attacked`)
- Static string `"BATTERY_DISCHARGE"` (`AutominePersonal::sub_battery` item history reason)
- Static string `"!CRITICAL! AutominePersonal::do_automine() >> Emb_AddStorage() is faield"` (`AutominePersonal::do_automine`)
- Static string `"!CRITICAL! AutominePersonal::do_automine() >> New Ore is not exist(%d)"` (`AutominePersonal::do_automine`)

## Newly logged (virtual-closure batch 2026-02-13-6)
- Static string `"[PERSONAL_AMINE_STOP] - %s\r\n"` (`CMgrAvatorItemHistory::personal_amine_stop`)
- Static string `"%s >> num:%d\r\n"` (`CMgrAvatorItemHistory::personal_amine_stop`)

## Newly logged (virtual-closure batch 2026-02-13-7)
- Packet type bytes `{4, 13}` with payload `_animus_fixpos_packet` (`CAnimus::SendMsg_FixPosition`)
- Packet type bytes `{4, 24}` with payload `_animus_realmove_packet` (`CAnimus::SendMsg_RealMovePoint`)
- Static string `"[ANIMUS][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)"` (`CAnimus::GetObjName`)

## Newly logged (virtual-closure batch 2026-02-13-8)
- Packet type bytes `{17, 16}` with payload `{byID,wIndex,dwObjSerial}` (`CCharacter::SendMsg_StunInform`)
- Packet type bytes `{4, 175}` with payload `{portalIndex,color}` (`CCircleZone::SendMsg_FixPosition`)
- Packet type bytes `{4, 171}` with payload `{wIndex,89,dwObjSerial,pos}` (`CGravityStone::SendMsg_FixPosition`)
- Packet type bytes `{4, 172}` with payload `{portalIndex,isRegen}` (`CGravityStoneRegener::SendMsg_FixPosition`)
- Item box loop state thresholds `20000`, `50000`, `60000` ms (`CItemBox::Loop`)
- Packet type bytes `{4, 16}` with payload `{wIndex,dwObjSerial,frame,parts[6],pos,ownerSerial}` (`CParkingUnit::SendMsg_FixPosition`)
- Packet type bytes `{4, 170}` with payload `{wIndex,dwObjSerial,dwOwnerSerial,ownerName,pos}` (`CReturnGate::SendMsg_FixPosition`)

## Newly logged (virtual-closure batch 2026-02-13-9)
- Packet type bytes `{35, 101}` with payload `_darkhole_create_zocl` (`CDarkHole::SendMsg_Create`)
- Packet type bytes `{35, 102}` with payload `_darkhole_destroy_zocl` (`CDarkHole::SendMsg_Destroy`)
- Packet type bytes `{35, 103}` with payload `_darkhole_state_change_zocl` (`CDarkHole::SendMsg_StateChange`)
- Packet type bytes `{35, 104}` with payload `_darkhole_fixpositon_zocl` (`CDarkHole::SendMsg_FixPosition`)

## Newly logged (virtual-closure batch 2026-02-13-10)
- Static string `"CNationSettingDataRU::Init()"` (`CNationSettingDataRU::Init`)
- Static string `"FireGuard Setting Enabled Invalid!"` (`CNationSettingDataRU::Init`)
- Static string `"CNationSettingDataRU::Init() : FireGuard Setting Enabled Invalid!"` (`CNationSettingDataRU::Init`)
- Static string `"Time Limit System Setting Eanbled Invalid!"` (`CNationSettingDataRU::Init`)
- Static string `"CNationSettingDataRU::Init() : Time Limit System Setting Eanbled Invalid!"` (`CNationSettingDataRU::Init`)
- Static string `"All Event Error!"` (`CNationSettingDataRU::Init`)
- Static string `"CNationSettingDataRU::Init() : All Event Error!"` (`CNationSettingDataRU::Init`)
- INI path `".\\Initialize\\NewHolySystem.ini"` and key tuple `("HolySystem","KeeperCallCode","X")` (`CNationSettingDataRU::ReadSystemPass`)

## Newly logged (virtual-closure batch 2026-02-13-11)
- Static string `"..\\ZoneServerLog\\SystemLog\\Patriarch\\ClassOrderProcessor_%d.log"` (`ClassOrderProcessor::Initialize`)
- Static string `"Patriarch Appoint (ES:%d, PS:%d) >> Class(%d) Avator(%d)"` (`ClassOrderProcessor::_ResponseAppoint`)
- Static string `"Patriarch Discharge (ES:%d, PS:%d) >> Class(%d) Avator(%s)"` (`ClassOrderProcessor::_RequestDischarge`)
- Packet type bytes `{56, 11}` (`ClassOrderProcessor::_RequestAppoint`)
- Packet type bytes `{56, 17}` (`ClassOrderProcessor::SendMsg_PatriarchInform` / `_RequestDischarge`)

## Newly logged (virtual-closure batch 2026-02-13-12)
- Static string `"CLOSE>> ExpulsionSocket[Buffer: Send Buffer Full] ID: %s, IP: %s"` (`CNetworkEX::ExpulsionSocket`)
- Static string `"CLOSE>> ExpulsionSocket[Certify None: %d,%d, size:%d] IP: %s"` (`CNetworkEX::ExpulsionSocket`)
- Static string `"CLOSE>> ExpulsionSocket[Certify Delay] IP: %s"` (`CNetworkEX::ExpulsionSocket`)
- Static string `"FireGuard Detect !pUser->m_bActive !!"` (`CNetworkEX::ExpulsionSocket`)
- Static string `"IP(%s) : Account(%s) : AccountSerial(%u) : FGStatus(%#x)"` (`CNetworkEX::ExpulsionSocket`)
- Static string `"Fire Guard Punishment type Invaild : Type(%#x)"` (`CNetworkEX::ExpulsionSocket`)
- Static string `"CLOSE>> ExpulsionSocket[SpeedHack: No Respon] ID: %s, IP: %s"` (`CNetworkEX::ExpulsionSocket`)

## Newly logged (virtual-closure batch 2026-02-13-13)
- Static string `"[TOWER][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)"` (`CGuardTower::GetObjName`)
- Static string `"[SYSTEM TOWER][%d] >> %s (pos: %s {%d, %d, %d})"` (`CGuardTower::GetObjName`)
- Packet type bytes `{4, 14}` with payload `{wRecordIndex,wIndex,dwObjSerial,pos[3],buildSec,dwMasterSerial}` (`CGuardTower::SendMsg_FixPosition`)

## Newly logged (virtual-closure batch 2026-02-13-14)
- Packet type bytes `{5, 15}` with payload `{dwTowerSerial,byPart,byCritical,byTargetId,dwTargetSerial,nDamage}` (`CGuardTower::SendMsg_Attack`)
- Packet type bytes `{17, 23}` with payload `{dwTowerSerial,dwMasterSerial}` (`CGuardTower::SendMsg_TowerCompleteInform`)

## Newly logged (virtual-closure batch 2026-02-13-15)
- Static string `"..\\ZoneServerLog\\SystemLog\\Patriarch\\FinalDecisionProcessor_%d.log"` (`FinalDecisionProcessor::Initialize`)
- Static string `"..\\ZoneServerLog\\SystemLog\\Patriarch\\SecondCandidateCrystallizer_%d.log"` (`SecondCandidateCrystallizer::Initialize`)
- Static string `"..\\ZoneServerLog\\SystemLog\\Patriarch\\Voter_%d.log"` (`Voter::Initialize`)

## Newly logged (virtual-closure batch 2026-02-13-16)
- Static string `"[KEEPER][%d] >> %s (pos: %s {%d, %d, %d})"` (`CHolyKeeper::GetObjName`)
- Packet type bytes `{4, 167}` (`CHolyKeeper::SendMsg_FixPosition`)
- Packet type bytes `{4, 125}` (`CHolyKeeper::SendMsg_RealMovePoint`)
- Packet type bytes `{5, 151}` (`CHolyKeeper::SendMsg_Attack`)
- Static string `"[STONE][%d] >> %s (pos: %s {%d, %d, %d})"` (`CHolyStone::GetObjName`)
- Packet type bytes `{4, 166}` (`CHolyStone::SendMsg_FixPosition`)

## Newly logged (virtual-closure batch 2026-02-13-17)
- Static string `"[Trap][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)"` (`CTrap::GetObjName`)
- Packet type bytes `{4, 168}` (`CTrap::SendMsg_FixPosition`)

## Newly logged (virtual-closure batch 2026-02-13-18)
- Static string `"[MONSTER] >> %s (pos: %s {%d, %d, %d})"` (`CMonster::GetObjName`)
- Packet type bytes `{4, 11}` (`CMonster::SendMsg_FixPosition`)
- Packet type bytes `{4, 22}` (`CMonster::SendMsg_RealMovePoint`)

## Newly logged (virtual-closure batch 2026-02-13-19)
- Static string `"[PLAYER][%d] >> %s (pos: %s {%d, %d, %d})"` (`CPlayer::GetObjName`)
- Static string `"CLOSE>> OutOfSec() ID: %s, NM: %s"` (`CPlayer::OutOfSec`)
- Packet type bytes `{4, 9}` (`CPlayer::SendMsg_FixPosition`)
- Packet type bytes `{4, 21}` (`CPlayer::SendMsg_RealMovePoint`)
- Packet type bytes `{17, 13}` (`CPlayer::SendMsg_SetHPInform`)

## Newly logged (PushDQS missing-caller closure 2026-02-13-20)
- DQS query case `0x0D` payload `_qry_case_addpvppoint` (`CTrap::RecvKillMessage`, `GUILD_BATTLE::CNormalGuildBattleGuildMember::PushDQSPvpPoint`)
- DQS query case `0x18` payload `_log_change_class_after_init_class` (`CUserDB::WriteLog_ChangeClassAfterInitClass`)
- DQS query case `0x1F` payload `_qry_case_updatewinloseguildbattlerank` (`GUILD_BATTLE::CNormalGuildBattle::PushDQSWinLoseRank`)
- DQS query case `0x20` payload `_qry_case_updatedrawguildbattlerank` (`GUILD_BATTLE::CNormalGuildBattle::PushDQSDrawRank`)
- DQS query case `0x27` payload `_qry_case_in_guildbattlerewardmoney` (`CGuild::PushDQSInGuildBattleRewardMoney`)
- DQS query case `0x2F` payload `_qry_case_updateweeklyguildpvppointsum` (`CWeeklyGuildRankManager::PushDQSIncWeeklyPvpPointSum`)
- DQS query case `0x34` payload `_qry_case_in_atrade_tax` (`TRC_AutoTrade::PushDQSData_GuildInMoney`)
- DQS query case `0x53` with null payload (`RFEvent_ClassRefine::SetEvent`)
- DQS query case `0x54` payload `_qry_case_guild_battel_result_log` (`GUILD_BATTLE::CNormalGuildBattle::GuildBattleResultLogPushDBLog`)
- DQS query case `0x86` payload `_qry_case_update_honor_guild` (`CHonorGuild::SetNextHonorGuild`)
- DQS query case `0x92` payload `_qry_case_character_rename` (`DE_Potion_CharReName`)
- DQS query case `0x9D` with null payload (`CashItemRemoteStore::Check_CashEvent_Status`)
- Static string `"%s [ %d ] DST: [ %d ] type: logoff_inc  >> pvp : %.0f  last: %.0f"` (`CTrap::RecvKillMessage`)
- Static string `"[ %d ] type: guildbattle  >> pvp : %d  last: %.0f"` (`GUILD_BATTLE::CNormalGuildBattleGuildMember::PushDQSPvpPoint`)
- Static string `"CWeeklyGuildRankManager::PushDQSIncWeeklyPvpPointSum( %u, %f ) Fail!"` (`CWeeklyGuildRankManager::PushDQSIncWeeklyPvpPointSum`)
- Static string `"CHonorGuild::SetNextHonorGuild() : !::IsSQLValidString( pRecv->GuildList[i].wszGuildName(%s) ) Invalid!"` (`CHonorGuild::SetNextHonorGuild`)
- Static string `"Set Next Honor Guild (RACE:%d) : (%s:%d%%)"` (`CHonorGuild::SetNextHonorGuild`)
- Static string `"1900-00-00 00:00"` (`GUILD_BATTLE::CNormalGuildBattle::GuildBattleResultLogPushDBLog`)
- Static string `"NONE"` (`GUILD_BATTLE::CNormalGuildBattle::GuildBattleResultLogPushDBLog`)
- Static value `dwGuildBattleCostGold = 5000` (`GUILD_BATTLE::CNormalGuildBattle::GuildBattleResultLogPushDBLog`)

## Newly logged (patriarch-invalid-check restore 2026-02-13)
- DQS query case byte `0x8A` with 1-byte payload `{_eProcessType}` (`PatriarchElectProcessor::PushDQSCheckInvalidChar`)

## Newly logged (fireguard-expulsion alignment 2026-02-13)
- Packet type bytes `{102, 5}` (`sbyCcrFgBlock`, `CNetworkEX::ExpulsionSocket`)
- Payload `_ccrfg_detect_alret{byValue=0}` with `size()==1` (`sCcrFgBlock`, `CNetworkEX::ExpulsionSocket`)

## Newly logged (patriarch-virtual-override de-stub 2026-02-13)
- Packet type bytes `{56, 5}` (`Voter::_SendVotePaper`, `Voter::_SendVotePaperAll`)
- Packet type bytes `{56, 6}` (`Voter::_SendVoteScore`, `Voter::_SendVoteScoreAll`)
- Packet type bytes `{56, 7}` (`FinalDecisionProcessor::_FinalDecision`, `FinalDecisionProcessor::_ReqNetFinalDecision`)
- Static string `"Invalid value >> CandidateMgr::Instance()->GetCandidate_2st(%d, %d)"` (`Voter::_MakeVotePaper`)
- Static string `"_Vote() Invalid player pointer : Candidate(%s) Rank(%d) : %s"` (`Voter::_Vote`)
- Static string `"================================================ [Race:%d]"` (guessed, `CandidateMgr::FinalDecision`)
- Static string `"RACE(%d) IDX(%d) Serial(%d) Name(%s) Score(%d)"` (guessed, `CandidateMgr::FinalDecision`)
- Static string `"RACE(%d) TOTAL_VOTE(%d) NON_VOTE(%d) HIGH_GRADE(%d)"` (guessed, `CandidateMgr::FinalDecision`)
- Static string `"------------------------------------------------ [Race:%d]"` (guessed, `CandidateMgr::FinalDecision`)

## Newly logged (void-audit alignment batch 2026-02-13)
- Static string `"CItemLootTable.. %d rec %s item no search table"` (`CItemLootTable::Indexing`)
- Static string `"CItemLootTable.. %d rec %s item no search index"` (`CItemLootTable::Indexing`)
- Static string `"CItemUpgradeTable.. record Num <= 0"` (`CItemUpgradeTable::ReadRecord`)
- Static string `"CItemUpgradeTable.. %d rec %s data no search index"` (`CItemUpgradeTable::Indexing`)
- Static string `"COreCuttingTable.. ore rec num <= 0"` (`COreCuttingTable::ReadRecord`)
- Static string `"COreCuttingTable.. %s no search index"` (`COreCuttingTable::Indexing`)
- Packet type bytes `{59, 7}` (`CCouponMgr::SendMsg_InPcBangTime`)
- Packet type bytes `{59, 6}` (`CCouponMgr::SendMsg_CouponEnsure`)
- Packet type bytes `{59, 9}` (`CCouponMgr::SendMsg_RemainCouponInform`)
- Packet type bytes `{59, 8}` (`CCouponMgr::SendMsg_CouponError`)
- Packet type bytes `{7, 69}` (`CCouponMgr::SendMsg_CouponLendResult`)
