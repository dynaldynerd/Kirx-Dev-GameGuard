using OpenTK.Mathematics;

namespace MapEditor.Formats;

public static class MapEditOperations
{
  public static bool TryAppendCollisionWallSegment(
    LoadedMap source,
    Vector3 sourceStart,
    Vector3 sourceEnd,
    float wallHeight,
    float embedDepth,
    float snapDistance,
    out LoadedMap result,
    out string error,
    out float resolvedWallHeight,
    out float resolvedEmbedDepth,
    out bool startSnapped,
    out bool endSnapped)
  {
    result = source;
    error = string.Empty;
    resolvedWallHeight = wallHeight;
    resolvedEmbedDepth = embedDepth;
    startSnapped = false;
    endSnapped = false;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if (!IsFinite(sourceStart) || !IsFinite(sourceEnd))
    {
      error = "Collision segment points are invalid.";
      return false;
    }

    if (!float.IsFinite(wallHeight) || wallHeight <= 0.0f)
    {
      error = "Wall height must be a finite number greater than zero.";
      return false;
    }

    if (!float.IsFinite(embedDepth) || embedDepth < 0.0f)
    {
      error = "Embed depth must be a finite non-negative number.";
      return false;
    }

    if (!float.IsFinite(snapDistance) || snapDistance < 0.0f)
    {
      error = "Snap distance must be a finite non-negative number.";
      return false;
    }

    float lengthSq = (sourceEnd - sourceStart).LengthSquared;
    if (!float.IsFinite(lengthSq) || lengthSq < 0.0001f)
    {
      error = "Collision segment is too short.";
      return false;
    }

    List<Vector3> vertices = new(source.CollisionVertices.Length + 2);
    vertices.AddRange(source.CollisionVertices);
    List<CollisionLine> lines = new(source.CollisionLines.Length + 1);
    lines.AddRange(source.CollisionLines);

    if (vertices.Count > ushort.MaxValue - 2)
    {
      error = $"Collision vertex count would exceed {ushort.MaxValue}.";
      return false;
    }

    if (lines.Count > ushort.MaxValue - 1)
    {
      error = $"Collision line count would exceed {ushort.MaxValue}.";
      return false;
    }

    Vector3 buriedStart = new(sourceStart.X, sourceStart.Y - embedDepth, sourceStart.Z);
    Vector3 buriedEnd = new(sourceEnd.X, sourceEnd.Y - embedDepth, sourceEnd.Z);
    if (!IsFinite(buriedStart) || !IsFinite(buriedEnd))
    {
      error = "Buried collision segment points are invalid.";
      return false;
    }

    if (snapDistance > 0.0f && source.CollisionVertices.Length > 0)
    {
      buriedStart = SnapPointToNearestVertex(
        source.CollisionVertices,
        buriedStart,
        snapDistance,
        excludedIndex: -1,
        out startSnapped,
        out int snappedStartIndex);
      buriedEnd = SnapPointToNearestVertex(
        source.CollisionVertices,
        buriedEnd,
        snapDistance,
        excludedIndex: snappedStartIndex,
        out endSnapped,
        out int snappedEndIndex);

      int primarySnapVertexIndex = startSnapped ? snappedStartIndex : (endSnapped ? snappedEndIndex : -1);
      if (primarySnapVertexIndex >= 0 &&
          TryGetFirstCollisionLineAtVertex(source.CollisionLines, primarySnapVertexIndex, out CollisionLine snapSourceLine) &&
          float.IsFinite(snapSourceLine.Height) &&
          snapSourceLine.Height > 0.0f)
      {
        resolvedWallHeight = snapSourceLine.Height;
      }

      if (startSnapped || endSnapped)
      {
        float snapDepthSum = 0.0f;
        int snapDepthCount = 0;

        if (startSnapped)
        {
          float startSnapDepth = sourceStart.Y - buriedStart.Y;
          if (float.IsFinite(startSnapDepth) && startSnapDepth >= 0.0f)
          {
            snapDepthSum += startSnapDepth;
            ++snapDepthCount;
          }
        }

        if (endSnapped)
        {
          float endSnapDepth = sourceEnd.Y - buriedEnd.Y;
          if (float.IsFinite(endSnapDepth) && endSnapDepth >= 0.0f)
          {
            snapDepthSum += endSnapDepth;
            ++snapDepthCount;
          }
        }

        if (snapDepthCount > 0)
        {
          resolvedEmbedDepth = snapDepthSum / snapDepthCount;
        }

        if (startSnapped && !endSnapped)
        {
          buriedEnd = new Vector3(sourceEnd.X, sourceEnd.Y - resolvedEmbedDepth, sourceEnd.Z);
        }
        else if (!startSnapped && endSnapped)
        {
          buriedStart = new Vector3(sourceStart.X, sourceStart.Y - resolvedEmbedDepth, sourceStart.Z);
        }
      }
    }

    if (!IsFinite(buriedStart) || !IsFinite(buriedEnd))
    {
      error = "Resolved collision segment points are invalid.";
      return false;
    }

    if (!float.IsFinite(resolvedWallHeight) || resolvedWallHeight <= 0.0f)
    {
      error = "Resolved wall height is invalid.";
      return false;
    }

    if (!float.IsFinite(resolvedEmbedDepth) || resolvedEmbedDepth < 0.0f)
    {
      error = "Resolved embed depth is invalid.";
      return false;
    }

    lengthSq = (buriedEnd - buriedStart).LengthSquared;
    if (!float.IsFinite(lengthSq) || lengthSq < 0.0001f)
    {
      error = "Collision segment is too short after snap.";
      return false;
    }

    int startVertex = vertices.Count;
    vertices.Add(buriedStart);
    vertices.Add(buriedEnd);

    uint attr = 0u;
    ushort front = 0;
    ushort back = 0;
    lines.Add(new CollisionLine(attr, (ushort)startVertex, (ushort)(startVertex + 1), resolvedWallHeight, front, back));

    result = WithCollisionData(source, vertices.ToArray(), lines.ToArray());
    return true;
  }

