using OpenTK.Mathematics;
using NumericsMatrix4 = System.Numerics.Matrix4x4;

namespace MapEditor.Formats;

internal readonly record struct BspEntry(uint Offset, uint Size);

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
  bool FogRangeEnabled,
  bool NoFogSky,
  bool HasLensFlare,
  Vector3 LensFlarePosition)
{
  public static MapEnvironmentSettings Default =>
    new(
      false,
      5.0f,
      5000.0f,
      new Vector3(0.05f, 0.06f, 0.08f),
      false,
      false,
      false,
      Vector3.Zero);
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

public sealed class LoadedMap
{
  public required string Name { get; init; }
  public required string BspPath { get; init; }
  public required string EbpPath { get; init; }
  public required MapEnvironmentSettings Environment { get; init; }
  public required ExtDummyDefinition[] ExtDummies { get; init; }
  public required MapBounds Bounds { get; init; }
  public required Vector3[] BspTriangleVertices { get; init; }
  public required BspRenderVertex[] BspRenderVertices { get; init; }
  public required BspMaterialSpan[] BspMaterialSpans { get; init; }
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
  public required BspRenderVertex[] EntityRenderVertices { get; init; }
  public required BspMaterialSpan[] EntityMaterialSpans { get; init; }
  public required MaterialDefinition[] EntityMaterials { get; init; }
  public required int[] EntityMaterialSurfaceIds { get; init; }
  public required uint[] EntityMaterialAlphaTypes { get; init; }
  public required R3TextureBlob[] EntitySurfaceTextures { get; init; }
  public required int MapEntityModelCount { get; init; }
  public required int MapEntityInstanceCount { get; init; }
  public required Vector3[] ParticleInstancePositions { get; init; }
  public required Vector3[] CollisionVertices { get; init; }
  public required CollisionLine[] CollisionLines { get; init; }
}
