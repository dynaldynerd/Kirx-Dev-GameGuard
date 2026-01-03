using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>
/// Contract for handling network events. Packets with OpCode <= 100 go to OnPacketAsync (DataAnalysis).
/// Packets with OpCode > 100 are treated as internal and go to OnInternalPacketAsync.
/// </summary>
public interface INetworkHandler
{
    Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken);
    Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken);
    Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken);
    Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken);
}
