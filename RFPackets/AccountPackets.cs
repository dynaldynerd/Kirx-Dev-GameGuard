using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace RFPackets.Login;

// Account -> Login packets (_aclo)
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _join_account_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 7) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRetCode = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _holy_quest_now_report_aclo
{
    public ushort wWorldCode;
    public byte byMasterRaceCode;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 3) return false;
        wWorldCode = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        byMasterRaceCode = payload[2];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _inform_open_world_aclo
{
    public uint dwWorldCode;
    public uint dwGateIP;
    public ushort wGatePort;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 10) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(0, 4));
        dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(4, 4));
        wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(8, 2));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _inform_close_world_aclo
{
    public uint dwWorldCode;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 4) return false;
        dwWorldCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(0, 4));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _inform_usernum_world_aclo
{
    public byte byServiceWorldNum;
    public ushort[] wUserNum = new ushort[40];

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 1) return false;
        byServiceWorldNum = payload[0];
        int offset = 1;
        for (int i = 0; i < 40 && offset + 2 <= payload.Length; i++)
        {
            wUserNum[i] = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(offset, 2));
            offset += 2;
        }
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _login_server_stat_request_aclo
{
    public byte byStat;
    public ushort wClientIndex;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 3) return false;
        byStat = payload[0];
        wClientIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(1, 2));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _login_account_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
    public _GLBID gidNewGlobal;
    public uint dwAccountSerial;
    public byte byUserGrade;
    public byte bySubGrade;
    public byte[] uszBlockReason = new byte[32];
    public int nTrans;
    public byte byBlockReasonType;
    public byte byCancelUILockBlockRet;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 59) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRetCode = payload[6];
        gidNewGlobal.dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(7, 4));
        gidNewGlobal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(11, 4));
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(15, 4));
        byUserGrade = payload[19];
        bySubGrade = payload[20];
        payload.Slice(21, 32).CopyTo(uszBlockReason);
        nTrans = BinaryPrimitives.ReadInt32LittleEndian(payload.Slice(53, 4));
        byBlockReasonType = payload[57];
        byCancelUILockBlockRet = payload[58];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _account_db_info_result_aclo
{
    public byte[] szDBName = new byte[32];
    public byte[] szIP = new byte[16];

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 48) return false;
        payload.Slice(0, 32).CopyTo(szDBName);
        payload.Slice(32, 16).CopyTo(szIP);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _world_list_result_aclo__list
{
    public bool bOpen;
    public byte[] szWorldName = new byte[33];
    public uint dwGateIP;
    public ushort wGatePort;
    public byte byType;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _world_list_result_aclo
{
    public byte byServiceWorldNum;
    public byte byWorldNum;
    public _world_list_result_aclo__list[] WorldList = new _world_list_result_aclo__list[40];

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 2) return false;
        byServiceWorldNum = payload[0];
        byWorldNum = payload[1];
        int offset = 2;
        for (int i = 0; i < 40 && offset + 41 <= payload.Length; i++)
        {
            var entry = new _world_list_result_aclo__list
            {
                bOpen = payload[offset] != 0
            };
            payload.Slice(offset + 1, 33).CopyTo(entry.szWorldName);
            entry.dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(offset + 34, 4));
            entry.wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(offset + 38, 2));
            entry.byType = payload[offset + 40];
            WorldList[i] = entry;
            offset += 41;
        }
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _force_close_command_aclo
{
    public _CLID idLocal;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 6) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _push_close_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 7) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRetCode = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _select_world_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
    public uint[] dwWorldMasterKey = new uint[4];

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 23) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRetCode = payload[6];
        for (int i = 0; i < 4; i++)
        {
            dwWorldMasterKey[i] = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(7 + i * 4, 4));
        }
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _notify_manage_account_auth_info_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
    public byte[] byKey = new byte[149];

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 6 + 1 + 149) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRetCode = payload[6];
        payload.Slice(7, 149).CopyTo(byKey);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_limit_run_account_result_aclo
{
    public _CLID idLocal;
    public byte byRet;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 7) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRet = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_force_exit_result_aclo
{
    public _CLID idLocal;
    public byte byRet;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 7) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRet = payload[6];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_client_limit_run_world_result_aclo
{
    public _CLID idLocal;
    public byte byRet;
    public uint m_dwCode;
    public byte[] m_szName = new byte[32];
    public byte[] m_szDBName = new byte[32];
    public byte m_byType;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 6 + 1 + 4 + 32 + 32 + 1) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRet = payload[6];
        m_dwCode = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(7, 4));
        payload.Slice(11, 32).CopyTo(m_szName);
        payload.Slice(43, 32).CopyTo(m_szDBName);
        m_byType = payload[75];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public sealed class _manage_account_auth_result_aclo
{
    public _CLID idLocal;
    public byte byRet;

    public bool Load(ReadOnlySpan<byte> payload)
    {
        if (payload.Length < 7) return false;
        idLocal.wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.Slice(0, 2));
        idLocal.dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.Slice(2, 4));
        byRet = payload[6];
        return true;
    }
}
