using System;
using System.Buffers.Binary;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace LoginServer.Packets;

internal static class PacketStringUtil
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

/// <summary>Global ID (as seen in login/account exchange).</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _GLBID
{
    public uint dwIndex;
    public uint dwSerial;
}

/// <summary>Client ID (local to login server).</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _CLID
{
    public ushort wIndex;
    public uint dwSerial;
}

/// <summary>Win32 SYSTEMTIME equivalent.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 2)]
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

#region Login -> Account (loac)

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

    public static _login_account_request_loac FromSession(State.ClientSession session)
    {
        var req = new _login_account_request_loac
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial },
            byUserCode = session.LoginCode,
            dwClientIP = session.ClientIp,
            bCheckDoubleIP = true,
            iType = (short)session.BillType,
            lRemainTime = session.RemainTime,
            authtype = 0,
            nTrans = session.Trans,
            bPrimium = session.IsPremium,
            bAgeLimit = false,
            bCancelWebUILockBlock = false,
            stEndDate = new _SYSTEMTIME()
        };

        PacketStringUtil.FillFixed(req.szAccountID, session.AccountId);
        PacketStringUtil.FillFixed(req.szPassword, session.Password);
        return req;
    }
}

/// <summary>
/// Blittable form of login_account_request_loac for fast serialization.
/// </summary>
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

    public static _login_account_request_loac_blit FromSession(State.ClientSession session)
    {
        var req = new _login_account_request_loac_blit
        {
            idLocal = new _CLID { wIndex = session.Index, dwSerial = session.ClidSerial },
            byUserCode = session.LoginCode,
            dwClientIP = session.ClientIp,
            bCheckDoubleIP = 1,
            iType = (short)session.BillType,
            lRemainTime = session.RemainTime,
            wYear = 0,
            wMonth = 0,
            wDayOfWeek = 0,
            wDay = 0,
            wHour = 0,
            wMinute = 0,
            wSecond = 0,
            wMilliseconds = 0,
            authtype = 0,
            nTrans = session.Trans,
            bPrimium = (byte)(session.IsPremium ? 1 : 0),
            bAgeLimit = session.AgeLimit ? (byte)1 : (byte)0,
            bCancelWebUILockBlock = 0
        };

        FillFixed(req.szAccountID, 13, session.AccountId);
        FillFixed(req.szPassword, 13, session.Password);
        return req;
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

    private static void FillFixed(byte* target, int length, string value)
    {
        for (int i = 0; i < length; i++) target[i] = 0;
        if (string.IsNullOrEmpty(value)) return;
        var bytes = Encoding.ASCII.GetBytes(value);
        int copy = Math.Min(bytes.Length, length);
        for (int i = 0; i < copy; i++)
        {
            target[i] = bytes[i];
        }
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _logout_account_request_loac
{
    public _GLBID gidGlobal;

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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _push_close_request_loac
{
    public _CLID idLocal;
    public byte byUserCode;
    public uint dwAccountSerial;
    public uint dwClientIP;

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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_force_exit_request_loac
{
    public _CLID idLocal;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_request_loac
{
    public _CLID idLocal;
}

#endregion

#region Account -> Login (aclo)

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

#endregion
