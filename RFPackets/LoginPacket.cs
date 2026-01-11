using System;
using System.Buffers.Binary;

namespace LoginServer.Packets;

#region Login -> Account (loac) and Login -> Client (locl)

// loac
public struct _select_world_request_loac
{
    public _GLBID gidGlobal;
    public ushort wWorldIndex;
    public uint dwClientIP;

    
    public uint[] dwRequestMoveCharacterSerialList;

    
    public uint[] dwRTournamentCharacterSerialList;

    public _select_world_request_loac()
    {
    this = default;
        gidGlobal = new _GLBID();
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

    public byte[] ToArray()
    {
        var buffer = new byte[38];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(8, 2), wWorldIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(10, 4), dwClientIP);
        int offset = 14;
        for (int i = 0; i < 3; i++)
        {
            var value = dwRequestMoveCharacterSerialList != null && i < dwRequestMoveCharacterSerialList.Length
                ? dwRequestMoveCharacterSerialList[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        for (int i = 0; i < 3; i++)
        {
            var value = dwRTournamentCharacterSerialList != null && i < dwRTournamentCharacterSerialList.Length
                ? dwRTournamentCharacterSerialList[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        return buffer;
    }
}

public struct _select_world_request_loac_blit
{
    public _GLBID gidGlobal;
    public ushort wWorldIndex;
    public uint dwClientIP;
    
    public uint[] dwRequestMoveCharacterSerialList;
    
    public uint[] dwRTournamentCharacterSerialList;

    public _select_world_request_loac_blit()
    {
    this = default;
        gidGlobal = new _GLBID();
        dwRequestMoveCharacterSerialList = new uint[3];
        dwRTournamentCharacterSerialList = new uint[3];
    }

    public static _select_world_request_loac_blit From(_select_world_request_loac src)
    {
        var dst = new _select_world_request_loac_blit
        {
            gidGlobal = src.gidGlobal,
            wWorldIndex = src.wWorldIndex,
            dwClientIP = src.dwClientIP
        };
        dst.dwRequestMoveCharacterSerialList = new uint[3];
        dst.dwRTournamentCharacterSerialList = new uint[3];
        for (int i = 0; i < 3; i++)
        {
            dst.dwRequestMoveCharacterSerialList[i] = src.dwRequestMoveCharacterSerialList != null && i < src.dwRequestMoveCharacterSerialList.Length
                ? src.dwRequestMoveCharacterSerialList[i] : 0;
            dst.dwRTournamentCharacterSerialList[i] = src.dwRTournamentCharacterSerialList != null && i < src.dwRTournamentCharacterSerialList.Length
                ? src.dwRTournamentCharacterSerialList[i] : 0;
        }
        return dst;
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
        dwRequestMoveCharacterSerialList ??= new uint[3];
        dwRTournamentCharacterSerialList ??= new uint[3];
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

    public byte[] ToArray()
    {
        var buffer = new byte[38];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(8, 2), wWorldIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(10, 4), dwClientIP);
        int offset = 14;
        for (int i = 0; i < 3; i++)
        {
            var value = dwRequestMoveCharacterSerialList != null && i < dwRequestMoveCharacterSerialList.Length
                ? dwRequestMoveCharacterSerialList[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        for (int i = 0; i < 3; i++)
        {
            var value = dwRTournamentCharacterSerialList != null && i < dwRTournamentCharacterSerialList.Length
                ? dwRTournamentCharacterSerialList[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        return buffer;
    }
}

public struct _login_account_request_loac
{
    public _CLID idLocal;
    public byte byUserCode;

    
    public byte[] szAccountID;

    
    public byte[] szPassword;

    public uint dwClientIP;

    
    public bool bCheckDoubleIP;

    public short iType;

    
    public byte[] szCMS;

    public int lRemainTime;
    public _SYSTEMTIME stEndDate;
    public int authtype;
    public int nTrans;

    
    public bool bPrimium;

    
    public bool bAgeLimit;

    
    public bool bCancelWebUILockBlock;

    public _login_account_request_loac()
    {
    this = default;
        idLocal = new _CLID();
        szAccountID = new byte[13];
        szPassword = new byte[13];
        szCMS = new byte[7];
        stEndDate = new _SYSTEMTIME();
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 78) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        byUserCode = payload[6];
        szAccountID ??= new byte[13];
        szPassword ??= new byte[13];
        szCMS ??= new byte[7];
        Buffer.BlockCopy(payload, 7, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 20, szPassword, 0, 13);
        dwClientIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(33, 4));
        bCheckDoubleIP = payload[37] != 0;
        iType = BinaryPrimitives.ReadInt16LittleEndian(payload.AsSpan(38, 2));
        Buffer.BlockCopy(payload, 40, szCMS, 0, 7);
        lRemainTime = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(47, 4));
        stEndDate = new _SYSTEMTIME();
        stEndDate.wYear = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(51, 2));
        stEndDate.wMonth = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(53, 2));
        stEndDate.wDayOfWeek = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(55, 2));
        stEndDate.wDay = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(57, 2));
        stEndDate.wHour = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(59, 2));
        stEndDate.wMinute = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(61, 2));
        stEndDate.wSecond = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(63, 2));
        stEndDate.wMilliseconds = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(65, 2));
        authtype = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(67, 4));
        nTrans = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(71, 4));
        bPrimium = payload[75] != 0;
        bAgeLimit = payload[76] != 0;
        bCancelWebUILockBlock = payload[77] != 0;
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[78];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byUserCode;
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 7, Math.Min(13, szAccountID.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 20, Math.Min(13, szPassword.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(33, 4), dwClientIP);
        buffer[37] = bCheckDoubleIP ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteInt16LittleEndian(buffer.AsSpan(38, 2), iType);
        if (szCMS != null)
        {
            Buffer.BlockCopy(szCMS, 0, buffer, 40, Math.Min(7, szCMS.Length));
        }
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(47, 4), lRemainTime);
        var endDate = stEndDate;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(51, 2), endDate.wYear);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(53, 2), endDate.wMonth);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(55, 2), endDate.wDayOfWeek);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(57, 2), endDate.wDay);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(59, 2), endDate.wHour);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(61, 2), endDate.wMinute);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(63, 2), endDate.wSecond);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(65, 2), endDate.wMilliseconds);
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(67, 4), authtype);
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(71, 4), nTrans);
        buffer[75] = bPrimium ? (byte)1 : (byte)0;
        buffer[76] = bAgeLimit ? (byte)1 : (byte)0;
        buffer[77] = bCancelWebUILockBlock ? (byte)1 : (byte)0;
        return buffer;
    }
}

