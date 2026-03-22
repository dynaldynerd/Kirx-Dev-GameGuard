using OpenTK.Mathematics;
using NumericsMatrix4 = System.Numerics.Matrix4x4;

namespace MapEditor.Formats;

public readonly record struct BspEntry(uint Offset, uint Size);

public sealed class BspBinaryLayout
{
  public required uint Version { get; init; }
  public required BspEntry[] Entries { get; init; }
  public required byte[][] Sections { get; init; }
  public required byte[] Trailing { get; init; }
}

internal readonly record struct ExtBspHeader(
  uint Version,
  BspEntry CfVertexSection,
  BspEntry CfLineSection,
  BspEntry CfLineIdSection,
  BspEntry CfLeafSection,
  BspEntry EntityListSection,
  BspEntry EntityIdSection,
  BspEntry LeafEntityListSection,
  BspEntry SoundEntityIdSection,
  BspEntry LeafSoundEntityListSection,
  uint ReadSpareTotalSize,
  BspEntry MapEntitiesListSection,
  BspEntry SoundEntityListSection,
  BspEntry SoundEntitiesListSection);

public readonly record struct CollisionLine(
  uint Attribute,
  ushort StartVertex,
  ushort EndVertex,
  float Height,
  ushort FrontLeaf,
  ushort BackLeaf);

public readonly record struct MapBounds(Vector3 Min, Vector3 Max)
{
  public Vector3 Size => Max - Min;
  public Vector3 Center => (Min + Max) * 0.5f;
}

public readonly record struct BspLeafBounds(Vector3 Min, Vector3 Max);
public readonly record struct BspNode(
  uint NormalIndex,
  float Distance,
  short Front,
  short Back,
  Vector3 Min,
  Vector3 Max);

public readonly record struct BspRenderVertex(
  Vector3 Position,
  Vector2 Uv,
  Vector2 LightUv,
  Vector3 Color);

public readonly record struct BspMaterialSpan(
  int MaterialId,
  int StartVertex,
  int VertexCount,
  int LightMapId = -1);

public readonly record struct MaterialLayerDefinition(
  short TileAniTextureCount,
  int SurfaceId,
  uint AlphaType,
  uint Argb,
  uint LayerFlags,
  short UvLavaWave,
  short UvLavaSpeed,
  short UvScrollU,
  short UvScrollV,
  short UvRotate,
  short UvScaleStart,
  short UvScaleEnd,
  short UvScaleSpeed,
  short UvMetal,
  short AniAlphaFlicker,
  ushort AniAlphaFlickerRange,
  short AniTexFrame,
  short AniTexSpeed,
  short GradientAlpha);

public readonly record struct MaterialDefinition(
  uint MaterialFlags,
  int DetailSurfaceId,
  float DetailScale,
  MaterialLayerDefinition[] Layers);

public readonly record struct R3TextureBlob(
  int SurfaceId,
  string Name,
  byte[] DdsBytes);

public readonly record struct MapEnvironmentSettings(
  bool FogEnabled,
  float FogStart,
  float FogEnd,
  Vector3 FogColor,
  bool Fog2Enabled,
  float FogStart2,
  float FogEnd2,
  Vector3 FogColor2,
  Vector3 Fog2BoxMin,
  Vector3 Fog2BoxMax,
  bool FogRangeEnabled,
  bool NoFogSky,
  bool HasLensFlare,
  float[] LensFlareScales,
  string LensFlareTexturePath,
  Vector3 LensFlarePosition)
{
  public static MapEnvironmentSettings Default =>
    new(
      false,
      5.0f,
      5000.0f,
      new Vector3(0.05f, 0.06f, 0.08f),
      false,
      0.0f,
      0.0f,
      Vector3.Zero,
      Vector3.Zero,
      Vector3.Zero,
      false,
      false,
      false,
      CreateDefaultLensFlareScales(),
      string.Empty,
      Vector3.Zero);

  private static float[] CreateDefaultLensFlareScales()
  {
    float[] scales = new float[16];
    Array.Fill(scales, 1.0f);
    return scales;
  }
}

public readonly record struct ExtDummyDefinition(
  string Name,
  uint Flags,
  Vector3 LocalMin,
  Vector3 LocalMax,
  uint Id,
  uint Arg0,
  float Arg1,
  float Arg2,
  NumericsMatrix4 Transform,
  NumericsMatrix4 InverseTransform);

public readonly record struct EntityScenePositionTrack(float Frame, Vector3 Position);

public readonly record struct EntitySceneRotationTrack(float Frame, Vector4 Quaternion);

public readonly record struct EntitySceneScaleTrack(float Frame, Vector3 Scale, Vector4 ScaleQuaternion);

public readonly record struct EntitySceneObject(
  ushort Parent,
  int Frames,
  Vector3 Scale,
  Vector4 ScaleQuaternion,
  Vector3 Position,
  Vector4 Quaternion,
  EntityScenePositionTrack[] PositionTracks,
  EntitySceneRotationTrack[] RotationTracks,
  EntitySceneScaleTrack[] ScaleTracks);

public readonly record struct EntitySceneMatGroup(
  int MaterialId,
  ushort ObjectId,
  BspRenderVertex[] LocalVertices);

public readonly record struct EntitySceneInstance(
  int EntityId,
  float Scale,
  Vector3 Position,
  float RotX,
  float RotY);

public readonly record struct ParticleFloatRange(float Min, float Max);

public enum ParticleSpawnShape
{
  Box = 0,
  Sphere = 1,
  SphereEdge = 2,
}

