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
8. all new stub function must be added to `todo.md`. (this rule now obsolet, please follow rule #14)
9. all `CRecordData::GetRecord` usually only return _base_fld hence the decompiled code might produce weird code such as Record[2].m_strCode[10], in that case you must write a note on `Record.md` so I could review and correct the implementation
10. for any static string/array/data that appear in the decompiled code please write it in a log file before proceeding, if it is just used on log you could guess it, don't worry if it is wrong. 
11. you are not allowed to implement a function as Class:Function() any function that had "this" as first parameter mean it's a member function and shouldn't be made into a static function, unless you are sure it is a static function make sure to implement it as a member function this->Function()
12. avoid shortened function implementation/half baked implementation as it would be dangerous when previewing the code where I thought everything is implemented turned out it was only half baked and wondered later what was wrong, the logic/implementation must exactly match the decompiled code without any shortened logic and must adhere to the all previous rules
13. added `RF Online Old Sourcecode` an older (ten years or more older) version of the sourcecode, it might be useful for reference, but it's not the same as the decompiled code, so use it with caution and use it only when you're sure it's correct, this source mostly useful for map/path related function (R3Engine) but mostly useless for anything else
14. please don't create any stub function or shortened version of a function compared to decompiled source, let it be undeclared so we could know which function need implementation, if the original function is no-op or just return false/true and look like a stub please add a comment saying "this is not a stub"
15. remove/don't implement any thunk function and directly implement only the real function
16. for iterating global variable that the size is allocated using macro definition, use the macro for iteration instead of 500 for g_NPC, 2523 for g_Player, etc
17. avoid ida pro variable such as a1, a2, v1, v2 as much as possible unless you don't have any idea what that variable is
18. please skip implementing stackFill or debug stack-fill junk (0xCCCCCCCC) at the start of a function, it just MSVC Debug being MSVC and we should get rid of it from our program as it was automatically generated to a debug MSVC program 
19. for any nation-related implementation, only implement RU; other nation variants should be left unimplemented unless explicitly requested
20. before rebasing, always create a backup branch first (and push it to remote before continuing).
21. when fixing merge conflicts, do the simplest merge: list all conflicted functions/headers and only add missing function/struct/class/global variable/static variable from both sides. Example: if one side has A, B, E and the other has B, C, D then merge into A, B, C, D, E with no duplicate B.
22. for `Record.md` conflicts, combine both sides and then remove duplicate entries.

## Special helper/non-standard function rules
- don't fix the helper yet, but check the function caller and check if the function caller is already match the ida decompiled logic while maintaining sane variable and modern C++ practice
- if the caller is fixed, implement the correct member function
- if the correct member function has been fixed, remove the helper from todo.md list
