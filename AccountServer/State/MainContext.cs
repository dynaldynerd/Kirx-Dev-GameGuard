using System;
using System.Collections.Concurrent;
using System.Linq;
using RFNetworking;
using LoginServer.Packets;

namespace AccountServer.State;

public sealed class AccountMainContext
{
    public static AccountMainContext Instance { get; } = new();

    private readonly ConcurrentDictionary<ulong, LoginServerSession> _loginServers = new();
    private readonly ConcurrentDictionary<uint, ClientSession> _activeAccounts = new();
    private readonly ConcurrentDictionary<int, WorldServerSession> _worldServers = new();
    private readonly ConcurrentDictionary<ulong, WorldServerSession> _worldConnections = new();

    private AccountMainContext() { }

    public LoginServerSession Register(PublicConnection connection)
    {
        if (_loginServers.TryGetValue(connection.ConnectionId, out var existing))
        {
            existing.UpdateConnection(connection);
            return existing;
        }

        var session = new LoginServerSession(connection.ConnectionId, connection);
        _loginServers[connection.ConnectionId] = session;
        return session;
    }

    public void Unregister(PublicConnection connection)
    {
        if (_loginServers.TryRemove(connection.ConnectionId, out var loginSession))
        {
            foreach (var client in loginSession.GetClientsSnapshot())
            {
                if (client.AccountSerial != 0)
                {
                    _activeAccounts.TryRemove(client.AccountSerial, out _);
                }
            }
            loginSession.RemoveAllClients();
        }

        UnregisterWorld(connection);
    }

    public LoginServerSession? GetSession(ulong connectionId)
    {
        _loginServers.TryGetValue(connectionId, out var session);
        return session;
    }

    public bool TryGetLoginConnection(ulong connectionId, out PublicConnection connection)
    {
        if (_loginServers.TryGetValue(connectionId, out var session) && session.Connection != null)
        {
            connection = session.Connection;
            return true;
        }

        connection = null!;
        return false;
    }

    public ClientSession RegisterClient(ulong loginConnId, _CLID clid)
    {
        var loginSession = _loginServers.GetOrAdd(loginConnId, id => new LoginServerSession(id, null));
        return loginSession.RegisterClient(clid);
    }

    public void RemoveClient(ulong loginConnId, _CLID clid)
    {
        if (_loginServers.TryGetValue(loginConnId, out var session))
        {
            session.RemoveClient(clid);
        }
    }

    public bool TryGetActiveAccount(uint accountSerial, out ClientSession session)
    {
        return _activeAccounts.TryGetValue(accountSerial, out session!);
    }

    public void RegisterActiveAccount(ClientSession session)
    {
        if (session.AccountSerial == 0) return;
        _activeAccounts[session.AccountSerial] = session;
    }

    public void RemoveActiveAccount(uint accountSerial)
    {
        if (accountSerial == 0) return;
        _activeAccounts.TryRemove(accountSerial, out _);
    }

    public WorldServerSession RegisterWorld(int worldCode, PublicConnection connection)
    {
        var session = new WorldServerSession(connection.ConnectionId, worldCode, connection);
        _worldServers[worldCode] = session;
        _worldConnections[connection.ConnectionId] = session;
        return session;
    }

    public bool TryGetWorldConnection(int worldCode, out PublicConnection connection)
    {
        if (_worldServers.TryGetValue(worldCode, out var session) && session.Connection != null)
        {
            connection = session.Connection;
            return true;
        }

        connection = null!;
        return false;
    }

    public void UnregisterWorld(PublicConnection connection)
    {
        if (_worldConnections.TryRemove(connection.ConnectionId, out var session))
        {
            _worldServers.TryRemove(session.WorldCode, out _);
        }
    }
}

public sealed class ClientSession
{
    public ulong ConnectionId { get; }
    public _CLID Clid { get; }