  public static bool TryResolveCollisionWallPreview(
    LoadedMap source,
    Vector3 sourceStart,
    Vector3 sourceEnd,
    float embedDepth,
    float snapDistance,
    out Vector3 resolvedSourceStart,
    out Vector3 resolvedSourceEnd,
    out bool startSnapped,
    out bool endSnapped,
    out string error)
  {
    resolvedSourceStart = sourceStart;
    resolvedSourceEnd = sourceEnd;
    startSnapped = false;
    endSnapped = false;
    error = string.Empty;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if (!IsFinite(sourceStart) || !IsFinite(sourceEnd))
    {
      error = "Collision segment points are invalid.";
      return false;
    }

    if (!float.IsFinite(embedDepth) || embedDepth < 0.0f)
    {
      error = "Embed depth must be a finite non-negative number.";
      return false;
    }

    if (!float.IsFinite(snapDistance) || snapDistance < 0.0f)
    {
      error = "Snap distance must be a finite non-negative number.";
      return false;
    }

    float lengthSq = (sourceEnd - sourceStart).LengthSquared;
    if (!float.IsFinite(lengthSq) || lengthSq < 0.0001f)
    {
      error = "Collision segment is too short.";
      return false;
    }

    Vector3 buriedStart = new(sourceStart.X, sourceStart.Y - embedDepth, sourceStart.Z);
    Vector3 buriedEnd = new(sourceEnd.X, sourceEnd.Y - embedDepth, sourceEnd.Z);
    if (!IsFinite(buriedStart) || !IsFinite(buriedEnd))
    {
      error = "Buried collision segment points are invalid.";
      return false;
    }

    float resolvedEmbedDepth = embedDepth;
    if (snapDistance > 0.0f && source.CollisionVertices.Length > 0)
    {
      buriedStart = SnapPointToNearestVertex(
        source.CollisionVertices,
        buriedStart,
        snapDistance,
        excludedIndex: -1,
        out startSnapped,
        out int snappedStartIndex);
      buriedEnd = SnapPointToNearestVertex(
        source.CollisionVertices,
        buriedEnd,
        snapDistance,
        excludedIndex: snappedStartIndex,
        out endSnapped,
        out _);

      if (startSnapped || endSnapped)
      {
        float snapDepthSum = 0.0f;
        int snapDepthCount = 0;

        if (startSnapped)
        {
          float startSnapDepth = sourceStart.Y - buriedStart.Y;
          if (float.IsFinite(startSnapDepth) && startSnapDepth >= 0.0f)
          {
            snapDepthSum += startSnapDepth;
            ++snapDepthCount;
          }
        }

        if (endSnapped)
        {
          float endSnapDepth = sourceEnd.Y - buriedEnd.Y;
          if (float.IsFinite(endSnapDepth) && endSnapDepth >= 0.0f)
          {
            snapDepthSum += endSnapDepth;
            ++snapDepthCount;
          }
        }

        if (snapDepthCount > 0)
        {
          resolvedEmbedDepth = snapDepthSum / snapDepthCount;
        }

        if (startSnapped && !endSnapped)
        {
          buriedEnd = new Vector3(sourceEnd.X, sourceEnd.Y - resolvedEmbedDepth, sourceEnd.Z);
        }
        else if (!startSnapped && endSnapped)
        {
          buriedStart = new Vector3(sourceStart.X, sourceStart.Y - resolvedEmbedDepth, sourceStart.Z);
        }
      }
    }

    if (!IsFinite(buriedStart) || !IsFinite(buriedEnd))
    {
      error = "Resolved collision segment points are invalid.";
      return false;
    }

    lengthSq = (buriedEnd - buriedStart).LengthSquared;
    if (!float.IsFinite(lengthSq) || lengthSq < 0.0001f)
    {
      error = "Collision segment is too short after snap.";
      return false;
    }

    resolvedSourceStart = new Vector3(buriedStart.X, buriedStart.Y + resolvedEmbedDepth, buriedStart.Z);
    resolvedSourceEnd = new Vector3(buriedEnd.X, buriedEnd.Y + resolvedEmbedDepth, buriedEnd.Z);
    return IsFinite(resolvedSourceStart) && IsFinite(resolvedSourceEnd);
  }

  public static bool TryAppendBoundaryWalls(
    LoadedMap source,
    float margin,
    float wallHeight,
    float embedDepth,
    out LoadedMap result,
    out string error)
  {
    result = source;
    error = string.Empty;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if (!float.IsFinite(margin) || margin < 0.0f)
    {
      error = "Boundary margin must be a finite non-negative number.";
      return false;
    }

    if (!float.IsFinite(wallHeight) || wallHeight <= 0.0f)
    {
      error = "Boundary wall height must be a finite number greater than zero.";
      return false;
    }

    if (!float.IsFinite(embedDepth) || embedDepth < 0.0f)
    {
      error = "Boundary embed depth must be a finite non-negative number.";
      return false;
    }

    List<Vector3> vertices = new(source.CollisionVertices.Length + 4);
    vertices.AddRange(source.CollisionVertices);
    List<CollisionLine> lines = new(source.CollisionLines.Length + 4);
    lines.AddRange(source.CollisionLines);

    if (vertices.Count > ushort.MaxValue - 4)
    {
      error = $"Collision vertex count would exceed {ushort.MaxValue}.";
      return false;
    }

    if (lines.Count > ushort.MaxValue - 4)
    {
      error = $"Collision line count would exceed {ushort.MaxValue}.";
      return false;
    }

    float baseY = ResolveBoundaryBaseY(source) - embedDepth;
    float minX = source.Bounds.Min.X - margin;
    float maxX = source.Bounds.Max.X + margin;
    float minZ = source.Bounds.Min.Z - margin;
    float maxZ = source.Bounds.Max.Z + margin;
    if (!float.IsFinite(minX) || !float.IsFinite(maxX) || !float.IsFinite(minZ) || !float.IsFinite(maxZ))
    {
      error = "Map bounds are not finite.";
      return false;
    }

    int startVertex = vertices.Count;
    vertices.Add(new Vector3(minX, baseY, minZ));
    vertices.Add(new Vector3(maxX, baseY, minZ));
    vertices.Add(new Vector3(maxX, baseY, maxZ));
    vertices.Add(new Vector3(minX, baseY, maxZ));

    uint attr = 0u;
    ushort front = 0;
    ushort back = 0;
    lines.Add(new CollisionLine(attr, (ushort)startVertex, (ushort)(startVertex + 1), wallHeight, front, back));
    lines.Add(new CollisionLine(attr, (ushort)(startVertex + 1), (ushort)(startVertex + 2), wallHeight, front, back));
    lines.Add(new CollisionLine(attr, (ushort)(startVertex + 2), (ushort)(startVertex + 3), wallHeight, front, back));
    lines.Add(new CollisionLine(attr, (ushort)(startVertex + 3), (ushort)startVertex, wallHeight, front, back));

    result = WithCollisionData(source, vertices.ToArray(), lines.ToArray());
    return true;
  }

