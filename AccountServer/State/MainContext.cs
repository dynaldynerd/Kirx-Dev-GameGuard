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

    private AccountMainContext() { }

    public LoginServerSession Register(PublicConnection connection)
    {
        var session = new LoginServerSession(connection.ConnectionId);
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
    }

    public LoginServerSession? GetSession(ulong connectionId)
    {
        _loginServers.TryGetValue(connectionId, out var session);
        return session;
    }

    public ClientSession RegisterClient(ulong loginConnId, _CLID clid)
    {
        var loginSession = _loginServers.GetOrAdd(loginConnId, id => new LoginServerSession(id));
        return loginSession.RegisterClient(clid);
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

    public ClientSession(ulong loginConnId, _CLID clid)
    {
        ConnectionId = loginConnId;
        Clid = clid;
    }

    public unsafe void UpdateFromRequest(_login_account_request_loac_blit req)
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
        for (int i = 0; i < Cms.Length; i++)
        {
            Cms[i] = req.szCMS[i];
        }
    }
}

public sealed class LoginServerSession
{
    public ulong ConnectionId { get; }
    private readonly ConcurrentDictionary<string, ClientSession> _clients = new();

    public LoginServerSession(ulong connectionId)
    {
        ConnectionId = connectionId;
    }

    private static string KeyFor(_CLID clid) => $"{clid.wIndex}:{clid.dwSerial}";

    public ClientSession RegisterClient(_CLID clid)
    {
        return _clients.GetOrAdd(KeyFor(clid), _ => new ClientSession(ConnectionId, clid));
    }

    public ClientSession[] GetClientsSnapshot()
    {
        return _clients.Values.ToArray();
    }

    public void RemoveAllClients() => _clients.Clear();
}
