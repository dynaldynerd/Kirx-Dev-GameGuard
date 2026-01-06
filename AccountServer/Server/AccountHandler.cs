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
            _ => Task.FromResult(false)
        };
    }

    private Task<bool> JoinAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] JoinAccountRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LoginAccountRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] SelectWorldRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> PushCloseRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] PushCloseRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> CloseAccountFromLoginRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] CloseAccountFromLoginRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> WorldListRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] WorldListRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginServerStatResult(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LoginServerStatResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> AccountDbInfoRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] AccountDBInfoRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> UpdateUserLoginFailureCntRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] UpdateUserLoginFailureCntRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> LogoutManageAccountRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] LogoutManageAccountRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageAccountAuthRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageClientLimitRunRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken token)
    {
        _log($"[{connection.ConnectionId}] ManageClientForceExitRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }
}
