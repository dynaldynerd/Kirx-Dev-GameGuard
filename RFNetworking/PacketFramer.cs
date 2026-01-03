using System;
using System.Buffers.Binary;

namespace RFNetworking;

/// <summary>Packet framing helpers (size + op + sub + payload).</summary>
public static class PacketFramer
{
    public const int HeaderSize = 4; // 2 bytes length + 1 byte opcode + 1 byte subcode
    public const int MaxBodyBytes = 60000;

    public static bool TryExtractPacket(ReadOnlySpan<byte> buffer, out PacketEnvelope? packet, out int consumed)
    {
        packet = null;
        consumed = 0;

        if (buffer.Length < HeaderSize)
        {
            return false; // not enough data
        }

        ushort totalSize = BinaryPrimitives.ReadUInt16LittleEndian(buffer);
        if (totalSize < HeaderSize)
        {
            throw new InvalidOperationException($"Packet size {totalSize} smaller than header.");
        }

        if (totalSize - HeaderSize > MaxBodyBytes)
        {
            throw new InvalidOperationException($"Packet body {totalSize - HeaderSize} exceeds max {MaxBodyBytes} bytes.");
        }

        if (buffer.Length < totalSize)
        {
            return false; // wait for more
        }

        byte opCode = buffer[2];
        byte subCode = buffer[3];
        byte[] payload = Array.Empty<byte>();
        int bodyLen = totalSize - HeaderSize;
        if (bodyLen > 0)
        {
            payload = buffer.Slice(HeaderSize, bodyLen).ToArray();
        }

        packet = new PacketEnvelope
        {
            OpCode = opCode,
            SubCode = subCode,
            Payload = payload
        };
        consumed = totalSize;
        return true;
    }

    public static int WritePacket(Span<byte> targetBuffer, PacketEnvelope packet)
    {
        var payload = packet.Payload ?? Array.Empty<byte>();
        if (payload.Length > MaxBodyBytes)
        {
            throw new ArgumentOutOfRangeException(nameof(packet.Payload), $"Payload exceeds {MaxBodyBytes} bytes.");
        }

        ushort totalSize = (ushort)(payload.Length + HeaderSize);
        if (targetBuffer.Length < totalSize)
        {
            throw new ArgumentException("Target buffer too small for packet.");
        }

        BinaryPrimitives.WriteUInt16LittleEndian(targetBuffer, totalSize);
        targetBuffer[2] = packet.OpCode;
        targetBuffer[3] = packet.SubCode;
        payload.AsSpan().CopyTo(targetBuffer.Slice(HeaderSize));

        return totalSize;
    }
}
