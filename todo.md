# WorldServer IDA C* class tracker

Total IDA C* types: 443
Already in WorldServer/Entities: 283
Missing (game-specific): 71
Deferred UI/App (not yet imported): 22
Note: CryptoPP::* types are external Crypto++ library; add include/lib paths later.
Note: Win32/COM SDK types are external; ensure Windows SDK headers/libs are available later if needed.




## Missing C* types
- CLASS_REFINE_EVENT
- CLogTypeDBTask
- CLogTypeDBTaskManager
- CLogTypeDBTaskPool
- CLootingMgr::_list
- CLuaCommandEx::_State
- CLuaEventMgr
- CLuaEventNode
- CLuaEventNode::_State
- CLuaLootingMgr
- CLuaLooting_Novus_Item
- CLuaLooting_Novus_Item::_State
- CLuaScript
- CLuaScript::_State
- CLuaScriptMgr
- CLuaSignalReActor::RF_LUA_SIGNAL
- CLuaSignalReActor::_Action
- CM_Power_Data_s
- CPostSystemManager
- CProcessLocal
- CProcessLocalObject
- CProcessThread
- CPvpCashMng
- CPvpUserAndGuildRankingSystem
- CPvpUserRankingInfo
- CPvpUserRankingTargetUserList
- CRFBillingdatabase
- CRFCashItemDatabase
- CRFDBItemLog
- CRFMonsterAIMgr
- CRaceBossMsgController
- CRaceBossWinRate
- CRecallEffectController
- CRecallRequest
- CReturnGate
- CReturnGateController
- CReturnGateCreateParam
- CRtc
- CRusiaBillingMgr
- CSUItemSystem
- CSetItemEffect::set_effect_info
- CSetItemType
- CSlotData
- CSurface
- CTSingleton
- CTakeOut
- CTalkCrystalCombineManager
- CTerm
- CThreadSlotData
- CTotalGuildRankInfo
- CTotalGuildRankManager
- CTotalGuildRankRecord
- CUnDeadLock
- CUserRankingProcess
- CWeaponBulletLinkTable
- CWeeklyGuildRankInfo
- CWeeklyGuildRankManager
- CWeeklyGuildRankOwnerInfo
- CWeeklyGuildRankRecord
- CWorldSchedule
- CashDbWorker
- CashItemRemoteStore
- CashItemRemoteStore::_remain_num_of_good
- ClassOrderProcessor
- ControllerTaxRate
- CookieDecision


## Deferred UI/App C* types
- CAboutDlg
- CDisplay
- CDisplayView
- CGameServerApp
- CGameServerDoc
- CGameServerView
- CIPXTab
- CInfoSheet
- CMainFrame
- CMapDisplay
- CMapTab
- CObjectSearchDlg
- CObjectTab
- COpenDlg
- CParkingWnd
- CPushRoutingFrame
- CPushRoutingView
- CReflectorWnd
- CServerTab
- CTCPTab
- CsFrame
