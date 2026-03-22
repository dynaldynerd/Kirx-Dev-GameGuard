# RF Online Server

This repository contains the current tracked source for the RF Online server workspace.

The main solution file is `RFOnlineServer.sln`.

## Included In This Repository

- `AccountServer`
  WinForms server/admin application (`net10.0-windows`, x64)
- `LoginServer`
  WinForms server application (`net10.0-windows`, x64)
- `LoginServer.Data`
  Shared EF/data layer for `LoginServer`
- `RFNetworking`
  Shared networking library
- `RFPackets`
  Shared packet definitions
- `WorldServer`
  Native C++/MFC server project
- `MapEditor`
  Auxiliary WinForms tool
- `AccountServer/scheme`
  Tracked SQL schema files for account-side databases
- `LoginServer/scheme`
  Tracked SQL schema files for billing/login-side databases


## Requirements

### Managed Projects

- Windows
- .NET 10 SDK

### WorldServer

- Windows
- Visual Studio 2022 with MSVC `v143`
- MFC for `v143` / x64
- Windows 10 SDK

`WorldServer/WorldServer.vcxproj` uses:

- `PlatformToolset = v143`
- `UseOfMfc = Dynamic`
- `WindowsTargetPlatformVersion = 10.0`

## Building

### Managed Projects

From the repository root:

```powershell
dotnet build AccountServer/AccountServer.csproj
dotnet build LoginServer/LoginServer.csproj
```

### WorldServer

Open `RFOnlineServer.sln` in Visual Studio and select an x64 configuration, or build the project directly with MSBuild.

Example command used in this workspace:

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe" .\WorldServer\WorldServer.vcxproj /m /p:Configuration=Debug /p:Platform=x64
```

Important notes for `WorldServer`:

- x64 is required
- `Debug|x64`, `ShipDebug|x64`, and `Release|x64` are the relevant configurations
- the project currently has hardcoded x64 output/debug directories under `D:\RF Server\Server\Zone Server\RF_Bin\`
- if your local machine does not use that path, you may need to adjust the `.vcxproj` or your debugger settings

## Solution Layout

`RFOnlineServer.sln` currently includes:

- `AccountServer`
- `LoginServer`
- `LoginServer.Data`
- `RFNetworking`
- `RFPackets`
- `WorldServer`
- `MapEditor`

## Runtime Notes

- `AccountServer` and `LoginServer` create their settings JSON files at runtime; those files are not committed
- database provider support in the managed servers includes SQL Server, MariaDB, and SQLite
- the root README is intentionally limited to tracked repository content and build prerequisites
