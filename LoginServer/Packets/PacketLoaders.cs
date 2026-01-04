using System;
using System.Buffers.Binary;
using System.Text;

namespace LoginServer.Packets;

internal static class PacketStringUtil
{
    public static string ToAscii(byte[] bytes) => Encoding.ASCII.GetString(bytes).TrimEnd('\0');
}

public partial struct _world_list_result_aclo
{
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

public partial struct _inform_open_world_aclo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 10) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload);
        dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4));
        wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(8));
        return true;
    }
}

public partial struct _inform_close_world_aclo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }
}

public partial struct _holy_quest_now_report_aclo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        wWorldCode = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        byMasterRaceCode = payload[2];
        return true;
    }
}

public partial struct _inform_usernum_world_aclo
{
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

public partial struct _join_account_result_aclo
{
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

public partial struct _login_account_result_aclo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 60) return false;
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

public partial struct _account_db_info_result_aclo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 48) return false;
        Buffer.BlockCopy(payload, 0, szDBName, 0, 32);
        Buffer.BlockCopy(payload, 32, szIP, 0, 16);
        return true;
    }
}

public partial struct _select_world_result_aclo
{
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

public partial struct _force_close_command_aclo
{
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

public partial struct _push_close_result_aclo
{
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

public partial struct _notify_manage_account_auth_info_aclo
{
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

public partial struct _manage_client_limit_run_world_result_aclo
{
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

public partial struct _login_server_stat_request_aclo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        byStat = payload[0];
        wClientIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2));
        return true;
    }
}

public partial struct _manage_account_auth_result_aclo
{
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

public partial struct _manage_client_limit_run_account_result_aclo
{
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

public partial struct _manage_client_force_exit_result_aclo
{
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

// ----- Client-line (cllo/locl) loaders -----

public partial struct _join_account_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 26) return false;
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 13, szPassword, 0, 13);
        return true;
    }
}

public partial struct _login_account_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 27) return false;
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 13, szPassword, 0, 13);
        byServerType = payload[26];
        return true;
    }
}

public partial struct _world_list_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwClientVersion = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }
}

public partial struct _select_world_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        wWorldIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        return true;
    }
}

public partial struct _motp_validation_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 14) return false;
        byType = payload[0];
        Buffer.BlockCopy(payload, 1, szMOTP, 0, 13);
        return true;
    }
}

public partial struct _manage_account_auth_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 32) return false;
        Buffer.BlockCopy(payload, 0, byBin, 0, 32);
        return true;
    }
}

public partial struct _push_close_request_cllo
{
    public bool Load(byte[] payload)
    {
        if (payload.Length < 26) return false;
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 13, szPassword, 0, 13);
        return true;
    }
}

public partial struct _crypty_key_request_cllo
{
    public bool Load(byte[] payload) => true; // no payload data
}

public partial struct _manage_client_limit_run_request_cllo
{
    public bool Load(byte[] payload) => true; // empty
}

public partial struct _manage_client_force_exit_request_cllo
{
    public bool Load(byte[] payload) => true; // empty
}
