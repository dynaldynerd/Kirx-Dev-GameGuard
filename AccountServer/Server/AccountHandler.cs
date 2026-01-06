using RFNetworking;
using LoginServer.Packets;

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
        return DispatchLoginLineAsync(connection, packet, cancellationToken);
    }

    private Task DispatchLoginLineAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        if (packet.OpCode != 1)
        {
            // Accept internal ping 40/1
            if (packet.OpCode == 40 && packet.SubCode == 1) return Task.CompletedTask;
            _log($"[{connection.ConnectionId}] unknown op={packet.OpCode} sub={packet.SubCode}");
            return Task.CompletedTask;
        }

        return packet.SubCode switch
        {
            1 => JoinAccountRequest(connection, packet, token),
            3 => LoginAccountRequest(connection, packet, token),
            5 => SelectWorldRequest(connection, packet, token),
            7 => PushCloseRequest(connection, packet, token),
            9 => CloseAccountFromLoginRequest(connection, packet, token),
            10 => WorldListRequest(connection, packet, token),
            13 => LoginServerStatResult(connection, packet, token),
            15 => AccountDbInfoRequest(connection, packet, token),
            17 => UpdateUserLoginFailureCntRequest(connection, packet, token),
            18 => LogoutManageAccountRequest(connection, packet, token),
            20 => ManageAccountAuthRequest(connection, packet, token),
            22 => ManageClientLimitRunRequest(connection, packet, token),
            25 => ManageClientForceExitRequest(connection, packet, token),
            _ => Task.CompletedTask
        };
    }

    private Task JoinAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] JoinAccountRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task LoginAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LoginAccountRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task SelectWorldRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] SelectWorldRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task PushCloseRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] PushCloseRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task CloseAccountFromLoginRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] CloseAccountFromLoginRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task WorldListRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] WorldListRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task LoginServerStatResult(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LoginServerStatResult len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task AccountDbInfoRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] AccountDBInfoRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task UpdateUserLoginFailureCntRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] UpdateUserLoginFailureCntRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task LogoutManageAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LogoutManageAccountRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task ManageAccountAuthRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageAccountAuthRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task ManageClientLimitRunRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageClientLimitRunRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }

    private Task ManageClientForceExitRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageClientForceExitRequest len={packet.Payload.Length}");
        return Task.CompletedTask;
    }
}
