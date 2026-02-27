using System.Buffers.Binary;
using OpenTK.Mathematics;

namespace MapEditor.Formats;

public static class MapExporter
{
  private const int ExtBspHeaderSize = 0x184;
  private const int ExtBspEntryCount = 48;
  private const uint ExpectedEbpVersion = 20;
  private static readonly string[] LightmapR3tSuffixes = ["lgt.r3t", ".lgt.r3t", "_lgt.r3t"];

  public static void ExportBspEbpPair(
    LoadedMap map,
    string sourceBspPath,
    string sourceEbpPath,
    string targetBspPath,
    string targetEbpPath)
  {
    if (map == null)
    {
      throw new ArgumentNullException(nameof(map));
    }

    if (string.IsNullOrWhiteSpace(sourceBspPath))
    {
      throw new ArgumentException("Source BSP path is required.", nameof(sourceBspPath));
    }

    if (string.IsNullOrWhiteSpace(sourceEbpPath))
    {
      throw new ArgumentException("Source EBP path is required.", nameof(sourceEbpPath));
    }

    if (string.IsNullOrWhiteSpace(targetBspPath))
    {
      throw new ArgumentException("Target BSP path is required.", nameof(targetBspPath));
    }

    if (string.IsNullOrWhiteSpace(targetEbpPath))
    {
      throw new ArgumentException("Target EBP path is required.", nameof(targetEbpPath));
    }

    string sourceBsp = Path.GetFullPath(sourceBspPath);
    string sourceEbp = Path.GetFullPath(sourceEbpPath);
    string targetBsp = Path.GetFullPath(targetBspPath);
    string targetEbp = Path.GetFullPath(targetEbpPath);

    if (!File.Exists(sourceBsp))
    {
      throw new FileNotFoundException("Source BSP file does not exist.", sourceBsp);
    }

    if (!File.Exists(sourceEbp))
    {
      throw new FileNotFoundException("Source EBP file does not exist.", sourceEbp);
    }

    EnsureParentDirectory(targetBsp);
    EnsureParentDirectory(targetEbp);

    CopyFileIfDifferent(sourceBsp, targetBsp);
    ExportEbpWithCollisionData(
      sourceEbp,
      targetEbp,
      map.BspCollisionNormals,
      map.BspCollisionNodes,
      map.BspLeafBounds,
      map.CollisionVertices,
      map.CollisionLines);
    CopyMapCompanionAssets(sourceBsp, targetBsp);
  }

  public static void ExportEbpOnly(
    LoadedMap map,
    string sourceEbpPath,
    string targetEbpPath)
  {
    if (map == null)
    {
      throw new ArgumentNullException(nameof(map));
    }

    if (string.IsNullOrWhiteSpace(sourceEbpPath))
    {
      throw new ArgumentException("Source EBP path is required.", nameof(sourceEbpPath));
    }

    if (string.IsNullOrWhiteSpace(targetEbpPath))
    {
      throw new ArgumentException("Target EBP path is required.", nameof(targetEbpPath));
    }

    string sourceEbp = Path.GetFullPath(sourceEbpPath);
    string targetEbp = Path.GetFullPath(targetEbpPath);
    if (!File.Exists(sourceEbp))
    {
      throw new FileNotFoundException("Source EBP file does not exist.", sourceEbp);
    }

    EnsureParentDirectory(targetEbp);
    ExportEbpWithCollisionData(
      sourceEbp,
      targetEbp,
      map.BspCollisionNormals,
      map.BspCollisionNodes,
      map.BspLeafBounds,
      map.CollisionVertices,
      map.CollisionLines);
  }

  private static void EnsureParentDirectory(string filePath)
  {
    string? directory = Path.GetDirectoryName(filePath);
    if (!string.IsNullOrWhiteSpace(directory))
    {
      Directory.CreateDirectory(directory);
    }
  }

  private static void CopyFileIfDifferent(string sourcePath, string targetPath)
  {
    if (string.Equals(sourcePath, targetPath, StringComparison.OrdinalIgnoreCase))
    {
      return;
    }

    FileInfo sourceInfo = new(sourcePath);
    FileInfo targetInfo = new(targetPath);
    if (targetInfo.Exists &&
        targetInfo.Length == sourceInfo.Length &&
        targetInfo.LastWriteTimeUtc == sourceInfo.LastWriteTimeUtc)
    {
      return;
    }

    EnsureParentDirectory(targetPath);
    File.Copy(sourcePath, targetPath, overwrite: true);
    File.SetLastWriteTimeUtc(targetPath, sourceInfo.LastWriteTimeUtc);
  }

