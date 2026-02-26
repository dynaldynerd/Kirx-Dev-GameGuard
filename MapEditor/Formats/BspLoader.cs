using System.Buffers.Binary;
using System.Globalization;
using System.Text;
using NumericsMatrix4 = System.Numerics.Matrix4x4;
using NumericsQuaternion = System.Numerics.Quaternion;
using NumericsVector3 = System.Numerics.Vector3;
using OpenTK.Mathematics;

namespace MapEditor.Formats;

public enum SkySourceMode
{
  Sky = 0,
  Sky2 = 1,
}

public readonly record struct EntityArchiveCacheInfo(
  string EntityRootPath,
  int ArchiveCount,
  int AssetCount,
  bool WasCached);

public static class BspLoader
{
  private const int BspHeaderSize = 0x2AC;
  private const int ExtBspHeaderSize = 0x184;
  private const int EntityListEntrySize = 0x54;
  private const int ReadMapEntityEntrySize = 0x26;
  private const int EntityFileHeaderEntryCount = 10;
  private const int ReadAniObjectEntrySize = 88;
  private const uint ExpectedBspVersion = 39;
  private const uint ExpectedEbpVersion = 20;
  private const uint ExpectedR3eMagic = 113;
  private const float ExpectedR3xVersion = 1.1f;
  private const uint MaterialFlagLightMap = 0x00000001;
  private const uint ExtMatNoFogSky = 0x00000001;
  private const uint ExtMatFogRange = 0x00000002;
  private const uint ExtMatExistFirstFog = 0x00000004;
  private const uint ExtMatExistLensFlare = 0x00000010;
  private const uint ExtDummyFlagMusic = 0x00000001;
  private const uint ExtDummyFlagFog = 0x00000002;
  private const uint DefaultParticleAlphaType = 3u;
  private const uint ParticleAlphaFlagDisableDepthTest = 0x40000000u;
  private const int ExtMatLensFlareScaleCount = 16;
  private const int ExtMatNameBytes = 128;
  private const int DefaultR3tSurfaceIdBase = 1;
  private const int LightmapR3tSurfaceIdBase = 0;
  private static readonly object EntityArchiveCacheSync = new();
  private static readonly Dictionary<string, RpkArchiveManager> EntityArchiveCache = new(StringComparer.OrdinalIgnoreCase);
  private static readonly uint[] R3tHeaderXorKey =
  [
    0x764D802E, 0xF0D1F82E, 0x81863FBD, 0x3F3F2C58,
    0x6F672E2E, 0x783F403F, 0xC0F13F3C, 0x9F3BF6A5,
    0xD73F20C1, 0x85E9C1C8, 0x56EFBD86, 0x2EFBA13F,
    0x4C618687, 0xB44E3B21, 0x97AE5778, 0x2E4A2E3F,
    0x442E4C3F, 0xE85FC5CD, 0xBDEBECE9, 0x6CF7BBBE,
    0x2EE4F22E, 0x9F973F3F, 0xB921B39D, 0x3F546576,
    0xF0C6F6E6, 0xB2E2DB79, 0xEB2E2E4B, 0xABCAD3D3,
    0x9CEDC7EA, 0x65D0D9C7, 0x35FAB448, 0x9B6A2E2E,
  ];

  public static EntityArchiveCacheInfo PrewarmEntityArchives(string bspPath)
  {
    string resolvedBsp = Path.GetFullPath(bspPath);
    string? entityRoot = TryResolveEntityRootPath(resolvedBsp);
    if (entityRoot == null)
    {
      return new EntityArchiveCacheInfo(string.Empty, 0, 0, false);
    }

    RpkArchiveManager manager = GetOrCreateEntityArchiveManager(entityRoot, out bool wasCached);
    return new EntityArchiveCacheInfo(entityRoot, manager.ArchiveCount, manager.AssetCount, wasCached);
  }

  public static LoadedMap Load(string bspPath, string ebpPath, SkySourceMode skySourceMode = SkySourceMode.Sky2)
  {
    string resolvedBsp = Path.GetFullPath(bspPath);
    string resolvedEbp = Path.GetFullPath(ebpPath);

    if (!File.Exists(resolvedBsp))
    {
      throw new FileNotFoundException("BSP file not found.", resolvedBsp);
    }

    if (!File.Exists(resolvedEbp))
    {
      throw new FileNotFoundException("EBP file not found.", resolvedEbp);
    }

    MaterialData materialData = ReadMaterialData(resolvedBsp);
    BspData bsp = ReadBsp(resolvedBsp, materialData);
    ExtBspData ext = ReadExtBsp(resolvedEbp);
    MapEnvironmentSettings environmentSettings = ReadMapEnvironmentSettings(resolvedBsp);
    ExtDummyDefinition[] extDummies = ReadExtDummyDefinitions(resolvedBsp, environmentSettings);
    R3TextureBlob[] lightmapTextures = ReadLightmapTextures(resolvedBsp);
    SkyData skyData = ReadSkyData(resolvedBsp, skySourceMode);
    EntityRenderData entityData = ReadEntityRenderData(resolvedBsp, ext.EntityDefinitions, ext.MapEntities);

    Vector3[] boundsSource = bsp.TriangleVertices.Length > 0 ? bsp.TriangleVertices : ext.CollisionVertices;
    MapBounds bounds = ComputeBoundsFromVertices(boundsSource);

    return new LoadedMap
    {
      Name = Path.GetFileNameWithoutExtension(resolvedBsp),
      BspPath = resolvedBsp,
      EbpPath = resolvedEbp,
      Environment = environmentSettings,
      ExtDummies = extDummies,
      Bounds = bounds,
      BspTriangleVertices = bsp.TriangleVertices,
      BspRenderVertices = bsp.RenderVertices,
      BspMaterialSpans = bsp.MaterialSpans,
      Materials = materialData.Materials,
      MaterialSurfaceIds = materialData.MaterialSurfaceIds,
      MaterialAlphaTypes = materialData.MaterialAlphaTypes,
      SurfaceTextures = materialData.SurfaceTextures,
      LightmapTextures = lightmapTextures,
      SkyRenderVertices = skyData.Mesh.Vertices,
      SkyMaterialSpans = skyData.Mesh.MaterialSpans,
      SkyMaterials = skyData.Mesh.Materials,
      SkyMaterialSurfaceIds = skyData.Mesh.MaterialSurfaceIds,
      SkyMaterialAlphaTypes = skyData.Mesh.MaterialAlphaTypes,
      SkySurfaceTextures = skyData.Mesh.SurfaceTextures,
      EntityRenderVertices = entityData.Mesh.Vertices,
      EntityMaterialSpans = entityData.Mesh.MaterialSpans,
      EntityMaterials = entityData.Mesh.Materials,
      EntityMaterialSurfaceIds = entityData.Mesh.MaterialSurfaceIds,
      EntityMaterialAlphaTypes = entityData.Mesh.MaterialAlphaTypes,
      EntitySurfaceTextures = entityData.Mesh.SurfaceTextures,
      EntityScene = entityData.SceneData,
      MapEntityModelCount = entityData.ModelCount,
      MapEntityInstanceCount = entityData.InstanceCount,
      ParticleInstancePositions = entityData.ParticlePositions,
      CollisionVertices = ext.CollisionVertices,
      CollisionLines = ext.CollisionLines,
    };
  }

  private static BspData ReadBsp(string bspPath, MaterialData materialData)
  {
    using FileStream stream = File.OpenRead(bspPath);
    using BinaryReader reader = new(stream);

    if (stream.Length < BspHeaderSize)
    {
      throw new InvalidDataException("BSP file is too small to contain a valid header.");
    }

    BspHeader header = ReadBspHeader(reader);
    if (header.Version != ExpectedBspVersion)
    {
      throw new InvalidDataException($"Unexpected BSP version {header.Version}; expected {ExpectedBspVersion}.");
    }

    long sectionStart = BspHeaderSize
      + header.CPlanes.Size
      + header.CFaceId.Size
      + header.Node.Size
      + header.Leaf.Size
      + header.MatListInLeaf.Size
      + header.Object.Size
      + header.Track.Size
      + header.EventObjectId.Size
      + header.ReadSpareTotalSize;

    if (sectionStart > stream.Length)
    {
      throw new InvalidDataException("BSP sections exceed file length before dynamic payload.");
    }

    stream.Position = sectionStart;

    int bVertexByteSize = checked((int)header.BVertex.Size);
    int wVertexByteSize = checked((int)header.WVertex.Size);
    int fVertexByteSize = checked((int)header.FVertex.Size);
    if ((bVertexByteSize % 3) != 0)
    {
      throw new InvalidDataException("BVertex section size is not aligned to 3 bytes.");
    }
    if ((wVertexByteSize % 6) != 0)
    {
      throw new InvalidDataException("WVertex section size is not aligned to 6 bytes.");
    }
    if ((fVertexByteSize % 12) != 0)
    {
      throw new InvalidDataException("FVertex section size is not aligned to 12 bytes.");
    }

    int bVertexCount = bVertexByteSize / 3;
    int wVertexCount = wVertexByteSize / 6;
    int fVertexCount = fVertexByteSize / 12;

    byte[] bVertexData = ReadExactBytes(reader, bVertexByteSize, "BVertex");
    byte[] wVertexData = ReadExactBytes(reader, wVertexByteSize, "WVertex");
    byte[] fVertexData = ReadExactBytes(reader, fVertexByteSize, "FVertex");
    byte[] packedVertexData = new byte[bVertexByteSize + wVertexByteSize + fVertexByteSize];
    Buffer.BlockCopy(bVertexData, 0, packedVertexData, 0, bVertexByteSize);
    Buffer.BlockCopy(wVertexData, 0, packedVertexData, bVertexByteSize, wVertexByteSize);
    Buffer.BlockCopy(fVertexData, 0, packedVertexData, bVertexByteSize + wVertexByteSize, fVertexByteSize);

    if ((header.VertexColor.Size % 4) != 0)
    {
      throw new InvalidDataException("VertexColor section size is not aligned to 4 bytes.");
    }
    if ((header.Uv.Size % 8) != 0)
    {
      throw new InvalidDataException("UV section size is not aligned to 8 bytes.");
    }
    if ((header.LgtUv.Size % 4) != 0)
    {
      throw new InvalidDataException("LgtUV section size is not aligned to 4 bytes.");
    }

    uint[] vertexColor = ReadUInt32Array(reader, checked((int)(header.VertexColor.Size / 4)), "VertexColor");
    float[] uvData = ReadSingleArray(reader, checked((int)(header.Uv.Size / 4)), "UV");
    short[] lgtUvData = ReadInt16Array(reader, checked((int)(header.LgtUv.Size / 2)), "LgtUV");

    ReadFace[] faces = ReadFaces(reader, checked((int)(header.Face.Size / 6)));
    uint[] faceIds = ReadUInt32Array(reader, checked((int)(header.FaceId.Size / 4)), "FaceId");
    uint[] vertexIds = ReadUInt32Array(reader, checked((int)(header.VertexId.Size / 4)), "VertexId");
    ReadMatGroup[] matGroups = ReadMatGroups(reader, checked((int)(header.ReadMatGroup.Size / 42)));

    MeshBuildResult mesh = BuildTriangleVertices(
      packedVertexData,
      bVertexByteSize,
      wVertexByteSize,
      bVertexCount,
      wVertexCount,
      fVertexCount,
      faces,
      faceIds,
      vertexIds,
      matGroups,
      uvData,
      lgtUvData,
      vertexColor,
      materialData.Materials);

    BspRenderVertex[] renderVertices = mesh.Vertices;
    Vector3[] triangleVertices = new Vector3[renderVertices.Length];
    for (int index = 0; index < renderVertices.Length; ++index)
    {
      triangleVertices[index] = renderVertices[index].Position;
    }

    return new BspData(triangleVertices, renderVertices, mesh.MaterialSpans);
  }

  private static ExtBspData ReadExtBsp(string ebpPath)
  {
    using FileStream stream = File.OpenRead(ebpPath);
    using BinaryReader reader = new(stream);
    if (stream.Length < ExtBspHeaderSize)
    {
      throw new InvalidDataException("EBP file is too small to contain a valid header.");
    }

    ExtBspHeader header = ReadExtBspHeader(reader);
    if (header.Version != ExpectedEbpVersion)
    {
      throw new InvalidDataException(
        $"Unexpected EBP version {header.Version}; expected {ExpectedEbpVersion}.");
    }

    int vertexCount = checked((int)(header.CfVertexSection.Size / 12));
    int lineCount = checked((int)(header.CfLineSection.Size / 16));
    if ((header.CfVertexSection.Size % 12) != 0)
    {
      throw new InvalidDataException("CFVertex section size is not aligned to 12 bytes.");
    }
    if ((header.CfLineSection.Size % 16) != 0)
    {
      throw new InvalidDataException("CFLine section size is not aligned to 16 bytes.");
    }

    if ((header.EntityListSection.Size % EntityListEntrySize) != 0)
    {
      throw new InvalidDataException("EntityList section size is not aligned to 84 bytes.");
    }
    if ((header.MapEntitiesListSection.Size % ReadMapEntityEntrySize) != 0)
    {
      throw new InvalidDataException("MapEntitiesList section size is not aligned to 38 bytes.");
    }

    // Native loader consumes EBP sequentially after header by section size.
    stream.Position = ExtBspHeaderSize;
    Vector3[] vertices = ReadCollisionVerticesSequential(reader, vertexCount);
    CollisionLine[] lines = ReadCollisionLinesSequential(reader, lineCount);
    SkipSection(stream, header.CfLineIdSection.Size, "CFLineId");
    SkipSection(stream, header.CfLeafSection.Size, "CFLeaf");
    ExtEntityDefinition[] entityDefinitions = ReadEntityDefinitions(reader, checked((int)(header.EntityListSection.Size / EntityListEntrySize)));
    SkipSection(stream, header.EntityIdSection.Size, "EntityID");
    SkipSection(stream, header.LeafEntityListSection.Size, "LeafEntityList");
    SkipSection(stream, header.SoundEntityIdSection.Size, "SoundEntityID");
    SkipSection(stream, header.LeafSoundEntityListSection.Size, "LeafSoundEntityList");
    ExtMapEntity[] mapEntities = ReadMapEntities(reader, checked((int)(header.MapEntitiesListSection.Size / ReadMapEntityEntrySize)));
    SkipSection(stream, header.SoundEntityListSection.Size, "SoundEntityList");
    SkipSection(stream, header.SoundEntitiesListSection.Size, "SoundEntitiesList");

    return new ExtBspData(vertices, lines, entityDefinitions, mapEntities);
  }

  private static MaterialData ReadMaterialData(string bspPath)
  {
    string? r3mPath = FindSiblingFileWithExtension(bspPath, ".r3m");
    string? r3tPath = FindSiblingFileWithExtension(bspPath, ".r3t");

    MaterialDefinition[] materials = Array.Empty<MaterialDefinition>();
    int[] materialSurfaceIds = Array.Empty<int>();
    uint[] materialAlphaTypes = Array.Empty<uint>();
    if (r3mPath != null)
    {
      try
      {
        R3MaterialData r3m = ReadR3mMaterials(r3mPath);
        materials = r3m.Materials;
        materialSurfaceIds = r3m.SurfaceIds;
        materialAlphaTypes = r3m.LayerAlphaTypes;
      }
      catch
      {
        materials = Array.Empty<MaterialDefinition>();
        materialSurfaceIds = Array.Empty<int>();
        materialAlphaTypes = Array.Empty<uint>();
      }
    }

    R3TextureBlob[] surfaceTextures = Array.Empty<R3TextureBlob>();
    if (r3tPath != null)
    {
      try
      {
        surfaceTextures = ReadR3tTextureBlobs(r3tPath, DefaultR3tSurfaceIdBase);
      }
      catch
      {
        surfaceTextures = Array.Empty<R3TextureBlob>();
      }
    }

    return new MaterialData(materials, materialSurfaceIds, materialAlphaTypes, surfaceTextures);
  }

  private static R3TextureBlob[] ReadLightmapTextures(string bspPath)
  {
    string? mapDirectory = Path.GetDirectoryName(bspPath);
    if (string.IsNullOrEmpty(mapDirectory))
    {
      return Array.Empty<R3TextureBlob>();
    }

    string mapName = Path.GetFileNameWithoutExtension(bspPath);
    string[] candidates =
    [
      Path.Combine(mapDirectory, $"{mapName}lgt.r3t"),
      Path.Combine(mapDirectory, $"{mapName}.lgt.r3t"),
      Path.Combine(mapDirectory, $"{mapName}_lgt.r3t"),
    ];

    for (int i = 0; i < candidates.Length; ++i)
    {
      string? resolved = FindPathCaseInsensitive(candidates[i]);
      if (resolved == null)
      {
        continue;
      }

      try
      {
        return ReadR3tTextureBlobs(resolved, LightmapR3tSurfaceIdBase);
      }
      catch
      {
        return Array.Empty<R3TextureBlob>();
      }
    }

    return Array.Empty<R3TextureBlob>();
  }

  private static MapEnvironmentSettings ReadMapEnvironmentSettings(string bspPath)
  {
    string? r3xPath = FindSiblingFileWithExtension(bspPath, ".r3x");
    if (r3xPath == null)
    {
      return MapEnvironmentSettings.Default;
    }

    try
    {
      using FileStream stream = File.OpenRead(r3xPath);
      using BinaryReader reader = new(stream);

      if (stream.Length < 8)
      {
        return MapEnvironmentSettings.Default;
      }

      float version = reader.ReadSingle();
      if (MathF.Abs(version - ExpectedR3xVersion) > 0.0001f)
      {
        return MapEnvironmentSettings.Default;
      }

      uint flags = reader.ReadUInt32();
      float fogStart = reader.ReadSingle();
      float fogEnd = reader.ReadSingle();
      uint fogColorArgb = reader.ReadUInt32();

      _ = reader.ReadSingle(); // FogStart2
      _ = reader.ReadSingle(); // FogEnd2
      _ = reader.ReadUInt32(); // FogColor2
      _ = reader.ReadSingle(); // Fog2 BBMin.X
      _ = reader.ReadSingle(); // Fog2 BBMin.Y
      _ = reader.ReadSingle(); // Fog2 BBMin.Z
      _ = reader.ReadSingle(); // Fog2 BBMax.X
      _ = reader.ReadSingle(); // Fog2 BBMax.Y
      _ = reader.ReadSingle(); // Fog2 BBMax.Z

      for (int i = 0; i < ExtMatLensFlareScaleCount; ++i)
      {
        _ = reader.ReadSingle();
      }

      _ = ReadExactBytes(reader, ExtMatNameBytes, "R3X lens texture name");
      float lensX = reader.ReadSingle();
      float lensY = reader.ReadSingle();
      float lensZ = reader.ReadSingle();
      _ = ReadExactBytes(reader, ExtMatNameBytes, "R3X environment entity name");
      _ = reader.ReadUInt32(); // EnvId

      bool fogEnabled = (flags & ExtMatExistFirstFog) != 0;
      if (fogEnabled)
      {
        if (!float.IsFinite(fogStart) || fogStart <= 0.0f)
        {
          fogStart = 5.0f;
        }

        if (!float.IsFinite(fogEnd) || fogEnd <= fogStart)
        {
          fogEnd = MathF.Max(5000.0f, fogStart + 1.0f);
        }
      }
      else
      {
        fogStart = 5.0f;
        fogEnd = 5000.0f;
      }

      return new MapEnvironmentSettings(
        fogEnabled,
        fogStart,
        fogEnd,
        DecodeRgbFromArgb(fogColorArgb),
        (flags & ExtMatFogRange) != 0,
        (flags & ExtMatNoFogSky) != 0,
        (flags & ExtMatExistLensFlare) != 0,
        new Vector3(lensX, lensY, lensZ));
    }
    catch
    {
      return MapEnvironmentSettings.Default;
    }
  }

