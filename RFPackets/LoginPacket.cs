using System.Buffers.Binary;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace LoginServer.Packets;

#region Login -> Account (loac) and Login -> Client (locl)

// loac
[StructLayout(LayoutKind.Sequential, Pack = 2)]
public struct _select_world_request_loac
{
    public _GLBID gidGlobal;
    public ushort wWorldIndex;
    public uint dwClientIP;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public uint[] dwRequestMoveCharacterSerialList;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public uint[] dwRTournamentCharacterSerialList;

    public _select_world_request_loac()
    {
        this = default;
        dwRequestMoveCharacterSerialList = new uint[3];
        dwRTournamentCharacterSerialList = new uint[3];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 38) return false;
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4, 4))
        };
        wWorldIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(8, 2));
        dwClientIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(10, 4));
        dwRequestMoveCharacterSerialList = new uint[3];
        dwRTournamentCharacterSerialList = new uint[3];
        int offset = 14;
        for (int i = 0; i < 3; i++)
        {
            dwRequestMoveCharacterSerialList[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
            offset += 4;
        }
        for (int i = 0; i < 3; i++)
        {
            dwRTournamentCharacterSerialList[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
            offset += 4;
        }
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct _select_world_request_loac_blit
{
    public _GLBID gidGlobal;
    public ushort wWorldIndex;
    public uint dwClientIP;
    public fixed uint dwRequestMoveCharacterSerialList[3];
    public fixed uint dwRTournamentCharacterSerialList[3];

    public static _select_world_request_loac_blit From(_select_world_request_loac src)
    {
        var dst = new _select_world_request_loac_blit
        {
            gidGlobal = src.gidGlobal,
            wWorldIndex = src.wWorldIndex,
            dwClientIP = src.dwClientIP
        };
        for (int i = 0; i < 3; i++)
        {
            dst.dwRequestMoveCharacterSerialList[i] = src.dwRequestMoveCharacterSerialList != null && i < src.dwRequestMoveCharacterSerialList.Length
                ? src.dwRequestMoveCharacterSerialList[i] : 0;
            dst.dwRTournamentCharacterSerialList[i] = src.dwRTournamentCharacterSerialList != null && i < src.dwRTournamentCharacterSerialList.Length
                ? src.dwRTournamentCharacterSerialList[i] : 0;
        }
        return dst;
    }

    public byte[] ToArray()
    {
        int size = Unsafe.SizeOf<_select_world_request_loac_blit>();
        var buffer = new byte[size];
        fixed (byte* dst = buffer)
        {
            fixed (_select_world_request_loac_blit* src = &this)
            {
                Unsafe.CopyBlockUnaligned(dst, src, (uint)size);
            }
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _login_account_request_loac
{
    public _CLID idLocal;
    public byte byUserCode;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szAccountID;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szPassword;

    public uint dwClientIP;

    [MarshalAs(UnmanagedType.U1)]
    public bool bCheckDoubleIP;

    public short iType;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 7)]
    public byte[] szCMS;

    public int lRemainTime;
    public _SYSTEMTIME stEndDate;
    public int authtype;
    public int nTrans;

    [MarshalAs(UnmanagedType.U1)]
    public bool bPrimium;

    [MarshalAs(UnmanagedType.U1)]
    public bool bAgeLimit;

    [MarshalAs(UnmanagedType.U1)]
    public bool bCancelWebUILockBlock;

    public _login_account_request_loac()
    {
        this = default;
        szAccountID = new byte[13];
        szPassword = new byte[13];
        szCMS = new byte[7];
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct _login_account_request_loac_blit
{
    public _CLID idLocal;
    public byte byUserCode;
    public fixed byte szAccountID[13];
    public fixed byte szPassword[13];
    public uint dwClientIP;
    public byte bCheckDoubleIP;
    public short iType;
    public fixed byte szCMS[7];
    public int lRemainTime;
    public ushort wYear;
    public ushort wMonth;
    public ushort wDayOfWeek;
    public ushort wDay;
    public ushort wHour;
    public ushort wMinute;
    public ushort wSecond;
    public ushort wMilliseconds;
    public int authtype;
    public int nTrans;
    public byte bPrimium;
    public byte bAgeLimit;
    public byte bCancelWebUILockBlock;

    public bool Load(byte[] payload)
    {
        int size = Unsafe.SizeOf<_login_account_request_loac_blit>();
        if (payload.Length < size) return false;
        this = MemoryMarshal.Read<_login_account_request_loac_blit>(payload.AsSpan(0, size));
        return true;
    }

    public byte[] GetAccountId()
    {
        var result = new byte[13];
        unsafe
        {
            fixed (byte* p = szAccountID)
            {
                new ReadOnlySpan<byte>(p, 13).CopyTo(result);
            }
        }
        return result;
    }

    public byte[] GetPassword()
    {
        var result = new byte[13];
        unsafe
        {
            fixed (byte* p = szPassword)
            {
                new ReadOnlySpan<byte>(p, 13).CopyTo(result);
            }
        }
        return result;
    }

    public byte[] ToArray()
    {
        var size = Unsafe.SizeOf<_login_account_request_loac_blit>();
        var buffer = new byte[size];
        fixed (byte* dst = buffer)
        {
            fixed (_login_account_request_loac_blit* src = &this)
            {
                Unsafe.CopyBlockUnaligned(dst, src, (uint)size);
            }
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _logout_account_request_loac
{
    public _GLBID gidGlobal;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 8) return false;
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4, 4))
        };
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[8];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _world_list_request_loac
{
    public bool Load(byte[] payload) => true; // no payload
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _join_account_request_loac
{
    public _CLID idLocal;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szAccountID;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szPassword;

    public uint dwClientIP;

    public _join_account_request_loac()
    {
        this = default;
        szAccountID = new byte[13];
        szPassword = new byte[13];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 36) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        Buffer.BlockCopy(payload, 6, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 19, szPassword, 0, 13);
        dwClientIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(32, 4));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _push_close_request_loac
{
    public _CLID idLocal;
    public byte byUserCode;
    public uint dwAccountSerial;
    public uint dwClientIP;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 15) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        byUserCode = payload[6];
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7, 4));
        dwClientIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(11, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[15];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byUserCode;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7, 4), dwAccountSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(11, 4), dwClientIP);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _login_server_stat_result_loac
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

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _update_user_login_failure_cnt_loac
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szUserID;

    public byte byType;

    public _update_user_login_failure_cnt_loac()
    {
        this = default;
        szUserID = new byte[13];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 14) return false;
        Buffer.BlockCopy(payload, 0, szUserID, 0, 13);
        byType = payload[13];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_account_auth_request_loac
{
    public _CLID idLocal;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] byBin;

    public _manage_account_auth_request_loac()
    {
        this = default;
        byBin = new byte[32];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 38) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        Buffer.BlockCopy(payload, 6, byBin, 0, 32);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_force_exit_request_loac
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
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_request_loac
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
        return true;
    }
}

// locl
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _push_close_result_locl
{
    public byte byRetCode;

    public byte[] ToArray() => new[] { byRetCode };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _server_notify_inform_locl
{
    public ushort wMsgCode;

    public byte[] ToArray()
    {
        var buffer = new byte[2];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wMsgCode);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _result_new_agree_locl
{
    public sbyte nRet;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _free_server_inform_locl
{
    public byte byServiceWorldNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
    public byte[] bFree;

    public _free_server_inform_locl()
    {
        this = default;
        bFree = new byte[40];
    }

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 40];
        buffer[0] = byServiceWorldNum;
        if (bFree != null)
        {
            Buffer.BlockCopy(bFree, 0, buffer, 1, Math.Min(40, bFree.Length));
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _world_user_inform_locl
{
    public byte byServiceWorldNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
    public ushort[] wUserNum;

    public _world_user_inform_locl()
    {
        this = default;
        wUserNum = new ushort[40];
    }

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 40 * 2];
        buffer[0] = byServiceWorldNum;
        for (int i = 0; i < 40; i++)
        {
            BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1 + i * 2, 2), wUserNum?[i] ?? 0);
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _motp_validation_reply_locl
{
    public byte byResult;
    [MarshalAs(UnmanagedType.U1)] public bool bDuple;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _crypty_key_inform_locl
{
    public byte byPlus;
    public ushort wKey;

    public byte[] ToArray()
    {
        var buffer = new byte[3];
        buffer[0] = byPlus;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), wKey);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _world_list_result_locl
{
    public byte byRetCode;
    public ushort wDataSize;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4095)]
    public byte[] sListData;

    public _world_list_result_locl()
    {
        this = default;
        sListData = new byte[4095];
    }

    public byte[] ToArray()
    {
        int size = 3 + wDataSize;
        var buffer = new byte[size];
        buffer[0] = byRetCode;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), wDataSize);
        if (wDataSize > 0 && sListData != null)
        {
            Buffer.BlockCopy(sListData, 0, buffer, 3, Math.Min(wDataSize, sListData.Length));
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _select_world_result_locl
{
    public byte byRetCode;
    public uint dwWorldGateIP;
    public ushort wWorldGatePort;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
    public uint[] dwWorldMasterKey;

    [MarshalAs(UnmanagedType.U1)]
    public bool bAllowAltTab;

    public _select_world_result_locl()
    {
        this = default;
        dwWorldMasterKey = new uint[4];
    }

    public byte[] ToArray()
    {
        var buffer = new byte[24];
        buffer[0] = byRetCode;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(1, 4), dwWorldGateIP);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(5, 2), wWorldGatePort);
        for (int i = 0; i < 4; i++)
        {
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7 + i * 4, 4), dwWorldMasterKey[i]);
        }
        buffer[23] = bAllowAltTab ? (byte)1 : (byte)0;
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct _login_account_result_locl
{
    public byte byRetCode;
    public uint dwAccountSerial;
    public uint dwBillingType;
    public byte byBeChangedPass;
    public fixed byte nNewAgree[2];
    [MarshalAs(UnmanagedType.U1)] public bool bAdult;
    public uint dwTime;
    [MarshalAs(UnmanagedType.U1)] public bool bMOTPEntry;
    public fixed byte uszBlockReason[32];

    public byte[] ToArray()
    {
        int size = Marshal.SizeOf<_login_account_result_locl>();
        var buffer = new byte[size];
        fixed (byte* dst = buffer)
        {
            Unsafe.CopyBlockUnaligned(dst, Unsafe.AsPointer(ref this), (uint)size);
        }
        return buffer;
    }

    public static unsafe byte[] FromAclos(_login_account_result_aclo src, byte BillingType)
    {
        var dst = new _login_account_result_locl
        {
            byRetCode = src.byRetCode,
            dwAccountSerial = src.dwAccountSerial,
            dwBillingType = BillingType,
            byBeChangedPass = 0,
            bAdult = true,
            dwTime = 0,
            bMOTPEntry = false
        };
        for (int i = 0; i < 32 && i < src.uszBlockReason.Length; i++)
        {
            dst.uszBlockReason[i] = src.uszBlockReason[i];
        }

        int size = Marshal.SizeOf<_login_account_result_locl>();
        var buffer = new byte[size];
        System.Runtime.InteropServices.MemoryMarshal.Write(buffer.AsSpan(), in dst);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _join_account_result_locl
{
    public byte byRetCode;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _notify_manage_account_auth_info_locl
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 149)]
    public byte[] byKey;

    public _notify_manage_account_auth_info_locl()
    {
        this = default;
        byKey = new byte[149];
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_force_exit_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_account_auth_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_account_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_world_result_locl
{
    public byte byRet;
    public uint m_dwCode;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] m_szName;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] m_szDBName;

    public byte m_byType;

    public _manage_client_limit_run_world_result_locl()
    {
        this = default;
        m_szName = new byte[32];
        m_szDBName = new byte[32];
    }

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 4 + 32 + 32 + 1];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(1, 4), m_dwCode);
        Buffer.BlockCopy(m_szName, 0, buffer, 5, 32);
        Buffer.BlockCopy(m_szDBName, 0, buffer, 37, 32);
        buffer[69] = m_byType;
        return buffer;
    }
}

#endregion
