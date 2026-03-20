using System;
using System.Buffers.Binary;

namespace LoginServer.Packets;

/// <summary>Account -> Login packets (_aclo).</summary>
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

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRetCode;
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[3];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wWorldCode);
        buffer[2] = byMasterRaceCode;
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[10];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwWorldCode);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), dwGateIP);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(8, 2), wGatePort);
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[3];
        buffer[0] = byStat;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), wClientIndex);
        return buffer;
    }
}

public struct _inform_usernum_world_aclo
{
    public byte byServiceWorldNum;

    
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

    public byte[] ToArray()
    {
        var buffer = new byte[1 + 40 * 2];
        buffer[0] = byServiceWorldNum;
        for (int i = 0; i < 40; i++)
        {
            var value = wUserNum != null && i < wUserNum.Length ? wUserNum[i] : (ushort)0;
            BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1 + i * 2, 2), value);
        }
        return buffer;
    }
}

public struct _login_account_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
    public _GLBID gidNewGlobal;
    public uint dwAccountSerial;
    public byte byUserGrade;
    public byte bySubGrade;

    
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
        var buffer = new byte[59];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRetCode;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7, 4), gidNewGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(11, 4), gidNewGlobal.dwSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(15, 4), dwAccountSerial);
        buffer[19] = byUserGrade;
        buffer[20] = bySubGrade;
        if (uszBlockReason != null)
        {
            Buffer.BlockCopy(uszBlockReason, 0, buffer, 21, Math.Min(32, uszBlockReason.Length));
        }
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(53, 4), nTrans);
        buffer[57] = byBlockReasonType;
        buffer[58] = byCancelUILockBlockRet;
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
        uszBlockReason ??= new byte[32];
        Buffer.BlockCopy(payload, 21, uszBlockReason, 0, 32);
        nTrans = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(53, 4));
        byBlockReasonType = payload[57];
        byCancelUILockBlockRet = payload[58];
        return true;
    }
}

public struct _account_db_info_result_aclo
{
    
    public byte[] szDBName;

    
    public byte[] szIP;

    
    public byte[] szArgon2SaltBase64;

    public _account_db_info_result_aclo()
    {
    this = default;
        szDBName = new byte[32];
        szIP = new byte[16];
        szArgon2SaltBase64 = new byte[64];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 48) return false;
        Buffer.BlockCopy(payload, 0, szDBName, 0, 32);
        Buffer.BlockCopy(payload, 32, szIP, 0, 16);
        if (payload.Length >= 112)
        {
            Buffer.BlockCopy(payload, 48, szArgon2SaltBase64, 0, 64);
        }
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[112];
        if (szDBName != null)
        {
            Buffer.BlockCopy(szDBName, 0, buffer, 0, Math.Min(32, szDBName.Length));
        }
        if (szIP != null)
        {
            Buffer.BlockCopy(szIP, 0, buffer, 32, Math.Min(16, szIP.Length));
        }
        if (szArgon2SaltBase64 != null)
        {
            Buffer.BlockCopy(szArgon2SaltBase64, 0, buffer, 48, Math.Min(64, szArgon2SaltBase64.Length));
        }
        return buffer;
    }
}

public struct _world_list_result_aclo
{
    public byte byServiceWorldNum;
    public byte byWorldNum;

    
    public _world_list_result_aclo__list[] WorldList;

    public _world_list_result_aclo()
    {
    this = default;
        WorldList = new _world_list_result_aclo__list[40];
        for (int i = 0; i < WorldList.Length; i++)
        {
            WorldList[i] = new _world_list_result_aclo__list();
        }
    }

    public struct _world_list_result_aclo__list
    {
        