  public static bool TryAppendWallInFrontOfCamera(
    LoadedMap source,
    Vector3 sourceCameraPosition,
    Vector3 sourceForwardDirection,
    float forwardDistance,
    float wallWidth,
    float wallHeight,
    float embedDepth,
    float snapDistance,
    out LoadedMap result,
    out string error,
    out float resolvedWallHeight,
    out float resolvedEmbedDepth,
    out bool startSnapped,
    out bool endSnapped)
  {
    result = source;
    error = string.Empty;
    resolvedWallHeight = wallHeight;
    resolvedEmbedDepth = embedDepth;
    startSnapped = false;
    endSnapped = false;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if (!IsFinite(sourceCameraPosition))
    {
      error = "Camera position is invalid.";
      return false;
    }

    if (!IsFinite(sourceForwardDirection))
    {
      error = "Camera direction is invalid.";
      return false;
    }

    if (!float.IsFinite(forwardDistance) || forwardDistance < 0.0f)
    {
      error = "Forward distance must be a finite non-negative number.";
      return false;
    }

    if (!float.IsFinite(wallWidth) || wallWidth <= 0.0f)
    {
      error = "Wall width must be a finite number greater than zero.";
      return false;
    }

    if (!float.IsFinite(wallHeight) || wallHeight <= 0.0f)
    {
      error = "Wall height must be a finite number greater than zero.";
      return false;
    }

    if (!float.IsFinite(embedDepth) || embedDepth < 0.0f)
    {
      error = "Embed depth must be a finite non-negative number.";
      return false;
    }

    Vector3 flatForward = new(sourceForwardDirection.X, 0.0f, sourceForwardDirection.Z);
    if (flatForward.LengthSquared < 0.000001f)
    {
      flatForward = new Vector3(0.0f, 0.0f, -1.0f);
    }
    else
    {
      flatForward = Vector3.Normalize(flatForward);
    }

    Vector3 center = sourceCameraPosition + flatForward * forwardDistance;
    float baseY = ResolveGroundYNear(source, center, sourceCameraPosition.Y);
    center.Y = baseY;

    Vector3 right = Vector3.Cross(flatForward, Vector3.UnitY);
    if (right.LengthSquared < 0.000001f)
    {
      right = Vector3.UnitX;
    }
    else
    {
      right = Vector3.Normalize(right);
    }

    float halfWidth = wallWidth * 0.5f;
    Vector3 start = center - right * halfWidth;
    Vector3 end = center + right * halfWidth;
    if (!IsFinite(start) || !IsFinite(end))
    {
      error = "Computed wall coordinates are invalid.";
      return false;
    }

    return TryAppendCollisionWallSegment(
      source,
      start,
      end,
      wallHeight,
      embedDepth,
      snapDistance,
      out result,
      out error,
      out resolvedWallHeight,
      out resolvedEmbedDepth,
      out startSnapped,
      out endSnapped);
  }

  public static bool TryRemoveCollisionLine(
    LoadedMap source,
    int lineIndex,
    out LoadedMap result,
    out string error)
  {
    result = source;
    error = string.Empty;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if ((uint)lineIndex >= (uint)source.CollisionLines.Length)
    {
      error = $"Collision line index {lineIndex} is out of range.";
      return false;
    }

    List<CollisionLine> remainingLines = new(source.CollisionLines.Length - 1);
    for (int i = 0; i < source.CollisionLines.Length; ++i)
    {
      if (i == lineIndex)
      {
        continue;
      }

      remainingLines.Add(source.CollisionLines[i]);
    }

    if (remainingLines.Count == 0)
    {
      result = WithCollisionData(source, Array.Empty<Vector3>(), Array.Empty<CollisionLine>());
      return true;
    }

    if (source.CollisionVertices.Length == 0)
    {
      error = "Collision vertex list is empty while collision lines exist.";
      return false;
    }

    bool[] usedVertices = new bool[source.CollisionVertices.Length];
    for (int i = 0; i < remainingLines.Count; ++i)
    {
      CollisionLine line = remainingLines[i];
      if (line.StartVertex >= source.CollisionVertices.Length || line.EndVertex >= source.CollisionVertices.Length)
      {
        error = $"Collision line {i} references out-of-range vertices.";
        return false;
      }

      usedVertices[line.StartVertex] = true;
      usedVertices[line.EndVertex] = true;
    }

    int[] remap = new int[source.CollisionVertices.Length];
    Array.Fill(remap, -1);
    List<Vector3> remappedVertices = new(Math.Max(16, remainingLines.Count * 2));
    for (int oldIndex = 0; oldIndex < usedVertices.Length; ++oldIndex)
    {
      if (!usedVertices[oldIndex])
      {
        continue;
      }

      remap[oldIndex] = remappedVertices.Count;
      remappedVertices.Add(source.CollisionVertices[oldIndex]);
    }

    if (remappedVertices.Count > ushort.MaxValue)
    {
      error = $"Collision vertex count would exceed {ushort.MaxValue}.";
      return false;
    }

    List<CollisionLine> remappedLines = new(remainingLines.Count);
    for (int i = 0; i < remainingLines.Count; ++i)
    {
      CollisionLine line = remainingLines[i];
      int newStart = remap[line.StartVertex];
      int newEnd = remap[line.EndVertex];
      if (newStart < 0 || newEnd < 0)
      {
        error = $"Collision line {i} references a removed vertex.";
        return false;
      }

      remappedLines.Add(new CollisionLine(
        line.Attribute,
        (ushort)newStart,
        (ushort)newEnd,
        line.Height,
        line.FrontLeaf,
        line.BackLeaf));
    }

    result = WithCollisionData(source, remappedVertices.ToArray(), remappedLines.ToArray());
    return true;
  }

