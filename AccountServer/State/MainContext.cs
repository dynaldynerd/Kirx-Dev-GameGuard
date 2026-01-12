using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Net;
using AccountServer.Settings;
using RFNetworking;
using LoginServer.Packets;

namespace AccountServer.State;

public sealed class AccountMainContext
{
    public static AccountMainContext Instance { get; } = new();

    private readonly object _worldLock = new();
    private readonly WorldEntry[] _worlds = new WorldEntry[40];
    private int _worldCount;
    private readonly ConcurrentDictionary<ulong, LoginServerSession> _loginServers = new();
    private readonly ConcurrentDictionary<string, ClientSession> _clients = new();
    private readonly ConcurrentDictionary<uint, ClientSession> _activeAccounts = new();
    private readonly ConcurrentDictionary<uint, ClientSession> _activeGlobals = new();
    private readonly ConcurrentDictionary<int, WorldServerSession> _worldServers = new();
    private readonly ConcurrentDictionary<ulong, WorldServerSession> _worldConnections = new();

    private AccountMainContext() { }

    public int WorldCount
    {
        get
        {
            lock (_worldLock)
            {
                return _worldCount;
            }
        }
    }

    public void LoadWorldList(IEnumerable<WorldEntry> worlds)
    {
        lock (_worldLock)
        {
            int i = 0;
            foreach (var entry in worlds)
            {
                if (i >= _worlds.Length) break;
                _worlds[i] = CloneWorldEntry(entry);
                _worlds[i].IsService = false;
                ParseGateAddress(ref _worlds[i]);
                i++;
            }
            _worldCount = i;
            for (; i < _worlds.Length; i++)
            {
                _worlds[i] = new WorldEntry();
            }
        }
    }

    public bool TryGetWorld(int index, out WorldEntry world)
    {
        lock (_worldLock)
        {
            if (index < 0 || index >= _worldCount)
            {
                world = new WorldEntry();
                return false;
            }
            world = _worlds[index];
            return true;
        }
    }

    public WorldEntry[] GetWorldSnapshot()
    {
        lock (_worldLock)
        {
            var copy = new WorldEntry[_worlds.Length];
            for (int i = 0; i < _worlds.Length; i++)
            {
                copy[i] = CloneWorldEntry(_worlds[i]);
            }
            return copy;
        }
    }

    public void UpdateWorldService(int index, bool isService)
    {
        lock (_worldLock)
        {
            if (index < 0 || index >= _worldCount) return;
            _worlds[index].IsService = isService;
        }
    }