        public bool bOpen;

        
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
        if (WorldList == null || WorldList.Length != 40)
        {
            WorldList = new _world_list_result_aclo__list[40];
        }
        for (int i = 0; i < byWorldNum; i++)
        {
            var entry = new _world_list_result_aclo__list
            {
                bOpen = payload[offset] != 0
            };
            entry.szWorldName ??= new byte[33];
            Buffer.BlockCopy(payload, offset + 1, entry.szWorldName, 0, 33);
            entry.dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset + 34, 4));
            entry.wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 38, 2));
            entry.byType = payload[offset + 40];
            WorldList[i] = entry;
            offset += entrySize;
        }

        return true;
    }

    public byte[] ToArray()
    {
        int count = byWorldNum;
        if (count > 40) count = 40;
        var buffer = new byte[2 + count * 41];
        buffer[0] = byServiceWorldNum;
        buffer[1] = (byte)count;
        int offset = 2;
        for (int i = 0; i < count; i++)
        {
            var entry = WorldList != null && i < WorldList.Length
                ? WorldList[i]
                : default;
            buffer[offset] = entry.bOpen ? (byte)1 : (byte)0;
            if (entry.szWorldName != null)
            {
                Buffer.BlockCopy(entry.szWorldName, 0, buffer, offset + 1, Math.Min(33, entry.szWorldName.Length));
            }
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset + 34, 4), entry.dwGateIP);
            BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 38, 2), entry.wGatePort);
            buffer[offset + 40] = entry.byType;
            offset += 41;
        }
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[6];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRetCode;
        return buffer;
    }
}

public struct _inform_close_world_aclo
{
    public uint dwWorldCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[4];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwWorldCode);
        return buffer;
    }
}

public struct _select_world_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    
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

    public byte[] ToArray()
    {
        var buffer = new byte[23];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRetCode;
        for (int i = 0; i < 4; i++)
        {
            var value = dwWorldMasterKey != null && i < dwWorldMasterKey.Length ? dwWorldMasterKey[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7 + i * 4, 4), value);
        }
        return buffer;
    }
}

public struct _notify_manage_account_auth_info_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    
    public byte[] byKey;

    public _notify_manage_account_auth_info_aclo()
    {
    this = default;
        byKey = new byte[149];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 156) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRetCode = payload[6];
        Buffer.BlockCopy(payload, 7, byKey, 0, 149);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[156];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRetCode;
        if (byKey != null)
        {
            Buffer.BlockCopy(byKey, 0, buffer, 7, Math.Min(149, byKey.Length));
        }
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRet;
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRet;
        return buffer;
    }
}

public struct _manage_client_limit_run_world_result_aclo
{
    public _CLID idLocal;
    public byte byRet;
    public uint m_dwCode;

    
    public byte[] m_szName;

    
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
        if (payload.Length < 76) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        if (idLocal.wIndex >= 0x1400) return false;
        byRet = payload[6];
        m_dwCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7, 4));
        Buffer.BlockCopy(payload, 11, m_szName, 0, 32);
        Buffer.BlockCopy(payload, 43, m_szDBName, 0, 32);
        m_byType = payload[75];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[76];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRet;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7, 4), m_dwCode);
        if (m_szName != null)
        {
            Buffer.BlockCopy(m_szName, 0, buffer, 11, Math.Min(32, m_szName.Length));
        }
        if (m_szDBName != null)
        {
            Buffer.BlockCopy(m_szDBName, 0, buffer, 43, Math.Min(32, m_szDBName.Length));
        }
        buffer[75] = m_byType;
        return buffer;
    }
}

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

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRet;
        return buffer;
    }
}



#region Account -> World (acwr)

public struct _cashdb_setting_result_acwr
{
    public byte[] szIP;

    public byte[] szDSN;

    public byte[] szDBName;

    public byte[] szAccount;

    public byte[] szPassword;

    public uint dwPort;

