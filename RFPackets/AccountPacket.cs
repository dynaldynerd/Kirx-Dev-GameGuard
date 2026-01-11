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

    public byte[] ToArray()
    {
        var buffer = new byte[48];
        if (szDBName != null)
        {
            Buffer.BlockCopy(szDBName, 0, buffer, 0, Math.Min(32, szDBName.Length));
        }
        if (szIP != null)
        {
            Buffer.BlockCopy(szIP, 0, buffer, 32, Math.Min(16, szIP.Length));
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



