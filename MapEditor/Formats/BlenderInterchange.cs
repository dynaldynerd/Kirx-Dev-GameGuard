using System.Text.Json;
using System.Text.Json.Serialization;
using System.Buffers.Binary;
using System.Globalization;
using OpenTK.Mathematics;

namespace MapEditor.Formats;

public readonly record struct BlenderPackageImportReport(
  int ImportedMeshTriangleCount,
  int ImportedCollisionLineCount,
  int ImportedMaterialCount,
  int ImportedTextureCount,
  int ImportedEntityInstanceCount,
  string[] Warnings);

public static class BlenderInterchange
{
  private const string PackageVersion = "1";
  private const string ManifestFileName = "rf_map_package.json";
  private const string MaterialsFileName = "map_materials.json";
  private const string TexturesIndexFileName = "map_textures.json";
  private const string EntitiesFileName = "entity_instances.json";
  private const string MeshObjFileName = "map_mesh.obj";
  private const string MeshMtlFileName = "map_mesh.mtl";
  private const string TexturesDirectoryName = "MapTextures";
  private const int BspHeaderEntryCount = 85;
  private const int BspSectionBVertex = 43;
  private const int BspSectionWVertex = 44;
  private const int BspSectionFVertex = 45;
  private const int BspSectionVertexColor = 46;
  private const int BspSectionUv = 47;
  private const int BspSectionLgtUv = 48;
  private const int BspSectionFace = 49;
  private const int BspSectionFaceId = 50;
  private const int BspSectionVertexId = 51;
  private const int BspSectionReadMatGroup = 52;
  private const float CollisionFaceMaxAbsNormalY = 0.55f;
  private const float CollisionEdgeMinHorizontalLength = 25.0f;
  private const float CollisionMinWallHeight = 120.0f;
  private const float CollisionBottomEdgeTolerance = 8.0f;
  private const float CollisionQuantizeScale = 1.0f;
  private const int MaxCollisionLines = ushort.MaxValue - 1;
  private const int MaxCollisionVertices = ushort.MaxValue;

  private static readonly JsonSerializerOptions JsonOptions = new()
  {
    WriteIndented = true,
    PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
  };

  public static void ExportPackage(LoadedMap map, string packageDirectoryPath)
  {
    if (map == null)
    {
      throw new ArgumentNullException(nameof(map));
    }

    if (string.IsNullOrWhiteSpace(packageDirectoryPath))
    {
      throw new ArgumentException("Package directory path is required.", nameof(packageDirectoryPath));
    }

    string packageDirectory = Path.GetFullPath(packageDirectoryPath);
    Directory.CreateDirectory(packageDirectory);

    string texturesDirectory = Path.Combine(packageDirectory, TexturesDirectoryName);
    Directory.CreateDirectory(texturesDirectory);

    string[] oldTextures = Directory.GetFiles(texturesDirectory, "*.dds", SearchOption.TopDirectoryOnly);
    for (int i = 0; i < oldTextures.Length; ++i)
    {
      File.Delete(oldTextures[i]);
    }

    List<TextureIndexEntry> textureIndex = WriteTextureSet(map.SurfaceTextures, texturesDirectory);
    WriteMeshObjAndMtl(map, packageDirectory, textureIndex);

    MapPackageManifest manifest = new()
    {
      PackageVersion = PackageVersion,
      MapName = map.Name,
      ExportedUtc = DateTime.UtcNow,
      MaterialCount = map.Materials.Length,
      TextureCount = map.SurfaceTextures.Length,
      EntityModelCount = map.EntityScene.Models.Length,
      EntityInstanceCount = map.EntityScene.Instances.Length,
      Notes =
        "Materials/textures/mesh import are save-ready; collision rebuild from mesh is optional (experimental). " +
        "Entity instances import currently updates viewer scene only.",
    };

    string manifestPath = Path.Combine(packageDirectory, ManifestFileName);
    string materialsPath = Path.Combine(packageDirectory, MaterialsFileName);
    string texturesIndexPath = Path.Combine(packageDirectory, TexturesIndexFileName);
    string entitiesPath = Path.Combine(packageDirectory, EntitiesFileName);

    File.WriteAllText(manifestPath, JsonSerializer.Serialize(manifest, JsonOptions));
    File.WriteAllText(materialsPath, JsonSerializer.Serialize(map.Materials, JsonOptions));
    File.WriteAllText(texturesIndexPath, JsonSerializer.Serialize(textureIndex, JsonOptions));
    File.WriteAllText(entitiesPath, JsonSerializer.Serialize(BuildEntityExport(map.EntityScene), JsonOptions));
  }