    public void UpdateWorldGate(int index, uint gateIp, ushort gatePort)
    {
        lock (_worldLock)
        {
            if (index < 0 || index >= _worldCount) return;
            _worlds[index].GateIp = gateIp;
            _worlds[index].GatePort = gatePort;
        }
    }

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
        _loginServers.TryRemove(connection.ConnectionId, out _);
    }

    public LoginServerSession[] GetLoginSessionsSnapshot()
    {
        return _loginServers.Values.ToArray();
    }

    public bool TryFindWorldByName(string name, out int index, out WorldEntry world)
    {
        lock (_worldLock)
        {
            for (int i = 0; i < _worldCount; i++)
            {
                if (string.Equals(_worlds[i].Name, name, StringComparison.Ordinal))
                {
                    index = i;
                    world = _worlds[i];
                    return true;
                }
            }
        }

        index = -1;
        world = new WorldEntry();
        return false;
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
        string key = ClientKey(loginConnId, clid);
        var session = _clients.GetOrAdd(key, _ => new ClientSession(loginConnId, clid));
        session.LoginServerIndex = loginConnId;
        return session;
    }

    public void RemoveClient(ulong loginConnId, _CLID clid)
    {
        _clients.TryRemove(ClientKey(loginConnId, clid), out _);
    }

    public bool TryGetActiveAccount(uint accountSerial, out ClientSession session)
    {
        return _activeAccounts.TryGetValue(accountSerial, out session!);
    }

    public bool TryGetActiveGlobal(uint globalIndex, out ClientSession session)
    {
        return _activeGlobals.TryGetValue(globalIndex, out session!);
    }

    public void RegisterActiveAccount(ClientSession session)
    {
        if (session.AccountSerial == 0) return;
        _activeAccounts[session.AccountSerial] = session;
        if (session.GlobalId.dwIndex != 0)
        {
            _activeGlobals[session.GlobalId.dwIndex] = session;
        }
    }

    public void RemoveActiveAccount(uint accountSerial)
    {
        if (accountSerial == 0) return;
        if (_activeAccounts.TryRemove(accountSerial, out var session))
        {
            if (session.GlobalId.dwIndex != 0)
            {
                _activeGlobals.TryRemove(session.GlobalId.dwIndex, out _);
            }
        }
    }

    public ClientSession[] GetActiveAccountsSnapshot()
    {
        return _activeAccounts.Values.ToArray();
    }

    public WorldServerSession RegisterWorld(int worldCode, PublicConnection connection)
    {
        if (_worldConnections.TryGetValue(connection.ConnectionId, out var existing))
        {
            existing.UpdateConnection(connection);
            existing.UpdateWorldCode(worldCode);
            _worldServers[worldCode] = existing;
            return existing;
        }

        var session = new WorldServerSession(connection.ConnectionId, worldCode, connection);
        _worldServers[worldCode] = session;
        _worldConnections[connection.ConnectionId] = session;
        return session;
    }

    public WorldServerSession RegisterWorldConnection(PublicConnection connection)
    {
        if (_worldConnections.TryGetValue(connection.ConnectionId, out var existing))
        {
            existing.UpdateConnection(connection);
            return existing;
        }

        var session = new WorldServerSession(connection.ConnectionId, -1, connection);
        _worldConnections[connection.ConnectionId] = session;
        return session;
    }

    public bool AssignWorldCode(ulong connectionId, int worldCode)
    {
        if (!_worldConnections.TryGetValue(connectionId, out var session))
        {
            session = new WorldServerSession(connectionId, worldCode, null);
            _worldConnections[connectionId] = session;
        }

        session.UpdateWorldCode(worldCode);
        _worldServers[worldCode] = session;
        return true;
    }

    public bool TryGetWorldSessionByConnection(ulong connectionId, out WorldServerSession session)
    {
        return _worldConnections.TryGetValue(connectionId, out session!);
    }

    public bool TryGetWorldSession(int worldCode, out WorldServerSession session)
    {
        return _worldServers.TryGetValue(worldCode, out session!);
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

    private static WorldEntry CloneWorldEntry(WorldEntry entry)
    {
        return new WorldEntry
        {
            Name = entry.Name,
            Address = entry.Address,
            DbName = entry.DbName,
            Type = entry.Type,
            IsService = entry.IsService,
            GateIp = entry.GateIp,
            GatePort = entry.GatePort
        };
    }

    private static void ParseGateAddress(ref WorldEntry entry)
    {
        if (string.IsNullOrWhiteSpace(entry.Address))
        {
            entry.GateIp = 0;
            entry.GatePort = 0;
            return;
        }

        string host = entry.Address.Trim();
        ushort port = 0;
        int colon = host.LastIndexOf(':');
        if (colon > 0 && host.IndexOf(':') == colon)
        {
            if (ushort.TryParse(host.Substring(colon + 1), out var parsed))
            {
                port = parsed;
                host = host.Substring(0, colon);
            }
        }

        if (!IPAddress.TryParse(host, out var ip))
        {
            entry.GateIp = 0;
            entry.GatePort = port;
            return;
        }

        var bytes = ip.GetAddressBytes();
        if (bytes.Length != 4)
        {
            entry.GateIp = 0;
            entry.GatePort = port;
            return;
        }

        entry.GateIp = (uint)(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
        entry.GatePort = port;
    }

    private static string ClientKey(ulong loginConnId, _CLID clid)
    {
        return $"{loginConnId}:{clid.wIndex}:{clid.dwSerial}";
    }
}

public sealed class ClientSession
{
    public ulong ConnectionId { get; }
    public ulong LoginServerIndex { get; set; }
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
    public bool IsLogin { get; set; }
    public byte UiLock { get; set; }
    public byte UiLockFailCnt { get; set; }
    public byte UiLockHintIndex { get; set; }
    public byte UiLockFindPassFailCount { get; set; }
    public byte[] UiLockPw { get; } = new byte[13];
    public byte[] UiLockHintAnswer { get; } = new byte[17];
    public byte[] AccountPassword { get; } = new byte[13];
    public _GLBID GlobalId { get; set; }
    public int WorldCode { get; set; } = -1;
    public uint[] MasterKey { get; } = new uint[4];
    public byte[] AvatarName { get; } = new byte[17];
    public uint AvatarSerial { get; set; } = uint.MaxValue;
    public byte Level { get; set; }
    public bool IsSelectChar { get; set; }

    public ClientSession(ulong loginConnId, _CLID clid)
    {
        ConnectionId = loginConnId;
        LoginServerIndex = loginConnId;
        Clid = clid;
        GlobalId = new _GLBID { dwIndex = 0, dwSerial = uint.MaxValue };
    }

    public void UpdateFromRequest(_login_account_request_loac_blit req)
    {
        AccountId = PacketStringUtil.ToAsciiNullTerm(req.GetAccountId());
        Password = PacketStringUtil.ToAsciiNullTerm(req.GetPassword());
        PacketStringUtil.FillFixed(AccountPassword, Password);
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

    public LoginServerSession(ulong connectionId, PublicConnection? connection)
    {
        ConnectionId = connectionId;
        Connection = connection;
    }

    public void UpdateConnection(PublicConnection connection)
    {
        Connection = connection;
    }
}

public sealed class WorldServerSession
{
    public ulong ConnectionId { get; }
    public int WorldCode { get; private set; }
    public PublicConnection? Connection { get; private set; }
    public uint LastPingTime { get; private set; }

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

    public void UpdatePing(uint tick)
    {
        LastPingTime = tick;
    }
}
