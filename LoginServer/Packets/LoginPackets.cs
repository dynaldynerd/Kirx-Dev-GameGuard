using System.Buffers.Binary;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace LoginServer.Packets;

/// <summary>Client -> Login: select world request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _select_world_request_cllo
{
    public ushort wWorldIndex;
}

/// <summary>Login -> Client: close/push result.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _push_close_result_locl
{
    public byte byRetCode;
}

/// <summary>Login -> Client: notify message.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _server_notify_inform_locl
{
    public ushort wMsgCode;
}

/// <summary>Login -> Client: new agree (terms) result.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _result_new_agree_locl
{
    public sbyte nRet;
}

/// <summary>Login -> Client: free server availability.</summary>
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
}

/// <summary>Login -> Client: user count indicator per world.</summary>
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
}

/// <summary>Login -> Client: MOTP validation reply.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _motp_validation_reply_locl
{
    public byte byResult;

    [MarshalAs(UnmanagedType.U1)]
    public bool bDuple;
}

/// <summary>Client -> Login: account login request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _login_account_request_cllo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szAccountID;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szPassword;

    public byte byServerType;

    public _login_account_request_cllo()
    {
        this = default;
        szAccountID = new byte[13];
        szPassword = new byte[13];
    }
}

/// <summary>Client -> Login: account join/create request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _join_account_request_cllo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szAccountID;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szPassword;

    public _join_account_request_cllo()
    {
        this = default;
        szAccountID = new byte[13];
        szPassword = new byte[13];
    }
}

/// <summary>Client -> Login: request new agree/terms.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _request_new_agree_cllo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szID;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
    public byte[] nNewAgree;

    public _request_new_agree_cllo()
    {
        this = default;
        szID = new byte[13];
        nNewAgree = new byte[2];
    }
}

/// <summary>Login -> Client: world list result (list blob in sListData).</summary>
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
}

/// <summary>Login -> Client: world selection result.</summary>
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
        // Pack=1 manual layout to ensure size stays correct (1+4+2+16+1=24 bytes).
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

/// <summary>Login -> Client: account login result.</summary>
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
        MemoryMarshal.Write(buffer.AsSpan(), in dst);
        return buffer;
    }
}

/// <summary>Login -> Client: account join result.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _join_account_result_locl
{
    public byte byRetCode;
}

/// <summary>Client -> Login: world list request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _world_list_request_cllo
{
    public uint dwClientVersion;
}

/// <summary>Client -> Login: MOTP validation request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _motp_validation_request_cllo
{
    public byte byType;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szMOTP;

    public _motp_validation_request_cllo()
    {
        this = default;
        szMOTP = new byte[13];
    }
}

/// <summary>Login -> Client: crypt key inform.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _crypty_key_inform_locl
{
    public byte byPlus;
    public ushort wKey;
}

/// <summary>Client -> Login: manage account auth request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_account_auth_request_cllo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] byBin;

    public _manage_account_auth_request_cllo()
    {
        this = default;
        byBin = new byte[32];
    }
}

/// <summary>Client -> Login: push close request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _push_close_request_cllo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szAccountID;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szPassword;

    public _push_close_request_cllo()
    {
        this = default;
        szAccountID = new byte[13];
        szPassword = new byte[13];
    }
}

/// <summary>Client -> Login: crypt key request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _crypty_key_request_cllo
{
}

/// <summary>Client -> Login: manage client limit run request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_client_limit_run_request_cllo
{
}

/// <summary>Client -> Login: manage client force exit request.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public partial struct _manage_client_force_exit_request_cllo
{
}

/// <summary>Login -> Client: notify manage account auth info.</summary>
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

/// <summary>Login -> Client: force exit result.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_force_exit_result_locl
{
    public byte byRet;
}

/// <summary>Login -> Client: manage account auth result.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_account_auth_result_locl
{
    public byte byRet;
}

/// <summary>Login -> Client: limit run account result.</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_account_result_locl
{
    public byte byRet;
}
