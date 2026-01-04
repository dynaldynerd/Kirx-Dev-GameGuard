using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Net;
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
                return await HolyQuestNowStat(connection, cancellationToken).ConfigureAwait(false);
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
            string name = PacketStringUtil.ToAscii(w.szWorldName);
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
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountResult(PublicConnection connection, _login_account_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"LoginAccountResult: index={result.idLocal.wIndex} ret={result.byRetCode} accountSerial={result.dwAccountSerial} grade={result.byUserGrade}/{result.bySubGrade} nTrans={result.nTrans}");
        MainContext.Instance.RecordLoginResult(result.idLocal.wIndex, result.byRetCode, result.dwAccountSerial, result.byUserGrade, result.bySubGrade, result.nTrans);
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldResult(PublicConnection connection, _select_world_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"SelectWorldResult: index={result.idLocal.wIndex} ret={result.byRetCode} masterKey[0]={result.dwWorldMasterKey[0]}");
        MainContext.Instance.RecordSelectWorldResult(result.idLocal.wIndex, result.byRetCode, result.dwWorldMasterKey);
        return Task.FromResult(true);
    }

    private Task<bool> PushCloseResult(PublicConnection connection, _push_close_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"PushCloseResult: index={result.idLocal.wIndex} ret={result.byRetCode}");
        MainContext.Instance.RecordPushCloseResult(result.idLocal.wIndex, result.byRetCode);
        return Task.FromResult(true);
    }

    private Task<bool> ForceCloseCommand(PublicConnection connection, _force_close_command_aclo cmd, CancellationToken cancellationToken)
    {
        _log($"ForceCloseCommand: index={cmd.idLocal.wIndex}");
        MainContext.Instance.RecordForceClose(cmd.idLocal.wIndex);
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

    private Task<bool> HolyQuestNowStat(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log("HolyQuestNowStat: no-op");
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
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthResult(PublicConnection connection, _manage_account_auth_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"ManageAccountAuthResult: index={result.idLocal.wIndex} ret={result.byRet}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        if (session != null)
        {
            session.ManageAccountAuthRet = result.byRet;
        }
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunAccountResult(PublicConnection connection, _manage_client_limit_run_account_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"ManageClientLimitRunAccountResult: index={result.idLocal.wIndex} ret={result.byRet}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        if (session != null)
        {
            session.ManageLimitRunAccountRet = result.byRet;
        }
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunWorldResult(PublicConnection connection, _manage_client_limit_run_world_result_aclo result, CancellationToken cancellationToken)
    {
        string name = PacketStringUtil.ToAscii(result.m_szName);
        string db = PacketStringUtil.ToAscii(result.m_szDBName);
        _log($"ManageClientLimitRunWorldResult: index={result.idLocal.wIndex} ret={result.byRet} code={result.m_dwCode} name={name} db={db} type={result.m_byType}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        if (session != null)
        {
            session.ManageLimitWorld = new ManageLimitWorldInfo
            {
                Code = result.m_dwCode,
                Name = name,
                DbName = db,
                Type = result.m_byType
            };
        }
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitResult(PublicConnection connection, _manage_client_force_exit_result_aclo result, CancellationToken cancellationToken)
    {
        _log($"ManageClientForceExitResult: index={result.idLocal.wIndex} ret={result.byRet}");
        var session = MainContext.Instance.GetClient(result.idLocal.wIndex);
        if (session != null)
        {
            session.ManageForceExitRet = result.byRet;
        }
        return Task.FromResult(true);
    }

}
