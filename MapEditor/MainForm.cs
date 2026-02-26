using MapEditor.Formats;
using MapEditor.Viewer;
using System.Text.Json;

namespace MapEditor;

internal sealed class MainForm : Form
{
  private readonly MapViewerControl _viewer;
  private readonly ToolStripStatusLabel _statusLabel;
  private readonly ToolStripComboBox _mapComboBox;
  private readonly ToolStripDropDownButton _skySourceButton;
  private readonly ToolStripMenuItem _skyMenuItem;
  private readonly ToolStripMenuItem _sky2MenuItem;
  private readonly ToolStripDropDownButton _pipelineButton;
  private readonly ToolStripMenuItem _legacyPipelineMenuItem;
  private readonly ToolStripMenuItem _parityPipelineMenuItem;
  private readonly NumericUpDown _teleportXUpDown;
  private readonly NumericUpDown _teleportYUpDown;
  private readonly NumericUpDown _teleportZUpDown;
  private readonly System.Windows.Forms.Timer _statusTimer;
  private readonly List<MapListEntry> _mapEntries = [];
  private readonly Dictionary<string, EntityArchiveCacheInfo> _entityArchiveCacheByRoot = new(StringComparer.OrdinalIgnoreCase);
  private readonly Dictionary<string, SavedCameraPosition> _lastCameraByMapPath = new(StringComparer.OrdinalIgnoreCase);
  private SkySourceMode _skySourceMode = SkySourceMode.Sky2;
  private RenderPipelineMode _renderPipelineMode = RenderPipelineMode.R3Parity;
  private bool _suppressMapSelectionChanged;
  private string? _mapRootPath;
  private string? _loadedBspPath;
  private string? _loadedEbpPath;
  private string _statusBaseText = "Ready. Ctrl+O open map folder | Select map in top combobox | Right mouse look | WASD move | Wheel zoom | F1 controls";