  public static bool TryGetCollisionLineEditValues(
    LoadedMap source,
    int lineIndex,
    out float margin,
    out float height,
    out float embedDepth,
    out string error)
  {
    margin = 0.0f;
    height = 0.0f;
    embedDepth = 0.0f;
    error = string.Empty;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if ((uint)lineIndex >= (uint)source.CollisionLines.Length)
    {
      error = $"Collision line index {lineIndex} is out of range.";
      return false;
    }

    CollisionLine line = source.CollisionLines[lineIndex];
    if (line.StartVertex >= source.CollisionVertices.Length || line.EndVertex >= source.CollisionVertices.Length)
    {
      error = $"Collision line {lineIndex} references out-of-range vertices.";
      return false;
    }

    Vector3 start = source.CollisionVertices[line.StartVertex];
    Vector3 end = source.CollisionVertices[line.EndVertex];
    if (!IsFinite(start) || !IsFinite(end))
    {
      error = $"Collision line {lineIndex} has invalid vertex coordinates.";
      return false;
    }

    Vector2 horizontal = new(end.X - start.X, end.Z - start.Z);
    margin = horizontal.Length;
    if (!float.IsFinite(margin) || margin <= 0.0f)
    {
      margin = 0.0f;
    }

    height = MathF.Max(0.0f, line.Height);
    Vector3 center = (start + end) * 0.5f;
    float baseY = (start.Y + end.Y) * 0.5f;
    float groundY = ResolveGroundYNear(source, center, baseY);
    embedDepth = MathF.Max(0.0f, groundY - baseY);
    if (!float.IsFinite(embedDepth))
    {
      embedDepth = 0.0f;
    }

    return true;
  }

  public static bool TryEditCollisionLineDimensions(
    LoadedMap source,
    int lineIndex,
    float margin,
    float wallHeight,
    float embedDepth,
    out LoadedMap result,
    out string error)
  {
    result = source;
    error = string.Empty;
    if (source == null)
    {
      error = "Map is not loaded.";
      return false;
    }

    if ((uint)lineIndex >= (uint)source.CollisionLines.Length)
    {
      error = $"Collision line index {lineIndex} is out of range.";
      return false;
    }

    if (!float.IsFinite(margin) || margin <= 0.0f)
    {
      error = "Collision margin must be a finite number greater than zero.";
      return false;
    }

    if (!float.IsFinite(wallHeight) || wallHeight <= 0.0f)
    {
      error = "Collision height must be a finite number greater than zero.";
      return false;
    }

    if (!float.IsFinite(embedDepth) || embedDepth < 0.0f)
    {
      error = "Collision bury depth must be a finite non-negative number.";
      return false;
    }

    CollisionLine[] lines = source.CollisionLines.ToArray();
    if ((uint)lineIndex >= (uint)lines.Length)
    {
      error = $"Collision line index {lineIndex} is out of range.";
      return false;
    }

    List<Vector3> vertices = new(source.CollisionVertices);
    if (!EnsureLineHasPrivateVertices(vertices, lines, lineIndex, out CollisionLine editableLine, out error))
    {
      return false;
    }

    if (editableLine.StartVertex >= vertices.Count || editableLine.EndVertex >= vertices.Count)
    {
      error = $"Collision line {lineIndex} references out-of-range vertices.";
      return false;
    }

    Vector3 start = vertices[editableLine.StartVertex];
    Vector3 end = vertices[editableLine.EndVertex];
    if (!IsFinite(start) || !IsFinite(end))
    {
      error = $"Collision line {lineIndex} has invalid vertex coordinates.";
      return false;
    }

    Vector2 horizontalDirection = new(end.X - start.X, end.Z - start.Z);
    float horizontalLength = horizontalDirection.Length;
    if (!float.IsFinite(horizontalLength) || horizontalLength < 0.0001f)
    {
      horizontalDirection = Vector2.UnitX;
      horizontalLength = 1.0f;
    }
    else
    {
      horizontalDirection /= horizontalLength;
    }

    Vector3 center = (start + end) * 0.5f;
    float baseY = (start.Y + end.Y) * 0.5f;
    float groundY = ResolveGroundYNear(source, center, baseY);
    float newBottomY = groundY - embedDepth;
    if (!float.IsFinite(newBottomY))
    {
      error = "Resolved collision base height is invalid.";
      return false;
    }

    float halfMargin = margin * 0.5f;
    Vector3 newStart = new(
      center.X - horizontalDirection.X * halfMargin,
      newBottomY,
      center.Z - horizontalDirection.Y * halfMargin);
    Vector3 newEnd = new(
      center.X + horizontalDirection.X * halfMargin,
      newBottomY,
      center.Z + horizontalDirection.Y * halfMargin);
    if (!IsFinite(newStart) || !IsFinite(newEnd))
    {
      error = "Resolved collision segment coordinates are invalid.";
      return false;
    }

    vertices[editableLine.StartVertex] = newStart;
    vertices[editableLine.EndVertex] = newEnd;
    lines[lineIndex] = new CollisionLine(
      editableLine.Attribute,
      editableLine.StartVertex,
      editableLine.EndVertex,
      wallHeight,
      editableLine.FrontLeaf,
      editableLine.BackLeaf);

    result = WithCollisionData(source, vertices.ToArray(), lines);
    return true;
  }