  private static void CopyMapCompanionAssets(string sourceBspPath, string targetBspPath)
  {
    string sourceMapDirectory = Path.GetDirectoryName(sourceBspPath) ?? string.Empty;
    string targetMapDirectory = Path.GetDirectoryName(targetBspPath) ?? string.Empty;
    if (!Directory.Exists(sourceMapDirectory) || string.IsNullOrWhiteSpace(targetMapDirectory))
    {
      return;
    }

    string sourceMapName = Path.GetFileNameWithoutExtension(sourceBspPath);
    string targetMapName = Path.GetFileNameWithoutExtension(targetBspPath);

    CopySiblingAssetByExtension(sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName, ".r3x");
    CopySiblingAssetByExtension(sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName, ".r3m");
    CopySiblingAssetByExtension(sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName, ".r3t");
    CopyLightmapR3t(sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName);

    string? extSptPath = FindExtSptPath(sourceMapDirectory, sourceMapName);
    if (extSptPath != null)
    {
      string targetExtSptPath = Path.Combine(targetMapDirectory, $"{targetMapName}EXT.spt");
      EnsureParentDirectory(targetExtSptPath);
      CopyFileIfDifferent(extSptPath, targetExtSptPath);
    }

    CopySkyAssetSet(sourceMapDirectory, targetMapDirectory, sourceMapName, targetMapName, "sky", "sky");
    CopySkyAssetSet(sourceMapDirectory, targetMapDirectory, sourceMapName, targetMapName, "sky2", "sky2");
    CopyEntityAssetRoot(sourceBspPath, targetBspPath);
  }

  private static void CopySiblingAssetByExtension(
    string sourceDirectory,
    string sourceMapName,
    string targetDirectory,
    string targetMapName,
    string extension)
  {
    string? sourcePath = FindSiblingFileWithExtension(sourceDirectory, sourceMapName, extension);
    if (sourcePath == null)
    {
      return;
    }

    string targetPath = Path.Combine(targetDirectory, targetMapName + extension);
    EnsureParentDirectory(targetPath);
    CopyFileIfDifferent(sourcePath, targetPath);
  }

  private static void CopyLightmapR3t(
    string sourceDirectory,
    string sourceMapName,
    string targetDirectory,
    string targetMapName)
  {
    for (int i = 0; i < LightmapR3tSuffixes.Length; ++i)
    {
      string suffix = LightmapR3tSuffixes[i];
      string candidate = Path.Combine(sourceDirectory, sourceMapName + suffix);
      string? sourcePath = FindPathCaseInsensitive(candidate);
      if (sourcePath == null)
      {
        continue;
      }

      string targetPath = Path.Combine(targetDirectory, targetMapName + suffix);
      EnsureParentDirectory(targetPath);
      CopyFileIfDifferent(sourcePath, targetPath);
      return;
    }
  }

  private static void CopySkyAssetSet(
    string sourceMapDirectory,
    string targetMapDirectory,
    string sourceMapName,
    string targetMapName,
    string skyFolderName,
    string skySuffix)
  {
    string sourceSkyDirectory = Path.Combine(sourceMapDirectory, skyFolderName);
    if (!Directory.Exists(sourceSkyDirectory))
    {
      return;
    }

    string sourceSkyR3eFileName = $"{sourceMapName}{skySuffix}.r3e";
    string? sourceSkyR3ePath = FindPathCaseInsensitive(Path.Combine(sourceSkyDirectory, sourceSkyR3eFileName));
    if (sourceSkyR3ePath == null)
    {
      return;
    }

    string targetSkyDirectory = Path.Combine(targetMapDirectory, skyFolderName);
    string targetSkyBasePath = Path.Combine(targetSkyDirectory, $"{targetMapName}{skySuffix}");
    string targetSkyR3ePath = targetSkyBasePath + ".r3e";
    EnsureParentDirectory(targetSkyR3ePath);
    CopyFileIfDifferent(sourceSkyR3ePath, targetSkyR3ePath);

    string sourceSkyBaseName = Path.GetFileNameWithoutExtension(sourceSkyR3ePath);
    CopyAssetByBaseNameAndExtension(sourceSkyDirectory, sourceSkyBaseName, targetSkyBasePath, ".r3m");
    CopyAssetByBaseNameAndExtension(sourceSkyDirectory, sourceSkyBaseName, targetSkyBasePath, ".r3t");
  }

  private static void CopyAssetByBaseNameAndExtension(
    string sourceDirectory,
    string sourceBaseName,
    string targetBasePath,
    string extension)
  {
    string? sourcePath = FindFileByBaseNameAndExtension(sourceDirectory, sourceBaseName, extension);
    if (sourcePath == null)
    {
      return;
    }

    string targetPath = targetBasePath + extension;
    EnsureParentDirectory(targetPath);
    CopyFileIfDifferent(sourcePath, targetPath);
  }

  private static string? FindSiblingFileWithExtension(string directory, string fileNameWithoutExtension, string extension)
  {
    string candidate = Path.Combine(directory, fileNameWithoutExtension + extension);
    if (File.Exists(candidate))
    {
      return candidate;
    }

    if (!Directory.Exists(directory))
    {
      return null;
    }

    foreach (string siblingPath in Directory.EnumerateFiles(directory))
    {
      if (!string.Equals(Path.GetExtension(siblingPath), extension, StringComparison.OrdinalIgnoreCase))
      {
        continue;
      }

      if (string.Equals(Path.GetFileNameWithoutExtension(siblingPath), fileNameWithoutExtension, StringComparison.OrdinalIgnoreCase))
      {
        return siblingPath;
      }
    }

    return null;
  }

