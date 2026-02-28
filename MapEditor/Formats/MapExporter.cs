using System.Buffers.Binary;
using System.Text;
using OpenTK.Mathematics;

namespace MapEditor.Formats;

public static class MapExporter
{
  private const int BspHeaderSize = 0x2AC;
  private const int BspHeaderEntryCount = 85;
  private const int ExtBspHeaderSize = 0x184;
  private const int ExtBspEntryCount = 48;
  private const uint ExpectedEbpVersion = 20;
  private const float DefaultR3mVersion = 1.2f;
  private const float DefaultR3tVersion = 1.2f;
  private const float DefaultR3xVersion = 1.1f;
  private const int R3mMaterialHeaderSize = 144;
  private const int R3mLayerSize = 46;
  private const int R3mNameBytes = 128;
  private const int R3xFixedSize = 392;
  private const uint ExtMatNoFogSky = 0x00000001;
  private const uint ExtMatFogRange = 0x00000002;
  private const uint ExtMatExistFirstFog = 0x00000004;
  private const uint ExtMatExistLensFlare = 0x00000010;
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

    BspStrictValidator.ValidateForSave(map);

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

    WriteBspFromModel(map, sourceBsp, targetBsp);
    ExportEbpWithCollisionData(
      sourceEbp,
      targetEbp,
      map.BspCollisionNormals,
      map.BspCollisionNodes,
      map.BspLeafBounds,
      map.CollisionVertices,
      map.CollisionLines);
    CopyMapCompanionAssets(map, sourceBsp, targetBsp);
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

    BspStrictValidator.ValidateForSave(map);

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

  private static void CopyFileOverwrite(string sourcePath, string targetPath)
  {
    if (string.Equals(sourcePath, targetPath, StringComparison.OrdinalIgnoreCase))
    {
      return;
    }

    EnsureParentDirectory(targetPath);
    File.Copy(sourcePath, targetPath, overwrite: true);
  }

  private static void CopyMapCompanionAssets(LoadedMap map, string sourceBspPath, string targetBspPath)
  {
    string sourceMapDirectory = Path.GetDirectoryName(sourceBspPath) ?? string.Empty;
    string targetMapDirectory = Path.GetDirectoryName(targetBspPath) ?? string.Empty;
    if (!Directory.Exists(sourceMapDirectory) || string.IsNullOrWhiteSpace(targetMapDirectory))
    {
      return;
    }

    string sourceMapName = Path.GetFileNameWithoutExtension(sourceBspPath);
    string targetMapName = Path.GetFileNameWithoutExtension(targetBspPath);

    ExportMapR3x(map, sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName);
    ExportMapR3m(map, sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName);
    ExportMapR3t(map, sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName);
    ExportLightmapR3t(map, sourceMapDirectory, sourceMapName, targetMapDirectory, targetMapName);

    string? extSptPath = FindExtSptPath(sourceMapDirectory, sourceMapName);
    if (extSptPath != null)
    {
      string targetExtSptPath = Path.Combine(targetMapDirectory, $"{targetMapName}EXT.spt");
      EnsureParentDirectory(targetExtSptPath);
      CopyFileOverwrite(extSptPath, targetExtSptPath);
    }

    CopySkyAssetSetRaw(sourceMapDirectory, targetMapDirectory, sourceMapName, targetMapName, "sky", "sky");
    CopySkyAssetSetRaw(sourceMapDirectory, targetMapDirectory, sourceMapName, targetMapName, "sky2", "sky2");
    ExportLoadedSkySidecars(map, sourceMapDirectory, targetMapDirectory, sourceMapName, targetMapName);
    CopyEntityAssetRoot(sourceBspPath, targetBspPath);
  }

  private static void ExportMapR3x(
    LoadedMap map,
    string sourceDirectory,
    string sourceMapName,
    string targetDirectory,
    string targetMapName)
  {
    string? sourcePath = FindSiblingFileWithExtension(sourceDirectory, sourceMapName, ".r3x");
    if (sourcePath == null && string.Equals(sourceDirectory, targetDirectory, StringComparison.OrdinalIgnoreCase))
    {
      return;
    }

    string targetPath = Path.Combine(targetDirectory, targetMapName + ".r3x");
    WriteR3x(map.Environment, sourcePath, targetPath);
  }

