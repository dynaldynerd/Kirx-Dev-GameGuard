# Send Helper Refactor TODO (IDA Alignment)

All non-standard free `Send*` helper entries have been converted to `CPlayer::SendMsg_*` member functions.

## CMainThread::Init Audit TODO (IDA Alignment)

- [ ] `CMainThread::OnRun` currently uses a temporary non-GUI `CMapDisplay::DrawDisplay()` path in `WorldServer/Entities/CMapDisplay.cpp`; replace with the real GUI MapDisplay subsystem implementation later.

## send packet caller
none (all current RU-scoped entries implemented)

### ida half-implemented/stub caller audit

#### Scan basis
- Sources: `WorldServerIdaPro/functions` (IDA decompiled) vs `WorldServer/Entities/*.cpp` (current implementation).
- Target scope: all entries under `## send packet caller` in this file.
- Status rules: `missing_callee`, `misaligned_caller_gap`, `stub_callee`, with `definition_only_in_ida` tag when IDA hit exists only on line 1.
- IDA caller extraction: only matches where line number is `> 1` are treated as callers.
- IDA caller normalization: decompiled file stem `A__B__C_1.c` -> `A::B::C`.

## C* ctor/dtor implementation audit

- Scan scope: WorldServer/**/*.h|hpp|inl|cpp (excluding WorldServer/ThirdParty).
- Source of truth: IDA ctor/dtor symbols in WorldServerIdaPro/functions.
- Filter: RU scope only; non-RU nation variants excluded.
- Queue status: remaining actionable = 0, need ctor+dtor = 0, need ctor only = 0, need dtor only = 0, removed non-actionable(no IDA) = 0, removed non-RU = 4, completed in codebase = 86.

### Missing ctor + dtor (0)
none

### Missing ctor only (0)
none

### Missing dtor only (0)
none
