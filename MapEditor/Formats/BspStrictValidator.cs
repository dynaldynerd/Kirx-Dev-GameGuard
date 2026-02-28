using System.Buffers.Binary;
using System.Text;
using OpenTK.Mathematics;

namespace MapEditor.Formats;

public static class BspStrictValidator
{
  private const int BspHeaderSize = 0x2AC;
  private const int BspHeaderEntryCount = 85;
  private const int BspSectionCPlanes = 0;
  private const int BspSectionCFaceId = 1;
  private const int BspSectionNode = 2;
  private const int BspSectionLeaf = 3;
  private const int BspSectionMatListInLeaf = 4;
  private const int BspSectionObject = 5;
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
  private const uint ExpectedBspVersion = 39;
  private const int ReadAniObjectEntrySize = 88;
  private const int MaxCollectedErrors = 128;
  private const ushort MatGroupAttrByteVertex = 0x8000;
  private const ushort MatGroupAttrWordVertex = 0x4000;
  private const ushort MatGroupAttrObject = 0x2000;

  public static void ValidateForStrictLoad(LoadedMap map)
  {
    ValidateCore(map, "Strict load");
  }

  public static void ValidateForSave(LoadedMap map)
  {
    ValidateCore(map, "Save");
  }

  private static void ValidateCore(LoadedMap map, string context)
  {
    ArgumentNullException.ThrowIfNull(map);

    bool saveValidation = string.Equals(context, "Save", StringComparison.OrdinalIgnoreCase);
    List<string> errors = new();
    ValidateLoadedArrays(map, errors);
    ValidateCollisionData(map, errors);
    ValidateBspLayout(map, errors, saveValidation);

    if (errors.Count == 0)
    {
      return;
    }

    StringBuilder builder = new();
    builder.Append(context).Append(" strict validation failed:").AppendLine();
    for (int i = 0; i < errors.Count; ++i)
    {
      builder.Append("- ").Append(errors[i]).AppendLine();
    }

    throw new InvalidDataException(builder.ToString().TrimEnd());
  }

  private static void ValidateLoadedArrays(LoadedMap map, List<string> errors)
  {
    if (map.BspRenderVertices.Length != map.BspTriangleVertices.Length)
    {
      AddError(errors, $"Render vertex count ({map.BspRenderVertices.Length}) does not match triangle vertex count ({map.BspTriangleVertices.Length}).");
    }

    if (map.BspRenderVertexObjectIds.Length != map.BspRenderVertices.Length)
    {
      AddError(errors, $"Render-object id count ({map.BspRenderVertexObjectIds.Length}) does not match render vertex count ({map.BspRenderVertices.Length}).");
    }

    if (map.BspRenderVertexLocalPositions.Length != map.BspRenderVertices.Length)
    {
      AddError(errors, $"Render-local position count ({map.BspRenderVertexLocalPositions.Length}) does not match render vertex count ({map.BspRenderVertices.Length}).");
    }

    for (int vertexIndex = 0; vertexIndex < map.BspRenderVertices.Length; ++vertexIndex)
    {
      BspRenderVertex vertex = map.BspRenderVertices[vertexIndex];
      if (!IsFinite(vertex.Position) || !IsFinite(vertex.Uv) || !IsFinite(vertex.LightUv) || !IsFinite(vertex.Color))
      {
        AddError(errors, $"Render vertex {vertexIndex} contains non-finite values.");
      }
    }

    if (map.BspMaterialSpans.Length == 0 && map.BspRenderVertices.Length > 0)
    {
      AddError(errors, "Render vertices exist but BSP material span list is empty.");
      return;
    }

    int expectedStart = 0;
    for (int spanIndex = 0; spanIndex < map.BspMaterialSpans.Length; ++spanIndex)
    {
      BspMaterialSpan span = map.BspMaterialSpans[spanIndex];
      if (span.StartVertex != expectedStart)
      {
        AddError(errors, $"Material span {spanIndex} start vertex ({span.StartVertex}) is not contiguous (expected {expectedStart}).");
      }

      if (span.VertexCount <= 0 || (span.VertexCount % 3) != 0)
      {
        AddError(errors, $"Material span {spanIndex} has invalid vertex count ({span.VertexCount}).");
      }

      if (span.MaterialId < -1)
      {
        AddError(errors, $"Material span {spanIndex} has invalid material id ({span.MaterialId}).");
      }

      if (span.LightMapId < -1)
      {
        AddError(errors, $"Material span {spanIndex} has invalid lightmap id ({span.LightMapId}).");
      }

      expectedStart = checked(expectedStart + Math.Max(0, span.VertexCount));
      if (expectedStart > map.BspRenderVertices.Length)
      {
        AddError(errors, $"Material spans exceed render vertex count at span {spanIndex}.");
        break;
      }
    }

    if (map.BspRenderVertices.Length > 0 && expectedStart != map.BspRenderVertices.Length)
    {
      AddError(errors, $"Material spans cover {expectedStart} vertices, but render mesh has {map.BspRenderVertices.Length}.");
    }
  }