    public string AccountId { get; private set; } = string.Empty;
    public string Password { get; private set; } = string.Empty;
    public uint ClientIp { get; private set; }
    public byte UserCode { get; private set; }
    public bool CheckDoubleIp { get; private set; }
    public short Type { get; private set; }
    public int RemainTime { get; private set; }
    public byte[] Cms { get; } = new byte[7];
    public _SYSTEMTIME EndDate { get; private set; }
    public bool IsPCBang { get; private set; }
    public int Trans { get; private set; }
    public bool AgeLimit { get; set; }
    public byte AuthType { get; private set; }
    public byte UserGrade { get; set; }
    public byte SubGrade { get; set; }
    public uint AccountSerial { get; set; }
    public byte RetCode { get; set; }
    public byte BlockReasonType { get; set; }
    public byte CancelUiLockBlockRet { get; set; }
    public byte[] BlockReason { get; } = new byte[32];
    public bool ChatLock { get; set; }
    public _GLBID GlobalId { get; set; }
    public int WorldCode { get; set; } = -1;

    public ClientSession(ulong loginConnId, _CLID clid)
    {
        ConnectionId = loginConnId;
        Clid = clid;
        GlobalId = new _GLBID { dwIndex = 0, dwSerial = uint.MaxValue };
    }

    public void UpdateFromRequest(_login_account_request_loac_blit req)
    {
        AccountId = PacketStringUtil.ToAsciiNullTerm(req.GetAccountId());
        Password = PacketStringUtil.ToAsciiNullTerm(req.GetPassword());
        ClientIp = req.dwClientIP;
        UserCode = req.byUserCode;
        CheckDoubleIp = req.bCheckDoubleIP != 0;
        Type = req.iType;
        RemainTime = req.lRemainTime;
        EndDate = new _SYSTEMTIME
        {
            wYear = req.wYear,
            wMonth = req.wMonth,
            wDayOfWeek = req.wDayOfWeek,
            wDay = req.wDay,
            wHour = req.wHour,
            wMinute = req.wMinute,
            wSecond = req.wSecond,
            wMilliseconds = req.wMilliseconds
        };
        IsPCBang = req.bPrimium != 0;
        Trans = req.nTrans;
        AgeLimit = req.bAgeLimit != 0;
        AuthType = (byte)req.authtype;
        if (req.szCMS != null)
        {
            Buffer.BlockCopy(req.szCMS, 0, Cms, 0, Math.Min(Cms.Length, req.szCMS.Length));
        }
    }
}

public sealed class LoginServerSession
{
    public ulong ConnectionId { get; }
    public PublicConnection? Connection { get; private set; }
    private readonly ConcurrentDictionary<string, ClientSession> _clients = new();

    public LoginServerSession(ulong connectionId, PublicConnection? connection)
    {
        ConnectionId = connectionId;
        Connection = connection;
    }

    private static string KeyFor(_CLID clid) => $"{clid.wIndex}:{clid.dwSerial}";

    public ClientSession RegisterClient(_CLID clid)
    {
        return _clients.GetOrAdd(KeyFor(clid), _ => new ClientSession(ConnectionId, clid));
    }

    public void UpdateConnection(PublicConnection connection)
    {
        Connection = connection;
    }

    public ClientSession[] GetClientsSnapshot()
    {
        return _clients.Values.ToArray();
    }

    public void RemoveClient(_CLID clid)
    {
        _clients.TryRemove(KeyFor(clid), out _);
    }

    public void RemoveAllClients() => _clients.Clear();
}

public sealed class WorldServerSession
{
    public ulong ConnectionId { get; }
    public int WorldCode { get; private set; }
    public PublicConnection? Connection { get; private set; }

    public WorldServerSession(ulong connectionId, int worldCode, PublicConnection? connection)
    {
        ConnectionId = connectionId;
        WorldCode = worldCode;
        Connection = connection;
    }

    public void UpdateConnection(PublicConnection connection)
    {
        Connection = connection;
    }

    public void UpdateWorldCode(int worldCode)
    {
        WorldCode = worldCode;
    }
}