  private static Vector3 DecodeRgbFromArgb(uint argb)
  {
    float r = ((argb >> 16) & 0xFF) / 255.0f;
    float g = ((argb >> 8) & 0xFF) / 255.0f;
    float b = (argb & 0xFF) / 255.0f;
    return new Vector3(r, g, b);
  }

  private static string? FindSiblingFileWithExtension(string path, string extension)
  {
    string directory = Path.GetDirectoryName(path) ?? string.Empty;
    string fileName = Path.GetFileNameWithoutExtension(path);

    string candidate = Path.Combine(directory, fileName + extension);
    if (File.Exists(candidate))
    {
      return candidate;
    }

    if (!Directory.Exists(directory))
    {
      return null;
    }

    foreach (string sibling in Directory.EnumerateFiles(directory))
    {
      if (!string.Equals(Path.GetExtension(sibling), extension, StringComparison.OrdinalIgnoreCase))
      {
        continue;
      }

      if (string.Equals(Path.GetFileNameWithoutExtension(sibling), fileName, StringComparison.OrdinalIgnoreCase))
      {
        return sibling;
      }
    }

    return null;
  }

  private static ExtDummyDefinition[] ReadExtDummyDefinitions(string bspPath, MapEnvironmentSettings environmentSettings)
  {
    string? extSptPath = FindExtSptPath(bspPath);
    if (extSptPath == null)
    {
      return Array.Empty<ExtDummyDefinition>();
    }

    try
    {
      string script = File.ReadAllText(extSptPath);
      return ParseExtDummyScript(script, environmentSettings);
    }
    catch
    {
      return Array.Empty<ExtDummyDefinition>();
    }
  }

  private static string? FindExtSptPath(string bspPath)
  {
    string directory = Path.GetDirectoryName(bspPath) ?? string.Empty;
    if (!Directory.Exists(directory))
    {
      return null;
    }

    string mapName = Path.GetFileNameWithoutExtension(bspPath);
    string candidate = Path.Combine(directory, $"{mapName}EXT.spt");
    string? resolved = FindPathCaseInsensitive(candidate);
    if (resolved != null)
    {
      return resolved;
    }

    string expectedFileName = $"{mapName}EXT";
    foreach (string file in Directory.EnumerateFiles(directory, "*.spt"))
    {
      if (string.Equals(Path.GetFileNameWithoutExtension(file), expectedFileName, StringComparison.OrdinalIgnoreCase))
      {
        return file;
      }
    }

    return null;
  }

  private static ExtDummyDefinition[] ParseExtDummyScript(string script, MapEnvironmentSettings environmentSettings)
  {
    string[] tokens = script.Split((char[]?)null, StringSplitOptions.RemoveEmptyEntries);
    if (tokens.Length == 0)
    {
      return Array.Empty<ExtDummyDefinition>();
    }

    int tokenIndex = 0;
    while (tokenIndex < tokens.Length
      && !string.Equals(tokens[tokenIndex], "script_begin", StringComparison.OrdinalIgnoreCase))
    {
      ++tokenIndex;
    }

    if (tokenIndex >= tokens.Length)
    {
      return Array.Empty<ExtDummyDefinition>();
    }

    ++tokenIndex;
    List<ExtDummyDefinition> dummies = new();
    string? pendingToken = null;
    bool scriptEnded = false;

    while (!scriptEnded)
    {
      string token;
      if (pendingToken != null)
      {
        token = pendingToken;
        pendingToken = null;
      }
      else
      {
        if (tokenIndex >= tokens.Length)
        {
          break;
        }

        token = tokens[tokenIndex++];
      }

      if (string.Equals(token, "script_end", StringComparison.OrdinalIgnoreCase))
      {
        break;
      }

      if (token.Length == 0 || token[0] != '*')
      {
        continue;
      }

      string name = token.Length > 1 ? token[1..] : string.Empty;
      if (!TryReadVector3Token(tokens, ref tokenIndex, out Vector3 localMin)
        || !TryReadVector3Token(tokens, ref tokenIndex, out Vector3 localMax))
      {
        break;
      }

      float defaultFogStart = environmentSettings.FogStart > 0.0f ? environmentSettings.FogStart : 5.0f;
      float defaultFogEnd = environmentSettings.FogEnd > defaultFogStart
        ? environmentSettings.FogEnd
        : MathF.Max(defaultFogStart + 1.0f, 5000.0f);

      uint flags = 0;
      uint id = 0;
      uint arg0 = 0;
      float arg1 = defaultFogStart;
      float arg2 = defaultFogEnd;
      NumericsMatrix4 transform = NumericsMatrix4.Identity;
      NumericsMatrix4 inverseTransform = NumericsMatrix4.Identity;

      while (tokenIndex < tokens.Length)
      {
        string command = tokens[tokenIndex++];
        if (string.Equals(command, "script_end", StringComparison.OrdinalIgnoreCase))
        {
          scriptEnded = true;
          break;
        }

        if (command.Length > 0 && command[0] == '*')
        {
          pendingToken = command;
          break;
        }

        if (string.Equals(command, "-node_tm", StringComparison.OrdinalIgnoreCase))
        {
          if (TryReadMatrix4Token(tokens, ref tokenIndex, out NumericsMatrix4 parsed))
          {
            transform = parsed;
            if (!NumericsMatrix4.Invert(transform, out inverseTransform))
            {
              inverseTransform = NumericsMatrix4.Identity;
            }
          }

          continue;
        }

        if (string.Equals(command, "-id", StringComparison.OrdinalIgnoreCase))
        {
          if (TryReadIntToken(tokens, ref tokenIndex, out int parsedId) && parsedId >= 0)
          {
            id = (uint)parsedId;
          }

          continue;
        }

        if (string.Equals(command, "-music", StringComparison.OrdinalIgnoreCase))
        {
          flags |= ExtDummyFlagMusic;
          continue;
        }

        if (string.Equals(command, "-fog_color", StringComparison.OrdinalIgnoreCase))
        {
          flags |= ExtDummyFlagFog;
          int r = ReadIntTokenOrDefault(tokens, ref tokenIndex);
          int g = ReadIntTokenOrDefault(tokens, ref tokenIndex);
          int b = ReadIntTokenOrDefault(tokens, ref tokenIndex);
          arg0 = 0xFF000000u
            | ((uint)Math.Clamp(r, 0, 255) << 16)
            | ((uint)Math.Clamp(g, 0, 255) << 8)
            | (uint)Math.Clamp(b, 0, 255);
          arg1 = defaultFogStart;
          arg2 = defaultFogEnd;
          continue;
        }

        if (string.Equals(command, "-fog_start", StringComparison.OrdinalIgnoreCase))
        {
          if (TryReadFloatToken(tokens, ref tokenIndex, out float parsedFogStart))
          {
            arg1 = parsedFogStart;
          }

          continue;
        }

        if (string.Equals(command, "-fog_end", StringComparison.OrdinalIgnoreCase))
        {
          if (TryReadFloatToken(tokens, ref tokenIndex, out float parsedFogEnd))
          {
            arg2 = parsedFogEnd;
          }
        }
      }

      dummies.Add(
        new ExtDummyDefinition(
          name,
          flags,
          localMin,
          localMax,
          id,
          arg0,
          arg1,
          arg2,
          transform,
          inverseTransform));
    }

    return dummies.ToArray();
  }

  private static bool TryReadVector3Token(string[] tokens, ref int tokenIndex, out Vector3 value)
  {
    value = Vector3.Zero;
    if (!TryReadFloatToken(tokens, ref tokenIndex, out float x)
      || !TryReadFloatToken(tokens, ref tokenIndex, out float y)
      || !TryReadFloatToken(tokens, ref tokenIndex, out float z))
    {
      return false;
    }

    value = new Vector3(x, y, z);
    return true;
  }

  private static bool TryReadMatrix4Token(string[] tokens, ref int tokenIndex, out NumericsMatrix4 matrix)
  {
    matrix = NumericsMatrix4.Identity;
    Span<float> values = stackalloc float[16];
    for (int i = 0; i < values.Length; ++i)
    {
      if (!TryReadFloatToken(tokens, ref tokenIndex, out values[i]))
      {
        return false;
      }
    }

    matrix = new NumericsMatrix4(
      values[0], values[1], values[2], values[3],
      values[4], values[5], values[6], values[7],
      values[8], values[9], values[10], values[11],
      values[12], values[13], values[14], values[15]);
    return true;
  }

  private static bool TryReadFloatToken(string[] tokens, ref int tokenIndex, out float value)
  {
    value = 0.0f;
    if (tokenIndex >= tokens.Length)
    {
      return false;
    }

    string token = tokens[tokenIndex++];
    return float.TryParse(token, NumberStyles.Float, CultureInfo.InvariantCulture, out value)
      || float.TryParse(token, NumberStyles.Float, CultureInfo.CurrentCulture, out value);
  }

