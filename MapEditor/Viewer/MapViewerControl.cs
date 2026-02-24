using System.Diagnostics;
using System.ComponentModel;
using System.Runtime.InteropServices;
using MapEditor.Formats;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using GLControlSettings = OpenTK.GLControl.GLControlSettings;
using GLControlWinForms = OpenTK.GLControl.GLControl;
using OpenTK.Windowing.Common;

namespace MapEditor.Viewer;

internal sealed class MapViewerControl : UserControl
{
  private const int MeshFloatStride = 8;
  private const int MeshByteStride = MeshFloatStride * 4;

  private const int DdsHeaderSize = 128;
  private const int FourCcDxt1 = 0x31545844;
  private const int FourCcDxt3 = 0x33545844;
  private const int FourCcDxt5 = 0x35545844;
  private const int GlCompressedRgbaS3tcDxt1Ext = 0x83F1;
  private const int GlCompressedRgbaS3tcDxt3Ext = 0x83F2;
  private const int GlCompressedRgbaS3tcDxt5Ext = 0x83F3;
  private const int DdpfRgb = 0x40;

  private readonly GLControlWinForms _glControl;
  private readonly System.Windows.Forms.Timer _renderTimer;
  private readonly Stopwatch _frameClock;
  private readonly HashSet<Keys> _keysDown = [];
  private readonly List<int> _mapTextures = [];

  private FreeCamera _camera = new(new Vector3(0f, 100f, 250f));
  private MapBounds _currentBounds = new(new Vector3(-100f, -100f, -100f), new Vector3(100f, 100f, 100f));
  private LoadedMap? _map;
  private float _moveSpeed = 350f;
  private float _sprintMultiplier = 1200f / 350f;
  private bool _showOverlay = true;

  private bool _glReady;
  private bool _capturingLook;
  private Point _lastMousePoint;

  private int _meshProgram;
  private int _meshVao;
  private int _meshVbo;
  private int _meshVertexCount;
  private int _meshUniformMvp;
  private int _meshUniformTexture;

  private int _lineProgram;
  private int _lineVao;
  private int _lineVbo;
  private int _lineVertexCount;
  private int _lineUniformMvp;
  private int _lineUniformColor;

  private int _clipDebugProgram;
  private int _checkerTexture;
  private int _loadedTextureCount;
  private DrawSpan[] _meshDrawSpans = Array.Empty<DrawSpan>();

  [DllImport("user32.dll")]
  private static extern short GetAsyncKeyState(int vKey);

