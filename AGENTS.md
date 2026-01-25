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