  private static void ExportMapR3m(
    LoadedMap map,
    string sourceDirectory,
    string sourceMapName,
    string targetDirectory,
    string targetMapName)
  {
    string? sourcePath = FindSiblingFileWithExtension(sourceDirectory, sourceMapName, ".r3m");
    string targetPath = Path.Combine(targetDirectory, targetMapName + ".r3m");
    if (map.Materials.Length == 0)
    {
      if (sourcePath == null)
      {
        return;
      }

      CopyFileOverwrite(sourcePath, targetPath);
      return;
    }

    WriteR3m(map.Materials, sourcePath, targetPath);
  }

  private static void ExportMapR3t(
    LoadedMap map,
    string sourceDirectory,
    string sourceMapName,
    string targetDirectory,
    string targetMapName)
  {
    string? sourcePath = FindSiblingFileWithExtension(sourceDirectory, sourceMapName, ".r3t");
    string targetPath = Path.Combine(targetDirectory, targetMapName + ".r3t");
    if (map.SurfaceTextures.Length == 0)
    {
      if (sourcePath == null)
      {
        return;
      }

      CopyFileOverwrite(sourcePath, targetPath);
      return;
    }

    WriteR3t(map.SurfaceTextures, sourcePath, targetPath);
  }

  private static void ExportLightmapR3t(
    LoadedMap map,
    string sourceDirectory,
    string sourceMapName,
    string targetDirectory,
    string targetMapName)
  {
    string? sourcePath = null;
    string suffix = LightmapR3tSuffixes[0];
    if (TryFindLightmapR3t(sourceDirectory, sourceMapName, out string foundSourcePath, out string foundSuffix))
    {
      sourcePath = foundSourcePath;
      suffix = foundSuffix;
    }

    string targetPath = Path.Combine(targetDirectory, targetMapName + suffix);
    if (map.LightmapTextures.Length == 0)
    {
      if (sourcePath == null)
      {
        return;
      }

      CopyFileOverwrite(sourcePath, targetPath);
      return;
    }

    WriteR3t(map.LightmapTextures, sourcePath, targetPath);
  }

  private static bool TryFindLightmapR3t(string directory, string mapName, out string sourcePath, out string suffix)
  {
    for (int i = 0; i < LightmapR3tSuffixes.Length; ++i)
    {
      suffix = LightmapR3tSuffixes[i];
      string candidate = Path.Combine(directory, mapName + suffix);
      string? resolved = FindPathCaseInsensitive(candidate);
      if (resolved != null)
      {
        sourcePath = resolved;
        return true;
      }
    }

    sourcePath = string.Empty;
    suffix = LightmapR3tSuffixes[0];
    return false;
  }

  private static void ExportLoadedSkySidecars(
    LoadedMap map,
    string sourceMapDirectory,
    string targetMapDirectory,
    string sourceMapName,
    string targetMapName)
  {
    if (map.SkyMaterials.Length == 0 && map.SkySurfaceTextures.Length == 0)
    {
      return;
    }

    string skyFolder = map.SkySourceMode == SkySourceMode.Sky ? "sky" : "sky2";
    string skySuffix = map.SkySourceMode == SkySourceMode.Sky ? "sky" : "sky2";
    ExportSkySidecars(map, sourceMapDirectory, targetMapDirectory, sourceMapName, targetMapName, skyFolder, skySuffix);
  }

  private static void ExportSkySidecars(
    LoadedMap map,
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
    string sourceSkyBaseName = Path.GetFileNameWithoutExtension(sourceSkyR3ePath);
    string? sourceR3mPath = FindFileByBaseNameAndExtension(sourceSkyDirectory, sourceSkyBaseName, ".r3m");
    string? sourceR3tPath = FindFileByBaseNameAndExtension(sourceSkyDirectory, sourceSkyBaseName, ".r3t");
    if (map.SkyMaterials.Length > 0)
    {
      WriteR3m(map.SkyMaterials, sourceR3mPath, targetSkyBasePath + ".r3m");
    }

    if (map.SkySurfaceTextures.Length > 0)
    {
      WriteR3t(map.SkySurfaceTextures, sourceR3tPath, targetSkyBasePath + ".r3t");
    }
  }

