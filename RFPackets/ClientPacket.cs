using System.Buffers.Binary;
using System.Runtime.InteropServices;

namespace LoginServer.Packets;

/// <summary>Client -> Login packets (_cllo).</summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _select_world_request_cllo
{
    public ushort wWorldIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        wWorldIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _login_account_request_cllo
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

    public bool Load(byte[] payload)
    {
        if (payload.Length < 27) return false;
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 13, szPassword, 0, 13);
        byServerType = payload[26];
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _join_account_request_cllo
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

    public bool Load(byte[] payload)
    {
        if (payload.Length < 26) return false;
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 13, szPassword, 0, 13);
        return true;
    }
}

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

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _world_list_request_cllo
{
    public uint dwClientVersion;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwClientVersion = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _motp_validation_request_cllo
{
    public byte byType;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 13)]
    public byte[] szMOTP;

    public _motp_validation_request_cllo()
    {
        this = default;
        szMOTP = new byte[13];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 14) return false;
        byType = payload[0];
        Buffer.BlockCopy(payload, 1, szMOTP, 0, 13);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_account_auth_request_cllo
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public byte[] byBin;

    public _manage_account_auth_request_cllo()
    {
        this = default;
        byBin = new byte[32];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 32) return false;
        Buffer.BlockCopy(payload, 0, byBin, 0, 32);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _push_close_request_cllo
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

    public bool Load(byte[] payload)
    {
        if (payload.Length < 26) return false;
        Buffer.BlockCopy(payload, 0, szAccountID, 0, 13);
        Buffer.BlockCopy(payload, 13, szPassword, 0, 13);
        return true;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _crypty_key_request_cllo
{
    public bool Load(byte[] payload) => true;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_limit_run_request_cllo
{
    public bool Load(byte[] payload) => true;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct _manage_client_force_exit_request_cllo
{
    public bool Load(byte[] payload) => true;
}
