using System;
using System.Buffers.Binary;

namespace RFPackets.Login;

// Client -> Login packets (_cllo)
public sealed class _join_account_request_cllo
{
    public byte[] szAccountID = new byte[13];
    public byte[] szPassword = new byte[13];

    public bool Load(byte[] payload)
    {
        if (payload.Length < 26) return false;
        payload.AsSpan(0, 13).CopyTo(szAccountID);
        payload.AsSpan(13, 13).CopyTo(szPassword);
        return true;
    }
}

public sealed class _login_account_request_cllo
{
    public byte[] szAccountID = new byte[13];
    public byte[] szPassword = new byte[13];
    public byte byServerType;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 27) return false;
        payload.AsSpan(0, 13).CopyTo(szAccountID);
        payload.AsSpan(13, 13).CopyTo(szPassword);
        byServerType = payload[26];
        return true;
    }
}

public sealed class _world_list_request_cllo
{
    public uint dwClientVersion;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwClientVersion = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }
}

public sealed class _select_world_request_cllo
{
    public ushort wWorldIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        wWorldIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        return true;
    }
}

public sealed class _push_close_request_cllo
{
    public byte[] szAccountID = new byte[13];
    public byte[] szPassword = new byte[13];
    public uint dwAccountSerial;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 30) return false;
        payload.AsSpan(0, 13).CopyTo(szAccountID);
        payload.AsSpan(13, 13).CopyTo(szPassword);
        dwAccountSerial = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(26, 4));
        return true;
    }
}

public sealed class _crypty_key_request_cllo
{
    public bool Load(byte[] payload) => true; // empty struct
}

public sealed class _motp_validation_request_cllo
{
    public byte byType;
    public byte[] szMOTP = new byte[13];

    public bool Load(byte[] payload)
    {
        if (payload.Length < 14) return false;
        byType = payload[0];
        payload.AsSpan(1, 13).CopyTo(szMOTP);
        return true;
    }
}

public sealed class _manage_account_auth_request_cllo
{
    public byte[] byBin = new byte[32];

    public bool Load(byte[] payload)
    {
        if (payload.Length < 32) return false;
        payload.AsSpan(0, 32).CopyTo(byBin);
        return true;
    }
}

public sealed class _manage_client_limit_run_request_cllo
{
    public bool Load(byte[] payload) => true;
}

public sealed class _manage_client_force_exit_request_cllo
{
    public bool Load(byte[] payload) => true;
}

public sealed class _request_new_agree_cllo
{
    public byte[] szID = new byte[13];
    public byte[] nNewAgree = new byte[2];

    public bool Load(byte[] payload)
    {
        if (payload.Length < 15) return false;
        payload.AsSpan(0, 13).CopyTo(szID);
        payload.AsSpan(13, 2).CopyTo(nNewAgree);
        return true;
    }
}
