using System;
using System.Buffers.Binary;

namespace LoginServer.Packets;

#region World -> Account (wrac)

public struct _world_service_report_wrac
{
    public bool bBefore;
    public bool bAfter;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        bBefore = payload[0] != 0;
        bAfter = payload[1] != 0;
        return true;
    }

    public byte[] ToArray() => new[] { bBefore ? (byte)1 : (byte)0, bAfter ? (byte)1 : (byte)0 };
}

public struct _world_msg_report_wrac
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

public struct _world_exit_report_wrac
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

public struct _world_account_ping_wrac
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

public struct _user_num_report_wrac
{
    public uint dwAveragePerHour;
    public uint dwMaxPerHour;

    public uint[] dwPlayerPerRace;

    public byte[] szLogDate;

    public _user_num_report_wrac()
    {
    this = default;
        dwPlayerPerRace = new uint[3];
        szLogDate = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 37) return false;
        dwAveragePerHour = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        dwMaxPerHour = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4, 4));
        dwPlayerPerRace ??= new uint[3];
        dwPlayerPerRace[0] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(8, 4));
        dwPlayerPerRace[1] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(12, 4));
        dwPlayerPerRace[2] = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(16, 4));
        szLogDate ??= new byte[17];
        Buffer.BlockCopy(payload, 20, szLogDate, 0, 17);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[37];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwAveragePerHour);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), dwMaxPerHour);
        for (int i = 0; i < 3; i++)
        {
            var value = dwPlayerPerRace != null && i < dwPlayerPerRace.Length ? dwPlayerPerRace[i] : 0;
            BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(8 + i * 4, 4), value);
        }
        if (szLogDate != null)
        {
            Buffer.BlockCopy(szLogDate, 0, buffer, 20, Math.Min(17, szLogDate.Length));
        }
        return buffer;
    }
}

public struct _user_block_request_wrac
{
    public byte byBlockType;
    public int iPeriod;
    public _GLBID gidGlobalForTarget;
    public _CLID idLocalForGM;

    public byte[] uszReason;

    public byte[] uszWriter;

    public uint dwAccountSerial;

    public _user_block_request_wrac()
    {
    this = default;
        uszReason = new byte[32];
        uszWriter = new byte[32];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 87) return false;
        byBlockType = payload[0];
        iPeriod = BinaryPrimitives.ReadInt32LittleEndian(payload.AsSpan(1, 4));
        gidGlobalForTarget = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(5, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(9, 4))
        };
        idLocalForGM = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(13, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(15, 4))
        };
        uszReason ??= new byte[32];
        uszWriter ??= new byte[32];
        Buffer.BlockCopy(payload, 19, uszReason, 0, 32);
        Buffer.BlockCopy(payload, 51, uszWriter, 0, 32);
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(83, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[87];
        buffer[0] = byBlockType;
        BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(1, 4), iPeriod);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(5, 4), gidGlobalForTarget.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(9, 4), gidGlobalForTarget.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(13, 2), idLocalForGM.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(15, 4), idLocalForGM.dwSerial);
        if (uszReason != null)
        {
            Buffer.BlockCopy(uszReason, 0, buffer, 19, Math.Min(32, uszReason.Length));
        }
        if (uszWriter != null)
        {
            Buffer.BlockCopy(uszWriter, 0, buffer, 51, Math.Min(32, uszWriter.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(83, 4), dwAccountSerial);
        return buffer;
    }
}

public struct _uilock_user_refresh_info_request_wrac
{
    public uint dwAccountSerial;
    public byte byFailCnt;
    public byte byFindPassFailCount;
    public _GLBID gidGlobal;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 14) return false;
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        byFailCnt = payload[4];
        byFindPassFailCount = payload[5];
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(6, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(10, 4))
        };
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[14];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwAccountSerial);
        buffer[4] = byFailCnt;
        buffer[5] = byFindPassFailCount;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(6, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(10, 4), gidGlobal.dwSerial);
        return buffer;
    }
}

public struct _uilock_update_request_wrac
{
    public ushort wUserIndex;
    public uint dwAccountSerial;

    public byte[] uszUILockPW;

    public byte byHintIndex;

    public byte[] uszHintAnswer;