  public int RenderVertexCount => _meshVertexCount;
  public int TextureCount => _loadedTextureCount;

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public float MoveSpeed
  {
    get => _moveSpeed;
    set => _moveSpeed = Math.Clamp(value, 10f, 50000f);
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public float SprintMultiplier
  {
    get => _sprintMultiplier;
    set => _sprintMultiplier = Math.Clamp(value, 1f, 10f);
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool ShowOverlay
  {
    get => _showOverlay;
    set => _showOverlay = value;
  }

  public MapViewerControl()
  {
    GLControlSettings glSettings = new()
    {
      API = ContextAPI.OpenGL,
      APIVersion = new Version(3, 3),
      Profile = ContextProfile.Core,
      Flags = ContextFlags.Default,
    };

    _glControl = new GLControlWinForms(glSettings)
    {
      Dock = DockStyle.Fill,
      BackColor = Color.Black,
      TabStop = true,
    };

    _glControl.Load += OnGlLoad;
    _glControl.Resize += OnGlResize;
    _glControl.Paint += OnGlPaint;
    _glControl.MouseDown += OnMouseDown;
    _glControl.MouseUp += OnMouseUp;
    _glControl.MouseMove += OnMouseMove;
    _glControl.KeyDown += OnKeyDown;
    _glControl.KeyUp += OnKeyUp;
    _glControl.MouseWheel += OnMouseWheel;
    Controls.Add(_glControl);

    _frameClock = Stopwatch.StartNew();
    _renderTimer = new System.Windows.Forms.Timer
    {
      Interval = 16,
      Enabled = true,
    };
    _renderTimer.Tick += OnRenderTick;
  }

  public void SetMap(LoadedMap map)
  {
    _map = map;
    _currentBounds = map.Bounds;
    _camera = CreateDefaultCamera(_currentBounds);

    if (_glReady)
    {
      _glControl.MakeCurrent();
      UploadGeometry();
    }

    _glControl.Invalidate();
  }

  private static FreeCamera CreateDefaultCamera(MapBounds bounds)
  {
    Vector3 center = bounds.Center;
    Vector3 size = bounds.Size;
    float maxAxis = MathF.Max(size.X, MathF.Max(size.Y, size.Z));
    if (maxAxis < 100f)
    {
      maxAxis = 100f;
    }

    Vector3 start = center + new Vector3(0f, maxAxis * 0.80f, maxAxis * 1.40f);
    return FreeCamera.CreateLookingAt(start, center);
  }

  private void OnGlLoad(object? sender, EventArgs e)
  {
    _glControl.MakeCurrent();
    GL.ClearColor(0.05f, 0.06f, 0.08f, 1f);
    GL.Enable(EnableCap.DepthTest);
    GL.Enable(EnableCap.CullFace);
    GL.CullFace(TriangleFace.Back);

    _meshProgram = CreateMeshShaderProgram();
    _meshUniformMvp = GL.GetUniformLocation(_meshProgram, "uMvp");
    _meshUniformTexture = GL.GetUniformLocation(_meshProgram, "uTexture0");
    _checkerTexture = CreateCheckerTexture();

    _lineProgram = CreateLineShaderProgram();
    _lineUniformMvp = GL.GetUniformLocation(_lineProgram, "uMvp");
    _lineUniformColor = GL.GetUniformLocation(_lineProgram, "uColor");

    _clipDebugProgram = CreateClipDebugShaderProgram();

    _meshVao = GL.GenVertexArray();
    _meshVbo = GL.GenBuffer();
    GL.BindVertexArray(_meshVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _meshVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, MeshByteStride, 0);
    GL.EnableVertexAttribArray(1);
    GL.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, false, MeshByteStride, 12);
    GL.EnableVertexAttribArray(2);
    GL.VertexAttribPointer(2, 3, VertexAttribPointerType.Float, false, MeshByteStride, 20);

    _lineVao = GL.GenVertexArray();
    _lineVbo = GL.GenBuffer();
    GL.BindVertexArray(_lineVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _lineVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);

    _glReady = true;
    UploadGeometry();
    ResizeViewport();
  }

  private void OnGlResize(object? sender, EventArgs e)
  {
    if (!_glReady)
    {
      return;
    }

    _glControl.MakeCurrent();
    ResizeViewport();
  }

  private void ResizeViewport()
  {
    int width = Math.Max(1, _glControl.ClientSize.Width);
    int height = Math.Max(1, _glControl.ClientSize.Height);
    GL.Viewport(0, 0, width, height);
  }

  private void OnGlPaint(object? sender, PaintEventArgs e)
  {
    if (!_glReady)
    {
      return;
    }

    _glControl.MakeCurrent();
    Render();
    _glControl.SwapBuffers();
  }

  private void OnRenderTick(object? sender, EventArgs e)
  {
    if (!_glReady)
    {
      return;
    }

    float deltaSeconds = (float)_frameClock.Elapsed.TotalSeconds;
    _frameClock.Restart();
    UpdateCamera(deltaSeconds);
    _glControl.Invalidate();
  }

  private void UpdateCamera(float deltaSeconds)
  {
    Vector3 move = Vector3.Zero;

    if (IsMoveKeyDown(Keys.W))
    {
      move += _camera.Forward;
    }
    if (IsMoveKeyDown(Keys.S))
    {
      move -= _camera.Forward;
    }
    if (IsMoveKeyDown(Keys.A))
    {
      move -= _camera.Right;
    }
    if (IsMoveKeyDown(Keys.D))
    {
      move += _camera.Right;
    }
    if (IsMoveKeyDown(Keys.Space))
    {
      move += Vector3.UnitY;
    }
    if (IsMoveKeyDown(Keys.ControlKey))
    {
      move -= Vector3.UnitY;
    }

    if (move.LengthSquared <= 0f)
    {
      return;
    }

    move = Vector3.Normalize(move);
    float speed = IsMoveKeyDown(Keys.ShiftKey) ? _moveSpeed * _sprintMultiplier : _moveSpeed;
    _camera.Position += move * speed * MathF.Max(0.0001f, deltaSeconds);
  }

  private bool IsKeyDown(Keys key)
  {
    return _keysDown.Contains(key);
  }

  private bool IsMoveKeyDown(Keys key)
  {
    if (_keysDown.Contains(key))
    {
      return true;
    }

    return (GetAsyncKeyState((int)key) & 0x8000) != 0;
  }

  private void Render()
  {
    GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

    float aspect = Math.Max(0.0001f, (float)_glControl.ClientSize.Width / Math.Max(1f, _glControl.ClientSize.Height));
    Matrix4 projection = Matrix4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(65f), aspect, 0.1f, 200000f);
    Matrix4 view = _camera.GetViewMatrix();
    Matrix4 mvp = view * projection;

    if (_meshVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_meshProgram);
      GL.UniformMatrix4(_meshUniformMvp, true, ref mvp);
      GL.Uniform1(_meshUniformTexture, 0);
      GL.ActiveTexture(TextureUnit.Texture0);
      GL.BindVertexArray(_meshVao);

      if (_meshDrawSpans.Length > 0)
      {
        for (int i = 0; i < _meshDrawSpans.Length; ++i)
        {
          DrawSpan span = _meshDrawSpans[i];
          GL.BindTexture(TextureTarget.Texture2D, span.TextureId);
          GL.DrawArrays(PrimitiveType.Triangles, span.StartVertex, span.VertexCount);
        }
      }
      else
      {
        GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
        GL.DrawArrays(PrimitiveType.Triangles, 0, _meshVertexCount);
      }
    }

