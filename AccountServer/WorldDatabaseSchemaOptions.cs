using System;

namespace AccountServer;

public sealed class WorldDatabaseSchemaOption
{
    public WorldDatabaseSchemaOption(string key, string displayName, string resourceName)
    {
        Key = key;
        DisplayName = displayName;
        ResourceName = resourceName;
    }

    public string Key { get; }
    public string DisplayName { get; }
    public string ResourceName { get; }

    public string Summary => DisplayName;

    public override string ToString() => DisplayName;
}

public static class WorldDatabaseSchemaOptions
{
    public static WorldDatabaseSchemaOption[] All { get; } =
    {
        new("2232", "2232", "AccountServer.Schemas.RF_World2232_mssql.sql"),
        new("aop415", "AOP415", "AccountServer.Schemas.RF_WorldAOP415_mssql.sql")
    };

    public static WorldDatabaseSchemaOption Default => All[1];

    public static WorldDatabaseSchemaOption FindByKey(string? key)
    {
        if (!string.IsNullOrWhiteSpace(key))
        {
            foreach (WorldDatabaseSchemaOption option in All)
            {
                if (string.Equals(option.Key, key, StringComparison.OrdinalIgnoreCase))
                {
                    return option;
                }
            }
        }

        return Default;
    }
}