public readonly record struct ParticleTrackKey(
  float Time,
  bool HasScale,
  ParticleFloatRange Scale,
  bool HasAlpha,
  ParticleFloatRange Alpha,
  bool HasColor,
  ParticleFloatRange ColorR,
  ParticleFloatRange ColorG,
  ParticleFloatRange ColorB,
  bool HasPower,
  ParticleFloatRange PowerX,
  ParticleFloatRange PowerY,
  ParticleFloatRange PowerZ,
  bool HasZRot,
  ParticleFloatRange ZRot,
  bool HasYRot,
  ParticleFloatRange YRot,
  bool HasFlicker);

public readonly record struct ParticleRuntimeDefinition(
  int Count,
  ParticleSpawnShape SpawnShape,
  ParticleFloatRange StartPosX,
  ParticleFloatRange StartPosY,
  ParticleFloatRange StartPosZ,
  ParticleFloatRange LiveTime,
  ParticleFloatRange TimeSpeed,
  ParticleFloatRange GravityX,
  ParticleFloatRange GravityY,
  ParticleFloatRange GravityZ,
  ParticleFloatRange StartPowerX,
  ParticleFloatRange StartPowerY,
  ParticleFloatRange StartPowerZ,
  ParticleFloatRange StartScale,
  ParticleFloatRange StartAlpha,
  ParticleFloatRange StartColorR,
  ParticleFloatRange StartColorG,
  ParticleFloatRange StartColorB,
  ParticleFloatRange StartZRot,
  ParticleFloatRange StartYRot,
  float CreateTimeEpsilon,
  float StartTimeRange,
  bool NoLoop,
  bool AlwaysLive,
  bool Free,
  bool CheckCollision,
  bool EmitTimeEnabled,
  ParticleFloatRange EmitTime,
  bool FlickerEnabled,
  ParticleFloatRange FlickerAlpha,
  ParticleFloatRange FlickerTime,
  bool NoBillboard,
  bool YBillboard,
  bool ZBillboard,
  bool ZFrontEnabled,
  float ZFront,
  ParticleTrackKey[] Tracks);

public sealed class EntitySceneModel
{
  public required int EntityId { get; init; }
  public required int MaterialBase { get; init; }
  public required string Name { get; init; }
  public required EntitySceneObject[] Objects { get; init; }
  public required EntitySceneMatGroup[] MatGroups { get; init; }
  public ParticleRuntimeDefinition? ParticleRuntime { get; init; }
}

public sealed class EntitySceneData
{
  public static EntitySceneData Empty { get; } = new()
  {
    Models = Array.Empty<EntitySceneModel>(),
    Instances = Array.Empty<EntitySceneInstance>(),
  };

  public required EntitySceneModel[] Models { get; init; }
  public required EntitySceneInstance[] Instances { get; init; }
}

public readonly record struct ParticleInstanceInfo(
  int EntityId,
  string Name,
  Vector3 Position);

public sealed class LoadedMap
{
  public required string Name { get; init; }
  public required string BspPath { get; init; }
  public required string EbpPath { get; init; }
  public required BspBinaryLayout BspBinaryLayout { get; init; }
  public required SkySourceMode SkySourceMode { get; init; }
  public required MapEnvironmentSettings Environment { get; init; }
  public required ExtDummyDefinition[] ExtDummies { get; init; }
  public required MapBounds Bounds { get; init; }
  public required Vector3[] BspTriangleVertices { get; init; }
  public required BspRenderVertex[] BspRenderVertices { get; init; }
  public required BspMaterialSpan[] BspMaterialSpans { get; init; }
  public required EntitySceneObject[] BspSceneObjects { get; init; }
  public required ushort[] BspRenderVertexObjectIds { get; init; }
  public required Vector3[] BspRenderVertexLocalPositions { get; init; }
  public required MaterialDefinition[] Materials { get; init; }
  public required int[] MaterialSurfaceIds { get; init; }
  public required uint[] MaterialAlphaTypes { get; init; }
  public required R3TextureBlob[] SurfaceTextures { get; init; }
  public required R3TextureBlob[] LightmapTextures { get; init; }
  public required BspRenderVertex[] SkyRenderVertices { get; init; }
  public required BspMaterialSpan[] SkyMaterialSpans { get; init; }
  public required MaterialDefinition[] SkyMaterials { get; init; }
  public required int[] SkyMaterialSurfaceIds { get; init; }
  public required uint[] SkyMaterialAlphaTypes { get; init; }
  public required R3TextureBlob[] SkySurfaceTextures { get; init; }
  public required EntitySceneObject[] SkySceneObjects { get; init; }
  public required EntitySceneMatGroup[] SkySceneMatGroups { get; init; }
  public required BspRenderVertex[] EntityRenderVertices { get; init; }
  public required BspMaterialSpan[] EntityMaterialSpans { get; init; }
  public required MaterialDefinition[] EntityMaterials { get; init; }
  public required int[] EntityMaterialSurfaceIds { get; init; }
  public required uint[] EntityMaterialAlphaTypes { get; init; }
  public required R3TextureBlob[] EntitySurfaceTextures { get; init; }
  public required EntitySceneData EntityScene { get; init; }
  public required int MapEntityModelCount { get; init; }
  public required int MapEntityInstanceCount { get; init; }
  public required Vector3[] ParticleInstancePositions { get; init; }
  public required ParticleInstanceInfo[] ParticleInstances { get; init; }
  public required Vector3[] BspCollisionNormals { get; init; }
  public required BspNode[] BspCollisionNodes { get; init; }
  public required BspLeafBounds[] BspLeafBounds { get; init; }
  public required Vector3[] CollisionVertices { get; init; }
  public required CollisionLine[] CollisionLines { get; init; }
}