    public _cashdb_setting_result_acwr()
    {
    this = default;
        szIP = new byte[16];
        szDSN = new byte[32];
        szDBName = new byte[32];
        szAccount = new byte[32];
        szPassword = new byte[32];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 148) return false;
        szIP ??= new byte[16];
        szDSN ??= new byte[32];
        szDBName ??= new byte[32];
        szAccount ??= new byte[32];
        szPassword ??= new byte[32];
        Buffer.BlockCopy(payload, 0, szIP, 0, 16);
        Buffer.BlockCopy(payload, 16, szDSN, 0, 32);
        Buffer.BlockCopy(payload, 48, szDBName, 0, 32);
        Buffer.BlockCopy(payload, 80, szAccount, 0, 32);
        Buffer.BlockCopy(payload, 112, szPassword, 0, 32);
        dwPort = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(144, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[148];
        if (szIP != null)
        {
            Buffer.BlockCopy(szIP, 0, buffer, 0, Math.Min(16, szIP.Length));
        }
        if (szDSN != null)
        {
            Buffer.BlockCopy(szDSN, 0, buffer, 16, Math.Min(32, szDSN.Length));
        }
        if (szDBName != null)
        {
            Buffer.BlockCopy(szDBName, 0, buffer, 48, Math.Min(32, szDBName.Length));
        }
        if (szAccount != null)
        {
            Buffer.BlockCopy(szAccount, 0, buffer, 80, Math.Min(32, szAccount.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 112, Math.Min(32, szPassword.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(144, 4), dwPort);
        return buffer;
    }
}

public struct _character_disconnect_request_acwr
{
    public ushort wClientIndex;

    public byte[] szCharacterName;

    public _character_disconnect_request_acwr()
    {
    this = default;
        szCharacterName = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 19) return false;
        wClientIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        szCharacterName ??= new byte[17];
        Buffer.BlockCopy(payload, 2, szCharacterName, 0, 17);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[19];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wClientIndex);
        if (szCharacterName != null)
        {
            Buffer.BlockCopy(szCharacterName, 0, buffer, 2, Math.Min(17, szCharacterName.Length));
        }
        return buffer;
    }
}

public struct _chat_lock_command_acwr
{
    public _CLID idLocal;
    public ushort wBlockTimeH;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 8) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        wBlockTimeH = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(6, 2));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[8];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(6, 2), wBlockTimeH);
        return buffer;
    }
}

public struct _check_is_block_ip_result_acwr
{
    public byte byRet;
    public _CLID idLocal;
    public uint ulIP;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 11) return false;
        byRet = payload[0];
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(3, 4))
        };
        ulIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[11];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(3, 4), idLocal.dwSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7, 4), ulIP);
        return buffer;
    }
}

public struct _enter_world_result_acwr
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
        byRetCode = payload[6];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byRetCode;
        return buffer;
    }
}

public struct _force_close_command_acwr
{
    public _CLID idLocal;
    public bool bDirectly;
    public byte byKickType;
    public uint dwPushIP;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 12) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        bDirectly = payload[6] != 0;
        byKickType = payload[7];
        dwPushIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(8, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[12];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = bDirectly ? (byte)1 : (byte)0;
        buffer[7] = byKickType;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(8, 4), dwPushIP);
        return buffer;
    }
}

public struct _manage_client_limit_run_request_acwr
{
    public _CLID idLocal;
    public byte byLoginServerIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 7) return false;
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4))
        };
        byLoginServerIndex = payload[6];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[7];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), idLocal.dwSerial);
        buffer[6] = byLoginServerIndex;
        return buffer;
    }
}

public struct _open_control_inform_acwr
{
    public bool bControlOpen;
    public uint dwControlIP;
    public ushort wControlPort;

    public uint[] dwMasterKey;

    public _open_control_inform_acwr()
    {
    this = default;
        dwMasterKey = new uint[4];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 23) return false;
        bControlOpen = payload[0] != 0;
        dwControlIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(1, 4));
        wControlPort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(5, 2));
        dwMasterKey ??= new uint[4];
        for (int i = 0; i < 4; i++)
        {
            dwMasterKey[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(7 + i * 4, 4));
        }
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[23];
        buffer[0] = bControlOpen ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(1, 4), dwControlIP);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(5, 2), wControlPort);
        for (int i = 0; i < 4; i++)
        {
            var value = dwMasterKey != null && i < dwMasterKey.Length ? dwMasterKey[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(7 + i * 4, 4), value);
        }
        return buffer;
    }
}

public struct _open_world_failure_acwr
{
    public byte[] szErrCode;

    public _open_world_failure_acwr()
    {
    this = default;
        szErrCode = new byte[64];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 64) return false;
        szErrCode ??= new byte[64];
        Buffer.BlockCopy(payload, 0, szErrCode, 0, 64);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[64];
        if (szErrCode != null)
        {
            Buffer.BlockCopy(szErrCode, 0, buffer, 0, Math.Min(64, szErrCode.Length));
        }
        return buffer;
    }
}