    if (_showOverlay && _lineVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_lineProgram);
      GL.UniformMatrix4(_lineUniformMvp, true, ref mvp);
      GL.Uniform3(_lineUniformColor, new Vector3(0.92f, 0.97f, 1.00f));
      GL.BindVertexArray(_lineVao);
      GL.DrawArrays(PrimitiveType.Lines, 0, _lineVertexCount);
    }

    GL.Disable(EnableCap.DepthTest);
    GL.UseProgram(_clipDebugProgram);
    GL.BindVertexArray(_lineVao);
    GL.DrawArrays(PrimitiveType.Lines, 0, 4);
  }

  private void UploadGeometry()
  {
    if (_map == null)
    {
      _meshVertexCount = 0;
      _lineVertexCount = 0;
      _meshDrawSpans = Array.Empty<DrawSpan>();
      ReleaseMapTextures();
      return;
    }

    BspRenderVertex[] meshVertices = _map.BspRenderVertices;
    _meshVertexCount = meshVertices.Length;

    float[] meshData = new float[_meshVertexCount * MeshFloatStride];
    for (int i = 0; i < _meshVertexCount; ++i)
    {
      int offset = i * MeshFloatStride;
      BspRenderVertex v = meshVertices[i];
      meshData[offset] = v.Position.X;
      meshData[offset + 1] = v.Position.Y;
      meshData[offset + 2] = v.Position.Z;
      meshData[offset + 3] = v.Uv.X;
      meshData[offset + 4] = v.Uv.Y;
      meshData[offset + 5] = v.Color.X;
      meshData[offset + 6] = v.Color.Y;
      meshData[offset + 7] = v.Color.Z;
    }

    GL.BindVertexArray(_meshVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _meshVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, meshData.Length * 4, meshData, BufferUsageHint.StaticDraw);

    ReleaseMapTextures();

    Dictionary<int, int> surfaceToTexture = new();
    if (_map.SurfaceTextures.Length > 0)
    {
      for (int i = 0; i < _map.SurfaceTextures.Length; ++i)
      {
        R3TextureBlob textureBlob = _map.SurfaceTextures[i];
        if (textureBlob.SurfaceId <= 0 || surfaceToTexture.ContainsKey(textureBlob.SurfaceId))
        {
          continue;
        }

        int textureId = CreateTextureFromDds(textureBlob.DdsBytes);
        if (textureId == 0)
        {
          continue;
        }

        surfaceToTexture[textureBlob.SurfaceId] = textureId;
        _mapTextures.Add(textureId);
      }
    }

    _loadedTextureCount = surfaceToTexture.Count;

    BspMaterialSpan[] materialSpans = _map.BspMaterialSpans;
    int[] materialSurfaceIds = _map.MaterialSurfaceIds;
    List<DrawSpan> spans = new(Math.Max(4, materialSpans.Length));

    for (int i = 0; i < materialSpans.Length; ++i)
    {
      BspMaterialSpan span = materialSpans[i];
      if (span.VertexCount <= 0)
      {
        continue;
      }

      int textureId = _checkerTexture;
      if (span.MaterialId >= 0 && span.MaterialId < materialSurfaceIds.Length)
      {
        int surfaceId = materialSurfaceIds[span.MaterialId];
        if (surfaceToTexture.TryGetValue(surfaceId, out int mappedTexture))
        {
          textureId = mappedTexture;
        }
      }

      AddOrMergeDrawSpan(spans, new DrawSpan(span.StartVertex, span.VertexCount, textureId));
    }

    if (spans.Count == 0 && _meshVertexCount > 0)
    {
      spans.Add(new DrawSpan(0, _meshVertexCount, _checkerTexture));
    }

    _meshDrawSpans = spans.ToArray();

    Vector3[] lineVertices = BuildOverlayLines(_map);
    _lineVertexCount = lineVertices.Length;
    GL.BindVertexArray(_lineVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _lineVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, lineVertices.Length * 12, lineVertices, BufferUsageHint.DynamicDraw);
  }

  private static void AddOrMergeDrawSpan(List<DrawSpan> spans, DrawSpan value)
  {
    if (spans.Count == 0)
    {
      spans.Add(value);
      return;
    }

    DrawSpan last = spans[spans.Count - 1];
    if (last.TextureId == value.TextureId && last.StartVertex + last.VertexCount == value.StartVertex)
    {
      spans[spans.Count - 1] = new DrawSpan(last.StartVertex, last.VertexCount + value.VertexCount, last.TextureId);
      return;
    }

    spans.Add(value);
  }

  private void ReleaseMapTextures()
  {
    _loadedTextureCount = 0;
    _meshDrawSpans = Array.Empty<DrawSpan>();

    if (_mapTextures.Count == 0)
    {
      return;
    }

    for (int i = 0; i < _mapTextures.Count; ++i)
    {
      int texture = _mapTextures[i];
      if (texture != 0)
      {
        GL.DeleteTexture(texture);
      }
    }

    _mapTextures.Clear();
  }

  private int CreateTextureFromDds(byte[] ddsBytes)
  {
    if (TryCreateCompressedDdsTexture(ddsBytes, out int texture))
    {
      return texture;
    }

    if (TryCreateUncompressedDdsTexture(ddsBytes, out texture))
    {
      return texture;
    }

    return 0;
  }

  private static bool TryCreateUncompressedDdsTexture(byte[] ddsBytes, out int texture)
  {
    texture = 0;
    if (ddsBytes.Length < DdsHeaderSize)
    {
      return false;
    }

    if (ddsBytes[0] != (byte)'D' || ddsBytes[1] != (byte)'D' || ddsBytes[2] != (byte)'S' || ddsBytes[3] != (byte)' ')
    {
      return false;
    }

    int width = BitConverter.ToInt32(ddsBytes, 16);
    int height = BitConverter.ToInt32(ddsBytes, 12);
    int pixelFormatFlags = BitConverter.ToInt32(ddsBytes, 80);
    int fourCc = BitConverter.ToInt32(ddsBytes, 84);
    int rgbBits = BitConverter.ToInt32(ddsBytes, 88);
    uint rMask = BitConverter.ToUInt32(ddsBytes, 92);
    uint gMask = BitConverter.ToUInt32(ddsBytes, 96);
    uint bMask = BitConverter.ToUInt32(ddsBytes, 100);
    uint aMask = BitConverter.ToUInt32(ddsBytes, 104);

    if (width <= 0 || height <= 0 || fourCc != 0 || (pixelFormatFlags & DdpfRgb) == 0)
    {
      return false;
    }

    if (rgbBits == 16)
    {
      int sourceSize = width * height * 2;
      if (DdsHeaderSize + sourceSize > ddsBytes.Length)
      {
        return false;
      }

      byte[] rgb = new byte[width * height * 3];
      int dst = 0;
      for (int src = DdsHeaderSize; src < DdsHeaderSize + sourceSize; src += 2)
      {
        ushort px = BitConverter.ToUInt16(ddsBytes, src);
        byte r = ScaleMaskComponent16(px, rMask);
        byte g = ScaleMaskComponent16(px, gMask);
        byte b = ScaleMaskComponent16(px, bMask);
        rgb[dst++] = r;
        rgb[dst++] = g;
        rgb[dst++] = b;
      }

      texture = GL.GenTexture();
      GL.BindTexture(TextureTarget.Texture2D, texture);
      GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgb8, width, height, 0, PixelFormat.Rgb, PixelType.UnsignedByte, rgb);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Linear);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.Repeat);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.Repeat);
      return GL.GetError() == ErrorCode.NoError;
    }

    if (rgbBits == 32)
    {
      int sourceSize = width * height * 4;
      if (DdsHeaderSize + sourceSize > ddsBytes.Length)
      {
        return false;
      }

      byte[] rgba = new byte[sourceSize];
      int dst = 0;
      for (int src = DdsHeaderSize; src < DdsHeaderSize + sourceSize; src += 4)
      {
        uint px = BitConverter.ToUInt32(ddsBytes, src);
        byte r = ScaleMaskComponent32(px, rMask);
        byte g = ScaleMaskComponent32(px, gMask);
        byte b = ScaleMaskComponent32(px, bMask);
        byte a = aMask != 0 ? ScaleMaskComponent32(px, aMask) : (byte)255;
        rgba[dst++] = r;
        rgba[dst++] = g;
        rgba[dst++] = b;
        rgba[dst++] = a;
      }

      texture = GL.GenTexture();
      GL.BindTexture(TextureTarget.Texture2D, texture);
      GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba8, width, height, 0, PixelFormat.Rgba, PixelType.UnsignedByte, rgba);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Linear);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.Repeat);
      GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.Repeat);
      return GL.GetError() == ErrorCode.NoError;
    }

    return false;
  }

  private static byte ScaleMaskComponent16(ushort value, uint mask)
  {
    if (mask == 0)
    {
      return 0;
    }

    int shift = System.Numerics.BitOperations.TrailingZeroCount(mask);
    int bits = System.Numerics.BitOperations.PopCount(mask);
    int component = (int)((value & mask) >> shift);
    int maxValue = (1 << bits) - 1;
    if (maxValue <= 0)
    {
      return 0;
    }

    return (byte)((component * 255) / maxValue);
  }

  private static byte ScaleMaskComponent32(uint value, uint mask)
  {
    if (mask == 0)
    {
      return 0;
    }

    int shift = System.Numerics.BitOperations.TrailingZeroCount(mask);
    int bits = System.Numerics.BitOperations.PopCount(mask);
    uint component = (value & mask) >> shift;
    int maxValue = (1 << bits) - 1;
    if (maxValue <= 0)
    {
      return 0;
    }

    return (byte)((component * 255u) / (uint)maxValue);
  }

  private static bool TryCreateCompressedDdsTexture(byte[] ddsBytes, out int texture)
  {
    texture = 0;

    if (ddsBytes.Length < DdsHeaderSize)
    {
      return false;
    }

    if (ddsBytes[0] != (byte)'D' || ddsBytes[1] != (byte)'D' || ddsBytes[2] != (byte)'S' || ddsBytes[3] != (byte)' ')
    {
      return false;
    }

    int headerSize = BitConverter.ToInt32(ddsBytes, 4);
    if (headerSize != 124)
    {
      return false;
    }

    int width = BitConverter.ToInt32(ddsBytes, 16);
    int height = BitConverter.ToInt32(ddsBytes, 12);
    if (width <= 0 || height <= 0)
    {
      return false;
    }

    int mipLevelCount = BitConverter.ToInt32(ddsBytes, 28);
    if (mipLevelCount <= 0)
    {
      mipLevelCount = 1;
    }

    int pixelFormatSize = BitConverter.ToInt32(ddsBytes, 76);
    if (pixelFormatSize != 32)
    {
      return false;
    }

    int fourCc = BitConverter.ToInt32(ddsBytes, 84);
    int blockSize;
    int compressedInternalFormat;
    switch (fourCc)
    {
      case FourCcDxt1:
        compressedInternalFormat = GlCompressedRgbaS3tcDxt1Ext;
        blockSize = 8;
        break;
      case FourCcDxt3:
        compressedInternalFormat = GlCompressedRgbaS3tcDxt3Ext;
        blockSize = 16;
        break;
      case FourCcDxt5:
        compressedInternalFormat = GlCompressedRgbaS3tcDxt5Ext;
        blockSize = 16;
        break;
      default:
        return false;
    }

    texture = GL.GenTexture();
    GL.BindTexture(TextureTarget.Texture2D, texture);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)(mipLevelCount > 1 ? TextureMinFilter.LinearMipmapLinear : TextureMinFilter.Linear));
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.Repeat);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.Repeat);

    int offset = DdsHeaderSize;
    int levelWidth = width;
    int levelHeight = height;

    for (int level = 0; level < mipLevelCount; ++level)
    {
      int blockWidth = Math.Max(1, (levelWidth + 3) / 4);
      int blockHeight = Math.Max(1, (levelHeight + 3) / 4);
      int levelByteSize = checked(blockWidth * blockHeight * blockSize);
      if (offset + levelByteSize > ddsBytes.Length)
      {
        GL.DeleteTexture(texture);
        texture = 0;
        return false;
      }

      byte[] levelData = new byte[levelByteSize];
      System.Buffer.BlockCopy(ddsBytes, offset, levelData, 0, levelByteSize);
      GL.CompressedTexImage2D(
        TextureTarget.Texture2D,
        level,
        (InternalFormat)compressedInternalFormat,
        levelWidth,
        levelHeight,
        0,
        levelByteSize,
        levelData);

      ErrorCode error = GL.GetError();
      if (error != ErrorCode.NoError)
      {
        GL.DeleteTexture(texture);
        texture = 0;
        return false;
      }

      offset += levelByteSize;
      levelWidth = Math.Max(1, levelWidth >> 1);
      levelHeight = Math.Max(1, levelHeight >> 1);
    }

    return true;
  }

  private static Vector3[] BuildOverlayLines(LoadedMap map)
  {
    List<Vector3> output = new(Math.Max(2048, map.CollisionLines.Length * 8));
    AddGrid(map.Bounds, output);

    for (int index = 0; index < map.CollisionLines.Length; ++index)
    {
      CollisionLine line = map.CollisionLines[index];
      if (line.StartVertex >= map.CollisionVertices.Length || line.EndVertex >= map.CollisionVertices.Length)
      {
        continue;
      }

      Vector3 start = map.CollisionVertices[line.StartVertex];
      Vector3 end = map.CollisionVertices[line.EndVertex];
      float height = MathF.Max(0f, line.Height);
      Vector3 startTop = start + new Vector3(0f, height, 0f);
      Vector3 endTop = end + new Vector3(0f, height, 0f);
      if (!IsFinite(start) || !IsFinite(end) || !IsFinite(startTop) || !IsFinite(endTop))
      {
        continue;
      }

      AddSegment(output, start, end);
      AddSegment(output, start, startTop);
      AddSegment(output, end, endTop);
      AddSegment(output, startTop, endTop);
    }

    return output.ToArray();
  }

  private static void AddGrid(MapBounds bounds, List<Vector3> output)
  {
    Vector3 center = bounds.Center;
    Vector3 size = bounds.Size;
    float extent = MathF.Max(200f, MathF.Max(size.X, size.Z) * 0.55f);
    float y = center.Y;
    const float step = 500f;

    for (float x = center.X - extent; x <= center.X + extent; x += step)
    {
      AddSegment(output, new Vector3(x, y, center.Z - extent), new Vector3(x, y, center.Z + extent));
    }

    for (float z = center.Z - extent; z <= center.Z + extent; z += step)
    {
      AddSegment(output, new Vector3(center.X - extent, y, z), new Vector3(center.X + extent, y, z));
    }
  }

  private static void AddSegment(List<Vector3> output, Vector3 a, Vector3 b)
  {
    output.Add(a);
    output.Add(b);
  }

  private static bool IsFinite(Vector3 v)
  {
    return float.IsFinite(v.X) && float.IsFinite(v.Y) && float.IsFinite(v.Z);
  }

  private static int CreateMeshShaderProgram()
  {
    const string vertexShaderSource =
      """
      #version 330 core
      layout (location = 0) in vec3 aPosition;
      layout (location = 1) in vec2 aUv;
      layout (location = 2) in vec3 aColor;

      uniform mat4 uMvp;
      out vec2 vUv;
      out vec3 vColor;

      void main()
      {
        gl_Position = vec4(aPosition, 1.0) * uMvp;
        vUv = aUv;
        vColor = aColor;
      }
      """;

    const string fragmentShaderSource =
      """
      #version 330 core
      in vec2 vUv;
      in vec3 vColor;
      uniform sampler2D uTexture0;
      out vec4 FragColor;

      void main()
      {
        vec3 tex = texture(uTexture0, vUv).rgb;
        vec3 lit = clamp(vColor + vec3(0.18), vec3(0.0), vec3(1.0));
        vec3 outColor = tex * lit;
        FragColor = vec4(outColor, 1.0);
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "Mesh");
  }

  private static int CreateLineShaderProgram()
  {
    const string vertexShaderSource =
      """
      #version 330 core
      layout (location = 0) in vec3 aPosition;
      uniform mat4 uMvp;

      void main()
      {
        gl_Position = vec4(aPosition, 1.0) * uMvp;
      }
      """;

    const string fragmentShaderSource =
      """
      #version 330 core
      uniform vec3 uColor;
      out vec4 FragColor;

      void main()
      {
        FragColor = vec4(uColor, 1.0);
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "Line");
  }

  private static int CreateClipDebugShaderProgram()
  {
    const string vertexShaderSource =
      """
      #version 330 core

      void main()
      {
        vec2 p = vec2(0.0, 0.0);
        if (gl_VertexID == 0) p = vec2(-0.05, 0.0);
        else if (gl_VertexID == 1) p = vec2(0.05, 0.0);
        else if (gl_VertexID == 2) p = vec2(0.0, -0.05);
        else p = vec2(0.0, 0.05);
        gl_Position = vec4(p, 0.0, 1.0);
      }
      """;

    const string fragmentShaderSource =
      """
      #version 330 core
      out vec4 FragColor;

      void main()
      {
        FragColor = vec4(1.0, 0.15, 0.15, 1.0);
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "ClipDebug");
  }

  private static int CreateShaderProgram(string vertexShaderSource, string fragmentShaderSource, string label)
  {
    int vertex = CompileShader(ShaderType.VertexShader, vertexShaderSource);
    int fragment = CompileShader(ShaderType.FragmentShader, fragmentShaderSource);
    int program = GL.CreateProgram();
    GL.AttachShader(program, vertex);
    GL.AttachShader(program, fragment);
    GL.LinkProgram(program);
    GL.GetProgram(program, GetProgramParameterName.LinkStatus, out int linked);
    if (linked == 0)
    {
      string message = GL.GetProgramInfoLog(program);
      GL.DeleteShader(vertex);
      GL.DeleteShader(fragment);
      GL.DeleteProgram(program);
      throw new InvalidOperationException($"{label} shader link failed: {message}");
    }

    GL.DetachShader(program, vertex);
    GL.DetachShader(program, fragment);
    GL.DeleteShader(vertex);
    GL.DeleteShader(fragment);
    return program;
  }

  private static int CompileShader(ShaderType type, string source)
  {
    int shader = GL.CreateShader(type);
    GL.ShaderSource(shader, source);
    GL.CompileShader(shader);
    GL.GetShader(shader, ShaderParameter.CompileStatus, out int compiled);
    if (compiled == 0)
    {
      string message = GL.GetShaderInfoLog(shader);
      GL.DeleteShader(shader);
      throw new InvalidOperationException($"Shader compile failed ({type}): {message}");
    }

    return shader;
  }

  private static int CreateCheckerTexture()
  {
    const int size = 64;
    byte[] pixels = new byte[size * size * 3];
    for (int y = 0; y < size; ++y)
    {
      for (int x = 0; x < size; ++x)
      {
        bool even = ((x / 8) + (y / 8)) % 2 == 0;
        byte r = even ? (byte)190 : (byte)110;
        byte g = even ? (byte)200 : (byte)120;
        byte b = even ? (byte)210 : (byte)130;
        int offset = (y * size + x) * 3;
        pixels[offset] = r;
        pixels[offset + 1] = g;
        pixels[offset + 2] = b;
      }
    }

    int texture = GL.GenTexture();
    GL.BindTexture(TextureTarget.Texture2D, texture);
    GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgb8, size, size, 0, PixelFormat.Rgb, PixelType.UnsignedByte, pixels);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.LinearMipmapLinear);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.Repeat);
    GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.Repeat);
    GL.GenerateMipmap(GenerateMipmapTarget.Texture2D);
    return texture;
  }

  private void OnMouseDown(object? sender, MouseEventArgs e)
  {
    _glControl.Focus();

    if (e.Button != MouseButtons.Right)
    {
      return;
    }

    _capturingLook = true;
    _lastMousePoint = e.Location;
    _glControl.Capture = true;
  }

  private void OnMouseUp(object? sender, MouseEventArgs e)
  {
    if (e.Button != MouseButtons.Right)
    {
      return;
    }

    _capturingLook = false;
    _glControl.Capture = false;
  }

  private void OnMouseMove(object? sender, MouseEventArgs e)
  {
    if (!_capturingLook)
    {
      return;
    }

    int dx = e.X - _lastMousePoint.X;
    int dy = e.Y - _lastMousePoint.Y;
    _lastMousePoint = e.Location;

    const float sensitivity = 0.17f;
    _camera.AddRotation(dx * sensitivity, -dy * sensitivity);
  }

  private void OnMouseWheel(object? sender, MouseEventArgs e)
  {
    float amount = e.Delta > 0 ? 150f : -150f;
    _camera.Position += _camera.Forward * amount;
  }

  private void OnKeyDown(object? sender, KeyEventArgs e)
  {
    _keysDown.Add(e.KeyCode);
    if (e.KeyCode == Keys.Escape)
    {
      _capturingLook = false;
      _glControl.Capture = false;
    }
    else if (e.KeyCode == Keys.R)
    {
      _camera = CreateDefaultCamera(_currentBounds);
    }
  }

  private void OnKeyUp(object? sender, KeyEventArgs e)
  {
    _keysDown.Remove(e.KeyCode);
  }

  protected override void Dispose(bool disposing)
  {
    if (disposing)
    {
      _renderTimer.Stop();
      _renderTimer.Tick -= OnRenderTick;

      if (_glReady)
      {
        _glControl.MakeCurrent();
        ReleaseMapTextures();

        if (_checkerTexture != 0)
        {
          GL.DeleteTexture(_checkerTexture);
        }

        if (_meshVbo != 0)
        {
          GL.DeleteBuffer(_meshVbo);
        }
        if (_lineVbo != 0)
        {
          GL.DeleteBuffer(_lineVbo);
        }
        if (_meshVao != 0)
        {
          GL.DeleteVertexArray(_meshVao);
        }
        if (_lineVao != 0)
        {
          GL.DeleteVertexArray(_lineVao);
        }

        if (_meshProgram != 0)
        {
          GL.DeleteProgram(_meshProgram);
        }
        if (_lineProgram != 0)
        {
          GL.DeleteProgram(_lineProgram);
        }
        if (_clipDebugProgram != 0)
        {
          GL.DeleteProgram(_clipDebugProgram);
        }
      }
    }

    base.Dispose(disposing);
  }

  private readonly record struct DrawSpan(int StartVertex, int VertexCount, int TextureId);
}
