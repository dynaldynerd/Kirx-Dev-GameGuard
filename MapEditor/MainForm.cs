using MapEditor.Formats;
using MapEditor.Viewer;
using OpenTK.Mathematics;
using System.ComponentModel;
using System.Text.Json;

namespace MapEditor;

internal sealed partial class MainForm : Form
{
  private readonly MapViewerControl _viewer;
  private readonly List<MapListEntry> _mapEntries = [];
  private readonly List<LoadedMap> _editUndoHistory = [];
  private readonly List<LoadedMap> _editRedoHistory = [];
  private readonly Dictionary<string, EntityArchiveCacheInfo> _entityArchiveCacheByRoot = new(StringComparer.OrdinalIgnoreCase);
  private readonly Dictionary<string, SavedCameraPosition> _lastCameraByMapPath = new(StringComparer.OrdinalIgnoreCase);
  private const int MaxUndoHistory = 64;
  private SkySourceMode _skySourceMode = SkySourceMode.Sky2;
  private RenderPipelineMode _renderPipelineMode = RenderPipelineMode.R3Parity;
  private bool _suppressMapSelectionChanged;
  private string? _mapRootPath;
  private LoadedMap? _loadedMap;
  private string? _loadedBspPath;
  private string? _loadedEbpPath;
  private bool _suppressBoundaryCollisionValueChanged;
  private string _statusBaseText = "Ready. Ctrl+O open map folder | Select map in top combobox | Right mouse look | WASD move | Wheel zoom | F1 controls";

  public MainForm()
    : this(Array.Empty<string>(), isDesignerCtor: true)
  {
  }

  public MainForm(string[] args)
    : this(args, isDesignerCtor: false)
  {
  }

  private MainForm(string[] args, bool isDesignerCtor)
  {
    InitializeComponent();

    if (isDesignerCtor || IsDesignTimeHost())
    {
      _viewer = null!;
      return;
    }

    KeyDown += OnMainFormKeyDown;
    _statusLabel.Text = _statusBaseText;
    WireUiEvents();

    _viewer = new MapViewerControl
    {
      Dock = DockStyle.Fill,
    };
    _viewerHostPanel.Controls.Clear();
    _viewerHostPanel.Controls.Add(_viewer);
    _viewer.BringToFront();
    InitializeViewerBindings();
    SyncTeleportInputsFromCamera();

    _statusTimer.Tick += (_, _) => RefreshRuntimeStatus();
    _statusTimer.Start();

    SetSkySourceMode(_skySourceMode, reloadCurrentMap: false);
    SetRenderPipelineMode(_renderPipelineMode, invalidateViewer: false);
    LoadCameraPositionsFromSettings();
    FormClosing += OnMainFormClosing;
    UpdateUndoUiState();

    if (args.Length > 0)
    {
      TryLoadFromArgs(args);
    }
  }

  private void WireUiEvents()
  {
    _openMapFolderMenuItem.Click += (_, _) => OpenMapRootFromDialog();
    _saveEbpOnlyMenuItem.Click += (_, _) => SaveCurrentEbpOnlyFromDialog();
    _saveMapAsMenuItem.Click += (_, _) => SaveCurrentMapAsFromDialog();
    _exitMenuItem.Click += (_, _) => Close();
    _controlsMenuItem.Click += (_, _) => ShowControls();
    _showSpeedStripMenuItem.CheckedChanged += (_, _) => _speedStrip.Visible = _showSpeedStripMenuItem.Checked;
    _showCollisionStripMenuItem.CheckedChanged += (_, _) => _collisionStrip.Visible = _showCollisionStripMenuItem.Checked;

    _undoMenuItem.Click += (_, _) => UndoLastCollisionEdit();
    _redoMenuItem.Click += (_, _) => RedoLastCollisionEdit();
    _deleteSelectedCollisionMenuItem.Click += (_, _) => DeleteSelectedCollisionLineFromUi();

    _goToCoordinateButton.Click += (_, _) => TeleportCameraToInputCoordinate();
    _teleportXUpDown.KeyDown += OnTeleportInputKeyDown;
    _teleportYUpDown.KeyDown += OnTeleportInputKeyDown;
    _teleportZUpDown.KeyDown += OnTeleportInputKeyDown;
    _mapComboBox.SelectedIndexChanged += (_, _) => OnMapSelectionChanged();
    _skyMenuItem.Click += (_, _) => SetSkySourceMode(SkySourceMode.Sky, reloadCurrentMap: true);
    _sky2MenuItem.Click += (_, _) => SetSkySourceMode(SkySourceMode.Sky2, reloadCurrentMap: true);
    _legacyPipelineMenuItem.Click += (_, _) => SetRenderPipelineMode(RenderPipelineMode.LegacyCompat, invalidateViewer: true);
    _parityPipelineMenuItem.Click += (_, _) => SetRenderPipelineMode(RenderPipelineMode.R3Parity, invalidateViewer: true);

    _deleteSelectedCollisionButton.Click += (_, _) => DeleteSelectedCollisionLineFromUi();
    _undoCollisionButton.Click += (_, _) => UndoLastCollisionEdit();
    _redoCollisionButton.Click += (_, _) => RedoLastCollisionEdit();
    _resetEditedCollisionButton.Click += (_, _) => ReloadCurrentMapFromDisk();
  }

