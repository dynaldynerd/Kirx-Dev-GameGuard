using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Serialization;
using LoginServer.Data;

namespace LoginServer.Settings;

public sealed class AppSettings
{
    public DatabaseSettings Database { get; set; } = new();
    public NetworkSettings Network { get; set; } = new();
    public SecuritySettings Security { get; set; } = new();
    public bool Autostart { get; set; }
    public bool AutoOpenExternalConnection { get; set; }

    [JsonIgnore]
    public static string DefaultPath => Path.Combine(AppContext.BaseDirectory, "appsettings.login.json");

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
        settings.Database ??= new DatabaseSettings();
        settings.Security ??= new SecuritySettings();
        if (!Enum.IsDefined(typeof(LoginDatabaseProvider), settings.Database.Provider))
        {
            settings.Database.Provider = LoginDatabaseProvider.SqlServer;
        }
        if (!Enum.IsDefined(typeof(LoginDatabaseProvider), settings.Database.BillingProvider))
        {
            settings.Database.BillingProvider = LoginDatabaseProvider.SqlServer;
        }
        settings.Database.Host = DatabaseSettings.NormalizeSqlServerHost(
            settings.Database.Host,
            settings.Database.Provider == LoginDatabaseProvider.SqlServer && settings.Database.TrustedConnection);
        settings.Database.BillingHost = DatabaseSettings.NormalizeSqlServerHost(
            settings.Database.BillingHost,
            settings.Database.BillingProvider == LoginDatabaseProvider.SqlServer && settings.Database.BillingTrustedConnection);
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
        Database.Host = DatabaseSettings.NormalizeSqlServerHost(
            Database.Host,
            Database.Provider == LoginDatabaseProvider.SqlServer && Database.TrustedConnection);
        Database.BillingHost = DatabaseSettings.NormalizeSqlServerHost(
            Database.BillingHost,
            Database.BillingProvider == LoginDatabaseProvider.SqlServer && Database.BillingTrustedConnection);
        var json = JsonSerializer.Serialize(this, JsonOptions());
        File.WriteAllText(path, json);
    }

    private static AppSettings CreateDefault()
    {
        return new AppSettings
        {
            Database = new DatabaseSettings
            {
                Provider = LoginDatabaseProvider.SqlServer,
                Host = "(local)",
                Port = 1433,
                User = string.Empty,
                Password = string.Empty,
                TrustedConnection = true,
                BillingProvider = LoginDatabaseProvider.SqlServer,
                BillingHost = "(local)",
                BillingPort = 1433,
                BillingDatabase = "Billing",
                BillingUser = string.Empty,
                BillingPassword = string.Empty,
                BillingTrustedConnection = true
            },
            Network = new NetworkSettings
            {
                ClientPort = 10001,
                AccountHost = "127.0.0.1",
                AccountPort = 27000,
                MaxConnections = 5000,
                UserLoadThresholds = new[] { 500, 1000, 1500 }
            },
            Security = new SecuritySettings(),
            Autostart = false,
            AutoOpenExternalConnection = false
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
    public const string TrustedSqlServerHost = "(local)";
    public const int DefaultSqlServerPort = 1433;
    public const int DefaultMariaDbPort = 3306;

    public LoginDatabaseProvider Provider { get; set; } = LoginDatabaseProvider.SqlServer;
    public string Host { get; set; } = TrustedSqlServerHost;
    public int Port { get; set; } = DefaultSqlServerPort;
    [JsonIgnore]
    public string Database { get; set; } = string.Empty;
    public string User { get; set; } = "";
    public string Password { get; set; } = "";
    public bool TrustedConnection { get; set; } = true;
    public LoginDatabaseProvider BillingProvider { get; set; } = LoginDatabaseProvider.SqlServer;
    public string BillingHost { get; set; } = TrustedSqlServerHost;
    public int BillingPort { get; set; } = DefaultSqlServerPort;
    public string BillingDatabase { get; set; } = "Billing";
    public string BillingUser { get; set; } = "";
    public string BillingPassword { get; set; } = "";
    public bool BillingTrustedConnection { get; set; } = true;

    public string BuildUserConnectionString(string? basePath = null)
    {
        return BuildConnectionString(Provider, Host, Port, Database, User, Password, TrustedConnection, basePath);
    }

    public string BuildConnectionString(string databaseName, string? basePath = null)
    {
        return BuildConnectionString(Provider, Host, Port, databaseName, User, Password, TrustedConnection, basePath);
    }

    public string BuildBillingConnectionString(string? basePath = null)
    {
        return BuildConnectionString(
            BillingProvider,
            BillingHost,
            BillingPort,
            BillingDatabase,
            BillingUser,
            BillingPassword,
            BillingTrustedConnection,
            basePath);
    }

    public static string GetSqlitePath(string basePath, string databaseName)
    {
        return Path.Combine(basePath, "Settings", "db", $"{databaseName}.db");
    }

    public static string NormalizeSqlServerHost(string host, bool trustedConnection)
    {
        return trustedConnection
            ? TrustedSqlServerHost
            : (string.IsNullOrWhiteSpace(host) ? string.Empty : host.Trim());
    }

    public static string NormalizeHostForStorage(LoginDatabaseProvider provider, string host, bool trustedConnection)
    {
        if (provider == LoginDatabaseProvider.SqlServer)
        {
            return NormalizeSqlServerHost(host, trustedConnection);
        }

        return string.IsNullOrWhiteSpace(host) ? string.Empty : host.Trim();
    }

    public static string GetDisplayHost(LoginDatabaseProvider provider, string host, int port, bool trustedConnection)
    {
        if (provider == LoginDatabaseProvider.Sqlite)
        {
            return string.Empty;
        }

        string normalizedHost = NormalizeHostForStorage(provider, host, trustedConnection);
        if (TrySplitExplicitPort(provider, normalizedHost, out _, out _))
        {
            return normalizedHost;
        }

        int effectivePort = GetEffectivePort(provider, normalizedHost, port);
        return provider == LoginDatabaseProvider.SqlServer
            ? (effectivePort == DefaultSqlServerPort ? normalizedHost : $"{normalizedHost},{effectivePort}")
            : (effectivePort == DefaultMariaDbPort ? normalizedHost : $"{normalizedHost}:{effectivePort}");
    }

    public static int GetEffectivePort(LoginDatabaseProvider provider, string host, int port)
    {
        if (provider == LoginDatabaseProvider.Sqlite)
        {
            return 0;
        }

        if (TrySplitExplicitPort(provider, host, out _, out int explicitPort))
        {
            return explicitPort;
        }

        return NormalizePort(provider, port);
    }

    public static string BuildSqlServerDataSource(string host, int port, bool trustedConnection)
    {
        string normalizedHost = NormalizeSqlServerHost(host, trustedConnection);
        if (string.IsNullOrWhiteSpace(normalizedHost))
        {
            normalizedHost = TrustedSqlServerHost;
        }

        if (TrySplitExplicitPort(LoginDatabaseProvider.SqlServer, normalizedHost, out string server, out int explicitPort))
        {
            return $"{server},{explicitPort}";
        }

        int effectivePort = NormalizePort(LoginDatabaseProvider.SqlServer, port);
        return effectivePort == DefaultSqlServerPort ? normalizedHost : $"{normalizedHost},{effectivePort}";
    }

    public static void ResolveMariaDbEndpoint(string host, int port, out string server, out int effectivePort)
    {
        string normalizedHost = string.IsNullOrWhiteSpace(host) ? "127.0.0.1" : host.Trim();
        if (TrySplitExplicitPort(LoginDatabaseProvider.MariaDb, normalizedHost, out string parsedServer, out int parsedPort))
        {
            server = parsedServer;
            effectivePort = parsedPort;
            return;
        }

        server = normalizedHost;
        effectivePort = NormalizePort(LoginDatabaseProvider.MariaDb, port);
    }

    private static string BuildConnectionString(
        LoginDatabaseProvider provider,
        string host,
        int port,
        string database,
        string user,
        string password,
        bool trustedConnection,
        string? basePath)
    {
        switch (provider)
        {
            case LoginDatabaseProvider.Sqlite:
            {
                string sqliteBasePath = basePath ?? AppContext.BaseDirectory;
                return $"Data Source={GetSqlitePath(sqliteBasePath, database)};";
            }
            case LoginDatabaseProvider.MariaDb:
                ResolveMariaDbEndpoint(host, port, out string mariaDbHost, out int mariaDbPort);
                return $"Server={mariaDbHost};Port={mariaDbPort};Database={database};User ID={user};Password={password};";
            default:
                string dataSource = BuildSqlServerDataSource(host, port, trustedConnection);
                if (trustedConnection)
                {
                    return $"Server={dataSource};Database={database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
                }
                return $"Server={dataSource};Database={database};User ID={user};Password={password};TrustServerCertificate=True;Encrypt=False;";
        }
    }

    private static int NormalizePort(LoginDatabaseProvider provider, int port)
    {
        if (port >= 1 && port <= 65535)
        {
            return port;
        }

        return provider == LoginDatabaseProvider.MariaDb ? DefaultMariaDbPort : DefaultSqlServerPort;
    }

    private static bool TrySplitExplicitPort(LoginDatabaseProvider provider, string host, out string server, out int port)
    {
        server = string.IsNullOrWhiteSpace(host) ? string.Empty : host.Trim();
        port = 0;
        if (string.IsNullOrWhiteSpace(server))
        {
            return false;
        }

        return provider switch
        {
            LoginDatabaseProvider.SqlServer =>
                TryExtractPort(server, ',', out server, out port) ||
                TryExtractPort(server, ':', out server, out port),
            LoginDatabaseProvider.MariaDb =>
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

public sealed class NetworkSettings
{
    public int ClientPort { get; set; } = 10001;
    public string AccountHost { get; set; } = "127.0.0.1";
    public int AccountPort { get; set; } = 27000;
    public int MaxConnections { get; set; } = 5000;

    /// <summary>
    /// User-count thresholds for load indicators (low/med/high). Length must be 3, ascending.
    /// </summary>
    public int[] UserLoadThresholds { get; set; } = new[] { 500, 1000, 1500 };
}

public sealed class SecuritySettings
{
    public string Argon2SaltBase64 { get; set; } = "";
}
