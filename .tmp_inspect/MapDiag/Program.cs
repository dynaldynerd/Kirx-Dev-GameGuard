using MapEditor.Formats;
using System.Numerics;

if (args.Length < 2)
{
  Console.WriteLine("Usage: MapDiag <path-to-bsp> <path-to-ebp>");
  return;
}

string bsp = Path.GetFullPath(args[0]);
string ebp = Path.GetFullPath(args[1]);
LoadedMap map = BspLoader.Load(bsp, ebp, SkySourceMode.Sky2);

Console.WriteLine($"Map={map.Name}");
Console.WriteLine($"EntModels={map.MapEntityModelCount} EntInst={map.MapEntityInstanceCount} FxInst={map.ParticleInstancePositions.Length}");
Console.WriteLine($"EntVerts={map.EntityRenderVertices.Length} EntSpans={map.EntityMaterialSpans.Length} EntTex={map.EntitySurfaceTextures.Length}");
Console.WriteLine($"EntMaterials={map.EntityMaterials.Length} EntMatSurfaceIds={map.EntityMaterialSurfaceIds.Length}");
Console.WriteLine($"SceneModels={map.EntityScene.Models.Length} SceneInst={map.EntityScene.Instances.Length}");

int fallbackSpanCount = map.EntityMaterialSpans.Count(span => span.MaterialId < 0 || span.MaterialId >= map.EntityMaterials.Length);
Console.WriteLine($"FallbackSpans={fallbackSpanCount}/{map.EntityMaterialSpans.Length}");

string[] names = map.EntitySurfaceTextures
  .Select(t => t.Name ?? string.Empty)
  .Distinct(StringComparer.OrdinalIgnoreCase)
  .OrderBy(n => n, StringComparer.OrdinalIgnoreCase)
  .ToArray();

string[] interesting = names
  .Where(n =>
    n.Contains("potal", StringComparison.OrdinalIgnoreCase)
    || n.Contains("wave", StringComparison.OrdinalIgnoreCase)
    || n.Contains("sand", StringComparison.OrdinalIgnoreCase)
    || n.Contains("line_light", StringComparison.OrdinalIgnoreCase)
    || n.Contains("cir_light", StringComparison.OrdinalIgnoreCase)
    || n.Contains("white_light", StringComparison.OrdinalIgnoreCase)
    || n.Contains("fog", StringComparison.OrdinalIgnoreCase))
  .ToArray();

Console.WriteLine($"InterestingTextureNames={interesting.Length}");
foreach (string name in interesting.Take(200))
{
  Console.WriteLine(name);
}

var textures = map.EntitySurfaceTextures
  .GroupBy(t => t.SurfaceId)
  .ToDictionary(g => g.Key, g => g.First().Name ?? string.Empty);

Console.WriteLine();
Console.WriteLine("Interesting material alpha types:");
for (int materialId = 0; materialId < map.EntityMaterialSurfaceIds.Length; materialId++)
{
  int surfaceId = map.EntityMaterialSurfaceIds[materialId];
  if (surfaceId <= 0 || !textures.TryGetValue(surfaceId, out string texName))
  {
    continue;
  }

  if (!texName.Contains("potal", StringComparison.OrdinalIgnoreCase)
    && !texName.Contains("wave", StringComparison.OrdinalIgnoreCase)
    && !texName.Contains("sand", StringComparison.OrdinalIgnoreCase)
    && !texName.Contains("line_light", StringComparison.OrdinalIgnoreCase)
    && !texName.Contains("cir_light", StringComparison.OrdinalIgnoreCase)
    && !texName.Contains("white_light", StringComparison.OrdinalIgnoreCase)
    && !texName.Contains("fog", StringComparison.OrdinalIgnoreCase))
  {
    continue;
  }

  uint alpha = materialId < map.EntityMaterialAlphaTypes.Length ? map.EntityMaterialAlphaTypes[materialId] : 0u;
  bool hasMaterial = materialId < map.EntityMaterials.Length;
  if (!hasMaterial)
  {
    Console.WriteLine($"mat={materialId} surface={surfaceId} alpha=0x{alpha:X8} tex={texName} hasMat=0");
    continue;
  }

  MaterialDefinition mat = map.EntityMaterials[materialId];
  if (mat.Layers.Length == 0)
  {
    Console.WriteLine($"mat={materialId} surface={surfaceId} alpha=0x{alpha:X8} tex={texName} hasMat=1 layers=0");
    continue;
  }

  MaterialLayerDefinition l0 = mat.Layers[0];
  Console.WriteLine(
    $"mat={materialId} surface={surfaceId} alpha=0x{alpha:X8} tex={texName} hasMat=1 layers={mat.Layers.Length} l0.alpha=0x{l0.AlphaType:X8} l0.argb=0x{l0.Argb:X8} l0.flags=0x{l0.LayerFlags:X8}");
}

