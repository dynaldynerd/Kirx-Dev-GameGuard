# Send Helper Refactor TODO (IDA Alignment)

All non-standard free `Send*` helper entries have been converted to `CPlayer::SendMsg_*` member functions.

## CMainThread::Init Audit TODO (IDA Alignment)

- [ ] `CMainThread::OnRun` currently uses a temporary `CMapDisplay::DrawDisplay(&g_MapDisplay)` non-GUI stub in `WorldServer/Entities/CMainThread.cpp`; replace with the real GUI MapDisplay subsystem implementation later.

## send packet caller
none (all current RU-scoped entries implemented)

### ida half-implemented/stub caller audit

#### Scan basis
- Sources: `WorldServerIdaPro/functions` (IDA decompiled) vs `WorldServer/Entities/*.cpp` (current implementation).
- Target scope: all entries under `## send packet caller` in this file.
- Status rules: `missing_callee`, `misaligned_caller_gap`, `stub_callee`, with `definition_only_in_ida` tag when IDA hit exists only on line 1.
- IDA caller extraction: only matches where line number is `> 1` are treated as callers.
- IDA caller normalization: decompiled file stem `A__B__C_1.c` -> `A::B::C`.
