using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace AccountServer;

internal static class EmbeddedSqlScripts
{
    public static async Task<string> LoadTextAsync(string resourceName)
    {
        using Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(resourceName)
            ?? throw new FileNotFoundException($"Missing embedded SQL resource '{resourceName}'.", resourceName);
        using var reader = new StreamReader(stream, Encoding.UTF8, detectEncodingFromByteOrderMarks: true);
        return await reader.ReadToEndAsync().ConfigureAwait(false);
    }

    public static List<string> SplitSqlServerBatches(string script)
    {
        var batches = new List<string>();
        var sb = new StringBuilder();
        using var reader = new StringReader(script);
        string? line;
        while ((line = reader.ReadLine()) != null)
        {
            if (line.Trim().Equals("GO", StringComparison.OrdinalIgnoreCase))
            {
                AddStatement(batches, sb.ToString());
                sb.Clear();
                continue;
            }

            sb.AppendLine(line);
        }

        AddStatement(batches, sb.ToString());
        return batches;
    }

    public static List<string> SplitDelimitedStatements(string script)
    {
        var statements = new List<string>();
        string delimiter = ";";
        var sb = new StringBuilder();
        using var reader = new StringReader(script);
        string? line;
        while ((line = reader.ReadLine()) != null)
        {
            string trimmed = line.Trim();
            if (trimmed.StartsWith("DELIMITER ", StringComparison.OrdinalIgnoreCase))
            {
                delimiter = trimmed["DELIMITER ".Length..].Trim();
                continue;
            }

            sb.AppendLine(line);
            if (!EndsWithDelimiter(sb, delimiter))
            {
                continue;
            }

            string statement = RemoveTrailingDelimiter(sb.ToString(), delimiter);
            AddStatement(statements, statement);
            sb.Clear();
        }

        AddStatement(statements, sb.ToString());
        return statements;
    }

    private static bool EndsWithDelimiter(StringBuilder sb, string delimiter)
    {
        string text = sb.ToString().TrimEnd();
        return !string.IsNullOrEmpty(delimiter) &&
               text.EndsWith(delimiter, StringComparison.Ordinal);
    }

    private static string RemoveTrailingDelimiter(string text, string delimiter)
    {
        string trimmed = text.TrimEnd();
        if (!string.IsNullOrEmpty(delimiter) &&
            trimmed.EndsWith(delimiter, StringComparison.Ordinal))
        {
            trimmed = trimmed[..^delimiter.Length];
        }

        return trimmed;
    }

    private static void AddStatement(ICollection<string> statements, string statement)
    {
        if (!string.IsNullOrWhiteSpace(statement))
        {
            statements.Add(statement.Trim());
        }
    }
}
