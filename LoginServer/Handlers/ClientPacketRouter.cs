using System;
using System.Threading;
using System.Threading.Tasks;
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
                return await JoinAccountRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 3:
                return await LoginAccountRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 5:
                return await WorldListRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 7:
                return await SelectWorldRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 9:
                return await PushCloseRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 12:
                return await CryptKeyRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 15:
                // Allowed no-op for nation 410 in the native implementation.
                if (NationCode == 410)
                {
                    _log("Client opcode 21/15 accepted (nation 410 noop).");
                    return true;
                }
                return false;
            case 17:
                return await MotpValidationRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 20:
                return await ManageAccountAuthRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 22:
                return await ManageClientLimitRunRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 25:
                return await ManageClientForceExitRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            default:
                _log($"Unhandled client opcode 21/{sub}");
                return false;
        }
    }

    private Task<bool> JoinAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"JoinAccountRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"LoginAccountRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> WorldListRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"WorldListRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"SelectWorldRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> PushCloseRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"PushCloseRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> CryptKeyRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"CryptKeyRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> MotpValidationRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"MotpValidationRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"ManageAccountAuthRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"ManageClientLimitRunRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"ManageClientForceExitRequest from {connection.RemoteEndPoint} len={packet.Payload.Length}");
        return Task.FromResult(true);
    }
}
