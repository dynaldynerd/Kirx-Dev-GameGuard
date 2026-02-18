@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
cl /nologo /EHsc /std:c++17 /I"." tmp_ida_size.cpp /link /OUT:tmp_ida_size.exe
if errorlevel 1 exit /b 1
tmp_ida_size.exe