  public static LoadedMap ImportPackage(
    LoadedMap source,
    string packageDirectoryPath,
    bool rebuildCollisionFromMesh,
    out BlenderPackageImportReport report)
  {
    if (source == null)
    {
      throw new ArgumentNullException(nameof(source));
    }

    if (string.IsNullOrWhiteSpace(packageDirectoryPath))
    {
      throw new ArgumentException("Package directory path is required.", nameof(packageDirectoryPath));
    }

    string packageDirectory = Path.GetFullPath(packageDirectoryPath);
    if (!Directory.Exists(packageDirectory))
    {
      throw new DirectoryNotFoundException($"Package directory does not exist: {packageDirectory}");
    }

    List<string> warnings = new();
    string materialsPath = Path.Combine(packageDirectory, MaterialsFileName);
    string texturesIndexPath = Path.Combine(packageDirectory, TexturesIndexFileName);
    string entitiesPath = Path.Combine(packageDirectory, EntitiesFileName);
    string texturesDirectory = Path.Combine(packageDirectory, TexturesDirectoryName);
    string? meshObjPath = ResolveMeshObjPath(packageDirectory, warnings);

    LoadedMap edited = source;
    int importedMeshTriangleCount = 0;
    int importedCollisionLineCount = 0;
    int importedMaterialCount = 0;
    int importedTextureCount = 0;
    int importedEntityCount = 0;

    if (File.Exists(materialsPath))
    {
      MaterialDefinition[]? importedMaterials = JsonSerializer.Deserialize<MaterialDefinition[]>(File.ReadAllText(materialsPath), JsonOptions);
      if (importedMaterials != null)
      {
        edited = MapEditOperations.WithMapMaterialData(edited, importedMaterials);
        importedMaterialCount = importedMaterials.Length;
      }
    }

    if (File.Exists(texturesIndexPath) && Directory.Exists(texturesDirectory))
    {
      List<TextureIndexEntry>? textureEntries =
        JsonSerializer.Deserialize<List<TextureIndexEntry>>(File.ReadAllText(texturesIndexPath), JsonOptions);
      if (textureEntries != null && textureEntries.Count > 0)
      {
        List<R3TextureBlob> importedTextures = new(textureEntries.Count);
        for (int index = 0; index < textureEntries.Count; ++index)
        {
          TextureIndexEntry entry = textureEntries[index];
          string fileName = entry.FileName ?? string.Empty;
          if (string.IsNullOrWhiteSpace(fileName))
          {
            warnings.Add($"Texture index {index}: missing file name, skipped.");
            continue;
          }

          string texturePath = Path.Combine(texturesDirectory, fileName);
          if (!File.Exists(texturePath))
          {
            warnings.Add($"Texture file not found: {fileName}, skipped.");
            continue;
          }

          byte[] bytes;
          try
          {
            bytes = File.ReadAllBytes(texturePath);
          }
          catch (Exception ex)
          {
            warnings.Add($"Texture read failed ({fileName}): {ex.Message}");
            continue;
          }

          if (!IsDds(bytes))
          {
            warnings.Add($"Texture file is not DDS: {fileName}, skipped.");
            continue;
          }

          importedTextures.Add(new R3TextureBlob(
            entry.SurfaceId,
            entry.Name ?? string.Empty,
            bytes));
        }

        if (importedTextures.Count > 0)
        {
          edited = MapEditOperations.WithMapTextureData(edited, importedTextures.ToArray());
          importedTextureCount = importedTextures.Count;
        }
      }
    }

    if (!string.IsNullOrWhiteSpace(meshObjPath) && File.Exists(meshObjPath))
    {
      ObjMeshData mesh = ReadObjMesh(meshObjPath, warnings);
      if (mesh.Vertices.Length > 0 && (mesh.Vertices.Length % 3) == 0)
      {
        if (IsMeshGeometryEquivalent(edited.BspRenderVertices, mesh.Vertices))
        {
          importedMeshTriangleCount = mesh.Vertices.Length / 3;
          warnings.Add("OBJ geometry matches current BSP mesh; skipped mesh/collision rebuild.");
        }
        else
        {
          edited = ApplyImportedMesh(edited, mesh, warnings);
          importedMeshTriangleCount = mesh.Vertices.Length / 3;
          if (rebuildCollisionFromMesh)
          {
            (Vector3[] collisionVertices, CollisionLine[] collisionLines) = BuildAutoCollisionFromMesh(edited.BspRenderVertices, edited.Bounds, warnings);
            if (collisionLines.Length > 1)
            {
              edited = MapEditOperations.WithCollisionData(edited, collisionVertices, collisionLines);
              importedCollisionLineCount = collisionLines.Length - 1;
            }
            else
            {
              warnings.Add("Auto collision rebuild produced no lines; keeping existing collision data.");
            }
          }
          else
          {
            warnings.Add("Collision kept unchanged (auto rebuild disabled).");
          }
        }
      }
      else
      {
        warnings.Add("map_mesh.obj contains no valid triangle faces; mesh import skipped.");
      }
    }

    if (File.Exists(entitiesPath))
    {
      EntityPackage? importedEntities = JsonSerializer.Deserialize<EntityPackage>(File.ReadAllText(entitiesPath), JsonOptions);
      if (importedEntities?.Instances != null)
      {
        HashSet<int> knownModelIds = new();
        for (int modelIndex = 0; modelIndex < edited.EntityScene.Models.Length; ++modelIndex)
        {
          knownModelIds.Add(edited.EntityScene.Models[modelIndex].EntityId);
        }

        List<EntitySceneInstance> instances = new(importedEntities.Instances.Length);
        for (int index = 0; index < importedEntities.Instances.Length; ++index)
        {
          EntityInstanceEntry instance = importedEntities.Instances[index];
          if (!knownModelIds.Contains(instance.EntityId))
          {
            warnings.Add($"Entity instance {index}: unknown model id {instance.EntityId}, skipped.");
            continue;
          }

          instances.Add(new EntitySceneInstance(
            instance.EntityId,
            instance.Scale,
            new Vector3(instance.PositionX, instance.PositionY, instance.PositionZ),
            instance.RotX,
            instance.RotY));
        }

        if (instances.Count > 0)
        {
          EntitySceneData scene = new()
          {
            Models = edited.EntityScene.Models,
            Instances = instances.ToArray(),
          };
          edited = MapEditOperations.WithEntitySceneData(edited, scene);
          importedEntityCount = instances.Count;
          warnings.Add("Entity import updated viewer scene only. Saving entity placement to EBP is not implemented yet.");
        }
      }
    }

    report = new BlenderPackageImportReport(
      importedMeshTriangleCount,
      importedCollisionLineCount,
      importedMaterialCount,
      importedTextureCount,
      importedEntityCount,
      warnings.ToArray());
    return edited;
  }

  private static List<TextureIndexEntry> WriteTextureSet(R3TextureBlob[] textures, string texturesDirectory)
  {
    List<TextureIndexEntry> textureIndex = new(textures.Length);
    for (int index = 0; index < textures.Length; ++index)
    {
      R3TextureBlob texture = textures[index];
      byte[] bytes = texture.DdsBytes ?? Array.Empty<byte>();
      string safeName = SanitizeFileStem(texture.Name, fallback: $"texture_{index:D4}");
      string fileName = $"sid_{texture.SurfaceId:D5}_{index:D4}_{safeName}.dds";
      string path = Path.Combine(texturesDirectory, fileName);
      File.WriteAllBytes(path, bytes);
      textureIndex.Add(new TextureIndexEntry
      {
        SurfaceId = texture.SurfaceId,
        Name = texture.Name ?? string.Empty,
        FileName = fileName,
        SizeBytes = bytes.Length,
      });
    }

    return textureIndex;
  }