public struct _login_account_request_loac_blit
{
    public _CLID idLocal;
    public byte byUserCode;
    
    public byte[] szAccountID;
    
    public byte[] szPassword;
    public uint dwClientIP;
    public byte bCheckDoubleIP;
    public short iType;
    
    public byte[] szCMS;
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

    public _login_account_request_loac_blit()
    {
    this = default;
        idLocal = new _CLID();
        szAccountID = new byte[13];
        szPassword = new byte[13];
        szCMS = new byte[7];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 78) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        byUserCode = payload[6];
        szAccountID ??= new byte[13];
        szPassword ??= new byte[13];
        szCMS ??= new byte[7];
        Buffer.BlockCopy(payload, 7, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 20, szPassword, 0, 13);
        dwClientIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(33, 4));
        bCheckDoubleIP = payload[37];
        iType = BinaryPrimitives.ReadInt16LittleEndian(payload.AsSpan(38, 2));
        Buffer.BlockCopy(payload, 40, szCMS, 0, 7);
        lRemainTime = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(47, 4));
        wYear = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(51, 2));
        wMonth = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(53, 2));
        wDayOfWeek = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(55, 2));
        wDay = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(57, 2));
        wHour = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(59, 2));
        wMinute = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(61, 2));
        wSecond = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(63, 2));
        wMilliseconds = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(65, 2));
        authtype = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(67, 4));
        nTrans = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(71, 4));
        bPrimium = payload[75];
        bAgeLimit = payload[76];
        bCancelWebUILockBlock = payload[77];
        return true;
    }

    public byte[] GetAccountId()
    {
        var result = new byte[13];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, result, 0, 13);
        }
        return result;
    }

    public byte[] GetPassword()
    {
        var result = new byte[13];
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, result, 0, 13);
        }
        return result;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[78];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byUserCode;
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 7, Math.Min(13, szAccountID.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 20, Math.Min(13, szPassword.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(33, 4), dwClientIP);
        buffer[37] = bCheckDoubleIP;
        BinaryPrimitives.WriteInt16LittleEndian(buffer.AsSpan(38, 2), iType);
        if (szCMS != null)
        {
            Buffer.BlockCopy(szCMS, 0, buffer, 40, Math.Min(7, szCMS.Length));
        }
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(47, 4), lRemainTime);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(51, 2), wYear);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(53, 2), wMonth);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(55, 2), wDayOfWeek);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(57, 2), wDay);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(59, 2), wHour);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(61, 2), wMinute);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(63, 2), wSecond);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(65, 2), wMilliseconds);
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(67, 4), authtype);
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(71, 4), nTrans);
        buffer[75] = bPrimium;
        buffer[76] = bAgeLimit;
        buffer[77] = bCancelWebUILockBlock;
        return buffer;
    }
}

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