  private static void ValidateCollisionData(LoadedMap map, List<string> errors)
  {
    if (map.CollisionVertices.Length > ushort.MaxValue)
    {
      AddError(errors, $"Collision vertex count exceeds {ushort.MaxValue}.");
    }

    if (map.CollisionLines.Length > ushort.MaxValue)
    {
      AddError(errors, $"Collision line count exceeds {ushort.MaxValue}.");
    }

    for (int vertexIndex = 0; vertexIndex < map.CollisionVertices.Length; ++vertexIndex)
    {
      if (!IsFinite(map.CollisionVertices[vertexIndex]))
      {
        AddError(errors, $"Collision vertex {vertexIndex} contains non-finite coordinates.");
      }
    }

    if (map.CollisionLines.Length > 0)
    {
      CollisionLine sentinel = map.CollisionLines[0];
      bool sentinelValid =
        sentinel.Attribute == 0 &&
        sentinel.StartVertex == 0 &&
        sentinel.EndVertex == 0 &&
        sentinel.Height == 0.0f &&
        sentinel.FrontLeaf == 0 &&
        sentinel.BackLeaf == 0;
      if (!sentinelValid)
      {
        AddError(errors, "Collision line 0 is not a valid sentinel line.");
      }
    }

    for (int lineIndex = 0; lineIndex < map.CollisionLines.Length; ++lineIndex)
    {
      CollisionLine line = map.CollisionLines[lineIndex];
      if (line.StartVertex >= map.CollisionVertices.Length || line.EndVertex >= map.CollisionVertices.Length)
      {
        AddError(errors, $"Collision line {lineIndex} references out-of-range vertices ({line.StartVertex}->{line.EndVertex}).");
        continue;
      }

      bool isSentinel = lineIndex == 0;
      if (!isSentinel)
      {
        if (!float.IsFinite(line.Height) || line.Height <= 0.0f)
        {
          AddError(errors, $"Collision line {lineIndex} has invalid height ({line.Height}).");
        }

        Vector3 start = map.CollisionVertices[line.StartVertex];
        Vector3 end = map.CollisionVertices[line.EndVertex];
        float lengthSq = (end - start).LengthSquared;
        if (!float.IsFinite(lengthSq) || lengthSq < 0.0001f)
        {
          AddError(errors, $"Collision line {lineIndex} is degenerate.");
        }
      }

    }

    if (map.CollisionLines.Length > 1)
    {
      if (map.BspCollisionNormals.Length == 0)
      {
        AddError(errors, "Collision lines exist but BSP collision normal table is empty.");
      }

      if (map.BspCollisionNodes.Length <= 1)
      {
        AddError(errors, "Collision lines exist but BSP collision node table does not contain a runtime root node.");
      }

      if (map.BspLeafBounds.Length <= 1)
      {
        AddError(errors, "Collision lines exist but BSP leaf table does not contain runtime leaves.");
      }
    }

    for (int nodeIndex = 0; nodeIndex < map.BspCollisionNodes.Length; ++nodeIndex)
    {
      BspNode node = map.BspCollisionNodes[nodeIndex];
      if (node.NormalIndex >= map.BspCollisionNormals.Length)
      {
        AddError(errors, $"Collision node {nodeIndex} references invalid normal index {node.NormalIndex}.");
      }

      ValidateNodeChildRef(nodeIndex, "front", node.Front, map.BspCollisionNodes.Length, map.BspLeafBounds.Length, errors);
      ValidateNodeChildRef(nodeIndex, "back", node.Back, map.BspCollisionNodes.Length, map.BspLeafBounds.Length, errors);
    }
  }

