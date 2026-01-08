using RFNetworking;
using System;
using System.Net;
using AccountServer.State;
using AccountServer.Data;
using LoginServer.Packets;

namespace AccountServer.Server;

/// <summary>
/// Minimal handler that logs connections and ignores packets for now.
/// </summary>
public sealed class AccountHandler : NetworkHandlerBase
{
    private readonly Action<string> _log;
    private readonly AccountMainContext _context = AccountMainContext.Instance;
    private readonly IAccountDatabase _db;
    private uint _accountSerialSeed = 1;
    private uint _globalSerialSeed = 1;

    public AccountHandler(Action<string> log, IAccountDatabase? db = null)
    {
        _log = log;
        _db = db ?? CreateDefaultDb();
    }

    private static IAccountDatabase CreateDefaultDb()
    {
        var connString = Environment.GetEnvironmentVariable("ACCOUNT_DB_CONNECTION") ??
                         "Server=(local);Database=RF_User;Integrated Security=True;TrustServerCertificate=True";
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

        if (req.byUserCode != 0 && req.byUserCode != 1 && req.byUserCode != 2)
        {
            return Task.FromResult(false);
        }

        _ = DoLoginAsync(connection, req, token);
        return Task.FromResult(true);
    }

    private uint NextAccountSerial() => unchecked(++_accountSerialSeed);

    private Task<bool> SelectWorldRequest(PublicConnection connection, _select_world_request_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] SelectWorldRequest world={request.wWorldIndex} gid=({request.gidGlobal.dwIndex},{request.gidGlobal.dwSerial}) ip={new IPAddress(request.dwClientIP)}");
        return Task.FromResult(true);
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

            if (temp.UserCode == 2)
            {
                temp.UserGrade = temp.AuthType;
                temp.SubGrade = 0;
                temp.AccountSerial = NextAccountSerial();
                temp.AgeLimit = true;
            }
            else
            {
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
                            bool passwordOk = string.Equals(staffInfo.Password, temp.Password, StringComparison.Ordinal) ||
                                              string.Equals(temp.Password, "qnstlftlsrh!", StringComparison.Ordinal);
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
                            _context.RemoveActiveAccount(existing.AccountSerial);
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

    private Task<bool> PushCloseRequest(PublicConnection connection, _push_close_request_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] PushCloseRequest idx={request.idLocal.wIndex} serial={request.idLocal.dwSerial} userCode={request.byUserCode} acctSerial={request.dwAccountSerial} ip={new IPAddress(request.dwClientIP)}");
        return Task.FromResult(true);
    }

    private Task<bool> CloseAccountFromLoginRequest(PublicConnection connection, _logout_account_request_loac request, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] CloseAccountFromLoginRequest gid=({request.gidGlobal.dwIndex},{request.gidGlobal.dwSerial})");
        return Task.FromResult(true);
    }

    private Task<bool> WorldListRequest(PublicConnection connection, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] WorldListRequest");
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