  private void InitializeViewerBindings()
  {
    _viewer.RenderPipelineMode = _renderPipelineMode;
    _viewer.FlipNorthSouth = _northSouthFlipButton.Checked;
    _viewer.MoveSpeed = (float)_speedUpDown.Value;
    _speedUpDown.ValueChanged += (_, _) => _viewer.MoveSpeed = (float)_speedUpDown.Value;

    _viewer.ShowSky = _skyRenderButton.Checked;
    _skyRenderButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowSky = _skyRenderButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.EnableR3tTextures = _r3tButton.Checked;
    _r3tButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableR3tTextures = _r3tButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.EnableR3mMaterials = _r3mButton.Checked;
    _r3mButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableR3mMaterials = _r3mButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.ShowR3eEntities = _r3eButton.Checked;
    _r3eButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowR3eEntities = _r3eButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.EnableR3xEnvironment = _r3xButton.Checked;
    _r3xButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableR3xEnvironment = _r3xButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.ShowCollisionOverlay = _collisionButton.Checked;
    _collisionButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowCollisionOverlay = _collisionButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.ShowGrid = _gridButton.Checked;
    _gridButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowGrid = _gridButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.EnableDynamicLighting = _dynamicLightButton.Checked;
    _dynamicLightButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableDynamicLighting = _dynamicLightButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.ShowParticleMarkers = _particleMarkersButton.Checked;
    _particleMarkersButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowParticleMarkers = _particleMarkersButton.Checked;
      _viewer.Invalidate();
    };

    _viewer.CollisionDrawModeEnabled = _mouseDrawCollisionButton.Checked;
    _mouseDrawCollisionButton.CheckedChanged += (_, _) =>
    {
      if (_mouseDrawCollisionButton.Checked && _selectCollisionButton.Checked)
      {
        _selectCollisionButton.Checked = false;
      }

      _viewer.CollisionDrawModeEnabled = _mouseDrawCollisionButton.Checked;
      _viewer.Focus();
    };

    _viewer.CollisionSelectModeEnabled = _selectCollisionButton.Checked;
    _selectCollisionButton.CheckedChanged += (_, _) =>
    {
      if (_selectCollisionButton.Checked && _mouseDrawCollisionButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
      }

      _viewer.CollisionSelectModeEnabled = _selectCollisionButton.Checked;
      _viewer.Focus();
      UpdateUndoUiState();
    };

    _viewer.CollisionDrawPreviewHeight = (float)_boundaryHeightUpDown.Value;
    _viewer.CollisionDrawEmbedDepth = (float)_boundaryEmbedDepthUpDown.Value;
    _viewer.CollisionDrawSnapDistance = (float)_boundarySnapDistanceUpDown.Value;
    _boundaryMarginUpDown.ValueChanged += (_, _) => ApplySelectedCollisionLineEditsFromBoundaryInputs();
    _boundaryHeightUpDown.ValueChanged += (_, _) => _viewer.CollisionDrawPreviewHeight = (float)_boundaryHeightUpDown.Value;
    _boundaryHeightUpDown.ValueChanged += (_, _) => ApplySelectedCollisionLineEditsFromBoundaryInputs();
    _boundaryEmbedDepthUpDown.ValueChanged += (_, _) => _viewer.CollisionDrawEmbedDepth = (float)_boundaryEmbedDepthUpDown.Value;
    _boundaryEmbedDepthUpDown.ValueChanged += (_, _) => ApplySelectedCollisionLineEditsFromBoundaryInputs();
    _boundarySnapDistanceUpDown.ValueChanged += (_, _) => _viewer.CollisionDrawSnapDistance = (float)_boundarySnapDistanceUpDown.Value;