  private static void ValidateBspLayout(LoadedMap map, List<string> errors, bool saveValidation)
  {
    BspBinaryLayout? layout = map.BspBinaryLayout;
    if (layout == null)
    {
      AddError(errors, "Loaded map does not contain BSP binary layout.");
      return;
    }

    if (layout.Version != ExpectedBspVersion)
    {
      AddError(errors, $"Unexpected BSP version {layout.Version}; expected {ExpectedBspVersion}.");
    }

    if (layout.Entries == null || layout.Entries.Length != BspHeaderEntryCount)
    {
      AddError(errors, $"BSP entry table must contain {BspHeaderEntryCount} entries.");
      return;
    }

    if (layout.Sections == null || layout.Sections.Length != BspHeaderEntryCount)
    {
      AddError(errors, $"BSP section table must contain {BspHeaderEntryCount} sections.");
      return;
    }

    for (int sectionIndex = 0; sectionIndex < layout.Sections.Length; ++sectionIndex)
    {
      if (layout.Sections[sectionIndex] == null)
      {
        AddError(errors, $"BSP section {sectionIndex} is null.");
      }
    }

    byte[] cPlanes = layout.Sections[BspSectionCPlanes] ?? Array.Empty<byte>();
    byte[] cFaceIds = layout.Sections[BspSectionCFaceId] ?? Array.Empty<byte>();
    byte[] nodes = layout.Sections[BspSectionNode] ?? Array.Empty<byte>();
    byte[] leaves = layout.Sections[BspSectionLeaf] ?? Array.Empty<byte>();
    byte[] matListInLeaf = layout.Sections[BspSectionMatListInLeaf] ?? Array.Empty<byte>();
    byte[] bVertex = layout.Sections[BspSectionBVertex] ?? Array.Empty<byte>();
    byte[] wVertex = layout.Sections[BspSectionWVertex] ?? Array.Empty<byte>();
    byte[] fVertex = layout.Sections[BspSectionFVertex] ?? Array.Empty<byte>();
    byte[] vertexColor = layout.Sections[BspSectionVertexColor] ?? Array.Empty<byte>();
    byte[] uv = layout.Sections[BspSectionUv] ?? Array.Empty<byte>();
    byte[] lgtUv = layout.Sections[BspSectionLgtUv] ?? Array.Empty<byte>();
    byte[] face = layout.Sections[BspSectionFace] ?? Array.Empty<byte>();
    byte[] faceId = layout.Sections[BspSectionFaceId] ?? Array.Empty<byte>();
    byte[] vertexId = layout.Sections[BspSectionVertexId] ?? Array.Empty<byte>();
    byte[] readMatGroup = layout.Sections[BspSectionReadMatGroup] ?? Array.Empty<byte>();

    CheckSectionStride("CPlanes", cPlanes.Length, 12, errors);
    CheckSectionStride("CFaceId", cFaceIds.Length, 4, errors);
    CheckSectionStride("Node", nodes.Length, 24, errors);
    CheckSectionStride("Leaf", leaves.Length, 25, errors);
    CheckSectionStride("MatListInLeaf", matListInLeaf.Length, 2, errors);
    CheckSectionStride("BVertex", bVertex.Length, 3, errors);
    CheckSectionStride("WVertex", wVertex.Length, 6, errors);
    CheckSectionStride("FVertex", fVertex.Length, 12, errors);
    CheckSectionStride("VertexColor", vertexColor.Length, 4, errors);
    CheckSectionStride("UV", uv.Length, 8, errors);
    CheckSectionStride("LgtUV", lgtUv.Length, 4, errors);
    CheckSectionStride("Face", face.Length, 6, errors);
    CheckSectionStride("FaceId", faceId.Length, 4, errors);
    CheckSectionStride("VertexId", vertexId.Length, 4, errors);
    CheckSectionStride("ReadMatGroup", readMatGroup.Length, 42, errors);

    int cPlaneCount = cPlanes.Length / 12;
    int cFaceIdCount = cFaceIds.Length / 4;
    int nodeCount = nodes.Length / 24;
    int leafCount = leaves.Length / 25;
    int matListInLeafCount = matListInLeaf.Length / 2;
    int bVertexCount = bVertex.Length / 3;
    int wVertexCount = wVertex.Length / 6;
    int fVertexCount = fVertex.Length / 12;
    int packedVertexCount = checked(bVertexCount + wVertexCount + fVertexCount);
    int vertexColorCount = vertexColor.Length / 4;
    int uvCount = uv.Length / 8;
    int lgtUvCount = lgtUv.Length / 4;
    int faceCount = face.Length / 6;
    int faceIdCount = faceId.Length / 4;
    int vertexIdCount = vertexId.Length / 4;
    int matGroupCount = readMatGroup.Length / 42;

    if (map.BspCollisionNormals.Length != cPlaneCount)
    {
      AddError(errors, $"Loaded collision normal count ({map.BspCollisionNormals.Length}) does not match CPlanes section count ({cPlaneCount}).");
    }

    if (map.BspCollisionNodes.Length != nodeCount)
    {
      AddError(errors, $"Loaded collision node count ({map.BspCollisionNodes.Length}) does not match Node section count ({nodeCount}).");
    }

    if (map.BspLeafBounds.Length != leafCount)
    {
      AddError(errors, $"Loaded leaf-bounds count ({map.BspLeafBounds.Length}) does not match Leaf section count ({leafCount}).");
    }

    if (faceCount > 0 && faceIdCount == 0)
    {
      AddError(errors, "Face section is non-empty but FaceId section is empty.");
    }

    if (faceCount > 0 && vertexIdCount == 0)
    {
      AddError(errors, "Face section is non-empty but VertexId section is empty.");
    }

    if (faceCount > 0 && matGroupCount == 0)
    {
      AddError(errors, "Face section is non-empty but ReadMatGroup section is empty.");
    }

    if (vertexIdCount > uvCount)
    {
      AddError(errors, $"UV count ({uvCount}) is smaller than VertexId count ({vertexIdCount}).");
    }

    if (vertexIdCount > lgtUvCount)
    {
      AddError(errors, $"LgtUV count ({lgtUvCount}) is smaller than VertexId count ({vertexIdCount}).");
    }

    FaceEntry[] faces = ParseFaces(face, errors);
    uint[] faceIds = ParseUInt32Array(faceId);
    uint[] vertexIds = ParseUInt32Array(vertexId);
    uint[] cFaceIdValues = ParseUInt32Array(cFaceIds);
    ushort[] matListInLeafValues = ParseUInt16Array(matListInLeaf);
    MatGroupEntry[] matGroups = ParseMatGroups(readMatGroup);
    NodeEntry[] nodeEntries = ParseNodes(nodes);
    LeafEntry[] leafEntries = ParseLeaves(leaves);
    byte[] objectSection = layout.Sections[BspSectionObject] ?? Array.Empty<byte>();
    if ((objectSection.Length % ReadAniObjectEntrySize) != 0)
    {
      AddError(errors, $"Object section size ({objectSection.Length}) is not aligned to {ReadAniObjectEntrySize} bytes.");
    }
    int objectCount = objectSection.Length / ReadAniObjectEntrySize;

    uint maxVertexId = 0u;
    for (int vertexIndex = 0; vertexIndex < vertexIds.Length; ++vertexIndex)
    {
      uint vertexRef = vertexIds[vertexIndex];
      if (vertexRef >= packedVertexCount)
      {
        AddError(errors, $"VertexId[{vertexIndex}] references out-of-range packed vertex {vertexRef} (count={packedVertexCount}).");
      }

      if (vertexRef > maxVertexId)
      {
        maxVertexId = vertexRef;
      }
    }

    if (vertexIds.Length > 0 && maxVertexId >= vertexColorCount)
    {
      AddError(errors, $"VertexColor count ({vertexColorCount}) is smaller than max vertex-color index ({maxVertexId}).");
    }

    for (int faceIndex = 0; faceIndex < faces.Length; ++faceIndex)
    {
      FaceEntry faceEntry = faces[faceIndex];
      if (faceEntry.VertexCount < 3)
      {
        AddError(errors, $"Face[{faceIndex}] has invalid vertex count {faceEntry.VertexCount}.");
      }

      ulong vertexEnd = faceEntry.VertexStartId + (ulong)faceEntry.VertexCount;
      if (vertexEnd > (ulong)vertexIds.Length)
      {
        AddError(errors, $"Face[{faceIndex}] vertex range exceeds VertexId section ({faceEntry.VertexStartId}+{faceEntry.VertexCount}>{vertexIds.Length}).");
      }
    }

    for (int faceIdIndex = 0; faceIdIndex < faceIds.Length; ++faceIdIndex)
    {
      uint referencedFace = faceIds[faceIdIndex];
      if (referencedFace >= faces.Length)
      {
        AddError(errors, $"FaceId[{faceIdIndex}] references out-of-range face {referencedFace} (count={faces.Length}).");
      }
    }

    for (int cFaceIdIndex = 0; cFaceIdIndex < cFaceIdValues.Length; ++cFaceIdIndex)
    {
      uint referencedFace = cFaceIdValues[cFaceIdIndex];
      if (referencedFace >= faces.Length)
      {
        AddError(errors, $"CFaceId[{cFaceIdIndex}] references out-of-range face {referencedFace} (count={faces.Length}).");
      }
    }

    long expectedRenderVertexCount = 0;
    for (int groupIndex = 0; groupIndex < matGroups.Length; ++groupIndex)
    {
      MatGroupEntry group = matGroups[groupIndex];
      bool byteVertexMode = (group.Attribute & MatGroupAttrByteVertex) != 0;
      bool wordVertexMode = (group.Attribute & MatGroupAttrWordVertex) != 0;
      if (byteVertexMode && wordVertexMode)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] has invalid attribute 0x{group.Attribute:X4}: byte and word vertex modes are both set.");
      }

