using AccountServer.Data;
using AccountServer.Settings;
using RFNetworking;

namespace AccountServer.Server;

public sealed class ControlHandler : AccountHandlerBase
{
    public ControlHandler(
        Action<string> log,
        AppSettings settings,
        string? connectionString = null,
        IAccountDatabase? db = null)
        : base(log, settings, connectionString, db)
    {
    }

    public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] connected from {connection.RemoteEndPoint} (control)");
        return Task.CompletedTask;
    }

    public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        _log($"[{connection.ConnectionId}] disconnected (control)");
        return Task.CompletedTask;
    }

    public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        return Task.CompletedTask;
    }

    public override Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        bool ok = packet.OpCode == 40 && packet.SubCode == 1;
        if (!ok)
        {
            _log($"[{connection.ConnectionId}] control handler returned false; disconnecting.");
            connection.Close();
        }
        return Task.CompletedTask;
    }
}