  private static void WriteMeshObjAndMtl(LoadedMap map, string packageDirectory, List<TextureIndexEntry> textureIndex)
  {
    string objPath = Path.Combine(packageDirectory, MeshObjFileName);
    string mtlPath = Path.Combine(packageDirectory, MeshMtlFileName);

    Dictionary<int, string> texturePathBySurfaceId = new();
    for (int i = 0; i < textureIndex.Count; ++i)
    {
      TextureIndexEntry entry = textureIndex[i];
      if (!texturePathBySurfaceId.ContainsKey(entry.SurfaceId))
      {
        texturePathBySurfaceId[entry.SurfaceId] = $"{TexturesDirectoryName}/{entry.FileName}".Replace('\\', '/');
      }
    }

    using (StreamWriter obj = new(objPath, false))
    {
      obj.WriteLine("# RF MapEditor Blender package OBJ");
      obj.WriteLine($"# Map: {map.Name}");
      obj.WriteLine($"mtllib {MeshMtlFileName}");

      BspRenderVertex[] vertices = map.BspRenderVertices;
      for (int i = 0; i < vertices.Length; ++i)
      {
        Vector3 p = ConvertSourcePositionToBlender(vertices[i].Position);
        obj.WriteLine(FormattableString.Invariant($"v {p.X:R} {p.Y:R} {p.Z:R}"));
      }

      for (int i = 0; i < vertices.Length; ++i)
      {
        Vector2 uv = vertices[i].Uv;
        obj.WriteLine(FormattableString.Invariant($"vt {uv.X:R} {1.0f - uv.Y:R}"));
      }

      BspMaterialSpan[] spans = map.BspMaterialSpans;
      for (int spanIndex = 0; spanIndex < spans.Length; ++spanIndex)
      {
        BspMaterialSpan span = spans[spanIndex];
        int start = Math.Clamp(span.StartVertex, 0, vertices.Length);
        int end = Math.Clamp(span.StartVertex + span.VertexCount, 0, vertices.Length);
        if (end - start < 3)
        {
          continue;
        }

        string materialName = BuildMtlName(span.MaterialId);
        obj.WriteLine($"g {materialName}");
        obj.WriteLine($"usemtl {materialName}");
        for (int vertex = start; vertex + 2 < end; vertex += 3)
        {
          int a = vertex + 1;
          int b = vertex + 2;
          int c = vertex + 3;
          obj.WriteLine($"f {a}/{a} {b}/{b} {c}/{c}");
        }
      }
    }

    HashSet<int> materialIds = new();
    for (int i = 0; i < map.BspMaterialSpans.Length; ++i)
    {
      materialIds.Add(map.BspMaterialSpans[i].MaterialId);
    }

    using StreamWriter mtl = new(mtlPath, false);
    mtl.WriteLine("# RF MapEditor Blender package MTL");
    foreach (int materialId in materialIds.OrderBy(id => id))
    {
      mtl.WriteLine($"newmtl {BuildMtlName(materialId)}");
      mtl.WriteLine("Ka 0.0000 0.0000 0.0000");
      mtl.WriteLine("Kd 1.0000 1.0000 1.0000");
      mtl.WriteLine("Ks 0.0000 0.0000 0.0000");
      mtl.WriteLine("d 1.0000");

      int surfaceId = ResolvePrimarySurfaceId(map.Materials, materialId);
      if (surfaceId >= 0 && texturePathBySurfaceId.TryGetValue(surfaceId, out string? texturePath))
      {
        mtl.WriteLine($"map_Kd {texturePath}");
      }

      mtl.WriteLine();
    }
  }

  private static int ResolvePrimarySurfaceId(MaterialDefinition[] materials, int materialId)
  {
    if (materialId < 0 || materialId >= materials.Length)
    {
      return -1;
    }

    MaterialLayerDefinition[] layers = materials[materialId].Layers ?? Array.Empty<MaterialLayerDefinition>();
    if (layers.Length == 0)
    {
      return -1;
    }

    return layers[0].SurfaceId;
  }

  private static ObjMeshData ReadObjMesh(string objPath, List<string> warnings)
  {
    List<Vector3> positions = new();
    List<Vector2> texcoords = new();
    List<ObjImportVertex> triangleVertices = new();
    int currentMaterialId = -1;
    int lineNumber = 0;

    foreach (string rawLine in File.ReadLines(objPath))
    {
      ++lineNumber;
      string line = rawLine.Trim();
      if (line.Length == 0 || line[0] == '#')
      {
        continue;
      }

      if (line.StartsWith("v ", StringComparison.Ordinal))
      {
        string[] parts = SplitObjParts(line, expectedMin: 4);
        if (parts.Length < 4)
        {
          warnings.Add($"OBJ line {lineNumber}: invalid vertex line.");
          continue;
        }

        if (!TryParseSingle(parts[1], out float x) ||
            !TryParseSingle(parts[2], out float y) ||
            !TryParseSingle(parts[3], out float z))
        {
          warnings.Add($"OBJ line {lineNumber}: invalid vertex coordinates.");
          continue;
        }

        positions.Add(new Vector3(x, y, z));
        continue;
      }

      if (line.StartsWith("vt ", StringComparison.Ordinal))
      {
        string[] parts = SplitObjParts(line, expectedMin: 3);
        if (parts.Length < 3)
        {
          warnings.Add($"OBJ line {lineNumber}: invalid texcoord line.");
          continue;
        }

        if (!TryParseSingle(parts[1], out float u) ||
            !TryParseSingle(parts[2], out float v))
        {
          warnings.Add($"OBJ line {lineNumber}: invalid texcoord values.");
          continue;
        }

        texcoords.Add(new Vector2(u, v));
        continue;
      }

      if (line.StartsWith("usemtl ", StringComparison.Ordinal))
      {
        string mtlName = line["usemtl ".Length..].Trim();
        currentMaterialId = ParseMaterialIdFromObjName(mtlName);
        continue;
      }

      if (line.StartsWith("f ", StringComparison.Ordinal))
      {
        string[] parts = SplitObjParts(line, expectedMin: 4);
        if (parts.Length < 4)
        {
          warnings.Add($"OBJ line {lineNumber}: face has fewer than 3 vertices.");
          continue;
        }

        List<ObjFaceRef> refs = new(parts.Length - 1);
        bool valid = true;
        for (int i = 1; i < parts.Length; ++i)
        {
          if (!TryParseFaceRef(parts[i], positions.Count, texcoords.Count, out ObjFaceRef faceRef))
          {
            warnings.Add($"OBJ line {lineNumber}: invalid face vertex token '{parts[i]}'.");
            valid = false;
            break;
          }

          refs.Add(faceRef);
        }

        if (!valid)
        {
          continue;
        }

        for (int tri = 1; tri + 1 < refs.Count; ++tri)
        {
          ObjFaceRef a = refs[0];
          ObjFaceRef b = refs[tri];
          ObjFaceRef c = refs[tri + 1];
          if (!TryBuildObjImportVertex(a, currentMaterialId, positions, texcoords, out ObjImportVertex va) ||
              !TryBuildObjImportVertex(b, currentMaterialId, positions, texcoords, out ObjImportVertex vb) ||
              !TryBuildObjImportVertex(c, currentMaterialId, positions, texcoords, out ObjImportVertex vc))
          {
            warnings.Add($"OBJ line {lineNumber}: skipped one triangle due to invalid position/texcoord index.");
            continue;
          }

          triangleVertices.Add(va);
          triangleVertices.Add(vb);
          triangleVertices.Add(vc);
        }
      }
    }

    return new ObjMeshData(triangleVertices.ToArray());
  }

