using System.Buffers.Binary;

namespace LoginServer.Packets;

/// <summary>Client -> Login packets (_cllo).</summary>
public struct _select_world_request_cllo
{
    public ushort wWorldIndex;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 2) return false;
        wWorldIndex = BinaryPrimitives.ReadUInt16LittleEndian(payload.AsSpan(0, 2));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[2];
        BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(0, 2), wWorldIndex);
        return buffer;
    }
}

public struct _login_account_request_cllo
{
    
    public byte[] szAccountID;

    
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

    public byte[] ToArray()
    {
        var buffer = new byte[27];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 0, Math.Min(13, szAccountID.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 13, Math.Min(13, szPassword.Length));
        }
        buffer[26] = byServerType;
        return buffer;
    }
}

public struct _join_account_request_cllo
{
    
    public byte[] szAccountID;

    
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

    public byte[] ToArray()
    {
        var buffer = new byte[26];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 0, Math.Min(13, szAccountID.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 13, Math.Min(13, szPassword.Length));
        }
        return buffer;
    }
}

public struct _request_new_agree_cllo
{
    
    public byte[] szID;

    
    public byte[] nNewAgree;

    public _request_new_agree_cllo()
    {
    this = default;
        szID = new byte[13];
        nNewAgree = new byte[2];
    }

    public bool Load(byte[] payload)
    {
        if (payload.Length < 15) return false;
        Buffer.BlockCopy(payload, 0, szID, 0, 13);
        Buffer.BlockCopy(payload, 13, nNewAgree, 0, 2);
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[15];
        if (szID != null)
        {
            Buffer.BlockCopy(szID, 0, buffer, 0, Math.Min(13, szID.Length));
        }
        if (nNewAgree != null)
        {
            Buffer.BlockCopy(nNewAgree, 0, buffer, 13, Math.Min(2, nNewAgree.Length));
        }
        return buffer;
    }
}

public struct _world_list_request_cllo
{
    public uint dwClientVersion;

    public bool Load(byte[] payload)
    {
        if (payload.Length < 4) return false;
        dwClientVersion = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        return true;
    }

    public byte[] ToArray()
    {
        var buffer = new byte[4];
        BinaryPrimitives.WriteUInt32LittleEndian(buffer.AsSpan(0, 4), dwClientVersion);
        return buffer;
    }
}

public struct _motp_validation_request_cllo
{
    public byte byType;

    
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

    public byte[] ToArray()
    {
        var buffer = new byte[14];
        buffer[0] = byType;
        if (szMOTP != null)
        {
            Buffer.BlockCopy(szMOTP, 0, buffer, 1, Math.Min(13, szMOTP.Length));
        }
        return buffer;
    }
}

public struct _manage_account_auth_request_cllo
{
    
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

    public byte[] ToArray()
    {
        var buffer = new byte[32];
        if (byBin != null)
        {
            Buffer.BlockCopy(byBin, 0, buffer, 0, Math.Min(32, byBin.Length));
        }
        return buffer;
    }
}

public struct _push_close_request_cllo
{
    
    public byte[] szAccountID;

    
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

    public byte[] ToArray()
    {
        var buffer = new byte[26];
        if (szAccountID != null)
        {
            Buffer.BlockCopy(szAccountID, 0, buffer, 0, Math.Min(13, szAccountID.Length));
        }
        if (szPassword != null)
        {
            Buffer.BlockCopy(szPassword, 0, buffer, 13, Math.Min(13, szPassword.Length));
        }
        return buffer;
    }
}

public struct _crypty_key_request_cllo
{
    public bool Load(byte[] payload) => payload.Length >= 1;

    public byte[] ToArray() => new byte[1];
}

public struct _manage_client_limit_run_request_cllo
{
    public bool Load(byte[] payload) => payload.Length >= 1;

    public byte[] ToArray() => new byte[1];
}

public struct _manage_client_force_exit_request_cllo
{
    public bool Load(byte[] payload) => payload.Length >= 1;

    public byte[] ToArray() => new byte[1];
}


