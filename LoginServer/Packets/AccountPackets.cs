using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace LoginServer.Packets;

/// <summary>Global ID (as seen in login/account exchange).</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _GLBID
{
    public uint dwIndex;
    public uint dwSerial;
}

/// <summary>Client ID (local to login server).</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _CLID
{
    public ushort wIndex;
    public uint dwSerial;
}

/// <summary>Win32 SYSTEMTIME equivalent.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 2)]
public partial struct _SYSTEMTIME
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
public partial struct _select_world_request_loac
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
public partial struct _login_account_request_loac
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
        // szCMS left zeroed
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
            bAgeLimit = 0,
            bCancelWebUILockBlock = 0
        };

        FillFixed(req.szAccountID, 13, session.AccountId);
        FillFixed(req.szPassword, 13, session.Password);
        // szCMS left zeroed
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
        var bytes = System.Text.Encoding.ASCII.GetBytes(value);
        int copy = Math.Min(bytes.Length, length);
        for (int i = 0; i < copy; i++)
        {
            target[i] = bytes[i];
        }
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _logout_account_request_loac
{
    public _GLBID gidGlobal;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _world_list_request_loac
{
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _join_account_request_loac
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
public partial struct _push_close_request_loac
{
    public _CLID idLocal;
    public byte byUserCode;
    public uint dwAccountSerial;
    public uint dwClientIP;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _login_server_stat_result_loac
{
    public byte byRet;
    public ushort wClientIndex;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _update_user_login_failure_cnt_loac
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
public partial struct _manage_account_auth_request_loac
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
public partial struct _manage_client_force_exit_request_loac
{
    public _CLID idLocal;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_client_limit_run_request_loac
{
    public _CLID idLocal;
}

#endregion

#region Account -> Login (aclo)

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _join_account_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _holy_quest_now_report_aclo
{
    public ushort wWorldCode;
    public byte byMasterRaceCode;
}

[StructLayout(LayoutKind.Sequential, Pack = 2)]
public partial struct _inform_open_world_aclo
{
    public uint dwWorldCode;
    public uint dwGateIP;
    public ushort wGatePort;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _login_server_stat_request_aclo
{
    public byte byStat;
    public ushort wClientIndex;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _inform_usernum_world_aclo
{
    public byte byServiceWorldNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
    public ushort[] wUserNum;

    public _inform_usernum_world_aclo()
    {
        this = default;
        wUserNum = new ushort[40];
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _login_account_result_aclo
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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _account_db_info_result_aclo
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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _world_list_result_aclo
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
    public partial struct _world_list_result_aclo__list
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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _force_close_command_aclo
{
    public _CLID idLocal;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _push_close_result_aclo
{
    public _CLID idLocal;
    public byte byRetCode;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _inform_close_world_aclo
{
    public uint dwWorldCode;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _select_world_result_aclo
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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _notify_manage_account_auth_info_aclo
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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_client_limit_run_account_result_aclo
{
    public _CLID idLocal;
    public byte byRet;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_client_force_exit_result_aclo
{
    public _CLID idLocal;
    public byte byRet;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_client_limit_run_world_result_aclo
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
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_account_auth_result_aclo
{
    public _CLID idLocal;
    public byte byRet;
}

#endregion