  public static LoadedMap WithCollisionData(
    LoadedMap source,
    Vector3[] collisionVertices,
    CollisionLine[] collisionLines)
  {
    return new LoadedMap
    {
      Name = source.Name,
      BspPath = source.BspPath,
      EbpPath = source.EbpPath,
      BspBinaryLayout = source.BspBinaryLayout,
      SkySourceMode = source.SkySourceMode,
      Environment = source.Environment,
      ExtDummies = source.ExtDummies,
      Bounds = source.Bounds,
      BspTriangleVertices = source.BspTriangleVertices,
      BspRenderVertices = source.BspRenderVertices,
      BspMaterialSpans = source.BspMaterialSpans,
      BspSceneObjects = source.BspSceneObjects,
      BspRenderVertexObjectIds = source.BspRenderVertexObjectIds,
      BspRenderVertexLocalPositions = source.BspRenderVertexLocalPositions,
      Materials = source.Materials,
      MaterialSurfaceIds = source.MaterialSurfaceIds,
      MaterialAlphaTypes = source.MaterialAlphaTypes,
      SurfaceTextures = source.SurfaceTextures,
      LightmapTextures = source.LightmapTextures,
      SkyRenderVertices = source.SkyRenderVertices,
      SkyMaterialSpans = source.SkyMaterialSpans,
      SkyMaterials = source.SkyMaterials,
      SkyMaterialSurfaceIds = source.SkyMaterialSurfaceIds,
      SkyMaterialAlphaTypes = source.SkyMaterialAlphaTypes,
      SkySurfaceTextures = source.SkySurfaceTextures,
      EntityRenderVertices = source.EntityRenderVertices,
      EntityMaterialSpans = source.EntityMaterialSpans,
      EntityMaterials = source.EntityMaterials,
      EntityMaterialSurfaceIds = source.EntityMaterialSurfaceIds,
      EntityMaterialAlphaTypes = source.EntityMaterialAlphaTypes,
      EntitySurfaceTextures = source.EntitySurfaceTextures,
      EntityScene = source.EntityScene,
      MapEntityModelCount = source.MapEntityModelCount,
      MapEntityInstanceCount = source.MapEntityInstanceCount,
      ParticleInstancePositions = source.ParticleInstancePositions,
      ParticleInstances = source.ParticleInstances,
      BspCollisionNormals = source.BspCollisionNormals,
      BspCollisionNodes = source.BspCollisionNodes,
      BspLeafBounds = source.BspLeafBounds,
      CollisionVertices = collisionVertices,
      CollisionLines = collisionLines,
    };
  }

  public static LoadedMap WithEnvironmentData(
    LoadedMap source,
    MapEnvironmentSettings environment)
  {
    return new LoadedMap
    {
      Name = source.Name,
      BspPath = source.BspPath,
      EbpPath = source.EbpPath,
      BspBinaryLayout = source.BspBinaryLayout,
      SkySourceMode = source.SkySourceMode,
      Environment = environment,
      ExtDummies = source.ExtDummies,
      Bounds = source.Bounds,
      BspTriangleVertices = source.BspTriangleVertices,
      BspRenderVertices = source.BspRenderVertices,
      BspMaterialSpans = source.BspMaterialSpans,
      BspSceneObjects = source.BspSceneObjects,
      BspRenderVertexObjectIds = source.BspRenderVertexObjectIds,
      BspRenderVertexLocalPositions = source.BspRenderVertexLocalPositions,
      Materials = source.Materials,
      MaterialSurfaceIds = source.MaterialSurfaceIds,
      MaterialAlphaTypes = source.MaterialAlphaTypes,
      SurfaceTextures = source.SurfaceTextures,
      LightmapTextures = source.LightmapTextures,
      SkyRenderVertices = source.SkyRenderVertices,
      SkyMaterialSpans = source.SkyMaterialSpans,
      SkyMaterials = source.SkyMaterials,
      SkyMaterialSurfaceIds = source.SkyMaterialSurfaceIds,
      SkyMaterialAlphaTypes = source.SkyMaterialAlphaTypes,
      SkySurfaceTextures = source.SkySurfaceTextures,
      EntityRenderVertices = source.EntityRenderVertices,
      EntityMaterialSpans = source.EntityMaterialSpans,
      EntityMaterials = source.EntityMaterials,
      EntityMaterialSurfaceIds = source.EntityMaterialSurfaceIds,
      EntityMaterialAlphaTypes = source.EntityMaterialAlphaTypes,
      EntitySurfaceTextures = source.EntitySurfaceTextures,
      EntityScene = source.EntityScene,
      MapEntityModelCount = source.MapEntityModelCount,
      MapEntityInstanceCount = source.MapEntityInstanceCount,
      ParticleInstancePositions = source.ParticleInstancePositions,
      ParticleInstances = source.ParticleInstances,
      BspCollisionNormals = source.BspCollisionNormals,
      BspCollisionNodes = source.BspCollisionNodes,
      BspLeafBounds = source.BspLeafBounds,
      CollisionVertices = source.CollisionVertices,
      CollisionLines = source.CollisionLines,
    };
  }

  public static LoadedMap WithEntitySceneData(
    LoadedMap source,
    EntitySceneData entityScene)
  {
    EntitySceneData scene = entityScene ?? EntitySceneData.Empty;
    return new LoadedMap
    {
      Name = source.Name,
      BspPath = source.BspPath,
      EbpPath = source.EbpPath,
      BspBinaryLayout = source.BspBinaryLayout,
      SkySourceMode = source.SkySourceMode,
      Environment = source.Environment,
      ExtDummies = source.ExtDummies,
      Bounds = source.Bounds,
      BspTriangleVertices = source.BspTriangleVertices,
      BspRenderVertices = source.BspRenderVertices,
      BspMaterialSpans = source.BspMaterialSpans,
      BspSceneObjects = source.BspSceneObjects,
      BspRenderVertexObjectIds = source.BspRenderVertexObjectIds,
      BspRenderVertexLocalPositions = source.BspRenderVertexLocalPositions,
      Materials = source.Materials,
      MaterialSurfaceIds = source.MaterialSurfaceIds,
      MaterialAlphaTypes = source.MaterialAlphaTypes,
      SurfaceTextures = source.SurfaceTextures,
      LightmapTextures = source.LightmapTextures,
      SkyRenderVertices = source.SkyRenderVertices,
      SkyMaterialSpans = source.SkyMaterialSpans,
      SkyMaterials = source.SkyMaterials,
      SkyMaterialSurfaceIds = source.SkyMaterialSurfaceIds,
      SkyMaterialAlphaTypes = source.SkyMaterialAlphaTypes,
      SkySurfaceTextures = source.SkySurfaceTextures,
      EntityRenderVertices = source.EntityRenderVertices,
      EntityMaterialSpans = source.EntityMaterialSpans,
      EntityMaterials = source.EntityMaterials,
      EntityMaterialSurfaceIds = source.EntityMaterialSurfaceIds,
      EntityMaterialAlphaTypes = source.EntityMaterialAlphaTypes,
      EntitySurfaceTextures = source.EntitySurfaceTextures,
      EntityScene = scene,
      MapEntityModelCount = scene.Models.Length,
      MapEntityInstanceCount = scene.Instances.Length,
      ParticleInstancePositions = source.ParticleInstancePositions,
      ParticleInstances = source.ParticleInstances,
      BspCollisionNormals = source.BspCollisionNormals,
      BspCollisionNodes = source.BspCollisionNodes,
      BspLeafBounds = source.BspLeafBounds,
      CollisionVertices = source.CollisionVertices,
      CollisionLines = source.CollisionLines,
    };
  }

