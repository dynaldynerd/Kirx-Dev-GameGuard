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
                User = new DbProfile
                {
                    Host = "(local)",
                    Port = 1433,
                    Database = "RF_User",
                    User = string.Empty,
                    Password = string.Empty,
                    TrustedConnection = true
                },
                Billing = new DbProfile
                {
                    Host = "(local)",
                    Port = 1433,
                    Database = "Billing",
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
                        Address = "127.0.0.1",
                        DbName = "RF_World",
                        Type = 0
                    }
                }
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
    public DbProfile User { get; set; } = new();
    public DbProfile Billing { get; set; } = new();

    public string BuildUserConnectionString() => BuildConnectionString(User);
    public string BuildBillingConnectionString() => BuildConnectionString(Billing);

    private static string BuildConnectionString(DbProfile profile)
    {
        if (profile.TrustedConnection)
        {
            return $"Server={profile.Host},{profile.Port};Database={profile.Database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }
        return $"Server={profile.Host},{profile.Port};Database={profile.Database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
    }
}

public sealed class DbProfile
{
    public string Host { get; set; } = "(local)";
    public int Port { get; set; } = 1433;
    public string Database { get; set; } = "";
    public string User { get; set; } = "";
    public string Password { get; set; } = "";
    public bool TrustedConnection { get; set; } = true;
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
    public string Name { get; set; } = "";
    public string Address { get; set; } = "";
    public string DbName { get; set; } = "";
    public int Type { get; set; }
}

public sealed class SecuritySettings
{
    public string Argon2SaltBase64 { get; set; } = "";
}
