using System.Diagnostics;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Linq;
using MapEditor.Formats;
using NumericsMatrix4 = System.Numerics.Matrix4x4;
using NumericsQuaternion = System.Numerics.Quaternion;
using NumericsVector3 = System.Numerics.Vector3;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using GLControlSettings = OpenTK.GLControl.GLControlSettings;
using GLControlWinForms = OpenTK.GLControl.GLControl;
using OpenTK.Windowing.Common;

namespace MapEditor.Viewer;

internal sealed class MapViewerControl : UserControl
{
  private enum MeshPassFilter
  {
    All = 0,
    OpaqueOnly = 1,
    AlphaOnly = 2,
  }

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
  private const uint MaterialAlphaFlagDisableDepthTest = 0x40000000;
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
  private const uint CollisionLineAttrFreeze = 0x40000000u;
  private const float SkyExposure = 1.18f;
  private const float ParityDynamicEntityCullDistance = 20000.0f;
  private static readonly Vector3 DefaultClearColor = new(0.05f, 0.06f, 0.08f);
  private static readonly Vector4 CollisionWallNormalOverlayColor = new(0.00f, 1.00f, 0.62f, 0.40f);
  private static readonly Vector4 CollisionWallFrozenOverlayColor = new(0.00f, 0.92f, 1.00f, 0.50f);
  private static readonly Vector3 BspSelectionColor = new(1.00f, 0.67f, 0.18f);
  private const int MaxDynamicLights = 4;
  private const int MaxAutoClusterFaceCap = 6000;
  private const float MaxAutoClusterFaceRatio = 0.20f;
  private const int MaxGlobalAlphaSortSpans = 256;
  private const int NearestEntityOverlayRefreshMs = 120;

  private readonly GLControlWinForms _glControl;
  private readonly Label _cameraOverlayLabel;
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
  private readonly Dictionary<int, EntitySceneModel> _entitySceneModelsById = [];
  private readonly Dictionary<int, bool> _entitySceneModelHasAnimation = [];
  private readonly Dictionary<int, float> _entitySceneModelApproxRadius = [];

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
  private bool _flipNorthSouth = true;
  private bool _enableHeadlight = true;
  private float _headlightRadius = 4500f;
  private float _headlightIntensity = 0.85f;
  private RenderPipelineMode _renderPipelineMode = RenderPipelineMode.LegacyCompat;

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
  private int _meshUniformAlphaTestEnabled;
  private int _meshUniformAlphaRef;
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
  private int _parityEntityVao;
  private int _parityEntityVbo;
  private int _parityEntityVertexCount;
  private int _parityStaticEntityVao;
  private int _parityStaticEntityVbo;
  private int _parityStaticEntityVertexCount;

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
  private int _wallFrozenVao;
  private int _wallFrozenVbo;
  private int _wallFrozenVertexCount;
  private int _wallUniformMvp;
  private int _wallUniformColor;
  private int _particleProgram;
  private int _particleVao;
  private int _particleVbo;
  private int _particleVertexCount;
  private int _particleUniformMvp;
  private int _particleUniformColor;
  private bool _showParticleMarkers = false;
  private bool _bspSelectModeEnabled = true;
  private bool _bspMoveModeEnabled;
  private bool _collisionDrawModeEnabled;
  private bool _collisionSelectModeEnabled;
  private bool _bspMoveDragging;
  private Point _bspMoveStartMouse;
  private Vector3 _bspMovePivotSource;
  private Vector3 _bspMovePlaneOriginSource;
  private Vector3 _bspMovePlaneNormalSource;
  private Vector3 _bspMoveStartSource;
  private Vector3 _bspMoveCurrentSource;
  private bool _collisionDrawDragging;
  private Vector3 _collisionDrawStartSource;
  private Vector3 _collisionDrawCurrentSource;
  private float _collisionDrawPreviewHeight = 3000f;
  private float _collisionDrawEmbedDepth = 80f;
  private float _collisionDrawSnapDistance = 80f;
  private bool _collisionDrawStartSnapped;
  private bool _collisionDrawEndSnapped;
  private int _collisionDrawPreviewVao;
  private int _collisionDrawPreviewVbo;
  private int _collisionDrawPreviewVertexCount;
  private int _collisionSelectionVao;
  private int _collisionSelectionVbo;
  private int _collisionSelectionVertexCount;
  private int _selectedCollisionLineIndex = -1;
  private int _bspSelectionVao;
  private int _bspSelectionVbo;
  private int _bspSelectionVertexCount;
  private readonly HashSet<int> _selectedBspObjectIds = [];
  private readonly Dictionary<int, bool> _selectedBspFaceSelections = [];
  private int _selectedBspFaceIndex = -1;
  private int _selectedBspObjectId = -1;
  private bool _selectedBspFaceOnly;
  private int[] _bspMovePreviewVertexIndices = Array.Empty<int>();
  private Vector3[] _bspMovePreviewBaseDisplayPositions = Array.Empty<Vector3>();
  private Vector3 _bspMovePreviewDisplayDelta = Vector3.Zero;
  private bool _bspMovePreviewApplied;

  private int _clipDebugProgram;
  private int _checkerTexture;
  private int _loadedMapTextureCount;
  private int _loadedSkyTextureCount;
  private int _loadedEntityTextureCount;
  private DrawSpan[] _meshDrawSpans = Array.Empty<DrawSpan>();
  private DrawSpan[] _skyDrawSpans = Array.Empty<DrawSpan>();
  private DrawSpan[] _entityDrawSpans = Array.Empty<DrawSpan>();
  private DrawSpan[] _parityEntityDrawSpans = Array.Empty<DrawSpan>();
  private DrawSpan[] _parityStaticEntityDrawSpans = Array.Empty<DrawSpan>();
  private BspRenderVertex[] _mapDrawVertices = Array.Empty<BspRenderVertex>();
  private EntitySceneObject[] _bspSceneObjects = Array.Empty<EntitySceneObject>();
  private ushort[] _bspRenderVertexObjectIds = Array.Empty<ushort>();
  private Vector3[] _bspRenderVertexLocalPositions = Array.Empty<Vector3>();
  private bool _hasAnimatedBspGeometry;
  private int _bspLastFrameTick = int.MinValue;
  private float[] _mapUploadBuffer = Array.Empty<float>();
  private int _mapUploadBufferLength;
  private BspRenderVertex[] _entityDrawVertices = Array.Empty<BspRenderVertex>();
  private BspRenderVertex[] _parityEntityFrameVertices = Array.Empty<BspRenderVertex>();
  private BspRenderVertex[] _parityStaticEntityVertices = Array.Empty<BspRenderVertex>();
  private int _parityEntityLastFrameTick = int.MinValue;
  private float[] _parityEntityUploadBuffer = Array.Empty<float>();
  private int _parityEntityUploadBufferLength;
  private int _parityEntityDrawCalls;
  private int _parityAnimatedObjectUpdates;
  private int _paritySkippedLayers;
  private int _parityCulledDynamicInstances;
  private bool _hasAnimatedEntityInstances;
  private string _lastCameraOverlayText = string.Empty;
  private long _lastNearestEntityOverlayTickMs = long.MinValue;
  private string _lastNearestEntityOverlayText = "Nearest Ent: n/a";
  private long _lastNearestFxOverlayTickMs = long.MinValue;
  private string _lastNearestFxOverlayText = "Nearest FX: n/a";

  [DllImport("user32.dll")]
  private static extern short GetAsyncKeyState(int vKey);

  public int RenderVertexCount => _meshVertexCount;
  public int TextureCount => _loadedMapTextureCount;
  public int SkyVertexCount => _skyVertexCount;
  public int EntityVertexCount => _entityVertexCount;
  public int SkyTextureCount => _loadedSkyTextureCount;
  public int EntityTextureCount => _loadedEntityTextureCount;
  public int ParityEntityDrawCalls => _parityEntityDrawCalls;
  public int ParityAnimatedObjectUpdates => _parityAnimatedObjectUpdates;
  public int ParitySkippedLayers => _paritySkippedLayers;
  public int ParityCulledDynamicInstances => _parityCulledDynamicInstances;
  public int SelectedCollisionLineIndex => _selectedCollisionLineIndex;
  public int SelectedBspFaceIndex => _selectedBspFaceIndex;
  public int SelectedBspObjectId => _selectedBspObjectId;
  public bool SelectedBspFaceOnly => _selectedBspFaceOnly;
  public bool HasAnyBspSelection => _selectedBspObjectIds.Count > 0 || _selectedBspFaceSelections.Count > 0;
  public event Action<Vector3, Vector3>? CollisionWallDrawRequested;
  public event Action<int>? CollisionLineSelectionChanged;
  public event Action<int, int>? BspSelectionChanged;
  public event Action<Vector3>? BspTranslateRequested;

  public (float X, float Y, float Z) GetCameraDisplayPosition()
  {
    return (_camera.Position.X, _camera.Position.Y, _camera.Position.Z);
  }

  public (float X, float Y, float Z, float Yaw, float Pitch) GetCameraDisplayPose()
  {
    return (_camera.Position.X, _camera.Position.Y, _camera.Position.Z, _camera.Yaw, _camera.Pitch);
  }

  public bool TrySetCameraDisplayPosition(float x, float y, float z)
  {
    if (_map == null)
    {
      return false;
    }

    if (!float.IsFinite(x) || !float.IsFinite(y) || !float.IsFinite(z))
    {
      return false;
    }

    _camera.Position = new Vector3(x, y, z);
    UpdateCameraOverlay();
    _glControl.Invalidate();
    return true;
  }

  public bool TrySetCameraDisplayPose(float x, float y, float z, float yaw, float pitch)
  {
    if (_map == null)
    {
      return false;
    }

    if (!float.IsFinite(x) || !float.IsFinite(y) || !float.IsFinite(z) || !float.IsFinite(yaw) || !float.IsFinite(pitch))
    {
      return false;
    }

    _camera = new FreeCamera(new Vector3(x, y, z), yaw, MathHelper.Clamp(pitch, -89f, 89f));
    UpdateCameraOverlay();
    _glControl.Invalidate();
    return true;
  }

  public void ApplyCollisionEditedMap(LoadedMap map)
  {
    if (map == null)
    {
      return;
    }

    _map = map;
    _currentBounds = ConvertBoundsForDisplay(map.Bounds);
    ResetBspMovePreviewState(restorePositions: false);
    _bspMoveDragging = false;
    if (_selectedCollisionLineIndex >= map.CollisionLines.Length)
    {
      _selectedCollisionLineIndex = -1;
    }
    PruneBspSelectionsForCurrentMap();

    if (_glReady)
    {
      if (TryMakeCurrent())
      {
        UploadCollisionGeometryOnly();
        UpdateBspSelectionBuffer();
      }
    }

    UpdateCameraOverlay();
    _glControl.Invalidate();
  }

  public void ApplyMapGeometryEdited(LoadedMap map)
  {
    if (map == null)
    {
      return;
    }

    _map = map;
    _currentBounds = ConvertBoundsForDisplay(map.Bounds);
    ResetBspMovePreviewState(restorePositions: false);
    _bspMoveDragging = false;
    BuildEntitySceneModelLookup(map);
    if (_selectedCollisionLineIndex >= map.CollisionLines.Length)
    {
      _selectedCollisionLineIndex = -1;
    }
    PruneBspSelectionsForCurrentMap();

    if (_glReady)
    {
      if (TryMakeCurrent())
      {
        UploadGeometry();
      }
    }

    UpdateCameraOverlay();
    _glControl.Invalidate();
  }

  public (float X, float Y, float Z) GetCameraSourcePosition()
  {
    Vector3 sourcePosition = ConvertSourcePosition(_camera.Position);
    return (sourcePosition.X, sourcePosition.Y, sourcePosition.Z);
  }

  public bool TryGetCameraSourcePose(out Vector3 sourcePosition, out Vector3 sourceForward)
  {
    if (_map == null)
    {
      sourcePosition = Vector3.Zero;
      sourceForward = new Vector3(0f, 0f, -1f);
      return false;
    }

    sourcePosition = ConvertSourcePosition(_camera.Position);
    sourceForward = ConvertSourceDirection(_camera.Forward);
    if (sourceForward.LengthSquared < 0.000001f)
    {
      sourceForward = new Vector3(0f, 0f, -1f);
    }
    else
    {
      sourceForward = Vector3.Normalize(sourceForward);
    }

    return true;
  }

