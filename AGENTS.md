# AGENTS

## Focus
- Implement WorldServer core functions in C++ based on IDA.

## Sources of Truth
- WorldServer IDA structs live in `WorldServerIdaPro/structs`.
- WorldServer IDA functions live in `WorldServerIdaPro/functions`.

## Build
- WorldServer builds target x64 only.
- MSBuild: `C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe`
- Build command: `& "C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe" .\WorldServer\WorldServer.vcxproj /m /p:Configuration=Debug /p:Platform=x64`

## WorldServer Rules
1. Structs/classes live in dedicated `.h` files, not inside `.cpp`.
2. Function declarations go in `.h`; definitions go in `.cpp`.
3. Do not model vtable pointers as fields; use virtual methods instead.
4. Every new class header also needs a minimal `.cpp` (even if it just includes the header) so Visual Studio picks up the symbol.
5. Headers with only `struct` do not need a `.cpp`.
6. Any type name starting with `C` should be declared as a `class`, not `struct`.
7. all g_* variable are meant to be a global variable and must be put on `GlobalObjects.h` and `GlobalObjects.cpp`
8. all new stub function must be added to `todo.md`
9. all `CRecordData::GetRecord` usually only return _base_fld hence the decompiled code might produce weird code such as Record[2].m_strCode[10], in that case you must write a note on `Record.md` so I could review and correct the implementation
10. for any static string/array/data that appear in the decompiled code please write it in a log file before proceeding 