using System.Security.Cryptography;
using System.Text;
using AccountServer.Security;
using AccountServer.Settings;
using Microsoft.EntityFrameworkCore;
using Pomelo.EntityFrameworkCore.MySql.Infrastructure;

namespace AccountServer.Data;

public sealed class AccountAdminService
{
    private readonly DbContextOptions<AccountDbContext> _options;
    private readonly byte[] _hmacKey;
    private readonly byte[] _aesKey;
    private readonly byte[] _argonSalt;

    public AccountAdminService(AppSettings settings)
    {
        if (!SecuritySettings.TryDecodeArgon2Salt(settings.Security.Argon2SaltBase64, out _argonSalt) || _argonSalt.Length == 0)
        {
            throw new InvalidOperationException("AccountServer is missing a valid Argon2 salt.");
        }

        _hmacKey = _argonSalt;
        _aesKey = SHA256.HashData(_hmacKey);

        string connectionString = settings.Database.BuildUserConnectionString(settings.Database.Provider, AppContext.BaseDirectory);
        var builder = new DbContextOptionsBuilder<AccountDbContext>();
        switch (settings.Database.Provider)
        {
            case DatabaseProvider.Sqlite:
                builder.UseSqlite(connectionString);
                break;
            case DatabaseProvider.MariaDb:
                builder.UseMySql(connectionString, new MariaDbServerVersion(new Version(10, 4, 0)));
                break;
            default:
                builder.UseSqlServer(connectionString);
                break;
        }

        _options = builder.Options;
    }

    public async Task<AccountAdminResult> CreateNormalAccountAsync(string username, string password, CancellationToken token)
    {
        username = username.Trim();
        password = password.Trim();

        string? validationError = ValidateNormalAccount(username, password);
        if (validationError != null)
        {
            return AccountAdminResult.Fail(validationError);
        }

        byte[] idHmac = ComputeIdHmac(username);

        await using var db = new AccountDbContext(_options);
        bool authExists = await db.AccountAuths.AsNoTracking()
            .AnyAsync(x => x.IdHmac == idHmac, token)
            .ConfigureAwait(false);
        if (authExists)
        {
            return AccountAdminResult.Fail("That normal account already exists.");
        }

        string passwordHash = CryptoHelper.HashArgon2idBase64(password, _argonSalt);
        byte[] idEnc = EncryptId(username);

        db.AccountAuths.Add(new AccountAuth
        {
            IdHmac = idHmac,
            IdEnc = idEnc,
            PasswordHash = passwordHash,
            AccountType = 0,
            BirthDate = null
        });

        await db.SaveChangesAsync(token).ConfigureAwait(false);

        return AccountAdminResult.Success($"Normal account '{username}' created.");
    }

    public async Task<AccountAdminResult> CreateGmAccountAsync(
        string usernameWithoutPrefix,
        string password,
        byte grade,
        byte subGrade,
        DateTime expireDate,
        CancellationToken token)
    {
        usernameWithoutPrefix = usernameWithoutPrefix.Trim();
        password = password.Trim();

        string? validationError = ValidateGmAccount(usernameWithoutPrefix, password, grade, expireDate);
        if (validationError != null)
        {
            return AccountAdminResult.Fail(validationError);
        }

        string finalUsername = "!" + usernameWithoutPrefix;
        byte[] idHmac = ComputeIdHmac(finalUsername);

        await using var db = new AccountDbContext(_options);
        bool exists = await db.StaffAccounts.AsNoTracking()
            .AnyAsync(x => x.IdHmac == idHmac, token)
            .ConfigureAwait(false);
        bool authExists = await db.AccountAuths.AsNoTracking()
            .AnyAsync(x => x.IdHmac == idHmac, token)
            .ConfigureAwait(false);
        if (exists || authExists)
        {
            return AccountAdminResult.Fail("That GM account already exists.");
        }

        string passwordHash = CryptoHelper.HashArgon2idBase64(password, _argonSalt);

        db.StaffAccounts.Add(new StaffAccount
        {
            IdHmac = idHmac,
            IdEnc = EncryptId(finalUsername),
            PwHash = passwordHash,
            Grade = grade,
            Depart = string.Empty,
            RealName = string.Empty,
            SubGrade = subGrade,
            TotalLogMin = 0,
            LastLoginDt = DateTime.Now,
            LastLogoffDt = DateTime.Now,
            LastConnIp = "0",
            ExpireDt = expireDate
        });

        await db.SaveChangesAsync(token).ConfigureAwait(false);
        return AccountAdminResult.Success($"GM account '{finalUsername}' created.");
    }

    private byte[] ComputeIdHmac(string username)
    {
        return CryptoHelper.ComputeHmacSha256(Encoding.UTF8.GetBytes(username), _hmacKey);
    }

    private byte[] EncryptId(string username)
    {
        return CryptoHelper.EncryptAesGcm(_aesKey, Encoding.UTF8.GetBytes(username));
    }

    private static string? ValidateNormalAccount(string username, string password)
    {
        if (string.IsNullOrWhiteSpace(username))
        {
            return "Username is required.";
        }

        if (username.StartsWith('!'))
        {
            return "Normal account usernames must not start with '!'.";
        }

        if (username.Length > 12)
        {
            return "Username must be 12 characters or fewer.";
        }

        if (!IsAscii(username))
        {
            return "Username must use ASCII characters only.";
        }

        if (string.IsNullOrWhiteSpace(password))
        {
            return "Password is required.";
        }

        if (password.Length > 12)
        {
            return "Password must be 12 characters or fewer.";
        }

        if (!IsAscii(password))
        {
            return "Password must use ASCII characters only.";
        }

        return null;
    }

    private static string? ValidateGmAccount(string usernameWithoutPrefix, string password, byte grade, DateTime expireDate)
    {
        if (string.IsNullOrWhiteSpace(usernameWithoutPrefix))
        {
            return "GM username is required.";
        }

        if (usernameWithoutPrefix.StartsWith('!'))
        {
            return "Enter the GM username without the '!' prefix.";
        }

        if (usernameWithoutPrefix.Length > 11)
        {
            return "GM username must be 11 characters or fewer because '!' is added automatically.";
        }

        if (!IsAscii(usernameWithoutPrefix))
        {
            return "GM username must use ASCII characters only.";
        }

        if (string.IsNullOrWhiteSpace(password))
        {
            return "Password is required.";
        }

        if (password.Length > 12)
        {
            return "Password must be 12 characters or fewer.";
        }

        if (!IsAscii(password))
        {
            return "Password must use ASCII characters only.";
        }

        if (grade == 0)
        {
            return "GM grade must be greater than 0.";
        }

        if (expireDate <= DateTime.Now)
        {
            return "Expire date must be in the future.";
        }

        return null;
    }

    private static bool IsAscii(string value)
    {
        foreach (char ch in value)
        {
            if (ch is < '!' or > '~')
            {
                return false;
            }
        }

        return true;
    }
}

public readonly record struct AccountAdminResult(bool IsSuccess, string Message)
{
    public static AccountAdminResult Success(string message) => new(true, message);
    public static AccountAdminResult Fail(string message) => new(false, message);
}
