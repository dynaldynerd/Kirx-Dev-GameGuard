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
    private readonly Func<bool> _isVerboseLoggingEnabled;
    protected readonly AccountMainContext _context = AccountMainContext.Instance;
    protected readonly AppSettings _settings;
    protected readonly IAccountDatabase _db;

    protected AccountHandlerBase(
        Action<string> log,
        Func<bool> isVerboseLoggingEnabled,
        AppSettings settings,
        string? connectionString = null,
        IAccountDatabase? db = null)
    {
        _log = log;
        _isVerboseLoggingEnabled = isVerboseLoggingEnabled;
        _settings = settings;
        _db = db ?? CreateDefaultDb(settings, connectionString);
    }

    protected void LogVerbose(string message)
    {
        if (_isVerboseLoggingEnabled())
        {
            _log(message);
        }
    }

    protected static IAccountDatabase CreateDefaultDb(AppSettings settings, string? connectionString)
    {
        string connString = connectionString ?? string.Empty;
        if (string.IsNullOrWhiteSpace(connString))
        {
            connString = settings.Database.BuildUserConnectionString(settings.Database.Provider, AppContext.BaseDirectory);
        }

        return new AccountDatabaseEf(settings.Database.Provider, connString, settings.Security);
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

        uint accountSerial = account.AccountSerial;
        byte userCode = account.UserCode;
        uint clientIp = account.ClientIp;
        string accountId = account.AccountId;

        if (userCode == 0 && accountSerial != 0)
        {
            string ip = new IPAddress(clientIp).ToString();
            _ = _db.Insert_UserPushLogAsync((int)accountSerial, ip, ip, CancellationToken.None);
        }

        if (accountSerial != 0)
        {
            _ = UpdateAccountLogoffAsync(accountSerial, userCode);
        }

        account.IsLogin = false;
        account.WorldCode = -1;
        account.GlobalId = default;

        _context.RemoveClient(account.LoginServerIndex, account.Clid);
        if (accountSerial != 0)
        {
            _context.RemoveActiveAccount(accountSerial);
        }

        LogVerbose($"ReleaseAccount: {reason} account={accountId} serial={accountSerial}");
    }

    private async Task UpdateAccountLogoffAsync(uint accountSerial, byte userCode)
    {
        try
        {
            bool updated = userCode == 1
                ? await _db.Update_StaffLogoffDateAsync(accountSerial, CancellationToken.None).ConfigureAwait(false)
                : await _db.Update_UserLogoffDateAsync(accountSerial, CancellationToken.None).ConfigureAwait(false);
            if (!updated)
            {
                _log($"Update logoff date failed for serial={accountSerial} userCode={userCode}");
            }
        }
        catch (Exception ex)
        {
            _log($"Update logoff date failed for serial={accountSerial}: {ex.Message}");
        }
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
