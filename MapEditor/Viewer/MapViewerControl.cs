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
  private const uint MaterialAlphaFlagZWrite = 0x00002000;
  private const uint ExtDummyFlagFog = 0x00000002;
  private const uint MaterialLayerFlagEnvBump = 0x00008000;
  private const uint MaterialLayerFlagUvEnv = 0x00000001;
  private const uint MaterialLayerFlagUvMetalFloor = 0x00000002;
  private const uint MaterialLayerFlagUvMetalWall = 0x00000008;
  private const uint MaterialLayerFlagUvMetal = MaterialLayerFlagUvMetalFloor | MaterialLayerFlagUvMetalWall;
  private const uint MaterialLayerFlagUvLava = 0x00000004;
  private const uint MaterialLayerFlagUvScrollU = 0x00000010;
  private const uint MaterialLayerFlagUvScrollV = 0x00000020;
  private const uint MaterialLayerFlagUvRotate = 0x00000040;
  private const uint MaterialLayerFlagUvScale = 0x00000080;
  private const uint MaterialLayerFlagAniAlphaFlicker = 0x00000400;
  private const uint MaterialLayerFlagAniTexture = 0x00000800;
  private const uint MaterialLayerFlagAniTileTexture = 0x00001000;
  private const float SkyExposure = 1.18f;
  private static readonly Vector3 DefaultClearColor = new(0.05f, 0.06f, 0.08f);
  private const int MaxDynamicLights = 4;

  private readonly GLControlWinForms _glControl;
  private readonly System.Windows.Forms.Timer _renderTimer;
  private readonly Stopwatch _frameClock;
  private readonly Stopwatch _animationClock;
  private readonly HashSet<Keys> _keysDown = [];
  private readonly List<int> _mapTextures = [];
  private readonly List<int> _lightmapTextures = [];
  private readonly List<int> _skyTextures = [];
  private readonly List<int> _entityTextures = [];
  private readonly Dictionary<int, int> _mapSurfaceToTexture = [];
  private readonly Dictionary<int, int> _skySurfaceToTexture = [];
  private readonly Dictionary<int, int> _entitySurfaceToTexture = [];
  private readonly Dictionary<int, int> _lightMapIdToTexture = [];

  private FreeCamera _camera = new(new Vector3(0f, 100f, 250f));
  private MapBounds _currentBounds = new(new Vector3(-100f, -100f, -100f), new Vector3(100f, 100f, 100f));
  private LoadedMap? _map;
  private float _moveSpeed = 350f;
  private float _sprintMultiplier = 1200f / 350f;
  private bool _showCollisionOverlay = true;
  private bool _showGrid;
  private bool _showSky = true;
  private bool _showR3eEntities = true;
  private bool _enableR3tTextures = true;
  private bool _enableR3mMaterials = true;
  private bool _enableR3xEnvironment = true;
  private bool _enableDynamicLighting = true;
  private bool _enableHeadlight = true;
  private float _headlightRadius = 4500f;
  private float _headlightIntensity = 0.85f;

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
  private int _meshUniformTexture1;
  private int _meshUniformUseEnvBump;
  private int _meshUniformBumpMatrix;
  private int _meshUniformUvChannel;
  private int _meshUniformLayerColor;
  private int _meshUniformUseUvTransform;
  private int _meshUniformUvTransform;
  private int _meshUniformAnimTime;
  private int _meshUniformLayerFlags;
  private int _meshUniformLavaWave;
  private int _meshUniformLavaSpeed;
  private int _meshUniformUvGenMode;
  private int _meshUniformMetalMode;
  private int _meshUniformMetalScale;
  private int _meshUniformSecondaryUseUvTransform;
  private int _meshUniformSecondaryUvTransform;
  private int _meshUniformSecondaryLayerFlags;
  private int _meshUniformSecondaryLavaWave;
  private int _meshUniformSecondaryLavaSpeed;
  private int _meshUniformSecondaryUvGenMode;
  private int _meshUniformSecondaryMetalMode;
  private int _meshUniformSecondaryMetalScale;
  private int _meshUniformView;
  private int _meshUniformUseVertexLighting;
  private int _meshUniformAmbientStrength;
  private int _meshUniformDirLightDirection;
  private int _meshUniformDirLightColor;
  private int _meshUniformPointLightCount;
  private int[] _meshUniformPointLightPosRadius = Array.Empty<int>();
  private int[] _meshUniformPointLightColorIntensity = Array.Empty<int>();
  private int _meshUniformCameraPos;
  private int _meshUniformFogEnabled;
  private int _meshUniformFogColor;
  private int _meshUniformFogStart;
  private int _meshUniformFogEnd;
  private int _skyUniformMvp;
  private int _skyUniformTexture;
  private int _skyUniformExposure;
  private int _skyUniformLayerColor;
  private int _skyUniformUseUvTransform;
  private int _skyUniformUvTransform;
  private int _skyUniformAnimTime;
  private int _skyUniformLayerFlags;
  private int _skyUniformLavaWave;
  private int _skyUniformLavaSpeed;
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
  private int _particleProgram;
  private int _particleVao;
  private int _particleVbo;
  private int _particleVertexCount;
  private int _particleUniformMvp;
  private int _particleUniformColor;
  private bool _showParticleMarkers = true;

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

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool ShowSky
  {
    get => _showSky;
    set => _showSky = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool ShowR3eEntities
  {
    get => _showR3eEntities;
    set => _showR3eEntities = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool EnableR3tTextures
  {
    get => _enableR3tTextures;
    set => _enableR3tTextures = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool EnableR3xEnvironment
  {
    get => _enableR3xEnvironment;
    set => _enableR3xEnvironment = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool EnableR3mMaterials
  {
    get => _enableR3mMaterials;
    set
    {
      if (_enableR3mMaterials == value)
      {
        return;
      }

      _enableR3mMaterials = value;
      RebuildGeometryForMaterialMode();
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool ShowParticleMarkers
  {
    get => _showParticleMarkers;
    set => _showParticleMarkers = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool EnableDynamicLighting
  {
    get => _enableDynamicLighting;
    set => _enableDynamicLighting = value;
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
    _animationClock = Stopwatch.StartNew();
    _renderTimer = new System.Windows.Forms.Timer
    {
      Interval = 16,
      Enabled = true,
    };
    _renderTimer.Tick += OnRenderTick;
  }

  private void RebuildGeometryForMaterialMode()
  {
    if (_glReady && _map != null)
    {
      _glControl.MakeCurrent();
      UploadGeometry();
    }

    _glControl.Invalidate();
  }

  public void SetMap(LoadedMap map)
  {
    _map = map;
    _currentBounds = map.Bounds;
    _camera = CreateDefaultCamera(map);

    if (_glReady)
    {
      _glControl.MakeCurrent();
      UploadGeometry();
    }

    _glControl.Invalidate();
  }

  private static FreeCamera CreateDefaultCamera(LoadedMap map)
  {
    MapBounds bounds = map.Bounds;
    Vector3 center = bounds.Center;

    float highestY = GetHighestFiniteY(map, bounds.Max.Y);
    Vector3 size = bounds.Size;
    float maxAxis = MathF.Max(size.X, MathF.Max(size.Y, size.Z));
    if (maxAxis < 100f)
    {
      maxAxis = 100f;
    }

    float eyeHeightOffset = MathF.Max(50f, maxAxis * 0.06f);
    float backwardOffset = MathF.Max(120f, maxAxis * 0.12f);

    Vector3 focus = new(center.X, highestY, center.Z);
    Vector3 start = new(center.X, highestY + eyeHeightOffset, center.Z + backwardOffset);
    return FreeCamera.CreateLookingAt(start, focus);
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

  private static float GetHighestFiniteY(LoadedMap map, float fallbackY)
  {
    float highestY = float.NegativeInfinity;
    bool found = false;
    for (int i = 0; i < map.BspRenderVertices.Length; ++i)
    {
      Vector3 position = map.BspRenderVertices[i].Position;
      if (!IsFinite(position))
      {
        continue;
      }

      if (!found || position.Y > highestY)
      {
        highestY = position.Y;
        found = true;
      }
    }

    if (!found || !float.IsFinite(highestY))
    {
      highestY = fallbackY;
    }

    if (!float.IsFinite(highestY))
    {
      highestY = 0f;
    }

    return highestY;
  }

  private void OnGlLoad(object? sender, EventArgs e)
  {
    _glControl.MakeCurrent();
    GL.ClearColor(DefaultClearColor.X, DefaultClearColor.Y, DefaultClearColor.Z, 1f);
    GL.Enable(EnableCap.DepthTest);
    GL.Enable(EnableCap.CullFace);
    GL.CullFace(TriangleFace.Back);

    _meshProgram = CreateMeshShaderProgram();
    _meshUniformMvp = GL.GetUniformLocation(_meshProgram, "uMvp");
    _meshUniformTexture = GL.GetUniformLocation(_meshProgram, "uTexture0");
    _meshUniformTexture1 = GL.GetUniformLocation(_meshProgram, "uTexture1");
    _meshUniformUseEnvBump = GL.GetUniformLocation(_meshProgram, "uUseEnvBump");
    _meshUniformBumpMatrix = GL.GetUniformLocation(_meshProgram, "uBumpMatrix");
    _meshUniformUvChannel = GL.GetUniformLocation(_meshProgram, "uUvChannel");
    _meshUniformLayerColor = GL.GetUniformLocation(_meshProgram, "uLayerColor");
    _meshUniformUseUvTransform = GL.GetUniformLocation(_meshProgram, "uUseUvTransform");
    _meshUniformUvTransform = GL.GetUniformLocation(_meshProgram, "uUvTransform");
    _meshUniformAnimTime = GL.GetUniformLocation(_meshProgram, "uAnimTime");
    _meshUniformLayerFlags = GL.GetUniformLocation(_meshProgram, "uLayerFlags");
    _meshUniformLavaWave = GL.GetUniformLocation(_meshProgram, "uLavaWave");
    _meshUniformLavaSpeed = GL.GetUniformLocation(_meshProgram, "uLavaSpeed");
    _meshUniformUvGenMode = GL.GetUniformLocation(_meshProgram, "uUvGenMode");
    _meshUniformMetalMode = GL.GetUniformLocation(_meshProgram, "uMetalMode");
    _meshUniformMetalScale = GL.GetUniformLocation(_meshProgram, "uMetalScale");
    _meshUniformSecondaryUseUvTransform = GL.GetUniformLocation(_meshProgram, "uSecondaryUseUvTransform");
    _meshUniformSecondaryUvTransform = GL.GetUniformLocation(_meshProgram, "uSecondaryUvTransform");
    _meshUniformSecondaryLayerFlags = GL.GetUniformLocation(_meshProgram, "uSecondaryLayerFlags");
    _meshUniformSecondaryLavaWave = GL.GetUniformLocation(_meshProgram, "uSecondaryLavaWave");
    _meshUniformSecondaryLavaSpeed = GL.GetUniformLocation(_meshProgram, "uSecondaryLavaSpeed");
    _meshUniformSecondaryUvGenMode = GL.GetUniformLocation(_meshProgram, "uSecondaryUvGenMode");
    _meshUniformSecondaryMetalMode = GL.GetUniformLocation(_meshProgram, "uSecondaryMetalMode");
    _meshUniformSecondaryMetalScale = GL.GetUniformLocation(_meshProgram, "uSecondaryMetalScale");
    _meshUniformView = GL.GetUniformLocation(_meshProgram, "uView");
    _meshUniformUseVertexLighting = GL.GetUniformLocation(_meshProgram, "uUseVertexLighting");
    _meshUniformAmbientStrength = GL.GetUniformLocation(_meshProgram, "uAmbientStrength");
    _meshUniformDirLightDirection = GL.GetUniformLocation(_meshProgram, "uDirLightDirection");
    _meshUniformDirLightColor = GL.GetUniformLocation(_meshProgram, "uDirLightColor");
    _meshUniformPointLightCount = GL.GetUniformLocation(_meshProgram, "uPointLightCount");
    _meshUniformCameraPos = GL.GetUniformLocation(_meshProgram, "uCameraPos");
    _meshUniformFogEnabled = GL.GetUniformLocation(_meshProgram, "uFogEnabled");
    _meshUniformFogColor = GL.GetUniformLocation(_meshProgram, "uFogColor");
    _meshUniformFogStart = GL.GetUniformLocation(_meshProgram, "uFogStart");
    _meshUniformFogEnd = GL.GetUniformLocation(_meshProgram, "uFogEnd");
    _meshUniformPointLightPosRadius = new int[MaxDynamicLights];
    _meshUniformPointLightColorIntensity = new int[MaxDynamicLights];
    for (int i = 0; i < MaxDynamicLights; ++i)
    {
      _meshUniformPointLightPosRadius[i] = GL.GetUniformLocation(_meshProgram, $"uPointLightPosRadius[{i}]");
      _meshUniformPointLightColorIntensity[i] = GL.GetUniformLocation(_meshProgram, $"uPointLightColorIntensity[{i}]");
    }
    _skyProgram = CreateSkyShaderProgram();
    _skyUniformMvp = GL.GetUniformLocation(_skyProgram, "uMvp");
    _skyUniformTexture = GL.GetUniformLocation(_skyProgram, "uTexture0");
    _skyUniformExposure = GL.GetUniformLocation(_skyProgram, "uExposure");
    _skyUniformLayerColor = GL.GetUniformLocation(_skyProgram, "uLayerColor");
    _skyUniformUseUvTransform = GL.GetUniformLocation(_skyProgram, "uUseUvTransform");
    _skyUniformUvTransform = GL.GetUniformLocation(_skyProgram, "uUvTransform");
    _skyUniformAnimTime = GL.GetUniformLocation(_skyProgram, "uAnimTime");
    _skyUniformLayerFlags = GL.GetUniformLocation(_skyProgram, "uLayerFlags");
    _skyUniformLavaWave = GL.GetUniformLocation(_skyProgram, "uLavaWave");
    _skyUniformLavaSpeed = GL.GetUniformLocation(_skyProgram, "uLavaSpeed");
    _checkerTexture = CreateCheckerTexture();

    _lineProgram = CreateLineShaderProgram();
    _lineUniformMvp = GL.GetUniformLocation(_lineProgram, "uMvp");
    _lineUniformColor = GL.GetUniformLocation(_lineProgram, "uColor");
    _wallProgram = CreateWallShaderProgram();
    _wallUniformMvp = GL.GetUniformLocation(_wallProgram, "uMvp");
    _wallUniformColor = GL.GetUniformLocation(_wallProgram, "uColor");
    _particleProgram = CreateParticleShaderProgram();
    _particleUniformMvp = GL.GetUniformLocation(_particleProgram, "uMvp");
    _particleUniformColor = GL.GetUniformLocation(_particleProgram, "uColor");

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
    _particleVao = GL.GenVertexArray();
    _particleVbo = GL.GenBuffer();
    GL.BindVertexArray(_particleVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _particleVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);
    GL.Enable(EnableCap.ProgramPointSize);

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
    Vector3 clearColor = GetMapClearColor();
    GL.ClearColor(clearColor.X, clearColor.Y, clearColor.Z, 1.0f);
    GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

    float aspect = Math.Max(0.0001f, (float)_glControl.ClientSize.Width / Math.Max(1f, _glControl.ClientSize.Height));
    Matrix4 projection = Matrix4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(65f), aspect, 0.1f, 200000f);
    Matrix4 view = _camera.GetViewMatrix();
    Matrix4 mvp = view * projection;

    if (_showSky && _skyVertexCount > 0)
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
      DrawSkyMesh(_skyVao, _skyVertexCount, _skyDrawSpans, ref skyMvp, _skySurfaceToTexture, null);
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
      DrawMesh(_meshVao, _meshVertexCount, _meshDrawSpans, ref mvp, ref view, _mapSurfaceToTexture, null);
    }

    if (_showR3eEntities && _entityVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_meshProgram);
      GL.Uniform1(_meshUniformTexture, 0);
      GL.ActiveTexture(TextureUnit.Texture0);
      DrawMesh(_entityVao, _entityVertexCount, _entityDrawSpans, ref mvp, ref view, _entitySurfaceToTexture, _mapSurfaceToTexture);
    }

    if (_showR3eEntities && _showParticleMarkers && _particleVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.Enable(EnableCap.Blend);
      GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.One);
      GL.UseProgram(_particleProgram);
      GL.UniformMatrix4(_particleUniformMvp, true, ref mvp);
      GL.Uniform4(_particleUniformColor, new Vector4(0.45f, 0.78f, 1.0f, 0.78f));
      GL.BindVertexArray(_particleVao);
      GL.DrawArrays(PrimitiveType.Points, 0, _particleVertexCount);
      GL.Disable(EnableCap.Blend);
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

  private Vector3 GetMapClearColor()
  {
    if (_map == null || !_enableR3xEnvironment)
    {
      return DefaultClearColor;
    }

    MapEnvironmentSettings env = _map.Environment;
    if (!env.FogEnabled)
    {
      return DefaultClearColor;
    }

    Vector3 fog = env.FogColor;
    if (!IsFinite(fog))
    {
      return DefaultClearColor;
    }

    return new Vector3(
      Math.Clamp(fog.X, 0.0f, 1.0f),
      Math.Clamp(fog.Y, 0.0f, 1.0f),
      Math.Clamp(fog.Z, 0.0f, 1.0f));
  }

  private void UploadGeometry()
  {
    if (_map == null)
    {
      _meshVertexCount = 0;
      _skyVertexCount = 0;
      _entityVertexCount = 0;
      _particleVertexCount = 0;
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
      _map.SkyMaterials,
      _map.SkyMaterialSurfaceIds,
      _map.SkyMaterialAlphaTypes,
      _map.SkySurfaceTextures,
      _skyTextures,
      ref _skyVertexCount,
      ref _skyDrawSpans,
      ref _loadedSkyTextureCount,
      null,
      _skySurfaceToTexture);

    UploadMeshLayer(
      _entityVao,
      _entityVbo,
      _map.EntityRenderVertices,
      _map.EntityMaterialSpans,
      _map.EntityMaterials,
      _map.EntityMaterialSurfaceIds,
      _map.EntityMaterialAlphaTypes,
      _map.EntitySurfaceTextures,
      _entityTextures,
      ref _entityVertexCount,
      ref _entityDrawSpans,
      ref _loadedEntityTextureCount,
      _mapSurfaceToTexture,
      _entitySurfaceToTexture);

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

    _particleVertexCount = _map.ParticleInstancePositions.Length;
    GL.BindVertexArray(_particleVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _particleVbo);
    GL.BufferData(
      BufferTarget.ArrayBuffer,
      _particleVertexCount * 12,
      _map.ParticleInstancePositions,
      BufferUsageHint.DynamicDraw);
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

      bool hasMaterial = _enableR3mMaterials
        && meshSpan.MaterialId >= 0
        && meshSpan.MaterialId < map.Materials.Length;
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

        if (envBumpSecondLayer && material.Layers.Length > 1)
        {
          MaterialLayerDefinition baseLayer = material.Layers[0];
          MaterialLayerDefinition bumpLayer = material.Layers[1];
          int baseTextureId = ResolveLayerTextureId(baseLayer.SurfaceId, mapTextureMap, null);
          int bumpTextureId = ResolveLayerTextureId(bumpLayer.SurfaceId, mapTextureMap, null);
          uint alphaType = baseLayer.AlphaType;
          if ((baseLayer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0 && alphaType == BlendNone)
          {
            alphaType = BlendOnlyTransparency;
          }

          AddOrMergeDrawSpan(
            spans,
            new DrawSpan(
              meshSpan.StartVertex,
              meshSpan.VertexCount,
              baseTextureId,
              bumpTextureId,
              alphaType,
              0,
              DecodeArgb(baseLayer.Argb),
              true,
              true,
              baseLayer,
              true,
              bumpLayer));
        }
        else
        {
        int layerCount = material.Layers.Length;
        for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
        {
          MaterialLayerDefinition layer = material.Layers[layerIndex];
          int textureId = ResolveLayerTextureId(layer.SurfaceId, mapTextureMap, null);

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
              0,
              alphaType,
              0,
              DecodeArgb(layer.Argb),
              true,
              true,
              layer,
              false,
              default));
        }
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
            0,
            alphaType,
            0,
            Vector4.One,
            true,
            false,
            default,
            false,
            default));
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
          0,
          BlendLightMap,
          1,
          Vector4.One,
          false,
          false,
          default,
          false,
          default));
    }

    if (spans.Count == 0 && _meshVertexCount > 0)
    {
      spans.Add(new DrawSpan(0, _meshVertexCount, _checkerTexture, 0, 0, 0, Vector4.One, true, false, default, false, default));
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

  private int ResolveLayerTextureId(
    int surfaceId,
    IReadOnlyDictionary<int, int> textureMap,
    IReadOnlyDictionary<int, int>? fallbackTextureMap)
  {
    if (surfaceId > 0 && textureMap.TryGetValue(surfaceId, out int textureId))
    {
      return textureId;
    }

    if (surfaceId > 0 && fallbackTextureMap != null && fallbackTextureMap.TryGetValue(surfaceId, out int fallbackTextureId))
    {
      return fallbackTextureId;
    }

    return _checkerTexture;
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

  private void DrawMesh(
    int vao,
    int vertexCount,
    DrawSpan[] spans,
    ref Matrix4 mvp,
    ref Matrix4 view,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture)
  {
    if (vertexCount <= 0)
    {
      return;
    }

    float animTime = (float)_animationClock.Elapsed.TotalSeconds;
    GL.UseProgram(_meshProgram);
    GL.UniformMatrix4(_meshUniformMvp, true, ref mvp);
    if (_meshUniformView >= 0)
    {
      GL.UniformMatrix4(_meshUniformView, true, ref view);
    }

    if (_meshUniformTexture >= 0)
    {
      GL.Uniform1(_meshUniformTexture, 0);
    }

    if (_meshUniformTexture1 >= 0)
    {
      GL.Uniform1(_meshUniformTexture1, 1);
    }

    ApplyDynamicLightingUniforms();
    ApplyMapEnvironmentUniforms();
    if (_meshUniformAnimTime >= 0)
    {
      GL.Uniform1(_meshUniformAnimTime, animTime);
    }

    GL.BindVertexArray(vao);
    if (spans.Length > 0)
    {
      for (int i = 0; i < spans.Length; ++i)
      {
        DrawSpan span = spans[i];
        int blendMode = (int)(span.AlphaType & 0x0F);
        if (!_enableR3tTextures && (blendMode == BlendLightMap || blendMode == BlendInvLightMap))
        {
          continue;
        }

        LayerAnimationState layerState = EvaluateLayerAnimation(span, animTime, surfaceToTexture, fallbackSurfaceToTexture);
        LayerAnimationState secondaryLayerState = span.HasSecondaryLayerDefinition
          ? EvaluateLayerAnimation(
            span.SecondaryTextureId,
            Vector4.One,
            span.HasSecondaryLayerDefinition,
            span.SecondaryLayer,
            animTime,
            surfaceToTexture,
            fallbackSurfaceToTexture)
          : new LayerAnimationState(_checkerTexture, Vector4.One, false, Matrix3.Identity, 0, 0.0f, 0.0f, 0, 0, 0.0f);

        int primaryTextureId = _enableR3tTextures ? layerState.TextureId : _checkerTexture;
        int secondaryTextureId = _enableR3tTextures ? secondaryLayerState.TextureId : _checkerTexture;
        bool useEnvBump = _enableR3mMaterials && _enableR3tTextures && span.HasSecondaryLayerDefinition;

        ApplyBlendState(span.AlphaType);
        bool zWriteEnabled = (span.AlphaType & MaterialAlphaFlagZWrite) != 0 || (span.AlphaType & 0x0F) == BlendNone;
        GL.DepthMask(zWriteEnabled);
        GL.Uniform1(_meshUniformUvChannel, span.UvChannel);
        GL.Uniform4(_meshUniformLayerColor, layerState.LayerColor);
        if (_meshUniformUseUvTransform >= 0)
        {
          GL.Uniform1(_meshUniformUseUvTransform, layerState.UseUvTransform ? 1 : 0);
        }

        if (_meshUniformUvTransform >= 0)
        {
          Matrix3 uvTransform = layerState.UvTransform;
          GL.UniformMatrix3(_meshUniformUvTransform, true, ref uvTransform);
        }

        if (_meshUniformLayerFlags >= 0)
        {
          GL.Uniform1(_meshUniformLayerFlags, (int)layerState.LayerFlags);
        }

        if (_meshUniformLavaWave >= 0)
        {
          GL.Uniform1(_meshUniformLavaWave, layerState.LavaWave);
        }

        if (_meshUniformLavaSpeed >= 0)
        {
          GL.Uniform1(_meshUniformLavaSpeed, layerState.LavaSpeed);
        }

        if (_meshUniformUvGenMode >= 0)
        {
          GL.Uniform1(_meshUniformUvGenMode, layerState.UvGenMode);
        }

        if (_meshUniformMetalMode >= 0)
        {
          GL.Uniform1(_meshUniformMetalMode, layerState.MetalMode);
        }

        if (_meshUniformMetalScale >= 0)
        {
          GL.Uniform1(_meshUniformMetalScale, layerState.MetalScale);
        }

        if (_meshUniformUseEnvBump >= 0)
        {
          GL.Uniform1(_meshUniformUseEnvBump, useEnvBump ? 1 : 0);
        }

        if (useEnvBump)
        {
          if (_meshUniformSecondaryUseUvTransform >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryUseUvTransform, secondaryLayerState.UseUvTransform ? 1 : 0);
          }

          if (_meshUniformSecondaryUvTransform >= 0)
          {
            Matrix3 secondaryUvTransform = secondaryLayerState.UvTransform;
            GL.UniformMatrix3(_meshUniformSecondaryUvTransform, true, ref secondaryUvTransform);
          }

          if (_meshUniformSecondaryLayerFlags >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryLayerFlags, (int)secondaryLayerState.LayerFlags);
          }

          if (_meshUniformSecondaryLavaWave >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryLavaWave, secondaryLayerState.LavaWave);
          }

          if (_meshUniformSecondaryLavaSpeed >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryLavaSpeed, secondaryLayerState.LavaSpeed);
          }

          if (_meshUniformSecondaryUvGenMode >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryUvGenMode, secondaryLayerState.UvGenMode);
          }

          if (_meshUniformSecondaryMetalMode >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryMetalMode, secondaryLayerState.MetalMode);
          }

          if (_meshUniformSecondaryMetalScale >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryMetalScale, secondaryLayerState.MetalScale);
          }

          if (_meshUniformBumpMatrix >= 0)
          {
            float theta = animTime * 3.0f;
            const float bumpStrength = 0.01f;
            float c = bumpStrength * MathF.Cos(theta);
            float s = bumpStrength * MathF.Sin(theta);
            Matrix2 bumpMatrix = new(c, -s, s, c);
            GL.UniformMatrix2(_meshUniformBumpMatrix, true, ref bumpMatrix);
          }

          GL.ActiveTexture(TextureUnit.Texture1);
          GL.BindTexture(TextureTarget.Texture2D, secondaryTextureId);
        }
        else
        {
          if (_meshUniformSecondaryUseUvTransform >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryUseUvTransform, 0);
          }

          if (_meshUniformSecondaryUvTransform >= 0)
          {
            Matrix3 identity = Matrix3.Identity;
            GL.UniformMatrix3(_meshUniformSecondaryUvTransform, true, ref identity);
          }

          if (_meshUniformSecondaryLayerFlags >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryLayerFlags, 0);
          }

          if (_meshUniformSecondaryLavaWave >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryLavaWave, 0.0f);
          }

          if (_meshUniformSecondaryLavaSpeed >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryLavaSpeed, 0.0f);
          }

          if (_meshUniformSecondaryUvGenMode >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryUvGenMode, 0);
          }

          if (_meshUniformSecondaryMetalMode >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryMetalMode, 0);
          }

          if (_meshUniformSecondaryMetalScale >= 0)
          {
            GL.Uniform1(_meshUniformSecondaryMetalScale, 0.0f);
          }

          if (_meshUniformBumpMatrix >= 0)
          {
            Matrix2 identityBump = Matrix2.Identity;
            GL.UniformMatrix2(_meshUniformBumpMatrix, true, ref identityBump);
          }

          GL.ActiveTexture(TextureUnit.Texture1);
          GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
        }

        GL.Uniform1(_meshUniformUseVertexLighting, span.UseVertexLighting ? 1 : 0);
        GL.ActiveTexture(TextureUnit.Texture0);
        GL.BindTexture(TextureTarget.Texture2D, primaryTextureId);
        GL.DrawArrays(PrimitiveType.Triangles, span.StartVertex, span.VertexCount);
      }

      GL.ActiveTexture(TextureUnit.Texture0);
      GL.DepthMask(true);
      GL.Disable(EnableCap.Blend);
    }
    else
    {
      GL.Uniform1(_meshUniformUvChannel, 0);
      GL.Uniform4(_meshUniformLayerColor, Vector4.One);
      if (_meshUniformUseUvTransform >= 0)
      {
        GL.Uniform1(_meshUniformUseUvTransform, 0);
      }

      if (_meshUniformUvTransform >= 0)
      {
        Matrix3 identity = Matrix3.Identity;
        GL.UniformMatrix3(_meshUniformUvTransform, true, ref identity);
      }

      if (_meshUniformLayerFlags >= 0)
      {
        GL.Uniform1(_meshUniformLayerFlags, 0);
      }

      if (_meshUniformLavaWave >= 0)
      {
        GL.Uniform1(_meshUniformLavaWave, 0.0f);
      }

      if (_meshUniformLavaSpeed >= 0)
      {
        GL.Uniform1(_meshUniformLavaSpeed, 0.0f);
      }

      if (_meshUniformUvGenMode >= 0)
      {
        GL.Uniform1(_meshUniformUvGenMode, 0);
      }

      if (_meshUniformMetalMode >= 0)
      {
        GL.Uniform1(_meshUniformMetalMode, 0);
      }

      if (_meshUniformMetalScale >= 0)
      {
        GL.Uniform1(_meshUniformMetalScale, 0.0f);
      }

      if (_meshUniformUseEnvBump >= 0)
      {
        GL.Uniform1(_meshUniformUseEnvBump, 0);
      }

      if (_meshUniformSecondaryUseUvTransform >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryUseUvTransform, 0);
      }

      if (_meshUniformSecondaryUvTransform >= 0)
      {
        Matrix3 identity = Matrix3.Identity;
        GL.UniformMatrix3(_meshUniformSecondaryUvTransform, true, ref identity);
      }

      if (_meshUniformSecondaryLayerFlags >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryLayerFlags, 0);
      }

      if (_meshUniformSecondaryLavaWave >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryLavaWave, 0.0f);
      }

      if (_meshUniformSecondaryLavaSpeed >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryLavaSpeed, 0.0f);
      }

      if (_meshUniformSecondaryUvGenMode >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryUvGenMode, 0);
      }

      if (_meshUniformSecondaryMetalMode >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryMetalMode, 0);
      }

      if (_meshUniformSecondaryMetalScale >= 0)
      {
        GL.Uniform1(_meshUniformSecondaryMetalScale, 0.0f);
      }

      if (_meshUniformBumpMatrix >= 0)
      {
        Matrix2 identityBump = Matrix2.Identity;
        GL.UniformMatrix2(_meshUniformBumpMatrix, true, ref identityBump);
      }

      GL.Uniform1(_meshUniformUseVertexLighting, 1);
      GL.ActiveTexture(TextureUnit.Texture1);
      GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
      GL.ActiveTexture(TextureUnit.Texture0);
      GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
      GL.DrawArrays(PrimitiveType.Triangles, 0, vertexCount);
    }
  }

  private void DrawSkyMesh(
    int vao,
    int vertexCount,
    DrawSpan[] spans,
    ref Matrix4 mvp,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture)
  {
    if (vertexCount <= 0)
    {
      return;
    }

    float animTime = (float)_animationClock.Elapsed.TotalSeconds;
    GL.UseProgram(_skyProgram);
    GL.UniformMatrix4(_skyUniformMvp, true, ref mvp);
    if (_skyUniformAnimTime >= 0)
    {
      GL.Uniform1(_skyUniformAnimTime, animTime);
    }

    GL.BindVertexArray(vao);
    if (spans.Length > 0)
    {
      for (int i = 0; i < spans.Length; ++i)
      {
        DrawSpan span = spans[i];
        LayerAnimationState layerState = EvaluateLayerAnimation(span, animTime, surfaceToTexture, fallbackSurfaceToTexture);
        ApplyBlendState(span.AlphaType);
        if (_skyUniformLayerColor >= 0)
        {
          GL.Uniform4(_skyUniformLayerColor, layerState.LayerColor);
        }

        if (_skyUniformUseUvTransform >= 0)
        {
          GL.Uniform1(_skyUniformUseUvTransform, layerState.UseUvTransform ? 1 : 0);
        }

        if (_skyUniformUvTransform >= 0)
        {
          Matrix3 uvTransform = layerState.UvTransform;
          GL.UniformMatrix3(_skyUniformUvTransform, true, ref uvTransform);
        }

        if (_skyUniformLayerFlags >= 0)
        {
          GL.Uniform1(_skyUniformLayerFlags, (int)layerState.LayerFlags);
        }

        if (_skyUniformLavaWave >= 0)
        {
          GL.Uniform1(_skyUniformLavaWave, layerState.LavaWave);
        }

        if (_skyUniformLavaSpeed >= 0)
        {
          GL.Uniform1(_skyUniformLavaSpeed, layerState.LavaSpeed);
        }

        int textureId = _enableR3tTextures ? layerState.TextureId : _checkerTexture;
        GL.BindTexture(TextureTarget.Texture2D, textureId);
        GL.DrawArrays(PrimitiveType.Triangles, span.StartVertex, span.VertexCount);
      }
    }
    else
    {
      ApplyBlendState(0);
      if (_skyUniformLayerColor >= 0)
      {
        GL.Uniform4(_skyUniformLayerColor, Vector4.One);
      }

      if (_skyUniformUseUvTransform >= 0)
      {
        GL.Uniform1(_skyUniformUseUvTransform, 0);
      }

      if (_skyUniformUvTransform >= 0)
      {
        Matrix3 identity = Matrix3.Identity;
        GL.UniformMatrix3(_skyUniformUvTransform, true, ref identity);
      }

      if (_skyUniformLayerFlags >= 0)
      {
        GL.Uniform1(_skyUniformLayerFlags, 0);
      }

      if (_skyUniformLavaWave >= 0)
      {
        GL.Uniform1(_skyUniformLavaWave, 0.0f);
      }

      if (_skyUniformLavaSpeed >= 0)
      {
        GL.Uniform1(_skyUniformLavaSpeed, 0.0f);
      }

      GL.BindTexture(TextureTarget.Texture2D, _checkerTexture);
      GL.DrawArrays(PrimitiveType.Triangles, 0, vertexCount);
    }
  }

  private LayerAnimationState EvaluateLayerAnimation(
    DrawSpan span,
    float animTime,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture)
  {
    return EvaluateLayerAnimation(
      span.TextureId,
      span.LayerColor,
      span.HasLayerDefinition,
      span.Layer,
      animTime,
      surfaceToTexture,
      fallbackSurfaceToTexture);
  }

  private LayerAnimationState EvaluateLayerAnimation(
    int baseTextureId,
    Vector4 baseLayerColor,
    bool hasLayerDefinition,
    MaterialLayerDefinition layer,
    float animTime,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture)
  {
    int textureId = baseTextureId;
    Vector4 layerColor = baseLayerColor;
    Matrix3 uvTransform = Matrix3.Identity;
    bool useUvTransform = false;
    uint layerFlags = 0;
    float lavaWave = 0.0f;
    float lavaSpeed = 0.0f;
    int uvGenMode = 0;
    int metalMode = 0;
    float metalScale = 0.0f;

    if (!hasLayerDefinition || !_enableR3mMaterials)
    {
      return new LayerAnimationState(textureId, layerColor, useUvTransform, uvTransform, layerFlags, lavaWave, lavaSpeed, uvGenMode, metalMode, metalScale);
    }

    layerFlags = layer.LayerFlags;
    textureId = ResolveAnimatedTextureId(textureId, layer, animTime, surfaceToTexture, fallbackSurfaceToTexture);
    if ((layer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0)
    {
      layerColor.W *= ComputeAnimatedAlpha(layer, animTime);
    }

    if ((layer.LayerFlags & MaterialLayerFlagUvLava) != 0)
    {
      lavaWave = FixedShortToFloat(layer.UvLavaWave);
      lavaSpeed = FixedShortToFloat((short)(layer.UvLavaSpeed * 3));
    }

    if ((layer.LayerFlags & MaterialLayerFlagUvMetal) != 0)
    {
      uvGenMode = 1;
      metalScale = 0.003f * FixedShortToFloat(layer.UvMetal);
      if (!float.IsFinite(metalScale) || MathF.Abs(metalScale) < 0.00001f)
      {
        metalScale = 0.003f;
      }

      if ((layer.LayerFlags & MaterialLayerFlagUvMetalFloor) != 0)
      {
        metalMode = 1;
      }
      else if ((layer.LayerFlags & MaterialLayerFlagUvMetalWall) != 0)
      {
        metalMode = 2;
      }
    }
    else if ((layer.LayerFlags & MaterialLayerFlagUvEnv) != 0)
    {
      uvGenMode = 2;
    }

    if (TryBuildUvTransform(layer, animTime, out Matrix3 animatedTransform))
    {
      useUvTransform = true;
      uvTransform = animatedTransform;
    }

    return new LayerAnimationState(textureId, layerColor, useUvTransform, uvTransform, layerFlags, lavaWave, lavaSpeed, uvGenMode, metalMode, metalScale);
  }

  private static int ResolveAnimatedTextureId(
    int fallbackTextureId,
    MaterialLayerDefinition layer,
    float animTime,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture)
  {
    if ((layer.LayerFlags & MaterialLayerFlagAniTileTexture) == 0 || layer.TileAniTextureCount <= 0 || layer.SurfaceId <= 0)
    {
      return fallbackTextureId;
    }

    int frameCount = layer.TileAniTextureCount;
    float speed = FixedShortToFloat(layer.AniTexSpeed);
    if (!float.IsFinite(speed) || MathF.Abs(speed) <= 0.0001f)
    {
      speed = 1.0f;
    }

    int frame = (int)(animTime * speed);
    if (frameCount > 0)
    {
      frame %= frameCount;
      if (frame < 0)
      {
        frame += frameCount;
      }
    }

    int targetSurfaceId = layer.SurfaceId + frame;
    if (surfaceToTexture != null && surfaceToTexture.TryGetValue(targetSurfaceId, out int textureId))
    {
      return textureId;
    }

    if (fallbackSurfaceToTexture != null && fallbackSurfaceToTexture.TryGetValue(targetSurfaceId, out int mappedFallbackTexture))
    {
      return mappedFallbackTexture;
    }

    return fallbackTextureId;
  }

  private static float ComputeAnimatedAlpha(MaterialLayerDefinition layer, float animTime)
  {
    float speed = FixedShortToFloat(layer.AniAlphaFlicker);
    ushort range = layer.AniAlphaFlickerRange;
    int start = (range >> 8) & 0xFF;
    int end = range & 0xFF;
    if (start == 0 && end == 0)
    {
      start = 0;
      end = 255;
    }

    int distance = Math.Abs(end - start);
    if (distance == 0)
    {
      return Math.Clamp(start / 255.0f, 0.0f, 1.0f);
    }

    float alpha;
    if (speed < 0.0f)
    {
      int pulse = (int)(animTime * -speed * 200.0f);
      int animated = start + (pulse % distance);
      alpha = animated / 255.0f;
    }
    else
    {
      float halfDistance = distance * 0.5f;
      alpha = (MathF.Sin(animTime * speed) * halfDistance + halfDistance + start) / 255.0f;
    }

    return Math.Clamp(alpha, 0.0f, 1.0f);
  }

  private static bool TryBuildUvTransform(MaterialLayerDefinition layer, float animTime, out Matrix3 uvTransform)
  {
    uvTransform = Matrix3.Identity;
    bool hasTransform = false;
    uint flags = layer.LayerFlags;

    if ((flags & MaterialLayerFlagAniTexture) != 0)
    {
      int frameNum = (int)FixedShortToFloat(layer.AniTexFrame);
      if (frameNum > 0)
      {
        float speed = FixedShortToFloat(layer.AniTexSpeed);
        int frame = (int)(speed * animTime);
        frame %= frameNum;
        if (frame < 0)
        {
          frame += frameNum;
        }

        int divU = GetAniTexDivU(frameNum);
        int divV = GetAniTexDivV(frameNum);
        float addU = (frame % divU) / (float)divU;
        float addV = ((frame + divU) / (float)divU - 1.0f) / divV;

        uvTransform = CreateUvScale(1.0f / divU, 1.0f / divV) * CreateUvTranslation(addU, addV);
        return true;
      }
    }

    if ((flags & MaterialLayerFlagUvRotate) != 0)
    {
      float angle = FixedShortToFloat(layer.UvRotate) * animTime;
      uvTransform = uvTransform
        * CreateUvTranslation(-0.5f, -0.5f)
        * CreateUvRotation(angle)
        * CreateUvTranslation(0.5f, 0.5f);
      hasTransform = true;
    }

    if ((flags & MaterialLayerFlagUvScale) != 0)
    {
      float start = FixedShortToFloat(layer.UvScaleStart);
      float end = FixedShortToFloat(layer.UvScaleEnd);
      float speed = FixedShortToFloat(layer.UvScaleSpeed);
      float scale = 1.0f;
      if (MathF.Abs(speed) > 0.0001f)
      {
        float phase = (animTime * speed / 6.0f) * (2.0f * MathF.PI);
        scale = (end - start) * ((MathF.Sin(phase) + 1.0f) * 0.5f) + start;
      }
      else
      {
        scale = start;
      }

      uvTransform = uvTransform
        * CreateUvTranslation(-0.5f, -0.5f)
        * CreateUvScale(scale, scale)
        * CreateUvTranslation(0.5f, 0.5f);
      hasTransform = true;
    }

    if ((flags & (MaterialLayerFlagUvScrollU | MaterialLayerFlagUvScrollV)) != 0)
    {
      float offsetU = FixedShortToFloat(layer.UvScrollU) * animTime / 4.0f;
      float offsetV = FixedShortToFloat(layer.UvScrollV) * animTime / 4.0f;
      uvTransform = uvTransform * CreateUvTranslation(offsetU, offsetV);
      hasTransform = true;
    }

    return hasTransform;
  }

  private static int GetAniTexDivU(int frameNum)
  {
    if (frameNum <= 4)
    {
      return 2;
    }

    if (frameNum <= 16)
    {
      return 4;
    }

    if (frameNum <= 64)
    {
      return 8;
    }

    return 16;
  }

  private static int GetAniTexDivV(int frameNum)
  {
    if (frameNum <= 2)
    {
      return 1;
    }

    if (frameNum <= 8)
    {
      return 2;
    }

    if (frameNum <= 32)
    {
      return 4;
    }

    return 8;
  }

  private static float FixedShortToFloat(short value)
  {
    return value / 256.0f;
  }

  private static Matrix3 CreateUvTranslation(float tx, float ty)
  {
    return new Matrix3(
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      tx, ty, 1.0f);
  }

  private static Matrix3 CreateUvScale(float sx, float sy)
  {
    return new Matrix3(
      sx, 0.0f, 0.0f,
      0.0f, sy, 0.0f,
      0.0f, 0.0f, 1.0f);
  }

  private static Matrix3 CreateUvRotation(float angleRadians)
  {
    float c = MathF.Cos(angleRadians);
    float s = MathF.Sin(angleRadians);
    return new Matrix3(
      c, s, 0.0f,
      -s, c, 0.0f,
      0.0f, 0.0f, 1.0f);
  }

  private void ApplyDynamicLightingUniforms()
  {
    if (!_enableDynamicLighting)
    {
      GL.Uniform1(_meshUniformAmbientStrength, 1.0f);
      GL.Uniform3(_meshUniformDirLightDirection, Vector3.UnitY);
      GL.Uniform3(_meshUniformDirLightColor, Vector3.Zero);
      GL.Uniform1(_meshUniformPointLightCount, 0);
      return;
    }

    // Approximate old R3 dynamic-light feel: moderate ambient + strong directional
    // light, plus a camera-follow point light for close-up inspection.
    GL.Uniform1(_meshUniformAmbientStrength, 0.44f);
    GL.Uniform3(_meshUniformDirLightDirection, Vector3.Normalize(new Vector3(-0.28f, -0.91f, -0.22f)));
    GL.Uniform3(_meshUniformDirLightColor, new Vector3(0.78f, 0.84f, 0.92f));

    Span<Vector4> posRadius = stackalloc Vector4[MaxDynamicLights];
    Span<Vector4> colorIntensity = stackalloc Vector4[MaxDynamicLights];
    int lightCount = 0;

    if (_enableHeadlight)
    {
      Vector3 headlightPos = _camera.Position + _camera.Forward * 160.0f;
      posRadius[lightCount] = new Vector4(headlightPos.X, headlightPos.Y, headlightPos.Z, _headlightRadius);
      colorIntensity[lightCount] = new Vector4(0.95f, 0.94f, 0.90f, _headlightIntensity);
      ++lightCount;
    }

    Vector3 mapCenter = _currentBounds.Center + new Vector3(0.0f, MathF.Max(350.0f, _currentBounds.Size.Y * 0.35f), 0.0f);
    float maxAxis = MathF.Max(_currentBounds.Size.X, MathF.Max(_currentBounds.Size.Y, _currentBounds.Size.Z));
    posRadius[lightCount] = new Vector4(mapCenter.X, mapCenter.Y, mapCenter.Z, MathF.Max(1200.0f, maxAxis * 0.85f));
    colorIntensity[lightCount] = new Vector4(0.30f, 0.36f, 0.44f, 0.35f);
    ++lightCount;

    GL.Uniform1(_meshUniformPointLightCount, lightCount);
    for (int i = 0; i < MaxDynamicLights; ++i)
    {
      Vector4 pr = i < lightCount ? posRadius[i] : Vector4.Zero;
      Vector4 ci = i < lightCount ? colorIntensity[i] : Vector4.Zero;
      if (_meshUniformPointLightPosRadius[i] >= 0)
      {
        GL.Uniform4(_meshUniformPointLightPosRadius[i], pr);
      }

      if (_meshUniformPointLightColorIntensity[i] >= 0)
      {
        GL.Uniform4(_meshUniformPointLightColorIntensity[i], ci);
      }
    }
  }

  private void ApplyMapEnvironmentUniforms()
  {
    if (_meshUniformCameraPos >= 0)
    {
      GL.Uniform3(_meshUniformCameraPos, _camera.Position);
    }

    if (_map == null || !_enableR3xEnvironment)
    {
      if (_meshUniformFogEnabled >= 0)
      {
        GL.Uniform1(_meshUniformFogEnabled, 0);
      }

      if (_meshUniformFogColor >= 0)
      {
        GL.Uniform3(_meshUniformFogColor, DefaultClearColor);
      }

      if (_meshUniformFogStart >= 0)
      {
        GL.Uniform1(_meshUniformFogStart, 5.0f);
      }

      if (_meshUniformFogEnd >= 0)
      {
        GL.Uniform1(_meshUniformFogEnd, 5000.0f);
      }

      return;
    }

    MapEnvironmentSettings env = _map.Environment;
    bool fogEnabled = env.FogEnabled && env.FogEnd > env.FogStart && env.FogStart >= 0.0f;
    float fogStart = fogEnabled ? env.FogStart : 5.0f;
    float fogEnd = fogEnabled ? env.FogEnd : 5000.0f;
    Vector3 fogColor = env.FogEnabled ? env.FogColor : DefaultClearColor;
    if (TryGetExtDummyFogOverride(out Vector3 extFogColor, out float extFogStart, out float extFogEnd))
    {
      fogEnabled = extFogEnd > extFogStart && extFogStart >= 0.0f;
      fogStart = fogEnabled ? extFogStart : 5.0f;
      fogEnd = fogEnabled ? extFogEnd : 5000.0f;
      fogColor = extFogColor;
    }

    fogColor.X = Math.Clamp(fogColor.X, 0.0f, 1.0f);
    fogColor.Y = Math.Clamp(fogColor.Y, 0.0f, 1.0f);
    fogColor.Z = Math.Clamp(fogColor.Z, 0.0f, 1.0f);

    if (_meshUniformFogEnabled >= 0)
    {
      GL.Uniform1(_meshUniformFogEnabled, fogEnabled ? 1 : 0);
    }

    if (_meshUniformFogColor >= 0)
    {
      GL.Uniform3(_meshUniformFogColor, fogColor);
    }

    if (_meshUniformFogStart >= 0)
    {
      GL.Uniform1(_meshUniformFogStart, fogStart);
    }

    if (_meshUniformFogEnd >= 0)
    {
      GL.Uniform1(_meshUniformFogEnd, fogEnd);
    }
  }

  private bool TryGetExtDummyFogOverride(out Vector3 fogColor, out float fogStart, out float fogEnd)
  {
    fogColor = DefaultClearColor;
    fogStart = 5.0f;
    fogEnd = 5000.0f;

    if (_map == null || _map.ExtDummies.Length == 0)
    {
      return false;
    }

    System.Numerics.Vector3 cameraPosition = new(_camera.Position.X, _camera.Position.Y, _camera.Position.Z);
    for (int i = 0; i < _map.ExtDummies.Length; ++i)
    {
      ExtDummyDefinition dummy = _map.ExtDummies[i];
      if ((dummy.Flags & ExtDummyFlagFog) == 0)
      {
        continue;
      }

      System.Numerics.Vector3 localPosition = System.Numerics.Vector3.Transform(cameraPosition, dummy.InverseTransform);
      if (!IsInsideExtDummyLocalBounds(localPosition, dummy.LocalMin, dummy.LocalMax))
      {
        continue;
      }

      uint argb = dummy.Arg0;
      fogColor = new Vector3(
        ((argb >> 16) & 0xFF) / 255.0f,
        ((argb >> 8) & 0xFF) / 255.0f,
        (argb & 0xFF) / 255.0f);

      fogStart = float.IsFinite(dummy.Arg1) ? dummy.Arg1 : 5.0f;
      fogEnd = float.IsFinite(dummy.Arg2) ? dummy.Arg2 : MathF.Max(fogStart + 1.0f, 5000.0f);
      if (fogEnd <= fogStart)
      {
        fogEnd = fogStart + 1.0f;
      }

      return true;
    }

    return false;
  }

  private static bool IsInsideExtDummyLocalBounds(System.Numerics.Vector3 point, Vector3 localMin, Vector3 localMax)
  {
    float minX = MathF.Min(localMin.X, localMax.X);
    float minY = MathF.Min(localMin.Y, localMax.Y);
    float minZ = MathF.Min(localMin.Z, localMax.Z);
    float maxX = MathF.Max(localMin.X, localMax.X);
    float maxY = MathF.Max(localMin.Y, localMax.Y);
    float maxZ = MathF.Max(localMin.Z, localMax.Z);

    return point.X >= minX && point.X <= maxX
      && point.Y >= minY && point.Y <= maxY
      && point.Z >= minZ && point.Z <= maxZ;
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
    MaterialDefinition[] materials,
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

      bool hasMaterial = _enableR3mMaterials
        && span.MaterialId >= 0
        && span.MaterialId < materials.Length;
      if (hasMaterial)
      {
        MaterialDefinition material = materials[span.MaterialId];
        bool envBumpSecondLayer = material.Layers.Length > 1
          && (material.Layers[1].LayerFlags & MaterialLayerFlagEnvBump) != 0;

        if (envBumpSecondLayer && material.Layers.Length > 1)
        {
          MaterialLayerDefinition baseLayer = material.Layers[0];
          MaterialLayerDefinition bumpLayer = material.Layers[1];
          int baseTextureId = ResolveLayerTextureId(baseLayer.SurfaceId, surfaceToTexture, fallbackSurfaceToTexture);
          int bumpTextureId = ResolveLayerTextureId(bumpLayer.SurfaceId, surfaceToTexture, fallbackSurfaceToTexture);

          uint alphaType = baseLayer.AlphaType;
          if ((baseLayer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0 && alphaType == BlendNone)
          {
            alphaType = BlendOnlyTransparency;
          }

          AddOrMergeDrawSpan(
            spans,
            new DrawSpan(
              span.StartVertex,
              span.VertexCount,
              baseTextureId,
              bumpTextureId,
              alphaType,
              0,
              DecodeArgb(baseLayer.Argb),
              true,
              true,
              baseLayer,
              true,
              bumpLayer));
        }
        else
        {
          for (int layerIndex = 0; layerIndex < material.Layers.Length; ++layerIndex)
          {
            MaterialLayerDefinition layer = material.Layers[layerIndex];
            int textureId = ResolveLayerTextureId(layer.SurfaceId, surfaceToTexture, fallbackSurfaceToTexture);

            uint alphaType = layer.AlphaType;
            if ((layer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0 && alphaType == BlendNone)
            {
              alphaType = BlendOnlyTransparency;
            }

            AddOrMergeDrawSpan(
              spans,
              new DrawSpan(
                span.StartVertex,
                span.VertexCount,
                textureId,
                0,
                alphaType,
                0,
                DecodeArgb(layer.Argb),
                true,
                true,
                layer,
                false,
                default));
          }
        }
      }
      else
      {
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
            0,
            alphaType,
            0,
            Vector4.One,
            true,
            false,
            default,
            false,
            default));
      }
    }

    if (spans.Count == 0 && vertexCount > 0)
    {
      spans.Add(new DrawSpan(0, vertexCount, _checkerTexture, 0, 0, 0, Vector4.One, true, false, default, false, default));
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
      && last.SecondaryTextureId == value.SecondaryTextureId
      && last.AlphaType == value.AlphaType
      && last.UvChannel == value.UvChannel
      && last.LayerColor == value.LayerColor
      && last.HasLayerDefinition == value.HasLayerDefinition
      && last.Layer == value.Layer
      && last.HasSecondaryLayerDefinition == value.HasSecondaryLayerDefinition
      && last.SecondaryLayer == value.SecondaryLayer
      && last.UseVertexLighting == value.UseVertexLighting
      && last.StartVertex + last.VertexCount == value.StartVertex)
    {
      spans[spans.Count - 1] = new DrawSpan(
        last.StartVertex,
        last.VertexCount + value.VertexCount,
        last.TextureId,
        last.SecondaryTextureId,
        last.AlphaType,
        last.UvChannel,
        last.LayerColor,
        last.UseVertexLighting,
        last.HasLayerDefinition,
        last.Layer,
        last.HasSecondaryLayerDefinition,
        last.SecondaryLayer);
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
    _skySurfaceToTexture.Clear();
    ReleaseTextureList(_skyTextures, ref _loadedSkyTextureCount, ref _skyDrawSpans);
  }

  private void ReleaseEntityTextures()
  {
    _entitySurfaceToTexture.Clear();
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
      out vec3 vWorldPos;

      void main()
      {
        gl_Position = vec4(aPosition, 1.0) * uMvp;
        vUv0 = aUv0;
        vUv1 = aUv1;
        vColor = aColor;
        vWorldPos = aPosition;
      }
      """;

    const string fragmentShaderSource =
      """
      #version 330 core
      in vec2 vUv0;
      in vec2 vUv1;
      in vec3 vColor;
      in vec3 vWorldPos;
      uniform sampler2D uTexture0;
      uniform sampler2D uTexture1;
      uniform int uUseEnvBump;
      uniform mat2 uBumpMatrix;
      uniform mat4 uView;
      uniform int uUvChannel;
      uniform vec4 uLayerColor;
      uniform int uUseUvTransform;
      uniform mat3 uUvTransform;
      uniform float uAnimTime;
      uniform int uLayerFlags;
      uniform float uLavaWave;
      uniform float uLavaSpeed;
      uniform int uUvGenMode;
      uniform int uMetalMode;
      uniform float uMetalScale;
      uniform int uSecondaryUseUvTransform;
      uniform mat3 uSecondaryUvTransform;
      uniform int uSecondaryLayerFlags;
      uniform float uSecondaryLavaWave;
      uniform float uSecondaryLavaSpeed;
      uniform int uSecondaryUvGenMode;
      uniform int uSecondaryMetalMode;
      uniform float uSecondaryMetalScale;
      uniform int uUseVertexLighting;
      uniform float uAmbientStrength;
      uniform vec3 uDirLightDirection;
      uniform vec3 uDirLightColor;
      uniform int uPointLightCount;
      uniform vec4 uPointLightPosRadius[4];
      uniform vec4 uPointLightColorIntensity[4];
      uniform vec3 uCameraPos;
      uniform int uFogEnabled;
      uniform vec3 uFogColor;
      uniform float uFogStart;
      uniform float uFogEnd;
      out vec4 FragColor;

      vec3 SafeNormalize(vec3 value, vec3 fallback)
      {
        float len = length(value);
        if (len <= 0.00001)
        {
          return fallback;
        }

        return value / len;
      }

      vec2 BuildGeneratedUv(vec2 baseUv, vec3 viewPos, vec3 viewNormal, int uvGenMode, int metalMode, float metalScale)
      {
        vec2 uv = baseUv;
        if (uvGenMode == 1)
        {
          float scale = abs(metalScale) > 0.00001 ? metalScale : 0.003;
          if (metalMode == 1)
          {
            uv = vec2(viewPos.x, viewPos.z) * scale;
          }
          else if (metalMode == 2)
          {
            uv = vec2(viewPos.x, viewPos.y) * scale;
          }
          else
          {
            uv = vec2(viewPos.z, viewPos.y) * scale;
          }
        }
        else if (uvGenMode == 2)
        {
          vec3 reflected = reflect(SafeNormalize(viewPos, vec3(0.0, 0.0, 1.0)), SafeNormalize(viewNormal, vec3(0.0, 1.0, 0.0)));
          float m = max(0.0001, 2.0 * sqrt(max(0.0, reflected.x * reflected.x + reflected.y * reflected.y + (reflected.z + 1.0) * (reflected.z + 1.0))));
          uv = vec2(reflected.x / m + 0.5, reflected.y / m + 0.5);
        }

        return uv;
      }

      vec2 ApplyLayerUvAnimation(vec2 uv, int layerFlags, float lavaWave, float lavaSpeed, int useUvTransform, mat3 uvTransform, float animTime)
      {
        if ((layerFlags & 0x00000004) != 0)
        {
          vec2 uvBase = uv;
          uv.x = uvBase.x + sin(animTime * lavaSpeed + uvBase.y * 20.0) * lavaWave / 8.0;
          uv.y = uvBase.y + cos(animTime * lavaSpeed + uvBase.x * 20.0) * lavaWave / 8.0;
        }

        if (useUvTransform != 0)
        {
          uv = (vec3(uv, 1.0) * uvTransform).xy;
        }

        return uv;
      }

      void main()
      {
        vec2 baseUv = (uUvChannel == 1) ? vUv1 : vUv0;
        vec3 worldDx = dFdx(vWorldPos);
        vec3 worldDy = dFdy(vWorldPos);
        vec3 worldNormal = SafeNormalize(cross(worldDx, worldDy), vec3(0.0, 1.0, 0.0));

        vec3 viewPos = (vec4(vWorldPos, 1.0) * uView).xyz;
        vec3 viewDx = dFdx(viewPos);
        vec3 viewDy = dFdy(viewPos);
        vec3 viewNormal = SafeNormalize(cross(viewDx, viewDy), vec3(0.0, 1.0, 0.0));
        if (!gl_FrontFacing)
        {
          worldNormal = -worldNormal;
          viewNormal = -viewNormal;
        }

        vec2 uvPrimary = BuildGeneratedUv(baseUv, viewPos, viewNormal, uUvGenMode, uMetalMode, uMetalScale);
        uvPrimary = ApplyLayerUvAnimation(uvPrimary, uLayerFlags, uLavaWave, uLavaSpeed, uUseUvTransform, uUvTransform, uAnimTime);

        vec4 tex = texture(uTexture0, uvPrimary);
        if (uUseEnvBump != 0)
        {
          vec2 uvSecondary = BuildGeneratedUv(baseUv, viewPos, viewNormal, uSecondaryUvGenMode, uSecondaryMetalMode, uSecondaryMetalScale);
          uvSecondary = ApplyLayerUvAnimation(
            uvSecondary,
            uSecondaryLayerFlags,
            uSecondaryLavaWave,
            uSecondaryLavaSpeed,
            uSecondaryUseUvTransform,
            uSecondaryUvTransform,
            uAnimTime);

          vec4 bumpSample = texture(uTexture1, uvSecondary);
          vec2 bumpVector = bumpSample.rg * 2.0 - 1.0;
          vec2 bumpOffset = uBumpMatrix * bumpVector;
          vec4 bumped = texture(uTexture0, uvPrimary + bumpOffset);
          tex.rgb = mix(tex.rgb, bumped.rgb, 0.60);
        }

        vec3 lit = vec3(1.0);
        if (uUseVertexLighting != 0)
        {
          vec3 baseLit = clamp(vColor + vec3(0.18), vec3(0.0), vec3(1.0));

          float ndl = max(dot(worldNormal, -normalize(uDirLightDirection)), 0.0);
          vec3 dynamicLight = vec3(uAmbientStrength) + (uDirLightColor * ndl);

          for (int i = 0; i < uPointLightCount; ++i)
          {
            vec3 lightPos = uPointLightPosRadius[i].xyz;
            float radius = max(uPointLightPosRadius[i].w, 0.001);
            vec3 toLight = lightPos - vWorldPos;
            float dist = length(toLight);
            if (dist < radius)
            {
              vec3 l = SafeNormalize(toLight, vec3(0.0, 1.0, 0.0));
              float ndlPoint = max(dot(worldNormal, l), 0.0);
              float atten = 1.0 - (dist / radius);
              vec3 lc = uPointLightColorIntensity[i].rgb;
              float intensity = uPointLightColorIntensity[i].a;
              dynamicLight += lc * (atten * ndlPoint * intensity);
            }
          }

          lit = clamp(baseLit * dynamicLight, vec3(0.0), vec3(1.35));
        }

        vec3 outColor = tex.rgb * uLayerColor.rgb * lit;
        if (uFogEnabled != 0 && uFogEnd > uFogStart)
        {
          float dist = distance(vWorldPos, uCameraPos);
          float fogFactor = clamp((dist - uFogStart) / (uFogEnd - uFogStart), 0.0, 1.0);
          outColor = mix(outColor, uFogColor, fogFactor);
        }
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
      uniform vec4 uLayerColor;
      uniform int uUseUvTransform;
      uniform mat3 uUvTransform;
      uniform float uAnimTime;
      uniform int uLayerFlags;
      uniform float uLavaWave;
      uniform float uLavaSpeed;
      out vec4 FragColor;

      void main()
      {
        vec2 uv = vUv;
        if ((uLayerFlags & 0x00000004) != 0)
        {
          vec2 uvBase = uv;
          uv.x = uvBase.x + sin(uAnimTime * uLavaSpeed + uvBase.y * 20.0) * uLavaWave / 8.0;
          uv.y = uvBase.y + cos(uAnimTime * uLavaSpeed + uvBase.x * 20.0) * uLavaWave / 8.0;
        }
        if (uUseUvTransform != 0)
        {
          uv = (vec3(uv, 1.0) * uUvTransform).xy;
        }

        vec4 tex = texture(uTexture0, uv);
        vec3 color = tex.rgb * vColor * uLayerColor.rgb * uExposure;
        FragColor = vec4(clamp(color, vec3(0.0), vec3(1.0)), tex.a * uLayerColor.a);
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

  private static int CreateParticleShaderProgram()
  {
    const string vertexShaderSource =
      """
      #version 330 core
      layout (location = 0) in vec3 aPosition;
      uniform mat4 uMvp;

      void main()
      {
        gl_Position = vec4(aPosition, 1.0) * uMvp;
        gl_PointSize = 6.0;
      }
      """;

    const string fragmentShaderSource =
      """
      #version 330 core
      uniform vec4 uColor;
      out vec4 FragColor;

      void main()
      {
        vec2 p = gl_PointCoord * 2.0 - 1.0;
        float d2 = dot(p, p);
        if (d2 > 1.0)
        {
          discard;
        }

        float edge = smoothstep(1.0, 0.55, d2);
        FragColor = vec4(uColor.rgb, uColor.a * edge);
      }
      """;

    return CreateShaderProgram(vertexShaderSource, fragmentShaderSource, "Particle");
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
      _camera = _map != null ? CreateDefaultCamera(_map) : CreateDefaultCamera(_currentBounds);
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
        if (_particleVbo != 0)
        {
          GL.DeleteBuffer(_particleVbo);
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
        if (_particleVao != 0)
        {
          GL.DeleteVertexArray(_particleVao);
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
        if (_particleProgram != 0)
        {
          GL.DeleteProgram(_particleProgram);
        }
        if (_clipDebugProgram != 0)
        {
          GL.DeleteProgram(_clipDebugProgram);
        }
      }
    }

    base.Dispose(disposing);
  }

  private readonly record struct LayerAnimationState(
    int TextureId,
    Vector4 LayerColor,
    bool UseUvTransform,
    Matrix3 UvTransform,
    uint LayerFlags,
    float LavaWave,
    float LavaSpeed,
    int UvGenMode,
    int MetalMode,
    float MetalScale);

  private readonly record struct DrawSpan(
    int StartVertex,
    int VertexCount,
    int TextureId,
    int SecondaryTextureId,
    uint AlphaType,
    int UvChannel,
    Vector4 LayerColor,
    bool UseVertexLighting,
    bool HasLayerDefinition,
    MaterialLayerDefinition Layer,
    bool HasSecondaryLayerDefinition,
    MaterialLayerDefinition SecondaryLayer);
}
