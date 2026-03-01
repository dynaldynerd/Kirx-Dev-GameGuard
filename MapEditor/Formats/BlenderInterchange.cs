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
  public readonly record struct BspFaceSelection(int FaceIndex, bool FaceOnly);
  public readonly record struct BspVertexSelection(int XBits, int YBits, int ZBits);
  public readonly record struct BspEdgeSelection(BspVertexSelection A, BspVertexSelection B);

  private const string PackageVersion = "1";
  private const string ManifestFileName = "rf_map_package.json";
  private const string MaterialsFileName = "map_materials.json";
  private const string TexturesIndexFileName = "map_textures.json";
  private const string EntitiesFileName = "entity_instances.json";
  private const string MeshObjFileName = "map_mesh.obj";
  private const string MeshMtlFileName = "map_mesh.mtl";
  private const string TexturesDirectoryName = "MapTextures";
  private const int BspHeaderEntryCount = 85;
  private const int BspSectionCPlanes = 0;
  private const int BspSectionCFaceId = 1;
  private const int BspSectionNode = 2;
  private const int BspSectionLeaf = 3;
  private const int BspSectionMatListInLeaf = 4;
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
  private const int MaxLeafFaceCount = ushort.MaxValue;
  private const int MaxLeafMatGroupCount = ushort.MaxValue;
  private const int MaxBspLeafCount = short.MaxValue;
  private const int MaxBspNodeCount = short.MaxValue;
  private const int DefaultTargetFacesPerLeaf = 4096;
  private const int BspLeafSerializedSize = 25;
  private const int MaxSpatialTreeDepth = 20;
  private const int MaxConnectedClusterFacesForEdit = 6000;
  private const float MetadataMatchEpsilonSq = 0.000001f;
  private const float MetadataPositionQuantizeScale = 1000.0f;

  private static readonly JsonSerializerOptions JsonOptions = new()
  {
    WriteIndented = true,
    PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
  };

  public static bool TryTranslateMeshSelection(
    LoadedMap source,
    int selectedObjectId,
    int selectedFaceIndex,
    bool selectedFaceOnly,
    Vector3 delta,
    out LoadedMap edited,
    out string error)
  {
    int[] selectedObjectIds = selectedObjectId >= 0 ? [selectedObjectId] : Array.Empty<int>();
    BspFaceSelection[] selectedFaces = selectedFaceIndex >= 0
      ? [new BspFaceSelection(selectedFaceIndex, selectedFaceOnly)]
      : Array.Empty<BspFaceSelection>();
    return TryTranslateMeshSelection(source, selectedObjectIds, selectedFaces, delta, out edited, out error);
  }

  public static bool TryTranslateMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    Vector3 delta,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(delta) || delta.LengthSquared <= 0.0000001f)
    {
      error = "Translation delta is invalid.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] vertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedFaceSet(
      vertices,
      selectedObjectIds,
      selectedFaces,
      out HashSet<int> selectedFaceSet,
      out error))
    {
      return false;
    }

    bool[] moveVertex = new bool[vertices.Length];
    foreach (int faceIndex in selectedFaceSet)
    {
      int faceBase = faceIndex * 3;
      if ((uint)(faceBase + 2) >= (uint)vertices.Length)
      {
        continue;
      }

      moveVertex[faceBase] = true;
      moveVertex[faceBase + 1] = true;
      moveVertex[faceBase + 2] = true;
    }

    int movedVertexCount = 0;
    for (int vertexIndex = 0; vertexIndex < vertices.Length; ++vertexIndex)
    {
      if (!moveVertex[vertexIndex])
      {
        continue;
      }

      ObjImportVertex v = vertices[vertexIndex];
      vertices[vertexIndex] = new ObjImportVertex(v.Position + delta, v.LocalPosition + delta, v.Uv, v.LightUv, v.Color, v.MaterialId, v.LightMapId, v.ObjectId, v.HasExplicitUv);
      ++movedVertexCount;
    }

    if (movedVertexCount <= 0)
    {
      error = "Selected BSP mesh did not contain any editable vertices.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, vertices, warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"BSP translate rebuild failed: {ex.Message}";
      return false;
    }
  }

  public static bool TryDeleteMeshSelection(
    LoadedMap source,
    int selectedObjectId,
    int selectedFaceIndex,
    bool selectedFaceOnly,
    out LoadedMap edited,
    out string error)
  {
    int[] selectedObjectIds = selectedObjectId >= 0 ? [selectedObjectId] : Array.Empty<int>();
    BspFaceSelection[] selectedFaces = selectedFaceIndex >= 0
      ? [new BspFaceSelection(selectedFaceIndex, selectedFaceOnly)]
      : Array.Empty<BspFaceSelection>();
    return TryDeleteMeshSelection(source, selectedObjectIds, selectedFaces, out edited, out error);
  }

  public static bool TryDeleteMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] sourceVertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (sourceVertices.Length < 3 || (sourceVertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    int triangleCount = sourceVertices.Length / 3;
    List<ObjImportVertex> kept = new(sourceVertices.Length);
    int removedFaces = 0;

    if (!TryBuildSelectedFaceSet(
      sourceVertices,
      selectedObjectIds,
      selectedFaces,
      out HashSet<int> selectedFaceSet,
      out error))
    {
      return false;
    }

    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int baseIndex = faceIndex * 3;
      ObjImportVertex v0 = sourceVertices[baseIndex];
      ObjImportVertex v1 = sourceVertices[baseIndex + 1];
      ObjImportVertex v2 = sourceVertices[baseIndex + 2];

      bool removeFace = selectedFaceSet.Contains(faceIndex);

      if (removeFace)
      {
        ++removedFaces;
        continue;
      }

      kept.Add(v0);
      kept.Add(v1);
      kept.Add(v2);
    }

    if (removedFaces <= 0)
    {
      error = "No BSP triangles matched the selection.";
      return false;
    }

    if (kept.Count < 3)
    {
      error = "Delete would remove all BSP triangles; operation was blocked.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, kept.ToArray(), warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"BSP delete rebuild failed: {ex.Message}";
      return false;
    }
  }

  public static bool TryTranslateMeshPrimitiveSelection(
    LoadedMap source,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    Vector3 delta,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(delta) || delta.LengthSquared <= 0.0000001f)
    {
      error = "Translation delta is invalid.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] vertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedPrimitiveVertexIndexSet(
      vertices,
      selectedVertices,
      selectedEdges,
      out HashSet<int> selectedVertexIndices,
      out error))
    {
      return false;
    }

    int movedVertexCount = 0;
    foreach (int vertexIndex in selectedVertexIndices)
    {
      if ((uint)vertexIndex >= (uint)vertices.Length)
      {
        continue;
      }

      ObjImportVertex v = vertices[vertexIndex];
      vertices[vertexIndex] = new ObjImportVertex(
        v.Position + delta,
        v.LocalPosition + delta,
        v.Uv,
        v.LightUv,
        v.Color,
        v.MaterialId,
        v.LightMapId,
        v.ObjectId,
        v.HasExplicitUv);
      ++movedVertexCount;
    }

    if (movedVertexCount <= 0)
    {
      error = "Selected BSP primitive did not contain any editable vertices.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, vertices, warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"BSP primitive translate rebuild failed: {ex.Message}";
      return false;
    }
  }

  public static bool TryDeleteMeshPrimitiveSelection(
    LoadedMap source,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] sourceVertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (sourceVertices.Length < 3 || (sourceVertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedPrimitiveFaceSetForDelete(
      sourceVertices,
      selectedVertices,
      selectedEdges,
      out HashSet<int> selectedFaceSet,
      out error))
    {
      return false;
    }

    int triangleCount = sourceVertices.Length / 3;
    List<ObjImportVertex> kept = new(sourceVertices.Length);
    int removedFaces = 0;
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int baseIndex = faceIndex * 3;
      if (selectedFaceSet.Contains(faceIndex))
      {
        ++removedFaces;
        continue;
      }

      kept.Add(sourceVertices[baseIndex]);
      kept.Add(sourceVertices[baseIndex + 1]);
      kept.Add(sourceVertices[baseIndex + 2]);
    }

    if (removedFaces <= 0)
    {
      error = "No BSP triangles matched the primitive selection.";
      return false;
    }

    if (kept.Count < 3)
    {
      error = "Delete would remove all BSP triangles; operation was blocked.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, kept.ToArray(), warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"BSP primitive delete rebuild failed: {ex.Message}";
      return false;
    }
  }

  public static bool TryDuplicateMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    Vector3 offset,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(offset))
    {
      error = "Duplicate offset is invalid.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] sourceVertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (sourceVertices.Length < 3 || (sourceVertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedFaceSet(
      sourceVertices,
      selectedObjectIds,
      selectedFaces,
      out HashSet<int> selectedFaceSet,
      out error))
    {
      return false;
    }

    if (!TryDuplicateFaceSet(
      source,
      sourceVertices,
      selectedFaceSet,
      offset,
      warnings,
      "BSP duplicate",
      out edited,
      out error))
    {
      return false;
    }

    return true;
  }

  public static bool TryDuplicateMeshPrimitiveSelection(
    LoadedMap source,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    Vector3 offset,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(offset))
    {
      error = "Duplicate offset is invalid.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] sourceVertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (sourceVertices.Length < 3 || (sourceVertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedPrimitiveFaceSetForDelete(
      sourceVertices,
      selectedVertices,
      selectedEdges,
      out HashSet<int> selectedFaceSet,
      out error))
    {
      return false;
    }

    if (!TryDuplicateFaceSet(
      source,
      sourceVertices,
      selectedFaceSet,
      offset,
      warnings,
      "BSP primitive duplicate",
      out edited,
      out error))
    {
      return false;
    }

    return true;
  }

  public static bool TryMergeMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    List<string> warnings = new();
    ObjImportVertex[] vertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    HashSet<int> selectedFaceSet = new();
    bool hasObjectFaceSelection = selectedObjectIds.Count > 0 || selectedFaces.Count > 0;
    if (hasObjectFaceSelection)
    {
      if (!TryBuildSelectedFaceSet(
        vertices,
        selectedObjectIds,
        selectedFaces,
        out HashSet<int> objectFaceSet,
        out error))
      {
        return false;
      }

      selectedFaceSet.UnionWith(objectFaceSet);
    }

    bool hasPrimitiveSelection = selectedVertices.Count > 0 || selectedEdges.Count > 0;
    if (hasPrimitiveSelection)
    {
      if (!TryBuildSelectedPrimitiveFaceSetForDelete(
        vertices,
        selectedVertices,
        selectedEdges,
        out HashSet<int> primitiveFaceSet,
        out error))
      {
        return false;
      }

      selectedFaceSet.UnionWith(primitiveFaceSet);
    }

    if (selectedFaceSet.Count <= 0)
    {
      error = "No BSP object/face/edge/vertex selected.";
      return false;
    }

    if (!TryResolveMergeTargetObjectId(vertices, selectedFaceSet, selectedObjectIds, out ushort targetObjectId, out error))
    {
      return false;
    }

    int changedVertices = 0;
    foreach (int faceIndex in selectedFaceSet)
    {
      int baseIndex = faceIndex * 3;
      if ((uint)(baseIndex + 2) >= (uint)vertices.Length)
      {
        continue;
      }

      for (int k = 0; k < 3; ++k)
      {
        int vertexIndex = baseIndex + k;
        ObjImportVertex v = vertices[vertexIndex];
        if (v.ObjectId == targetObjectId)
        {
          continue;
        }

        vertices[vertexIndex] = new ObjImportVertex(
          v.Position,
          v.LocalPosition,
          v.Uv,
          v.LightUv,
          v.Color,
          v.MaterialId,
          v.LightMapId,
          targetObjectId,
          v.HasExplicitUv);
        ++changedVertices;
      }
    }

    if (changedVertices <= 0)
    {
      error = "Selected BSP faces are already merged to one object.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, vertices, warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"BSP merge rebuild failed: {ex.Message}";
      return false;
    }
  }

  private static bool TryResolveMergeTargetObjectId(
    ObjImportVertex[] vertices,
    HashSet<int> selectedFaceSet,
    IReadOnlyCollection<int> selectedObjectIds,
    out ushort objectId,
    out string error)
  {
    objectId = 0;
    error = string.Empty;

    int chosen = int.MaxValue;
    foreach (int selectedObjectId in selectedObjectIds)
    {
      if (selectedObjectId < 0 || selectedObjectId > ushort.MaxValue)
      {
        continue;
      }

      if (selectedObjectId < chosen)
      {
        chosen = selectedObjectId;
      }
    }

    if (chosen != int.MaxValue)
    {
      objectId = (ushort)chosen;
      return true;
    }

    int firstFace = int.MaxValue;
    foreach (int faceIndex in selectedFaceSet)
    {
      if (faceIndex >= 0 && faceIndex < firstFace)
      {
        firstFace = faceIndex;
      }
    }

    if (firstFace == int.MaxValue)
    {
      error = "Selected BSP face set is invalid.";
      return false;
    }

    int baseIndex = firstFace * 3;
    if ((uint)baseIndex >= (uint)vertices.Length)
    {
      error = "Selected BSP face set is out of range.";
      return false;
    }

    objectId = vertices[baseIndex].ObjectId;
    return true;
  }

  private static bool TryDuplicateFaceSet(
    LoadedMap source,
    ObjImportVertex[] sourceVertices,
    HashSet<int> selectedFaceSet,
    Vector3 offset,
    List<string> warnings,
    string operationName,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (selectedFaceSet.Count <= 0)
    {
      error = "No BSP faces selected for duplicate.";
      return false;
    }

    int triangleCount = sourceVertices.Length / 3;
    List<int> sortedFaces = selectedFaceSet.ToList();
    sortedFaces.Sort();

    List<ObjImportVertex> merged = new(sourceVertices.Length + (sortedFaces.Count * 3));
    merged.AddRange(sourceVertices);
    for (int i = 0; i < sortedFaces.Count; ++i)
    {
      int faceIndex = sortedFaces[i];
      if (faceIndex < 0 || faceIndex >= triangleCount)
      {
        continue;
      }

      int baseIndex = faceIndex * 3;
      for (int k = 0; k < 3; ++k)
      {
        ObjImportVertex sourceVertex = sourceVertices[baseIndex + k];

        Vector3 duplicatedPosition = sourceVertex.Position + offset;
        Vector3 duplicatedLocalPosition = sourceVertex.LocalPosition + offset;
        if (!IsFiniteVector3(duplicatedPosition) || !IsFiniteVector3(duplicatedLocalPosition))
        {
          error = $"{operationName} produced invalid vertex coordinates.";
          return false;
        }

        merged.Add(new ObjImportVertex(
          duplicatedPosition,
          duplicatedLocalPosition,
          sourceVertex.Uv,
          sourceVertex.LightUv,
          sourceVertex.Color,
          sourceVertex.MaterialId,
          sourceVertex.LightMapId,
          sourceVertex.ObjectId,
          sourceVertex.HasExplicitUv));
      }
    }

    if (merged.Count == sourceVertices.Length)
    {
      error = "No BSP faces were duplicated.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, merged.ToArray(), warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"{operationName} rebuild failed: {ex.Message}";
      return false;
    }
  }

  public static bool TryScaleMeshPrimitiveSelection(
    LoadedMap source,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    Vector3 scaleFactors,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(scaleFactors) ||
        scaleFactors.X <= 0.0001f ||
        scaleFactors.Y <= 0.0001f ||
        scaleFactors.Z <= 0.0001f)
    {
      error = "Scale factor is invalid.";
      return false;
    }

    return TryTransformMeshPrimitiveSelection(
      source,
      selectedVertices,
      selectedEdges,
      transform: (position, pivot) =>
      {
        Vector3 offset = position - pivot;
        return new Vector3(
          pivot.X + offset.X * scaleFactors.X,
          pivot.Y + offset.Y * scaleFactors.Y,
          pivot.Z + offset.Z * scaleFactors.Z);
      },
      operationName: "BSP primitive scale",
      out edited,
      out error);
  }

  public static bool TryRotateMeshPrimitiveSelection(
    LoadedMap source,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    Vector3 axis,
    float angleDegrees,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(axis) || axis.LengthSquared <= 0.000001f || !float.IsFinite(angleDegrees))
    {
      error = "Rotation axis/angle is invalid.";
      return false;
    }

    float angleRadians = MathHelper.DegreesToRadians(angleDegrees);
    if (!float.IsFinite(angleRadians) || MathF.Abs(angleRadians) <= 0.0000001f)
    {
      error = "Rotation angle is too small.";
      return false;
    }

    Vector3 axisNormalized = Vector3.Normalize(axis);
    return TryTransformMeshPrimitiveSelection(
      source,
      selectedVertices,
      selectedEdges,
      transform: (position, pivot) =>
      {
        Vector3 offset = position - pivot;
        Vector3 rotated = RotateVectorByAxisAngle(offset, axisNormalized, angleRadians);
        return pivot + rotated;
      },
      operationName: "BSP primitive rotate",
      out edited,
      out error);
  }

  public static bool TryScaleMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    Vector3 scaleFactors,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(scaleFactors) ||
        scaleFactors.X <= 0.0001f ||
        scaleFactors.Y <= 0.0001f ||
        scaleFactors.Z <= 0.0001f)
    {
      error = "Scale factor is invalid.";
      return false;
    }

    return TryTransformMeshSelection(
      source,
      selectedObjectIds,
      selectedFaces,
      transform: (position, pivot) =>
      {
        Vector3 offset = position - pivot;
        return new Vector3(
          pivot.X + offset.X * scaleFactors.X,
          pivot.Y + offset.Y * scaleFactors.Y,
          pivot.Z + offset.Z * scaleFactors.Z);
      },
      operationName: "BSP scale",
      out edited,
      out error);
  }

  public static bool TryRotateMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    Vector3 axis,
    float angleDegrees,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;
    if (source == null)
    {
      error = "No map is loaded.";
      return false;
    }

    if (!IsFiniteVector3(axis) || axis.LengthSquared <= 0.000001f || !float.IsFinite(angleDegrees))
    {
      error = "Rotation axis/angle is invalid.";
      return false;
    }

    float angleRadians = MathHelper.DegreesToRadians(angleDegrees);
    if (!float.IsFinite(angleRadians) || MathF.Abs(angleRadians) <= 0.0000001f)
    {
      error = "Rotation angle is too small.";
      return false;
    }

    Vector3 axisNormalized = Vector3.Normalize(axis);
    return TryTransformMeshSelection(
      source,
      selectedObjectIds,
      selectedFaces,
      transform: (position, pivot) =>
      {
        Vector3 offset = position - pivot;
        Vector3 rotated = RotateVectorByAxisAngle(offset, axisNormalized, angleRadians);
        return pivot + rotated;
      },
      operationName: "BSP rotate",
      out edited,
      out error);
  }

  private static bool TryTransformMeshSelection(
    LoadedMap source,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    Func<Vector3, Vector3, Vector3> transform,
    string operationName,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;

    List<string> warnings = new();
    ObjImportVertex[] vertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedFaceSet(
      vertices,
      selectedObjectIds,
      selectedFaces,
      out HashSet<int> selectedFaceSet,
      out error))
    {
      return false;
    }

    bool[] selectedVertex = new bool[vertices.Length];
    foreach (int faceIndex in selectedFaceSet)
    {
      int faceBase = faceIndex * 3;
      if ((uint)(faceBase + 2) >= (uint)vertices.Length)
      {
        continue;
      }

      selectedVertex[faceBase] = true;
      selectedVertex[faceBase + 1] = true;
      selectedVertex[faceBase + 2] = true;
    }

    Vector3 pivot = Vector3.Zero;
    int pivotCount = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      if (!selectedVertex[i])
      {
        continue;
      }

      if (!IsFiniteVector3(vertices[i].Position))
      {
        continue;
      }

      pivot += vertices[i].Position;
      ++pivotCount;
    }

    if (pivotCount <= 0)
    {
      error = "Selected BSP mesh did not contain any editable vertices.";
      return false;
    }

    pivot /= pivotCount;
    if (!IsFiniteVector3(pivot))
    {
      error = "Selection pivot is invalid.";
      return false;
    }

    int transformedVertexCount = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      if (!selectedVertex[i])
      {
        continue;
      }

      ObjImportVertex v = vertices[i];
      Vector3 transformedPosition = transform(v.Position, pivot);
      Vector3 transformedLocal = transform(v.LocalPosition, pivot);
      if (!IsFiniteVector3(transformedPosition) || !IsFiniteVector3(transformedLocal))
      {
        error = $"{operationName} produced invalid vertex coordinates.";
        return false;
      }

      vertices[i] = new ObjImportVertex(
        transformedPosition,
        transformedLocal,
        v.Uv,
        v.LightUv,
        v.Color,
        v.MaterialId,
        v.LightMapId,
        v.ObjectId,
        v.HasExplicitUv);
      ++transformedVertexCount;
    }

    if (transformedVertexCount <= 0)
    {
      error = "Selected BSP mesh did not contain any editable vertices.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, vertices, warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"{operationName} rebuild failed: {ex.Message}";
      return false;
    }
  }

  private static bool TryTransformMeshPrimitiveSelection(
    LoadedMap source,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    Func<Vector3, Vector3, Vector3> transform,
    string operationName,
    out LoadedMap edited,
    out string error)
  {
    edited = source;
    error = string.Empty;

    List<string> warnings = new();
    ObjImportVertex[] vertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    if (!TryBuildSelectedPrimitiveVertexIndexSet(
      vertices,
      selectedVertices,
      selectedEdges,
      out HashSet<int> selectedVertexIndices,
      out error))
    {
      return false;
    }

    Vector3 pivot = Vector3.Zero;
    int pivotCount = 0;
    foreach (int vertexIndex in selectedVertexIndices)
    {
      if ((uint)vertexIndex >= (uint)vertices.Length)
      {
        continue;
      }

      Vector3 position = vertices[vertexIndex].Position;
      if (!IsFiniteVector3(position))
      {
        continue;
      }

      pivot += position;
      ++pivotCount;
    }

    if (pivotCount <= 0)
    {
      error = "Selected BSP primitive did not contain any editable vertices.";
      return false;
    }

    pivot /= pivotCount;
    if (!IsFiniteVector3(pivot))
    {
      error = "Selection pivot is invalid.";
      return false;
    }

    int transformedVertexCount = 0;
    foreach (int vertexIndex in selectedVertexIndices)
    {
      if ((uint)vertexIndex >= (uint)vertices.Length)
      {
        continue;
      }

      ObjImportVertex v = vertices[vertexIndex];
      Vector3 transformedPosition = transform(v.Position, pivot);
      Vector3 transformedLocal = transform(v.LocalPosition, pivot);
      if (!IsFiniteVector3(transformedPosition) || !IsFiniteVector3(transformedLocal))
      {
        error = $"{operationName} produced invalid vertex coordinates.";
        return false;
      }

      vertices[vertexIndex] = new ObjImportVertex(
        transformedPosition,
        transformedLocal,
        v.Uv,
        v.LightUv,
        v.Color,
        v.MaterialId,
        v.LightMapId,
        v.ObjectId,
        v.HasExplicitUv);
      ++transformedVertexCount;
    }

    if (transformedVertexCount <= 0)
    {
      error = "Selected BSP primitive did not contain any editable vertices.";
      return false;
    }

    try
    {
      edited = ApplyMeshVerticesToMap(source, vertices, warnings);
      return true;
    }
    catch (Exception ex)
    {
      error = $"{operationName} rebuild failed: {ex.Message}";
      return false;
    }
  }

  private static Vector3 RotateVectorByAxisAngle(Vector3 vector, Vector3 axisNormalized, float angleRadians)
  {
    float cos = MathF.Cos(angleRadians);
    float sin = MathF.Sin(angleRadians);
    return vector * cos
      + Vector3.Cross(axisNormalized, vector) * sin
      + axisNormalized * (Vector3.Dot(axisNormalized, vector) * (1.0f - cos));
  }

  private static bool TryBuildSelectedFaceSet(
    ObjImportVertex[] vertices,
    IReadOnlyCollection<int> selectedObjectIds,
    IReadOnlyCollection<BspFaceSelection> selectedFaces,
    out HashSet<int> selectedFaceSet,
    out string error)
  {
    selectedFaceSet = new HashSet<int>();
    error = string.Empty;

    int triangleCount = vertices.Length / 3;
    HashSet<ushort> objectIdSet = new();
    if (selectedObjectIds != null)
    {
      foreach (int selectedObjectId in selectedObjectIds)
      {
        if (selectedObjectId < 0 || selectedObjectId > ushort.MaxValue)
        {
          continue;
        }

        objectIdSet.Add((ushort)selectedObjectId);
      }
    }

    if (objectIdSet.Count > 0)
    {
      for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
      {
        int baseIndex = faceIndex * 3;
        ObjImportVertex v0 = vertices[baseIndex];
        ObjImportVertex v1 = vertices[baseIndex + 1];
        ObjImportVertex v2 = vertices[baseIndex + 2];
        if (objectIdSet.Contains(v0.ObjectId) || objectIdSet.Contains(v1.ObjectId) || objectIdSet.Contains(v2.ObjectId))
        {
          selectedFaceSet.Add(faceIndex);
        }
      }
    }

    if (selectedFaces != null)
    {
      foreach (BspFaceSelection selection in selectedFaces)
      {
        int faceIndex = selection.FaceIndex;
        if (faceIndex < 0 || faceIndex >= triangleCount)
        {
          continue;
        }

        if (selection.FaceOnly)
        {
          selectedFaceSet.Add(faceIndex);
          continue;
        }

        HashSet<int> connectedFaces = CollectConnectedFaceIndices(vertices, faceIndex, MaxConnectedClusterFacesForEdit, out bool selectionTooLarge);
        if (selectionTooLarge)
        {
          error = $"Connected selection exceeds safe edit limit ({MaxConnectedClusterFacesForEdit} faces). Use face-only selection.";
          return false;
        }

        foreach (int connectedFace in connectedFaces)
        {
          selectedFaceSet.Add(connectedFace);
        }
      }
    }

    if (selectedFaceSet.Count <= 0)
    {
      error = "No BSP object/face is selected.";
      return false;
    }

    return true;
  }

  private static bool TryBuildSelectedPrimitiveVertexIndexSet(
    ObjImportVertex[] vertices,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    out HashSet<int> selectedVertexIndices,
    out string error)
  {
    selectedVertexIndices = new HashSet<int>();
    error = string.Empty;
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    Dictionary<BspVertexSelection, List<int>> vertexBuckets = BuildPrimitiveVertexBuckets(vertices);
    HashSet<BspVertexSelection> vertexKeySet = new();
    if (selectedVertices != null)
    {
      foreach (BspVertexSelection vertex in selectedVertices)
      {
        vertexKeySet.Add(vertex);
      }
    }

    if (selectedEdges != null)
    {
      foreach (BspEdgeSelection edge in selectedEdges)
      {
        vertexKeySet.Add(edge.A);
        vertexKeySet.Add(edge.B);
      }
    }

    if (vertexKeySet.Count <= 0)
    {
      error = "No BSP vertex/edge is selected.";
      return false;
    }

    foreach (BspVertexSelection key in vertexKeySet)
    {
      if (!vertexBuckets.TryGetValue(key, out List<int>? indices))
      {
        continue;
      }

      for (int i = 0; i < indices.Count; ++i)
      {
        selectedVertexIndices.Add(indices[i]);
      }
    }

    if (selectedVertexIndices.Count <= 0)
    {
      error = "Selected BSP primitive does not reference editable mesh vertices.";
      return false;
    }

    return true;
  }

  private static bool TryBuildSelectedPrimitiveFaceSetForDelete(
    ObjImportVertex[] vertices,
    IReadOnlyCollection<BspVertexSelection> selectedVertices,
    IReadOnlyCollection<BspEdgeSelection> selectedEdges,
    out HashSet<int> selectedFaceSet,
    out string error)
  {
    selectedFaceSet = new HashSet<int>();
    error = string.Empty;
    if (vertices.Length < 3 || (vertices.Length % 3) != 0)
    {
      error = "Current BSP mesh is empty or malformed.";
      return false;
    }

    HashSet<BspVertexSelection> vertexKeySet = new();
    if (selectedVertices != null)
    {
      foreach (BspVertexSelection vertex in selectedVertices)
      {
        vertexKeySet.Add(vertex);
      }
    }

    HashSet<PrimitiveEdgeSelectionKey> edgeKeySet = new();
    if (selectedEdges != null)
    {
      foreach (BspEdgeSelection edge in selectedEdges)
      {
        edgeKeySet.Add(CreatePrimitiveEdgeSelectionKey(edge.A, edge.B));
      }
    }

    if (vertexKeySet.Count <= 0 && edgeKeySet.Count <= 0)
    {
      error = "No BSP vertex/edge is selected.";
      return false;
    }

    int triangleCount = vertices.Length / 3;
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int baseIndex = faceIndex * 3;
      BspVertexSelection k0 = ToPrimitiveVertexSelection(vertices[baseIndex].Position);
      BspVertexSelection k1 = ToPrimitiveVertexSelection(vertices[baseIndex + 1].Position);
      BspVertexSelection k2 = ToPrimitiveVertexSelection(vertices[baseIndex + 2].Position);

      bool removeByVertex =
        vertexKeySet.Contains(k0) ||
        vertexKeySet.Contains(k1) ||
        vertexKeySet.Contains(k2);
      if (removeByVertex)
      {
        selectedFaceSet.Add(faceIndex);
        continue;
      }

      if (edgeKeySet.Count <= 0)
      {
        continue;
      }

      PrimitiveEdgeSelectionKey e01 = CreatePrimitiveEdgeSelectionKey(k0, k1);
      PrimitiveEdgeSelectionKey e12 = CreatePrimitiveEdgeSelectionKey(k1, k2);
      PrimitiveEdgeSelectionKey e20 = CreatePrimitiveEdgeSelectionKey(k2, k0);
      if (edgeKeySet.Contains(e01) || edgeKeySet.Contains(e12) || edgeKeySet.Contains(e20))
      {
        selectedFaceSet.Add(faceIndex);
      }
    }

    if (selectedFaceSet.Count <= 0)
    {
      error = "No BSP triangles matched the primitive selection.";
      return false;
    }

    return true;
  }

  private static Dictionary<BspVertexSelection, List<int>> BuildPrimitiveVertexBuckets(ObjImportVertex[] vertices)
  {
    Dictionary<BspVertexSelection, List<int>> buckets = new(Math.Max(1024, vertices.Length / 2));
    for (int vertexIndex = 0; vertexIndex < vertices.Length; ++vertexIndex)
    {
      BspVertexSelection key = ToPrimitiveVertexSelection(vertices[vertexIndex].Position);
      if (!buckets.TryGetValue(key, out List<int>? indices))
      {
        indices = new List<int>(4);
        buckets[key] = indices;
      }

      indices.Add(vertexIndex);
    }

    return buckets;
  }

  private static BspVertexSelection ToPrimitiveVertexSelection(Vector3 position)
  {
    return new(
      BitConverter.SingleToInt32Bits(position.X),
      BitConverter.SingleToInt32Bits(position.Y),
      BitConverter.SingleToInt32Bits(position.Z));
  }

  private static PrimitiveEdgeSelectionKey CreatePrimitiveEdgeSelectionKey(BspVertexSelection a, BspVertexSelection b)
  {
    return ComparePrimitiveVertexSelection(a, b) <= 0
      ? new PrimitiveEdgeSelectionKey(a, b)
      : new PrimitiveEdgeSelectionKey(b, a);
  }

  private static int ComparePrimitiveVertexSelection(BspVertexSelection left, BspVertexSelection right)
  {
    int x = left.XBits.CompareTo(right.XBits);
    if (x != 0)
    {
      return x;
    }

    int y = left.YBits.CompareTo(right.YBits);
    if (y != 0)
    {
      return y;
    }

    return left.ZBits.CompareTo(right.ZBits);
  }

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
      ObjMeshData mesh = ReadObjMesh(meshObjPath, source.Bounds, warnings);
      if (mesh.Vertices.Length > 0 && (mesh.Vertices.Length % 3) == 0)
      {
        mesh = PreserveSourceMetadataForImportedMesh(edited, mesh, warnings);
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

  private static ObjMeshData ReadObjMesh(string objPath, MapBounds referenceBounds, List<string> warnings)
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

    ObjImportVertex[] vertices = triangleVertices.ToArray();
    if (vertices.Length > 0)
    {
      ObjImportAxisMode axisMode = ResolveObjImportAxisMode(vertices, referenceBounds);
      for (int i = 0; i < vertices.Length; ++i)
      {
        ObjImportVertex vertex = vertices[i];
        Vector3 converted = ConvertObjPositionToSource(vertex.Position, axisMode);
        vertices[i] = new ObjImportVertex(
          converted,
          converted,
          vertex.Uv,
          vertex.LightUv,
          vertex.Color,
          vertex.MaterialId,
          vertex.LightMapId,
          vertex.ObjectId,
          vertex.HasExplicitUv);
      }

      if (axisMode == ObjImportAxisMode.BlenderExportDefault)
      {
        warnings.Add("OBJ axis detected as Blender default export (Forward -Z / Up Y); applied automatic coordinate conversion.");
      }
    }

    return new ObjMeshData(vertices);
  }

  private static LoadedMap ApplyImportedMesh(LoadedMap source, ObjMeshData mesh, List<string> warnings)
  {
    return ApplyMeshVerticesToMap(source, mesh.Vertices, warnings);
  }

  private static ObjImportVertex[] BuildMeshVerticesFromLoadedMap(LoadedMap source, List<string> warnings)
  {
    BspRenderVertex[] render = source.BspRenderVertices ?? Array.Empty<BspRenderVertex>();
    int vertexCount = render.Length;
    if (vertexCount == 0)
    {
      return Array.Empty<ObjImportVertex>();
    }

    int[] materialByVertex = new int[vertexCount];
    int[] lightMapByVertex = new int[vertexCount];
    Array.Fill(materialByVertex, int.MinValue);
    Array.Fill(lightMapByVertex, int.MinValue);
    for (int spanIndex = 0; spanIndex < source.BspMaterialSpans.Length; ++spanIndex)
    {
      BspMaterialSpan span = source.BspMaterialSpans[spanIndex];
      int start = Math.Clamp(span.StartVertex, 0, vertexCount);
      int end = Math.Clamp(span.StartVertex + span.VertexCount, 0, vertexCount);
      for (int i = start; i < end; ++i)
      {
        materialByVertex[i] = span.MaterialId;
        lightMapByVertex[i] = span.LightMapId;
      }
    }

    int uncoveredCount = 0;
    int fallbackMaterialId = source.BspMaterialSpans.Length > 0 ? source.BspMaterialSpans[0].MaterialId : 0;
    int fallbackLightMapId = source.BspMaterialSpans.Length > 0 ? source.BspMaterialSpans[0].LightMapId : -1;
    for (int i = 0; i < vertexCount; ++i)
    {
      if (materialByVertex[i] != int.MinValue && lightMapByVertex[i] != int.MinValue)
      {
        continue;
      }

      ++uncoveredCount;
      materialByVertex[i] = fallbackMaterialId;
      lightMapByVertex[i] = fallbackLightMapId;
    }

    if (uncoveredCount > 0)
    {
      warnings.Add(
        $"BSP material spans did not cover {uncoveredCount:N0} render vertices; " +
        $"fallback material/lightmap ({fallbackMaterialId}/{fallbackLightMapId}) was applied.");
    }

    bool hasObjectIds = source.BspRenderVertexObjectIds.Length == vertexCount;
    bool hasLocalPositions = source.BspRenderVertexLocalPositions.Length == vertexCount;
    ObjImportVertex[] vertices = new ObjImportVertex[vertexCount];
    for (int i = 0; i < vertexCount; ++i)
    {
      BspRenderVertex v = render[i];
      ushort objectId = hasObjectIds ? source.BspRenderVertexObjectIds[i] : (ushort)0;
      Vector3 localPosition = hasLocalPositions ? source.BspRenderVertexLocalPositions[i] : v.Position;
      vertices[i] = new ObjImportVertex(v.Position, localPosition, v.Uv, v.LightUv, v.Color, materialByVertex[i], lightMapByVertex[i], objectId, true);
    }

    if ((vertexCount % 3) != 0)
    {
      warnings.Add($"Loaded BSP render vertex count ({vertexCount}) is not tri-aligned.");
    }

    return vertices;
  }

  private static ObjMeshData PreserveSourceMetadataForImportedMesh(LoadedMap source, ObjMeshData importedMesh, List<string> warnings)
  {
    ObjImportVertex[] imported = importedMesh.Vertices ?? Array.Empty<ObjImportVertex>();
    if (imported.Length == 0)
    {
      return importedMesh;
    }

    ObjImportVertex[] sourceVertices = BuildMeshVerticesFromLoadedMap(source, warnings);
    if (sourceVertices.Length == 0)
    {
      return importedMesh;
    }

    ObjImportVertex[] merged = new ObjImportVertex[imported.Length];
    int materialCount = source.Materials?.Length ?? 0;
    int preservedCount = 0;

    if (sourceVertices.Length == imported.Length)
    {
      for (int index = 0; index < imported.Length; ++index)
      {
        merged[index] = CopySourceMetadata(imported[index], sourceVertices[index], materialCount, out bool preserved);
        if (preserved)
        {
          ++preservedCount;
        }
      }

      if (preservedCount > 0)
      {
        warnings.Add($"OBJ import preserved source light/color/object metadata for {preservedCount:N0} vertices (index-matched).");
      }

      return new ObjMeshData(merged);
    }

    Dictionary<QuantizedVertexKey, List<int>> sourceByPosition = new(Math.Max(1024, sourceVertices.Length / 3));
    for (int sourceIndex = 0; sourceIndex < sourceVertices.Length; ++sourceIndex)
    {
      QuantizedVertexKey key = QuantizeVertex(sourceVertices[sourceIndex].Position);
      if (!sourceByPosition.TryGetValue(key, out List<int>? indices))
      {
        indices = new List<int>(2);
        sourceByPosition[key] = indices;
      }

      indices.Add(sourceIndex);
    }

    Dictionary<QuantizedVertexKey, int> sourceCursorByKey = new(sourceByPosition.Count);
    for (int index = 0; index < imported.Length; ++index)
    {
      ObjImportVertex importedVertex = imported[index];
      QuantizedVertexKey key = QuantizeVertex(importedVertex.Position);
      if (sourceByPosition.TryGetValue(key, out List<int>? sourceIndices)
        && sourceIndices.Count > 0)
      {
        int cursor = 0;
        sourceCursorByKey.TryGetValue(key, out cursor);
        if (cursor < sourceIndices.Count)
        {
          int matchedSourceIndex = sourceIndices[cursor];
          sourceCursorByKey[key] = cursor + 1;
          merged[index] = CopySourceMetadata(importedVertex, sourceVertices[matchedSourceIndex], materialCount, out bool preserved);
          if (preserved)
          {
            ++preservedCount;
          }

          continue;
        }
      }

      merged[index] = importedVertex;
    }

    if (preservedCount > 0)
    {
      warnings.Add(
        $"OBJ import preserved source light/color/object metadata for {preservedCount:N0} vertices by position match " +
        $"(source vertices: {sourceVertices.Length:N0}, imported vertices: {imported.Length:N0}).");
    }
    else
    {
      warnings.Add(
        $"OBJ topology differs from source (source vertices: {sourceVertices.Length:N0}, imported vertices: {imported.Length:N0}); " +
        "new vertices keep OBJ defaults for light/color/object metadata.");
    }

    return new ObjMeshData(merged);
  }

  private static ObjImportVertex CopySourceMetadata(ObjImportVertex imported, ObjImportVertex source, int materialCount, out bool preserved)
  {
    int materialId = imported.MaterialId;
    if (materialCount > 0 && (materialId < 0 || materialId >= materialCount))
    {
      materialId = source.MaterialId;
    }

    Vector2 uv = imported.HasExplicitUv ? imported.Uv : source.Uv;
    if (!float.IsFinite(uv.X) || !float.IsFinite(uv.Y))
    {
      uv = source.Uv;
    }

    Vector3 localPosition = imported.LocalPosition;
    if (IsFiniteVector3(imported.Position)
      && IsFiniteVector3(source.Position)
      && (imported.Position - source.Position).LengthSquared <= MetadataMatchEpsilonSq)
    {
      localPosition = source.LocalPosition;
    }

    preserved = true;
    return new ObjImportVertex(
      imported.Position,
      localPosition,
      uv,
      source.LightUv,
      source.Color,
      materialId,
      source.LightMapId,
      source.ObjectId,
      imported.HasExplicitUv);
  }

  private static QuantizedVertexKey QuantizeVertex(in Vector3 position)
  {
    return new QuantizedVertexKey(
      QuantizePositionComponent(position.X),
      QuantizePositionComponent(position.Y),
      QuantizePositionComponent(position.Z));
  }

  private static int QuantizePositionComponent(float value)
  {
    if (!float.IsFinite(value))
    {
      return 0;
    }

    return checked((int)MathF.Round(value * MetadataPositionQuantizeScale));
  }

  private static LoadedMap ApplyMeshVerticesToMap(LoadedMap source, ObjImportVertex[] vertices, List<string> warnings)
  {
    BspBinaryLayout layout = source.BspBinaryLayout;
    if (layout == null || layout.Sections.Length != BspHeaderEntryCount || layout.Entries.Length != BspHeaderEntryCount)
    {
      throw new InvalidDataException("Current map does not expose a valid BSP binary layout for mesh rebuild.");
    }

    if ((vertices.Length % 3) != 0)
    {
      throw new InvalidDataException("BSP mesh edit does not resolve to a triangle list.");
    }

    int triangleCount = vertices.Length / 3;
    int vertexCount = vertices.Length;
    IndexedVertexData indexedVertices = BuildIndexedVertexData(vertices, warnings);

    BspRenderVertex[] renderVertices = new BspRenderVertex[vertexCount];
    ushort[] vertexObjectIds = new ushort[vertexCount];
    Vector3[] vertexLocalPositions = new Vector3[vertexCount];
    for (int i = 0; i < vertexCount; ++i)
    {
      ObjImportVertex sourceVertex = vertices[i];
      renderVertices[i] = new BspRenderVertex(
        sourceVertex.Position,
        sourceVertex.Uv,
        sourceVertex.LightUv,
        sourceVertex.Color);
      vertexObjectIds[i] = sourceVertex.ObjectId;
      vertexLocalPositions[i] = sourceVertex.LocalPosition;
    }

    List<MaterialRun> materialRuns = BuildMaterialRuns(vertices);
    List<MaterialRun> expandedMaterialRuns = ExpandMaterialRuns(materialRuns);
    int[] faceGroupLookup = BuildFaceGroupLookup(triangleCount, expandedMaterialRuns);
    SpatialBspSections spatialBsp;
    if (!TryBuildSourceGuidedSpatialBspSections(source, vertices, faceGroupLookup, expandedMaterialRuns.Count, warnings, out spatialBsp, out string spatialError))
    {
      warnings.Add($"Source-guided BSP leaf mapping unavailable ({spatialError}); using generated spatial BSP.");

      int sourceLeafCount = ResolveSourceBspLeafCount(layout);
      int collisionLeafHint = ResolveCollisionLeafCountHint(source.CollisionLines);
      int leafTargetCount = Math.Max(sourceLeafCount, collisionLeafHint);
      int targetFacesPerLeaf = ResolveTargetFacesPerLeaf(triangleCount, leafTargetCount);
      spatialBsp = BuildSpatialBspSections(vertices, faceGroupLookup, expandedMaterialRuns.Count, targetFacesPerLeaf, warnings);
    }

    BspMaterialSpan[] spans = new BspMaterialSpan[materialRuns.Count];
    for (int runIndex = 0; runIndex < materialRuns.Count; ++runIndex)
    {
      MaterialRun run = materialRuns[runIndex];
      spans[runIndex] = new BspMaterialSpan(
        run.MaterialId,
        run.FaceStart * 3,
        run.FaceCount * 3,
        run.LightMapId);
    }

    byte[] fVertexSection = SerializeFloatVertexSection(indexedVertices.UniquePositions);
    byte[] vertexColorSection = SerializeVertexColorSection(vertices);
    byte[] uvSection = SerializeUvSection(vertices);
    byte[] lightUvSection = SerializeLightUvSection(vertices);
    byte[] faceSection = SerializeFaceSection(triangleCount);
    byte[] faceIdSection = SerializeFaceIdSection(triangleCount);
    byte[] vertexIdSection = SerializeVertexIdSection(indexedVertices.VertexIds);
    SourceMatGroupMetadata sourceMatGroupMetadata = BuildSourceMatGroupMetadata(layout.Sections[BspSectionReadMatGroup], warnings);
    byte[] matGroupSection = SerializeMatGroupSection(expandedMaterialRuns, vertices, sourceMatGroupMetadata, warnings);

    byte[][] sections = CloneSections(layout.Sections);
    sections[BspSectionCPlanes] = spatialBsp.CPlaneSection;
    sections[BspSectionCFaceId] = spatialBsp.CFaceIdSection;
    sections[BspSectionNode] = spatialBsp.NodeSection;
    sections[BspSectionLeaf] = spatialBsp.LeafSection;
    sections[BspSectionMatListInLeaf] = spatialBsp.MatListInLeafSection;
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

    LoadedMap rebuilt = MapEditOperations.WithBspMeshData(
      source,
      updatedLayout,
      renderVertices,
      spans,
      source.BspSceneObjects,
      vertexObjectIds,
      vertexLocalPositions,
      spatialBsp.Normals,
      spatialBsp.Nodes,
      spatialBsp.LeafBounds);

    BspStrictValidator.ValidateForSave(rebuilt);
    return rebuilt;
  }

  private static HashSet<int> CollectConnectedFaceIndices(ObjImportVertex[] vertices, int startFaceIndex, int maxFaces, out bool selectionTooLarge)
  {
    selectionTooLarge = false;
    HashSet<int> result = new();
    int triangleCount = vertices.Length / 3;
    if (startFaceIndex < 0 || startFaceIndex >= triangleCount)
    {
      return result;
    }

    Dictionary<VertexPositionKey, List<int>> faceByVertex = new(Math.Max(1024, vertices.Length / 2));
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int baseVertex = faceIndex * 3;
      for (int k = 0; k < 3; ++k)
      {
        Vector3 p = vertices[baseVertex + k].Position;
        if (!IsFiniteVector3(p))
        {
          continue;
        }

        VertexPositionKey key = new(
          BitConverter.SingleToInt32Bits(p.X),
          BitConverter.SingleToInt32Bits(p.Y),
          BitConverter.SingleToInt32Bits(p.Z));
        if (!faceByVertex.TryGetValue(key, out List<int>? list))
        {
          list = new List<int>(4);
          faceByVertex[key] = list;
        }

        list.Add(faceIndex);
      }
    }

    Queue<int> pending = new();
    result.Add(startFaceIndex);
    pending.Enqueue(startFaceIndex);
    while (pending.Count > 0)
    {
      int faceIndex = pending.Dequeue();
      int baseVertex = faceIndex * 3;
      for (int k = 0; k < 3; ++k)
      {
        Vector3 p = vertices[baseVertex + k].Position;
        if (!IsFiniteVector3(p))
        {
          continue;
        }

        VertexPositionKey key = new(
          BitConverter.SingleToInt32Bits(p.X),
          BitConverter.SingleToInt32Bits(p.Y),
          BitConverter.SingleToInt32Bits(p.Z));
        if (!faceByVertex.TryGetValue(key, out List<int>? neighbors))
        {
          continue;
        }

        for (int i = 0; i < neighbors.Count; ++i)
        {
          int neighborFace = neighbors[i];
          if (result.Add(neighborFace))
          {
            if (result.Count >= maxFaces)
            {
              selectionTooLarge = true;
              return result;
            }

            pending.Enqueue(neighborFace);
          }
        }
      }
    }

    return result;
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
    int lightMapId = vertices[0].LightMapId;
    ushort objectId = vertices[0].ObjectId;
    int faceCount = 0;
    for (int face = 0; face < triangleCount; ++face)
    {
      int faceVertexBase = face * 3;
      int triMaterial = vertices[faceVertexBase].MaterialId;
      int triLightMap = vertices[faceVertexBase].LightMapId;
      ushort triObject = vertices[faceVertexBase].ObjectId;
      if ((triMaterial != materialId || triLightMap != lightMapId || triObject != objectId) && faceCount > 0)
      {
        runs.Add(new MaterialRun(materialId, lightMapId, objectId, faceStart, faceCount));
        faceStart = face;
        faceCount = 0;
        materialId = triMaterial;
        lightMapId = triLightMap;
        objectId = triObject;
      }

      ++faceCount;
    }

    if (faceCount > 0)
    {
      runs.Add(new MaterialRun(materialId, lightMapId, objectId, faceStart, faceCount));
    }

    return runs;
  }

  private static List<MaterialRun> ExpandMaterialRuns(List<MaterialRun> runs)
  {
    List<MaterialRun> expanded = new(Math.Max(1, runs.Count));
    for (int i = 0; i < runs.Count; ++i)
    {
      MaterialRun run = runs[i];
      int remaining = Math.Max(0, run.FaceCount);
      int faceStart = Math.Max(0, run.FaceStart);
      while (remaining > 0)
      {
        int chunk = Math.Min(remaining, ushort.MaxValue);
        expanded.Add(new MaterialRun(run.MaterialId, run.LightMapId, run.ObjectId, faceStart, chunk));
        faceStart += chunk;
        remaining -= chunk;
      }
    }

    return expanded;
  }

  private static int[] BuildFaceGroupLookup(int triangleCount, List<MaterialRun> expandedRuns)
  {
    int[] faceGroupLookup = new int[Math.Max(0, triangleCount)];
    if (triangleCount <= 0 || expandedRuns.Count == 0)
    {
      return faceGroupLookup;
    }

    bool[] assigned = new bool[triangleCount];
    for (int groupIndex = 0; groupIndex < expandedRuns.Count; ++groupIndex)
    {
      MaterialRun run = expandedRuns[groupIndex];
      int start = Math.Clamp(run.FaceStart, 0, triangleCount);
      int end = Math.Clamp(run.FaceStart + run.FaceCount, 0, triangleCount);
      for (int face = start; face < end; ++face)
      {
        faceGroupLookup[face] = groupIndex;
        assigned[face] = true;
      }
    }

    int fallback = 0;
    for (int face = 0; face < triangleCount; ++face)
    {
      if (!assigned[face])
      {
        faceGroupLookup[face] = fallback;
      }
      else
      {
        fallback = faceGroupLookup[face];
      }
    }

    return faceGroupLookup;
  }

  private static SpatialBspSections BuildSpatialBspSections(
    ObjImportVertex[] vertices,
    int[] faceGroupLookup,
    int materialGroupCount,
    int targetFacesPerLeaf,
    List<string> warnings)
  {
    int triangleCount = vertices.Length / 3;
    if (triangleCount <= 0)
    {
      Vector3[] fallbackNormals = [Vector3.Zero, Vector3.UnitX];
      BspNode[] fallbackNodes =
      [
        new BspNode(0u, 0.0f, 0, 0, Vector3.Zero, Vector3.Zero),
        new BspNode(1u, 0.0f, -2, -2, Vector3.Zero, Vector3.Zero),
      ];
      BspLeafBounds[] fallbackLeaves =
      [
        new BspLeafBounds(Vector3.Zero, Vector3.Zero),
        new BspLeafBounds(Vector3.Zero, Vector3.Zero),
      ];

      return new SpatialBspSections(
        SerializeCPlaneSection(fallbackNormals),
        Array.Empty<byte>(),
        SerializeNodeSection(fallbackNodes),
        SerializeLeafSection(
          [
            new SpatialLeafEntry(0, 0, 0u, 0, 0u, Vector3.Zero, Vector3.Zero),
            new SpatialLeafEntry(0, 0, 0u, 0, 0u, Vector3.Zero, Vector3.Zero),
          ]),
        Array.Empty<byte>(),
        fallbackNormals,
        fallbackNodes,
        fallbackLeaves);
    }

    if (faceGroupLookup.Length != triangleCount)
    {
      int[] resized = new int[triangleCount];
      int copy = Math.Min(faceGroupLookup.Length, triangleCount);
      if (copy > 0)
      {
        Array.Copy(faceGroupLookup, resized, copy);
      }

      faceGroupLookup = resized;
    }

    FaceSpatialData[] faces = BuildFaceSpatialData(vertices);
    List<Vector3> normals = [Vector3.Zero, Vector3.UnitX, Vector3.UnitY, Vector3.UnitZ];
    List<SpatialNodeTemp> nodes = [new SpatialNodeTemp(0u, 0.0f, 0, 0, Vector3.Zero, Vector3.Zero)];
    List<SpatialLeafEntry> leaves = [new SpatialLeafEntry(0, 0, 0u, 0, 0u, Vector3.Zero, Vector3.Zero)];
    List<uint> cFaceIds = new(Math.Max(1024, triangleCount));
    List<ushort> matListInLeaf = new(Math.Max(1024, triangleCount / 4));

    Vector3 rootMin = faces[0].Min;
    Vector3 rootMax = faces[0].Max;
    for (int i = 1; i < faces.Length; ++i)
    {
      rootMin = Vector3.ComponentMin(rootMin, faces[i].Min);
      rootMax = Vector3.ComponentMax(rootMax, faces[i].Max);
    }

    if (!IsFiniteVector3(rootMin) || !IsFiniteVector3(rootMax))
    {
      rootMin = Vector3.Zero;
      rootMax = Vector3.Zero;
    }

    List<int> allFaces = new(triangleCount);
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      allFaces.Add(faceIndex);
    }

    SpatialChildRef BuildRecursive(List<int> workingFaces, Vector3 boundsMin, Vector3 boundsMax, int depth)
    {
      bool forceSplitForLeafLimit = workingFaces.Count > MaxLeafFaceCount;
      bool forceSplitForDepth = depth < MaxSpatialTreeDepth && workingFaces.Count > targetFacesPerLeaf;
      bool shouldLeaf = !forceSplitForLeafLimit && !forceSplitForDepth;

      if (shouldLeaf)
      {
        int leafIndex = AddLeaf(
          workingFaces,
          boundsMin,
          boundsMax,
          faceGroupLookup,
          materialGroupCount,
          cFaceIds,
          matListInLeaf,
          leaves,
          warnings);
        return new SpatialChildRef(true, leafIndex);
      }

      bool hasSplit = TryFindSplitPlane(
        faces,
        workingFaces,
        boundsMin,
        boundsMax,
        out int splitAxis,
        out float splitDistance,
        out List<int> frontFaces,
        out List<int> backFaces);

      if (!hasSplit)
      {
        if (!TryForceSplitByMedian(
          faces,
          workingFaces,
          boundsMin,
          boundsMax,
          out splitAxis,
          out splitDistance,
          out frontFaces,
          out backFaces))
        {
          int forcedLeafIndex = AddLeaf(
            workingFaces,
            boundsMin,
            boundsMax,
            faceGroupLookup,
            materialGroupCount,
            cFaceIds,
            matListInLeaf,
            leaves,
            warnings);
          return new SpatialChildRef(true, forcedLeafIndex);
        }
      }

      if (nodes.Count >= MaxBspNodeCount)
      {
        warnings.Add($"Spatial BSP node cap {MaxBspNodeCount} reached; remaining faces packed into a leaf.");
        int cappedLeafIndex = AddLeaf(
          workingFaces,
          boundsMin,
          boundsMax,
          faceGroupLookup,
          materialGroupCount,
          cFaceIds,
          matListInLeaf,
          leaves,
          warnings);
        return new SpatialChildRef(true, cappedLeafIndex);
      }

      int nodeIndex = nodes.Count;
      nodes.Add(default);

      BuildChildBounds(boundsMin, boundsMax, splitAxis, splitDistance, out Vector3 frontMin, out Vector3 frontMax, out Vector3 backMin, out Vector3 backMax);

      SpatialChildRef frontRef = BuildRecursive(frontFaces, frontMin, frontMax, depth + 1);
      SpatialChildRef backRef = BuildRecursive(backFaces, backMin, backMax, depth + 1);

      short frontChild = EncodeSpatialChild(frontRef);
      short backChild = EncodeSpatialChild(backRef);
      uint normalIndex = (uint)ResolveAxisNormalIndex(splitAxis);

      nodes[nodeIndex] = new SpatialNodeTemp(
        normalIndex,
        splitDistance,
        frontChild,
        backChild,
        boundsMin,
        boundsMax);

      return new SpatialChildRef(false, nodeIndex);
    }

    SpatialChildRef rootRef = BuildRecursive(allFaces, rootMin, rootMax, 0);
    if (rootRef.IsLeaf)
    {
      // Runtime leaf walk starts at node index 1, so synthesize a root splitter when recursion ends in a single leaf.
      int axis = GetLongestAxis(rootMin, rootMax);
      float split = 0.5f * (GetAxisValue(rootMin, axis) + GetAxisValue(rootMax, axis));
      short child = EncodeSpatialChild(rootRef);
      nodes.Add(new SpatialNodeTemp((uint)ResolveAxisNormalIndex(axis), split, child, child, rootMin, rootMax));
    }

    BspNode[] nodeState = BuildBspNodeState(nodes);
    BspLeafBounds[] leafState = BuildBspLeafState(leaves);
    Vector3[] normalArray = normals.ToArray();
    byte[] cPlaneSection = SerializeCPlaneSection(normalArray);
    byte[] cFaceIdSection = SerializeCFaceIdSection(cFaceIds);
    byte[] nodeSection = SerializeNodeSection(nodeState);
    byte[] leafSection = SerializeLeafSection(leaves.ToArray());
    byte[] matListInLeafSection = SerializeMatListInLeafSection(matListInLeaf);

    return new SpatialBspSections(
      cPlaneSection,
      cFaceIdSection,
      nodeSection,
      leafSection,
      matListInLeafSection,
      normalArray,
      nodeState,
      leafState);
  }

  private static bool TryBuildSourceGuidedSpatialBspSections(
    LoadedMap source,
    ObjImportVertex[] vertices,
    int[] faceGroupLookup,
    int materialGroupCount,
    List<string> warnings,
    out SpatialBspSections spatialBsp,
    out string error)
  {
    spatialBsp = default;
    error = string.Empty;

    BspBinaryLayout? layout = source.BspBinaryLayout;
    if (layout == null
      || layout.Sections == null
      || layout.Sections.Length != BspHeaderEntryCount)
    {
      error = "missing BSP layout";
      return false;
    }

    byte[] sourceCPlaneSection = layout.Sections[BspSectionCPlanes] ?? Array.Empty<byte>();
    byte[] sourceNodeSection = layout.Sections[BspSectionNode] ?? Array.Empty<byte>();
    byte[] sourceLeafSection = layout.Sections[BspSectionLeaf] ?? Array.Empty<byte>();
    if (sourceCPlaneSection.Length == 0
      || sourceNodeSection.Length == 0
      || sourceLeafSection.Length == 0)
    {
      error = "source BSP tree sections are empty";
      return false;
    }

    if ((sourceCPlaneSection.Length % 12) != 0
      || (sourceNodeSection.Length % 24) != 0
      || (sourceLeafSection.Length % BspLeafSerializedSize) != 0)
    {
      error = "source BSP tree section stride mismatch";
      return false;
    }

    Vector3[] normals = ParseCPlaneNormals(sourceCPlaneSection);
    BspNode[] nodes = ParseSourceNodes(sourceNodeSection);
    SourceLeafEntry[] sourceLeaves = ParseSourceLeafEntries(sourceLeafSection);
    if (normals.Length == 0 || nodes.Length <= 1 || sourceLeaves.Length <= 1)
    {
      error = "source BSP tree does not contain runtime nodes/leaves";
      return false;
    }

    int triangleCount = vertices.Length / 3;
    if (triangleCount <= 0)
    {
      error = "mesh has no triangles";
      return false;
    }

    FaceSpatialData[] faces = BuildFaceSpatialData(vertices);
    List<uint>[] leafFaceRefs = new List<uint>[sourceLeaves.Length];
    List<ushort>[] leafMatRefs = new List<ushort>[sourceLeaves.Length];
    List<int> hitLeaves = new(32);
    Stack<int> pendingNodes = new(32);
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      FaceSpatialData face = faces[faceIndex];
      hitLeaves.Clear();
      CollectSourceLeafHitsFromBounds(face.Min, face.Max, normals, nodes, sourceLeaves.Length, pendingNodes, hitLeaves);
      if (hitLeaves.Count == 0)
      {
        int centroidLeaf = FindSourceLeafForPoint(face.Centroid, normals, nodes, sourceLeaves.Length);
        if (centroidLeaf > 0)
        {
          hitLeaves.Add(centroidLeaf);
        }
      }

      if (hitLeaves.Count == 0)
      {
        hitLeaves.Add(Math.Min(1, sourceLeaves.Length - 1));
      }

      int materialGroupId = faceIndex < faceGroupLookup.Length ? faceGroupLookup[faceIndex] : 0;
      for (int i = 0; i < hitLeaves.Count; ++i)
      {
        int leafId = hitLeaves[i];
        if (leafId <= 0 || leafId >= sourceLeaves.Length)
        {
          continue;
        }

        List<uint>? faceRefs = leafFaceRefs[leafId];
        if (faceRefs == null)
        {
          faceRefs = new List<uint>(8);
          leafFaceRefs[leafId] = faceRefs;
        }

        faceRefs.Add((uint)faceIndex);

        if (materialGroupId >= 0 && materialGroupId < materialGroupCount)
        {
          if (materialGroupId > ushort.MaxValue)
          {
            continue;
          }

          List<ushort>? matRefs = leafMatRefs[leafId];
          if (matRefs == null)
          {
            matRefs = new List<ushort>(4);
            leafMatRefs[leafId] = matRefs;
          }

          ushort matRef = (ushort)materialGroupId;
          if (!matRefs.Contains(matRef))
          {
            matRefs.Add(matRef);
          }
        }
      }
    }

    List<uint> cFaceIds = new(Math.Max(1024, triangleCount));
    List<ushort> matListInLeaf = new(Math.Max(1024, triangleCount / 4));
    SpatialLeafEntry[] rebuiltLeaves = new SpatialLeafEntry[sourceLeaves.Length];
    for (int leafIndex = 0; leafIndex < sourceLeaves.Length; ++leafIndex)
    {
      SourceLeafEntry sourceLeaf = sourceLeaves[leafIndex];

      uint faceStartId = (uint)cFaceIds.Count;
      List<uint>? faceRefs = leafFaceRefs[leafIndex];
      int leafFaceCount = faceRefs?.Count ?? 0;
      if (leafFaceCount > ushort.MaxValue)
      {
        warnings.Add($"Leaf {leafIndex} face ref count {leafFaceCount} exceeds ushort range; truncating to {ushort.MaxValue}.");
        leafFaceCount = ushort.MaxValue;
      }

      for (int i = 0; i < leafFaceCount; ++i)
      {
        cFaceIds.Add(faceRefs![i]);
      }

      uint matStartId = (uint)matListInLeaf.Count;
      List<ushort>? matRefs = leafMatRefs[leafIndex];
      int matCount = matRefs?.Count ?? 0;
      if (matCount > ushort.MaxValue)
      {
        warnings.Add($"Leaf {leafIndex} material-group ref count {matCount} exceeds ushort range; truncating to {ushort.MaxValue}.");
        matCount = ushort.MaxValue;
      }

      for (int i = 0; i < matCount; ++i)
      {
        matListInLeaf.Add(matRefs![i]);
      }

      rebuiltLeaves[leafIndex] = new SpatialLeafEntry(
        sourceLeaf.Type,
        (ushort)leafFaceCount,
        faceStartId,
        (ushort)matCount,
        matStartId,
        sourceLeaf.Min,
        sourceLeaf.Max);
    }

    byte[] cFaceIdSection = SerializeCFaceIdSection(cFaceIds);
    byte[] leafSection = SerializeLeafSection(rebuiltLeaves);
    byte[] matListInLeafSection = SerializeMatListInLeafSection(matListInLeaf);
    BspLeafBounds[] leafState = BuildSourceLeafState(sourceLeaves);

    spatialBsp = new SpatialBspSections(
      sourceCPlaneSection.ToArray(),
      cFaceIdSection,
      sourceNodeSection.ToArray(),
      leafSection,
      matListInLeafSection,
      normals,
      nodes,
      leafState);
    return true;
  }

  private static void CollectSourceLeafHitsFromBounds(
    Vector3 boundsMin,
    Vector3 boundsMax,
    Vector3[] normals,
    BspNode[] nodes,
    int leafCount,
    Stack<int> pendingNodes,
    List<int> resultLeaves)
  {
    if (nodes.Length <= 1)
    {
      return;
    }

    pendingNodes.Clear();
    pendingNodes.Push(1);
    while (pendingNodes.Count > 0)
    {
      int nodeIndex = pendingNodes.Pop();
      if (nodeIndex <= 0 || nodeIndex >= nodes.Length)
      {
        continue;
      }

      BspNode node = nodes[nodeIndex];
      if (node.NormalIndex >= normals.Length)
      {
        continue;
      }

      Vector3 normal = normals[node.NormalIndex];
      float nearDot = GetAabbDot(normal, boundsMin, boundsMax, far: false);
      float farDot = GetAabbDot(normal, boundsMin, boundsMax, far: true);
      float planeDistance = node.Distance;
      if (!float.IsFinite(nearDot) || !float.IsFinite(farDot) || !float.IsFinite(planeDistance))
      {
        continue;
      }

      if (nearDot >= planeDistance)
      {
        PushSpatialChild(node.Front, leafCount, pendingNodes, resultLeaves);
      }
      else if (farDot < planeDistance)
      {
        PushSpatialChild(node.Back, leafCount, pendingNodes, resultLeaves);
      }
      else
      {
        PushSpatialChild(node.Front, leafCount, pendingNodes, resultLeaves);
        PushSpatialChild(node.Back, leafCount, pendingNodes, resultLeaves);
      }
    }
  }

  private static void PushSpatialChild(
    short child,
    int leafCount,
    Stack<int> pendingNodes,
    List<int> resultLeaves)
  {
    if (child > 0)
    {
      pendingNodes.Push(child);
      return;
    }

    if (child >= 0)
    {
      return;
    }

    int leafIndex = -child - 1;
    if (leafIndex <= 0 || leafIndex >= leafCount)
    {
      return;
    }

    AddUniqueLeaf(resultLeaves, leafIndex);
  }

  private static void AddUniqueLeaf(List<int> leaves, int leafIndex)
  {
    for (int i = 0; i < leaves.Count; ++i)
    {
      if (leaves[i] == leafIndex)
      {
        return;
      }
    }

    leaves.Add(leafIndex);
  }

  private static int FindSourceLeafForPoint(
    Vector3 point,
    Vector3[] normals,
    BspNode[] nodes,
    int leafCount)
  {
    if (nodes.Length <= 1 || normals.Length == 0)
    {
      return -1;
    }

    int nodeIndex = 1;
    int guard = 0;
    while (nodeIndex > 0 && nodeIndex < nodes.Length && guard++ < nodes.Length + 16)
    {
      BspNode node = nodes[nodeIndex];
      if (node.NormalIndex >= normals.Length)
      {
        return -1;
      }

      Vector3 normal = normals[node.NormalIndex];
      float side = Vector3.Dot(normal, point) - node.Distance;
      short child = side >= 0.0f ? node.Front : node.Back;
      if (child < 0)
      {
        int leafIndex = -child - 1;
        return leafIndex > 0 && leafIndex < leafCount ? leafIndex : -1;
      }

      if (child <= 0 || child >= nodes.Length)
      {
        return -1;
      }

      nodeIndex = child;
    }

    return -1;
  }

  private static float GetAabbDot(Vector3 normal, Vector3 min, Vector3 max, bool far)
  {
    float x = far
      ? (normal.X >= 0.0f ? max.X : min.X)
      : (normal.X >= 0.0f ? min.X : max.X);
    float y = far
      ? (normal.Y >= 0.0f ? max.Y : min.Y)
      : (normal.Y >= 0.0f ? min.Y : max.Y);
    float z = far
      ? (normal.Z >= 0.0f ? max.Z : min.Z)
      : (normal.Z >= 0.0f ? min.Z : max.Z);
    return normal.X * x + normal.Y * y + normal.Z * z;
  }

  private static Vector3[] ParseCPlaneNormals(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<Vector3>();
    }

    int count = bytes.Length / 12;
    Vector3[] normals = new Vector3[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 12;
      float x = BinaryPrimitives.ReadSingleLittleEndian(bytes.AsSpan(offset, 4));
      float y = BinaryPrimitives.ReadSingleLittleEndian(bytes.AsSpan(offset + 4, 4));
      float z = BinaryPrimitives.ReadSingleLittleEndian(bytes.AsSpan(offset + 8, 4));
      normals[i] = new Vector3(x, y, z);
    }

    return normals;
  }

  private static BspNode[] ParseSourceNodes(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<BspNode>();
    }

    int count = bytes.Length / 24;
    BspNode[] nodes = new BspNode[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 24;
      uint normalIndex = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(offset, 4));
      float distance = BinaryPrimitives.ReadSingleLittleEndian(bytes.AsSpan(offset + 4, 4));
      short front = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 8, 2));
      short back = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 10, 2));
      short minX = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 12, 2));
      short minY = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 14, 2));
      short minZ = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 16, 2));
      short maxX = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 18, 2));
      short maxY = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 20, 2));
      short maxZ = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 22, 2));
      nodes[i] = new BspNode(
        normalIndex,
        distance,
        front,
        back,
        new Vector3(minX, minY, minZ),
        new Vector3(maxX, maxY, maxZ));
    }

    return nodes;
  }

  private static SourceLeafEntry[] ParseSourceLeafEntries(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<SourceLeafEntry>();
    }

    int count = bytes.Length / BspLeafSerializedSize;
    SourceLeafEntry[] leaves = new SourceLeafEntry[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * BspLeafSerializedSize;
      byte type = bytes[offset];
      short minX = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 13, 2));
      short minY = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 15, 2));
      short minZ = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 17, 2));
      short maxX = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 19, 2));
      short maxY = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 21, 2));
      short maxZ = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 23, 2));
      leaves[i] = new SourceLeafEntry(
        type,
        new Vector3(minX, minY, minZ),
        new Vector3(maxX, maxY, maxZ));
    }

    return leaves;
  }

  private static BspLeafBounds[] BuildSourceLeafState(SourceLeafEntry[] sourceLeaves)
  {
    BspLeafBounds[] state = new BspLeafBounds[sourceLeaves.Length];
    for (int i = 0; i < sourceLeaves.Length; ++i)
    {
      SourceLeafEntry leaf = sourceLeaves[i];
      state[i] = new BspLeafBounds(leaf.Min, leaf.Max);
    }

    return state;
  }

  private static int ResolveSourceBspLeafCount(BspBinaryLayout layout)
  {
    if (layout == null || layout.Sections == null || layout.Sections.Length <= BspSectionLeaf)
    {
      return 0;
    }

    byte[] section = layout.Sections[BspSectionLeaf] ?? Array.Empty<byte>();
    if (section.Length <= 0 || (section.Length % BspLeafSerializedSize) != 0)
    {
      return 0;
    }

    return section.Length / BspLeafSerializedSize;
  }

  private static int ResolveCollisionLeafCountHint(CollisionLine[] collisionLines)
  {
    if (collisionLines == null || collisionLines.Length <= 1)
    {
      return 0;
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

    return maxLeafId >= 0 ? maxLeafId + 1 : 0;
  }

  private static int ResolveTargetFacesPerLeaf(int triangleCount, int leafTargetCount)
  {
    if (triangleCount <= 0)
    {
      return DefaultTargetFacesPerLeaf;
    }

    int boundedLeafTarget = leafTargetCount;
    if (boundedLeafTarget <= 0)
    {
      return DefaultTargetFacesPerLeaf;
    }

    if (boundedLeafTarget > MaxBspLeafCount)
    {
      boundedLeafTarget = MaxBspLeafCount;
    }

    int target = (int)Math.Ceiling(triangleCount / (double)Math.Max(1, boundedLeafTarget));
    return Math.Clamp(target, 1, MaxLeafFaceCount);
  }

  private static FaceSpatialData[] BuildFaceSpatialData(ObjImportVertex[] vertices)
  {
    int triangleCount = vertices.Length / 3;
    FaceSpatialData[] faces = new FaceSpatialData[triangleCount];
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int vertexBase = faceIndex * 3;
      Vector3 a = vertices[vertexBase].Position;
      Vector3 b = vertices[vertexBase + 1].Position;
      Vector3 c = vertices[vertexBase + 2].Position;

      if (!IsFiniteVector3(a))
      {
        a = Vector3.Zero;
      }

      if (!IsFiniteVector3(b))
      {
        b = Vector3.Zero;
      }

      if (!IsFiniteVector3(c))
      {
        c = Vector3.Zero;
      }

      Vector3 min = Vector3.ComponentMin(Vector3.ComponentMin(a, b), c);
      Vector3 max = Vector3.ComponentMax(Vector3.ComponentMax(a, b), c);
      Vector3 centroid = (a + b + c) / 3.0f;
      faces[faceIndex] = new FaceSpatialData(min, max, centroid);
    }

    return faces;
  }

  private static bool TryFindSplitPlane(
    FaceSpatialData[] faces,
    List<int> workingFaces,
    Vector3 boundsMin,
    Vector3 boundsMax,
    out int splitAxis,
    out float splitDistance,
    out List<int> frontFaces,
    out List<int> backFaces)
  {
    splitAxis = 0;
    splitDistance = 0.0f;
    frontFaces = new List<int>();
    backFaces = new List<int>();

    if (workingFaces.Count <= 1)
    {
      return false;
    }

    int[] axisOrder = BuildAxisOrder(boundsMin, boundsMax);
    float bestScore = float.NegativeInfinity;
    int bestAxis = -1;
    float bestDistance = 0.0f;
    List<int>? bestFront = null;
    List<int>? bestBack = null;
    ReadOnlySpan<float> splitRatios = [0.5f, 1.0f / 3.0f, 2.0f / 3.0f];

    for (int axisOrderIndex = 0; axisOrderIndex < axisOrder.Length; ++axisOrderIndex)
    {
      int axis = axisOrder[axisOrderIndex];
      float min = GetAxisValue(boundsMin, axis);
      float max = GetAxisValue(boundsMax, axis);
      float size = max - min;
      if (!float.IsFinite(size) || size <= 0.0001f)
      {
        continue;
      }

      for (int ratioIndex = 0; ratioIndex < splitRatios.Length; ++ratioIndex)
      {
        float ratio = splitRatios[ratioIndex];
        float candidate = min + (size * ratio);
        if (!float.IsFinite(candidate))
        {
          continue;
        }

        if (candidate <= min + 0.001f || candidate >= max - 0.001f)
        {
          continue;
        }

        List<int> front = new(Math.Max(4, workingFaces.Count));
        List<int> back = new(Math.Max(4, workingFaces.Count));
        for (int i = 0; i < workingFaces.Count; ++i)
        {
          int faceId = workingFaces[i];
          FaceClass side = ClassifyFaceAgainstPlane(faces[faceId], axis, candidate);
          if (side == FaceClass.Front)
          {
            front.Add(faceId);
          }
          else
          {
            back.Add(faceId);
          }
        }

        if (front.Count == 0 || back.Count == 0)
        {
          continue;
        }

        float balance = Math.Min(front.Count, back.Count) / (float)Math.Max(front.Count, back.Count);
        float centerBias = 1.0f - MathF.Abs(0.5f - ratio) * 2.0f;
        float score = balance * 0.85f + centerBias * 0.15f;
        if (score > bestScore)
        {
          bestScore = score;
          bestAxis = axis;
          bestDistance = candidate;
          bestFront = front;
          bestBack = back;
        }
      }
    }

    if (bestAxis < 0 || bestFront == null || bestBack == null)
    {
      return false;
    }

    splitAxis = bestAxis;
    splitDistance = bestDistance;
    frontFaces = bestFront;
    backFaces = bestBack;
    return true;
  }

  private static bool TryForceSplitByMedian(
    FaceSpatialData[] faces,
    List<int> workingFaces,
    Vector3 boundsMin,
    Vector3 boundsMax,
    out int splitAxis,
    out float splitDistance,
    out List<int> frontFaces,
    out List<int> backFaces)
  {
    splitAxis = GetLongestAxis(boundsMin, boundsMax);
    splitDistance = 0.0f;
    frontFaces = new List<int>();
    backFaces = new List<int>();

    if (workingFaces.Count <= 1)
    {
      return false;
    }

    float[] values = new float[workingFaces.Count];
    for (int i = 0; i < workingFaces.Count; ++i)
    {
      values[i] = GetAxisValue(faces[workingFaces[i]].Centroid, splitAxis);
    }

    Array.Sort(values);
    float median = values[values.Length / 2];
    if (!float.IsFinite(median))
    {
      return false;
    }

    float min = GetAxisValue(boundsMin, splitAxis);
    float max = GetAxisValue(boundsMax, splitAxis);
    if (median <= min + 0.001f || median >= max - 0.001f)
    {
      median = 0.5f * (min + max);
    }

    if (!float.IsFinite(median))
    {
      return false;
    }

    splitDistance = median;
    frontFaces = new List<int>(workingFaces.Count);
    backFaces = new List<int>(workingFaces.Count);
    for (int i = 0; i < workingFaces.Count; ++i)
    {
      int faceId = workingFaces[i];
      FaceClass side = ClassifyFaceAgainstPlane(faces[faceId], splitAxis, splitDistance);
      if (side == FaceClass.Front)
      {
        frontFaces.Add(faceId);
      }
      else
      {
        backFaces.Add(faceId);
      }
    }

    return frontFaces.Count > 0 && backFaces.Count > 0;
  }

  private static int AddLeaf(
    List<int> faceIds,
    Vector3 boundsMin,
    Vector3 boundsMax,
    int[] faceGroupLookup,
    int materialGroupCount,
    List<uint> cFaceIds,
    List<ushort> matListInLeaf,
    List<SpatialLeafEntry> leaves,
    List<string> warnings)
  {
    if (leaves.Count >= MaxBspLeafCount)
    {
      throw new InvalidDataException($"Spatial BSP leaf count exceeded limit ({MaxBspLeafCount}).");
    }

    int faceCount = faceIds.Count;
    if (faceCount > MaxLeafFaceCount)
    {
      warnings.Add($"Leaf face count {faceCount} exceeds ushort range; truncating to {MaxLeafFaceCount}.");
      faceCount = MaxLeafFaceCount;
    }

    uint faceStartId = (uint)cFaceIds.Count;
    for (int i = 0; i < faceCount; ++i)
    {
      int faceId = faceIds[i];
      if (faceId < 0)
      {
        faceId = 0;
      }

      cFaceIds.Add((uint)faceId);
    }

    uint matGroupStartId = (uint)matListInLeaf.Count;
    int matGroupCount = 0;
    if (materialGroupCount > 0 && faceCount > 0)
    {
      bool[] seen = new bool[materialGroupCount];
      for (int i = 0; i < faceCount; ++i)
      {
        int faceId = faceIds[i];
        if ((uint)faceId >= (uint)faceGroupLookup.Length)
        {
          continue;
        }

        int groupIndex = faceGroupLookup[faceId];
        if (groupIndex < 0 || groupIndex >= materialGroupCount || seen[groupIndex])
        {
          continue;
        }

        if (matGroupCount >= MaxLeafMatGroupCount)
        {
          warnings.Add($"Leaf material-group count exceeded {MaxLeafMatGroupCount}; extra groups were dropped.");
          break;
        }

        seen[groupIndex] = true;
        matListInLeaf.Add((ushort)groupIndex);
        ++matGroupCount;
      }
    }

    int leafIndex = leaves.Count;
    leaves.Add(new SpatialLeafEntry(
      0,
      (ushort)faceCount,
      faceStartId,
      (ushort)matGroupCount,
      matGroupStartId,
      boundsMin,
      boundsMax));

    return leafIndex;
  }

  private static FaceClass ClassifyFaceAgainstPlane(FaceSpatialData face, int axis, float distance)
  {
    float min = GetAxisValue(face.Min, axis) - distance;
    float max = GetAxisValue(face.Max, axis) - distance;
    if (min >= 0.0f)
    {
      return FaceClass.Front;
    }

    if (max <= 0.0f)
    {
      return FaceClass.Back;
    }

    float centroid = GetAxisValue(face.Centroid, axis) - distance;
    return centroid >= 0.0f ? FaceClass.Front : FaceClass.Back;
  }

  private static byte[] SerializeCPlaneSection(Vector3[] normals)
  {
    byte[] bytes = new byte[normals.Length * 12];
    int offset = 0;
    for (int i = 0; i < normals.Length; ++i)
    {
      Vector3 n = normals[i];
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset, 4), n.X);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 4, 4), n.Y);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 8, 4), n.Z);
      offset += 12;
    }

    return bytes;
  }

  private static byte[] SerializeCFaceIdSection(List<uint> faceIds)
  {
    if (faceIds.Count == 0)
    {
      return Array.Empty<byte>();
    }

    byte[] bytes = new byte[faceIds.Count * 4];
    int offset = 0;
    for (int i = 0; i < faceIds.Count; ++i)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), faceIds[i]);
      offset += 4;
    }

    return bytes;
  }

  private static byte[] SerializeNodeSection(BspNode[] nodes)
  {
    byte[] bytes = new byte[nodes.Length * 24];
    int offset = 0;
    for (int i = 0; i < nodes.Length; ++i)
    {
      BspNode node = nodes[i];
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), node.NormalIndex);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 4, 4), node.Distance);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 8, 2), node.Front);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 10, 2), node.Back);

      short minX = ClampToInt16((int)node.Min.X);
      short minY = ClampToInt16((int)node.Min.Y);
      short minZ = ClampToInt16((int)node.Min.Z);
      short maxX = ClampToInt16((int)node.Max.X);
      short maxY = ClampToInt16((int)node.Max.Y);
      short maxZ = ClampToInt16((int)node.Max.Z);
      NormalizeBounds(ref minX, ref maxX);
      NormalizeBounds(ref minY, ref maxY);
      NormalizeBounds(ref minZ, ref maxZ);

      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 12, 2), minX);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 14, 2), minY);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 16, 2), minZ);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 18, 2), maxX);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 20, 2), maxY);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 22, 2), maxZ);
      offset += 24;
    }

    return bytes;
  }

  private static byte[] SerializeLeafSection(SpatialLeafEntry[] leaves)
  {
    byte[] bytes = new byte[leaves.Length * 25];
    int offset = 0;
    for (int i = 0; i < leaves.Length; ++i)
    {
      SpatialLeafEntry leaf = leaves[i];
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 1, 2), leaf.FaceCount);
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset + 3, 4), leaf.FaceStartId);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 7, 2), leaf.MaterialGroupCount);
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset + 9, 4), leaf.MaterialGroupStartId);

      short minX = ClampToInt16(MinFixFloatToInt(leaf.Min.X));
      short minY = ClampToInt16(MinFixFloatToInt(leaf.Min.Y));
      short minZ = ClampToInt16(MinFixFloatToInt(leaf.Min.Z));
      short maxX = ClampToInt16(MaxFixFloatToInt(leaf.Max.X));
      short maxY = ClampToInt16(MaxFixFloatToInt(leaf.Max.Y));
      short maxZ = ClampToInt16(MaxFixFloatToInt(leaf.Max.Z));
      NormalizeBounds(ref minX, ref maxX);
      NormalizeBounds(ref minY, ref maxY);
      NormalizeBounds(ref minZ, ref maxZ);

      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 13, 2), minX);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 15, 2), minY);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 17, 2), minZ);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 19, 2), maxX);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 21, 2), maxY);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 23, 2), maxZ);
      offset += 25;
    }

    return bytes;
  }

  private static byte[] SerializeMatListInLeafSection(List<ushort> materialGroupIds)
  {
    if (materialGroupIds.Count == 0)
    {
      return Array.Empty<byte>();
    }

    byte[] bytes = new byte[materialGroupIds.Count * 2];
    int offset = 0;
    for (int i = 0; i < materialGroupIds.Count; ++i)
    {
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset, 2), materialGroupIds[i]);
      offset += 2;
    }

    return bytes;
  }

  private static BspNode[] BuildBspNodeState(List<SpatialNodeTemp> nodes)
  {
    BspNode[] state = new BspNode[nodes.Count];
    for (int i = 0; i < nodes.Count; ++i)
    {
      SpatialNodeTemp source = nodes[i];
      short minX = ClampToInt16(MinFixFloatToInt(source.Min.X));
      short minY = ClampToInt16(MinFixFloatToInt(source.Min.Y));
      short minZ = ClampToInt16(MinFixFloatToInt(source.Min.Z));
      short maxX = ClampToInt16(MaxFixFloatToInt(source.Max.X));
      short maxY = ClampToInt16(MaxFixFloatToInt(source.Max.Y));
      short maxZ = ClampToInt16(MaxFixFloatToInt(source.Max.Z));
      NormalizeBounds(ref minX, ref maxX);
      NormalizeBounds(ref minY, ref maxY);
      NormalizeBounds(ref minZ, ref maxZ);
      state[i] = new BspNode(
        source.NormalIndex,
        source.Distance,
        source.Front,
        source.Back,
        new Vector3(minX, minY, minZ),
        new Vector3(maxX, maxY, maxZ));
    }

    return state;
  }

  private static BspLeafBounds[] BuildBspLeafState(List<SpatialLeafEntry> leaves)
  {
    BspLeafBounds[] state = new BspLeafBounds[leaves.Count];
    for (int i = 0; i < leaves.Count; ++i)
    {
      SpatialLeafEntry leaf = leaves[i];
      short minX = ClampToInt16(MinFixFloatToInt(leaf.Min.X));
      short minY = ClampToInt16(MinFixFloatToInt(leaf.Min.Y));
      short minZ = ClampToInt16(MinFixFloatToInt(leaf.Min.Z));
      short maxX = ClampToInt16(MaxFixFloatToInt(leaf.Max.X));
      short maxY = ClampToInt16(MaxFixFloatToInt(leaf.Max.Y));
      short maxZ = ClampToInt16(MaxFixFloatToInt(leaf.Max.Z));
      NormalizeBounds(ref minX, ref maxX);
      NormalizeBounds(ref minY, ref maxY);
      NormalizeBounds(ref minZ, ref maxZ);
      state[i] = new BspLeafBounds(
        new Vector3(minX, minY, minZ),
        new Vector3(maxX, maxY, maxZ));
    }

    return state;
  }

  private static short EncodeSpatialChild(SpatialChildRef child)
  {
    if (child.IsLeaf)
    {
      int encoded = -child.Index - 1;
      if (encoded < short.MinValue || encoded > short.MaxValue)
      {
        throw new InvalidDataException($"Spatial BSP leaf reference {child.Index} exceeds int16 encoding range.");
      }

      return (short)encoded;
    }

    if (child.Index < short.MinValue || child.Index > short.MaxValue)
    {
      throw new InvalidDataException($"Spatial BSP node reference {child.Index} exceeds int16 encoding range.");
    }

    return (short)child.Index;
  }

  private static void BuildChildBounds(
    Vector3 parentMin,
    Vector3 parentMax,
    int axis,
    float splitDistance,
    out Vector3 frontMin,
    out Vector3 frontMax,
    out Vector3 backMin,
    out Vector3 backMax)
  {
    frontMin = parentMin;
    frontMax = parentMax;
    backMin = parentMin;
    backMax = parentMax;

    float min = GetAxisValue(parentMin, axis);
    float max = GetAxisValue(parentMax, axis);
    float split = Math.Clamp(splitDistance, min, max);
    frontMin = SetAxisValue(frontMin, axis, split);
    backMax = SetAxisValue(backMax, axis, split);
  }

  private static int ResolveAxisNormalIndex(int axis)
  {
    return axis switch
    {
      0 => 1,
      1 => 2,
      _ => 3,
    };
  }

  private static int[] BuildAxisOrder(Vector3 boundsMin, Vector3 boundsMax)
  {
    float x = MathF.Abs(boundsMax.X - boundsMin.X);
    float y = MathF.Abs(boundsMax.Y - boundsMin.Y);
    float z = MathF.Abs(boundsMax.Z - boundsMin.Z);

    if (x >= y && x >= z)
    {
      return y >= z ? [0, 1, 2] : [0, 2, 1];
    }

    if (y >= x && y >= z)
    {
      return x >= z ? [1, 0, 2] : [1, 2, 0];
    }

    return x >= y ? [2, 0, 1] : [2, 1, 0];
  }

  private static int GetLongestAxis(Vector3 boundsMin, Vector3 boundsMax)
  {
    int[] order = BuildAxisOrder(boundsMin, boundsMax);
    return order[0];
  }

  private static float GetAxisValue(in Vector3 value, int axis)
  {
    return axis switch
    {
      0 => value.X,
      1 => value.Y,
      _ => value.Z,
    };
  }

  private static Vector3 SetAxisValue(in Vector3 value, int axis, float axisValue)
  {
    return axis switch
    {
      0 => new Vector3(axisValue, value.Y, value.Z),
      1 => new Vector3(value.X, axisValue, value.Z),
      _ => new Vector3(value.X, value.Y, axisValue),
    };
  }

  private static int MinFixFloatToInt(float value)
  {
    if (!float.IsFinite(value))
    {
      return 0;
    }

    float adjusted = value - 0.999f;
    if (adjusted > int.MaxValue)
    {
      return int.MaxValue;
    }

    if (adjusted < int.MinValue)
    {
      return int.MinValue;
    }

    return (int)adjusted;
  }

  private static int MaxFixFloatToInt(float value)
  {
    if (!float.IsFinite(value))
    {
      return 0;
    }

    float adjusted = value + 0.999f;
    if (adjusted > int.MaxValue)
    {
      return int.MaxValue;
    }

    if (adjusted < int.MinValue)
    {
      return int.MinValue;
    }

    return (int)adjusted;
  }

  private static short ClampToInt16(int value)
  {
    return (short)Math.Clamp(value, short.MinValue, short.MaxValue);
  }

  private static void NormalizeBounds(ref short min, ref short max)
  {
    if (min <= max)
    {
      return;
    }

    (min, max) = (max, min);
  }

  private static IndexedVertexData BuildIndexedVertexData(ObjImportVertex[] vertices, List<string> warnings)
  {
    // Keep 1:1 vertex indexing to preserve BSP per-vertex data (especially color/light behavior).
    Vector3[] uniquePositions = new Vector3[vertices.Length];
    int[] vertexIds = new int[vertices.Length];
    for (int i = 0; i < vertices.Length; ++i)
    {
      Vector3 position = vertices[i].LocalPosition;
      if (!IsFiniteVector3(position))
      {
        warnings.Add($"Imported vertex {i} has non-finite local position; replaced with 0,0,0.");
        position = Vector3.Zero;
      }

      uniquePositions[i] = position;
      vertexIds[i] = i;
    }

    return new IndexedVertexData(uniquePositions, vertexIds);
  }

  private static byte[] SerializeFloatVertexSection(Vector3[] vertices)
  {
    byte[] bytes = new byte[vertices.Length * 12];
    int offset = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      Vector3 p = vertices[i];
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset, 4), p.X);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 4, 4), p.Y);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 8, 4), p.Z);
      offset += 12;
    }

    return bytes;
  }

  private static byte[] SerializeVertexColorSection(ObjImportVertex[] vertices)
  {
    byte[] bytes = new byte[vertices.Length * 4];
    int offset = 0;
    for (int i = 0; i < vertices.Length; ++i)
    {
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), EncodeVertexColor(vertices[i].Color));
      offset += 4;
    }

    return bytes;
  }

  private static uint EncodeVertexColor(Vector3 color)
  {
    if (!IsFiniteVector3(color))
    {
      return 0xFFFFFFFFu;
    }

    byte r = (byte)Math.Clamp((int)MathF.Round(color.X * 255.0f), 0, 255);
    byte g = (byte)Math.Clamp((int)MathF.Round(color.Y * 255.0f), 0, 255);
    byte b = (byte)Math.Clamp((int)MathF.Round(color.Z * 255.0f), 0, 255);
    return 0xFF000000u | ((uint)r << 16) | ((uint)g << 8) | b;
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
      Vector2 uv = vertices[i].LightUv;
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

  private static byte[] SerializeVertexIdSection(int[] vertexIds)
  {
    byte[] bytes = new byte[vertexIds.Length * 4];
    int offset = 0;
    for (int vertexIndex = 0; vertexIndex < vertexIds.Length; ++vertexIndex)
    {
      int vertexId = vertexIds[vertexIndex];
      if (vertexId < 0)
      {
        vertexId = 0;
      }

      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset, 4), (uint)vertexId);
      offset += 4;
    }

    return bytes;
  }

  private static byte[] SerializeMatGroupSection(
    List<MaterialRun> runs,
    ObjImportVertex[] vertices,
    SourceMatGroupMetadata sourceMatGroupMetadata,
    List<string> warnings)
  {
    if (runs.Count == 0)
    {
      return Array.Empty<byte>();
    }

    byte[] bytes = new byte[runs.Count * 42];
    int preferredAttributeFallbackCount = 0;
    int defaultAttributeFallbackCount = 0;
    int offset = 0;
    for (int i = 0; i < runs.Count; ++i)
    {
      MaterialRun run = runs[i];
      short materialId = ClampMaterialIdToShort(run.MaterialId, warnings);
      short lightMapId = ClampLightMapIdToShort(run.LightMapId, warnings);
      ushort attribute = ResolveMatGroupAttribute(run, sourceMatGroupMetadata, out MatGroupAttributeResolveMode attributeMode);
      if (attributeMode == MatGroupAttributeResolveMode.PreferredFallback)
      {
        ++preferredAttributeFallbackCount;
      }
      else if (attributeMode == MatGroupAttributeResolveMode.DefaultFallback)
      {
        ++defaultAttributeFallbackCount;
      }
      int faceStart = Math.Max(0, run.FaceStart);
      int faceEndExclusive = Math.Max(faceStart, run.FaceStart + run.FaceCount);
      if (faceEndExclusive > vertices.Length / 3)
      {
        faceEndExclusive = vertices.Length / 3;
      }

      Vector3 bbMin = Vector3.Zero;
      Vector3 bbMax = Vector3.Zero;
      bool foundBounds = false;
      for (int faceIndex = faceStart; faceIndex < faceEndExclusive; ++faceIndex)
      {
        int vertexBase = faceIndex * 3;
        for (int vertexOffset = 0; vertexOffset < 3; ++vertexOffset)
        {
          Vector3 position = vertices[vertexBase + vertexOffset].LocalPosition;
          if (!IsFiniteVector3(position))
          {
            continue;
          }

          if (!foundBounds)
          {
            bbMin = position;
            bbMax = position;
            foundBounds = true;
          }
          else
          {
            bbMin = Vector3.ComponentMin(bbMin, position);
            bbMax = Vector3.ComponentMax(bbMax, position);
          }
        }
      }

      if (!foundBounds)
      {
        bbMin = Vector3.Zero;
        bbMax = Vector3.Zero;
      }

      short bbMinX = ClampToInt16(MinFixFloatToInt(bbMin.X));
      short bbMinY = ClampToInt16(MinFixFloatToInt(bbMin.Y));
      short bbMinZ = ClampToInt16(MinFixFloatToInt(bbMin.Z));
      short bbMaxX = ClampToInt16(MaxFixFloatToInt(bbMax.X));
      short bbMaxY = ClampToInt16(MaxFixFloatToInt(bbMax.Y));
      short bbMaxZ = ClampToInt16(MaxFixFloatToInt(bbMax.Z));
      NormalizeBounds(ref bbMinX, ref bbMaxX);
      NormalizeBounds(ref bbMinY, ref bbMaxY);
      NormalizeBounds(ref bbMinZ, ref bbMaxZ);

      // ReadMatGroup layout:
      // 0: attr, 2: faceCount, 4: faceStartId, 8: materialId, 10: lightMapId, ..., 40: objectId
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset, 2), attribute);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 2, 2), (ushort)run.FaceCount);
      BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset + 4, 4), (uint)run.FaceStart);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 8, 2), materialId);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 10, 2), lightMapId);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 12, 2), bbMinX);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 14, 2), bbMinY);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 16, 2), bbMinZ);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 18, 2), bbMaxX);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 20, 2), bbMaxY);
      BinaryPrimitives.WriteInt16LittleEndian(bytes.AsSpan(offset + 22, 2), bbMaxZ);

      Vector3 center = 0.5f * (bbMin + bbMax);
      Vector3 halfExtents = 0.5f * (bbMax - bbMin);
      float cullRadius = halfExtents.Length;
      if (!float.IsFinite(cullRadius) || cullRadius < 0.0001f)
      {
        cullRadius = 1.0f;
      }

      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 24, 4), center.X);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 28, 4), center.Y);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 32, 4), center.Z);
      BinaryPrimitives.WriteSingleLittleEndian(bytes.AsSpan(offset + 36, 4), cullRadius);
      BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(offset + 40, 2), run.ObjectId);
      offset += 42;
    }

    if (preferredAttributeFallbackCount > 0)
    {
      warnings.Add(
        $"ReadMatGroup attribute exact mapping was unavailable for {preferredAttributeFallbackCount:N0} groups; " +
        "used source-preferred fallback attributes.");
    }

    if (defaultAttributeFallbackCount > 0)
    {
      warnings.Add(
        $"ReadMatGroup attribute metadata was unavailable for {defaultAttributeFallbackCount:N0} groups; " +
        "used hardcoded default attribute 0x2000.");
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

  private static short ClampLightMapIdToShort(int lightMapId, List<string> warnings)
  {
    if (lightMapId < -1)
    {
      warnings.Add($"LightMap id {lightMapId} is below supported range; clamped to -1.");
      return -1;
    }

    if (lightMapId > short.MaxValue)
    {
      warnings.Add($"LightMap id {lightMapId} exceeds int16 range; clamped to {short.MaxValue}.");
      return short.MaxValue;
    }

    return (short)lightMapId;
  }

  private static ushort ResolveMatGroupAttribute(
    MaterialRun run,
    SourceMatGroupMetadata sourceMetadata,
    out MatGroupAttributeResolveMode mode)
  {
    if (sourceMetadata.TryResolveAndConsume(run, out ushort attribute))
    {
      mode = MatGroupAttributeResolveMode.Exact;
      return attribute;
    }

    if (sourceMetadata.TryGetPreferredAttribute(run, out attribute))
    {
      mode = MatGroupAttributeResolveMode.PreferredFallback;
      return attribute;
    }

    // RF map BSP groups generally keep object-flag bit set even when object id is 0.
    mode = MatGroupAttributeResolveMode.DefaultFallback;
    return 0x2000;
  }

  private static SourceMatGroupMetadata BuildSourceMatGroupMetadata(byte[] section, List<string> warnings)
  {
    SourceMatGroupMetadata metadata = new();
    if (section == null || section.Length == 0)
    {
      return metadata;
    }

    if ((section.Length % 42) != 0)
    {
      warnings.Add($"Source ReadMatGroup section size ({section.Length}) is not aligned to 42 bytes; metadata preservation disabled.");
      return metadata;
    }

    int count = section.Length / 42;
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 42;
      ushort attribute = BinaryPrimitives.ReadUInt16LittleEndian(section.AsSpan(offset, 2));
      ushort faceCount = BinaryPrimitives.ReadUInt16LittleEndian(section.AsSpan(offset + 2, 2));
      short materialId = BinaryPrimitives.ReadInt16LittleEndian(section.AsSpan(offset + 8, 2));
      short lightMapId = BinaryPrimitives.ReadInt16LittleEndian(section.AsSpan(offset + 10, 2));
      ushort objectId = BinaryPrimitives.ReadUInt16LittleEndian(section.AsSpan(offset + 40, 2));
      if (faceCount <= 0)
      {
        continue;
      }

      metadata.Add(materialId, lightMapId, objectId, attribute, faceCount);
    }

    return metadata;
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

    Vector3 position = positions[faceRef.PositionIndex];
    Vector2 uv = Vector2.Zero;
    bool hasExplicitUv = false;
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

      hasExplicitUv = true;
    }

    vertex = new ObjImportVertex(position, position, uv, uv, new Vector3(1.0f, 1.0f, 1.0f), materialId, -1, 0, hasExplicitUv);
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

  private static Vector3 ConvertObjPositionToSource(in Vector3 obj, ObjImportAxisMode axisMode)
  {
    return axisMode switch
    {
      ObjImportAxisMode.RfPackageDefault => new Vector3(obj.X, obj.Z, obj.Y),
      ObjImportAxisMode.BlenderExportDefault => new Vector3(obj.X, obj.Y, -obj.Z),
      _ => new Vector3(obj.X, obj.Z, obj.Y),
    };
  }

  private static ObjImportAxisMode ResolveObjImportAxisMode(ObjImportVertex[] vertices, MapBounds referenceBounds)
  {
    if (vertices.Length == 0)
    {
      return ObjImportAxisMode.RfPackageDefault;
    }

    float defaultScore = ComputeAxisFitScore(vertices, referenceBounds, ObjImportAxisMode.RfPackageDefault);
    float blenderExportScore = ComputeAxisFitScore(vertices, referenceBounds, ObjImportAxisMode.BlenderExportDefault);
    if (!float.IsFinite(defaultScore))
    {
      return ObjImportAxisMode.RfPackageDefault;
    }

    if (float.IsFinite(blenderExportScore) && blenderExportScore + 0.0001f < defaultScore)
    {
      return ObjImportAxisMode.BlenderExportDefault;
    }

    return ObjImportAxisMode.RfPackageDefault;
  }

  private static float ComputeAxisFitScore(ObjImportVertex[] vertices, MapBounds referenceBounds, ObjImportAxisMode axisMode)
  {
    if (!TryComputeBounds(vertices, axisMode, out MapBounds convertedBounds))
    {
      return float.PositiveInfinity;
    }

    Vector3 refSize = referenceBounds.Size;
    Vector3 impSize = convertedBounds.Size;
    Vector3 refCenter = referenceBounds.Center;
    Vector3 impCenter = convertedBounds.Center;
    if (!IsFiniteVector3(refSize) || !IsFiniteVector3(impSize) || !IsFiniteVector3(refCenter) || !IsFiniteVector3(impCenter))
    {
      return float.PositiveInfinity;
    }

    float refDiagonal = MathF.Max(1.0f, refSize.Length);
    float sizeDelta = (impSize - refSize).Length / refDiagonal;
    float centerDelta = (impCenter - refCenter).Length / refDiagonal;
    return sizeDelta * 0.8f + centerDelta * 0.2f;
  }

  private static bool TryComputeBounds(ObjImportVertex[] vertices, ObjImportAxisMode axisMode, out MapBounds bounds)
  {
    bounds = default;
    bool found = false;
    Vector3 min = Vector3.Zero;
    Vector3 max = Vector3.Zero;
    for (int index = 0; index < vertices.Length; ++index)
    {
      Vector3 p = ConvertObjPositionToSource(vertices[index].Position, axisMode);
      if (!IsFiniteVector3(p))
      {
        continue;
      }

      if (!found)
      {
        min = p;
        max = p;
        found = true;
      }
      else
      {
        min = Vector3.ComponentMin(min, p);
        max = Vector3.ComponentMax(max, p);
      }
    }

    if (!found)
    {
      return false;
    }

    bounds = new MapBounds(min, max);
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
    string[] objFiles = Directory.GetFiles(packageDirectory, "*.obj", SearchOption.TopDirectoryOnly);
    if (objFiles.Length == 0)
    {
      return null;
    }

    if (objFiles.Length == 1)
    {
      string single = objFiles[0];
      if (!string.Equals(Path.GetFileName(single), MeshObjFileName, StringComparison.OrdinalIgnoreCase))
      {
        warnings.Add($"map_mesh.obj not found; using '{Path.GetFileName(single)}' instead.");
      }

      return single;
    }

    string selected = objFiles[0];
    DateTime selectedWriteTimeUtc = DateTime.MinValue;
    long selectedSize = -1;
    for (int i = 0; i < objFiles.Length; ++i)
    {
      string filePath = objFiles[i];
      DateTime writeTimeUtc = DateTime.MinValue;
      long size = 0;
      try
      {
        FileInfo info = new(filePath);
        writeTimeUtc = info.LastWriteTimeUtc;
        size = new FileInfo(filePath).Length;
      }
      catch
      {
        // Keep best effort selection when file metadata cannot be read.
      }

      if (writeTimeUtc > selectedWriteTimeUtc ||
          (writeTimeUtc == selectedWriteTimeUtc && size > selectedSize))
      {
        selected = filePath;
        selectedWriteTimeUtc = writeTimeUtc;
        selectedSize = size;
      }
    }

    bool hasCanonical = false;
    for (int i = 0; i < objFiles.Length; ++i)
    {
      if (string.Equals(Path.GetFileName(objFiles[i]), MeshObjFileName, StringComparison.OrdinalIgnoreCase))
      {
        hasCanonical = true;
        break;
      }
    }

    if (hasCanonical && !string.Equals(Path.GetFileName(selected), MeshObjFileName, StringComparison.OrdinalIgnoreCase))
    {
      warnings.Add($"Multiple OBJ files found; selected newest '{Path.GetFileName(selected)}' instead of '{MeshObjFileName}'.");
    }
    else if (!hasCanonical)
    {
      warnings.Add($"map_mesh.obj not found; using '{Path.GetFileName(selected)}' instead.");
    }

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
  private readonly record struct ObjImportVertex(
    Vector3 Position,
    Vector3 LocalPosition,
    Vector2 Uv,
    Vector2 LightUv,
    Vector3 Color,
    int MaterialId,
    int LightMapId,
    ushort ObjectId = 0,
    bool HasExplicitUv = true);
  private readonly record struct QuantizedVertexKey(int X, int Y, int Z);
  private readonly record struct ObjFaceRef(int PositionIndex, int TexcoordIndex);
  private readonly record struct IndexedVertexData(Vector3[] UniquePositions, int[] VertexIds);
  private readonly record struct VertexPositionKey(int XBits, int YBits, int ZBits);
  private enum MatGroupAttributeResolveMode
  {
    Exact = 0,
    PreferredFallback = 1,
    DefaultFallback = 2,
  }
  private enum ObjImportAxisMode
  {
    RfPackageDefault = 0,
    BlenderExportDefault = 1,
  }
  private readonly record struct MaterialRun(int MaterialId, int LightMapId, ushort ObjectId, int FaceStart, int FaceCount);
  private sealed class SourceMatGroupChunk
  {
    public SourceMatGroupChunk(ushort attribute, int remainingFaces)
    {
      Attribute = attribute;
      RemainingFaces = remainingFaces;
    }

    public ushort Attribute { get; }
    public int RemainingFaces { get; set; }
  }
  private sealed class SourceMatGroupMetadata
  {
    private readonly Dictionary<MatGroupKey, Queue<SourceMatGroupChunk>> _byExact = new();
    private readonly Dictionary<MatGroupLooseKey, Queue<SourceMatGroupChunk>> _byLoose = new();
    private readonly Dictionary<MatGroupLooseKey, ushort> _preferredByLoose = new();
    private ushort _preferredGlobal = 0x2000;
    private bool _hasPreferredGlobal;

    public void Add(int materialId, int lightMapId, ushort objectId, ushort attribute, int faceCount)
    {
      if (faceCount <= 0)
      {
        return;
      }

      SourceMatGroupChunk chunk = new(attribute, faceCount);
      MatGroupKey exact = new(materialId, lightMapId, objectId);
      MatGroupLooseKey loose = new(materialId, objectId);

      if (!_byExact.TryGetValue(exact, out Queue<SourceMatGroupChunk>? exactQueue))
      {
        exactQueue = new Queue<SourceMatGroupChunk>();
        _byExact[exact] = exactQueue;
      }

      if (!_byLoose.TryGetValue(loose, out Queue<SourceMatGroupChunk>? looseQueue))
      {
        looseQueue = new Queue<SourceMatGroupChunk>();
        _byLoose[loose] = looseQueue;
      }

      if (!_preferredByLoose.TryGetValue(loose, out ushort preferred) || (preferred != 0x2000 && attribute == 0x2000))
      {
        _preferredByLoose[loose] = attribute;
      }

      if (!_hasPreferredGlobal || (_preferredGlobal != 0x2000 && attribute == 0x2000))
      {
        _preferredGlobal = attribute;
        _hasPreferredGlobal = true;
      }

      exactQueue.Enqueue(chunk);
      looseQueue.Enqueue(chunk);
    }

    public bool TryResolveAndConsume(MaterialRun run, out ushort attribute)
    {
      MatGroupKey exact = new(run.MaterialId, run.LightMapId, run.ObjectId);
      if (TryResolveAndConsume(_byExact, exact, run.FaceCount, out attribute))
      {
        return true;
      }

      MatGroupLooseKey loose = new(run.MaterialId, run.ObjectId);
      return TryResolveAndConsume(_byLoose, loose, run.FaceCount, out attribute);
    }

    public bool TryGetPreferredAttribute(MaterialRun run, out ushort attribute)
    {
      MatGroupLooseKey loose = new(run.MaterialId, run.ObjectId);
      if (_preferredByLoose.TryGetValue(loose, out attribute))
      {
        return true;
      }

      if (_hasPreferredGlobal)
      {
        attribute = _preferredGlobal;
        return true;
      }

      attribute = 0x2000;
      return false;
    }

    private static bool TryResolveAndConsume<TKey>(
      Dictionary<TKey, Queue<SourceMatGroupChunk>> source,
      TKey key,
      int faceCount,
      out ushort attribute)
      where TKey : notnull
    {
      attribute = 0;
      if (!source.TryGetValue(key, out Queue<SourceMatGroupChunk>? queue) || queue.Count == 0)
      {
        return false;
      }

      while (queue.Count > 0 && queue.Peek().RemainingFaces <= 0)
      {
        queue.Dequeue();
      }

      if (queue.Count == 0)
      {
        return false;
      }

      attribute = queue.Peek().Attribute;
      int remaining = Math.Max(0, faceCount);
      while (remaining > 0 && queue.Count > 0)
      {
        SourceMatGroupChunk chunk = queue.Peek();
        if (chunk.RemainingFaces <= 0)
        {
          queue.Dequeue();
          continue;
        }

        int consume = Math.Min(remaining, chunk.RemainingFaces);
        chunk.RemainingFaces -= consume;
        remaining -= consume;
        if (chunk.RemainingFaces <= 0)
        {
          queue.Dequeue();
        }
      }

      return true;
    }
  }
  private readonly record struct MatGroupKey(int MaterialId, int LightMapId, ushort ObjectId);
  private readonly record struct MatGroupLooseKey(int MaterialId, ushort ObjectId);
  private readonly record struct FaceSpatialData(Vector3 Min, Vector3 Max, Vector3 Centroid);
  private readonly record struct SourceLeafEntry(byte Type, Vector3 Min, Vector3 Max);
  private readonly record struct SpatialNodeTemp(
    uint NormalIndex,
    float Distance,
    short Front,
    short Back,
    Vector3 Min,
    Vector3 Max);
  private readonly record struct SpatialLeafEntry(
    byte Type,
    ushort FaceCount,
    uint FaceStartId,
    ushort MaterialGroupCount,
    uint MaterialGroupStartId,
    Vector3 Min,
    Vector3 Max);
  private readonly record struct SpatialChildRef(bool IsLeaf, int Index);
  private readonly record struct SpatialBspSections(
    byte[] CPlaneSection,
    byte[] CFaceIdSection,
    byte[] NodeSection,
    byte[] LeafSection,
    byte[] MatListInLeafSection,
    Vector3[] Normals,
    BspNode[] Nodes,
    BspLeafBounds[] LeafBounds);
  private enum FaceClass
  {
    Front = 0,
    Back = 1,
  }
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
  private readonly record struct PrimitiveEdgeSelectionKey(BspVertexSelection A, BspVertexSelection B);
}