  private static string? FindFileByBaseNameAndExtension(string directory, string baseName, string extension)
  {
    string candidate = Path.Combine(directory, baseName + extension);
    if (File.Exists(candidate))
    {
      return candidate;
    }

    if (!Directory.Exists(directory))
    {
      return null;
    }

    foreach (string filePath in Directory.EnumerateFiles(directory))
    {
      if (!string.Equals(Path.GetExtension(filePath), extension, StringComparison.OrdinalIgnoreCase))
      {
        continue;
      }

      if (string.Equals(Path.GetFileNameWithoutExtension(filePath), baseName, StringComparison.OrdinalIgnoreCase))
      {
        return filePath;
      }
    }

    return null;
  }

  private static string? FindExtSptPath(string directory, string mapName)
  {
    if (!Directory.Exists(directory))
    {
      return null;
    }

    string expectedFileName = $"{mapName}EXT";
    string? byExact = FindPathCaseInsensitive(Path.Combine(directory, expectedFileName + ".spt"));
    if (byExact != null)
    {
      return byExact;
    }

    foreach (string filePath in Directory.EnumerateFiles(directory, "*.spt"))
    {
      if (string.Equals(Path.GetFileNameWithoutExtension(filePath), expectedFileName, StringComparison.OrdinalIgnoreCase))
      {
        return filePath;
      }
    }

    return null;
  }

  private static string? FindPathCaseInsensitive(string path)
  {
    if (File.Exists(path))
    {
      return path;
    }

    string? directory = Path.GetDirectoryName(path);
    if (string.IsNullOrWhiteSpace(directory) || !Directory.Exists(directory))
    {
      return null;
    }

    string fileName = Path.GetFileName(path);
    foreach (string siblingPath in Directory.EnumerateFiles(directory))
    {
      if (string.Equals(Path.GetFileName(siblingPath), fileName, StringComparison.OrdinalIgnoreCase))
      {
        return siblingPath;
      }
    }

    return null;
  }

  private static void CopyEntityAssetRoot(string sourceBspPath, string targetBspPath)
  {
    string? sourceEntityRoot = TryResolveEntityRootPath(sourceBspPath, mustExist: true);
    if (string.IsNullOrWhiteSpace(sourceEntityRoot))
    {
      return;
    }

    string? targetEntityRoot = TryResolveEntityRootPath(targetBspPath, mustExist: false);
    if (string.IsNullOrWhiteSpace(targetEntityRoot))
    {
      return;
    }

    string normalizedSource = NormalizeDirectoryPath(sourceEntityRoot);
    string normalizedTarget = NormalizeDirectoryPath(targetEntityRoot);
    if (string.Equals(normalizedSource, normalizedTarget, StringComparison.OrdinalIgnoreCase))
    {
      return;
    }

    CopyDirectoryRecursive(sourceEntityRoot, targetEntityRoot);
  }

  private static string? TryResolveEntityRootPath(string bspPath, bool mustExist)
  {
    string fullPath = Path.GetFullPath(bspPath);
    string marker = $"{Path.DirectorySeparatorChar}Map{Path.DirectorySeparatorChar}";
    int markerIndex = fullPath.LastIndexOf(marker, StringComparison.OrdinalIgnoreCase);
    if (markerIndex >= 0)
    {
      string root = fullPath[..markerIndex];
      string candidate = Path.Combine(root, "Map", "Entity");
      if (!mustExist || Directory.Exists(candidate))
      {
        return candidate;
      }
    }

    string? mapFolder = Path.GetDirectoryName(fullPath);
    if (mapFolder == null)
    {
      return null;
    }

    DirectoryInfo? mapDirectory = Directory.GetParent(mapFolder);
    if (mapDirectory == null)
    {
      return null;
    }

    string fallback = Path.Combine(mapDirectory.FullName, "Entity");
    if (!mustExist || Directory.Exists(fallback))
    {
      return fallback;
    }

    return null;
  }

  private static string NormalizeDirectoryPath(string directoryPath)
  {
    return Path.GetFullPath(directoryPath)
      .TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
  }

  private static void CopyDirectoryRecursive(string sourceDirectory, string targetDirectory)
  {
    if (!Directory.Exists(sourceDirectory))
    {
      return;
    }

    Directory.CreateDirectory(targetDirectory);

    foreach (string sourceSubDirectory in Directory.EnumerateDirectories(sourceDirectory, "*", SearchOption.AllDirectories))
    {
      string relative = Path.GetRelativePath(sourceDirectory, sourceSubDirectory);
      string targetSubDirectory = Path.Combine(targetDirectory, relative);
      Directory.CreateDirectory(targetSubDirectory);
    }

    foreach (string sourceFilePath in Directory.EnumerateFiles(sourceDirectory, "*", SearchOption.AllDirectories))
    {
      string relative = Path.GetRelativePath(sourceDirectory, sourceFilePath);
      string targetFilePath = Path.Combine(targetDirectory, relative);
      CopyFileIfDifferent(sourceFilePath, targetFilePath);
    }
  }

