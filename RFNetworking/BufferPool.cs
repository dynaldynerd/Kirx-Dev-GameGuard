using System;
using System.Buffers;

namespace RFNetworking;

/// <summary>Pooled buffers to reduce per-connection allocations.</summary>
internal static class BufferPool
{
    private const int DefaultBufferSize = 128 * 1024; // big enough for max packet plus headroom
    private static readonly ArrayPool<byte> Pool = ArrayPool<byte>.Shared;

    public static byte[] Rent(int minimumSize = DefaultBufferSize) => Pool.Rent(minimumSize);

    public static void Return(byte[] buffer)
    {
        if (buffer == null) return;
        Pool.Return(buffer);
    }
}
