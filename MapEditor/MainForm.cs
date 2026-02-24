using MapEditor.Formats;
using MapEditor.Viewer;

namespace MapEditor;

internal sealed class MainForm : Form
{
  private readonly MapViewerControl _viewer;
  private readonly ToolStripStatusLabel _statusLabel;
  private readonly ToolStripDropDownButton _skySourceButton;
  private readonly ToolStripMenuItem _skyMenuItem;
  private readonly ToolStripMenuItem _sky2MenuItem;
  private SkySourceMode _skySourceMode = SkySourceMode.Sky2;
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
    ToolStripMenuItem openMenu = new("&Open BSP/EBP...");
    openMenu.ShortcutKeys = Keys.Control | Keys.O;
    openMenu.Click += (_, _) => OpenMapFromDialog();
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

    StatusStrip status = new();
    _statusLabel = new ToolStripStatusLabel("Ready. Ctrl+O open map | Right mouse look | WASD move | Wheel zoom | Speed + Sky/Collision/Grid on top bar | F1 controls");
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

  private void OpenMapFromDialog()
  {
    using OpenFileDialog dialog = new()
    {
      Title = "Open BSP or EBP",
      Filter = "RF Map Files (*.bsp;*.ebp)|*.bsp;*.ebp|BSP (*.bsp)|*.bsp|EBP (*.ebp)|*.ebp|All Files (*.*)|*.*",
      Multiselect = false,
      CheckFileExists = true,
    };

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    ResolvePair(dialog.FileName, out string? bspPath, out string? ebpPath, out string? error);
    if (error != null)
    {
      MessageBox.Show(this, error, "Open Map", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    LoadMap(bspPath!, ebpPath!);
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
  }

  private void LoadMap(string bspPath, string ebpPath)
  {
    try
    {
      LoadedMap map = BspLoader.Load(bspPath, ebpPath, _skySourceMode);
      _viewer.SetMap(map);
      _loadedBspPath = bspPath;
      _loadedEbpPath = ebpPath;
      Text = $"RF MapEditor (Viewer) - {map.Name}";
      _statusLabel.Text =
        $"Loaded {map.Name} | BSP TriVerts: {map.BspTriangleVertices.Length:N0} | EBP Vertices: {map.CollisionVertices.Length:N0} | EBP Lines: {map.CollisionLines.Length:N0}"
        + $" | RenderVerts: {map.BspRenderVertices.Length:N0}"
        + $" | TextureBlobs: {map.SurfaceTextures.Length:N0} | LgtTex: {map.LightmapTextures.Length:N0}"
        + $" | SkySource: {GetSkySourceName(_skySourceMode)} | SkyVerts: {map.SkyRenderVertices.Length:N0} | SkyTex: {map.SkySurfaceTextures.Length:N0}"
        + $" | EntModels: {map.MapEntityModelCount:N0} | EntInst: {map.MapEntityInstanceCount:N0} | EntVerts: {map.EntityRenderVertices.Length:N0} | EntTex: {map.EntitySurfaceTextures.Length:N0}"
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
      - Right mouse drag: look around
      - Mouse wheel: zoom in/out (forward/back)
      - W / A / S / D: move forward/left/back/right
      - Space / Ctrl: move up/down
      - Shift: faster movement
      - Move speed: top bar numeric box
      - Sky dropdown: switch between sky and sky2 source
      - Collision button: show/hide EBP collision walls
      - Grid button: show/hide white reference grid
      - R: reset camera to map default
      - Esc: release mouse-look capture
      """;

    MessageBox.Show(this, message, "Controls", MessageBoxButtons.OK, MessageBoxIcon.Information);
  }

  private static void ResolvePair(string selectedPath, out string? bspPath, out string? ebpPath, out string? error)
  {
    bspPath = null;
    ebpPath = null;
    error = null;

    string fullPath = Path.GetFullPath(selectedPath);
    string ext = Path.GetExtension(fullPath);

    if (ext.Equals(".bsp", StringComparison.OrdinalIgnoreCase))
    {
      bspPath = fullPath;
      ebpPath = Path.ChangeExtension(fullPath, ".ebp");
    }
    else if (ext.Equals(".ebp", StringComparison.OrdinalIgnoreCase))
    {
      ebpPath = fullPath;
      bspPath = Path.ChangeExtension(fullPath, ".bsp");
    }
    else
    {
      error = $"Unsupported file extension '{ext}'.";
      return;
    }

    if (!File.Exists(bspPath))
    {
      error = $"Missing BSP file: {bspPath}";
      return;
    }

    if (!File.Exists(ebpPath))
    {
      error = $"Missing EBP file: {ebpPath}";
    }
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
}
