using System.Globalization;
using System.Linq;
using System.Text;
using OpenTK.Mathematics;
using NumericsMatrix4 = System.Numerics.Matrix4x4;

namespace MapEditor.Formats;

public readonly record struct ServerMapSaveResult(
  string HelperScriptPath,
  string MonsterBlockTablePath,
  string PortalTablePath,
  int SavedDummyCount,
  int SavedMonsterBlockCount,
  int SavedMonsterSpawnCount,
  int SavedPortalCount);

public static class ServerMapSaver
{
  private const int MonBlockRecordSize = 1444;
  private const int MonActiveRecordSize = 92;
  private const int PortalRecordSize = 376;
  private const int MonsterBlockFieldCount = 1;
  private const int MonsterSpawnFieldCount = 1;
  private static readonly Encoding FixedStringEncoding = Encoding.ASCII;

  public static ServerMapSaveResult Save(ServerMapData serverData)
  {
    if (serverData == null)
    {
      throw new ArgumentNullException(nameof(serverData));
    }

    SaveHelperScript(serverData);
    SaveMonsterBlockTable(serverData);
    int savedSpawnCount = SaveMonsterSpawnTables(serverData);
    SavePortalTable(serverData);

    int savedDummyCount = BuildDummyLookup(serverData).Count;
    return new ServerMapSaveResult(
      serverData.HelperScriptPath,
      serverData.MonsterBlockTablePath,
      serverData.PortalTablePath,
      savedDummyCount,
      serverData.MonsterBlocks.Length,
      savedSpawnCount,
      serverData.Portals.Length);
  }

  private static void SaveHelperScript(ServerMapData serverData)
  {
    string originalText = File.ReadAllText(serverData.HelperScriptPath);
    string newline = originalText.Contains("\r\n", StringComparison.Ordinal) ? "\r\n" : "\n";
    bool endsWithNewline = originalText.EndsWith("\r\n", StringComparison.Ordinal) || originalText.EndsWith('\n');
    List<string> lines = File.ReadAllLines(serverData.HelperScriptPath).ToList();

    int helperBeginIndex = FindLineIndex(lines, "[HelperObjectBegin]");
    int helperEndIndex = FindLineIndex(lines, "[HelperObjectEnd]");
    if (helperBeginIndex < 0 || helperEndIndex <= helperBeginIndex)
    {
      throw new InvalidDataException($"Helper section markers were not found in '{serverData.HelperScriptPath}'.");
    }

    Dictionary<string, ServerDummyPosition> dummyByCode = BuildDummyLookup(serverData);
    Dictionary<string, ServerHelperObject> helperByName = serverData.Helpers.ToDictionary(static helper => helper.Name, StringComparer.OrdinalIgnoreCase);
    List<HelperScriptEntry> helperEntries = ParseHelperEntries(lines, helperBeginIndex + 1, helperEndIndex);
    HashSet<string> writtenDummyCodes = new(StringComparer.OrdinalIgnoreCase);

    List<string> outputLines = new(lines.Count + dummyByCode.Count * 8);
    for (int index = 0; index <= helperBeginIndex; ++index)
    {
      outputLines.Add(lines[index]);
    }

    outputLines.Add(string.Empty);
    for (int entryIndex = 0; entryIndex < helperEntries.Count; ++entryIndex)
    {
      HelperScriptEntry entry = helperEntries[entryIndex];
      if (dummyByCode.TryGetValue(entry.Name, out ServerDummyPosition? dummy))
      {
        ServerHelperObject? originalHelper = null;
        if (helperByName.TryGetValue(entry.Name, out ServerHelperObject? helper))
        {
          originalHelper = helper;
        }

        outputLines.AddRange(RewriteHelperEntry(entry.Lines, dummy, originalHelper));
        writtenDummyCodes.Add(dummy.Code);
      }
      else
      {
        outputLines.AddRange(entry.Lines);
      }

      EnsureTrailingBlankLine(outputLines);
    }

    ServerDummyPosition[] appendedDummies = dummyByCode.Values
      .Where(dummy => !writtenDummyCodes.Contains(dummy.Code))
      .OrderBy(static dummy => dummy.Code, StringComparer.OrdinalIgnoreCase)
      .ToArray();
    for (int index = 0; index < appendedDummies.Length; ++index)
    {
      outputLines.AddRange(BuildNewHelperEntry(appendedDummies[index]));
      EnsureTrailingBlankLine(outputLines);
    }

    for (int index = helperEndIndex; index < lines.Count; ++index)
    {
      outputLines.Add(lines[index]);
    }

    string savedText = string.Join(newline, outputLines);
    if (endsWithNewline)
    {
      savedText += newline;
    }

    File.WriteAllText(serverData.HelperScriptPath, savedText);
  }

