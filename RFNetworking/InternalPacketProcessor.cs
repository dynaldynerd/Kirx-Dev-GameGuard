using System;
using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>
/// Handles built-in internal packets (opcodes > 100) that should bypass server handlers.
/// Mirrors the native speed-hack handshake logic in a simplified form.
/// </summary>
internal sealed class InternalPacketProcessor
{
    private readonly Action<string>? _log;
    private readonly byte[] _speedHackKey = new byte[4];
    private int _keySet; // 0 not set, 1 set
    private int _sendSpeedHackTime;
    private int _contCount;

    public InternalPacketProcessor(Action<string>? log)
    {
        _log = log;
    }

    /// <summary>
    /// Returns true if the packet was handled internally and should not reach server handlers.
    /// </summary>
    public async Task<bool> HandleAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (packet.OpCode == 101)
        {
            // Accept internal keepalive/control messages (subcodes 1/2/3).
            if (packet.SubCode is 1 or 2 or 3)
            {
                return true;
            }
            return false;
        }

        if (packet.OpCode != 102)
        {
            return false;
        }

        if (packet.SubCode == 2)
        {
            // Receive speed-hack key (4 bytes).
            if (packet.Payload.Length >= 4)
            {
                Array.Copy(packet.Payload, _speedHackKey, 4);
                _keySet = 1;
                _sendSpeedHackTime = Environment.TickCount;
            }
            return true;
        }

        if (packet.SubCode != 3)
        {
            return false;
        }

        if (_keySet == 0 || packet.Payload.Length < 4)
        {
            return true; // ignore if not initialized
        }

        bool keyMatches = true;
        for (int i = 0; i < 4; i++)
        {
            if (packet.Payload[i] != _speedHackKey[i])
            {
                keyMatches = false;
                break;
            }
        }

        if (!keyMatches)
        {
            _log?.Invoke($"SpeedHack key mismatch from {connection.RemoteEndPoint}");
            return true;
        }

        int elapsed = unchecked(Environment.TickCount - _sendSpeedHackTime);
        if (elapsed < 0) elapsed = int.MaxValue;

        // Simplified thresholds from native logic (approx 4500-5000ms windows).
        if (elapsed < 4500)
        {
            _log?.Invoke($"SpeedHack timing violation ({elapsed}ms) from {connection.RemoteEndPoint}");
            return true;
        }

        if (elapsed > 5000)
        {
            await SendSpeedHackCheckAsync(connection, cancellationToken).ConfigureAwait(false);
            return true;
        }

        if (++_contCount > 5)
        {
            _log?.Invoke($"SpeedHack repeated timing issue ({elapsed}ms) from {connection.RemoteEndPoint}");
            return true;
        }

        await SendSpeedHackCheckAsync(connection, cancellationToken).ConfigureAwait(false);
        return true;
    }

    private Task SendSpeedHackCheckAsync(PublicConnection connection, CancellationToken cancellationToken)
    {
        // Native code would send a speed-hack challenge; here we just log.
        _log?.Invoke($"SpeedHack check request to {connection.RemoteEndPoint}");
        return Task.CompletedTask;
    }
}