  public static LoadedMap WithMapMaterialData(
    LoadedMap source,
    MaterialDefinition[] materials)
  {
    MaterialDefinition[] resolvedMaterials = materials ?? Array.Empty<MaterialDefinition>();
    int[] surfaceIds = new int[resolvedMaterials.Length];
    uint[] alphaTypes = new uint[resolvedMaterials.Length];
    Array.Fill(surfaceIds, -1);
    for (int materialIndex = 0; materialIndex < resolvedMaterials.Length; ++materialIndex)
    {
      MaterialLayerDefinition[] layers = resolvedMaterials[materialIndex].Layers ?? Array.Empty<MaterialLayerDefinition>();
      if (layers.Length == 0)
      {
        continue;
      }

      surfaceIds[materialIndex] = layers[0].SurfaceId;
      alphaTypes[materialIndex] = layers[0].AlphaType;
    }

    return new LoadedMap
    {
      Name = source.Name,
      BspPath = source.BspPath,
      EbpPath = source.EbpPath,
      BspBinaryLayout = source.BspBinaryLayout,
      SkySourceMode = source.SkySourceMode,
      Environment = source.Environment,
      ExtDummies = source.ExtDummies,
      Bounds = source.Bounds,
      BspTriangleVertices = source.BspTriangleVertices,
      BspRenderVertices = source.BspRenderVertices,
      BspMaterialSpans = source.BspMaterialSpans,
      BspSceneObjects = source.BspSceneObjects,
      BspRenderVertexObjectIds = source.BspRenderVertexObjectIds,
      BspRenderVertexLocalPositions = source.BspRenderVertexLocalPositions,
      Materials = resolvedMaterials,
      MaterialSurfaceIds = surfaceIds,
      MaterialAlphaTypes = alphaTypes,
      SurfaceTextures = source.SurfaceTextures,
      LightmapTextures = source.LightmapTextures,
      SkyRenderVertices = source.SkyRenderVertices,
      SkyMaterialSpans = source.SkyMaterialSpans,
      SkyMaterials = source.SkyMaterials,
      SkyMaterialSurfaceIds = source.SkyMaterialSurfaceIds,
      SkyMaterialAlphaTypes = source.SkyMaterialAlphaTypes,
      SkySurfaceTextures = source.SkySurfaceTextures,
      EntityRenderVertices = source.EntityRenderVertices,
      EntityMaterialSpans = source.EntityMaterialSpans,
      EntityMaterials = source.EntityMaterials,
      EntityMaterialSurfaceIds = source.EntityMaterialSurfaceIds,
      EntityMaterialAlphaTypes = source.EntityMaterialAlphaTypes,
      EntitySurfaceTextures = source.EntitySurfaceTextures,
      EntityScene = source.EntityScene,
      MapEntityModelCount = source.MapEntityModelCount,
      MapEntityInstanceCount = source.MapEntityInstanceCount,
      ParticleInstancePositions = source.ParticleInstancePositions,
      ParticleInstances = source.ParticleInstances,
      BspCollisionNormals = source.BspCollisionNormals,
      BspCollisionNodes = source.BspCollisionNodes,
      BspLeafBounds = source.BspLeafBounds,
      CollisionVertices = source.CollisionVertices,
      CollisionLines = source.CollisionLines,
    };
  }

  public static LoadedMap WithMapTextureData(
    LoadedMap source,
    R3TextureBlob[] surfaceTextures)
  {
    R3TextureBlob[] resolvedTextures = surfaceTextures ?? Array.Empty<R3TextureBlob>();
    return new LoadedMap
    {
      Name = source.Name,
      BspPath = source.BspPath,
      EbpPath = source.EbpPath,
      BspBinaryLayout = source.BspBinaryLayout,
      SkySourceMode = source.SkySourceMode,
      Environment = source.Environment,
      ExtDummies = source.ExtDummies,
      Bounds = source.Bounds,
      BspTriangleVertices = source.BspTriangleVertices,
      BspRenderVertices = source.BspRenderVertices,
      BspMaterialSpans = source.BspMaterialSpans,
      BspSceneObjects = source.BspSceneObjects,
      BspRenderVertexObjectIds = source.BspRenderVertexObjectIds,
      BspRenderVertexLocalPositions = source.BspRenderVertexLocalPositions,
      Materials = source.Materials,
      MaterialSurfaceIds = source.MaterialSurfaceIds,
      MaterialAlphaTypes = source.MaterialAlphaTypes,
      SurfaceTextures = resolvedTextures,
      LightmapTextures = source.LightmapTextures,
      SkyRenderVertices = source.SkyRenderVertices,
      SkyMaterialSpans = source.SkyMaterialSpans,
      SkyMaterials = source.SkyMaterials,
      SkyMaterialSurfaceIds = source.SkyMaterialSurfaceIds,
      SkyMaterialAlphaTypes = source.SkyMaterialAlphaTypes,
      SkySurfaceTextures = source.SkySurfaceTextures,
      EntityRenderVertices = source.EntityRenderVertices,
      EntityMaterialSpans = source.EntityMaterialSpans,
      EntityMaterials = source.EntityMaterials,
      EntityMaterialSurfaceIds = source.EntityMaterialSurfaceIds,
      EntityMaterialAlphaTypes = source.EntityMaterialAlphaTypes,
      EntitySurfaceTextures = source.EntitySurfaceTextures,
      EntityScene = source.EntityScene,
      MapEntityModelCount = source.MapEntityModelCount,
      MapEntityInstanceCount = source.MapEntityInstanceCount,
      ParticleInstancePositions = source.ParticleInstancePositions,
      ParticleInstances = source.ParticleInstances,
      BspCollisionNormals = source.BspCollisionNormals,
      BspCollisionNodes = source.BspCollisionNodes,
      BspLeafBounds = source.BspLeafBounds,
      CollisionVertices = source.CollisionVertices,
      CollisionLines = source.CollisionLines,
    };
  }

