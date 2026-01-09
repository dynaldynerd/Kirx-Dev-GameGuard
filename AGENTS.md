# AGENTS

## Purpose
- Emulate RF Online login/account servers in C# WinForms with a shared packet library and SAEA networking.

## Repo Layout
- `RFOnlineLoginServer.sln`
- `LoginServer/`: client-facing login server (WinForms).
- `AccountServer/`: central account server (WinForms, listen-only).
- `RFNetworking/`: SAEA networking, packet framing, internal packet handling.
- `RFPackets/`: packet structs with `Load()`/`ToArray()`, `PacketStringUtil`, `_CLID`, `_GLBID`.
- `LoginServerIdaPro/`, `AccountServerIdaPro/`: decompiled source and headers (source of truth).
- `rules.md`, `todo.md`: required guardrails and current tasks.

## Protocol
- Header: 2-byte total size (header + body), 1-byte OpCode, 1-byte SubCode.
- Max body: 60000 bytes.
- OpCode > 100 is internal; default ping is Op=101/Sub=1 with payload `[0]`.
- Framing logic lives in `RFNetworking/PacketFramer.cs`.

## Guardrails (must follow)
- Never pass `PacketEnvelope` into handler methods; parse to struct via `Load()`.
- Do not create TryParse helpers; add `Load()`/`ToArray()` on packet structs in `RFPackets/`.
- Use `StructLayout` with correct packing (typically `Pack=1`) per IDA headers.
- If a handler returns `false`, disconnect the connection.

## Runtime Flow
- LoginServer connects to AccountServer, then opens the client listener.
- AccountServer listens on login/world/control ports (see `AccountServer/UI/MainForm.cs`).
- LoginServer settings live in `appsettings.login.json` via `LoginServer/Settings/AppSettings.cs`.
- AccountServer DB connection uses `ACCOUNT_DB_CONNECTION` env var (SqlClient).

## Login Flow (Account Server)
- `AccountServer/Server/AccountHandler.cs` implements `DoLoginAsync`:
  - Validate ID/PW length and user code.
  - UserCode 1: `Select_StaffInfoEx` + `Select_StaffExpire`.
  - UserCode 0: `Select_UserInfo` or `Insert_User` + `Select_UserSerial`.
  - Ban checks: `Exist_UserBan` + `Select_UserAccountBlockInfo`.
  - Reply with `_login_account_result_aclo` (Op=1/Sub=4).
  - Register client session only on success.

## Build
- `dotnet build RFOnlineLoginServer.sln`
- `dotnet build LoginServer/LoginServer.csproj`
- `dotnet build AccountServer/AccountServer.csproj`

## Notes
- Use IDA headers as the source of truth for packet layout and flow.
- Keep ASCII in new files unless existing content requires otherwise.
- Planned refactor: convert packet structs to classes, store strings as `string` (annotate original fixed sizes, e.g. `// byte[13]`), keep `Load()`/`ToArray()` on packet classes with manual parsing/serialization. DB methods should fill the session object directly and return only success/retcode.