  public bool TrySetCameraSourcePosition(float x, float y, float z)
  {
    if (_map == null)
    {
      return false;
    }

    if (!float.IsFinite(x) || !float.IsFinite(y) || !float.IsFinite(z))
    {
      return false;
    }

    _camera.Position = ConvertWorldPosition(new Vector3(x, y, z));
    UpdateCameraOverlay();
    _glControl.Invalidate();
    return true;
  }

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
  public bool BspSelectModeEnabled
  {
    get => _bspSelectModeEnabled;
    set
    {
      _bspSelectModeEnabled = value;
      if (!_bspSelectModeEnabled && !_bspMoveModeEnabled)
      {
        _bspMoveDragging = false;
        CancelBspMovePreview();
      }
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool BspMoveModeEnabled
  {
    get => _bspMoveModeEnabled;
    set
    {
      if (_bspMoveModeEnabled == value)
      {
        return;
      }

      _bspMoveModeEnabled = value;
      if (!_bspMoveModeEnabled)
      {
        _bspMoveDragging = false;
        CancelBspMovePreview();
      }
    }
  }

  public int[] GetSelectedBspObjectIdsSnapshot()
  {
    HashSet<int> objectIds = new(_selectedBspObjectIds);
    if (objectIds.Count <= 0 && _selectedBspObjectId >= 0)
    {
      objectIds.Add(_selectedBspObjectId);
    }

    return objectIds.Count <= 0 ? Array.Empty<int>() : objectIds.ToArray();
  }

  public BlenderInterchange.BspFaceSelection[] GetSelectedBspFaceSelectionsSnapshot()
  {
    Dictionary<int, bool> faceSelections = new(_selectedBspFaceSelections);
    if (faceSelections.Count <= 0 && _selectedBspFaceIndex >= 0)
    {
      faceSelections[_selectedBspFaceIndex] = _selectedBspFaceOnly;
    }

    if (faceSelections.Count <= 0)
    {
      return Array.Empty<BlenderInterchange.BspFaceSelection>();
    }

    BlenderInterchange.BspFaceSelection[] snapshot = new BlenderInterchange.BspFaceSelection[faceSelections.Count];
    int index = 0;
    foreach ((int faceIndex, bool faceOnly) in faceSelections)
    {
      snapshot[index++] = new BlenderInterchange.BspFaceSelection(faceIndex, faceOnly);
    }

    return snapshot;
  }

  public void CancelBspMovePreview()
  {
    ResetBspMovePreviewState(restorePositions: true);
    _bspMoveDragging = false;
    _glControl.Capture = false;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool CollisionDrawModeEnabled
  {
    get => _collisionDrawModeEnabled;
    set
    {
      if (_collisionDrawModeEnabled == value)
      {
        return;
      }

      _collisionDrawModeEnabled = value;
      if (!_collisionDrawModeEnabled)
      {
        _collisionDrawDragging = false;
        UpdateCollisionDrawPreviewBuffer();
      }
      else if (_collisionSelectModeEnabled)
      {
        _collisionSelectModeEnabled = false;
      }

      _glControl.Invalidate();
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool CollisionSelectModeEnabled
  {
    get => _collisionSelectModeEnabled;
    set
    {
      if (_collisionSelectModeEnabled == value)
      {
        return;
      }

      _collisionSelectModeEnabled = value;
      if (_collisionSelectModeEnabled)
      {
        _collisionDrawModeEnabled = false;
        if (_collisionDrawDragging)
        {
          _collisionDrawDragging = false;
          UpdateCollisionDrawPreviewBuffer();
        }
      }

      _glControl.Invalidate();
    }
  }

  public void SetSelectedCollisionLineIndex(int lineIndex)
  {
    int normalized = -1;
    if (_map != null && (uint)lineIndex < (uint)_map.CollisionLines.Length)
    {
      normalized = lineIndex;
    }

    if (_selectedCollisionLineIndex == normalized)
    {
      return;
    }

    _selectedCollisionLineIndex = normalized;
    UpdateCollisionSelectionBuffer();
    CollisionLineSelectionChanged?.Invoke(_selectedCollisionLineIndex);
  }

  public void ClearSelectedCollisionLine()
  {
    SetSelectedCollisionLineIndex(-1);
  }

  public bool TryGetSelectedBspObjectId(out int objectId)
  {
    objectId = _selectedBspObjectId;
    return objectId >= 0;
  }

  public bool TryGetSelectedBspFaceIndex(out int faceIndex)
  {
    faceIndex = _selectedBspFaceIndex;
    return faceIndex >= 0;
  }

  public void ClearSelectedBspSelection()
  {
    CancelBspMovePreview();
    bool hadSelection = _selectedBspObjectIds.Count > 0
      || _selectedBspFaceSelections.Count > 0
      || _selectedBspFaceIndex >= 0
      || _selectedBspObjectId >= 0;
    _selectedBspObjectIds.Clear();
    _selectedBspFaceSelections.Clear();
    _selectedBspFaceIndex = -1;
    _selectedBspObjectId = -1;
    _selectedBspFaceOnly = false;
    if (hadSelection)
    {
      UpdateBspSelectionBuffer();
      BspSelectionChanged?.Invoke(_selectedBspFaceIndex, _selectedBspObjectId);
      _glControl.Invalidate();
    }
  }

  private void SetSelectedBspSelection(int faceIndex, int objectId, bool faceOnly)
  {
    CancelBspMovePreview();
    int normalizedFace = -1;
    int normalizedObject = -1;
    bool normalizedFaceOnly = false;
    if (_map != null)
    {
      int faceCount = _map.BspRenderVertices.Length / 3;
      if (faceIndex >= 0 && faceIndex < faceCount)
      {
        normalizedFace = faceIndex;
        normalizedFaceOnly = faceOnly;
      }

      if (objectId >= 0)
      {
        normalizedObject = objectId;
        normalizedFaceOnly = false;
      }
    }

    if (_selectedBspFaceIndex == normalizedFace
      && _selectedBspObjectId == normalizedObject
      && _selectedBspFaceOnly == normalizedFaceOnly)
    {
      return;
    }

    _selectedBspObjectIds.Clear();
    _selectedBspFaceSelections.Clear();
    if (normalizedObject >= 0)
    {
      _selectedBspObjectIds.Add(normalizedObject);
    }
    else if (normalizedFace >= 0)
    {
      _selectedBspFaceSelections[normalizedFace] = normalizedFaceOnly;
    }

    _selectedBspFaceIndex = normalizedFace;
    _selectedBspObjectId = normalizedObject;
    _selectedBspFaceOnly = normalizedFaceOnly;
    UpdateBspSelectionBuffer();
    BspSelectionChanged?.Invoke(_selectedBspFaceIndex, _selectedBspObjectId);
    _glControl.Invalidate();
  }

  private void ToggleBspSelection(int faceIndex, int objectId, bool faceOnly)
  {
    CancelBspMovePreview();
    int normalizedFace = -1;
    int normalizedObject = -1;
    bool normalizedFaceOnly = false;
    if (_map != null)
    {
      int faceCount = _map.BspRenderVertices.Length / 3;
      if (faceIndex >= 0 && faceIndex < faceCount)
      {
        normalizedFace = faceIndex;
        normalizedFaceOnly = faceOnly;
      }

      if (objectId >= 0)
      {
        normalizedObject = objectId;
        normalizedFaceOnly = false;
      }
    }

    if (normalizedObject < 0 && normalizedFace < 0)
    {
      return;
    }

    if (normalizedObject >= 0)
    {
      if (_selectedBspObjectIds.Contains(normalizedObject))
      {
        _selectedBspObjectIds.Remove(normalizedObject);
      }
      else
      {
        _selectedBspObjectIds.Add(normalizedObject);
      }

      _selectedBspObjectId = normalizedObject;
      _selectedBspFaceIndex = normalizedFace;
      _selectedBspFaceOnly = false;
    }
    else
    {
      if (_selectedBspFaceSelections.TryGetValue(normalizedFace, out bool existingFaceOnly))
      {
        if (existingFaceOnly == normalizedFaceOnly)
        {
          _selectedBspFaceSelections.Remove(normalizedFace);
        }
        else
        {
          _selectedBspFaceSelections[normalizedFace] = normalizedFaceOnly;
        }
      }
      else
      {
        _selectedBspFaceSelections[normalizedFace] = normalizedFaceOnly;
      }

      _selectedBspFaceIndex = normalizedFace;
      _selectedBspObjectId = -1;
      _selectedBspFaceOnly = normalizedFaceOnly;
    }

    NormalizePrimarySelectionFromMulti();
    UpdateBspSelectionBuffer();
    BspSelectionChanged?.Invoke(_selectedBspFaceIndex, _selectedBspObjectId);
    _glControl.Invalidate();
  }

  private void NormalizePrimarySelectionFromMulti()
  {
    if (_selectedBspObjectId >= 0 && _selectedBspObjectIds.Contains(_selectedBspObjectId))
    {
      _selectedBspFaceOnly = false;
      return;
    }

    if (_selectedBspObjectIds.Count > 0)
    {
      foreach (int objectId in _selectedBspObjectIds)
      {
        _selectedBspObjectId = objectId;
        _selectedBspFaceOnly = false;
        return;
      }
    }

    if (_selectedBspFaceIndex >= 0 && _selectedBspFaceSelections.TryGetValue(_selectedBspFaceIndex, out bool faceOnly))
    {
      _selectedBspObjectId = -1;
      _selectedBspFaceOnly = faceOnly;
      return;
    }

    if (_selectedBspFaceSelections.Count > 0)
    {
      foreach ((int faceIndex, bool selectionFaceOnly) in _selectedBspFaceSelections)
      {
        _selectedBspFaceIndex = faceIndex;
        _selectedBspObjectId = -1;
        _selectedBspFaceOnly = selectionFaceOnly;
        return;
      }
    }

    _selectedBspFaceIndex = -1;
    _selectedBspObjectId = -1;
    _selectedBspFaceOnly = false;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public float CollisionDrawPreviewHeight
  {
    get => _collisionDrawPreviewHeight;
    set
    {
      float bounded = Math.Clamp(value, 1.0f, 50000.0f);
      if (MathF.Abs(_collisionDrawPreviewHeight - bounded) < 0.0001f)
      {
        return;
      }

      _collisionDrawPreviewHeight = bounded;
      if (_collisionDrawDragging)
      {
        UpdateCollisionDrawPreviewBuffer();
      }
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public float CollisionDrawEmbedDepth
  {
    get => _collisionDrawEmbedDepth;
    set
    {
      float bounded = Math.Clamp(value, 0.0f, 50000.0f);
      if (MathF.Abs(_collisionDrawEmbedDepth - bounded) < 0.0001f)
      {
        return;
      }

      _collisionDrawEmbedDepth = bounded;
      if (_collisionDrawDragging)
      {
        UpdateCollisionDrawPreviewBuffer();
      }
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public float CollisionDrawSnapDistance
  {
    get => _collisionDrawSnapDistance;
    set
    {
      float bounded = Math.Clamp(value, 0.0f, 10000.0f);
      if (MathF.Abs(_collisionDrawSnapDistance - bounded) < 0.0001f)
      {
        return;
      }

      _collisionDrawSnapDistance = bounded;
      if (_collisionDrawDragging)
      {
        UpdateCollisionDrawPreviewBuffer();
      }
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool EnableDynamicLighting
  {
    get => _enableDynamicLighting;
    set => _enableDynamicLighting = value;
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public bool FlipNorthSouth
  {
    get => _flipNorthSouth;
    set
    {
      if (_flipNorthSouth == value)
      {
        return;
      }

      _flipNorthSouth = value;
      if (_map != null)
      {
        _currentBounds = ConvertBoundsForDisplay(_map.Bounds);
        _camera = CreateDefaultCamera(_map);
      }

      if (_glReady && _map != null)
      {
        if (TryMakeCurrent())
        {
          UploadGeometry();
        }
      }

      UpdateCameraOverlay();
      _glControl.Invalidate();
    }
  }

  [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
  public RenderPipelineMode RenderPipelineMode
  {
    get => _renderPipelineMode;
    set
    {
      if (_renderPipelineMode == value)
      {
        return;
      }

      _renderPipelineMode = value;
      if (_glReady && _map != null)
      {
        if (TryMakeCurrent())
        {
          UploadGeometry();
        }
      }

      _glControl.Invalidate();
    }
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
    _cameraOverlayLabel = new Label
    {
      AutoSize = true,
      BackColor = Color.FromArgb(170, 0, 0, 0),
      ForeColor = Color.WhiteSmoke,
      Font = new Font("Consolas", 9f, FontStyle.Regular, GraphicsUnit.Point),
      Location = new Point(10, 10),
      Padding = new Padding(6, 4, 6, 4),
      TabStop = false,
      Anchor = AnchorStyles.Top | AnchorStyles.Left,
    };
    Controls.Add(_cameraOverlayLabel);
    _cameraOverlayLabel.BringToFront();
    UpdateCameraOverlay();

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
      if (TryMakeCurrent())
      {
        UploadGeometry();
      }
    }

    _glControl.Invalidate();
  }

  public void SetMap(LoadedMap map)
  {
    _map = map;
    _currentBounds = ConvertBoundsForDisplay(map.Bounds);
    _camera = CreateDefaultCamera(map);
    _selectedCollisionLineIndex = -1;
    _selectedBspObjectIds.Clear();
    _selectedBspFaceSelections.Clear();
    _selectedBspFaceIndex = -1;
    _selectedBspObjectId = -1;
    _selectedBspFaceOnly = false;
    ResetBspMovePreviewState(restorePositions: false);
    _bspMoveDragging = false;
    _collisionDrawDragging = false;
    _collisionDrawStartSnapped = false;
    _collisionDrawEndSnapped = false;
    _lastNearestEntityOverlayTickMs = long.MinValue;
    _lastNearestEntityOverlayText = "Nearest Ent: n/a";
    _lastNearestFxOverlayTickMs = long.MinValue;
    _lastNearestFxOverlayText = "Nearest FX: n/a";
    BuildEntitySceneModelLookup(map);

    if (_glReady)
    {
      if (TryMakeCurrent())
      {
        UploadGeometry();
      }
    }

    UpdateCameraOverlay();
    _glControl.Invalidate();
  }

  private void BuildEntitySceneModelLookup(LoadedMap map)
  {
    _entitySceneModelsById.Clear();
    _entitySceneModelHasAnimation.Clear();
    _entitySceneModelApproxRadius.Clear();
    EntitySceneData scene = map.EntityScene;
    for (int index = 0; index < scene.Models.Length; ++index)
    {
      EntitySceneModel model = scene.Models[index];
      _entitySceneModelsById[model.EntityId] = model;
      _entitySceneModelHasAnimation[model.EntityId] = ModelHasAnimation(model);
      _entitySceneModelApproxRadius[model.EntityId] = ComputeApproxModelRadius(model);
    }

    _hasAnimatedEntityInstances = false;
    for (int index = 0; index < scene.Instances.Length; ++index)
    {
      int entityId = scene.Instances[index].EntityId;
      if (_entitySceneModelHasAnimation.TryGetValue(entityId, out bool hasAnimation) && hasAnimation)
      {
        _hasAnimatedEntityInstances = true;
        break;
      }
    }
  }

  private static bool ModelHasAnimation(EntitySceneModel model)
  {
    if (model.ParticleRuntime.HasValue)
    {
      return true;
    }

    for (int objectIndex = 0; objectIndex < model.Objects.Length; ++objectIndex)
    {
      EntitySceneObject obj = model.Objects[objectIndex];
      if (obj.PositionTracks.Length > 0 || obj.RotationTracks.Length > 0 || obj.ScaleTracks.Length > 0)
      {
        return true;
      }
    }

    return false;
  }

  private static bool HasAnimatedSceneObjects(EntitySceneObject[] objects)
  {
    for (int objectIndex = 0; objectIndex < objects.Length; ++objectIndex)
    {
      EntitySceneObject obj = objects[objectIndex];
      if (obj.PositionTracks.Length > 0 || obj.RotationTracks.Length > 0 || obj.ScaleTracks.Length > 0)
      {
        return true;
      }
    }

    return false;
  }

  private static float ComputeApproxModelRadius(EntitySceneModel model)
  {
    float maxDistanceSq = 0.0f;
    for (int groupIndex = 0; groupIndex < model.MatGroups.Length; ++groupIndex)
    {
      BspRenderVertex[] localVertices = model.MatGroups[groupIndex].LocalVertices;
      for (int vertexIndex = 0; vertexIndex < localVertices.Length; ++vertexIndex)
      {
        float distanceSq = localVertices[vertexIndex].Position.LengthSquared;
        if (float.IsFinite(distanceSq) && distanceSq > maxDistanceSq)
        {
          maxDistanceSq = distanceSq;
        }
      }
    }

    for (int objectIndex = 0; objectIndex < model.Objects.Length; ++objectIndex)
    {
      float distanceSq = model.Objects[objectIndex].Position.LengthSquared;
      if (float.IsFinite(distanceSq) && distanceSq > maxDistanceSq)
      {
        maxDistanceSq = distanceSq;
      }
    }

    if (!float.IsFinite(maxDistanceSq) || maxDistanceSq <= 0.0f)
    {
      return 0.0f;
    }

    return MathF.Sqrt(maxDistanceSq);
  }

  private bool ShouldCullParityDynamicInstance(
    EntitySceneInstance instance,
    float instanceScale,
    int entityId,
    Vector3 sourceCameraPosition)
  {
    float safeScale = float.IsFinite(instanceScale) ? MathF.Abs(instanceScale) : 1.0f;
    if (safeScale < 0.00001f)
    {
      safeScale = 1.0f;
    }

    float modelRadius = 0.0f;
    if (_entitySceneModelApproxRadius.TryGetValue(entityId, out float cachedRadius) && float.IsFinite(cachedRadius))
    {
      modelRadius = MathF.Max(0.0f, cachedRadius);
    }

    float maxVisibleDistance = ParityDynamicEntityCullDistance + modelRadius * safeScale;
    float maxVisibleDistanceSq = maxVisibleDistance * maxVisibleDistance;
    Vector3 delta = instance.Position - sourceCameraPosition;
    float distanceSq = delta.LengthSquared;
    return float.IsFinite(distanceSq) && distanceSq > maxVisibleDistanceSq;
  }

  private FreeCamera CreateDefaultCamera(LoadedMap map)
  {
    MapBounds bounds = ConvertBoundsForDisplay(map.Bounds);
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

  private MapBounds ConvertBoundsForDisplay(MapBounds bounds)
  {
    if (!_flipNorthSouth)
    {
      return bounds;
    }

    Vector3 a = ConvertWorldPosition(bounds.Min);
    Vector3 b = ConvertWorldPosition(bounds.Max);
    return new MapBounds(Vector3.ComponentMin(a, b), Vector3.ComponentMax(a, b));
  }

  private Vector3 ConvertWorldPosition(Vector3 position)
  {
    return _flipNorthSouth
      ? new Vector3(position.X, position.Y, -position.Z)
      : position;
  }

  private Vector3 ConvertSourcePosition(Vector3 displayPosition)
  {
    return _flipNorthSouth
      ? new Vector3(displayPosition.X, displayPosition.Y, -displayPosition.Z)
      : displayPosition;
  }

  private Vector3 ConvertSourceDirection(Vector3 displayDirection)
  {
    return _flipNorthSouth
      ? new Vector3(displayDirection.X, displayDirection.Y, -displayDirection.Z)
      : displayDirection;
  }

  private BspRenderVertex[] ConvertRenderVertices(BspRenderVertex[] source)
  {
    if (!_flipNorthSouth || source.Length == 0)
    {
      return source;
    }

    BspRenderVertex[] converted = new BspRenderVertex[source.Length];
    for (int index = 0; index < source.Length; ++index)
    {
      BspRenderVertex vertex = source[index];
      converted[index] = new BspRenderVertex(
        ConvertWorldPosition(vertex.Position),
        vertex.Uv,
        vertex.LightUv,
        vertex.Color);
    }

    return converted;
  }

  private Vector3[] ConvertDisplayPositions(Vector3[] source)
  {
    if (!_flipNorthSouth || source.Length == 0)
    {
      return source;
    }

    Vector3[] converted = new Vector3[source.Length];
    for (int index = 0; index < source.Length; ++index)
    {
      converted[index] = ConvertWorldPosition(source[index]);
    }

    return converted;
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
    if (!TryMakeCurrent())
    {
      return;
    }

    GL.ClearColor(DefaultClearColor.X, DefaultClearColor.Y, DefaultClearColor.Z, 1f);
    GL.Enable(EnableCap.DepthTest);
    GL.Enable(EnableCap.CullFace);
    GL.CullFace(TriangleFace.Back);

    _meshProgram = CreateMeshShaderProgram();
    _meshUniformMvp = GL.GetUniformLocation(_meshProgram, "uMvp");
    _meshUniformTexture = GL.GetUniformLocation(_meshProgram, "uTexture0");
    _meshUniformTexture1 = GL.GetUniformLocation(_meshProgram, "uTexture1");
    _meshUniformAlphaTestEnabled = GL.GetUniformLocation(_meshProgram, "uAlphaTestEnabled");
    _meshUniformAlphaRef = GL.GetUniformLocation(_meshProgram, "uAlphaRef");
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
    _parityStaticEntityVao = GL.GenVertexArray();
    _parityStaticEntityVbo = GL.GenBuffer();
    ConfigureMeshVao(_parityStaticEntityVao, _parityStaticEntityVbo);
    _parityEntityVao = GL.GenVertexArray();
    _parityEntityVbo = GL.GenBuffer();
    ConfigureMeshVao(_parityEntityVao, _parityEntityVbo);

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
    _wallFrozenVao = GL.GenVertexArray();
    _wallFrozenVbo = GL.GenBuffer();
    GL.BindVertexArray(_wallFrozenVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _wallFrozenVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);
    _particleVao = GL.GenVertexArray();
    _particleVbo = GL.GenBuffer();
    GL.BindVertexArray(_particleVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _particleVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);
    _collisionDrawPreviewVao = GL.GenVertexArray();
    _collisionDrawPreviewVbo = GL.GenBuffer();
    GL.BindVertexArray(_collisionDrawPreviewVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _collisionDrawPreviewVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);
    _collisionSelectionVao = GL.GenVertexArray();
    _collisionSelectionVbo = GL.GenBuffer();
    GL.BindVertexArray(_collisionSelectionVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _collisionSelectionVbo);
    GL.EnableVertexAttribArray(0);
    GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 12, 0);
    _bspSelectionVao = GL.GenVertexArray();
    _bspSelectionVbo = GL.GenBuffer();
    GL.BindVertexArray(_bspSelectionVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _bspSelectionVbo);
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

    if (!TryMakeCurrent())
    {
      return;
    }

    ResizeViewport();
  }

  private bool TryMakeCurrent()
  {
    if (_glControl.IsDisposed || !_glControl.IsHandleCreated)
    {
      return false;
    }

    try
    {
      _glControl.MakeCurrent();
      return true;
    }
    catch
    {
      return false;
    }
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

    if (!TryMakeCurrent())
    {
      return;
    }

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
    UpdateCameraOverlay();
    _glControl.Invalidate();
  }

  private void UpdateCameraOverlay()
  {
    Vector3 sourcePosition = ConvertSourcePosition(_camera.Position);
    string positionLine = FormattableString.Invariant($"Cam XYZ: {sourcePosition.X:F2}, {sourcePosition.Y:F2}, {sourcePosition.Z:F2}");
    string nearestEntityLine = GetNearestEntityOverlayText(sourcePosition);
    string nearestFxLine = GetNearestFxOverlayText(sourcePosition);
    string line = $"{positionLine}{Environment.NewLine}{nearestEntityLine}{Environment.NewLine}{nearestFxLine}";

    if (line == _lastCameraOverlayText)
    {
      return;
    }

    _lastCameraOverlayText = line;
    _cameraOverlayLabel.Text = line;
  }

  private string GetNearestEntityOverlayText(Vector3 sourceCameraPosition)
  {
    if (_map == null)
    {
      return "Nearest Ent: n/a";
    }

    EntitySceneInstance[] instances = _map.EntityScene.Instances;
    if (instances.Length == 0)
    {
      return "Nearest Ent: n/a";
    }

    long nowMs = Environment.TickCount64;
    if (_lastNearestEntityOverlayTickMs != long.MinValue &&
        nowMs - _lastNearestEntityOverlayTickMs < NearestEntityOverlayRefreshMs)
    {
      return _lastNearestEntityOverlayText;
    }

    float bestDistSq = float.MaxValue;
    int bestIndex = -1;
    for (int index = 0; index < instances.Length; ++index)
    {
      Vector3 delta = instances[index].Position - sourceCameraPosition;
      float distSq = delta.LengthSquared;
      if (distSq < bestDistSq)
      {
        bestDistSq = distSq;
        bestIndex = index;
      }
    }

    if (bestIndex < 0)
    {
      _lastNearestEntityOverlayText = "Nearest Ent: n/a";
      _lastNearestEntityOverlayTickMs = nowMs;
      return _lastNearestEntityOverlayText;
    }

    EntitySceneInstance nearest = instances[bestIndex];
    string modelName = $"id{nearest.EntityId}";
    bool hasAnimation = false;
    uint alphaType = 0;
    bool hasAlphaType = false;
    if (_entitySceneModelsById.TryGetValue(nearest.EntityId, out EntitySceneModel? model))
    {
      modelName = model.Name;
      if (_entitySceneModelHasAnimation.TryGetValue(nearest.EntityId, out bool cachedHasAnimation))
      {
        hasAnimation = cachedHasAnimation;
      }

      if (TryGetFirstEntityMaterialAlphaType(model, out uint resolvedAlphaType))
      {
        alphaType = resolvedAlphaType;
        hasAlphaType = true;
      }
    }

    float bestDist = MathF.Sqrt(bestDistSq);
    string alphaText = hasAlphaType ? $"0x{alphaType:X8}" : "n/a";
    _lastNearestEntityOverlayText = FormattableString.Invariant(
      $"Nearest Ent: id={nearest.EntityId} {modelName} dist={bestDist:F2} alpha={alphaText} anim={(hasAnimation ? "Y" : "N")}");
    _lastNearestEntityOverlayTickMs = nowMs;
    return _lastNearestEntityOverlayText;
  }

  private string GetNearestFxOverlayText(Vector3 sourceCameraPosition)
  {
    if (_map == null)
    {
      return "Nearest FX: n/a";
    }

    ParticleInstanceInfo[] fxInstances = _map.ParticleInstances;
    if (fxInstances.Length == 0)
    {
      return "Nearest FX: n/a";
    }

    long nowMs = Environment.TickCount64;
    if (_lastNearestFxOverlayTickMs != long.MinValue &&
        nowMs - _lastNearestFxOverlayTickMs < NearestEntityOverlayRefreshMs)
    {
      return _lastNearestFxOverlayText;
    }

    float bestDistSq = float.MaxValue;
    int bestIndex = -1;
    for (int index = 0; index < fxInstances.Length; ++index)
    {
      Vector3 delta = fxInstances[index].Position - sourceCameraPosition;
      float distSq = delta.LengthSquared;
      if (distSq < bestDistSq)
      {
        bestDistSq = distSq;
        bestIndex = index;
      }
    }

    if (bestIndex < 0)
    {
      _lastNearestFxOverlayText = "Nearest FX: n/a";
      _lastNearestFxOverlayTickMs = nowMs;
      return _lastNearestFxOverlayText;
    }

    ParticleInstanceInfo info = fxInstances[bestIndex];
    string fxName = string.IsNullOrWhiteSpace(info.Name) ? $"id{info.EntityId}" : info.Name;
    float bestDist = MathF.Sqrt(bestDistSq);
    _lastNearestFxOverlayText = FormattableString.Invariant(
      $"Nearest FX: idx={bestIndex} id={info.EntityId} {fxName} dist={bestDist:F2} pos=({info.Position.X:F2},{info.Position.Y:F2},{info.Position.Z:F2})");
    _lastNearestFxOverlayTickMs = nowMs;
    return _lastNearestFxOverlayText;
  }

  private bool TryGetFirstEntityMaterialAlphaType(EntitySceneModel model, out uint alphaType)
  {
    alphaType = 0;
    if (_map == null)
    {
      return false;
    }

    for (int groupIndex = 0; groupIndex < model.MatGroups.Length; ++groupIndex)
    {
      EntitySceneMatGroup matGroup = model.MatGroups[groupIndex];
      int materialId = matGroup.MaterialId;
      if (materialId < 0)
      {
        continue;
      }

      materialId += model.MaterialBase;
      if (materialId < 0 || materialId >= _map.EntityMaterialAlphaTypes.Length)
      {
        continue;
      }

      alphaType = _map.EntityMaterialAlphaTypes[materialId];
      return true;
    }

    return false;
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
    if (move.LengthSquared <= 0f)
    {
      return;
    }

    move = Vector3.Normalize(move);
    _camera.Position += move * _moveSpeed * MathF.Max(0.0001f, deltaSeconds);
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
    switch (_renderPipelineMode)
    {
      case RenderPipelineMode.R3Parity:
        RenderR3Parity();
        break;
      case RenderPipelineMode.LegacyCompat:
      default:
        RenderLegacyCompat();
        break;
    }
  }

  private void RenderLegacyCompat()
  {
    RenderCore(useLegacyLayerPath: true);
  }

  private void RenderR3Parity()
  {
    RenderCore(useLegacyLayerPath: false);
  }

  private void RenderCore(bool useLegacyLayerPath)
  {
    Vector3 clearColor = GetMapClearColor();
    GL.ClearColor(clearColor.X, clearColor.Y, clearColor.Z, 1.0f);
    GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
    GL.FrontFace(_flipNorthSouth ? FrontFaceDirection.Cw : FrontFaceDirection.Ccw);

    float aspect = Math.Max(0.0001f, (float)_glControl.ClientSize.Width / Math.Max(1f, _glControl.ClientSize.Height));
    Matrix4 projection = Matrix4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(65f), aspect, 0.1f, 200000f);
    Matrix4 view = _camera.GetViewMatrix();
    Matrix4 mvp = view * projection;
    if (!_bspMoveDragging)
    {
      PrepareAnimatedBspFrameMesh();
    }

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

    bool useParityStages = !useLegacyLayerPath;
    bool canRenderParityAnimatedEntities = useParityStages
      && _showR3eEntities
      && _map != null
      && _hasAnimatedEntityInstances;
    if (canRenderParityAnimatedEntities)
    {
      PrepareParityEntityFrameMesh();
    }
    else
    {
      _parityEntityDrawCalls = 0;
      _parityAnimatedObjectUpdates = 0;
      _paritySkippedLayers = 0;
      _parityEntityVertexCount = 0;
      _parityEntityDrawSpans = Array.Empty<DrawSpan>();
      _parityEntityFrameVertices = Array.Empty<BspRenderVertex>();
      _parityEntityLastFrameTick = int.MinValue;
    }

    bool hasLegacyEntityGeometry = _showR3eEntities && _entityVertexCount > 0;
    bool hasParityStaticEntityGeometry = useParityStages && _showR3eEntities && _parityStaticEntityVertexCount > 0;
    bool hasParityDynamicEntityGeometry = useParityStages && canRenderParityAnimatedEntities && _parityEntityVertexCount > 0;

    GL.Enable(EnableCap.DepthTest);
    GL.UseProgram(_meshProgram);
    GL.Uniform1(_meshUniformTexture, 0);
    GL.ActiveTexture(TextureUnit.Texture0);
    if (!useParityStages)
    {
      if (_meshVertexCount > 0)
      {
        DrawMesh(
          _meshVao,
          _meshVertexCount,
          _meshDrawSpans,
          ref mvp,
          ref view,
          _mapSurfaceToTexture,
          null,
          useLegacyLayerPath: true,
          sortVertices: _mapDrawVertices);
      }

      if (hasLegacyEntityGeometry)
      {
        DrawMesh(
          _entityVao,
          _entityVertexCount,
          _entityDrawSpans,
          ref mvp,
          ref view,
          _entitySurfaceToTexture,
          _mapSurfaceToTexture,
          useLegacyLayerPath: true,
          sortVertices: _entityDrawVertices);
      }
    }
    else
    {
      if (_meshVertexCount > 0)
      {
        DrawMesh(
          _meshVao,
          _meshVertexCount,
          _meshDrawSpans,
          ref mvp,
          ref view,
          _mapSurfaceToTexture,
          null,
          useLegacyLayerPath: false,
          sortVertices: _mapDrawVertices,
          passFilter: MeshPassFilter.OpaqueOnly);
      }

      int mapAlphaSpanCount = CountAlphaSpans(_meshDrawSpans);
      int parityStaticEntityAlphaSpanCount = hasParityStaticEntityGeometry
        ? CountAlphaSpans(_parityStaticEntityDrawSpans)
        : 0;
      int parityDynamicEntityAlphaSpanCount = hasParityDynamicEntityGeometry
        ? CountAlphaSpans(_parityEntityDrawSpans)
        : 0;
      int totalAlphaSpanCount = mapAlphaSpanCount + parityStaticEntityAlphaSpanCount + parityDynamicEntityAlphaSpanCount;
      bool useGlobalAlphaSort = totalAlphaSpanCount > 0 && totalAlphaSpanCount <= MaxGlobalAlphaSortSpans;

      if (hasParityStaticEntityGeometry)
      {
        DrawMesh(
          _parityStaticEntityVao,
          _parityStaticEntityVertexCount,
          _parityStaticEntityDrawSpans,
          ref mvp,
          ref view,
          _entitySurfaceToTexture,
          _mapSurfaceToTexture,
          useLegacyLayerPath: false,
          sortVertices: _parityStaticEntityVertices,
          passFilter: MeshPassFilter.OpaqueOnly);
      }

      if (hasParityDynamicEntityGeometry)
      {
        DrawMesh(
          _parityEntityVao,
          _parityEntityVertexCount,
          _parityEntityDrawSpans,
          ref mvp,
          ref view,
          _entitySurfaceToTexture,
          _mapSurfaceToTexture,
          useLegacyLayerPath: false,
          sortVertices: _parityEntityFrameVertices,
          passFilter: MeshPassFilter.OpaqueOnly);
      }

      if (useGlobalAlphaSort)
      {
        List<TransparentDrawCommand> transparentCommands = new(totalAlphaSpanCount);
        if (_meshVertexCount > 0 && mapAlphaSpanCount > 0)
        {
          CollectTransparentDrawCommands(
            transparentCommands,
            _meshVao,
            _meshVertexCount,
            _meshDrawSpans,
            _mapDrawVertices,
            _mapSurfaceToTexture,
            null,
            _camera.Position);
        }

        if (hasParityStaticEntityGeometry && parityStaticEntityAlphaSpanCount > 0)
        {
          CollectTransparentDrawCommands(
            transparentCommands,
            _parityStaticEntityVao,
            _parityStaticEntityVertexCount,
            _parityStaticEntityDrawSpans,
            _parityStaticEntityVertices,
            _entitySurfaceToTexture,
            _mapSurfaceToTexture,
            _camera.Position);
        }

        if (hasParityDynamicEntityGeometry && parityDynamicEntityAlphaSpanCount > 0)
        {
          CollectTransparentDrawCommands(
            transparentCommands,
            _parityEntityVao,
            _parityEntityVertexCount,
            _parityEntityDrawSpans,
            _parityEntityFrameVertices,
            _entitySurfaceToTexture,
            _mapSurfaceToTexture,
            _camera.Position);
        }

        if (transparentCommands.Count > 0)
        {
          transparentCommands.Sort(static (left, right) => right.DistanceSq.CompareTo(left.DistanceSq));
          int[] forcedSpanOrder = new int[1];
          for (int commandIndex = 0; commandIndex < transparentCommands.Count; ++commandIndex)
          {
            TransparentDrawCommand command = transparentCommands[commandIndex];
            forcedSpanOrder[0] = command.SpanIndex;
            DrawMesh(
              command.Vao,
              command.VertexCount,
              command.Spans,
              ref mvp,
              ref view,
              command.SurfaceToTexture,
              command.FallbackSurfaceToTexture,
              useLegacyLayerPath: false,
              sortVertices: null,
              passFilter: MeshPassFilter.AlphaOnly,
              forcedSpanOrder: forcedSpanOrder);
          }
        }
      }
      else
      {
        if (_meshVertexCount > 0 && mapAlphaSpanCount > 0)
        {
          DrawMesh(
            _meshVao,
            _meshVertexCount,
            _meshDrawSpans,
            ref mvp,
            ref view,
            _mapSurfaceToTexture,
            null,
            useLegacyLayerPath: false,
            sortVertices: _mapDrawVertices,
            passFilter: MeshPassFilter.AlphaOnly);
        }

        if (hasParityStaticEntityGeometry && parityStaticEntityAlphaSpanCount > 0)
        {
          DrawMesh(
            _parityStaticEntityVao,
            _parityStaticEntityVertexCount,
            _parityStaticEntityDrawSpans,
            ref mvp,
            ref view,
            _entitySurfaceToTexture,
            _mapSurfaceToTexture,
            useLegacyLayerPath: false,
            sortVertices: _parityStaticEntityVertices,
            passFilter: MeshPassFilter.AlphaOnly);
        }

        if (hasParityDynamicEntityGeometry && parityDynamicEntityAlphaSpanCount > 0)
        {
          DrawMesh(
            _parityEntityVao,
            _parityEntityVertexCount,
            _parityEntityDrawSpans,
            ref mvp,
            ref view,
            _entitySurfaceToTexture,
            _mapSurfaceToTexture,
            useLegacyLayerPath: false,
            sortVertices: _parityEntityFrameVertices,
            passFilter: MeshPassFilter.AlphaOnly);
        }
      }
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

    if (_showCollisionOverlay && (_wallVertexCount > 0 || _wallFrozenVertexCount > 0))
    {
      GL.Enable(EnableCap.DepthTest);
      GL.Disable(EnableCap.CullFace);
      GL.Enable(EnableCap.Blend);
      GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
      GL.UseProgram(_wallProgram);
      GL.UniformMatrix4(_wallUniformMvp, true, ref mvp);

      if (_wallVertexCount > 0)
      {
        GL.Uniform4(_wallUniformColor, CollisionWallNormalOverlayColor);
        GL.BindVertexArray(_wallVao);
        GL.DrawArrays(PrimitiveType.Triangles, 0, _wallVertexCount);
      }

      if (_wallFrozenVertexCount > 0)
      {
        GL.Uniform4(_wallUniformColor, CollisionWallFrozenOverlayColor);
        GL.BindVertexArray(_wallFrozenVao);
        GL.DrawArrays(PrimitiveType.Triangles, 0, _wallFrozenVertexCount);
      }

      GL.Disable(EnableCap.Blend);
      GL.Enable(EnableCap.CullFace);
    }

    if (_showCollisionOverlay && _collisionSelectionVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_lineProgram);
      GL.UniformMatrix4(_lineUniformMvp, true, ref mvp);
      GL.Uniform3(_lineUniformColor, new Vector3(1.00f, 0.33f, 0.22f));
      GL.BindVertexArray(_collisionSelectionVao);
      GL.DrawArrays(PrimitiveType.Lines, 0, _collisionSelectionVertexCount);
    }

    if (_bspSelectionVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_lineProgram);
      GL.UniformMatrix4(_lineUniformMvp, true, ref mvp);
      GL.Uniform3(_lineUniformColor, BspSelectionColor);
      GL.BindVertexArray(_bspSelectionVao);
      GL.DrawArrays(PrimitiveType.Lines, 0, _bspSelectionVertexCount);
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

    if (_collisionDrawPreviewVertexCount > 0)
    {
      GL.Enable(EnableCap.DepthTest);
      GL.UseProgram(_lineProgram);
      GL.UniformMatrix4(_lineUniformMvp, true, ref mvp);
      GL.Uniform3(_lineUniformColor, new Vector3(1.00f, 0.78f, 0.18f));
      GL.BindVertexArray(_collisionDrawPreviewVao);
      GL.DrawArrays(PrimitiveType.Lines, 0, _collisionDrawPreviewVertexCount);
      GL.Disable(EnableCap.DepthTest);
      GL.LineWidth(2.5f);
      if (_collisionDrawStartSnapped)
      {
        GL.Uniform3(_lineUniformColor, new Vector3(0.32f, 1.00f, 0.42f));
        GL.DrawArrays(PrimitiveType.Lines, 2, 2);
      }

      if (_collisionDrawEndSnapped)
      {
        GL.Uniform3(_lineUniformColor, new Vector3(0.32f, 1.00f, 0.42f));
        GL.DrawArrays(PrimitiveType.Lines, 4, 2);
      }
      GL.LineWidth(1.0f);
      GL.Enable(EnableCap.DepthTest);
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
      _parityStaticEntityVertexCount = 0;
      _parityEntityVertexCount = 0;
      _particleVertexCount = 0;
      _wallVertexCount = 0;
      _wallFrozenVertexCount = 0;
      _lineVertexCount = 0;
      _collisionDrawPreviewVertexCount = 0;
      _collisionSelectionVertexCount = 0;
      _bspSelectionVertexCount = 0;
      _selectedCollisionLineIndex = -1;
      _selectedBspObjectIds.Clear();
      _selectedBspFaceSelections.Clear();
      _selectedBspFaceIndex = -1;
      _selectedBspObjectId = -1;
      _selectedBspFaceOnly = false;
      ResetBspMovePreviewState(restorePositions: false);
      _bspMoveDragging = false;
      _collisionDrawDragging = false;
      _meshDrawSpans = Array.Empty<DrawSpan>();
      _skyDrawSpans = Array.Empty<DrawSpan>();
      _entityDrawSpans = Array.Empty<DrawSpan>();
      _parityStaticEntityDrawSpans = Array.Empty<DrawSpan>();
      _parityEntityDrawSpans = Array.Empty<DrawSpan>();
      _mapDrawVertices = Array.Empty<BspRenderVertex>();
      _bspSceneObjects = Array.Empty<EntitySceneObject>();
      _bspRenderVertexObjectIds = Array.Empty<ushort>();
      _bspRenderVertexLocalPositions = Array.Empty<Vector3>();
      _hasAnimatedBspGeometry = false;
      _bspLastFrameTick = int.MinValue;
      _mapUploadBufferLength = 0;
      _entityDrawVertices = Array.Empty<BspRenderVertex>();
      _parityStaticEntityVertices = Array.Empty<BspRenderVertex>();
      _parityEntityFrameVertices = Array.Empty<BspRenderVertex>();
      _parityEntityDrawCalls = 0;
      _parityAnimatedObjectUpdates = 0;
      _paritySkippedLayers = 0;
      _parityCulledDynamicInstances = 0;
      _parityEntityLastFrameTick = int.MinValue;
      _parityEntityUploadBufferLength = 0;
      _entitySceneModelsById.Clear();
      _entitySceneModelHasAnimation.Clear();
      _entitySceneModelApproxRadius.Clear();
      _hasAnimatedEntityInstances = false;
      ReleaseMapTextures();
      ReleaseSkyTextures();
      ReleaseEntityTextures();
      return;
    }

    UploadMapMesh(_map);
    _parityStaticEntityVertexCount = 0;
    _parityStaticEntityDrawSpans = Array.Empty<DrawSpan>();
    _parityStaticEntityVertices = Array.Empty<BspRenderVertex>();
    _parityEntityVertexCount = 0;
    _parityEntityDrawSpans = Array.Empty<DrawSpan>();
    _parityEntityFrameVertices = Array.Empty<BspRenderVertex>();
    _parityEntityDrawCalls = 0;
    _parityAnimatedObjectUpdates = 0;
    _paritySkippedLayers = 0;
    _parityCulledDynamicInstances = 0;
    _parityEntityLastFrameTick = int.MinValue;
    _parityEntityUploadBufferLength = 0;
    _bspLastFrameTick = int.MinValue;

    BspRenderVertex[] skyRenderVertices = ConvertRenderVertices(_map.SkyRenderVertices);
    UploadMeshLayer(
      _skyVao,
      _skyVbo,
      skyRenderVertices,
      _map.SkyMaterialSpans,
      _map.SkyMaterials,
      _map.SkyMaterialSurfaceIds,
      _map.SkyMaterialAlphaTypes,
      _map.SkySurfaceTextures,
      _skyTextures,
      ref _skyVertexCount,
      ref _skyDrawSpans,
      ref _loadedSkyTextureCount,
      false,
      null,
      _skySurfaceToTexture);

    BspRenderVertex[] entityRenderVertices = ConvertRenderVertices(_map.EntityRenderVertices);
    UploadMeshLayer(
      _entityVao,
      _entityVbo,
      entityRenderVertices,
      _map.EntityMaterialSpans,
      _map.EntityMaterials,
      _map.EntityMaterialSurfaceIds,
      _map.EntityMaterialAlphaTypes,
      _map.EntitySurfaceTextures,
      _entityTextures,
      ref _entityVertexCount,
      ref _entityDrawSpans,
      ref _loadedEntityTextureCount,
      true,
      _mapSurfaceToTexture,
      _entitySurfaceToTexture);
    _entityDrawVertices = entityRenderVertices;
    BuildParityStaticEntityMesh();

    UploadCollisionWallBuffers(_map);

    Vector3[] lineVertices = ConvertDisplayPositions(BuildOverlayLines(_map));
    _lineVertexCount = lineVertices.Length;
    GL.BindVertexArray(_lineVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _lineVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, lineVertices.Length * 12, lineVertices, BufferUsageHint.DynamicDraw);

    Vector3[] particlePositions = ConvertDisplayPositions(_map.ParticleInstancePositions);
    _particleVertexCount = particlePositions.Length;
    GL.BindVertexArray(_particleVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _particleVbo);
    GL.BufferData(
      BufferTarget.ArrayBuffer,
      _particleVertexCount * 12,
      particlePositions,
      BufferUsageHint.DynamicDraw);

    UpdateCollisionDrawPreviewBuffer();
    UpdateCollisionSelectionBuffer();
    UpdateBspSelectionBuffer();
  }

  private void UploadCollisionGeometryOnly()
  {
    if (_map == null)
    {
      _wallVertexCount = 0;
      _wallFrozenVertexCount = 0;
      _collisionDrawPreviewVertexCount = 0;
      _collisionSelectionVertexCount = 0;
      _bspSelectionVertexCount = 0;
      _collisionDrawDragging = false;
      return;
    }

    UploadCollisionWallBuffers(_map);

    UpdateCollisionDrawPreviewBuffer();
    UpdateCollisionSelectionBuffer();
    UpdateBspSelectionBuffer();
  }

  private void UploadCollisionWallBuffers(LoadedMap map)
  {
    (Vector3[] normalWalls, Vector3[] frozenWalls) = BuildCollisionWalls(map);

    Vector3[] normalDisplay = ConvertDisplayPositions(normalWalls);
    _wallVertexCount = normalDisplay.Length;
    GL.BindVertexArray(_wallVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _wallVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, normalDisplay.Length * 12, normalDisplay, BufferUsageHint.DynamicDraw);

    Vector3[] frozenDisplay = ConvertDisplayPositions(frozenWalls);
    _wallFrozenVertexCount = frozenDisplay.Length;
    GL.BindVertexArray(_wallFrozenVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _wallFrozenVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, frozenDisplay.Length * 12, frozenDisplay, BufferUsageHint.DynamicDraw);
  }

  private void UploadMapMesh(LoadedMap map)
  {
    ResetBspMovePreviewState(restorePositions: false);
    _mapDrawVertices = ConvertRenderVertices(map.BspRenderVertices);
    _meshVertexCount = _mapDrawVertices.Length;
    _bspSceneObjects = map.BspSceneObjects;
    bool hasMatchingBspAnimationMetadata =
      map.BspRenderVertexObjectIds.Length == _meshVertexCount &&
      map.BspRenderVertexLocalPositions.Length == _meshVertexCount;
    _bspRenderVertexObjectIds = hasMatchingBspAnimationMetadata
      ? map.BspRenderVertexObjectIds
      : Array.Empty<ushort>();
    _bspRenderVertexLocalPositions = hasMatchingBspAnimationMetadata
      ? map.BspRenderVertexLocalPositions
      : Array.Empty<Vector3>();
    _hasAnimatedBspGeometry =
      hasMatchingBspAnimationMetadata &&
      _bspSceneObjects.Length > 0 &&
      HasAnimatedSceneObjects(_bspSceneObjects);
    _bspLastFrameTick = int.MinValue;

    int meshFloatCount = _meshVertexCount * MeshFloatStride;
    if (_mapUploadBuffer.Length < meshFloatCount)
    {
      _mapUploadBuffer = new float[Math.Max(meshFloatCount, _mapUploadBuffer.Length * 2)];
    }

    float[] meshData = _mapUploadBuffer;
    for (int i = 0; i < _meshVertexCount; ++i)
    {
      int offset = i * MeshFloatStride;
      BspRenderVertex v = _mapDrawVertices[i];
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
    _mapUploadBufferLength = _mapUploadBuffer.Length;
    GL.BufferData(
      BufferTarget.ArrayBuffer,
      meshFloatCount * 4,
      meshData,
      _hasAnimatedBspGeometry ? BufferUsageHint.DynamicDraw : BufferUsageHint.StaticDraw);

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

        int layerCount = material.Layers.Length;
        for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
        {
          if (layerIndex > 0 && envBumpSecondLayer)
          {
            break;
          }

          MaterialLayerDefinition layer = material.Layers[layerIndex];
          int textureId = ResolveLayerTextureId(layer.SurfaceId, mapTextureMap, null);
          bool hasSecondaryLayer = envBumpSecondLayer && layerIndex == 0;
          MaterialLayerDefinition secondaryLayer = hasSecondaryLayer ? material.Layers[1] : default;
          int secondaryTextureId = hasSecondaryLayer
            ? ResolveLayerTextureId(secondaryLayer.SurfaceId, mapTextureMap, null)
            : 0;

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
              secondaryTextureId,
              alphaType,
              0,
              ResolveLegacyLayerColor(material, layerIndex),
              true,
              true,
              layer,
              hasSecondaryLayer,
              secondaryLayer));
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

  private static Vector4 ResolveLegacyLayerColor(MaterialDefinition material, int layerIndex)
  {
    if (layerIndex < 0 || layerIndex >= material.Layers.Length)
    {
      return Vector4.One;
    }

    MaterialLayerDefinition layer = material.Layers[layerIndex];
    Vector4 layerColor = DecodeArgb(layer.Argb);
    bool baseLayerHasTintOrAlpha =
      MathF.Abs(layerColor.X - 1.0f) > 0.001f
      || MathF.Abs(layerColor.Y - 1.0f) > 0.001f
      || MathF.Abs(layerColor.Z - 1.0f) > 0.001f
      || MathF.Abs(layerColor.W - 1.0f) > 0.001f;

    // Base layer is often neutral, but keep non-neutral ARGB so effect meshes
    // (portal water/rings, etc.) do not disappear.
    if (layerIndex == 0)
    {
      return baseLayerHasTintOrAlpha ? layerColor : Vector4.One;
    }

    MaterialLayerDefinition baseLayer = material.Layers[0];
    bool shouldApplyArgb =
      layer.Argb != baseLayer.Argb
      || (layer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0;

    return shouldApplyArgb ? layerColor : Vector4.One;
  }

  private static Vector4 ResolveLegacyEntityLayerColor(MaterialDefinition material, int layerIndex)
  {
    if (layerIndex < 0 || layerIndex >= material.Layers.Length)
    {
      return Vector4.One;
    }

    MaterialLayerDefinition layer = material.Layers[layerIndex];
    Vector4 layerColor = DecodeArgb(layer.Argb);
    bool hasRgbTint =
      MathF.Abs(layerColor.X - 1.0f) > 0.001f
      || MathF.Abs(layerColor.Y - 1.0f) > 0.001f
      || MathF.Abs(layerColor.Z - 1.0f) > 0.001f;

    // Old R3 entity path does not generally apply per-layer ARGB alpha directly.
    // Keep optional RGB tint, but default alpha to 1 unless explicitly animated.
    float alpha = (layer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0
      ? layerColor.W
      : 1.0f;
    if (hasRgbTint || MathF.Abs(alpha - 1.0f) > 0.001f)
    {
      return new Vector4(layerColor.X, layerColor.Y, layerColor.Z, alpha);
    }

    return Vector4.One;
  }

  private static Vector4 MultiplyLayerColor(Vector4 baseColor, Vector4? multiplier)
  {
    if (!multiplier.HasValue)
    {
      return baseColor;
    }

    Vector4 m = multiplier.Value;
    return new Vector4(
      Math.Clamp(baseColor.X * m.X, 0.0f, 1.0f),
      Math.Clamp(baseColor.Y * m.Y, 0.0f, 1.0f),
      Math.Clamp(baseColor.Z * m.Z, 0.0f, 1.0f),
      Math.Clamp(baseColor.W * m.W, 0.0f, 1.0f));
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

  private void BuildParityStaticEntityMesh()
  {
    if (_map == null)
    {
      _parityStaticEntityVertexCount = 0;
      _parityStaticEntityDrawSpans = Array.Empty<DrawSpan>();
      _parityStaticEntityVertices = Array.Empty<BspRenderVertex>();
      return;
    }

    BuildParityEntityGeometry(
      nowFrame: 0.0f,
      includeAnimatedModels: false,
      includeStaticModels: true,
      out BspRenderVertex[] vertices,
      out DrawSpan[] spans,
      out _,
      out _,
      out _);

    _parityStaticEntityVertexCount = vertices.Length;
    _parityStaticEntityDrawSpans = spans;
    _parityStaticEntityVertices = vertices;
    if (_parityStaticEntityVertexCount <= 0)
    {
      return;
    }

    float[] meshData = new float[_parityStaticEntityVertexCount * MeshFloatStride];
    for (int i = 0; i < _parityStaticEntityVertexCount; ++i)
    {
      int offset = i * MeshFloatStride;
      BspRenderVertex vertex = vertices[i];
      meshData[offset] = vertex.Position.X;
      meshData[offset + 1] = vertex.Position.Y;
      meshData[offset + 2] = vertex.Position.Z;
      meshData[offset + 3] = vertex.Uv.X;
      meshData[offset + 4] = vertex.Uv.Y;
      meshData[offset + 5] = vertex.LightUv.X;
      meshData[offset + 6] = vertex.LightUv.Y;
      meshData[offset + 7] = vertex.Color.X;
      meshData[offset + 8] = vertex.Color.Y;
      meshData[offset + 9] = vertex.Color.Z;
    }

    GL.BindVertexArray(_parityStaticEntityVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _parityStaticEntityVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, meshData.Length * 4, meshData, BufferUsageHint.StaticDraw);
  }

  private void PrepareParityEntityFrameMesh()
  {
    if (_map == null)
    {
      return;
    }

    int frameTick = (int)MathF.Floor((float)_animationClock.Elapsed.TotalSeconds * 30.0f);
    if (frameTick == _parityEntityLastFrameTick && _parityEntityVertexCount > 0)
    {
      return;
    }

    BuildParityEntityFrame(
      (float)_animationClock.Elapsed.TotalSeconds,
      out BspRenderVertex[] vertices,
      out DrawSpan[] spans);

    _parityEntityLastFrameTick = frameTick;
    _parityEntityVertexCount = vertices.Length;
    _parityEntityDrawSpans = spans;
    _parityEntityFrameVertices = vertices;
    _parityEntityDrawCalls = spans.Length;
    if (_parityEntityVertexCount <= 0)
    {
      return;
    }

    int floatCount = _parityEntityVertexCount * MeshFloatStride;
    if (_parityEntityUploadBuffer.Length < floatCount)
    {
      _parityEntityUploadBuffer = new float[Math.Max(floatCount, _parityEntityUploadBuffer.Length * 2)];
    }

    float[] meshData = _parityEntityUploadBuffer;
    for (int i = 0; i < _parityEntityVertexCount; ++i)
    {
      int offset = i * MeshFloatStride;
      BspRenderVertex vertex = vertices[i];
      meshData[offset] = vertex.Position.X;
      meshData[offset + 1] = vertex.Position.Y;
      meshData[offset + 2] = vertex.Position.Z;
      meshData[offset + 3] = vertex.Uv.X;
      meshData[offset + 4] = vertex.Uv.Y;
      meshData[offset + 5] = vertex.LightUv.X;
      meshData[offset + 6] = vertex.LightUv.Y;
      meshData[offset + 7] = vertex.Color.X;
      meshData[offset + 8] = vertex.Color.Y;
      meshData[offset + 9] = vertex.Color.Z;
    }

    GL.BindVertexArray(_parityEntityVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _parityEntityVbo);
    if (_parityEntityUploadBufferLength < floatCount)
    {
      _parityEntityUploadBufferLength = _parityEntityUploadBuffer.Length;
      GL.BufferData(BufferTarget.ArrayBuffer, _parityEntityUploadBufferLength * 4, IntPtr.Zero, BufferUsageHint.DynamicDraw);
    }

    GL.BufferSubData(BufferTarget.ArrayBuffer, IntPtr.Zero, floatCount * 4, meshData);
  }

  private void PrepareAnimatedBspFrameMesh()
  {
    if (_map == null || !_hasAnimatedBspGeometry || _meshVertexCount <= 0)
    {
      return;
    }

    if (_bspRenderVertexObjectIds.Length != _meshVertexCount || _bspRenderVertexLocalPositions.Length != _meshVertexCount)
    {
      return;
    }

    int frameTick = (int)MathF.Floor((float)_animationClock.Elapsed.TotalSeconds * 30.0f);
    if (frameTick == _bspLastFrameTick)
    {
      return;
    }

    _bspLastFrameTick = frameTick;
    float nowFrame = (float)_animationClock.Elapsed.TotalSeconds * 30.0f;
    NumericsMatrix4[] objectMatrices = BuildSceneObjectMatrices(_bspSceneObjects, nowFrame);

    int floatCount = _meshVertexCount * MeshFloatStride;
    if (_mapUploadBuffer.Length < floatCount)
    {
      _mapUploadBuffer = new float[Math.Max(floatCount, _mapUploadBuffer.Length * 2)];
      for (int vertexIndex = 0; vertexIndex < _meshVertexCount; ++vertexIndex)
      {
        int offset = vertexIndex * MeshFloatStride;
        BspRenderVertex vertex = _mapDrawVertices[vertexIndex];
        _mapUploadBuffer[offset] = vertex.Position.X;
        _mapUploadBuffer[offset + 1] = vertex.Position.Y;
        _mapUploadBuffer[offset + 2] = vertex.Position.Z;
        _mapUploadBuffer[offset + 3] = vertex.Uv.X;
        _mapUploadBuffer[offset + 4] = vertex.Uv.Y;
        _mapUploadBuffer[offset + 5] = vertex.LightUv.X;
        _mapUploadBuffer[offset + 6] = vertex.LightUv.Y;
        _mapUploadBuffer[offset + 7] = vertex.Color.X;
        _mapUploadBuffer[offset + 8] = vertex.Color.Y;
        _mapUploadBuffer[offset + 9] = vertex.Color.Z;
      }
    }

    for (int vertexIndex = 0; vertexIndex < _meshVertexCount; ++vertexIndex)
    {
      ushort objectId = _bspRenderVertexObjectIds[vertexIndex];
      Vector3 localPosition = _bspRenderVertexLocalPositions[vertexIndex];
      Vector3 worldPosition = localPosition;
      if (TryGetSceneObjectMatrix(objectMatrices, objectId, out NumericsMatrix4 objectMatrix))
      {
        worldPosition = TransformEntityPosition(worldPosition, objectMatrix);
      }

      Vector3 displayPosition = ConvertWorldPosition(worldPosition);
      BspRenderVertex sourceVertex = _mapDrawVertices[vertexIndex];
      _mapDrawVertices[vertexIndex] = new BspRenderVertex(displayPosition, sourceVertex.Uv, sourceVertex.LightUv, sourceVertex.Color);

      int offset = vertexIndex * MeshFloatStride;
      _mapUploadBuffer[offset] = displayPosition.X;
      _mapUploadBuffer[offset + 1] = displayPosition.Y;
      _mapUploadBuffer[offset + 2] = displayPosition.Z;
    }

    GL.BindVertexArray(_meshVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _meshVbo);
    if (_mapUploadBufferLength < floatCount)
    {
      _mapUploadBufferLength = _mapUploadBuffer.Length;
      GL.BufferData(BufferTarget.ArrayBuffer, _mapUploadBufferLength * 4, IntPtr.Zero, BufferUsageHint.DynamicDraw);
    }

    GL.BufferSubData(BufferTarget.ArrayBuffer, IntPtr.Zero, floatCount * 4, _mapUploadBuffer);
  }

  private void BuildParityEntityFrame(float animSeconds, out BspRenderVertex[] vertices, out DrawSpan[] spans)
  {
    BuildParityEntityGeometry(
      nowFrame: animSeconds * 30.0f,
      includeAnimatedModels: true,
      includeStaticModels: false,
      out vertices,
      out spans,
      out int animatedObjectUpdates,
      out int skippedLayers,
      out int culledDynamicInstances);

    _parityAnimatedObjectUpdates = animatedObjectUpdates;
    _paritySkippedLayers = skippedLayers;
    _parityCulledDynamicInstances = culledDynamicInstances;
  }

  private void BuildParityEntityGeometry(
    float nowFrame,
    bool includeAnimatedModels,
    bool includeStaticModels,
    out BspRenderVertex[] vertices,
    out DrawSpan[] spans,
    out int animatedObjectUpdates,
    out int skippedLayers,
    out int culledDynamicInstances)
  {
    animatedObjectUpdates = 0;
    skippedLayers = 0;
    culledDynamicInstances = 0;
    if (_map == null)
    {
      vertices = Array.Empty<BspRenderVertex>();
      spans = Array.Empty<DrawSpan>();
      return;
    }

    EntitySceneData sceneData = _map.EntityScene;
    if (sceneData.Models.Length == 0 || sceneData.Instances.Length == 0)
    {
      vertices = Array.Empty<BspRenderVertex>();
      spans = Array.Empty<DrawSpan>();
      return;
    }

    List<BspRenderVertex> frameVertices = new(Math.Max(8192, sceneData.Instances.Length * 128));
    List<DrawSpan> frameSpans = new(Math.Max(512, sceneData.Instances.Length * 4));
    Dictionary<int, NumericsMatrix4[]> modelObjectMatrices = new(Math.Max(16, sceneData.Models.Length));
    Vector3 sourceCameraPosition = ConvertSourcePosition(_camera.Position);

    for (int instanceIndex = 0; instanceIndex < sceneData.Instances.Length; ++instanceIndex)
    {
      EntitySceneInstance instance = sceneData.Instances[instanceIndex];
      if (!_entitySceneModelsById.TryGetValue(instance.EntityId, out EntitySceneModel? model))
      {
        continue;
      }

      if (!_entitySceneModelHasAnimation.TryGetValue(instance.EntityId, out bool modelHasAnimation))
      {
        continue;
      }

      if ((modelHasAnimation && !includeAnimatedModels) || (!modelHasAnimation && !includeStaticModels))
      {
        continue;
      }

      float instanceScale = float.IsFinite(instance.Scale) ? instance.Scale : 1.0f;
      if (MathF.Abs(instanceScale) < 0.00001f)
      {
        instanceScale = 1.0f;
      }

      if (includeAnimatedModels && modelHasAnimation
        && ShouldCullParityDynamicInstance(instance, instanceScale, model.EntityId, sourceCameraPosition))
      {
        ++culledDynamicInstances;
        continue;
      }

      if (model.MatGroups.Length == 0)
      {
        continue;
      }

      if (!modelObjectMatrices.TryGetValue(instance.EntityId, out NumericsMatrix4[]? objectMatrices))
      {
        objectMatrices = BuildSceneObjectMatrices(model.Objects, nowFrame);
        modelObjectMatrices[instance.EntityId] = objectMatrices;
        animatedObjectUpdates += objectMatrices.Length;
      }

      if (_showParticleMarkers && model.ParticleRuntime is ParticleRuntimeDefinition particleRuntime)
      {
        AppendParticleRuntimeGeometry(
          frameVertices,
          frameSpans,
          model,
          instance,
          instanceIndex,
          objectMatrices,
          particleRuntime,
          nowFrame,
          instanceScale,
          sourceCameraPosition,
          ref skippedLayers);
        continue;
      }

      if (model.ParticleRuntime.HasValue)
      {
        // Keep parity with the UI toggle: when FX is hidden, skip runtime particle entity geometry too.
        continue;
      }

      for (int groupIndex = 0; groupIndex < model.MatGroups.Length; ++groupIndex)
      {
        EntitySceneMatGroup matGroup = model.MatGroups[groupIndex];
        BspRenderVertex[] localVertices = matGroup.LocalVertices;
        if (localVertices.Length == 0)
        {
          continue;
        }

        bool hasObjectMatrix = TryGetSceneObjectMatrix(objectMatrices, matGroup.ObjectId, out NumericsMatrix4 objectMatrix);
        int startVertex = frameVertices.Count;
        for (int vertexIndex = 0; vertexIndex < localVertices.Length; ++vertexIndex)
        {
          BspRenderVertex localVertex = localVertices[vertexIndex];
          Vector3 worldPosition = localVertex.Position;
          if (hasObjectMatrix)
          {
            worldPosition = TransformEntityPosition(worldPosition, objectMatrix);
          }

          worldPosition = TransformEntityInstancePosition(
            worldPosition,
            instanceScale,
            instance.RotX,
            instance.RotY,
            instance.Position);

          frameVertices.Add(new BspRenderVertex(ConvertWorldPosition(worldPosition), localVertex.Uv, localVertex.LightUv, localVertex.Color));
        }

        int vertexCount = frameVertices.Count - startVertex;
        if (vertexCount <= 0)
        {
          continue;
        }

        int materialId = matGroup.MaterialId >= 0
          ? matGroup.MaterialId + model.MaterialBase
          : matGroup.MaterialId;
        AddParityMaterialSpans(frameSpans, materialId, startVertex, vertexCount, ref skippedLayers);
      }
    }

    if (frameSpans.Count == 0 && frameVertices.Count > 0)
    {
      frameSpans.Add(new DrawSpan(0, frameVertices.Count, _checkerTexture, 0, 0, 0, Vector4.One, true, false, default, false, default));
    }

    vertices = frameVertices.ToArray();
    spans = frameSpans.ToArray();
  }

  private void AppendParticleRuntimeGeometry(
    List<BspRenderVertex> frameVertices,
    List<DrawSpan> frameSpans,
    EntitySceneModel model,
    EntitySceneInstance instance,
    int instanceIndex,
    NumericsMatrix4[] objectMatrices,
    ParticleRuntimeDefinition runtime,
    float nowFrame,
    float instanceScale,
    Vector3 sourceCameraPosition,
    ref int skippedLayers)
  {
    int particleCount = Math.Clamp(runtime.Count, 1, 2048);
    for (int particleIndex = 0; particleIndex < particleCount; ++particleIndex)
    {
      ParticleRuntimeState state = EvaluateParticleRuntimeState(runtime, instance, instanceIndex, particleIndex, nowFrame);
      if (!state.IsAlive)
      {
        continue;
      }

      Vector3 particleRight;
      Vector3 particleUp;
      Vector3 particleForward;
      BuildParticleBillboardBasis(
        runtime,
        state.PositionOffset,
        instanceScale,
        instance,
        sourceCameraPosition,
        out particleRight,
        out particleUp,
        out particleForward);

      Vector4 colorOverride = new(
        Math.Clamp(state.Color.X / 255.0f, 0.0f, 1.0f),
        Math.Clamp(state.Color.Y / 255.0f, 0.0f, 1.0f),
        Math.Clamp(state.Color.Z / 255.0f, 0.0f, 1.0f),
        Math.Clamp(state.Alpha, 0.0f, 1.0f));

      for (int groupIndex = 0; groupIndex < model.MatGroups.Length; ++groupIndex)
      {
        EntitySceneMatGroup matGroup = model.MatGroups[groupIndex];
        BspRenderVertex[] localVertices = matGroup.LocalVertices;
        if (localVertices.Length == 0)
        {
          continue;
        }

        bool hasObjectMatrix = TryGetSceneObjectMatrix(objectMatrices, matGroup.ObjectId, out NumericsMatrix4 objectMatrix);
        int startVertex = frameVertices.Count;
        for (int vertexIndex = 0; vertexIndex < localVertices.Length; ++vertexIndex)
        {
          BspRenderVertex localVertex = localVertices[vertexIndex];
          Vector3 localPosition = localVertex.Position;
          if (hasObjectMatrix)
          {
            localPosition = TransformEntityPosition(localPosition, objectMatrix);
          }

          localPosition *= state.Scale;
          localPosition = RotateY(localPosition, state.YRotRadians);
          localPosition = RotateZ(localPosition, state.ZRotRadians);

          if (!runtime.NoBillboard)
          {
            localPosition =
              particleRight * localPosition.X +
              particleUp * localPosition.Y +
              particleForward * localPosition.Z;
          }

          localPosition += state.PositionOffset;
          Vector3 worldPosition = runtime.Free
            ? localPosition + instance.Position
            : TransformEntityInstancePosition(
                localPosition,
                instanceScale,
                instance.RotX,
                instance.RotY,
                instance.Position);

          if (runtime.ZFrontEnabled && MathF.Abs(runtime.ZFront) > 0.00001f)
          {
            Vector3 viewDirection = sourceCameraPosition - worldPosition;
            if (viewDirection.LengthSquared > 0.000001f)
            {
              worldPosition += Vector3.Normalize(viewDirection) * runtime.ZFront;
            }
          }

          frameVertices.Add(new BspRenderVertex(ConvertWorldPosition(worldPosition), localVertex.Uv, localVertex.LightUv, localVertex.Color));
        }

        int vertexCount = frameVertices.Count - startVertex;
        if (vertexCount <= 0)
        {
          continue;
        }

        int materialId = matGroup.MaterialId >= 0
          ? matGroup.MaterialId + model.MaterialBase
          : matGroup.MaterialId;
        AddParityMaterialSpans(frameSpans, materialId, startVertex, vertexCount, ref skippedLayers, colorOverride);
      }
    }
  }

  private static ParticleRuntimeState EvaluateParticleRuntimeState(
    ParticleRuntimeDefinition runtime,
    EntitySceneInstance instance,
    int instanceIndex,
    int particleIndex,
    float nowFrame)
  {
    uint seedBase = CreateParticleSeed(instance.EntityId, instanceIndex, particleIndex, 0xA17C9E3Du);

    float liveTime = MathF.Max(0.001f, SampleParticleRange(runtime.LiveTime, seedBase ^ 0x1001u));
    float timeSpeed = MathF.Max(0.001f, SampleParticleRange(runtime.TimeSpeed, seedBase ^ 0x1002u));
    float lifeSeconds = liveTime / timeSpeed;
    if (!float.IsFinite(lifeSeconds) || lifeSeconds <= 0.0f)
    {
      lifeSeconds = 0.001f;
    }

    float nowSeconds = MathF.Max(0.0f, nowFrame / 30.0f);
    int emissionCount = Math.Max(1, runtime.Count);
    float onePerTime = lifeSeconds / emissionCount;
    float emissionScale = Math.Clamp(1.0f - runtime.StartTimeRange, 0.0001f, 1.0f);
    float emissionInterval = MathF.Max(0.0001f, onePerTime * emissionScale);
    float emitJitter = (SampleParticleUnit(seedBase ^ 0x1003u) - 0.5f) * MathF.Max(0.0f, runtime.CreateTimeEpsilon) * emissionScale;
    float phase = particleIndex * emissionInterval + emitJitter;

    float emitLimit = runtime.EmitTimeEnabled
      ? MathF.Max(0.0f, SampleParticleRange(runtime.EmitTime, seedBase ^ 0x1004u))
      : float.MaxValue;

    if (!TryResolveParticleAge(runtime, nowSeconds, lifeSeconds, phase, emitLimit, out float ageSeconds))
    {
      return new ParticleRuntimeState(false, Vector3.Zero, Vector3.One * 255.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    }

    float trackTime = ageSeconds * timeSpeed;
    Vector3 startPos = BuildParticleStartPosition(runtime, seedBase ^ 0x1100u);
    Vector3 gravity = new(
      SampleParticleRange(runtime.GravityX, seedBase ^ 0x1201u),
      SampleParticleRange(runtime.GravityY, seedBase ^ 0x1202u),
      SampleParticleRange(runtime.GravityZ, seedBase ^ 0x1203u));
    if (runtime.Free)
    {
      gravity = RotateGravityLikeLegacy(gravity, instance.RotX, instance.RotY);
    }

    float powerX = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartPowerX, seedBase ^ 0x1301u),
      trackTime,
      seedBase,
      ParticleTrackChannel.PowerX);
    float powerY = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartPowerY, seedBase ^ 0x1302u),
      trackTime,
      seedBase,
      ParticleTrackChannel.PowerY);
    float powerZ = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartPowerZ, seedBase ^ 0x1303u),
      trackTime,
      seedBase,
      ParticleTrackChannel.PowerZ);

    Vector3 power = new(powerX, powerY, powerZ);
    if (runtime.Free)
    {
      power = RotateGravityLikeLegacy(power, instance.RotX, instance.RotY);
    }

    Vector3 positionOffset = startPos + power * trackTime + gravity * (0.5f * trackTime * trackTime);
    if (!float.IsFinite(positionOffset.X) || !float.IsFinite(positionOffset.Y) || !float.IsFinite(positionOffset.Z))
    {
      positionOffset = startPos;
    }

    float scale = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartScale, seedBase ^ 0x1401u),
      trackTime,
      seedBase,
      ParticleTrackChannel.Scale);
    if (!float.IsFinite(scale))
    {
      scale = 1.0f;
    }

    scale = MathF.Max(0.0001f, scale);

    float alpha = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartAlpha, seedBase ^ 0x1402u),
      trackTime,
      seedBase,
      ParticleTrackChannel.Alpha);
    alpha = Math.Clamp(alpha, 0.0f, 255.0f);

    float colorR = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartColorR, seedBase ^ 0x1403u),
      trackTime,
      seedBase,
      ParticleTrackChannel.ColorR);
    float colorG = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartColorG, seedBase ^ 0x1404u),
      trackTime,
      seedBase,
      ParticleTrackChannel.ColorG);
    float colorB = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartColorB, seedBase ^ 0x1405u),
      trackTime,
      seedBase,
      ParticleTrackChannel.ColorB);
    Vector3 color = new(
      Math.Clamp(colorR, 0.0f, 255.0f),
      Math.Clamp(colorG, 0.0f, 255.0f),
      Math.Clamp(colorB, 0.0f, 255.0f));

    float zRotDegrees = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartZRot, seedBase ^ 0x1501u),
      trackTime,
      seedBase,
      ParticleTrackChannel.ZRot);
    float yRotDegrees = EvaluateParticleTrackValue(
      runtime.Tracks,
      SampleParticleRange(runtime.StartYRot, seedBase ^ 0x1502u),
      trackTime,
      seedBase,
      ParticleTrackChannel.YRot);

