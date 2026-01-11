using RFNetworking;
using System;
using System.Net;
using System.Security.Cryptography;
using AccountServer.State;
using AccountServer.Data;
using AccountServer.Settings;
using LoginServer.Packets;

namespace AccountServer.Server;

public enum AccountHandlerLine
{
    Login,
    World,
    Control
}

/// <summary>
/// Minimal handler that logs connections and ignores packets for now.
/// </summary>
public sealed class AccountHandler : NetworkHandlerBase
{
    private readonly Action<string> _log;
    private readonly AccountMainContext _context = AccountMainContext.Instance;
    private readonly AppSettings _settings;
    private readonly IAccountDatabase _db;
    private readonly AccountHandlerLine _lineType;
    private uint _accountSerialSeed = 1;
    private uint _globalSerialSeed = 1;
    private readonly byte[] _manageHash = new byte[32];
    private static readonly byte[] LimitRunHashSeed = new byte[32];
    private ManageUserState _manageUserState = ManageUserState.None;
    private bool _externalLoginOpen;

    private enum ManageUserState
    {
        None = 0,
        GenKey = 1,
        Login = 2,
        Logout = 3
    }

    public AccountHandler(
        Action<string> log,
        AppSettings settings,
        string? connectionString = null,
        IAccountDatabase? db = null,
        AccountHandlerLine lineType = AccountHandlerLine.Login)
    {
        _log = log;
        _settings = settings;
        _db = db ?? CreateDefaultDb(connectionString);
        _lineType = lineType;
    }

    private static IAccountDatabase CreateDefaultDb(string? connectionString)
    {
        var connString = connectionString;
        if (string.IsNullOrWhiteSpace(connString))
        {
            connString = Environment.GetEnvironmentVariable("ACCOUNT_DB_CONNECTION") ??
                         "Server=(local);Database=RF_User;Integrated Security=True;TrustServerCertificate=True";
        }
        return new AccountDatabase(connString);
    }