  public MainForm(string[] args)
  {
    Text = "RF MapEditor (Viewer)";
    Width = 1600;
    Height = 900;
    StartPosition = FormStartPosition.CenterScreen;

    MenuStrip menu = new();
    ToolStripMenuItem fileMenu = new("&File");
    ToolStripMenuItem openMenu = new("&Open Map Folder...");
    openMenu.ShortcutKeys = Keys.Control | Keys.O;
    openMenu.Click += (_, _) => OpenMapRootFromDialog();
    ToolStripMenuItem exitMenu = new("E&xit");
    exitMenu.ShortcutKeys = Keys.Alt | Keys.F4;
    exitMenu.Click += (_, _) => Close();
    fileMenu.DropDownItems.Add(openMenu);
    fileMenu.DropDownItems.Add(new ToolStripSeparator());
    fileMenu.DropDownItems.Add(exitMenu);

    ToolStripMenuItem helpMenu = new("&Help");
    ToolStripMenuItem controlsMenu = new("&Controls");
    controlsMenu.ShortcutKeys = Keys.F1;
    controlsMenu.Click += (_, _) => ShowControls();
    helpMenu.DropDownItems.Add(controlsMenu);

    menu.Items.Add(fileMenu);
    menu.Items.Add(helpMenu);
    MainMenuStrip = menu;

    ToolStrip speedStrip = new()
    {
      GripStyle = ToolStripGripStyle.Hidden,
      Dock = DockStyle.Top,
      RenderMode = ToolStripRenderMode.System,
    };
    speedStrip.Items.Add(new ToolStripLabel("Move Speed"));
    NumericUpDown speedUpDown = new()
    {
      Minimum = 10,
      Maximum = 50000,
      Increment = 10,
      DecimalPlaces = 0,
      Value = 350,
      Width = 90,
    };
    ToolStripControlHost speedHost = new(speedUpDown)
    {
      AutoSize = false,
      Width = 90,
    };
    speedStrip.Items.Add(speedHost);
    speedStrip.Items.Add(new ToolStripLabel("units/s"));
    speedStrip.Items.Add(new ToolStripSeparator());
    speedStrip.Items.Add(new ToolStripLabel("Cam X"));
    _teleportXUpDown = new NumericUpDown
    {
      Minimum = -500000,
      Maximum = 500000,
      Increment = 10,
      DecimalPlaces = 2,
      Value = 0,
      Width = 88,
    };
    ToolStripControlHost teleportXHost = new(_teleportXUpDown)
    {
      AutoSize = false,
      Width = 88,
    };
    speedStrip.Items.Add(teleportXHost);
    speedStrip.Items.Add(new ToolStripLabel("Y"));
    _teleportYUpDown = new NumericUpDown
    {
      Minimum = -500000,
      Maximum = 500000,
      Increment = 10,
      DecimalPlaces = 2,
      Value = 0,
      Width = 88,
    };
    ToolStripControlHost teleportYHost = new(_teleportYUpDown)
    {
      AutoSize = false,
      Width = 88,
    };
    speedStrip.Items.Add(teleportYHost);
    speedStrip.Items.Add(new ToolStripLabel("Z"));
    _teleportZUpDown = new NumericUpDown
    {
      Minimum = -500000,
      Maximum = 500000,
      Increment = 10,
      DecimalPlaces = 2,
      Value = 0,
      Width = 88,
    };
    ToolStripControlHost teleportZHost = new(_teleportZUpDown)
    {
      AutoSize = false,
      Width = 88,
    };
    speedStrip.Items.Add(teleportZHost);
    ToolStripButton goToCoordinateButton = new("Go")
    {
      ToolTipText = "Teleport camera to Cam XYZ values (display-space coordinates)",
    };
    goToCoordinateButton.Click += (_, _) => TeleportCameraToInputCoordinate();
    speedStrip.Items.Add(goToCoordinateButton);
    _teleportXUpDown.KeyDown += OnTeleportInputKeyDown;
    _teleportYUpDown.KeyDown += OnTeleportInputKeyDown;
    _teleportZUpDown.KeyDown += OnTeleportInputKeyDown;
    speedStrip.Items.Add(new ToolStripSeparator());
    speedStrip.Items.Add(new ToolStripLabel("Map"));
    _mapComboBox = new ToolStripComboBox
    {
      AutoSize = false,
      Width = 220,
      DropDownStyle = ComboBoxStyle.DropDownList,
      ToolTipText = "Select a map folder from the loaded map root",
      Enabled = false,
    };
    _mapComboBox.SelectedIndexChanged += (_, _) => OnMapSelectionChanged();
    speedStrip.Items.Add(_mapComboBox);
    speedStrip.Items.Add(new ToolStripSeparator());
    _skySourceButton = new ToolStripDropDownButton("Sky: sky2")
    {
      ToolTipText = "Switch loaded sky source between sky and sky2",
    };
    _skyMenuItem = new ToolStripMenuItem("Use sky");
    _sky2MenuItem = new ToolStripMenuItem("Use sky2") { Checked = true };
    _skyMenuItem.Click += (_, _) => SetSkySourceMode(SkySourceMode.Sky, reloadCurrentMap: true);
    _sky2MenuItem.Click += (_, _) => SetSkySourceMode(SkySourceMode.Sky2, reloadCurrentMap: true);
    _skySourceButton.DropDownItems.Add(_skyMenuItem);
    _skySourceButton.DropDownItems.Add(_sky2MenuItem);
    speedStrip.Items.Add(_skySourceButton);
    _pipelineButton = new ToolStripDropDownButton("Pipe: R3Parity")
    {
      ToolTipText = "Switch renderer pipeline between LegacyCompat and R3Parity",
    };
    _legacyPipelineMenuItem = new ToolStripMenuItem("LegacyCompat");
    _parityPipelineMenuItem = new ToolStripMenuItem("R3Parity") { Checked = true };
    _legacyPipelineMenuItem.Click += (_, _) => SetRenderPipelineMode(RenderPipelineMode.LegacyCompat, invalidateViewer: true);
    _parityPipelineMenuItem.Click += (_, _) => SetRenderPipelineMode(RenderPipelineMode.R3Parity, invalidateViewer: true);
    _pipelineButton.DropDownItems.Add(_legacyPipelineMenuItem);
    _pipelineButton.DropDownItems.Add(_parityPipelineMenuItem);
    speedStrip.Items.Add(_pipelineButton);
    ToolStripButton northSouthFlipButton = new("NSFlip")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Flip north/south axis (invert world Z) to match in-game orientation",
    };
    speedStrip.Items.Add(northSouthFlipButton);
    speedStrip.Items.Add(new ToolStripSeparator());
    ToolStripButton skyRenderButton = new("Sky")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Enable/disable sky rendering",
    };
    ToolStripButton r3tButton = new("R3T")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Enable/disable texture usage from R3T/Lgt.R3T",
    };
    ToolStripButton r3mButton = new("R3M")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Enable/disable material/layer behavior from R3M",
    };
    ToolStripButton r3eButton = new("R3E")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Enable/disable map entity rendering from R3E",
    };
    ToolStripButton r3xButton = new("R3X")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Enable/disable environment settings (fog/clear color) from R3X + EXT.spt",
    };
    speedStrip.Items.Add(skyRenderButton);
    speedStrip.Items.Add(r3tButton);
    speedStrip.Items.Add(r3mButton);
    speedStrip.Items.Add(r3eButton);
    speedStrip.Items.Add(r3xButton);
    speedStrip.Items.Add(new ToolStripSeparator());
    ToolStripButton collisionButton = new("Collision")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Show/hide EBP collision walls",
    };
    ToolStripButton gridButton = new("Grid")
    {
      CheckOnClick = true,
      Checked = false,
      ToolTipText = "Show/hide white reference grid",
    };
    speedStrip.Items.Add(collisionButton);
    speedStrip.Items.Add(gridButton);
    ToolStripButton dynamicLightButton = new("DynLight")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Enable/disable dynamic lighting pass",
    };
    ToolStripButton particleMarkersButton = new("FxMark")
    {
      CheckOnClick = true,
      Checked = true,
      ToolTipText = "Show/hide particle/effect instance markers",
    };
    speedStrip.Items.Add(dynamicLightButton);
    speedStrip.Items.Add(particleMarkersButton);

    StatusStrip status = new();
    _statusLabel = new ToolStripStatusLabel(_statusBaseText);
    status.Items.Add(_statusLabel);

    _viewer = new MapViewerControl
    {
      Dock = DockStyle.Fill,
    };
    _viewer.RenderPipelineMode = _renderPipelineMode;
    _viewer.FlipNorthSouth = northSouthFlipButton.Checked;
    _viewer.MoveSpeed = (float)speedUpDown.Value;
    speedUpDown.ValueChanged += (_, _) => _viewer.MoveSpeed = (float)speedUpDown.Value;
    _viewer.ShowSky = skyRenderButton.Checked;
    skyRenderButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowSky = skyRenderButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.EnableR3tTextures = r3tButton.Checked;
    r3tButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableR3tTextures = r3tButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.EnableR3mMaterials = r3mButton.Checked;
    r3mButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableR3mMaterials = r3mButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.ShowR3eEntities = r3eButton.Checked;
    r3eButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowR3eEntities = r3eButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.EnableR3xEnvironment = r3xButton.Checked;
    r3xButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableR3xEnvironment = r3xButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.ShowCollisionOverlay = collisionButton.Checked;
    collisionButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowCollisionOverlay = collisionButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.ShowGrid = gridButton.Checked;
    gridButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowGrid = gridButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.EnableDynamicLighting = dynamicLightButton.Checked;
    dynamicLightButton.CheckedChanged += (_, _) =>
    {
      _viewer.EnableDynamicLighting = dynamicLightButton.Checked;
      _viewer.Invalidate();
    };
    _viewer.ShowParticleMarkers = particleMarkersButton.Checked;
    particleMarkersButton.CheckedChanged += (_, _) =>
    {
      _viewer.ShowParticleMarkers = particleMarkersButton.Checked;
      _viewer.Invalidate();
    };
    northSouthFlipButton.CheckedChanged += (_, _) =>
    {
      _viewer.FlipNorthSouth = northSouthFlipButton.Checked;
      _viewer.Invalidate();
      SyncTeleportInputsFromCamera();
    };

    SyncTeleportInputsFromCamera();

    _statusTimer = new System.Windows.Forms.Timer
    {
      Interval = 400,
      Enabled = true,
    };
    _statusTimer.Tick += (_, _) => RefreshRuntimeStatus();

    Controls.Add(_viewer);
    Controls.Add(status);
    Controls.Add(speedStrip);
    Controls.Add(menu);
    SetSkySourceMode(_skySourceMode, reloadCurrentMap: false);
    SetRenderPipelineMode(_renderPipelineMode, invalidateViewer: false);
    LoadCameraPositionsFromSettings();
    FormClosing += OnMainFormClosing;

    if (args.Length > 0)
    {
      TryLoadFromArgs(args);
    }
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
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Load Map Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Load failed.";
      RefreshRuntimeStatus();
    }
  }

  private void ShowControls()
  {
    const string message =
      """
      Camera Controls
      - Ctrl+O: select map root folder
      - Map combo (top bar): switch maps by folder name
      - Right mouse drag: look around
      - Mouse wheel: zoom in/out (forward/back)
      - W / A / S / D: move forward/left/back/right
      - Space / Ctrl: move up/down
      - Shift: faster movement
      - Move speed: top bar numeric box
      - Cam X/Y/Z + Go: teleport camera to exact Cam XYZ coordinate
      - Sky dropdown: switch between sky and sky2 source
      - Pipe dropdown: switch renderer (`LegacyCompat` or `R3Parity`)
      - Sky button: show/hide sky render
      - R3T button: toggle texture usage (map/light/sky/entity)
      - R3M button: toggle material-layer behavior
      - R3E button: toggle map entities/particles
      - R3X button: toggle environment fog/clear color
      - Collision button: show/hide EBP collision walls
      - Grid button: show/hide white reference grid
      - DynLight button: toggle dynamic lighting
      - FxMark button: toggle particle/effect map markers
      - R: reset camera to map default
      - Esc: release mouse-look capture
      - Top-left HUD: camera coordinates (Cam XYZ / Src XYZ)
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
    if (!_viewer.TrySetCameraDisplayPosition(x, y, z))
    {
      return;
    }

    SaveCurrentCameraPosition();
    _viewer.Focus();
  }

  private void SyncTeleportInputsFromCamera()
  {
    (float x, float y, float z) = _viewer.GetCameraDisplayPosition();
    SetNumericUpDownValue(_teleportXUpDown, x);
    SetNumericUpDownValue(_teleportYUpDown, y);
    SetNumericUpDownValue(_teleportZUpDown, z);
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
      + $" | ParitySkipLayer: {_viewer.ParitySkippedLayers:N0}";
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