public struct _open_world_success_acwr
{
    public byte byWorldCode;

    public byte[] szDBName;

    public byte[] szDBIP;

    public byte m_byWorldType;

    public _open_world_success_acwr()
    {
    this = default;
        szDBName = new byte[32];
        szDBIP = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 51) return false;
        byWorldCode = payload[0];
        szDBName ??= new byte[32];
        szDBIP ??= new byte[17];
        Buffer.BlockCopy(payload, 1, szDBName, 0, 32);
        Buffer.BlockCopy(payload, 33, szDBIP, 0, 17);
        m_byWorldType = payload[50];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[51];
        buffer[0] = byWorldCode;
        if (szDBName != null)
        {
            Buffer.BlockCopy(szDBName, 0, buffer, 1, Math.Min(32, szDBName.Length));
        }
        if (szDBIP != null)
        {
            Buffer.BlockCopy(szDBIP, 0, buffer, 33, Math.Min(17, szDBIP.Length));
        }
        buffer[50] = m_byWorldType;
        return buffer;
    }
}

public struct _trans_account_inform_acwr
{
    public _GLBID gidGlobal;

    public uint[] dwKey;

    public uint dwClientIP;
    public uint dwAccountSerial;

    public byte[] szAccountID;

    public byte byUserDgr;
    public byte bySubDgr;
    public bool bChatLock;
    public short iType;

    public byte[] szCMS;

    public int lRemainTime;
    public _SYSTEMTIME stEndDate;
    public byte byUILock;
    public byte byUILock_failcnt;

    public byte[] szUILock_pw;

    public byte[] szAccount_pw;

    public byte byUILock_HintIndex;

    public byte[] uszUILock_HintAnswer;

    public byte byUILockFindPassFailCount;

    public uint[] dwRequestMoveCharacterSerialList;

    public uint[] dwTournamentCharacterSerialList;

    public bool bIsPcBang;
    public int nTrans;
    public bool bAgeLimit;

