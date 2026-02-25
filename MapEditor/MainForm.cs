using MapEditor.Formats;
using MapEditor.Viewer;

namespace MapEditor;

internal sealed class MainForm : Form
{
  private readonly MapViewerControl _viewer;
  private readonly ToolStripStatusLabel _statusLabel;
  private readonly ToolStripComboBox _mapComboBox;
  private readonly ToolStripDropDownButton _skySourceButton;
  private readonly ToolStripMenuItem _skyMenuItem;
  private readonly ToolStripMenuItem _sky2MenuItem;
  private readonly List<MapListEntry> _mapEntries = [];
  private readonly Dictionary<string, EntityArchiveCacheInfo> _entityArchiveCacheByRoot = new(StringComparer.OrdinalIgnoreCase);
  private SkySourceMode _skySourceMode = SkySourceMode.Sky2;
  private bool _suppressMapSelectionChanged;
  private string? _mapRootPath;
  private string? _loadedBspPath;
  private string? _loadedEbpPath;

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
      Checked = true,
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
    _statusLabel = new ToolStripStatusLabel("Ready. Ctrl+O open map folder | Select map in top combobox | Right mouse look | WASD move | Wheel zoom | F1 controls");
    status.Items.Add(_statusLabel);

    _viewer = new MapViewerControl
    {
      Dock = DockStyle.Fill,
    };
    _viewer.MoveSpeed = (float)speedUpDown.Value;
    speedUpDown.ValueChanged += (_, _) => _viewer.MoveSpeed = (float)speedUpDown.Value;
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

    Controls.Add(_viewer);
    Controls.Add(status);
    Controls.Add(speedStrip);
    Controls.Add(menu);
    SetSkySourceMode(_skySourceMode, reloadCurrentMap: false);

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
    try
    {
      EntityArchiveCacheInfo archiveCache = EnsureEntityArchiveCacheForBsp(bspPath);
      LoadedMap map = BspLoader.Load(bspPath, ebpPath, _skySourceMode);
      _viewer.SetMap(map);
      _loadedBspPath = bspPath;
      _loadedEbpPath = ebpPath;
      SyncSelectedMapToLoadedPath(bspPath);
      Text = $"RF MapEditor (Viewer) - {map.Name}";
      string cacheTag = archiveCache.ArchiveCount > 0
        ? $"{(archiveCache.WasCached ? "RPKCache:Hit" : "RPKCache:Build")} {archiveCache.ArchiveCount:N0}rpk/{archiveCache.AssetCount:N0}assets"
        : "RPKCache:N/A";
      _statusLabel.Text =
        $"Loaded {map.Name} | BSP TriVerts: {map.BspTriangleVertices.Length:N0} | EBP Vertices: {map.CollisionVertices.Length:N0} | EBP Lines: {map.CollisionLines.Length:N0}"
        + $" | RenderVerts: {map.BspRenderVertices.Length:N0}"
        + $" | TextureBlobs: {map.SurfaceTextures.Length:N0} | LgtTex: {map.LightmapTextures.Length:N0}"
        + $" | Fog: {(map.Environment.FogEnabled ? $"On({map.Environment.FogStart:F0}-{map.Environment.FogEnd:F0})" : "Off")} | ExtDummy: {map.ExtDummies.Length:N0}"
        + $" | SkySource: {GetSkySourceName(_skySourceMode)} | SkyVerts: {map.SkyRenderVertices.Length:N0} | SkyTex: {map.SkySurfaceTextures.Length:N0}"
        + $" | EntModels: {map.MapEntityModelCount:N0} | EntInst: {map.MapEntityInstanceCount:N0} | FxInst: {map.ParticleInstancePositions.Length:N0} | EntVerts: {map.EntityRenderVertices.Length:N0} | EntTex: {map.EntitySurfaceTextures.Length:N0}"
        + $" | {cacheTag}"
        + $" | Bounds: ({map.Bounds.Min.X:F0},{map.Bounds.Min.Y:F0},{map.Bounds.Min.Z:F0})..({map.Bounds.Max.X:F0},{map.Bounds.Max.Y:F0},{map.Bounds.Max.Z:F0})";
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Load Map Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusLabel.Text = "Load failed.";
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
      - Sky dropdown: switch between sky and sky2 source
      - Collision button: show/hide EBP collision walls
      - Grid button: show/hide white reference grid
      - DynLight button: toggle dynamic lighting
      - FxMark button: toggle particle/effect map markers
      - R: reset camera to map default
      - Esc: release mouse-look capture
      """;

    MessageBox.Show(this, message, "Controls", MessageBoxButtons.OK, MessageBoxIcon.Information);
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

  private static string GetSkySourceName(SkySourceMode skySourceMode)
  {
    return skySourceMode == SkySourceMode.Sky2 ? "sky2" : "sky";
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

  private readonly record struct MapListEntry(string Name, string FolderPath, string BspPath, string EbpPath);
}
