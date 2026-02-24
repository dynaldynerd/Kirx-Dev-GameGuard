using OpenTK.Mathematics;

namespace MapEditor.Formats;

internal readonly record struct BspEntry(uint Offset, uint Size);

internal readonly record struct ExtBspHeader(uint Version, BspEntry CfVertexSection, BspEntry CfLineSection);

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
  Vector3 Color);

public readonly record struct BspMaterialSpan(
  int MaterialId,
  int StartVertex,
  int VertexCount);

public readonly record struct R3TextureBlob(
  int SurfaceId,
  string Name,
  byte[] DdsBytes);

public sealed class LoadedMap
{
  public required string Name { get; init; }
  public required string BspPath { get; init; }
  public required string EbpPath { get; init; }
  public required MapBounds Bounds { get; init; }
  public required Vector3[] BspTriangleVertices { get; init; }
  public required BspRenderVertex[] BspRenderVertices { get; init; }
  public required BspMaterialSpan[] BspMaterialSpans { get; init; }
  public required int[] MaterialSurfaceIds { get; init; }
  public required R3TextureBlob[] SurfaceTextures { get; init; }
  public required Vector3[] CollisionVertices { get; init; }
  public required CollisionLine[] CollisionLines { get; init; }
}
