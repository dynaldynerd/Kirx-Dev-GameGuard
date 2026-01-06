using System;
using System.Buffers.Binary;
using System.Runtime.InteropServices;

namespace RFPackets.Login;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _CLID
{
    public ushort wIndex;
    public uint dwSerial;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _GLBID
{
    public uint dwIndex;
    public uint dwSerial;
}

// Login -> Account
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _join_account_request_loac
{
    public _CLID idLocal;
    public byte[] szAccountID = new byte[13];
    public byte[] szPassword = new byte[13];
    public uint dwClientIP;

    public byte[] ToArray()
    {
        var buffer = new byte[6 + 13 + 13 + 4];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        szAccountID.AsSpan().CopyTo(buffer.AsSpan(6, 13));
        szPassword.AsSpan().CopyTo(buffer.AsSpan(19, 13));
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(32, 4), dwClientIP);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _login_account_request_loac
{
    public _CLID idLocal;
    public byte byUserCode;
    public byte[] szAccountID = new byte[13];
    public byte[] szPassword = new byte[13];
    public uint dwClientIP;
    public bool bCheckDoubleIP;
    public short iType;

    public byte[] ToArray()
    {
        var buffer = new byte[40];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byUserCode;
        szAccountID.AsSpan().CopyTo(buffer.AsSpan(7, 13));
        szPassword.AsSpan().CopyTo(buffer.AsSpan(20, 13));
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(33, 4), dwClientIP);
        buffer[37] = bCheckDoubleIP ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteInt16LittleEndian(buffer.AsSpan(38, 2), iType);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _select_world_request_loac
{
    public _GLBID gidGlobal;
    public ushort wWorldIndex;
    public uint dwClientIP;
    public uint[] dwRequestMoveCharacterSerialList = new uint[3];
    public uint[] dwRTournamentCharacterSerialList = new uint[3];

    public byte[] ToArray()
    {
        var buffer = new byte[38];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(8, 2), wWorldIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(10, 4), dwClientIP);
        for (int i = 0; i < 3; i++)
        {
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(14 + i * 4, 4), dwRequestMoveCharacterSerialList[i]);
        }
        for (int i = 0; i < 3; i++)
        {
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(26 + i * 4, 4), dwRTournamentCharacterSerialList[i]);
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _push_close_request_loac
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
public sealed class _logout_account_request_loac
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
public sealed class _world_list_request_loac
{
    public byte[] ToArray() => Array.Empty<byte>();
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _login_server_stat_result_loac
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
public sealed class _update_user_login_failure_cnt_loac
{
    public byte[] szUserID = new byte[13];
    public byte byType;

    public byte[] ToArray()
    {
        var buffer = new byte[14];
        szUserID.AsSpan().CopyTo(buffer);
        buffer[13] = byType;
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_account_auth_request_loac
{
    public _CLID idLocal;
    public byte[] byBin = new byte[32];

    public byte[] ToArray()
    {
        var buffer = new byte[38];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        byBin.AsSpan().CopyTo(buffer.AsSpan(6, 32));
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_limit_run_request_loac
{
    public _CLID idLocal;

    public byte[] ToArray()
    {
        var buffer = new byte[6];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_force_exit_request_loac
{
    public _CLID idLocal;

    public byte[] ToArray()
    {
        var buffer = new byte[6];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _push_close_result_locl
{
    public byte byRetCode;

    public byte[] ToArray() => new[] { byRetCode };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _result_new_agree_locl
{
    public byte nRet;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _free_server_inform_locl
{
    public byte byServiceWorldNum;
    public bool[] bFree = new bool[40];

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 40];
        buffer[0] = byServiceWorldNum;
        for (int i = 0; i < 40; i++)
        {
            buffer[1 + i] = bFree[i] ? (byte)1 : (byte)0;
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _motp_validation_reply_locl
{
    public byte byResult;
    public bool bDuple;

    public byte[] ToArray() => new[] { byResult, bDuple ? (byte)1 : (byte)0 };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _join_account_result_locl
{
    public byte byRetCode;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _login_account_result_locl
{
    public byte byRetCode;
    public uint dwAccountSerial;
    public uint dwBillingType;
    public byte byBeChangedPass;
    public byte[] nNewAgree = new byte[2];
    public bool bAdult;
    public uint dwTime;
    public bool bMOTPEntry;
    public byte[] uszBlockReason = new byte[32];

    public static byte[] FromAclos(_login_account_result_aclo src, byte billingType, bool isAdult = false, bool motp = false)
    {
        var locl = new _login_account_result_locl
        {
            byRetCode = src.byRetCode,
            dwAccountSerial = src.dwAccountSerial,
            dwBillingType = billingType,
            byBeChangedPass = 0,
            bAdult = isAdult,
            dwTime = 0,
            bMOTPEntry = motp
        };
        src.uszBlockReason.AsSpan().CopyTo(locl.uszBlockReason);
        return locl.ToArray();
    }

    public byte[] ToArray()
    {
        var buffer = new byte[50];
        buffer[0] = byRetCode;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(1, 4), dwAccountSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(5, 4), dwBillingType);
        buffer[9] = byBeChangedPass;
        nNewAgree.AsSpan().CopyTo(buffer.AsSpan(10, 2));
        buffer[12] = bAdult ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(13, 4), dwTime);
        buffer[17] = bMOTPEntry ? (byte)1 : (byte)0;
        uszBlockReason.AsSpan().CopyTo(buffer.AsSpan(18, 32));
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _select_world_result_locl
{
    public byte byRetCode;
    public uint dwWorldGateIP;
    public ushort wWorldGatePort;
    public uint[] dwWorldMasterKey = new uint[4];
    public bool bAllowAltTab;

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
public sealed class _crypty_key_inform_locl
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
public sealed class _server_notify_inform_locl
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
public sealed class _select_world_request_loac_blit
{
    public _GLBID gidGlobal;
    public ushort wWorldIndex;
    public uint dwClientIP;
    public uint dwRequestMoveCharacterSerialList0;
    public uint dwRequestMoveCharacterSerialList1;
    public uint dwRequestMoveCharacterSerialList2;
    public uint dwRTournamentCharacterSerialList0;
    public uint dwRTournamentCharacterSerialList1;
    public uint dwRTournamentCharacterSerialList2;

    public static _select_world_request_loac_blit From(_select_world_request_loac src)
    {
        return new _select_world_request_loac_blit
        {
            gidGlobal = src.gidGlobal,
            wWorldIndex = src.wWorldIndex,
            dwClientIP = src.dwClientIP,
            dwRequestMoveCharacterSerialList0 = src.dwRequestMoveCharacterSerialList[0],
            dwRequestMoveCharacterSerialList1 = src.dwRequestMoveCharacterSerialList[1],
            dwRequestMoveCharacterSerialList2 = src.dwRequestMoveCharacterSerialList[2],
            dwRTournamentCharacterSerialList0 = src.dwRTournamentCharacterSerialList[0],
            dwRTournamentCharacterSerialList1 = src.dwRTournamentCharacterSerialList[1],
            dwRTournamentCharacterSerialList2 = src.dwRTournamentCharacterSerialList[2]
        };
    }

    public byte[] ToArray()
    {
        var buffer = new byte[38];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(8, 2), wWorldIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(10, 4), dwClientIP);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(14, 4), dwRequestMoveCharacterSerialList0);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(18, 4), dwRequestMoveCharacterSerialList1);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(22, 4), dwRequestMoveCharacterSerialList2);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(26, 4), dwRTournamentCharacterSerialList0);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(30, 4), dwRTournamentCharacterSerialList1);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(34, 4), dwRTournamentCharacterSerialList2);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _login_account_request_loac_blit
{
    public _CLID idLocal;
    public byte byUserCode;
    public byte[] szAccountID = new byte[13];
    public byte[] szPassword = new byte[13];
    public uint dwClientIP;
    public byte bCheckDoubleIP;
    public short iType;

    public static _login_account_request_loac_blit FromValues(_CLID idLocal, byte userCode, string accountId, string password, uint clientIp, bool checkDoubleIp, short loginType)
    {
        var blit = new _login_account_request_loac_blit
        {
            idLocal = idLocal,
            byUserCode = userCode,
            dwClientIP = clientIp,
            bCheckDoubleIP = checkDoubleIp ? (byte)1 : (byte)0,
            iType = loginType
        };
        PacketStringUtil.GetBytes(accountId, blit.szAccountID.AsSpan());
        PacketStringUtil.GetBytes(password, blit.szPassword.AsSpan());
        return blit;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[40];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byUserCode;
        szAccountID.AsSpan().CopyTo(buffer.AsSpan(7, 13));
        szPassword.AsSpan().CopyTo(buffer.AsSpan(20, 13));
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(33, 4), dwClientIP);
        buffer[37] = bCheckDoubleIP;
        BinaryPrimitives.WriteInt16LittleEndian(buffer.AsSpan(38, 2), iType);
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _world_list_result_locl
{
    public byte byRetCode;
    public ushort wDataSize;
    public byte[] sListData = new byte[4095];

    public int Size() => 4098 - (4095 - wDataSize);
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _world_user_inform_locl
{
    public byte byServiceWorldNum;
    public ushort[] wUserNum = new ushort[40];

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 40 * 2];
        buffer[0] = byServiceWorldNum;
        for (int i = 0; i < 40; i++)
        {
            BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1 + i * 2, 2), wUserNum[i]);
        }
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_limit_run_world_result_locl
{
    public byte byRet;
    public uint m_dwCode;
    public byte[] m_szName = new byte[32];
    public byte[] m_szDBName = new byte[32];
    public byte m_byType;

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 4 + 32 + 32 + 1];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(1, 4), m_dwCode);
        m_szName.AsSpan().CopyTo(buffer.AsSpan(5, 32));
        m_szDBName.AsSpan().CopyTo(buffer.AsSpan(37, 32));
        buffer[69] = m_byType;
        return buffer;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _notify_manage_account_auth_info_locl
{
    public byte[] byKey = new byte[149];
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_force_exit_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_account_auth_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_limit_run_account_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };
}