public struct _world_list_request_loac
{
    public bool Load(byte[] payload) => payload.Length >= 1;

    public byte[] ToArray() => new byte[1];
}

public struct _join_account_request_loac
{
    public _CLID idLocal;

    
    public byte[] szAccountID;

    
    public byte[] szPassword;

    public uint dwClientIP;

    public _join_account_request_loac()
    {
    this = default;
        idLocal = new _CLID();
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

    public byte[] ToArray()
    {
        var buffer = new byte[36];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 6, Math.Min(13, szAccountID.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 19, Math.Min(13, szPassword.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(32, 4), dwClientIP);
        return buffer;
    }
}

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

public struct _update_user_login_failure_cnt_loac
{
    
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

    public byte[] ToArray()
    {
        var buffer = new byte[14];
        if (szUserID != null)
        {
            Buffer.BlockCopy(szUserID, 0, buffer, 0, Math.Min(13, szUserID.Length));
        }
        buffer[13] = byType;
        return buffer;
    }
}

public struct _manage_account_auth_request_loac
{
    public _CLID idLocal;

    
    public byte[] byBin;

    public _manage_account_auth_request_loac()
    {
    this = default;
        idLocal = new _CLID();
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

    public byte[] ToArray()
    {
        var buffer = new byte[38];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        if (byBin != null)
        {
            Buffer.BlockCopy(byBin, 0, buffer, 6, Math.Min(32, byBin.Length));
        }
        return buffer;
    }
}

public struct _manage_client_force_exit_request_loac
{
    public _CLID idLocal;

    public _manage_client_force_exit_request_loac()
    {
    this = default;
        idLocal = new _CLID();
    }

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

    public byte[] ToArray()
    {
        var buffer = new byte[6];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        return buffer;
    }
}

public struct _manage_client_limit_run_request_loac
{
    public _CLID idLocal;

    public _manage_client_limit_run_request_loac()
    {
    this = default;
        idLocal = new _CLID();
    }

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

    public byte[] ToArray()
    {
        var buffer = new byte[6];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        return buffer;
    }
}

// locl
public struct _push_close_result_locl
{
    public byte byRetCode;

    public byte[] ToArray() => new[] { byRetCode };

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        byRetCode = payload[0];
        return true;
    }
}

public struct _server_notify_inform_locl
{
    public ushort wMsgCode;

    public byte[] ToArray()
    {
        var buffer = new byte[2];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wMsgCode);
        return buffer;
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        wMsgCode = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        return true;
    }
}

public struct _result_new_agree_locl
{
    public sbyte nRet;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        nRet = unchecked((sbyte)payload[0]);
        return true;
    }

    public byte[] ToArray() => new[] { unchecked((byte)nRet) };
}

public struct _free_server_inform_locl
{
    public byte byServiceWorldNum;

    
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

    public bool Load(byte[] payload)
    {
        if (payload.Length < 41) return false;
        byServiceWorldNum = payload[0];
        bFree ??= new byte[40];
        Buffer.BlockCopy(payload, 1, bFree, 0, 40);
        return true;
    }
}

public struct _world_user_inform_locl
{
    public byte byServiceWorldNum;

    
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
            BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1 + i * 2, 2), wUserNum != null && i < wUserNum.Length ? wUserNum[i] : (ushort)0);
        }
        return buffer;
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 81) return false;
        byServiceWorldNum = payload[0];
        wUserNum ??= new ushort[40];
        for (int i = 0; i < 40; i++)
        {
            wUserNum[i] = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1 + i * 2, 2));
        }
        return true;
    }
}

public struct _motp_validation_reply_locl
{
    public byte byResult;
     public bool bDuple;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        byResult = payload[0];
        bDuple = payload[1] != 0;
        return true;
    }

    public byte[] ToArray() => new[] { byResult, bDuple ? (byte)1 : (byte)0 };
}

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

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        byPlus = payload[0];
        wKey = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        return true;
    }
}

public struct _world_list_result_locl
{
    public byte byRetCode;
    public ushort wDataSize;

    
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

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        byRetCode = payload[0];
        wDataSize = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        if (wDataSize > 4095) return false;
        if (payload.Length < 3 + wDataSize) return false;
        sListData ??= new byte[4095];
        Array.Clear(sListData, 0, sListData.Length);
        if (wDataSize > 0)
        {
            Buffer.BlockCopy(payload, 3, sListData, 0, Math.Min(wDataSize, sListData.Length));
        }
        return true;
    }
}

