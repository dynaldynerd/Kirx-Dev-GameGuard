using System.Buffers.Binary;
using System.Text;
using OpenTK.Mathematics;

namespace MapEditor.Formats;

public static class BspLoader
{
  private const int BspHeaderSize = 0x2AC;
  private const int ExtBspHeaderSize = 0x184;
  private const uint ExpectedBspVersion = 39;
  private const uint ExpectedEbpVersion = 20;
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

  public static LoadedMap Load(string bspPath, string ebpPath)
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

    BspData bsp = ReadBsp(resolvedBsp);
    ExtBspData ext = ReadExtBsp(resolvedEbp);
    MaterialData materialData = ReadMaterialData(resolvedBsp);

    Vector3[] boundsSource = bsp.TriangleVertices.Length > 0 ? bsp.TriangleVertices : ext.CollisionVertices;
    MapBounds bounds = ComputeBoundsFromVertices(boundsSource);

    return new LoadedMap
    {
      Name = Path.GetFileNameWithoutExtension(resolvedBsp),
      BspPath = resolvedBsp,
      EbpPath = resolvedEbp,
      Bounds = bounds,
      BspTriangleVertices = bsp.TriangleVertices,
      BspRenderVertices = bsp.RenderVertices,
      BspMaterialSpans = bsp.MaterialSpans,
      MaterialSurfaceIds = materialData.MaterialSurfaceIds,
      SurfaceTextures = materialData.SurfaceTextures,
      CollisionVertices = ext.CollisionVertices,
      CollisionLines = ext.CollisionLines,
    };
  }

  private static BspData ReadBsp(string bspPath)
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

    uint[] vertexColor = ReadUInt32Array(reader, checked((int)(header.VertexColor.Size / 4)), "VertexColor");
    float[] uvData = ReadSingleArray(reader, checked((int)(header.Uv.Size / 4)), "UV");
    SkipSection(stream, header.LgtUv.Size, "LgtUV");

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
      vertexColor);

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

    // Native loader consumes EBP sequentially after header by section size.
    stream.Position = ExtBspHeaderSize;
    Vector3[] vertices = ReadCollisionVerticesSequential(reader, vertexCount);
    CollisionLine[] lines = ReadCollisionLinesSequential(reader, lineCount);

    return new ExtBspData(vertices, lines);
  }

  private static MaterialData ReadMaterialData(string bspPath)
  {
    string? r3mPath = FindSiblingFileWithExtension(bspPath, ".r3m");
    string? r3tPath = FindSiblingFileWithExtension(bspPath, ".r3t");

    int[] materialSurfaceIds = Array.Empty<int>();
    if (r3mPath != null)
    {
      try
      {
        materialSurfaceIds = ReadR3mSurfaceIds(r3mPath);
      }
      catch
      {
        materialSurfaceIds = Array.Empty<int>();
      }
    }

    R3TextureBlob[] surfaceTextures = Array.Empty<R3TextureBlob>();
    if (r3tPath != null)
    {
      try
      {
        surfaceTextures = ReadR3tTextureBlobs(r3tPath);
      }
      catch
      {
        surfaceTextures = Array.Empty<R3TextureBlob>();
      }
    }

    return new MaterialData(materialSurfaceIds, surfaceTextures);
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

  private static int[] ReadR3mSurfaceIds(string r3mPath)
  {
    using FileStream stream = File.OpenRead(r3mPath);
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

    int[] surfaceIds = new int[materialCount];
    Array.Fill(surfaceIds, -1);

    for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
    {
      if (stream.Position + 144 > stream.Length)
      {
        throw new EndOfStreamException("Unexpected EOF while reading R3M material header.");
      }

      uint layerCount = reader.ReadUInt32();
      _ = reader.ReadUInt32(); // m_dwFlag
      _ = reader.ReadInt32(); // m_iDetailSurface
      _ = reader.ReadSingle(); // m_fDetailScale
      _ = ReadExactBytes(reader, 128, "R3M material name");

      for (uint layerIndex = 0; layerIndex < layerCount; ++layerIndex)
      {
        if (stream.Position + 46 > stream.Length)
        {
          throw new EndOfStreamException("Unexpected EOF while reading R3M layer block.");
        }

        _ = reader.ReadInt16(); // m_iTileAniTexNum
        int surfaceId = reader.ReadInt32(); // m_iSurface
        _ = reader.ReadUInt32(); // m_dwAlphaType
        _ = reader.ReadUInt32(); // m_ARGB
        _ = reader.ReadUInt32(); // m_dwFlag
        SkipSection(stream, 28u, "R3M layer tail");

        if (layerIndex == 0)
        {
          surfaceIds[materialIndex] = surfaceId;
        }
      }
    }

    return surfaceIds;
  }

  private static R3TextureBlob[] ReadR3tTextureBlobs(string r3tPath)
  {
    using FileStream stream = File.OpenRead(r3tPath);
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
      textures.Add(new R3TextureBlob(i + 1, names[i], ddsBytes));
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
    uint[] vertexColor)
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
            uvData,
            vertexColor));
          triangles.Add(CreateRenderVertex(
            decompressed[faceVertexIds[tri - 1]],
            faceUvIds[tri - 1],
            faceColorIds[tri - 1],
            uvData,
            vertexColor));
          triangles.Add(CreateRenderVertex(
            decompressed[faceVertexIds[tri]],
            faceUvIds[tri],
            faceColorIds[tri],
            uvData,
            vertexColor));
        }
      }

      int groupVertexCount = triangles.Count - groupStartVertex;
      if (groupVertexCount > 0)
      {
        if (spans.Count > 0)
        {
          BspMaterialSpan last = spans[spans.Count - 1];
          if (last.MaterialId == group.MaterialId && last.StartVertex + last.VertexCount == groupStartVertex)
          {
            spans[spans.Count - 1] = new BspMaterialSpan(last.MaterialId, last.StartVertex, last.VertexCount + groupVertexCount);
          }
          else
          {
            spans.Add(new BspMaterialSpan(group.MaterialId, groupStartVertex, groupVertexCount));
          }
        }
        else
        {
          spans.Add(new BspMaterialSpan(group.MaterialId, groupStartVertex, groupVertexCount));
        }
      }
    }

    return new MeshBuildResult(triangles.ToArray(), spans.ToArray());
  }

  private static BspRenderVertex CreateRenderVertex(
    Vector3 position,
    int uvId,
    int colorId,
    float[] uvData,
    uint[] vertexColor)
  {
    Vector2 uv = DecodeUv(uvData, uvId);
    Vector3 color = DecodeColor(vertexColor, colorId);
    return new BspRenderVertex(position, uv, color);
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

  private static Vector3 DecodeColor(uint[] vertexColor, int colorId)
  {
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

    return new ExtBspHeader(version, entries[0], entries[1]);
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
      _ = reader.ReadInt16(); // lgt_id
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

      groups[i] = new ReadMatGroup(attr, faceCount, faceStartId, mtlId, new Vector3(posX, posY, posZ), scale);
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
    Vector3 Position,
    float Scale);

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
    public ExtBspData(Vector3[] collisionVertices, CollisionLine[] collisionLines)
    {
      CollisionVertices = collisionVertices;
      CollisionLines = collisionLines;
    }

    public Vector3[] CollisionVertices { get; }
    public CollisionLine[] CollisionLines { get; }
  }

  private sealed class MaterialData
  {
    public MaterialData(int[] materialSurfaceIds, R3TextureBlob[] surfaceTextures)
    {
      MaterialSurfaceIds = materialSurfaceIds;
      SurfaceTextures = surfaceTextures;
    }

    public int[] MaterialSurfaceIds { get; }
    public R3TextureBlob[] SurfaceTextures { get; }
  }
}
