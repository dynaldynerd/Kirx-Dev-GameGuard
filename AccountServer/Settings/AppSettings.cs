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
    public WorldListSettings WorldList { get; set; } = new();
    public SecuritySettings Security { get; set; } = new();
    public ListenerSettings Listener { get; set; } = new();
    public bool Autostart { get; set; }
    public bool VerboseLogging { get; set; }
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
                    Port = DbProfile.DefaultSqlServerPort,
                    Database = "RF_User",
                    User = string.Empty,
                    Password = string.Empty,
                    TrustedConnection = true
                }
            },
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
            Autostart = false,
            VerboseLogging = false,
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
        string host = DbProfile.BuildSqlServerDataSource(profile.Host, profile.Port, profile.TrustedConnection);
        if (profile.TrustedConnection)
        {
            return $"Server={host};Database={profile.Database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }
        return $"Server={host};Database={profile.Database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
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
                DbProfile.ResolveMariaDbEndpoint(profile.Host, profile.Port, out string host, out int port);
                return $"Server={host};Port={port};Database={profile.Database};User ID={profile.User};Password={profile.Password};";
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
    public const int DefaultSqlServerPort = 1433;
    public const int DefaultMariaDbPort = 3306;

    public string Host { get; set; } = TrustedSqlServerHost;
    public int Port { get; set; } = DefaultSqlServerPort;
    public string Database { get; set; } = "";
    public string User { get; set; } = "";
    public string Password { get; set; } = "";
    public bool TrustedConnection { get; set; } = true;

    public string GetEffectiveSqlServerHost()
    {
        return TrustedConnection ? TrustedSqlServerHost : Host;
    }

    public static string NormalizeHostForStorage(DatabaseProvider provider, string host, bool trustedConnection)
    {
        if (provider == DatabaseProvider.SqlServer)
        {
            return trustedConnection
                ? TrustedSqlServerHost
                : (string.IsNullOrWhiteSpace(host) ? string.Empty : host.Trim());
        }

        return string.IsNullOrWhiteSpace(host) ? string.Empty : host.Trim();
    }

    public static string GetDisplayHost(DatabaseProvider provider, string host, int port, bool trustedConnection)
    {
        if (provider == DatabaseProvider.Sqlite)
        {
            return string.Empty;
        }

        string normalizedHost = NormalizeHostForStorage(provider, host, trustedConnection);
        if (TrySplitExplicitPort(provider, normalizedHost, out _, out _))
        {
            return normalizedHost;
        }

        int effectivePort = GetEffectivePort(provider, normalizedHost, port);
        return provider == DatabaseProvider.SqlServer
            ? (effectivePort == DefaultSqlServerPort ? normalizedHost : $"{normalizedHost},{effectivePort}")
            : (effectivePort == DefaultMariaDbPort ? normalizedHost : $"{normalizedHost}:{effectivePort}");
    }

    public static int GetEffectivePort(DatabaseProvider provider, string host, int port)
    {
        if (provider == DatabaseProvider.Sqlite)
        {
            return 0;
        }

        if (TrySplitExplicitPort(provider, host, out _, out int explicitPort))
        {
            return explicitPort;
        }

        return NormalizePort(provider, port);
    }

    public static string GetEndpointHost(DatabaseProvider provider, string host, bool trustedConnection)
    {
        string normalizedHost = NormalizeHostForStorage(provider, host, trustedConnection);
        if (TrySplitExplicitPort(provider, normalizedHost, out string server, out _))
        {
            return server;
        }

        if (!string.IsNullOrWhiteSpace(normalizedHost))
        {
            return normalizedHost;
        }

        return provider == DatabaseProvider.MariaDb ? "127.0.0.1" : TrustedSqlServerHost;
    }

    public static string BuildSqlServerDataSource(string host, int port, bool trustedConnection)
    {
        string normalizedHost = NormalizeHostForStorage(DatabaseProvider.SqlServer, host, trustedConnection);
        if (string.IsNullOrWhiteSpace(normalizedHost))
        {
            normalizedHost = TrustedSqlServerHost;
        }

        if (TrySplitExplicitPort(DatabaseProvider.SqlServer, normalizedHost, out string server, out int explicitPort))
        {
            return $"{server},{explicitPort}";
        }

        int effectivePort = NormalizePort(DatabaseProvider.SqlServer, port);
        return effectivePort == DefaultSqlServerPort ? normalizedHost : $"{normalizedHost},{effectivePort}";
    }

    public static void ResolveMariaDbEndpoint(string host, int port, out string server, out int effectivePort)
    {
        string normalizedHost = string.IsNullOrWhiteSpace(host) ? "127.0.0.1" : host.Trim();
        if (TrySplitExplicitPort(DatabaseProvider.MariaDb, normalizedHost, out string parsedServer, out int parsedPort))
        {
            server = parsedServer;
            effectivePort = parsedPort;
            return;
        }

        server = normalizedHost;
        effectivePort = NormalizePort(DatabaseProvider.MariaDb, port);
    }

    private static int NormalizePort(DatabaseProvider provider, int port)
    {
        if (port >= 1 && port <= 65535)
        {
            return port;
        }

        return provider == DatabaseProvider.MariaDb ? DefaultMariaDbPort : DefaultSqlServerPort;
    }

    private static bool TrySplitExplicitPort(DatabaseProvider provider, string host, out string server, out int port)
    {
        server = string.IsNullOrWhiteSpace(host) ? string.Empty : host.Trim();
        port = 0;
        if (string.IsNullOrWhiteSpace(server))
        {
            return false;
        }

        return provider switch
        {
            DatabaseProvider.SqlServer =>
                TryExtractPort(server, ',', out server, out port) ||
                TryExtractPort(server, ':', out server, out port),
            DatabaseProvider.MariaDb =>
                TryExtractPort(server, ':', out server, out port) ||
                TryExtractPort(server, ',', out server, out port),
            _ => false
        };
    }

    private static bool TryExtractPort(string value, char separator, out string server, out int port)
    {
        server = value.Trim();
        port = 0;

        int separatorIndex = value.LastIndexOf(separator);
        if (separatorIndex <= 0 || separatorIndex >= value.Length - 1)
        {
            return false;
        }

        string suffix = value[(separatorIndex + 1)..].Trim();
        if (!int.TryParse(suffix, out int parsedPort) || parsedPort < 1 || parsedPort > 65535)
        {
            return false;
        }

        string host = value[..separatorIndex].Trim();
        if (host.Length == 0)
        {
            return false;
        }

        server = host;
        port = parsedPort;
        return true;
    }
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
