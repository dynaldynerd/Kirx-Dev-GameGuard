using System.Buffers.Binary;

namespace LoginServer.Packets;

// Control-line packets (acco/coac).
public struct _login_server_stat_result_acco
{
    public byte byRet;
    public ushort wClientIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        byRet = payload[0];
        wClientIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[3];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), wClientIndex);
        return buffer;
    }
}
