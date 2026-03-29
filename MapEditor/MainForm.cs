using MapEditor.Formats;
using MapEditor.Viewer;
using OpenTK.Mathematics;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
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
  private const float FixedSnapDistance = 10.0f;
  private const float BspTranslateStep = 50.0f;
  private const float BspScaleStepPercent = 5.0f;
  private const float BspRotateStepDegrees = 5.0f;
  private const string DefaultServerRootPath = @"D:\RF Server\Server\Zone Server\RF_Bin";
  private SkySourceMode _skySourceMode = SkySourceMode.Sky2;
  private RenderPipelineMode _renderPipelineMode = RenderPipelineMode.R3Parity;
  private bool _suppressMapSelectionChanged;
  private string? _mapRootPath;
  private string? _serverRootPath;
  private LoadedMap? _loadedMap;
  private MapDocument? _mapDocument;
  private string? _loadedBspPath;
  private string? _loadedEbpPath;
  private EntityArchiveCacheInfo _lastEntityArchiveCache;
  private bool _suppressBoundaryCollisionValueChanged;
  private bool _suppressToolModeSync;
  private bool _suppressBspSelectionModeSync;
  private bool _strictLoadModeEnabled = true;
  private string _statusBaseText = "Ready. Ctrl+O open map folder | Select map in top combobox | Right mouse look | WASD move | Wheel zoom | F1 controls";
  private Panel _viewerCanvasPanel = null!;
  private Panel _serverInspectorOverlayPanel = null!;
  private Button _serverInspectorDockTabButton = null!;
  private Label _serverInspectorSummaryLabel = null!;
  private ComboBox _serverInspectorModeComboBox = null!;
  private Panel _serverInspectorContentPanel = null!;
  private Panel _serverMonsterPanel = null!;
  private TextBox _serverMonsterSearchTextBox = null!;
  private TextBox _serverMonsterDefinitionSearchTextBox = null!;
  private ComboBox _serverMonsterDefinitionComboBox = null!;
  private ComboBox _serverMonsterSpawnSetComboBox = null!;
  private NumericUpDown _serverMonsterIntervalUpDown = null!;
  private Button _serverMonsterAddBlockButton = null!;
  private Button _serverMonsterAddDummyButton = null!;
  private Button _serverMonsterAddSpawnButton = null!;
  private Label _serverMonsterBlockLabel = null!;
  private Label _serverMonsterDetailLabel = null!;
  private Button _serverInspectorJumpButton = null!;
  private Button _serverInspectorAutoHideButton = null!;
  private ListView _serverSummaryListView = null!;
  private ListView _serverMonsterListView = null!;
  private ListView _serverMonsterDummyListView = null!;
  private ListView _serverMonsterSpawnListView = null!;
  private ListView _serverPortalListView = null!;
  private ListView _serverStoreListView = null!;
  private ListView _serverStartListView = null!;
  private ListView _serverResourceListView = null!;
  private string _serverMonsterSearchText = string.Empty;
  private string _serverMonsterDefinitionSearchText = string.Empty;
  private ServerInspectorMode _serverInspectorMode = ServerInspectorMode.Summary;
  private bool _suppressServerSelectionSync;
  private bool _suppressMonsterSpawnIntervalValueChanged;
  private bool _suppressMonsterDefinitionSelectionChanged;
  private bool _serverInspectorDockHidden;

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
    _strictLoadModeEnabled = true;
    _strictLoadModeMenuItem.Checked = true;
    _strictLoadModeMenuItem.Enabled = false;
    _strictLoadModeMenuItem.ToolTipText = "Strict load is always ON (client-parity mode).";
    InitializeServerInspectorUi();

    _viewer = new MapViewerControl
    {
      Dock = DockStyle.Fill,
    };
    _viewerCanvasPanel.Controls.Add(_viewer);
    _viewer.BringToFront();
    _serverInspectorDockTabButton.BringToFront();
    _serverInspectorOverlayPanel.BringToFront();
    InitializeViewerBindings();
    SyncTeleportInputsFromCamera();

    _statusTimer.Tick += (_, _) => RefreshRuntimeStatus();
    _statusTimer.Start();

    SetSkySourceMode(_skySourceMode, reloadCurrentMap: false);
    SetRenderPipelineMode(_renderPipelineMode, invalidateViewer: false);
    LoadCameraPositionsFromSettings();
    FormClosing += OnMainFormClosing;
    UpdateUndoUiState();
    RefreshServerInspector();

    if (args.Length > 0)
    {
      TryLoadFromArgs(args);
    }
  }

  private enum ServerInspectorMode
  {
    Monsters = 0,
    Portals = 1,
    Stores = 2,
    Starts = 3,
    Resources = 4,
    Summary = 5,
  }

  private void InitializeServerInspectorUi()
  {
    _viewerHostPanel.SuspendLayout();
    _viewerHostPanel.Controls.Clear();

    _viewerCanvasPanel = new Panel
    {
      Dock = DockStyle.Fill,
      BackColor = _viewerHostPanel.BackColor,
    };
    _viewerHintLabel.Dock = DockStyle.Fill;
    _viewerCanvasPanel.Controls.Add(_viewerHintLabel);
    _viewerCanvasPanel.Resize += (_, _) => ApplyServerInspectorSplitterDistance();

    _serverInspectorDockTabButton = new Button
    {
      Width = 34,
      Height = 82,
      Text = "Srv\r\nData",
      FlatStyle = FlatStyle.System,
      Visible = false,
      TabStop = false,
    };
    _serverInspectorDockTabButton.TextAlign = ContentAlignment.MiddleCenter;
    _serverInspectorDockTabButton.UseMnemonic = false;
    _serverInspectorDockTabButton.Cursor = Cursors.Hand;
    _serverInspectorDockTabButton.Click += (_, _) => ShowServerInspectorDockPanel();

    _serverInspectorOverlayPanel = new Panel
    {
      Width = 360,
      Padding = new Padding(8),
      BorderStyle = BorderStyle.FixedSingle,
      BackColor = Color.FromArgb(245, 248, 252),
      Visible = false,
    };
    Panel inspectorPanel = new()
    {
      Dock = DockStyle.Fill,
      BackColor = _serverInspectorOverlayPanel.BackColor,
    };
    Panel inspectorHeaderPanel = new()
    {
      Dock = DockStyle.Top,
      Height = 28,
    };
    Label inspectorTitleLabel = new()
    {
      Dock = DockStyle.Fill,
      Text = "Server Data",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    _serverInspectorAutoHideButton = new Button
    {
      Dock = DockStyle.Right,
      Width = 78,
      Text = "Auto Hide",
      TabStop = false,
    };
    _serverInspectorAutoHideButton.UseMnemonic = false;
    _serverInspectorAutoHideButton.Cursor = Cursors.Hand;
    _serverInspectorAutoHideButton.Click += (_, _) => HideServerInspectorDockPanel();
    inspectorHeaderPanel.Controls.Add(_serverInspectorAutoHideButton);
    inspectorHeaderPanel.Controls.Add(inspectorTitleLabel);
    _serverInspectorSummaryLabel = new Label
    {
      Dock = DockStyle.Top,
      Height = 44,
      Text = "Load File > Load > Server Folder to inspect server-side map data.",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    _serverInspectorModeComboBox = new ComboBox
    {
      Dock = DockStyle.Top,
      DropDownStyle = ComboBoxStyle.DropDownList,
      Height = 28,
    };
    _serverInspectorModeComboBox.Items.AddRange(
    [
      new ServerInspectorModeItem("Monsters", ServerInspectorMode.Monsters),
      new ServerInspectorModeItem("Portals", ServerInspectorMode.Portals),
      new ServerInspectorModeItem("Stores", ServerInspectorMode.Stores),
      new ServerInspectorModeItem("Starts", ServerInspectorMode.Starts),
      new ServerInspectorModeItem("Resources", ServerInspectorMode.Resources),
      new ServerInspectorModeItem("Summary", ServerInspectorMode.Summary),
    ]);
    _serverInspectorModeComboBox.SelectedIndexChanged += OnServerInspectorModeComboBoxSelectedIndexChanged;

    _serverInspectorJumpButton = new Button
    {
      Dock = DockStyle.Top,
      Height = 28,
      Text = "Go To Selected Dummy",
    };
    _serverInspectorJumpButton.Click += (_, _) =>
    {
      ListView? activeListView = GetActiveServerListView();
      if (activeListView != null)
      {
        JumpToSelectedServerItem(activeListView);
      }
    };

    _serverInspectorContentPanel = new Panel
    {
      Dock = DockStyle.Fill,
    };

    _serverSummaryListView = CreateServerListView(("Field", 120), ("Value", 220));
    _serverMonsterListView = CreateServerListView(("Block", 92), ("Code", 92), ("Monster", 132), ("Dummies", 54), ("Entries", 54), ("Limit", 54), ("Center", 120));
    _serverMonsterListView.SelectedIndexChanged += OnServerMonsterListSelectionChanged;
    _serverMonsterDummyListView = CreateServerListView(("Dummy", 88), ("Pos", 148), ("Size", 88));
    _serverMonsterDummyListView.SelectedIndexChanged += OnServerMonsterDummyListSelectionChanged;
    _serverMonsterDummyListView.SelectedIndexChanged += OnServerInspectorListSelectionChanged;
    _serverMonsterSpawnListView = CreateServerListView(("Set", 42), ("Monster Code", 94), ("Monster", 132), ("Interval", 64), ("Limit", 48), ("Min", 44), ("Max", 44));
    _serverMonsterSpawnListView.SelectedIndexChanged += OnServerMonsterSpawnListSelectionChanged;
    _serverPortalListView = CreateServerListView(("Code", 92), ("Link Map", 92), ("Link Portal", 92), ("Menu", 124), ("Need Lv", 60), ("Dummy", 84), ("Pos", 170));
    _serverPortalListView.SelectedIndexChanged += OnServerInspectorListSelectionChanged;
    _serverStoreListView = CreateServerListView(("Store", 92), ("NPC", 148), ("NPC Code", 96), ("Trade", 56), ("Angle", 56), ("Dummy", 84), ("Pos", 170));
    _serverStoreListView.SelectedIndexChanged += OnServerInspectorListSelectionChanged;
    _serverStartListView = CreateServerListView(("Index", 54), ("Dummy", 84), ("Pos", 180));
    _serverStartListView.SelectedIndexChanged += OnServerInspectorListSelectionChanged;
    _serverResourceListView = CreateServerListView(("Index", 54), ("Grade", 68), ("Dummy", 84), ("Pos", 180));
    _serverResourceListView.SelectedIndexChanged += OnServerInspectorListSelectionChanged;

    _serverMonsterPanel = new Panel
    {
      Dock = DockStyle.Fill,
    };

    Panel monsterSearchPanel = new()
    {
      Dock = DockStyle.Top,
      Height = 28,
      Padding = new Padding(0, 0, 0, 4),
    };
    Label monsterSearchLabel = new()
    {
      Dock = DockStyle.Left,
      Width = 52,
      Text = "Search",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    _serverMonsterSearchTextBox = new TextBox
    {
      Dock = DockStyle.Fill,
      PlaceholderText = "Monster code or English name",
    };
    _serverMonsterSearchTextBox.TextChanged += OnServerMonsterSearchTextChanged;
    monsterSearchPanel.Controls.Add(_serverMonsterSearchTextBox);
    monsterSearchPanel.Controls.Add(monsterSearchLabel);

    Panel monsterEditorPanel = new()
    {
      Dock = DockStyle.Bottom,
      Height = 380,
      Padding = new Padding(0, 6, 0, 0),
    };
    Panel monsterEditorHeaderPanel = new()
    {
      Dock = DockStyle.Top,
      Height = 98,
    };
    _serverMonsterBlockLabel = new Label
    {
      Dock = DockStyle.Top,
      Height = 34,
      Text = "Select a monster block to edit dummies and spawn entries.",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    Panel monsterDefinitionPanel = new()
    {
      Dock = DockStyle.Top,
      Height = 28,
      Padding = new Padding(0, 0, 0, 4),
    };
    Label monsterDefinitionLabel = new()
    {
      Dock = DockStyle.Left,
      Width = 50,
      Text = "Monster",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    _serverMonsterAddSpawnButton = new Button
    {
      Dock = DockStyle.Right,
      Width = 98,
      Text = "Spawn Monster",
    };
    _serverMonsterAddSpawnButton.Click += (_, _) => AddMonsterSpawnFromUi();
    _serverMonsterAddBlockButton = new Button
    {
      Dock = DockStyle.Right,
      Width = 92,
      Text = "Add Monster",
    };
    _serverMonsterAddBlockButton.Click += (_, _) => AddMonsterBlockFromUi();
    _serverMonsterDefinitionComboBox = new ComboBox
    {
      Dock = DockStyle.Fill,
      DropDownStyle = ComboBoxStyle.DropDownList,
    };
    _serverMonsterDefinitionComboBox.SelectedIndexChanged += OnServerMonsterDefinitionComboBoxSelectedIndexChanged;
    _serverMonsterDefinitionSearchTextBox = new TextBox
    {
      Dock = DockStyle.Right,
      Width = 132,
      PlaceholderText = "Find monster",
    };
    _serverMonsterDefinitionSearchTextBox.TextChanged += OnServerMonsterDefinitionSearchTextChanged;
    monsterDefinitionPanel.Controls.Add(_serverMonsterDefinitionComboBox);
    monsterDefinitionPanel.Controls.Add(_serverMonsterDefinitionSearchTextBox);
    monsterDefinitionPanel.Controls.Add(_serverMonsterAddSpawnButton);
    monsterDefinitionPanel.Controls.Add(_serverMonsterAddBlockButton);
    monsterDefinitionPanel.Controls.Add(monsterDefinitionLabel);

    Panel monsterActionPanel = new()
    {
      Dock = DockStyle.Top,
      Height = 28,
    };
    Label monsterSetLabel = new()
    {
      Dock = DockStyle.Left,
      Width = 28,
      Text = "Set",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    _serverMonsterSpawnSetComboBox = new ComboBox
    {
      Dock = DockStyle.Left,
      Width = 72,
      DropDownStyle = ComboBoxStyle.DropDownList,
    };
    _serverMonsterSpawnSetComboBox.SelectedIndexChanged += (_, _) => UpdateMonsterEditorActions();
    Label monsterIntervalLabel = new()
    {
      Dock = DockStyle.Left,
      Width = 52,
      Text = "Interval",
      TextAlign = ContentAlignment.MiddleLeft,
    };
    _serverMonsterIntervalUpDown = new NumericUpDown
    {
      Dock = DockStyle.Left,
      Width = 92,
      Minimum = 0,
      Maximum = uint.MaxValue,
      ThousandsSeparator = true,
      Value = 60,
    };
    _serverMonsterIntervalUpDown.ValueChanged += OnServerMonsterSpawnIntervalValueChanged;
    _serverMonsterAddDummyButton = new Button
    {
      Dock = DockStyle.Right,
      Width = 92,
      Text = "Add Dummy",
    };
    _serverMonsterAddDummyButton.Click += (_, _) => AddMonsterDummyFromUi();
    monsterActionPanel.Controls.Add(_serverMonsterAddDummyButton);
    monsterActionPanel.Controls.Add(_serverMonsterIntervalUpDown);
    monsterActionPanel.Controls.Add(monsterIntervalLabel);
    monsterActionPanel.Controls.Add(_serverMonsterSpawnSetComboBox);
    monsterActionPanel.Controls.Add(monsterSetLabel);

    monsterEditorHeaderPanel.Controls.Add(monsterActionPanel);
    monsterEditorHeaderPanel.Controls.Add(monsterDefinitionPanel);
    monsterEditorHeaderPanel.Controls.Add(_serverMonsterBlockLabel);

    Label monsterDummyListLabel = new()
    {
      Dock = DockStyle.Top,
      Height = 20,
      Text = "Dummy Boxes",
      TextAlign = ContentAlignment.BottomLeft,
    };
    Panel monsterDummyListPanel = new()
    {
      Dock = DockStyle.Fill,
      Padding = new Padding(0, 6, 0, 0),
    };
    monsterDummyListPanel.Controls.Add(_serverMonsterDummyListView);
    monsterDummyListPanel.Controls.Add(monsterDummyListLabel);

    Label monsterSpawnListLabel = new()
    {
      Dock = DockStyle.Top,
      Height = 20,
      Text = "Spawn Entries",
      TextAlign = ContentAlignment.BottomLeft,
    };

    Panel monsterSpawnListPanel = new()
    {
      Dock = DockStyle.Fill,
      Padding = new Padding(0, 6, 0, 0),
    };
    monsterSpawnListPanel.Controls.Add(_serverMonsterSpawnListView);
    monsterSpawnListPanel.Controls.Add(monsterSpawnListLabel);

    SplitContainer monsterListsSplitContainer = new()
    {
      Dock = DockStyle.Fill,
      Orientation = Orientation.Horizontal,
      FixedPanel = FixedPanel.None,
      IsSplitterFixed = false,
      Panel1MinSize = 90,
      Panel2MinSize = 120,
      SplitterWidth = 6,
    };
    bool monsterListsSplitterInitialized = false;
    void EnsureMonsterListsSplitterDistance()
    {
      if (monsterListsSplitterInitialized)
      {
        return;
      }

      int totalHeight = monsterListsSplitContainer.ClientSize.Height;
      int maxDistance = totalHeight - monsterListsSplitContainer.Panel2MinSize - monsterListsSplitContainer.SplitterWidth;
      if (maxDistance < monsterListsSplitContainer.Panel1MinSize)
      {
        return;
      }

      monsterListsSplitContainer.SplitterDistance = Math.Clamp(108, monsterListsSplitContainer.Panel1MinSize, maxDistance);
      monsterListsSplitterInitialized = true;
    }

    monsterListsSplitContainer.HandleCreated += (_, _) => EnsureMonsterListsSplitterDistance();
    monsterListsSplitContainer.Resize += (_, _) => EnsureMonsterListsSplitterDistance();
    monsterListsSplitContainer.Panel1.Controls.Add(monsterDummyListPanel);
    monsterListsSplitContainer.Panel2.Controls.Add(monsterSpawnListPanel);

    _serverMonsterDetailLabel = new Label
    {
      Dock = DockStyle.Bottom,
      Height = 52,
      Text = "Load a server folder to inspect monster spawns.",
      TextAlign = ContentAlignment.TopLeft,
    };

    monsterEditorPanel.Controls.Add(monsterListsSplitContainer);
    monsterEditorPanel.Controls.Add(_serverMonsterDetailLabel);
    monsterEditorPanel.Controls.Add(monsterEditorHeaderPanel);

    _serverMonsterPanel.Controls.Add(_serverMonsterListView);
    _serverMonsterPanel.Controls.Add(monsterEditorPanel);
    _serverMonsterPanel.Controls.Add(monsterSearchPanel);

    inspectorPanel.Controls.Add(_serverInspectorContentPanel);
    inspectorPanel.Controls.Add(_serverInspectorJumpButton);
    inspectorPanel.Controls.Add(_serverInspectorModeComboBox);
    inspectorPanel.Controls.Add(_serverInspectorSummaryLabel);
    inspectorPanel.Controls.Add(inspectorHeaderPanel);
    _serverInspectorOverlayPanel.Controls.Add(inspectorPanel);
    _viewerCanvasPanel.Controls.Add(_serverInspectorDockTabButton);
    _viewerCanvasPanel.Controls.Add(_serverInspectorOverlayPanel);
    _viewerHostPanel.Controls.Add(_viewerCanvasPanel);
    ApplyServerInspectorSplitterDistance();
    _viewerHostPanel.ResumeLayout(performLayout: true);
    SetServerInspectorMode(ServerInspectorMode.Summary, syncComboBox: true);
  }

  private void ApplyServerInspectorSplitterDistance()
  {
    if (_viewerCanvasPanel == null || _viewerCanvasPanel.IsDisposed || _serverInspectorOverlayPanel == null || _serverInspectorOverlayPanel.IsDisposed)
    {
      return;
    }

    int availableWidth = _viewerCanvasPanel.ClientSize.Width;
    int availableHeight = _viewerCanvasPanel.ClientSize.Height;
    if (availableWidth <= 0 || availableHeight <= 0)
    {
      return;
    }

    const int margin = 8;
    int overlayWidth = Math.Min(380, Math.Max(300, availableWidth / 3));
    int overlayHeight = Math.Max(260, availableHeight - margin * 2);
    _serverInspectorOverlayPanel.Bounds = new Rectangle(
      Math.Max(margin, availableWidth - overlayWidth - margin),
      margin,
      overlayWidth,
      overlayHeight);

    if (_serverInspectorDockTabButton != null && !_serverInspectorDockTabButton.IsDisposed)
    {
      int tabWidth = 34;
      int tabHeight = Math.Min(92, Math.Max(72, availableHeight / 6));
      _serverInspectorDockTabButton.Bounds = new Rectangle(
        Math.Max(margin, availableWidth - tabWidth - margin),
        Math.Max(margin, margin + 28),
        tabWidth,
        tabHeight);
    }
  }

  private void UpdateServerInspectorDockVisibility()
  {
    bool hasServerData = _loadedMap?.ServerData != null;
    if (_serverInspectorOverlayPanel != null)
    {
      _serverInspectorOverlayPanel.Visible = hasServerData && !_serverInspectorDockHidden;
      if (_serverInspectorOverlayPanel.Visible)
      {
        _serverInspectorOverlayPanel.BringToFront();
      }
    }

    if (_serverInspectorDockTabButton != null)
    {
      _serverInspectorDockTabButton.Visible = hasServerData && _serverInspectorDockHidden;
      if (_serverInspectorDockTabButton.Visible)
      {
        _serverInspectorDockTabButton.BringToFront();
      }
    }

    if (_serverInspectorAutoHideButton != null)
    {
      _serverInspectorAutoHideButton.Visible = hasServerData && !_serverInspectorDockHidden;
    }

    ApplyServerInspectorSplitterDistance();
  }

  private void ShowServerInspectorDockPanel()
  {
    _serverInspectorDockHidden = false;
    UpdateServerInspectorDockVisibility();
  }

  private void HideServerInspectorDockPanel()
  {
    _serverInspectorDockHidden = true;
    UpdateServerInspectorDockVisibility();
  }

  private ListView CreateServerListView(params (string Title, int Width)[] columns)
  {
    ListView listView = new()
    {
      Dock = DockStyle.Fill,
      FullRowSelect = true,
      GridLines = true,
      HideSelection = false,
      MultiSelect = false,
      UseCompatibleStateImageBehavior = false,
      View = View.Details,
    };
    listView.ItemActivate += OnServerListItemActivate;

    for (int index = 0; index < columns.Length; ++index)
    {
      (string title, int width) = columns[index];
      listView.Columns.Add(title, width);
    }

    return listView;
  }

  private void OnServerInspectorModeComboBoxSelectedIndexChanged(object? sender, EventArgs e)
  {
    if (_serverInspectorModeComboBox.SelectedItem is ServerInspectorModeItem item)
    {
      SetServerInspectorMode(item.Mode, syncComboBox: false);
    }
  }

  private void SetServerInspectorMode(ServerInspectorMode mode, bool syncComboBox)
  {
    _serverInspectorMode = mode;

    if (syncComboBox && _serverInspectorModeComboBox != null)
    {
      for (int index = 0; index < _serverInspectorModeComboBox.Items.Count; ++index)
      {
        if (_serverInspectorModeComboBox.Items[index] is ServerInspectorModeItem item && item.Mode == mode)
        {
          if (_serverInspectorModeComboBox.SelectedIndex != index)
          {
            _serverInspectorModeComboBox.SelectedIndex = index;
          }
          break;
        }
      }
    }

    if (_serverInspectorContentPanel != null)
    {
      _serverInspectorContentPanel.SuspendLayout();
      _serverInspectorContentPanel.Controls.Clear();
      switch (mode)
      {
        case ServerInspectorMode.Monsters:
          _serverInspectorContentPanel.Controls.Add(_serverMonsterPanel);
          break;
        case ServerInspectorMode.Portals:
          _serverInspectorContentPanel.Controls.Add(_serverPortalListView);
          break;
        case ServerInspectorMode.Stores:
          _serverInspectorContentPanel.Controls.Add(_serverStoreListView);
          break;
        case ServerInspectorMode.Starts:
          _serverInspectorContentPanel.Controls.Add(_serverStartListView);
          break;
        case ServerInspectorMode.Resources:
          _serverInspectorContentPanel.Controls.Add(_serverResourceListView);
          break;
        default:
          _serverInspectorContentPanel.Controls.Add(_serverSummaryListView);
          break;
      }
      _serverInspectorContentPanel.ResumeLayout(performLayout: true);
    }

    UpdateServerOverlayModeFromInspector();
  }

  private void UpdateServerOverlayModeFromInspector()
  {
    if (_viewer == null)
    {
      return;
    }

    bool hasServerData = _loadedMap?.ServerData != null;
    _viewer.ShowServerOverlay = hasServerData && _serverInspectorMode != ServerInspectorMode.Summary;
    _viewer.ActiveServerOverlayFilter = _serverInspectorMode switch
    {
      ServerInspectorMode.Monsters => MapViewerControl.ServerOverlayFilter.Monsters,
      ServerInspectorMode.Portals => MapViewerControl.ServerOverlayFilter.Portals,
      ServerInspectorMode.Stores => MapViewerControl.ServerOverlayFilter.Stores,
      ServerInspectorMode.Starts => MapViewerControl.ServerOverlayFilter.Starts,
      ServerInspectorMode.Resources => MapViewerControl.ServerOverlayFilter.Resources,
      _ => MapViewerControl.ServerOverlayFilter.None,
    };
    UpdateDummySelectionToolState();
    UpdateViewerServerInteraction();
    UpdateServerInspectorActions();
    _viewer.Invalidate();
  }

  private void OnServerInspectorListSelectionChanged(object? sender, EventArgs e)
  {
    if (_suppressServerSelectionSync || sender is not ListView listView)
    {
      return;
    }

    if (!ReferenceEquals(listView, GetActiveServerListView()))
    {
      return;
    }

    if (_serverInspectorMode == ServerInspectorMode.Monsters)
    {
      _viewer.SetSelectedServerDummy(GetSelectedMonsterNavigationDummy());
    }
    else
    {
      _viewer.SetSelectedServerDummy(TryGetSelectedServerInspectorItem(listView)?.Dummy);
    }

    UpdateServerInspectorActions();
  }

  private void UpdateServerInspectorActions()
  {
    bool hasSelection = _serverInspectorMode == ServerInspectorMode.Monsters
      ? GetSelectedMonsterInspectorItem() != null || GetSelectedMonsterDummyInspectorItem() != null
      : TryGetSelectedServerInspectorItem(GetActiveServerListView()) != null;

    if (_serverInspectorJumpButton != null)
    {
      _serverInspectorJumpButton.Enabled = hasSelection;
    }
  }

  private void UpdateViewerServerInteraction()
  {
    if (_viewer == null)
    {
      return;
    }

    bool hasServerData = _loadedMap?.ServerData != null;
    _viewer.ServerInteractionEnabled =
      hasServerData &&
      _selectDummyButton.Checked &&
      _serverInspectorMode != ServerInspectorMode.Summary;
  }

  private void UpdateDummySelectionToolState()
  {
    if (_selectDummyButton == null)
    {
      return;
    }

    bool hasServerData = _loadedMap?.ServerData != null;
    if (_selectDummyButton.Enabled == hasServerData)
    {
      return;
    }

    _selectDummyButton.Enabled = hasServerData;
    if (!hasServerData && _selectDummyButton.Checked)
    {
      _selectDummyButton.Checked = false;
    }
  }

  private ListView? GetActiveServerListView()
  {
    return _serverInspectorMode switch
    {
      ServerInspectorMode.Monsters => _serverMonsterDummyListView.SelectedItems.Count > 0 ? _serverMonsterDummyListView : _serverMonsterListView,
      ServerInspectorMode.Portals => _serverPortalListView,
      ServerInspectorMode.Stores => _serverStoreListView,
      ServerInspectorMode.Starts => _serverStartListView,
      ServerInspectorMode.Resources => _serverResourceListView,
      ServerInspectorMode.Summary => _serverSummaryListView,
      _ => null,
    };
  }

  private IEnumerable<(ServerInspectorMode Mode, ListView ListView)> EnumerateServerInspectorLists()
  {
    yield return (ServerInspectorMode.Portals, _serverPortalListView);
    yield return (ServerInspectorMode.Stores, _serverStoreListView);
    yield return (ServerInspectorMode.Starts, _serverStartListView);
    yield return (ServerInspectorMode.Resources, _serverResourceListView);
  }

  private static IServerInspectorItem? TryGetSelectedServerInspectorItem(ListView? listView)
  {
    if (listView == null || listView.SelectedItems.Count <= 0)
    {
      return null;
    }

    return listView.SelectedItems[0].Tag as IServerInspectorItem;
  }

  private void OnViewerServerDummySelectionChanged(ServerDummyPosition? dummy)
  {
    if (_loadedMap?.ServerData == null)
    {
      return;
    }

    SelectServerInspectorItem(dummy);
    if (dummy != null)
    {
      _statusBaseText = $"Selected server dummy {dummy.Code} | Pos: ({dummy.EditedWorldCenter.X:F1},{dummy.EditedWorldCenter.Y:F1},{dummy.EditedWorldCenter.Z:F1})";
      RefreshRuntimeStatus();
    }
  }

  private void OnViewerServerDummyMoved(ServerDummyPosition dummy)
  {
    if (_loadedMap?.ServerData is not ServerMapData serverData)
    {
      return;
    }

    for (int blockIndex = 0; blockIndex < serverData.MonsterBlocks.Length; ++blockIndex)
    {
      ServerMapMonsterBlock block = serverData.MonsterBlocks[blockIndex];
      if (block.Dummies.Any(dummyRef => ReferenceEquals(dummyRef.Dummy, dummy)))
      {
        block.WorldCenter = ComputeMonsterBlockCenter(block.Dummies);
      }
    }

    RefreshServerInspector();
    SelectServerInspectorItem(dummy);
    _statusBaseText = $"Moved server dummy {dummy.Code} | Pos: ({dummy.EditedWorldCenter.X:F1},{dummy.EditedWorldCenter.Y:F1},{dummy.EditedWorldCenter.Z:F1})";
    RefreshRuntimeStatus();
  }

  private void SelectServerInspectorItem(ServerDummyPosition? dummy)
  {
    _suppressServerSelectionSync = true;
    try
    {
      if (TrySelectMonsterInspectorItem(dummy))
      {
        UpdateServerInspectorActions();
        return;
      }

      foreach ((ServerInspectorMode mode, ListView listView) in EnumerateServerInspectorLists())
      {
        ListViewItem? match = FindServerInspectorListItemByDummy(listView, dummy);
        if (match == null)
        {
          continue;
        }

        SetServerInspectorMode(mode, syncComboBox: true);
        ClearListViewSelection(listView);
        match.Selected = true;
        match.Focused = true;
        match.EnsureVisible();
        UpdateServerInspectorActions();
        return;
      }

      if (dummy != null && !string.IsNullOrWhiteSpace(_serverMonsterSearchText))
      {
        _serverMonsterSearchTextBox.Text = string.Empty;
        SelectServerInspectorItem(dummy);
        return;
      }

      ClearListViewSelection(_serverMonsterListView);
      ClearListViewSelection(_serverMonsterDummyListView);
      ClearListViewSelection(_serverMonsterSpawnListView);
      UpdateMonsterInspectorSelectionDetails();
      foreach ((_, ListView listView) in EnumerateServerInspectorLists())
      {
        ClearListViewSelection(listView);
      }
      UpdateServerInspectorActions();
    }
    finally
    {
      _suppressServerSelectionSync = false;
    }
  }

  private bool TrySelectMonsterInspectorItem(ServerDummyPosition? dummy)
  {
    if (dummy == null || _serverMonsterListView == null)
    {
      return false;
    }

    for (int index = 0; index < _serverMonsterListView.Items.Count; ++index)
    {
      if (_serverMonsterListView.Items[index].Tag is not ServerMonsterBlockInspectorItem blockItem)
      {
        continue;
      }

      if (!blockItem.Block.Dummies.Any(dummyRef => ReferenceEquals(dummyRef.Dummy, dummy)))
      {
        continue;
      }

      SetServerInspectorMode(ServerInspectorMode.Monsters, syncComboBox: true);
      ClearListViewSelection(_serverMonsterListView);
      ListViewItem blockListItem = _serverMonsterListView.Items[index];
      blockListItem.Selected = true;
      blockListItem.Focused = true;
      blockListItem.EnsureVisible();
      RefreshMonsterEditorSelection(dummy, preferredSpawn: null, selectFirstDummyWhenNeeded: false, selectFirstSpawnWhenNeeded: false);
      return true;
    }

    return false;
  }

  private ServerDummyPosition? GetSelectedMonsterNavigationDummy()
  {
    return GetSelectedMonsterDummyInspectorItem()?.Dummy ?? GetSelectedMonsterInspectorItem()?.Dummy;
  }

  private static ListViewItem? FindServerInspectorListItemByDummy(ListView listView, ServerDummyPosition? dummy)
  {
    if (dummy == null)
    {
      return null;
    }

    foreach (ListViewItem item in listView.Items)
    {
      if (item.Tag is IServerInspectorItem inspectorItem && ReferenceEquals(inspectorItem.Dummy, dummy))
      {
        return item;
      }
    }

    return null;
  }

  private static void ClearListViewSelection(ListView listView)
  {
    foreach (ListViewItem item in listView.Items)
    {
      item.Selected = false;
    }
  }

  private void WireUiEvents()
  {
    _openMapFolderMenuItem.Click += (_, _) => OpenMapRootFromDialog();
    _openServerFolderMenuItem.Click += (_, _) => OpenServerRootFromDialog();
    _saveServerDataMenuItem.Click += (_, _) => SaveServerDataFromUi();
    _saveEbpOnlyMenuItem.Click += (_, _) => SaveCurrentEbpOnlyFromDialog();
    _saveMapAsMenuItem.Click += (_, _) => SaveCurrentMapAsFromDialog();
    _exportBlenderPackageMenuItem.Click += (_, _) => ExportBlenderPackageFromDialog();
    _importBlenderPackageMenuItem.Click += (_, _) => ImportBlenderPackageFromDialog();
    _exitMenuItem.Click += (_, _) => Close();
    _controlsMenuItem.Click += (_, _) => ShowControls();
    _showSpeedStripMenuItem.CheckedChanged += (_, _) => _speedStrip.Visible = _showSpeedStripMenuItem.Checked;
    _showCollisionStripMenuItem.CheckedChanged += (_, _) => _collisionStrip.Visible = _showCollisionStripMenuItem.Checked;
    _showBspEditorStripMenuItem.CheckedChanged += (_, _) => _bspEditorStrip.Visible = _showBspEditorStripMenuItem.Checked;
    _strictLoadModeMenuItem.CheckedChanged += (_, _) => OnStrictLoadModeChanged();

    _undoMenuItem.Click += (_, _) => UndoLastCollisionEdit();
    _redoMenuItem.Click += (_, _) => RedoLastCollisionEdit();
    _editMapMaterialsMenuItem.Click += (_, _) => EditMapMaterialsFromDialog();
    _editMapTexturesMenuItem.Click += (_, _) => EditMapTexturesFromDialog();
    _editEnvironmentMenuItem.Click += (_, _) => EditEnvironmentFromDialog();
    _analyzeCollisionLeakJointsMenuItem.Click += (_, _) => AnalyzeCollisionLeakJointsFromUi();
    _deleteSelectedCollisionMenuItem.Click += (_, _) => DeleteSelectionFromActiveModeFromUi();

    _goToCoordinateButton.Click += (_, _) => TeleportCameraToInputCoordinate();
    _teleportXUpDown.KeyDown += OnTeleportInputKeyDown;
    _teleportYUpDown.KeyDown += OnTeleportInputKeyDown;
    _teleportZUpDown.KeyDown += OnTeleportInputKeyDown;
    _mapComboBox.SelectedIndexChanged += (_, _) => OnMapSelectionChanged();
    _skyMenuItem.Click += (_, _) => SetSkySourceMode(SkySourceMode.Sky, reloadCurrentMap: true);
    _sky2MenuItem.Click += (_, _) => SetSkySourceMode(SkySourceMode.Sky2, reloadCurrentMap: true);
    _legacyPipelineMenuItem.Click += (_, _) => SetRenderPipelineMode(RenderPipelineMode.LegacyCompat, invalidateViewer: true);
    _parityPipelineMenuItem.Click += (_, _) => SetRenderPipelineMode(RenderPipelineMode.R3Parity, invalidateViewer: true);

    _deleteSelectedCollisionButton.Click += (_, _) => DeleteSelectionFromActiveModeFromUi();
    _undoCollisionButton.Click += (_, _) => UndoLastCollisionEdit();
    _redoCollisionButton.Click += (_, _) => RedoLastCollisionEdit();
    _resetEditedCollisionButton.Click += (_, _) => ReloadCurrentMapFromDisk();

    _bspDeleteButton.Click += (_, _) => DeleteSelectedBspMeshFromUi();
    _bspCopyButton.Click += (_, _) => DuplicateSelectedBspMeshFromUi();
    _bspMergeButton.Click += (_, _) => MergeSelectedBspMeshFromUi();
    _bspMoveNegXButton.Click += (_, _) => MoveSelectedBspMeshAxis(-1f, 0f, 0f);
    _bspMovePosXButton.Click += (_, _) => MoveSelectedBspMeshAxis(1f, 0f, 0f);
    _bspMoveNegYButton.Click += (_, _) => MoveSelectedBspMeshAxis(0f, -1f, 0f);
    _bspMovePosYButton.Click += (_, _) => MoveSelectedBspMeshAxis(0f, 1f, 0f);
    _bspMoveNegZButton.Click += (_, _) => MoveSelectedBspMeshAxis(0f, 0f, -1f);
    _bspMovePosZButton.Click += (_, _) => MoveSelectedBspMeshAxis(0f, 0f, 1f);
    _bspScaleDownButton.Click += (_, _) => ScaleSelectedBspMeshFromUi(1.0f - BspScaleStepPercent / 100.0f);
    _bspScaleUpButton.Click += (_, _) => ScaleSelectedBspMeshFromUi(1.0f + BspScaleStepPercent / 100.0f);
    _bspRotateNegYButton.Click += (_, _) => RotateSelectedBspMeshFromUi(-BspRotateStepDegrees);
    _bspRotatePosYButton.Click += (_, _) => RotateSelectedBspMeshFromUi(BspRotateStepDegrees);
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

    _mouseDrawCollisionButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.CollisionDraw);
    _selectCollisionButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.CollisionSelect);
    _selectDummyButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.DummySelect);
    _bspSelectModeButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.BspSelect);
    _bspMoveModeButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.BspMove);
    _bspScaleModeButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.BspScale);
    _bspRotateModeButton.Click += (_, _) => ActivatePrimaryToolMode(PrimaryToolMode.BspRotate);
    _mouseDrawCollisionButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _selectCollisionButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _selectDummyButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _bspSelectModeButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _bspMoveModeButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _bspScaleModeButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _bspRotateModeButton.CheckedChanged += (_, _) => SyncToolModesFromUi();
    _bspModeObjectButton.Click += (_, _) => SetBspSelectionMode(MapViewerControl.BspSelectionMode.Object);
    _bspModeFaceButton.Click += (_, _) => SetBspSelectionMode(MapViewerControl.BspSelectionMode.Face);
    _bspModeEdgeButton.Click += (_, _) => SetBspSelectionMode(MapViewerControl.BspSelectionMode.Edge);
    _bspModeVertexButton.Click += (_, _) => SetBspSelectionMode(MapViewerControl.BspSelectionMode.Vertex);
    SyncToolModesFromUi();
    SetBspSelectionMode(MapViewerControl.BspSelectionMode.Object);

    _viewer.CollisionDrawPreviewHeight = (float)_boundaryHeightUpDown.Value;
    _viewer.CollisionDrawEmbedDepth = (float)_boundaryEmbedDepthUpDown.Value;
    _viewer.CollisionDrawSnapDistance = FixedSnapDistance;
    _boundaryMarginUpDown.ValueChanged += (_, _) => ApplySelectedCollisionLineEditsFromBoundaryInputs();
    _boundaryHeightUpDown.ValueChanged += (_, _) => _viewer.CollisionDrawPreviewHeight = (float)_boundaryHeightUpDown.Value;
    _boundaryHeightUpDown.ValueChanged += (_, _) => ApplySelectedCollisionLineEditsFromBoundaryInputs();
    _boundaryEmbedDepthUpDown.ValueChanged += (_, _) => _viewer.CollisionDrawEmbedDepth = (float)_boundaryEmbedDepthUpDown.Value;
    _boundaryEmbedDepthUpDown.ValueChanged += (_, _) => ApplySelectedCollisionLineEditsFromBoundaryInputs();

    _viewer.CollisionWallDrawRequested += OnViewerCollisionWallDrawRequested;
    _viewer.CollisionLineSelectionChanged += OnViewerCollisionLineSelectionChanged;
    _viewer.BspSelectionChanged += OnViewerBspSelectionChanged;
    _viewer.BspTranslateRequested += OnViewerBspTranslateRequested;
    _viewer.BspScaleRequested += OnViewerBspScaleRequested;
    _viewer.BspRotateAxisRequested += OnViewerBspRotateAxisRequested;
    _viewer.ServerDummySelectionChanged += OnViewerServerDummySelectionChanged;
    _viewer.ServerDummyMoved += OnViewerServerDummyMoved;
    _northSouthFlipButton.CheckedChanged += (_, _) =>
    {
      _viewer.FlipNorthSouth = _northSouthFlipButton.Checked;
      _viewer.Invalidate();
      SyncTeleportInputsFromCamera();
    };
  }

  private enum ActiveDeleteMode
  {
    None = 0,
    Collision = 1,
    Bsp = 2,
  }

  private enum PrimaryToolMode
  {
    CollisionDraw = 0,
    CollisionSelect = 1,
    DummySelect = 2,
    BspSelect = 3,
    BspMove = 4,
    BspScale = 5,
    BspRotate = 6,
  }

  private void SyncToolModesFromUi()
  {
    if (_suppressToolModeSync)
    {
      return;
    }

    _suppressToolModeSync = true;
    try
    {
      if (_mouseDrawCollisionButton.Checked)
      {
        _selectCollisionButton.Checked = false;
        _selectDummyButton.Checked = false;
        _bspSelectModeButton.Checked = false;
        _bspMoveModeButton.Checked = false;
        _bspScaleModeButton.Checked = false;
        _bspRotateModeButton.Checked = false;
      }
      else if (_selectCollisionButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
        _selectDummyButton.Checked = false;
        _bspSelectModeButton.Checked = false;
        _bspMoveModeButton.Checked = false;
        _bspScaleModeButton.Checked = false;
        _bspRotateModeButton.Checked = false;
      }
      else if (_selectDummyButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
        _selectCollisionButton.Checked = false;
        _bspSelectModeButton.Checked = false;
        _bspMoveModeButton.Checked = false;
        _bspScaleModeButton.Checked = false;
        _bspRotateModeButton.Checked = false;
      }
      else if (_bspMoveModeButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
        _selectCollisionButton.Checked = false;
        _selectDummyButton.Checked = false;
        _bspScaleModeButton.Checked = false;
        _bspRotateModeButton.Checked = false;
        _bspSelectModeButton.Checked = true;
      }
      else if (_bspScaleModeButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
        _selectCollisionButton.Checked = false;
        _selectDummyButton.Checked = false;
        _bspMoveModeButton.Checked = false;
        _bspRotateModeButton.Checked = false;
        _bspSelectModeButton.Checked = true;
      }
      else if (_bspRotateModeButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
        _selectCollisionButton.Checked = false;
        _selectDummyButton.Checked = false;
        _bspMoveModeButton.Checked = false;
        _bspScaleModeButton.Checked = false;
        _bspSelectModeButton.Checked = true;
      }
      else if (_bspSelectModeButton.Checked)
      {
        _mouseDrawCollisionButton.Checked = false;
        _selectCollisionButton.Checked = false;
        _selectDummyButton.Checked = false;
        _bspMoveModeButton.Checked = false;
        _bspScaleModeButton.Checked = false;
        _bspRotateModeButton.Checked = false;
      }
    }
    finally
    {
      _suppressToolModeSync = false;
    }

    _viewer.CollisionDrawModeEnabled = _mouseDrawCollisionButton.Checked;
    _viewer.CollisionSelectModeEnabled = _selectCollisionButton.Checked;
    _viewer.BspSelectModeEnabled = _bspSelectModeButton.Checked;
    _viewer.BspMoveModeEnabled = _bspMoveModeButton.Checked;
    _viewer.BspScaleModeEnabled = _bspScaleModeButton.Checked;
    _viewer.BspRotateModeEnabled = _bspRotateModeButton.Checked;
    UpdateViewerServerInteraction();
    if (!_bspSelectModeButton.Checked &&
        !_bspMoveModeButton.Checked &&
        !_bspScaleModeButton.Checked &&
        !_bspRotateModeButton.Checked)
    {
      _viewer.ClearSelectedBspSelection();
    }

    _viewer.Focus();
    UpdateUndoUiState();
  }

  private void ActivatePrimaryToolMode(PrimaryToolMode mode)
  {
    if (_suppressToolModeSync)
    {
      return;
    }

    _suppressToolModeSync = true;
    try
    {
      _mouseDrawCollisionButton.Checked = mode == PrimaryToolMode.CollisionDraw;
      _selectCollisionButton.Checked = mode == PrimaryToolMode.CollisionSelect;
      _selectDummyButton.Checked = mode == PrimaryToolMode.DummySelect && _selectDummyButton.Enabled;
      bool isBspTool = mode == PrimaryToolMode.BspSelect
        || mode == PrimaryToolMode.BspMove
        || mode == PrimaryToolMode.BspScale
        || mode == PrimaryToolMode.BspRotate;
      _bspSelectModeButton.Checked = isBspTool;
      _bspMoveModeButton.Checked = mode == PrimaryToolMode.BspMove;
      _bspScaleModeButton.Checked = mode == PrimaryToolMode.BspScale;
      _bspRotateModeButton.Checked = mode == PrimaryToolMode.BspRotate;
    }
    finally
    {
      _suppressToolModeSync = false;
    }

    SyncToolModesFromUi();
  }

  private void SetBspSelectionMode(MapViewerControl.BspSelectionMode mode)
  {
    if (_suppressBspSelectionModeSync)
    {
      return;
    }

    _suppressBspSelectionModeSync = true;
    try
    {
      _bspModeObjectButton.Checked = mode == MapViewerControl.BspSelectionMode.Object;
      _bspModeFaceButton.Checked = mode == MapViewerControl.BspSelectionMode.Face;
      _bspModeEdgeButton.Checked = mode == MapViewerControl.BspSelectionMode.Edge;
      _bspModeVertexButton.Checked = mode == MapViewerControl.BspSelectionMode.Vertex;
    }
    finally
    {
      _suppressBspSelectionModeSync = false;
    }

    _viewer.ActiveBspSelectionMode = mode;
    _viewer.Focus();
  }

  private void OnViewerBspTranslateRequested(Vector3 sourceDelta)
  {
    if (!float.IsFinite(sourceDelta.X) || !float.IsFinite(sourceDelta.Y) || !float.IsFinite(sourceDelta.Z))
    {
      return;
    }

    if (sourceDelta.LengthSquared <= 0.0001f)
    {
      return;
    }

    TranslateSelectedBspMeshFromUi(sourceDelta);
  }

  private void OnViewerBspScaleRequested(float uniformScale)
  {
    if (!float.IsFinite(uniformScale))
    {
      return;
    }

    ScaleSelectedBspMeshFromUi(uniformScale);
  }

  private void OnViewerBspRotateAxisRequested(Vector3 axisSource, float angleDegrees)
  {
    if (!float.IsFinite(angleDegrees))
    {
      return;
    }

    if (!float.IsFinite(axisSource.X) || !float.IsFinite(axisSource.Y) || !float.IsFinite(axisSource.Z))
    {
      return;
    }

    if (axisSource.LengthSquared <= 0.000001f)
    {
      return;
    }

    RotateSelectedBspMeshFromUi(Vector3.Normalize(axisSource), angleDegrees);
  }

  private ActiveDeleteMode GetActiveDeleteMode()
  {
    if (_bspMoveModeButton.Checked ||
        _bspScaleModeButton.Checked ||
        _bspRotateModeButton.Checked ||
        _bspSelectModeButton.Checked)
    {
      return ActiveDeleteMode.Bsp;
    }

    if (_selectCollisionButton.Checked)
    {
      return ActiveDeleteMode.Collision;
    }

    return ActiveDeleteMode.None;
  }

  private void DeleteSelectionFromActiveModeFromUi()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Delete Selection", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    ActiveDeleteMode mode = GetActiveDeleteMode();
    switch (mode)
    {
      case ActiveDeleteMode.Bsp:
      {
        bool hasBspSelection = _viewer.HasAnyBspSelection;
        if (!hasBspSelection)
        {
          _statusBaseText = "Delete skipped: BSP mode is active, but no BSP object/face/edge/vertex is selected.";
          RefreshRuntimeStatus();
          return;
        }

        DeleteSelectedBspMeshFromUi();
        return;
      }
      case ActiveDeleteMode.Collision:
      {
        bool hasCollisionSelection =
          _viewer.SelectedCollisionLineIndex >= 0 &&
          _viewer.SelectedCollisionLineIndex < _loadedMap.CollisionLines.Length;
        if (!hasCollisionSelection)
        {
          _statusBaseText = "Delete skipped: collision mode is active, but no collision segment is selected.";
          RefreshRuntimeStatus();
          return;
        }

        DeleteSelectedCollisionLineFromUi();
        return;
      }
      default:
        _statusBaseText = "Delete skipped: no selectable edit mode active (use SelWall or BSP Select/BSP Move).";
        RefreshRuntimeStatus();
        return;
    }
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

  private void OpenServerRootFromDialog()
  {
    using FolderBrowserDialog dialog = new()
    {
      Description = "Select WorldServer RF_Bin folder (contains Map and Script)",
      UseDescriptionForTitle = true,
      ShowNewFolderButton = false,
    };

    string? initialPath = GetInitialServerRootPath();
    if (!string.IsNullOrWhiteSpace(initialPath) && Directory.Exists(initialPath))
    {
      dialog.SelectedPath = initialPath;
    }

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    LoadServerRoot(dialog.SelectedPath, showMessageOnSuccess: true);
  }

  private string? GetInitialServerRootPath()
  {
    if (!string.IsNullOrWhiteSpace(_serverRootPath) && Directory.Exists(_serverRootPath))
    {
      return _serverRootPath;
    }

    if (_loadedBspPath != null)
    {
      string? mapRoot = TryGetMapRootFromBspPath(_loadedBspPath);
      if (!string.IsNullOrWhiteSpace(mapRoot))
      {
        DirectoryInfo? parent = Directory.GetParent(mapRoot);
        if (parent != null && parent.Name.Equals("RF_Bin", StringComparison.OrdinalIgnoreCase))
        {
          return parent.FullName;
        }
      }
    }

    string candidate = Path.Combine(Environment.CurrentDirectory, "RF_Bin");
    if (Directory.Exists(candidate))
    {
      return candidate;
    }

    if (Directory.Exists(DefaultServerRootPath))
    {
      return DefaultServerRootPath;
    }

    return null;
  }

  private void LoadServerRoot(string serverRootPath, bool showMessageOnSuccess)
  {
    string fullRoot = Path.GetFullPath(serverRootPath);
    if (!Directory.Exists(fullRoot))
    {
      MessageBox.Show(this, $"Server root folder not found:{Environment.NewLine}{fullRoot}", "Load Server Folder", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    _serverRootPath = NormalizeDirectoryPath(fullRoot);
    if (_loadedMap == null || string.IsNullOrWhiteSpace(_loadedBspPath))
    {
      _statusBaseText = $"Server folder set: {_serverRootPath}";
      RefreshRuntimeStatus();
      RefreshServerInspector();
      return;
    }

    if (TryApplyServerDataToCurrentMap(showErrorDialog: true, out string summary) && showMessageOnSuccess)
    {
      MessageBox.Show(this, summary, "Load Server Folder", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }
  }

  private bool TryApplyServerDataToCurrentMap(bool showErrorDialog, out string summary)
  {
    summary = string.Empty;
    if (_loadedMap == null || string.IsNullOrWhiteSpace(_loadedBspPath) || string.IsNullOrWhiteSpace(_serverRootPath))
    {
      return false;
    }

    try
    {
      ServerMapData serverData = ServerMapLoader.Load(_loadedMap, _loadedBspPath, _serverRootPath);
      _loadedMap = MapEditOperations.WithServerData(_loadedMap, serverData);
      _mapDocument = MapDocument.FromLoadedMap(_loadedMap);
      _viewer.ApplyServerDataEditedMap(_loadedMap);
      _statusBaseText = BuildLoadedMapStatus(_loadedMap, _lastEntityArchiveCache, null);
      RefreshRuntimeStatus();
      SetServerInspectorMode(ServerInspectorMode.Monsters, syncComboBox: true);
      RefreshServerInspector();
      summary =
        $"Loaded server data for {_loadedMap.Name}{Environment.NewLine}{Environment.NewLine}"
        + $"Monster definitions: {serverData.MonsterDefinitions.Length:N0}{Environment.NewLine}"
        + $"Monster blocks: {serverData.MonsterBlocks.Length:N0}{Environment.NewLine}"
        + $"Monster spawns: {serverData.MonsterSpawnCount:N0}{Environment.NewLine}"
        + $"Portals: {serverData.Portals.Length:N0}{Environment.NewLine}"
        + $"Stores: {serverData.Stores.Length:N0}{Environment.NewLine}"
        + $"Start points: {serverData.StartPoints.Length:N0}{Environment.NewLine}"
        + $"Resources: {serverData.ResourceNodes.Length:N0}";
      return true;
    }
    catch (Exception ex)
    {
      _loadedMap = MapEditOperations.WithServerData(_loadedMap, null);
      _mapDocument = MapDocument.FromLoadedMap(_loadedMap);
      _viewer.ApplyServerDataEditedMap(_loadedMap);
      _statusBaseText = BuildLoadedMapStatus(_loadedMap, _lastEntityArchiveCache, $"ServerLoad: {ex.Message}");
      RefreshRuntimeStatus();
      SetServerInspectorMode(ServerInspectorMode.Summary, syncComboBox: true);
      RefreshServerInspector();
      if (showErrorDialog)
      {
        MessageBox.Show(this, ex.Message, "Load Server Folder Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }

      return false;
    }
  }

  private void RefreshServerInspector()
  {
    if (_serverSummaryListView == null)
    {
      return;
    }

    ServerMapData? serverData = _loadedMap?.ServerData;
    UpdateDummySelectionToolState();
    UpdateServerInspectorDockVisibility();

    if (serverData != null)
    {
      _serverInspectorSummaryLabel.Text =
        $"RF_Bin: {serverData.ServerRootPath}{Environment.NewLine}"
        + $"Map: {serverData.MapDefinition.FileName} ({serverData.MapDefinition.Code})";
    }
    else if (!string.IsNullOrWhiteSpace(_serverRootPath) && _loadedMap != null)
    {
      _serverInspectorSummaryLabel.Text =
        $"RF_Bin: {_serverRootPath}{Environment.NewLine}"
        + "Current map has no attached server-side data.";
    }
    else if (!string.IsNullOrWhiteSpace(_serverRootPath))
    {
      _serverInspectorSummaryLabel.Text =
        $"RF_Bin: {_serverRootPath}{Environment.NewLine}"
        + "Open a map to inspect server-side records.";
    }
    else
    {
      _serverInspectorSummaryLabel.Text = "Load File > Load > Server Folder to inspect server-side map data.";
    }

    PopulateServerSummaryList(serverData);
    PopulateMonsterInspectorList(serverData);
    PopulatePortalInspectorList(serverData);
    PopulateStoreInspectorList(serverData);
    PopulateStartInspectorList(serverData);
    PopulateResourceInspectorList(serverData);
    RefreshMonsterEditorSelection(preferredDummy: null, preferredSpawn: null, selectFirstDummyWhenNeeded: false, selectFirstSpawnWhenNeeded: false);
    UpdateServerOverlayModeFromInspector();
  }

  private void PopulateServerSummaryList(ServerMapData? serverData)
  {
    _serverSummaryListView.BeginUpdate();
    try
    {
      _serverSummaryListView.Items.Clear();
      if (serverData == null)
      {
        AddServerListItem(_serverSummaryListView, null, "Server Root", _serverRootPath ?? "Not loaded");
        AddServerListItem(_serverSummaryListView, null, "Map", _loadedMap?.Name ?? "No map loaded");
        AddServerListItem(_serverSummaryListView, null, "Status", "No server-side map data attached");
        return;
      }

      AddServerListItem(_serverSummaryListView, null, "Server Root", serverData.ServerRootPath);
      AddServerListItem(_serverSummaryListView, null, "Map", $"{serverData.MapDefinition.FileName} ({serverData.MapDefinition.Code})");
      AddServerListItem(_serverSummaryListView, null, "Script", serverData.ScriptFolderPath);
      AddServerListItem(_serverSummaryListView, null, "Monster Def", serverData.MonsterDefinitions.Length.ToString("N0"));
      AddServerListItem(_serverSummaryListView, null, "Monster Block", serverData.MonsterBlocks.Length.ToString("N0"));
      AddServerListItem(_serverSummaryListView, null, "Monster Spawn", serverData.MonsterSpawnCount.ToString("N0"));
      AddServerListItem(_serverSummaryListView, null, "Portal", serverData.Portals.Length.ToString("N0"));
      AddServerListItem(_serverSummaryListView, null, "Store", serverData.Stores.Length.ToString("N0"));
      AddServerListItem(_serverSummaryListView, null, "Start", serverData.StartPoints.Length.ToString("N0"));
      AddServerListItem(_serverSummaryListView, null, "Resource", serverData.ResourceNodes.Length.ToString("N0"));
    }
    finally
    {
      _serverSummaryListView.EndUpdate();
    }
  }

  private void PopulateMonsterInspectorList(ServerMapData? serverData)
  {
    string? previouslySelectedBlockCode = GetSelectedMonsterInspectorItem()?.Block.Code;
    _serverMonsterListView.BeginUpdate();
    try
    {
      _serverMonsterListView.Items.Clear();
      if (serverData == null)
      {
        return;
      }

      string filter = _serverMonsterSearchText.Trim();
      for (int blockIndex = 0; blockIndex < serverData.MonsterBlocks.Length; ++blockIndex)
      {
        ServerMapMonsterBlock block = serverData.MonsterBlocks[blockIndex];
        if (block.Dummies.Length == 0)
        {
          continue;
        }

        List<string> monsterCodes = new();
        List<string> monsterNames = new();
        int spawnCount = 0;
        ulong totalRegenLimit = 0;
        bool matchesFilter = string.IsNullOrWhiteSpace(filter);
        for (int setIndex = 0; setIndex < block.SpawnSets.Length; ++setIndex)
        {
          ServerMapMonsterSpawnSet spawnSet = block.SpawnSets[setIndex];
          for (int spawnIndex = 0; spawnIndex < spawnSet.Spawns.Length; ++spawnIndex)
          {
            ServerMapMonsterSpawn spawn = spawnSet.Spawns[spawnIndex];
            ++spawnCount;
            totalRegenLimit += spawn.RegenLimit;
            if (!monsterCodes.Contains(spawn.MonsterCode, StringComparer.OrdinalIgnoreCase))
            {
              monsterCodes.Add(spawn.MonsterCode);
            }

            if (!string.IsNullOrWhiteSpace(spawn.MonsterName) &&
                !monsterNames.Contains(spawn.MonsterName, StringComparer.OrdinalIgnoreCase))
            {
              monsterNames.Add(spawn.MonsterName);
            }

            if (!matchesFilter &&
                (spawn.MonsterCode.IndexOf(filter, StringComparison.OrdinalIgnoreCase) >= 0 ||
                 spawn.MonsterName.IndexOf(filter, StringComparison.OrdinalIgnoreCase) >= 0))
            {
              matchesFilter = true;
            }
          }
        }

        if (!matchesFilter)
        {
          continue;
        }

        string monsterCodeSummary = BuildMonsterSummary(monsterCodes, monsterCodes);
        string monsterSummary = BuildMonsterSummary(monsterNames, monsterCodes);
        ServerDummyPosition primaryDummy = block.Dummies[0].Dummy;
        ServerMonsterBlockInspectorItem monsterItem = new()
        {
          Label = $"monster block {block.Code}",
          Dummy = primaryDummy,
          Block = block,
          MonsterSummary = monsterSummary,
          MonsterCodes = monsterCodes.ToArray(),
          MonsterNames = monsterNames.ToArray(),
          SpawnCount = spawnCount,
          TotalRegenLimit = totalRegenLimit,
          SetCount = block.SpawnSets.Length,
        };
        ListViewItem listItem = AddServerListItem(
          _serverMonsterListView,
          monsterItem,
          block.Code,
          monsterCodeSummary,
          monsterSummary,
          block.Dummies.Length.ToString(),
          spawnCount.ToString(),
          totalRegenLimit.ToString("N0"),
          FormatSourcePosition(block.WorldCenter));
        if (!string.IsNullOrWhiteSpace(previouslySelectedBlockCode) &&
            string.Equals(block.Code, previouslySelectedBlockCode, StringComparison.OrdinalIgnoreCase))
        {
          listItem.Selected = true;
          listItem.Focused = true;
        }
      }
    }
    finally
    {
      _serverMonsterListView.EndUpdate();
    }
  }

  private void OnServerMonsterSearchTextChanged(object? sender, EventArgs e)
  {
    _serverMonsterSearchText = _serverMonsterSearchTextBox.Text;
    PopulateMonsterInspectorList(_loadedMap?.ServerData);
    RefreshMonsterEditorSelection(preferredDummy: null, preferredSpawn: null, selectFirstDummyWhenNeeded: true, selectFirstSpawnWhenNeeded: true);
  }

  private void OnServerMonsterDefinitionSearchTextChanged(object? sender, EventArgs e)
  {
    _serverMonsterDefinitionSearchText = _serverMonsterDefinitionSearchTextBox.Text;
    PopulateMonsterDefinitionComboBox(_loadedMap?.ServerData);
    UpdateMonsterEditorActions();
  }

  private void OnServerMonsterDefinitionComboBoxSelectedIndexChanged(object? sender, EventArgs e)
  {
    if (_suppressMonsterDefinitionSelectionChanged)
    {
      return;
    }

    UpdateMonsterEditorActions();
  }

  private void OnServerMonsterListSelectionChanged(object? sender, EventArgs e)
  {
    RefreshMonsterEditorSelection(preferredDummy: null, preferredSpawn: null, selectFirstDummyWhenNeeded: true, selectFirstSpawnWhenNeeded: true);
    if (!_suppressServerSelectionSync)
    {
      _viewer.SetSelectedServerDummy(GetSelectedMonsterNavigationDummy());
      UpdateServerInspectorActions();
    }
  }

  private void OnServerMonsterDummyListSelectionChanged(object? sender, EventArgs e)
  {
    UpdateMonsterInspectorSelectionDetails();
  }

  private void OnServerMonsterSpawnListSelectionChanged(object? sender, EventArgs e)
  {
    UpdateMonsterInspectorSelectionDetails();
  }

  private void RefreshMonsterEditorSelection(
    ServerDummyPosition? preferredDummy,
    ServerMapMonsterSpawn? preferredSpawn,
    bool selectFirstDummyWhenNeeded,
    bool selectFirstSpawnWhenNeeded)
  {
    ServerMonsterDummyInspectorItem? currentDummyItem = GetSelectedMonsterDummyInspectorItem();
    ServerMonsterSpawnInspectorItem? currentSpawnItem = GetSelectedMonsterSpawnItem();
    ServerDummyPosition? dummyToSelect = preferredDummy ?? currentDummyItem?.Dummy;
    ServerMapMonsterSpawn? spawnToSelect = preferredSpawn ?? currentSpawnItem?.Spawn;
    int preferredSetIndex = currentSpawnItem?.SetIndex ?? GetSelectedMonsterSpawnSetIndex();
    ServerMonsterBlockInspectorItem? monsterItem = GetSelectedMonsterInspectorItem();
    PopulateMonsterDummyList(monsterItem, dummyToSelect, selectFirstDummyWhenNeeded);
    PopulateMonsterSpawnSetComboBox(_loadedMap?.ServerData, monsterItem?.Block, preferredSetIndex);
    PopulateMonsterSpawnList(monsterItem, spawnToSelect, selectFirstSpawnWhenNeeded);
    UpdateMonsterInspectorSelectionDetails();
  }

  private void UpdateMonsterInspectorSelectionDetails()
  {
    if (_serverMonsterDetailLabel == null || _serverMonsterBlockLabel == null)
    {
      return;
    }

    ServerMapData? serverData = _loadedMap?.ServerData;
    if (serverData == null)
    {
      PopulateMonsterDefinitionComboBox(null);
      PopulateMonsterDummyList(null, null, selectFirstWhenNeeded: false);
      PopulateMonsterSpawnSetComboBox(null, null, 0);
      PopulateMonsterSpawnList(null, null, selectFirstWhenNeeded: false);
      _serverMonsterBlockLabel.Text = "Load a server folder to inspect monster spawns.";
      _serverMonsterDetailLabel.Text = "Load a server folder to inspect monster spawns.";
      UpdateMonsterEditorActions();
      return;
    }

    PopulateMonsterDefinitionComboBox(serverData);

    ServerMonsterBlockInspectorItem? monsterItem = GetSelectedMonsterInspectorItem();
    ServerMonsterDummyInspectorItem? dummyItem = GetSelectedMonsterDummyInspectorItem();
    ServerMonsterSpawnInspectorItem? spawnItem = GetSelectedMonsterSpawnItem();
    if (monsterItem == null)
    {
      int visibleCount = _serverMonsterListView.Items.Count;
      _serverMonsterBlockLabel.Text =
        visibleCount > 0
          ? $"Showing {visibleCount:N0} monster blocks. Select one to edit its dummy boxes and spawn entries."
          : "No monster blocks match the current search.";
      _serverMonsterDetailLabel.Text = "Choose a monster definition, set, and interval, then click Add Monster to create a new monster block at the camera.";
      UpdateMonsterEditorActions();
      return;
    }

    _serverMonsterBlockLabel.Text =
      $"Block: {monsterItem.Block.Code} | Dummies: {monsterItem.Block.Dummies.Length:N0} | Sets: {monsterItem.Block.SpawnSets.Length:N0} | Entries: {monsterItem.SpawnCount:N0} | Limit: {monsterItem.TotalRegenLimit:N0}{Environment.NewLine}"
      + $"Center: {FormatSourcePosition(monsterItem.Block.WorldCenter)}";

    if (spawnItem != null)
    {
      _suppressMonsterSpawnIntervalValueChanged = true;
      try
      {
        decimal intervalValue = Math.Clamp((decimal)spawnItem.Spawn.RegenTime, _serverMonsterIntervalUpDown.Minimum, _serverMonsterIntervalUpDown.Maximum);
        if (_serverMonsterIntervalUpDown.Value != intervalValue)
        {
          _serverMonsterIntervalUpDown.Value = intervalValue;
        }
      }
      finally
      {
        _suppressMonsterSpawnIntervalValueChanged = false;
      }

      _serverMonsterDetailLabel.Text =
        $"Spawn Set: {spawnItem.SetIndex} | Monster: {spawnItem.Spawn.MonsterCode} ({spawnItem.Spawn.MonsterName}){Environment.NewLine}"
        + $"Interval: {spawnItem.Spawn.RegenTime:N0} | Limit: {spawnItem.Spawn.RegenLimit:N0} | Min: {spawnItem.Spawn.RegenMin:N0} | Max: {spawnItem.Spawn.RegenMax:N0}";
      UpdateMonsterEditorActions();
      return;
    }

    string codes = BuildMonsterSummary(monsterItem.MonsterCodes, monsterItem.MonsterCodes);
    string names = BuildMonsterSummary(monsterItem.MonsterNames, monsterItem.MonsterCodes);

    if (dummyItem != null)
    {
      Vector3 dummySize = dummyItem.Dummy.EditedWorldMax - dummyItem.Dummy.EditedWorldMin;
      _serverMonsterDetailLabel.Text =
        $"Dummy: {dummyItem.Dummy.Code} | Center: {FormatSourcePosition(dummyItem.Dummy.EditedWorldCenter)}{Environment.NewLine}"
        + $"Size: {dummySize.X:F1}, {dummySize.Y:F1}, {dummySize.Z:F1}{Environment.NewLine}"
        + "Move this dummy in the viewport or use Add Dummy to clone another spawn box into the same block.";
      UpdateMonsterEditorActions();
      return;
    }

    _serverMonsterDetailLabel.Text =
      $"Monster Code: {codes}{Environment.NewLine}"
      + $"Monsters: {names}{Environment.NewLine}"
      + $"Primary Dummy: {monsterItem.Dummy.Code}{Environment.NewLine}"
      + "Select a dummy row to focus one spawn box, or select a spawn row to edit interval.";
    UpdateMonsterEditorActions();
  }

  private void PopulateMonsterDummyList(ServerMonsterBlockInspectorItem? monsterItem, ServerDummyPosition? preferredDummy, bool selectFirstWhenNeeded)
  {
    if (_serverMonsterDummyListView == null)
    {
      return;
    }

    ListViewItem? itemToSelect = null;
    _serverMonsterDummyListView.BeginUpdate();
    try
    {
      _serverMonsterDummyListView.Items.Clear();
      if (monsterItem == null)
      {
        return;
      }

      for (int dummyIndex = 0; dummyIndex < monsterItem.Block.Dummies.Length; ++dummyIndex)
      {
        ServerMapMonsterDummyRef dummyRef = monsterItem.Block.Dummies[dummyIndex];
        ServerDummyPosition dummy = dummyRef.Dummy;
        Vector3 dummySize = dummy.EditedWorldMax - dummy.EditedWorldMin;
        ServerMonsterDummyInspectorItem dummyItem = new()
        {
          Label = $"monster dummy {dummy.Code}",
          Dummy = dummy,
          DummyRef = dummyRef,
          Block = monsterItem.Block,
        };
        ListViewItem listViewItem = AddServerListItem(
          _serverMonsterDummyListView,
          dummyItem,
          dummy.Code,
          FormatSourcePosition(dummy.EditedWorldCenter),
          $"{dummySize.X:F0},{dummySize.Y:F0},{dummySize.Z:F0}");
        if (preferredDummy != null && ReferenceEquals(preferredDummy, dummy))
        {
          itemToSelect = listViewItem;
        }
      }
    }
    finally
    {
      _serverMonsterDummyListView.EndUpdate();
    }

    if (itemToSelect == null && selectFirstWhenNeeded && _serverMonsterDummyListView.Items.Count > 0)
    {
      itemToSelect = _serverMonsterDummyListView.Items[0];
    }

    if (itemToSelect != null)
    {
      ClearListViewSelection(_serverMonsterDummyListView);
      itemToSelect.Selected = true;
      itemToSelect.Focused = true;
      itemToSelect.EnsureVisible();
    }
  }

  private void PopulateMonsterDefinitionComboBox(ServerMapData? serverData)
  {
    if (_serverMonsterDefinitionComboBox == null)
    {
      return;
    }

    string? previouslySelectedCode = GetSelectedMonsterDefinition()?.Code;
    string filter = _serverMonsterDefinitionSearchText.Trim();
    _suppressMonsterDefinitionSelectionChanged = true;
    try
    {
      _serverMonsterDefinitionComboBox.BeginUpdate();
      _serverMonsterDefinitionComboBox.Items.Clear();
      if (serverData == null)
      {
        return;
      }

      foreach (ServerMonsterDefinition definition in serverData.MonsterDefinitions)
      {
        if (!string.IsNullOrWhiteSpace(filter) &&
            definition.Code.IndexOf(filter, StringComparison.OrdinalIgnoreCase) < 0 &&
            definition.DisplayName.IndexOf(filter, StringComparison.OrdinalIgnoreCase) < 0)
        {
          continue;
        }

        _serverMonsterDefinitionComboBox.Items.Add(new ServerMonsterDefinitionComboItem
        {
          Definition = definition,
        });
      }

      if (_serverMonsterDefinitionComboBox.Items.Count <= 0)
      {
        return;
      }

      int selectedIndex = 0;
      if (!string.IsNullOrWhiteSpace(previouslySelectedCode))
      {
        for (int index = 0; index < _serverMonsterDefinitionComboBox.Items.Count; ++index)
        {
          if (_serverMonsterDefinitionComboBox.Items[index] is ServerMonsterDefinitionComboItem item &&
              string.Equals(item.Definition.Code, previouslySelectedCode, StringComparison.OrdinalIgnoreCase))
          {
            selectedIndex = index;
            break;
          }
        }
      }

      _serverMonsterDefinitionComboBox.SelectedIndex = selectedIndex;
    }
    finally
    {
      _serverMonsterDefinitionComboBox.EndUpdate();
      _suppressMonsterDefinitionSelectionChanged = false;
    }
  }

  private void PopulateMonsterSpawnSetComboBox(ServerMapData? serverData, ServerMapMonsterBlock? block, int preferredSetIndex)
  {
    if (_serverMonsterSpawnSetComboBox == null)
    {
      return;
    }

    int setCount = block?.SpawnSets.Length ?? Math.Max(1, serverData?.MapDefinition.MonsterSetFileCount ?? 1);
    preferredSetIndex = Math.Clamp(preferredSetIndex, 0, Math.Max(0, setCount - 1));
    _serverMonsterSpawnSetComboBox.BeginUpdate();
    try
    {
      _serverMonsterSpawnSetComboBox.Items.Clear();
      for (int setIndex = 0; setIndex < setCount; ++setIndex)
      {
        _serverMonsterSpawnSetComboBox.Items.Add(new MonsterSpawnSetComboItem(setIndex));
      }

      if (_serverMonsterSpawnSetComboBox.Items.Count > 0)
      {
        _serverMonsterSpawnSetComboBox.SelectedIndex = preferredSetIndex;
      }
    }
    finally
    {
      _serverMonsterSpawnSetComboBox.EndUpdate();
    }
  }

  private void PopulateMonsterSpawnList(ServerMonsterBlockInspectorItem? monsterItem, ServerMapMonsterSpawn? preferredSpawn, bool selectFirstWhenNeeded)
  {
    if (_serverMonsterSpawnListView == null)
    {
      return;
    }

    ListViewItem? itemToSelect = null;
    _serverMonsterSpawnListView.BeginUpdate();
    try
    {
      _serverMonsterSpawnListView.Items.Clear();
      if (monsterItem == null)
      {
        return;
      }

      for (int setIndex = 0; setIndex < monsterItem.Block.SpawnSets.Length; ++setIndex)
      {
        ServerMapMonsterSpawnSet spawnSet = monsterItem.Block.SpawnSets[setIndex];
        for (int spawnIndex = 0; spawnIndex < spawnSet.Spawns.Length; ++spawnIndex)
        {
          ServerMapMonsterSpawn spawn = spawnSet.Spawns[spawnIndex];
          ServerMonsterSpawnInspectorItem spawnItem = new()
          {
            Block = monsterItem.Block,
            SetIndex = spawnSet.SetIndex,
            Spawn = spawn,
          };
          ListViewItem listViewItem = AddServerListItem(
            _serverMonsterSpawnListView,
            spawnItem,
            spawnSet.SetIndex.ToString(),
            spawn.MonsterCode,
            spawn.MonsterName,
            spawn.RegenTime.ToString("N0"),
            spawn.RegenLimit.ToString("N0"),
            spawn.RegenMin.ToString("N0"),
            spawn.RegenMax.ToString("N0"));
          if (preferredSpawn != null && ReferenceEquals(preferredSpawn, spawn))
          {
            itemToSelect = listViewItem;
          }
        }
      }
    }
    finally
    {
      _serverMonsterSpawnListView.EndUpdate();
    }

    if (itemToSelect == null && selectFirstWhenNeeded && _serverMonsterSpawnListView.Items.Count > 0)
    {
      itemToSelect = _serverMonsterSpawnListView.Items[0];
    }

    if (itemToSelect != null)
    {
      ClearListViewSelection(_serverMonsterSpawnListView);
      itemToSelect.Selected = true;
      itemToSelect.Focused = true;
      itemToSelect.EnsureVisible();
    }
  }

  private void UpdateMonsterEditorActions()
  {
    bool hasServerData = _loadedMap?.ServerData != null;
    bool hasDefinition = GetSelectedMonsterDefinition() != null;
    bool hasMonsterSelection = GetSelectedMonsterInspectorItem() != null;
    bool hasSpawnSelection = GetSelectedMonsterSpawnItem() != null;

    if (_serverMonsterDefinitionSearchTextBox != null)
    {
      _serverMonsterDefinitionSearchTextBox.Enabled = hasServerData;
    }

    if (_serverMonsterDefinitionComboBox != null)
    {
      _serverMonsterDefinitionComboBox.Enabled = hasServerData && _serverMonsterDefinitionComboBox.Items.Count > 0;
    }

    if (_serverMonsterSpawnSetComboBox != null)
    {
      _serverMonsterSpawnSetComboBox.Enabled = hasServerData && _serverMonsterSpawnSetComboBox.Items.Count > 0;
    }

    if (_serverMonsterIntervalUpDown != null)
    {
      _serverMonsterIntervalUpDown.Enabled = hasServerData;
    }

    if (_serverMonsterAddBlockButton != null)
    {
      _serverMonsterAddBlockButton.Enabled = hasServerData && hasDefinition;
    }

    if (_serverMonsterAddDummyButton != null)
    {
      _serverMonsterAddDummyButton.Enabled = hasMonsterSelection;
    }

    if (_serverMonsterAddSpawnButton != null)
    {
      _serverMonsterAddSpawnButton.Enabled = hasMonsterSelection && hasDefinition;
    }

    if (_serverMonsterSpawnListView != null)
    {
      _serverMonsterSpawnListView.Enabled = hasMonsterSelection;
    }

    if (_serverMonsterDummyListView != null)
    {
      _serverMonsterDummyListView.Enabled = hasMonsterSelection;
    }

    if (!hasSpawnSelection && _serverMonsterIntervalUpDown != null && hasServerData && _serverMonsterIntervalUpDown.Value <= 0)
    {
      _serverMonsterIntervalUpDown.Value = 60;
    }
  }

  private void OnServerMonsterSpawnIntervalValueChanged(object? sender, EventArgs e)
  {
    if (_suppressMonsterSpawnIntervalValueChanged)
    {
      return;
    }

    ServerMonsterSpawnInspectorItem? spawnItem = GetSelectedMonsterSpawnItem();
    if (spawnItem == null)
    {
      return;
    }

    uint newInterval = (uint)_serverMonsterIntervalUpDown.Value;
    if (spawnItem.Spawn.RegenTime == newInterval)
    {
      return;
    }

    spawnItem.Spawn.RegenTime = newInterval;
    PopulateMonsterSpawnList(GetSelectedMonsterInspectorItem(), spawnItem.Spawn, selectFirstWhenNeeded: false);
    UpdateMonsterInspectorSelectionDetails();
    _statusBaseText = $"Updated spawn interval for {spawnItem.Spawn.MonsterCode} in block {spawnItem.Block.Code} to {newInterval:N0}.";
    RefreshRuntimeStatus();
  }

  private void AddMonsterBlockFromUi()
  {
    if (_loadedMap?.ServerData is not ServerMapData serverData)
    {
      return;
    }

    ServerMonsterDefinition? definition = GetSelectedMonsterDefinition();
    if (definition == null)
    {
      MessageBox.Show(this, "Pick a monster definition first.", "Add Monster", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    ServerMonsterBlockInspectorItem? templateItem = GetSelectedMonsterInspectorItem();
    ServerMonsterDummyInspectorItem? templateDummyItem = GetSelectedMonsterDummyInspectorItem();
    ServerMapMonsterSpawn? templateSpawn = GetSelectedMonsterSpawnItem()?.Spawn;
    if (templateSpawn == null && templateItem != null)
    {
      templateSpawn = templateItem.Block.SpawnSets.SelectMany(static set => set.Spawns).FirstOrDefault();
    }

    int newBlockRecordIndex = GetNextMonsterBlockRecordIndex(serverData);
    string newBlockCode = GenerateUniqueMonsterBlockCode(serverData);
    ServerDummyPosition newDummy = CreateMonsterDummyAtCamera(
      templateDummyItem?.Dummy ?? templateItem?.Dummy,
      GenerateUniqueMonsterDummyCode(serverData, newBlockCode));
    int targetSetIndex = GetSelectedMonsterSpawnSetIndex();
    ServerMapMonsterSpawn newSpawn = CreateMonsterSpawn(serverData, definition, (uint)_serverMonsterIntervalUpDown.Value, templateSpawn);
    int spawnSetCount = Math.Max(1, serverData.MapDefinition.MonsterSetFileCount);
    ServerMapMonsterSpawnSet[] spawnSets = new ServerMapMonsterSpawnSet[spawnSetCount];
    for (int setIndex = 0; setIndex < spawnSets.Length; ++setIndex)
    {
      spawnSets[setIndex] = new ServerMapMonsterSpawnSet
      {
        SetIndex = setIndex,
        Spawns = setIndex == Math.Clamp(targetSetIndex, 0, spawnSets.Length - 1)
          ? [newSpawn]
          : Array.Empty<ServerMapMonsterSpawn>(),
      };
    }

    ServerMapMonsterBlock newBlock = new()
    {
      RecordIndex = newBlockRecordIndex,
      DeclaredIndex = checked((uint)newBlockRecordIndex),
      Code = newBlockCode,
      MinCount = templateItem?.Block.MinCount ?? 1,
      MobCount = templateItem?.Block.MobCount ?? 1,
      MaxCount = templateItem?.Block.MaxCount ?? 1,
      WorldCenter = newDummy.EditedWorldCenter,
      Dummies =
      [
        new ServerMapMonsterDummyRef
        {
          DummyCode = newDummy.Code,
          SelectProp = templateDummyItem?.DummyRef.SelectProp ?? templateItem?.Block.Dummies.FirstOrDefault()?.SelectProp ?? 0u,
          Dummy = newDummy,
        }
      ],
      SpawnSets = spawnSets,
    };

    serverData.MonsterBlocks = [.. serverData.MonsterBlocks, newBlock];
    serverData.MonsterDummies = [.. serverData.MonsterDummies, newDummy];
    serverData.MonsterSpawnCount += 1;
    OnMonsterDataEdited(newDummy, newSpawn, $"Added monster block {newBlock.Code} with {definition.Code}.");
  }

  private void AddMonsterDummyFromUi()
  {
    if (_loadedMap?.ServerData is not ServerMapData serverData)
    {
      return;
    }

    ServerMonsterBlockInspectorItem? monsterItem = GetSelectedMonsterInspectorItem();
    if (monsterItem == null)
    {
      MessageBox.Show(this, "Select a monster block first.", "Add Dummy", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    ServerMonsterDummyInspectorItem? selectedDummyItem = GetSelectedMonsterDummyInspectorItem();
    ServerDummyPosition newDummy = CreateMonsterDummyAtCamera(
      selectedDummyItem?.Dummy ?? monsterItem.Dummy,
      GenerateUniqueMonsterDummyCode(serverData, monsterItem.Block.Code));
    ServerMapMonsterDummyRef newDummyRef = new()
    {
      DummyCode = newDummy.Code,
      SelectProp = selectedDummyItem?.DummyRef.SelectProp ?? monsterItem.Block.Dummies.FirstOrDefault()?.SelectProp ?? 0u,
      Dummy = newDummy,
    };

    monsterItem.Block.Dummies = [.. monsterItem.Block.Dummies, newDummyRef];
    monsterItem.Block.WorldCenter = ComputeMonsterBlockCenter(monsterItem.Block.Dummies);
    serverData.MonsterDummies = [.. serverData.MonsterDummies, newDummy];
    OnMonsterDataEdited(newDummy, null, $"Added dummy {newDummy.Code} to monster block {monsterItem.Block.Code}.");
  }

  private void AddMonsterSpawnFromUi()
  {
    if (_loadedMap?.ServerData is not ServerMapData serverData)
    {
      return;
    }

    ServerMonsterBlockInspectorItem? monsterItem = GetSelectedMonsterInspectorItem();
    if (monsterItem == null)
    {
      MessageBox.Show(this, "Select a monster block first.", "Spawn Monster", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    ServerMonsterDefinition? definition = GetSelectedMonsterDefinition();
    if (definition == null)
    {
      MessageBox.Show(this, "Pick a monster definition first.", "Spawn Monster", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    int targetSetIndex = Math.Clamp(GetSelectedMonsterSpawnSetIndex(), 0, Math.Max(0, monsterItem.Block.SpawnSets.Length - 1));
    ServerMapMonsterSpawn? templateSpawn = GetSelectedMonsterSpawnItem()?.Spawn;
    if (templateSpawn == null)
    {
      templateSpawn = monsterItem.Block.SpawnSets[targetSetIndex].Spawns.FirstOrDefault()
        ?? monsterItem.Block.SpawnSets.SelectMany(static set => set.Spawns).FirstOrDefault();
    }

    ServerMapMonsterSpawn newSpawn = CreateMonsterSpawn(serverData, definition, (uint)_serverMonsterIntervalUpDown.Value, templateSpawn);
    ServerMapMonsterSpawnSet targetSet = monsterItem.Block.SpawnSets[targetSetIndex];
    targetSet.Spawns = [.. targetSet.Spawns, newSpawn];
    serverData.MonsterSpawnCount += 1;
    OnMonsterDataEdited(GetSelectedMonsterNavigationDummy() ?? monsterItem.Dummy, newSpawn, $"Added spawn {definition.Code} to block {monsterItem.Block.Code} set {targetSet.SetIndex}.");
  }

  private void OnMonsterDataEdited(ServerDummyPosition? preferredDummy, ServerMapMonsterSpawn? preferredSpawn, string statusText)
  {
    _viewer.RefreshServerOverlay();
    RefreshServerInspector();
    if (preferredDummy != null)
    {
      _viewer.SetSelectedServerDummy(preferredDummy);
      SelectServerInspectorItem(preferredDummy);
    }

    if (preferredSpawn != null)
    {
      SelectMonsterSpawnInspectorItem(preferredSpawn);
    }

    _statusBaseText = statusText;
    RefreshRuntimeStatus();
  }

  private void SelectMonsterSpawnInspectorItem(ServerMapMonsterSpawn? spawn)
  {
    if (spawn == null || _serverMonsterSpawnListView == null)
    {
      return;
    }

    foreach (ListViewItem item in _serverMonsterSpawnListView.Items)
    {
      if (item.Tag is ServerMonsterSpawnInspectorItem spawnItem && ReferenceEquals(spawnItem.Spawn, spawn))
      {
        ClearListViewSelection(_serverMonsterSpawnListView);
        item.Selected = true;
        item.Focused = true;
        item.EnsureVisible();
        return;
      }
    }
  }

  private ServerMonsterBlockInspectorItem? GetSelectedMonsterInspectorItem()
  {
    return _serverMonsterListView.SelectedItems.Count > 0
      ? _serverMonsterListView.SelectedItems[0].Tag as ServerMonsterBlockInspectorItem
      : null;
  }

  private ServerMonsterDummyInspectorItem? GetSelectedMonsterDummyInspectorItem()
  {
    return _serverMonsterDummyListView != null && _serverMonsterDummyListView.SelectedItems.Count > 0
      ? _serverMonsterDummyListView.SelectedItems[0].Tag as ServerMonsterDummyInspectorItem
      : null;
  }

  private ServerMonsterSpawnInspectorItem? GetSelectedMonsterSpawnItem()
  {
    return _serverMonsterSpawnListView != null && _serverMonsterSpawnListView.SelectedItems.Count > 0
      ? _serverMonsterSpawnListView.SelectedItems[0].Tag as ServerMonsterSpawnInspectorItem
      : null;
  }

  private ServerMonsterDefinition? GetSelectedMonsterDefinition()
  {
    return _serverMonsterDefinitionComboBox.SelectedItem is ServerMonsterDefinitionComboItem item ? item.Definition : null;
  }

  private int GetSelectedMonsterSpawnSetIndex()
  {
    if (_serverMonsterSpawnSetComboBox.SelectedItem is MonsterSpawnSetComboItem setItem)
    {
      return setItem.SetIndex;
    }

    return 0;
  }

  private ServerDummyPosition CreateMonsterDummyAtCamera(ServerDummyPosition? templateDummy, string dummyCode)
  {
    Vector3 center = GetPreferredMonsterPlacementCenter();
    Vector3 halfExtents = GetMonsterDummyHalfExtents(templateDummy);
    return new ServerDummyPosition
    {
      Code = dummyCode,
      HelperIndex = ushort.MaxValue,
      LocalMin = -halfExtents,
      LocalMax = halfExtents,
      WorldMin = center - halfExtents,
      WorldMax = center + halfExtents,
      WorldRightTop = center + halfExtents,
      WorldLeftBottom = center - halfExtents,
      WorldCenter = center,
      WorldDirection = Vector3.UnitZ,
      IsPositionable = true,
    };
  }

  private Vector3 GetPreferredMonsterPlacementCenter()
  {
    if (_viewer.TryGetCameraSourcePose(out Vector3 sourcePosition, out Vector3 sourceForward))
    {
      sourceForward.Y = 0.0f;
      if (sourceForward.LengthSquared < 0.000001f)
      {
        sourceForward = -Vector3.UnitZ;
      }
      else
      {
        sourceForward = Vector3.Normalize(sourceForward);
      }

      return sourcePosition + sourceForward * 250.0f;
    }

    if (_loadedMap != null)
    {
      return (_loadedMap.Bounds.Min + _loadedMap.Bounds.Max) * 0.5f;
    }

    return Vector3.Zero;
  }

  private static Vector3 GetMonsterDummyHalfExtents(ServerDummyPosition? templateDummy)
  {
    if (templateDummy != null)
    {
      Vector3 editedSize = templateDummy.EditedWorldMax - templateDummy.EditedWorldMin;
      if (IsFinite(editedSize) &&
          editedSize.X > 0.0001f &&
          editedSize.Y > 0.0001f &&
          editedSize.Z > 0.0001f)
      {
        Vector3 halfExtents = editedSize * 0.5f;
        return new Vector3(
          MathF.Max(halfExtents.X, 40.0f),
          MathF.Max(halfExtents.Y, 40.0f),
          MathF.Max(halfExtents.Z, 40.0f));
      }
    }

    return new Vector3(180.0f, 120.0f, 180.0f);
  }

  private static Vector3 ComputeMonsterBlockCenter(ServerMapMonsterDummyRef[] dummyRefs)
  {
    if (dummyRefs.Length <= 0)
    {
      return Vector3.Zero;
    }

    Vector3 sum = Vector3.Zero;
    for (int index = 0; index < dummyRefs.Length; ++index)
    {
      sum += dummyRefs[index].Dummy.EditedWorldCenter;
    }

    return sum / dummyRefs.Length;
  }

  private static int GetNextMonsterBlockRecordIndex(ServerMapData serverData)
  {
    int nextIndex = 0;
    for (int index = 0; index < serverData.MonsterBlocks.Length; ++index)
    {
      nextIndex = Math.Max(nextIndex, serverData.MonsterBlocks[index].RecordIndex + 1);
    }

    return nextIndex;
  }

  private static int GetNextMonsterSpawnRecordIndex(ServerMapData serverData)
  {
    int nextIndex = 0;
    for (int blockIndex = 0; blockIndex < serverData.MonsterBlocks.Length; ++blockIndex)
    {
      ServerMapMonsterBlock block = serverData.MonsterBlocks[blockIndex];
      for (int setIndex = 0; setIndex < block.SpawnSets.Length; ++setIndex)
      {
        ServerMapMonsterSpawnSet spawnSet = block.SpawnSets[setIndex];
        for (int spawnIndex = 0; spawnIndex < spawnSet.Spawns.Length; ++spawnIndex)
        {
          nextIndex = Math.Max(nextIndex, spawnSet.Spawns[spawnIndex].RecordIndex + 1);
        }
      }
    }

    return nextIndex;
  }

  private static string GenerateUniqueMonsterBlockCode(ServerMapData serverData)
  {
    HashSet<string> existingCodes = new(serverData.MonsterBlocks.Select(static block => block.Code), StringComparer.OrdinalIgnoreCase);
    string mapFileName = serverData.MapDefinition.FileName;
    string prefix = $"{mapFileName.ToLowerInvariant()}m";
    int minimumDigits = 2;
    for (int blockIndex = 0; blockIndex < serverData.MonsterBlocks.Length; ++blockIndex)
    {
      if (TryParseMonsterBlockCodeNumber(serverData.MonsterBlocks[blockIndex].Code, mapFileName, out _, out string digits))
      {
        minimumDigits = Math.Max(minimumDigits, digits.Length);
      }
    }

    for (int index = 1; index < 10000; ++index)
    {
      string candidate = $"{prefix}{index.ToString($"D{minimumDigits}", CultureInfo.InvariantCulture)}";
      if (!existingCodes.Contains(candidate))
      {
        return candidate;
      }
    }

    return $"{prefix}{Guid.NewGuid():N}";
  }

  private static string GenerateUniqueMonsterDummyCode(ServerMapData serverData, string? blockCode)
  {
    HashSet<string> existingCodes = new(serverData.Helpers.Select(static helper => helper.Name), StringComparer.OrdinalIgnoreCase);
    for (int dummyIndex = 0; dummyIndex < serverData.MonsterDummies.Length; ++dummyIndex)
    {
      existingCodes.Add(serverData.MonsterDummies[dummyIndex].Code);
    }

    if (TryBuildMonsterDummyPrefix(serverData.MapDefinition.FileName, blockCode, out string dummyPrefix))
    {
      int nextDummyIndex = 0;
      for (int dummyIndex = 0; dummyIndex < serverData.MonsterDummies.Length; ++dummyIndex)
      {
        if (!TryParseMonsterDummySequence(serverData.MonsterDummies[dummyIndex].Code, dummyPrefix, out int parsedIndex))
        {
          continue;
        }

        nextDummyIndex = Math.Max(nextDummyIndex, parsedIndex + 1);
      }

      for (int index = nextDummyIndex; index < nextDummyIndex + 10000; ++index)
      {
        string candidate = $"{dummyPrefix}{index.ToString(CultureInfo.InvariantCulture)}";
        if (!existingCodes.Contains(candidate))
        {
          return candidate;
        }
      }
    }

    string fallbackPrefix = $"{serverData.MapDefinition.FileName.ToLowerInvariant()}_dm_";
    for (int index = 1; index < 10000; ++index)
    {
      string candidate = $"{fallbackPrefix}{index:0000}";
      if (!existingCodes.Contains(candidate))
      {
        return candidate;
      }
    }

    return $"{fallbackPrefix}{Guid.NewGuid():N}";
  }

  private static bool TryParseMonsterBlockCodeNumber(string blockCode, string mapFileName, out int numericValue, out string digits)
  {
    numericValue = 0;
    digits = string.Empty;
    if (string.IsNullOrWhiteSpace(blockCode) || string.IsNullOrWhiteSpace(mapFileName))
    {
      return false;
    }

    string expectedPrefix = $"{mapFileName}m";
    if (!blockCode.StartsWith(expectedPrefix, StringComparison.OrdinalIgnoreCase))
    {
      return false;
    }

    digits = blockCode[expectedPrefix.Length..];
    if (digits.Length == 0)
    {
      return false;
    }

    for (int index = 0; index < digits.Length; ++index)
    {
      if (!char.IsDigit(digits[index]))
      {
        return false;
      }
    }

    return int.TryParse(digits, CultureInfo.InvariantCulture, out numericValue);
  }

  private static bool TryBuildMonsterDummyPrefix(string mapFileName, string? blockCode, out string dummyPrefix)
  {
    dummyPrefix = string.Empty;
    if (!TryParseMonsterBlockCodeNumber(blockCode ?? string.Empty, mapFileName, out _, out string digits))
    {
      return false;
    }

    dummyPrefix = $"dmm{digits}_";
    return true;
  }

  private static bool TryParseMonsterDummySequence(string dummyCode, string dummyPrefix, out int sequence)
  {
    sequence = 0;
    if (!dummyCode.StartsWith(dummyPrefix, StringComparison.OrdinalIgnoreCase))
    {
      return false;
    }

    string suffix = dummyCode[dummyPrefix.Length..];
    if (suffix.Length == 0)
    {
      return false;
    }

    for (int index = 0; index < suffix.Length; ++index)
    {
      if (!char.IsDigit(suffix[index]))
      {
        return false;
      }
    }

    return int.TryParse(suffix, CultureInfo.InvariantCulture, out sequence);
  }

  private static ServerMapMonsterSpawn CreateMonsterSpawn(
    ServerMapData serverData,
    ServerMonsterDefinition definition,
    uint regenTime,
    ServerMapMonsterSpawn? templateSpawn)
  {
    int recordIndex = GetNextMonsterSpawnRecordIndex(serverData);
    return new ServerMapMonsterSpawn
    {
      RecordIndex = recordIndex,
      DeclaredIndex = checked((uint)recordIndex),
      MonsterCode = definition.Code,
      MonsterRecordIndex = definition.RecordIndex,
      MonsterName = definition.DisplayName,
      MonsterLevel = definition.Level,
      MonsterGrade = definition.MobGrade,
      MonsterRaceCode = definition.RaceCode,
      RegenTime = regenTime,
      RegenLimit = templateSpawn?.RegenLimit ?? 1u,
      RegenProp = templateSpawn?.RegenProp ?? 0u,
      RegenMin = templateSpawn?.RegenMin ?? 1u,
      StandardKill = templateSpawn?.StandardKill ?? 0u,
      RegenMax = templateSpawn?.RegenMax ?? 1u,
    };
  }

  private static string BuildMonsterSummary(IReadOnlyList<string> names, IReadOnlyList<string> codes)
  {
    IReadOnlyList<string> preferred = names.Count > 0 ? names : codes;
    if (preferred.Count == 0)
    {
      return "Unknown";
    }

    if (preferred.Count == 1)
    {
      return preferred[0];
    }

    if (preferred.Count == 2)
    {
      return $"{preferred[0]}, {preferred[1]}";
    }

    return $"{preferred[0]}, {preferred[1]} +{preferred.Count - 2}";
  }

  private void PopulatePortalInspectorList(ServerMapData? serverData)
  {
    _serverPortalListView.BeginUpdate();
    try
    {
      _serverPortalListView.Items.Clear();
      if (serverData == null)
      {
        return;
      }

      for (int index = 0; index < serverData.Portals.Length; ++index)
      {
        ServerMapPortal portal = serverData.Portals[index];
        ServerDummyInspectorItem inspectorItem = new()
        {
          Label = $"portal {portal.Code}",
          Mode = ServerInspectorMode.Portals,
          Dummy = portal.Dummy,
        };
        AddServerListItem(
          _serverPortalListView,
          inspectorItem,
          portal.Code,
          portal.LinkMapCode,
          portal.LinkPortalCode,
          portal.MenuText,
          portal.NeedCharacterLevel.ToString(),
          portal.Dummy.Code,
          FormatSourcePosition(portal.Dummy.EditedWorldCenter));
      }
    }
    finally
    {
      _serverPortalListView.EndUpdate();
    }
  }

  private void PopulateStoreInspectorList(ServerMapData? serverData)
  {
    _serverStoreListView.BeginUpdate();
    try
    {
      _serverStoreListView.Items.Clear();
      if (serverData == null)
      {
        return;
      }

      for (int index = 0; index < serverData.Stores.Length; ++index)
      {
        ServerMapStore store = serverData.Stores[index];
        ServerDummyInspectorItem inspectorItem = new()
        {
          Label = $"store {store.Code}",
          Mode = ServerInspectorMode.Stores,
          Dummy = store.Dummy,
        };
        AddServerListItem(
          _serverStoreListView,
          inspectorItem,
          store.Code,
          store.NpcName,
          store.NpcCode,
          store.TradeType.ToString(),
          store.SetNpcAngle ? store.NpcAngle.ToString() : "-",
          store.Dummy.Code,
          FormatSourcePosition(store.Dummy.EditedWorldCenter));
      }
    }
    finally
    {
      _serverStoreListView.EndUpdate();
    }
  }

  private void PopulateStartInspectorList(ServerMapData? serverData)
  {
    _serverStartListView.BeginUpdate();
    try
    {
      _serverStartListView.Items.Clear();
      if (serverData == null)
      {
        return;
      }

      for (int index = 0; index < serverData.StartPoints.Length; ++index)
      {
        ServerMapStartPoint startPoint = serverData.StartPoints[index];
        ServerDummyInspectorItem inspectorItem = new()
        {
          Label = $"start point {startPoint.Index}",
          Mode = ServerInspectorMode.Starts,
          Dummy = startPoint.Dummy,
        };
        AddServerListItem(
          _serverStartListView,
          inspectorItem,
          startPoint.Index.ToString(),
          startPoint.Dummy.Code,
          FormatSourcePosition(startPoint.Dummy.EditedWorldCenter));
      }
    }
    finally
    {
      _serverStartListView.EndUpdate();
    }
  }

  private void PopulateResourceInspectorList(ServerMapData? serverData)
  {
    _serverResourceListView.BeginUpdate();
    try
    {
      _serverResourceListView.Items.Clear();
      if (serverData == null)
      {
        return;
      }

      for (int index = 0; index < serverData.ResourceNodes.Length; ++index)
      {
        ServerMapResourceNode resourceNode = serverData.ResourceNodes[index];
        ServerDummyInspectorItem inspectorItem = new()
        {
          Label = $"resource {resourceNode.Index}",
          Mode = ServerInspectorMode.Resources,
          Dummy = resourceNode.Dummy,
        };
        AddServerListItem(
          _serverResourceListView,
          inspectorItem,
          resourceNode.Index.ToString(),
          resourceNode.Grade.ToString(),
          resourceNode.Dummy.Code,
          FormatSourcePosition(resourceNode.Dummy.EditedWorldCenter));
      }
    }
    finally
    {
      _serverResourceListView.EndUpdate();
    }
  }

  private static ListViewItem AddServerListItem(ListView listView, object? tag, params string[] values)
  {
    ListViewItem item = new(values);
    if (tag != null)
    {
      item.Tag = tag;
    }

    listView.Items.Add(item);
    return item;
  }

  private static string FormatSourcePosition(Vector3 position)
  {
    return FormattableString.Invariant($"{position.X:F1}, {position.Y:F1}, {position.Z:F1}");
  }

  private static bool IsFinite(Vector3 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y) && float.IsFinite(value.Z);
  }

  private void OnServerListItemActivate(object? sender, EventArgs e)
  {
    if (sender is not ListView listView || listView.SelectedItems.Count <= 0)
    {
      return;
    }

    JumpToSelectedServerItem(listView);
  }

  private void JumpToSelectedServerItem(ListView listView)
  {
    ServerNavigationTarget? navigationTarget = TryGetSelectedServerNavigationTarget(listView);
    if (!navigationTarget.HasValue)
    {
      return;
    }

    if (!TryJumpToServerTarget(navigationTarget.Value))
    {
      return;
    }

    SaveCurrentCameraPosition();
    SyncTeleportInputsFromCamera();
    _statusBaseText =
      $"Jumped to {navigationTarget.Value.Label} | Pos: ({navigationTarget.Value.Position.X:F1},{navigationTarget.Value.Position.Y:F1},{navigationTarget.Value.Position.Z:F1})";
    RefreshRuntimeStatus();
    _viewer.Focus();
  }

  private static ServerNavigationTarget? TryGetSelectedServerNavigationTarget(ListView listView)
  {
    if (listView.SelectedItems.Count <= 0)
    {
      return null;
    }

    return listView.SelectedItems[0].Tag switch
    {
      IServerInspectorItem inspectorItem => new ServerNavigationTarget(inspectorItem.Label, GetServerDummyTopCenter(inspectorItem.Dummy)),
      _ => null,
    };
  }

  private bool TryJumpToServerTarget(ServerNavigationTarget navigationTarget)
  {
    Vector3 position = navigationTarget.Position;
    return _viewer.TrySetCameraSourcePosition(position.X, position.Y, position.Z);
  }

  private static Vector3 GetServerDummyTopCenter(ServerDummyPosition dummy)
  {
    Vector3 center = dummy.EditedWorldCenter;
    float topY = MathF.Max(dummy.EditedWorldMin.Y, dummy.EditedWorldMax.Y);
    return new Vector3(center.X, topY, center.Z);
  }

  private void SaveServerDataFromUi()
  {
    if (_loadedMap?.ServerData is not ServerMapData serverData)
    {
      MessageBox.Show(this, "No server-side map data is loaded.", "Save Server Data", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    try
    {
      ServerMapSaveResult saveResult = ServerMapSaver.Save(serverData);
      _statusBaseText =
        $"Saved server data | Helpers: {saveResult.SavedDummyCount:N0} | Monster blocks: {saveResult.SavedMonsterBlockCount:N0} | Monster spawns: {saveResult.SavedMonsterSpawnCount:N0} | Portals: {saveResult.SavedPortalCount:N0}";
      RefreshRuntimeStatus();
      MessageBox.Show(
        this,
        $"Saved server data back to RF_Bin.{Environment.NewLine}{Environment.NewLine}"
        + $"Helper SPT: {saveResult.HelperScriptPath}{Environment.NewLine}"
        + $"Monster blocks: {saveResult.MonsterBlockTablePath}{Environment.NewLine}"
        + $"Portal table: {saveResult.PortalTablePath}",
        "Save Server Data",
        MessageBoxButtons.OK,
        MessageBoxIcon.Information);
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Save Server Data Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Save server data failed.";
      RefreshRuntimeStatus();
    }
  }

  private void SaveCurrentMapAsFromDialog()
  {
    LoadedMap? map = _mapDocument?.Map;
    if (map == null || string.IsNullOrWhiteSpace(_loadedBspPath) || string.IsNullOrWhiteSpace(_loadedEbpPath))
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
      if (!TryValidateMapForSave(map, out string validationError))
      {
        MessageBox.Show(this, validationError, "Save Map As Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        _statusBaseText = "Save blocked: strict validation failed.";
        RefreshRuntimeStatus();
        return;
      }

      MapExporter.ExportBspEbpPair(map, _loadedBspPath, _loadedEbpPath, targetBspPath, targetEbpPath);
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
    LoadedMap? map = _mapDocument?.Map;
    if (map == null || string.IsNullOrWhiteSpace(_loadedEbpPath))
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
      if (!TryValidateMapForSave(map, out string validationError))
      {
        MessageBox.Show(this, validationError, "Save EBP Only Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        _statusBaseText = "Save blocked: strict validation failed.";
        RefreshRuntimeStatus();
        return;
      }

      MapExporter.ExportEbpOnly(map, _loadedEbpPath, targetEbpPath);
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

  private void ExportBlenderPackageFromDialog()
  {
    LoadedMap? map = _mapDocument?.Map;
    if (map == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Export Blender Package", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    string initialPath = _loadedBspPath != null
      ? Path.Combine(Path.GetDirectoryName(_loadedBspPath) ?? Environment.CurrentDirectory, $"{map.Name}_Blender")
      : Environment.CurrentDirectory;

    using FolderBrowserDialog dialog = new()
    {
      Description = "Select output folder for Blender package export",
      UseDescriptionForTitle = true,
      ShowNewFolderButton = true,
      SelectedPath = initialPath,
    };

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    try
    {
      BlenderInterchange.ExportPackage(map, dialog.SelectedPath);
      _statusBaseText = $"Exported Blender package: {dialog.SelectedPath}";
      RefreshRuntimeStatus();
      MessageBox.Show(this, "Blender package export completed.", "Export Blender Package", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Export Blender Package Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Blender package export failed.";
      RefreshRuntimeStatus();
    }
  }

  private void ImportBlenderPackageFromDialog()
  {
    LoadedMap? map = _mapDocument?.Map;
    if (map == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Import Blender Package", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    string initialPath = _loadedBspPath != null
      ? Path.Combine(Path.GetDirectoryName(_loadedBspPath) ?? Environment.CurrentDirectory, $"{map.Name}_Blender")
      : Environment.CurrentDirectory;

    using FolderBrowserDialog dialog = new()
    {
      Description = "Select Blender package folder to import",
      UseDescriptionForTitle = true,
      ShowNewFolderButton = false,
      SelectedPath = initialPath,
    };

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    DialogResult collisionChoice = MessageBox.Show(
      this,
      "Rebuild collision from imported mesh?\n\nYes: rebuild collision (experimental)\nNo: keep current collision data",
      "Import Blender Package",
      MessageBoxButtons.YesNoCancel,
      MessageBoxIcon.Question,
      MessageBoxDefaultButton.Button2);
    if (collisionChoice == DialogResult.Cancel)
    {
      return;
    }

    bool rebuildCollisionFromMesh = collisionChoice == DialogResult.Yes;

    try
    {
      LoadedMap edited = BlenderInterchange.ImportPackage(map, dialog.SelectedPath, rebuildCollisionFromMesh, out BlenderPackageImportReport report);
      ApplyNonCollisionEditedMap(edited);

      List<string> summaryLines = new()
      {
        $"Imported mesh triangles: {report.ImportedMeshTriangleCount:N0}",
        $"Imported collision lines: {report.ImportedCollisionLineCount:N0}",
        $"Imported materials: {report.ImportedMaterialCount:N0}",
        $"Imported textures: {report.ImportedTextureCount:N0}",
        $"Imported entity instances: {report.ImportedEntityInstanceCount:N0}",
      };
      if (report.Warnings.Length > 0)
      {
        summaryLines.Add(string.Empty);
        summaryLines.Add("Warnings:");
        summaryLines.AddRange(report.Warnings);
      }

      _statusBaseText =
        $"Imported Blender package | MeshTri:{report.ImportedMeshTriangleCount:N0} Col:{report.ImportedCollisionLineCount:N0} Mats:{report.ImportedMaterialCount:N0} Tex:{report.ImportedTextureCount:N0} Ent:{report.ImportedEntityInstanceCount:N0}";
      RefreshRuntimeStatus();
      MessageBox.Show(this, string.Join(Environment.NewLine, summaryLines), "Import Blender Package", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "Import Blender Package Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      _statusBaseText = "Blender package import failed.";
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
      _lastEntityArchiveCache = archiveCache;
      LoadedMap map = BspLoader.Load(bspPath, ebpPath, _skySourceMode, _strictLoadModeEnabled);
      _editUndoHistory.Clear();
      _editRedoHistory.Clear();
      _loadedBspPath = NormalizeMapPath(bspPath);
      _loadedEbpPath = Path.GetFullPath(ebpPath);
      string? serverLoadNote = null;
      if (!string.IsNullOrWhiteSpace(_serverRootPath))
      {
        try
        {
          ServerMapData serverData = ServerMapLoader.Load(map, _loadedBspPath, _serverRootPath);
          map = MapEditOperations.WithServerData(map, serverData);
        }
        catch (Exception ex)
        {
          serverLoadNote = $"ServerLoad: {ex.Message}";
        }
      }

      _loadedMap = map;
      _mapDocument = MapDocument.FromLoadedMap(map);
      _viewer.SetMap(map);
      RestoreCameraPositionForMap(_loadedBspPath);
      SyncTeleportInputsFromCamera();
      SyncSelectedMapToLoadedPath(_loadedBspPath);
      Text = $"RF MapEditor (Viewer) - {map.Name}";
      _statusBaseText = BuildLoadedMapStatus(map, archiveCache, serverLoadNote);
      RefreshRuntimeStatus();
      SetServerInspectorMode(map.ServerData != null ? ServerInspectorMode.Monsters : ServerInspectorMode.Summary, syncComboBox: true);
      RefreshServerInspector();
      UpdateUndoUiState();
    }
    catch (Exception ex)
    {
      _loadedMap = null;
      _mapDocument = null;
      _editUndoHistory.Clear();
      _editRedoHistory.Clear();
      _statusBaseText = "Load failed.";
      RefreshRuntimeStatus();
      SetServerInspectorMode(ServerInspectorMode.Summary, syncComboBox: true);
      RefreshServerInspector();
      UpdateUndoUiState();
      string title = _strictLoadModeEnabled
        ? "Load Map Failed (Strict Validation)"
        : "Load Map Failed";
      MessageBox.Show(this, ex.Message, title, MessageBoxButtons.OK, MessageBoxIcon.Error);
    }
  }

  private string BuildLoadedMapStatus(LoadedMap map, EntityArchiveCacheInfo archiveCache, string? serverLoadNote)
  {
    string cacheTag = archiveCache.ArchiveCount > 0
      ? $"{(archiveCache.WasCached ? "RPKCache:Hit" : "RPKCache:Build")} {archiveCache.ArchiveCount:N0}rpk/{archiveCache.AssetCount:N0}assets"
      : "RPKCache:N/A";

    string status =
      $"Loaded {map.Name} | BSP TriVerts: {map.BspTriangleVertices.Length:N0} | EBP Vertices: {map.CollisionVertices.Length:N0} | EBP Lines: {map.CollisionLines.Length:N0}"
      + $" | RenderVerts: {map.BspRenderVertices.Length:N0}"
      + $" | TextureBlobs: {map.SurfaceTextures.Length:N0} | LgtTex: {map.LightmapTextures.Length:N0}"
      + $" | Fog: {(map.Environment.FogEnabled ? $"On({map.Environment.FogStart:F0}-{map.Environment.FogEnd:F0})" : "Off")} | ExtDummy: {map.ExtDummies.Length:N0}"
      + $" | SkySource: {GetSkySourceName(_skySourceMode)} | SkyVerts: {map.SkyRenderVertices.Length:N0} | SkyTex: {map.SkySurfaceTextures.Length:N0}"
      + $" | EntModels: {map.MapEntityModelCount:N0} | EntInst: {map.MapEntityInstanceCount:N0} | FxInst: {map.ParticleInstancePositions.Length:N0} | EntVerts: {map.EntityRenderVertices.Length:N0} | EntTex: {map.EntitySurfaceTextures.Length:N0}"
      + $" | {cacheTag}"
      + $" | Pipe: {GetRenderPipelineModeName(_renderPipelineMode)}"
      + $" | StrictLoad: {(_strictLoadModeEnabled ? "On" : "Off")}"
      + $" | Bounds: ({map.Bounds.Min.X:F0},{map.Bounds.Min.Y:F0},{map.Bounds.Min.Z:F0})..({map.Bounds.Max.X:F0},{map.Bounds.Max.Y:F0},{map.Bounds.Max.Z:F0})";

    if (map.ServerData != null)
    {
      ServerMapData serverData = map.ServerData;
      status +=
        $" | Srv MonDef: {serverData.MonsterDefinitions.Length:N0}"
        + $" | Srv MonBlk: {serverData.MonsterBlocks.Length:N0}"
        + $" | Srv Spawn: {serverData.MonsterSpawnCount:N0}"
        + $" | Srv Portal: {serverData.Portals.Length:N0}"
        + $" | Srv Store: {serverData.Stores.Length:N0}"
        + $" | Srv Start: {serverData.StartPoints.Length:N0}"
        + $" | Srv Res: {serverData.ResourceNodes.Length:N0}";
    }
    else if (!string.IsNullOrWhiteSpace(serverLoadNote))
    {
      status += $" | {serverLoadNote}";
    }

    return status;
  }

  private void ShowControls()
  {
    const string message =
      """
      Camera Controls
      - Ctrl+O: select map root folder
      - Ctrl+Shift+O: select WorldServer RF_Bin folder for server-side map data
      - Ctrl+S: save current EBP only (collision changes)
      - Ctrl+Shift+S: save current loaded BSP/EBP as a new pair (+sidecars/entity)
      - Ctrl+M: edit map R3M materials
      - Ctrl+T: edit map R3T textures
      - Ctrl+E: edit R3X environment values (fog/lens/flags)
      - Edit > Analyze Collision Leak Joints: rank likely corner leak spots by line index
      - Ctrl+Shift+E: export Blender package (mesh/materials/textures/entities)
      - Ctrl+Shift+I: import Blender package (materials/textures/entities)
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
      - View > Strict Load Mode: fail map load on malformed BSP/EBP references
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
      - DelSel/Delete: mode-aware delete (SelWall deletes collision, BSP Select/BSP Move deletes BSP)
      - DrawWall preview turns green when endpoint glue/snap will happen
      - Undo button or Ctrl+Z: undo latest collision edits (up to 64 steps)
      - Redo button or Ctrl+Y/Ctrl+Shift+Z: redo last undone edit
      - ResetMap: discard unsaved edits and reload map from source files
      - BSP Editor strip (3rd row): enable `BSP Select`, then pick mode `Obj`/`Face`/`Edge`/`Vert` (Edge/Vert are true primitive selection)
      - Shift+click in BSP Select/BSP Move: add/remove from BSP multi-selection
      - BSP Move: click-drag selected BSP object/face/edge/vertex with mouse (view-plane drag)
      - X-/X+/Y-/Y+/Z+: nudge selected BSP by step value
      - S-/S+: uniform scale selected BSP (5% step)
      - RY-/RY+: rotate selected BSP around Y axis (5 deg step)
      - BSP Del: delete selected BSP object/face/edge/vertex
      - BSP Copy: duplicate selected BSP object/face/edge/vertex (+X by step)
      - BSP Merge: merge selected BSP object/face/edge/vertex into one object group
      - DynLight button: toggle dynamic lighting
      - FxMark button: toggle particle/effect instances and map markers
      - R: reset camera to map default
      - Esc: release mouse-look capture
      - Top-left HUD: camera coordinates (Cam XYZ, source/game-space)
      - Camera position is saved per map and restored on next load
      """;

    MessageBox.Show(this, message, "Controls", MessageBoxButtons.OK, MessageBoxIcon.Information);
  }

  private void AnalyzeCollisionLeakJointsFromUi()
  {
    LoadedMap? map = _loadedMap;
    if (map == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Analyze Collision Leak Joints", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    const float scoreThreshold = 2.0f;
    const int maxResults = 64;
    CollisionLeakJointReport report = CollisionLeakJointAnalyzer.Analyze(map, scoreThreshold, maxResults);
    CollisionLeakJointRisk[] selectedLineRisks = Array.Empty<CollisionLeakJointRisk>();
    int selectedLineIndex = _viewer.SelectedCollisionLineIndex;
    if ((uint)selectedLineIndex < (uint)map.CollisionLines.Length)
    {
      CollisionLeakJointReport allRisks = CollisionLeakJointAnalyzer.Analyze(map, scoreThreshold: 0.0f, maxResults: int.MaxValue);
      selectedLineRisks = allRisks.Risks
        .Where(static risk => risk.LineIndex >= 0)
        .Where(risk => risk.LineIndex == selectedLineIndex)
        .OrderByDescending(static risk => risk.Score)
        .ToArray();
    }

    StringBuilder builder = new();
    builder.AppendLine("Collision Leak-Prone Joint Checker (heuristic)");
    builder.AppendLine($"Map: {map.Name}");
    builder.AppendLine($"Scanned lines: {report.ScannedLineCount:N0}");
    builder.AppendLine($"Threshold: {report.ScoreThreshold:F2}");
    builder.AppendLine($"Flagged lines: {report.Risks.Length:N0}");
    builder.AppendLine();
    if ((uint)selectedLineIndex < (uint)map.CollisionLines.Length)
    {
      builder.AppendLine(FormattableString.Invariant($"Selected line: L{selectedLineIndex}"));
      if (selectedLineRisks.Length == 0)
      {
        builder.AppendLine("Selected line risk: no endpoint risk found.");
      }
      else
      {
        for (int i = 0; i < selectedLineRisks.Length; ++i)
        {
          CollisionLeakJointRisk risk = selectedLineRisks[i];
          string neighbor = risk.NeighborLineIndex >= 0 ? $"L{risk.NeighborLineIndex}" : "none";
          builder.AppendLine(
            FormattableString.Invariant(
              $"Selected {i + 1}. L{risk.LineIndex} {risk.Endpoint} V{risk.VertexIndex} score={risk.Score:F2} angle={risk.JointAngleDegrees:F2} ratio={risk.LengthRatio:F2} deg={risk.Degree} len={risk.LineLength:F2}/{risk.NeighborLength:F2} neigh={neighbor} pos=({risk.VertexPosition.X:F2},{risk.VertexPosition.Y:F2},{risk.VertexPosition.Z:F2})"));
        }
      }

      builder.AppendLine();
    }

    if (report.Risks.Length == 0)
    {
      builder.AppendLine("No lines exceeded the leak-risk threshold.");
      builder.AppendLine("If a known exploit still exists, lower threshold or inspect that line manually.");
      _statusBaseText = "Leak checker: no high-risk collision joints found.";
      RefreshRuntimeStatus();
      ShowReadOnlyReportDialog("Collision Leak Joint Report", builder.ToString());
      return;
    }

    builder.AppendLine("Top candidates:");
    for (int i = 0; i < report.Risks.Length; ++i)
    {
      CollisionLeakJointRisk risk = report.Risks[i];
      string neighbor = risk.NeighborLineIndex >= 0 ? $"L{risk.NeighborLineIndex}" : "none";
      builder.AppendLine(
        FormattableString.Invariant(
          $"{i + 1,2}. L{risk.LineIndex} {risk.Endpoint} V{risk.VertexIndex} score={risk.Score:F2} angle={risk.JointAngleDegrees:F2} ratio={risk.LengthRatio:F2} deg={risk.Degree} len={risk.LineLength:F2}/{risk.NeighborLength:F2} neigh={neighbor} pos=({risk.VertexPosition.X:F2},{risk.VertexPosition.Y:F2},{risk.VertexPosition.Z:F2})"));
    }

    CollisionLeakJointRisk topRisk = report.Risks[0];
    _viewer.SetSelectedCollisionLineIndex(topRisk.LineIndex);
    _statusBaseText = FormattableString.Invariant(
      $"Leak checker: {report.Risks.Length:N0} flagged | Top L{topRisk.LineIndex} {topRisk.Endpoint} score={topRisk.Score:F2} angle={topRisk.JointAngleDegrees:F1} ratio={topRisk.LengthRatio:F2}");
    RefreshRuntimeStatus();
    ShowReadOnlyReportDialog("Collision Leak Joint Report", builder.ToString());
  }

  private void ShowReadOnlyReportDialog(string title, string content)
  {
    using Form dialog = new()
    {
      Text = title,
      StartPosition = FormStartPosition.CenterParent,
      ClientSize = new Size(980, 640),
      MinimumSize = new Size(700, 420),
      ShowInTaskbar = false,
      MinimizeBox = false,
      MaximizeBox = false,
    };

    TextBox reportBox = new()
    {
      Multiline = true,
      ReadOnly = true,
      WordWrap = false,
      ScrollBars = ScrollBars.Both,
      Dock = DockStyle.Fill,
      Font = new Font("Consolas", 9.0f, FontStyle.Regular, GraphicsUnit.Point),
      Text = content,
    };

    Button copyButton = new()
    {
      Text = "Copy",
      AutoSize = true,
      Anchor = AnchorStyles.Right | AnchorStyles.Top,
    };
    copyButton.Click += (_, _) =>
    {
      try
      {
        Clipboard.SetText(content);
      }
      catch
      {
      }
    };

    Button closeButton = new()
    {
      Text = "Close",
      AutoSize = true,
      DialogResult = DialogResult.OK,
      Anchor = AnchorStyles.Right | AnchorStyles.Top,
    };

    FlowLayoutPanel buttonPanel = new()
    {
      Dock = DockStyle.Bottom,
      FlowDirection = FlowDirection.RightToLeft,
      Padding = new Padding(8),
      Height = 48,
      WrapContents = false,
    };
    buttonPanel.Controls.Add(closeButton);
    buttonPanel.Controls.Add(copyButton);

    dialog.AcceptButton = closeButton;
    dialog.CancelButton = closeButton;
    dialog.Controls.Add(reportBox);
    dialog.Controls.Add(buttonPanel);
    dialog.ShowDialog(this);
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
    float snap = FixedSnapDistance;
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
        _statusBaseText = $"Mouse draw skipped: {error} | Tip: drag longer.";
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

  private void OnViewerBspSelectionChanged(int selectedFaceIndex, int selectedObjectId)
  {
    UpdateUndoUiState();
    if (_loadedMap == null)
    {
      return;
    }

    int selectedObjectCount = _viewer.GetSelectedBspObjectIdsSnapshot().Length;
    int selectedFaceSeedCount = _viewer.GetSelectedBspFaceSelectionsSnapshot().Length;
    int selectedVertexCount = _viewer.GetSelectedBspVertexSelectionsSnapshot().Length;
    int selectedEdgeCount = _viewer.GetSelectedBspEdgeSelectionsSnapshot().Length;

    if (selectedVertexCount > 0 || selectedEdgeCount > 0)
    {
      _statusBaseText =
        $"Selected BSP primitive ({selectedVertexCount} vertex key(s), {selectedEdgeCount} edge(s)) | Mode: {_viewer.ActiveBspSelectionMode}";
    }
    else if (selectedObjectCount > 1 || selectedFaceSeedCount > 1)
    {
      _statusBaseText = $"Selected BSP multi ({selectedObjectCount} object(s), {selectedFaceSeedCount} face seed(s))";
    }
    else if (selectedObjectId >= 0)
    {
      _statusBaseText = $"Selected BSP object #{selectedObjectId:N0} (face #{selectedFaceIndex:N0})";
    }
    else if (selectedFaceIndex >= 0)
    {
      _statusBaseText = _viewer.SelectedBspFaceOnly
        ? $"Selected BSP face #{selectedFaceIndex:N0}"
        : $"Selected BSP mesh cluster from face #{selectedFaceIndex:N0}";
    }
    else
    {
      _statusBaseText = "BSP selection cleared.";
    }

    RefreshRuntimeStatus();
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

  private void DeleteSelectedBspMeshFromUi()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Delete BSP Mesh", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    int[] selectedObjectIds = _viewer.GetSelectedBspObjectIdsSnapshot();
    BlenderInterchange.BspFaceSelection[] selectedFaces = _viewer.GetSelectedBspFaceSelectionsSnapshot();
    BlenderInterchange.BspVertexSelection[] selectedVertices = _viewer.GetSelectedBspVertexSelectionsSnapshot();
    BlenderInterchange.BspEdgeSelection[] selectedEdges = _viewer.GetSelectedBspEdgeSelectionsSnapshot();
    if (selectedObjectIds.Length <= 0 &&
        selectedFaces.Length <= 0 &&
        selectedVertices.Length <= 0 &&
        selectedEdges.Length <= 0)
    {
      MessageBox.Show(this, "No BSP object/face/edge/vertex is selected.", "Delete BSP Mesh", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    bool primitiveMode =
      _viewer.ActiveBspSelectionMode is MapViewerControl.BspSelectionMode.Vertex or MapViewerControl.BspSelectionMode.Edge;
    bool deleted = primitiveMode
      ? BlenderInterchange.TryDeleteMeshPrimitiveSelection(
        _loadedMap,
        selectedVertices,
        selectedEdges,
        out LoadedMap editedMap,
        out string error)
      : BlenderInterchange.TryDeleteMeshSelection(
        _loadedMap,
        selectedObjectIds,
        selectedFaces,
        out editedMap,
        out error);
    if (!deleted)
    {
      MessageBox.Show(this, error, "Delete BSP Mesh Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: -1, trackUndo: true, clearRedo: true, geometryEdited: true);
    _viewer.ClearSelectedBspSelection();
    _statusBaseText = primitiveMode
      ? $"Deleted BSP primitive selection ({selectedVertices.Length} vertex key(s), {selectedEdges.Length} edge(s)) | BSP TriVerts: {_loadedMap.BspRenderVertices.Length:N0}"
      : $"Deleted BSP selection ({selectedObjectIds.Length} object(s), {selectedFaces.Length} face seed(s)) | BSP TriVerts: {_loadedMap.BspRenderVertices.Length:N0}";
    RefreshRuntimeStatus();
  }

  private void DuplicateSelectedBspMeshFromUi()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Duplicate BSP Mesh", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    int[] selectedObjectIds = _viewer.GetSelectedBspObjectIdsSnapshot();
    BlenderInterchange.BspFaceSelection[] selectedFaces = _viewer.GetSelectedBspFaceSelectionsSnapshot();
    BlenderInterchange.BspVertexSelection[] selectedVertices = _viewer.GetSelectedBspVertexSelectionsSnapshot();
    BlenderInterchange.BspEdgeSelection[] selectedEdges = _viewer.GetSelectedBspEdgeSelectionsSnapshot();
    if (selectedObjectIds.Length <= 0 &&
        selectedFaces.Length <= 0 &&
        selectedVertices.Length <= 0 &&
        selectedEdges.Length <= 0)
    {
      MessageBox.Show(this, "No BSP object/face/edge/vertex is selected.", "Duplicate BSP Mesh", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    float step = (float)_bspMoveStepUpDown.Value;
    if (!float.IsFinite(step) || step <= 0.0f)
    {
      step = BspTranslateStep;
    }

    Vector3 duplicateOffset = new(step, 0f, 0f);
    bool primitiveMode =
      _viewer.ActiveBspSelectionMode is MapViewerControl.BspSelectionMode.Vertex or MapViewerControl.BspSelectionMode.Edge;
    bool duplicated = primitiveMode
      ? BlenderInterchange.TryDuplicateMeshPrimitiveSelection(
        _loadedMap,
        selectedVertices,
        selectedEdges,
        duplicateOffset,
        out LoadedMap editedMap,
        out string error)
      : BlenderInterchange.TryDuplicateMeshSelection(
        _loadedMap,
        selectedObjectIds,
        selectedFaces,
        duplicateOffset,
        out editedMap,
        out error);
    if (!duplicated)
    {
      MessageBox.Show(this, error, "Duplicate BSP Mesh Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: -1, trackUndo: true, clearRedo: true, geometryEdited: true);
    _statusBaseText = primitiveMode
      ? $"Duplicated BSP primitive selection ({selectedVertices.Length} vertex key(s), {selectedEdges.Length} edge(s)) by ({duplicateOffset.X:F1},{duplicateOffset.Y:F1},{duplicateOffset.Z:F1})"
      : $"Duplicated BSP selection ({selectedObjectIds.Length} object(s), {selectedFaces.Length} face seed(s)) by ({duplicateOffset.X:F1},{duplicateOffset.Y:F1},{duplicateOffset.Z:F1})";
    RefreshRuntimeStatus();
  }

  private void MergeSelectedBspMeshFromUi()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "Merge BSP Mesh", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    int[] selectedObjectIds = _viewer.GetSelectedBspObjectIdsSnapshot();
    BlenderInterchange.BspFaceSelection[] selectedFaces = _viewer.GetSelectedBspFaceSelectionsSnapshot();
    BlenderInterchange.BspVertexSelection[] selectedVertices = _viewer.GetSelectedBspVertexSelectionsSnapshot();
    BlenderInterchange.BspEdgeSelection[] selectedEdges = _viewer.GetSelectedBspEdgeSelectionsSnapshot();
    if (selectedObjectIds.Length <= 0 &&
        selectedFaces.Length <= 0 &&
        selectedVertices.Length <= 0 &&
        selectedEdges.Length <= 0)
    {
      MessageBox.Show(this, "No BSP object/face/edge/vertex is selected.", "Merge BSP Mesh", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    if (!BlenderInterchange.TryMergeMeshSelection(
      _loadedMap,
      selectedObjectIds,
      selectedFaces,
      selectedVertices,
      selectedEdges,
      out LoadedMap editedMap,
      out string error))
    {
      if (error.Contains("already merged to one object", StringComparison.OrdinalIgnoreCase))
      {
        _statusBaseText = "Merge skipped: selected BSP faces are already in one object group.";
        RefreshRuntimeStatus();
        return;
      }

      MessageBox.Show(this, error, "Merge BSP Mesh Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: -1, trackUndo: true, clearRedo: true, geometryEdited: true);
    _statusBaseText = $"Merged BSP selection into one object group | BSP TriVerts: {_loadedMap.BspRenderVertices.Length:N0}";
    RefreshRuntimeStatus();
  }

  private void TranslateSelectedBspMeshFromUi(Vector3 sourceDelta)
  {
    if (_loadedMap == null)
    {
      return;
    }

    int[] selectedObjectIds = _viewer.GetSelectedBspObjectIdsSnapshot();
    BlenderInterchange.BspFaceSelection[] selectedFaces = _viewer.GetSelectedBspFaceSelectionsSnapshot();
    BlenderInterchange.BspVertexSelection[] selectedVertices = _viewer.GetSelectedBspVertexSelectionsSnapshot();
    BlenderInterchange.BspEdgeSelection[] selectedEdges = _viewer.GetSelectedBspEdgeSelectionsSnapshot();
    if (selectedObjectIds.Length <= 0 &&
        selectedFaces.Length <= 0 &&
        selectedVertices.Length <= 0 &&
        selectedEdges.Length <= 0)
    {
      _statusBaseText = "BSP translate skipped: no BSP object/face/edge/vertex selected.";
      RefreshRuntimeStatus();
      return;
    }

    bool primitiveMode =
      _viewer.ActiveBspSelectionMode is MapViewerControl.BspSelectionMode.Vertex or MapViewerControl.BspSelectionMode.Edge;
    bool moved = primitiveMode
      ? BlenderInterchange.TryTranslateMeshPrimitiveSelection(
        _loadedMap,
        selectedVertices,
        selectedEdges,
        sourceDelta,
        out LoadedMap editedMap,
        out string error)
      : BlenderInterchange.TryTranslateMeshSelection(
        _loadedMap,
        selectedObjectIds,
        selectedFaces,
        sourceDelta,
        out editedMap,
        out error);
    if (!moved)
    {
      _viewer.CancelBspMovePreview();
      _statusBaseText = $"BSP translate failed: {error}";
      RefreshRuntimeStatus();
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: -1, trackUndo: true, clearRedo: true, geometryEdited: true);
    _statusBaseText = primitiveMode
      ? $"Moved BSP primitive selection ({selectedVertices.Length} vertex key(s), {selectedEdges.Length} edge(s)) by ({sourceDelta.X:F1},{sourceDelta.Y:F1},{sourceDelta.Z:F1})"
      : $"Moved BSP selection ({selectedObjectIds.Length} object(s), {selectedFaces.Length} face seed(s)) by ({sourceDelta.X:F1},{sourceDelta.Y:F1},{sourceDelta.Z:F1})";
    RefreshRuntimeStatus();
  }

  private void MoveSelectedBspMeshAxis(float xSign, float ySign, float zSign)
  {
    if (_loadedMap == null)
    {
      return;
    }

    float step = (float)_bspMoveStepUpDown.Value;
    if (!float.IsFinite(step) || step <= 0.0f)
    {
      step = BspTranslateStep;
    }

    Vector3 sourceDelta = new(step * xSign, step * ySign, step * zSign);
    TranslateSelectedBspMeshFromUi(sourceDelta);
  }

  private void ScaleSelectedBspMeshFromUi(float uniformScale)
  {
    if (_loadedMap == null)
    {
      return;
    }

    if (!float.IsFinite(uniformScale) || uniformScale <= 0.0001f)
    {
      _statusBaseText = "BSP scale skipped: invalid scale factor.";
      RefreshRuntimeStatus();
      return;
    }

    int[] selectedObjectIds = _viewer.GetSelectedBspObjectIdsSnapshot();
    BlenderInterchange.BspFaceSelection[] selectedFaces = _viewer.GetSelectedBspFaceSelectionsSnapshot();
    BlenderInterchange.BspVertexSelection[] selectedVertices = _viewer.GetSelectedBspVertexSelectionsSnapshot();
    BlenderInterchange.BspEdgeSelection[] selectedEdges = _viewer.GetSelectedBspEdgeSelectionsSnapshot();
    if (selectedObjectIds.Length <= 0 &&
        selectedFaces.Length <= 0 &&
        selectedVertices.Length <= 0 &&
        selectedEdges.Length <= 0)
    {
      _statusBaseText = "BSP scale skipped: no BSP object/face/edge/vertex selected.";
      RefreshRuntimeStatus();
      return;
    }

    Vector3 scale = new(uniformScale, uniformScale, uniformScale);
    bool primitiveMode =
      _viewer.ActiveBspSelectionMode is MapViewerControl.BspSelectionMode.Vertex or MapViewerControl.BspSelectionMode.Edge;
    bool scaled = primitiveMode
      ? BlenderInterchange.TryScaleMeshPrimitiveSelection(
        _loadedMap,
        selectedVertices,
        selectedEdges,
        scale,
        out LoadedMap editedMap,
        out string error)
      : BlenderInterchange.TryScaleMeshSelection(
        _loadedMap,
        selectedObjectIds,
        selectedFaces,
        scale,
        out editedMap,
        out error);
    if (!scaled)
    {
      _statusBaseText = $"BSP scale failed: {error}";
      RefreshRuntimeStatus();
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: -1, trackUndo: true, clearRedo: true, geometryEdited: true);
    float percent = (uniformScale - 1.0f) * 100.0f;
    _statusBaseText = primitiveMode
      ? $"Scaled BSP primitive selection ({selectedVertices.Length} vertex key(s), {selectedEdges.Length} edge(s)) by {percent:+0.0;-0.0;0.0}%"
      : $"Scaled BSP selection ({selectedObjectIds.Length} object(s), {selectedFaces.Length} face seed(s)) by {percent:+0.0;-0.0;0.0}%";
    RefreshRuntimeStatus();
  }

  private void RotateSelectedBspMeshFromUi(float degreesY)
  {
    RotateSelectedBspMeshFromUi(Vector3.UnitY, degreesY);
  }

  private void RotateSelectedBspMeshFromUi(Vector3 axis, float degreesY)
  {
    if (_loadedMap == null)
    {
      return;
    }

    if (!float.IsFinite(degreesY) || MathF.Abs(degreesY) <= 0.0001f)
    {
      _statusBaseText = "BSP rotate skipped: invalid angle.";
      RefreshRuntimeStatus();
      return;
    }

    int[] selectedObjectIds = _viewer.GetSelectedBspObjectIdsSnapshot();
    BlenderInterchange.BspFaceSelection[] selectedFaces = _viewer.GetSelectedBspFaceSelectionsSnapshot();
    BlenderInterchange.BspVertexSelection[] selectedVertices = _viewer.GetSelectedBspVertexSelectionsSnapshot();
    BlenderInterchange.BspEdgeSelection[] selectedEdges = _viewer.GetSelectedBspEdgeSelectionsSnapshot();
    if (selectedObjectIds.Length <= 0 &&
        selectedFaces.Length <= 0 &&
        selectedVertices.Length <= 0 &&
        selectedEdges.Length <= 0)
    {
      _statusBaseText = "BSP rotate skipped: no BSP object/face/edge/vertex selected.";
      RefreshRuntimeStatus();
      return;
    }

    if (!float.IsFinite(axis.X) || !float.IsFinite(axis.Y) || !float.IsFinite(axis.Z) || axis.LengthSquared <= 0.000001f)
    {
      _statusBaseText = "BSP rotate skipped: invalid axis.";
      RefreshRuntimeStatus();
      return;
    }

    Vector3 axisNormalized = Vector3.Normalize(axis);
    bool primitiveMode =
      _viewer.ActiveBspSelectionMode is MapViewerControl.BspSelectionMode.Vertex or MapViewerControl.BspSelectionMode.Edge;
    bool rotated = primitiveMode
      ? BlenderInterchange.TryRotateMeshPrimitiveSelection(
        _loadedMap,
        selectedVertices,
        selectedEdges,
        axisNormalized,
        degreesY,
        out LoadedMap editedMap,
        out string error)
      : BlenderInterchange.TryRotateMeshSelection(
        _loadedMap,
        selectedObjectIds,
        selectedFaces,
        axisNormalized,
        degreesY,
        out editedMap,
        out error);
    if (!rotated)
    {
      _statusBaseText = $"BSP rotate failed: {error}";
      RefreshRuntimeStatus();
      return;
    }

    ApplyEditedMap(editedMap, preferredSelectionIndex: -1, trackUndo: true, clearRedo: true, geometryEdited: true);
    string axisLabel = $"axis ({axisNormalized.X:+0.00;-0.00;0.00},{axisNormalized.Y:+0.00;-0.00;0.00},{axisNormalized.Z:+0.00;-0.00;0.00})";
    _statusBaseText = primitiveMode
      ? $"Rotated BSP primitive selection ({selectedVertices.Length} vertex key(s), {selectedEdges.Length} edge(s)) around {axisLabel} by {degreesY:+0.0;-0.0;0.0} deg"
      : $"Rotated BSP selection ({selectedObjectIds.Length} object(s), {selectedFaces.Length} face seed(s)) around {axisLabel} by {degreesY:+0.0;-0.0;0.0} deg";
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

  private void ApplyEditedMap(
    LoadedMap editedMap,
    int preferredSelectionIndex = -1,
    bool trackUndo = true,
    bool clearRedo = true,
    bool geometryEdited = false)
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
    if (_mapDocument == null)
    {
      _mapDocument = MapDocument.FromLoadedMap(editedMap);
    }
    else
    {
      _mapDocument.ReplaceMap(editedMap);
    }
    if (geometryEdited)
    {
      _viewer.ApplyMapGeometryEdited(editedMap);
    }
    else
    {
      _viewer.ApplyCollisionEditedMap(editedMap);
    }
    _viewer.SetSelectedCollisionLineIndex(selectionIndex);
    SyncTeleportInputsFromCamera();
    UpdateUndoUiState();
  }

  private void EditEnvironmentFromDialog()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "R3X Environment", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    using R3xEnvironmentDialog dialog = new(_loadedMap.Environment);
    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    LoadedMap editedMap;
    if (_mapDocument == null)
    {
      editedMap = MapEditOperations.WithEnvironmentData(_loadedMap, dialog.EditedSettings);
      _mapDocument = MapDocument.FromLoadedMap(editedMap);
    }
    else
    {
      editedMap = _mapDocument.ApplyEnvironment(dialog.EditedSettings);
    }

    _loadedMap = editedMap;
    _viewer.ApplyCollisionEditedMap(editedMap);
    _statusBaseText =
      $"R3X environment updated | Fog: {(editedMap.Environment.FogEnabled ? $"On({editedMap.Environment.FogStart:F0}-{editedMap.Environment.FogEnd:F0})" : "Off")}";
    RefreshRuntimeStatus();
  }

  private void EditMapMaterialsFromDialog()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "R3M Materials", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    if (_loadedMap.Materials.Length == 0)
    {
      MessageBox.Show(this, "This map has no loaded R3M material data.", "R3M Materials", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    using R3mMaterialsDialog dialog = new(_loadedMap.Materials);
    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    LoadedMap editedMap;
    if (_mapDocument == null)
    {
      editedMap = MapEditOperations.WithMapMaterialData(_loadedMap, dialog.EditedMaterials);
      _mapDocument = MapDocument.FromLoadedMap(editedMap);
    }
    else
    {
      editedMap = _mapDocument.ApplyMapMaterials(dialog.EditedMaterials);
    }

    ApplyNonCollisionEditedMap(editedMap);
    _statusBaseText = $"R3M materials updated | Count: {editedMap.Materials.Length:N0}";
    RefreshRuntimeStatus();
  }

  private void EditMapTexturesFromDialog()
  {
    if (_loadedMap == null)
    {
      MessageBox.Show(this, "No map is loaded.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    if (_loadedMap.SurfaceTextures.Length == 0)
    {
      MessageBox.Show(this, "This map has no loaded R3T texture data.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    using R3tTexturesDialog dialog = new(_loadedMap.SurfaceTextures);
    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    LoadedMap editedMap;
    if (_mapDocument == null)
    {
      editedMap = MapEditOperations.WithMapTextureData(_loadedMap, dialog.EditedTextures);
      _mapDocument = MapDocument.FromLoadedMap(editedMap);
    }
    else
    {
      editedMap = _mapDocument.ApplyMapTextures(dialog.EditedTextures);
    }

    ApplyNonCollisionEditedMap(editedMap);
    _statusBaseText = $"R3T textures updated | Count: {editedMap.SurfaceTextures.Length:N0}";
    RefreshRuntimeStatus();
  }

  private void ApplyNonCollisionEditedMap(LoadedMap editedMap)
  {
    (float x, float y, float z, float yaw, float pitch) pose = _viewer.GetCameraDisplayPose();
    int selectionIndex = _viewer.SelectedCollisionLineIndex;
    _loadedMap = editedMap;
    if (_mapDocument == null)
    {
      _mapDocument = MapDocument.FromLoadedMap(editedMap);
    }
    else
    {
      _mapDocument.ReplaceMap(editedMap);
    }

    _viewer.SetMap(editedMap);
    _ = _viewer.TrySetCameraDisplayPose(pose.x, pose.y, pose.z, pose.yaw, pose.pitch);
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

    ApplyEditedMap(previousMap, trackUndo: false, clearRedo: false, geometryEdited: true);
    _statusBaseText =
      $"Undo map edit | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l"
      + $" | BSP TriVerts: {_loadedMap.BspRenderVertices.Length:N0}"
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

    ApplyEditedMap(nextMap, trackUndo: false, clearRedo: false, geometryEdited: true);
    _statusBaseText =
      $"Redo map edit | Collision: {_loadedMap.CollisionVertices.Length:N0}v/{_loadedMap.CollisionLines.Length:N0}l"
      + $" | BSP TriVerts: {_loadedMap.BspRenderVertices.Length:N0}"
      + $" | Undo: {_editUndoHistory.Count:N0} | Redo left: {_editRedoHistory.Count:N0}";
    RefreshRuntimeStatus();
  }

  private void UpdateUndoUiState()
  {
    bool hasMap = _loadedMap != null;
    bool canUndo = hasMap && _editUndoHistory.Count > 0;
    bool canRedo = hasMap && _editRedoHistory.Count > 0;
    bool hasCollisionSelection =
      hasMap &&
      _viewer.SelectedCollisionLineIndex >= 0 &&
      _viewer.SelectedCollisionLineIndex < _loadedMap!.CollisionLines.Length;
    bool hasBspSelection = hasMap && _viewer.HasAnyBspSelection;
    ActiveDeleteMode deleteMode = GetActiveDeleteMode();
    bool canDeleteSelected = deleteMode switch
    {
      ActiveDeleteMode.Collision => hasCollisionSelection,
      ActiveDeleteMode.Bsp => hasBspSelection,
      _ => false,
    };
    _undoCollisionButton.Enabled = canUndo;
    _undoMenuItem.Enabled = canUndo;
    _redoCollisionButton.Enabled = canRedo;
    _redoMenuItem.Enabled = canRedo;
    _deleteSelectedCollisionButton.Enabled = canDeleteSelected;
    _deleteSelectedCollisionMenuItem.Enabled = canDeleteSelected;
    _deleteSelectedCollisionButton.ToolTipText = deleteMode switch
    {
      ActiveDeleteMode.Collision => "Delete selected collision wall segment (active mode: SelWall)",
      ActiveDeleteMode.Bsp => "Delete selected BSP object/face/edge/vertex (active mode: BSP Select/BSP Move)",
      _ => "Enable SelWall or BSP Select/BSP Move to delete selection",
    };
    _deleteSelectedCollisionMenuItem.Text = deleteMode switch
    {
      ActiveDeleteMode.Bsp => "Delete Selected BSP",
      ActiveDeleteMode.Collision => "Delete Selected Collision",
      _ => "Delete Selected",
    };
    _bspDeleteButton.Enabled = hasBspSelection;
    _bspCopyButton.Enabled = hasBspSelection;
    _bspMergeButton.Enabled = hasBspSelection;
    _bspMoveNegXButton.Enabled = hasBspSelection;
    _bspMovePosXButton.Enabled = hasBspSelection;
    _bspMoveNegYButton.Enabled = hasBspSelection;
    _bspMovePosYButton.Enabled = hasBspSelection;
    _bspMoveNegZButton.Enabled = hasBspSelection;
    _bspMovePosZButton.Enabled = hasBspSelection;
    _bspScaleDownButton.Enabled = hasBspSelection;
    _bspScaleUpButton.Enabled = hasBspSelection;
    _bspRotateNegYButton.Enabled = hasBspSelection;
    _bspRotatePosYButton.Enabled = hasBspSelection;
  }

  private void OnMainFormKeyDown(object? sender, KeyEventArgs e)
  {
    if (!e.Control && !e.Alt && e.KeyCode == Keys.Delete)
    {
      e.Handled = true;
      e.SuppressKeyPress = true;
      DeleteSelectionFromActiveModeFromUi();
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

  private static bool TryValidateMapForSave(LoadedMap map, out string error)
  {
    if (!TryValidateCollisionForSave(map, out error))
    {
      return false;
    }

    try
    {
      BspStrictValidator.ValidateForSave(map);
      error = string.Empty;
      return true;
    }
    catch (Exception ex)
    {
      error = ex.Message;
      return false;
    }
  }

  private void OnStrictLoadModeChanged()
  {
    if (!_strictLoadModeMenuItem.Checked)
    {
      _strictLoadModeMenuItem.Checked = true;
      return;
    }

    _strictLoadModeEnabled = true;
    _statusBaseText = "Strict load mode: ON (locked)";
    RefreshRuntimeStatus();
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

  private readonly record struct ServerInspectorModeItem(string Label, ServerInspectorMode Mode)
  {
    public override string ToString() => Label;
  }

  private readonly record struct ServerNavigationTarget(string Label, Vector3 Position);
  private interface IServerInspectorItem
  {
    ServerInspectorMode Mode { get; }
    string Label { get; }
    ServerDummyPosition Dummy { get; }
  }

  private sealed class ServerDummyInspectorItem : IServerInspectorItem
  {
    public required ServerInspectorMode Mode { get; init; }
    public required string Label { get; init; }
    public required ServerDummyPosition Dummy { get; init; }
  }

  private sealed class ServerMonsterBlockInspectorItem : IServerInspectorItem
  {
    public ServerInspectorMode Mode => ServerInspectorMode.Monsters;
    public required string Label { get; init; }
    public required ServerDummyPosition Dummy { get; init; }
    public required ServerMapMonsterBlock Block { get; init; }
    public required string MonsterSummary { get; init; }
    public required string[] MonsterCodes { get; init; }
    public required string[] MonsterNames { get; init; }
    public required int SpawnCount { get; init; }
    public required ulong TotalRegenLimit { get; init; }
    public required int SetCount { get; init; }
  }

  private sealed class ServerMonsterDummyInspectorItem : IServerInspectorItem
  {
    public ServerInspectorMode Mode => ServerInspectorMode.Monsters;
    public required string Label { get; init; }
    public required ServerDummyPosition Dummy { get; init; }
    public required ServerMapMonsterDummyRef DummyRef { get; init; }
    public required ServerMapMonsterBlock Block { get; init; }
  }

  private sealed class ServerMonsterSpawnInspectorItem
  {
    public required ServerMapMonsterBlock Block { get; init; }
    public required int SetIndex { get; init; }
    public required ServerMapMonsterSpawn Spawn { get; init; }
  }

  private sealed class ServerMonsterDefinitionComboItem
  {
    public required ServerMonsterDefinition Definition { get; init; }

    public override string ToString()
    {
      return string.IsNullOrWhiteSpace(Definition.DisplayName)
        ? Definition.Code
        : $"{Definition.Code} - {Definition.DisplayName}";
    }
  }

  private readonly record struct MonsterSpawnSetComboItem(int SetIndex)
  {
    public override string ToString() => $"Set {SetIndex}";
  }

  private readonly record struct MapListEntry(string Name, string FolderPath, string BspPath, string EbpPath);
}