  public static LoadedMap WithBspMeshData(
    LoadedMap source,
    BspBinaryLayout bspBinaryLayout,
    BspRenderVertex[] bspRenderVertices,
    BspMaterialSpan[] bspMaterialSpans,
    EntitySceneObject[]? bspSceneObjects = null,
    ushort[]? bspRenderVertexObjectIds = null,
    Vector3[]? bspRenderVertexLocalPositions = null,
    Vector3[]? bspCollisionNormals = null,
    BspNode[]? bspCollisionNodes = null,
    BspLeafBounds[]? bspLeafBounds = null)
  {
    BspBinaryLayout layout = bspBinaryLayout ?? source.BspBinaryLayout;
    BspRenderVertex[] renderVertices = bspRenderVertices ?? Array.Empty<BspRenderVertex>();
    BspMaterialSpan[] materialSpans = bspMaterialSpans ?? Array.Empty<BspMaterialSpan>();
    EntitySceneObject[] sceneObjects = bspSceneObjects ?? source.BspSceneObjects;

    Vector3[] triangleVertices = new Vector3[renderVertices.Length];
    for (int i = 0; i < renderVertices.Length; ++i)
    {
      triangleVertices[i] = renderVertices[i].Position;
    }

    bool hasProvidedMetadata =
      bspRenderVertexObjectIds != null &&
      bspRenderVertexLocalPositions != null &&
      bspRenderVertexObjectIds.Length == renderVertices.Length &&
      bspRenderVertexLocalPositions.Length == renderVertices.Length;
    bool canReuseSourceMetadata =
      ReferenceEquals(renderVertices, source.BspRenderVertices) &&
      source.BspRenderVertexObjectIds.Length == renderVertices.Length &&
      source.BspRenderVertexLocalPositions.Length == renderVertices.Length;
    ushort[] renderVertexObjectIds;
    Vector3[] renderVertexLocalPositions;
    if (hasProvidedMetadata)
    {
      renderVertexObjectIds = bspRenderVertexObjectIds!;
      renderVertexLocalPositions = bspRenderVertexLocalPositions!;
    }
    else if (canReuseSourceMetadata)
    {
      renderVertexObjectIds = source.BspRenderVertexObjectIds;
      renderVertexLocalPositions = source.BspRenderVertexLocalPositions;
    }
    else
    {
      renderVertexObjectIds = new ushort[renderVertices.Length];
      renderVertexLocalPositions = new Vector3[renderVertices.Length];
      for (int i = 0; i < renderVertices.Length; ++i)
      {
        renderVertexLocalPositions[i] = renderVertices[i].Position;
      }
    }

    MapBounds bounds = ComputeBoundsFromVertices(triangleVertices, source.CollisionVertices, source.Bounds);
    Vector3[] collisionNormals = bspCollisionNormals ?? source.BspCollisionNormals;
    BspNode[] collisionNodes = bspCollisionNodes ?? source.BspCollisionNodes;
    BspLeafBounds[] leafBounds = bspLeafBounds ?? source.BspLeafBounds;

    return new LoadedMap
    {
      Name = source.Name,
      BspPath = source.BspPath,
      EbpPath = source.EbpPath,
      BspBinaryLayout = layout,
      SkySourceMode = source.SkySourceMode,
      Environment = source.Environment,
      ExtDummies = source.ExtDummies,
      Bounds = bounds,
      BspTriangleVertices = triangleVertices,
      BspRenderVertices = renderVertices,
      BspMaterialSpans = materialSpans,
      BspSceneObjects = sceneObjects,
      BspRenderVertexObjectIds = renderVertexObjectIds,
      BspRenderVertexLocalPositions = renderVertexLocalPositions,
      Materials = source.Materials,
      MaterialSurfaceIds = source.MaterialSurfaceIds,
      MaterialAlphaTypes = source.MaterialAlphaTypes,
      SurfaceTextures = source.SurfaceTextures,
      LightmapTextures = source.LightmapTextures,
      SkyRenderVertices = source.SkyRenderVertices,
      SkyMaterialSpans = source.SkyMaterialSpans,
      SkyMaterials = source.SkyMaterials,
      SkyMaterialSurfaceIds = source.SkyMaterialSurfaceIds,
      SkyMaterialAlphaTypes = source.SkyMaterialAlphaTypes,
      SkySurfaceTextures = source.SkySurfaceTextures,
      EntityRenderVertices = source.EntityRenderVertices,
      EntityMaterialSpans = source.EntityMaterialSpans,
      EntityMaterials = source.EntityMaterials,
      EntityMaterialSurfaceIds = source.EntityMaterialSurfaceIds,
      EntityMaterialAlphaTypes = source.EntityMaterialAlphaTypes,
      EntitySurfaceTextures = source.EntitySurfaceTextures,
      EntityScene = source.EntityScene,
      MapEntityModelCount = source.MapEntityModelCount,
      MapEntityInstanceCount = source.MapEntityInstanceCount,
      ParticleInstancePositions = source.ParticleInstancePositions,
      ParticleInstances = source.ParticleInstances,
      BspCollisionNormals = collisionNormals,
      BspCollisionNodes = collisionNodes,
      BspLeafBounds = leafBounds,
      CollisionVertices = source.CollisionVertices,
      CollisionLines = source.CollisionLines,
    };
  }

  private static MapBounds ComputeBoundsFromVertices(Vector3[] primary, Vector3[] fallback, MapBounds defaultBounds)
  {
    if (TryComputeBounds(primary, out MapBounds primaryBounds))
    {
      return primaryBounds;
    }

    if (TryComputeBounds(fallback, out MapBounds fallbackBounds))
    {
      return fallbackBounds;
    }

    return defaultBounds;
  }

  private static bool TryComputeBounds(Vector3[] vertices, out MapBounds bounds)
  {
    bounds = default;
    if (vertices == null || vertices.Length == 0)
    {
      return false;
    }

    bool found = false;
    Vector3 min = Vector3.Zero;
    Vector3 max = Vector3.Zero;
    for (int i = 0; i < vertices.Length; ++i)
    {
      Vector3 v = vertices[i];
      if (!IsFinite(v))
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
      return false;
    }

    bounds = new MapBounds(min, max);
    return true;
  }