  private static void ExportEbpWithCollisionData(
    string sourceEbpPath,
    string targetEbpPath,
    Vector3[] bspCollisionNormals,
    BspNode[] bspCollisionNodes,
    BspLeafBounds[] bspLeafBounds,
    Vector3[] collisionVertices,
    CollisionLine[] collisionLines)
  {
    byte[] sourceBytes = File.ReadAllBytes(sourceEbpPath);
    if (sourceBytes.Length < ExtBspHeaderSize)
    {
      throw new InvalidDataException("Source EBP is too small.");
    }

    SectionEntry[] sourceEntries = ReadExtHeaderEntries(sourceBytes, out uint version);
    if (version != ExpectedEbpVersion)
    {
      throw new InvalidDataException($"Unexpected EBP version {version}; expected {ExpectedEbpVersion}.");
    }

    byte[][] sections = ReadExtSections(sourceBytes, sourceEntries);
    if (sections.Length != ExtBspEntryCount)
    {
      throw new InvalidDataException("Invalid EBP section table.");
    }

    if (sections[0].Length % 12 != 0 || sections[1].Length % 16 != 0)
    {
      throw new InvalidDataException("Source EBP collision sections are malformed.");
    }

    if (collisionVertices.Length > ushort.MaxValue)
    {
      throw new InvalidOperationException(
        $"Collision vertex count {collisionVertices.Length} exceeds ushort index range ({ushort.MaxValue}).");
    }

    if (collisionLines.Length > ushort.MaxValue)
    {
      throw new InvalidOperationException(
        $"Collision line count {collisionLines.Length} exceeds ushort index range ({ushort.MaxValue}).");
    }

    byte[] serializedCollisionVertices = SerializeCollisionVertices(collisionVertices);
    byte[] serializedCollisionLines = SerializeCollisionLines(collisionLines, collisionVertices.Length);
    sections[0] = serializedCollisionVertices;
    sections[1] = serializedCollisionLines;
    if (bspLeafBounds.Length > 0)
    {
      BuildCollisionLeafAssignment(
        collisionVertices,
        collisionLines,
        bspCollisionNormals,
        bspCollisionNodes,
        bspLeafBounds,
        out byte[] rebuiltLineIds,
        out byte[] rebuiltLeafLinks);

      sections[2] = rebuiltLineIds;
      sections[3] = rebuiltLeafLinks;
    }
    else
    {
      sections[2] = BuildCollisionLineIdSection(collisionLines.Length, sections[2].Length);
      int leafCount = ResolveLeafCount(sections[3].Length);
      if (leafCount > 0)
      {
        sections[3] = BuildCollisionLeafSection(leafCount, collisionLines.Length);
      }
    }

    WriteExtEbp(targetEbpPath, version, sections, sourceEntries);
  }

  private static SectionEntry[] ReadExtHeaderEntries(byte[] fileBytes, out uint version)
  {
    version = BitConverter.ToUInt32(fileBytes, 0);
    SectionEntry[] entries = new SectionEntry[ExtBspEntryCount];
    int offset = 4;
    for (int index = 0; index < entries.Length; ++index)
    {
      uint sectionOffset = BitConverter.ToUInt32(fileBytes, offset);
      uint sectionSize = BitConverter.ToUInt32(fileBytes, offset + 4);
      entries[index] = new SectionEntry(sectionOffset, sectionSize);
      offset += 8;
    }

    return entries;
  }

  private static byte[][] ReadExtSections(byte[] fileBytes, SectionEntry[] entries)
  {
    byte[][] sections = new byte[entries.Length][];
    bool allOffsetsValid = true;
    for (int i = 0; i < entries.Length; ++i)
    {
      SectionEntry entry = entries[i];
      if (!IsValidSectionRange(fileBytes.Length, entry))
      {
        allOffsetsValid = false;
        break;
      }
    }

    if (allOffsetsValid)
    {
      for (int i = 0; i < entries.Length; ++i)
      {
        SectionEntry entry = entries[i];
        sections[i] = entry.Size == 0
          ? Array.Empty<byte>()
          : fileBytes.AsSpan((int)entry.Offset, (int)entry.Size).ToArray();
      }

      return sections;
    }

    int readOffset = ExtBspHeaderSize;
    for (int i = 0; i < entries.Length; ++i)
    {
      uint size = entries[i].Size;
      if (size == 0)
      {
        sections[i] = Array.Empty<byte>();
        continue;
      }

      if (readOffset < 0 || readOffset + size > fileBytes.Length)
      {
        throw new InvalidDataException("EBP sequential section layout is invalid.");
      }

      sections[i] = fileBytes.AsSpan(readOffset, (int)size).ToArray();
      readOffset += (int)size;
    }

    return sections;
  }

