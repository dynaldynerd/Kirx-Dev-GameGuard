# Send Helper Refactor TODO (IDA Alignment)

All non-standard free `Send*` helper entries have been converted to `CPlayer::SendMsg_*` member functions.

## CMainThread::Init Audit TODO (IDA Alignment)

- [ ] `CMainThread::OnRun` currently uses a temporary `CMapDisplay::DrawDisplay(&g_MapDisplay)` non-GUI stub in `WorldServer/Entities/CMainThread.cpp`; replace with the real GUI MapDisplay subsystem implementation later.
