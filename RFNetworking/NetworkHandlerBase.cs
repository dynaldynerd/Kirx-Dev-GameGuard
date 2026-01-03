using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>Convenience base class with no-op implementations.</summary>
public abstract class NetworkHandlerBase : INetworkHandler
{
    public virtual Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken) => Task.CompletedTask;

    public virtual Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken) => Task.CompletedTask;

    public virtual Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken) => Task.CompletedTask;

    public virtual Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken) => Task.CompletedTask;
}
