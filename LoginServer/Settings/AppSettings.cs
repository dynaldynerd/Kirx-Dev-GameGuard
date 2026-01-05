using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace LoginServer.Settings;

public sealed class AppSettings
{
    public DatabaseSettings Database { get; set; } = new();
    public NetworkSettings Network { get; set; } = new();

    [JsonIgnore]
    public static string DefaultPath => Path.Combine(AppContext.BaseDirectory, "appsettings.login.json");

    public static AppSettings Load(string? path = null)
    {
        path ??= DefaultPath;
        if (!File.Exists(path))
        {
            return new AppSettings();
        }

        var json = File.ReadAllText(path);
        var settings = JsonSerializer.Deserialize<AppSettings>(json, JsonOptions()) ?? new AppSettings();
        return settings;
    }

    public void Save(string? path = null)
    {
        path ??= DefaultPath;
        var json = JsonSerializer.Serialize(this, JsonOptions());
        File.WriteAllText(path, json);
    }

    private static JsonSerializerOptions JsonOptions() => new()
    {
        WriteIndented = true,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };
}

public sealed class DatabaseSettings
{
    public string Host { get; set; } = "127.0.0.1";
    public int Port { get; set; } = 1433;
    public string Database { get; set; } = "RF_Account";
    public string User { get; set; } = "";
    public string Password { get; set; } = "";
    public string BillingHost { get; set; } = "127.0.0.1";
    public int BillingPort { get; set; } = 1433;
    public string BillingDatabase { get; set; } = "Billing";
    public string BillingUser { get; set; } = "";
    public string BillingPassword { get; set; } = "";

    public string BuildConnectionString()
    {
        return $"Server={Host},{Port};Database={Database};User ID={User};Password={Password};TrustServerCertificate=True;Encrypt=False;";
    }

    public string BuildBillingConnectionString()
    {
        return $"Server={BillingHost},{BillingPort};Database={BillingDatabase};User ID={BillingUser};Password={BillingPassword};TrustServerCertificate=True;Encrypt=False;";
    }
}

public sealed class NetworkSettings
{
    public int ClientPort { get; set; } = 10001;
    public string AccountHost { get; set; } = "127.0.0.1";
    public int AccountPort { get; set; } = 27000;
    public int MaxConnections { get; set; } = -1;

    /// <summary>
    /// User-count thresholds for load indicators (low/med/high). Length must be 3, ascending.
    /// </summary>
    public int[] UserLoadThresholds { get; set; } = new[] { 500, 1000, 1500 };
}
