using AccountServer.Data;
using AccountServer.Settings;
using AccountServer.State;
using RFNetworking;
using System.Net;
using LoginServer.Packets;

namespace AccountServer.Server;

public sealed class WorldHandler : AccountHandlerBase
{
    private const string FireguardReason = "Fireguard Block";
    private const string FireguardWriter = "Fireguard System";
    private const string UiLockFindPassReason = "UILockFindPass";
    private const string UiLockFailReason = "UILockFail";

    public WorldHandler(
        Action<string> log,
        AppSettings settings,
        string? connectionString = null,
        IAccountDatabase? db = null)
        : base(log, settings, connectionString, db)
    {
    }

    public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] connected from {connection.RemoteEndPoint} (world)");
        _context.RegisterWorldConnection(connection);
        return Task.CompletedTask;
    }

    public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] disconnected (world)");
        return CloseWorldServerAsync(connection, cancellationToken);
    }

    public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        return Task.CompletedTask;
    }

    private async Task CloseWorldServerAsync(PublicConnection connection, CancellationToken token)
    {
        if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession))
        {
            _log($"CMainThread::CloseWorldServer({connection.ConnectionId}).. GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
            _context.UnregisterWorld(connection);
            return;
        }

        int worldCode = worldSession.WorldCode;
        if (worldCode < 0 || !_context.TryGetWorld(worldCode, out var world))
        {
            _log($"CMainThread::CloseWorldServer({connection.ConnectionId}).. GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
            _context.UnregisterWorld(connection);
            return;
        }

        var send = new _inform_close_world_aclo
        {
            dwWorldCode = (uint)worldCode
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 101,
            Payload = send.ToArray()
        };

        await SendToLoginServersAsync(env, token).ConfigureAwait(false);

        foreach (var account in _context.GetActiveAccountsSnapshot())
        {
            if (account.IsLogin && account.WorldCode == worldCode)
            {
                ReleaseAccount(account, "CloseWorldServer");
            }
        }

        _context.UpdateWorldService(worldCode, false);
        _context.UpdateWorldGate(worldCode, 0, 0);
        _context.UnregisterWorld(connection);
        _log($"{DateTime.Now:HH:mm:ss}/ Disconnect World Server : Code ({worldCode}), Name ({world.Name})");
    }

    public override async Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        bool ok = await DispatchWorldLineAsync(connection, packet, cancellationToken).ConfigureAwait(false);
        if (!ok)
        {
            _log($"[{connection.ConnectionId}] handler returned false; disconnecting.");
            connection.Close();
        }
    }

    private Task<bool> DispatchWorldLineAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        switch (packet.OpCode)
        {
            case 1:
                switch (packet.SubCode)
                {
                    case 1:
                    {
                        var req = new _open_world_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return OpenWorldRequest(connection, req, token);
                    }
                    case 3:
                    {
                        var req = new _start_world_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return StartWorldRequest(connection, req, token);
                    }
                    case 4:
                    {
                        var req = new _stop_world_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return StopWorldRequest(connection, req, token);
                    }
                    case 5:
                    {
                        var req = new _logout_account_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return CloseAccountFromWorldRequest(connection, req, token);
                    }
                    case 9:
                    {
                        var req = new _trans_account_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return TransAccountReport(connection, req, token);
                    }
                    case 10:
                    {
                        var req = new _enter_world_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return EnterWorldRequest(connection, req, token);
                    }
                    case 12:
                    {
                        var req = new _select_avator_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return SelectAvatarReport(connection, req, token);
                    }
                    case 13:
                    {
                        var req = new _enter_lobby_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return EnterLobbyReport(connection, req, token);
                    }
                    case 14:
                    {
                        var req = new _fireguard_block_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return FireguardBlockRequest(connection, req, token);
                    }
                    case 15:
                    {
                        var req = new _uilock_init_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return UiLockInitRequest(connection, req, token);
                    }
                    case 17:
                    {
                        var req = new _uilock_update_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return UiLockUpdateRequest(connection, req, token);
                    }
                    case 19:
                    {
                        var req = new _uilock_user_refresh_info_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return UiLockRefreshRequest(connection, req, token);
                    }
                    case 22:
                    {
                        var req = new _fireguard_divide_block_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return FireguardDivideBlockRequest(connection, req, token);
                    }
                    case 26:
                    {
                        var req = new _manage_client_limit_run_result_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return ManageClientLimitRunResult(connection, req, token);
                    }
                    case 28:
                    {
                        var req = new _apex_block_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return ApexBlockRequest(connection, req, token);
                    }
                    case 31:
                        return CashDbSettingRequest(connection, token);
                    default:
                        return Task.FromResult(false);
                }
            case 50:
                switch (packet.SubCode)
                {
                    case 2:
                    {
                        var req = new _world_service_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return WorldServiceReport(connection, req, token);
                    }
                    case 100:
                    {
                        var req = new _user_num_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return UserNumReport(connection, req, token);
                    }
                    case 101:
                    {
                        var req = new _holy_quest_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return HolyQuestReport(connection, req, token);
                    }
                    case 102:
                    {
                        var req = new _holy_quest_now_report_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return HolyQuestNowReport(connection, req, token);
                    }
                    case 110:
                    {
                        var req = new _user_block_request_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return UserBlockRequest(connection, req, token);
                    }
                    case 25:
                    {
                        var req = new _character_disconnect_result_wrac();
                        if (!req.Load(packet.Payload)) return Task.FromResult(false);
                        return DisconnectGuildWarCharacterResult(connection, req, token);
                    }
                    default:
                        return Task.FromResult(false);
                }
            case 20:
                if (packet.SubCode == 1)
                {
                    var req = new _world_account_ping_wrac();
                    if (!req.Load(packet.Payload)) return Task.FromResult(false);
                    return WorldAccountPing(connection, req, token);
                }
                return Task.FromResult(false);
            default:
                return Task.FromResult(false);
        }
    }

    private async Task<bool> OpenWorldRequest(PublicConnection connection, _open_world_request_wrac request, CancellationToken token)
    {
        string name = PacketStringUtil.ToAsciiNullTerm(request.szWorldName);
        string worldIp = new IPAddress(request.dwWorldServerIP).ToString();
        _log($"WorldServer Start Name = {name}, IP = {worldIp}");

        if (!_context.TryFindWorldByName(name, out var worldIndex, out var worldEntry))
        {
            await SendOpenWorldFailureAsync(connection, "Not RegedWorld", token).ConfigureAwait(false);
            connection.Close();
            return true;
        }

        if (_context.TryGetWorldSession(worldIndex, out _))
        {
            await SendOpenWorldFailureAsync(connection, "Not RegedWorld", token).ConfigureAwait(false);
            connection.Close();
            return true;
        }

        _context.RegisterWorld(worldIndex, connection);

        var send = new _open_world_success_acwr
        {
            byWorldCode = (byte)worldIndex,
            m_byWorldType = (byte)worldEntry.Type
        };
        PacketStringUtil.FillFixed(send.szDBName, worldEntry.DbName);
        PacketStringUtil.FillFixed(send.szDBIP, ExtractHost(worldEntry.Address));

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 2,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
            _log($"{name} : World Open Success");
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] open world send failed: {ex.Message}");
        }
        return true;
    }

    private async Task SendOpenWorldFailureAsync(PublicConnection connection, string reason, CancellationToken token)
    {
        var send = new _open_world_failure_acwr();
        PacketStringUtil.FillFixed(send.szErrCode, reason);

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 22,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] open world failure send failed: {ex.Message}");
        }
    }

    private async Task<bool> StartWorldRequest(PublicConnection connection, _start_world_request_wrac request, CancellationToken token)
    {
        if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession))
        {
            _log($"CMainThread::pc_StartWorld() : GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
            return true;
        }

        int worldCode = worldSession.WorldCode;
        if (!_context.TryGetWorld(worldCode, out var worldEntry))
        {
            _log($"CMainThread::pc_StartWorld() : GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
            return true;
        }

        if (worldEntry.IsService)
        {
            _log($"CMainThread::pc_StartWorld({worldEntry.Name}) : Already Service Open");
            return true;
        }

        _context.UpdateWorldGate(worldCode, request.dwGateIP, request.wGatePort);
        _context.UpdateWorldService(worldCode, true);

        var send = new _inform_open_world_aclo
        {
            dwWorldCode = (uint)worldCode,
            dwGateIP = request.dwGateIP,
            wGatePort = request.wGatePort
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 100,
            Payload = send.ToArray()
        };

        await SendToLoginServersAsync(env, token).ConfigureAwait(false);
        _log($"{DateTime.Now:HH:mm:ss}: Start World: Code ({worldCode}), Name ({worldEntry.Name})");
        return true;
    }

    private async Task<bool> StopWorldRequest(PublicConnection connection, _stop_world_request_wrac request, CancellationToken token)
    {
        if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession))
        {
            _log($"CMainThread::pc_CloseWorld() : GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
            return true;
        }

        int worldCode = worldSession.WorldCode;
        if (!_context.TryGetWorld(worldCode, out var worldEntry))
        {
            _log($"CMainThread::pc_CloseWorld() : GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
            return true;
        }

        if (!worldEntry.IsService)
        {
            _log($"CMainThread::pc_StartWorld({worldEntry.Name}) : Already Service Not Open");
            return true;
        }

        _context.UpdateWorldService(worldCode, false);

        var send = new _inform_close_world_aclo
        {
            dwWorldCode = (uint)worldCode
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 101,
            Payload = send.ToArray()
        };

        await SendToLoginServersAsync(env, token).ConfigureAwait(false);
        _log($"{DateTime.Now:HH:mm:ss}: Stop World: Code ({worldCode}), Name ({worldEntry.Name})");
        return true;
    }

    private Task<bool> CloseAccountFromWorldRequest(PublicConnection connection, _logout_account_request_wrac request, CancellationToken token)
    {
        if (request.gidGlobal.dwIndex >= 100000)
        {
            _log($"CNetworkEX::CloseAccountFromWorldRequest() : G_id Error[{request.gidGlobal.dwIndex}]");
            return Task.FromResult(false);
        }

        PcCloseAccount(connection.ConnectionId, request.gidGlobal, 1);
        return Task.FromResult(true);
    }

    private async Task<bool> EnterWorldRequest(PublicConnection connection, _enter_world_request_wrac request, CancellationToken token)
    {
        if (request.gidGlobal.dwIndex >= 100000)
        {
            _log($"CNetworkEX::EnterWorldRequest() : G_id Error[{request.gidGlobal.dwIndex}]");
            return false;
        }

        byte retCode = 0;
        WorldServerSession? worldSession = null;
        if (!_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var user) || !user.IsLogin)
        {
            retCode = 3;
        }
        else if (user.GlobalId.dwSerial != request.gidGlobal.dwSerial)
        {
            _log("CMainThread::pc_EnterWorldRequest() : G_id Different");
            retCode = 4;
        }
        else if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out worldSession))
        {
            _log($"CMainThread::pc_EnterWorldRequest() : GetWorldPtrFormSocketIndex({connection.ConnectionId}) == false");
            retCode = 100;
        }

        if (retCode != 0)
        {
            var send = new _enter_world_result_acwr
            {
                idLocal = request.idLocal,
                byRetCode = retCode
            };

            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 11,
                Payload = send.ToArray()
            };

            try
            {
                await connection.SendAsync(env, token).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _log($"[{connection.ConnectionId}] enter world result send failed: {ex.Message}");
            }
            return true;
        }

        if (worldSession != null)
        {
            user.WorldCode = worldSession.WorldCode;
        }
        return true;
    }

    private async Task<bool> TransAccountReport(PublicConnection connection, _trans_account_report_wrac request, CancellationToken token)
    {
        if (request.gidGlobal.dwIndex >= 100000)
        {
            _log($"CNetworkEX::TransAccountReport() : G_id Error[{request.gidGlobal.dwIndex}]");
            return false;
        }

        if (!_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var user) || !user.IsLogin)
        {
            _log("CMainThread::pc_TransAccountReport() : Already Logoff");
            return true;
        }

        if (user.GlobalId.dwSerial != request.gidGlobal.dwSerial)
        {
            _log("CMainThread::pc_TransAccountReport() : G_id Different");
            return true;
        }

        if (request.byRetCode == 0)
        {
            if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession))
            {
                _log($"CMainThread::pc_TransAccountReport().. GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
                return true;
            }
            user.WorldCode = worldSession.WorldCode;
        }

        var send = new _select_world_result_aclo
        {
            idLocal = user.Clid,
            byRetCode = request.byRetCode
        };

        for (int i = 0; i < send.dwWorldMasterKey.Length; i++)
        {
            send.dwWorldMasterKey[i] = i < user.MasterKey.Length ? user.MasterKey[i] : 0;
        }

        if (!_context.TryGetLoginConnection(user.ConnectionId, out var loginConnection))
        {
            _log($"[{user.ConnectionId}] login connection not found for select world result.");
            return true;
        }

        await SendSelectWorldResultAsync(loginConnection, send, token).ConfigureAwait(false);
        return true;
    }

    private Task<bool> SelectAvatarReport(PublicConnection connection, _select_avator_report_wrac request, CancellationToken token)
    {
        if (_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var user) && user.IsLogin)
        {
            if (user.GlobalId.dwSerial != request.gidGlobal.dwSerial)
            {
                return Task.FromResult(true);
            }

            Array.Clear(user.AvatarName, 0, user.AvatarName.Length);
            if (request.wszCharName != null)
            {
                Buffer.BlockCopy(request.wszCharName, 0, user.AvatarName, 0, Math.Min(user.AvatarName.Length, request.wszCharName.Length));
            }
            user.AvatarSerial = request.dwAvatorSerial;
            user.Level = request.byLevel;
            user.IsSelectChar = true;
        }
        return Task.FromResult(true);
    }

    private Task<bool> EnterLobbyReport(PublicConnection connection, _enter_lobby_report_wrac request, CancellationToken token)
    {
        if (_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var user) && user.IsLogin)
        {
            if (user.GlobalId.dwSerial != request.gidGlobal.dwSerial)
            {
                return Task.FromResult(true);
            }

            Array.Clear(user.AvatarName, 0, user.AvatarName.Length);
            user.AvatarSerial = uint.MaxValue;
            user.Level = 0;
            user.IsSelectChar = false;
        }
        return Task.FromResult(true);
    }

    private async Task<bool> FireguardBlockRequest(PublicConnection connection, _fireguard_block_request_wrac request, CancellationToken token)
    {
        string id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"FireguardBlockRequest id='{id}' serial={request.dwAccountSerial} ip={new IPAddress(request.dwIP)}");

        var (ret, err, res) = await _db.Check_Fireguard_BlockAsync(id, token).ConfigureAwait(false);
        if (ret != 0)
        {
            _log($"Check_Fireguard_Block failed id='{id}' ret={ret}");
        }
        else
        {
            _log($"Check_Fireguard_Block id='{id}' err={err} ret={res}");
        }
        return true;
    }

    private async Task<bool> UiLockInitRequest(PublicConnection connection, _uilock_init_request_wrac request, CancellationToken token)
    {
        string pw = PacketStringUtil.ToAsciiNullTerm(request.uszUILockPW);
        string answer = PacketStringUtil.ToAsciiNullTerm(request.uszHintAnswer);
        var (dbRet, result) = await _db.UILock_InitAsync(
            request.dwAccountSerial,
            pw,
            request.byHintIndex,
            answer,
            token).ConfigureAwait(false);

        var send = new _uilock_init_result_acwr
        {
            byRet = dbRet == 0 ? result : (byte)24
        };

        if (send.byRet == 0)
        {
            send.wUserIndex = request.wUserIndex;
            send.byHintIndex = request.byHintIndex;
            Buffer.BlockCopy(request.uszUILockPW, 0, send.uszUILockPW, 0, Math.Min(send.uszUILockPW.Length, request.uszUILockPW.Length));
            Buffer.BlockCopy(request.uszHintAnswer, 0, send.uszHintAnswer, 0, Math.Min(send.uszHintAnswer.Length, request.uszHintAnswer.Length));
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 16,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] uilock init send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> UiLockUpdateRequest(PublicConnection connection, _uilock_update_request_wrac request, CancellationToken token)
    {
        string pw = PacketStringUtil.ToAsciiNullTerm(request.uszUILockPW);
        string answer = PacketStringUtil.ToAsciiNullTerm(request.uszHintAnswer);
        var (dbRet, result) = await _db.UILock_UpdateAsync(
            request.dwAccountSerial,
            pw,
            request.byHintIndex,
            answer,
            token).ConfigureAwait(false);

        var send = new _uilock_update_result_acwr
        {
            byRet = dbRet == 0 ? result : (byte)24
        };

        if (send.byRet == 0)
        {
            send.wUserIndex = request.wUserIndex;
            send.byHintIndex = request.byHintIndex;
            Buffer.BlockCopy(request.uszUILockPW, 0, send.uszUILockPW, 0, Math.Min(send.uszUILockPW.Length, request.uszUILockPW.Length));
            Buffer.BlockCopy(request.uszHintAnswer, 0, send.uszHintAnswer, 0, Math.Min(send.uszHintAnswer.Length, request.uszHintAnswer.Length));
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 18,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] uilock update send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> UiLockRefreshRequest(PublicConnection connection, _uilock_user_refresh_info_request_wrac request, CancellationToken token)
    {
        byte retCode = 0;
        bool blocked = false;

        byte failCount = request.byFailCnt;
        byte findPassFailCount = request.byFindPassFailCount;

        if (failCount < 5)
        {
            if (findPassFailCount >= 5)
            {
                findPassFailCount = 0;
                string writer = $"WS{connection.ConnectionId}";
                byte blockResult = await UpdateUserBanAsync(
                    request.dwAccountSerial,
                    0,
                    0x18u,
                    UiLockFindPassReason,
                    writer,
                    5,
                    token).ConfigureAwait(false);
                if (blockResult != 0)
                {
                    blocked = true;
                }
                else
                {
                    retCode = 5;
                }
            }
        }
        else
        {
            failCount = 0;
            string writer = $"WS{connection.ConnectionId}";
            byte blockResult = await UpdateUserBanAsync(
                request.dwAccountSerial,
                0,
                0x18u,
                UiLockFailReason,
                writer,
                4,
                token).ConfigureAwait(false);
            if (blockResult != 0)
            {
                blocked = true;
            }
            else
            {
                retCode = 4;
            }
        }

        var (dbRet, result) = await _db.UILock_RefreshAsync(
            request.dwAccountSerial,
            failCount,
            findPassFailCount,
            token).ConfigureAwait(false);

        var send = new _uilock_user_refresh_info_result_acwr
        {
            dwAccountSerial = request.dwAccountSerial,
            byResult = dbRet == 0 ? (result != 0 ? result : retCode) : (byte)24
        };

        if (blocked)
        {
            if (_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var account) && account.IsLogin)
            {
                _log($"UILock refresh blocked account {account.AccountId}");
            }
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 20,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] uilock refresh send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> FireguardDivideBlockRequest(PublicConnection connection, _fireguard_divide_block_request_wrac request, CancellationToken token)
    {
        string id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"FireguardDivideBlockRequest id='{id}' serial={request.dwAccountSerial} ip={new IPAddress(request.dwIP)}");

        var (ret, accountCount, ipCount) = await _db.Fireguard_Block_Type1Async(id, request.dwIP, token).ConfigureAwait(false);
        if (ret != 0)
        {
            _log($"Fireguard_Block_Type1 failed id='{id}' ret={ret}");
            return true;
        }

        if (accountCount >= 10)
        {
            var ban = await _db.Select_UserBanAsync(request.dwAccountSerial, token).ConfigureAwait(false);
            if (ban.Ret != 0)
            {
                bool inserted = await _db.Insert_UserBanAsync(
                    request.dwAccountSerial,
                    0,
                    0x3E7u,
                    FireguardReason,
                    FireguardWriter,
                    3,
                    token).ConfigureAwait(false);
                if (inserted)
                {
                    _ = _db.Insert_UserBan_LogAsync(request.dwAccountSerial, 0, 0x3E7u, FireguardReason, token);
                }
                else
                {
                    _log($"Insert_UserBan failed for serial={request.dwAccountSerial}");
                }
            }
            else
            {
                bool updated = await _db.Update_UserBanAsync(
                    request.dwAccountSerial,
                    0,
                    0x3E7u,
                    FireguardReason,
                    token).ConfigureAwait(false);
                if (!updated)
                {
                    _log($"Update_UserBan failed for serial={request.dwAccountSerial}");
                }
                else
                {
                    _ = _db.Insert_UserBan_LogAsync(request.dwAccountSerial, 0, 0x3E7u, FireguardReason, token);
                }
            }
        }

        if (ipCount >= 20)
        {
            byte ipRet = await _db.IsBlockIPAsync(request.dwIP, token).ConfigureAwait(false);
            if (ipRet != 0)
            {
                bool inserted = await _db.Insert_BlockIPAsync(request.dwIP, token).ConfigureAwait(false);
                if (!inserted)
                {
                    _log($"Insert_BlockIP failed ip={new IPAddress(request.dwIP)}");
                }
            }
        }

        return true;
    }

    private async Task<bool> ManageClientLimitRunResult(PublicConnection connection, _manage_client_limit_run_result_wrac request, CancellationToken token)
    {
        var send = new _manage_client_limit_run_world_result_aclo
        {
            idLocal = request.idLocal,
            byRet = request.byRet
        };

        if (_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession)
            && _context.TryGetWorld(worldSession.WorldCode, out var world))
        {
            send.m_dwCode = (uint)worldSession.WorldCode;
            send.m_byType = (byte)world.Type;
            PacketStringUtil.FillFixed(send.m_szName, world.Name);
            PacketStringUtil.FillFixed(send.m_szDBName, world.DbName);
        }
        else
        {
            send.m_dwCode = uint.MaxValue;
            send.m_byType = 0xFF;
            PacketStringUtil.FillFixed(send.m_szName, "invalid");
            PacketStringUtil.FillFixed(send.m_szDBName, "invalid");
        }

        ulong loginIndex = request.byLoginServerIndex;
        if (!_context.TryGetLoginConnection(loginIndex, out var loginConnection))
        {
            _log($"[{loginIndex}] login connection not found for limit run result.");
            return true;
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 24,
            Payload = send.ToArray()
        };

        try
        {
            await loginConnection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{loginConnection.ConnectionId}] manage limit run world result send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> ApexBlockRequest(PublicConnection connection, _apex_block_request_wrac request, CancellationToken token)
    {
        string id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"ApexBlockRequest id='{id}' serial={request.dwAccountSerial} ip={new IPAddress(request.dwIP)}");

        bool ok = await _db.Insert_UserBan_ApexAsync(request.dwAccountSerial, token).ConfigureAwait(false);
        if (!ok)
        {
            _log($"Insert_UserBan_Apex failed serial={request.dwAccountSerial}");
        }
        return true;
    }

    private Task<bool> WorldServiceReport(PublicConnection connection, _world_service_report_wrac request, CancellationToken token)
    {
        if (_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession)
            && _context.TryGetWorld(worldSession.WorldCode, out var world))
        {
            _log($"{worldSession.WorldCode}..{world.Name}: WorldOperReport : before : {(request.bBefore ? 1 : 0)} -> after: {(request.bAfter ? 1 : 0)}");
        }
        else
        {
            _log($"CMainThread::pc_WorldOperReport().. GetWorldPtrFormSocketIndex({connection.ConnectionId}) == NULL");
        }
        return Task.FromResult(true);
    }

    private async Task<bool> UserNumReport(PublicConnection connection, _user_num_report_wrac request, CancellationToken token)
    {
        if (_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession)
            && _context.TryGetWorld(worldSession.WorldCode, out var world))
        {
            string date = PacketStringUtil.ToAsciiNullTerm(request.szLogDate);
            _log($"UserNumReport {world.Name} avg={request.dwAveragePerHour} max={request.dwMaxPerHour} date={date}");

            uint[] playerPerRace = request.dwPlayerPerRace ?? new uint[3];
            bool ok = await _db.Insert_ServerUserLogAsync(
                request.dwAveragePerHour,
                request.dwMaxPerHour,
                playerPerRace,
                world.Name,
                date,
                token).ConfigureAwait(false);
            if (!ok)
            {
                _log($"Insert_ServerUserLog failed for {world.Name} date={date}");
            }
        }
        return true;
    }

    private Task<bool> HolyQuestReport(PublicConnection connection, _holy_quest_report_wrac request, CancellationToken token)
    {
        return Task.FromResult(true);
    }

    private async Task<bool> HolyQuestNowReport(PublicConnection connection, _holy_quest_now_report_wrac request, CancellationToken token)
    {
        if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession))
        {
            return true;
        }

        var send = new _holy_quest_now_report_aclo
        {
            wWorldCode = (ushort)worldSession.WorldCode,
            byMasterRaceCode = request.byMasterRaceCode
        };

        if (!_context.TryGetLoginConnection(0, out var loginConnection))
        {
            return true;
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 14,
            Payload = send.ToArray()
        };

        try
        {
            await loginConnection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{loginConnection.ConnectionId}] holy quest now send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> UserBlockRequest(PublicConnection connection, _user_block_request_wrac request, CancellationToken token)
    {
        string reason = PacketStringUtil.ToAsciiNullTerm(request.uszReason);
        string writer = PacketStringUtil.ToAsciiNullTerm(request.uszWriter);

        byte blockResult = await UpdateUserBanAsync(
            request.dwAccountSerial,
            request.byBlockType,
            unchecked((uint)request.iPeriod),
            reason,
            writer,
            6,
            token).ConfigureAwait(false);

        var send = new _user_block_result_acwr
        {
            byBlockResult = blockResult,
            idLocalForGM = request.idLocalForGM,
            dwAccountSerial = request.dwAccountSerial
        };

        if (_context.TryGetActiveGlobal(request.gidGlobalForTarget.dwIndex, out var target) && target.IsLogin)
        {
            send.idLocalForTarget = target.Clid;
            send.bLogin = 1;
        }
        else
        {
            send.bLogin = 0;
        }

        var env = new PacketEnvelope
        {
            OpCode = 50,
            SubCode = 111,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] user block result send failed: {ex.Message}");
        }
        return true;
    }

    private Task<bool> DisconnectGuildWarCharacterResult(PublicConnection connection, _character_disconnect_result_wrac request, CancellationToken token)
    {
        _log($"DisconnectGuildWarCharacterResult clientIndex={request.wClientIndex} result={request.byResult}");
        return Task.FromResult(true);
    }

    private Task<bool> WorldAccountPing(PublicConnection connection, _world_account_ping_wrac request, CancellationToken token)
    {
        if (_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var session))
        {
            session.UpdatePing(unchecked((uint)Environment.TickCount));
        }
        return Task.FromResult(true);
    }

    private async Task<byte> UpdateUserBanAsync(
        uint accountSerial,
        byte blockType,
        uint period,
        string reason,
        string writer,
        byte reasonType,
        CancellationToken token)
    {
        var ban = await _db.Select_UserBanAsync(accountSerial, token).ConfigureAwait(false);
        if (ban.Ret != 0)
        {
            bool inserted = await _db.Insert_UserBanAsync(
                accountSerial,
                blockType,
                period,
                reason,
                writer,
                reasonType,
                token).ConfigureAwait(false);
            if (inserted)
            {
                _ = _db.Insert_UserBan_LogAsync(accountSerial, blockType, period, reason, token);
                return 1;
            }

            _log($"Insert_UserBan failed serial={accountSerial}");
            return 0;
        }

        if (ban.Kind == blockType)
        {
            return unchecked((byte)0xFF);
        }

        return unchecked((byte)0xFE);
    }

    private async Task<bool> CashDbSettingRequest(PublicConnection connection, CancellationToken token)
    {
        var send = new _cashdb_setting_result_acwr();
        PacketStringUtil.FillFixed(send.szIP, _settings.Database.User.Host);
        PacketStringUtil.FillFixed(send.szDBName, _settings.Database.User.Database);
        PacketStringUtil.FillFixed(send.szDSN, _settings.Database.User.Database);
        PacketStringUtil.FillFixed(send.szAccount, _settings.Database.User.User);
        PacketStringUtil.FillFixed(send.szPassword, _settings.Database.User.Password);
        send.dwPort = (uint)_settings.Database.User.Port;

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 32,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] cash db setting send failed: {ex.Message}");
        }
        return true;
    }
}
