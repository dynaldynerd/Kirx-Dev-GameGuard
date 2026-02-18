@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
cl /nologo /EHsc /std:c++17 /I".\WorldServer" /I".\WorldServer\Entities" /I".\WorldServer\ThirdParty\tinyxml" /I".\WorldServer\ThirdParty\lua-5.1.5\src" /I".\WorldServer\ThirdParty\cryptopp" tmp_offsets.cpp /link /OUT:tmp_offsets.exe
if errorlevel 1 exit /b 1
tmp_offsets.exe
