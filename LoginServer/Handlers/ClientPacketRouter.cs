using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using LoginServer.Data;
using LoginServer.Data.Contexts;
using LoginServer.Data.Entities;
using LoginServer.Packets;
using LoginServer.Security;
using LoginServer.Settings;
using LoginServer.State;
using RFNetworking;
using Microsoft.EntityFrameworkCore;

namespace LoginServer.Handlers;

/// <summary>
/// Mirrors the native CNetworkEX::ClinetLineAnalysis dispatch. Routes client packets (op 21) by subcode.
/// </summary>
public sealed class ClientPacketRouter
{
    private readonly Action<string> _log;
    private readonly AppSettings _settings;

    public ClientPacketRouter(Action<string> log, AppSettings settings)
    {
        _log = log;
        _settings = settings;
    }

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
                // Historically nation-specific noop; we accept it universally.
                _log("Client opcode 21/15 accepted (noop).");
                return true;
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
        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null)
        {
            return Task.FromResult(false);
        }

        byte retCode = 0;

        if (!session.IsLogin)
        {
            retCode = 3;
        }
        else if (session.RegisteredWorld)
        {
            retCode = 31;
        }

        session.RegisteredWorld = true;

        var worlds = MainContext.Instance.Worlds;
        int limit = MainContext.Instance.WorldCount;
        if (MainContext.Instance.ServiceWorldNum > 0 && MainContext.Instance.ServiceWorldNum < limit)
        {
            limit = MainContext.Instance.ServiceWorldNum;
        }

        // Build list payload
        Span<byte> listBuffer = stackalloc byte[4095];
        int dataUsed = 0;
        var userLevels = new List<ushort>();
        var freeFlags = new List<byte>();

        if (session.ServerType == 1)
        {
            var world = FindWorldByType(worlds, limit, 1);
            if (world == null)
            {
                retCode = 31;
            }
            else
            {
                listBuffer[0] = 1; // world count
                dataUsed = 1 + AppendWorldEntry(listBuffer.Slice(1), world.Value);
                userLevels.Add(MapUserLoad(world.Value.UserCount));
                freeFlags.Add((byte)(world.Value.FreeServer ? 1 : 0));
            }
        }
        else
        {
            int included = 0;
            listBuffer[0] = 0; // placeholder for count
            int offset = 1;
            for (int i = 0; i < limit && i < worlds.Count; i++)
            {
                var world = worlds[i];
                if (world.Type == 1 || world.Type == 2)
                {
                    continue;
                }
                included++;
                int written = AppendWorldEntry(listBuffer.Slice(offset), world);
                offset += written;
                userLevels.Add(MapUserLoad(world.UserCount));
                freeFlags.Add((byte)(world.FreeServer ? 1 : 0));
                if (offset >= listBuffer.Length) break;
            }

            if (included == 0)
            {
                retCode = 31;
                offset = 0;
            }
            else
            {
                listBuffer[0] = (byte)included;
            }
            dataUsed = offset;
        }

        if (dataUsed > listBuffer.Length) dataUsed = listBuffer.Length;

        var listPacket = new _world_list_result_locl
        {
            byRetCode = retCode,
            wDataSize = (ushort)Math.Min(dataUsed, 4095)
        };
        if (listPacket.wDataSize > 0)
        {
            listBuffer.Slice(0, listPacket.wDataSize).CopyTo(listPacket.sListData);
        }

        _ = SendWorldListResultAsync(connection, listPacket, cancellationToken);
        _ = SendWorldUserInformAsync(connection, (byte)userLevels.Count, userLevels, cancellationToken);
        _ = SendFreeServerInformAsync(connection, (byte)freeFlags.Count, freeFlags, cancellationToken);
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldRequest(PublicConnection connection, _select_world_request_cllo request, CancellationToken cancellationToken)
    {
        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null)
        {
            return Task.FromResult(false);
        }

        byte retCode = 0;
        int worldIndex = request.wWorldIndex;

        if (!session.IsLogin)
        {
            retCode = 3;
        }
        else if (!session.RegisteredWorld)
        {
            retCode = 32;
        }
        // No nation-specific gating.

        var worlds = MainContext.Instance.Worlds;
        int worldLimit = MainContext.Instance.WorldCount;

        if (worldIndex >= worldLimit)
        {
            return Task.FromResult(false);
        }

        if (retCode == 0 && session.ServerType == 1)
        {
            int testIdx = FindTestWorld(worlds, worldLimit);
            if (testIdx < 0)
            {
                retCode = 34;
            }
            else
            {
                worldIndex = testIdx;
            }
        }

        if (retCode == 0)
        {
            if (worldIndex < 0 || worldIndex >= worldLimit || worldIndex >= worlds.Count)
            {
                retCode = 34;
            }
            else
            {
                var world = worlds[worldIndex];
                if (!world.IsOpen)
                {
                    retCode = 29;
                }
                else if (world.Type == 3 && !session.IsAdult)
                {
                    retCode = 72;
                }
                // Skip nation-specific caps/billing rules.
            }
        }

        if (retCode != 0)
        {
            SendSelectWorldResult(connection, retCode, cancellationToken);
            return Task.FromResult(true);
        }

        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            SendSelectWorldResult(connection, 24, cancellationToken);
            return Task.FromResult(true);
        }

        session.SelectedWorld = true;
        session.SelectedWorldCode = worldIndex;

        var send = new _select_world_request_loac
        {
            gidGlobal = session.GlobalId,
            wWorldIndex = (ushort)worldIndex,
            dwClientIP = session.ClientIp,
            dwRequestMoveCharacterSerialList = new uint[3],
            dwRTournamentCharacterSerialList = new uint[3]
        };

        var sendBlit = _select_world_request_loac_blit.From(send);
        var payload = sendBlit.ToArray();
        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 5,
            Payload = payload
        };
        _ = accountConn.SendAsync(env, cancellationToken);
        _log($"SelectWorldRequest from {connection.RemoteEndPoint} worldIndex={worldIndex}");
        return Task.FromResult(true);
    }

    private Task<bool> PushCloseRequest(PublicConnection connection, _push_close_request_cllo request, CancellationToken cancellationToken)
    {
        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null)
        {
            return Task.FromResult(false);
        }

        byte retCode = 0;
        if (session.OverlapUser)
        {
            // No MOTP checks yet; always allow.
        }
        else
        {
            retCode = 200; // -56 in native (overflow)
        }

        if (retCode != 0)
        {
            // Nothing to send on client line; just record and return.
            MainContext.Instance.RecordPushCloseResult(session.Index, retCode);
            return Task.FromResult(true);
        }

        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            return Task.FromResult(false);
        }

        var send = new _push_close_request_loac
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial },
            byUserCode = session.LoginCode,
            dwAccountSerial = session.AccountSerial,
            dwClientIP = session.ClientIp
        };
        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 7,
            Payload = send.ToArray()
        };
        _ = accountConn.SendAsync(env, cancellationToken);

        // Clear overlap flag as native does.
        session.OverlapUser = false;
        session.AccountSerial = uint.MaxValue;

        _log($"PushCloseRequest forwarded for idx={session.Index} serial={session.ClidSerial}");
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

        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 13,
            Payload = send.ToArray()
        };
        _ = connection.SendAsync(env, cancellationToken);

        // Advance keys as native code does (client applies same increment after receipt).
        session.PlusKey = (byte)(plusKey + 1);
        session.XorKey = (ushort)(xorKey + 3);
        return Task.FromResult(true);
    }

    private static int AppendWorldEntry(Span<byte> buffer, WorldData world)
    {
        int offset = 0;
        if (buffer.Length < 3) return 0;
        buffer[offset++] = world.IsOpen ? (byte)1 : (byte)0;
        var name = world.Name ?? string.Empty;
        int nullIdx = name.IndexOf('\0');
        if (nullIdx >= 0) name = name.Substring(0, nullIdx);
        var nameBytes = Encoding.ASCII.GetBytes(name);
        byte nameLen = (byte)Math.Min(255, nameBytes.Length + 1);
        buffer[offset++] = nameLen;
        int copyLen = Math.Min(nameBytes.Length, nameLen - 1);
        copyLen = Math.Min(copyLen, buffer.Length - offset - 2);
        if (copyLen > 0)
        {
            nameBytes.AsSpan(0, copyLen).CopyTo(buffer.Slice(offset));
        }
        offset += copyLen;
        if (offset < buffer.Length) buffer[offset++] = 0;
        if (offset < buffer.Length) buffer[offset++] = world.Type;
        return offset;
    }

    private static WorldData? FindWorldByType(IReadOnlyList<WorldData> worlds, int limit, byte type)
    {
        for (int i = 0; i < limit && i < worlds.Count; i++)
        {
            if (worlds[i].Type == type)
            {
                return worlds[i];
            }
        }
        return null;
    }

    private static int FindTestWorld(IReadOnlyList<WorldData> worlds, int limit)
    {
        for (int i = 0; i < limit && i < worlds.Count; i++)
        {
            if (worlds[i].Type == 1)
            {
                return i;
            }
        }
        return -1;
    }

    private void SendSelectWorldResult(PublicConnection connection, byte retCode, CancellationToken token)
    {
        var result = new _select_world_result_locl
        {
            byRetCode = retCode,
            dwWorldGateIP = 0,
            wWorldGatePort = 0,
            dwWorldMasterKey = new uint[4],
            bAllowAltTab = false
        };
        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 8,
            Payload = result.ToArray()
        };
        _ = connection.SendAsync(env, token);
    }

    private ushort MapUserLoad(int count)
    {
        var thresholds = _settings.Network.UserLoadThresholds ?? Array.Empty<int>();
        int t0 = thresholds.Length > 0 ? thresholds[0] : 500;
        int t1 = thresholds.Length > 1 ? thresholds[1] : 1000;
        int t2 = thresholds.Length > 2 ? thresholds[2] : 1500;

        if (count >= t2) return 3;
        if (count >= t1) return 2;
        if (count >= t0) return 1;
        return 0;
    }

    private static Task SendWorldListResultAsync(PublicConnection connection, _world_list_result_locl data, CancellationToken token)
    {
        var payload = data.ToArray();
        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 6,
            Payload = payload
        };
        return connection.SendAsync(env, token);
    }

    private static Task SendWorldUserInformAsync(PublicConnection connection, byte serviceWorldNum, IReadOnlyList<ushort> userLevels, CancellationToken token)
    {
        int count = Math.Min(serviceWorldNum, userLevels.Count);
        var packet = new _world_user_inform_locl { byServiceWorldNum = serviceWorldNum };
        for (int i = 0; i < count; i++)
        {
            packet.wUserNum[i] = userLevels[i];
        }
        var payload = packet.ToArray();

        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 66,
            Payload = payload
        };
        return connection.SendAsync(env, token);
    }

    private static Task SendFreeServerInformAsync(PublicConnection connection, byte serviceWorldNum, IReadOnlyList<byte> freeFlags, CancellationToken token)
    {
        int count = Math.Min(serviceWorldNum, freeFlags.Count);
        var packet = new _free_server_inform_locl { byServiceWorldNum = serviceWorldNum, bFree = new byte[40] };
        for (int i = 0; i < count; i++)
        {
            packet.bFree[i] = freeFlags[i];
        }
        var payload = packet.ToArray();

        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 67,
            Payload = payload
        };
        return connection.SendAsync(env, token);
    }

    private Task<bool> MotpValidationRequest(PublicConnection connection, _motp_validation_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"MotpValidationRequest from {connection.RemoteEndPoint} type={request.byType}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthRequest(PublicConnection connection, _manage_account_auth_request_cllo request, CancellationToken cancellationToken)
    {
        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null || session.LoginCode != 2)
        {
            return Task.FromResult(false);
        }

        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            return Task.FromResult(false);
        }

        var send = new _manage_account_auth_request_loac
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial },
            byBin = new byte[32]
        };
        Buffer.BlockCopy(request.byBin, 0, send.byBin, 0, Math.Min(send.byBin.Length, request.byBin.Length));

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 20,
            Payload = send.ToArray()
        };
        _ = accountConn.SendAsync(env, cancellationToken);
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunRequest(PublicConnection connection, _manage_client_limit_run_request_cllo request, CancellationToken cancellationToken)
    {
        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null || session.LoginCode != 2)
        {
            return Task.FromResult(false);
        }

        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            return Task.FromResult(false);
        }

        var send = new _manage_client_limit_run_request_loac
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial }
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 22,
            Payload = send.ToArray()
        };
        _ = accountConn.SendAsync(env, cancellationToken);
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitRequest(PublicConnection connection, _manage_client_force_exit_request_cllo request, CancellationToken cancellationToken)
    {
        uint idx = (uint)connection.ConnectionId;
        var session = MainContext.Instance.GetClient(idx);
        if (session == null || session.LoginCode != 2)
        {
            return Task.FromResult(false);
        }

        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            return Task.FromResult(false);
        }

        var send = new _manage_client_force_exit_request_loac
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial }
        };

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 25,
            Payload = send.ToArray()
        };
        _ = accountConn.SendAsync(env, cancellationToken);
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
                session.LoginCode = 1; // GM code
                session.AgeLimit = true;
            }
            else if (!MainContext.Instance.ExternalOpen)
            {
                retCode = 48;
                session.LoginCode = 0;
                session.BillType = 100;
                session.RemainTime = 0;
            }
            else
            {
                string? accountDbName = MainContext.Instance.AccountDbName;
                if (string.IsNullOrWhiteSpace(accountDbName))
                {
                    await SendLoginResultAsync(connection, 24, cancellationToken).ConfigureAwait(false);
                    return;
                }

                string userConnStr = settings.Database.BuildConnectionString(accountDbName);
                string? passwordHash = null;
                byte[] salt;
                try
                {
                    string? saltBase64 = MainContext.Instance.AccountArgon2SaltBase64;
                    if (string.IsNullOrWhiteSpace(saltBase64))
                    {
                        retCode = 24;
                        salt = Array.Empty<byte>();
                    }
                    else
                    {
                        salt = Convert.FromBase64String(saltBase64);
                    }
                }
                catch (FormatException)
                {
                    retCode = 24;
                    salt = Array.Empty<byte>();
                }

                if (retCode != 0)
                {
                    await SendLoginResultAsync(connection, retCode, cancellationToken).ConfigureAwait(false);
                    return;
                }

                var idHmac = CryptoHelper.ComputeHmacSha256(Encoding.UTF8.GetBytes(session.AccountId), salt);
                var userDbOptions = DbContextOptionsFactory.Create<LoginDbContext>(settings.Database.Provider, userConnStr);
                await using (var userCtx = new LoginDbContext(userDbOptions))
                {
                    var account = await userCtx.Accounts
                        .AsNoTracking()
                        .FirstOrDefaultAsync(a => a.IdHmac == idHmac, cancellationToken)
                        .ConfigureAwait(false);

                    if (account != null)
                    {
                        passwordHash = account.PasswordHash?.Trim();
                    }
                }

                if (passwordHash == null)
                {
                    retCode = 6;
                }
                else
                {
                    byte[] expectedHash;
                    try
                    {
                        expectedHash = Convert.FromBase64String(passwordHash);
                    }
                    catch (FormatException)
                    {
                        retCode = 24;
                        expectedHash = Array.Empty<byte>();
                    }

                    if (retCode == 0)
                    {
                        if (!CryptoHelper.VerifyArgon2id(Encoding.UTF8.GetBytes(session.Password), salt, expectedHash))
                        {
                            retCode = 7;
                        }
                    }
                }
                if (retCode == 0)
                {
                    // Billing check via EF (equivalent to RF_CheckAccountStatus proc)
                    int status = 1;
                    var options = DbContextOptionsFactory.Create<BillingDbContext>(settings.Database.BillingProvider, billingConnStr);

                    await using (var billingCtx = new BillingDbContext(options))
                    {
                        var entry = await GetOrCreateBillingUserStatusAsync(billingCtx, session.AccountId, cancellationToken)
                            .ConfigureAwait(false);

                        if (entry != null)
                        {
                            if (entry.DtEndPrem.HasValue && entry.DtEndPrem.Value > DateTime.Now)
                            {
                                status = entry.Status;
                            }
                            else
                            {
                                status = 1;
                                if (entry.Status != status)
                                {
                                    entry.Status = status;
                                    billingCtx.UserStatuses.Update(entry);
                                    await billingCtx.SaveChangesAsync(cancellationToken).ConfigureAwait(false);
                                }
                            }
                        }
                        else
                        {
                            status = 1;
                        }
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

        var req = BuildLoginAccountRequest(session);
        byte[] payload = req.ToArray();
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

    private static _login_account_request_loac_blit BuildLoginAccountRequest(State.ClientSession session)
    {
        var req = new _login_account_request_loac_blit
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial },
            byUserCode = session.LoginCode,
            dwClientIP = session.ClientIp,
            bCheckDoubleIP = 1,
            iType = (short)session.BillType,
            lRemainTime = session.RemainTime,
            wYear = 0,
            wMonth = 0,
            wDayOfWeek = 0,
            wDay = 0,
            wHour = 0,
            wMinute = 0,
            wSecond = 0,
            wMilliseconds = 0,
            authtype = 0,
            nTrans = session.Trans,
            bPrimium = (byte)(session.IsPremium ? 1 : 0),
            bAgeLimit = session.AgeLimit ? (byte)1 : (byte)0,
            bCancelWebUILockBlock = 0
        };

        PacketStringUtil.FillFixed(req.szAccountID, session.AccountId);
        PacketStringUtil.FillFixed(req.szPassword, session.Password);
        return req;
    }

    private static async Task<BillingUserStatus?> GetOrCreateBillingUserStatusAsync(
        BillingDbContext billingCtx,
        string accountId,
        CancellationToken cancellationToken)
    {
        BillingUserStatus? entry = await billingCtx.UserStatuses
            .FirstOrDefaultAsync(u => u.Id == accountId, cancellationToken)
            .ConfigureAwait(false);
        if (entry != null)
        {
            return string.Equals(entry.Id, accountId, StringComparison.Ordinal) ? entry : null;
        }

        DateTime now = DateTime.Now;
        billingCtx.UserStatuses.Add(new BillingUserStatus
        {
            Id = accountId,
            Status = 1,
            DtStartPrem = now,
            DtEndPrem = now,
            Cash = 0
        });

        try
        {
            await billingCtx.SaveChangesAsync(cancellationToken).ConfigureAwait(false);
        }
        catch (DbUpdateException)
        {
            billingCtx.ChangeTracker.Clear();
        }

        entry = await billingCtx.UserStatuses
            .FirstOrDefaultAsync(u => u.Id == accountId, cancellationToken)
            .ConfigureAwait(false);
        if (entry == null)
        {
            return null;
        }

        return string.Equals(entry.Id, accountId, StringComparison.Ordinal) ? entry : null;
    }

    private static bool IsDevUser(string accountId)
    {
        // CAuthorizer::IsDevUser returns true when first char is '!' (ASCII 33).
        return !string.IsNullOrEmpty(accountId) && accountId[0] == '!';
    }

}
