using System;

namespace RFNetworking;

/// <summary>
/// Represents a framed packet: size (2 bytes, little endian), opcode (1 byte), subcode (1 byte), payload.
/// </summary>
public sealed class PacketEnvelope
{
    public byte OpCode { get; init; }
    public byte SubCode { get; init; }
    public byte[] Payload { get; init; } = Array.Empty<byte>();

    /// <summary>Total packet size including header and payload.</summary>
    public ushort PacketSize => (ushort)((Payload?.Length ?? 0) + 4);
}
