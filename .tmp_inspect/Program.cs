using System.Security.Cryptography;
using MapEditor.Formats;

string targetMapDir = args.Length > 0
  ? Path.GetFullPath(args[0])
  : @"D:\Private Files\playrf\RFOnline\Map\neutralB";
string backupMapDir = args.Length > 1
  ? Path.GetFullPath(args[1])
  : @"D:\Private Files\playrf\RFOnline\Map\neutralB - Copy";

if (!Directory.Exists(targetMapDir))
{
  Console.WriteLine($"ERROR: target map folder not found: {targetMapDir}");
  return;
}

if (!Directory.Exists(backupMapDir))
{
  Console.WriteLine($"ERROR: backup map folder not found: {backupMapDir}");
  return;
}

string? bspPath = Directory.EnumerateFiles(targetMapDir, "*.bsp", SearchOption.TopDirectoryOnly).FirstOrDefault();
if (bspPath == null)
{
  Console.WriteLine($"ERROR: no BSP found in: {targetMapDir}");
  return;
}

string mapBase = Path.GetFileNameWithoutExtension(bspPath);
string? ebpPath = TryResolveCaseInsensitiveFile(targetMapDir, mapBase + ".ebp");
if (ebpPath == null)
{
  ebpPath = Directory.EnumerateFiles(targetMapDir, "*.ebp", SearchOption.TopDirectoryOnly).FirstOrDefault();
}

if (ebpPath == null)
{
  Console.WriteLine($"ERROR: no EBP found in: {targetMapDir}");
  return;
}

Console.WriteLine($"Roundtrip open/resave (in-place)...");
Console.WriteLine($"BSP: {bspPath}");
Console.WriteLine($"EBP: {ebpPath}");

LoadedMap map = BspLoader.Load(bspPath, ebpPath, SkySourceMode.Sky2);
MapExporter.ExportBspEbpPair(map, bspPath, ebpPath, bspPath, ebpPath);
Console.WriteLine("Resave complete.");

DirectoryComparisonResult comparison = CompareDirectories(targetMapDir, backupMapDir);
Console.WriteLine();
Console.WriteLine("Directory compare result:");
Console.WriteLine($"Target : {targetMapDir}");
Console.WriteLine($"Backup : {backupMapDir}");
Console.WriteLine($"OnlyInTarget : {comparison.OnlyInLeft.Count}");
Console.WriteLine($"OnlyInBackup : {comparison.OnlyInRight.Count}");
Console.WriteLine($"Different    : {comparison.Different.Count}");

if (comparison.OnlyInLeft.Count > 0)
{
  Console.WriteLine();
  Console.WriteLine("Only in target:");
  foreach (string relative in comparison.OnlyInLeft.Take(64))
  {
    Console.WriteLine($"  + {relative}");
  }
}

if (comparison.OnlyInRight.Count > 0)
{
  Console.WriteLine();
  Console.WriteLine("Only in backup:");
  foreach (string relative in comparison.OnlyInRight.Take(64))
  {
    Console.WriteLine($"  - {relative}");
  }
}

if (comparison.Different.Count > 0)
{
  Console.WriteLine();
  Console.WriteLine("Different files (target vs backup):");
  foreach ((string relative, FileDiff diff) in comparison.Different.Take(128))
  {
    Console.WriteLine(
      $"  * {relative} | size {diff.LeftSize} vs {diff.RightSize} | sha256 {diff.LeftSha256[..16]}.. vs {diff.RightSha256[..16]}..");
  }
}

if (comparison.OnlyInLeft.Count == 0 && comparison.OnlyInRight.Count == 0 && comparison.Different.Count == 0)
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

static DirectoryComparisonResult CompareDirectories(string leftRoot, string rightRoot)
{
  Dictionary<string, string> leftFiles = Directory.EnumerateFiles(leftRoot, "*", SearchOption.AllDirectories)
    .ToDictionary(
      filePath => Path.GetRelativePath(leftRoot, filePath).Replace('\\', '/'),
      filePath => filePath,
      StringComparer.OrdinalIgnoreCase);
  Dictionary<string, string> rightFiles = Directory.EnumerateFiles(rightRoot, "*", SearchOption.AllDirectories)
    .ToDictionary(
      filePath => Path.GetRelativePath(rightRoot, filePath).Replace('\\', '/'),
      filePath => filePath,
      StringComparer.OrdinalIgnoreCase);

  List<string> onlyInLeft = [];
  List<string> onlyInRight = [];
  Dictionary<string, FileDiff> different = new(StringComparer.OrdinalIgnoreCase);

  foreach ((string relative, string leftPath) in leftFiles)
  {
    if (!rightFiles.TryGetValue(relative, out string? rightPath))
    {
      onlyInLeft.Add(relative);
      continue;
    }

    FileInfo leftInfo = new(leftPath);
    FileInfo rightInfo = new(rightPath);
    if (leftInfo.Length == rightInfo.Length)
    {
      string leftHash = ComputeSha256Hex(leftPath);
      string rightHash = ComputeSha256Hex(rightPath);
      if (string.Equals(leftHash, rightHash, StringComparison.OrdinalIgnoreCase))
      {
        continue;
      }

      different[relative] = new FileDiff(leftInfo.Length, rightInfo.Length, leftHash, rightHash);
      continue;
    }

    different[relative] = new FileDiff(
      leftInfo.Length,
      rightInfo.Length,
      ComputeSha256Hex(leftPath),
      ComputeSha256Hex(rightPath));
  }

  foreach (string relative in rightFiles.Keys)
  {
    if (!leftFiles.ContainsKey(relative))
    {
      onlyInRight.Add(relative);
    }
  }

  onlyInLeft.Sort(StringComparer.OrdinalIgnoreCase);
  onlyInRight.Sort(StringComparer.OrdinalIgnoreCase);

  return new DirectoryComparisonResult(onlyInLeft, onlyInRight, different.OrderBy(pair => pair.Key, StringComparer.OrdinalIgnoreCase)
    .ToDictionary(pair => pair.Key, pair => pair.Value, StringComparer.OrdinalIgnoreCase));
}

static string ComputeSha256Hex(string filePath)
{
  using FileStream stream = File.OpenRead(filePath);
  byte[] hash = SHA256.HashData(stream);
  return Convert.ToHexString(hash);
}

internal sealed record DirectoryComparisonResult(
  List<string> OnlyInLeft,
  List<string> OnlyInRight,
  Dictionary<string, FileDiff> Different);

internal sealed record FileDiff(
  long LeftSize,
  long RightSize,
  string LeftSha256,
  string RightSha256);
