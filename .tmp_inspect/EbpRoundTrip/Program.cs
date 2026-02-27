using MapEditor.Formats;
using System.Security.Cryptography;

if (args.Length < 2)
{
  Console.WriteLine("Usage: EbpRoundTrip <mapDir> <backupDir>");
  return;
}

string mapDir = Path.GetFullPath(args[0]);
string backupDir = Path.GetFullPath(args[1]);
if (!Directory.Exists(mapDir))
{
  Console.WriteLine($"ERROR: map directory not found: {mapDir}");
  return;
}

if (!Directory.Exists(backupDir))
{
  Console.WriteLine($"ERROR: backup directory not found: {backupDir}");
  return;
}

string? bspPath = Directory.EnumerateFiles(mapDir, "*.bsp", SearchOption.TopDirectoryOnly).FirstOrDefault();
if (bspPath == null)
{
  Console.WriteLine($"ERROR: no BSP found in {mapDir}");
  return;
}

string mapBase = Path.GetFileNameWithoutExtension(bspPath);
string? ebpPath = TryResolveCaseInsensitiveFile(mapDir, mapBase + ".ebp")
  ?? Directory.EnumerateFiles(mapDir, "*.ebp", SearchOption.TopDirectoryOnly).FirstOrDefault();
if (ebpPath == null)
{
  Console.WriteLine($"ERROR: no EBP found in {mapDir}");
  return;
}

Console.WriteLine($"Roundtrip in-place resave:");
Console.WriteLine($"  BSP: {bspPath}");
Console.WriteLine($"  EBP: {ebpPath}");

LoadedMap loadedMap = BspLoader.Load(bspPath, ebpPath, SkySourceMode.Sky2);
MapExporter.ExportBspEbpPair(loadedMap, bspPath, ebpPath, bspPath, ebpPath);
Console.WriteLine("Resave complete.");

Dictionary<string, string> mapFiles = Directory.EnumerateFiles(mapDir, "*", SearchOption.AllDirectories)
  .ToDictionary(
    path => Path.GetRelativePath(mapDir, path).Replace('\\', '/'),
    path => path,
    StringComparer.OrdinalIgnoreCase);
Dictionary<string, string> backupFiles = Directory.EnumerateFiles(backupDir, "*", SearchOption.AllDirectories)
  .ToDictionary(
    path => Path.GetRelativePath(backupDir, path).Replace('\\', '/'),
    path => path,
    StringComparer.OrdinalIgnoreCase);

List<string> onlyInMap = [];
List<string> onlyInBackup = [];
List<string> different = [];

foreach ((string relative, string mapFilePath) in mapFiles)
{
  if (!backupFiles.TryGetValue(relative, out string? backupFilePath))
  {
    onlyInMap.Add(relative);
    continue;
  }

  FileInfo mapInfo = new(mapFilePath);
  FileInfo backupInfo = new(backupFilePath);
  if (mapInfo.Length != backupInfo.Length)
  {
    different.Add($"{relative} | size {mapInfo.Length} vs {backupInfo.Length}");
    continue;
  }

  string mapHash = ComputeSha256Hex(mapFilePath);
  string backupHash = ComputeSha256Hex(backupFilePath);
  if (!string.Equals(mapHash, backupHash, StringComparison.OrdinalIgnoreCase))
  {
    different.Add($"{relative} | sha256 {mapHash[..16]}.. vs {backupHash[..16]}..");
  }
}

foreach (string relative in backupFiles.Keys)
{
  if (!mapFiles.ContainsKey(relative))
  {
    onlyInBackup.Add(relative);
  }
}

onlyInMap.Sort(StringComparer.OrdinalIgnoreCase);
onlyInBackup.Sort(StringComparer.OrdinalIgnoreCase);
different.Sort(StringComparer.OrdinalIgnoreCase);

Console.WriteLine();
Console.WriteLine($"Compare summary:");
Console.WriteLine($"  Only in map   : {onlyInMap.Count}");
Console.WriteLine($"  Only in backup: {onlyInBackup.Count}");
Console.WriteLine($"  Different     : {different.Count}");

if (onlyInMap.Count > 0)
{
  Console.WriteLine();
  Console.WriteLine("Only in map:");
  foreach (string line in onlyInMap.Take(64))
  {
    Console.WriteLine($"  + {line}");
  }
}

if (onlyInBackup.Count > 0)
{
  Console.WriteLine();
  Console.WriteLine("Only in backup:");
  foreach (string line in onlyInBackup.Take(64))
  {
    Console.WriteLine($"  - {line}");
  }
}

if (different.Count > 0)
{
  Console.WriteLine();
  Console.WriteLine("Different files:");
  foreach (string line in different.Take(256))
  {
    Console.WriteLine($"  * {line}");
  }
}

if (onlyInMap.Count == 0 && onlyInBackup.Count == 0 && different.Count == 0)
{
  Console.WriteLine("All files are byte-identical.");
}

static string? TryResolveCaseInsensitiveFile(string directoryPath, string fileName)
{
  string direct = Path.Combine(directoryPath, fileName);
  if (File.Exists(direct))
  {
    return direct;
  }

  foreach (string filePath in Directory.EnumerateFiles(directoryPath))
  {
    if (string.Equals(Path.GetFileName(filePath), fileName, StringComparison.OrdinalIgnoreCase))
    {
      return filePath;
    }
  }

  return null;
}

static string ComputeSha256Hex(string filePath)
{
  using FileStream stream = File.OpenRead(filePath);
  return Convert.ToHexString(SHA256.HashData(stream));
}
