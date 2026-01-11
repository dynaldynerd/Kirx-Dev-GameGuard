using System;
using System.Text;

namespace LoginServer.Packets;

public static class PacketStringUtil
{
    public static string ToAscii(byte[] bytes) => Encoding.ASCII.GetString(bytes).TrimEnd('\0');

    public static string ToAsciiNullTerm(byte[] bytes)
    {
        int len = Array.IndexOf(bytes, (byte)0);
        if (len < 0) len = bytes.Length;
        return Encoding.ASCII.GetString(bytes, 0, len);
    }

    public static void FillFixed(byte[] target, string value)
    {
        Array.Clear(target, 0, target.Length);
        var bytes = Encoding.ASCII.GetBytes(value ?? string.Empty);
        int copy = Math.Min(bytes.Length, target.Length);
        Buffer.BlockCopy(bytes, 0, target, 0, copy);
        if (copy < target.Length)
        {
            target[copy] = 0;
        }
    }
}

public struct _GLBID
{
    public uint dwIndex;
    public uint dwSerial;
}

public struct _CLID
{
    public ushort wIndex;
    public uint dwSerial;
}

public struct _SYSTEMTIME
{
    public ushort wYear;
    public ushort wMonth;
    public ushort wDayOfWeek;
    public ushort wDay;
    public ushort wHour;
    public ushort wMinute;
    public ushort wSecond;
    public ushort wMilliseconds;
}

