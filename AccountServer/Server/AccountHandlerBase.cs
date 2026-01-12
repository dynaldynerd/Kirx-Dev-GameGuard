using AccountServer.Data;
using AccountServer.Settings;
using AccountServer.State;
using RFNetworking;
using System.Net;
using System.Security.Cryptography;
using LoginServer.Packets;

namespace AccountServer.Server;

public abstract class AccountHandlerBase : NetworkHandlerBase
{
    protected readonly Action<string> _log;
    protected readonly AccountMainContext _context = AccountMainContext.Instance;
    protected readonly AppSettings _settings;
    protected readonly IAccountDatabase _db;

    protected AccountHandlerBase(
        Action<string> log,
        AppSettings settings,
        string? connectionString = null,
        IAccountDatabase? db = null)
    {
        _log = log;
        _settings = settings;
        _db = db ?? CreateDefaultDb(connectionString);
    }

    protected static IAccountDatabase CreateDefaultDb(string? connectionString)
    {
        var connString = connectionString;
        if (string.IsNullOrWhiteSpace(connString))
        {
            connString = Environment.GetEnvironmentVariable("ACCOUNT_DB_CONNECTION") ??
                         "Server=(local);Database=RF_User;Integrated Security=True;TrustServerCertificate=True";
        }
        return new AccountDatabase(connString);
    }

    protected static void GenerateMasterKey(ClientSession session)
    {
        for (int i = 0; i < session.MasterKey.Length; i++)
        {
            uint dwR = (uint)Random.Shared.Next(0, 0x8000);
            uint dwRR = dwR | ((Random.Shared.Next() % 2 != 0) ? 0u : 0x8000u);
            uint dwL = (uint)Random.Shared.Next(0, 0x8000);
            session.MasterKey[i] = dwL | ((Random.Shared.Next() % 2 != 0) ? 0u : 0x8000u) | (dwRR << 16);
        }
    }

    protected static uint[] CalcCodeKey(uint[] masterKey)
    {
        const uint multiplier = 313210060;
        uint v1 = (masterKey[0] + masterKey[2] - masterKey[1] - masterKey[3]) * multiplier;
        return new[]
        {
            masterKey[0] ^ v1,
            v1 ^ masterKey[1],
            v1 ^ masterKey[2],
            v1 ^ masterKey[3]
        };
    }

    protected async Task SendToLoginServersAsync(PacketEnvelope env, CancellationToken token)
    {
        foreach (var session in _context.GetLoginSessionsSnapshot())
        {
            if (session.Connection == null) continue;
            try
            {
                await session.Connection.SendAsync(env, token).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _log($"[{session.ConnectionId}] login broadcast failed: {ex.Message}");
            }
        }
    }

    protected static string ExtractHost(string address)
    {
        if (string.IsNullOrWhiteSpace(address))
        {
            return string.Empty;
        }

        string host = address.Trim();
        int colon = host.LastIndexOf(':');
        if (colon > 0 && host.IndexOf(':') == colon)
        {
            host = host.Substring(0, colon);
        }
        return host;
    }

    protected static byte[] ComputeHash(byte[] seed)
    {
        return SHA256.HashData(seed);
    }

    protected void PcCloseAccount(ulong sourceConnectionId, _GLBID gidGlobal, byte posType)
    {
        string position = posType == 1 ? "World" : "LogIn";

        if (!_context.TryGetActiveGlobal(gidGlobal.dwIndex, out var account) || !account.IsLogin)
        {
            _log($"CMainThread::pc_CloseAccount() : Not Login ({position}) serial={account?.AccountSerial ?? 0} gidIndex={gidGlobal.dwIndex}");
            return;
        }
        if (account.GlobalId.dwSerial != gidGlobal.dwSerial)
        {
            _log($"CMainThread::pc_CloseAccount() : G_id Different ({position}) serial={account.AccountSerial} cur={account.GlobalId.dwSerial} req={gidGlobal.dwSerial}");
            return;
        }

        ReleaseAccount(account, "pc_CloseAccount");
    }

    protected void ReleaseAccount(ClientSession account, string reason)
    {
        if (!account.IsLogin)
        {
            return;
        }

        if (account.UserCode == 0 && account.AccountSerial != 0)
        {
            string ip = new IPAddress(account.ClientIp).ToString();
            _ = _db.Insert_UserPushLogAsync((int)account.AccountSerial, ip, ip, CancellationToken.None);
        }

        account.IsLogin = false;
        account.WorldCode = -1;
        account.GlobalId = default;

        _context.RemoveClient(account.LoginServerIndex, account.Clid);
        if (account.AccountSerial != 0)
        {
            _context.RemoveActiveAccount(account.AccountSerial);
        }

        _log($"ReleaseAccount: {reason} account={account.AccountId} serial={account.AccountSerial}");
    }

    protected async Task SendSelectWorldResultAsync(PublicConnection connection, _select_world_result_aclo payload, CancellationToken token)
    {
        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 6,
            Payload = payload.ToArray()
        };

        try
        {
            await connection.SendAsync(env, token).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            _log($"[{connection.ConnectionId}] send select world result failed: {ex.Message}");
        }
    }
}