      if (byteVertexMode && bVertexCount == 0)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] requests byte-vertex mode but BVertex section is empty.");
      }

      if (wordVertexMode && wVertexCount == 0)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] requests word-vertex mode but WVertex section is empty.");
      }

      bool hasObjectBit = (group.Attribute & MatGroupAttrObject) != 0;
      if (objectCount > 0 && !hasObjectBit)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] has attribute 0x{group.Attribute:X4} without object flag (0x2000) while object section is present (count={objectCount}).");
      }

      if (group.MaterialId < -1 || group.MaterialId >= map.Materials.Length)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] has invalid material id {group.MaterialId} (materials={map.Materials.Length}, valid -1..{Math.Max(-1, map.Materials.Length - 1)}).");
      }

      if (group.LightMapId < -1 || group.LightMapId >= map.LightmapTextures.Length)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] has invalid lightmap id {group.LightMapId} (lightmaps={map.LightmapTextures.Length}, valid -1..{Math.Max(-1, map.LightmapTextures.Length - 1)}).");
      }

      if (group.ObjectId > objectCount)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] has invalid object id {group.ObjectId} (object count={objectCount}, valid 0..{objectCount}).");
      }

      ulong groupEnd = group.FaceStartId + group.FaceCount;
      if (groupEnd > (ulong)faceIds.Length)
      {
        AddError(errors, $"ReadMatGroup[{groupIndex}] face range exceeds FaceId section ({group.FaceStartId}+{group.FaceCount}>{faceIds.Length}).");
        continue;
      }

      int functionId = byteVertexMode ? 1 : wordVertexMode ? 2 : 4;
      int baseIndex = functionId == 2 ? bVertexCount : 0;
      for (ulong groupFaceIndex = group.FaceStartId; groupFaceIndex < groupEnd; ++groupFaceIndex)
      {
        uint referencedFace = faceIds[(int)groupFaceIndex];
        if (referencedFace >= faces.Length)
        {
          continue;
        }

        FaceEntry faceEntry = faces[(int)referencedFace];
        if (faceEntry.VertexCount < 3)
        {
          continue;
        }

        bool faceValid = true;
        for (int faceVertex = 0; faceVertex < faceEntry.VertexCount; ++faceVertex)
        {
          ulong vertexIdIndex = faceEntry.VertexStartId + (ulong)faceVertex;
          if (vertexIdIndex >= (ulong)vertexIds.Length)
          {
            faceValid = false;
            break;
          }

          uint sourceIndex = vertexIds[(int)vertexIdIndex];
          ulong decompressedIndex = (ulong)baseIndex + sourceIndex;
          if (decompressedIndex >= (ulong)packedVertexCount)
          {
            AddError(errors, $"Face vertex reference resolves to out-of-range decompressed index {decompressedIndex} (packed count={packedVertexCount}).");
            faceValid = false;
            break;
          }
        }

        if (!faceValid)
        {
          continue;
        }

        expectedRenderVertexCount = checked(expectedRenderVertexCount + ((long)faceEntry.VertexCount - 2L) * 3L);
      }
    }

    if (expectedRenderVertexCount != map.BspRenderVertices.Length)
    {
      AddError(errors, $"Expected render vertex count from BSP sections is {expectedRenderVertexCount}, but loaded map contains {map.BspRenderVertices.Length}.");
    }

    for (int nodeIndex = 0; nodeIndex < nodeEntries.Length; ++nodeIndex)
    {
      NodeEntry node = nodeEntries[nodeIndex];
      if (node.NormalIndex >= cPlaneCount)
      {
        AddError(errors, $"Node[{nodeIndex}] has out-of-range normal index {node.NormalIndex} (count={cPlaneCount}).");
      }

      ValidateNodeChildRef(nodeIndex, "front", node.Front, nodeEntries.Length, leafEntries.Length, errors);
      ValidateNodeChildRef(nodeIndex, "back", node.Back, nodeEntries.Length, leafEntries.Length, errors);
    }

    if (faceCount > 0 && nodeEntries.Length <= 1)
    {
      AddError(errors, "Node section does not contain a runtime root node (index 1).");
    }

    if (faceCount > 0 && leafEntries.Length <= 1)
    {
      AddError(errors, "Leaf section does not contain runtime leaves (index 1+).");
    }

    for (int leafIndex = 0; leafIndex < leafEntries.Length; ++leafIndex)
    {
      LeafEntry leaf = leafEntries[leafIndex];
      ulong faceEnd = leaf.FaceStartId + leaf.FaceCount;
      if (faceEnd > (ulong)cFaceIdValues.Length)
      {
        AddError(errors, $"Leaf[{leafIndex}] face range exceeds CFaceId section ({leaf.FaceStartId}+{leaf.FaceCount}>{cFaceIdValues.Length}).");
      }

      ulong matEnd = leaf.MatGroupStartId + leaf.MatGroupCount;
      if (matEnd > (ulong)matListInLeafValues.Length)
      {
        AddError(errors, $"Leaf[{leafIndex}] material-group range exceeds MatListInLeaf section ({leaf.MatGroupStartId}+{leaf.MatGroupCount}>{matListInLeafValues.Length}).");
        continue;
      }

      for (ulong matOffset = leaf.MatGroupStartId; matOffset < matEnd; ++matOffset)
      {
        ushort matGroupId = matListInLeafValues[(int)matOffset];
        if (matGroupId >= matGroups.Length)
        {
          AddError(errors, $"MatListInLeaf[{matOffset}] references out-of-range mat-group id {matGroupId} (count={matGroups.Length}).");
        }
      }
    }

    if (saveValidation)
    {
      SpatialPatternMetrics rebuiltPattern = BuildSpatialPatternMetrics(faceCount, cFaceIdValues, leafEntries);
      ValidateLegacySpatialPatternOnSave(map.BspPath, rebuiltPattern, errors);
    }
  }

  private static void ValidateLegacySpatialPatternOnSave(
    string sourceBspPath,
    SpatialPatternMetrics rebuiltPattern,
    List<string> errors)
  {
    if (string.IsNullOrWhiteSpace(sourceBspPath))
    {
      AddError(errors, "Save strict validation requires source BSP path for legacy CFaceId/Leaf diagnostics.");
      return;
    }

    if (!TryReadSourceSpatialPatternMetrics(sourceBspPath, out SpatialPatternMetrics sourcePattern, out string readError))
    {
      AddError(errors, $"Save strict validation failed to inspect source BSP pattern: {readError}");
      return;
    }

    if (sourcePattern.CFaceIdCount <= 0 || sourcePattern.RuntimeLeafCount <= 0)
    {
      return;
    }

    if (sourcePattern.DuplicateCFaceCount > 0 && rebuiltPattern.DuplicateCFaceCount <= 0)
    {
      AddError(
        errors,
        "CFaceId/Leaf rebuild lost all duplicate CFaceId entries. " +
        "Legacy BSP expects repeated face references across runtime leaves.");
    }

    if (sourcePattern.CFaceIdCount > sourcePattern.FaceCount && rebuiltPattern.CFaceIdCount <= rebuiltPattern.FaceCount)
    {
      AddError(
        errors,
        "CFaceId count collapsed to one-reference-per-face, but source BSP uses multi-leaf face membership.");
    }

    if (sourcePattern.DuplicateRatio >= 0.10f && rebuiltPattern.DuplicateRatio < sourcePattern.DuplicateRatio * 0.25f)
    {
      AddError(
        errors,
        $"CFaceId duplicate ratio dropped too far from source ({rebuiltPattern.DuplicateRatio:F3} vs {sourcePattern.DuplicateRatio:F3}).");
    }

    if (sourcePattern.AvgRuntimeLeafFaceRefs >= 8.0f
      && rebuiltPattern.AvgRuntimeLeafFaceRefs < sourcePattern.AvgRuntimeLeafFaceRefs * 0.50f)
    {
      AddError(
        errors,
        $"Average runtime leaf face references dropped too far from source ({rebuiltPattern.AvgRuntimeLeafFaceRefs:F2} vs {sourcePattern.AvgRuntimeLeafFaceRefs:F2}).");
    }

    if (sourcePattern.MaxRuntimeLeafFaceRefs >= 128
      && rebuiltPattern.MaxRuntimeLeafFaceRefs < sourcePattern.MaxRuntimeLeafFaceRefs / 8)
    {
      AddError(
        errors,
        $"Max runtime leaf face references dropped too far from source ({rebuiltPattern.MaxRuntimeLeafFaceRefs} vs {sourcePattern.MaxRuntimeLeafFaceRefs}).");
    }
  }

  private static bool TryReadSourceSpatialPatternMetrics(
    string sourceBspPath,
    out SpatialPatternMetrics metrics,
    out string error)
  {
    metrics = default;
    error = string.Empty;

    string resolvedPath = Path.GetFullPath(sourceBspPath);
    if (!File.Exists(resolvedPath))
    {
      error = $"Source BSP does not exist: {resolvedPath}";
      return false;
    }

    try
    {
      using FileStream stream = File.OpenRead(resolvedPath);
      using BinaryReader reader = new(stream);

      if (stream.Length < BspHeaderSize)
      {
        error = $"Source BSP is too small ({stream.Length} bytes).";
        return false;
      }

      _ = reader.ReadUInt32(); // version
      BspEntry[] entries = new BspEntry[BspHeaderEntryCount];
      for (int i = 0; i < entries.Length; ++i)
      {
        uint offset = reader.ReadUInt32();
        uint size = reader.ReadUInt32();
        entries[i] = new BspEntry(offset, size);
      }

      if ((entries[BspSectionFace].Size % 6u) != 0u
        || (entries[BspSectionCFaceId].Size % 4u) != 0u
        || (entries[BspSectionLeaf].Size % 25u) != 0u)
      {
        error = "Source BSP section stride mismatch (Face/CFaceId/Leaf).";
        return false;
      }

      byte[] cFaceIdBytes = ReadSectionBytes(stream, entries[BspSectionCFaceId], BspSectionCFaceId);
      byte[] leafBytes = ReadSectionBytes(stream, entries[BspSectionLeaf], BspSectionLeaf);

      int faceCount = checked((int)(entries[BspSectionFace].Size / 6u));
      uint[] cFaceIds = ParseUInt32Array(cFaceIdBytes);
      LeafEntry[] leaves = ParseLeaves(leafBytes);
      metrics = BuildSpatialPatternMetrics(faceCount, cFaceIds, leaves);
      return true;
    }
    catch (Exception ex)
    {
      error = ex.Message;
      return false;
    }
  }

  private static byte[] ReadSectionBytes(FileStream stream, BspEntry entry, int sectionIndex)
  {
    if (entry.Size == 0u)
    {
      return Array.Empty<byte>();
    }

    long offset = entry.Offset;
    int size = checked((int)entry.Size);
    if (offset < 0 || offset + size > stream.Length)
    {
      throw new InvalidDataException(
        $"Source BSP section {sectionIndex} is outside file bounds (offset={offset}, size={size}, file={stream.Length}).");
    }

    stream.Position = offset;
    byte[] bytes = new byte[size];
    int read = stream.Read(bytes, 0, size);
    if (read != size)
    {
      throw new EndOfStreamException($"Failed to read full source BSP section {sectionIndex} ({read}/{size} bytes).");
    }

    return bytes;
  }

  private static SpatialPatternMetrics BuildSpatialPatternMetrics(int faceCount, uint[] cFaceIds, LeafEntry[] leaves)
  {
    HashSet<uint> uniqueCFaceIds = new();
    for (int i = 0; i < cFaceIds.Length; ++i)
    {
      uniqueCFaceIds.Add(cFaceIds[i]);
    }

    int duplicateCFaceCount = cFaceIds.Length - uniqueCFaceIds.Count;
    int runtimeLeafCount = Math.Max(0, leaves.Length - 1);
    ulong runtimeLeafFaceRefTotal = 0ul;
    int maxRuntimeLeafFaceRefs = 0;
    for (int i = 1; i < leaves.Length; ++i)
    {
      int faceRefs = leaves[i].FaceCount > int.MaxValue ? int.MaxValue : (int)leaves[i].FaceCount;
      runtimeLeafFaceRefTotal += leaves[i].FaceCount;
      if (faceRefs > maxRuntimeLeafFaceRefs)
      {
        maxRuntimeLeafFaceRefs = faceRefs;
      }
    }

    float avgRuntimeLeafFaceRefs = runtimeLeafCount > 0
      ? (float)runtimeLeafFaceRefTotal / runtimeLeafCount
      : 0.0f;
    float duplicateRatio = cFaceIds.Length > 0
      ? (float)duplicateCFaceCount / cFaceIds.Length
      : 0.0f;

    return new SpatialPatternMetrics(
      faceCount,
      cFaceIds.Length,
      uniqueCFaceIds.Count,
      duplicateCFaceCount,
      leaves.Length,
      runtimeLeafCount,
      maxRuntimeLeafFaceRefs,
      avgRuntimeLeafFaceRefs,
      duplicateRatio);
  }

  private static void ValidateNodeChildRef(
    int nodeIndex,
    string sideName,
    short child,
    int nodeCount,
    int leafCount,
    List<string> errors)
  {
    if (child > 0)
    {
      if (child >= nodeCount)
      {
        AddError(errors, $"Node[{nodeIndex}].{sideName} child references missing node {child} (node count={nodeCount}).");
      }

      return;
    }

    if (child < 0)
    {
      int leafIndex = -child - 1;
      if (leafIndex < 0 || leafIndex >= leafCount)
      {
        AddError(errors, $"Node[{nodeIndex}].{sideName} child references missing leaf {leafIndex} (leaf count={leafCount}).");
      }
    }
  }

  private static void CheckSectionStride(string sectionName, int byteLength, int stride, List<string> errors)
  {
    if ((byteLength % stride) != 0)
    {
      AddError(errors, $"{sectionName} section size ({byteLength}) is not aligned to {stride} bytes.");
    }
  }

  private static FaceEntry[] ParseFaces(byte[] bytes, List<string> errors)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<FaceEntry>();
    }

    int count = bytes.Length / 6;
    FaceEntry[] faces = new FaceEntry[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 6;
      ushort vertexCount = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(offset, 2));
      uint vertexStartId = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(offset + 2, 4));
      faces[i] = new FaceEntry(vertexCount, vertexStartId);
      if (vertexCount == 0)
      {
        AddError(errors, $"Face[{i}] has zero vertex count.");
      }
    }

    return faces;
  }

  private static MatGroupEntry[] ParseMatGroups(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<MatGroupEntry>();
    }

    int count = bytes.Length / 42;
    MatGroupEntry[] groups = new MatGroupEntry[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 42;
      ushort attribute = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(offset, 2));
      ushort faceCount = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(offset + 2, 2));
      uint faceStartId = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(offset + 4, 4));
      short materialId = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 8, 2));
      short lightMapId = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 10, 2));
      ushort objectId = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(offset + 40, 2));
      groups[i] = new MatGroupEntry(attribute, faceCount, faceStartId, materialId, lightMapId, objectId);
    }

    return groups;
  }

  private static NodeEntry[] ParseNodes(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<NodeEntry>();
    }

    int count = bytes.Length / 24;
    NodeEntry[] nodes = new NodeEntry[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 24;
      uint normalIndex = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(offset, 4));
      short front = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 8, 2));
      short back = BinaryPrimitives.ReadInt16LittleEndian(bytes.AsSpan(offset + 10, 2));
      nodes[i] = new NodeEntry(normalIndex, front, back);
    }

    return nodes;
  }

  private static LeafEntry[] ParseLeaves(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<LeafEntry>();
    }

    int count = bytes.Length / 25;
    LeafEntry[] leaves = new LeafEntry[count];
    for (int i = 0; i < count; ++i)
    {
      int offset = i * 25;
      ushort faceCount = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(offset + 1, 2));
      uint faceStartId = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(offset + 3, 4));
      ushort matGroupCount = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(offset + 7, 2));
      uint matGroupStartId = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(offset + 9, 4));
      leaves[i] = new LeafEntry(faceCount, faceStartId, matGroupCount, matGroupStartId);
    }

    return leaves;
  }

  private static uint[] ParseUInt32Array(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<uint>();
    }

    int count = bytes.Length / 4;
    uint[] values = new uint[count];
    for (int i = 0; i < count; ++i)
    {
      values[i] = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(i * 4, 4));
    }

    return values;
  }

  private static ushort[] ParseUInt16Array(byte[] bytes)
  {
    if (bytes.Length == 0)
    {
      return Array.Empty<ushort>();
    }

    int count = bytes.Length / 2;
    ushort[] values = new ushort[count];
    for (int i = 0; i < count; ++i)
    {
      values[i] = BinaryPrimitives.ReadUInt16LittleEndian(bytes.AsSpan(i * 2, 2));
    }

    return values;
  }

  private static bool IsFinite(Vector3 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y) && float.IsFinite(value.Z);
  }

  private static bool IsFinite(Vector2 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y);
  }

  private static void AddError(List<string> errors, string message)
  {
    if (errors.Count < MaxCollectedErrors)
    {
      errors.Add(message);
    }
  }

  private readonly record struct FaceEntry(ushort VertexCount, ulong VertexStartId);
  private readonly record struct MatGroupEntry(
    ushort Attribute,
    ulong FaceCount,
    ulong FaceStartId,
    short MaterialId,
    short LightMapId,
    ushort ObjectId);
  private readonly record struct NodeEntry(uint NormalIndex, short Front, short Back);
  private readonly record struct LeafEntry(ulong FaceCount, ulong FaceStartId, ulong MatGroupCount, ulong MatGroupStartId);
  private readonly record struct SpatialPatternMetrics(
    int FaceCount,
    int CFaceIdCount,
    int UniqueCFaceIdCount,
    int DuplicateCFaceCount,
    int LeafCount,
    int RuntimeLeafCount,
    int MaxRuntimeLeafFaceRefs,
    float AvgRuntimeLeafFaceRefs,
    float DuplicateRatio);
}