    public _uilock_update_request_wrac()
    {
    this = default;
        uszUILockPW = new byte[13];
        uszHintAnswer = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 37) return false;
        wUserIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4));
        uszUILockPW ??= new byte[13];
        uszHintAnswer ??= new byte[17];
        Buffer.BlockCopy(payload, 6, uszUILockPW, 0, 13);
        byHintIndex = payload[19];
        Buffer.BlockCopy(payload, 20, uszHintAnswer, 0, 17);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[37];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wUserIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), dwAccountSerial);
        if (uszUILockPW != null)
        {
            Buffer.BlockCopy(uszUILockPW, 0, buffer, 6, Math.Min(13, uszUILockPW.Length));
        }
        buffer[19] = byHintIndex;
        if (uszHintAnswer != null)
        {
            Buffer.BlockCopy(uszHintAnswer, 0, buffer, 20, Math.Min(17, uszHintAnswer.Length));
        }
        return buffer;
    }
}

public struct _uilock_init_request_wrac
{
    public ushort wUserIndex;
    public uint dwAccountSerial;

    public byte[] uszUILockPW;

    public byte byHintIndex;

    public byte[] uszHintAnswer;

    public _uilock_init_request_wrac()
    {
    this = default;
        uszUILockPW = new byte[13];
        uszHintAnswer = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 37) return false;
        wUserIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(2, 4));
        uszUILockPW ??= new byte[13];
        uszHintAnswer ??= new byte[17];
        Buffer.BlockCopy(payload, 6, uszUILockPW, 0, 13);
        byHintIndex = payload[19];
        Buffer.BlockCopy(payload, 20, uszHintAnswer, 0, 17);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[37];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wUserIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(2, 4), dwAccountSerial);
        if (uszUILockPW != null)
        {
            Buffer.BlockCopy(uszUILockPW, 0, buffer, 6, Math.Min(13, uszUILockPW.Length));
        }
        buffer[19] = byHintIndex;
        if (uszHintAnswer != null)
        {
            Buffer.BlockCopy(uszHintAnswer, 0, buffer, 20, Math.Min(17, uszHintAnswer.Length));
        }
        return buffer;
    }
}

public struct _trans_account_report_wrac
{
    public _GLBID gidGlobal;
    public byte byRetCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 9) return false;
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4, 4))
        };
        byRetCode = payload[8];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[9];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        buffer[8] = byRetCode;
        return buffer;
    }
}

public struct _stop_world_request_wrac
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

public struct _start_world_request_wrac
{
    public uint dwGateIP;
    public ushort wGatePort;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 6) return false;
        dwGateIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        wGatePort = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(4, 2));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[6];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwGateIP);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(4, 2), wGatePort);
        return buffer;
    }
}

public struct _select_avator_report_wrac
{
    public _GLBID gidGlobal;

    public byte[] wszCharName;

    public uint dwAvatorSerial;
    public byte byLevel;

    public _select_avator_report_wrac()
    {
    this = default;
        wszCharName = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 30) return false;
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4, 4))
        };
        wszCharName ??= new byte[17];
        Buffer.BlockCopy(payload, 8, wszCharName, 0, 17);
        dwAvatorSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(25, 4));
        byLevel = payload[29];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[30];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        if (wszCharName != null)
        {
            Buffer.BlockCopy(wszCharName, 0, buffer, 8, Math.Min(17, wszCharName.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(25, 4), dwAvatorSerial);
        buffer[29] = byLevel;
        return buffer;
    }
}

public struct _open_world_request_wrac
{
    public byte[] szWorldName;
    public byte byRunMode;
    public uint dwWorldServerIP;

    public byte[] byHash;

    public _open_world_request_wrac()
    {
    this = default;
        szWorldName = new byte[33];
        byHash = new byte[32];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 70) return false;
        szWorldName ??= new byte[33];
        byHash ??= new byte[32];
        Buffer.BlockCopy(payload, 0, szWorldName, 0, 33);
        byRunMode = payload[33];
        dwWorldServerIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(34, 4));
        Buffer.BlockCopy(payload, 38, byHash, 0, 32);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[70];
        if (szWorldName != null)
        {
            Buffer.BlockCopy(szWorldName, 0, buffer, 0, Math.Min(33, szWorldName.Length));
        }
        buffer[33] = byRunMode;
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(34, 4), dwWorldServerIP);
        if (byHash != null)
        {
            Buffer.BlockCopy(byHash, 0, buffer, 38, Math.Min(32, byHash.Length));
        }
        return buffer;
    }
}

