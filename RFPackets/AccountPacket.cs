using System.Buffers.Binary;
using System.Runtime.InteropServices;

namespace LoginServer.Packets;

/// <summary>Account -> Login packets (_aclo).</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _join_account_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 7) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRetCode = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _holy_quest_now_report_aclo
{
    public ushort wWorldCode;
    public byte byMasterRaceCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        wWorldCode = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        byMasterRaceCode = payload[2];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 2)]
public struct _inform_open_world_aclo
{
    public uint dwWorldCode;
    public uint dwGateIP;
    public ushort wGatePort;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 10) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload);
        dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4));
        wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(8));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _login_server_stat_request_aclo
{
    public byte byStat;
    public ushort wClientIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        byStat = payload[0];
        wClientIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _inform_usernum_world_aclo
{
    public byte byServiceWorldNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
    public ushort[] wUserNum;

    public _inform_usernum_world_aclo()
    {
        this = default;
        wUserNum = new ushort[40];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1 + 40 * 2) return false;
        byServiceWorldNum = payload[0];
        wUserNum = new ushort[40];
        var span = payload.AsSpan(1);
        for (int i = 0; i < 40; i++)
        {
            wUserNum[i] = BinaryPrimitives.ReadUInt16LittleEndian(span.Slice(i * 2, 2));
        }
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _login_account_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
    public _GLBID gidNewGlobal;
    public uint dwAccountSerial;
    public byte byUserGrade;
    public byte bySubGrade;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] uszBlockReason;

    public int nTrans;
    public byte byBlockReasonType;
    public byte byCancelUILockBlockRet;

    public _login_account_result_aclo()
    {
        this = default;
        uszBlockReason = new byte[32];
    }

    public byte[] ToArray()
    {
        int size = Marshal.SizeOf<_login_account_result_aclo>();
        var buffer = new byte[size];
        var span = new Span<byte>(buffer);
        MemoryMarshal.Write(span, in this);
        return buffer;
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 59) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRetCode = payload[6];
        gidNewGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(11, 4))
        };
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(15, 4));
        byUserGrade = payload[19];
        bySubGrade = payload[20];
        Buffer.BlockCopy(payload, 21, uszBlockReason, 0, 32);
        nTrans = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(53, 4));
        byBlockReasonType = payload[57];
        byCancelUILockBlockRet = payload[58];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _account_db_info_result_aclo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] szDBName;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public byte[] szIP;

    public _account_db_info_result_aclo()
    {
        this = default;
        szDBName = new byte[32];
        szIP = new byte[16];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 48) return false;
        Buffer.BlockCopy(payload, 0, szDBName, 0, 32);
        Buffer.BlockCopy(payload, 32, szIP, 0, 16);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _world_list_result_aclo
{
    public byte byServiceWorldNum;
    public byte byWorldNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
    public _world_list_result_aclo__list[] WorldList;

    public _world_list_result_aclo()
    {
        this = default;
        WorldList = new _world_list_result_aclo__list[40];
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct _world_list_result_aclo__list
    {
        [MarshalAs(UnmanagedType.U1)]
        public bool bOpen;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 33)]
        public byte[] szWorldName;

        public uint dwGateIP;
        public ushort wGatePort;
        public byte byType;

        public _world_list_result_aclo__list()
        {
            this = default;
            szWorldName = new byte[33];
        }
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        byServiceWorldNum = payload[0];
        byWorldNum = payload[1];
        if (byWorldNum > 40) return false;
        int entrySize = 41;
        int required = 2 + byWorldNum * entrySize;
        if (payload.Length < required) return false;

        int offset = 2;
        for (int i = 0; i < byWorldNum; i++)
        {
            var entry = new _world_list_result_aclo__list
            {
                bOpen = payload[offset] != 0
            };
            Buffer.BlockCopy(payload, offset + 1, entry.szWorldName, 0, 33);
            entry.dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset + 34, 4));
            entry.wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 38, 2));
            entry.byType = payload[offset + 40];
            WorldList[i] = entry;
            offset += entrySize;
        }

        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _force_close_command_aclo
{
    public _CLID idLocal;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 6) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _push_close_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 7) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRetCode = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _inform_close_world_aclo
{
    public uint dwWorldCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _select_world_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
    public uint[] dwWorldMasterKey;

    public _select_world_result_aclo()
    {
        this = default;
        dwWorldMasterKey = new uint[4];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 23) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRetCode = payload[6];
        dwWorldMasterKey = new uint[4];
        for (int i = 0; i < 4; i++)
        {
            dwWorldMasterKey[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7 + i * 4, 4));
        }
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _notify_manage_account_auth_info_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 149)]
    public byte[] byKey;

    public _notify_manage_account_auth_info_aclo()
    {
        this = default;
        byKey = new byte[149];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3 + 149) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRetCode = payload[2];
        Buffer.BlockCopy(payload, 3, byKey, 0, 149);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_account_result_aclo
{
    public _CLID idLocal;
    public byte byRet;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 7) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRet = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_force_exit_result_aclo
{
    public _CLID idLocal;
    public byte byRet;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 7) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRet = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_world_result_aclo
{
    public _CLID idLocal;
    public byte byRet;
    public uint m_dwCode;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] m_szName;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] m_szDBName;

    public byte m_byType;

    public _manage_client_limit_run_world_result_aclo()
    {
        this = default;
        m_szName = new byte[32];
        m_szDBName = new byte[32];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3 + 4 + 32 + 32 + 1) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRet = payload[2];
        m_dwCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(3, 4));
        Buffer.BlockCopy(payload, 7, m_szName, 0, 32);
        Buffer.BlockCopy(payload, 39, m_szDBName, 0, 32);
        m_byType = payload[71];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_account_auth_result_aclo
{
    public _CLID idLocal;
    public byte byRet;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 7) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRet = payload[6];
        return true;
    }
}