  private static void SaveMonsterBlockTable(ServerMapData serverData)
  {
    List<byte[]> records = new(serverData.MonsterBlocks.Length);
    for (int blockIndex = 0; blockIndex < serverData.MonsterBlocks.Length; ++blockIndex)
    {
      ServerMapMonsterBlock block = serverData.MonsterBlocks[blockIndex];
      if (block.Dummies.Length > 20)
      {
        throw new InvalidDataException($"Monster block '{block.Code}' has {block.Dummies.Length} dummies, exceeding the 20-slot record limit.");
      }

      byte[] bytes = block.RawBytes is { Length: MonBlockRecordSize }
        ? (byte[])block.RawBytes.Clone()
        : new byte[MonBlockRecordSize];
      WriteUInt32(bytes, 0, block.DeclaredIndex);
      WriteFixedString(bytes, 4, 64, block.Code);
      WriteUInt32(bytes, 68, checked((uint)block.Dummies.Length));
      ClearRange(bytes, 72, 20 * 68);
      for (int dummyIndex = 0; dummyIndex < block.Dummies.Length; ++dummyIndex)
      {
        int dummyOffset = 72 + dummyIndex * 68;
        WriteFixedString(bytes, dummyOffset, 64, block.Dummies[dummyIndex].DummyCode);
        WriteUInt32(bytes, dummyOffset + 64, block.Dummies[dummyIndex].SelectProp);
      }

      WriteInt32(bytes, 1432, block.MinCount);
      WriteInt32(bytes, 1436, block.MobCount);
      WriteInt32(bytes, 1440, block.MaxCount);
      records.Add(bytes);
    }

    WriteRecordTable(serverData.MonsterBlockTablePath, MonsterBlockFieldCount, MonBlockRecordSize, records);
  }

  private static int SaveMonsterSpawnTables(ServerMapData serverData)
  {
    int savedSpawnCount = 0;
    for (int blockIndex = 0; blockIndex < serverData.MonsterBlocks.Length; ++blockIndex)
    {
      ServerMapMonsterBlock block = serverData.MonsterBlocks[blockIndex];
      for (int setIndex = 0; setIndex < block.SpawnSets.Length; ++setIndex)
      {
        ServerMapMonsterSpawnSet spawnSet = block.SpawnSets[setIndex];
        List<byte[]> records = new(spawnSet.Spawns.Length);
        for (int spawnIndex = 0; spawnIndex < spawnSet.Spawns.Length; ++spawnIndex)
        {
          ServerMapMonsterSpawn spawn = spawnSet.Spawns[spawnIndex];
          byte[] bytes = spawn.RawBytes is { Length: MonActiveRecordSize }
            ? (byte[])spawn.RawBytes.Clone()
            : new byte[MonActiveRecordSize];
          WriteUInt32(bytes, 0, spawn.DeclaredIndex);
          WriteFixedString(bytes, 4, 64, spawn.MonsterCode);
          WriteUInt32(bytes, 68, spawn.RegenTime);
          WriteUInt32(bytes, 72, spawn.RegenLimit);
          WriteUInt32(bytes, 76, spawn.RegenProp);
          WriteUInt32(bytes, 80, spawn.RegenMin);
          WriteUInt32(bytes, 84, spawn.StandardKill);
          WriteUInt32(bytes, 88, spawn.RegenMax);
          records.Add(bytes);
        }

        string spawnTablePath = GetMonsterSpawnTablePath(serverData, block.Code, spawnSet.SetIndex);
        WriteRecordTable(spawnTablePath, MonsterSpawnFieldCount, MonActiveRecordSize, records);
        savedSpawnCount += spawnSet.Spawns.Length;
      }
    }

    return savedSpawnCount;
  }

  private static void SavePortalTable(ServerMapData serverData)
  {
    List<byte[]> records = new(serverData.Portals.Length);
    for (int portalIndex = 0; portalIndex < serverData.Portals.Length; ++portalIndex)
    {
      ServerMapPortal portal = serverData.Portals[portalIndex];
      byte[] bytes = portal.RawBytes is { Length: PortalRecordSize }
        ? (byte[])portal.RawBytes.Clone()
        : new byte[PortalRecordSize];
      WriteUInt32(bytes, 0, portal.DeclaredIndex);
      WriteFixedString(bytes, 4, 64, portal.Code);
      WriteFixedString(bytes, 68, 64, portal.LinkMapCode);
      WriteFixedString(bytes, 132, 64, portal.LinkPortalCode);
      if (portal.RawBytes == null)
      {
        WriteFixedString(bytes, 196, 64, portal.MenuText);
      }

      WriteInt32(bytes, 260, portal.NeedConCheck);
      WriteInt32(bytes, 264, portal.NeedCharacterLevel);
      WriteInt32(bytes, 268, portal.UpLevelLimit);
      records.Add(bytes);
    }

    int fieldCount = serverData.PortalTableFieldCount > 0 ? serverData.PortalTableFieldCount : 16;
    WriteRecordTable(serverData.PortalTablePath, fieldCount, PortalRecordSize, records);
  }