  private static float ResolveBoundaryBaseY(LoadedMap source)
  {
    float minY = float.PositiveInfinity;
    for (int i = 0; i < source.CollisionVertices.Length; ++i)
    {
      float y = source.CollisionVertices[i].Y;
      if (float.IsFinite(y) && y < minY)
      {
        minY = y;
      }
    }

    if (float.IsFinite(minY))
    {
      return minY;
    }

    return float.IsFinite(source.Bounds.Min.Y) ? source.Bounds.Min.Y : 0.0f;
  }

  private static float ResolveGroundYNear(LoadedMap source, Vector3 sampleSourcePosition, float fallbackY)
  {
    float bestDistanceSq = float.PositiveInfinity;
    float bestY = fallbackY;

    for (int i = 0; i < source.BspRenderVertices.Length; ++i)
    {
      Vector3 position = source.BspRenderVertices[i].Position;
      if (!IsFinite(position))
      {
        continue;
      }

      float dx = position.X - sampleSourcePosition.X;
      float dz = position.Z - sampleSourcePosition.Z;
      float distanceSq = dx * dx + dz * dz;
      if (!float.IsFinite(distanceSq) || distanceSq >= bestDistanceSq)
      {
        continue;
      }

      bestDistanceSq = distanceSq;
      bestY = position.Y;
    }

    if (!float.IsFinite(bestY))
    {
      return 0.0f;
    }

    return bestY;
  }

  private static Vector3 SnapPointToNearestVertex(
    Vector3[] vertices,
    Vector3 point,
    float maxDistance,
    int excludedIndex,
    out bool snapped,
    out int snappedIndex)
  {
    snapped = false;
    snappedIndex = -1;
    if (vertices.Length == 0 || maxDistance <= 0.0f)
    {
      return point;
    }

    float maxDistanceSq = maxDistance * maxDistance;
    float bestHorizontalDistanceSq = maxDistanceSq;
    float bestVerticalDistance = float.PositiveInfinity;
    int bestIndex = -1;
    for (int index = 0; index < vertices.Length; ++index)
    {
      if (index == excludedIndex)
      {
        continue;
      }

      Vector3 candidate = vertices[index];
      if (!IsFinite(candidate))
      {
        continue;
      }

      float dx = candidate.X - point.X;
      float dz = candidate.Z - point.Z;
      float horizontalDistanceSq = dx * dx + dz * dz;
      if (!float.IsFinite(horizontalDistanceSq) || horizontalDistanceSq > bestHorizontalDistanceSq)
      {
        continue;
      }

      float verticalDistance = MathF.Abs(candidate.Y - point.Y);
      bool betterHorizontal = horizontalDistanceSq < bestHorizontalDistanceSq - 0.0001f;
      bool equalHorizontalBetterVertical =
        MathF.Abs(horizontalDistanceSq - bestHorizontalDistanceSq) <= 0.0001f &&
        verticalDistance < bestVerticalDistance;
      if (!betterHorizontal && !equalHorizontalBetterVertical)
      {
        continue;
      }

      bestHorizontalDistanceSq = horizontalDistanceSq;
      bestVerticalDistance = verticalDistance;
      bestIndex = index;
    }

    if (bestIndex >= 0)
    {
      snapped = true;
      snappedIndex = bestIndex;
      return vertices[bestIndex];
    }

    return point;
  }

  private static bool IsFinite(Vector3 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y) && float.IsFinite(value.Z);
  }

  private static bool EnsureLineHasPrivateVertices(
    List<Vector3> vertices,
    CollisionLine[] lines,
    int lineIndex,
    out CollisionLine editableLine,
    out string error)
  {
    error = string.Empty;
    editableLine = lines[lineIndex];
    if (editableLine.StartVertex >= vertices.Count || editableLine.EndVertex >= vertices.Count)
    {
      error = $"Collision line {lineIndex} references out-of-range vertices.";
      return false;
    }

    ushort startVertex = editableLine.StartVertex;
    if (CountVertexUsage(lines, startVertex) > 1)
    {
      if (vertices.Count >= ushort.MaxValue)
      {
        error = $"Collision vertex count would exceed {ushort.MaxValue}.";
        return false;
      }

      vertices.Add(vertices[startVertex]);
      startVertex = (ushort)(vertices.Count - 1);
    }

    ushort endVertex = editableLine.EndVertex;
    if (CountVertexUsage(lines, endVertex) > 1)
    {
      if (vertices.Count >= ushort.MaxValue)
      {
        error = $"Collision vertex count would exceed {ushort.MaxValue}.";
        return false;
      }

      vertices.Add(vertices[endVertex]);
      endVertex = (ushort)(vertices.Count - 1);
    }

    if (startVertex != editableLine.StartVertex || endVertex != editableLine.EndVertex)
    {
      editableLine = new CollisionLine(
        editableLine.Attribute,
        startVertex,
        endVertex,
        editableLine.Height,
        editableLine.FrontLeaf,
        editableLine.BackLeaf);
      lines[lineIndex] = editableLine;
    }

    return true;
  }

  private static int CountVertexUsage(CollisionLine[] lines, ushort vertexIndex)
  {
    int usage = 0;
    for (int i = 0; i < lines.Length; ++i)
    {
      CollisionLine line = lines[i];
      if (line.StartVertex == vertexIndex)
      {
        ++usage;
      }

      if (line.EndVertex == vertexIndex)
      {
        ++usage;
      }
    }

    return usage;
  }

  private static bool TryGetFirstCollisionLineAtVertex(CollisionLine[] lines, int vertexIndex, out CollisionLine line)
  {
    line = default;
    if (vertexIndex < 0 || vertexIndex > ushort.MaxValue)
    {
      return false;
    }

    ushort targetVertex = (ushort)vertexIndex;
    for (int i = 0; i < lines.Length; ++i)
    {
      CollisionLine candidate = lines[i];
      if (candidate.StartVertex == targetVertex || candidate.EndVertex == targetVertex)
      {
        line = candidate;
        return true;
      }
    }

    return false;
  }
}