    if (IsParticleFlickerActive(runtime, trackTime, seedBase))
    {
      float minFlickerAlpha = Math.Clamp(SampleParticleRange(runtime.FlickerAlpha, seedBase ^ 0x1601u), 0.0f, 255.0f);
      if (alpha > minFlickerAlpha)
      {
        alpha = minFlickerAlpha;
      }
    }

    return new ParticleRuntimeState(
      true,
      positionOffset,
      color,
      Math.Clamp(alpha / 255.0f, 0.0f, 1.0f),
      scale,
      MathHelper.DegreesToRadians(zRotDegrees),
      MathHelper.DegreesToRadians(yRotDegrees));
  }

  private static bool TryResolveParticleAge(
    ParticleRuntimeDefinition runtime,
    float nowSeconds,
    float lifeSeconds,
    float phase,
    float emitLimit,
    out float ageSeconds)
  {
    ageSeconds = 0.0f;
    if (!float.IsFinite(lifeSeconds) || lifeSeconds <= 0.00001f)
    {
      return false;
    }

    float birthTime = MathF.Max(0.0f, phase);
    if (runtime.AlwaysLive)
    {
      float alwaysAge = PositiveModulo(nowSeconds + birthTime, lifeSeconds);
      ageSeconds = float.IsFinite(alwaysAge) ? alwaysAge : 0.0f;
      return true;
    }

    if (runtime.NoLoop)
    {
      if (birthTime > emitLimit || nowSeconds < birthTime)
      {
        return false;
      }

      float singleAge = nowSeconds - birthTime;
      if (!float.IsFinite(singleAge) || singleAge < 0.0f || singleAge >= lifeSeconds)
      {
        return false;
      }

      ageSeconds = singleAge;
      return true;
    }

    if (nowSeconds < birthTime)
    {
      return false;
    }

    if (!runtime.EmitTimeEnabled || emitLimit >= float.MaxValue * 0.5f)
    {
      ageSeconds = PositiveModulo(nowSeconds - birthTime, lifeSeconds);
      return true;
    }

    if (birthTime > emitLimit)
    {
      return false;
    }

    if (nowSeconds <= emitLimit)
    {
      ageSeconds = PositiveModulo(nowSeconds - birthTime, lifeSeconds);
      return true;
    }

    float elapsedAtEmitLimit = emitLimit - birthTime;
    if (elapsedAtEmitLimit < 0.0f)
    {
      return false;
    }

    float ageAtEmitLimit = PositiveModulo(elapsedAtEmitLimit, lifeSeconds);
    float lastBirth = emitLimit - ageAtEmitLimit;
    float particleEnd = lastBirth + lifeSeconds;
    if (nowSeconds >= particleEnd)
    {
      return false;
    }

    ageSeconds = nowSeconds - lastBirth;
    return ageSeconds >= 0.0f && ageSeconds < lifeSeconds;
  }

  private static Vector3 BuildParticleStartPosition(ParticleRuntimeDefinition runtime, uint seedBase)
  {
    if (runtime.SpawnShape == ParticleSpawnShape.Box)
    {
      return new Vector3(
        SampleParticleRange(runtime.StartPosX, seedBase ^ 0x1111u),
        SampleParticleRange(runtime.StartPosY, seedBase ^ 0x2222u),
        SampleParticleRange(runtime.StartPosZ, seedBase ^ 0x3333u));
    }

    Vector3 center = new(
      (runtime.StartPosX.Min + runtime.StartPosX.Max) * 0.5f,
      (runtime.StartPosY.Min + runtime.StartPosY.Max) * 0.5f,
      (runtime.StartPosZ.Min + runtime.StartPosZ.Max) * 0.5f);
    Vector3 radius = new(
      MathF.Abs(runtime.StartPosX.Max - runtime.StartPosX.Min) * 0.5f,
      MathF.Abs(runtime.StartPosY.Max - runtime.StartPosY.Min) * 0.5f,
      MathF.Abs(runtime.StartPosZ.Max - runtime.StartPosZ.Min) * 0.5f);
    if (!float.IsFinite(radius.X) || radius.X < 0.0f)
    {
      radius.X = 0.0f;
    }

    if (!float.IsFinite(radius.Y) || radius.Y < 0.0f)
    {
      radius.Y = 0.0f;
    }

    if (!float.IsFinite(radius.Z) || radius.Z < 0.0f)
    {
      radius.Z = 0.0f;
    }

    Vector3 direction = SampleUnitSphereDirection(seedBase ^ 0x4444u, seedBase ^ 0x5555u);
    float distance = runtime.SpawnShape == ParticleSpawnShape.SphereEdge
      ? 1.0f
      : MathF.Pow(Math.Clamp(SampleParticleUnit(seedBase ^ 0x6666u), 0.0f, 1.0f), 1.0f / 3.0f);

    Vector3 point = new(
      center.X + direction.X * radius.X * distance,
      center.Y + direction.Y * radius.Y * distance,
      center.Z + direction.Z * radius.Z * distance);
    return IsFinite(point) ? point : center;
  }

  private static Vector3 RotateGravityLikeLegacy(Vector3 value, float rotXDeg, float rotYDeg)
  {
    if (!IsFinite(value))
    {
      return Vector3.Zero;
    }

    Vector3 rotated = RotateY(value, MathHelper.DegreesToRadians(rotYDeg));
    rotated = RotateX(rotated, MathHelper.DegreesToRadians(rotXDeg));
    return IsFinite(rotated) ? rotated : value;
  }

  private static bool IsParticleFlickerActive(
    ParticleRuntimeDefinition runtime,
    float trackTime,
    uint seedBase)
  {
    bool flickerState = runtime.FlickerEnabled;
    if (runtime.Tracks.Length > 0)
    {
      for (int trackIndex = 0; trackIndex < runtime.Tracks.Length; ++trackIndex)
      {
        ParticleTrackKey track = runtime.Tracks[trackIndex];
        if (!track.HasFlicker || trackTime + 0.00001f < track.Time)
        {
          continue;
        }

        flickerState = !flickerState;
      }
    }

    if (!flickerState)
    {
      return false;
    }

    float flickerPeriod = SampleParticleRange(runtime.FlickerTime, seedBase ^ 0x1701u);
    if (!float.IsFinite(flickerPeriod) || flickerPeriod <= 0.00001f)
    {
      return false;
    }

    float phase = PositiveModulo(trackTime, flickerPeriod);
    return phase < flickerPeriod * 0.5f;
  }

  private static float EvaluateParticleTrackValue(
    ParticleTrackKey[] tracks,
    float initialValue,
    float trackTime,
    uint seedBase,
    ParticleTrackChannel channel)
  {
    if (tracks.Length == 0)
    {
      return initialValue;
    }

    float previousValue = initialValue;
    float previousTime = 0.0f;
    for (int trackIndex = 0; trackIndex < tracks.Length; ++trackIndex)
    {
      ParticleTrackKey track = tracks[trackIndex];
      if (!TryGetParticleTrackRange(track, channel, out ParticleFloatRange range))
      {
        continue;
      }

      uint trackSeed = seedBase ^ unchecked((uint)trackIndex * 0x45D9F3Bu) ^ (unchecked((uint)channel) * 0x9E3779B9u);
      float trackValue = SampleParticleRange(range, trackSeed);
      if (trackTime <= track.Time)
      {
        if (track.Time <= previousTime + 0.00001f)
        {
          return trackValue;
        }

        float alpha = (trackTime - previousTime) / (track.Time - previousTime);
        alpha = Math.Clamp(alpha, 0.0f, 1.0f);
        return MathHelper.Lerp(previousValue, trackValue, alpha);
      }

      previousValue = trackValue;
      previousTime = track.Time;
    }

    return previousValue;
  }

  private static bool TryGetParticleTrackRange(ParticleTrackKey track, ParticleTrackChannel channel, out ParticleFloatRange range)
  {
    range = default;
    switch (channel)
    {
      case ParticleTrackChannel.Scale:
        if (track.HasScale)
        {
          range = track.Scale;
          return true;
        }

        return false;
      case ParticleTrackChannel.Alpha:
        if (track.HasAlpha)
        {
          range = track.Alpha;
          return true;
        }

        return false;
      case ParticleTrackChannel.ColorR:
        if (track.HasColor)
        {
          range = track.ColorR;
          return true;
        }

        return false;
      case ParticleTrackChannel.ColorG:
        if (track.HasColor)
        {
          range = track.ColorG;
          return true;
        }

        return false;
      case ParticleTrackChannel.ColorB:
        if (track.HasColor)
        {
          range = track.ColorB;
          return true;
        }

        return false;
      case ParticleTrackChannel.PowerX:
        if (track.HasPower)
        {
          range = track.PowerX;
          return true;
        }

        return false;
      case ParticleTrackChannel.PowerY:
        if (track.HasPower)
        {
          range = track.PowerY;
          return true;
        }

        return false;
      case ParticleTrackChannel.PowerZ:
        if (track.HasPower)
        {
          range = track.PowerZ;
          return true;
        }

        return false;
      case ParticleTrackChannel.ZRot:
        if (track.HasZRot)
        {
          range = track.ZRot;
          return true;
        }

        return false;
      case ParticleTrackChannel.YRot:
        if (track.HasYRot)
        {
          range = track.YRot;
          return true;
        }

        return false;
      default:
        return false;
    }
  }

  private static float SampleParticleRange(ParticleFloatRange range, uint seed)
  {
    float min = range.Min;
    float max = range.Max;
    if (!float.IsFinite(min))
    {
      min = 0.0f;
    }

    if (!float.IsFinite(max))
    {
      max = min;
    }

    if (max < min)
    {
      (min, max) = (max, min);
    }

    if (MathF.Abs(max - min) < 0.00001f)
    {
      return min;
    }

    return min + (max - min) * SampleParticleUnit(seed);
  }

  private static uint CreateParticleSeed(int entityId, int instanceIndex, int particleIndex, uint salt)
  {
    uint seed = 2166136261u;
    seed = (seed ^ (uint)entityId) * 16777619u;
    seed = (seed ^ (uint)instanceIndex) * 16777619u;
    seed = (seed ^ (uint)particleIndex) * 16777619u;
    seed = (seed ^ salt) * 16777619u;
    return MixParticleSeed(seed);
  }

  private static uint MixParticleSeed(uint value)
  {
    value ^= value >> 16;
    value *= 0x7FEB352Du;
    value ^= value >> 15;
    value *= 0x846CA68Bu;
    value ^= value >> 16;
    return value;
  }

  private static float SampleParticleUnit(uint seed)
  {
    uint mixed = MixParticleSeed(seed);
    return (mixed & 0x00FFFFFFu) / 16777215.0f;
  }

  private static Vector3 SampleUnitSphereDirection(uint seedA, uint seedB)
  {
    float u = Math.Clamp(SampleParticleUnit(seedA), 0.0f, 1.0f);
    float v = Math.Clamp(SampleParticleUnit(seedB), 0.0f, 1.0f);
    float z = u * 2.0f - 1.0f;
    float theta = v * MathF.Tau;
    float radial = MathF.Sqrt(MathF.Max(0.0f, 1.0f - z * z));
    Vector3 direction = new(
      radial * MathF.Cos(theta),
      z,
      radial * MathF.Sin(theta));
    return direction.LengthSquared > 0.000001f ? Vector3.Normalize(direction) : Vector3.UnitY;
  }

  private static float PositiveModulo(float value, float period)
  {
    if (!float.IsFinite(value) || !float.IsFinite(period) || period <= 0.00001f)
    {
      return 0.0f;
    }

    float modulo = value % period;
    return modulo < 0.0f ? modulo + period : modulo;
  }

  private static Vector3 RotateX(Vector3 value, float radians)
  {
    if (MathF.Abs(radians) < 0.000001f)
    {
      return value;
    }

    float c = MathF.Cos(radians);
    float s = MathF.Sin(radians);
    return new Vector3(
      value.X,
      value.Y * c - value.Z * s,
      value.Y * s + value.Z * c);
  }

  private static Vector3 RotateY(Vector3 value, float radians)
  {
    if (MathF.Abs(radians) < 0.000001f)
    {
      return value;
    }

    float c = MathF.Cos(radians);
    float s = MathF.Sin(radians);
    return new Vector3(
      value.X * c + value.Z * s,
      value.Y,
      -value.X * s + value.Z * c);
  }

  private static Vector3 RotateZ(Vector3 value, float radians)
  {
    if (MathF.Abs(radians) < 0.000001f)
    {
      return value;
    }

    float c = MathF.Cos(radians);
    float s = MathF.Sin(radians);
    return new Vector3(
      value.X * c - value.Y * s,
      value.X * s + value.Y * c,
      value.Z);
  }

  private static void BuildParticleBillboardBasis(
    ParticleRuntimeDefinition runtime,
    Vector3 particleLocalOffset,
    float instanceScale,
    EntitySceneInstance instance,
    Vector3 sourceCameraPosition,
    out Vector3 right,
    out Vector3 up,
    out Vector3 forward)
  {
    if (runtime.NoBillboard)
    {
      right = Vector3.UnitX;
      up = Vector3.UnitY;
      forward = Vector3.UnitZ;
      return;
    }

    Vector3 particleWorld = runtime.Free
      ? particleLocalOffset + instance.Position
      : TransformEntityInstancePosition(
          particleLocalOffset,
          instanceScale,
          instance.RotX,
          instance.RotY,
          instance.Position);
    Vector3 toCamera = sourceCameraPosition - particleWorld;
    if (runtime.YBillboard)
    {
      toCamera.Y = 0.0f;
      if (toCamera.LengthSquared < 0.000001f)
      {
        toCamera = new Vector3(0.0f, 0.0f, 1.0f);
      }

      forward = Vector3.Normalize(toCamera);
      right = Vector3.Cross(Vector3.UnitY, forward);
      if (right.LengthSquared < 0.000001f)
      {
        right = Vector3.UnitX;
      }
      else
      {
        right = Vector3.Normalize(right);
      }

      up = Vector3.UnitY;
      return;
    }

    if (toCamera.LengthSquared < 0.000001f)
    {
      toCamera = new Vector3(0.0f, 0.0f, 1.0f);
    }

    forward = Vector3.Normalize(toCamera);
    Vector3 referenceUp = runtime.ZBillboard ? Vector3.UnitX : Vector3.UnitY;
    right = Vector3.Cross(referenceUp, forward);
    if (right.LengthSquared < 0.000001f)
    {
      right = Vector3.UnitX;
    }
    else
    {
      right = Vector3.Normalize(right);
    }

    up = Vector3.Cross(forward, right);
    if (up.LengthSquared < 0.000001f)
    {
      up = Vector3.UnitY;
    }
    else
    {
      up = Vector3.Normalize(up);
    }
  }

  private void AddParityMaterialSpans(
    List<DrawSpan> spans,
    int materialId,
    int startVertex,
    int vertexCount,
    ref int skippedLayers,
    Vector4? layerColorMultiplier = null)
  {
    if (_map == null || vertexCount <= 0)
    {
      return;
    }

    bool hasMaterial = _enableR3mMaterials
      && materialId >= 0
      && materialId < _map.EntityMaterials.Length;
    if (hasMaterial)
    {
      MaterialDefinition material = _map.EntityMaterials[materialId];
      bool envBumpSecondLayer = material.Layers.Length > 1
        && (material.Layers[1].LayerFlags & MaterialLayerFlagEnvBump) != 0;
      if (envBumpSecondLayer && material.Layers.Length > 1)
      {
        skippedLayers += material.Layers.Length - 1;
      }

      for (int layerIndex = 0; layerIndex < material.Layers.Length; ++layerIndex)
      {
        if (layerIndex > 0 && envBumpSecondLayer)
        {
          break;
        }

        MaterialLayerDefinition layer = material.Layers[layerIndex];
        int textureId = ResolveLayerTextureId(layer.SurfaceId, _entitySurfaceToTexture, _mapSurfaceToTexture);
        bool hasSecondaryLayer = envBumpSecondLayer && layerIndex == 0;
        MaterialLayerDefinition secondaryLayer = hasSecondaryLayer ? material.Layers[1] : default;
        int secondaryTextureId = hasSecondaryLayer
          ? ResolveLayerTextureId(secondaryLayer.SurfaceId, _entitySurfaceToTexture, _mapSurfaceToTexture)
          : 0;

        uint alphaType = layer.AlphaType;
        if ((layer.LayerFlags & MaterialLayerFlagAniAlphaFlicker) != 0 && alphaType == BlendNone)
        {
          alphaType = BlendOnlyTransparency;
        }

        AddOrMergeDrawSpan(
          spans,
          new DrawSpan(
            startVertex,
            vertexCount,
              textureId,
              secondaryTextureId,
            alphaType,
            0,
            MultiplyLayerColor(ResolveLegacyEntityLayerColor(material, layerIndex), layerColorMultiplier),
            (int)(alphaType & 0x0F) == BlendNone,
            true,
            layer,
            hasSecondaryLayer,
            secondaryLayer));
      }

      return;
    }

    int texture = _checkerTexture;
    uint fallbackAlphaType = 0;
    if (materialId >= 0 && materialId < _map.EntityMaterialSurfaceIds.Length)
    {
      int surfaceId = _map.EntityMaterialSurfaceIds[materialId];
      if (_entitySurfaceToTexture.TryGetValue(surfaceId, out int entityTexture))
      {
        texture = entityTexture;
      }
      else if (_mapSurfaceToTexture.TryGetValue(surfaceId, out int mapTexture))
      {
        texture = mapTexture;
      }

      if (materialId < _map.EntityMaterialAlphaTypes.Length)
      {
        fallbackAlphaType = _map.EntityMaterialAlphaTypes[materialId];
      }
    }

    AddOrMergeDrawSpan(
      spans,
      new DrawSpan(
        startVertex,
        vertexCount,
        texture,
        0,
        fallbackAlphaType,
        0,
        MultiplyLayerColor(Vector4.One, layerColorMultiplier),
        (int)(fallbackAlphaType & 0x0F) == BlendNone,
        false,
        default,
        false,
        default));
  }

  private static NumericsMatrix4[] BuildSceneObjectMatrices(EntitySceneObject[] objects, float nowFrame)
  {
    if (objects.Length == 0)
    {
      return Array.Empty<NumericsMatrix4>();
    }

    NumericsMatrix4[] legacyWorldMatrices = new NumericsMatrix4[objects.Length];
    byte[] visitState = new byte[objects.Length];
    for (int objectIndex = 0; objectIndex < objects.Length; ++objectIndex)
    {
      _ = ResolveSceneObjectMatrix(objectIndex, objects, nowFrame, legacyWorldMatrices, visitState);
    }

    NumericsMatrix4[] convertedMatrices = new NumericsMatrix4[objects.Length];
    for (int index = 0; index < convertedMatrices.Length; ++index)
    {
      convertedMatrices[index] = ConvertFrom3dsMaxMatrix(legacyWorldMatrices[index]);
    }

    return convertedMatrices;
  }

  private static NumericsMatrix4 ResolveSceneObjectMatrix(
    int objectIndex,
    EntitySceneObject[] objects,
    float nowFrame,
    NumericsMatrix4[] cache,
    byte[] visitState)
  {
    byte state = visitState[objectIndex];
    if (state == 2)
    {
      return cache[objectIndex];
    }

    if (state == 1)
    {
      return NumericsMatrix4.Identity;
    }

    visitState[objectIndex] = 1;
    EntitySceneObject obj = objects[objectIndex];
    NumericsMatrix4 parentMatrix = NumericsMatrix4.Identity;
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

    NumericsMatrix4 localMatrix = BuildSceneObjectLocalMatrix(obj, objectNowFrame);
    NumericsMatrix4 worldMatrix = LegacyMatrixMultiply(parentMatrix, localMatrix);
    cache[objectIndex] = worldMatrix;
    visitState[objectIndex] = 2;
    return worldMatrix;
  }

  private static NumericsMatrix4 BuildSceneObjectLocalMatrix(EntitySceneObject obj, float nowFrame)
  {
    bool hasTracks = obj.PositionTracks.Length > 0 || obj.RotationTracks.Length > 0 || obj.ScaleTracks.Length > 0;
    if (!hasTracks)
    {
      NumericsMatrix4 baseRotation = CreateLegacyQuaternionMatrix(obj.Quaternion);
      NumericsMatrix4 baseScaleMatrix = BuildLegacyScaleMatrix(obj.Scale, obj.ScaleQuaternion);
      NumericsMatrix4 baseLocal = LegacyMatrixMultiply(baseRotation, baseScaleMatrix);
      baseLocal.M41 = obj.Position.X;
      baseLocal.M42 = obj.Position.Y;
      baseLocal.M43 = obj.Position.Z;
      baseLocal.M44 = 1.0f;
      return baseLocal;
    }

    Vector4 rotationQuat = obj.Quaternion;
    Vector3 scale = obj.Scale;
    Vector4 scaleQuaternion = obj.ScaleQuaternion;
    Vector3 position = obj.Position;

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

    NumericsMatrix4 rotation = CreateLegacyQuaternionMatrix(rotationQuat);
    NumericsMatrix4 scaleMatrix = BuildLegacyScaleMatrix(scale, scaleQuaternion);
    NumericsMatrix4 local = LegacyMatrixMultiply(rotation, scaleMatrix);
    local.M41 = position.X;
    local.M42 = position.Y;
    local.M43 = position.Z;
    local.M44 = 1.0f;
    return local;
  }

  private static Vector3 SamplePositionTrack(EntityScenePositionTrack[] tracks, float nowFrame)
  {
    if (tracks.Length == 0)
    {
      return Vector3.Zero;
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

    return Vector3.Lerp(tracks[root].Position, tracks[next].Position, alpha);
  }

  private static Vector4 SampleRotationTrack(EntitySceneRotationTrack[] tracks, float nowFrame)
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
    NumericsQuaternion q0 = NumericsQuaternion.Normalize(new NumericsQuaternion(rootQ.X, rootQ.Y, rootQ.Z, rootQ.W));
    NumericsQuaternion q1 = NumericsQuaternion.Normalize(new NumericsQuaternion(nextQ.X, nextQ.Y, nextQ.Z, nextQ.W));
    NumericsQuaternion q = NumericsQuaternion.Normalize(NumericsQuaternion.Slerp(q0, q1, alpha));
    if (!float.IsFinite(q.X) || !float.IsFinite(q.Y) || !float.IsFinite(q.Z) || !float.IsFinite(q.W))
    {
      return rootQ;
    }

    return new Vector4(q.X, q.Y, q.Z, q.W);
  }

  private static void SampleScaleTrack(EntitySceneScaleTrack[] tracks, float nowFrame, out Vector3 scale, out Vector4 scaleQuaternion)
  {
    scale = Vector3.One;
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

    scale = Vector3.Lerp(tracks[root].Scale, tracks[next].Scale, alpha);

    Vector4 rootQ = tracks[root].ScaleQuaternion;
    Vector4 nextQ = tracks[next].ScaleQuaternion;
    NumericsQuaternion q0 = NumericsQuaternion.Normalize(new NumericsQuaternion(rootQ.X, rootQ.Y, rootQ.Z, rootQ.W));
    NumericsQuaternion q1 = NumericsQuaternion.Normalize(new NumericsQuaternion(nextQ.X, nextQ.Y, nextQ.Z, nextQ.W));
    NumericsQuaternion q = NumericsQuaternion.Normalize(NumericsQuaternion.Slerp(q0, q1, alpha));
    if (!float.IsFinite(q.X) || !float.IsFinite(q.Y) || !float.IsFinite(q.Z) || !float.IsFinite(q.W))
    {
      scaleQuaternion = rootQ;
      return;
    }

    scaleQuaternion = new Vector4(q.X, q.Y, q.Z, q.W);
  }

  private static void GetTrackFrameIndices(
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

  private static NumericsMatrix4 BuildLegacyScaleMatrix(Vector3 scale, Vector4 scaleQuaternion)
  {
    NumericsMatrix4 scaleMatrix = NumericsMatrix4.Identity;
    scaleMatrix.M11 = scale.X;
    scaleMatrix.M22 = scale.Y;
    scaleMatrix.M33 = scale.Z;

    NumericsMatrix4 sqMatrix = CreateLegacyQuaternionMatrix(scaleQuaternion);
    if (!NumericsMatrix4.Invert(sqMatrix, out NumericsMatrix4 invSqMatrix))
    {
      return scaleMatrix;
    }

    NumericsMatrix4 temp = LegacyMatrixMultiply(scaleMatrix, invSqMatrix);
    return LegacyMatrixMultiply(sqMatrix, temp);
  }

  private static NumericsMatrix4 CreateLegacyQuaternionMatrix(Vector4 quaternion)
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

    return new NumericsMatrix4(
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

  private static NumericsMatrix4 LegacyMatrixMultiply(in NumericsMatrix4 a, in NumericsMatrix4 b)
  {
    return NumericsMatrix4.Multiply(b, a);
  }

  private static NumericsMatrix4 ConvertFrom3dsMaxMatrix(NumericsMatrix4 matrix)
  {
    (matrix.M12, matrix.M13) = (matrix.M13, matrix.M12);
    (matrix.M22, matrix.M23) = (matrix.M23, matrix.M22);
    (matrix.M32, matrix.M33) = (matrix.M33, matrix.M32);
    (matrix.M42, matrix.M43) = (matrix.M43, matrix.M42);
    return matrix;
  }

  private static bool TryGetSceneObjectMatrix(NumericsMatrix4[] objectMatrices, ushort objectId, out NumericsMatrix4 matrix)
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

    matrix = NumericsMatrix4.Identity;
    return false;
  }

  private static Vector3 TransformEntityPosition(Vector3 position, in NumericsMatrix4 matrix)
  {
    NumericsVector3 transformed = NumericsVector3.Transform(new NumericsVector3(position.X, position.Y, position.Z), matrix);
    if (!float.IsFinite(transformed.X) || !float.IsFinite(transformed.Y) || !float.IsFinite(transformed.Z))
    {
      return position;
    }

    return new Vector3(transformed.X, transformed.Y, transformed.Z);
  }

  private static Vector3 TransformEntityInstancePosition(Vector3 local, float scale, float rotXDeg, float rotYDeg, Vector3 translation)
  {
    float x = local.X * scale;
    float y = local.Y * scale;
    float z = local.Z * scale;

    float rotY = MathHelper.DegreesToRadians(rotYDeg);
    float cosY = MathF.Cos(rotY);
    float sinY = MathF.Sin(rotY);
    float xAfterY = x * cosY + z * sinY;
    float zAfterY = -x * sinY + z * cosY;

    float rotX = MathHelper.DegreesToRadians(rotXDeg);
    float cosX = MathF.Cos(rotX);
    float sinX = MathF.Sin(rotX);
    float yAfterX = y * cosX - zAfterY * sinX;
    float zAfterX = y * sinX + zAfterY * cosX;

    return new Vector3(
      xAfterY + translation.X,
      yAfterX + translation.Y,
      zAfterX + translation.Z);
  }

  private static int[] BuildSortedAlphaSpanIndices(DrawSpan[] spans, BspRenderVertex[] vertices, Vector3 cameraPosition)
  {
    List<(int SpanIndex, float DistanceSq)> alphaSpans = new(spans.Length);
    for (int spanIndex = 0; spanIndex < spans.Length; ++spanIndex)
    {
      DrawSpan span = spans[spanIndex];
      if (((int)(span.AlphaType & 0x0F)) == BlendNone)
      {
        continue;
      }

      float distanceSq = ComputeSpanDistanceSq(span, vertices, cameraPosition);
      alphaSpans.Add((spanIndex, distanceSq));
    }

    alphaSpans.Sort(static (left, right) => right.DistanceSq.CompareTo(left.DistanceSq));
    int[] sortedIndices = new int[alphaSpans.Count];
    for (int index = 0; index < alphaSpans.Count; ++index)
    {
      sortedIndices[index] = alphaSpans[index].SpanIndex;
    }

    return sortedIndices;
  }

  private static float ComputeSpanDistanceSq(DrawSpan span, BspRenderVertex[] vertices, Vector3 cameraPosition)
  {
    if (span.VertexCount <= 0 || span.StartVertex < 0 || span.StartVertex >= vertices.Length)
    {
      return 0.0f;
    }

    int i0 = span.StartVertex;
    int i1 = span.StartVertex + (span.VertexCount / 2);
    int i2 = span.StartVertex + span.VertexCount - 1;
    if (i1 >= vertices.Length)
    {
      i1 = vertices.Length - 1;
    }

    if (i2 >= vertices.Length)
    {
      i2 = vertices.Length - 1;
    }

    Vector3 center =
      (vertices[i0].Position + vertices[i1].Position + vertices[i2].Position)
      / 3.0f;
    Vector3 delta = center - cameraPosition;
    return delta.LengthSquared;
  }

  private static int CountAlphaSpans(DrawSpan[] spans)
  {
    int count = 0;
    for (int spanIndex = 0; spanIndex < spans.Length; ++spanIndex)
    {
      if (((int)(spans[spanIndex].AlphaType & 0x0F)) != BlendNone)
      {
        ++count;
      }
    }

    return count;
  }

  private static void CollectTransparentDrawCommands(
    List<TransparentDrawCommand> output,
    int vao,
    int vertexCount,
    DrawSpan[] spans,
    BspRenderVertex[] vertices,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture,
    Vector3 cameraPosition)
  {
    if (vertexCount <= 0 || spans.Length == 0 || vertices.Length == 0)
    {
      return;
    }

    for (int spanIndex = 0; spanIndex < spans.Length; ++spanIndex)
    {
      DrawSpan span = spans[spanIndex];
      if (((int)(span.AlphaType & 0x0F)) == BlendNone)
      {
        continue;
      }

      float distanceSq = ComputeSpanDistanceSq(span, vertices, cameraPosition);
      output.Add(new TransparentDrawCommand(
        Vao: vao,
        VertexCount: vertexCount,
        Spans: spans,
        SpanIndex: spanIndex,
        DistanceSq: distanceSq,
        SurfaceToTexture: surfaceToTexture,
        FallbackSurfaceToTexture: fallbackSurfaceToTexture));
    }
  }

  private void DrawMesh(
    int vao,
    int vertexCount,
    DrawSpan[] spans,
    ref Matrix4 mvp,
    ref Matrix4 view,
    IReadOnlyDictionary<int, int>? surfaceToTexture,
    IReadOnlyDictionary<int, int>? fallbackSurfaceToTexture,
    bool useLegacyLayerPath = false,
    BspRenderVertex[]? sortVertices = null,
    MeshPassFilter passFilter = MeshPassFilter.All,
    int[]? forcedSpanOrder = null)
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
      int passStart = 0;
      int passEnd = useLegacyLayerPath ? 0 : 1;
      if (!useLegacyLayerPath)
      {
        switch (passFilter)
        {
          case MeshPassFilter.OpaqueOnly:
            passStart = 0;
            passEnd = 0;
            break;
          case MeshPassFilter.AlphaOnly:
            passStart = 1;
            passEnd = 1;
            break;
          case MeshPassFilter.All:
          default:
            passStart = 0;
            passEnd = 1;
            break;
        }
      }

      for (int pass = passStart; pass <= passEnd; ++pass)
      {
        int[]? sortedAlphaIndices = null;
        if (forcedSpanOrder == null && !useLegacyLayerPath && pass == 1 && sortVertices != null && sortVertices.Length > 0)
        {
          sortedAlphaIndices = BuildSortedAlphaSpanIndices(spans, sortVertices, _camera.Position);
        }

        int drawCount = forcedSpanOrder != null
          ? forcedSpanOrder.Length
          : (sortedAlphaIndices != null ? sortedAlphaIndices.Length : spans.Length);
        for (int orderIndex = 0; orderIndex < drawCount; ++orderIndex)
        {
          int spanIndex = forcedSpanOrder != null
            ? forcedSpanOrder[orderIndex]
            : (sortedAlphaIndices != null ? sortedAlphaIndices[orderIndex] : orderIndex);
          if ((uint)spanIndex >= (uint)spans.Length)
          {
            continue;
          }

          DrawSpan span = spans[spanIndex];
          int blendMode = (int)(span.AlphaType & 0x0F);
          bool isAlphaSpan = blendMode != BlendNone;
          if (!useLegacyLayerPath)
          {
            if (pass == 0 && isAlphaSpan)
            {
              continue;
            }

            if (pass == 1 && !isAlphaSpan)
            {
              continue;
            }
          }

          if (!_enableR3tTextures && (blendMode == BlendLightMap || blendMode == BlendInvLightMap))
          {
            continue;
          }

          int primaryTextureId;
          int secondaryTextureId = _checkerTexture;
          bool useEnvBump = false;
          Vector4 layerColor = span.LayerColor;
          bool useUvTransform = false;
          Matrix3 uvTransform = Matrix3.Identity;
          uint layerFlags = 0;
          float lavaWave = 0.0f;
          float lavaSpeed = 0.0f;
          int uvGenMode = 0;
          int metalMode = 0;
          float metalScale = 0.0f;
          LayerAnimationState secondaryLayerState = new(_checkerTexture, Vector4.One, false, Matrix3.Identity, 0, 0.0f, 0.0f, 0, 0, 0.0f);

          if (useLegacyLayerPath)
          {
            LayerAnimationState layerState = EvaluateLayerAnimation(span, animTime, surfaceToTexture, fallbackSurfaceToTexture);
            primaryTextureId = _enableR3tTextures ? layerState.TextureId : _checkerTexture;
            layerColor = layerState.LayerColor;
            useUvTransform = layerState.UseUvTransform;
            uvTransform = layerState.UvTransform;
            layerFlags = layerState.LayerFlags;
            lavaWave = layerState.LavaWave;
            lavaSpeed = layerState.LavaSpeed;
            uvGenMode = layerState.UvGenMode;
            metalMode = layerState.MetalMode;
            metalScale = layerState.MetalScale;
            GL.Enable(EnableCap.DepthTest);
            GL.DepthMask(true);
          }
          else
          {
            LayerAnimationState layerState = EvaluateLayerAnimation(span, animTime, surfaceToTexture, fallbackSurfaceToTexture);
            secondaryLayerState = span.HasSecondaryLayerDefinition
              ? EvaluateLayerAnimation(
                span.SecondaryTextureId,
                Vector4.One,
                span.HasSecondaryLayerDefinition,
                span.SecondaryLayer,
                animTime,
                surfaceToTexture,
                fallbackSurfaceToTexture)
              : secondaryLayerState;

            primaryTextureId = _enableR3tTextures ? layerState.TextureId : _checkerTexture;
            secondaryTextureId = _enableR3tTextures ? secondaryLayerState.TextureId : _checkerTexture;
            useEnvBump = _enableR3mMaterials && _enableR3tTextures && span.HasSecondaryLayerDefinition;
            layerColor = layerState.LayerColor;
            useUvTransform = layerState.UseUvTransform;
            uvTransform = layerState.UvTransform;
            layerFlags = layerState.LayerFlags;
            lavaWave = layerState.LavaWave;
            lavaSpeed = layerState.LavaSpeed;
            uvGenMode = layerState.UvGenMode;
            metalMode = layerState.MetalMode;
            metalScale = layerState.MetalScale;

            bool disableDepthTest = (span.AlphaType & MaterialAlphaFlagDisableDepthTest) != 0;
            if (disableDepthTest)
            {
              GL.Disable(EnableCap.DepthTest);
            }
            else
            {
              GL.Enable(EnableCap.DepthTest);
            }

            bool zWriteEnabled = !disableDepthTest
              && ((span.AlphaType & MaterialAlphaFlagZWrite) != 0 || (span.AlphaType & 0x0F) == BlendNone);
            GL.DepthMask(zWriteEnabled);
          }

          ApplyBlendState(span.AlphaType);
          bool alphaTestEnabled = blendMode == BlendOppa;
          if (_meshUniformAlphaTestEnabled >= 0)
          {
            GL.Uniform1(_meshUniformAlphaTestEnabled, alphaTestEnabled ? 1 : 0);
          }

          if (_meshUniformAlphaRef >= 0)
          {
            GL.Uniform1(_meshUniformAlphaRef, 8.0f / 255.0f);
          }

          GL.Uniform1(_meshUniformUvChannel, span.UvChannel);
          GL.Uniform4(_meshUniformLayerColor, layerColor);
          if (_meshUniformUseUvTransform >= 0)
          {
            GL.Uniform1(_meshUniformUseUvTransform, useUvTransform ? 1 : 0);
          }

          if (_meshUniformUvTransform >= 0)
          {
            GL.UniformMatrix3(_meshUniformUvTransform, true, ref uvTransform);
          }

          if (_meshUniformLayerFlags >= 0)
          {
            GL.Uniform1(_meshUniformLayerFlags, (int)layerFlags);
          }

          if (_meshUniformLavaWave >= 0)
          {
            GL.Uniform1(_meshUniformLavaWave, lavaWave);
          }

          if (_meshUniformLavaSpeed >= 0)
          {
            GL.Uniform1(_meshUniformLavaSpeed, lavaSpeed);
          }

          if (_meshUniformUvGenMode >= 0)
          {
            GL.Uniform1(_meshUniformUvGenMode, uvGenMode);
          }

          if (_meshUniformMetalMode >= 0)
          {
            GL.Uniform1(_meshUniformMetalMode, metalMode);
          }

          if (_meshUniformMetalScale >= 0)
          {
            GL.Uniform1(_meshUniformMetalScale, metalScale);
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
      }

      GL.ActiveTexture(TextureUnit.Texture0);
      GL.Enable(EnableCap.DepthTest);
      GL.DepthMask(true);
      GL.Disable(EnableCap.Blend);
      if (_meshUniformAlphaTestEnabled >= 0)
      {
        GL.Uniform1(_meshUniformAlphaTestEnabled, 0);
      }
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

      if (_meshUniformAlphaTestEnabled >= 0)
      {
        GL.Uniform1(_meshUniformAlphaTestEnabled, 0);
      }

      if (_meshUniformAlphaRef >= 0)
      {
        GL.Uniform1(_meshUniformAlphaRef, 8.0f / 255.0f);
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

    Vector3 sourceCamera = ConvertSourcePosition(_camera.Position);
    System.Numerics.Vector3 cameraPosition = new(sourceCamera.X, sourceCamera.Y, sourceCamera.Z);
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
    bool useEntityLayerColorRule = false,
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

        for (int layerIndex = 0; layerIndex < material.Layers.Length; ++layerIndex)
        {
          if (layerIndex > 0 && envBumpSecondLayer)
          {
            break;
          }

          MaterialLayerDefinition layer = material.Layers[layerIndex];
          int textureId = ResolveLayerTextureId(layer.SurfaceId, surfaceToTexture, fallbackSurfaceToTexture);
          bool hasSecondaryLayer = envBumpSecondLayer && layerIndex == 0;
          MaterialLayerDefinition secondaryLayer = hasSecondaryLayer ? material.Layers[1] : default;
          int secondaryTextureId = hasSecondaryLayer
            ? ResolveLayerTextureId(secondaryLayer.SurfaceId, surfaceToTexture, fallbackSurfaceToTexture)
            : 0;

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
              secondaryTextureId,
              alphaType,
              0,
              useEntityLayerColorRule
                ? ResolveLegacyEntityLayerColor(material, layerIndex)
                : ResolveLegacyLayerColor(material, layerIndex),
              !useEntityLayerColorRule || (int)(alphaType & 0x0F) == BlendNone,
              true,
              layer,
              hasSecondaryLayer,
              secondaryLayer));
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
            !useEntityLayerColorRule || (int)(alphaType & 0x0F) == BlendNone,
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

  private static (Vector3[] NormalWalls, Vector3[] FrozenWalls) BuildCollisionWalls(LoadedMap map)
  {
    List<Vector3> normalVertices = new(Math.Max(4096, map.CollisionLines.Length * 8));
    List<Vector3> frozenVertices = new(Math.Max(1024, map.CollisionLines.Length * 4));

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

      List<Vector3> target = (line.Attribute & CollisionLineAttrFreeze) != 0
        ? frozenVertices
        : normalVertices;

      AddTriangle(target, start, end, endTop);
      AddTriangle(target, start, endTop, startTop);
    }

    return (normalVertices.ToArray(), frozenVertices.ToArray());
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
      uniform int uAlphaTestEnabled;
      uniform float uAlphaRef;
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
        if (uAlphaTestEnabled != 0 && outAlpha < uAlphaRef)
        {
          discard;
        }

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
        vec3 frontRgb = clamp(uColor.rgb * 1.08, vec3(0.0), vec3(1.0));
        float frontAlpha = clamp(uColor.a * 1.10, 0.0, 1.0);
        vec3 backRgb = uColor.rgb * 0.22;
        float backAlpha = clamp(uColor.a * 0.70, 0.0, 1.0);

        if (gl_FrontFacing)
        {
          FragColor = vec4(backRgb, backAlpha);
        }
        else
        {
          FragColor = vec4(frontRgb, frontAlpha);
        }
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

  private void UpdateCollisionDrawPreviewBuffer()
  {
    if (!_glReady)
    {
      _collisionDrawPreviewVertexCount = 0;
      _collisionDrawStartSnapped = false;
      _collisionDrawEndSnapped = false;
      return;
    }

    if (!TryMakeCurrent())
    {
      return;
    }

    if (!_collisionDrawDragging)
    {
      _collisionDrawPreviewVertexCount = 0;
      _collisionDrawStartSnapped = false;
      _collisionDrawEndSnapped = false;
      _glControl.Invalidate();
      return;
    }

    Vector3 previewStartSource = _collisionDrawStartSource;
    Vector3 previewEndSource = _collisionDrawCurrentSource;
    if (_map != null &&
        MapEditOperations.TryResolveCollisionWallPreview(
          _map,
          _collisionDrawStartSource,
          _collisionDrawCurrentSource,
          _collisionDrawEmbedDepth,
          _collisionDrawSnapDistance,
          out Vector3 resolvedStartSource,
          out Vector3 resolvedEndSource,
          out _collisionDrawStartSnapped,
          out _collisionDrawEndSnapped,
          out _))
    {
      previewStartSource = resolvedStartSource;
      previewEndSource = resolvedEndSource;
    }
    else
    {
      _collisionDrawStartSnapped = false;
      _collisionDrawEndSnapped = false;
    }

    Vector3 startBottom = ConvertWorldPosition(previewStartSource);
    Vector3 endBottom = ConvertWorldPosition(previewEndSource);
    float height = Math.Clamp(_collisionDrawPreviewHeight, 1.0f, 50000.0f);
    Vector3 startTop = startBottom + new Vector3(0f, height, 0f);
    Vector3 endTop = endBottom + new Vector3(0f, height, 0f);

    Vector3[] previewLines =
    [
      startBottom, endBottom,
      startBottom, startTop,
      endBottom, endTop,
      startTop, endTop,
    ];

    _collisionDrawPreviewVertexCount = previewLines.Length;
    GL.BindVertexArray(_collisionDrawPreviewVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _collisionDrawPreviewVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, _collisionDrawPreviewVertexCount * 12, previewLines, BufferUsageHint.DynamicDraw);
    _glControl.Invalidate();
  }

  private void UpdateCollisionSelectionBuffer()
  {
    if (!_glReady)
    {
      _collisionSelectionVertexCount = 0;
      return;
    }

    if (!TryMakeCurrent())
    {
      return;
    }

    if (!TryBuildSelectedCollisionDisplayLines(out Vector3[] displayLines))
    {
      _collisionSelectionVertexCount = 0;
      _glControl.Invalidate();
      return;
    }

    _collisionSelectionVertexCount = displayLines.Length;
    GL.BindVertexArray(_collisionSelectionVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _collisionSelectionVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, _collisionSelectionVertexCount * 12, displayLines, BufferUsageHint.DynamicDraw);
    _glControl.Invalidate();
  }

  private void UpdateBspSelectionBuffer()
  {
    if (!_glReady)
    {
      _bspSelectionVertexCount = 0;
      return;
    }

    if (!TryMakeCurrent())
    {
      return;
    }

    if (!TryBuildSelectedBspDisplayLines(out Vector3[] displayLines))
    {
      _bspSelectionVertexCount = 0;
      _glControl.Invalidate();
      return;
    }

    _bspSelectionVertexCount = displayLines.Length;
    GL.BindVertexArray(_bspSelectionVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _bspSelectionVbo);
    GL.BufferData(BufferTarget.ArrayBuffer, _bspSelectionVertexCount * 12, displayLines, BufferUsageHint.DynamicDraw);
    _glControl.Invalidate();
  }

  private bool TryBuildSelectedBspDisplayLines(out Vector3[] displayLines)
  {
    displayLines = Array.Empty<Vector3>();
    if (_map == null || _map.BspRenderVertices.Length < 3)
    {
      return false;
    }

    if (!TryBuildSelectedFaceSetFromCurrentSelection(_map, out HashSet<int> selectedFaceSet))
    {
      return false;
    }

    List<Vector3> lines = new(1024);
    BspRenderVertex[] vertices = _map.BspRenderVertices;
    foreach (int faceIndex in selectedFaceSet)
    {
      int triangleCount = vertices.Length / 3;
      if (faceIndex < 0 || faceIndex >= triangleCount)
      {
        continue;
      }

      int vertexBase = faceIndex * 3;
      Vector3 v0 = ConvertWorldPosition(vertices[vertexBase].Position);
      Vector3 v1 = ConvertWorldPosition(vertices[vertexBase + 1].Position);
      Vector3 v2 = ConvertWorldPosition(vertices[vertexBase + 2].Position);
      if (!IsFinite(v0) || !IsFinite(v1) || !IsFinite(v2))
      {
        continue;
      }

      lines.Add(v0);
      lines.Add(v1);
      lines.Add(v1);
      lines.Add(v2);
      lines.Add(v2);
      lines.Add(v0);
    }

    if (lines.Count <= 0)
    {
      return false;
    }

    displayLines = lines.ToArray();
    return true;
  }

  private static HashSet<int> CollectConnectedFaceIndices(LoadedMap map, int startFaceIndex, int maxFaces, out bool exceededLimit)
  {
    exceededLimit = false;
    HashSet<int> result = new();
    BspRenderVertex[] vertices = map.BspRenderVertices;
    int triangleCount = vertices.Length / 3;
    if (startFaceIndex < 0 || startFaceIndex >= triangleCount)
    {
      return result;
    }

    Dictionary<VertexKey, List<int>> faceByVertex = new(Math.Max(1024, vertices.Length / 2));
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int baseVertex = faceIndex * 3;
      for (int k = 0; k < 3; ++k)
      {
        Vector3 p = vertices[baseVertex + k].Position;
        if (!IsFinite(p))
        {
          continue;
        }

        VertexKey key = new(
          BitConverter.SingleToInt32Bits(p.X),
          BitConverter.SingleToInt32Bits(p.Y),
          BitConverter.SingleToInt32Bits(p.Z));
        if (!faceByVertex.TryGetValue(key, out List<int>? list))
        {
          list = new List<int>(4);
          faceByVertex[key] = list;
        }

        list.Add(faceIndex);
      }
    }

    Queue<int> pending = new();
    result.Add(startFaceIndex);
    pending.Enqueue(startFaceIndex);
    while (pending.Count > 0)
    {
      int face = pending.Dequeue();
      int baseVertex = face * 3;
      for (int k = 0; k < 3; ++k)
      {
        Vector3 p = vertices[baseVertex + k].Position;
        if (!IsFinite(p))
        {
          continue;
        }

        VertexKey key = new(
          BitConverter.SingleToInt32Bits(p.X),
          BitConverter.SingleToInt32Bits(p.Y),
          BitConverter.SingleToInt32Bits(p.Z));
        if (!faceByVertex.TryGetValue(key, out List<int>? neighborFaces))
        {
          continue;
        }

        for (int i = 0; i < neighborFaces.Count; ++i)
        {
          int neighbor = neighborFaces[i];
          if (result.Add(neighbor))
          {
            if (result.Count >= maxFaces)
            {
              exceededLimit = true;
              return result;
            }

            pending.Enqueue(neighbor);
          }
        }
      }
    }

    return result;
  }

  private bool TryBuildSelectedFaceSetFromCurrentSelection(LoadedMap map, out HashSet<int> selectedFaceSet)
  {
    selectedFaceSet = new HashSet<int>();
    int triangleCount = map.BspRenderVertices.Length / 3;
    if (triangleCount <= 0)
    {
      return false;
    }

    HashSet<int> selectedObjectIds = new(_selectedBspObjectIds);
    Dictionary<int, bool> selectedFaces = new(_selectedBspFaceSelections);
    if (selectedObjectIds.Count <= 0 && selectedFaces.Count <= 0)
    {
      if (_selectedBspObjectId >= 0)
      {
        selectedObjectIds.Add(_selectedBspObjectId);
      }
      else if (_selectedBspFaceIndex >= 0)
      {
        selectedFaces[_selectedBspFaceIndex] = _selectedBspFaceOnly;
      }
    }

    if (selectedObjectIds.Count > 0 && map.BspRenderVertexObjectIds.Length == map.BspRenderVertices.Length)
    {
      ushort[] objectIds = map.BspRenderVertexObjectIds;
      for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
      {
        int baseVertex = faceIndex * 3;
        int a = objectIds[baseVertex];
        int b = objectIds[baseVertex + 1];
        int c = objectIds[baseVertex + 2];
        if (selectedObjectIds.Contains(a) || selectedObjectIds.Contains(b) || selectedObjectIds.Contains(c))
        {
          selectedFaceSet.Add(faceIndex);
        }
      }
    }

    if (selectedFaces.Count > 0)
    {
      int maxFaces = ComputeAutoClusterFaceLimit(triangleCount);
      foreach ((int selectedFaceIndex, bool faceOnly) in selectedFaces)
      {
        if (selectedFaceIndex < 0 || selectedFaceIndex >= triangleCount)
        {
          continue;
        }

        if (faceOnly)
        {
          selectedFaceSet.Add(selectedFaceIndex);
          continue;
        }

        HashSet<int> connectedFaces = CollectConnectedFaceIndices(map, selectedFaceIndex, maxFaces, out bool exceededLimit);
        if (exceededLimit)
        {
          selectedFaceSet.Add(selectedFaceIndex);
          continue;
        }

        foreach (int connectedFace in connectedFaces)
        {
          selectedFaceSet.Add(connectedFace);
        }
      }
    }

    return selectedFaceSet.Count > 0;
  }

  private void PruneBspSelectionsForCurrentMap()
  {
    if (_map == null)
    {
      _selectedBspObjectIds.Clear();
      _selectedBspFaceSelections.Clear();
      _selectedBspFaceIndex = -1;
      _selectedBspObjectId = -1;
      _selectedBspFaceOnly = false;
      return;
    }

    int faceCount = _map.BspRenderVertices.Length / 3;
    if (_selectedBspFaceIndex >= faceCount)
    {
      _selectedBspFaceIndex = -1;
    }

    List<int>? removeFaces = null;
    foreach (int faceIndex in _selectedBspFaceSelections.Keys)
    {
      if (faceIndex < 0 || faceIndex >= faceCount)
      {
        removeFaces ??= new List<int>();
        removeFaces.Add(faceIndex);
      }
    }

    if (removeFaces != null)
    {
      for (int i = 0; i < removeFaces.Count; ++i)
      {
        _selectedBspFaceSelections.Remove(removeFaces[i]);
      }
    }

    NormalizePrimarySelectionFromMulti();
  }

  private bool TryBuildSelectedCollisionDisplayLines(out Vector3[] displayLines)
  {
    displayLines = Array.Empty<Vector3>();
    if (_map == null || (uint)_selectedCollisionLineIndex >= (uint)_map.CollisionLines.Length)
    {
      return false;
    }

    if (!TryBuildCollisionLineQuadSource(_map, _selectedCollisionLineIndex, out Vector3 startBottom, out Vector3 endBottom, out Vector3 startTop, out Vector3 endTop))
    {
      return false;
    }

    Vector3[] sourceLines =
    [
      startBottom, endBottom,
      startBottom, startTop,
      endBottom, endTop,
      startTop, endTop,
    ];
    displayLines = ConvertDisplayPositions(sourceLines);
    return displayLines.Length > 0;
  }

  private static bool TryBuildCollisionLineQuadSource(
    LoadedMap map,
    int lineIndex,
    out Vector3 startBottom,
    out Vector3 endBottom,
    out Vector3 startTop,
    out Vector3 endTop)
  {
    startBottom = Vector3.Zero;
    endBottom = Vector3.Zero;
    startTop = Vector3.Zero;
    endTop = Vector3.Zero;
    if ((uint)lineIndex >= (uint)map.CollisionLines.Length)
    {
      return false;
    }

    CollisionLine line = map.CollisionLines[lineIndex];
    if (line.StartVertex >= map.CollisionVertices.Length || line.EndVertex >= map.CollisionVertices.Length)
    {
      return false;
    }

    float height = MathF.Max(0.0f, line.Height);
    if (height < 0.01f)
    {
      return false;
    }

    startBottom = map.CollisionVertices[line.StartVertex];
    endBottom = map.CollisionVertices[line.EndVertex];
    startTop = startBottom + new Vector3(0f, height, 0f);
    endTop = endBottom + new Vector3(0f, height, 0f);
    return IsFinite(startBottom) && IsFinite(endBottom) && IsFinite(startTop) && IsFinite(endTop);
  }

  private bool TryPickCollisionLine(Point mouseLocation, out int lineIndex)
  {
    lineIndex = -1;
    if (_map == null || _map.CollisionLines.Length == 0)
    {
      return false;
    }

    if (!TryBuildCameraRayInSource(mouseLocation, out Vector3 rayOrigin, out Vector3 rayDirection))
    {
      return false;
    }

    float bestT = float.PositiveInfinity;
    int bestIndex = -1;
    for (int i = 0; i < _map.CollisionLines.Length; ++i)
    {
      if (!TryBuildCollisionLineQuadSource(_map, i, out Vector3 startBottom, out Vector3 endBottom, out Vector3 startTop, out Vector3 endTop))
      {
        continue;
      }

      if (TryIntersectRayTriangle(rayOrigin, rayDirection, startBottom, endBottom, endTop, out float tA) && tA < bestT)
      {
        bestT = tA;
        bestIndex = i;
      }

      if (TryIntersectRayTriangle(rayOrigin, rayDirection, startBottom, endTop, startTop, out float tB) && tB < bestT)
      {
        bestT = tB;
        bestIndex = i;
      }
    }

    if (bestIndex < 0 || !float.IsFinite(bestT))
    {
      return false;
    }

    lineIndex = bestIndex;
    return true;
  }

  private bool TryPickBspFace(Point mouseLocation, out int faceIndex)
  {
    faceIndex = -1;
    if (_map == null)
    {
      return false;
    }

    if (!TryBuildCameraRayInSource(mouseLocation, out Vector3 rayOrigin, out Vector3 rayDirection))
    {
      return false;
    }

    BspRenderVertex[] vertices = _map.BspRenderVertices;
    if (vertices.Length < 3)
    {
      return false;
    }

    float bestT = float.PositiveInfinity;
    int bestFace = -1;
    int triangleCount = vertices.Length / 3;
    for (int i = 0; i < triangleCount; ++i)
    {
      int vertexBase = i * 3;
      Vector3 v0 = vertices[vertexBase].Position;
      Vector3 v1 = vertices[vertexBase + 1].Position;
      Vector3 v2 = vertices[vertexBase + 2].Position;
      if (!IsFinite(v0) || !IsFinite(v1) || !IsFinite(v2))
      {
        continue;
      }

      if (!TryIntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, out float t))
      {
        continue;
      }

      if (t < bestT)
      {
        bestT = t;
        bestFace = i;
      }
    }

    if (bestFace < 0 || !float.IsFinite(bestT))
    {
      return false;
    }

    faceIndex = bestFace;
    return true;
  }

  private bool TryResolveFaceObjectId(int faceIndex, out int objectId)
  {
    objectId = -1;
    if (_map == null)
    {
      return false;
    }

    int vertexBase = faceIndex * 3;
    if (vertexBase < 0 || vertexBase + 2 >= _map.BspRenderVertices.Length)
    {
      return false;
    }

    ushort[] objectIds = _map.BspRenderVertexObjectIds;
    if (objectIds.Length != _map.BspRenderVertices.Length)
    {
      return false;
    }

    ushort a = objectIds[vertexBase];
    ushort b = objectIds[vertexBase + 1];
    ushort c = objectIds[vertexBase + 2];
    if (a == b && a == c)
    {
      objectId = a;
      return true;
    }

    return false;
  }

  private bool TryBeginBspSelectionDrag(Point mouseLocation)
  {
    if (!TryGetSelectedBspPivotSource(out Vector3 pivotSource))
    {
      return false;
    }

    if (!TryGetCameraSourcePose(out _, out Vector3 sourceForward))
    {
      sourceForward = new Vector3(0f, 0f, -1f);
    }

    if (sourceForward.LengthSquared < 0.000001f)
    {
      sourceForward = new Vector3(0f, 0f, -1f);
    }
    else
    {
      sourceForward = Vector3.Normalize(sourceForward);
    }

    _bspMovePivotSource = pivotSource;
    _bspMoveStartMouse = mouseLocation;
    _bspMovePlaneOriginSource = pivotSource;
    _bspMovePlaneNormalSource = sourceForward;
    if (!TryProjectMouseToBspDragPlane(mouseLocation, out Vector3 projectedPoint))
    {
      return false;
    }

    _bspMoveStartSource = projectedPoint;
    _bspMoveCurrentSource = projectedPoint;
    return true;
  }

  private bool TryGetSelectedBspPivotSource(out Vector3 pivotSource)
  {
    pivotSource = Vector3.Zero;
    if (_map == null || _map.BspRenderVertices.Length < 3)
    {
      return false;
    }

    if (!TryBuildSelectedFaceSetFromCurrentSelection(_map, out HashSet<int> selectedFaceSet))
    {
      return false;
    }

    BspRenderVertex[] vertices = _map.BspRenderVertices;
    Vector3 sum = Vector3.Zero;
    int count = 0;
    int triangleCount = vertices.Length / 3;
    foreach (int faceIndex in selectedFaceSet)
    {
      if (faceIndex < 0 || faceIndex >= triangleCount)
      {
        continue;
      }

      int vertexBase = faceIndex * 3;
      Vector3 v0 = vertices[vertexBase].Position;
      Vector3 v1 = vertices[vertexBase + 1].Position;
      Vector3 v2 = vertices[vertexBase + 2].Position;
      if (IsFinite(v0))
      {
        sum += v0;
        ++count;
      }

      if (IsFinite(v1))
      {
        sum += v1;
        ++count;
      }

      if (IsFinite(v2))
      {
        sum += v2;
        ++count;
      }
    }

    if (count <= 0)
    {
      return false;
    }

    pivotSource = sum / count;
    return IsFinite(pivotSource);
  }

  private bool TryProjectMouseToBspDragPlane(Point mouseLocation, out Vector3 pointSource)
  {
    pointSource = Vector3.Zero;
    if (TryProjectMouseToBspDragFallback(mouseLocation, out pointSource))
    {
      return true;
    }

    if (!TryBuildCameraRayInSource(mouseLocation, out Vector3 rayOrigin, out Vector3 rayDirection))
    {
      return false;
    }

    if (TryIntersectRayWithPlane(rayOrigin, rayDirection, _bspMovePlaneOriginSource, _bspMovePlaneNormalSource, out pointSource))
    {
      return true;
    }

    return TryIntersectRayWithPlaneY(rayOrigin, rayDirection, _bspMovePlaneOriginSource.Y, out pointSource);
  }

  private bool TryProjectMouseToBspDragFallback(Point mouseLocation, out Vector3 pointSource)
  {
    pointSource = Vector3.Zero;
    if (!TryGetCameraSourcePose(out Vector3 sourceCameraPosition, out _))
    {
      return false;
    }

    int height = Math.Max(1, _glControl.ClientSize.Height);
    float distanceToPivot = (_bspMovePivotSource - sourceCameraPosition).Length;
    if (!float.IsFinite(distanceToPivot) || distanceToPivot < 1.0f)
    {
      distanceToPivot = 1.0f;
    }

    float unitsPerPixel = 2.0f * distanceToPivot * MathF.Tan(MathHelper.DegreesToRadians(65f) * 0.5f) / height;
    float dx = mouseLocation.X - _bspMoveStartMouse.X;
    float dy = mouseLocation.Y - _bspMoveStartMouse.Y;

    Vector3 displayRight = _camera.Right;
    if (displayRight.LengthSquared < 0.000001f)
    {
      displayRight = Vector3.UnitX;
    }
    else
    {
      displayRight = Vector3.Normalize(displayRight);
    }

    Vector3 displayForward = _camera.Forward;
    if (displayForward.LengthSquared < 0.000001f)
    {
      displayForward = -Vector3.UnitZ;
    }
    else
    {
      displayForward = Vector3.Normalize(displayForward);
    }

    Vector3 displayUp = Vector3.Cross(displayRight, displayForward);
    if (displayUp.LengthSquared < 0.000001f)
    {
      displayUp = Vector3.UnitY;
    }
    else
    {
      displayUp = Vector3.Normalize(displayUp);
    }

    Vector3 displayDelta = displayRight * (dx * unitsPerPixel) - displayUp * (dy * unitsPerPixel);
    Vector3 sourceDelta = ConvertSourceDirection(displayDelta);
    if (!IsFinite(sourceDelta))
    {
      return false;
    }

    pointSource = _bspMoveStartSource + sourceDelta;
    return IsFinite(pointSource);
  }

  private bool IsPickedFaceAlreadySelected(int faceIndex, int selectedObjectId)
  {
    if (selectedObjectId >= 0 && _selectedBspObjectIds.Contains(selectedObjectId))
    {
      return true;
    }

    if (_selectedBspFaceSelections.ContainsKey(faceIndex))
    {
      return true;
    }

    return _map != null
      && TryBuildSelectedFaceSetFromCurrentSelection(_map, out HashSet<int> selectedFaceSet)
      && selectedFaceSet.Contains(faceIndex);
  }

  private bool TryInitializeBspMovePreview()
  {
    ResetBspMovePreviewState(restorePositions: false);
    if (_map == null || _meshVertexCount <= 0 || _mapDrawVertices.Length != _meshVertexCount)
    {
      return false;
    }

    if (!TryBuildSelectedFaceSetFromCurrentSelection(_map, out HashSet<int> selectedFaceSet))
    {
      return false;
    }

    HashSet<int> vertexIndexSet = new();
    int triangleCount = _mapDrawVertices.Length / 3;
    foreach (int faceIndex in selectedFaceSet)
    {
      if (faceIndex < 0 || faceIndex >= triangleCount)
      {
        continue;
      }

      int baseIndex = faceIndex * 3;
      vertexIndexSet.Add(baseIndex);
      vertexIndexSet.Add(baseIndex + 1);
      vertexIndexSet.Add(baseIndex + 2);
    }

    if (vertexIndexSet.Count <= 0)
    {
      return false;
    }

    _bspMovePreviewVertexIndices = vertexIndexSet.ToArray();
    Array.Sort(_bspMovePreviewVertexIndices);
    _bspMovePreviewBaseDisplayPositions = new Vector3[_bspMovePreviewVertexIndices.Length];
    for (int i = 0; i < _bspMovePreviewVertexIndices.Length; ++i)
    {
      int vertexIndex = _bspMovePreviewVertexIndices[i];
      _bspMovePreviewBaseDisplayPositions[i] = _mapDrawVertices[vertexIndex].Position;
    }

    _bspMovePreviewDisplayDelta = Vector3.Zero;
    _bspMovePreviewApplied = false;
    return true;
  }

  private void ApplyBspMovePreviewFromCurrentDrag()
  {
    if (!_bspMoveDragging)
    {
      return;
    }

    Vector3 sourceDelta = _bspMoveCurrentSource - _bspMoveStartSource;
    ApplyBspMovePreview(sourceDelta);
  }

  private void ApplyBspMovePreview(Vector3 sourceDelta)
  {
    if (_bspMovePreviewVertexIndices.Length <= 0 || _meshVertexCount <= 0)
    {
      return;
    }

    if (!IsFinite(sourceDelta))
    {
      return;
    }

    Vector3 displayDelta = ConvertSourceDeltaToDisplay(sourceDelta);
    if ((displayDelta - _bspMovePreviewDisplayDelta).LengthSquared <= 0.0000001f && _bspMovePreviewApplied)
    {
      return;
    }

    if (!TryMakeCurrent())
    {
      return;
    }

    for (int i = 0; i < _bspMovePreviewVertexIndices.Length; ++i)
    {
      int vertexIndex = _bspMovePreviewVertexIndices[i];
      if (vertexIndex < 0 || vertexIndex >= _mapDrawVertices.Length)
      {
        continue;
      }

      Vector3 previewPosition = _bspMovePreviewBaseDisplayPositions[i] + displayDelta;
      BspRenderVertex sourceVertex = _mapDrawVertices[vertexIndex];
      _mapDrawVertices[vertexIndex] = new BspRenderVertex(previewPosition, sourceVertex.Uv, sourceVertex.LightUv, sourceVertex.Color);

      int floatOffset = vertexIndex * MeshFloatStride;
      if (floatOffset + 2 >= _mapUploadBuffer.Length)
      {
        continue;
      }

      _mapUploadBuffer[floatOffset] = previewPosition.X;
      _mapUploadBuffer[floatOffset + 1] = previewPosition.Y;
      _mapUploadBuffer[floatOffset + 2] = previewPosition.Z;
    }

    GL.BindVertexArray(_meshVao);
    GL.BindBuffer(BufferTarget.ArrayBuffer, _meshVbo);
    GL.BufferSubData(BufferTarget.ArrayBuffer, IntPtr.Zero, _meshVertexCount * MeshByteStride, _mapUploadBuffer);
    _bspMovePreviewDisplayDelta = displayDelta;
    _bspMovePreviewApplied = true;
    _glControl.Invalidate();
  }

  private void ResetBspMovePreviewState(bool restorePositions)
  {
    if (restorePositions &&
        _bspMovePreviewApplied &&
        _bspMovePreviewVertexIndices.Length == _bspMovePreviewBaseDisplayPositions.Length &&
        _meshVertexCount > 0 &&
        _mapDrawVertices.Length == _meshVertexCount)
    {
      if (TryMakeCurrent())
      {
        for (int i = 0; i < _bspMovePreviewVertexIndices.Length; ++i)
        {
          int vertexIndex = _bspMovePreviewVertexIndices[i];
          if (vertexIndex < 0 || vertexIndex >= _mapDrawVertices.Length)
          {
            continue;
          }

          Vector3 basePosition = _bspMovePreviewBaseDisplayPositions[i];
          BspRenderVertex sourceVertex = _mapDrawVertices[vertexIndex];
          _mapDrawVertices[vertexIndex] = new BspRenderVertex(basePosition, sourceVertex.Uv, sourceVertex.LightUv, sourceVertex.Color);

          int floatOffset = vertexIndex * MeshFloatStride;
          if (floatOffset + 2 >= _mapUploadBuffer.Length)
          {
            continue;
          }

          _mapUploadBuffer[floatOffset] = basePosition.X;
          _mapUploadBuffer[floatOffset + 1] = basePosition.Y;
          _mapUploadBuffer[floatOffset + 2] = basePosition.Z;
        }

        GL.BindVertexArray(_meshVao);
        GL.BindBuffer(BufferTarget.ArrayBuffer, _meshVbo);
        GL.BufferSubData(BufferTarget.ArrayBuffer, IntPtr.Zero, _meshVertexCount * MeshByteStride, _mapUploadBuffer);
      }
    }

    _bspMovePreviewVertexIndices = Array.Empty<int>();
    _bspMovePreviewBaseDisplayPositions = Array.Empty<Vector3>();
    _bspMovePreviewDisplayDelta = Vector3.Zero;
    _bspMovePreviewApplied = false;
  }

  private Vector3 ConvertSourceDeltaToDisplay(Vector3 sourceDelta)
  {
    return _flipNorthSouth
      ? new Vector3(sourceDelta.X, sourceDelta.Y, -sourceDelta.Z)
      : sourceDelta;
  }

  private bool IsMeaningfulBspObjectSelection(int objectId)
  {
    if (_map == null || objectId < 0)
    {
      return false;
    }

    ushort[] objectIds = _map.BspRenderVertexObjectIds;
    int triangleCount = _map.BspRenderVertices.Length / 3;
    if (triangleCount <= 0 || objectIds.Length != _map.BspRenderVertices.Length)
    {
      return false;
    }

    int faceCount = 0;
    for (int faceIndex = 0; faceIndex < triangleCount; ++faceIndex)
    {
      int baseVertex = faceIndex * 3;
      ushort a = objectIds[baseVertex];
      ushort b = objectIds[baseVertex + 1];
      ushort c = objectIds[baseVertex + 2];
      if (a == objectId || b == objectId || c == objectId)
      {
        ++faceCount;
      }
    }

    if (faceCount <= 0)
    {
      return false;
    }

    float ratio = (float)faceCount / triangleCount;
    return ratio < 0.90f;
  }

  private bool WouldAutoClusterBeTooLarge(int faceIndex)
  {
    if (_map == null)
    {
      return false;
    }

    int triangleCount = _map.BspRenderVertices.Length / 3;
    int maxFaces = ComputeAutoClusterFaceLimit(triangleCount);
    _ = CollectConnectedFaceIndices(_map, faceIndex, maxFaces, out bool exceededLimit);
    return exceededLimit;
  }

  private static int ComputeAutoClusterFaceLimit(int triangleCount)
  {
    int ratioLimit = triangleCount > 0
      ? (int)MathF.Ceiling(triangleCount * MaxAutoClusterFaceRatio)
      : 256;
    return Math.Max(256, Math.Min(MaxAutoClusterFaceCap, ratioLimit));
  }

  private bool TryPickSourcePoint(Point mouseLocation, bool preferFastPlane, float preferredPlaneY, out Vector3 point)
  {
    point = Vector3.Zero;
    if (_map == null)
    {
      return false;
    }

    if (!TryBuildCameraRayInSource(mouseLocation, out Vector3 rayOrigin, out Vector3 rayDirection))
    {
      return false;
    }

    if (!preferFastPlane && TryRaycastMapSurface(rayOrigin, rayDirection, out Vector3 hitPoint))
    {
      point = hitPoint;
      return true;
    }

    float planeY = float.IsFinite(preferredPlaneY) ? preferredPlaneY : rayOrigin.Y;
    if (TryIntersectRayWithPlaneY(rayOrigin, rayDirection, planeY, out hitPoint))
    {
      point = hitPoint;
      return true;
    }

    point = rayOrigin + rayDirection * 500.0f;
    return true;
  }

  private bool TryBuildCameraRayInSource(Point mouseLocation, out Vector3 origin, out Vector3 direction)
  {
    origin = Vector3.Zero;
    direction = -Vector3.UnitZ;

    int width = Math.Max(1, _glControl.ClientSize.Width);
    int height = Math.Max(1, _glControl.ClientSize.Height);
    if (width <= 0 || height <= 0)
    {
      return false;
    }

    float nx = ((mouseLocation.X + 0.5f) / width) * 2.0f - 1.0f;
    float ny = 1.0f - ((mouseLocation.Y + 0.5f) / height) * 2.0f;
    float aspect = width / Math.Max(1.0f, (float)height);
    float tanHalfFov = MathF.Tan(MathHelper.DegreesToRadians(65f) * 0.5f);

    Vector3 displayForward = _camera.Forward;
    if (displayForward.LengthSquared < 0.000001f)
    {
      displayForward = -Vector3.UnitZ;
    }
    else
    {
      displayForward = Vector3.Normalize(displayForward);
    }

    Vector3 displayRight = _camera.Right;
    if (displayRight.LengthSquared < 0.000001f)
    {
      displayRight = Vector3.UnitX;
    }
    else
    {
      displayRight = Vector3.Normalize(displayRight);
    }

    Vector3 displayUp = Vector3.Cross(displayRight, displayForward);
    if (displayUp.LengthSquared < 0.000001f)
    {
      displayUp = Vector3.UnitY;
    }
    else
    {
      displayUp = Vector3.Normalize(displayUp);
    }

    Vector3 displayDirection = displayForward
      + displayRight * (nx * aspect * tanHalfFov)
      + displayUp * (ny * tanHalfFov);
    if (displayDirection.LengthSquared < 0.000001f)
    {
      return false;
    }

    displayDirection = Vector3.Normalize(displayDirection);
    origin = ConvertSourcePosition(_camera.Position);
    direction = ConvertSourceDirection(displayDirection);
    if (direction.LengthSquared < 0.000001f)
    {
      return false;
    }

    direction = Vector3.Normalize(direction);
    return true;
  }

  private bool TryRaycastMapSurface(Vector3 rayOrigin, Vector3 rayDirection, out Vector3 point)
  {
    point = Vector3.Zero;
    if (_map == null)
    {
      return false;
    }

    BspRenderVertex[] vertices = _map.BspRenderVertices;
    if (vertices.Length < 3)
    {
      return false;
    }

    float bestT = float.PositiveInfinity;
    bool found = false;
    for (int index = 0; index + 2 < vertices.Length; index += 3)
    {
      Vector3 v0 = vertices[index].Position;
      Vector3 v1 = vertices[index + 1].Position;
      Vector3 v2 = vertices[index + 2].Position;
      if (!IsFinite(v0) || !IsFinite(v1) || !IsFinite(v2))
      {
        continue;
      }

      if (!TryIntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, out float t))
      {
        continue;
      }

      if (t < bestT)
      {
        bestT = t;
        found = true;
      }
    }

    if (!found || !float.IsFinite(bestT))
    {
      return false;
    }

    point = rayOrigin + rayDirection * bestT;
    return IsFinite(point);
  }

  private static bool TryIntersectRayTriangle(
    Vector3 rayOrigin,
    Vector3 rayDirection,
    Vector3 v0,
    Vector3 v1,
    Vector3 v2,
    out float t)
  {
    const float epsilon = 0.00001f;
    t = 0.0f;

    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 pvec = Vector3.Cross(rayDirection, edge2);
    float det = Vector3.Dot(edge1, pvec);
    if (MathF.Abs(det) < epsilon)
    {
      return false;
    }

    float invDet = 1.0f / det;
    Vector3 tvec = rayOrigin - v0;
    float u = Vector3.Dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
    {
      return false;
    }

    Vector3 qvec = Vector3.Cross(tvec, edge1);
    float v = Vector3.Dot(rayDirection, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
    {
      return false;
    }

    float hitT = Vector3.Dot(edge2, qvec) * invDet;
    if (hitT < epsilon)
    {
      return false;
    }

    t = hitT;
    return true;
  }

  private static bool TryIntersectRayWithPlane(
    Vector3 rayOrigin,
    Vector3 rayDirection,
    Vector3 planeOrigin,
    Vector3 planeNormal,
    out Vector3 point)
  {
    point = Vector3.Zero;
    if (!IsFinite(planeOrigin) || !IsFinite(planeNormal))
    {
      return false;
    }

    if (planeNormal.LengthSquared < 0.000001f)
    {
      return false;
    }

    Vector3 normal = Vector3.Normalize(planeNormal);
    float denom = Vector3.Dot(rayDirection, normal);
    if (MathF.Abs(denom) < 0.000001f)
    {
      return false;
    }

    float t = Vector3.Dot(planeOrigin - rayOrigin, normal) / denom;
    if (!float.IsFinite(t) || t < 0.0f)
    {
      return false;
    }

    point = rayOrigin + rayDirection * t;
    return IsFinite(point);
  }

  private static bool TryIntersectRayWithPlaneY(Vector3 rayOrigin, Vector3 rayDirection, float y, out Vector3 point)
  {
    point = Vector3.Zero;
    if (!float.IsFinite(y))
    {
      return false;
    }

    if (MathF.Abs(rayDirection.Y) < 0.000001f)
    {
      return false;
    }

    float t = (y - rayOrigin.Y) / rayDirection.Y;
    if (!float.IsFinite(t) || t < 0.0f)
    {
      return false;
    }

    point = rayOrigin + rayDirection * t;
    return IsFinite(point);
  }

  private void OnMouseDown(object? sender, MouseEventArgs e)
  {
    _glControl.Focus();

    if (_collisionSelectModeEnabled && e.Button == MouseButtons.Left)
    {
      if (TryPickCollisionLine(e.Location, out int selectedLineIndex))
      {
        SetSelectedCollisionLineIndex(selectedLineIndex);
      }
      else
      {
        ClearSelectedCollisionLine();
      }

      return;
    }

    if (_collisionDrawModeEnabled && e.Button == MouseButtons.Left)
    {
      if (TryPickSourcePoint(e.Location, preferFastPlane: false, preferredPlaneY: _camera.Position.Y, out Vector3 startPoint))
      {
        _collisionDrawDragging = true;
        _collisionDrawStartSource = startPoint;
        _collisionDrawCurrentSource = startPoint;
        UpdateCollisionDrawPreviewBuffer();
      }

      return;
    }

    if (_bspMoveModeEnabled && e.Button == MouseButtons.Left)
    {
      bool appendSelection = (ModifierKeys & Keys.Shift) == Keys.Shift;
      if (!TryPickBspFace(e.Location, out int faceIndex))
      {
        if (!appendSelection)
        {
          ClearSelectedBspSelection();
        }

        CancelBspMovePreview();
        _bspMoveDragging = false;
        _glControl.Capture = false;
        return;
      }

      bool forceFaceSelection = (ModifierKeys & Keys.Control) == Keys.Control;
      int selectedObjectId = -1;
      if (!forceFaceSelection &&
          TryResolveFaceObjectId(faceIndex, out int objectId) &&
          IsMeaningfulBspObjectSelection(objectId))
      {
        selectedObjectId = objectId;
      }

      if (!forceFaceSelection && selectedObjectId < 0 && WouldAutoClusterBeTooLarge(faceIndex))
      {
        forceFaceSelection = true;
      }

      bool preserveExistingSelection = !appendSelection
        && HasAnyBspSelection
        && IsPickedFaceAlreadySelected(faceIndex, selectedObjectId);

      if (appendSelection)
      {
        ToggleBspSelection(faceIndex, selectedObjectId, faceOnly: forceFaceSelection);
      }
      else if (!preserveExistingSelection)
      {
        SetSelectedBspSelection(faceIndex, selectedObjectId, faceOnly: forceFaceSelection);
      }

      _bspMoveDragging = TryBeginBspSelectionDrag(e.Location);
      if (_bspMoveDragging)
      {
        _glControl.Capture = true;
        if (!TryInitializeBspMovePreview())
        {
          _bspMoveDragging = false;
          _glControl.Capture = false;
          return;
        }

        ApplyBspMovePreviewFromCurrentDrag();
      }
      return;
    }

    if (_bspSelectModeEnabled && e.Button == MouseButtons.Left)
    {
      bool appendSelection = (ModifierKeys & Keys.Shift) == Keys.Shift;
      if (TryPickBspFace(e.Location, out int faceIndex))
      {
        bool forceFaceSelection = (ModifierKeys & Keys.Control) == Keys.Control;
        int selectedObjectId = -1;
        if (!forceFaceSelection &&
            TryResolveFaceObjectId(faceIndex, out int objectId) &&
            IsMeaningfulBspObjectSelection(objectId))
        {
          selectedObjectId = objectId;
        }

        if (!forceFaceSelection && selectedObjectId < 0 && WouldAutoClusterBeTooLarge(faceIndex))
        {
          forceFaceSelection = true;
        }

        if (appendSelection)
        {
          ToggleBspSelection(faceIndex, selectedObjectId, faceOnly: forceFaceSelection);
        }
        else
        {
          SetSelectedBspSelection(faceIndex, selectedObjectId, faceOnly: forceFaceSelection);
        }
      }
      else
      {
        if (!appendSelection)
        {
          ClearSelectedBspSelection();
        }
      }

      return;
    }

    if (e.Button == MouseButtons.Right)
    {
      _capturingLook = true;
      _lastMousePoint = e.Location;
      _glControl.Capture = true;
    }
  }

  private void OnMouseUp(object? sender, MouseEventArgs e)
  {
    if (_collisionDrawModeEnabled && e.Button == MouseButtons.Left)
    {
      if (_collisionDrawDragging)
      {
        if (TryPickSourcePoint(e.Location, preferFastPlane: false, preferredPlaneY: _collisionDrawStartSource.Y, out Vector3 endPoint))
        {
          _collisionDrawCurrentSource = endPoint;
        }

        Vector3 delta = _collisionDrawCurrentSource - _collisionDrawStartSource;
        _collisionDrawDragging = false;
        UpdateCollisionDrawPreviewBuffer();

        if (delta.LengthSquared > 0.0001f)
        {
          CollisionWallDrawRequested?.Invoke(_collisionDrawStartSource, _collisionDrawCurrentSource);
        }
      }

      return;
    }

    if (_bspMoveModeEnabled && e.Button == MouseButtons.Left)
    {
      if (_bspMoveDragging)
      {
        if (TryProjectMouseToBspDragPlane(e.Location, out Vector3 projectedPoint))
        {
          _bspMoveCurrentSource = projectedPoint;
        }

        Vector3 sourceDelta = _bspMoveCurrentSource - _bspMoveStartSource;
        _bspMoveDragging = false;
        if (sourceDelta.LengthSquared > 0.0001f && IsFinite(sourceDelta))
        {
          BspTranslateRequested?.Invoke(sourceDelta);
        }
        else
        {
          CancelBspMovePreview();
        }
      }

      _glControl.Capture = false;
      return;
    }

    if (e.Button == MouseButtons.Right)
    {
      _capturingLook = false;
      _glControl.Capture = false;
    }
  }

  private void OnMouseMove(object? sender, MouseEventArgs e)
  {
    if (_collisionDrawModeEnabled && _collisionDrawDragging)
    {
      if (TryPickSourcePoint(e.Location, preferFastPlane: true, preferredPlaneY: _collisionDrawStartSource.Y, out Vector3 currentPoint))
      {
        _collisionDrawCurrentSource = currentPoint;
        UpdateCollisionDrawPreviewBuffer();
      }
      return;
    }

    if (_bspMoveModeEnabled && _bspMoveDragging)
    {
      if (TryProjectMouseToBspDragPlane(e.Location, out Vector3 projectedPoint))
      {
        _bspMoveCurrentSource = projectedPoint;
        ApplyBspMovePreviewFromCurrentDrag();
      }

      return;
    }

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
    UpdateCameraOverlay();
  }

  private void OnKeyDown(object? sender, KeyEventArgs e)
  {
    _keysDown.Add(e.KeyCode);
    if (e.KeyCode == Keys.Escape)
    {
      _capturingLook = false;
      _glControl.Capture = false;
      if (_bspMoveDragging)
      {
        _bspMoveDragging = false;
        CancelBspMovePreview();
      }

      if (_collisionDrawDragging)
      {
        _collisionDrawDragging = false;
        UpdateCollisionDrawPreviewBuffer();
      }

      if (_selectedCollisionLineIndex >= 0)
      {
        ClearSelectedCollisionLine();
      }

      if (_selectedBspFaceIndex >= 0 || _selectedBspObjectId >= 0)
      {
        ClearSelectedBspSelection();
      }
    }
    else if (e.KeyCode == Keys.R)
    {
      _camera = _map != null ? CreateDefaultCamera(_map) : CreateDefaultCamera(_currentBounds);
      UpdateCameraOverlay();
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
        if (!TryMakeCurrent())
        {
          base.Dispose(disposing);
          return;
        }

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
        if (_parityStaticEntityVbo != 0)
        {
          GL.DeleteBuffer(_parityStaticEntityVbo);
        }
        if (_parityEntityVbo != 0)
        {
          GL.DeleteBuffer(_parityEntityVbo);
        }
        if (_lineVbo != 0)
        {
          GL.DeleteBuffer(_lineVbo);
        }
        if (_wallVbo != 0)
        {
          GL.DeleteBuffer(_wallVbo);
        }
        if (_wallFrozenVbo != 0)
        {
          GL.DeleteBuffer(_wallFrozenVbo);
        }
        if (_particleVbo != 0)
        {
          GL.DeleteBuffer(_particleVbo);
        }
        if (_collisionDrawPreviewVbo != 0)
        {
          GL.DeleteBuffer(_collisionDrawPreviewVbo);
        }
        if (_collisionSelectionVbo != 0)
        {
          GL.DeleteBuffer(_collisionSelectionVbo);
        }
        if (_bspSelectionVbo != 0)
        {
          GL.DeleteBuffer(_bspSelectionVbo);
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
        if (_parityStaticEntityVao != 0)
        {
          GL.DeleteVertexArray(_parityStaticEntityVao);
        }
        if (_parityEntityVao != 0)
        {
          GL.DeleteVertexArray(_parityEntityVao);
        }
        if (_lineVao != 0)
        {
          GL.DeleteVertexArray(_lineVao);
        }
        if (_wallVao != 0)
        {
          GL.DeleteVertexArray(_wallVao);
        }
        if (_wallFrozenVao != 0)
        {
          GL.DeleteVertexArray(_wallFrozenVao);
        }
        if (_particleVao != 0)
        {
          GL.DeleteVertexArray(_particleVao);
        }
        if (_collisionDrawPreviewVao != 0)
        {
          GL.DeleteVertexArray(_collisionDrawPreviewVao);
        }
        if (_collisionSelectionVao != 0)
        {
          GL.DeleteVertexArray(_collisionSelectionVao);
        }
        if (_bspSelectionVao != 0)
        {
          GL.DeleteVertexArray(_bspSelectionVao);
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

  private enum ParticleTrackChannel
  {
    Scale,
    Alpha,
    ColorR,
    ColorG,
    ColorB,
    PowerX,
    PowerY,
    PowerZ,
    ZRot,
    YRot,
  }

  private readonly record struct ParticleRuntimeState(
    bool IsAlive,
    Vector3 PositionOffset,
    Vector3 Color,
    float Alpha,
    float Scale,
    float ZRotRadians,
    float YRotRadians);

  private readonly record struct TransparentDrawCommand(
    int Vao,
    int VertexCount,
    DrawSpan[] Spans,
    int SpanIndex,
    float DistanceSq,
    IReadOnlyDictionary<int, int>? SurfaceToTexture,
    IReadOnlyDictionary<int, int>? FallbackSurfaceToTexture);

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

  private readonly record struct VertexKey(int XBits, int YBits, int ZBits);
}
