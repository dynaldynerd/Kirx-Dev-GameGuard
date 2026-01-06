using System;
using System.Text;

namespace RFPackets.Login;

public static class PacketStringUtil
{
    public static string ToAscii(byte[] bytes) => ToAscii(bytes.AsSpan());

    public static string ToAscii(ReadOnlySpan<byte> bytes)
    {
        int len = bytes.IndexOf((byte)0);
        if (len < 0) len = bytes.Length;
        return Encoding.ASCII.GetString(bytes.Slice(0, len));
    }

    public static string ToAsciiNullTerm(byte[] bytes) => ToAscii(bytes);

    public static void GetBytes(string? value, Span<byte> dest)
    {
        dest.Clear();
        if (string.IsNullOrEmpty(value)) return;
        var encoded = Encoding.ASCII.GetBytes(value);
        int count = Math.Min(dest.Length - 1, encoded.Length);
        encoded.AsSpan(0, count).CopyTo(dest);
        // ensure null terminator left at end via Clear/unused bytes.
    }
}
