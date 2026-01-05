using System;
using System.Buffers.Binary;
using System.Data;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using LoginServer.Data.Contexts;
using LoginServer.Packets;
using LoginServer.Settings;
using LoginServer.State;
using RFNetworking;
using Microsoft.Data.SqlClient;
using Microsoft.EntityFrameworkCore;

namespace LoginServer.Handlers;

/// <summary>
/// Mirrors the native CNetworkEX::ClinetLineAnalysis dispatch. Routes client packets (op 21) by subcode.
/// </summary>
public sealed class ClientPacketRouter
{
    private readonly Action<string> _log;

    public ClientPacketRouter(Action<string> log, int nationCode = 410)
    {
        _log = log;
        NationCode = nationCode;
    }

    public int NationCode { get; set; }

    public async Task<bool> HandleAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (packet.OpCode != 21)
        {
            return false;
        }

        byte sub = packet.SubCode;

        switch (sub)
        {
            case 1:
            {
                var req = new _join_account_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await JoinAccountRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 3:
            {
                var req = new _login_account_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await LoginAccountRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 5:
            {
                var req = new _world_list_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await WorldListRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 7:
            {
                var req = new _select_world_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await SelectWorldRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 9:
            {
                var req = new _push_close_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await PushCloseRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 12:
            {
                var req = new _crypty_key_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await CryptKeyRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 15:
                // Allowed no-op for nation 410 in the native implementation.
                if (NationCode == 410)
                {
                    _log("Client opcode 21/15 accepted (nation 410 noop).");
                    return true;
                }
                return false;
            case 17:
            {
                var req = new _motp_validation_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await MotpValidationRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 20:
            {
                var req = new _manage_account_auth_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await ManageAccountAuthRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 22:
            {
                var req = new _manage_client_limit_run_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await ManageClientLimitRunRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 25:
            {
                var req = new _manage_client_force_exit_request_cllo();
                if (!req.Load(packet.Payload)) return false;
                return await ManageClientForceExitRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            default:
                _log($"Unhandled client opcode 21/{sub}");
                return false;
        }
    }

    private Task<bool> JoinAccountRequest(PublicConnection connection, _join_account_request_cllo request, CancellationToken cancellationToken)
    {
        string account = PacketStringUtil.ToAscii(request.szAccountID);
        string password = PacketStringUtil.ToAscii(request.szPassword);

        return Task.FromResult(account.Length <= 12 && password.Length <= 12);
    }

    private Task<bool> LoginAccountRequest(PublicConnection connection, _login_account_request_cllo request, CancellationToken cancellationToken)
    {
        uint idx = (uint)connection.ConnectionId;
        if (!TryGetKeys(idx, out byte plusKey, out ushort xorKey))
        {
            return Task.FromResult(false);
        }

        Span<byte> idBuf = stackalloc byte[13];
        Span<byte> pwBuf = stackalloc byte[13];
        request.szAccountID.AsSpan().CopyTo(idBuf);
        request.szPassword.AsSpan().CopyTo(pwBuf);
        DecryptString(idBuf, plusKey, xorKey);
        DecryptString(pwBuf, plusKey, xorKey);

        string account = PacketStringUtil.ToAscii(idBuf.ToArray());
        string password = PacketStringUtil.ToAscii(pwBuf.ToArray());

        if (account.Length > 12 || password.Length > 12)
        {
            return Task.FromResult(false);
        }

        // Persist session fields (analogous to AUTHWORK/USER)
        var session = MainContext.Instance.GetOrAddClient(idx, i => new ClientSession((ushort)i));
        session.ServerType = request.byServerType;
        session.ClientIp = GetClientIp(connection);
        session.AccountId = account;
        session.Password = password;
        MainContext.Instance.IncrementUserCount();

        _ = UserAuthAsync(session, connection, cancellationToken);
        return Task.FromResult(true);
    }

    private Task<bool> WorldListRequest(PublicConnection connection, _world_list_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"WorldListRequest from {connection.RemoteEndPoint} clientVer={request.dwClientVersion}");
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldRequest(PublicConnection connection, _select_world_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"SelectWorldRequest from {connection.RemoteEndPoint} worldIndex={request.wWorldIndex}");
        return Task.FromResult(true);
    }

    private Task<bool> PushCloseRequest(PublicConnection connection, _push_close_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"PushCloseRequest from {connection.RemoteEndPoint}");
        return Task.FromResult(true);
    }

    private Task<bool> CryptKeyRequest(PublicConnection connection, _crypty_key_request_cllo request, CancellationToken cancellationToken)
    {
        // Generate per-client crypt seeds.
        var rnd = Random.Shared;
        byte plusKey = (byte)(rnd.Next(0, 5));
        ushort xorKey = (ushort)(rnd.Next(0, 1024));

        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetOrAddClient(idx, i => new ClientSession((ushort)i));
        session.PlusKey = plusKey;
        session.XorKey = xorKey;
        session.HasCryptKeys = true;

        var send = new _crypty_key_inform_locl
        {
            byPlus = plusKey,
            wKey = xorKey
        };
        Span<byte> payload = stackalloc byte[3];
        payload[0] = send.byPlus;
        BinaryPrimitives.WriteUInt16LittleEndian(payload.Slice(1, 2), send.wKey);

        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 13,
            Payload = payload.ToArray()
        };
        _ = connection.SendAsync(env, cancellationToken);

        // Advance keys as native code does (client applies same increment after receipt).
        session.PlusKey = (byte)(plusKey + 1);
        session.XorKey = (ushort)(xorKey + 3);
        return Task.FromResult(true);
    }

    private Task<bool> MotpValidationRequest(PublicConnection connection, _motp_validation_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"MotpValidationRequest from {connection.RemoteEndPoint} type={request.byType}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthRequest(PublicConnection connection, _manage_account_auth_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"ManageAccountAuthRequest from {connection.RemoteEndPoint} len={request.byBin.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunRequest(PublicConnection connection, _manage_client_limit_run_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"ManageClientLimitRunRequest from {connection.RemoteEndPoint}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitRequest(PublicConnection connection, _manage_client_force_exit_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"ManageClientForceExitRequest from {connection.RemoteEndPoint}");
        return Task.FromResult(true);
    }

    private static void DecryptString(Span<byte> buffer, byte plusKey, ushort xorKey)
    {
        for (int i = 0; i < buffer.Length; i++)
        {
            buffer[i] ^= (byte)xorKey;
            buffer[i] = (byte)(buffer[i] - plusKey);
        }
    }

    private static uint GetClientIp(PublicConnection connection)
    {
        if (connection.RemoteEndPoint is System.Net.IPEndPoint ipEp && ipEp.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
        {
            var bytes = ipEp.Address.GetAddressBytes();
            return (uint)(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
        }
        return 0;
    }

    private bool TryGetKeys(uint idx, out byte plusKey, out ushort xorKey)
    {
        plusKey = 0;
        xorKey = 0;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null || !session.HasCryptKeys) return false;
        plusKey = session.PlusKey;
        xorKey = session.XorKey;
        return true;
    }

    private async Task UserAuthAsync(ClientSession session, PublicConnection connection, CancellationToken cancellationToken)
    {
        // Mirrors RusiaAuthorizer::UserAuth + CMainThread::LoginProcessOverseas
        if (session.AccountId.Length > 12 || session.Password.Length > 12)
        {
            await SendLoginResultAsync(connection, 6, cancellationToken).ConfigureAwait(false);
            return;
        }

        var settings = AppSettings.Load();
        string userConnStr = settings.Database.BuildConnectionString();
        string billingConnStr = settings.Database.BuildBillingConnectionString();
        byte retCode = 0;
        session.LoginCode = 0;

        try
        {
            if (IsDevUser(session.AccountId))
            {
                // Dev/GM account: bypass user DB/billing; account server will handle auth.
                session.BillType = 1;
                session.RemainTime = 255;
                session.IsPremium = true;
            }
            else
            {
                string? dbPassword = null;
                await using (var userConn = new SqlConnection(userConnStr))
                {
                    await userConn.OpenAsync(cancellationToken).ConfigureAwait(false);
                    using var cmd = userConn.CreateCommand();
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "pSelect_AccountPass";
                    cmd.Parameters.AddWithValue("@id", session.AccountId);

                    using var reader = await cmd.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
                    if (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
                    {
                        dbPassword = reader.IsDBNull(0) ? null : reader.GetString(0);
                    }
                }

                if (dbPassword == null)
                {
                    retCode = 6;
                }
                else if (!string.Equals(dbPassword, session.Password, StringComparison.Ordinal))
                {
                    retCode = 7;
                }
                else
                {
                    // Billing check for verified accounts
                    int status = 0;
                    await using (var billingConn = new SqlConnection(billingConnStr))
                    {
                        await billingConn.OpenAsync(cancellationToken).ConfigureAwait(false);
                        using var cmd = billingConn.CreateCommand();
                        cmd.CommandType = CommandType.StoredProcedure;
                        cmd.CommandText = "RF_CheckAccountStatus";
                        cmd.Parameters.AddWithValue("@id", session.AccountId);
                        var statusParam = new SqlParameter("@Status", SqlDbType.Int)
                        {
                            Direction = ParameterDirection.Output
                        };
                        cmd.Parameters.Add(statusParam);

                        await cmd.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);
                        status = statusParam.Value is int s ? s : 0;
                    }

                    switch (status)
                    {
                        case -2:
                            session.BillType = 100;
                            session.RemainTime = 0;
                            retCode = 43;
                            break;
                        case -1:
                            session.BillType = 100;
                            session.RemainTime = 0;
                            retCode = 40;
                            break;
                        case 0:
                            session.BillType = 100;
                            session.RemainTime = 0;
                            retCode = 54;
                            break;
                        case 1:
                            session.BillType = 8;
                            session.RemainTime = 255;
                            break;
                        case 2:
                            session.BillType = 1;
                            session.IsPremium = true;
                            session.RemainTime = 255;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        catch (Exception ex)
        {
            _log($"LoginAccountRequest: auth error {ex.Message}");
            retCode = 24;
        }

        if (retCode != 0)
        {
            await SendLoginResultAsync(connection, retCode, cancellationToken).ConfigureAwait(false);
            return;
        }

        // Forward to account server
        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            await SendLoginResultAsync(connection, 24, cancellationToken).ConfigureAwait(false);
            return;
        }

        var req = _login_account_request_loac.FromSession(session);
        byte[] payload = new byte[Marshal.SizeOf<_login_account_request_loac>()];
        MemoryMarshal.Write(payload, in req);
        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 3,
            Payload = payload
        };
        try
        {
            await accountConn.SendAsync(env, cancellationToken).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"Forward login to account failed: {ex.Message}");
            await SendLoginResultAsync(connection, 24, cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendLoginResultAsync(PublicConnection connection, byte retCode, CancellationToken cancellationToken)
    {
        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 4,
            Payload = new[] { retCode }
        };
        try
        {
            await connection.SendAsync(env, cancellationToken).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"SendLoginResult failed: {ex.Message}");
        }
    }

    private static bool IsDevUser(string accountId)
    {
        // CAuthorizer::IsDevUser returns true when first char is '!' (ASCII 33).
        return !string.IsNullOrEmpty(accountId) && accountId[0] == '!';
    }

}
