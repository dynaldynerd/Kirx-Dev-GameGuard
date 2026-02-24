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
  private const int MeshFloatStride = 10;
  private const int MeshByteStride = MeshFloatStride * 4;

  private const int DdsHeaderSize = 128;
  private const int FourCcDxt1 = 0x31545844;
  private const int FourCcDxt3 = 0x33545844;
  private const int FourCcDxt5 = 0x35545844;
  private const int GlCompressedRgbaS3tcDxt1Ext = 0x83F1;
  private const int GlCompressedRgbaS3tcDxt3Ext = 0x83F2;
  private const int GlCompressedRgbaS3tcDxt5Ext = 0x83F3;
  private const int DdpfRgb = 0x40;

  private const int BlendNone = 0;
  private const int BlendOppa = 1;
  private const int BlendDefault = 2;
  private const int BlendBright = 3;
  private const int BlendInvDefault = 5;
  private const int BlendInvBright = 6;
  private const int BlendBackBright = 8;
  private const int BlendLightMap = 10;
  private const int BlendInvLightMap = 11;
  private const int BlendOnlyTransparency = 13;
  private const int BlendShadow = 14;
  private const uint MaterialFlagLightMap = 0x00000001;
  private const uint MaterialLayerFlagEnvBump = 0x00008000;
  private const uint MaterialLayerFlagAniAlphaFlicker = 0x00000400;
  private const float SkyExposure = 1.18f;

  private readonly GLControlWinForms _glControl;
  private readonly System.Windows.Forms.Timer _renderTimer;
  private readonly Stopwatch _frameClock;
  private readonly HashSet<Keys> _keysDown = [];
  private readonly List<int> _mapTextures = [];
  private readonly List<int> _lightmapTextures = [];
  private readonly List<int> _skyTextures = [];
  private readonly List<int> _entityTextures = [];
  private readonly Dictionary<int, int> _mapSurfaceToTexture = [];
  private readonly Dictionary<int, int> _lightMapIdToTexture = [];

  private FreeCamera _camera = new(new Vector3(0f, 100f, 250f));
  private MapBounds _currentBounds = new(new Vector3(-100f, -100f, -100f), new Vector3(100f, 100f, 100f));
  private LoadedMap? _map;
  private float _moveSpeed = 350f;
  private float _sprintMultiplier = 1200f / 350f;
  private bool _showCollisionOverlay = true;
  private bool _showGrid = true;

  private bool _glReady;
  private bool _capturingLook;
  private Point _lastMousePoint;

  private int _meshProgram;
  private int _skyProgram;
  private int _meshVao;
  private int _meshVbo;
  private int _meshVertexCount;
  private int _meshUniformMvp;
  private int _meshUniformTexture;
  private int _meshUniformUvChannel;
  private int _meshUniformLayerColor;
  private int _meshUniformUseVertexLighting;
  private int _skyUniformMvp;
  private int _skyUniformTexture;
  private int _skyUniformExposure;
  private int _skyVao;
  private int _skyVbo;
  private int _skyVertexCount;
  private int _entityVao;
  private int _entityVbo;
  private int _entityVertexCount;

  private int _lineProgram;
  private int _lineVao;
  private int _lineVbo;
  private int _lineVertexCount;
  private int _lineUniformMvp;
  private int _lineUniformColor;
  private int _wallProgram;
  private int _wallVao;
  private int _wallVbo;
  private int _wallVertexCount;
  private int _wallUniformMvp;
  private int _wallUniformColor;

  private int _clipDebugProgram;
  private int _checkerTexture;
  private int _loadedMapTextureCount;
  private int _loadedSkyTextureCount;
  private int _loadedEntityTextureCount;
  private DrawSpan[] _meshDrawSpans = Array.Empty<DrawSpan>();
  private DrawSpan[] _skyDrawSpans = Array.Empty<DrawSpan>();
  private DrawSpan[] _entityDrawSpans = Array.Empty<DrawSpan>();

  [DllImport("user32.dll")]
  private static extern short GetAsyncKeyState(int vKey);

  public int RenderVertexCount => _meshVertexCount;
  public int TextureCount => _loadedMapTextureCount;
  public int SkyVertexCount => _skyVertexCount;
  public int EntityVertexCount => _entityVertexCount;
  public int SkyTextureCount => _loadedSkyTextureCount;
  public int EntityTextureCount => _loadedEntityTextureCount;

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
  public bool ShowCollisionOverlay
  {
    get => _showCollisionOverlay;
    set => _showCollisionOverlay = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool ShowGrid
  {
    get => _showGrid;
    set => _showGrid = value;
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
    _meshUniformUvChannel = GL.GetUniformLocation(_meshProgram, "uUvChannel");
    _meshUniformLayerColor = GL.GetUniformLocation(_meshProgram, "uLayerColor");
    _meshUniformUseVertexLighting = GL.GetUniformLocation(_meshProgram, "uUseVertexLighting");
    _skyProgram = CreateSkyShaderProgram();
    _skyUniformMvp = GL.GetUniformLocation(_skyProgram, "uMvp");
    _skyUniformTexture = GL.GetUniformLocation(_skyProgram, "uTexture0");
    _skyUniformExposure = GL.GetUniformLocation(_skyProgram, "uExposure");
    _checkerTexture = CreateCheckerTexture();

    _lineProgram = CreateLineShaderProgram();
    _lineUniformMvp = GL.GetUniformLocation(_lineProgram, "uMvp");
    _lineUniformColor = GL.GetUniformLocation(_lineProgram, "uColor");
    _wallProgram = CreateWallShaderProgram();
    _wallUniformMvp = GL.GetUniformLocation(_wallProgram, "uMvp");
    _wallUniformColor = GL.GetUniformLocation(_wallProgram, "uColor");

    _clipDebugProgram = CreateClipDebugShaderProgram();

    _meshVao = GL.GenVertexArray();
    _meshVbo = GL.GenBuffer();
    ConfigureMeshVao(_meshVao, _meshVbo);
    _skyVao = GL.GenVertexArray();
    _skyVbo = GL.GenBuffer();
    ConfigureMeshVao(_skyVao, _skyVbo);
    _entityVao = GL.GenVertexArray();
    _entityVbo = GL.GenBuffer();
    ConfigureMeshVao(_entityVao, _entityVbo);

    _lineVao = GL.GenVertexArray();
    _lineVbo = GL.GenBuffer();
    GL.BindVertexArray(_lineVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _lineVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);

    _wallVao = GL.GenVertexArray();
    _wallVbo = GL.GenBuffer();
    GL.BindVertexArray(_wallVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _wallVbo);
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

    if (_skyVertexCount > 0)
    {
      Matrix4 skyView = view;
      skyView.M41 = 0f;
      skyView.M42 = 0f;
      skyView.M43 = 0f;
      Matrix4 skyMvp = skyView * projection;
      GL.UseProgram(_skyProgram);
      GL.Uniform1(_skyUniformTexture, 0);
      GL.Uniform1(_skyUniformExposure, SkyExposure);
      GL.ActiveTexture(TextureUnit.Texture0);
      GL.Disable(EnableCap.DepthTest);
      GL.Disable(EnableCap.CullFace);
      GL.DepthMask(false);
      DrawSkyMesh(_skyVao, _skyVertexCount, _skyDrawSpans, ref skyMvp);
      GL.Disable(EnableCap.Blend);
      GL.DepthMask(true);
      GL.Enable(EnableCap.CullFace);
    }

    if (_meshVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_meshProgram);
      GL.Uniform1(_meshUniformTexture, 0);
      GL.ActiveTexture(TextureUnit.Texture0);
      DrawMesh(_meshVao, _meshVertexCount, _meshDrawSpans, ref mvp);
    }

    if (_entityVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_meshProgram);
      GL.Uniform1(_meshUniformTexture, 0);
      GL.ActiveTexture(TextureUnit.Texture0);
      DrawMesh(_entityVao, _entityVertexCount, _entityDrawSpans, ref mvp);
    }

    if (_showCollisionOverlay && _wallVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.Disable(EnableCap.CullFace);
      GL.Enable(EnableCap.Blend);
      GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
      GL.UseProgram(_wallProgram);
      GL.UniformMatrix4(_wallUniformMvp, true, ref mvp);
      GL.Uniform4(_wallUniformColor, new Vector4(0.30f, 0.58f, 0.95f, 0.30f));
      GL.BindVertexArray(_wallVao);
      GL.DrawArrays(PrimitiveType.Triangles, 0, _wallVertexCount);
      GL.Disable(EnableCap.Blend);
      GL.Enable(EnableCap.CullFace);
    }

    if (_showGrid && _lineVertexCount > 0)
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
      _skyVertexCount = 0;
      _entityVertexCount = 0;
      _wallVertexCount = 0;
      _lineVertexCount = 0;
      _meshDrawSpans = Array.Empty<DrawSpan>();
      _skyDrawSpans = Array.Empty<DrawSpan>();
      _entityDrawSpans = Array.Empty<DrawSpan>();
      ReleaseMapTextures();
      ReleaseSkyTextures();
      ReleaseEntityTextures();
      return;
    }

    UploadMapMesh(_map);

    UploadMeshLayer(
      _skyVao,
      _skyVbo,
      _map.SkyRenderVertices,
      _map.SkyMaterialSpans,
      _map.SkyMaterialSurfaceIds,
      _map.SkyMaterialAlphaTypes,
      _map.SkySurfaceTextures,
      _skyTextures,
      ref _skyVertexCount,
      ref _skyDrawSpans,
      ref _loadedSkyTextureCount);

    UploadMeshLayer(
      _entityVao,
      _entityVbo,
      _map.EntityRenderVertices,
      _map.EntityMaterialSpans,
      _map.EntityMaterialSurfaceIds,
      _map.EntityMaterialAlphaTypes,
      _map.EntitySurfaceTextures,
      _entityTextures,
      ref _entityVertexCount,
      ref _entityDrawSpans,
      ref _loadedEntityTextureCount,
      _mapSurfaceToTexture);

    Vector3[] wallVertices = BuildCollisionWalls(_map);
    _wallVertexCount = wallVertices.Length;
    GL.BindVertexArray(_wallVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _wallVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, wallVertices.Length * 12, wallVertices, BufferUsageHint.DynamicDraw);

    Vector3[] lineVertices = BuildOverlayLines(_map);
    _lineVertexCount = lineVertices.Length;
    GL.BindVertexArray(_lineVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _lineVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, lineVertices.Length * 12, lineVertices, BufferUsageHint.DynamicDraw);
  }

  private void UploadMapMesh(LoadedMap map)
  {
    _meshVertexCount = map.BspRenderVertices.Length;
    float[] meshData = new float[_meshVertexCount * MeshFloatStride];
    for (int i = 0; i < _meshVertexCount; ++i)
    {
      int offset = i * MeshFloatStride;
      BspRenderVertex v = map.BspRenderVertices[i];
      meshData[offset] = v.Position.X;
      meshData[offset + 1] = v.Position.Y;
      meshData[offset + 2] = v.Position.Z;
      meshData[offset + 3] = v.Uv.X;
      meshData[offset + 4] = v.Uv.Y;
      meshData[offset + 5] = v.LightUv.X;
      meshData[offset + 6] = v.LightUv.Y;
      meshData[offset + 7] = v.Color.X;
      meshData[offset + 8] = v.Color.Y;
      meshData[offset + 9] = v.Color.Z;
    }

    GL.BindVertexArray(_meshVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _meshVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, meshData.Length * 4, meshData, BufferUsageHint.StaticDraw);

    ReleaseMapTextures();

    Dictionary<int, int> mapSurfaceToTexture = LoadTextureMap(map.SurfaceTextures, _mapTextures);
    Dictionary<int, int> lightMapToTexture = LoadTextureMap(map.LightmapTextures, _lightmapTextures);
    _loadedMapTextureCount = mapSurfaceToTexture.Count;

    _mapSurfaceToTexture.Clear();
    foreach ((int key, int value) in mapSurfaceToTexture)
    {
      _mapSurfaceToTexture[key] = value;
    }

    _lightMapIdToTexture.Clear();
    foreach ((int key, int value) in lightMapToTexture)
    {
      _lightMapIdToTexture[key] = value;
    }

    _meshDrawSpans = BuildMapDrawSpans(map, mapSurfaceToTexture, lightMapToTexture);
  }

  private Dictionary<int, int> LoadTextureMap(IEnumerable<R3TextureBlob> textureBlobs, List<int> textureHandles)
  {
    Dictionary<int, int> surfaceToTexture = new();
    foreach (R3TextureBlob textureBlob in textureBlobs)
    {
      if (textureBlob.SurfaceId < 0 || surfaceToTexture.ContainsKey(textureBlob.SurfaceId))
      {
        continue;
      }

      int textureId = CreateTextureFromDds(textureBlob.DdsBytes);
      if (textureId == 0)
      {
        continue;
      }

      surfaceToTexture[textureBlob.SurfaceId] = textureId;
      textureHandles.Add(textureId);
    }

    return surfaceToTexture;
  }

  private DrawSpan[] BuildMapDrawSpans(
    LoadedMap map,
    IReadOnlyDictionary<int, int> mapTextureMap,
    IReadOnlyDictionary<int, int> lightMapTextureMap)
  {
    List<DrawSpan> spans = new(Math.Max(16, map.BspMaterialSpans.Length * 2));

    for (int i = 0; i < map.BspMaterialSpans.Length; ++i)
    {
      BspMaterialSpan meshSpan = map.BspMaterialSpans[i];
      if (meshSpan.VertexCount <= 0)
      {
        continue;
      }

      bool hasMaterial = meshSpan.MaterialId >= 0 && meshSpan.MaterialId < map.Materials.Length;
      bool lightMapEnabled = false;
      bool envBumpSecondLayer = false;
      if (hasMaterial)
      {
        MaterialDefinition material = map.Materials[meshSpan.MaterialId];
        lightMapEnabled = (material.MaterialFlags & MaterialFlagLightMap) != 0;
        if (material.Layers.Length > 1)
        {
          envBumpSecondLayer = (material.Layers[1].LayerFlags & MaterialLayerFlagEnvBump) != 0;
        }

        int layerCount = material.Layers.Length;
        for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
        {
          if (layerIndex > 0 && envBumpSecondLayer)
          {
            break;
          }

          MaterialLayerDefinition layer = material.Layers[layerIndex];
          int textureId = _checkerTexture;
          if (layer.SurfaceId > 0 && mapTextureMap.TryGetValue(layer.SurfaceId, out int mappedTexture))
          {
            textureId = mappedTexture;
          }

          uint alphaType = layer.AlphaType;
          if ((layer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0 && alphaType == BlendNone)
          {
            alphaType = BlendOnlyTransparency;
          }

          AddOrMergeDrawSpan(
            spans,
            new DrawSpan(
              meshSpan.StartVertex,
              meshSpan.VertexCount,
              textureId,
              alphaType,
              0,
              DecodeArgb(layer.Argb),
              true));
        }
      }
      else
      {
        int textureId = _checkerTexture;
        uint alphaType = 0;
        if (meshSpan.MaterialId >= 0 && meshSpan.MaterialId < map.MaterialSurfaceIds.Length)
        {
          int surfaceId = map.MaterialSurfaceIds[meshSpan.MaterialId];
          if (surfaceId > 0 && mapTextureMap.TryGetValue(surfaceId, out int mappedTexture))
          {
            textureId = mappedTexture;
          }

          if (meshSpan.MaterialId < map.MaterialAlphaTypes.Length)
          {
            alphaType = map.MaterialAlphaTypes[meshSpan.MaterialId];
          }
        }

        AddOrMergeDrawSpan(
          spans,
          new DrawSpan(
            meshSpan.StartVertex,
            meshSpan.VertexCount,
            textureId,
            alphaType,
            0,
            Vector4.One,
            true));
      }

      if (!lightMapEnabled || meshSpan.LightMapId < 0)
      {
        continue;
      }

      int lightMapTextureId = 0;
      if (!lightMapTextureMap.TryGetValue(meshSpan.LightMapId, out lightMapTextureId))
      {
        _ = lightMapTextureMap.TryGetValue(meshSpan.LightMapId + 1, out lightMapTextureId);
      }

      if (lightMapTextureId == 0)
      {
        continue;
      }

      AddOrMergeDrawSpan(
        spans,
        new DrawSpan(
          meshSpan.StartVertex,
          meshSpan.VertexCount,
          lightMapTextureId,
          BlendLightMap,
          1,
          Vector4.One,
          false));
    }

    if (spans.Count == 0 && _meshVertexCount > 0)
    {
      spans.Add(new DrawSpan(0, _meshVertexCount, _checkerTexture, 0, 0, Vector4.One, true));
    }

    return spans.ToArray();
  }

  private static Vector4 DecodeArgb(uint argb)
  {
    float a = ((argb >> 24) & 0xFF) / 255.0f;
    float r = ((argb >> 16) & 0xFF) / 255.0f;
    float g = ((argb >> 8) & 0xFF) / 255.0f;
    float b = (argb & 0xFF) / 255.0f;
    return new Vector4(r, g, b, a);
  }

  private void ConfigureMeshVao(int vao, int vbo)
  {
    GL.BindVertexArray(vao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, MeshByteStride, 0);
    GL.EnableVertexAttribArray(1);
    GL.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, false, MeshByteStride, 12);
    GL.EnableVertexAttribArray(2);
    GL.VertexAttribPointer(2, 2, VertexAttribPointerType.Float, false, MeshByteStride, 20);
    GL.EnableVertexAttribArray(3);
    GL.VertexAttribPointer(3, 3, VertexAttribPointerType.Float, false, MeshByteStride, 28);
  }

  private void DrawMesh(int vao, int vertexCount, DrawSpan[] spans, ref Matrix4 mvp)
  {
    if (vertexCount <= 0)
    {
      return;
    }

    GL.UseProgram(_meshProgram);
    GL.UniformMatrix4(_meshUniformMvp, true, ref mvp);
    GL.BindVertexArray(vao);
    if (spans.Length > 0)
    {
      for (int i = 0; i < spans.Length; ++i)
      {
        DrawSpan span = spans[i];
        ApplyBlendState(span.AlphaType);
        GL.Uniform1(_meshUniformUvChannel, span.UvChannel);
        GL.Uniform4(_meshUniformLayerColor, span.LayerColor);
        GL.Uniform1(_meshUniformUseVertexLighting, span.UseVertexLighting ? 1 : 0);
        GL.BindTexture(TextureTarget.Texture2D, span.TextureId);
        GL.DrawArrays(PrimitiveType.Triangles, span.StartVertex, span.VertexCount);
      }

      GL.Disable(EnableCap.Blend);
    }
    else
    {
      GL.Uniform1(_meshUniformUvChannel, 0);
      GL.Uniform4(_meshUniformLayerColor, Vector4.One);
      GL.Uniform1(_meshUniformUseVertexLighting, 1);
      GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
      GL.DrawArrays(PrimitiveType.Triangles, 0, vertexCount);
    }
  }

  private void DrawSkyMesh(int vao, int vertexCount, DrawSpan[] spans, ref Matrix4 mvp)
  {
    if (vertexCount <= 0)
    {
      return;
    }

    GL.UseProgram(_skyProgram);
    GL.UniformMatrix4(_skyUniformMvp, true, ref mvp);
    GL.BindVertexArray(vao);
    if (spans.Length > 0)
    {
      for (int i = 0; i < spans.Length; ++i)
      {
        DrawSpan span = spans[i];
        ApplyBlendState(span.AlphaType);
        GL.BindTexture(TextureTarget.Texture2D, span.TextureId);
        GL.DrawArrays(PrimitiveType.Triangles, span.StartVertex, span.VertexCount);
      }
    }
    else
    {
      ApplyBlendState(0);
      GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
      GL.DrawArrays(PrimitiveType.Triangles, 0, vertexCount);
    }
  }

  private static void ApplyBlendState(uint alphaType)
  {
    int blendMode = (int)(alphaType & 0x0F);
    if (blendMode == BlendNone)
    {
      GL.Disable(EnableCap.Blend);
      return;
    }

    GL.Enable(EnableCap.Blend);
    switch (blendMode)
    {
      case BlendOppa:
      case BlendOnlyTransparency:
        GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
        break;
      case BlendDefault:
        GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcColor);
        break;
      case BlendBright:
        GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.One);
        break;
      case BlendBackBright:
        GL.BlendFunc(BlendingFactor.One, BlendingFactor.OneMinusSrcColor);
        break;
      case BlendInvDefault:
        GL.BlendFunc(BlendingFactor.SrcColor, BlendingFactor.OneMinusSrcAlpha);
        break;
      case BlendInvBright:
        GL.BlendFunc(BlendingFactor.OneMinusSrcAlpha, BlendingFactor.Zero);
        break;
      case BlendLightMap:
        GL.BlendFunc(BlendingFactor.DstColor, BlendingFactor.SrcColor);
        break;
      case BlendInvLightMap:
        GL.BlendFunc(BlendingFactor.Zero, BlendingFactor.OneMinusSrcColor);
        break;
      case BlendShadow:
        GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcColor);
        break;
      default:
        GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
        break;
    }
  }

  private void UploadMeshLayer(
    int vao,
    int vbo,
    BspRenderVertex[] meshVertices,
    BspMaterialSpan[] materialSpans,
    int[] materialSurfaceIds,
    uint[] materialAlphaTypes,
    R3TextureBlob[] textureBlobs,
    List<int> textureHandles,
    ref int vertexCount,
    ref DrawSpan[] drawSpans,
    ref int loadedTextureCount,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture = null,
    Dictionary<int, int>? exportSurfaceToTexture = null)
  {
    vertexCount = meshVertices.Length;
    float[] meshData = new float[vertexCount * MeshFloatStride];
    for (int i = 0; i < vertexCount; ++i)
    {
      int offset = i * MeshFloatStride;
      BspRenderVertex v = meshVertices[i];
      meshData[offset] = v.Position.X;
      meshData[offset + 1] = v.Position.Y;
      meshData[offset + 2] = v.Position.Z;
      meshData[offset + 3] = v.Uv.X;
      meshData[offset + 4] = v.Uv.Y;
      meshData[offset + 5] = v.LightUv.X;
      meshData[offset + 6] = v.LightUv.Y;
      meshData[offset + 7] = v.Color.X;
      meshData[offset + 8] = v.Color.Y;
      meshData[offset + 9] = v.Color.Z;
    }

    GL.BindVertexArray(vao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
    GL.BufferData(BufferTarget.ArrayBuffer, meshData.Length * 4, meshData, BufferUsageHint.StaticDraw);

    ReleaseTextureList(textureHandles, ref loadedTextureCount, ref drawSpans);

    Dictionary<int, int> surfaceToTexture = new();
    for (int i = 0; i < textureBlobs.Length; ++i)
    {
      R3TextureBlob textureBlob = textureBlobs[i];
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
      textureHandles.Add(textureId);
    }

    loadedTextureCount = surfaceToTexture.Count;
    if (exportSurfaceToTexture != null)
    {
      exportSurfaceToTexture.Clear();
      foreach ((int key, int value) in surfaceToTexture)
      {
        exportSurfaceToTexture[key] = value;
      }
    }

    List<DrawSpan> spans = new(Math.Max(4, materialSpans.Length));

    for (int i = 0; i < materialSpans.Length; ++i)
    {
      BspMaterialSpan span = materialSpans[i];
      if (span.VertexCount <= 0)
      {
        continue;
      }

      int textureId = _checkerTexture;
      uint alphaType = 0;
      if (span.MaterialId >= 0 && span.MaterialId < materialSurfaceIds.Length)
      {
        int surfaceId = materialSurfaceIds[span.MaterialId];
        if (surfaceToTexture.TryGetValue(surfaceId, out int mappedTexture))
        {
          textureId = mappedTexture;
        }
        else if (fallbackSurfaceToTexture != null && fallbackSurfaceToTexture.TryGetValue(surfaceId, out int fallbackTexture))
        {
          textureId = fallbackTexture;
        }

        if (span.MaterialId < materialAlphaTypes.Length)
        {
          alphaType = materialAlphaTypes[span.MaterialId];
        }
      }

      AddOrMergeDrawSpan(
        spans,
        new DrawSpan(
          span.StartVertex,
          span.VertexCount,
          textureId,
          alphaType,
          0,
          Vector4.One,
          true));
    }

    if (spans.Count == 0 && vertexCount > 0)
    {
      spans.Add(new DrawSpan(0, vertexCount, _checkerTexture, 0, 0, Vector4.One, true));
    }

    drawSpans = spans.ToArray();
  }

  private static void AddOrMergeDrawSpan(List<DrawSpan> spans, DrawSpan value)
  {
    if (spans.Count == 0)
    {
      spans.Add(value);
      return;
    }

    DrawSpan last = spans[spans.Count - 1];
    if (last.TextureId == value.TextureId
      && last.AlphaType == value.AlphaType
      && last.UvChannel == value.UvChannel
      && last.LayerColor == value.LayerColor
      && last.UseVertexLighting == value.UseVertexLighting
      && last.StartVertex + last.VertexCount == value.StartVertex)
    {
      spans[spans.Count - 1] = new DrawSpan(
        last.StartVertex,
        last.VertexCount + value.VertexCount,
        last.TextureId,
        last.AlphaType,
        last.UvChannel,
        last.LayerColor,
        last.UseVertexLighting);
      return;
    }

    spans.Add(value);
  }

  private void ReleaseMapTextures()
  {
    _mapSurfaceToTexture.Clear();
    _lightMapIdToTexture.Clear();
    ReleaseTextureList(_mapTextures, ref _loadedMapTextureCount, ref _meshDrawSpans);
    int unused = 0;
    DrawSpan[] unusedSpans = Array.Empty<DrawSpan>();
    ReleaseTextureList(_lightmapTextures, ref unused, ref unusedSpans);
  }

  private void ReleaseSkyTextures()
  {
    ReleaseTextureList(_skyTextures, ref _loadedSkyTextureCount, ref _skyDrawSpans);
  }

  private void ReleaseEntityTextures()
  {
    ReleaseTextureList(_entityTextures, ref _loadedEntityTextureCount, ref _entityDrawSpans);
  }

  private static void ReleaseTextureList(List<int> textures, ref int loadedCount, ref DrawSpan[] spans)
  {
    loadedCount = 0;
    spans = Array.Empty<DrawSpan>();

    if (textures.Count == 0)
    {
      return;
    }

    for (int i = 0; i < textures.Count; ++i)
    {
      int texture = textures[i];
      if (texture != 0)
      {
        GL.DeleteTexture(texture);
      }
    }

    textures.Clear();
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

    if (TryCreateSoftwareDecodedDxtTexture(ddsBytes, out texture))
    {
      return texture;
    }

    return 0;
  }

  private static bool TryCreateSoftwareDecodedDxtTexture(byte[] ddsBytes, out int texture)
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
    int fourCc = BitConverter.ToInt32(ddsBytes, 84);
    if (width <= 0 || height <= 0)
    {
      return false;
    }

    byte[] rgba = fourCc switch
    {
      FourCcDxt1 => DecodeDxt1(ddsBytes, DdsHeaderSize, width, height),
      FourCcDxt3 => DecodeDxt3(ddsBytes, DdsHeaderSize, width, height),
      FourCcDxt5 => DecodeDxt5(ddsBytes, DdsHeaderSize, width, height),
      _ => Array.Empty<byte>(),
    };

    if (rgba.Length == 0)
    {
      return false;
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

  private static byte[] DecodeDxt1(byte[] bytes, int offset, int width, int height)
  {
    int blockWidth = (width + 3) / 4;
    int blockHeight = (height + 3) / 4;
    int required = blockWidth * blockHeight * 8;
    if (offset + required > bytes.Length)
    {
      return Array.Empty<byte>();
    }

    byte[] rgba = new byte[width * height * 4];
    int src = offset;
    for (int by = 0; by < blockHeight; ++by)
    {
      for (int bx = 0; bx < blockWidth; ++bx)
      {
        ushort c0 = BitConverter.ToUInt16(bytes, src);
        ushort c1 = BitConverter.ToUInt16(bytes, src + 2);
        uint indices = BitConverter.ToUInt32(bytes, src + 4);
        src += 8;

        uint[] colors = new uint[4];
        colors[0] = Rgb565ToRgba(c0, 255);
        colors[1] = Rgb565ToRgba(c1, 255);
        if (c0 > c1)
        {
          colors[2] = InterpolateColor(colors[0], colors[1], 2, 1);
          colors[3] = InterpolateColor(colors[0], colors[1], 1, 2);
        }
        else
        {
          colors[2] = InterpolateColor(colors[0], colors[1], 1, 1);
          colors[3] = 0x00000000;
        }

        WriteBlockPixels(rgba, width, height, bx, by, colors, indices, null);
      }
    }

    return rgba;
  }

  private static byte[] DecodeDxt3(byte[] bytes, int offset, int width, int height)
  {
    int blockWidth = (width + 3) / 4;
    int blockHeight = (height + 3) / 4;
    int required = blockWidth * blockHeight * 16;
    if (offset + required > bytes.Length)
    {
      return Array.Empty<byte>();
    }

    byte[] rgba = new byte[width * height * 4];
    int src = offset;
    for (int by = 0; by < blockHeight; ++by)
    {
      for (int bx = 0; bx < blockWidth; ++bx)
      {
        ulong alphaBits = BitConverter.ToUInt64(bytes, src);
        ushort c0 = BitConverter.ToUInt16(bytes, src + 8);
        ushort c1 = BitConverter.ToUInt16(bytes, src + 10);
        uint indices = BitConverter.ToUInt32(bytes, src + 12);
        src += 16;

        uint[] colors = new uint[4];
        colors[0] = Rgb565ToRgba(c0, 255);
        colors[1] = Rgb565ToRgba(c1, 255);
        colors[2] = InterpolateColor(colors[0], colors[1], 2, 1);
        colors[3] = InterpolateColor(colors[0], colors[1], 1, 2);

        byte[] alpha = new byte[16];
        for (int i = 0; i < 16; ++i)
        {
          byte a4 = (byte)((alphaBits >> (i * 4)) & 0x0F);
          alpha[i] = (byte)(a4 * 17);
        }

        WriteBlockPixels(rgba, width, height, bx, by, colors, indices, alpha);
      }
    }

    return rgba;
  }

  private static byte[] DecodeDxt5(byte[] bytes, int offset, int width, int height)
  {
    int blockWidth = (width + 3) / 4;
    int blockHeight = (height + 3) / 4;
    int required = blockWidth * blockHeight * 16;
    if (offset + required > bytes.Length)
    {
      return Array.Empty<byte>();
    }

    byte[] rgba = new byte[width * height * 4];
    int src = offset;
    for (int by = 0; by < blockHeight; ++by)
    {
      for (int bx = 0; bx < blockWidth; ++bx)
      {
        byte a0 = bytes[src];
        byte a1 = bytes[src + 1];
        ulong alphaBits = 0;
        for (int i = 0; i < 6; ++i)
        {
          alphaBits |= (ulong)bytes[src + 2 + i] << (8 * i);
        }

        ushort c0 = BitConverter.ToUInt16(bytes, src + 8);
        ushort c1 = BitConverter.ToUInt16(bytes, src + 10);
        uint indices = BitConverter.ToUInt32(bytes, src + 12);
        src += 16;

        byte[] alphaPalette = new byte[8];
        alphaPalette[0] = a0;
        alphaPalette[1] = a1;
        if (a0 > a1)
        {
          alphaPalette[2] = (byte)((6 * a0 + 1 * a1) / 7);
          alphaPalette[3] = (byte)((5 * a0 + 2 * a1) / 7);
          alphaPalette[4] = (byte)((4 * a0 + 3 * a1) / 7);
          alphaPalette[5] = (byte)((3 * a0 + 4 * a1) / 7);
          alphaPalette[6] = (byte)((2 * a0 + 5 * a1) / 7);
          alphaPalette[7] = (byte)((1 * a0 + 6 * a1) / 7);
        }
        else
        {
          alphaPalette[2] = (byte)((4 * a0 + 1 * a1) / 5);
          alphaPalette[3] = (byte)((3 * a0 + 2 * a1) / 5);
          alphaPalette[4] = (byte)((2 * a0 + 3 * a1) / 5);
          alphaPalette[5] = (byte)((1 * a0 + 4 * a1) / 5);
          alphaPalette[6] = 0;
          alphaPalette[7] = 255;
        }

        byte[] alpha = new byte[16];
        for (int i = 0; i < 16; ++i)
        {
          int code = (int)((alphaBits >> (i * 3)) & 0x07);
          alpha[i] = alphaPalette[code];
        }

        uint[] colors = new uint[4];
        colors[0] = Rgb565ToRgba(c0, 255);
        colors[1] = Rgb565ToRgba(c1, 255);
        if (c0 > c1)
        {
          colors[2] = InterpolateColor(colors[0], colors[1], 2, 1);
          colors[3] = InterpolateColor(colors[0], colors[1], 1, 2);
        }
        else
        {
          colors[2] = InterpolateColor(colors[0], colors[1], 1, 1);
          colors[3] = 0x00000000;
        }

        WriteBlockPixels(rgba, width, height, bx, by, colors, indices, alpha);
      }
    }

    return rgba;
  }

  private static void WriteBlockPixels(
    byte[] rgba,
    int width,
    int height,
    int blockX,
    int blockY,
    uint[] colors,
    uint colorIndices,
    byte[]? alphaOverride)
  {
    for (int py = 0; py < 4; ++py)
    {
      int y = blockY * 4 + py;
      if (y >= height)
      {
        continue;
      }

      for (int px = 0; px < 4; ++px)
      {
        int x = blockX * 4 + px;
        if (x >= width)
        {
          continue;
        }

        int pixelIndex = py * 4 + px;
        int colorIndex = (int)((colorIndices >> (2 * pixelIndex)) & 0x03);
        uint c = colors[colorIndex];
        byte r = (byte)(c & 0xFF);
        byte g = (byte)((c >> 8) & 0xFF);
        byte b = (byte)((c >> 16) & 0xFF);
        byte a = alphaOverride != null ? alphaOverride[pixelIndex] : (byte)((c >> 24) & 0xFF);

        int dst = (y * width + x) * 4;
        rgba[dst] = r;
        rgba[dst + 1] = g;
        rgba[dst + 2] = b;
        rgba[dst + 3] = a;
      }
    }
  }

  private static uint Rgb565ToRgba(ushort c, byte a)
  {
    int r = (c >> 11) & 0x1F;
    int g = (c >> 5) & 0x3F;
    int b = c & 0x1F;
    byte rr = (byte)((r * 255 + 15) / 31);
    byte gg = (byte)((g * 255 + 31) / 63);
    byte bb = (byte)((b * 255 + 15) / 31);
    return (uint)(rr | (gg << 8) | (bb << 16) | (a << 24));
  }

  private static uint InterpolateColor(uint c0, uint c1, int w0, int w1)
  {
    int sum = w0 + w1;
    byte r = (byte)((w0 * (c0 & 0xFF) + w1 * (c1 & 0xFF)) / sum);
    byte g = (byte)((w0 * ((c0 >> 8) & 0xFF) + w1 * ((c1 >> 8) & 0xFF)) / sum);
    byte b = (byte)((w0 * ((c0 >> 16) & 0xFF) + w1 * ((c1 >> 16) & 0xFF)) / sum);
    byte a = (byte)((w0 * ((c0 >> 24) & 0xFF) + w1 * ((c1 >> 24) & 0xFF)) / sum);
    return (uint)(r | (g << 8) | (b << 16) | (a << 24));
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
    List<Vector3> output = new(2048);
    AddGrid(map.Bounds, output);

    return output.ToArray();
  }

  private static Vector3[] BuildCollisionWalls(LoadedMap map)
  {
    List<Vector3> vertices = new(Math.Max(4096, map.CollisionLines.Length * 12));

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
      if (height < 0.01f)
      {
        continue;
      }

      Vector3 startTop = start + new Vector3(0f, height, 0f);
      Vector3 endTop = end + new Vector3(0f, height, 0f);
      if (!IsFinite(start) || !IsFinite(end) || !IsFinite(startTop) || !IsFinite(endTop))
      {
        continue;
      }

      AddTriangle(vertices, start, end, endTop);
      AddTriangle(vertices, start, endTop, startTop);
      AddTriangle(vertices, endTop, end, start);
      AddTriangle(vertices, startTop, endTop, start);
    }

    return vertices.ToArray();
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

  private static void AddTriangle(List<Vector3> output, Vector3 a, Vector3 b, Vector3 c)
  {
    output.Add(a);
    output.Add(b);
    output.Add(c);
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
      layout (location = 1) in vec2 aUv0;
      layout (location = 2) in vec2 aUv1;
      layout (location = 3) in vec3 aColor;

      uniform mat4 uMvp;
      out vec2 vUv0;
      out vec2 vUv1;
      out vec3 vColor;

      void main()
      {
        gl_Position = vec4(aPosition, 1.0) * uMvp;
        vUv0 = aUv0;
        vUv1 = aUv1;
        vColor = aColor;
      }
      """;

    const string fragmentShaderSource =
      """
      #version 330 core
      in vec2 vUv0;
      in vec2 vUv1;
      in vec3 vColor;
      uniform sampler2D uTexture0;
      uniform int uUvChannel;
      uniform vec4 uLayerColor;
      uniform int uUseVertexLighting;
      out vec4 FragColor;

      void main()
      {
        vec2 uv = (uUvChannel == 1) ? vUv1 : vUv0;
        vec4 tex = texture(uTexture0, uv);
        vec3 lit = (uUseVertexLighting != 0)
          ? clamp(vColor + vec3(0.18), vec3(0.0), vec3(1.0))
          : vec3(1.0);
        vec3 outColor = tex.rgb * uLayerColor.rgb * lit;
        float outAlpha = tex.a * uLayerColor.a;
        FragColor = vec4(outColor, outAlpha);
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "Mesh");
  }

  private static int CreateSkyShaderProgram()
  {
    const string vertexShaderSource =
      """
      #version 330 core
      layout (location = 0) in vec3 aPosition;
      layout (location = 1) in vec2 aUv;
      layout (location = 3) in vec3 aColor;

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
      uniform float uExposure;
      out vec4 FragColor;

      void main()
      {
        vec4 tex = texture(uTexture0, vUv);
        vec3 color = tex.rgb * vColor * uExposure;
        FragColor = vec4(clamp(color, vec3(0.0), vec3(1.0)), tex.a);
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "Sky");
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

  private static int CreateWallShaderProgram()
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
      uniform vec4 uColor;
      out vec4 FragColor;

      void main()
      {
        FragColor = uColor;
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "Wall");
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
        ReleaseSkyTextures();
        ReleaseEntityTextures();

        if (_checkerTexture != 0)
        {
          GL.DeleteTexture(_checkerTexture);
        }

        if (_meshVbo != 0)
        {
          GL.DeleteBuffer(_meshVbo);
        }
        if (_skyVbo != 0)
        {
          GL.DeleteBuffer(_skyVbo);
        }
        if (_entityVbo != 0)
        {
          GL.DeleteBuffer(_entityVbo);
        }
        if (_lineVbo != 0)
        {
          GL.DeleteBuffer(_lineVbo);
        }
        if (_wallVbo != 0)
        {
          GL.DeleteBuffer(_wallVbo);
        }
        if (_meshVao != 0)
        {
          GL.DeleteVertexArray(_meshVao);
        }
        if (_skyVao != 0)
        {
          GL.DeleteVertexArray(_skyVao);
        }
        if (_entityVao != 0)
        {
          GL.DeleteVertexArray(_entityVao);
        }
        if (_lineVao != 0)
        {
          GL.DeleteVertexArray(_lineVao);
        }
        if (_wallVao != 0)
        {
          GL.DeleteVertexArray(_wallVao);
        }

        if (_meshProgram != 0)
        {
          GL.DeleteProgram(_meshProgram);
        }
        if (_skyProgram != 0)
        {
          GL.DeleteProgram(_skyProgram);
        }
        if (_lineProgram != 0)
        {
          GL.DeleteProgram(_lineProgram);
        }
        if (_wallProgram != 0)
        {
          GL.DeleteProgram(_wallProgram);
        }
        if (_clipDebugProgram != 0)
        {
          GL.DeleteProgram(_clipDebugProgram);
        }
      }
    }

    base.Dispose(disposing);
  }

  private readonly record struct DrawSpan(
    int StartVertex,
    int VertexCount,
    int TextureId,
    uint AlphaType,
    int UvChannel,
    Vector4 LayerColor,
    bool UseVertexLighting);
}