  private static Dictionary<string, ServerDummyPosition> BuildDummyLookup(ServerMapData serverData)
  {
    Dictionary<string, ServerDummyPosition> dummyByCode = new(StringComparer.OrdinalIgnoreCase);
    AddDummies(dummyByCode, serverData.MonsterDummies);
    AddDummies(dummyByCode, serverData.PortalDummies);
    AddDummies(dummyByCode, serverData.StoreDummies);
    AddDummies(dummyByCode, serverData.StartDummies);
    AddDummies(dummyByCode, serverData.ResourceHighDummies);
    AddDummies(dummyByCode, serverData.ResourceMiddleDummies);
    AddDummies(dummyByCode, serverData.ResourceLowDummies);
    AddDummies(dummyByCode, serverData.SafeDummies);
    return dummyByCode;
  }

  private static void AddDummies(Dictionary<string, ServerDummyPosition> dummyByCode, ServerDummyPosition[] dummies)
  {
    for (int index = 0; index < dummies.Length; ++index)
    {
      dummyByCode[dummies[index].Code] = dummies[index];
    }
  }

  private static List<HelperScriptEntry> ParseHelperEntries(IReadOnlyList<string> lines, int startIndex, int endIndex)
  {
    List<HelperScriptEntry> entries = new();
    int lineIndex = startIndex;
    while (lineIndex < endIndex)
    {
      while (lineIndex < endIndex && !TryGetHelperEntryName(lines[lineIndex], out _))
      {
        ++lineIndex;
      }

      if (lineIndex >= endIndex)
      {
        break;
      }

      int entryStart = lineIndex;
      ++lineIndex;
      while (lineIndex < endIndex && !TryGetHelperEntryName(lines[lineIndex], out _))
      {
        ++lineIndex;
      }

      string[] entryLines = new string[lineIndex - entryStart];
      for (int index = 0; index < entryLines.Length; ++index)
      {
        entryLines[index] = lines[entryStart + index];
      }

      if (!TryGetHelperEntryName(entryLines[0], out string? entryName))
      {
        continue;
      }

      entries.Add(new HelperScriptEntry(entryName!, entryLines));
    }

    return entries;
  }

  private static IEnumerable<string> RewriteHelperEntry(IReadOnlyList<string> originalLines, ServerDummyPosition dummy, ServerHelperObject? originalHelper)
  {
    List<string> rewrittenLines = new(originalLines.Count + 2)
    {
      BuildHelperHeaderLine(dummy),
    };

    NumericsMatrix4 transform = BuildSavedTransform(dummy, originalHelper);
    bool wroteNodeTransform = false;
    int lineIndex = 1;
    while (lineIndex < originalLines.Count)
    {
      string line = originalLines[lineIndex];
      if (string.Equals(line.Trim(), "-node_tm", StringComparison.OrdinalIgnoreCase))
      {
        rewrittenLines.Add("\t-node_tm");
        AppendMatrixLines(rewrittenLines, transform);
        wroteNodeTransform = true;
        ++lineIndex;
        int matrixSkipCount = Math.Min(4, originalLines.Count - lineIndex);
        lineIndex += matrixSkipCount;
        continue;
      }

      rewrittenLines.Add(line);
      ++lineIndex;
    }

    if (!wroteNodeTransform)
    {
      rewrittenLines.Add("\t-node_tm");
      AppendMatrixLines(rewrittenLines, transform);
    }

    return rewrittenLines;
  }

  private static IEnumerable<string> BuildNewHelperEntry(ServerDummyPosition dummy)
  {
    List<string> lines =
    [
      BuildHelperHeaderLine(dummy),
      "\t-node_tm",
    ];
    AppendMatrixLines(lines, BuildSavedTransform(dummy, originalHelper: null));
    return lines;
  }

  private static string BuildHelperHeaderLine(ServerDummyPosition dummy)
  {
    return FormattableString.Invariant(
      $"\t*{dummy.Code}\t{RoundToInt(dummy.LocalMin.X)}\t{RoundToInt(dummy.LocalMin.Y)}\t{RoundToInt(dummy.LocalMin.Z)}\t\t{RoundToInt(dummy.LocalMax.X)}\t{RoundToInt(dummy.LocalMax.Y)}\t{RoundToInt(dummy.LocalMax.Z)}");
  }

