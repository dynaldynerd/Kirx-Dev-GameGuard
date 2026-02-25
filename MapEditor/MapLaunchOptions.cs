using System.Text;

namespace MapEditor;

internal sealed class MapLaunchOptions
{
  public static string UsageText =>
    """
    Usage:
      MapEditor --bsp <path-to-map.bsp> [--ebp <path-to-map.ebp>]
      MapEditor <path-to-map.bsp>
      MapEditor <path-to-map.ebp>

    Notes:
      - If --ebp is omitted, the viewer uses the same file name as BSP with .ebp extension.
      - Both files are required for collision-line rendering.
    """;

  public required string BspPath { get; init; }
  public required string EbpPath { get; init; }

  public static bool TryParse(string[] args, out MapLaunchOptions? options, out string? error)
  {
    options = null;
    error = null;

    if (args.Length == 0)
    {
      error = "Missing map path arguments.";
      return false;
    }

    string? bspPath = null;
    string? ebpPath = null;

    if (args.Length == 1)
    {
      if (!TryResolveSinglePath(args[0], out bspPath, out ebpPath, out error))
      {
        return false;
      }
    }
    else
    {
      if (!args[0].StartsWith("--", StringComparison.Ordinal))
      {
        string combinedPath = string.Join(" ", args);
        if (!TryResolveSinglePath(combinedPath, out bspPath, out ebpPath, out error))
        {
          return false;
        }

        options = new MapLaunchOptions
        {
          BspPath = bspPath!,
          EbpPath = ebpPath!,
        };
        return true;
      }

      Dictionary<string, string> pairs = ParseNamedArgs(args, out error);
      if (error != null)
      {
        return false;
      }

      if (!pairs.TryGetValue("--bsp", out string? bsp))
      {
        error = "Missing required argument: --bsp <path>";
        return false;
      }

      bspPath = Path.GetFullPath(bsp);
      if (pairs.TryGetValue("--ebp", out string? ebp))
      {
        ebpPath = Path.GetFullPath(ebp);
      }
      else
      {
        ebpPath = Path.ChangeExtension(bspPath, ".ebp");
      }
    }

    if (!File.Exists(bspPath))
    {
      error = $"BSP file not found: {bspPath}";
      return false;
    }

    if (!File.Exists(ebpPath))
    {
      StringBuilder sb = new();
      sb.AppendLine($"EBP file not found: {ebpPath}");
      sb.Append("Provide it explicitly with --ebp <path> if it is stored elsewhere.");
      error = sb.ToString();
      return false;
    }

    options = new MapLaunchOptions
    {
      BspPath = bspPath,
      EbpPath = ebpPath,
    };

    return true;
  }

  private static Dictionary<string, string> ParseNamedArgs(string[] args, out string? error)
  {
    error = null;
    Dictionary<string, string> values = new(StringComparer.OrdinalIgnoreCase);

    for (int i = 0; i < args.Length; ++i)
    {
      string token = args[i];
      if (!token.StartsWith("--", StringComparison.Ordinal))
      {
        error = $"Unexpected positional argument: {token}";
        return values;
      }

      if (i + 1 >= args.Length)
      {
        error = $"Missing value for argument: {token}";
        return values;
      }

      List<string> valueParts = [];
      int cursor = i + 1;
      while (cursor < args.Length && !args[cursor].StartsWith("--", StringComparison.Ordinal))
      {
        valueParts.Add(args[cursor]);
        ++cursor;
      }

      if (valueParts.Count == 0)
      {
        error = $"Missing value for argument: {token}";
        return values;
      }

      values[token] = string.Join(" ", valueParts);
      i = cursor - 1;
    }

    return values;
  }

  private static bool TryResolveSinglePath(
    string inputPath,
    out string? bspPath,
    out string? ebpPath,
    out string? error)
  {
    bspPath = null;
    ebpPath = null;
    error = null;

    string single = Path.GetFullPath(inputPath);
    if (!File.Exists(single))
    {
      error = $"Input file does not exist: {single}";
      return false;
    }

    string extension = Path.GetExtension(single);
    if (extension.Equals(".bsp", StringComparison.OrdinalIgnoreCase))
    {
      bspPath = single;
      ebpPath = Path.ChangeExtension(single, ".ebp");
      return true;
    }

    if (extension.Equals(".ebp", StringComparison.OrdinalIgnoreCase))
    {
      ebpPath = single;
      bspPath = Path.ChangeExtension(single, ".bsp");
      return true;
    }

    error = $"Unsupported extension '{extension}'. Expected .bsp or .ebp.";
    return false;
  }
}
