using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>Connection view exposed to handlers.</summary>
public sealed class PublicConnection
{
    private readonly SaeaConnection _inner;
    private readonly Socket _socket;

    internal PublicConnection(SaeaConnection inner, Socket socket)
    {
        _inner = inner;
        _socket = socket;
    }

    public int ConnectionId => _inner.ConnectionId;

    public EndPoint? RemoteEndPoint => _socket.RemoteEndPoint;

    public bool IsConnected => _socket.Connected;

    public Task SendAsync(PacketEnvelope packet, CancellationToken cancellationToken) => _inner.SendAsync(packet, cancellationToken);
}
