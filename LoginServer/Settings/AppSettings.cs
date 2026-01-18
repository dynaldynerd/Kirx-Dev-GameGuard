using System;
using System.IO;
using System.Security.Cryptography;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace LoginServer.Settings;

public sealed class AppSettings
{
    public DatabaseSettings Database { get; set; } = new();
    public NetworkSettings Network { get; set; } = new();
    public SecuritySettings Security { get; set; } = new();

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
        settings.Security ??= new SecuritySettings();
        if (!IsValidBase64(settings.Security.Argon2SaltBase64))
        {
            settings.Security.Argon2SaltBase64 = GenerateSaltBase64();
            settings.Save(path);
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
        var json = JsonSerializer.Serialize(this, JsonOptions());
        File.WriteAllText(path, json);
    }

    private static AppSettings CreateDefault()
    {
        return new AppSettings
        {
            Database = new DatabaseSettings
            {
                Host = "(local)",
                Port = 1433,
                Database = "RF_User",
                User = string.Empty,
                Password = string.Empty,
                TrustedConnection = true,
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
            Security = new SecuritySettings
            {
                Argon2SaltBase64 = GenerateSaltBase64()
            }
        };
    }

    private static JsonSerializerOptions JsonOptions() => new()
    {
        WriteIndented = true,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };

    private static string GenerateSaltBase64()
    {
        var bytes = RandomNumberGenerator.GetBytes(16);
        return Convert.ToBase64String(bytes);
    }

    private static bool IsValidBase64(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return false;
        }
        try
        {
            _ = Convert.FromBase64String(value);
            return true;
        }
        catch (FormatException)
        {
            return false;
        }
    }
}

public sealed class DatabaseSettings
{
    public string Host { get; set; } = "(local)";
    public int Port { get; set; } = 1433;
    public string Database { get; set; } = "RF_User";
    public string User { get; set; } = "";
    public string Password { get; set; } = "";
    public bool TrustedConnection { get; set; } = true;
    public string BillingHost { get; set; } = "(local)";
    public int BillingPort { get; set; } = 1433;
    public string BillingDatabase { get; set; } = "Billing";
    public string BillingUser { get; set; } = "";
    public string BillingPassword { get; set; } = "";
    public bool BillingTrustedConnection { get; set; } = true;

    public string BuildConnectionString()
    {
        if (TrustedConnection)
        {
            return $"Server={Host},{Port};Database={Database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }
        return $"Server={Host},{Port};Database={Database};User ID={User};Password={Password};TrustServerCertificate=True;Encrypt=False;";
    }

    public string BuildBillingConnectionString()
    {
        if (BillingTrustedConnection)
        {
            return $"Server={BillingHost},{BillingPort};Database={BillingDatabase};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }
        return $"Server={BillingHost},{BillingPort};Database={BillingDatabase};User ID={BillingUser};Password={BillingPassword};TrustServerCertificate=True;Encrypt=False;";
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
