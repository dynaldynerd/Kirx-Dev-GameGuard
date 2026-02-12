# CMainThread Init/Run Parity Execution Plan (with `plan.md` first)

## Summary
1. First mutation will be creating/updating `plan.md` with this exact execution plan.
2. Second mutation will ensure `todo.md` contains unresolved tracking for `CMapDisplay::DrawDisplay(&g_MapDisplay)` and mark it as intentional.
3. Then implement full IDA-aligned `CMainThread` init/run-loop chain (strict parity, no stubs, member functions where `this` exists).
4. Keep `CMapDisplay` unresolved for this pass as requested, and allow temporary build failure only for that unresolved symbol.

## Public API / Interface Changes
1. Update `WorldServer/Entities/CMainThread.h` with missing members:
`OnRun`, `CheckAvatorState`, `CheckAccountLineState`, `ForceCloseUserInTiming`, `CheckConnNumLog`, `ContUserSaveJobCheck`, `CheckDayChangedPvpPointClear`, `CheckRadarItemDelay`, `CheckServerRateINIFile`, `CheckForceClose`, `ServerStateMsgGotoWebAgent`, `PingToAccount`, `LoadServerRateINIFile`, `SetServerRate`.
2. Update `WorldServer/Entities/EconomySystemFunctions.h` with missing free functions:
`eUpdateEconomySystem`, `SendMsg_EconomyDataToWeb`, `_UpdateRateSendToAllPlayer`, `OnLoop_VoteSystem`, `OnLoop_GuildSystem`.
3. Update RF event interfaces:
add `virtual void Loop();` to `RFEventBase` and `void Loop();` to `RFEvent_ClassRefine` (plus IDA-required refine helpers if missing).
4. Update globals in `WorldServer/Entities/GlobalObjects.h` and `WorldServer/Entities/GlobalObjects.cpp`:
`g_dwCurTime`, `e_dwMinCount`, `e_dwUserCumCount[3]`.

## Implementation Steps
1. Write `plan.md` first with this plan.
2. Update `todo.md` to explicitly track unresolved `CMapDisplay::DrawDisplay(&g_MapDisplay)` as intentional pending subsystem reconstruction.
3. Implement `CMainThread::RuleThread` parity:
set `g_dwCurTime`, call `m_MainFrameRate.CalcSpeedPerFrame()`, call `OnRun()`, preserve IDA sleep behavior.
4. Implement `CMainThread::OnRun` in IDA call order from `CMainThread__OnRun_1.c`, including the unresolved `CMapDisplay::DrawDisplay(&g_MapDisplay)` call.
5. Implement missing `CMainThread` methods from matching IDA files:
`CheckAvatorState`, `CheckAccountLineState`, `ForceCloseUserInTiming`, `CheckConnNumLog`, `ContUserSaveJobCheck`, `CheckDayChangedPvpPointClear`, `CheckRadarItemDelay`, `CheckServerRateINIFile`, `CheckForceClose`, `ServerStateMsgGotoWebAgent`, `PingToAccount`, `LoadServerRateINIFile`, `SetServerRate`.
6. Implement free-loop/economy helpers from IDA:
`eUpdateEconomySystem`, `SendMsg_EconomyDataToWeb`, `_UpdateRateSendToAllPlayer`, `OnLoop_VoteSystem`, `OnLoop_GuildSystem`.
7. Implement dependency-closure methods required by `OnRun` where currently missing, each from its exact IDA function:
`CAsyncLogger::Loop`, `CDarkHoleDungeonQuest::CheckQuestOnLoop`, `CGoldenBoxItemMgr::Loop_Event`, `CGuildBattleController::Loop`, `CHolyStoneSystem::OnLoop`, `CHonorGuild::Loop`, `CItemStoreManager::Loop`, `CMapOperation::OnLoop`, `CMoneySupplyMgr::LoopMoneySupply`, `CMoveMapLimitManager::Loop`, `CNationSettingManager::Loop`, `CPostSystemManager::Loop`, `CPvpUserAndGuildRankingSystem::Loop`, `CRaceBossMsgController::OnLoop`, `CRaceBuffManager::Loop`, `CRecallEffectController::OnLoop`, `CReturnGateController::OnLoop`, `CTransportShip::Loop`, `CUnmannedTraderController::Loop`, `CWorldSchedule::Loop`, `CashItemRemoteStore::Loop_TatalCashEvent`, `GuildCreateEventInfo::Loop`, `PatriarchElectProcessor::Loop`, `TimeLimitMgr::Chack_Time`, `RFEventBase::Loop`, `RFEvent_ClassRefine::Loop`.
8. Update `CMainThread::MakeSystemTower` to full IDA branch behavior, including `CreateSystemTower(...)`.
9. Apply strict Init mismatches:
`CandidateMgr` failure text to `(300)` and IDA-style `g_pFrame` send path.
10. Log newly introduced static strings/data into `StaticDataLog.md` before each related implementation (rule #10).

## Validation / Test Cases
1. Build with:
`& "C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe" .\WorldServer\WorldServer.vcxproj /m /p:Configuration=Debug /p:Platform=x64`.
2. Expectation for this pass:
only unresolved `CMapDisplay::DrawDisplay` / `g_MapDisplay` is allowed.
3. Symbol coverage checks:
`rg` for all newly declared/implemented methods listed above in headers and cpp files.
4. TODO tracking check:
`todo.md` contains unresolved `CMapDisplay::DrawDisplay` entry and no duplicate stale lines.
5. Behavioral parity checks:
verify `OnRun` call order matches IDA and `CheckForceClose` is exact IDA behavior.

## Assumptions and Defaults
1. Strict IDA parity is enforced over local safety tweaks unless you override.
2. No stubs or shortened implementations are allowed.
3. Functions with `this` in IDA are implemented as member functions only.
4. RU-only nation rule remains unchanged.
5. This pass intentionally leaves `CMapDisplay` unresolved and tracked in `todo.md`.

## Current Progress
1. `CMainThread::RuleThread` / `CMainThread::OnRun` are wired and building.
2. Run-loop dependency symbols were added so build now links through those paths.
3. CandidateMgr init mismatch in `CMainThread::Init` was aligned to `(300)`.
4. Current linker status is reduced to intentional `CMapDisplay::DrawDisplay` + `g_MapDisplay` unresolved only.
5. Dependency-limited parity gaps were moved to `todo.md` for explicit follow-up.
