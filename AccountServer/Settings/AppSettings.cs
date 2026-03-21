using System;
using System.Collections.Generic;
using System.IO;
using System.Security.Cryptography;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace AccountServer.Settings;

public sealed class AppSettings
{
    public DatabaseSettings Database { get; set; } = new();
    public GmFilterSettings GmFilter { get; set; } = new();
    public WorldListSettings WorldList { get; set; } = new();
    public SecuritySettings Security { get; set; } = new();
    public ListenerSettings Listener { get; set; } = new();
    public int MaxActiveClients { get; set; } = -1;

    [JsonIgnore]
    public static string DefaultPath =>
        Path.Combine(AppContext.BaseDirectory, "Settings", "appsettings.account.json");

    public static AppSettings Load(string? path = null)
    {
        path ??= DefaultPath;
        if (!File.Exists(path))
        {
            var defaults = CreateDefault();
            defaults.Save(path);
            return defaults;
        }

        var json = File.ReadAllText(path);
        var settings = JsonSerializer.Deserialize<AppSettings>(json, JsonOptions()) ?? new AppSettings();
        settings.Security ??= new SecuritySettings();
        settings.Listener ??= new ListenerSettings();
        settings.Database ??= new DatabaseSettings();
        if (!Enum.IsDefined(typeof(DatabaseProvider), settings.Database.Provider))
        {
            settings.Database.Provider = DatabaseProvider.SqlServer;
        }
        if (settings.Database.Provider == DatabaseProvider.SqlServer && settings.Database.User.TrustedConnection)
        {
            settings.Database.User.Host = DbProfile.TrustedSqlServerHost;
        }
        settings.WorldList ??= new WorldListSettings();
        foreach (var world in settings.WorldList.Worlds)
        {
            world.Address = WorldEntry.NormalizeDatabaseServerAddress(world.Address);
        }
        return settings;
    }

    public void Save(string? path = null)
    {
        path ??= DefaultPath;
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
        {
            Directory.CreateDirectory(dir);
        }
        if (Database.Provider == DatabaseProvider.SqlServer && Database.User.TrustedConnection)
        {
            Database.User.Host = DbProfile.TrustedSqlServerHost;
        }
        WorldList ??= new WorldListSettings();
        foreach (var world in WorldList.Worlds)
        {
            world.Address = WorldEntry.NormalizeDatabaseServerAddress(world.Address);
        }
        var json = JsonSerializer.Serialize(this, JsonOptions());
        File.WriteAllText(path, json);
    }

    private static AppSettings CreateDefault()
    {
        return new AppSettings
        {
            Database = new DatabaseSettings
            {
                Provider = DatabaseProvider.SqlServer,
                User = new DbProfile
                {
                    Host = "(local)",
                    Port = 1433,
                    Database = "RF_User",
                    User = string.Empty,
                    Password = string.Empty,
                    TrustedConnection = true
                }
            },
            GmFilter = new GmFilterSettings(),
            WorldList = new WorldListSettings
            {
                Worlds = new List<WorldEntry>
                {
                    new()
                    {
                        Name = "RF_LEET",
                        Address = DbProfile.TrustedSqlServerHost,
                        DbName = "RF_World",
                        Type = 0
                    }
                }
            },
            MaxActiveClients = -1,
            Security = new SecuritySettings(),
            Listener = new ListenerSettings
            {
                Host = "0.0.0.0",
                LoginPort = 27000,
                WorldPort = 29000,
                ControlPort = 28000
            }
        };
    }

    private static JsonSerializerOptions JsonOptions() => new()
    {
        WriteIndented = true,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };
}

public sealed class DatabaseSettings
{
    public DatabaseProvider Provider { get; set; } = DatabaseProvider.SqlServer;
    public DbProfile User { get; set; } = new();

    public string BuildUserConnectionString() => BuildConnectionString(User);

    private static string BuildConnectionString(DbProfile profile)
    {
        string host = profile.GetEffectiveSqlServerHost();
        if (profile.TrustedConnection)
        {
            return $"Server={host},{profile.Port};Database={profile.Database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }
        return $"Server={host},{profile.Port};Database={profile.Database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
    }

    public string BuildUserConnectionString(DatabaseProvider provider, string basePath) =>
        BuildConnectionString(User, provider, basePath);

    public static string GetSqlitePath(string basePath, string databaseName)
    {
        return Path.Combine(basePath, "Settings", "db", $"{databaseName}.db");
    }

    private static string BuildConnectionString(DbProfile profile, DatabaseProvider provider, string basePath)
    {
        switch (provider)
        {
            case DatabaseProvider.Sqlite:
                return $"Data Source={GetSqlitePath(basePath, profile.Database)};";
            case DatabaseProvider.MariaDb:
                return $"Server={profile.Host};Port={profile.Port};Database={profile.Database};User ID={profile.User};Password={profile.Password};";
            default:
                return BuildConnectionString(profile);
        }
    }
}

public enum DatabaseProvider
{
    SqlServer = 0,
    MariaDb = 1,
    Sqlite = 2
}

public sealed class DbProfile
{
    public const string TrustedSqlServerHost = "(local)";

    public string Host { get; set; } = TrustedSqlServerHost;
    public int Port { get; set; } = 1433;
    public string Database { get; set; } = "";
    public string User { get; set; } = "";
    public string Password { get; set; } = "";
    public bool TrustedConnection { get; set; } = true;

    public string GetEffectiveSqlServerHost()
    {
        return TrustedConnection ? TrustedSqlServerHost : Host;
    }
}

public sealed class GmFilterSettings
{
    public List<string> Prefixes { get; set; } = new();
}

public sealed class WorldListSettings
{
    public List<WorldEntry> Worlds { get; set; } = new();
}

public sealed class WorldEntry
{
    public static string NormalizeDatabaseServerAddress(string? address)
    {
        if (string.IsNullOrWhiteSpace(address))
        {
            return DbProfile.TrustedSqlServerHost;
        }

        string trimmed = address.Trim();
        return trimmed.Equals("127.0.0.1", StringComparison.OrdinalIgnoreCase) ||
               trimmed.Equals("localhost", StringComparison.OrdinalIgnoreCase)
            ? DbProfile.TrustedSqlServerHost
            : trimmed;
    }

    public string Name { get; set; } = "";
    public string Address { get; set; } = DbProfile.TrustedSqlServerHost;
    public string DbName { get; set; } = "";
    public int Type { get; set; }
    [JsonIgnore]
    public bool IsService { get; set; }
    [JsonIgnore]
    public uint GateIp { get; set; }
    [JsonIgnore]
    public ushort GatePort { get; set; }
}

public sealed class SecuritySettings
{
    public string Argon2SaltBase64 { get; set; } = "";

    public bool HasValidArgon2Salt()
    {
        return TryDecodeArgon2Salt(Argon2SaltBase64, out var bytes) && bytes.Length >= 16;
    }

    public static string CreateArgon2SaltBase64()
    {
        return Convert.ToBase64String(RandomNumberGenerator.GetBytes(16));
    }

    public static bool TryDecodeArgon2Salt(string? value, out byte[] bytes)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            bytes = Array.Empty<byte>();
            return false;
        }

        try
        {
            bytes = Convert.FromBase64String(value);
            return true;
        }
        catch (FormatException)
        {
            bytes = Array.Empty<byte>();
            return false;
        }
    }
}

public sealed class ListenerSettings
{
    public string Host { get; set; } = "0.0.0.0";
    public int LoginPort { get; set; } = 27000;
    public int WorldPort { get; set; } = 29000;
    public int ControlPort { get; set; } = 28000;
}