  private static bool IsValidSectionRange(int fileLength, SectionEntry entry)
  {
    if (entry.Size == 0)
    {
      return true;
    }

    if (entry.Offset < ExtBspHeaderSize)
    {
      return false;
    }

    ulong end = (ulong)entry.Offset + entry.Size;
    return end <= (ulong)fileLength;
  }

  private static byte[] SerializeCollisionVertices(Vector3[] vertices)
  {
    byte[] bytes = new byte[vertices.Length * 12];
    int offset = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset, 4), vertices[i].X);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 4, 4), vertices[i].Y);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 8, 4), vertices[i].Z);
      offset += 12;
    }

    return bytes;
  }

  private static byte[] SerializeCollisionLines(CollisionLine[] lines, int vertexCount)
  {
    byte[] bytes = new byte[lines.Length * 16];
    int offset = 0;
    for (int i = 0; i < lines.Length; ++i)
    {
      CollisionLine line = lines[i];
      if (line.StartVertex >= vertexCount || line.EndVertex >= vertexCount)
      {
        throw new InvalidDataException(
          $"Collision line {i} references out-of-range vertex ({line.StartVertex}->{line.EndVertex}, vertices={vertexCount}).");
      }

      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), line.Attribute);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 4, 2), line.StartVertex);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 6, 2), line.EndVertex);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 8, 4), line.Height);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 12, 2), line.FrontLeaf);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 14, 2), line.BackLeaf);
      offset += 16;
    }

    return bytes;
  }

  private static int ResolveLeafCount(int cfLeafSectionSize)
  {
    if (cfLeafSectionSize >= 6 && (cfLeafSectionSize % 6) == 0)
    {
      return cfLeafSectionSize / 6;
    }

    return 0;
  }

  private static byte[] BuildCollisionLineIdSection(int lineCount, int minimumSize)
  {
    if (lineCount < 0)
    {
      lineCount = 0;
    }

    int requiredSize = checked(lineCount * 2);
    int outputSize = Math.Max(requiredSize, Math.Max(0, minimumSize));
    if ((outputSize & 1) != 0)
    {
      outputSize += 1;
    }

    if (outputSize == 0)
    {
      return Array.Empty<byte>();
    }

    byte[] bytes = new byte[outputSize];
    int offset = 0;
    for (ushort lineIndex = 0; lineIndex < lineCount; ++lineIndex)
    {
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset, 2), lineIndex);
      offset += 2;
    }

    return bytes;
  }

  private static byte[] BuildCollisionLeafSection(int leafCount, int lineCount)
  {
    if (leafCount <= 0)
    {
      return Array.Empty<byte>();
    }

    byte[] bytes = new byte[leafCount * 6];
    ushort leafLineCount = (ushort)Math.Clamp(lineCount, 0, ushort.MaxValue);
    int offset = 0;
    for (int leafIndex = 0; leafIndex < leafCount; ++leafIndex)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), 0u);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 4, 2), leafLineCount);
      offset += 6;
    }

    return bytes;
  }

  private static void BuildCollisionLeafAssignment(
    Vector3[] collisionVertices,
    CollisionLine[] collisionLines,
    Vector3[] bspCollisionNormals,
    BspNode[] bspCollisionNodes,
    BspLeafBounds[] bspLeafBounds,
    out byte[] collisionLineIds,
    out byte[] collisionLeafLinks)
  {
    if (bspLeafBounds.Length == 0)
    {
      collisionLineIds = Array.Empty<byte>();
      collisionLeafLinks = Array.Empty<byte>();
      return;
    }

    List<ushort> hitLeafIds = new(Math.Max(1024, collisionLines.Length * 2));
    List<ushort> hitLineIndices = new(Math.Max(1024, collisionLines.Length * 2));
    bool[] hasLeafAssignmentByLine = new bool[collisionLines.Length];
    Span<PlaneEq> planes = stackalloc PlaneEq[6];

    // Old tool uses 1-based iteration and reserves index 0 as a dummy line.
    for (int lineIndex = 1; lineIndex < collisionLines.Length; ++lineIndex)
    {
      CollisionLine line = collisionLines[lineIndex];
      if (line.StartVertex >= collisionVertices.Length || line.EndVertex >= collisionVertices.Length)
      {
        throw new InvalidDataException(
          $"Collision line {lineIndex} references out-of-range vertex ({line.StartVertex}->{line.EndVertex}, vertices={collisionVertices.Length}).");
      }

      Vector3 startBottom = collisionVertices[line.StartVertex];
      Vector3 endBottom = collisionVertices[line.EndVertex];
      float height = line.Height;
      Vector3 startTop = new(startBottom.X, startBottom.Y + height, startBottom.Z);
      Vector3 endTop = new(endBottom.X, endBottom.Y + height, endBottom.Z);

      if (!IsFinite(startBottom) || !IsFinite(endBottom) || !IsFinite(startTop) || !IsFinite(endTop))
      {
        continue;
      }

      Vector3 lineMin = Vector3.ComponentMin(Vector3.ComponentMin(startBottom, endBottom), Vector3.ComponentMin(startTop, endTop));
      Vector3 lineMax = Vector3.ComponentMax(Vector3.ComponentMax(startBottom, endBottom), Vector3.ComponentMax(startTop, endTop));

      short[] fastLeafList = new short[9999];
      int fastLeafCount = GetFastLeafListFromBBox(
        lineMin,
        lineMax,
        bspCollisionNormals,
        bspCollisionNodes,
        fastLeafList);

      for (int leafListIndex = 0; leafListIndex < fastLeafCount; ++leafListIndex)
      {
        int leafIndex = fastLeafList[leafListIndex];
        if ((uint)leafIndex >= (uint)bspLeafBounds.Length)
        {
          continue;
        }
        BspLeafBounds leaf = bspLeafBounds[leafIndex];
        BuildLeafClipPlanes(leaf.Min, leaf.Max, planes);
        bool intersects =
          IsCollisionBBoxToFace(planes, startBottom, startTop, endTop) ||
          IsCollisionBBoxToFace(planes, startBottom, endTop, endBottom);

        if (!intersects)
        {
          continue;
        }

        hitLeafIds.Add((ushort)leafIndex);
        hitLineIndices.Add((ushort)lineIndex);
        hasLeafAssignmentByLine[lineIndex] = true;
      }
    }

    EnsureAllCollisionLinesHaveLeafAssignment(hasLeafAssignmentByLine);

    uint[] leafStartIds = new uint[bspLeafBounds.Length];
    ushort[] leafLineCounts = new ushort[bspLeafBounds.Length];
    List<ushort> lineIds = new(Math.Max(1024, hitLineIndices.Count));

    // Match old R3 loops (leaf and hit walk start at index 1).
    for (int leafIndex = 1; leafIndex < bspLeafBounds.Length; ++leafIndex)
    {
      leafStartIds[leafIndex] = (uint)lineIds.Count;
      int leafLineCount = 0;
      for (int hitIndex = 1; hitIndex < hitLeafIds.Count; ++hitIndex)
      {
        if (hitLeafIds[hitIndex] != leafIndex)
        {
          continue;
        }

        lineIds.Add(hitLineIndices[hitIndex]);
        ++leafLineCount;
      }

      if (leafLineCount > ushort.MaxValue)
      {
        throw new InvalidDataException($"Leaf {leafIndex} line assignment exceeds ushort range ({leafLineCount}).");
      }

      leafLineCounts[leafIndex] = (ushort)leafLineCount;
    }

    collisionLineIds = new byte[lineIds.Count * 2];
    int lineOffset = 0;
    for (int i = 0; i < lineIds.Count; ++i)
    {
      BinaryPrimitives.WriteUInt16LittleEndian(collisionLineIds.AsSpan(lineOffset, 2), lineIds[i]);
      lineOffset += 2;
    }

    collisionLeafLinks = new byte[bspLeafBounds.Length * 6];
    int leafOffset = 0;
    for (int i = 0; i < bspLeafBounds.Length; ++i)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(collisionLeafLinks.AsSpan(leafOffset, 4), leafStartIds[i]);
      BinaryPrimitives.WriteUInt16LittleEndian(collisionLeafLinks.AsSpan(leafOffset + 4, 2), leafLineCounts[i]);
      leafOffset += 6;
    }
  }

  private static void EnsureAllCollisionLinesHaveLeafAssignment(bool[] hasLeafAssignmentByLine)
  {
    if (hasLeafAssignmentByLine.Length <= 1)
    {
      return;
    }

    List<int> missingLineIndices = new();
    for (int lineIndex = 1; lineIndex < hasLeafAssignmentByLine.Length; ++lineIndex)
    {
      if (!hasLeafAssignmentByLine[lineIndex])
      {
        missingLineIndices.Add(lineIndex);
      }
    }

    if (missingLineIndices.Count == 0)
    {
      return;
    }

    const int maxPreview = 16;
    int previewCount = Math.Min(maxPreview, missingLineIndices.Count);
    string[] preview = new string[previewCount];
    for (int i = 0; i < previewCount; ++i)
    {
      preview[i] = missingLineIndices[i].ToString();
    }

    throw new InvalidDataException(
      $"Collision rebuild failed: {missingLineIndices.Count} line(s) were not assigned to any BSP leaf. " +
      $"Sample line indices: {string.Join(", ", preview)}.");
  }

  private static int GetFastLeafListFromBBox(
    Vector3 bbMin,
    Vector3 bbMax,
    Vector3[] normals,
    BspNode[] nodes,
    short[] leafList)
  {
    if (leafList.Length == 0 || normals.Length == 0 || nodes.Length <= 1)
    {
      return 0;
    }

    Vector3[] bbList =
    [
      new Vector3(bbMax.X, bbMax.Y, bbMax.Z),
      new Vector3(bbMax.X, bbMax.Y, bbMin.Z),
      new Vector3(bbMin.X, bbMax.Y, bbMin.Z),
      new Vector3(bbMin.X, bbMax.Y, bbMax.Z),
      new Vector3(bbMax.X, bbMin.Y, bbMax.Z),
      new Vector3(bbMax.X, bbMin.Y, bbMin.Z),
      new Vector3(bbMin.X, bbMin.Y, bbMin.Z),
      new Vector3(bbMin.X, bbMin.Y, bbMax.Z),
    ];

    int leafIds = 0;
    FastWalkNodeForLeafListFromBBox(1, bbList, normals, nodes, leafList, ref leafIds, leafList.Length);
    return leafIds;
  }

  private static void SubLeafListFromBBox(
    float splitResult,
    in BspNode node,
    Vector3[] bbList,
    Vector3[] normals,
    BspNode[] nodes,
    short[] leafList,
    ref int leafIds,
    int maxLeafNum)
  {
    short next = splitResult >= 0.0f ? node.Front : node.Back;
    if (next < 0)
    {
      if (leafIds < maxLeafNum)
      {
        leafList[leafIds++] = (short)(-next - 1);
      }

      return;
    }

    FastWalkNodeForLeafListFromBBox(next, bbList, normals, nodes, leafList, ref leafIds, maxLeafNum);
  }

  private static void FastWalkNodeForLeafListFromBBox(
    short nodeId,
    Vector3[] bbList,
    Vector3[] normals,
    BspNode[] nodes,
    short[] leafList,
    ref int leafIds,
    int maxLeafNum)
  {
    if (nodeId <= 0 || nodeId >= nodes.Length)
    {
      return;
    }

    BspNode node = nodes[nodeId];
    if (node.NormalIndex >= normals.Length)
    {
      return;
    }

    Vector3 normal = normals[node.NormalIndex];
    bool isSplit = false;
    float resultA = 0.0f;
    float resultB = 0.0f;
    Vector3 cross = Vector3.Zero;
    Vector3[] bbListA = new Vector3[8];
    Vector3[] bbListB = new Vector3[8];

    static bool TrySplitEdge(
      Vector3 normal,
      float distance,
      in Vector3 src,
      in Vector3 dst,
      out float r1,
      out float r2,
      out Vector3 crossPoint)
    {
      r1 = Vector3.Dot(normal, src) - distance;
      r2 = Vector3.Dot(normal, dst) - distance;
      bool split = (r1 <= 0.0f && r2 > 0.0f) || (r2 <= 0.0f && r1 > 0.0f);
      if (!split)
      {
        crossPoint = Vector3.Zero;
        return false;
      }

      float rate = r1 / (r1 - r2);
      crossPoint = src + rate * (dst - src);
      return true;
    }

    if (TrySplitEdge(normal, node.Distance, bbList[0], bbList[1], out resultA, out resultB, out cross))
    {
      isSplit = true;
      Array.Copy(bbList, bbListA, 8);
      Array.Copy(bbList, bbListB, 8);
      bbListA[1].Z = cross.Z;
      bbListA[2].Z = cross.Z;
      bbListA[5].Z = cross.Z;
      bbListA[6].Z = cross.Z;
      bbListB[0].Z = cross.Z;
      bbListB[3].Z = cross.Z;
      bbListB[4].Z = cross.Z;
      bbListB[7].Z = cross.Z;
    }

    if (!isSplit && TrySplitEdge(normal, node.Distance, bbList[1], bbList[2], out resultA, out resultB, out cross))
    {
      isSplit = true;
      Array.Copy(bbList, bbListA, 8);
      Array.Copy(bbList, bbListB, 8);
      bbListA[2].X = cross.X;
      bbListA[3].X = cross.X;
      bbListA[6].X = cross.X;
      bbListA[7].X = cross.X;
      bbListB[0].X = cross.X;
      bbListB[1].X = cross.X;
      bbListB[4].X = cross.X;
      bbListB[5].X = cross.X;
    }

    if (!isSplit && TrySplitEdge(normal, node.Distance, bbList[1], bbList[5], out resultA, out resultB, out cross))
    {
      isSplit = true;
      Array.Copy(bbList, bbListA, 8);
      Array.Copy(bbList, bbListB, 8);
      bbListA[4].Y = cross.Y;
      bbListA[5].Y = cross.Y;
      bbListA[6].Y = cross.Y;
      bbListA[7].Y = cross.Y;
      bbListB[0].Y = cross.Y;
      bbListB[1].Y = cross.Y;
      bbListB[2].Y = cross.Y;
      bbListB[3].Y = cross.Y;
    }

    if (isSplit)
    {
      SubLeafListFromBBox(resultA, node, bbListA, normals, nodes, leafList, ref leafIds, maxLeafNum);
      SubLeafListFromBBox(resultB, node, bbListB, normals, nodes, leafList, ref leafIds, maxLeafNum);
    }
    else
    {
      SubLeafListFromBBox(resultA, node, bbList, normals, nodes, leafList, ref leafIds, maxLeafNum);
    }
  }

  private static bool IsAabbOverlap(Vector3 aMin, Vector3 aMax, Vector3 bMin, Vector3 bMax)
  {
    return aMin.X <= bMax.X && bMin.X <= aMax.X
      && aMin.Y <= bMax.Y && bMin.Y <= aMax.Y
      && aMin.Z <= bMax.Z && bMin.Z <= aMax.Z;
  }

  private static bool IsFinite(Vector3 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y) && float.IsFinite(value.Z);
  }

  private static void BuildLeafClipPlanes(Vector3 min, Vector3 max, Span<PlaneEq> planes)
  {
    // top, bottom, left, right, front, back (same order as original collision.cpp)
    planes[0] = new PlaneEq(new Vector3(0f, -1f, 0f), -max.Y);
    planes[1] = new PlaneEq(new Vector3(0f, 1f, 0f), min.Y);
    planes[2] = new PlaneEq(new Vector3(1f, 0f, 0f), min.X);
    planes[3] = new PlaneEq(new Vector3(-1f, 0f, 0f), -max.X);
    planes[4] = new PlaneEq(new Vector3(0f, 0f, 1f), min.Z);
    planes[5] = new PlaneEq(new Vector3(0f, 0f, -1f), -max.Z);
  }

  private static bool IsCollisionBBoxToFace(Span<PlaneEq> planes, Vector3 a, Vector3 b, Vector3 c)
  {
    Span<Vector3> input = stackalloc Vector3[32];
    Span<Vector3> output = stackalloc Vector3[32];

    int inputCount = 3;
    input[0] = a;
    input[1] = b;
    input[2] = c;

    for (int planeIndex = 0; planeIndex < planes.Length; ++planeIndex)
    {
      PlaneEq plane = planes[planeIndex];
      if (inputCount < 3)
      {
        return false;
      }

      int outputCount = 0;
      Vector3 prev = input[inputCount - 1];
      bool prevInside = IsInsidePlane(plane, prev);

      for (int i = 0; i < inputCount; ++i)
      {
        Vector3 curr = input[i];
        bool currInside = IsInsidePlane(plane, curr);

        if (currInside)
        {
          if (!prevInside && TryIntersectPlane(plane, prev, curr, out Vector3 enter))
          {
            output[outputCount++] = enter;
          }

          output[outputCount++] = curr;
        }
        else if (prevInside && TryIntersectPlane(plane, prev, curr, out Vector3 exit))
        {
          output[outputCount++] = exit;
        }

        prev = curr;
        prevInside = currInside;
      }

      inputCount = outputCount;
      for (int i = 0; i < inputCount; ++i)
      {
        input[i] = output[i];
      }
    }

    return inputCount >= 3;
  }

  private static bool IsInsidePlane(PlaneEq plane, Vector3 point)
  {
    return Vector3.Dot(plane.Normal, point) - plane.Distance >= 0.0f;
  }

  private static bool TryIntersectPlane(PlaneEq plane, Vector3 a, Vector3 b, out Vector3 intersection)
  {
    float front = Vector3.Dot(plane.Normal, a) - plane.Distance;
    float back = Vector3.Dot(plane.Normal, b) - plane.Distance;
    if ((front > 0f && back > 0f) || (front < 0f && back < 0f))
    {
      intersection = default;
      return false;
    }

    float denominator = front - back;
    if (!float.IsFinite(denominator) || MathF.Abs(denominator) < 1e-20f)
    {
      intersection = a;
      return true;
    }

    float ratio = front / denominator;
    intersection = a + ratio * (b - a);
    return IsFinite(intersection);
  }

  private static void WriteExtEbp(
    string targetPath,
    uint version,
    byte[][] sections,
    SectionEntry[]? sourceEntries)
  {
    if (sections.Length != ExtBspEntryCount)
    {
      throw new ArgumentException($"Expected {ExtBspEntryCount} sections.", nameof(sections));
    }

    if (sourceEntries != null && sourceEntries.Length != ExtBspEntryCount)
    {
      throw new ArgumentException($"Expected {ExtBspEntryCount} source entries.", nameof(sourceEntries));
    }

    SectionEntry[] entries = new SectionEntry[ExtBspEntryCount];
    int offset = ExtBspHeaderSize;
    for (int i = 0; i < sections.Length; ++i)
    {
      byte[] section = sections[i] ?? Array.Empty<byte>();
      if (section.Length == 0)
      {
        uint preservedOffset = sourceEntries != null ? sourceEntries[i].Offset : 0u;
        entries[i] = new SectionEntry(preservedOffset, 0u);
      }
      else
      {
        entries[i] = new SectionEntry((uint)offset, (uint)section.Length);
        offset += section.Length;
      }
    }

    using FileStream stream = new(targetPath, FileMode.Create, FileAccess.Write, FileShare.None);
    using BinaryWriter writer = new(stream);
    writer.Write(version);
    for (int i = 0; i < entries.Length; ++i)
    {
      writer.Write(entries[i].Offset);
      writer.Write(entries[i].Size);
    }

    for (int i = 0; i < sections.Length; ++i)
    {
      byte[] section = sections[i];
      if (section.Length > 0)
      {
        writer.Write(section);
      }
    }
  }

  private readonly record struct PlaneEq(Vector3 Normal, float Distance);
  private readonly record struct SectionEntry(uint Offset, uint Size);
}