  private static void CopySkyAssetSetRaw(
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
    CopyFileOverwrite(sourceSkyR3ePath, targetSkyR3ePath);

    string sourceSkyBaseName = Path.GetFileNameWithoutExtension(sourceSkyR3ePath);
    CopyAssetByBaseNameAndExtensionRaw(sourceSkyDirectory, sourceSkyBaseName, targetSkyBasePath, ".r3m");
    CopyAssetByBaseNameAndExtensionRaw(sourceSkyDirectory, sourceSkyBaseName, targetSkyBasePath, ".r3t");
  }

  private static void CopyAssetByBaseNameAndExtensionRaw(
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
    CopyFileOverwrite(sourcePath, targetPath);
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
      CopyFileOverwrite(sourceFilePath, targetFilePath);
    }
  }

  private static void WriteBspFromModel(LoadedMap map, string sourceBspPath, string targetBspPath)
  {
    BspBinaryLayout? layout = map.BspBinaryLayout;
    if (layout == null
      || layout.Entries.Length != BspHeaderEntryCount
      || layout.Sections.Length != BspHeaderEntryCount)
    {
      throw new InvalidDataException("Loaded map does not contain a valid BSP binary layout for rebuild.");
    }

    BspEntry[] rebuiltEntries = RebuildBspEntries(layout.Entries, layout.Sections);
    using MemoryStream stream = new();
    using BinaryWriter writer = new(stream);
    writer.Write(layout.Version);
    for (int i = 0; i < rebuiltEntries.Length; ++i)
    {
      writer.Write(rebuiltEntries[i].Offset);
      writer.Write(rebuiltEntries[i].Size);
    }

    for (int i = 0; i < layout.Sections.Length; ++i)
    {
      byte[] section = layout.Sections[i] ?? Array.Empty<byte>();
      if (section.Length > 0)
      {
        writer.Write(section);
      }
    }

    if (layout.Trailing.Length > 0)
    {
      writer.Write(layout.Trailing);
    }

    WriteBytesAlways(targetBspPath, stream.ToArray());
  }

  private static BspEntry[] RebuildBspEntries(BspEntry[] sourceEntries, byte[][] sections)
  {
    if (sourceEntries.Length != sections.Length)
    {
      throw new InvalidDataException("BSP entry count mismatch while rebuilding.");
    }

    BspEntry[] rebuilt = new BspEntry[sourceEntries.Length];
    int offset = BspHeaderSize;
    for (int i = 0; i < sections.Length; ++i)
    {
      byte[] section = sections[i] ?? Array.Empty<byte>();
      if (section.Length == 0)
      {
        rebuilt[i] = new BspEntry(sourceEntries[i].Offset, 0u);
        continue;
      }

      rebuilt[i] = new BspEntry((uint)offset, (uint)section.Length);
      offset = checked(offset + section.Length);
    }

    return rebuilt;
  }

  private static void WriteR3m(MaterialDefinition[] materials, string? sourcePath, string targetPath)
  {
    float version = DefaultR3mVersion;
    byte[][] sourceNames = Array.Empty<byte[]>();
    TryReadR3mMetadata(sourcePath, out version, out sourceNames);

    using MemoryStream stream = new();
    using BinaryWriter writer = new(stream);
    writer.Write(version);
    writer.Write(materials.Length);
    for (int materialIndex = 0; materialIndex < materials.Length; ++materialIndex)
    {
      MaterialDefinition material = materials[materialIndex];
      MaterialLayerDefinition[] layers = material.Layers ?? Array.Empty<MaterialLayerDefinition>();
      writer.Write((uint)layers.Length);
      writer.Write(material.MaterialFlags);
      writer.Write(material.DetailSurfaceId);
      writer.Write(material.DetailScale);
      byte[] nameBytes = materialIndex < sourceNames.Length && sourceNames[materialIndex].Length == R3mNameBytes
        ? sourceNames[materialIndex]
        : BuildDefaultR3mName(materialIndex);
      WriteFixedBytes(writer, nameBytes, R3mNameBytes);
      for (int layerIndex = 0; layerIndex < layers.Length; ++layerIndex)
      {
        MaterialLayerDefinition layer = layers[layerIndex];
        writer.Write(layer.TileAniTextureCount);
        writer.Write(layer.SurfaceId);
        writer.Write(layer.AlphaType);
        writer.Write(layer.Argb);
        writer.Write(layer.LayerFlags);
        writer.Write(layer.UvLavaWave);
        writer.Write(layer.UvLavaSpeed);
        writer.Write(layer.UvScrollU);
        writer.Write(layer.UvScrollV);
        writer.Write(layer.UvRotate);
        writer.Write(layer.UvScaleStart);
        writer.Write(layer.UvScaleEnd);
        writer.Write(layer.UvScaleSpeed);
        writer.Write(layer.UvMetal);
        writer.Write(layer.AniAlphaFlicker);
        writer.Write(layer.AniAlphaFlickerRange);
        writer.Write(layer.AniTexFrame);
        writer.Write(layer.AniTexSpeed);
        writer.Write(layer.GradientAlpha);
      }
    }

    WriteBytesAlways(targetPath, stream.ToArray());
  }

  private static bool TryReadR3mMetadata(string? sourcePath, out float version, out byte[][] names)
  {
    version = DefaultR3mVersion;
    names = Array.Empty<byte[]>();
    if (string.IsNullOrWhiteSpace(sourcePath) || !File.Exists(sourcePath))
    {
      return false;
    }

    try
    {
      using FileStream stream = File.OpenRead(sourcePath);
      using BinaryReader reader = new(stream);
      if (stream.Length < 8)
      {
        return false;
      }

      version = reader.ReadSingle();
      int materialCount = reader.ReadInt32();
      if (materialCount < 0 || materialCount > 100_000)
      {
        return false;
      }

      byte[][] parsedNames = new byte[materialCount][];
      for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
      {
        if (stream.Position + R3mMaterialHeaderSize > stream.Length)
        {
          return false;
        }

        uint layerCount = reader.ReadUInt32();
        _ = reader.ReadUInt32();
        _ = reader.ReadInt32();
        _ = reader.ReadSingle();
        byte[] nameBytes = reader.ReadBytes(R3mNameBytes);
        if (nameBytes.Length != R3mNameBytes)
        {
          return false;
        }

        parsedNames[materialIndex] = nameBytes;
        long skip = (long)layerCount * R3mLayerSize;
        if (skip < 0 || stream.Position + skip > stream.Length)
        {
          return false;
        }

        stream.Position += skip;
      }

      names = parsedNames;
      return true;
    }
    catch
    {
      names = Array.Empty<byte[]>();
      return false;
    }
  }

  private static byte[] BuildDefaultR3mName(int materialIndex)
  {
    string name = $"material_{materialIndex:D4}";
    return Encoding.ASCII.GetBytes(name);
  }

  private static void WriteR3t(R3TextureBlob[] textures, string? sourcePath, string targetPath)
  {
    float version = DefaultR3tVersion;
    bool hasNameTable = HasAnyTextureName(textures);
    byte[][] sourceNames = Array.Empty<byte[]>();
    if (TryReadR3tMetadata(sourcePath, out float sourceVersion, out bool sourceHasNameTable, out sourceNames))
    {
      version = sourceVersion;
      hasNameTable = sourceHasNameTable;
    }

    using MemoryStream stream = new();
    using BinaryWriter writer = new(stream);
    writer.Write(version);
    writer.Write((uint)textures.Length);
    if (hasNameTable)
    {
      for (int textureIndex = 0; textureIndex < textures.Length; ++textureIndex)
      {
        byte[] nameBytes;
        if (textureIndex < sourceNames.Length && sourceNames[textureIndex].Length == R3mNameBytes)
        {
          nameBytes = sourceNames[textureIndex];
        }
        else if (string.IsNullOrWhiteSpace(textures[textureIndex].Name))
        {
          nameBytes = Encoding.ASCII.GetBytes($"tex_{textureIndex:D4}");
        }
        else
        {
          nameBytes = Encoding.ASCII.GetBytes(textures[textureIndex].Name);
        }

        WriteFixedBytes(writer, nameBytes, R3mNameBytes);
      }
    }

    for (int textureIndex = 0; textureIndex < textures.Length; ++textureIndex)
    {
      byte[] block = textures[textureIndex].DdsBytes ?? Array.Empty<byte>();
      if (block.Length == 0)
      {
        throw new InvalidDataException($"R3T texture {textureIndex} has empty DDS data.");
      }

      writer.Write(block.Length);
      writer.Write(block);
    }

    WriteBytesAlways(targetPath, stream.ToArray());
  }

  private static bool HasAnyTextureName(R3TextureBlob[] textures)
  {
    for (int i = 0; i < textures.Length; ++i)
    {
      if (!string.IsNullOrWhiteSpace(textures[i].Name))
      {
        return true;
      }
    }

    return false;
  }

  private static bool TryReadR3tMetadata(string? sourcePath, out float version, out bool hasNameTable, out byte[][] names)
  {
    version = DefaultR3tVersion;
    hasNameTable = false;
    names = Array.Empty<byte[]>();
    if (string.IsNullOrWhiteSpace(sourcePath) || !File.Exists(sourcePath))
    {
      return false;
    }

    try
    {
      byte[] bytes = File.ReadAllBytes(sourcePath);
      if (bytes.Length < 8)
      {
        return false;
      }

      version = BitConverter.ToSingle(bytes, 0);
      int textureCount = checked((int)BitConverter.ToUInt32(bytes, 4));
      bool namedLayout = TryProbeR3tLayout(bytes, textureCount, true);
      bool namelessLayout = TryProbeR3tLayout(bytes, textureCount, false);
      if (!namedLayout && !namelessLayout)
      {
        return false;
      }

      hasNameTable = namedLayout;
      if (hasNameTable)
      {
        byte[][] parsedNames = new byte[textureCount][];
        int offset = 8;
        for (int i = 0; i < textureCount; ++i)
        {
          parsedNames[i] = bytes.AsSpan(offset, R3mNameBytes).ToArray();
          offset += R3mNameBytes;
        }

        names = parsedNames;
      }

      return true;
    }
    catch
    {
      hasNameTable = false;
      names = Array.Empty<byte[]>();
      return false;
    }
  }

  private static bool TryProbeR3tLayout(byte[] bytes, int textureCount, bool hasNameTable)
  {
    long offset = 8L + (hasNameTable ? (long)textureCount * R3mNameBytes : 0L);
    if (offset < 0 || offset > bytes.Length)
    {
      return false;
    }

    for (int textureIndex = 0; textureIndex < textureCount; ++textureIndex)
    {
      if (offset + 4 > bytes.Length)
      {
        return false;
      }

      int blockLength = BinaryPrimitives.ReadInt32LittleEndian(bytes.AsSpan((int)offset, 4));
      offset += 4;
      if (blockLength <= 0 || offset + blockLength > bytes.Length)
      {
        return false;
      }

      offset += blockLength;
    }

    return true;
  }

  private static void WriteR3x(MapEnvironmentSettings environment, string? sourcePath, string targetPath)
  {
    R3xState state = ReadR3xState(sourcePath);
    state.Version = DefaultR3xVersion;
    state.Flags = SetFlag(state.Flags, ExtMatExistFirstFog, environment.FogEnabled);
    state.Flags = SetFlag(state.Flags, ExtMatFogRange, environment.FogRangeEnabled);
    state.Flags = SetFlag(state.Flags, ExtMatNoFogSky, environment.NoFogSky);
    state.Flags = SetFlag(state.Flags, ExtMatExistLensFlare, environment.HasLensFlare);

    if (float.IsFinite(environment.FogStart) && environment.FogStart > 0.0f)
    {
      state.FogStart = environment.FogStart;
    }

    if (float.IsFinite(environment.FogEnd) && environment.FogEnd > state.FogStart)
    {
      state.FogEnd = environment.FogEnd;
    }
    else if (state.FogEnd <= state.FogStart)
    {
      state.FogEnd = state.FogStart + 1.0f;
    }

    state.FogColor = EncodeArgb(environment.FogColor);
    if (IsFinite(environment.LensFlarePosition))
    {
      state.LensPositionX = environment.LensFlarePosition.X;
      state.LensPositionY = environment.LensFlarePosition.Y;
      state.LensPositionZ = environment.LensFlarePosition.Z;
    }

    using MemoryStream stream = new();
    using BinaryWriter writer = new(stream);
    writer.Write(state.Version);
    writer.Write(state.Flags);
    writer.Write(state.FogStart);
    writer.Write(state.FogEnd);
    writer.Write(state.FogColor);
    writer.Write(state.FogStart2);
    writer.Write(state.FogEnd2);
    writer.Write(state.FogColor2);
    writer.Write(state.Fog2BbMinX);
    writer.Write(state.Fog2BbMinY);
    writer.Write(state.Fog2BbMinZ);
    writer.Write(state.Fog2BbMaxX);
    writer.Write(state.Fog2BbMaxY);
    writer.Write(state.Fog2BbMaxZ);
    for (int i = 0; i < state.LensFlareScales.Length; ++i)
    {
      writer.Write(state.LensFlareScales[i]);
    }

    WriteFixedBytes(writer, state.LensTextureNameBytes, R3mNameBytes);
    writer.Write(state.LensPositionX);
    writer.Write(state.LensPositionY);
    writer.Write(state.LensPositionZ);
    WriteFixedBytes(writer, state.EnvEntityNameBytes, R3mNameBytes);
    writer.Write(state.EnvId);
    if (state.Trailing.Length > 0)
    {
      writer.Write(state.Trailing);
    }

    WriteBytesAlways(targetPath, stream.ToArray());
  }

  private static R3xState ReadR3xState(string? sourcePath)
  {
    if (string.IsNullOrWhiteSpace(sourcePath) || !File.Exists(sourcePath))
    {
      return R3xState.Default;
    }

    try
    {
      byte[] bytes = File.ReadAllBytes(sourcePath);
      if (bytes.Length < R3xFixedSize)
      {
        return R3xState.Default;
      }

      using MemoryStream stream = new(bytes, writable: false);
      using BinaryReader reader = new(stream);
      float[] lensFlareScales = new float[16];
      float version = reader.ReadSingle();
      uint flags = reader.ReadUInt32();
      float fogStart = reader.ReadSingle();
      float fogEnd = reader.ReadSingle();
      uint fogColor = reader.ReadUInt32();
      float fogStart2 = reader.ReadSingle();
      float fogEnd2 = reader.ReadSingle();
      uint fogColor2 = reader.ReadUInt32();
      float fog2BbMinX = reader.ReadSingle();
      float fog2BbMinY = reader.ReadSingle();
      float fog2BbMinZ = reader.ReadSingle();
      float fog2BbMaxX = reader.ReadSingle();
      float fog2BbMaxY = reader.ReadSingle();
      float fog2BbMaxZ = reader.ReadSingle();
      for (int i = 0; i < lensFlareScales.Length; ++i)
      {
        lensFlareScales[i] = reader.ReadSingle();
      }

      byte[] lensTextureNameBytes = reader.ReadBytes(R3mNameBytes);
      float lensPositionX = reader.ReadSingle();
      float lensPositionY = reader.ReadSingle();
      float lensPositionZ = reader.ReadSingle();
      byte[] envEntityNameBytes = reader.ReadBytes(R3mNameBytes);
      uint envId = reader.ReadUInt32();
      byte[] trailing = bytes.AsSpan(checked((int)stream.Position)).ToArray();
      return new R3xState
      {
        Version = version,
        Flags = flags,
        FogStart = fogStart,
        FogEnd = fogEnd,
        FogColor = fogColor,
        FogStart2 = fogStart2,
        FogEnd2 = fogEnd2,
        FogColor2 = fogColor2,
        Fog2BbMinX = fog2BbMinX,
        Fog2BbMinY = fog2BbMinY,
        Fog2BbMinZ = fog2BbMinZ,
        Fog2BbMaxX = fog2BbMaxX,
        Fog2BbMaxY = fog2BbMaxY,
        Fog2BbMaxZ = fog2BbMaxZ,
        LensFlareScales = lensFlareScales,
        LensTextureNameBytes = lensTextureNameBytes,
        LensPositionX = lensPositionX,
        LensPositionY = lensPositionY,
        LensPositionZ = lensPositionZ,
        EnvEntityNameBytes = envEntityNameBytes,
        EnvId = envId,
        Trailing = trailing,
      };
    }
    catch
    {
      return R3xState.Default;
    }
  }

  private static uint SetFlag(uint flags, uint mask, bool enabled)
  {
    return enabled ? (flags | mask) : (flags & ~mask);
  }

  private static uint EncodeArgb(Vector3 rgb)
  {
    int r = ClampColorByte(rgb.X);
    int g = ClampColorByte(rgb.Y);
    int b = ClampColorByte(rgb.Z);
    return (uint)(0xFF000000u | (uint)(r << 16) | (uint)(g << 8) | (uint)b);
  }

  private static int ClampColorByte(float channel)
  {
    float finite = float.IsFinite(channel) ? channel : 0.0f;
    return (int)Math.Clamp(MathF.Round(finite * 255.0f), 0.0f, 255.0f);
  }

  private static void WriteFixedBytes(BinaryWriter writer, byte[] bytes, int expectedLength)
  {
    byte[] source = bytes ?? Array.Empty<byte>();
    if (source.Length == expectedLength)
    {
      writer.Write(source);
      return;
    }

    byte[] fixedBytes = new byte[expectedLength];
    int copyLength = Math.Min(source.Length, expectedLength);
    if (copyLength > 0)
    {
      Buffer.BlockCopy(source, 0, fixedBytes, 0, copyLength);
    }

    writer.Write(fixedBytes);
  }

  private static void WriteBytesAlways(string targetPath, byte[] bytes)
  {
    string fullTargetPath = Path.GetFullPath(targetPath);
    EnsureParentDirectory(fullTargetPath);
    File.WriteAllBytes(fullTargetPath, bytes);
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
    int sourceLeafCount = ResolveLeafCount(sections[3].Length);
    int maxLineLeafId = ResolveMaxCollisionLeafId(collisionLines);
    bool hasIncompatibleLeafSpace =
      maxLineLeafId >= 0 &&
      (bspLeafBounds.Length == 0 || maxLineLeafId >= bspLeafBounds.Length);

    if (bspLeafBounds.Length > 0 && !hasIncompatibleLeafSpace)
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
      int fallbackLeafCount = ResolveFallbackLeafCount(sourceLeafCount, maxLineLeafId);
      BuildCollisionLeafAssignmentFromLineLeafRefs(
        collisionLines,
        fallbackLeafCount,
        out byte[] rebuiltLineIds,
        out byte[] rebuiltLeafLinks);
      sections[2] = rebuiltLineIds;
      sections[3] = rebuiltLeafLinks;
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

  private static int ResolveMaxCollisionLeafId(CollisionLine[] collisionLines)
  {
    if (collisionLines == null || collisionLines.Length == 0)
    {
      return -1;
    }

    int maxLeafId = -1;
    for (int lineIndex = 1; lineIndex < collisionLines.Length; ++lineIndex)
    {
      CollisionLine line = collisionLines[lineIndex];
      if (line.FrontLeaf > maxLeafId)
      {
        maxLeafId = line.FrontLeaf;
      }

      if (line.BackLeaf > maxLeafId)
      {
        maxLeafId = line.BackLeaf;
      }
    }

    return maxLeafId;
  }

  private static int ResolveFallbackLeafCount(int sourceLeafCount, int maxLineLeafId)
  {
    int requiredByLines = maxLineLeafId >= 0 ? maxLineLeafId + 1 : 0;
    int resolved = Math.Max(sourceLeafCount, requiredByLines);
    return Math.Max(resolved, 1);
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

  private static void BuildCollisionLeafAssignmentFromLineLeafRefs(
    CollisionLine[] collisionLines,
    int leafCount,
    out byte[] collisionLineIds,
    out byte[] collisionLeafLinks)
  {
    if (leafCount <= 0)
    {
      collisionLineIds = Array.Empty<byte>();
      collisionLeafLinks = Array.Empty<byte>();
      return;
    }

    List<ushort>[] lineIdsByLeaf = new List<ushort>[leafCount];
    for (int lineIndex = 1; lineIndex < collisionLines.Length; ++lineIndex)
    {
      CollisionLine line = collisionLines[lineIndex];
      ushort lineId = (ushort)lineIndex;
      AddLineIdToLeaf(lineIdsByLeaf, line.FrontLeaf, lineId, leafCount);
      if (line.BackLeaf != line.FrontLeaf)
      {
        AddLineIdToLeaf(lineIdsByLeaf, line.BackLeaf, lineId, leafCount);
      }
    }

    uint[] leafStartIds = new uint[leafCount];
    ushort[] leafLineCounts = new ushort[leafCount];
    List<ushort> orderedLineIds = new(Math.Max(16, collisionLines.Length * 2));
    for (int leafIndex = 0; leafIndex < leafCount; ++leafIndex)
    {
      leafStartIds[leafIndex] = (uint)orderedLineIds.Count;
      List<ushort>? ids = lineIdsByLeaf[leafIndex];
      int count = ids?.Count ?? 0;
      if (count > ushort.MaxValue)
      {
        throw new InvalidDataException($"Collision leaf {leafIndex} line assignment exceeds ushort range ({count}).");
      }

      leafLineCounts[leafIndex] = (ushort)count;
      if (ids == null || ids.Count == 0)
      {
        continue;
      }

      orderedLineIds.AddRange(ids);
    }

    collisionLineIds = new byte[orderedLineIds.Count * 2];
    int lineOffset = 0;
    for (int i = 0; i < orderedLineIds.Count; ++i)
    {
      BinaryPrimitives.WriteUInt16LittleEndian(collisionLineIds.AsSpan(lineOffset, 2), orderedLineIds[i]);
      lineOffset += 2;
    }

    collisionLeafLinks = new byte[leafCount * 6];
    int leafOffset = 0;
    for (int i = 0; i < leafCount; ++i)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(collisionLeafLinks.AsSpan(leafOffset, 4), leafStartIds[i]);
      BinaryPrimitives.WriteUInt16LittleEndian(collisionLeafLinks.AsSpan(leafOffset + 4, 2), leafLineCounts[i]);
      leafOffset += 6;
    }
  }

  private static void AddLineIdToLeaf(List<ushort>[] lineIdsByLeaf, ushort leafId, ushort lineId, int leafCount)
  {
    if (leafId >= leafCount)
    {
      return;
    }

    List<ushort>? ids = lineIdsByLeaf[leafId];
    if (ids == null)
    {
      ids = new List<ushort>(8);
      lineIdsByLeaf[leafId] = ids;
    }

    ids.Add(lineId);
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
  private struct R3xState
  {
    public float Version;
    public uint Flags;
    public float FogStart;
    public float FogEnd;
    public uint FogColor;
    public float FogStart2;
    public float FogEnd2;
    public uint FogColor2;
    public float Fog2BbMinX;
    public float Fog2BbMinY;
    public float Fog2BbMinZ;
    public float Fog2BbMaxX;
    public float Fog2BbMaxY;
    public float Fog2BbMaxZ;
    public float[] LensFlareScales;
    public byte[] LensTextureNameBytes;
    public float LensPositionX;
    public float LensPositionY;
    public float LensPositionZ;
    public byte[] EnvEntityNameBytes;
    public uint EnvId;
    public byte[] Trailing;

    public static R3xState Default { get; } = new()
    {
      Version = DefaultR3xVersion,
      Flags = 0u,
      FogStart = 5.0f,
      FogEnd = 5000.0f,
      FogColor = 0xFF101010u,
      FogStart2 = 0.0f,
      FogEnd2 = 0.0f,
      FogColor2 = 0u,
      Fog2BbMinX = 0.0f,
      Fog2BbMinY = 0.0f,
      Fog2BbMinZ = 0.0f,
      Fog2BbMaxX = 0.0f,
      Fog2BbMaxY = 0.0f,
      Fog2BbMaxZ = 0.0f,
      LensFlareScales = new float[16],
      LensTextureNameBytes = new byte[R3mNameBytes],
      LensPositionX = 0.0f,
      LensPositionY = 0.0f,
      LensPositionZ = 0.0f,
      EnvEntityNameBytes = new byte[R3mNameBytes],
      EnvId = 0u,
      Trailing = Array.Empty<byte>(),
    };
  }
  private readonly record struct SectionEntry(uint Offset, uint Size);
}
