using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Net;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using LoginServer.Packets;
using LoginServer.State;
using RFNetworking;

namespace LoginServer.Handlers;

/// <summary>
/// Mirrors CNetworkEX::AccountLineAnalysis dispatch for packets from account server (op 1).
/// </summary>
public sealed class AccountPacketRouter
{
    private readonly Action<string> _log;

    public AccountPacketRouter(Action<string> log)
    {
        _log = log;
    }

    public async Task<bool> HandleAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (packet.OpCode != 1)
        {
            return false;
        }

        byte sub = packet.SubCode;
        switch (sub)
        {
            //account to login for server stat
            case 11:
            {
                var worldList = new _world_list_result_aclo();
                if (!worldList.Load(packet.Payload))
                {
                    return false;
                }
                return await WorldListResult(connection, worldList, cancellationToken).ConfigureAwait(false);
            }
            case 100:
            {
                var info = new _inform_open_world_aclo();
                if (!info.Load(packet.Payload))
                {
                    return false;
                }
                return await InformOpenWorld(connection, info, cancellationToken).ConfigureAwait(false);
            }
            case 101:
            {
                var info = new _inform_close_world_aclo();
                if (!info.Load(packet.Payload))
                {
                    return false;
                }
                return await InformCloseWorld(connection, info, cancellationToken).ConfigureAwait(false);
            }
            case 102:
            {
                var info = new _inform_usernum_world_aclo();
                if (!info.Load(packet.Payload))
                {
                    return false;
                }
                return await InformUserNumWorld(connection, info, cancellationToken).ConfigureAwait(false);
            }
            case 2:
            {
                var result = new _join_account_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await JoinAccountResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 4:
            {
                var result = new _login_account_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await LoginAccountResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 6:
            {
                var result = new _select_world_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await SelectWorldResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 8:
            {
                var result = new _push_close_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await PushCloseResult(connection, result, cancellationToken).ConfigureAwait(false);
            }



            //account to client
            case 200:
            {
                var cmd = new _force_close_command_aclo();
                if (!cmd.Load(packet.Payload))
                {
                    return false;
                }
                return await ForceCloseCommand(connection, cmd, cancellationToken).ConfigureAwait(false);
            }
            case 12:
            {
                var req = new _login_server_stat_request_aclo();
                if (!req.Load(packet.Payload))
                {
                    return false;
                }
                return await LoginStatRequest(connection, req, cancellationToken).ConfigureAwait(false);
            }
            case 14:
            {
                var report = new _holy_quest_now_report_aclo();
                if (!report.Load(packet.Payload))
                {
                    return false;
                }
                return await HolyQuestNowStat(connection, report, cancellationToken).ConfigureAwait(false);
            }
            case 16:
            {
                var result = new _account_db_info_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await AccountDBInfoResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 19:
            {
                var result = new _notify_manage_account_auth_info_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await NotifyManageAccountAuthInfo(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 21:
            {
                var result = new _manage_account_auth_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await ManageAccountAuthResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 23:
            {
                var result = new _manage_client_limit_run_account_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await ManageClientLimitRunAccountResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 24:
            {
                var result = new _manage_client_limit_run_world_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await ManageClientLimitRunWorldResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            case 26:
            {
                var result = new _manage_client_force_exit_result_aclo();
                if (!result.Load(packet.Payload))
                {
                    return false;
                }
                return await ManageClientForceExitResult(connection, result, cancellationToken).ConfigureAwait(false);
            }
            default:
                _log($"Unhandled account opcode 1/{sub}");
                return false;
        }
    }

    private Task<bool> WorldListResult(PublicConnection connection, _world_list_result_aclo worldList, CancellationToken cancellationToken)
    {
        var worlds = new List<WorldData>(worldList.byWorldNum);
        for (int i = 0; i < worldList.byWorldNum; i++)
        {
            var w = worldList.WorldList[i];
            string name = PacketStringUtil.ToAsciiNullTerm(w.szWorldName);
            string ipStr = new IPAddress(w.dwGateIP).ToString();
            _log($"World {i}: open={w.bOpen} name='{name}' ip={ipStr}:{w.wGatePort} type={w.byType}");
            worlds.Add(new WorldData
            {
                IsOpen = w.bOpen,
                Name = name,
                GateIp = new IPAddress(w.dwGateIP),
                GatePort = w.wGatePort,
                Type = w.byType,
                FreeServer = false
            });
        }

        _log($"WorldListResult: serviceWorldNum={worldList.byServiceWorldNum} worldNum={worldList.byWorldNum}");
        MainContext.Instance.UpdateWorldList(worldList.byServiceWorldNum, worlds);
        return Task.FromResult(true);
    }

    private Task<bool> InformOpenWorld(PublicConnection connection, _inform_open_world_aclo info, CancellationToken cancellationToken)
    {
        string gateIpStr = new IPAddress(info.dwGateIP).ToString();
        _log($"InformOpenWorld: worldCode={info.dwWorldCode} gate={gateIpStr}:{info.wGatePort}");
        MainContext.Instance.SetWorldOpen((int)info.dwWorldCode, new IPAddress(info.dwGateIP), info.wGatePort);
        return Task.FromResult(true);
    }

    private Task<bool> InformCloseWorld(PublicConnection connection, _inform_close_world_aclo info, CancellationToken cancellationToken)
    {
        _log($"InformCloseWorld: worldCode={info.dwWorldCode}");
        MainContext.Instance.SetWorldClosed((int)info.dwWorldCode);
        return Task.FromResult(true);
    }

    private Task<bool> InformUserNumWorld(PublicConnection connection, _inform_usernum_world_aclo info, CancellationToken cancellationToken)
    {
        _log($"InformUserNumWorld: serviceWorldNum={info.byServiceWorldNum} users[0]={info.wUserNum[0]} ...");
        MainContext.Instance.UpdateUserCounts(info.byServiceWorldNum, info.wUserNum);
        return Task.FromResult(true);
    }

    private Task<bool> JoinAccountResult(PublicConnection connection, _join_account_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"JoinAccountResult: index={result.idLocal.wIndex} ret={result.byRetCode}");
        MainContext.Instance.RecordJoinResult(result.idLocal.wIndex, result.byRetCode);

        var client = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);
        if (client != null)
        {
            var send = new _join_account_result_locl { byRetCode = result.byRetCode };
            var payload = new byte[1] { send.byRetCode };
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 2,
                Payload = payload
            };
            _ = client.SendAsync(env, cancellationToken);
        }
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountResult(PublicConnection connection, _login_account_result_aclo result, CancellationToken cancellationToken)
    {
        //_log($"LoginAccountResult: index={result.idLocal.wIndex} ret={result.byRetCode} accountSerial={result.dwAccountSerial} grade={result.byUserGrade}/{result.bySubGrade} nTrans={result.nTrans}");
        MainContext.Instance.RecordLoginResult(result.idLocal.wIndex, result.byRetCode, result.dwAccountSerial, result.byUserGrade, result.bySubGrade, result.nTrans);

        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var client = session?.Connection;// ?? MainContext.Instance.GetClientConnection(result.idLocal.wIndex);
        if (session == null || client == null)
            return Task.FromResult(true);
        // If the serial mismatches current session, send a logout request to account server and skip replying to client.
        if (client != null && session?.ClidSerial != result.idLocal.dwSerial)
        {
            var logout = new _logout_account_request_loac { gidGlobal = result.gidNewGlobal };
            var payloadLogout = new byte[Marshal.SizeOf<_logout_account_request_loac>()];
            MemoryMarshal.Write(payloadLogout.AsSpan(), in logout);
            var envLogout = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 9,
                Payload = payloadLogout
            };
            _ = client.SendAsync(envLogout, cancellationToken);
            return Task.FromResult(true);
        }
        if (session != null)
        {
            if (result.byRetCode == 5)
            {
                // SetOverlap
                session.OverlapUser = true;
                session.AccountSerial = result.dwAccountSerial;
            }
            else
            {
                // Mirror _CLIENT_DATA::Login state
                session.IsLogin = true;
                session.IsLoginChecked = true;
                session.RegisteredWorld = false;
                session.SelectedWorld = false;
                session.OverlapUser = false;
                session.SelectedWorldCode = -1;
                session.GlobalId = result.gidNewGlobal;
                session.AccountSerial = result.dwAccountSerial;
                session.UserGrade = result.byUserGrade;
                session.SubGrade = result.bySubGrade;
                session.Trans = result.nTrans;
            }
        }

        if (client != null)
        {
            var payload = _login_account_result_locl.FromAclos(result, session!.BillType);
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 4,
                Payload = payload
            };
            _ = client.SendAsync(env, cancellationToken);
        }
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldResult(PublicConnection connection, _select_world_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"SelectWorldResult: index={result.idLocal.wIndex} ret={result.byRetCode} masterKey[0]={result.dwWorldMasterKey[0]}");
        MainContext.Instance.RecordSelectWorldResult(result.idLocal.wIndex, result.byRetCode, result.dwWorldMasterKey);

        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var client = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);

        if (session == null || client == null || session.ClidSerial != result.idLocal.dwSerial)
        {
            return Task.FromResult(true);
        }

        uint gateIp = 0;
        ushort gatePort = 0;
        if (result.byRetCode == 0 && session.SelectedWorldCode >= 0)
        {
            var worlds = MainContext.Instance.Worlds;
            if (session.SelectedWorldCode < worlds.Count)
            {
                var w = worlds[session.SelectedWorldCode];
                gateIp = w.GateIp != null ? BitConverter.ToUInt32(w.GateIp.GetAddressBytes(), 0) : 0;
                gatePort = w.GatePort;
            }
        }

        var send = new _select_world_result_locl
        {
            byRetCode = result.byRetCode,
            dwWorldGateIP = gateIp,
            wWorldGatePort = gatePort,
            dwWorldMasterKey = result.dwWorldMasterKey,
            bAllowAltTab = false
        };
        var buffer = send.ToArray();
        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 8,
            Payload = buffer
        };
        _ = client.SendAsync(env, cancellationToken);

        return Task.FromResult(true);
    }

    private Task<bool> PushCloseResult(PublicConnection connection, _push_close_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"PushCloseResult: index={result.idLocal.wIndex} ret={result.byRetCode}");
        MainContext.Instance.RecordPushCloseResult(result.idLocal.wIndex, result.byRetCode);

        var client = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);
        if (client != null)
        {
            var send = new _push_close_result_locl { byRetCode = result.byRetCode };
            var payload = new byte[1] { send.byRetCode };
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 8,
                Payload = payload
            };
            _ = client.SendAsync(env, cancellationToken);
        }
        return Task.FromResult(true);
    }

    private Task<bool> ForceCloseCommand(PublicConnection connection, _force_close_command_aclo cmd, CancellationToken cancellationToken)
    {
        int maxConn = MainContext.Instance.MaxConnections;
        if (maxConn > 0 && cmd.idLocal.wIndex >= maxConn)
        {
            _log($"ForceCloseCommand: invalid index {cmd.idLocal.wIndex}");
            return Task.FromResult(false);
        }

        _log($"ForceCloseCommand: index={cmd.idLocal.wIndex} serial={cmd.idLocal.dwSerial}");

        var session = MainContext.Instance.GetClient(cmd.idLocal.wIndex);
        var target = MainContext.Instance.GetClientConnection(cmd.idLocal.wIndex);
        if (session == null || target == null || session.ClidSerial != cmd.idLocal.dwSerial)
        {
            // Serial mismatch or missing client: do nothing (native only closes on match).
            return Task.FromResult(true);
        }

        // Notify client and close.
        var notify = new _server_notify_inform_locl { wMsgCode = 1 };
        var payload = new byte[Marshal.SizeOf<_server_notify_inform_locl>()];
        MemoryMarshal.Write(payload.AsSpan(), in notify);
        var env = new PacketEnvelope
        {
            OpCode = 21,
            SubCode = 11,
            Payload = payload
        };
        _ = target.SendAsync(env, cancellationToken);

        MainContext.Instance.RecordForceClose(cmd.idLocal.wIndex);
        try
        {
            target.Close();
        }
        catch (Exception ex)
        {
            _log($"ForceCloseCommand: error closing connection {cmd.idLocal.wIndex}: {ex.Message}");
        }
        return Task.FromResult(true);
    }

    private Task<bool> LoginStatRequest(PublicConnection connection, _login_server_stat_request_aclo request, CancellationToken cancellationToken)
    {
        bool externalOpen = request.byStat switch
        {
            1 => true,
            2 => false,
            _ => false
        };

        _log($"LoginStatRequest: clientIndex={request.wClientIndex} stat={request.byStat} => externalOpen={externalOpen}");
        MainContext.Instance.ExternalOpen = externalOpen;

        // Send response to the client indexed by wClientIndex if connected.
        var target = MainContext.Instance.GetClientConnection(request.wClientIndex);
        if (target != null)
        {
            byte byRet = externalOpen ? (byte)1 : (byte)2;
            Span<byte> payload = stackalloc byte[3];
            payload[0] = byRet;
            BinaryPrimitives.WriteUInt16LittleEndian(payload.Slice(1, 2), request.wClientIndex);
            var env = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 13,
                Payload = payload.ToArray()
            };
            _ = target.SendAsync(env, cancellationToken);
        }
        return Task.FromResult(true);
    }

    private Task<bool> HolyQuestNowStat(PublicConnection connection, _holy_quest_now_report_aclo report, CancellationToken cancellationToken)
    {
        // Native implementation is a no-op; we just log receipt.
        _log($"HolyQuestNowStat: worldCode={report.wWorldCode} masterRace={report.byMasterRaceCode}");
        return Task.FromResult(true);
    }

    private Task<bool> AccountDBInfoResult(PublicConnection connection, _account_db_info_result_aclo result, CancellationToken cancellationToken)
    {
        string dbName = PacketStringUtil.ToAscii(result.szDBName);
        string ip = PacketStringUtil.ToAscii(result.szIP);
        MainContext.Instance.RecordAccountDbInfo(dbName, ip);
        _log($"AccountDBInfoResult: dbName={dbName} ip={ip}");
        return Task.FromResult(true);
    }

    private Task<bool> NotifyManageAccountAuthInfo(PublicConnection connection, _notify_manage_account_auth_info_aclo result, CancellationToken cancellationToken)
    {
        _log($"NotifyManageAccountAuthInfo: index={result.idLocal.wIndex} ret={result.byRetCode}");

        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var target = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);

        if (session != null && target != null)
        {
            session.ManageAccountAuthRet = result.byRetCode;

            // forward key blob to the client (op 21/19)
            var sendPayload = new byte[result.byKey.Length];
            Buffer.BlockCopy(result.byKey, 0, sendPayload, 0, sendPayload.Length);
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 19,
                Payload = sendPayload
            };
            _ = target.SendAsync(env, cancellationToken);

            if (result.byRetCode != 0)
            {
                session.ForcedClosed = true;
            }
            return Task.FromResult(true);
        }

        // Session not found or not connected: tell account line it failed (op 1/18).
        var reject = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 18,
            Payload = Array.Empty<byte>()
        };
        _ = connection.SendAsync(reject, cancellationToken);
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthResult(PublicConnection connection, _manage_account_auth_result_aclo result, CancellationToken cancellationToken)
    {
        int maxConn = MainContext.Instance.MaxConnections;
        if (maxConn > 0 && result.idLocal.wIndex >= maxConn)
        {
            _log($"ManageAccountAuthResult: invalid index {result.idLocal.wIndex}");
            return Task.FromResult(false);
        }

        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var clientConn = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);

        _log($"ManageAccountAuthResult: index={result.idLocal.wIndex} ret={result.byRet}");

        if (session != null)
        {
            session.ManageAccountAuthRet = result.byRet;
            if (result.byRet != 0)
            {
                session.ForcedClosed = true;
            }
        }

        if (clientConn != null)
        {
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 21,
                Payload = new[] { result.byRet }
            };
            _ = clientConn.SendAsync(env, cancellationToken);
        }

        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunAccountResult(PublicConnection connection, _manage_client_limit_run_account_result_aclo result, CancellationToken cancellationToken)
    {
        int maxConn = MainContext.Instance.MaxConnections;
        if (maxConn > 0 && result.idLocal.wIndex >= maxConn)
        {
            _log($"ManageClientLimitRunAccountResult: invalid index {result.idLocal.wIndex}");
            return Task.FromResult(false);
        }

        _log($"ManageClientLimitRunAccountResult: index={result.idLocal.wIndex} ret={result.byRet}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var clientConn = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);

        if (session != null)
        {
            session.ManageLimitRunAccountRet = result.byRet;
        }

        if (clientConn != null)
        {
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 23,
                Payload = new[] { result.byRet }
            };
            _ = clientConn.SendAsync(env, cancellationToken);
        }
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunWorldResult(PublicConnection connection, _manage_client_limit_run_world_result_aclo result, CancellationToken cancellationToken)
    {
        int maxConn = MainContext.Instance.MaxConnections;
        if (maxConn > 0 && result.idLocal.wIndex >= maxConn)
        {
            _log($"ManageClientLimitRunWorldResult: invalid index {result.idLocal.wIndex}");
            return Task.FromResult(false);
        }

        string name = PacketStringUtil.ToAscii(result.m_szName);
        string db = PacketStringUtil.ToAscii(result.m_szDBName);
        _log($"ManageClientLimitRunWorldResult: index={result.idLocal.wIndex} ret={result.byRet} code={result.m_dwCode} name={name} db={db} type={result.m_byType}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var clientConn = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);
        if (session != null)
        {
            session.ManageLimitWorld = new ManageLimitWorldInfo
            {
                Code = result.m_dwCode,
                Name = name,
                DbName = db,
                Type = result.m_byType
            };
            session.ManageLimitRunAccountRet = result.byRet;
        }

        if (clientConn != null)
        {
            var payload = new byte[70];
            payload[0] = result.byRet;
            payload[1] = result.m_byType;
            BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(2, 4), result.m_dwCode);
            Buffer.BlockCopy(result.m_szName, 0, payload, 6, 32);
            Buffer.BlockCopy(result.m_szDBName, 0, payload, 38, 32);

            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 24,
                Payload = payload
            };
            _ = clientConn.SendAsync(env, cancellationToken);
        }
        else
        {
            var reject = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 18,
                Payload = Array.Empty<byte>()
            };
            _ = connection.SendAsync(reject, cancellationToken);
        }
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitResult(PublicConnection connection, _manage_client_force_exit_result_aclo result, CancellationToken cancellationToken)
    {
        int maxConn = MainContext.Instance.MaxConnections;
        if (maxConn > 0 && result.idLocal.wIndex >= maxConn)
        {
            _log($"ManageClientForceExitResult: invalid index {result.idLocal.wIndex}");
            return Task.FromResult(false);
        }

        _log($"ManageClientForceExitResult: index={result.idLocal.wIndex} ret={result.byRet}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        var clientConn = MainContext.Instance.GetClientConnection(result.idLocal.wIndex);
        if (session != null)
        {
            session.ManageForceExitRet = result.byRet;
        }
        if (clientConn != null)
        {
            var env = new PacketEnvelope
            {
                OpCode = 21,
                SubCode = 26,
                Payload = new[] { result.byRet }
            };
            _ = clientConn.SendAsync(env, cancellationToken);
        }
        else
        {
            var reject = new PacketEnvelope
            {
                OpCode = 1,
                SubCode = 18,
                Payload = Array.Empty<byte>()
            };
            _ = connection.SendAsync(reject, cancellationToken);
        }
        return Task.FromResult(true);
    }

}
