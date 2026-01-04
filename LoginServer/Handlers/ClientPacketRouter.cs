using System;
using System.Threading;
using System.Threading.Tasks;
using LoginServer.Packets;
using RFNetworking;

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
        _log($"JoinAccountRequest from {connection.RemoteEndPoint}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountRequest(PublicConnection connection, _login_account_request_cllo request, CancellationToken cancellationToken)
    {
        _log($"LoginAccountRequest from {connection.RemoteEndPoint}");
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
        _log($"CryptKeyRequest from {connection.RemoteEndPoint}");
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
}