  private static LoadedMap ApplyImportedMesh(LoadedMap source, ObjMeshData mesh, List<string> warnings)
  {
    BspBinaryLayout layout = source.BspBinaryLayout;
    if (layout == null || layout.Sections.Length != BspHeaderEntryCount || layout.Entries.Length != BspHeaderEntryCount)
    {
      throw new InvalidDataException("Current map does not expose a valid BSP binary layout for mesh import.");
    }

    if ((mesh.Vertices.Length % 3) != 0)
    {
      throw new InvalidDataException("Imported OBJ mesh does not resolve to a triangle list.");
    }

    int triangleCount = mesh.Vertices.Length / 3;
    int vertexCount = mesh.Vertices.Length;

    BspRenderVertex[] renderVertices = new BspRenderVertex[vertexCount];
    for (int i = 0; i < vertexCount; ++i)
    {
      ObjImportVertex sourceVertex = mesh.Vertices[i];
      Vector2 uv = sourceVertex.Uv;
      Vector2 lightUv = new(uv.X, uv.Y);
      renderVertices[i] = new BspRenderVertex(
        sourceVertex.Position,
        uv,
        lightUv,
        new Vector3(1.0f, 1.0f, 1.0f));
    }

    List<MaterialRun> materialRuns = BuildMaterialRuns(mesh.Vertices);
    BspMaterialSpan[] spans = new BspMaterialSpan[materialRuns.Count];
    for (int runIndex = 0; runIndex < materialRuns.Count; ++runIndex)
    {
      MaterialRun run = materialRuns[runIndex];
      spans[runIndex] = new BspMaterialSpan(
        run.MaterialId,
        run.FaceStart * 3,
        run.FaceCount * 3,
        -1);
    }

    byte[] fVertexSection = SerializeFloatVertexSection(mesh.Vertices);
    byte[] vertexColorSection = SerializeVertexColorSection(vertexCount);
    byte[] uvSection = SerializeUvSection(mesh.Vertices);
    byte[] lightUvSection = SerializeLightUvSection(mesh.Vertices);
    byte[] faceSection = SerializeFaceSection(triangleCount);
    byte[] faceIdSection = SerializeFaceIdSection(triangleCount);
    byte[] vertexIdSection = SerializeVertexIdSection(vertexCount);
    byte[] matGroupSection = SerializeMatGroupSection(materialRuns, warnings);

    byte[][] sections = CloneSections(layout.Sections);
    sections[BspSectionBVertex] = Array.Empty<byte>();
    sections[BspSectionWVertex] = Array.Empty<byte>();
    sections[BspSectionFVertex] = fVertexSection;
    sections[BspSectionVertexColor] = vertexColorSection;
    sections[BspSectionUv] = uvSection;
    sections[BspSectionLgtUv] = lightUvSection;
    sections[BspSectionFace] = faceSection;
    sections[BspSectionFaceId] = faceIdSection;
    sections[BspSectionVertexId] = vertexIdSection;
    sections[BspSectionReadMatGroup] = matGroupSection;

    BspEntry[] clonedEntries = (BspEntry[])layout.Entries.Clone();
    byte[] clonedTrailing = layout.Trailing?.ToArray() ?? Array.Empty<byte>();
    BspBinaryLayout updatedLayout = new()
    {
      Version = layout.Version,
      Entries = clonedEntries,
      Sections = sections,
      Trailing = clonedTrailing,
    };

    return MapEditOperations.WithBspMeshData(source, updatedLayout, renderVertices, spans);
  }

  private static List<MaterialRun> BuildMaterialRuns(ObjImportVertex[] vertices)
  {
    List<MaterialRun> runs = new();
    if (vertices.Length == 0)
    {
      return runs;
    }

    int triangleCount = vertices.Length / 3;
    int faceStart = 0;
    int materialId = vertices[0].MaterialId;
    int faceCount = 0;
    for (int face = 0; face < triangleCount; ++face)
    {
      int triMaterial = vertices[face * 3].MaterialId;
      if (triMaterial != materialId && faceCount > 0)
      {
        runs.Add(new MaterialRun(materialId, faceStart, faceCount));
        faceStart = face;
        faceCount = 0;
        materialId = triMaterial;
      }

      ++faceCount;
    }

    if (faceCount > 0)
    {
      runs.Add(new MaterialRun(materialId, faceStart, faceCount));
    }

    return runs;
  }