Console.WriteLine();
Console.WriteLine("Interesting scene models:");
foreach (EntitySceneModel model in map.EntityScene.Models
  .Where(m => m.Name.Contains("potal", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("portal", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("wave", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("line", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("light", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("fog", StringComparison.OrdinalIgnoreCase))
  .OrderBy(m => m.EntityId))
{
  bool hasAnim = model.Objects.Any(o => o.PositionTracks.Length > 0 || o.RotationTracks.Length > 0 || o.ScaleTracks.Length > 0);
  int instCount = map.EntityScene.Instances.Count(i => i.EntityId == model.EntityId);
  Console.WriteLine($"id={model.EntityId} inst={instCount} name={model.Name} mats={model.MatGroups.Length} objs={model.Objects.Length} anim={hasAnim}");
  if (hasAnim)
  {
    foreach (EntitySceneObject obj in model.Objects.Take(8))
    {
      if (obj.PositionTracks.Length == 0 && obj.RotationTracks.Length == 0 && obj.ScaleTracks.Length == 0)
      {
        continue;
      }

      Console.WriteLine($"  obj parent={obj.Parent} frames={obj.Frames} posT={obj.PositionTracks.Length} rotT={obj.RotationTracks.Length} sclT={obj.ScaleTracks.Length}");
    }
  }
}

Console.WriteLine();
Console.WriteLine("All animated scene models:");
foreach (EntitySceneModel model in map.EntityScene.Models.OrderBy(m => m.EntityId))
{
  bool hasAnim = model.Objects.Any(o => o.PositionTracks.Length > 0 || o.RotationTracks.Length > 0 || o.ScaleTracks.Length > 0);
  if (!hasAnim)
  {
    continue;
  }

  int instCount = map.EntityScene.Instances.Count(i => i.EntityId == model.EntityId);
  Console.WriteLine($"id={model.EntityId} inst={instCount} name={model.Name} mats={model.MatGroups.Length} objs={model.Objects.Length}");
}

Console.WriteLine();
Console.WriteLine("Portal candidate material groups:");
foreach (EntitySceneModel model in map.EntityScene.Models
  .Where(m => m.Name.Contains("gate", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("circlezone", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("potal", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("portal", StringComparison.OrdinalIgnoreCase))
  .OrderBy(m => m.EntityId))
{
  int instCount = map.EntityScene.Instances.Count(i => i.EntityId == model.EntityId);
  Console.WriteLine($"model id={model.EntityId} name={model.Name} inst={instCount} matGroups={model.MatGroups.Length} matBase={model.MaterialBase}");
  for (int oi = 0; oi < model.Objects.Length; ++oi)
  {
    EntitySceneObject o = model.Objects[oi];
    Console.WriteLine(
      $"  objIndex={oi} parent={o.Parent} frames={o.Frames} pos=({o.Position.X:F2},{o.Position.Y:F2},{o.Position.Z:F2}) " +
      $"scale=({o.Scale.X:F3},{o.Scale.Y:F3},{o.Scale.Z:F3}) " +
      $"rotQ=({o.Quaternion.X:F3},{o.Quaternion.Y:F3},{o.Quaternion.Z:F3},{o.Quaternion.W:F3}) " +
      $"posT={o.PositionTracks.Length} rotT={o.RotationTracks.Length} sclT={o.ScaleTracks.Length}");
    if (o.RotationTracks.Length > 1)
    {
      EntitySceneRotationTrack r0 = o.RotationTracks[0];
      EntitySceneRotationTrack r1 = o.RotationTracks[^1];
      Console.WriteLine($"    rot0 frame={r0.Frame:F2} q=({r0.Quaternion.X:F3},{r0.Quaternion.Y:F3},{r0.Quaternion.Z:F3},{r0.Quaternion.W:F3})");
      Console.WriteLine($"    rotN frame={r1.Frame:F2} q=({r1.Quaternion.X:F3},{r1.Quaternion.Y:F3},{r1.Quaternion.Z:F3},{r1.Quaternion.W:F3})");
      int[] sampleIds =
      [
        0,
        o.RotationTracks.Length / 4,
        o.RotationTracks.Length / 2,
        (o.RotationTracks.Length * 3) / 4,
        o.RotationTracks.Length - 1
      ];
      foreach (int sampleId in sampleIds.Distinct())
      {
        EntitySceneRotationTrack rs = o.RotationTracks[sampleId];
        Console.WriteLine($"    rotS frame={rs.Frame:F2} q=({rs.Quaternion.X:F3},{rs.Quaternion.Y:F3},{rs.Quaternion.Z:F3},{rs.Quaternion.W:F3})");
      }
    }
    if (o.PositionTracks.Length > 1)
    {
      EntityScenePositionTrack p0 = o.PositionTracks[0];
      EntityScenePositionTrack p1 = o.PositionTracks[^1];
      float minPx = float.MaxValue, minPy = float.MaxValue, minPz = float.MaxValue;
      float maxPx = float.MinValue, maxPy = float.MinValue, maxPz = float.MinValue;
      foreach (EntityScenePositionTrack pt in o.PositionTracks)
      {
        minPx = Math.Min(minPx, pt.Position.X);
        minPy = Math.Min(minPy, pt.Position.Y);
        minPz = Math.Min(minPz, pt.Position.Z);
        maxPx = Math.Max(maxPx, pt.Position.X);
        maxPy = Math.Max(maxPy, pt.Position.Y);
        maxPz = Math.Max(maxPz, pt.Position.Z);
      }

      Console.WriteLine($"    pos0 frame={p0.Frame:F2} p=({p0.Position.X:F3},{p0.Position.Y:F3},{p0.Position.Z:F3})");
      Console.WriteLine($"    posN frame={p1.Frame:F2} p=({p1.Position.X:F3},{p1.Position.Y:F3},{p1.Position.Z:F3})");
      Console.WriteLine($"    posMin=({minPx:F3},{minPy:F3},{minPz:F3}) posMax=({maxPx:F3},{maxPy:F3},{maxPz:F3})");
    }
    if (o.ScaleTracks.Length > 1)
    {
      EntitySceneScaleTrack s0 = o.ScaleTracks[0];
      EntitySceneScaleTrack s1 = o.ScaleTracks[^1];
      Console.WriteLine($"    scl0 frame={s0.Frame:F2} s=({s0.Scale.X:F3},{s0.Scale.Y:F3},{s0.Scale.Z:F3})");
      Console.WriteLine($"    sclN frame={s1.Frame:F2} s=({s1.Scale.X:F3},{s1.Scale.Y:F3},{s1.Scale.Z:F3})");
    }
  }

  for (int gi = 0; gi < model.MatGroups.Length; ++gi)
  {
    EntitySceneMatGroup g = model.MatGroups[gi];
    int materialId = g.MaterialId >= 0 ? g.MaterialId + model.MaterialBase : g.MaterialId;
    string tex = "n/a";
    uint alpha = 0;
    uint l0Alpha = 0;
    uint l0Argb = 0;
    uint l0Flags = 0;
    if (materialId >= 0 && materialId < map.EntityMaterialSurfaceIds.Length)
    {
      int sid = map.EntityMaterialSurfaceIds[materialId];
      if (textures.TryGetValue(sid, out string t))
      {
        tex = t;
      }

      if (materialId < map.EntityMaterialAlphaTypes.Length)
      {
        alpha = map.EntityMaterialAlphaTypes[materialId];
      }

      if (materialId < map.EntityMaterials.Length && map.EntityMaterials[materialId].Layers.Length > 0)
      {
        MaterialLayerDefinition l0 = map.EntityMaterials[materialId].Layers[0];
        l0Alpha = l0.AlphaType;
        l0Argb = l0.Argb;
        l0Flags = l0.LayerFlags;
      }
    }

    Console.WriteLine(
      $"  gi={gi} obj={g.ObjectId} mat={materialId} alpha=0x{alpha:X8} l0.alpha=0x{l0Alpha:X8} l0.argb=0x{l0Argb:X8} l0.flags=0x{l0Flags:X8} tex={tex} verts={g.LocalVertices.Length}");
    if (g.LocalVertices.Length > 0)
    {
      float minR = float.MaxValue, minG = float.MaxValue, minB = float.MaxValue;
      float maxR = float.MinValue, maxG = float.MinValue, maxB = float.MinValue;
      float minX = float.MaxValue, minY = float.MaxValue, minZ = float.MaxValue;
      float maxX = float.MinValue, maxY = float.MinValue, maxZ = float.MinValue;
      foreach (BspRenderVertex v in g.LocalVertices)
      {
        minR = Math.Min(minR, v.Color.X);
        minG = Math.Min(minG, v.Color.Y);
        minB = Math.Min(minB, v.Color.Z);
        maxR = Math.Max(maxR, v.Color.X);
        maxG = Math.Max(maxG, v.Color.Y);
        maxB = Math.Max(maxB, v.Color.Z);
        minX = Math.Min(minX, v.Position.X);
        minY = Math.Min(minY, v.Position.Y);
        minZ = Math.Min(minZ, v.Position.Z);
        maxX = Math.Max(maxX, v.Position.X);
        maxY = Math.Max(maxY, v.Position.Y);
        maxZ = Math.Max(maxZ, v.Position.Z);
      }

      Console.WriteLine($"    color min=({minR:F3},{minG:F3},{minB:F3}) max=({maxR:F3},{maxG:F3},{maxB:F3})");
      Console.WriteLine($"    pos min=({minX:F3},{minY:F3},{minZ:F3}) max=({maxX:F3},{maxY:F3},{maxZ:F3})");
    }
  }
}

Console.WriteLine();
Console.WriteLine("Portal candidate instances:");
HashSet<int> portalIds = map.EntityScene.Models
  .Where(m => m.Name.Contains("gate", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("circlezone", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("potal", StringComparison.OrdinalIgnoreCase)
    || m.Name.Contains("portal", StringComparison.OrdinalIgnoreCase))
  .Select(m => m.EntityId)
  .ToHashSet();

foreach (EntitySceneInstance inst in map.EntityScene.Instances.Where(i => portalIds.Contains(i.EntityId)).Take(200))
{
  string modelName = map.EntityScene.Models.FirstOrDefault(m => m.EntityId == inst.EntityId).Name;
  Console.WriteLine(
    $"  id={inst.EntityId} name={modelName} pos=({inst.Position.X:F3},{inst.Position.Y:F3},{inst.Position.Z:F3}) " +
    $"scale={inst.Scale:F4} rotX={inst.RotX:F3} rotY={inst.RotY:F3}");
}

Console.WriteLine();
Console.WriteLine("Entities near b_gate portal:");
EntitySceneInstance? bGateInst = map.EntityScene.Instances.FirstOrDefault(i => i.EntityId == 62);
if (bGateInst.HasValue)
{
  var center = bGateInst.Value.Position;
  foreach (EntitySceneInstance inst in map.EntityScene.Instances)
  {
    var d = inst.Position - center;
    float distSq = d.LengthSquared;
    if (distSq > 250.0f * 250.0f)
    {
      continue;
    }

    EntitySceneModel model = map.EntityScene.Models.First(m => m.EntityId == inst.EntityId);
    Console.WriteLine(
      $"  id={inst.EntityId} name={model.Name} dist={MathF.Sqrt(distSq):F2} " +
      $"mats={model.MatGroups.Length} objs={model.Objects.Length} pos=({inst.Position.X:F2},{inst.Position.Y:F2},{inst.Position.Z:F2})");
  }
}

static Matrix4x4 LegacyMatrixMultiply(in Matrix4x4 a, in Matrix4x4 b)
{
  return Matrix4x4.Multiply(b, a);
}

static Matrix4x4 CreateLegacyQuaternionMatrix(Vector4 quaternion)
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

  return new Matrix4x4(
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

static Matrix4x4 BuildLegacyScaleMatrix(OpenTK.Mathematics.Vector3 scale, Vector4 scaleQuaternion)
{
  Matrix4x4 scaleMatrix = Matrix4x4.Identity;
  scaleMatrix.M11 = scale.X;
  scaleMatrix.M22 = scale.Y;
  scaleMatrix.M33 = scale.Z;

  Matrix4x4 sqMatrix = CreateLegacyQuaternionMatrix(scaleQuaternion);
  if (!Matrix4x4.Invert(sqMatrix, out Matrix4x4 invSqMatrix))
  {
    return scaleMatrix;
  }

  Matrix4x4 temp = LegacyMatrixMultiply(scaleMatrix, invSqMatrix);
  return LegacyMatrixMultiply(sqMatrix, temp);
}

static Matrix4x4 ConvertFrom3dsMaxMatrix(Matrix4x4 matrix)
{
  (matrix.M12, matrix.M13) = (matrix.M13, matrix.M12);
  (matrix.M22, matrix.M23) = (matrix.M23, matrix.M22);
  (matrix.M32, matrix.M33) = (matrix.M33, matrix.M32);
  (matrix.M42, matrix.M43) = (matrix.M43, matrix.M42);
  return matrix;
}

static void GetTrackFrameIndices(
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

static OpenTK.Mathematics.Vector3 SamplePositionTrack(EntityScenePositionTrack[] tracks, float nowFrame)
{
  if (tracks.Length == 0)
  {
    return OpenTK.Mathematics.Vector3.Zero;
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

  return OpenTK.Mathematics.Vector3.Lerp(tracks[root].Position, tracks[next].Position, alpha);
}

static Vector4 SampleRotationTrack(EntitySceneRotationTrack[] tracks, float nowFrame)
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
  Quaternion q0 = Quaternion.Normalize(new Quaternion(rootQ.X, rootQ.Y, rootQ.Z, rootQ.W));
  Quaternion q1 = Quaternion.Normalize(new Quaternion(nextQ.X, nextQ.Y, nextQ.Z, nextQ.W));
  Quaternion q = Quaternion.Normalize(Quaternion.Slerp(q0, q1, alpha));
  if (!float.IsFinite(q.X) || !float.IsFinite(q.Y) || !float.IsFinite(q.Z) || !float.IsFinite(q.W))
  {
    return rootQ;
  }

  return new Vector4(q.X, q.Y, q.Z, q.W);
}

static void SampleScaleTrack(EntitySceneScaleTrack[] tracks, float nowFrame, out OpenTK.Mathematics.Vector3 scale, out Vector4 scaleQuaternion)
{
  scale = OpenTK.Mathematics.Vector3.One;
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

  scale = OpenTK.Mathematics.Vector3.Lerp(tracks[root].Scale, tracks[next].Scale, alpha);

  Vector4 rootQ = tracks[root].ScaleQuaternion;
  Vector4 nextQ = tracks[next].ScaleQuaternion;
  Quaternion q0 = Quaternion.Normalize(new Quaternion(rootQ.X, rootQ.Y, rootQ.Z, rootQ.W));
  Quaternion q1 = Quaternion.Normalize(new Quaternion(nextQ.X, nextQ.Y, nextQ.Z, nextQ.W));
  Quaternion q = Quaternion.Normalize(Quaternion.Slerp(q0, q1, alpha));
  if (!float.IsFinite(q.X) || !float.IsFinite(q.Y) || !float.IsFinite(q.Z) || !float.IsFinite(q.W))
  {
    scaleQuaternion = rootQ;
    return;
  }

  scaleQuaternion = new Vector4(q.X, q.Y, q.Z, q.W);
}

static Matrix4x4 BuildSceneObjectLocalMatrix(EntitySceneObject obj, float nowFrame)
{
  bool hasTracks = obj.PositionTracks.Length > 0 || obj.RotationTracks.Length > 0 || obj.ScaleTracks.Length > 0;
  if (!hasTracks)
  {
    Matrix4x4 baseRotation = CreateLegacyQuaternionMatrix(obj.Quaternion);
    Matrix4x4 baseScaleMatrix = BuildLegacyScaleMatrix(obj.Scale, obj.ScaleQuaternion);
    Matrix4x4 baseLocal = LegacyMatrixMultiply(baseRotation, baseScaleMatrix);
    baseLocal.M41 = obj.Position.X;
    baseLocal.M42 = obj.Position.Y;
    baseLocal.M43 = obj.Position.Z;
    baseLocal.M44 = 1.0f;
    return baseLocal;
  }

  Vector4 rotationQuat = obj.Quaternion;
  OpenTK.Mathematics.Vector3 scale = obj.Scale;
  Vector4 scaleQuaternion = obj.ScaleQuaternion;
  OpenTK.Mathematics.Vector3 position = obj.Position;

  if (obj.RotationTracks.Length > 0)
  {
    rotationQuat = SampleRotationTrack(obj.RotationTracks, nowFrame);
  }

  if (obj.ScaleTracks.Length > 0)
  {
    SampleScaleTrack(obj.ScaleTracks, nowFrame, out scale, out scaleQuaternion);
  }

  if (obj.PositionTracks.Length > 0)
  {
    position = SamplePositionTrack(obj.PositionTracks, nowFrame);
  }

  Matrix4x4 rotation = CreateLegacyQuaternionMatrix(rotationQuat);
  Matrix4x4 scaleMatrix = BuildLegacyScaleMatrix(scale, scaleQuaternion);
  Matrix4x4 local = LegacyMatrixMultiply(rotation, scaleMatrix);
  local.M41 = position.X;
  local.M42 = position.Y;
  local.M43 = position.Z;
  local.M44 = 1.0f;
  return local;
}

static Matrix4x4 ResolveSceneObjectMatrix(
  int objectIndex,
  EntitySceneObject[] objects,
  float nowFrame,
  Matrix4x4[] cache,
  byte[] visitState)
{
  byte state = visitState[objectIndex];
  if (state == 2)
  {
    return cache[objectIndex];
  }

  if (state == 1)
  {
    return Matrix4x4.Identity;
  }

  visitState[objectIndex] = 1;
  EntitySceneObject obj = objects[objectIndex];
  Matrix4x4 parentMatrix = Matrix4x4.Identity;
  int parentIndex = obj.Parent - 1;
  if (parentIndex >= 0 && parentIndex < objects.Length && parentIndex != objectIndex)
  {
    parentMatrix = ResolveSceneObjectMatrix(parentIndex, objects, nowFrame, cache, visitState);
  }

  float objectNowFrame = nowFrame;
  if (obj.Frames > 0)
  {
    objectNowFrame %= obj.Frames;
    if (objectNowFrame < 0.0f)
    {
      objectNowFrame += obj.Frames;
    }
  }

  Matrix4x4 localMatrix = BuildSceneObjectLocalMatrix(obj, objectNowFrame);
  Matrix4x4 worldMatrix = LegacyMatrixMultiply(parentMatrix, localMatrix);
  cache[objectIndex] = worldMatrix;
  visitState[objectIndex] = 2;
  return worldMatrix;
}

static Matrix4x4[] BuildSceneObjectMatrices(EntitySceneObject[] objects, float nowFrame)
{
  if (objects.Length == 0)
  {
    return Array.Empty<Matrix4x4>();
  }

  Matrix4x4[] legacyWorldMatrices = new Matrix4x4[objects.Length];
  byte[] visitState = new byte[objects.Length];
  for (int objectIndex = 0; objectIndex < objects.Length; ++objectIndex)
  {
    _ = ResolveSceneObjectMatrix(objectIndex, objects, nowFrame, legacyWorldMatrices, visitState);
  }

  Matrix4x4[] convertedMatrices = new Matrix4x4[objects.Length];
  for (int index = 0; index < convertedMatrices.Length; ++index)
  {
    convertedMatrices[index] = ConvertFrom3dsMaxMatrix(legacyWorldMatrices[index]);
  }

  return convertedMatrices;
}

static bool TryGetSceneObjectMatrix(Matrix4x4[] objectMatrices, ushort objectId, out Matrix4x4 matrix)
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

  matrix = Matrix4x4.Identity;
  return false;
}

static OpenTK.Mathematics.Vector3 TransformEntityPosition(OpenTK.Mathematics.Vector3 position, in Matrix4x4 matrix)
{
  System.Numerics.Vector3 transformed = System.Numerics.Vector3.Transform(new System.Numerics.Vector3(position.X, position.Y, position.Z), matrix);
  return new OpenTK.Mathematics.Vector3(transformed.X, transformed.Y, transformed.Z);
}

static OpenTK.Mathematics.Vector3 TransformEntityInstancePosition(OpenTK.Mathematics.Vector3 local, float scale, float rotXDeg, float rotYDeg, OpenTK.Mathematics.Vector3 translation)
{
  float x = local.X * scale;
  float y = local.Y * scale;
  float z = local.Z * scale;

  float rotY = OpenTK.Mathematics.MathHelper.DegreesToRadians(rotYDeg);
  float cosY = MathF.Cos(rotY);
  float sinY = MathF.Sin(rotY);
  float xAfterY = x * cosY + z * sinY;
  float zAfterY = -x * sinY + z * cosY;

  float rotX = OpenTK.Mathematics.MathHelper.DegreesToRadians(rotXDeg);
  float cosX = MathF.Cos(rotX);
  float sinX = MathF.Sin(rotX);
  float yAfterX = y * cosX - zAfterY * sinX;
  float zAfterX = y * sinX + zAfterY * cosX;

  return new OpenTK.Mathematics.Vector3(
    xAfterY + translation.X,
    yAfterX + translation.Y,
    zAfterX + translation.Z);
}

Console.WriteLine();
Console.WriteLine("b_gate group centers (viewer transform):");
EntitySceneModel bGateModel = map.EntityScene.Models.FirstOrDefault(m => m.EntityId == 62);
if (bGateInst.HasValue && bGateModel.EntityId == 62)
{
  float[] testFrames = [0.0f, 87.5f, 175.0f, 262.5f];
  foreach (float nowFrame in testFrames)
  {
    Matrix4x4[] objMats = BuildSceneObjectMatrices(bGateModel.Objects, nowFrame);
    Console.WriteLine($"  frame={nowFrame:F1}");
    for (int gi = 0; gi < bGateModel.MatGroups.Length; ++gi)
    {
      EntitySceneMatGroup g = bGateModel.MatGroups[gi];
      if (g.LocalVertices.Length == 0)
      {
        continue;
      }

      bool hasObject = TryGetSceneObjectMatrix(objMats, g.ObjectId, out Matrix4x4 om);
      OpenTK.Mathematics.Vector3 sum = OpenTK.Mathematics.Vector3.Zero;
      int samples = Math.Min(g.LocalVertices.Length, 256);
      for (int vi = 0; vi < samples; ++vi)
      {
        OpenTK.Mathematics.Vector3 p = g.LocalVertices[vi].Position;
        if (hasObject)
        {
          p = TransformEntityPosition(p, om);
        }

        p = TransformEntityInstancePosition(p, bGateInst.Value.Scale, bGateInst.Value.RotX, bGateInst.Value.RotY, bGateInst.Value.Position);
        sum += p;
      }

      OpenTK.Mathematics.Vector3 c = sum / samples;
      Console.WriteLine($"    gi={gi} obj={g.ObjectId} center=({c.X:F2},{c.Y:F2},{c.Z:F2})");
    }
  }
}
