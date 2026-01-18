---
name: rf-online-server-emulation
description: Work on the RF Online server emulator in this repo, including packet struct layout/serialization, SAEA networking, login/account flow, and IDA-driven behavior matching. Use when editing RFPackets, RFNetworking, LoginServer, AccountServer, or when translating decompiled logic into C#.
---

# RF Online Server Emulation

## Workflow
1. Identify which line the packet belongs to (client/login/account/world/control).
2. Update packet structs only in `RFPackets/` and add `Load()`/`ToArray()` there.
3. In handlers, parse payload with `Load()` and pass the struct (no `PacketEnvelope` in handler signatures).
4. Follow `todo.md` and decompiled sources in `LoginServerIdaPro/` and `AccountServerIdaPro/` for logic.
5. Keep DB access async via `IAccountDatabase` and keep login/account serials sourced from DB.
6. Planned refactor: move packet structs to classes, replace fixed `byte[]` strings with `string` + size comment (e.g. `// byte[13]`), keep `Load()`/`ToArray()` on the class using manual parsing, and let DB fill session objects directly.

## Repo Anchors
- Packets: `RFPackets/`
- Networking/framing: `RFNetworking/`
- Login server: `LoginServer/`
- Account server: `AccountServer/`

## Guardrails
- No TryParse helpers; use `Load()` on packet structs.
- Use the exact struct layout from IDA headers.
- Internal packets are OpCode > 100; ping is Op=101/Sub=1, payload `[0]`.

## Tooling
- Shell: PowerShell 7 (use `&&` when chaining).
- Build: `dotnet build RFOnlineLoginServer.sln`
- MSBuild: `C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe`
- Full build command: `& "C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe" .\RFOnlineLoginServer.sln /m /p:Configuration=Debug /p:Platform=x64`