    public _trans_account_inform_acwr()
    {
    this = default;
        dwKey = new uint[4];
        szAccountID = new byte[13];
        szCMS = new byte[7];
        stEndDate = new _SYSTEMTIME();
        szUILock_pw = new byte[13];
        szAccount_pw = new byte[13];
        uszUILock_HintAnswer = new byte[17];
        dwRequestMoveCharacterSerialList = new uint[3];
        dwTournamentCharacterSerialList = new uint[3];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 154) return false;
        int offset = 0;
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset + 4, 4))
        };
        offset += 8;
        dwKey ??= new uint[4];
        for (int i = 0; i < 4; i++)
        {
            dwKey[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
            offset += 4;
        }
        dwClientIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
        offset += 4;
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
        offset += 4;
        szAccountID ??= new byte[13];
        Buffer.BlockCopy(payload, offset, szAccountID, 0, 13);
        offset += 13;
        byUserDgr = payload[offset++];
        bySubDgr = payload[offset++];
        bChatLock = payload[offset++] != 0;
        iType = BinaryPrimitives.ReadInt16LittleEndian(payload.AsSpan(offset, 2));
        offset += 2;
        szCMS ??= new byte[7];
        Buffer.BlockCopy(payload, offset, szCMS, 0, 7);
        offset += 7;
        lRemainTime = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(offset, 4));
        offset += 4;
        stEndDate = new _SYSTEMTIME
        {
            wYear = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset, 2)),
            wMonth = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 2, 2)),
            wDayOfWeek = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 4, 2)),
            wDay = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 6, 2)),
            wHour = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 8, 2)),
            wMinute = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 10, 2)),
            wSecond = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 12, 2)),
            wMilliseconds = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(offset + 14, 2))
        };
        offset += 16;
        byUILock = payload[offset++];
        byUILock_failcnt = payload[offset++];
        szUILock_pw ??= new byte[13];
        Buffer.BlockCopy(payload, offset, szUILock_pw, 0, 13);
        offset += 13;
        szAccount_pw ??= new byte[13];
        Buffer.BlockCopy(payload, offset, szAccount_pw, 0, 13);
        offset += 13;
        byUILock_HintIndex = payload[offset++];
        uszUILock_HintAnswer ??= new byte[17];
        Buffer.BlockCopy(payload, offset, uszUILock_HintAnswer, 0, 17);
        offset += 17;
        byUILockFindPassFailCount = payload[offset++];
        dwRequestMoveCharacterSerialList ??= new uint[3];
        for (int i = 0; i < 3; i++)
        {
            dwRequestMoveCharacterSerialList[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
            offset += 4;
        }
        dwTournamentCharacterSerialList ??= new uint[3];
        for (int i = 0; i < 3; i++)
        {
            dwTournamentCharacterSerialList[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(offset, 4));
            offset += 4;
        }
        bIsPcBang = payload[offset++] != 0;
        nTrans = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(offset, 4));
        offset += 4;
        bAgeLimit = payload[offset] != 0;
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[154];
        int offset = 0;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset + 4, 4), gidGlobal.dwSerial);
        offset += 8;
        for (int i = 0; i < 4; i++)
        {
            var value = dwKey != null && i < dwKey.Length ? dwKey[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), dwClientIP);
        offset += 4;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), dwAccountSerial);
        offset += 4;
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, offset, Math.Min(13, szAccountID.Length));
        }
        offset += 13;
        buffer[offset++] = byUserDgr;
        buffer[offset++] = bySubDgr;
        buffer[offset++] = bChatLock ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteInt16LittleEndian(buffer.AsSpan(offset, 2), iType);
        offset += 2;
        if (szCMS != null)
        {
            Buffer.BlockCopy(szCMS, 0, buffer, offset, Math.Min(7, szCMS.Length));
        }
        offset += 7;
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(offset, 4), lRemainTime);
        offset += 4;
        var endDate = stEndDate;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset, 2), endDate.wYear);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 2, 2), endDate.wMonth);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 4, 2), endDate.wDayOfWeek);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 6, 2), endDate.wDay);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 8, 2), endDate.wHour);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 10, 2), endDate.wMinute);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 12, 2), endDate.wSecond);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset + 14, 2), endDate.wMilliseconds);
        offset += 16;
        buffer[offset++] = byUILock;
        buffer[offset++] = byUILock_failcnt;
        if (szUILock_pw != null)
        {
            Buffer.BlockCopy(szUILock_pw, 0, buffer, offset, Math.Min(13, szUILock_pw.Length));
        }
        offset += 13;
        if (szAccount_pw != null)
        {
            Buffer.BlockCopy(szAccount_pw, 0, buffer, offset, Math.Min(13, szAccount_pw.Length));
        }
        offset += 13;
        buffer[offset++] = byUILock_HintIndex;
        if (uszUILock_HintAnswer != null)
        {
            Buffer.BlockCopy(uszUILock_HintAnswer, 0, buffer, offset, Math.Min(17, uszUILock_HintAnswer.Length));
        }
        offset += 17;
        buffer[offset++] = byUILockFindPassFailCount;
        for (int i = 0; i < 3; i++)
        {
            var value = dwRequestMoveCharacterSerialList != null && i < dwRequestMoveCharacterSerialList.Length
                ? dwRequestMoveCharacterSerialList[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        for (int i = 0; i < 3; i++)
        {
            var value = dwTournamentCharacterSerialList != null && i < dwTournamentCharacterSerialList.Length
                ? dwTournamentCharacterSerialList[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(offset, 4), value);
            offset += 4;
        }
        buffer[offset++] = bIsPcBang ? (byte)1 : (byte)0;
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(offset, 4), nTrans);
        offset += 4;
        buffer[offset] = bAgeLimit ? (byte)1 : (byte)0;
        return buffer;
    }
}

public struct _uilock_user_refresh_info_result_acwr
{
    public uint dwAccountSerial;
    public byte byResult;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 5) return false;
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        byResult = payload[4];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[5];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwAccountSerial);
        buffer[4] = byResult;
        return buffer;
    }
}

public struct _uilock_update_result_acwr
{
    public byte byRet;
    public ushort wUserIndex;

    public byte[] uszUILockPW;

    public byte byHintIndex;

    public byte[] uszHintAnswer;