  private static byte[] SerializeFloatVertexSection(ObjImportVertex[] vertices)
  {
    byte[] bytes = new byte[vertices.Length * 12];
    int offset = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      Vector3 p = vertices[i].Position;
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset, 4), p.X);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 4, 4), p.Y);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 8, 4), p.Z);
      offset += 12;
    }

    return bytes;
  }

  private static byte[] SerializeVertexColorSection(int vertexCount)
  {
    byte[] bytes = new byte[vertexCount * 4];
    int offset = 0;
    for (int i = 0; i < vertexCount; ++i)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), 0xFFFFFFFFu);
      offset += 4;
    }

    return bytes;
  }

  private static byte[] SerializeUvSection(ObjImportVertex[] vertices)
  {
    byte[] bytes = new byte[vertices.Length * 8];
    int offset = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      Vector2 uv = vertices[i].Uv;
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset, 4), uv.X);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 4, 4), uv.Y);
      offset += 8;
    }

    return bytes;
  }

  private static byte[] SerializeLightUvSection(ObjImportVertex[] vertices)
  {
    byte[] bytes = new byte[vertices.Length * 4];
    int offset = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      Vector2 uv = vertices[i].Uv;
      short u = EncodeLightUv(uv.X);
      short v = EncodeLightUv(uv.Y);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset, 2), u);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 2, 2), v);
      offset += 4;
    }

    return bytes;
  }

  private static short EncodeLightUv(float value)
  {
    if (!float.IsFinite(value))
    {
      return 0;
    }

    float clamped = Math.Clamp(value, -1.0f, 1.0f);
    int scaled = (int)MathF.Round(clamped * 32767.0f);
    return (short)Math.Clamp(scaled, short.MinValue, short.MaxValue);
  }

  private static byte[] SerializeFaceSection(int triangleCount)
  {
    byte[] bytes = new byte[triangleCount * 6];
    int offset = 0;
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset, 2), 3);
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset + 2, 4), (uint)(faceIndex * 3));
      offset += 6;
    }

    return bytes;
  }

  private static byte[] SerializeFaceIdSection(int triangleCount)
  {
    byte[] bytes = new byte[triangleCount * 4];
    int offset = 0;
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), (uint)faceIndex);
      offset += 4;
    }

    return bytes;
  }

  private static byte[] SerializeVertexIdSection(int vertexCount)
  {
    byte[] bytes = new byte[vertexCount * 4];
    int offset = 0;
    for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), (uint)vertexIndex);
      offset += 4;
    }

    return bytes;
  }

  private static byte[] SerializeMatGroupSection(List<MaterialRun> runs, List<string> warnings)
  {
    if (runs.Count == 0)
    {
      return Array.Empty<byte>();
    }

    List<MaterialRun> expanded = new();
    for (int i = 0; i < runs.Count; ++i)
    {
      MaterialRun run = runs[i];
      int remaining = run.FaceCount;
      int faceStart = run.FaceStart;
      while (remaining > 0)
      {
        int chunk = Math.Min(remaining, ushort.MaxValue);
        expanded.Add(new MaterialRun(run.MaterialId, faceStart, chunk));
        faceStart += chunk;
        remaining -= chunk;
      }
    }

    byte[] bytes = new byte[expanded.Count * 42];
    int offset = 0;
    for (int i = 0; i < expanded.Count; ++i)
    {
      MaterialRun run = expanded[i];
      short materialId = ClampMaterialIdToShort(run.MaterialId, warnings);

      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset, 2), 0);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 2, 2), (ushort)run.FaceCount);
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset + 4, 4), (uint)run.FaceStart);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 8, 2), materialId);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 10, 2), -1);

      // bb_min / bb_max (6 * int16), pos (3 * float), scale (float), object_id (uint16)
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 24, 4), 0.0f);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 28, 4), 0.0f);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 32, 4), 0.0f);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 36, 4), 1.0f);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 40, 2), 0);
      offset += 42;
    }

    return bytes;
  }

  private static short ClampMaterialIdToShort(int materialId, List<string> warnings)
  {
    if (materialId < short.MinValue)
    {
      warnings.Add($"Material id {materialId} is below int16 range; clamped to {short.MinValue}.");
      return short.MinValue;
    }

    if (materialId > short.MaxValue)
    {
      warnings.Add($"Material id {materialId} exceeds int16 range; clamped to {short.MaxValue}.");
      return short.MaxValue;
    }

    return (short)materialId;
  }

  private static byte[][] CloneSections(byte[][] sections)
  {
    byte[][] cloned = new byte[sections.Length][];
    for (int i = 0; i < sections.Length; ++i)
    {
      byte[] source = sections[i] ?? Array.Empty<byte>();
      byte[] copy = new byte[source.Length];
      if (source.Length > 0)
      {
        Buffer.BlockCopy(source, 0, copy, 0, source.Length);
      }

      cloned[i] = copy;
    }

    return cloned;
  }

  private static (Vector3[] Vertices, CollisionLine[] Lines) BuildAutoCollisionFromMesh(
    BspRenderVertex[] renderVertices,
    MapBounds bounds,
    List<string> warnings)
  {
    if (renderVertices == null || renderVertices.Length < 3)
    {
      warnings.Add("Auto collision: no BSP mesh vertices available; using map-boundary fallback.");
      return BuildFallbackBoundaryCollision(bounds);
    }

    Dictionary<CollisionEdgeKey, CollisionEdgeAccum> candidates = new();
    for (int i = 0; i + 2 < renderVertices.Length; i += 3)
    {
      Vector3 p0 = renderVertices[i].Position;
      Vector3 p1 = renderVertices[i + 1].Position;
      Vector3 p2 = renderVertices[i + 2].Position;
      if (!IsFiniteVector3(p0) || !IsFiniteVector3(p1) || !IsFiniteVector3(p2))
      {
        continue;
      }

      Vector3 normal = Vector3.Cross(p1 - p0, p2 - p0);
      float normalLength = normal.Length;
      if (!float.IsFinite(normalLength) || normalLength <= 0.0001f)
      {
        continue;
      }

      normal /= normalLength;
      if (MathF.Abs(normal.Y) > CollisionFaceMaxAbsNormalY)
      {
        continue;
      }

      float triMinY = MathF.Min(p0.Y, MathF.Min(p1.Y, p2.Y));
      float triMaxY = MathF.Max(p0.Y, MathF.Max(p1.Y, p2.Y));
      float triHeight = triMaxY - triMinY;
      if (!float.IsFinite(triHeight) || triHeight <= 0.0001f)
      {
        continue;
      }

      AddCollisionEdgeCandidate(p0, p1, triMinY, triHeight, candidates);
      AddCollisionEdgeCandidate(p1, p2, triMinY, triHeight, candidates);
      AddCollisionEdgeCandidate(p2, p0, triMinY, triHeight, candidates);
    }

    if (candidates.Count == 0)
    {
      warnings.Add("Auto collision: no valid wall edges detected; using map-boundary fallback.");
      return BuildFallbackBoundaryCollision(bounds);
    }

    List<CollisionEdgeAccum> edges = candidates.Values
      .OrderByDescending(static edge => edge.HorizontalLength)
      .ToList();

    Vector3 sentinel = BuildCollisionSentinelVertex(bounds);
    List<Vector3> vertices = [sentinel];
    List<CollisionLine> lines = [new CollisionLine(0u, 0, 0, 0.0f, 0, 0)];
    Dictionary<CollisionVertexKey, ushort> vertexMap = new();

    CollisionVertexKey sentinelKey = BuildCollisionVertexKey(sentinel);
    vertexMap[sentinelKey] = 0;

    for (int edgeIndex = 0; edgeIndex < edges.Count; ++edgeIndex)
    {
      if (lines.Count > MaxCollisionLines)
      {
        warnings.Add($"Auto collision: line cap {MaxCollisionLines} reached; remaining edges were skipped.");
        break;
      }

      CollisionEdgeAccum edge = edges[edgeIndex];
      Vector3 start = new(edge.StartX, edge.BottomY, edge.StartZ);
      Vector3 end = new(edge.EndX, edge.BottomY, edge.EndZ);
      if (!IsFiniteVector3(start) || !IsFiniteVector3(end))
      {
        continue;
      }

      float horizontalLength = new Vector2(end.X - start.X, end.Z - start.Z).Length;
      if (!float.IsFinite(horizontalLength) || horizontalLength <= 0.5f)
      {
        continue;
      }

      if (!TryGetOrAddCollisionVertex(start, vertices, vertexMap, out ushort startIndex))
      {
        warnings.Add($"Auto collision: vertex cap {MaxCollisionVertices} reached; stopped adding lines.");
        break;
      }

      if (!TryGetOrAddCollisionVertex(end, vertices, vertexMap, out ushort endIndex))
      {
        warnings.Add($"Auto collision: vertex cap {MaxCollisionVertices} reached; stopped adding lines.");
        break;
      }

      if (startIndex == endIndex)
      {
        continue;
      }

      float height = Math.Clamp(edge.Height, 1.0f, 50000.0f);
      lines.Add(new CollisionLine(0u, startIndex, endIndex, height, 0, 0));
    }

    if (lines.Count <= 1)
    {
      warnings.Add("Auto collision: generated zero collision lines; using map-boundary fallback.");
      return BuildFallbackBoundaryCollision(bounds);
    }

    return (vertices.ToArray(), lines.ToArray());
  }

  private static void AddCollisionEdgeCandidate(
    in Vector3 a,
    in Vector3 b,
    float triMinY,
    float triHeight,
    Dictionary<CollisionEdgeKey, CollisionEdgeAccum> candidates)
  {
    Vector2 a2 = new(a.X, a.Z);
    Vector2 b2 = new(b.X, b.Z);
    Vector2 delta = b2 - a2;
    float horizontalLength = delta.Length;
    if (!float.IsFinite(horizontalLength) || horizontalLength < CollisionEdgeMinHorizontalLength)
    {
      return;
    }

    float edgeVerticalSpan = MathF.Abs(a.Y - b.Y);
    if (!float.IsFinite(edgeVerticalSpan) || edgeVerticalSpan > triHeight * 0.35f + 5.0f)
    {
      return;
    }

    // Use only footprint edges near triangle bottom to avoid top/diagonal ghost walls.
    float bottomTolerance = MathF.Max(CollisionBottomEdgeTolerance, triHeight * 0.08f);
    if (MathF.Abs(a.Y - triMinY) > bottomTolerance || MathF.Abs(b.Y - triMinY) > bottomTolerance)
    {
      return;
    }

    int ax = QuantizeCollision(a2.X);
    int az = QuantizeCollision(a2.Y);
    int bx = QuantizeCollision(b2.X);
    int bz = QuantizeCollision(b2.Y);
    if (ax == bx && az == bz)
    {
      return;
    }

    CollisionEdgeKey key = CollisionEdgeKey.Create(ax, az, bx, bz);
    float startX = DequantizeCollision(key.X0);
    float startZ = DequantizeCollision(key.Z0);
    float endX = DequantizeCollision(key.X1);
    float endZ = DequantizeCollision(key.Z1);
    float bottomY = MathF.Min(a.Y, b.Y);
    float height = Math.Max(CollisionMinWallHeight, triHeight);

    if (candidates.TryGetValue(key, out CollisionEdgeAccum existing))
    {
      candidates[key] = new CollisionEdgeAccum(
        existing.StartX,
        existing.StartZ,
        existing.EndX,
        existing.EndZ,
        MathF.Min(existing.BottomY, bottomY),
        MathF.Max(existing.Height, height),
        MathF.Max(existing.HorizontalLength, horizontalLength));
      return;
    }

    candidates[key] = new CollisionEdgeAccum(startX, startZ, endX, endZ, bottomY, height, horizontalLength);
  }

  private static (Vector3[] Vertices, CollisionLine[] Lines) BuildFallbackBoundaryCollision(MapBounds bounds)
  {
    float minX = float.IsFinite(bounds.Min.X) ? bounds.Min.X : -100.0f;
    float minY = float.IsFinite(bounds.Min.Y) ? bounds.Min.Y : 0.0f;
    float minZ = float.IsFinite(bounds.Min.Z) ? bounds.Min.Z : -100.0f;
    float maxX = float.IsFinite(bounds.Max.X) ? bounds.Max.X : 100.0f;
    float maxZ = float.IsFinite(bounds.Max.Z) ? bounds.Max.Z : 100.0f;

    if (maxX - minX < 1.0f)
    {
      maxX = minX + 100.0f;
    }

    if (maxZ - minZ < 1.0f)
    {
      maxZ = minZ + 100.0f;
    }

    float height = Math.Max(CollisionMinWallHeight, MathF.Max(500.0f, bounds.Size.Y + 250.0f));
    Vector3[] vertices =
    [
      new Vector3(minX, minY, minZ), // sentinel vertex for line 0
      new Vector3(minX, minY, minZ),
      new Vector3(maxX, minY, minZ),
      new Vector3(maxX, minY, maxZ),
      new Vector3(minX, minY, maxZ),
    ];

    CollisionLine[] lines =
    [
      new CollisionLine(0u, 0, 0, 0.0f, 0, 0), // sentinel line
      new CollisionLine(0u, 1, 2, height, 0, 0),
      new CollisionLine(0u, 2, 3, height, 0, 0),
      new CollisionLine(0u, 3, 4, height, 0, 0),
      new CollisionLine(0u, 4, 1, height, 0, 0),
    ];

    return (vertices, lines);
  }

  private static Vector3 BuildCollisionSentinelVertex(MapBounds bounds)
  {
    if (IsFiniteVector3(bounds.Min))
    {
      return bounds.Min;
    }

    return Vector3.Zero;
  }

  private static bool TryGetOrAddCollisionVertex(
    in Vector3 position,
    List<Vector3> vertices,
    Dictionary<CollisionVertexKey, ushort> vertexMap,
    out ushort index)
  {
    index = 0;
    CollisionVertexKey key = BuildCollisionVertexKey(position);
    if (vertexMap.TryGetValue(key, out ushort existing))
    {
      index = existing;
      return true;
    }

    if (vertices.Count >= MaxCollisionVertices)
    {
      return false;
    }

    index = (ushort)vertices.Count;
    vertices.Add(new Vector3(
      DequantizeCollision(key.X),
      DequantizeCollision(key.Y),
      DequantizeCollision(key.Z)));
    vertexMap[key] = index;
    return true;
  }

  private static CollisionVertexKey BuildCollisionVertexKey(in Vector3 position)
  {
    return new CollisionVertexKey(
      QuantizeCollision(position.X),
      QuantizeCollision(position.Y),
      QuantizeCollision(position.Z));
  }

  private static int QuantizeCollision(float value)
  {
    if (!float.IsFinite(value))
    {
      return 0;
    }

    return (int)MathF.Round(value * CollisionQuantizeScale);
  }

  private static float DequantizeCollision(int value)
  {
    return value / CollisionQuantizeScale;
  }

  private static bool IsFiniteVector3(in Vector3 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y) && float.IsFinite(value.Z);
  }

  private static int ParseMaterialIdFromObjName(string name)
  {
    if (string.IsNullOrWhiteSpace(name))
    {
      return -1;
    }

    string trimmed = name.Trim();
    if (string.Equals(trimmed, "mat_none", StringComparison.OrdinalIgnoreCase))
    {
      return -1;
    }

    const string prefix = "mat_";
    if (trimmed.StartsWith(prefix, StringComparison.OrdinalIgnoreCase)
      && int.TryParse(trimmed[prefix.Length..], NumberStyles.Integer, CultureInfo.InvariantCulture, out int prefixed))
    {
      return prefixed;
    }

    return int.TryParse(trimmed, NumberStyles.Integer, CultureInfo.InvariantCulture, out int plain)
      ? plain
      : -1;
  }

  private static string[] SplitObjParts(string line, int expectedMin)
  {
    string[] parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
    if (parts.Length < expectedMin)
    {
      return Array.Empty<string>();
    }

    return parts;
  }

  private static bool TryParseFaceRef(string token, int positionCount, int texcoordCount, out ObjFaceRef faceRef)
  {
    faceRef = default;
    if (string.IsNullOrWhiteSpace(token))
    {
      return false;
    }

    string[] parts = token.Split('/');
    if (parts.Length == 0)
    {
      return false;
    }

    if (!int.TryParse(parts[0], NumberStyles.Integer, CultureInfo.InvariantCulture, out int posRaw))
    {
      return false;
    }

    int positionIndex = ResolveObjIndex(posRaw, positionCount);
    if (positionIndex < 0)
    {
      return false;
    }

    int texcoordIndex = -1;
    if (parts.Length >= 2 && !string.IsNullOrWhiteSpace(parts[1]))
    {
      if (!int.TryParse(parts[1], NumberStyles.Integer, CultureInfo.InvariantCulture, out int texRaw))
      {
        return false;
      }

      texcoordIndex = ResolveObjIndex(texRaw, texcoordCount);
      if (texcoordIndex < 0)
      {
        return false;
      }
    }

    faceRef = new ObjFaceRef(positionIndex, texcoordIndex);
    return true;
  }

  private static int ResolveObjIndex(int raw, int count)
  {
    if (count <= 0 || raw == 0)
    {
      return -1;
    }

    if (raw > 0)
    {
      int index = raw - 1;
      return index >= 0 && index < count ? index : -1;
    }

    int relative = count + raw;
    return relative >= 0 && relative < count ? relative : -1;
  }

  private static bool TryBuildObjImportVertex(
    ObjFaceRef faceRef,
    int materialId,
    List<Vector3> positions,
    List<Vector2> texcoords,
    out ObjImportVertex vertex)
  {
    vertex = default;
    if ((uint)faceRef.PositionIndex >= (uint)positions.Count)
    {
      return false;
    }

    Vector3 position = ConvertBlenderPositionToSource(positions[faceRef.PositionIndex]);
    Vector2 uv = Vector2.Zero;
    if (faceRef.TexcoordIndex >= 0)
    {
      if ((uint)faceRef.TexcoordIndex >= (uint)texcoords.Count)
      {
        return false;
      }

      Vector2 objUv = texcoords[faceRef.TexcoordIndex];
      uv = new Vector2(objUv.X, 1.0f - objUv.Y);
      if (!float.IsFinite(uv.X) || !float.IsFinite(uv.Y))
      {
        uv = Vector2.Zero;
      }
    }

    vertex = new ObjImportVertex(position, uv, materialId);
    return true;
  }

  private static bool TryParseSingle(string text, out float value)
  {
    return float.TryParse(text, NumberStyles.Float | NumberStyles.AllowThousands, CultureInfo.InvariantCulture, out value)
      && float.IsFinite(value);
  }

  private static Vector3 ConvertSourcePositionToBlender(in Vector3 source)
  {
    // RF source space uses Y-up. Blender uses Z-up.
    return new Vector3(source.X, source.Z, source.Y);
  }

  private static Vector3 ConvertBlenderPositionToSource(in Vector3 blender)
  {
    // Inverse of ConvertSourcePositionToBlender.
    return new Vector3(blender.X, blender.Z, blender.Y);
  }

  private static bool IsMeshGeometryEquivalent(BspRenderVertex[] currentVertices, ObjImportVertex[] importedVertices)
  {
    if (currentVertices == null || importedVertices == null)
    {
      return false;
    }

    if (currentVertices.Length != importedVertices.Length)
    {
      return false;
    }

    const float positionEpsilonSq = 0.0005f * 0.0005f;
    for (int i = 0; i < currentVertices.Length; ++i)
    {
      Vector3 a = currentVertices[i].Position;
      Vector3 b = importedVertices[i].Position;
      if (!IsFiniteVector3(a) || !IsFiniteVector3(b))
      {
        return false;
      }

      if ((a - b).LengthSquared > positionEpsilonSq)
      {
        return false;
      }
    }

    return true;
  }

  private static EntityPackage BuildEntityExport(EntitySceneData scene)
  {
    EntityModelEntry[] models = new EntityModelEntry[scene.Models.Length];
    for (int index = 0; index < scene.Models.Length; ++index)
    {
      EntitySceneModel model = scene.Models[index];
      models[index] = new EntityModelEntry
      {
        EntityId = model.EntityId,
        Name = model.Name ?? string.Empty,
      };
    }

    EntityInstanceEntry[] instances = new EntityInstanceEntry[scene.Instances.Length];
    for (int index = 0; index < scene.Instances.Length; ++index)
    {
      EntitySceneInstance instance = scene.Instances[index];
      instances[index] = new EntityInstanceEntry
      {
        EntityId = instance.EntityId,
        Scale = instance.Scale,
        PositionX = instance.Position.X,
        PositionY = instance.Position.Y,
        PositionZ = instance.Position.Z,
        RotX = instance.RotX,
        RotY = instance.RotY,
      };
    }

    return new EntityPackage
    {
      Models = models,
      Instances = instances,
    };
  }

  private static string BuildMtlName(int materialId)
  {
    return materialId >= 0 ? $"mat_{materialId:D4}" : "mat_none";
  }

  private static string? ResolveMeshObjPath(string packageDirectory, List<string> warnings)
  {
    string canonical = Path.Combine(packageDirectory, MeshObjFileName);
    if (File.Exists(canonical))
    {
      return canonical;
    }

    string[] objFiles = Directory.GetFiles(packageDirectory, "*.obj", SearchOption.TopDirectoryOnly);
    if (objFiles.Length == 0)
    {
      return null;
    }

    string selected = objFiles[0];
    long selectedSize = 0;
    for (int i = 0; i < objFiles.Length; ++i)
    {
      string filePath = objFiles[i];
      long size = 0;
      try
      {
        size = new FileInfo(filePath).Length;
      }
      catch
      {
        // Keep best effort selection when file metadata cannot be read.
      }

      if (size > selectedSize)
      {
        selected = filePath;
        selectedSize = size;
      }
    }

    warnings.Add($"map_mesh.obj not found; using '{Path.GetFileName(selected)}' instead.");
    return selected;
  }

  private static string SanitizeFileStem(string? text, string fallback)
  {
    string input = text ?? string.Empty;
    if (string.IsNullOrWhiteSpace(input))
    {
      return fallback;
    }

    char[] invalid = Path.GetInvalidFileNameChars();
    HashSet<char> invalidSet = new(invalid);
    Span<char> buffer = stackalloc char[input.Length];
    int length = 0;
    for (int i = 0; i < input.Length; ++i)
    {
      char c = input[i];
      if (invalidSet.Contains(c))
      {
        continue;
      }

      if (char.IsWhiteSpace(c))
      {
        c = '_';
      }

      if (c == '.' || c == '/' || c == '\\' || c == ':')
      {
        c = '_';
      }

      buffer[length++] = c;
    }

    if (length == 0)
    {
      return fallback;
    }

    string sanitized = new(buffer[..length]);
    return string.IsNullOrWhiteSpace(sanitized) ? fallback : sanitized;
  }

  private static bool IsDds(byte[] bytes)
  {
    return bytes.Length >= 4
      && bytes[0] == (byte)'D'
      && bytes[1] == (byte)'D'
      && bytes[2] == (byte)'S'
      && bytes[3] == (byte)' ';
  }

  private sealed class MapPackageManifest
  {
    public required string PackageVersion { get; init; }
    public required string MapName { get; init; }
    public required DateTime ExportedUtc { get; init; }
    public required int MaterialCount { get; init; }
    public required int TextureCount { get; init; }
    public required int EntityModelCount { get; init; }
    public required int EntityInstanceCount { get; init; }
    public required string Notes { get; init; }
  }

  private sealed class TextureIndexEntry
  {
    public required int SurfaceId { get; init; }
    public required string Name { get; init; }
    public required string FileName { get; init; }
    public required int SizeBytes { get; init; }
  }

  private sealed class EntityPackage
  {
    public required EntityModelEntry[] Models { get; init; }
    public required EntityInstanceEntry[] Instances { get; init; }
  }

  private sealed class EntityModelEntry
  {
    public required int EntityId { get; init; }
    public required string Name { get; init; }
  }

  private sealed class EntityInstanceEntry
  {
    public required int EntityId { get; init; }
    public required float Scale { get; init; }
    public required float PositionX { get; init; }
    public required float PositionY { get; init; }
    public required float PositionZ { get; init; }
    public required float RotX { get; init; }
    public required float RotY { get; init; }
  }

  private readonly record struct ObjMeshData(ObjImportVertex[] Vertices);
  private readonly record struct ObjImportVertex(Vector3 Position, Vector2 Uv, int MaterialId);
  private readonly record struct ObjFaceRef(int PositionIndex, int TexcoordIndex);
  private readonly record struct MaterialRun(int MaterialId, int FaceStart, int FaceCount);
  private readonly record struct CollisionEdgeAccum(
    float StartX,
    float StartZ,
    float EndX,
    float EndZ,
    float BottomY,
    float Height,
    float HorizontalLength);

  private readonly record struct CollisionEdgeKey(int X0, int Z0, int X1, int Z1)
  {
    public static CollisionEdgeKey Create(int ax, int az, int bx, int bz)
    {
      bool aFirst = ax < bx || (ax == bx && az <= bz);
      return aFirst
        ? new CollisionEdgeKey(ax, az, bx, bz)
        : new CollisionEdgeKey(bx, bz, ax, az);
    }
  }

  private readonly record struct CollisionVertexKey(int X, int Y, int Z);
}