public struct _manage_client_limit_run_result_wrac
{
    public byte byRet;
    public _CLID idLocal;
    public byte byLoginServerIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 8) return false;
        byRet = payload[0];
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(1, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(3, 4))
        };
        byLoginServerIndex = payload[7];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[8];
        buffer[0] = byRet;
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(1, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(3, 4), idLocal.dwSerial);
        buffer[7] = byLoginServerIndex;
        return buffer;
    }
}

public struct _logout_account_request_wrac
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

public struct _holy_quest_report_wrac
{
    public byte byRaceCode;

    public byte[] wszCharName;

    public byte byDestroyedRaceCode;

    public _holy_quest_report_wrac()
    {
    this = default;
        wszCharName = new byte[17];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 19) return false;
        byRaceCode = payload[0];
        wszCharName ??= new byte[17];
        Buffer.BlockCopy(payload, 1, wszCharName, 0, 17);
        byDestroyedRaceCode = payload[18];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[19];
        buffer[0] = byRaceCode;
        if (wszCharName != null)
        {
            Buffer.BlockCopy(wszCharName, 0, buffer, 1, Math.Min(17, wszCharName.Length));
        }
        buffer[18] = byDestroyedRaceCode;
        return buffer;
    }
}

public struct _holy_quest_now_report_wrac
{
    public byte byMasterRaceCode;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 1) return false;
        byMasterRaceCode = payload[0];
        return true;
    }

    public byte[] ToArray() => new[] { byMasterRaceCode };
}

public struct _fireguard_divide_block_request_wrac
{
    public byte[] szAccountID;
    public uint dwAccountSerial;
    public uint dwIP;

    public _fireguard_divide_block_request_wrac()
    {
    this = default;
        szAccountID = new byte[13];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 21) return false;
        szAccountID ??= new byte[13];
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(13, 4));
        dwIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(17, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[21];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 0, Math.Min(13, szAccountID.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(13, 4), dwAccountSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(17, 4), dwIP);
        return buffer;
    }
}

public struct _fireguard_block_request_wrac
{
    public byte[] szAccountID;
    public uint dwAccountSerial;
    public uint dwIP;

    public _fireguard_block_request_wrac()
    {
    this = default;
        szAccountID = new byte[13];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 21) return false;
        szAccountID ??= new byte[13];
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(13, 4));
        dwIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(17, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[21];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 0, Math.Min(13, szAccountID.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(13, 4), dwAccountSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(17, 4), dwIP);
        return buffer;
    }
}

public struct _enter_world_request_wrac
{
    public _GLBID gidGlobal;
    public _CLID idLocal;
    public uint ulConnectIP;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 18) return false;
        gidGlobal = new _GLBID
        {
            dwIndex = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(4, 4))
        };
        idLocal = new _CLID
        {
            wIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(8, 2)),
            dwSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(10, 4))
        };
        ulConnectIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(14, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[18];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), gidGlobal.dwIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(4, 4), gidGlobal.dwSerial);
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(8, 2), idLocal.wIndex);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(10, 4), idLocal.dwSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(14, 4), ulConnectIP);
        return buffer;
    }
}

public struct _enter_lobby_report_wrac
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

public struct _apex_block_request_wrac
{
    public byte[] szAccountID;
    public uint dwAccountSerial;
    public uint dwIP;

    public _apex_block_request_wrac()
    {
    this = default;
        szAccountID = new byte[13];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 21) return false;
        szAccountID ??= new byte[13];
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(13, 4));
        dwIP = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(17, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[21];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 0, Math.Min(13, szAccountID.Length));
        }
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(13, 4), dwAccountSerial);
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(17, 4), dwIP);
        return buffer;
    }
}

public struct _character_disconnect_result_wrac
{
    public ushort wClientIndex;
    public byte byResult;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 3) return false;
        wClientIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        byResult = payload[2];
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[3];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wClientIndex);
        buffer[2] = byResult;
        return buffer;
    }
}

public struct _cashdb_setting_request_wrac
{
    public bool Load(byte[] payload) => payload.Length >= 0;

    public byte[] ToArray() => Array.Empty<byte>();
}

#endregion