public struct _select_world_result_locl
{
    public byte byRetCode;
    public uint dwWorldGateIP;
    public ushort wWorldGatePort;

    
    public uint[] dwWorldMasterKey;

    
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
            var value = dwWorldMasterKey != null && i < dwWorldMasterKey.Length ? dwWorldMasterKey[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7 + i * 4, 4), value);
        }
        buffer[23] = bAllowAltTab ? (byte)1 : (byte)0;
        return buffer;
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 24) return false;
        byRetCode = payload[0];
        dwWorldGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(1, 4));
        wWorldGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(5, 2));
        dwWorldMasterKey ??= new uint[4];
        for (int i = 0; i < 4; i++)
        {
            dwWorldMasterKey[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7 + i * 4, 4));
        }
        bAllowAltTab = payload[23] != 0;
        return true;
    }
}

public struct _login_account_result_locl
{
    public byte byRetCode;
    public uint dwAccountSerial;
    public uint dwBillingType;
    public byte byBeChangedPass;
    
    public byte[] nNewAgree;
     public bool bAdult;
    public uint dwTime;
     public bool bMOTPEntry;
    
    public byte[] uszBlockReason;

    public _login_account_result_locl()
    {
    this = default;
        nNewAgree = new byte[2];
        uszBlockReason = new byte[32];
    }

    public byte[] ToArray()
    {
        var buffer = new byte[50];
        buffer[0] = byRetCode;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(1, 4), dwAccountSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(5, 4), dwBillingType);
        buffer[9] = byBeChangedPass;
        if (nNewAgree != null)
        {
            Buffer.BlockCopy(nNewAgree, 0, buffer, 10, Math.Min(2, nNewAgree.Length));
        }
        buffer[12] = bAdult ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(13, 4), dwTime);
        buffer[17] = bMOTPEntry ? (byte)1 : (byte)0;
        if (uszBlockReason != null)
        {
            Buffer.BlockCopy(uszBlockReason, 0, buffer, 18, Math.Min(32, uszBlockReason.Length));
        }
        return buffer;
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 50) return false;
        byRetCode = payload[0];
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(1, 4));
        dwBillingType = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(5, 4));
        byBeChangedPass = payload[9];
        nNewAgree ??= new byte[2];
        Buffer.BlockCopy(payload, 10, nNewAgree, 0, 2);
        bAdult = payload[12] != 0;
        dwTime = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(13, 4));
        bMOTPEntry = payload[17] != 0;
        uszBlockReason ??= new byte[32];
        Buffer.BlockCopy(payload, 18, uszBlockReason, 0, 32);
        return true;
    }

    public static byte[] FromAclos(_login_account_result_aclo src, byte BillingType)
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
        if (src.uszBlockReason != null)
        {
            Buffer.BlockCopy(src.uszBlockReason, 0, dst.uszBlockReason, 0, Math.Min(32, src.uszBlockReason.Length));
        }
        return dst.ToArray();
    }
}

public struct _join_account_result_locl
{
    public byte byRetCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        byRetCode = payload[0];
        return true;
    }

    public byte[] ToArray() => new[] { byRetCode };
}

public struct _notify_manage_account_auth_info_locl
{
    
    public byte[] byKey;

    public _notify_manage_account_auth_info_locl()
    {
    this = default;
        byKey = new byte[149];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 149) return false;
        Buffer.BlockCopy(payload, 0, byKey, 0, 149);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[149];
        if (byKey != null)
        {
            Buffer.BlockCopy(byKey, 0, buffer, 0, Math.Min(149, byKey.Length));
        }
        return buffer;
    }
}

public struct _manage_client_force_exit_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        byRet = payload[0];
        return true;
    }
}

public struct _manage_account_auth_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        byRet = payload[0];
        return true;
    }
}

public struct _manage_client_limit_run_account_result_locl
{
    public byte byRet;

    public byte[] ToArray() => new[] { byRet };

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        byRet = payload[0];
        return true;
    }
}

public struct _manage_client_limit_run_world_result_locl
{
    public byte byRet;
    public uint m_dwCode;

    
    public byte[] m_szName;

    
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
        if (m_szName != null)
        {
            Buffer.BlockCopy(m_szName, 0, buffer, 5, Math.Min(32, m_szName.Length));
        }
        if (m_szDBName != null)
        {
            Buffer.BlockCopy(m_szDBName, 0, buffer, 37, Math.Min(32, m_szDBName.Length));
        }
        buffer[69] = m_byType;
        return buffer;
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 70) return false;
        byRet = payload[0];
        m_dwCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(1, 4));
        m_szName ??= new byte[32];
        m_szDBName ??= new byte[32];
        Buffer.BlockCopy(payload, 5, m_szName, 0, 32);
        Buffer.BlockCopy(payload, 37, m_szDBName, 0, 32);
        m_byType = payload[69];
        return true;
    }
}

#endregion



