# Send Helper Refactor TODO (IDA Alignment)

All non-standard free `Send*` helper entries have been converted to `CPlayer::SendMsg_*` member functions.

## CMainThread::Init Audit TODO (IDA Alignment)

- [ ] `CMainThread::OnRun` currently uses a temporary `CMapDisplay::DrawDisplay(&g_MapDisplay)` non-GUI stub in `WorldServer/Entities/CMainThread.cpp`; replace with the real GUI MapDisplay subsystem implementation later.
- [ ] `CDarkHoleDungeonQuest::CheckQuestOnLoop` cannot call full IDA flow yet because `CDarkHoleChannel::OnLoop` and its dependent DarkHole channel loop chain are not reconstructed.
- [ ] `CMapOperation::OnLoop` is currently dependency-limited (`CMapData::OnLoop`, `CGameObject::OnLoop`, player auto-recover chain, and event respawn loop symbols are missing from active interfaces).
- [ ] `CHolyStoneSystem::OnLoop` is dependency-limited (`HSKRespawnSystem`, `UpdateNotifyHolyStoneHPToRaceBoss`, `CheckDestroyerIsArriveMine` are still unresolved in active code path).
- [ ] `CRaceBossMsgController::OnLoop` is dependency-limited (`UpdateSend`/manager refresh pipeline from IDA depends on missing race-boss message manager interfaces in current source).
- [ ] `PatriarchElectProcessor::TimeCheck` still needs final closure for missing source interfaces (`CandidateMgr::InitCandidate`, reset-token push flow, and race-vote reset helper path).