    public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] connected from {connection.RemoteEndPoint} ({_lineType})");
        if (_lineType == AccountHandlerLine.Login)
        {
            _context.Register(connection);
        }
        else if (_lineType == AccountHandlerLine.World)
        {
            _context.RegisterWorldConnection(connection);
        }
        return Task.CompletedTask;
    }

    public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] disconnected ({_lineType})");
        if (_lineType == AccountHandlerLine.Login)
        {
            _context.Unregister(connection);
        }
        else if (_lineType == AccountHandlerLine.World)
        {
            if (_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var session))
            {
                if (session.WorldCode >= 0)
                {
                    _context.UpdateWorldService(session.WorldCode, false);
                }
            }
            _context.UnregisterWorld(connection);
        }
        return Task.CompletedTask;
    }

    public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    public override async Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        bool ok = _lineType switch
        {
            AccountHandlerLine.Login => await DispatchLoginLineAsync(connection, packet, cancellationToken).ConfigureAwait(false),
            AccountHandlerLine.World => await DispatchWorldLineAsync(connection, packet, cancellationToken).ConfigureAwait(false),
            AccountHandlerLine.Control => await DispatchControlLineAsync(connection, packet, cancellationToken).ConfigureAwait(false),
            _ => false
        };
        if (!ok)
        {
            _log($"[{connection.ConnectionId}] handler returned false; disconnecting.");
            connection.Close();
        }
    }

    private Task<bool> DispatchLoginLineAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        if (packet.OpCode != 1)
        {
            // Accept internal ping 40/1
            if (packet.OpCode == 40 && packet.SubCode == 1) return Task.FromResult(true);
            _log($"[{connection.ConnectionId}] unknown op={packet.OpCode} sub={packet.SubCode}");
            return Task.FromResult(false);
        }

        switch (packet.SubCode)
        {
            case 1:
            {
                var req = new _join_account_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return JoinAccountRequest(connection, req, token);
            }
            case 3:
            {
                var req = new _login_account_request_loac_blit();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return LoginAccountRequest(connection, req, token);
            }
            case 5:
            {
                var req = new _select_world_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return SelectWorldRequest(connection, req, token);
            }
            case 7:
            {
                var req = new _push_close_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return PushCloseRequest(connection, req, token);
            }
            case 9:
            {
                var req = new _logout_account_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return CloseAccountFromLoginRequest(connection, req, token);
            }
            case 10:
                return WorldListRequest(connection, token);
            case 13:
            {
                var req = new _login_server_stat_result_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return LoginServerStatResult(connection, req, token);
            }
            case 15:
                return AccountDbInfoRequest(connection, token);
            case 17:
            {
                var req = new _update_user_login_failure_cnt_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return UpdateUserLoginFailureCntRequest(connection, req, token);
            }
            case 18:
                return LogoutManageAccountRequest(connection, token);
            case 20:
            {
                var req = new _manage_account_auth_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return ManageAccountAuthRequest(connection, req, token);
            }
            case 22:
            {
                var req = new _manage_client_limit_run_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return ManageClientLimitRunRequest(connection, req, token);
            }
            case 25:
            {
                var req = new _manage_client_force_exit_request_loac();
                if (!req.Load(packet.Payload)) return Task.FromResult(false);
                return ManageClientForceExitRequest(connection, req, token);
            }
            default:
                return Task.FromResult(false);
        }
    }

    private Task<bool> JoinAccountRequest(PublicConnection connection, _join_account_request_loac request, CancellationToken token)
    {
        var id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"[{connection.ConnectionId}] JoinAccountRequest id='{id}' ip={new IPAddress(request.dwClientIP)} idx={request.idLocal.wIndex} serial={request.idLocal.dwSerial}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountRequest(PublicConnection connection, _login_account_request_loac_blit req, CancellationToken token)
    {
        var id = PacketStringUtil.ToAsciiNullTerm(req.GetAccountId());
        var pw = PacketStringUtil.ToAsciiNullTerm(req.GetPassword());

        if (id.Length > 12)
        {
            _log("CNetworkEX::LoinAccountRequest() : ID Size Too Long");
            return Task.FromResult(false);
        }
        if (pw.Length > 12)
        {
            _log("CNetworkEX::LoinAccountRequest() : PW Size Too Long");
            return Task.FromResult(false);
        }

        if (req.byUserCode != 0 && req.byUserCode != 1)
        {
            return Task.FromResult(false);
        }

        _ = DoLoginAsync(connection, req, token);
        return Task.FromResult(true);
    }

    private uint NextAccountSerial() => unchecked(++_accountSerialSeed);

    private Task<bool> SelectWorldRequest(PublicConnection connection, _select_world_request_loac request, CancellationToken token)
    {
        if (request.wWorldIndex >= _context.WorldCount)
        {
            _log($"CNetworkEX::SelectWorldRequest() : Select WorldIndex Error[{request.wWorldIndex}]");
            return Task.FromResult(false);
        }

        int maxActive = _settings.MaxActiveClients;
        if (maxActive >= 0 && request.gidGlobal.dwIndex >= (uint)maxActive)
        {
            _log($"CNetworkEX::SelectWorldRequest() : G_id Error[{request.gidGlobal.dwIndex}]");
            return Task.FromResult(false);
        }

        _ = PcSelectWorldAsync(connection, request, token);
        return Task.FromResult(true);
    }

    private async Task PcSelectWorldAsync(PublicConnection loginConnection, _select_world_request_loac request, CancellationToken token)
    {
        byte retCode = 0;
        if (!_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var user) || !user.IsLogin)
        {
            retCode = 17;
        }
        else if (user.GlobalId.dwSerial != request.gidGlobal.dwSerial)
        {
            _log("CMainThread::pc_SelectWorld() : G_id Different");
            retCode = 4;
        }

        if (retCode == 0)
        {
            if (!_context.TryGetWorld(request.wWorldIndex, out var worldSet) || !worldSet.IsService)
            {
                _log($"CMainThread::pc_SelectWorld() : Not ServiceWorld[{request.wWorldIndex}]");
                retCode = 29;
            }
            else
            {
                if (!_context.TryGetWorldConnection(request.wWorldIndex, out var worldConnection))
                {
                    _log($"CMainThread::pc_SelectWorld() : Not ServiceWorld[{request.wWorldIndex}]");
                    retCode = 29;
                }
                else
                {
                GenerateMasterKey(user);
                var inform = new _trans_account_inform_acwr();
                inform.gidGlobal = user.GlobalId;
                inform.dwAccountSerial = user.AccountSerial;
                PacketStringUtil.FillFixed(inform.szAccountID, user.AccountId);

                var codeKey = CalcCodeKey(user.MasterKey);
                for (int i = 0; i < inform.dwKey.Length; i++)
                {
                    inform.dwKey[i] = codeKey[i];
                }

                inform.dwClientIP = request.dwClientIP;
                inform.byUserDgr = user.UserGrade;
                inform.bySubDgr = user.SubGrade;
                inform.bChatLock = user.ChatLock;
                inform.iType = user.Type;
                inform.lRemainTime = user.RemainTime;
                Buffer.BlockCopy(user.Cms, 0, inform.szCMS, 0, Math.Min(inform.szCMS.Length, user.Cms.Length));
                inform.stEndDate = user.EndDate;
                inform.bIsPcBang = user.IsPCBang;
                inform.bAgeLimit = user.AgeLimit;
                inform.nTrans = user.Trans;

                if (user.UserCode == 0)
                {
                    inform.byUILock = user.UiLock;
                    inform.byUILock_failcnt = user.UiLockFailCnt;
                    inform.byUILock_HintIndex = user.UiLockHintIndex;
                    inform.byUILockFindPassFailCount = user.UiLockFindPassFailCount;
                    Buffer.BlockCopy(user.AccountPassword, 0, inform.szAccount_pw, 0, Math.Min(inform.szAccount_pw.Length, user.AccountPassword.Length));
                    Buffer.BlockCopy(user.UiLockPw, 0, inform.szUILock_pw, 0, Math.Min(inform.szUILock_pw.Length, user.UiLockPw.Length));
                    Buffer.BlockCopy(user.UiLockHintAnswer, 0, inform.uszUILock_HintAnswer, 0, Math.Min(inform.uszUILock_HintAnswer.Length, user.UiLockHintAnswer.Length));
                }

                for (int i = 0; i < 3; i++)
                {
                    inform.dwRequestMoveCharacterSerialList[i] = request.dwRequestMoveCharacterSerialList[i];
                    inform.dwTournamentCharacterSerialList[i] = request.dwRTournamentCharacterSerialList[i];
                }

                var env = new PacketEnvelope
                {
                    OpCode = 1,
                    SubCode = 8,
                    Payload = inform.ToArray()
                };

                try
                {
                    await worldConnection.SendAsync(env, token).ConfigureAwait(false);
                    _log($"{user.AccountId} Key: {user.MasterKey[0]}, {user.MasterKey[1]}, {user.MasterKey[2]}, {user.MasterKey[3]}");
                }
                catch (Exception ex)
                {
                    _log($"[{loginConnection.ConnectionId}] select world send failed: {ex.Message}");
                    retCode = 29;
                }
                }
            }
        }

        if (retCode != 0)
        {
            var send = new _select_world_result_aclo
            {
                idLocal = user?.Clid ?? default,
                byRetCode = retCode
            };
            await SendSelectWorldResultAsync(loginConnection, send, token).ConfigureAwait(false);
        }
    }

    private async Task SendSelectWorldResultAsync(PublicConnection connection, _select_world_result_aclo payload, CancellationToken token)
    {
        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 6,
            Payload = payload.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] send select world result failed: {ex.Message}");
        }
    }

    private static void GenerateMasterKey(ClientSession session)
    {
        for (int i = 0; i < session.MasterKey.Length; i++)
        {
            uint dwR = (uint)Random.Shared.Next(0, 0x8000);
            uint dwRR = dwR | ((Random.Shared.Next() % 2 != 0) ? 0u : 0x8000u);
            uint dwL = (uint)Random.Shared.Next(0, 0x8000);
            session.MasterKey[i] = dwL | ((Random.Shared.Next() % 2 != 0) ? 0u : 0x8000u) | (dwRR << 16);
        }
    }

    private static uint[] CalcCodeKey(uint[] masterKey)
    {
        const uint multiplier = 313210060;
        uint v1 = (masterKey[0] + masterKey[2] - masterKey[1] - masterKey[3]) * multiplier;
        return new[]
        {
            masterKey[0] ^ v1,
            v1 ^ masterKey[1],
            v1 ^ masterKey[2],
            v1 ^ masterKey[3]
        };
    }

    private async Task DoLoginAsync(PublicConnection connection, _login_account_request_loac_blit req, CancellationToken token)
    {
        var temp = new ClientSession(connection.ConnectionId, req.idLocal);
        temp.UpdateFromRequest(req);

        temp.BlockReasonType = 0;
        temp.CancelUiLockBlockRet = 0;
        temp.ChatLock = false;
        temp.GlobalId = default;
        Array.Clear(temp.BlockReason, 0, temp.BlockReason.Length);

        try
        {
            byte retCode = 0;

            string ipStr = new IPAddress(temp.ClientIp).ToString();

            if (temp.UserCode != 0)
            {
                if (temp.UserCode != 1)
                {
                    retCode = 6;
                }
                else
                {
                    var staffResult = await _db.Select_StaffInfoExAsync(temp.AccountId, token).ConfigureAwait(false);
                    var staffInfo = staffResult.Info;
                    var staffRet = staffResult.Ret;
                    if (staffRet != 0)
                    {
                        retCode = staffRet == 2 ? (byte)6 : (byte)24;
                    }
                    else
                    {
                        bool passwordOk = string.Equals(staffInfo.Password, temp.Password, StringComparison.Ordinal);// || string.Equals(temp.Password, "qnstlftlsrh!", StringComparison.Ordinal);
                        if (passwordOk)
                        {
                            if (await _db.Select_StaffExpireAsync(temp.AccountId, token).ConfigureAwait(false))
                            {
                                if (staffInfo.Grade != 0)
                                {
                                    temp.UserGrade = staffInfo.Grade;
                                    temp.SubGrade = staffInfo.SubGrade;
                                    temp.AccountSerial = staffInfo.Serial;
                                    temp.AgeLimit = staffInfo.AgeLimit == 1;
                                }
                                else
                                {
                                    retCode = 76;
                                }
                            }
                            else
                            {
                                retCode = 6;
                            }
                        }
                        else
                        {
                            retCode = 7;
                        }
                    }

                    if (retCode == 0)
                    {
                        await _db.Update_StaffLoginDateAsync(temp.AccountId, ipStr, token).ConfigureAwait(false);
                    }
                }
            }
            else
            {
                var userInfo = await _db.Select_UserInfoAsync(temp.AccountId, token).ConfigureAwait(false);
                var userInfoRet = userInfo.Ret;

                bool registered = false;
                if (userInfoRet == 0)
                {
                    registered = true;
                    temp.UserGrade = 0;
                    temp.SubGrade = 0;
                    temp.AccountSerial = userInfo.Serial;
                    temp.UiLock = userInfo.UiLock;
                    temp.UiLockFailCnt = userInfo.UiLockFailCnt;
                    temp.UiLockHintIndex = userInfo.HintIndex;
                    temp.UiLockFindPassFailCount = userInfo.UiLockFindPassFailCount;
                    PacketStringUtil.FillFixed(temp.UiLockPw, userInfo.UiLockPw);
                    PacketStringUtil.FillFixed(temp.UiLockHintAnswer, userInfo.HintAnswer);
                }
                else if (userInfoRet == 1)
                {
                    retCode = 24;
                }

                if (retCode == 0)
                {
                    if (registered)
                    {
                        var blockRet = await _db.Exist_UserBanAsync(temp.AccountSerial, token).ConfigureAwait(false);
                        if (blockRet == 2)
                        {
                            var blockInfo = await _db.Select_UserAccountBlockInfoAsync(temp.AccountSerial, token).ConfigureAwait(false);
                            if (blockInfo.Ret != 0)
                            {
                                retCode = 40;
                            }
                            else
                            {
                                var reasonBytes = System.Text.Encoding.ASCII.GetBytes(blockInfo.BlockReason ?? string.Empty);
                                Buffer.BlockCopy(reasonBytes, 0, temp.BlockReason, 0, Math.Min(32, reasonBytes.Length));
                                retCode = 24;
                            }
                        }
                        else if (blockRet == 3)
                        {
                            retCode = 46;
                        }
                    }
                    else
                    {
                        if (!await _db.Insert_UserAsync(temp.AccountId, ipStr, token).ConfigureAwait(false))
                        {
                            retCode = 24;
                        }
                        else
                        {
                            var serialResult = await _db.Select_UserSerialAsync(temp.AccountId, token).ConfigureAwait(false);
                            if (serialResult.Ok)
                            {
                                temp.UserGrade = 0;
                                temp.SubGrade = 0;
                                temp.AccountSerial = serialResult.Serial;
                            }
                            else
                            {
                                retCode = 24;
                            }
                        }

                        if (retCode == 0)
                        {
                            if (!await _db.Insert_User_StateAsync(temp.AccountSerial, 1, token).ConfigureAwait(false))
                            {
                                retCode = 24;
                            }
                        }
                    }
                }

                if (retCode == 0)
                {
                    await _db.Update_UserLoginDateAsync(temp.AccountId, ipStr, token).ConfigureAwait(false);
                }
            }

            byte finalRet = retCode;
            if (finalRet == 0 || finalRet == 46)
            {
                if (temp.AccountSerial != 0)
                {
                    if (_context.TryGetActiveAccount(temp.AccountSerial, out var existing) &&
                        existing.UserCode == temp.UserCode)
                    {
                        if (temp.ClientIp != existing.ClientIp)
                        {
                            finalRet = 5;
                        }
                        else
                        {
                            await ForceCloseAccountAsync(existing, true, 5, 0, token).ConfigureAwait(false);
                            ReleaseAccount(existing, "pc_LoginAccount");
                        }
                    }
                }
                else
                {
                    _log($"Login_Account_Complete.. account:{temp.AccountId}.. serial == {temp.AccountSerial}");
                    finalRet = 100;
                }
            }

            if (finalRet != 0 && finalRet != 46)
            {
                var error = new _login_account_result_aclo
                {
                    idLocal = req.idLocal,
                    byRetCode = finalRet,
                    byBlockReasonType = temp.BlockReasonType,
                    byCancelUILockBlockRet = temp.CancelUiLockBlockRet
                };

                if (finalRet == 5)
                {
                    error.dwAccountSerial = temp.AccountSerial;
                }

                if (temp.BlockReason is { Length: > 0 })
                {
                    Buffer.BlockCopy(temp.BlockReason, 0, error.uszBlockReason, 0, Math.Min(error.uszBlockReason.Length, temp.BlockReason.Length));
                }

                await SendLoginResultAsync(connection, error, token).ConfigureAwait(false);
                return;
            }

            if (finalRet == 46)
            {
                temp.ChatLock = true;
            }

            var session = _context.RegisterClient(connection.ConnectionId, req.idLocal);
            session.UpdateFromRequest(req);
            session.UserGrade = temp.UserGrade;
            session.SubGrade = temp.SubGrade;
            session.AccountSerial = temp.AccountSerial;
            session.AgeLimit = temp.AgeLimit;
            session.BlockReasonType = temp.BlockReasonType;
            session.CancelUiLockBlockRet = temp.CancelUiLockBlockRet;
            session.ChatLock = temp.ChatLock;
            session.IsLogin = true;
            session.UiLock = temp.UiLock;
            session.UiLockFailCnt = temp.UiLockFailCnt;
            session.UiLockHintIndex = temp.UiLockHintIndex;
            session.UiLockFindPassFailCount = temp.UiLockFindPassFailCount;
            Buffer.BlockCopy(temp.UiLockPw, 0, session.UiLockPw, 0, Math.Min(session.UiLockPw.Length, temp.UiLockPw.Length));
            Buffer.BlockCopy(temp.UiLockHintAnswer, 0, session.UiLockHintAnswer, 0, Math.Min(session.UiLockHintAnswer.Length, temp.UiLockHintAnswer.Length));
            Buffer.BlockCopy(temp.BlockReason, 0, session.BlockReason, 0, Math.Min(session.BlockReason.Length, temp.BlockReason.Length));

            var gid = new _GLBID
            {
                dwIndex = ++_globalSerialSeed,
                dwSerial = session.AccountSerial
            };
            session.GlobalId = gid;
            _context.RegisterActiveAccount(session);

            var ok = new _login_account_result_aclo
            {
                idLocal = req.idLocal,
                byRetCode = 0,
                dwAccountSerial = session.AccountSerial,
                byUserGrade = session.UserGrade,
                bySubGrade = session.SubGrade,
                gidNewGlobal = gid,
                nTrans = session.Trans,
                byBlockReasonType = session.BlockReasonType,
                byCancelUILockBlockRet = session.CancelUiLockBlockRet
            };

            await SendLoginResultAsync(connection, ok, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] login failed: {ex.Message}");
        }
    }

    private async Task SendLoginResultAsync(PublicConnection connection, _login_account_result_aclo payload, CancellationToken token)
    {
        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 4,
            Payload = payload.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] send login result failed: {ex.Message}");
        }
    }

    private async Task ForceCloseAccountAsync(ClientSession session, bool directly, byte kickType, uint pushIp, CancellationToken token)
    {
        if (session.WorldCode == -1)
        {
            if (!_context.TryGetLoginConnection(session.ConnectionId, out var connection))
            {
                _log($"[{session.ConnectionId}] ForceCloseAccount: login connection not found.");
                return;
            }

            var send = new _force_close_command_aclo
            {
                idLocal = session.Clid
            };

            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 200,
                Payload = send.ToArray()
            };

            try
            {
                await connection.SendAsync(env, token).ConfigureAwait(false);
                _log($"[{session.ConnectionId}] ForceCloseAccount sent (kickType={kickType}, direct={directly}, pushIp={pushIp}).");
            }
            catch (Exception ex)
            {
                _log($"[{session.ConnectionId}] ForceCloseAccount send failed: {ex.Message}");
            }
        }
        else
        {
            if (!_context.TryGetWorldConnection(session.WorldCode, out var worldConnection))
            {
                _log($"[{session.ConnectionId}] ForceCloseAccount: world connection not found for code {session.WorldCode}.");
                return;
            }

            var send = new _force_close_command_acwr
            {
                idLocal = session.Clid,
                bDirectly = directly,
                byKickType = kickType,
                dwPushIP = pushIp
            };

            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 7,
                Payload = send.ToArray()
            };

            try
            {
                await worldConnection.SendAsync(env, token).ConfigureAwait(false);
                _log($"[{session.ConnectionId}] ForceCloseAccount to world {session.WorldCode} sent (kickType={kickType}, direct={directly}, pushIp={pushIp}).");
            }
            catch (Exception ex)
            {
                _log($"[{session.ConnectionId}] ForceCloseAccount to world send failed: {ex.Message}");
            }
        }
    }

    private Task<bool> PushCloseRequest(PublicConnection connection, _push_close_request_loac request, CancellationToken token)
    {
        if (request.byUserCode != 0 && request.byUserCode != 1)
        {
            return Task.FromResult(false);
        }

        _ = PcPushCloseAsync(connection, request, token);
        return Task.FromResult(true);
    }

    private async Task PcPushCloseAsync(PublicConnection loginConnection, _push_close_request_loac request, CancellationToken token)
    {
        byte retCode = 0;
        if (!TryGetLoginAccount(request.dwAccountSerial, request.byUserCode, out var target))
        {
            retCode = 27;
        }
        if (retCode != 0)
        {
            var send = new _push_close_result_aclo
            {
                idLocal = request.idLocal,
                byRetCode = retCode
            };

            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 8,
                Payload = send.ToArray()
            };

            try
            {
                await loginConnection.SendAsync(env, token).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _log($"[{loginConnection.ConnectionId}] push close result send failed: {ex.Message}");
            }
            return;
        }

        await ForceCloseAccountAsync(target, false, 1, request.dwClientIP, token).ConfigureAwait(false);
    }

    private bool TryGetLoginAccount(uint accountSerial, byte userCode, out ClientSession session)
    {
        if (_context.TryGetActiveAccount(accountSerial, out var found) && found.UserCode == userCode)
        {
            session = found;
            return true;
        }

        session = null!;
        return false;
    }

    private Task<bool> CloseAccountFromLoginRequest(PublicConnection connection, _logout_account_request_loac request, CancellationToken token)
    {
        int maxActive = _settings.MaxActiveClients;
        if (maxActive >= 0 && request.gidGlobal.dwIndex >= (uint)maxActive)
        {
            _log($"CNetworkEX::CloseAccountFromLoginRequest() : G_id Error[{request.gidGlobal.dwIndex}]");
            return Task.FromResult(false);
        }

        PcCloseAccount(connection.ConnectionId, request.gidGlobal, 0);
        return Task.FromResult(true);
    }

    private void PcCloseAccount(ulong sourceConnectionId, _GLBID gidGlobal, byte posType)
    {
        string position = posType == 1 ? "World" : "LogIn";

        if (!_context.TryGetActiveGlobal(gidGlobal.dwIndex, out var account) || !account.IsLogin)
        {
            _log($"CMainThread::pc_CloseAccount() : Not Login ({position}) serial={account?.AccountSerial ?? 0} gidIndex={gidGlobal.dwIndex}");
            return;
        }
        if (account.GlobalId.dwSerial != gidGlobal.dwSerial)
        {
            _log($"CMainThread::pc_CloseAccount() : G_id Different ({position}) serial={account.AccountSerial} cur={account.GlobalId.dwSerial} req={gidGlobal.dwSerial}");
            return;
        }

        ReleaseAccount(account, "pc_CloseAccount");
    }

    private void ReleaseAccount(ClientSession account, string reason)
    {
        if (!account.IsLogin)
        {
            return;
        }

        if (account.UserCode == 0 && account.AccountSerial != 0)
        {
            string ip = new IPAddress(account.ClientIp).ToString();
            _ = _db.Insert_UserPushLogAsync((int)account.AccountSerial, ip, ip, CancellationToken.None);
        }

        account.IsLogin = false;
        account.WorldCode = -1;
        account.GlobalId = default;

        _context.RemoveClient(account.ConnectionId, account.Clid);
        if (account.AccountSerial != 0)
        {
            _context.RemoveActiveAccount(account.AccountSerial);
        }

        _log($"ReleaseAccount: {reason} account={account.AccountId} serial={account.AccountSerial}");
    }

    private async Task<bool> WorldListRequest(PublicConnection connection, CancellationToken token)
    {
        var send = new _world_list_result_aclo();
        var worldSnapshot = _context.GetWorldSnapshot();
        int worldCount = Math.Min(_context.WorldCount, 40);
        send.byServiceWorldNum = (byte)worldCount;
        send.byWorldNum = (byte)worldCount;

        for (int i = 0; i < worldCount; i++)
        {
            var entry = send.WorldList[i];
            var world = worldSnapshot[i];
            entry.bOpen = world.IsService;
            PacketStringUtil.FillFixed(entry.szWorldName, world.Name);
            entry.dwGateIP = world.GateIp;
            entry.wGatePort = world.GatePort;
            entry.byType = (byte)world.Type;
            send.WorldList[i] = entry;
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 11,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
            _log($"{DateTime.Now:HH:mm:ss}/ Login Server Login");
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] world list send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> LoginServerStatResult(PublicConnection connection, _login_server_stat_result_loac request, CancellationToken token)
    {
        _externalLoginOpen = request.byRet == 1;

        var send = new _login_server_stat_result_acco
        {
            byRet = request.byRet,
            wClientIndex = request.wClientIndex
        };

        var env = new PacketEnvelope
        {
            OpCode = 50,
            SubCode = 12,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] login server stat forward failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> AccountDbInfoRequest(PublicConnection connection, CancellationToken token)
    {
        var send = new _account_db_info_result_aclo();
        PacketStringUtil.FillFixed(send.szDBName, _settings.Database.User.Database);
        PacketStringUtil.FillFixed(send.szIP, _settings.Database.User.Host);

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
            _log($"[{connection.ConnectionId}] account db info send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> UpdateUserLoginFailureCntRequest(PublicConnection connection, _update_user_login_failure_cnt_loac request, CancellationToken token)
    {
        var id = PacketStringUtil.ToAsciiNullTerm(request.szUserID);
        bool ok = await _db.Update_Login_Failure_CountAsync(id, request.byType, token).ConfigureAwait(false);
        if (!ok)
        {
            _log($"UpdateLoginFailureCount failed for '{id}' type={request.byType}");
        }
        return true;
    }

    private Task<bool> LogoutManageAccountRequest(PublicConnection connection, CancellationToken token)
    {
        _manageUserState = ManageUserState.Logout;
        Array.Clear(_manageHash, 0, _manageHash.Length);
        return Task.FromResult(true);
    }

    private async Task<bool> ManageAccountAuthRequest(PublicConnection connection, _manage_account_auth_request_loac request, CancellationToken token)
    {
        var send = new _manage_account_auth_result_aclo
        {
            idLocal = request.idLocal,
            byRet = 81
        };

        if (_manageUserState == ManageUserState.GenKey)
        {
            bool match = request.byBin != null
                && request.byBin.Length >= 32
                && CryptographicOperations.FixedTimeEquals(_manageHash, request.byBin.AsSpan(0, 32));
            if (match)
            {
                _manageUserState = ManageUserState.Login;
                send.byRet = 0;
            }
            else
            {
                send.byRet = 24;
            }
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 21,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] manage auth send failed: {ex.Message}");
        }
        return true;
    }

    private async Task<bool> ManageClientLimitRunRequest(PublicConnection connection, _manage_client_limit_run_request_loac request, CancellationToken token)
    {
        if (_manageUserState != ManageUserState.Login)
        {
            await SendManageClientLimitRunResultAsync(connection, request.idLocal, 81, token).ConfigureAwait(false);
            return true;
        }

        byte loginIndex = unchecked((byte)connection.ConnectionId);
        var forward = new _manage_client_limit_run_request_acwr
        {
            idLocal = request.idLocal,
            byLoginServerIndex = loginIndex
        };
        var worldSnapshot = _context.GetWorldSnapshot();
        int worldCount = Math.Min(_context.WorldCount, worldSnapshot.Length);

        for (int i = 0; i < worldCount; i++)
        {
            if (!worldSnapshot[i].IsService) continue;
            if (!_context.TryGetWorldConnection(i, out var worldConnection)) continue;

            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 26,
                Payload = forward.ToArray()
            };

            try
            {
                await worldConnection.SendAsync(env, token).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _log($"[{connection.ConnectionId}] manage limit run forward failed: {ex.Message}");
            }
        }

        _ = SetLimitRunAndRespondAsync(connection, request.idLocal, token);
        return true;
    }

    private async Task<bool> ManageClientForceExitRequest(PublicConnection connection, _manage_client_force_exit_request_loac request, CancellationToken token)
    {
        if (_manageUserState != ManageUserState.Login)
        {
            await SendManageClientForceExitResultAsync(connection, request.idLocal, 24, token).ConfigureAwait(false);
            return true;
        }

        var worldSnapshot = _context.GetWorldSnapshot();
        int worldCount = Math.Min(_context.WorldCount, worldSnapshot.Length);
        for (int i = 0; i < worldCount; i++)
        {
            if (!worldSnapshot[i].IsService) continue;
            if (!_context.TryGetWorldConnection(i, out var worldConnection)) continue;

            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 27,
                Payload = Array.Empty<byte>()
            };
            try
            {
                await worldConnection.SendAsync(env, token).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _log($"[{connection.ConnectionId}] manage force exit forward failed: {ex.Message}");
            }
        }

        await SendManageClientForceExitResultAsync(connection, request.idLocal, 0, token).ConfigureAwait(false);
        return true;
    }

    private async Task SetLimitRunAndRespondAsync(PublicConnection connection, _CLID idLocal, CancellationToken token)
    {
        byte ret = await SetLimitRunAsync(token).ConfigureAwait(false);
        await SendManageClientLimitRunResultAsync(connection, idLocal, ret, token).ConfigureAwait(false);
    }

    private async Task<byte> SetLimitRunAsync(CancellationToken token)
    {
        byte[] hash;
        try
        {
            hash = SHA256.HashData(LimitRunHashSeed);
        }
        catch
        {
            return 82;
        }

        string hashStr = "0x" + Convert.ToHexString(hash);
        var (ret, serial) = await _db.Select_Limit_Run_Record_SerialAsync(token).ConfigureAwait(false);
        if (ret == 2)
        {
            byte insertRet = await _db.Insert_Set_Limit_RunAsync(hashStr, token).ConfigureAwait(false);
            return insertRet == 0 ? (byte)0 : (byte)24;
        }
        if (ret != 0)
        {
            return 24;
        }

        byte updateRet = await _db.Update_Set_Limit_RunAsync(serial, hashStr, token).ConfigureAwait(false);
        return updateRet == 0 ? (byte)0 : (byte)24;
    }

    private async Task SendManageClientLimitRunResultAsync(PublicConnection connection, _CLID idLocal, byte ret, CancellationToken token)
    {
        var send = new _manage_client_limit_run_account_result_aclo
        {
            idLocal = idLocal,
            byRet = ret
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 23,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] manage limit run result send failed: {ex.Message}");
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

    private static Task<bool> DispatchControlLineAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        if (packet.OpCode == 40 && packet.SubCode == 1)
        {
            return Task.FromResult(true);
        }
        return Task.FromResult(false);
    }

    private async Task SendManageClientForceExitResultAsync(PublicConnection connection, _CLID idLocal, byte ret, CancellationToken token)
    {
        var send = new _manage_client_force_exit_result_aclo
        {
            idLocal = idLocal,
            byRet = ret
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 26,
            Payload = send.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] manage force exit result send failed: {ex.Message}");
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
        if (!_context.TryGetActiveGlobal(request.gidGlobal.dwIndex, out var user) || !user.IsLogin)
        {
            retCode = 3;
        }
        else if (user.GlobalId.dwSerial != request.gidGlobal.dwSerial)
        {
            _log("CMainThread::pc_EnterWorldRequest() : G_id Different");
            retCode = 4;
        }
        else if (!_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession))
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

        user.WorldCode = worldSession!.WorldCode;
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

    private Task<bool> FireguardBlockRequest(PublicConnection connection, _fireguard_block_request_wrac request, CancellationToken token)
    {
        string id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"FireguardBlockRequest id='{id}' serial={request.dwAccountSerial} ip={new IPAddress(request.dwIP)}");
        return Task.FromResult(true);
    }

    private async Task<bool> UiLockInitRequest(PublicConnection connection, _uilock_init_request_wrac request, CancellationToken token)
    {
        var send = new _uilock_init_result_acwr
        {
            byRet = 0,
            wUserIndex = request.wUserIndex,
            byHintIndex = request.byHintIndex
        };
        Buffer.BlockCopy(request.uszUILockPW, 0, send.uszUILockPW, 0, Math.Min(send.uszUILockPW.Length, request.uszUILockPW.Length));
        Buffer.BlockCopy(request.uszHintAnswer, 0, send.uszHintAnswer, 0, Math.Min(send.uszHintAnswer.Length, request.uszHintAnswer.Length));

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
        var send = new _uilock_update_result_acwr
        {
            byRet = 0,
            wUserIndex = request.wUserIndex,
            byHintIndex = request.byHintIndex
        };
        Buffer.BlockCopy(request.uszUILockPW, 0, send.uszUILockPW, 0, Math.Min(send.uszUILockPW.Length, request.uszUILockPW.Length));
        Buffer.BlockCopy(request.uszHintAnswer, 0, send.uszHintAnswer, 0, Math.Min(send.uszHintAnswer.Length, request.uszHintAnswer.Length));

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
        var send = new _uilock_user_refresh_info_result_acwr
        {
            dwAccountSerial = request.dwAccountSerial,
            byResult = 0
        };

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

    private Task<bool> FireguardDivideBlockRequest(PublicConnection connection, _fireguard_divide_block_request_wrac request, CancellationToken token)
    {
        string id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"FireguardDivideBlockRequest id='{id}' serial={request.dwAccountSerial} ip={new IPAddress(request.dwIP)}");
        return Task.FromResult(true);
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

    private Task<bool> ApexBlockRequest(PublicConnection connection, _apex_block_request_wrac request, CancellationToken token)
    {
        string id = PacketStringUtil.ToAsciiNullTerm(request.szAccountID);
        _log($"ApexBlockRequest id='{id}' serial={request.dwAccountSerial} ip={new IPAddress(request.dwIP)}");
        return Task.FromResult(true);
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

    private Task<bool> UserNumReport(PublicConnection connection, _user_num_report_wrac request, CancellationToken token)
    {
        if (_context.TryGetWorldSessionByConnection(connection.ConnectionId, out var worldSession)
            && _context.TryGetWorld(worldSession.WorldCode, out var world))
        {
            string date = PacketStringUtil.ToAsciiNullTerm(request.szLogDate);
            _log($"UserNumReport {world.Name} avg={request.dwAveragePerHour} max={request.dwMaxPerHour} date={date}");
        }
        return Task.FromResult(true);
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
        var send = new _user_block_result_acwr
        {
            byBlockResult = 0,
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

    private async Task<bool> CashDbSettingRequest(PublicConnection connection, CancellationToken token)
    {
        var send = new _cashdb_setting_result_acwr();
        PacketStringUtil.FillFixed(send.szIP, _settings.Database.Billing.Host);
        PacketStringUtil.FillFixed(send.szDBName, _settings.Database.Billing.Database);
        PacketStringUtil.FillFixed(send.szDSN, _settings.Database.Billing.Database);
        PacketStringUtil.FillFixed(send.szAccount, _settings.Database.Billing.User);
        PacketStringUtil.FillFixed(send.szPassword, _settings.Database.Billing.Password);
        send.dwPort = (uint)_settings.Database.Billing.Port;

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

    private async Task SendToLoginServersAsync(PacketEnvelope env, CancellationToken token)
    {
        foreach (var session in _context.GetLoginSessionsSnapshot())
        {
            if (session.Connection == null) continue;
            try
            {
                await session.Connection.SendAsync(env, token).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _log($"[{session.ConnectionId}] login broadcast failed: {ex.Message}");
            }
        }
    }

    private static string ExtractHost(string address)
    {
        if (string.IsNullOrWhiteSpace(address))
        {
            return string.Empty;
        }

        string host = address.Trim();
        int colon = host.LastIndexOf(':');
        if (colon > 0 && host.IndexOf(':') == colon)
        {
            host = host.Substring(0, colon);
        }
        return host;
    }
}