  private static NumericsMatrix4 BuildSavedTransform(ServerDummyPosition dummy, ServerHelperObject? originalHelper)
  {
    if (originalHelper != null)
    {
      NumericsMatrix4 translated = originalHelper.Transform;
      translated.M41 += dummy.EditOffset.X;
      translated.M42 += dummy.EditOffset.Y;
      translated.M43 += dummy.EditOffset.Z;
      return translated;
    }

    NumericsMatrix4 identity = NumericsMatrix4.Identity;
    Vector3 center = dummy.EditedWorldCenter;
    identity.M41 = center.X;
    identity.M42 = center.Y;
    identity.M43 = center.Z;
    return identity;
  }

  private static void AppendMatrixLines(List<string> lines, in NumericsMatrix4 matrix)
  {
    lines.Add(FormatMatrixLine(matrix.M11, matrix.M12, matrix.M13, matrix.M14));
    lines.Add(FormatMatrixLine(matrix.M21, matrix.M22, matrix.M23, matrix.M24));
    lines.Add(FormatMatrixLine(matrix.M31, matrix.M32, matrix.M33, matrix.M34));
    lines.Add(FormatMatrixLine(matrix.M41, matrix.M42, matrix.M43, matrix.M44));
  }

  private static string FormatMatrixLine(float x, float y, float z, float w)
  {
    return FormattableString.Invariant($"\t{FormatFloat(x)} {FormatFloat(y)} {FormatFloat(z)} {FormatFloat(w)}");
  }

  private static string FormatFloat(float value)
  {
    return value.ToString("F6", CultureInfo.InvariantCulture);
  }

  private static int RoundToInt(float value)
  {
    return (int)MathF.Round(value, MidpointRounding.AwayFromZero);
  }

  private static bool TryGetHelperEntryName(string line, out string? name)
  {
    name = null;
    string trimmed = line.TrimStart();
    if (trimmed.Length == 0 || trimmed[0] != '*')
    {
      return false;
    }

    int separatorIndex = trimmed.IndexOfAny([' ', '\t']);
    string token = separatorIndex >= 0 ? trimmed[..separatorIndex] : trimmed;
    if (token.Length <= 1)
    {
      return false;
    }

    name = token[1..].ToLowerInvariant();
    return true;
  }

  private static void EnsureTrailingBlankLine(List<string> lines)
  {
    if (lines.Count == 0 || lines[^1].Length != 0)
    {
      lines.Add(string.Empty);
    }
  }

  private static int FindLineIndex(IReadOnlyList<string> lines, string marker)
  {
    for (int index = 0; index < lines.Count; ++index)
    {
      if (string.Equals(lines[index].Trim(), marker, StringComparison.OrdinalIgnoreCase))
      {
        return index;
      }
    }

    return -1;
  }

  private static string GetMonsterSpawnTablePath(ServerMapData serverData, string blockCode, int setIndex)
  {
    string fileName = serverData.MapDefinition.MapType == 1
      ? $"{blockCode}_{setIndex}.dat"
      : $"{blockCode}.dat";
    return Path.Combine(serverData.MapFolderPath, fileName);
  }

  private static void WriteRecordTable(string path, int fieldCount, int recordSize, IReadOnlyList<byte[]> records)
  {
    string? directoryPath = Path.GetDirectoryName(path);
    if (!string.IsNullOrWhiteSpace(directoryPath))
    {
      Directory.CreateDirectory(directoryPath);
    }

    using FileStream stream = File.Create(path);
    using BinaryWriter writer = new(stream);
    writer.Write(records.Count);
    writer.Write(fieldCount);
    writer.Write(recordSize);
    for (int recordIndex = 0; recordIndex < records.Count; ++recordIndex)
    {
      byte[] bytes = records[recordIndex];
      if (bytes.Length != recordSize)
      {
        throw new InvalidDataException($"Record {recordIndex} for '{path}' has size {bytes.Length}, expected {recordSize}.");
      }

      writer.Write(bytes);
    }
  }

  private static void ClearRange(byte[] bytes, int offset, int length)
  {
    Array.Clear(bytes, offset, length);
  }

  private static void WriteUInt32(byte[] bytes, int offset, uint value)
  {
    BitConverter.TryWriteBytes(bytes.AsSpan(offset, sizeof(uint)), value);
  }

  private static void WriteInt32(byte[] bytes, int offset, int value)
  {
    BitConverter.TryWriteBytes(bytes.AsSpan(offset, sizeof(int)), value);
  }

  private static void WriteFixedString(byte[] bytes, int offset, int length, string value)
  {
    ClearRange(bytes, offset, length);
    if (string.IsNullOrEmpty(value))
    {
      return;
    }

    byte[] encodedBytes = FixedStringEncoding.GetBytes(value);
    int copyLength = Math.Min(encodedBytes.Length, Math.Max(0, length - 1));
    if (copyLength > 0)
    {
      Buffer.BlockCopy(encodedBytes, 0, bytes, offset, copyLength);
    }
  }

  private readonly record struct HelperScriptEntry(string Name, string[] Lines);
}
