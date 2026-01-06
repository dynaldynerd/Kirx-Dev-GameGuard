using RFNetworking;

namespace AccountServer.Server;

/// <summary>
/// Minimal handler that logs connections and ignores packets for now.
/// </summary>
public sealed class AccountHandler : NetworkHandlerBase
{
    private readonly Action<string> _log;

    public AccountHandler(Action<string> log)
    {
        _log = log;
    }

    public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] connected from {connection.RemoteEndPoint}");
        return Task.CompletedTask;
    }

    public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] disconnected");
        return Task.CompletedTask;
    }

    public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    public override Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] packet op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length} (no handler yet)");
        return Task.CompletedTask;
    }
}