    public _uilock_update_result_acwr()
    {
    this = default;
        uszUILockPW = new byte[13];
        uszHintAnswer = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 34) return false;
        byRet = payload[0];
        wUserIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        uszUILockPW ??= new byte[13];
        uszHintAnswer ??= new byte[17];
        Buffer.BlockCopy(payload, 3, uszUILockPW, 0, 13);
        byHintIndex = payload[16];
        Buffer.BlockCopy(payload, 17, uszHintAnswer, 0, 17);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[34];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), wUserIndex);
        if (uszUILockPW != null)
        {
            Buffer.BlockCopy(uszUILockPW, 0, buffer, 3, Math.Min(13, uszUILockPW.Length));
        }
        buffer[16] = byHintIndex;
        if (uszHintAnswer != null)
        {
            Buffer.BlockCopy(uszHintAnswer, 0, buffer, 17, Math.Min(17, uszHintAnswer.Length));
        }
        return buffer;
    }
}

public struct _uilock_init_result_acwr
{
    public byte byRet;
    public ushort wUserIndex;

    public byte[] uszUILockPW;

    public byte byHintIndex;

    public byte[] uszHintAnswer;

    public _uilock_init_result_acwr()
    {
    this = default;
        uszUILockPW = new byte[13];
        uszHintAnswer = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 34) return false;
        byRet = payload[0];
        wUserIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        uszUILockPW ??= new byte[13];
        uszHintAnswer ??= new byte[17];
        Buffer.BlockCopy(payload, 3, uszUILockPW, 0, 13);
        byHintIndex = payload[16];
        Buffer.BlockCopy(payload, 17, uszHintAnswer, 0, 17);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[34];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), wUserIndex);
        if (uszUILockPW != null)
        {
            Buffer.BlockCopy(uszUILockPW, 0, buffer, 3, Math.Min(13, uszUILockPW.Length));
        }
        buffer[16] = byHintIndex;
        if (uszHintAnswer != null)
        {
            Buffer.BlockCopy(uszHintAnswer, 0, buffer, 17, Math.Min(17, uszHintAnswer.Length));
        }
        return buffer;
    }
}

public struct _user_block_result_acwr
{
    public byte byBlockResult;
    public _CLID idLocalForTarget;
    public _CLID idLocalForGM;
    public uint dwAccountSerial;
    public int bLogin;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 21) return false;
        byBlockResult = payload[0];
        idLocalForTarget = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(3, 4))
        };
        idLocalForGM = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(7, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(9, 4))
        };
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(13, 4));
        bLogin = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(17, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[21];
        buffer[0] = byBlockResult;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), idLocalForTarget.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(3, 4), idLocalForTarget.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(7, 2), idLocalForGM.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(9, 4), idLocalForGM.dwSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(13, 4), dwAccountSerial);
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(17, 4), bLogin);
        return buffer;
    }
}

public struct _world_exit_inform_acwr
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

public struct _world_msg_inform_acwr
{
    public ushort wMsgSize;

    public byte[] wszMsg;

    public _world_msg_inform_acwr()
    {
    this = default;
        wszMsg = new byte[1281];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        wMsgSize = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        if (wMsgSize > 1281) return false;
        if (payload.Length < 2 + wMsgSize) return false;
        wszMsg ??= new byte[1281];
        Array.Clear(wszMsg, 0, wszMsg.Length);
        if (wMsgSize > 0)
        {
            Buffer.BlockCopy(payload, 2, wszMsg, 0, wMsgSize);
        }
        return true;
    }

    public byte[] ToArray()
    {
        int msgSize = wMsgSize;
        if (msgSize > 1281) msgSize = 1281;
        var buffer = new byte[2 + msgSize];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), (ushort)msgSize);
        if (msgSize > 0 && wszMsg != null)
        {
            Buffer.BlockCopy(wszMsg, 0, buffer, 2, Math.Min(msgSize, wszMsg.Length));
        }
        return buffer;
    }
}

public struct _world_service_inform_acwr
{
    public bool bService;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        bService = payload[0] != 0;
        return true;
    }

    public byte[] ToArray() => new[] { bService ? (byte)1 : (byte)0 };
}

public struct _world_total_sales_check_time_acwr
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

#endregion