    _viewer.CollisionWallDrawRequested += OnViewerCollisionWallDrawRequested;
    _viewer.CollisionLineSelectionChanged += OnViewerCollisionLineSelectionChanged;
    _northSouthFlipButton.CheckedChanged += (_, _) =>
    {
      _viewer.FlipNorthSouth = _northSouthFlipButton.Checked;
      _viewer.Invalidate();
      SyncTeleportInputsFromCamera();
    };
  }

  private static bool IsDesignTimeHost()
  {
    return LicenseManager.UsageMode == LicenseUsageMode.Designtime;
  }

  private void OpenMapRootFromDialog()
  {
    using FolderBrowserDialog dialog = new()
    {
      Description = "Select RF Map root folder (contains map subfolders and Entity)",
      UseDescriptionForTitle = true,
      ShowNewFolderButton = false,
    };

    string? initialPath = _mapRootPath;
    if (string.IsNullOrWhiteSpace(initialPath) && _loadedBspPath != null)
    {
      initialPath = TryGetMapRootFromBspPath(_loadedBspPath);
    }

    if (!string.IsNullOrWhiteSpace(initialPath) && Directory.Exists(initialPath))
    {
      dialog.SelectedPath = initialPath;
    }

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    LoadMapRoot(dialog.SelectedPath, preferredBspPath: null, loadSelectedMap: true);
  }

  private void SaveCurrentMapAsFromDialog()
  {
    if (_loadedMap == null || string.IsNullOrWhiteSpace(_loadedBspPath) || string.IsNullOrWhiteSpace(_loadedEbpPath))
    {
      MessageBox.Show(this, "No map is loaded.", "Save Map As", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    using SaveFileDialog dialog = new()
    {
      Title = "Save BSP (EBP will be written alongside it)",
      Filter = "RF BSP (*.bsp)|*.bsp|All files (*.*)|*.*",
      DefaultExt = "bsp",
      AddExtension = true,
      OverwritePrompt = true,
      FileName = Path.GetFileName(_loadedBspPath),
      InitialDirectory = Path.GetDirectoryName(_loadedBspPath),
    };

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    string targetBspPath = Path.GetFullPath(dialog.FileName);
    string targetEbpPath = Path.ChangeExtension(targetBspPath, ".ebp");
    if (File.Exists(targetEbpPath))
    {
      DialogResult overwriteEbp = MessageBox.Show(
        this,
        $"Target EBP already exists:{Environment.NewLine}{targetEbpPath}{Environment.NewLine}{Environment.NewLine}Overwrite it?",
        "Save Map As",
        MessageBoxButtons.YesNo,
        MessageBoxIcon.Question);
      if (overwriteEbp != DialogResult.Yes)
      {
        return;
      }
    }

    try
    {
      if (!TryValidateCollisionForSave(_loadedMap, out string validationError))
      {
        MessageBox.Show(this, validationError, "Save Map As Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        _statusBaseText = "Save blocked: invalid collision data.";
        RefreshRuntimeStatus();
        return;
      }

      MapExporter.ExportBspEbpPair(_loadedMap, _loadedBspPath, _loadedEbpPath, targetBspPath, targetEbpPath);
      _statusBaseText = $"Saved BSP/EBP (+sidecars/entity): {Path.GetFileName(targetBspPath)} / {Path.GetFileName(targetEbpPath)}";
      RefreshRuntimeStatus();
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Save Map As Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Save map failed.";
      RefreshRuntimeStatus();
    }
  }

  private void SaveCurrentEbpOnlyFromDialog()
  {
    if (_loadedMap == null || string.IsNullOrWhiteSpace(_loadedEbpPath))
    {
      MessageBox.Show(this, "No map is loaded.", "Save EBP Only", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    using SaveFileDialog dialog = new()
    {
      Title = "Save EBP Only",
      Filter = "RF EBP (*.ebp)|*.ebp|All files (*.*)|*.*",
      DefaultExt = "ebp",
      AddExtension = true,
      OverwritePrompt = true,
      FileName = Path.GetFileName(_loadedEbpPath),
      InitialDirectory = Path.GetDirectoryName(_loadedEbpPath),
    };

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    string targetEbpPath = Path.GetFullPath(dialog.FileName);
    try
    {
      if (!TryValidateCollisionForSave(_loadedMap, out string validationError))
      {
        MessageBox.Show(this, validationError, "Save EBP Only Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        _statusBaseText = "Save blocked: invalid collision data.";
        RefreshRuntimeStatus();
        return;
      }

      MapExporter.ExportEbpOnly(_loadedMap, _loadedEbpPath, targetEbpPath);
      _statusBaseText = $"Saved EBP: {Path.GetFileName(targetEbpPath)}";
      RefreshRuntimeStatus();
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Save EBP Only Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Save EBP failed.";
      RefreshRuntimeStatus();
    }
  }

  private void TryLoadFromArgs(string[] args)
  {
    if (!MapLaunchOptions.TryParse(args, out MapLaunchOptions? options, out string? error))
    {
      MessageBox.Show(this, $"{error}{Environment.NewLine}{Environment.NewLine}{MapLaunchOptions.UsageText}", "Map Arguments", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    if (options == null)
    {
      return;
    }

    LoadMap(options.BspPath, options.EbpPath);

    string? mapRootPath = TryGetMapRootFromBspPath(options.BspPath);
    if (mapRootPath != null)
    {
      LoadMapRoot(mapRootPath, options.BspPath, loadSelectedMap: false);
    }
  }

  private void LoadMap(string bspPath, string ebpPath)
  {
    SaveCurrentCameraPosition();

    try
    {
      EntityArchiveCacheInfo archiveCache = EnsureEntityArchiveCacheForBsp(bspPath);
      LoadedMap map = BspLoader.Load(bspPath, ebpPath, _skySourceMode);
      _editUndoHistory.Clear();
      _editRedoHistory.Clear();
      _loadedMap = map;
      _viewer.SetMap(map);
      _loadedBspPath = NormalizeMapPath(bspPath);
      _loadedEbpPath = Path.GetFullPath(ebpPath);
      RestoreCameraPositionForMap(_loadedBspPath);
      SyncTeleportInputsFromCamera();
      SyncSelectedMapToLoadedPath(_loadedBspPath);
      Text = $"RF MapEditor (Viewer) - {map.Name}";
      string cacheTag = archiveCache.ArchiveCount > 0
        ? $"{(archiveCache.WasCached ? "RPKCache:Hit" : "RPKCache:Build")} {archiveCache.ArchiveCount:N0}rpk/{archiveCache.AssetCount:N0}assets"
        : "RPKCache:N/A";
      _statusBaseText =
        $"Loaded {map.Name} | BSP TriVerts: {map.BspTriangleVertices.Length:N0} | EBP Vertices: {map.CollisionVertices.Length:N0} | EBP Lines: {map.CollisionLines.Length:N0}"
        + $" | RenderVerts: {map.BspRenderVertices.Length:N0}"
        + $" | TextureBlobs: {map.SurfaceTextures.Length:N0} | LgtTex: {map.LightmapTextures.Length:N0}"
        + $" | Fog: {(map.Environment.FogEnabled ? $"On({map.Environment.FogStart:F0}-{map.Environment.FogEnd:F0})" : "Off")} | ExtDummy: {map.ExtDummies.Length:N0}"
        + $" | SkySource: {GetSkySourceName(_skySourceMode)} | SkyVerts: {map.SkyRenderVertices.Length:N0} | SkyTex: {map.SkySurfaceTextures.Length:N0}"
        + $" | EntModels: {map.MapEntityModelCount:N0} | EntInst: {map.MapEntityInstanceCount:N0} | FxInst: {map.ParticleInstancePositions.Length:N0} | EntVerts: {map.EntityRenderVertices.Length:N0} | EntTex: {map.EntitySurfaceTextures.Length:N0}"
        + $" | {cacheTag}"
        + $" | Pipe: {GetRenderPipelineModeName(_renderPipelineMode)}"
        + $" | Bounds: ({map.Bounds.Min.X:F0},{map.Bounds.Min.Y:F0},{map.Bounds.Min.Z:F0})..({map.Bounds.Max.X:F0},{map.Bounds.Max.Y:F0},{map.Bounds.Max.Z:F0})";
      RefreshRuntimeStatus();
      UpdateUndoUiState();
    }
    catch (Exception ex)
    {
      _loadedMap = null;
      _editUndoHistory.Clear();
      _editRedoHistory.Clear();
      MessageBox.Show(this, ex.Message, "Load Map Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Load failed.";
      RefreshRuntimeStatus();
      UpdateUndoUiState();
    }
  }

  private void ShowControls()
  {
    const string message =
      """
      Camera Controls
      - Ctrl+O: select map root folder
      - Ctrl+S: save current EBP only (collision changes)
      - Ctrl+Shift+S: save current loaded BSP/EBP as a new pair (+sidecars/entity)
      - View menu: show/hide each toolbar row
      - Map combo (top bar): switch maps by folder name
      - Right mouse drag: look around
      - Mouse wheel: zoom in/out (forward/back)
      - W / A / S / D: move forward/left/back/right
      - Space/Ctrl/Shift movement is disabled (reserved for shortcuts)
      - Move speed: top bar numeric box
      - Cam X/Y/Z + Go: teleport camera to exact source/game XYZ coordinate
      - Sky dropdown: switch between sky and sky2 source
      - Pipe dropdown: switch renderer (`LegacyCompat` or `R3Parity`)
      - Sky button: show/hide sky render
      - R3T button: toggle texture usage (map/light/sky/entity)
      - R3M button: toggle material-layer behavior
      - R3E button: toggle map entities/particles
      - R3X button: toggle environment fog/clear color
      - Collision strip (2nd row): EBP/collision tools
      - Collision button: show/hide EBP collision walls
      - Grid button: show/hide white reference grid
      - DrawWall: left click-drag-release in viewport to append one collision wall
      - SelWall: left click wall segment in viewport to select one collision wall
      - DelSel/Delete: remove currently selected collision wall segment
      - DrawWall preview turns green when endpoint glue/snap will happen
      - Undo button or Ctrl+Z: undo latest collision edits (up to 64 steps)
      - Redo button or Ctrl+Y/Ctrl+Shift+Z: redo last undone edit
      - ResetMap: discard unsaved edits and reload map from source files
      - DynLight button: toggle dynamic lighting
      - FxMark button: toggle particle/effect map markers
      - R: reset camera to map default
      - Esc: release mouse-look capture
      - Top-left HUD: camera coordinates (Cam XYZ, source/game-space)
      - Camera position is saved per map and restored on next load
      """;

    MessageBox.Show(this, message, "Controls", MessageBoxButtons.OK, MessageBoxIcon.Information);
  }

  private void OnTeleportInputKeyDown(object? sender, KeyEventArgs e)
  {
    if (e.KeyCode != Keys.Enter)
    {
      return;
    }

    e.Handled = true;
    e.SuppressKeyPress = true;
    TeleportCameraToInputCoordinate();
  }

  private void TeleportCameraToInputCoordinate()
  {
    float x = (float)_teleportXUpDown.Value;
    float y = (float)_teleportYUpDown.Value;
    float z = (float)_teleportZUpDown.Value;
    if (!_viewer.TrySetCameraSourcePosition(x, y, z))
    {
      return;
    }

    SaveCurrentCameraPosition();
    _viewer.Focus();
  }

  private void SyncTeleportInputsFromCamera()
  {
    (float x, float y, float z) = _viewer.GetCameraSourcePosition();
    SetNumericUpDownValue(_teleportXUpDown, x);
    SetNumericUpDownValue(_teleportYUpDown, y);
    SetNumericUpDownValue(_teleportZUpDown, z);
  }

  private void OnViewerCollisionWallDrawRequested(Vector3 sourceStart, Vector3 sourceEnd)
  {
    if (_loadedMap == null)
    {
      return;
    }

    TryOrientDrawnCollisionSegmentTowardCamera(ref sourceStart, ref sourceEnd);

    float wallHeight = (float)_boundaryHeightUpDown.Value;
    float bury = (float)_boundaryEmbedDepthUpDown.Value;
    float snap = (float)_boundarySnapDistanceUpDown.Value;
    if (!MapEditOperations.TryAppendCollisionWallSegment(
      _loadedMap,
      sourceStart,
      sourceEnd,
      wallHeight,
      bury,
      snap,
      out LoadedMap editedMap,
      out string error,
      out float resolvedWallHeight,
      out float resolvedEmbedDepth,
      out bool startSnapped,
      out bool endSnapped))
    {
      if (string.Equals(error, "Collision segment is too short after snap.", StringComparison.Ordinal) ||
          string.Equals(error, "Collision segment is too short.", StringComparison.Ordinal))
      {
        _statusBaseText = $"Mouse draw skipped: {error} | Tip: drag longer or reduce B-Snap";
        RefreshRuntimeStatus();
        return;
      }

      MessageBox.Show(this, error, "Mouse Draw Collision Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    ApplyEditedMap(editedMap);
    _statusBaseText =
      $"Collision wall drawn | Height: {resolvedWallHeight:F0} | Bury: {resolvedEmbedDepth:F0} | Snap: {snap:F0}"
      + $" | Glue: {(startSnapped || endSnapped ? "Y" : "N")}"
      + $" | A: ({sourceStart.X:F1},{sourceStart.Y:F1},{sourceStart.Z:F1})"
      + $" | B: ({sourceEnd.X:F1},{sourceEnd.Y:F1},{sourceEnd.Z:F1})"
      + $" | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l";
    RefreshRuntimeStatus();
  }

  private void OnViewerCollisionLineSelectionChanged(int selectedLineIndex)
  {
    UpdateUndoUiState();
    if (_loadedMap == null)
    {
      return;
    }

    if ((uint)selectedLineIndex < (uint)_loadedMap.CollisionLines.Length)
    {
      CollisionLine selected = _loadedMap.CollisionLines[selectedLineIndex];
      if (MapEditOperations.TryGetCollisionLineEditValues(
        _loadedMap,
        selectedLineIndex,
        out float selectedMargin,
        out float selectedHeight,
        out float selectedBury,
        out _))
      {
        _suppressBoundaryCollisionValueChanged = true;
        try
        {
          SetNumericUpDownValue(_boundaryMarginUpDown, selectedMargin);
          SetNumericUpDownValue(_boundaryHeightUpDown, selectedHeight);
          SetNumericUpDownValue(_boundaryEmbedDepthUpDown, selectedBury);
        }
        finally
        {
          _suppressBoundaryCollisionValueChanged = false;
        }
      }

      _statusBaseText =
        $"Selected collision #{selectedLineIndex:N0} | V:{selected.StartVertex}->{selected.EndVertex} | Height:{selected.Height:F1} | Attr:0x{selected.Attribute:X8}";
      RefreshRuntimeStatus();
    }
  }

  private void TryOrientDrawnCollisionSegmentTowardCamera(ref Vector3 sourceStart, ref Vector3 sourceEnd)
  {
    if (!_viewer.TryGetCameraSourcePose(out Vector3 sourceCameraPosition, out _))
    {
      return;
    }

    Vector3 horizontalDirection = sourceEnd - sourceStart;
    horizontalDirection.Y = 0.0f;
    if (horizontalDirection.LengthSquared < 0.000001f)
    {
      return;
    }

    Vector3 segmentCenter = (sourceStart + sourceEnd) * 0.5f;
    Vector3 toCamera = sourceCameraPosition - segmentCenter;
    toCamera.Y = 0.0f;
    if (toCamera.LengthSquared < 0.000001f)
    {
      return;
    }

    // RF collision front uses the opposite side of the generated triangle winding.
    Vector3 frontNormal = Vector3.Cross(Vector3.UnitY, horizontalDirection);
    if (frontNormal.LengthSquared < 0.000001f)
    {
      return;
    }

    if (Vector3.Dot(frontNormal, toCamera) < 0.0f)
    {
      (sourceStart, sourceEnd) = (sourceEnd, sourceStart);
    }
  }

  private void ApplySelectedCollisionLineEditsFromBoundaryInputs()
  {
    if (_suppressBoundaryCollisionValueChanged || _loadedMap == null)
    {
      return;
    }

    int selectedLineIndex = _viewer.SelectedCollisionLineIndex;
    if ((uint)selectedLineIndex >= (uint)_loadedMap.CollisionLines.Length)
    {
      return;
    }

    float margin = (float)_boundaryMarginUpDown.Value;
    float height = (float)_boundaryHeightUpDown.Value;
    float bury = (float)_boundaryEmbedDepthUpDown.Value;
    if (!MapEditOperations.TryEditCollisionLineDimensions(
      _loadedMap,
      selectedLineIndex,
      margin,
      height,
      bury,
      out LoadedMap editedMap,
      out string error))
    {
      _statusBaseText = $"Selected collision edit skipped: {error}";
      RefreshRuntimeStatus();
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: selectedLineIndex);
    _statusBaseText =
      $"Edited collision #{selectedLineIndex:N0} | Margin:{margin:F0} | Height:{height:F0} | Bury:{bury:F0}"
      + $" | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l";
    RefreshRuntimeStatus();
  }

  private void DeleteSelectedCollisionLineFromUi()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Delete Collision Segment", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    int selectedLineIndex = _viewer.SelectedCollisionLineIndex;
    if ((uint)selectedLineIndex >= (uint)_loadedMap.CollisionLines.Length)
    {
      MessageBox.Show(this, "No collision segment is selected.", "Delete Collision Segment", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    if (!MapEditOperations.TryRemoveCollisionLine(_loadedMap, selectedLineIndex, out LoadedMap editedMap, out string error))
    {
      MessageBox.Show(this, error, "Delete Collision Segment Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    int nextSelectionIndex = Math.Min(selectedLineIndex, editedMap.CollisionLines.Length - 1);
    ApplyEditedMap(editedMap, preferredSelectionIndex: nextSelectionIndex);
    _statusBaseText =
      $"Collision segment deleted #{selectedLineIndex:N0} | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l";
    RefreshRuntimeStatus();
  }

  private void ReloadCurrentMapFromDisk()
  {
    if (string.IsNullOrWhiteSpace(_loadedBspPath) || string.IsNullOrWhiteSpace(_loadedEbpPath))
    {
      return;
    }

    LoadMap(_loadedBspPath, _loadedEbpPath);
  }

  private void ApplyEditedMap(LoadedMap editedMap, int preferredSelectionIndex = -1, bool trackUndo = true, bool clearRedo = true)
  {
    if (trackUndo && _loadedMap != null)
    {
      _editUndoHistory.Add(_loadedMap);
      if (_editUndoHistory.Count > MaxUndoHistory)
      {
        _editUndoHistory.RemoveAt(0);
      }
    }
    if (clearRedo)
    {
      _editRedoHistory.Clear();
    }

    int selectionIndex = preferredSelectionIndex >= 0 ? preferredSelectionIndex : _viewer.SelectedCollisionLineIndex;
    _loadedMap = editedMap;
    _viewer.ApplyCollisionEditedMap(editedMap);
    _viewer.SetSelectedCollisionLineIndex(selectionIndex);
    SyncTeleportInputsFromCamera();
    UpdateUndoUiState();
  }

  private void UndoLastCollisionEdit()
  {
    if (_loadedMap == null || _editUndoHistory.Count == 0)
    {
      return;
    }

    int lastIndex = _editUndoHistory.Count - 1;
    LoadedMap previousMap = _editUndoHistory[lastIndex];
    _editUndoHistory.RemoveAt(lastIndex);
    _editRedoHistory.Add(_loadedMap);
    if (_editRedoHistory.Count > MaxUndoHistory)
    {
      _editRedoHistory.RemoveAt(0);
    }

    ApplyEditedMap(previousMap, trackUndo: false, clearRedo: false);
    _statusBaseText =
      $"Undo collision edit | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l"
      + $" | Undo left: {_editUndoHistory.Count:N0} | Redo: {_editRedoHistory.Count:N0}";
    RefreshRuntimeStatus();
  }

  private void RedoLastCollisionEdit()
  {
    if (_loadedMap == null || _editRedoHistory.Count == 0)
    {
      return;
    }

    int lastIndex = _editRedoHistory.Count - 1;
    LoadedMap nextMap = _editRedoHistory[lastIndex];
    _editRedoHistory.RemoveAt(lastIndex);
    _editUndoHistory.Add(_loadedMap);
    if (_editUndoHistory.Count > MaxUndoHistory)
    {
      _editUndoHistory.RemoveAt(0);
    }

    ApplyEditedMap(nextMap, trackUndo: false, clearRedo: false);
    _statusBaseText =
      $"Redo collision edit | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l"
      + $" | Undo: {_editUndoHistory.Count:N0} | Redo left: {_editRedoHistory.Count:N0}";
    RefreshRuntimeStatus();
  }

  private void UpdateUndoUiState()
  {
    bool hasMap = _loadedMap != null;
    bool canUndo = hasMap && _editUndoHistory.Count > 0;
    bool canRedo = hasMap && _editRedoHistory.Count > 0;
    bool canDeleteSelected = hasMap && _viewer.SelectedCollisionLineIndex >= 0;
    _undoCollisionButton.Enabled = canUndo;
    _undoMenuItem.Enabled = canUndo;
    _redoCollisionButton.Enabled = canRedo;
    _redoMenuItem.Enabled = canRedo;
    _deleteSelectedCollisionButton.Enabled = canDeleteSelected;
    _deleteSelectedCollisionMenuItem.Enabled = canDeleteSelected;
  }

  private void OnMainFormKeyDown(object? sender, KeyEventArgs e)
  {
    if (!e.Control && !e.Alt && e.KeyCode == Keys.Delete)
    {
      e.Handled = true;
      e.SuppressKeyPress = true;
      DeleteSelectedCollisionLineFromUi();
      return;
    }

    if (e.Control && e.KeyCode == Keys.Z)
    {
      e.Handled = true;
      e.SuppressKeyPress = true;
      if (e.Shift)
      {
        RedoLastCollisionEdit();
      }
      else
      {
        UndoLastCollisionEdit();
      }
      return;
    }

    if (e.Control && e.KeyCode == Keys.Y)
    {
      e.Handled = true;
      e.SuppressKeyPress = true;
      RedoLastCollisionEdit();
    }
  }

  private static void SetNumericUpDownValue(NumericUpDown input, float value)
  {
    if (!float.IsFinite(value))
    {
      return;
    }

    decimal bounded = (decimal)value;
    if (bounded < input.Minimum)
    {
      bounded = input.Minimum;
    }
    else if (bounded > input.Maximum)
    {
      bounded = input.Maximum;
    }

    input.Value = bounded;
  }

  private static bool TryValidateCollisionForSave(LoadedMap map, out string error)
  {
    error = string.Empty;
    if (map.CollisionVertices.Length > ushort.MaxValue)
    {
      error = $"Collision vertex count exceeds {ushort.MaxValue}.";
      return false;
    }

    if (map.CollisionLines.Length > ushort.MaxValue)
    {
      error = $"Collision line count exceeds {ushort.MaxValue}.";
      return false;
    }

    for (int vertexIndex = 0; vertexIndex < map.CollisionVertices.Length; ++vertexIndex)
    {
      Vector3 vertex = map.CollisionVertices[vertexIndex];
      if (!float.IsFinite(vertex.X) || !float.IsFinite(vertex.Y) || !float.IsFinite(vertex.Z))
      {
        error = $"Collision vertex {vertexIndex} has invalid coordinate.";
        return false;
      }
    }

    for (int lineIndex = 0; lineIndex < map.CollisionLines.Length; ++lineIndex)
    {
      CollisionLine line = map.CollisionLines[lineIndex];
      if (line.StartVertex >= map.CollisionVertices.Length || line.EndVertex >= map.CollisionVertices.Length)
      {
        error = $"Collision line {lineIndex} references out-of-range vertex ({line.StartVertex}->{line.EndVertex}).";
        return false;
      }

      bool isSentinelLine =
        lineIndex == 0 &&
        line.Attribute == 0 &&
        line.StartVertex == 0 &&
        line.EndVertex == 0 &&
        line.FrontLeaf == 0 &&
        line.BackLeaf == 0 &&
        line.Height == 0.0f;

      if (!isSentinelLine && (!float.IsFinite(line.Height) || line.Height <= 0.0f))
      {
        error = $"Collision line {lineIndex} has invalid height ({line.Height}).";
        return false;
      }

      Vector3 start = map.CollisionVertices[line.StartVertex];
      Vector3 end = map.CollisionVertices[line.EndVertex];
      float lengthSq = (end - start).LengthSquared;
      if (!isSentinelLine && (!float.IsFinite(lengthSq) || lengthSq < 0.0001f))
      {
        error = $"Collision line {lineIndex} is too short (degenerate).";
        return false;
      }
    }

    return true;
  }

  private void SetSkySourceMode(SkySourceMode skySourceMode, bool reloadCurrentMap)
  {
    _skySourceMode = skySourceMode;
    _skyMenuItem.Checked = skySourceMode == SkySourceMode.Sky;
    _sky2MenuItem.Checked = skySourceMode == SkySourceMode.Sky2;
    _skySourceButton.Text = $"Sky: {GetSkySourceName(skySourceMode)}";

    if (reloadCurrentMap && _loadedBspPath != null && _loadedEbpPath != null)
    {
      LoadMap(_loadedBspPath, _loadedEbpPath);
    }
  }

  private void SetRenderPipelineMode(RenderPipelineMode renderPipelineMode, bool invalidateViewer)
  {
    _renderPipelineMode = renderPipelineMode;
    _legacyPipelineMenuItem.Checked = renderPipelineMode == RenderPipelineMode.LegacyCompat;
    _parityPipelineMenuItem.Checked = renderPipelineMode == RenderPipelineMode.R3Parity;
    _pipelineButton.Text = $"Pipe: {GetRenderPipelineModeName(renderPipelineMode)}";
    _viewer.RenderPipelineMode = renderPipelineMode;

    if (invalidateViewer)
    {
      _viewer.Invalidate();
    }

    if (_loadedBspPath == null)
    {
      _statusBaseText = $"Renderer: {GetRenderPipelineModeName(renderPipelineMode)}";
      RefreshRuntimeStatus();
      return;
    }

    string mapName = Path.GetFileNameWithoutExtension(_loadedBspPath);
    _statusBaseText = $"Renderer: {GetRenderPipelineModeName(renderPipelineMode)} | Map: {mapName}";
    RefreshRuntimeStatus();
  }

  private static string GetSkySourceName(SkySourceMode skySourceMode)
  {
    return skySourceMode == SkySourceMode.Sky2 ? "sky2" : "sky";
  }

  private static string GetRenderPipelineModeName(RenderPipelineMode renderPipelineMode)
  {
    return renderPipelineMode switch
    {
      RenderPipelineMode.R3Parity => "R3Parity",
      _ => "LegacyCompat",
    };
  }

  private void RefreshRuntimeStatus()
  {
    if (_renderPipelineMode != RenderPipelineMode.R3Parity)
    {
      _statusLabel.Text = _statusBaseText;
      return;
    }

    _statusLabel.Text =
      _statusBaseText
      + $" | ParityDC: {_viewer.ParityEntityDrawCalls:N0}"
      + $" | ParityAnimObj: {_viewer.ParityAnimatedObjectUpdates:N0}"
      + $" | ParitySkipLayer: {_viewer.ParitySkippedLayers:N0}"
      + $" | ParityCull: {_viewer.ParityCulledDynamicInstances:N0}";
  }

  private void OnMainFormClosing(object? sender, FormClosingEventArgs e)
  {
    SaveCurrentCameraPosition();
    SaveCameraPositionsToSettings();
  }

  private void LoadCameraPositionsFromSettings()
  {
    _lastCameraByMapPath.Clear();

    string json = Properties.Settings.Default.LastMapCameraByPathJson;
    if (string.IsNullOrWhiteSpace(json))
    {
      return;
    }

    Dictionary<string, SavedCameraPosition>? parsed;
    try
    {
      parsed = JsonSerializer.Deserialize<Dictionary<string, SavedCameraPosition>>(json);
    }
    catch
    {
      return;
    }

    if (parsed == null)
    {
      return;
    }

    foreach ((string mapPath, SavedCameraPosition position) in parsed)
    {
      string normalizedMapPath;
      try
      {
        normalizedMapPath = NormalizeMapPath(mapPath);
      }
      catch
      {
        continue;
      }

      if (!IsFinite(position))
      {
        continue;
      }

      _lastCameraByMapPath[normalizedMapPath] = position;
    }
  }

  private void SaveCurrentCameraPosition()
  {
    if (string.IsNullOrWhiteSpace(_loadedBspPath))
    {
      return;
    }

    (float x, float y, float z) = _viewer.GetCameraSourcePosition();
    SavedCameraPosition position = new() { X = x, Y = y, Z = z };
    if (!IsFinite(position))
    {
      return;
    }

    string mapPath = NormalizeMapPath(_loadedBspPath);
    _lastCameraByMapPath[mapPath] = position;
  }

  private void RestoreCameraPositionForMap(string bspPath)
  {
    string mapPath = NormalizeMapPath(bspPath);
    if (!_lastCameraByMapPath.TryGetValue(mapPath, out SavedCameraPosition? position))
    {
      return;
    }

    if (!IsFinite(position))
    {
      return;
    }

    _viewer.TrySetCameraSourcePosition(position.X, position.Y, position.Z);
  }

  private void SaveCameraPositionsToSettings()
  {
    try
    {
      Properties.Settings.Default.LastMapCameraByPathJson = JsonSerializer.Serialize(_lastCameraByMapPath);
      Properties.Settings.Default.Save();
    }
    catch
    {
      // Keep map viewer usable even when user settings cannot be persisted.
    }
  }

  private static string NormalizeMapPath(string path)
  {
    return Path.GetFullPath(path);
  }

  private static bool IsFinite(SavedCameraPosition position)
  {
    return float.IsFinite(position.X) && float.IsFinite(position.Y) && float.IsFinite(position.Z);
  }

  private EntityArchiveCacheInfo EnsureEntityArchiveCacheForBsp(string bspPath)
  {
    string? knownEntityRoot = TryGetEntityRootFromBspPath(bspPath);
    if (!string.IsNullOrWhiteSpace(knownEntityRoot) &&
        _entityArchiveCacheByRoot.TryGetValue(knownEntityRoot, out EntityArchiveCacheInfo cachedInfo))
    {
      return new EntityArchiveCacheInfo(cachedInfo.EntityRootPath, cachedInfo.ArchiveCount, cachedInfo.AssetCount, true);
    }

    EntityArchiveCacheInfo prewarmInfo = BspLoader.PrewarmEntityArchives(bspPath);
    if (!string.IsNullOrWhiteSpace(prewarmInfo.EntityRootPath))
    {
      string normalizedRoot = NormalizeDirectoryPath(prewarmInfo.EntityRootPath);
      _entityArchiveCacheByRoot[normalizedRoot] = new EntityArchiveCacheInfo(normalizedRoot, prewarmInfo.ArchiveCount, prewarmInfo.AssetCount, true);
      return new EntityArchiveCacheInfo(normalizedRoot, prewarmInfo.ArchiveCount, prewarmInfo.AssetCount, prewarmInfo.WasCached);
    }

    return prewarmInfo;
  }

  private void LoadMapRoot(string mapRootPath, string? preferredBspPath, bool loadSelectedMap)
  {
    string fullRoot = Path.GetFullPath(mapRootPath);
    if (!Directory.Exists(fullRoot))
    {
      MessageBox.Show(this, $"Map root folder not found:{Environment.NewLine}{fullRoot}", "Open Map Folder", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    List<MapListEntry> discoveredMaps = DiscoverMaps(fullRoot);
    if (discoveredMaps.Count == 0)
    {
      MessageBox.Show(this, $"No map folders with BSP+EBP were found under:{Environment.NewLine}{fullRoot}", "Open Map Folder", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    _mapRootPath = fullRoot;
    _mapEntries.Clear();
    _mapEntries.AddRange(discoveredMaps);
    _ = EnsureEntityArchiveCacheForBsp(discoveredMaps[0].BspPath);

    int selectedIndex = GetPreferredMapIndex(discoveredMaps, preferredBspPath);
    if (selectedIndex < 0)
    {
      selectedIndex = 0;
    }

    _suppressMapSelectionChanged = true;
    try
    {
      _mapComboBox.Items.Clear();
      for (int i = 0; i < _mapEntries.Count; ++i)
      {
        _mapComboBox.Items.Add(_mapEntries[i].Name);
      }

      _mapComboBox.Enabled = _mapEntries.Count > 0;
      _mapComboBox.SelectedIndex = selectedIndex;
    }
    finally
    {
      _suppressMapSelectionChanged = false;
    }

    if (loadSelectedMap)
    {
      OnMapSelectionChanged();
    }
  }

  private void OnMapSelectionChanged()
  {
    if (_suppressMapSelectionChanged)
    {
      return;
    }

    int selectedIndex = _mapComboBox.SelectedIndex;
    if (selectedIndex < 0 || selectedIndex >= _mapEntries.Count)
    {
      return;
    }

    MapListEntry entry = _mapEntries[selectedIndex];
    LoadMap(entry.BspPath, entry.EbpPath);
  }

  private void SyncSelectedMapToLoadedPath(string bspPath)
  {
    int selectedIndex = _mapComboBox.SelectedIndex;
    if (selectedIndex >= 0 &&
        selectedIndex < _mapEntries.Count &&
        string.Equals(_mapEntries[selectedIndex].BspPath, bspPath, StringComparison.OrdinalIgnoreCase))
    {
      return;
    }

    int matchIndex = GetPreferredMapIndex(_mapEntries, bspPath);
    if (matchIndex < 0)
    {
      return;
    }

    _suppressMapSelectionChanged = true;
    try
    {
      _mapComboBox.SelectedIndex = matchIndex;
    }
    finally
    {
      _suppressMapSelectionChanged = false;
    }
  }

  private static int GetPreferredMapIndex(IReadOnlyList<MapListEntry> entries, string? preferredBspPath)
  {
    if (entries.Count == 0)
    {
      return -1;
    }

    if (!string.IsNullOrWhiteSpace(preferredBspPath))
    {
      string preferred = Path.GetFullPath(preferredBspPath);
      for (int i = 0; i < entries.Count; ++i)
      {
        if (string.Equals(entries[i].BspPath, preferred, StringComparison.OrdinalIgnoreCase))
        {
          return i;
        }
      }
    }

    return -1;
  }

  private static string? TryGetMapRootFromBspPath(string bspPath)
  {
    string fullBspPath = Path.GetFullPath(bspPath);
    string? mapFolderPath = Path.GetDirectoryName(fullBspPath);
    if (string.IsNullOrWhiteSpace(mapFolderPath))
    {
      return null;
    }

    DirectoryInfo? parent = Directory.GetParent(mapFolderPath);
    if (parent == null)
    {
      return null;
    }

    if (parent.Name.Equals("Map", StringComparison.OrdinalIgnoreCase))
    {
      return parent.FullName;
    }

    return null;
  }

  private static string? TryGetEntityRootFromBspPath(string bspPath)
  {
    string? mapRoot = TryGetMapRootFromBspPath(bspPath);
    if (string.IsNullOrWhiteSpace(mapRoot))
    {
      return null;
    }

    string entityRoot = Path.Combine(mapRoot, "Entity");
    if (!Directory.Exists(entityRoot))
    {
      return null;
    }

    return NormalizeDirectoryPath(entityRoot);
  }

  private static string NormalizeDirectoryPath(string path)
  {
    return Path.GetFullPath(path).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
  }

  private static List<MapListEntry> DiscoverMaps(string mapRootPath)
  {
    List<MapListEntry> maps = [];
    HashSet<string> seenBspPaths = new(StringComparer.OrdinalIgnoreCase);

    Stack<string> pendingDirectories = new();
    pendingDirectories.Push(Path.GetFullPath(mapRootPath));

    while (pendingDirectories.Count > 0)
    {
      string currentPath = pendingDirectories.Pop();
      string[] childDirectories;
      try
      {
        childDirectories = Directory.GetDirectories(currentPath);
      }
      catch
      {
        continue;
      }

      for (int i = 0; i < childDirectories.Length; ++i)
      {
        string childPath = childDirectories[i];
        string folderName = Path.GetFileName(childPath);
        if (folderName.Equals("Entity", StringComparison.OrdinalIgnoreCase))
        {
          continue;
        }

        if (TryFindMapFilesInFolder(childPath, folderName, out string? bspPath, out string? ebpPath))
        {
          string normalizedBspPath = Path.GetFullPath(bspPath!);
          if (seenBspPaths.Add(normalizedBspPath))
          {
            maps.Add(new MapListEntry(folderName, childPath, normalizedBspPath, Path.GetFullPath(ebpPath!)));
          }
        }

        pendingDirectories.Push(childPath);
      }
    }

    maps.Sort(static (left, right) =>
    {
      int nameCompare = string.Compare(left.Name, right.Name, StringComparison.OrdinalIgnoreCase);
      if (nameCompare != 0)
      {
        return nameCompare;
      }

      return string.Compare(left.FolderPath, right.FolderPath, StringComparison.OrdinalIgnoreCase);
    });

    return maps;
  }

  private static bool TryFindMapFilesInFolder(string folderPath, string folderName, out string? bspPath, out string? ebpPath)
  {
    bspPath = null;
    ebpPath = null;

    string[] bspCandidates;
    try
    {
      bspCandidates = Directory.GetFiles(folderPath, "*.bsp", SearchOption.TopDirectoryOnly);
    }
    catch
    {
      return false;
    }

    if (bspCandidates.Length == 0)
    {
      return false;
    }

    for (int i = 0; i < bspCandidates.Length; ++i)
    {
      string candidate = bspCandidates[i];
      string mapName = Path.GetFileNameWithoutExtension(candidate);
      if (mapName.Equals(folderName, StringComparison.OrdinalIgnoreCase))
      {
        bspPath = candidate;
        break;
      }
    }

    bspPath ??= bspCandidates[0];
    string candidateEbp = Path.ChangeExtension(bspPath, ".ebp");
    if (!File.Exists(candidateEbp))
    {
      return false;
    }

    ebpPath = candidateEbp;
    return true;
  }

  private sealed class SavedCameraPosition
  {
    public float X { get; init; }
    public float Y { get; init; }
    public float Z { get; init; }
  }

  private readonly record struct MapListEntry(string Name, string FolderPath, string BspPath, string EbpPath);
}
