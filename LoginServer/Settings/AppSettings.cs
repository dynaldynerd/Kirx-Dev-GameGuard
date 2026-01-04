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
    public string ConnectionString { get; set; } = "";
}

public sealed class NetworkSettings
{
    public int ClientPort { get; set; } = 10001;
    public string AccountHost { get; set; } = "127.0.0.1";
    public int AccountPort { get; set; } = 27000;
    public int MaxConnections { get; set; } = -1;
}