  private static bool TryReadIntToken(string[] tokens, ref int tokenIndex, out int value)
  {
    value = 0;
    if (tokenIndex >= tokens.Length)
    {
      return false;
    }

    string token = tokens[tokenIndex++];
    return int.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out value)
      || int.TryParse(token, NumberStyles.Integer, CultureInfo.CurrentCulture, out value);
  }

  private static int ReadIntTokenOrDefault(string[] tokens, ref int tokenIndex, int fallback = 0)
  {
    return TryReadIntToken(tokens, ref tokenIndex, out int value) ? value : fallback;
  }

  private static R3MaterialData ReadR3mMaterials(string r3mPath)
  {
    return ReadR3mMaterials(File.ReadAllBytes(r3mPath));
  }

  private static R3MaterialData ReadR3mMaterials(byte[] r3mBytes)
  {
    using MemoryStream stream = new(r3mBytes, writable: false);
    using BinaryReader reader = new(stream);

    if (stream.Length < 8)
    {
      throw new InvalidDataException("R3M file is too small.");
    }

    _ = reader.ReadSingle(); // version
    int materialCount = reader.ReadInt32();
    if (materialCount <= 0 || materialCount > 100000)
    {
      throw new InvalidDataException("R3M material count is invalid.");
    }

    string[] names = new string[materialCount];
    int[] surfaceIds = new int[materialCount];
    MaterialDefinition[] materials = new MaterialDefinition[materialCount];
    uint[] materialFlags = new uint[materialCount];
    uint[] layerAlphaTypes = new uint[materialCount];
    uint[] layerArgb = new uint[materialCount];
    Array.Fill(surfaceIds, -1);

    for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
    {
      if (stream.Position + 144 > stream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading R3M material header.");
      }

      uint layerCount = reader.ReadUInt32();
      uint materialFlag = reader.ReadUInt32();
      int detailSurface = reader.ReadInt32(); // m_iDetailSurface
      float detailScale = reader.ReadSingle(); // m_fDetailScale
      byte[] nameBytes = ReadExactBytes(reader, 128, "R3M material name");
      names[materialIndex] = DecodeAscii(nameBytes);
      materialFlags[materialIndex] = materialFlag;

      uint firstLayerAlphaType = 0;
      uint firstLayerArgb = 0;
      List<MaterialLayerDefinition> layers = new(checked((int)layerCount));

      for (uint layerIndex = 0; layerIndex < layerCount; ++layerIndex)
      {
        if (stream.Position + 46 > stream.Length)
        {
          throw new EndOfStreamException("Unexpected EOF while reading R3M layer block.");
        }

        short tileAniTextureCount = reader.ReadInt16(); // m_iTileAniTexNum
        int surfaceId = reader.ReadInt32(); // m_iSurface
        uint alphaType = reader.ReadUInt32(); // m_dwAlphaType
        uint argb = reader.ReadUInt32(); // m_ARGB
        uint layerFlag = reader.ReadUInt32(); // m_dwFlag
        short uvLavaWave = reader.ReadInt16(); // m_sUVLavaWave
        short uvLavaSpeed = reader.ReadInt16(); // m_sUVLavaSpeed
        short uvScrollU = reader.ReadInt16(); // m_sUVScrollU
        short uvScrollV = reader.ReadInt16(); // m_sUVScrollV
        short uvRotate = reader.ReadInt16(); // m_sUVRotate
        short uvScaleStart = reader.ReadInt16(); // m_sUVScaleStart
        short uvScaleEnd = reader.ReadInt16(); // m_sUVScaleEnd
        short uvScaleSpeed = reader.ReadInt16(); // m_sUVScaleSpeed
        short uvMetal = reader.ReadInt16(); // m_sUVMetal
        short aniAlphaFlicker = reader.ReadInt16(); // m_sANIAlphaFlicker
        ushort aniAlphaFlickerRange = reader.ReadUInt16(); // m_sANIAlphaFlickerAni
        short aniTexFrame = reader.ReadInt16(); // m_sANITexFrame
        short aniTexSpeed = reader.ReadInt16(); // m_sANITexSpeed
        short gradientAlpha = reader.ReadInt16(); // m_sGradientAlpha
        layers.Add(
          new MaterialLayerDefinition(
            tileAniTextureCount,
            surfaceId,
            alphaType,
            argb,
            layerFlag,
            uvLavaWave,
            uvLavaSpeed,
            uvScrollU,
            uvScrollV,
            uvRotate,
            uvScaleStart,
            uvScaleEnd,
            uvScaleSpeed,
            uvMetal,
            aniAlphaFlicker,
            aniAlphaFlickerRange,
            aniTexFrame,
            aniTexSpeed,
            gradientAlpha));

        if (layerIndex == 0)
        {
          surfaceIds[materialIndex] = surfaceId;
          firstLayerAlphaType = alphaType;
          firstLayerArgb = argb;
        }
      }

      layerAlphaTypes[materialIndex] = firstLayerAlphaType;
      layerArgb[materialIndex] = firstLayerArgb;
      materials[materialIndex] = new MaterialDefinition(
        materialFlag,
        detailSurface,
        detailScale,
        layers.ToArray());
    }

    return new R3MaterialData(names, materials, surfaceIds, materialFlags, layerAlphaTypes, layerArgb);
  }

  private static R3TextureBlob[] ReadR3tTextureBlobs(string r3tPath)
  {
    return ReadR3tTextureBlobs(File.ReadAllBytes(r3tPath), DefaultR3tSurfaceIdBase);
  }

  private static R3TextureBlob[] ReadR3tTextureBlobs(string r3tPath, int surfaceIdBase)
  {
    return ReadR3tTextureBlobs(File.ReadAllBytes(r3tPath), surfaceIdBase);
  }

  private static R3TextureBlob[] ReadR3tTextureBlobs(byte[] r3tBytes, int surfaceIdBase)
  {
    using MemoryStream stream = new(r3tBytes, writable: false);
    using BinaryReader reader = new(stream);

    if (stream.Length < 8)
    {
      throw new InvalidDataException("R3T file is too small.");
    }

    float version = reader.ReadSingle();
    if (version < 1.1f)
    {
      return Array.Empty<R3TextureBlob>();
    }

    uint textureCountRaw = reader.ReadUInt32();
    int textureCount = checked((int)textureCountRaw);
    if (textureCount <= 0)
    {
      return Array.Empty<R3TextureBlob>();
    }

    string[] names = new string[textureCount];
    for (int i = 0; i < textureCount; ++i)
    {
      byte[] nameBytes = ReadExactBytes(reader, 128, "R3T texture name");
      names[i] = DecodeAscii(nameBytes);
    }

    List<R3TextureBlob> textures = new(textureCount);
    for (int i = 0; i < textureCount; ++i)
    {
      if (stream.Position + 4 > stream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading R3T texture size.");
      }

      int textureByteSize = reader.ReadInt32();
      if (textureByteSize <= 0 || stream.Position + textureByteSize > stream.Length)
      {
        throw new InvalidDataException("R3T texture block size is invalid.");
      }

      byte[] textureBytes = ReadExactBytes(reader, textureByteSize, "R3T DDS texture block");
      byte[] ddsBytes = DecodeR3tTextureBytes(textureBytes);
      textures.Add(new R3TextureBlob(surfaceIdBase + i, names[i], ddsBytes));
    }

    return textures.ToArray();
  }

  private static string DecodeAscii(byte[] bytes)
  {
    int zeroIndex = Array.IndexOf(bytes, (byte)0);
    int length = zeroIndex >= 0 ? zeroIndex : bytes.Length;
    return Encoding.ASCII.GetString(bytes, 0, length);
  }

  private static byte[] DecodeR3tTextureBytes(byte[] source)
  {
    if (source.Length < 128)
    {
      return source;
    }

    if (IsDds(source))
    {
      return source;
    }

    byte[] decoded = new byte[source.Length];
    Buffer.BlockCopy(source, 0, decoded, 0, source.Length);

    for (int index = 0; index < 32; ++index)
    {
      int offset = index * 4;
      uint value = BinaryPrimitives.ReadUInt32LittleEndian(decoded.AsSpan(offset, 4));
      value ^= R3tHeaderXorKey[index];
      BinaryPrimitives.WriteUInt32LittleEndian(decoded.AsSpan(offset, 4), value);
    }

    return IsDds(decoded) ? decoded : source;
  }

  private static bool IsDds(byte[] bytes)
  {
    return bytes.Length >= 4
      && bytes[0] == (byte)'D'
      && bytes[1] == (byte)'D'
      && bytes[2] == (byte)'S'
      && bytes[3] == (byte)' ';
  }

  private static MeshBuildResult BuildTriangleVertices(
    byte[] packedVertexData,
    int bVertexByteSize,
    int wVertexByteSize,
    int bVertexCount,
    int wVertexCount,
    int fVertexCount,
    ReadFace[] faces,
    uint[] faceIds,
    uint[] vertexIds,
    ReadMatGroup[] matGroups,
    float[] uvData,
    short[] lgtUvData,
    uint[] vertexColor,
    MaterialDefinition[] materials)
  {
    int totalVertexCount = bVertexCount + wVertexCount + fVertexCount;
    if (totalVertexCount <= 0 || matGroups.Length == 0)
    {
      return new MeshBuildResult(Array.Empty<BspRenderVertex>(), Array.Empty<BspMaterialSpan>());
    }

    Vector3[] decompressed = new Vector3[totalVertexCount];
    bool[] written = new bool[totalVertexCount];
    List<BspRenderVertex> triangles = new(Math.Max(1024, faceIds.Length * 6));
    List<BspMaterialSpan> spans = new(Math.Max(32, matGroups.Length));

    int baseCharIndex = bVertexCount;
    int bVertexOffset = 0;
    int wVertexOffset = bVertexByteSize;
    int fVertexOffset = bVertexByteSize + wVertexByteSize;

    for (int groupIndex = 0; groupIndex < matGroups.Length; ++groupIndex)
    {
      ReadMatGroup group = matGroups[groupIndex];
      int functionId = (group.Attribute & 0x8000) != 0 ? 1 : (group.Attribute & 0x4000) != 0 ? 2 : 4;
      int baseIndex = functionId == 2 ? baseCharIndex : 0;
      int groupStartVertex = triangles.Count;

      for (int faceInGroup = 0; faceInGroup < group.FaceCount; ++faceInGroup)
      {
        int faceIdIndex = (int)group.FaceStartId + faceInGroup;
        if (faceIdIndex < 0 || faceIdIndex >= faceIds.Length)
        {
          continue;
        }

        int faceIndex = checked((int)faceIds[faceIdIndex]);
        if (faceIndex < 0 || faceIndex >= faces.Length)
        {
          continue;
        }

        ReadFace face = faces[faceIndex];
        if (face.VertexCount < 3)
        {
          continue;
        }

        int[] faceVertexIds = new int[face.VertexCount];
        int[] faceUvIds = new int[face.VertexCount];
        int[] faceColorIds = new int[face.VertexCount];

        bool faceValid = true;
        for (int vertexOffset = 0; vertexOffset < face.VertexCount; ++vertexOffset)
        {
          int vertexIdIndex = checked((int)face.VertexStartId + vertexOffset);
          if (vertexIdIndex < 0 || vertexIdIndex >= vertexIds.Length)
          {
            faceValid = false;
            break;
          }

          int sourceIndex = checked((int)vertexIds[vertexIdIndex]);
          int decompressedIndex = baseIndex + sourceIndex;
          if (decompressedIndex < 0 || decompressedIndex >= decompressed.Length)
          {
            faceValid = false;
            break;
          }

          if (!written[decompressedIndex])
          {
            decompressed[decompressedIndex] = DecodeVertex(
              functionId,
              sourceIndex,
              packedVertexData,
              bVertexOffset,
              wVertexOffset,
              fVertexOffset,
              group);
            written[decompressedIndex] = true;
          }

          faceVertexIds[vertexOffset] = decompressedIndex;
          faceUvIds[vertexOffset] = vertexIdIndex;
          faceColorIds[vertexOffset] = sourceIndex;
        }

        if (!faceValid)
        {
          continue;
        }

        int root = faceVertexIds[0];
        for (int tri = 2; tri < faceVertexIds.Length; ++tri)
        {
          triangles.Add(CreateRenderVertex(
            decompressed[root],
            faceUvIds[0],
            faceColorIds[0],
            group.MaterialId,
            group.LightMapId,
            uvData,
            lgtUvData,
            vertexColor,
            materials));
          triangles.Add(CreateRenderVertex(
            decompressed[faceVertexIds[tri - 1]],
            faceUvIds[tri - 1],
            faceColorIds[tri - 1],
            group.MaterialId,
            group.LightMapId,
            uvData,
            lgtUvData,
            vertexColor,
            materials));
          triangles.Add(CreateRenderVertex(
            decompressed[faceVertexIds[tri]],
            faceUvIds[tri],
            faceColorIds[tri],
            group.MaterialId,
            group.LightMapId,
            uvData,
            lgtUvData,
            vertexColor,
            materials));
        }
      }

      int groupVertexCount = triangles.Count - groupStartVertex;
      if (groupVertexCount > 0)
      {
        if (spans.Count > 0)
        {
          BspMaterialSpan last = spans[spans.Count - 1];
          if (last.MaterialId == group.MaterialId
            && last.LightMapId == group.LightMapId
            && last.StartVertex + last.VertexCount == groupStartVertex)
          {
            spans[spans.Count - 1] = new BspMaterialSpan(
              last.MaterialId,
              last.StartVertex,
              last.VertexCount + groupVertexCount,
              last.LightMapId);
          }
          else
          {
            spans.Add(new BspMaterialSpan(group.MaterialId, groupStartVertex, groupVertexCount, group.LightMapId));
          }
        }
        else
        {
          spans.Add(new BspMaterialSpan(group.MaterialId, groupStartVertex, groupVertexCount, group.LightMapId));
        }
      }
    }

    return new MeshBuildResult(triangles.ToArray(), spans.ToArray());
  }

  private static BspRenderVertex CreateRenderVertex(
    Vector3 position,
    int uvId,
    int colorId,
    int materialId,
    int lightMapId,
    float[] uvData,
    short[] lgtUvData,
    uint[] vertexColor,
    MaterialDefinition[] materials)
  {
    Vector2 uv = DecodeUv(uvData, uvId);
    Vector2 lightUv = DecodeLightUv(lgtUvData, uvId);
    Vector3 color = DecodeColor(vertexColor, colorId, materialId, lightMapId, materials);
    if (materialId >= 0
      && materialId < materials.Length
      && materials[materialId].DetailSurfaceId >= 0
      && (materials[materialId].MaterialFlags & MaterialFlagLightMap) != 0
      && lightMapId == -1)
    {
      float detailScale = float.IsFinite(materials[materialId].DetailScale)
        ? materials[materialId].DetailScale
        : 1.0f;
      lightUv = uv * detailScale * 8.0f;
    }

    return new BspRenderVertex(position, uv, lightUv, color);
  }

  private static Vector2 DecodeUv(float[] uvData, int uvId)
  {
    int uvOffset = uvId * 2;
    if (uvOffset < 0 || uvOffset + 1 >= uvData.Length)
    {
      return Vector2.Zero;
    }

    float u = uvData[uvOffset];
    float v = uvData[uvOffset + 1];
    if (!float.IsFinite(u) || !float.IsFinite(v))
    {
      return Vector2.Zero;
    }

    return new Vector2(u, v);
  }

  private static Vector2 DecodeLightUv(short[] lgtUvData, int uvId)
  {
    int uvOffset = uvId * 2;
    if (uvOffset < 0 || uvOffset + 1 >= lgtUvData.Length)
    {
      return Vector2.Zero;
    }

    float u = lgtUvData[uvOffset] / 32767.0f;
    float v = lgtUvData[uvOffset + 1] / 32767.0f;
    if (!float.IsFinite(u) || !float.IsFinite(v))
    {
      return Vector2.Zero;
    }

    return new Vector2(u, v);
  }

  private static Vector3 DecodeColor(
    uint[] vertexColor,
    int colorId,
    int materialId,
    int lightMapId,
    MaterialDefinition[] materials)
  {
    uint materialArgb = 0xFFFFFFFF;
    if (materialId >= 0
      && materialId < materials.Length
      && materials[materialId].Layers.Length > 0)
    {
      materialArgb = materials[materialId].Layers[0].Argb;
    }

    if (lightMapId != -1)
    {
      float rr = ((materialArgb >> 16) & 0xFF) / 255.0f;
      float gg = ((materialArgb >> 8) & 0xFF) / 255.0f;
      float bb = (materialArgb & 0xFF) / 255.0f;
      return new Vector3(rr, gg, bb);
    }

    if (colorId < 0 || colorId >= vertexColor.Length)
    {
      return new Vector3(0.7f, 0.75f, 0.8f);
    }

    uint argb = vertexColor[colorId];
    float r = ((argb >> 16) & 0xFF) / 255.0f;
    float g = ((argb >> 8) & 0xFF) / 255.0f;
    float b = (argb & 0xFF) / 255.0f;
    return new Vector3(r, g, b);
  }

  private static Vector3 DecodeVertex(
    int functionId,
    int sourceIndex,
    byte[] packedVertexData,
    int bVertexOffset,
    int wVertexOffset,
    int fVertexOffset,
    ReadMatGroup group)
  {
    return functionId switch
    {
      1 => DecodeByteVertex(sourceIndex, packedVertexData, bVertexOffset, group),
      2 => DecodeWordVertex(sourceIndex, packedVertexData, wVertexOffset, group),
      _ => DecodeFloatVertex(sourceIndex, packedVertexData, fVertexOffset),
    };
  }

  private static Vector3 DecodeByteVertex(int sourceIndex, byte[] data, int baseOffset, ReadMatGroup group)
  {
    int offset = baseOffset + sourceIndex * 3;
    if (offset < 0 || offset + 2 >= data.Length)
    {
      return Vector3.Zero;
    }

    float x = ((sbyte)data[offset] / 127.0f) * group.Scale + group.Position.X;
    float y = ((sbyte)data[offset + 1] / 127.0f) * group.Scale + group.Position.Y;
    float z = ((sbyte)data[offset + 2] / 127.0f) * group.Scale + group.Position.Z;
    return new Vector3(x, y, z);
  }

  private static Vector3 DecodeWordVertex(int sourceIndex, byte[] data, int baseOffset, ReadMatGroup group)
  {
    int offset = baseOffset + sourceIndex * 6;
    if (offset < 0 || offset + 5 >= data.Length)
    {
      return Vector3.Zero;
    }

    short xRaw = BinaryPrimitives.ReadInt16LittleEndian(data.AsSpan(offset, 2));
    short yRaw = BinaryPrimitives.ReadInt16LittleEndian(data.AsSpan(offset + 2, 2));
    short zRaw = BinaryPrimitives.ReadInt16LittleEndian(data.AsSpan(offset + 4, 2));
    float x = (xRaw / 32767.0f) * group.Scale + group.Position.X;
    float y = (yRaw / 32767.0f) * group.Scale + group.Position.Y;
    float z = (zRaw / 32767.0f) * group.Scale + group.Position.Z;
    return new Vector3(x, y, z);
  }

  private static Vector3 DecodeFloatVertex(int sourceIndex, byte[] data, int baseOffset)
  {
    int offset = baseOffset + sourceIndex * 12;
    if (offset < 0 || offset + 11 >= data.Length)
    {
      return Vector3.Zero;
    }

    float x = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(data.AsSpan(offset, 4)));
    float y = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(data.AsSpan(offset + 4, 4)));
    float z = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(data.AsSpan(offset + 8, 4)));
    return new Vector3(x, y, z);
  }

  private static BspHeader ReadBspHeader(BinaryReader reader)
  {
    uint version = reader.ReadUInt32();
    BspEntry[] entries = new BspEntry[85];
    for (int index = 0; index < entries.Length; ++index)
    {
      entries[index] = ReadEntry(reader);
    }

    uint readSpareTotal = 0;
    // Header layout matches current CBsp.h:
    // 0..6 fixed sections, 7 EventObjectID, 8..42 ReadSpare[35].
    for (int index = 8; index <= 42; ++index)
    {
      readSpareTotal += entries[index].Size;
    }

    return new BspHeader(
      version,
      entries[0],
      entries[1],
      entries[2],
      entries[3],
      entries[4],
      entries[5],
      entries[6],
      entries[7],
      readSpareTotal,
      entries[43],
      entries[44],
      entries[45],
      entries[46],
      entries[47],
      entries[48],
      entries[49],
      entries[50],
      entries[51],
      entries[52]);
  }

  private static ExtBspHeader ReadExtBspHeader(BinaryReader reader)
  {
    uint version = reader.ReadUInt32();
    BspEntry[] entries = new BspEntry[48];
    for (int index = 0; index < entries.Length; ++index)
    {
      entries[index] = ReadEntry(reader);
    }

    uint readSpareTotal = 0;
    for (int index = 9; index <= 26; ++index)
    {
      readSpareTotal += entries[index].Size;
    }

    return new ExtBspHeader(
      version,
      entries[0],
      entries[1],
      entries[2],
      entries[3],
      entries[4],
      entries[5],
      entries[6],
      entries[7],
      entries[8],
      readSpareTotal,
      entries[27],
      entries[28],
      entries[29]);
  }

  private static BspEntry ReadEntry(BinaryReader reader)
  {
    uint offset = reader.ReadUInt32();
    uint size = reader.ReadUInt32();
    return new BspEntry(offset, size);
  }

  private static byte[] ReadExactBytes(BinaryReader reader, int size, string sectionName)
  {
    byte[] data = reader.ReadBytes(size);
    if (data.Length != size)
    {
      throw new EndOfStreamException($"Unexpected EOF while reading {sectionName}.");
    }
    return data;
  }

  private static short[] ReadInt16Array(BinaryReader reader, int count, string sectionName)
  {
    short[] data = new short[count];
    for (int i = 0; i < count; ++i)
    {
      if (reader.BaseStream.Position + 2 > reader.BaseStream.Length)
      {
        throw new EndOfStreamException($"Unexpected EOF while reading {sectionName}.");
      }
      data[i] = reader.ReadInt16();
    }
    return data;
  }

  private static float[] ReadSingleArray(BinaryReader reader, int count, string sectionName)
  {
    float[] data = new float[count];
    for (int i = 0; i < count; ++i)
    {
      if (reader.BaseStream.Position + 4 > reader.BaseStream.Length)
      {
        throw new EndOfStreamException($"Unexpected EOF while reading {sectionName}.");
      }
      data[i] = reader.ReadSingle();
    }
    return data;
  }

  private static uint[] ReadUInt32Array(BinaryReader reader, int count, string sectionName)
  {
    uint[] data = new uint[count];
    for (int i = 0; i < count; ++i)
    {
      if (reader.BaseStream.Position + 4 > reader.BaseStream.Length)
      {
        throw new EndOfStreamException($"Unexpected EOF while reading {sectionName}.");
      }
      data[i] = reader.ReadUInt32();
    }
    return data;
  }

  private static ReadFace[] ReadFaces(BinaryReader reader, int count)
  {
    ReadFace[] faces = new ReadFace[count];
    for (int i = 0; i < count; ++i)
    {
      if (reader.BaseStream.Position + 6 > reader.BaseStream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading Face section.");
      }

      ushort vertexCount = reader.ReadUInt16();
      uint vertexStartId = reader.ReadUInt32();
      faces[i] = new ReadFace(vertexCount, vertexStartId);
    }
    return faces;
  }

  private static ReadMatGroup[] ReadMatGroups(BinaryReader reader, int count)
  {
    ReadMatGroup[] groups = new ReadMatGroup[count];

    for (int i = 0; i < count; ++i)
    {
      if (reader.BaseStream.Position + 42 > reader.BaseStream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading ReadMatGroup section.");
      }

      ushort attr = reader.ReadUInt16();
      ushort faceCount = reader.ReadUInt16();
      uint faceStartId = reader.ReadUInt32();
      short mtlId = reader.ReadInt16();
      short lgtId = reader.ReadInt16();
      _ = reader.ReadInt16(); // bb_min[0]
      _ = reader.ReadInt16(); // bb_min[1]
      _ = reader.ReadInt16(); // bb_min[2]
      _ = reader.ReadInt16(); // bb_max[0]
      _ = reader.ReadInt16(); // bb_max[1]
      _ = reader.ReadInt16(); // bb_max[2]
      float posX = reader.ReadSingle();
      float posY = reader.ReadSingle();
      float posZ = reader.ReadSingle();
      float scale = reader.ReadSingle();
      _ = reader.ReadUInt16(); // object_id

      groups[i] = new ReadMatGroup(attr, faceCount, faceStartId, mtlId, lgtId, new Vector3(posX, posY, posZ), scale);
    }

    return groups;
  }

  private static void SkipSection(Stream stream, uint size, string name)
  {
    if (size == 0)
    {
      return;
    }

    long next = stream.Position + size;
    if (next > stream.Length)
    {
      throw new EndOfStreamException($"Unexpected EOF while skipping {name} section.");
    }

    stream.Position = next;
  }

  private static Vector3[] ReadCollisionVerticesSequential(BinaryReader reader, int count)
  {
    Vector3[] vertices = new Vector3[count];

    for (int index = 0; index < count; ++index)
    {
      float x = reader.ReadSingle();
      float y = reader.ReadSingle();
      float z = reader.ReadSingle();
      vertices[index] = new Vector3(x, y, z);
    }

    return vertices;
  }

  private static CollisionLine[] ReadCollisionLinesSequential(BinaryReader reader, int count)
  {
    CollisionLine[] lines = new CollisionLine[count];

    for (int index = 0; index < count; ++index)
    {
      uint attribute = reader.ReadUInt32();
      ushort start = reader.ReadUInt16();
      ushort end = reader.ReadUInt16();
      float height = reader.ReadSingle();
      ushort front = reader.ReadUInt16();
      ushort back = reader.ReadUInt16();
      lines[index] = new CollisionLine(attribute, start, end, height, front, back);
    }

    return lines;
  }

  private static ExtEntityDefinition[] ReadEntityDefinitions(BinaryReader reader, int count)
  {
    ExtEntityDefinition[] definitions = new ExtEntityDefinition[count];

    for (int index = 0; index < count; ++index)
    {
      if (reader.BaseStream.Position + EntityListEntrySize > reader.BaseStream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading EntityList section.");
      }

      byte isParticle = reader.ReadByte();
      _ = reader.ReadByte(); // IsFileExist
      string name = DecodeAscii(ReadExactBytes(reader, 62, "EntityList.Name"));
      _ = reader.ReadSingle(); // FadeStart
      _ = reader.ReadSingle(); // FadeEnd
      _ = reader.ReadUInt16(); // Flag
      _ = reader.ReadUInt16(); // ShaderID
      _ = reader.ReadSingle(); // Factor[0]
      _ = reader.ReadSingle(); // Factor[1]

      definitions[index] = new ExtEntityDefinition(isParticle != 0, name);
    }

    return definitions;
  }

  private static ExtMapEntity[] ReadMapEntities(BinaryReader reader, int count)
  {
    ExtMapEntity[] entities = new ExtMapEntity[count];

    for (int index = 0; index < count; ++index)
    {
      if (reader.BaseStream.Position + ReadMapEntityEntrySize > reader.BaseStream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading MapEntitiesList section.");
      }

      ushort id = reader.ReadUInt16();
      float scale = reader.ReadSingle();
      float px = reader.ReadSingle();
      float py = reader.ReadSingle();
      float pz = reader.ReadSingle();
      float rotX = reader.ReadSingle();
      float rotY = reader.ReadSingle();
      _ = reader.ReadInt16(); // bbmin[0]
      _ = reader.ReadInt16(); // bbmin[1]
      _ = reader.ReadInt16(); // bbmin[2]
      _ = reader.ReadInt16(); // bbmax[0]
      _ = reader.ReadInt16(); // bbmax[1]
      _ = reader.ReadInt16(); // bbmax[2]

      entities[index] = new ExtMapEntity(id, scale, new Vector3(px, py, pz), rotX, rotY);
    }

    return entities;
  }

  private static SkyData ReadSkyData(string bspPath, SkySourceMode skySourceMode)
  {
    string[] skyPaths = ResolveSkyPaths(bspPath, skySourceMode);
    if (skyPaths.Length == 0)
    {
      return SkyData.Empty;
    }

    List<EntityMeshData> meshes = new(skyPaths.Length);
    for (int i = 0; i < skyPaths.Length; ++i)
    {
      try
      {
        meshes.Add(LoadEntityMesh(skyPaths[i], true));
      }
      catch
      {
      }
    }

    if (meshes.Count == 0)
    {
      return SkyData.Empty;
    }

    return new SkyData(MergeEntityMeshes(meshes));
  }

  private static EntityRenderData ReadEntityRenderData(
    string bspPath,
    ExtEntityDefinition[] entityDefinitions,
    ExtMapEntity[] mapEntities)
  {
    if (entityDefinitions.Length == 0 || mapEntities.Length == 0)
    {
      return EntityRenderData.Empty;
    }

    List<Vector3> particlePositions = new();
    for (int index = 0; index < mapEntities.Length; ++index)
    {
      ExtMapEntity instance = mapEntities[index];
      int entityId = instance.EntityId;
      if (entityId < 0 || entityId >= entityDefinitions.Length)
      {
        continue;
      }

      if (entityDefinitions[entityId].IsParticle && IsFinite(instance.Position))
      {
        particlePositions.Add(instance.Position);
      }
    }

    string? entityRoot = TryResolveEntityRootPath(bspPath);
    if (entityRoot == null)
    {
      return new EntityRenderData(RenderMeshData.Empty, EntitySceneData.Empty, 0, 0, particlePositions.ToArray());
    }

    RpkArchiveManager rpkManager = GetOrCreateEntityArchiveManager(entityRoot, out _);
    Dictionary<int, EntityMeshData> modelsById = new();
    HashSet<int> missingModelIds = new();
    int instanceCount = 0;

    for (int index = 0; index < mapEntities.Length; ++index)
    {
      ExtMapEntity instance = mapEntities[index];
      int entityId = instance.EntityId;
      if (entityId < 0 || entityId >= entityDefinitions.Length)
      {
        continue;
      }

      if (missingModelIds.Contains(entityId))
      {
        continue;
      }

      ExtEntityDefinition definition = entityDefinitions[entityId];
      if (!modelsById.ContainsKey(entityId))
      {
        bool loaded = false;
        try
        {
          EntityMeshData model = null!;
          if (definition.IsParticle)
          {
            loaded = TryLoadParticleEntityMesh(entityRoot, definition.Name, rpkManager, out model);
          }
          else
          {
            string? modelPath = TryResolveEntityModelPath(entityRoot, definition.Name);
            string? virtualPath = NormalizeArchiveAssetPath(definition.Name, ".r3e");
            if (modelPath != null)
            {
              model = LoadEntityMesh(modelPath, false);
              loaded = true;
            }
            else if (virtualPath != null)
            {
              model = LoadEntityMeshFromArchive(virtualPath, rpkManager, false);
              loaded = true;
            }
            else
            {
              loaded = false;
            }
          }

          if (loaded)
          {
            modelsById[entityId] = model;
          }
          else
          {
            missingModelIds.Add(entityId);
            continue;
          }
        }
        catch
        {
          missingModelIds.Add(entityId);
          continue;
        }
      }

      ++instanceCount;
    }

    if (modelsById.Count == 0 || instanceCount == 0)
    {
      return new EntityRenderData(RenderMeshData.Empty, EntitySceneData.Empty, 0, 0, particlePositions.ToArray());
    }

    RenderMeshData combined = BuildCombinedEntityMesh(modelsById, mapEntities);
    EntitySceneData sceneData = BuildEntitySceneData(modelsById, mapEntities);
    return new EntityRenderData(combined, sceneData, modelsById.Count, instanceCount, particlePositions.ToArray());
  }

  private static bool TryLoadParticleEntityMesh(
    string entityRoot,
    string particleDefinitionPath,
    RpkArchiveManager rpkManager,
    out EntityMeshData model)
  {
    model = null!;
    if (!TryReadParticleScriptText(entityRoot, particleDefinitionPath, rpkManager, out string scriptText)
      || string.IsNullOrWhiteSpace(scriptText))
    {
      return false;
    }

    ParticleScriptData script = ParseParticleScript(scriptText);
    string[] references = script.EntityFileReferences;
    uint particleAlphaType = script.AlphaType;
    if (script.DisableDepthTest)
    {
      particleAlphaType |= ParticleAlphaFlagDisableDepthTest;
    }

    List<EntityMeshData> loadedModels = new();
    for (int i = 0; i < references.Length; ++i)
    {
      string reference = references[i];
      string? modelPath = TryResolveEntityModelPath(entityRoot, reference);
      string? archivePath = NormalizeArchiveAssetPath(reference, ".r3e");

      try
      {
        if (modelPath != null)
        {
          EntityMeshData loadedModel = LoadEntityMesh(modelPath, false);
          loadedModels.Add(ApplyParticleScriptOverrides(loadedModel, particleAlphaType));
          continue;
        }

        if (archivePath != null)
        {
          EntityMeshData loadedModel = LoadEntityMeshFromArchive(archivePath, rpkManager, false);
          loadedModels.Add(ApplyParticleScriptOverrides(loadedModel, particleAlphaType));
          continue;
        }
      }
      catch
      {
      }
    }

    if (loadedModels.Count == 0)
    {
      return false;
    }

    if (loadedModels.Count == 1)
    {
      model = loadedModels[0];
      return true;
    }

    RenderMeshData merged = MergeEntityMeshes(loadedModels);
    model = new EntityMeshData(
      Path.GetFileNameWithoutExtension(particleDefinitionPath),
      merged.Vertices,
      merged.MaterialSpans,
      merged.Materials,
      merged.MaterialSurfaceIds,
      merged.MaterialAlphaTypes,
      merged.SurfaceTextures,
      Array.Empty<EntitySceneObject>(),
      Array.Empty<EntitySceneMatGroup>());
    return true;
  }

  private static bool TryReadParticleScriptText(
    string entityRoot,
    string particleDefinitionPath,
    RpkArchiveManager rpkManager,
    out string scriptText)
  {
    scriptText = string.Empty;
    string normalized = NormalizeSptAssetPathReference(particleDefinitionPath);
    if (normalized.Length == 0)
    {
      return false;
    }

    string? filePath = TryResolveEntityModelPath(entityRoot, normalized);
    if (filePath != null)
    {
      try
      {
        byte[] bytes = File.ReadAllBytes(filePath);
        scriptText = DecodeParticleScriptText(bytes);
        if (scriptText.Length > 0)
        {
          return true;
        }
      }
      catch
      {
      }
    }

    string? archivePath = NormalizeArchiveAssetPath(normalized, ".spt");
    if (archivePath != null && rpkManager.TryReadAsset(archivePath, out byte[] archiveBytes))
    {
      scriptText = DecodeParticleScriptText(archiveBytes);
      return scriptText.Length > 0;
    }

    return false;
  }

  private static string DecodeParticleScriptText(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return string.Empty;
    }

    string ascii = Encoding.ASCII.GetString(bytes);
    if (ascii.IndexOf("entity_file", StringComparison.OrdinalIgnoreCase) >= 0)
    {
      return ascii;
    }

    try
    {
      Encoding euckr = Encoding.GetEncoding(949);
      return euckr.GetString(bytes);
    }
    catch
    {
      return ascii;
    }
  }

  private static ParticleScriptData ParseParticleScript(string scriptText)
  {
    HashSet<string> references = new(StringComparer.OrdinalIgnoreCase);
    uint alphaType = DefaultParticleAlphaType;
    bool disableDepthTest = false;

    using StringReader reader = new(scriptText);
    string? line;
    while ((line = reader.ReadLine()) != null)
    {
      int commentStart = line.IndexOf("//", StringComparison.Ordinal);
      if (commentStart >= 0)
      {
        line = line[..commentStart];
      }

      line = line.Trim();
      if (line.Length == 0)
      {
        continue;
      }

      if (line.StartsWith("entity_file", StringComparison.OrdinalIgnoreCase))
      {
        string tail = line["entity_file".Length..].Trim();
        if (tail.Length == 0)
        {
          continue;
        }

        string token = ReadSptPathToken(tail);
        string normalized = NormalizeSptAssetPathReference(token);
        if (normalized.Length > 0)
        {
          references.Add(normalized);
        }

        continue;
      }

      if (line.StartsWith("alpha_type", StringComparison.OrdinalIgnoreCase))
      {
        string tail = line["alpha_type".Length..].Trim();
        string token = ReadSptPathToken(tail);
        if (int.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out int parsedAlphaType)
          && parsedAlphaType >= 0)
        {
          alphaType = (uint)parsedAlphaType;
        }

        continue;
      }

      if (line.StartsWith("z_disable", StringComparison.OrdinalIgnoreCase))
      {
        string tail = line["z_disable".Length..].Trim();
        if (tail.Length == 0)
        {
          disableDepthTest = true;
          continue;
        }

        string token = ReadSptPathToken(tail);
        if (int.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out int parsedValue))
        {
          disableDepthTest = parsedValue != 0;
        }
        else
        {
          disableDepthTest = true;
        }
      }
    }

    return new ParticleScriptData(references.ToArray(), alphaType, disableDepthTest);
  }

  private static EntityMeshData ApplyParticleScriptOverrides(EntityMeshData model, uint alphaType)
  {
    MaterialDefinition[] materials = model.Materials;
    if (materials.Length > 0)
    {
      MaterialDefinition[] overriddenMaterials = new MaterialDefinition[materials.Length];
      for (int materialIndex = 0; materialIndex < materials.Length; ++materialIndex)
      {
        MaterialDefinition sourceMaterial = materials[materialIndex];
        MaterialLayerDefinition[] sourceLayers = sourceMaterial.Layers;
        if (sourceLayers.Length == 0)
        {
          overriddenMaterials[materialIndex] = sourceMaterial;
          continue;
        }

        MaterialLayerDefinition[] overriddenLayers = new MaterialLayerDefinition[sourceLayers.Length];
        for (int layerIndex = 0; layerIndex < sourceLayers.Length; ++layerIndex)
        {
          MaterialLayerDefinition sourceLayer = sourceLayers[layerIndex];
          overriddenLayers[layerIndex] = sourceLayer with
          {
            AlphaType = alphaType,
            Argb = 0xFFFFFFFFu,
            LayerFlags = 0u,
          };
        }

        overriddenMaterials[materialIndex] = sourceMaterial with { Layers = overriddenLayers };
      }

      materials = overriddenMaterials;
    }

    int alphaCount = model.MaterialAlphaTypes.Length;
    if (alphaCount == 0)
    {
      alphaCount = model.MaterialSurfaceIds.Length;
    }
    if (alphaCount == 0)
    {
      alphaCount = model.Materials.Length;
    }

    uint[] materialAlphaTypes;
    if (alphaCount > 0)
    {
      materialAlphaTypes = new uint[alphaCount];
      for (int index = 0; index < materialAlphaTypes.Length; ++index)
      {
        materialAlphaTypes[index] = alphaType;
      }
    }
    else
    {
      materialAlphaTypes = Array.Empty<uint>();
    }

    return new EntityMeshData(
      model.Name,
      model.Vertices,
      model.MaterialSpans,
      materials,
      model.MaterialSurfaceIds,
      materialAlphaTypes,
      model.SurfaceTextures,
      model.SceneObjects,
      model.SceneMatGroups);
  }

  private static string ReadSptPathToken(string text)
  {
    text = text.Trim();
    if (text.Length == 0)
    {
      return string.Empty;
    }

    if (text[0] == '"' || text[0] == '\'')
    {
      char quote = text[0];
      int closingQuote = text.IndexOf(quote, 1);
      string quoted = closingQuote > 0 ? text[1..closingQuote] : text[1..];
      return quoted.Trim().TrimEnd(';', ',');
    }

    int splitIndex = text.IndexOfAny([' ', '\t', ';', ',']);
    if (splitIndex >= 0)
    {
      return text[..splitIndex].Trim();
    }

    return text.Trim();
  }

  private static string NormalizeSptAssetPathReference(string path)
  {
    string normalized = path.Trim().Trim('"', '\'');
    if (normalized.Length == 0)
    {
      return string.Empty;
    }

    normalized = normalized.Replace('/', '\\');
    if (normalized.StartsWith(".\\", StringComparison.Ordinal))
    {
      normalized = normalized[2..];
    }

    const string mapEntityPrefix = "map\\entity\\";
    if (normalized.StartsWith("\\" + mapEntityPrefix, StringComparison.OrdinalIgnoreCase))
    {
      normalized = normalized[(mapEntityPrefix.Length + 1)..];
    }
    else if (normalized.StartsWith(mapEntityPrefix, StringComparison.OrdinalIgnoreCase))
    {
      normalized = normalized[mapEntityPrefix.Length..];
    }

    if (!normalized.StartsWith("\\", StringComparison.Ordinal))
    {
      normalized = "\\" + normalized;
    }

    return normalized;
  }

  private static string[] ResolveSkyPaths(string bspPath, SkySourceMode skySourceMode)
  {
    string? mapFolder = Path.GetDirectoryName(bspPath);
    if (string.IsNullOrEmpty(mapFolder))
    {
      return Array.Empty<string>();
    }

    string mapName = Path.GetFileNameWithoutExtension(bspPath);
    string sky = Path.Combine(mapFolder, "sky", $"{mapName}sky.r3e");
    string sky2 = Path.Combine(mapFolder, "sky2", $"{mapName}sky2.r3e");

    if (skySourceMode == SkySourceMode.Sky2)
    {
      string? resolvedSky2 = FindPathCaseInsensitive(sky2);
      if (resolvedSky2 != null)
      {
        return [resolvedSky2];
      }

      string? fallbackSky = FindPathCaseInsensitive(sky);
      if (fallbackSky != null)
      {
        return [fallbackSky];
      }

      return Array.Empty<string>();
    }

    string? resolvedSky = FindPathCaseInsensitive(sky);
    if (resolvedSky != null)
    {
      return [resolvedSky];
    }

    string? fallbackSky2 = FindPathCaseInsensitive(sky2);
    if (fallbackSky2 != null)
    {
      return [fallbackSky2];
    }

    return Array.Empty<string>();
  }

  private static RpkArchiveManager GetOrCreateEntityArchiveManager(string entityRoot, out bool wasCached)
  {
    string cacheKey = Path.GetFullPath(entityRoot).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
    lock (EntityArchiveCacheSync)
    {
      if (EntityArchiveCache.TryGetValue(cacheKey, out RpkArchiveManager? cached))
      {
        wasCached = true;
        return cached;
      }

      RpkArchiveManager created = new(cacheKey);
      EntityArchiveCache[cacheKey] = created;
      wasCached = false;
      return created;
    }
  }

  private static string? TryResolveEntityRootPath(string bspPath)
  {
    string fullPath = Path.GetFullPath(bspPath);
    string marker = $"{Path.DirectorySeparatorChar}Map{Path.DirectorySeparatorChar}";
    int markerIndex = fullPath.LastIndexOf(marker, StringComparison.OrdinalIgnoreCase);
    if (markerIndex >= 0)
    {
      string root = fullPath[..markerIndex];
      string candidate = Path.Combine(root, "Map", "Entity");
      if (Directory.Exists(candidate))
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
    return Directory.Exists(fallback) ? fallback : null;
  }

  private static string? TryResolveEntityModelPath(string entityRoot, string rawName)
  {
    string name = rawName.Trim();
    if (name.Length == 0)
    {
      return null;
    }

    name = name.Replace('/', Path.DirectorySeparatorChar).Replace('\\', Path.DirectorySeparatorChar);
    name = name.TrimStart(Path.DirectorySeparatorChar);
    if (name.StartsWith($".{Path.DirectorySeparatorChar}", StringComparison.Ordinal))
    {
      name = name[2..];
    }

    string mapEntityPrefix = $"Map{Path.DirectorySeparatorChar}Entity{Path.DirectorySeparatorChar}";
    if (name.StartsWith(mapEntityPrefix, StringComparison.OrdinalIgnoreCase))
    {
      name = name[mapEntityPrefix.Length..];
    }

    string candidate = Path.Combine(entityRoot, name);
    string? resolved = FindPathCaseInsensitive(candidate);
    if (resolved != null)
    {
      return resolved;
    }

    if (!Path.HasExtension(candidate))
    {
      resolved = FindPathCaseInsensitive(candidate + ".r3e");
      if (resolved != null)
      {
        return resolved;
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
    if (string.IsNullOrEmpty(directory) || !Directory.Exists(directory))
    {
      return null;
    }

    string fileName = Path.GetFileName(path);
    foreach (string file in Directory.EnumerateFiles(directory))
    {
      if (string.Equals(Path.GetFileName(file), fileName, StringComparison.OrdinalIgnoreCase))
      {
        return file;
      }
    }

    return null;
  }

  private static string? NormalizeArchiveAssetPath(string rawName, string defaultExtension)
  {
    string name = rawName.Trim();
    if (name.Length == 0)
    {
      return null;
    }

    name = name.Replace('/', '\\').ToLowerInvariant();
    if (!name.StartsWith("\\", StringComparison.Ordinal))
    {
      name = "\\" + name;
    }

    if (!Path.HasExtension(name))
    {
      name += defaultExtension;
    }

    return name;
  }

  private static RenderMeshData MergeEntityMeshes(IEnumerable<EntityMeshData> meshes)
  {
    List<BspRenderVertex> vertices = new();
    List<BspMaterialSpan> spans = new();
    List<MaterialDefinition> materials = new();
    List<int> materialSurfaceIds = new();
    List<uint> materialAlphaTypes = new();
    List<R3TextureBlob> textures = new();
    int nextSurfaceId = 1;

    foreach (EntityMeshData mesh in meshes)
    {
      if (mesh.Vertices.Length == 0)
      {
        continue;
      }

      Dictionary<int, int> surfaceIdMap = new();
      for (int i = 0; i < mesh.SurfaceTextures.Length; ++i)
      {
        R3TextureBlob source = mesh.SurfaceTextures[i];
        int newSurfaceId = nextSurfaceId++;
        surfaceIdMap[source.SurfaceId] = newSurfaceId;
        textures.Add(new R3TextureBlob(newSurfaceId, $"{mesh.Name}:{source.Name}", source.DdsBytes));
      }

      int materialBase = materialSurfaceIds.Count;
      MaterialDefinition[] remappedMaterials = RemapMaterialDefinitions(mesh.Materials, surfaceIdMap, null);
      for (int i = 0; i < remappedMaterials.Length; ++i)
      {
        materials.Add(remappedMaterials[i]);
      }

      for (int i = 0; i < mesh.MaterialSurfaceIds.Length; ++i)
      {
        int localSurfaceId = mesh.MaterialSurfaceIds[i];
        materialSurfaceIds.Add(
          localSurfaceId > 0 && surfaceIdMap.TryGetValue(localSurfaceId, out int mappedSurface)
            ? mappedSurface
            : 0);
        uint alphaType = i < mesh.MaterialAlphaTypes.Length ? mesh.MaterialAlphaTypes[i] : 0u;
        materialAlphaTypes.Add(alphaType);
      }

      int vertexBase = vertices.Count;
      vertices.AddRange(mesh.Vertices);
      for (int i = 0; i < mesh.MaterialSpans.Length; ++i)
      {
        BspMaterialSpan span = mesh.MaterialSpans[i];
        int materialId = span.MaterialId >= 0 ? span.MaterialId + materialBase : span.MaterialId;
        AddOrMergeSpan(spans, new BspMaterialSpan(materialId, vertexBase + span.StartVertex, span.VertexCount));
      }
    }

    return new RenderMeshData(
      vertices.ToArray(),
      spans.ToArray(),
      materials.ToArray(),
      materialSurfaceIds.ToArray(),
      materialAlphaTypes.ToArray(),
      textures.ToArray());
  }

  private static RenderMeshData BuildCombinedEntityMesh(
    IReadOnlyDictionary<int, EntityMeshData> modelsById,
    ExtMapEntity[] mapEntities)
  {
    List<BspRenderVertex> vertices = new(Math.Max(8192, mapEntities.Length * 512));
    List<BspMaterialSpan> spans = new(Math.Max(128, mapEntities.Length * 4));
    List<MaterialDefinition> materials = new();
    List<int> materialSurfaceIds = new();
    List<uint> materialAlphaTypes = new();
    List<R3TextureBlob> textures = new();
    Dictionary<int, ModelRemapData> remapByEntityId = new();
    int nextSurfaceId = 1_000_000;

    List<int> modelIds = new(modelsById.Keys);
    modelIds.Sort();
    for (int modelIndex = 0; modelIndex < modelIds.Count; ++modelIndex)
    {
      int modelId = modelIds[modelIndex];
      EntityMeshData model = modelsById[modelId];
      Dictionary<int, int> surfaceIdMap = new();

      for (int i = 0; i < model.SurfaceTextures.Length; ++i)
      {
        R3TextureBlob local = model.SurfaceTextures[i];
        int globalSurfaceId = nextSurfaceId++;
        surfaceIdMap[local.SurfaceId] = globalSurfaceId;
        textures.Add(new R3TextureBlob(globalSurfaceId, $"{model.Name}:{local.Name}", local.DdsBytes));
      }

      int materialBase = materialSurfaceIds.Count;
      MaterialDefinition[] remappedMaterials = RemapMaterialDefinitions(model.Materials, surfaceIdMap, true);
      for (int i = 0; i < remappedMaterials.Length; ++i)
      {
        materials.Add(remappedMaterials[i]);
      }

      for (int i = 0; i < model.MaterialSurfaceIds.Length; ++i)
      {
        int localSurfaceId = model.MaterialSurfaceIds[i];
        int globalSurfaceId = 0;
        if (localSurfaceId > 0 && surfaceIdMap.TryGetValue(localSurfaceId, out int mappedSurface))
        {
          globalSurfaceId = mappedSurface;
        }
        else if (localSurfaceId > 0)
        {
          // Some map entities do not have a local .r3t; their materials reference
          // global map surface ids directly. Preserve them for BSP-texture fallback.
          globalSurfaceId = localSurfaceId;
        }

        materialSurfaceIds.Add(globalSurfaceId);
        uint alphaType = i < model.MaterialAlphaTypes.Length ? model.MaterialAlphaTypes[i] : 0u;
        materialAlphaTypes.Add(alphaType);
      }

      remapByEntityId[modelId] = new ModelRemapData(model, materialBase);
    }

    for (int index = 0; index < mapEntities.Length; ++index)
    {
      ExtMapEntity instance = mapEntities[index];
      if (!remapByEntityId.TryGetValue(instance.EntityId, out ModelRemapData remap))
      {
        continue;
      }

      EntityMeshData model = remap.Model;
      if (model.Vertices.Length == 0)
      {
        continue;
      }

      float scale = float.IsFinite(instance.Scale) ? instance.Scale : 1.0f;
      if (MathF.Abs(scale) < 0.00001f)
      {
        scale = 1.0f;
      }

      int vertexBase = vertices.Count;
      for (int i = 0; i < model.Vertices.Length; ++i)
      {
        BspRenderVertex source = model.Vertices[i];
        Vector3 position = TransformEntityPosition(source.Position, scale, instance.RotX, instance.RotY, instance.Position);
        if (!IsFinite(position))
        {
          position = Vector3.Zero;
        }

        vertices.Add(new BspRenderVertex(position, source.Uv, source.LightUv, source.Color));
      }

      for (int i = 0; i < model.MaterialSpans.Length; ++i)
      {
        BspMaterialSpan span = model.MaterialSpans[i];
        int materialId = span.MaterialId;
        if (materialId >= 0)
        {
          materialId += remap.MaterialBase;
        }

        AddOrMergeSpan(spans, new BspMaterialSpan(materialId, vertexBase + span.StartVertex, span.VertexCount));
      }
    }

    return new RenderMeshData(
      vertices.ToArray(),
      spans.ToArray(),
      materials.ToArray(),
      materialSurfaceIds.ToArray(),
      materialAlphaTypes.ToArray(),
      textures.ToArray());
  }

  private static EntitySceneData BuildEntitySceneData(
    IReadOnlyDictionary<int, EntityMeshData> modelsById,
    ExtMapEntity[] mapEntities)
  {
    if (modelsById.Count == 0 || mapEntities.Length == 0)
    {
      return EntitySceneData.Empty;
    }

    List<EntitySceneModel> models = new(modelsById.Count);
    List<int> modelIds = new(modelsById.Keys);
    modelIds.Sort();
    int materialBase = 0;
    for (int modelIndex = 0; modelIndex < modelIds.Count; ++modelIndex)
    {
      int modelId = modelIds[modelIndex];
      EntityMeshData model = modelsById[modelId];
      models.Add(new EntitySceneModel
      {
        EntityId = modelId,
        MaterialBase = materialBase,
        Name = model.Name,
        Objects = model.SceneObjects,
        MatGroups = model.SceneMatGroups,
      });
      materialBase += model.MaterialSurfaceIds.Length;
    }

    List<EntitySceneInstance> instances = new(mapEntities.Length);
    for (int index = 0; index < mapEntities.Length; ++index)
    {
      ExtMapEntity mapEntity = mapEntities[index];
      if (!modelsById.ContainsKey(mapEntity.EntityId))
      {
        continue;
      }

      instances.Add(new EntitySceneInstance(
        mapEntity.EntityId,
        mapEntity.Scale,
        mapEntity.Position,
        mapEntity.RotX,
        mapEntity.RotY));
    }

    return new EntitySceneData
    {
      Models = models.ToArray(),
      Instances = instances.ToArray(),
    };
  }

  private static MaterialDefinition[] RemapMaterialDefinitions(
    MaterialDefinition[] sourceMaterials,
    IReadOnlyDictionary<int, int> surfaceIdMap,
    bool? preserveUnmappedPositiveSurfaceIds)
  {
    if (sourceMaterials.Length == 0)
    {
      return Array.Empty<MaterialDefinition>();
    }

    MaterialDefinition[] remapped = new MaterialDefinition[sourceMaterials.Length];
    for (int materialIndex = 0; materialIndex < sourceMaterials.Length; ++materialIndex)
    {
      MaterialDefinition source = sourceMaterials[materialIndex];
      MaterialLayerDefinition[] remappedLayers = new MaterialLayerDefinition[source.Layers.Length];

      for (int layerIndex = 0; layerIndex < source.Layers.Length; ++layerIndex)
      {
        MaterialLayerDefinition layer = source.Layers[layerIndex];
        int surfaceId = RemapSurfaceId(layer.SurfaceId, surfaceIdMap, preserveUnmappedPositiveSurfaceIds);
        remappedLayers[layerIndex] = layer with { SurfaceId = surfaceId };
      }

      int detailSurface = RemapSurfaceId(source.DetailSurfaceId, surfaceIdMap, preserveUnmappedPositiveSurfaceIds);
      remapped[materialIndex] = source with
      {
        DetailSurfaceId = detailSurface,
        Layers = remappedLayers,
      };
    }

    return remapped;
  }

  private static int RemapSurfaceId(
    int surfaceId,
    IReadOnlyDictionary<int, int> surfaceIdMap,
    bool? preserveUnmappedPositiveSurfaceIds)
  {
    if (surfaceId <= 0)
    {
      return surfaceId;
    }

    if (surfaceIdMap.TryGetValue(surfaceId, out int remapped))
    {
      return remapped;
    }

    if (preserveUnmappedPositiveSurfaceIds.GetValueOrDefault())
    {
      return surfaceId;
    }

    return 0;
  }

  private static void AddOrMergeSpan(List<BspMaterialSpan> spans, BspMaterialSpan span)
  {
    if (span.VertexCount <= 0)
    {
      return;
    }

    if (spans.Count == 0)
    {
      spans.Add(span);
      return;
    }

    BspMaterialSpan last = spans[^1];
    if (last.MaterialId == span.MaterialId
      && last.LightMapId == span.LightMapId
      && last.StartVertex + last.VertexCount == span.StartVertex)
    {
      spans[^1] = new BspMaterialSpan(
        last.MaterialId,
        last.StartVertex,
        last.VertexCount + span.VertexCount,
        last.LightMapId);
      return;
    }

    spans.Add(span);
  }

  private static Vector3 TransformEntityPosition(Vector3 local, float scale, float rotXDeg, float rotYDeg, Vector3 translation)
  {
    float x = local.X * scale;
    float y = local.Y * scale;
    float z = local.Z * scale;

    float rotY = MathHelper.DegreesToRadians(rotYDeg);
    float cosY = MathF.Cos(rotY);
    float sinY = MathF.Sin(rotY);
    float xAfterY = x * cosY + z * sinY;
    float zAfterY = -x * sinY + z * cosY;

    float rotX = MathHelper.DegreesToRadians(rotXDeg);
    float cosX = MathF.Cos(rotX);
    float sinX = MathF.Sin(rotX);
    float yAfterX = y * cosX - zAfterY * sinX;
    float zAfterX = y * sinX + zAfterY * cosX;

    return new Vector3(
      xAfterY + translation.X,
      yAfterX + translation.Y,
      zAfterX + translation.Z);
  }

  private static EntityMeshData LoadEntityMesh(string r3ePath, bool useMaterialNameFallback)
  {
    byte[] r3eBytes = File.ReadAllBytes(r3ePath);
    string assetName = Path.GetFileNameWithoutExtension(r3ePath);

    byte[]? SidecarResolver(string extension)
    {
      string? sibling = FindSiblingFileWithExtension(r3ePath, extension);
      return sibling != null && File.Exists(sibling) ? File.ReadAllBytes(sibling) : null;
    }

    return LoadEntityMeshCore(assetName, r3eBytes, SidecarResolver, useMaterialNameFallback);
  }

  private static EntityMeshData LoadEntityMeshFromArchive(string archivePath, RpkArchiveManager manager, bool useMaterialNameFallback)
  {
    if (!manager.TryReadAsset(archivePath, out byte[] r3eBytes))
    {
      throw new FileNotFoundException("RPK asset not found.", archivePath);
    }

    string assetName = Path.GetFileNameWithoutExtension(archivePath);
    string basePath = Path.ChangeExtension(archivePath, null) ?? archivePath;
    byte[]? SidecarResolver(string extension)
    {
      string lookup = basePath + extension;
      return manager.TryReadAsset(lookup, out byte[] bytes) ? bytes : null;
    }

    return LoadEntityMeshCore(assetName, r3eBytes, SidecarResolver, useMaterialNameFallback);
  }

  private static EntityMeshData LoadEntityMeshCore(
    string assetName,
    byte[] r3eBytes,
    Func<string, byte[]?> sidecarResolver,
    bool useMaterialNameFallback)
  {
    using MemoryStream stream = new(r3eBytes, writable: false);
    using BinaryReader reader = new(stream);

    if (stream.Length < 8 + EntityFileHeaderEntryCount * 8)
    {
      throw new InvalidDataException("R3E file is too small.");
    }

    uint magic = reader.ReadUInt32();
    _ = reader.ReadUInt32();
    if (magic != ExpectedR3eMagic)
    {
      throw new InvalidDataException($"Unexpected R3E magic in {assetName}.");
    }

    EntityFileHeader header = ReadEntityFileHeader(reader);
    byte[] compHeaderBytes = ReadExactBytes(reader, checked((int)header.CompHeader.Size), "R3E.CompHeader");
    byte[] vertexBytes = ReadExactBytes(reader, checked((int)header.Vertex.Size), "R3E.Vertex");
    byte[] colorBytes = ReadExactBytes(reader, checked((int)header.VertexColor.Size), "R3E.VColor");
    byte[] uvBytes = ReadExactBytes(reader, checked((int)header.Uv.Size), "R3E.UV");
    byte[] faceBytes = ReadExactBytes(reader, checked((int)header.Face.Size), "R3E.Face");
    byte[] faceIdBytes = ReadExactBytes(reader, checked((int)header.FaceId.Size), "R3E.FaceId");
    byte[] vertexIdBytes = ReadExactBytes(reader, checked((int)header.VertexId.Size), "R3E.VertexId");
    byte[] matGroupBytes = ReadExactBytes(reader, checked((int)header.MatGroup.Size), "R3E.MatGroup");
    byte[] objectBytes = ReadExactBytes(reader, checked((int)header.Object.Size), "R3E.Object");
    byte[] trackBytes = ReadExactBytes(reader, checked((int)header.Track.Size), "R3E.Track");
    _ = trackBytes;

    EntityCompHeader comp = ReadEntityCompHeader(compHeaderBytes);
    Vector3[] entityVertices = DecodeEntityVertices(comp, vertexBytes);
    if ((colorBytes.Length % 4) != 0)
    {
      throw new InvalidDataException("R3E color section size is not aligned to 4 bytes.");
    }
    if ((uvBytes.Length % 4) != 0)
    {
      throw new InvalidDataException("R3E UV section size is not aligned to 4 bytes.");
    }
    if ((faceBytes.Length % 6) != 0)
    {
      throw new InvalidDataException("R3E face section size is not aligned to 6 bytes.");
    }
    if ((faceIdBytes.Length % 2) != 0)
    {
      throw new InvalidDataException("R3E face-id section size is not aligned to 2 bytes.");
    }
    if ((vertexIdBytes.Length % 2) != 0)
    {
      throw new InvalidDataException("R3E vertex-id section size is not aligned to 2 bytes.");
    }
    if ((matGroupBytes.Length % 22) != 0)
    {
      throw new InvalidDataException("R3E mat-group section size is not aligned to 22 bytes.");
    }

    uint[] colors = new uint[colorBytes.Length / 4];
    Buffer.BlockCopy(colorBytes, 0, colors, 0, colorBytes.Length);

    short[] uvRaw = new short[uvBytes.Length / 2];
    Buffer.BlockCopy(uvBytes, 0, uvRaw, 0, uvBytes.Length);

    ReadEntityFace[] faces = new ReadEntityFace[faceBytes.Length / 6];
    using (MemoryStream faceStream = new(faceBytes, writable: false))
    using (BinaryReader faceReader = new(faceStream))
    {
      for (int i = 0; i < faces.Length; ++i)
      {
        faces[i] = new ReadEntityFace(faceReader.ReadUInt16(), faceReader.ReadUInt32());
      }
    }

    ushort[] faceIds = new ushort[faceIdBytes.Length / 2];
    Buffer.BlockCopy(faceIdBytes, 0, faceIds, 0, faceIdBytes.Length);

    ushort[] vertexIds = new ushort[vertexIdBytes.Length / 2];
    Buffer.BlockCopy(vertexIdBytes, 0, vertexIds, 0, vertexIdBytes.Length);

    ReadEntityMatGroup[] matGroups = new ReadEntityMatGroup[matGroupBytes.Length / 22];
    using (MemoryStream groupStream = new(matGroupBytes, writable: false))
    using (BinaryReader groupReader = new(groupStream))
    {
      for (int i = 0; i < matGroups.Length; ++i)
      {
        ushort faceCount = groupReader.ReadUInt16();
        uint faceStartId = groupReader.ReadUInt32();
        short materialId = groupReader.ReadInt16();
        ushort objectId = groupReader.ReadUInt16();
        _ = groupReader.ReadInt16();
        _ = groupReader.ReadInt16();
        _ = groupReader.ReadInt16();
        _ = groupReader.ReadInt16();
        _ = groupReader.ReadInt16();
        _ = groupReader.ReadInt16();
        matGroups[i] = new ReadEntityMatGroup(faceCount, faceStartId, materialId, objectId);
      }
    }

    ReadEntityAniObject[] entityObjects = ReadEntityAniObjects(objectBytes);
    EntityObjectTracks[] objectTracks = BuildEntityObjectTracks(entityObjects, trackBytes);
    NumericsMatrix4[] objectMatrices = BuildEntityObjectMatrices(entityObjects, objectTracks);
    EntitySceneObject[] sceneObjects = BuildEntitySceneObjects(entityObjects, objectTracks);

    R3MaterialData materialData = R3MaterialData.Empty;
    try
    {
      byte[]? r3mBytes = sidecarResolver(".r3m");
      if (r3mBytes != null)
      {
        materialData = ReadR3mMaterials(r3mBytes);
      }
    }
    catch
    {
      materialData = R3MaterialData.Empty;
    }

    R3TextureBlob[] surfaceTextures = Array.Empty<R3TextureBlob>();
    try
    {
        byte[]? r3tBytes = sidecarResolver(".r3t");
        if (r3tBytes != null)
        {
          surfaceTextures = ReadR3tTextureBlobs(r3tBytes, DefaultR3tSurfaceIdBase);
        }
    }
    catch
    {
      surfaceTextures = Array.Empty<R3TextureBlob>();
    }

    Dictionary<int, int> materialNameMap = useMaterialNameFallback
      ? BuildMaterialNameNumberMap(materialData.Names)
      : EmptyMaterialNameMap;
    int[] matGroupDrawOrder = BuildEntityMatGroupDrawOrder(matGroups, materialData.Names, useMaterialNameFallback);

    MeshBuildResult mesh = BuildEntityTriangles(
      entityVertices,
      colors,
      uvRaw,
      faces,
      faceIds,
      vertexIds,
      matGroups,
      matGroupDrawOrder,
      objectMatrices,
      materialData,
      materialNameMap,
      comp.UvMin,
      comp.UvMax);

    EntitySceneMatGroup[] sceneMatGroups = BuildEntitySceneMatGroups(
      entityVertices,
      colors,
      uvRaw,
      faces,
      faceIds,
      vertexIds,
      matGroups,
      matGroupDrawOrder,
      materialData,
      materialNameMap,
      comp.UvMin,
      comp.UvMax);

    return new EntityMeshData(
      assetName,
      mesh.Vertices,
      mesh.MaterialSpans,
      materialData.Materials,
      materialData.SurfaceIds,
      materialData.LayerAlphaTypes,
      surfaceTextures,
      sceneObjects,
      sceneMatGroups);
  }

  private static EntityFileHeader ReadEntityFileHeader(BinaryReader reader)
  {
    BspEntry[] entries = new BspEntry[EntityFileHeaderEntryCount];
    for (int index = 0; index < entries.Length; ++index)
    {
      entries[index] = ReadEntry(reader);
    }

    return new EntityFileHeader(
      entries[0],
      entries[1],
      entries[2],
      entries[3],
      entries[4],
      entries[5],
      entries[6],
      entries[7],
      entries[8],
      entries[9]);
  }

  private static EntityCompHeader ReadEntityCompHeader(byte[] bytes)
  {
    if (bytes.Length < 38)
    {
      throw new InvalidDataException("R3E comp-header section is too small.");
    }

    using MemoryStream stream = new(bytes, writable: false);
    using BinaryReader reader = new(stream);
    ushort type = reader.ReadUInt16();
    _ = reader.ReadInt16();
    _ = reader.ReadInt16();
    _ = reader.ReadInt16();
    _ = reader.ReadInt16();
    _ = reader.ReadInt16();
    _ = reader.ReadInt16();
    float px = reader.ReadSingle();
    float py = reader.ReadSingle();
    float pz = reader.ReadSingle();
    float scale = reader.ReadSingle();
    float uvMin = reader.ReadSingle();
    float uvMax = reader.ReadSingle();
    return new EntityCompHeader(type, new Vector3(px, py, pz), scale, uvMin, uvMax);
  }

  private static Vector3[] DecodeEntityVertices(EntityCompHeader comp, byte[] vertexBytes)
  {
    int vertexCount = comp.Type switch
    {
      0x8000 => vertexBytes.Length / 3,
      0x4000 => vertexBytes.Length / 6,
      _ => vertexBytes.Length / 12,
    };

    Vector3[] vertices = new Vector3[vertexCount];
    short signedType = unchecked((short)comp.Type);
    if (signedType < 0)
    {
      for (int i = 0; i < vertexCount; ++i)
      {
        int offset = i * 3;
        sbyte xRaw = unchecked((sbyte)vertexBytes[offset]);
        sbyte yRaw = unchecked((sbyte)vertexBytes[offset + 1]);
        sbyte zRaw = unchecked((sbyte)vertexBytes[offset + 2]);
        float x = (xRaw / 127.0f) * comp.Scale + comp.Position.X;
        float y = (yRaw / 127.0f) * comp.Scale + comp.Position.Y;
        float z = (zRaw / 127.0f) * comp.Scale + comp.Position.Z;
        vertices[i] = new Vector3(x, y, z);
      }

      return vertices;
    }

    if ((comp.Type & 0x4000) != 0)
    {
      for (int i = 0; i < vertexCount; ++i)
      {
        int offset = i * 6;
        short xRaw = BinaryPrimitives.ReadInt16LittleEndian(vertexBytes.AsSpan(offset, 2));
        short yRaw = BinaryPrimitives.ReadInt16LittleEndian(vertexBytes.AsSpan(offset + 2, 2));
        short zRaw = BinaryPrimitives.ReadInt16LittleEndian(vertexBytes.AsSpan(offset + 4, 2));
        float x = (xRaw / 32767.0f) * comp.Scale + comp.Position.X;
        float y = (yRaw / 32767.0f) * comp.Scale + comp.Position.Y;
        float z = (zRaw / 32767.0f) * comp.Scale + comp.Position.Z;
        vertices[i] = new Vector3(x, y, z);
      }

      return vertices;
    }

    for (int i = 0; i < vertexCount; ++i)
    {
      int offset = i * 12;
      float x = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(vertexBytes.AsSpan(offset, 4)));
      float y = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(vertexBytes.AsSpan(offset + 4, 4)));
      float z = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(vertexBytes.AsSpan(offset + 8, 4)));
      vertices[i] = new Vector3(x, y, z);
    }

    return vertices;
  }

  private static ReadEntityAniObject[] ReadEntityAniObjects(byte[] objectBytes)
  {
    if (objectBytes.Length == 0)
    {
      return Array.Empty<ReadEntityAniObject>();
    }

    if ((objectBytes.Length % ReadAniObjectEntrySize) != 0)
    {
      throw new InvalidDataException("R3E object section size is not aligned to 88 bytes.");
    }

    ReadEntityAniObject[] objects = new ReadEntityAniObject[objectBytes.Length / ReadAniObjectEntrySize];
    using MemoryStream stream = new(objectBytes, writable: false);
    using BinaryReader reader = new(stream);
    for (int index = 0; index < objects.Length; ++index)
    {
      ushort flag = reader.ReadUInt16();
      ushort parent = reader.ReadUInt16();
      int frames = reader.ReadInt32();
      int posCount = reader.ReadInt32();
      int rotCount = reader.ReadInt32();
      int scaleCount = reader.ReadInt32();
      Vector3 scale = new(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
      Vector4 scaleQuat = new(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
      Vector3 position = new(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
      Vector4 quat = new(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
      uint posOffset = reader.ReadUInt32();
      uint rotOffset = reader.ReadUInt32();
      uint scaleOffset = reader.ReadUInt32();
      objects[index] = new ReadEntityAniObject(
        flag,
        parent,
        frames,
        posCount,
        rotCount,
        scaleCount,
        scale,
        scaleQuat,
        position,
        quat,
        posOffset,
        rotOffset,
        scaleOffset);
    }

    return objects;
  }

  private static NumericsMatrix4[] BuildEntityObjectMatrices(ReadEntityAniObject[] objects, EntityObjectTracks[] objectTracks)
  {
    if (objects.Length == 0 || objectTracks.Length == 0)
    {
      return Array.Empty<NumericsMatrix4>();
    }

    NumericsMatrix4[] legacyWorldMatrices = new NumericsMatrix4[objects.Length];
    byte[] visitState = new byte[objects.Length];

    for (int index = 0; index < objects.Length; ++index)
    {
      _ = ResolveLegacyObjectMatrix(index, objects, objectTracks, legacyWorldMatrices, visitState);
    }

    NumericsMatrix4[] convertedMatrices = new NumericsMatrix4[objects.Length];
    for (int index = 0; index < convertedMatrices.Length; ++index)
    {
      convertedMatrices[index] = ConvertFrom3dsMaxMatrix(legacyWorldMatrices[index]);
    }

    return convertedMatrices;
  }

  private static EntitySceneObject[] BuildEntitySceneObjects(ReadEntityAniObject[] objects, EntityObjectTracks[] objectTracks)
  {
    if (objects.Length == 0)
    {
      return Array.Empty<EntitySceneObject>();
    }

    EntitySceneObject[] sceneObjects = new EntitySceneObject[objects.Length];
    for (int index = 0; index < objects.Length; ++index)
    {
      ReadEntityAniObject obj = objects[index];
      EntityObjectTracks tracks = index < objectTracks.Length
        ? objectTracks[index]
        : new EntityObjectTracks(Array.Empty<PositionTrack>(), Array.Empty<RotationTrack>(), Array.Empty<ScaleTrack>());

      EntityScenePositionTrack[] positionTracks = new EntityScenePositionTrack[tracks.PositionTracks.Length];
      for (int positionIndex = 0; positionIndex < positionTracks.Length; ++positionIndex)
      {
        PositionTrack source = tracks.PositionTracks[positionIndex];
        positionTracks[positionIndex] = new EntityScenePositionTrack(source.Frame, source.Position);
      }

      EntitySceneRotationTrack[] rotationTracks = new EntitySceneRotationTrack[tracks.RotationTracks.Length];
      for (int rotationIndex = 0; rotationIndex < rotationTracks.Length; ++rotationIndex)
      {
        RotationTrack source = tracks.RotationTracks[rotationIndex];
        rotationTracks[rotationIndex] = new EntitySceneRotationTrack(source.Frame, source.Quaternion);
      }

      EntitySceneScaleTrack[] scaleTracks = new EntitySceneScaleTrack[tracks.ScaleTracks.Length];
      for (int scaleIndex = 0; scaleIndex < scaleTracks.Length; ++scaleIndex)
      {
        ScaleTrack source = tracks.ScaleTracks[scaleIndex];
        scaleTracks[scaleIndex] = new EntitySceneScaleTrack(source.Frame, source.Scale, source.ScaleQuaternion);
      }

      sceneObjects[index] = new EntitySceneObject(
        obj.Parent,
        obj.Frames,
        obj.Scale,
        obj.ScaleQuaternion,
        obj.Position,
        obj.Quaternion,
        positionTracks,
        rotationTracks,
        scaleTracks);
    }

    return sceneObjects;
  }

  private static NumericsMatrix4 ResolveLegacyObjectMatrix(
    int objectIndex,
    ReadEntityAniObject[] objects,
    EntityObjectTracks[] objectTracks,
    NumericsMatrix4[] cache,
    byte[] visitState)
  {
    byte state = visitState[objectIndex];
    if (state == 2)
    {
      return cache[objectIndex];
    }

    if (state == 1)
    {
      return NumericsMatrix4.Identity;
    }

    visitState[objectIndex] = 1;
    ReadEntityAniObject obj = objects[objectIndex];

    NumericsMatrix4 parentMatrix = NumericsMatrix4.Identity;
    int parentIndex = obj.Parent - 1;
    if (parentIndex >= 0 && parentIndex < objects.Length && parentIndex != objectIndex)
    {
      parentMatrix = ResolveLegacyObjectMatrix(parentIndex, objects, objectTracks, cache, visitState);
    }

    NumericsMatrix4 localMatrix = BuildEntityObjectLocalMatrix(obj, objectTracks[objectIndex], 0.0f);
    NumericsMatrix4 worldMatrix = LegacyMatrixMultiply(parentMatrix, localMatrix);
    cache[objectIndex] = worldMatrix;
    visitState[objectIndex] = 2;
    return worldMatrix;
  }

  private static NumericsMatrix4 BuildEntityObjectLocalMatrix(ReadEntityAniObject obj, EntityObjectTracks tracks, float nowFrame)
  {
    if (tracks.PositionTracks.Length == 0 && tracks.RotationTracks.Length == 0 && tracks.ScaleTracks.Length == 0)
    {
      NumericsMatrix4 baseRotation = CreateLegacyQuaternionMatrix(obj.Quaternion);
      NumericsMatrix4 baseScaleMatrix = BuildLegacyScaleMatrix(obj.Scale, obj.ScaleQuaternion);
      NumericsMatrix4 baseLocal = LegacyMatrixMultiply(baseRotation, baseScaleMatrix);
      baseLocal.M41 = obj.Position.X;
      baseLocal.M42 = obj.Position.Y;
      baseLocal.M43 = obj.Position.Z;
      baseLocal.M44 = 1.0f;
      return baseLocal;
    }

    Vector4 rotationQuat = obj.Quaternion;
    Vector3 scale = obj.Scale;
    Vector4 scaleQuaternion = obj.ScaleQuaternion;
    Vector3 position = obj.Position;

    if (tracks.RotationTracks.Length > 0)
    {
      rotationQuat = SampleRotationTrack(tracks.RotationTracks, nowFrame);
    }

    if (tracks.ScaleTracks.Length > 0)
    {
      SampleScaleTrack(tracks.ScaleTracks, nowFrame, out scale, out scaleQuaternion);
    }

    if (tracks.PositionTracks.Length > 0)
    {
      position = SamplePositionTrack(tracks.PositionTracks, nowFrame);
    }

    NumericsMatrix4 rotation = CreateLegacyQuaternionMatrix(rotationQuat);
    NumericsMatrix4 scaleMatrix = BuildLegacyScaleMatrix(scale, scaleQuaternion);
    NumericsMatrix4 local = LegacyMatrixMultiply(rotation, scaleMatrix);
    local.M41 = position.X;
    local.M42 = position.Y;
    local.M43 = position.Z;
    local.M44 = 1.0f;
    return local;
  }

  private static EntityObjectTracks[] BuildEntityObjectTracks(ReadEntityAniObject[] objects, byte[] trackBytes)
  {
    if (objects.Length == 0)
    {
      return Array.Empty<EntityObjectTracks>();
    }

    EntityObjectTracks[] tracks = new EntityObjectTracks[objects.Length];
    for (int index = 0; index < objects.Length; ++index)
    {
      ReadEntityAniObject obj = objects[index];
      PositionTrack[] positionTracks = ReadPositionTracks(trackBytes, obj.PositionOffset, obj.PositionTrackCount);
      RotationTrack[] rotationTracks = ReadRotationTracks(trackBytes, obj.RotationOffset, obj.RotationTrackCount);
      ScaleTrack[] scaleTracks = ReadScaleTracks(trackBytes, obj.ScaleOffset, obj.ScaleTrackCount);
      tracks[index] = new EntityObjectTracks(positionTracks, rotationTracks, scaleTracks);
    }

    return tracks;
  }

  private static PositionTrack[] ReadPositionTracks(byte[] trackBytes, uint offset, int count)
  {
    if (count <= 0 || offset >= trackBytes.Length)
    {
      return Array.Empty<PositionTrack>();
    }

    const int entrySize = 16;
    long totalSize = (long)count * entrySize;
    if (totalSize <= 0 || offset + totalSize > trackBytes.Length)
    {
      return Array.Empty<PositionTrack>();
    }

    PositionTrack[] tracks = new PositionTrack[count];
    int cursor = (int)offset;
    for (int index = 0; index < count; ++index)
    {
      float frame = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor, 4)));
      float x = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 4, 4)));
      float y = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 8, 4)));
      float z = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 12, 4)));
      tracks[index] = new PositionTrack(frame, new Vector3(x, y, z));
      cursor += entrySize;
    }

    return tracks;
  }

  private static RotationTrack[] ReadRotationTracks(byte[] trackBytes, uint offset, int count)
  {
    if (count <= 0 || offset >= trackBytes.Length)
    {
      return Array.Empty<RotationTrack>();
    }

    const int entrySize = 20;
    long totalSize = (long)count * entrySize;
    if (totalSize <= 0 || offset + totalSize > trackBytes.Length)
    {
      return Array.Empty<RotationTrack>();
    }

    RotationTrack[] tracks = new RotationTrack[count];
    int cursor = (int)offset;
    for (int index = 0; index < count; ++index)
    {
      float frame = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor, 4)));
      float x = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 4, 4)));
      float y = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 8, 4)));
      float z = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 12, 4)));
      float w = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 16, 4)));
      tracks[index] = new RotationTrack(frame, new Vector4(x, y, z, w));
      cursor += entrySize;
    }

    return tracks;
  }

  private static ScaleTrack[] ReadScaleTracks(byte[] trackBytes, uint offset, int count)
  {
    if (count <= 0 || offset >= trackBytes.Length)
    {
      return Array.Empty<ScaleTrack>();
    }

    const int entrySize = 32;
    long totalSize = (long)count * entrySize;
    if (totalSize <= 0 || offset + totalSize > trackBytes.Length)
    {
      return Array.Empty<ScaleTrack>();
    }

    ScaleTrack[] tracks = new ScaleTrack[count];
    int cursor = (int)offset;
    for (int index = 0; index < count; ++index)
    {
      float frame = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor, 4)));
      float sx = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 4, 4)));
      float sy = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 8, 4)));
      float sz = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 12, 4)));
      float qx = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 16, 4)));
      float qy = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 20, 4)));
      float qz = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 24, 4)));
      float qw = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(trackBytes.AsSpan(cursor + 28, 4)));
      tracks[index] = new ScaleTrack(frame, new Vector3(sx, sy, sz), new Vector4(qx, qy, qz, qw));
      cursor += entrySize;
    }

    return tracks;
  }

  private static Vector3 SamplePositionTrack(PositionTrack[] tracks, float nowFrame)
  {
    if (tracks.Length == 0)
    {
      return Vector3.Zero;
    }

    if (tracks.Length == 1)
    {
      return tracks[0].Position;
    }

    GetTrackFrameIndices(tracks.Length, index => tracks[index].Frame, nowFrame, out int root, out int next, out float alpha);
    if (root == next)
    {
      return tracks[root].Position;
    }

    return Vector3.Lerp(tracks[root].Position, tracks[next].Position, alpha);
  }

  private static Vector4 SampleRotationTrack(RotationTrack[] tracks, float nowFrame)
  {
    if (tracks.Length == 0)
    {
      return Vector4.Zero;
    }

    if (tracks.Length == 1)
    {
      return tracks[0].Quaternion;
    }

    GetTrackFrameIndices(tracks.Length, index => tracks[index].Frame, nowFrame, out int root, out int next, out float alpha);
    if (root == next)
    {
      return tracks[root].Quaternion;
    }

    Vector4 rootQ = tracks[root].Quaternion;
    Vector4 nextQ = tracks[next].Quaternion;
    NumericsQuaternion q0 = NumericsQuaternion.Normalize(new NumericsQuaternion(rootQ.X, rootQ.Y, rootQ.Z, rootQ.W));
    NumericsQuaternion q1 = NumericsQuaternion.Normalize(new NumericsQuaternion(nextQ.X, nextQ.Y, nextQ.Z, nextQ.W));
    NumericsQuaternion q = NumericsQuaternion.Normalize(NumericsQuaternion.Slerp(q0, q1, alpha));
    if (!float.IsFinite(q.X) || !float.IsFinite(q.Y) || !float.IsFinite(q.Z) || !float.IsFinite(q.W))
    {
      return rootQ;
    }

    return new Vector4(q.X, q.Y, q.Z, q.W);
  }

  private static void SampleScaleTrack(ScaleTrack[] tracks, float nowFrame, out Vector3 scale, out Vector4 scaleQuaternion)
  {
    scale = Vector3.One;
    scaleQuaternion = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    if (tracks.Length == 0)
    {
      return;
    }

    if (tracks.Length == 1)
    {
      scale = tracks[0].Scale;
      scaleQuaternion = tracks[0].ScaleQuaternion;
      return;
    }

    GetTrackFrameIndices(tracks.Length, index => tracks[index].Frame, nowFrame, out int root, out int next, out float alpha);
    if (root == next)
    {
      scale = tracks[root].Scale;
      scaleQuaternion = tracks[root].ScaleQuaternion;
      return;
    }

    scale = Vector3.Lerp(tracks[root].Scale, tracks[next].Scale, alpha);

    Vector4 rootQ = tracks[root].ScaleQuaternion;
    Vector4 nextQ = tracks[next].ScaleQuaternion;
    NumericsQuaternion q0 = NumericsQuaternion.Normalize(new NumericsQuaternion(rootQ.X, rootQ.Y, rootQ.Z, rootQ.W));
    NumericsQuaternion q1 = NumericsQuaternion.Normalize(new NumericsQuaternion(nextQ.X, nextQ.Y, nextQ.Z, nextQ.W));
    NumericsQuaternion q = NumericsQuaternion.Normalize(NumericsQuaternion.Slerp(q0, q1, alpha));
    if (!float.IsFinite(q.X) || !float.IsFinite(q.Y) || !float.IsFinite(q.Z) || !float.IsFinite(q.W))
    {
      scaleQuaternion = rootQ;
      return;
    }

    scaleQuaternion = new Vector4(q.X, q.Y, q.Z, q.W);
  }

  private static void GetTrackFrameIndices(
    int keyCount,
    Func<int, float> frameAccessor,
    float nowFrame,
    out int root,
    out int next,
    out float alpha)
  {
    root = -1;
    next = -1;
    alpha = 0.0f;

    if (keyCount <= 0)
    {
      root = 0;
      next = 0;
      return;
    }

    for (int index = 0; index < keyCount - 1; ++index)
    {
      float rootFrame = frameAccessor(index);
      float nextFrame = frameAccessor(index + 1);
      if (rootFrame <= nowFrame && nextFrame > nowFrame)
      {
        root = index;
        next = index + 1;
        float denom = nextFrame - rootFrame;
        if (MathF.Abs(denom) > 0.000001f)
        {
          alpha = (nowFrame - rootFrame) / denom;
        }

        if (!float.IsFinite(alpha))
        {
          alpha = 0.0f;
        }
        else
        {
          alpha = Math.Clamp(alpha, 0.0f, 1.0f);
        }

        return;
      }
    }

    root = keyCount - 1;
    next = keyCount - 1;
  }

  private static NumericsMatrix4 BuildLegacyScaleMatrix(Vector3 scale, Vector4 scaleQuaternion)
  {
    NumericsMatrix4 scaleMatrix = NumericsMatrix4.Identity;
    scaleMatrix.M11 = scale.X;
    scaleMatrix.M22 = scale.Y;
    scaleMatrix.M33 = scale.Z;

    NumericsMatrix4 sqMatrix = CreateLegacyQuaternionMatrix(scaleQuaternion);
    if (!NumericsMatrix4.Invert(sqMatrix, out NumericsMatrix4 invSqMatrix))
    {
      return scaleMatrix;
    }

    NumericsMatrix4 temp = LegacyMatrixMultiply(scaleMatrix, invSqMatrix);
    return LegacyMatrixMultiply(sqMatrix, temp);
  }

  private static NumericsMatrix4 CreateLegacyQuaternionMatrix(Vector4 quaternion)
  {
    float x = quaternion.X;
    float y = quaternion.Y;
    float z = quaternion.Z;
    float w = quaternion.W;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    return new NumericsMatrix4(
      1.0f - 2.0f * (yy + zz),
      2.0f * (xy - wz),
      2.0f * (xz + wy),
      0.0f,
      2.0f * (xy + wz),
      1.0f - 2.0f * (xx + zz),
      2.0f * (yz - wx),
      0.0f,
      2.0f * (xz - wy),
      2.0f * (yz + wx),
      1.0f - 2.0f * (xx + yy),
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f);
  }

  private static NumericsMatrix4 LegacyMatrixMultiply(in NumericsMatrix4 a, in NumericsMatrix4 b)
  {
    // Legacy R3 matrix multiply stores q = b * a.
    return NumericsMatrix4.Multiply(b, a);
  }

  private static NumericsMatrix4 ConvertFrom3dsMaxMatrix(NumericsMatrix4 matrix)
  {
    (matrix.M12, matrix.M13) = (matrix.M13, matrix.M12);
    (matrix.M22, matrix.M23) = (matrix.M23, matrix.M22);
    (matrix.M32, matrix.M33) = (matrix.M33, matrix.M32);
    (matrix.M42, matrix.M43) = (matrix.M43, matrix.M42);
    return matrix;
  }

  private static bool TryGetEntityObjectMatrix(NumericsMatrix4[] objectMatrices, ushort objectId, out NumericsMatrix4 matrix)
  {
    if (objectId != 0)
    {
      int objectIndex = objectId - 1;
      if (objectIndex >= 0 && objectIndex < objectMatrices.Length)
      {
        matrix = objectMatrices[objectIndex];
        return true;
      }
    }

    matrix = NumericsMatrix4.Identity;
    return false;
  }

  private static Vector3 TransformEntityPosition(Vector3 position, in NumericsMatrix4 matrix)
  {
    NumericsVector3 transformed = NumericsVector3.Transform(new NumericsVector3(position.X, position.Y, position.Z), matrix);
    if (!float.IsFinite(transformed.X) || !float.IsFinite(transformed.Y) || !float.IsFinite(transformed.Z))
    {
      return position;
    }

    return new Vector3(transformed.X, transformed.Y, transformed.Z);
  }

  private static EntitySceneMatGroup[] BuildEntitySceneMatGroups(
    Vector3[] vertices,
    uint[] colors,
    short[] uvRaw,
    ReadEntityFace[] faces,
    ushort[] faceIds,
    ushort[] vertexIds,
    ReadEntityMatGroup[] matGroups,
    int[] matGroupDrawOrder,
    R3MaterialData materialData,
    IReadOnlyDictionary<int, int> materialNameMap,
    float uvMin,
    float uvMax)
  {
    List<EntitySceneMatGroup> sceneGroups = new(Math.Max(64, matGroups.Length));

    for (int drawIndex = 0; drawIndex < matGroupDrawOrder.Length; ++drawIndex)
    {
      int groupIndex = matGroupDrawOrder[drawIndex];
      if (groupIndex < 0 || groupIndex >= matGroups.Length)
      {
        continue;
      }

      ReadEntityMatGroup group = matGroups[groupIndex];
      int resolvedMaterialId = ResolveEntityMaterialId(group.MaterialId, materialData.Names.Length, materialNameMap);
      List<BspRenderVertex> localVertices = new(Math.Max(9, group.FaceCount * 3));

      for (int faceInGroup = 0; faceInGroup < group.FaceCount; ++faceInGroup)
      {
        int faceIdIndex = (int)group.FaceStartId + faceInGroup;
        if (faceIdIndex < 0 || faceIdIndex >= faceIds.Length)
        {
          continue;
        }

        int faceIndex = faceIds[faceIdIndex];
        if (faceIndex < 0 || faceIndex >= faces.Length)
        {
          continue;
        }

        ReadEntityFace face = faces[faceIndex];
        if (face.VertexCount < 3)
        {
          continue;
        }

        int[] localVertexIds = new int[face.VertexCount];
        int[] localUvIds = new int[face.VertexCount];
        bool validFace = true;

        for (int vertexOffset = 0; vertexOffset < face.VertexCount; ++vertexOffset)
        {
          int sourceVertexId = (int)face.VertexStartId + vertexOffset;
          if (sourceVertexId < 0 || sourceVertexId >= vertexIds.Length)
          {
            validFace = false;
            break;
          }

          int modelVertexId = vertexIds[sourceVertexId];
          if (modelVertexId < 0 || modelVertexId >= vertices.Length)
          {
            validFace = false;
            break;
          }

          localVertexIds[vertexOffset] = modelVertexId;
          localUvIds[vertexOffset] = sourceVertexId;
        }

        if (!validFace)
        {
          continue;
        }

        for (int tri = 2; tri < localVertexIds.Length; ++tri)
        {
          int rootVertexId = localVertexIds[0];
          int midVertexId = localVertexIds[tri - 1];
          int tipVertexId = localVertexIds[tri];

          localVertices.Add(CreateEntityRenderVertex(
            vertices[rootVertexId],
            localUvIds[0],
            rootVertexId,
            uvRaw,
            colors,
            resolvedMaterialId,
            materialData.MaterialFlags,
            uvMin,
            uvMax));
          localVertices.Add(CreateEntityRenderVertex(
            vertices[midVertexId],
            localUvIds[tri - 1],
            midVertexId,
            uvRaw,
            colors,
            resolvedMaterialId,
            materialData.MaterialFlags,
            uvMin,
            uvMax));
          localVertices.Add(CreateEntityRenderVertex(
            vertices[tipVertexId],
            localUvIds[tri],
            tipVertexId,
            uvRaw,
            colors,
            resolvedMaterialId,
            materialData.MaterialFlags,
            uvMin,
            uvMax));
        }
      }

      if (localVertices.Count <= 0)
      {
        continue;
      }

      sceneGroups.Add(new EntitySceneMatGroup(resolvedMaterialId, group.ObjectId, localVertices.ToArray()));
    }

    return sceneGroups.ToArray();
  }

  private static MeshBuildResult BuildEntityTriangles(
    Vector3[] vertices,
    uint[] colors,
    short[] uvRaw,
    ReadEntityFace[] faces,
    ushort[] faceIds,
    ushort[] vertexIds,
    ReadEntityMatGroup[] matGroups,
    int[] matGroupDrawOrder,
    NumericsMatrix4[] objectMatrices,
    R3MaterialData materialData,
    IReadOnlyDictionary<int, int> materialNameMap,
    float uvMin,
    float uvMax)
  {
    List<BspRenderVertex> triangles = new(Math.Max(4096, faceIds.Length * 3));
    List<BspMaterialSpan> spans = new(Math.Max(64, matGroups.Length));

    for (int drawIndex = 0; drawIndex < matGroupDrawOrder.Length; ++drawIndex)
    {
      int groupIndex = matGroupDrawOrder[drawIndex];
      if (groupIndex < 0 || groupIndex >= matGroups.Length)
      {
        continue;
      }

      ReadEntityMatGroup group = matGroups[groupIndex];
      int resolvedMaterialId = ResolveEntityMaterialId(group.MaterialId, materialData.Names.Length, materialNameMap);
      int groupStartVertex = triangles.Count;
      bool hasObjectMatrix = TryGetEntityObjectMatrix(objectMatrices, group.ObjectId, out NumericsMatrix4 objectMatrix);

      for (int faceInGroup = 0; faceInGroup < group.FaceCount; ++faceInGroup)
      {
        int faceIdIndex = (int)group.FaceStartId + faceInGroup;
        if (faceIdIndex < 0 || faceIdIndex >= faceIds.Length)
        {
          continue;
        }

        int faceIndex = faceIds[faceIdIndex];
        if (faceIndex < 0 || faceIndex >= faces.Length)
        {
          continue;
        }

        ReadEntityFace face = faces[faceIndex];
        if (face.VertexCount < 3)
        {
          continue;
        }

        int[] localVertexIds = new int[face.VertexCount];
        int[] localUvIds = new int[face.VertexCount];
        bool validFace = true;

        for (int vertexOffset = 0; vertexOffset < face.VertexCount; ++vertexOffset)
        {
          int sourceVertexId = (int)face.VertexStartId + vertexOffset;
          if (sourceVertexId < 0 || sourceVertexId >= vertexIds.Length)
          {
            validFace = false;
            break;
          }

          int modelVertexId = vertexIds[sourceVertexId];
          if (modelVertexId < 0 || modelVertexId >= vertices.Length)
          {
            validFace = false;
            break;
          }

          localVertexIds[vertexOffset] = modelVertexId;
          localUvIds[vertexOffset] = sourceVertexId;
        }

        if (!validFace)
        {
          continue;
        }

        for (int tri = 2; tri < localVertexIds.Length; ++tri)
        {
          int rootVertexId = localVertexIds[0];
          int midVertexId = localVertexIds[tri - 1];
          int tipVertexId = localVertexIds[tri];
          Vector3 rootPosition = vertices[rootVertexId];
          Vector3 midPosition = vertices[midVertexId];
          Vector3 tipPosition = vertices[tipVertexId];
          if (hasObjectMatrix)
          {
            rootPosition = TransformEntityPosition(rootPosition, objectMatrix);
            midPosition = TransformEntityPosition(midPosition, objectMatrix);
            tipPosition = TransformEntityPosition(tipPosition, objectMatrix);
          }

          triangles.Add(CreateEntityRenderVertex(
            rootPosition,
            localUvIds[0],
            rootVertexId,
            uvRaw,
            colors,
            resolvedMaterialId,
            materialData.MaterialFlags,
            uvMin,
            uvMax));
          triangles.Add(CreateEntityRenderVertex(
            midPosition,
            localUvIds[tri - 1],
            midVertexId,
            uvRaw,
            colors,
            resolvedMaterialId,
            materialData.MaterialFlags,
            uvMin,
            uvMax));
          triangles.Add(CreateEntityRenderVertex(
            tipPosition,
            localUvIds[tri],
            tipVertexId,
            uvRaw,
            colors,
            resolvedMaterialId,
            materialData.MaterialFlags,
            uvMin,
            uvMax));
        }
      }

      int groupVertexCount = triangles.Count - groupStartVertex;
      if (groupVertexCount > 0)
      {
        AddOrMergeSpan(spans, new BspMaterialSpan(resolvedMaterialId, groupStartVertex, groupVertexCount));
      }
    }

    return new MeshBuildResult(triangles.ToArray(), spans.ToArray());
  }

  private static BspRenderVertex CreateEntityRenderVertex(
    Vector3 position,
    int uvId,
    int vertexId,
    short[] uvRaw,
    uint[] colors,
    int materialId,
    uint[] materialFlags,
    float uvMin,
    float uvMax)
  {
    Vector2 uv = DecodeEntityUv(uvRaw, uvId, uvMin, uvMax);
    Vector3 color = DecodeEntityColor(colors, vertexId, materialId, materialFlags);
    return new BspRenderVertex(position, uv, uv, color);
  }

  private static Vector2 DecodeEntityUv(short[] uvRaw, int uvId, float uvMin, float uvMax)
  {
    int offset = uvId * 2;
    if (offset < 0 || offset + 1 >= uvRaw.Length)
    {
      return Vector2.Zero;
    }

    float uvScale = (uvMax - uvMin) * 0.5f;
    float uvPos = uvMin + uvScale;
    float u = (uvRaw[offset] / 32767.0f) * uvScale + uvPos;
    float v = (uvRaw[offset + 1] / 32767.0f) * uvScale + uvPos;
    if (!float.IsFinite(u) || !float.IsFinite(v))
    {
      return Vector2.Zero;
    }

    return new Vector2(u, v);
  }

  private static Vector3 DecodeEntityColor(uint[] colors, int vertexId, int materialId, uint[] materialFlags)
  {
    bool forceGray = materialId >= 0
      && materialId < materialFlags.Length
      && (materialFlags[materialId] & 1u) != 0;

    if (forceGray)
    {
      return new Vector3(0.5f, 0.5f, 0.5f);
    }

    if (vertexId < 0 || vertexId >= colors.Length)
    {
      return new Vector3(0.7f, 0.72f, 0.76f);
    }

    uint argb = colors[vertexId];
    float r = ((argb >> 16) & 0xFF) / 255.0f;
    float g = ((argb >> 8) & 0xFF) / 255.0f;
    float b = (argb & 0xFF) / 255.0f;
    return new Vector3(r, g, b);
  }

  private static int ResolveEntityMaterialId(short rawMaterialId, int materialCount, IReadOnlyDictionary<int, int> materialNameMap)
  {
    int materialId = rawMaterialId;
    if (materialId >= 0 && materialId < materialCount)
    {
      return materialId;
    }

    if (materialId > 0 && materialId - 1 < materialCount)
    {
      return materialId - 1;
    }

    if (materialNameMap.TryGetValue(materialId, out int mapped))
    {
      return mapped;
    }

    return materialId;
  }

  private static readonly Dictionary<int, int> EmptyMaterialNameMap = new();

  private static Dictionary<int, int> BuildMaterialNameNumberMap(string[] materialNames)
  {
    Dictionary<int, int> map = new();
    for (int index = 0; index < materialNames.Length; ++index)
    {
      int number = ParseMaterialNameNumber(materialNames[index]);
      if (number < 0)
      {
        continue;
      }

      map[number] = index;
    }

    return map;
  }

  private static int ParseMaterialNameNumber(string name)
  {
    if (string.IsNullOrEmpty(name))
    {
      return -1;
    }

    int underscore = name.LastIndexOf('_');
    string tail = underscore >= 0 ? name[(underscore + 1)..] : name;
    if (tail.Length == 0)
    {
      return -1;
    }

    int digitStart = 0;
    while (digitStart < tail.Length && !char.IsDigit(tail[digitStart]))
    {
      ++digitStart;
    }

    if (digitStart >= tail.Length)
    {
      return -1;
    }

    int digitEnd = digitStart;
    while (digitEnd < tail.Length && char.IsDigit(tail[digitEnd]))
    {
      ++digitEnd;
    }

    return int.TryParse(tail[digitStart..digitEnd], out int value) ? value : -1;
  }

  private static int[] BuildEntityMatGroupDrawOrder(
    ReadEntityMatGroup[] matGroups,
    string[] materialNames,
    bool useMaterialNameFallback)
  {
    if (matGroups.Length == 0)
    {
      return Array.Empty<int>();
    }

    int[] defaultOrder = new int[matGroups.Length];
    for (int index = 0; index < defaultOrder.Length; ++index)
    {
      defaultOrder[index] = index;
    }

    if (!useMaterialNameFallback || materialNames.Length == 0)
    {
      return defaultOrder;
    }

    bool[] added = new bool[matGroups.Length];
    List<int> ordered = new(matGroups.Length);

    for (int materialIndex = 0; materialIndex < materialNames.Length; ++materialIndex)
    {
      int materialNumber = ParseMaterialNameNumber(materialNames[materialIndex]);
      if (materialNumber < 0)
      {
        continue;
      }

      for (int groupIndex = 0; groupIndex < matGroups.Length; ++groupIndex)
      {
        if (added[groupIndex])
        {
          continue;
        }

        if (matGroups[groupIndex].MaterialId == materialNumber)
        {
          added[groupIndex] = true;
          ordered.Add(groupIndex);
        }
      }
    }

    for (int groupIndex = 0; groupIndex < matGroups.Length; ++groupIndex)
    {
      if (!added[groupIndex])
      {
        ordered.Add(groupIndex);
      }
    }

    return ordered.Count == matGroups.Length ? ordered.ToArray() : defaultOrder;
  }

  private static MapBounds ComputeBoundsFromVertices(Vector3[] vertices)
  {
    if (vertices.Length == 0)
    {
      return new MapBounds(new Vector3(-100, -100, -100), new Vector3(100, 100, 100));
    }

    bool found = false;
    Vector3 min = Vector3.Zero;
    Vector3 max = Vector3.Zero;

    for (int index = 0; index < vertices.Length; ++index)
    {
      Vector3 v = vertices[index];
      if (!IsReasonable(v))
      {
        continue;
      }

      if (!found)
      {
        min = v;
        max = v;
        found = true;
      }
      else
      {
        min = Vector3.ComponentMin(min, v);
        max = Vector3.ComponentMax(max, v);
      }
    }

    if (!found)
    {
      return new MapBounds(new Vector3(-100, -100, -100), new Vector3(100, 100, 100));
    }

    return new MapBounds(min, max);
  }

  private static bool IsReasonable(Vector3 v)
  {
    if (!float.IsFinite(v.X) || !float.IsFinite(v.Y) || !float.IsFinite(v.Z))
    {
      return false;
    }

    const float limit = 500000f;
    return MathF.Abs(v.X) < limit && MathF.Abs(v.Y) < limit && MathF.Abs(v.Z) < limit;
  }

  private static bool IsFinite(Vector3 v)
  {
    return float.IsFinite(v.X) && float.IsFinite(v.Y) && float.IsFinite(v.Z);
  }

  private readonly record struct BspHeader(
    uint Version,
    BspEntry CPlanes,
    BspEntry CFaceId,
    BspEntry Node,
    BspEntry Leaf,
    BspEntry MatListInLeaf,
    BspEntry Object,
    BspEntry Track,
    BspEntry EventObjectId,
    uint ReadSpareTotalSize,
    BspEntry BVertex,
    BspEntry WVertex,
    BspEntry FVertex,
    BspEntry VertexColor,
    BspEntry Uv,
    BspEntry LgtUv,
    BspEntry Face,
    BspEntry FaceId,
    BspEntry VertexId,
    BspEntry ReadMatGroup);

  private readonly record struct ReadFace(ushort VertexCount, uint VertexStartId);

  private readonly record struct ReadMatGroup(
    ushort Attribute,
    ushort FaceCount,
    uint FaceStartId,
    short MaterialId,
    short LightMapId,
    Vector3 Position,
    float Scale);

  private readonly record struct ExtEntityDefinition(bool IsParticle, string Name);

  private readonly record struct ExtMapEntity(int EntityId, float Scale, Vector3 Position, float RotX, float RotY);

  private readonly record struct ParticleScriptData(string[] EntityFileReferences, uint AlphaType, bool DisableDepthTest);

  private readonly record struct ReadEntityFace(ushort VertexCount, uint VertexStartId);

  private readonly record struct ReadEntityMatGroup(ushort FaceCount, uint FaceStartId, short MaterialId, ushort ObjectId);

  private readonly record struct ReadEntityAniObject(
    ushort Flag,
    ushort Parent,
    int Frames,
    int PositionTrackCount,
    int RotationTrackCount,
    int ScaleTrackCount,
    Vector3 Scale,
    Vector4 ScaleQuaternion,
    Vector3 Position,
    Vector4 Quaternion,
    uint PositionOffset,
    uint RotationOffset,
    uint ScaleOffset);

  private readonly record struct PositionTrack(float Frame, Vector3 Position);
  private readonly record struct RotationTrack(float Frame, Vector4 Quaternion);
  private readonly record struct ScaleTrack(float Frame, Vector3 Scale, Vector4 ScaleQuaternion);
  private readonly record struct EntityObjectTracks(
    PositionTrack[] PositionTracks,
    RotationTrack[] RotationTracks,
    ScaleTrack[] ScaleTracks);

  private readonly record struct EntityCompHeader(ushort Type, Vector3 Position, float Scale, float UvMin, float UvMax);

  private readonly record struct EntityFileHeader(
    BspEntry CompHeader,
    BspEntry Vertex,
    BspEntry VertexColor,
    BspEntry Uv,
    BspEntry Face,
    BspEntry FaceId,
    BspEntry VertexId,
    BspEntry MatGroup,
    BspEntry Object,
    BspEntry Track);

  private readonly record struct ModelRemapData(EntityMeshData Model, int MaterialBase);

  private sealed class BspData
  {
    public BspData(Vector3[] triangleVertices, BspRenderVertex[] renderVertices, BspMaterialSpan[] materialSpans)
    {
      TriangleVertices = triangleVertices;
      RenderVertices = renderVertices;
      MaterialSpans = materialSpans;
    }

    public Vector3[] TriangleVertices { get; }
    public BspRenderVertex[] RenderVertices { get; }
    public BspMaterialSpan[] MaterialSpans { get; }
  }

  private sealed class MeshBuildResult
  {
    public MeshBuildResult(BspRenderVertex[] vertices, BspMaterialSpan[] materialSpans)
    {
      Vertices = vertices;
      MaterialSpans = materialSpans;
    }

    public BspRenderVertex[] Vertices { get; }
    public BspMaterialSpan[] MaterialSpans { get; }
  }

  private sealed class ExtBspData
  {
    public ExtBspData(
      Vector3[] collisionVertices,
      CollisionLine[] collisionLines,
      ExtEntityDefinition[] entityDefinitions,
      ExtMapEntity[] mapEntities)
    {
      CollisionVertices = collisionVertices;
      CollisionLines = collisionLines;
      EntityDefinitions = entityDefinitions;
      MapEntities = mapEntities;
    }

    public Vector3[] CollisionVertices { get; }
    public CollisionLine[] CollisionLines { get; }
    public ExtEntityDefinition[] EntityDefinitions { get; }
    public ExtMapEntity[] MapEntities { get; }
  }

  private sealed class MaterialData
  {
    public MaterialData(
      MaterialDefinition[] materials,
      int[] materialSurfaceIds,
      uint[] materialAlphaTypes,
      R3TextureBlob[] surfaceTextures)
    {
      Materials = materials;
      MaterialSurfaceIds = materialSurfaceIds;
      MaterialAlphaTypes = materialAlphaTypes;
      SurfaceTextures = surfaceTextures;
    }

    public MaterialDefinition[] Materials { get; }
    public int[] MaterialSurfaceIds { get; }
    public uint[] MaterialAlphaTypes { get; }
    public R3TextureBlob[] SurfaceTextures { get; }
  }

  private sealed class R3MaterialData
  {
    public static R3MaterialData Empty { get; } = new(
      Array.Empty<string>(),
      Array.Empty<MaterialDefinition>(),
      Array.Empty<int>(),
      Array.Empty<uint>(),
      Array.Empty<uint>(),
      Array.Empty<uint>());

    public R3MaterialData(
      string[] names,
      MaterialDefinition[] materials,
      int[] surfaceIds,
      uint[] materialFlags,
      uint[] layerAlphaTypes,
      uint[] layerArgb)
    {
      Names = names;
      Materials = materials;
      SurfaceIds = surfaceIds;
      MaterialFlags = materialFlags;
      LayerAlphaTypes = layerAlphaTypes;
      LayerArgb = layerArgb;
    }

    public string[] Names { get; }
    public MaterialDefinition[] Materials { get; }
    public int[] SurfaceIds { get; }
    public uint[] MaterialFlags { get; }
    public uint[] LayerAlphaTypes { get; }
    public uint[] LayerArgb { get; }
  }

  private sealed class RenderMeshData
  {
    public static RenderMeshData Empty { get; } = new(
      Array.Empty<BspRenderVertex>(),
      Array.Empty<BspMaterialSpan>(),
      Array.Empty<MaterialDefinition>(),
      Array.Empty<int>(),
      Array.Empty<uint>(),
      Array.Empty<R3TextureBlob>());

    public RenderMeshData(
      BspRenderVertex[] vertices,
      BspMaterialSpan[] materialSpans,
      MaterialDefinition[] materials,
      int[] materialSurfaceIds,
      uint[] materialAlphaTypes,
      R3TextureBlob[] surfaceTextures)
    {
      Vertices = vertices;
      MaterialSpans = materialSpans;
      Materials = materials;
      MaterialSurfaceIds = materialSurfaceIds;
      MaterialAlphaTypes = materialAlphaTypes;
      SurfaceTextures = surfaceTextures;
    }

    public BspRenderVertex[] Vertices { get; }
    public BspMaterialSpan[] MaterialSpans { get; }
    public MaterialDefinition[] Materials { get; }
    public int[] MaterialSurfaceIds { get; }
    public uint[] MaterialAlphaTypes { get; }
    public R3TextureBlob[] SurfaceTextures { get; }
  }

  private sealed class SkyData
  {
    public static SkyData Empty { get; } = new(RenderMeshData.Empty);

    public SkyData(RenderMeshData mesh)
    {
      Mesh = mesh;
    }

    public RenderMeshData Mesh { get; }
  }

  private sealed class EntityRenderData
  {
    public static EntityRenderData Empty { get; } = new(RenderMeshData.Empty, EntitySceneData.Empty, 0, 0, Array.Empty<Vector3>());

    public EntityRenderData(
      RenderMeshData mesh,
      EntitySceneData sceneData,
      int modelCount,
      int instanceCount,
      Vector3[] particlePositions)
    {
      Mesh = mesh;
      SceneData = sceneData;
      ModelCount = modelCount;
      InstanceCount = instanceCount;
      ParticlePositions = particlePositions;
    }

    public RenderMeshData Mesh { get; }
    public EntitySceneData SceneData { get; }
    public int ModelCount { get; }
    public int InstanceCount { get; }
    public Vector3[] ParticlePositions { get; }
  }

  private sealed class EntityMeshData
  {
    public EntityMeshData(
      string name,
      BspRenderVertex[] vertices,
      BspMaterialSpan[] materialSpans,
      MaterialDefinition[] materials,
      int[] materialSurfaceIds,
      uint[] materialAlphaTypes,
      R3TextureBlob[] surfaceTextures,
      EntitySceneObject[] sceneObjects,
      EntitySceneMatGroup[] sceneMatGroups)
    {
      Name = name;
      Vertices = vertices;
      MaterialSpans = materialSpans;
      Materials = materials;
      MaterialSurfaceIds = materialSurfaceIds;
      MaterialAlphaTypes = materialAlphaTypes;
      SurfaceTextures = surfaceTextures;
      SceneObjects = sceneObjects;
      SceneMatGroups = sceneMatGroups;
    }

    public string Name { get; }
    public BspRenderVertex[] Vertices { get; }
    public BspMaterialSpan[] MaterialSpans { get; }
    public MaterialDefinition[] Materials { get; }
    public int[] MaterialSurfaceIds { get; }
    public uint[] MaterialAlphaTypes { get; }
    public R3TextureBlob[] SurfaceTextures { get; }
    public EntitySceneObject[] SceneObjects { get; }
    public EntitySceneMatGroup[] SceneMatGroups { get; }
  }

  private sealed class RpkArchiveManager
  {
    private readonly List<RpkArchive> _archives;
    public int ArchiveCount => _archives.Count;
    public int AssetCount { get; private set; }

    public RpkArchiveManager(string directoryPath)
    {
      _archives = new List<RpkArchive>();
      if (!Directory.Exists(directoryPath))
      {
        return;
      }

      foreach (string path in Directory.EnumerateFiles(directoryPath, "*.rpk"))
      {
        try
        {
          RpkArchive archive = new(path);
          _archives.Add(archive);
          AssetCount += archive.FileNodeCount;
        }
        catch
        {
        }
      }
    }

    public bool TryReadAsset(string virtualPath, out byte[] bytes)
    {
      string normalized = virtualPath.Replace('/', '\\').ToLowerInvariant();
      for (int i = 0; i < _archives.Count; ++i)
      {
        try
        {
          if (_archives[i].TryReadAsset(normalized, out bytes))
          {
            return true;
          }
        }
        catch
        {
          // Some archives contain malformed directory/file entries for the same path.
          // Ignore the broken archive and continue searching the remaining archives.
        }
      }

      bytes = Array.Empty<byte>();
      return false;
    }
  }

  private sealed class RpkArchive
  {
    private readonly string _archivePath;
    private readonly uint[] _index;
    private readonly uint[] _offset;
    private readonly MergeNode[] _nodes;
    private readonly int _headerSize;
    public int FileNodeCount { get; }

    public RpkArchive(string archivePath)
    {
      _archivePath = archivePath;
      using FileStream stream = File.OpenRead(archivePath);
      using BinaryReader reader = new(stream);

      float version = reader.ReadSingle();
      if (MathF.Abs(version - 1.0f) > 0.0001f)
      {
        throw new InvalidDataException($"Invalid RPK version in {archivePath}.");
      }

      int count = reader.ReadInt32();
      if (count <= 0 || count > 500000)
      {
        throw new InvalidDataException($"Invalid RPK entry count in {archivePath}.");
      }

      _index = new uint[count];
      _offset = new uint[count];
      _nodes = new MergeNode[count];
      for (int i = 0; i < count; ++i)
      {
        _index[i] = reader.ReadUInt32();
      }
      for (int i = 0; i < count; ++i)
      {
        _offset[i] = reader.ReadUInt32();
      }
      for (int i = 0; i < count; ++i)
      {
        byte[] nameBytes = ReadExactBytes(reader, 52, "RPK node name");
        uint fileLength = reader.ReadUInt32();
        ushort nameCount = reader.ReadUInt16();
        ushort childCount = reader.ReadUInt16();
        uint childStart = reader.ReadUInt32();
        _nodes[i] = new MergeNode(nameBytes, fileLength, nameCount, childCount, childStart);
      }

      _headerSize = 8 + count * 72;
      int fileNodeCount = 0;
      for (int i = 0; i < _offset.Length; ++i)
      {
        if (_offset[i] != 0xFFFFFFFF)
        {
          ++fileNodeCount;
        }
      }

      FileNodeCount = fileNodeCount;
    }

    public bool TryReadAsset(string virtualPath, out byte[] bytes)
    {
      if (!TryFindNode(0, virtualPath, out int nodeIndex))
      {
        bytes = Array.Empty<byte>();
        return false;
      }

      uint fileOffset = _offset[nodeIndex];
      if (fileOffset == 0xFFFFFFFF)
      {
        bytes = Array.Empty<byte>();
        return false;
      }

      using FileStream stream = File.OpenRead(_archivePath);
      long dataOffset = _headerSize + fileOffset;
      if (dataOffset < 0 || dataOffset > stream.Length)
      {
        bytes = Array.Empty<byte>();
        return false;
      }

      int length = 0;
      uint rawLength = _nodes[nodeIndex].FileLength;
      bool hasValidLength = rawLength > 0 && rawLength <= int.MaxValue && dataOffset + rawLength <= stream.Length;
      if (hasValidLength)
      {
        length = (int)rawLength;
      }
      else
      {
        long dataSectionLength = stream.Length - _headerSize;
        if (!TryResolveLengthFromOffsets(fileOffset, dataSectionLength, out length))
        {
          bytes = Array.Empty<byte>();
          return false;
        }
      }

      if (length <= 0 || dataOffset + length > stream.Length)
      {
        bytes = Array.Empty<byte>();
        return false;
      }

      stream.Position = dataOffset;
      bytes = new byte[length];
      int read = stream.Read(bytes, 0, length);
      if (read != length)
      {
        bytes = Array.Empty<byte>();
        return false;
      }

      return true;
    }

    private bool TryResolveLengthFromOffsets(uint fileOffset, long dataSectionLength, out int length)
    {
      length = 0;
      if (dataSectionLength <= 0 || fileOffset >= dataSectionLength)
      {
        return false;
      }

      uint nextOffset = uint.MaxValue;
      for (int i = 0; i < _offset.Length; ++i)
      {
        uint current = _offset[i];
        if (current == 0xFFFFFFFF || current <= fileOffset)
        {
          continue;
        }

        if (current < nextOffset)
        {
          nextOffset = current;
        }
      }

      long endOffset = nextOffset == uint.MaxValue ? dataSectionLength : nextOffset;
      long candidate = endOffset - fileOffset;
      if (candidate <= 0 || candidate > int.MaxValue)
      {
        return false;
      }

      length = (int)candidate;
      return true;
    }

    private bool TryFindNode(int nodeIndex, string path, out int resultNodeIndex)
    {
      resultNodeIndex = -1;
      if (string.IsNullOrEmpty(path))
      {
        return false;
      }

      int consumed;
      string segment = GetSegment(path, out consumed);
      if (segment.Length == 0)
      {
        return false;
      }

      MergeNode node = _nodes[nodeIndex];
      for (int i = 0; i < node.ChildCount; ++i)
      {
        int childIndexOffset = checked((int)(node.ChildStartIndex + i));
        if (childIndexOffset < 0 || childIndexOffset >= _index.Length)
        {
          continue;
        }

        int childNodeIndex = checked((int)_index[childIndexOffset]);
        if (childNodeIndex < 0 || childNodeIndex >= _nodes.Length)
        {
          continue;
        }

        MergeNode child = _nodes[childNodeIndex];
        if (!NameMatches(child, segment))
        {
          continue;
        }

        string next = consumed < path.Length && path[consumed] == '\\'
          ? path[(consumed + 1)..]
          : path[consumed..];
        bool hasMoreSegments = next.Length > 0;

        if (!hasMoreSegments && _offset[childNodeIndex] != 0xFFFFFFFF)
        {
          resultNodeIndex = childNodeIndex;
          return true;
        }

        if (hasMoreSegments && TryFindNode(childNodeIndex, next, out resultNodeIndex))
        {
          return true;
        }
      }

      return false;
    }

    private static string GetSegment(string path, out int consumed)
    {
      int i = 0;
      while (i < path.Length && path[i] == '\\')
      {
        ++i;
      }

      int start = i;
      while (i < path.Length && path[i] != '\\')
      {
        ++i;
      }

      consumed = i;
      return i > start ? path[start..i] : string.Empty;
    }

    private static bool NameMatches(MergeNode node, string segment)
    {
      if (node.NameCount != segment.Length)
      {
        return false;
      }

      byte[] nameBytes = Encoding.ASCII.GetBytes(segment);
      for (int i = 0; i < node.NameCount; ++i)
      {
        if (ToLowerAscii(node.NameBytes[i]) != ToLowerAscii(nameBytes[i]))
        {
          return false;
        }
      }

      return true;
    }

    private static byte ToLowerAscii(byte value)
    {
      if (value >= (byte)'A' && value <= (byte)'Z')
      {
        return (byte)(value + 32);
      }

      return value;
    }
  }

  private readonly record struct MergeNode(
    byte[] NameBytes,
    uint FileLength,
    int NameCount,
    int ChildCount,
    uint ChildStartIndex);
}
