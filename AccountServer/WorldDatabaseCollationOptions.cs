using System;

namespace AccountServer;

public sealed class WorldDatabaseCollationOption
{
    public WorldDatabaseCollationOption(string key, string displayName, string sqlServerCollation)
    {
        Key = key;
        DisplayName = displayName;
        SqlServerCollation = sqlServerCollation;
    }

    public string Key { get; }
    public string DisplayName { get; }
    public string SqlServerCollation { get; }

    public string Summary => $"{DisplayName} ({SqlServerCollation})";

    public override string ToString() => DisplayName;
}

public static class WorldDatabaseCollationOptions
{
    public static WorldDatabaseCollationOption[] All { get; } =
    {
        new("korea", "Korea", "Korean_Wansung_CI_AS"),
        new("brazil", "Brazil", "Latin1_General_CI_AS"),
        new("china", "China", "Chinese_PRC_CI_AS"),
        new("north_america", "North America", "Latin1_General_CI_AS"),
        new("indonesia", "Indonesia", "Latin1_General_CI_AS"),
        new("japan", "Japan", "Japanese_CI_AS"),
        new("philippines", "Philippines", "Latin1_General_CI_AS"),
        new("russia", "Russia", "Cyrillic_General_CI_AS"),
        new("taiwan", "Taiwan", "Chinese_Taiwan_Stroke_CI_AS"),
        new("vietnam", "Vietnam", "Vietnamese_CI_AS")
    };

    public static WorldDatabaseCollationOption Default => All[3];

    public static WorldDatabaseCollationOption FindByKey(string? key)
    {
        if (!string.IsNullOrWhiteSpace(key))
        {
            foreach (WorldDatabaseCollationOption option in All)
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
