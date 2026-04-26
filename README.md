# RF Online Server

## 💖 Support this project

### Crypto (Recommended)
- USDT (TRC20): `TAiR5W8Qrsz6QQX25G7rWAq6ZqeYP9VzhQ`
⚠️ Make sure to use TRC20 network

- Binance ID : `1225316330`

### Non-Crypto
- Saweria : https://saweria.co/likertuban


## Solution
This repository contains the Account, Login, and World server projects plus shared libraries and tools.
`RFOnlineServer.sln` includes:
- AccountServer (net9.0-windows WinForms, x64)
- LoginServer (net9.0-windows WinForms, x64)
- LoginServer.Data (net9.0 class library)
- RFNetworking and RFPackets (shared C# libraries)
- WorldServer (C++/MFC, v143)
- MapEditor (C#)

## Current Status
### AccountServer
- Windows desktop app targeting net9.0-windows, x64.
- Uses EF Core with SQL Server, SQLite, and MySQL providers.
- Database schema scripts live in `AccountServer/scheme/`.

### LoginServer
- Windows desktop app targeting net9.0-windows, x64.
- Depends on LoginServer.Data plus RFNetworking/RFPackets.
- Uses Argon2 and System.Text.Json.

### WorldServer
- C++ MFC application (toolset v143), intended to build x64.
- current development focus