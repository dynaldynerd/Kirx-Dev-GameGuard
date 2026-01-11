using RFNetworking;
using System;
using System.Net;
using AccountServer.State;
using AccountServer.Data;
using AccountServer.Settings;
using LoginServer.Packets;

namespace AccountServer.Server;

/// <summary>
/// Minimal handler that logs connections and ignores packets for now.
/// </summary>
public sealed class AccountHandler : NetworkHandlerBase
{
    private readonly Action<string> _log;
    private readonly AccountMainContext _context = AccountMainContext.Instance;
    private readonly AppSettings _settings;
    private readonly IAccountDatabase _db;
    private uint _accountSerialSeed = 1;
    private uint _globalSerialSeed = 1;

    public AccountHandler(Action<string> log, AppSettings settings, string? connectionString = null, IAccountDatabase? db = null)
    {
        _log = log;
        _settings = settings;
        _db = db ?? CreateDefaultDb(connectionString);
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
        _log($"[{connection.ConnectionId}] connected from {connection.RemoteEndPoint}");
        _context.Register(connection);
        return Task.CompletedTask;
    }

    public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] disconnected");
        _context.Unregister(connection);
        return Task.CompletedTask;
    }

    public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    public override async Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        bool ok = await DispatchLoginLineAsync(connection, packet, cancellationToken).ConfigureAwait(false);
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
        return Task.FromResult(true);
    }

    private Task<bool> LoginServerStatResult(PublicConnection connection, _login_server_stat_result_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LoginServerStatResult ret={request.byRet} clientIdx={request.wClientIndex}");
        return Task.FromResult(true);
    }

    private Task<bool> AccountDbInfoRequest(PublicConnection connection, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] AccountDBInfoRequest");
        return Task.FromResult(true);
    }

    private Task<bool> UpdateUserLoginFailureCntRequest(PublicConnection connection, _update_user_login_failure_cnt_loac request, CancellationToken token)
    {
        var id = PacketStringUtil.ToAsciiNullTerm(request.szUserID);
        _log($"[{connection.ConnectionId}] UpdateUserLoginFailureCntRequest id='{id}' type={request.byType}");
        return Task.FromResult(true);
    }

    private Task<bool> LogoutManageAccountRequest(PublicConnection connection, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LogoutManageAccountRequest");
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthRequest(PublicConnection connection, _manage_account_auth_request_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageAccountAuthRequest idx={request.idLocal.wIndex} serial={request.idLocal.dwSerial}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunRequest(PublicConnection connection, _manage_client_limit_run_request_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageClientLimitRunRequest idx={request.idLocal.wIndex} serial={request.idLocal.dwSerial}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitRequest(PublicConnection connection, _manage_client_force_exit_request_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageClientForceExitRequest idx={request.idLocal.wIndex} serial={request.idLocal.dwSerial}");
        return Task.FromResult(true);
    }
}
