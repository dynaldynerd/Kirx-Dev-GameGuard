#nullable disable

namespace MapEditor;

partial class MainForm
{
  private System.ComponentModel.IContainer components = null;
  private MenuStrip _mainMenuStrip = null!;
  private ToolStripMenuItem _fileMenuItem = null!;
  private ToolStripMenuItem _openMapFolderMenuItem = null!;
  private ToolStripMenuItem _saveEbpOnlyMenuItem = null!;
  private ToolStripMenuItem _saveMapAsMenuItem = null!;
  private ToolStripSeparator _fileBlenderSeparator = null!;
  private ToolStripMenuItem _exportBlenderPackageMenuItem = null!;
  private ToolStripMenuItem _importBlenderPackageMenuItem = null!;
  private ToolStripSeparator _fileMenuSeparator = null!;
  private ToolStripMenuItem _exitMenuItem = null!;
  private ToolStripMenuItem _editMenuItem = null!;
  private ToolStripMenuItem _undoMenuItem = null!;
  private ToolStripMenuItem _redoMenuItem = null!;
  private ToolStripMenuItem _editMapMaterialsMenuItem = null!;
  private ToolStripMenuItem _editMapTexturesMenuItem = null!;
  private ToolStripMenuItem _editEnvironmentMenuItem = null!;
  private ToolStripSeparator _editMenuSeparator = null!;
  private ToolStripMenuItem _deleteSelectedCollisionMenuItem = null!;
  private ToolStripMenuItem _viewMenuItem = null!;
  private ToolStripMenuItem _showSpeedStripMenuItem = null!;
  private ToolStripMenuItem _showCollisionStripMenuItem = null!;
  private ToolStripMenuItem _showBspEditorStripMenuItem = null!;
  private ToolStripMenuItem _strictLoadModeMenuItem = null!;
  private ToolStripMenuItem _helpMenuItem = null!;
  private ToolStripMenuItem _controlsMenuItem = null!;
  private ToolStrip _speedStrip = null!;
  private ToolStripLabel _moveSpeedLabel = null!;
  private ToolStripControlHost _speedHost = null!;
  private ToolStripLabel _moveSpeedUnitLabel = null!;
  private ToolStripSeparator _speedSeparator1 = null!;
  private ToolStripLabel _camXLabel = null!;
  private ToolStripControlHost _teleportXHost = null!;
  private ToolStripLabel _camYLabel = null!;
  private ToolStripControlHost _teleportYHost = null!;
  private ToolStripLabel _camZLabel = null!;
  private ToolStripControlHost _teleportZHost = null!;
  private ToolStripButton _goToCoordinateButton = null!;
  private ToolStripSeparator _speedSeparator2 = null!;
  private ToolStripLabel _mapLabel = null!;
  private ToolStripComboBox _mapComboBox = null!;
  private ToolStripSeparator _speedSeparator3 = null!;
  private ToolStripDropDownButton _skySourceButton = null!;
  private ToolStripMenuItem _skyMenuItem = null!;
  private ToolStripMenuItem _sky2MenuItem = null!;
  private ToolStripDropDownButton _pipelineButton = null!;
  private ToolStripMenuItem _legacyPipelineMenuItem = null!;
  private ToolStripMenuItem _parityPipelineMenuItem = null!;
  private ToolStripButton _northSouthFlipButton = null!;
  private ToolStripSeparator _speedSeparator4 = null!;
  private ToolStripButton _skyRenderButton = null!;
  private ToolStripButton _r3tButton = null!;
  private ToolStripButton _r3mButton = null!;
  private ToolStripButton _r3eButton = null!;
  private ToolStripButton _r3xButton = null!;
  private ToolStripSeparator _speedSeparator5 = null!;
  private ToolStripButton _dynamicLightButton = null!;
  private ToolStripButton _particleMarkersButton = null!;
  private ToolStrip _collisionStrip = null!;
  private ToolStripButton _collisionButton = null!;
  private ToolStripButton _gridButton = null!;
  private ToolStripButton _mouseDrawCollisionButton = null!;
  private ToolStripButton _selectCollisionButton = null!;
  private ToolStripButton _deleteSelectedCollisionButton = null!;
  private ToolStripSeparator _collisionSeparator1 = null!;
  private ToolStripLabel _boundaryMarginLabel = null!;
  private ToolStripControlHost _boundaryMarginHost = null!;
  private ToolStripLabel _boundaryHeightLabel = null!;
  private ToolStripControlHost _boundaryHeightHost = null!;
  private ToolStripLabel _boundaryBuryLabel = null!;
  private ToolStripControlHost _boundaryEmbedHost = null!;
  private ToolStripLabel _boundarySnapLabel = null!;
  private ToolStripControlHost _boundarySnapHost = null!;
  private ToolStripButton _undoCollisionButton = null!;
  private ToolStripButton _redoCollisionButton = null!;
  private ToolStripButton _resetEditedCollisionButton = null!;
  private ToolStrip _bspEditorStrip = null!;
  private ToolStripButton _bspSelectModeButton = null!;
  private ToolStripButton _bspDeleteButton = null!;
  private ToolStripSeparator _bspEditorSeparator1 = null!;
  private ToolStripLabel _bspMoveStepLabel = null!;
  private ToolStripControlHost _bspMoveStepHost = null!;
  private ToolStripButton _bspMoveNegXButton = null!;
  private ToolStripButton _bspMovePosXButton = null!;
  private ToolStripButton _bspMoveNegYButton = null!;
  private ToolStripButton _bspMovePosYButton = null!;
  private ToolStripButton _bspMoveNegZButton = null!;
  private ToolStripButton _bspMovePosZButton = null!;
  private StatusStrip _statusStrip = null!;
  private ToolStripStatusLabel _statusLabel = null!;
  private Panel _viewerHostPanel = null!;
  private Label _viewerHintLabel = null!;
  private NumericUpDown _speedUpDown = null!;
  private NumericUpDown _teleportXUpDown = null!;
  private NumericUpDown _teleportYUpDown = null!;
  private NumericUpDown _teleportZUpDown = null!;
  private NumericUpDown _boundaryMarginUpDown = null!;
  private NumericUpDown _boundaryHeightUpDown = null!;
  private NumericUpDown _boundaryEmbedDepthUpDown = null!;
  private NumericUpDown _boundarySnapDistanceUpDown = null!;
  private NumericUpDown _bspMoveStepUpDown = null!;
  private System.Windows.Forms.Timer _statusTimer = null!;

  protected override void Dispose(bool disposing)
  {
    if (disposing)
    {
      components?.Dispose();
    }

    base.Dispose(disposing);
  }

  private void InitializeComponent()
  {
    components = new System.ComponentModel.Container();
    _mainMenuStrip = new MenuStrip();
    _fileMenuItem = new ToolStripMenuItem();
    _openMapFolderMenuItem = new ToolStripMenuItem();
    _saveEbpOnlyMenuItem = new ToolStripMenuItem();
    _saveMapAsMenuItem = new ToolStripMenuItem();
    _fileBlenderSeparator = new ToolStripSeparator();
    _exportBlenderPackageMenuItem = new ToolStripMenuItem();
    _importBlenderPackageMenuItem = new ToolStripMenuItem();
    _fileMenuSeparator = new ToolStripSeparator();
    _exitMenuItem = new ToolStripMenuItem();
    _editMenuItem = new ToolStripMenuItem();
    _undoMenuItem = new ToolStripMenuItem();
    _redoMenuItem = new ToolStripMenuItem();
    _editMapMaterialsMenuItem = new ToolStripMenuItem();
    _editMapTexturesMenuItem = new ToolStripMenuItem();
    _editEnvironmentMenuItem = new ToolStripMenuItem();
    _editMenuSeparator = new ToolStripSeparator();
    _deleteSelectedCollisionMenuItem = new ToolStripMenuItem();
    _viewMenuItem = new ToolStripMenuItem();
    _showSpeedStripMenuItem = new ToolStripMenuItem();
    _showCollisionStripMenuItem = new ToolStripMenuItem();
    _showBspEditorStripMenuItem = new ToolStripMenuItem();
    _strictLoadModeMenuItem = new ToolStripMenuItem();
    _helpMenuItem = new ToolStripMenuItem();
    _controlsMenuItem = new ToolStripMenuItem();
    _speedStrip = new ToolStrip();
    _moveSpeedLabel = new ToolStripLabel();
    _speedUpDown = new NumericUpDown();
    _speedHost = new ToolStripControlHost(_speedUpDown);
    _moveSpeedUnitLabel = new ToolStripLabel();
    _speedSeparator1 = new ToolStripSeparator();
    _camXLabel = new ToolStripLabel();
    _teleportXUpDown = new NumericUpDown();
    _teleportXHost = new ToolStripControlHost(_teleportXUpDown);
    _camYLabel = new ToolStripLabel();
    _teleportYUpDown = new NumericUpDown();
    _teleportYHost = new ToolStripControlHost(_teleportYUpDown);
    _camZLabel = new ToolStripLabel();
    _teleportZUpDown = new NumericUpDown();
    _teleportZHost = new ToolStripControlHost(_teleportZUpDown);
    _goToCoordinateButton = new ToolStripButton();
    _speedSeparator2 = new ToolStripSeparator();
    _mapLabel = new ToolStripLabel();
    _mapComboBox = new ToolStripComboBox();
    _speedSeparator3 = new ToolStripSeparator();
    _skySourceButton = new ToolStripDropDownButton();
    _skyMenuItem = new ToolStripMenuItem();
    _sky2MenuItem = new ToolStripMenuItem();
    _pipelineButton = new ToolStripDropDownButton();
    _legacyPipelineMenuItem = new ToolStripMenuItem();
    _parityPipelineMenuItem = new ToolStripMenuItem();
    _northSouthFlipButton = new ToolStripButton();
    _speedSeparator4 = new ToolStripSeparator();
    _skyRenderButton = new ToolStripButton();
    _r3tButton = new ToolStripButton();
    _r3mButton = new ToolStripButton();
    _r3eButton = new ToolStripButton();
    _r3xButton = new ToolStripButton();
    _speedSeparator5 = new ToolStripSeparator();
    _dynamicLightButton = new ToolStripButton();
    _particleMarkersButton = new ToolStripButton();
    _collisionStrip = new ToolStrip();
    _collisionButton = new ToolStripButton();
    _gridButton = new ToolStripButton();
    _mouseDrawCollisionButton = new ToolStripButton();
    _selectCollisionButton = new ToolStripButton();
    _deleteSelectedCollisionButton = new ToolStripButton();
    _collisionSeparator1 = new ToolStripSeparator();
    _boundaryMarginLabel = new ToolStripLabel();
    _boundaryMarginUpDown = new NumericUpDown();
    _boundaryMarginHost = new ToolStripControlHost(_boundaryMarginUpDown);
    _boundaryHeightLabel = new ToolStripLabel();
    _boundaryHeightUpDown = new NumericUpDown();
    _boundaryHeightHost = new ToolStripControlHost(_boundaryHeightUpDown);
    _boundaryBuryLabel = new ToolStripLabel();
    _boundaryEmbedDepthUpDown = new NumericUpDown();
    _boundaryEmbedHost = new ToolStripControlHost(_boundaryEmbedDepthUpDown);
    _boundarySnapLabel = new ToolStripLabel();
    _boundarySnapDistanceUpDown = new NumericUpDown();
    _boundarySnapHost = new ToolStripControlHost(_boundarySnapDistanceUpDown);
    _undoCollisionButton = new ToolStripButton();
    _redoCollisionButton = new ToolStripButton();
    _resetEditedCollisionButton = new ToolStripButton();
    _bspEditorStrip = new ToolStrip();
    _bspSelectModeButton = new ToolStripButton();
    _bspDeleteButton = new ToolStripButton();
    _bspEditorSeparator1 = new ToolStripSeparator();
    _bspMoveStepLabel = new ToolStripLabel();
    _bspMoveStepUpDown = new NumericUpDown();
    _bspMoveStepHost = new ToolStripControlHost(_bspMoveStepUpDown);
    _bspMoveNegXButton = new ToolStripButton();
    _bspMovePosXButton = new ToolStripButton();
    _bspMoveNegYButton = new ToolStripButton();
    _bspMovePosYButton = new ToolStripButton();
    _bspMoveNegZButton = new ToolStripButton();
    _bspMovePosZButton = new ToolStripButton();
    _statusStrip = new StatusStrip();
    _statusLabel = new ToolStripStatusLabel();
    _viewerHostPanel = new Panel();
    _viewerHintLabel = new Label();
    _statusTimer = new System.Windows.Forms.Timer(components);
    SuspendLayout();

    _mainMenuStrip.Items.AddRange(new ToolStripItem[]
    {
      _fileMenuItem,
      _editMenuItem,
      _viewMenuItem,
      _helpMenuItem,
    });
    _mainMenuStrip.Location = new Point(0, 0);
    _mainMenuStrip.Name = "_mainMenuStrip";
    _mainMenuStrip.Size = new Size(1600, 24);
    _mainMenuStrip.TabIndex = 0;

    _fileMenuItem.DropDownItems.AddRange(new ToolStripItem[]
    {
      _openMapFolderMenuItem,
      _saveEbpOnlyMenuItem,
      _saveMapAsMenuItem,
      _fileBlenderSeparator,
      _exportBlenderPackageMenuItem,
      _importBlenderPackageMenuItem,
      _fileMenuSeparator,
      _exitMenuItem,
    });
    _fileMenuItem.Name = "_fileMenuItem";
    _fileMenuItem.Text = "&File";

    _openMapFolderMenuItem.Name = "_openMapFolderMenuItem";
    _openMapFolderMenuItem.ShortcutKeys = Keys.Control | Keys.O;
    _openMapFolderMenuItem.Text = "&Open Map Folder...";

    _saveEbpOnlyMenuItem.Name = "_saveEbpOnlyMenuItem";
    _saveEbpOnlyMenuItem.ShortcutKeys = Keys.Control | Keys.S;
    _saveEbpOnlyMenuItem.Text = "Save &EBP Only...";

    _saveMapAsMenuItem.Name = "_saveMapAsMenuItem";
    _saveMapAsMenuItem.ShortcutKeys = Keys.Control | Keys.Shift | Keys.S;
    _saveMapAsMenuItem.Text = "Save Map &As...";

    _fileBlenderSeparator.Name = "_fileBlenderSeparator";

    _exportBlenderPackageMenuItem.Name = "_exportBlenderPackageMenuItem";
    _exportBlenderPackageMenuItem.ShortcutKeys = Keys.Control | Keys.Shift | Keys.E;
    _exportBlenderPackageMenuItem.Text = "Export &Blender Package...";
    _exportBlenderPackageMenuItem.ToolTipText = "Export map mesh/materials/textures/entities for Blender workflow";

    _importBlenderPackageMenuItem.Name = "_importBlenderPackageMenuItem";
    _importBlenderPackageMenuItem.ShortcutKeys = Keys.Control | Keys.Shift | Keys.I;
    _importBlenderPackageMenuItem.Text = "Import B&lender Package...";
    _importBlenderPackageMenuItem.ToolTipText = "Import map materials/textures/entities from Blender package folder";

    _fileMenuSeparator.Name = "_fileMenuSeparator";

    _exitMenuItem.Name = "_exitMenuItem";
    _exitMenuItem.ShortcutKeys = Keys.Alt | Keys.F4;
    _exitMenuItem.Text = "E&xit";

    _editMenuItem.DropDownItems.AddRange(new ToolStripItem[]
    {
      _undoMenuItem,
      _redoMenuItem,
      _editMapMaterialsMenuItem,
      _editMapTexturesMenuItem,
      _editEnvironmentMenuItem,
      _editMenuSeparator,
      _deleteSelectedCollisionMenuItem,
    });
    _editMenuItem.Name = "_editMenuItem";
    _editMenuItem.Text = "&Edit";

    _undoMenuItem.Enabled = false;
    _undoMenuItem.Name = "_undoMenuItem";
    _undoMenuItem.ShortcutKeys = Keys.Control | Keys.Z;
    _undoMenuItem.Text = "&Undo Collision Edit";
    _undoMenuItem.ToolTipText = "Undo last collision edit without resetting the whole map";

    _redoMenuItem.Enabled = false;
    _redoMenuItem.Name = "_redoMenuItem";
    _redoMenuItem.ShortcutKeys = Keys.Control | Keys.Y;
    _redoMenuItem.Text = "&Redo Collision Edit";
    _redoMenuItem.ToolTipText = "Redo the last undone collision edit";

    _editMapMaterialsMenuItem.Name = "_editMapMaterialsMenuItem";
    _editMapMaterialsMenuItem.ShortcutKeys = Keys.Control | Keys.M;
    _editMapMaterialsMenuItem.Text = "R3&M Materials...";
    _editMapMaterialsMenuItem.ToolTipText = "Edit map R3M materials";

    _editMapTexturesMenuItem.Name = "_editMapTexturesMenuItem";
    _editMapTexturesMenuItem.ShortcutKeys = Keys.Control | Keys.T;
    _editMapTexturesMenuItem.Text = "R3&T Textures...";
    _editMapTexturesMenuItem.ToolTipText = "Edit map R3T textures";

    _editEnvironmentMenuItem.Name = "_editEnvironmentMenuItem";
    _editEnvironmentMenuItem.ShortcutKeys = Keys.Control | Keys.E;
    _editEnvironmentMenuItem.Text = "R3X &Environment...";
    _editEnvironmentMenuItem.ToolTipText = "Edit map fog/environment values saved to R3X";

    _editMenuSeparator.Name = "_editMenuSeparator";

    _deleteSelectedCollisionMenuItem.Enabled = false;
    _deleteSelectedCollisionMenuItem.Name = "_deleteSelectedCollisionMenuItem";
    _deleteSelectedCollisionMenuItem.ShortcutKeys = Keys.Delete;
    _deleteSelectedCollisionMenuItem.Text = "&Delete Selected Collision";
    _deleteSelectedCollisionMenuItem.ToolTipText = "Delete currently selected collision wall segment";

    _viewMenuItem.DropDownItems.AddRange(new ToolStripItem[]
    {
      _showSpeedStripMenuItem,
      _showCollisionStripMenuItem,
      _showBspEditorStripMenuItem,
      _strictLoadModeMenuItem,
    });
    _viewMenuItem.Name = "_viewMenuItem";
    _viewMenuItem.Text = "&View";

    _showSpeedStripMenuItem.Checked = true;
    _showSpeedStripMenuItem.CheckOnClick = true;
    _showSpeedStripMenuItem.CheckState = CheckState.Checked;
    _showSpeedStripMenuItem.Name = "_showSpeedStripMenuItem";
    _showSpeedStripMenuItem.Text = "Show Main Toolbar";

    _showCollisionStripMenuItem.Checked = true;
    _showCollisionStripMenuItem.CheckOnClick = true;
    _showCollisionStripMenuItem.CheckState = CheckState.Checked;
    _showCollisionStripMenuItem.Name = "_showCollisionStripMenuItem";
    _showCollisionStripMenuItem.Text = "Show Collision Toolbar";

    _showBspEditorStripMenuItem.Checked = true;
    _showBspEditorStripMenuItem.CheckOnClick = true;
    _showBspEditorStripMenuItem.CheckState = CheckState.Checked;
    _showBspEditorStripMenuItem.Name = "_showBspEditorStripMenuItem";
    _showBspEditorStripMenuItem.Text = "Show BSP Editor Toolbar";

    _strictLoadModeMenuItem.Checked = true;
    _strictLoadModeMenuItem.CheckOnClick = true;
    _strictLoadModeMenuItem.CheckState = CheckState.Checked;
    _strictLoadModeMenuItem.Name = "_strictLoadModeMenuItem";
    _strictLoadModeMenuItem.Text = "Strict Load Mode";
    _strictLoadModeMenuItem.ToolTipText = "When enabled, malformed BSP/EBP is rejected instead of tolerated.";

    _helpMenuItem.DropDownItems.AddRange(new ToolStripItem[] { _controlsMenuItem });
    _helpMenuItem.Name = "_helpMenuItem";
    _helpMenuItem.Text = "&Help";

    _controlsMenuItem.Name = "_controlsMenuItem";
    _controlsMenuItem.ShortcutKeys = Keys.F1;
    _controlsMenuItem.Text = "&Controls";

    _speedStrip.Dock = DockStyle.Top;
    _speedStrip.GripStyle = ToolStripGripStyle.Hidden;
    _speedStrip.Items.AddRange(new ToolStripItem[]
    {
      _moveSpeedLabel,
      _speedHost,
      _moveSpeedUnitLabel,
      _speedSeparator1,
      _camXLabel,
      _teleportXHost,
      _camYLabel,
      _teleportYHost,
      _camZLabel,
      _teleportZHost,
      _goToCoordinateButton,
      _speedSeparator2,
      _mapLabel,
      _mapComboBox,
      _speedSeparator3,
      _skySourceButton,
      _pipelineButton,
      _northSouthFlipButton,
      _speedSeparator4,
      _skyRenderButton,
      _r3tButton,
      _r3mButton,
      _r3eButton,
      _r3xButton,
      _speedSeparator5,
      _dynamicLightButton,
      _particleMarkersButton,
    });
    _speedStrip.Location = new Point(0, 24);
    _speedStrip.Name = "_speedStrip";
    _speedStrip.RenderMode = ToolStripRenderMode.System;
    _speedStrip.Size = new Size(1600, 25);
    _speedStrip.TabIndex = 1;

    _moveSpeedLabel.Name = "_moveSpeedLabel";
    _moveSpeedLabel.Text = "Move Speed";

    _speedUpDown.DecimalPlaces = 0;
    _speedUpDown.Increment = 10;
    _speedUpDown.Maximum = 50000;
    _speedUpDown.Minimum = 10;
    _speedUpDown.Name = "_speedUpDown";
    _speedUpDown.Size = new Size(90, 23);
    _speedUpDown.Value = 350;

    _speedHost.AutoSize = false;
    _speedHost.Name = "_speedHost";
    _speedHost.Size = new Size(90, 23);

    _moveSpeedUnitLabel.Name = "_moveSpeedUnitLabel";
    _moveSpeedUnitLabel.Text = "units/s";

    _speedSeparator1.Name = "_speedSeparator1";

    _camXLabel.Name = "_camXLabel";
    _camXLabel.Text = "Src X";

    _teleportXUpDown.DecimalPlaces = 2;
    _teleportXUpDown.Increment = 10;
    _teleportXUpDown.Maximum = 500000;
    _teleportXUpDown.Minimum = -500000;
    _teleportXUpDown.Name = "_teleportXUpDown";
    _teleportXUpDown.Size = new Size(88, 23);

    _teleportXHost.AutoSize = false;
    _teleportXHost.Name = "_teleportXHost";
    _teleportXHost.Size = new Size(88, 23);

    _camYLabel.Name = "_camYLabel";
    _camYLabel.Text = "Y";

    _teleportYUpDown.DecimalPlaces = 2;
    _teleportYUpDown.Increment = 10;
    _teleportYUpDown.Maximum = 500000;
    _teleportYUpDown.Minimum = -500000;
    _teleportYUpDown.Name = "_teleportYUpDown";
    _teleportYUpDown.Size = new Size(88, 23);

    _teleportYHost.AutoSize = false;
    _teleportYHost.Name = "_teleportYHost";
    _teleportYHost.Size = new Size(88, 23);

    _camZLabel.Name = "_camZLabel";
    _camZLabel.Text = "Z";

    _teleportZUpDown.DecimalPlaces = 2;
    _teleportZUpDown.Increment = 10;
    _teleportZUpDown.Maximum = 500000;
    _teleportZUpDown.Minimum = -500000;
    _teleportZUpDown.Name = "_teleportZUpDown";
    _teleportZUpDown.Size = new Size(88, 23);

    _teleportZHost.AutoSize = false;
    _teleportZHost.Name = "_teleportZHost";
    _teleportZHost.Size = new Size(88, 23);

    _goToCoordinateButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _goToCoordinateButton.Name = "_goToCoordinateButton";
    _goToCoordinateButton.Text = "Go";
    _goToCoordinateButton.ToolTipText = "Teleport camera to Cam XYZ values (source/game coordinates)";

    _speedSeparator2.Name = "_speedSeparator2";

    _mapLabel.Name = "_mapLabel";
    _mapLabel.Text = "Map";

    _mapComboBox.AutoSize = false;
    _mapComboBox.DropDownStyle = ComboBoxStyle.DropDownList;
    _mapComboBox.Enabled = false;
    _mapComboBox.Name = "_mapComboBox";
    _mapComboBox.Size = new Size(220, 25);
    _mapComboBox.ToolTipText = "Select a map folder from the loaded map root";

    _speedSeparator3.Name = "_speedSeparator3";

    _skySourceButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _skySourceButton.DropDownItems.AddRange(new ToolStripItem[]
    {
      _skyMenuItem,
      _sky2MenuItem,
    });
    _skySourceButton.Name = "_skySourceButton";
    _skySourceButton.Text = "Sky: sky2";
    _skySourceButton.ToolTipText = "Switch loaded sky source between sky and sky2";

    _skyMenuItem.Name = "_skyMenuItem";
    _skyMenuItem.Text = "Use sky";

    _sky2MenuItem.Checked = true;
    _sky2MenuItem.CheckState = CheckState.Checked;
    _sky2MenuItem.Name = "_sky2MenuItem";
    _sky2MenuItem.Text = "Use sky2";

    _pipelineButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _pipelineButton.DropDownItems.AddRange(new ToolStripItem[]
    {
      _legacyPipelineMenuItem,
      _parityPipelineMenuItem,
    });
    _pipelineButton.Name = "_pipelineButton";
    _pipelineButton.Text = "Pipe: R3Parity";
    _pipelineButton.ToolTipText = "Switch renderer pipeline between LegacyCompat and R3Parity";

    _legacyPipelineMenuItem.Name = "_legacyPipelineMenuItem";
    _legacyPipelineMenuItem.Text = "LegacyCompat";

    _parityPipelineMenuItem.Checked = true;
    _parityPipelineMenuItem.CheckState = CheckState.Checked;
    _parityPipelineMenuItem.Name = "_parityPipelineMenuItem";
    _parityPipelineMenuItem.Text = "R3Parity";

    _northSouthFlipButton.Checked = true;
    _northSouthFlipButton.CheckOnClick = true;
    _northSouthFlipButton.CheckState = CheckState.Checked;
    _northSouthFlipButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _northSouthFlipButton.Name = "_northSouthFlipButton";
    _northSouthFlipButton.Text = "NSFlip";
    _northSouthFlipButton.ToolTipText = "Display-only north/south mirror (debug/inspection).";

    _speedSeparator4.Name = "_speedSeparator4";

    _skyRenderButton.Checked = true;
    _skyRenderButton.CheckOnClick = true;
    _skyRenderButton.CheckState = CheckState.Checked;
    _skyRenderButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _skyRenderButton.Name = "_skyRenderButton";
    _skyRenderButton.Text = "Sky";
    _skyRenderButton.ToolTipText = "Enable/disable sky rendering";

    _r3tButton.Checked = true;
    _r3tButton.CheckOnClick = true;
    _r3tButton.CheckState = CheckState.Checked;
    _r3tButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _r3tButton.Name = "_r3tButton";
    _r3tButton.Text = "R3T";
    _r3tButton.ToolTipText = "Enable/disable texture usage from R3T/Lgt.R3T";

    _r3mButton.Checked = true;
    _r3mButton.CheckOnClick = true;
    _r3mButton.CheckState = CheckState.Checked;
    _r3mButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _r3mButton.Name = "_r3mButton";
    _r3mButton.Text = "R3M";
    _r3mButton.ToolTipText = "Enable/disable material/layer behavior from R3M";

    _r3eButton.Checked = true;
    _r3eButton.CheckOnClick = true;
    _r3eButton.CheckState = CheckState.Checked;
    _r3eButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _r3eButton.Name = "_r3eButton";
    _r3eButton.Text = "R3E";
    _r3eButton.ToolTipText = "Enable/disable map entity rendering from R3E";

    _r3xButton.Checked = true;
    _r3xButton.CheckOnClick = true;
    _r3xButton.CheckState = CheckState.Checked;
    _r3xButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _r3xButton.Name = "_r3xButton";
    _r3xButton.Text = "R3X";
    _r3xButton.ToolTipText = "Enable/disable environment settings (fog/clear color) from R3X + EXT.spt";

    _speedSeparator5.Name = "_speedSeparator5";

    _dynamicLightButton.Checked = true;
    _dynamicLightButton.CheckOnClick = true;
    _dynamicLightButton.CheckState = CheckState.Checked;
    _dynamicLightButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _dynamicLightButton.Name = "_dynamicLightButton";
    _dynamicLightButton.Text = "DynLight";
    _dynamicLightButton.ToolTipText = "Enable/disable dynamic lighting pass";

    _particleMarkersButton.Checked = false;
    _particleMarkersButton.CheckOnClick = true;
    _particleMarkersButton.CheckState = CheckState.Unchecked;
    _particleMarkersButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _particleMarkersButton.Name = "_particleMarkersButton";
    _particleMarkersButton.Text = "FxMark";
    _particleMarkersButton.ToolTipText = "Show/hide particle/effect instances and markers";

    _collisionStrip.Dock = DockStyle.Top;
    _collisionStrip.GripStyle = ToolStripGripStyle.Hidden;
    _collisionStrip.Items.AddRange(new ToolStripItem[]
    {
      _collisionButton,
      _gridButton,
      _mouseDrawCollisionButton,
      _selectCollisionButton,
      _deleteSelectedCollisionButton,
      _undoCollisionButton,
      _redoCollisionButton,
      _resetEditedCollisionButton,
      _collisionSeparator1,
      _boundaryMarginLabel,
      _boundaryMarginHost,
      _boundaryHeightLabel,
      _boundaryHeightHost,
      _boundaryBuryLabel,
      _boundaryEmbedHost,
      _boundarySnapLabel,
      _boundarySnapHost,
    });
    _collisionStrip.Location = new Point(0, 49);
    _collisionStrip.Name = "_collisionStrip";
    _collisionStrip.RenderMode = ToolStripRenderMode.System;
    _collisionStrip.Size = new Size(1600, 25);
    _collisionStrip.TabIndex = 2;

    _collisionButton.Checked = true;
    _collisionButton.CheckOnClick = true;
    _collisionButton.CheckState = CheckState.Checked;
    _collisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _collisionButton.Name = "_collisionButton";
    _collisionButton.Text = "Collision";
    _collisionButton.ToolTipText = "Show/hide EBP collision walls";

    _gridButton.CheckOnClick = true;
    _gridButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _gridButton.Name = "_gridButton";
    _gridButton.Text = "Grid";
    _gridButton.ToolTipText = "Show/hide white reference grid";

    _mouseDrawCollisionButton.CheckOnClick = true;
    _mouseDrawCollisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _mouseDrawCollisionButton.Name = "_mouseDrawCollisionButton";
    _mouseDrawCollisionButton.Text = "DrawWall";
    _mouseDrawCollisionButton.ToolTipText = "Mouse wall drawing mode: left click-drag-release to append one collision wall";

    _selectCollisionButton.CheckOnClick = true;
    _selectCollisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _selectCollisionButton.Name = "_selectCollisionButton";
    _selectCollisionButton.Text = "SelWall";
    _selectCollisionButton.ToolTipText = "Mouse collision selection mode: left click wall segment to select";

    _deleteSelectedCollisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _deleteSelectedCollisionButton.Enabled = false;
    _deleteSelectedCollisionButton.Name = "_deleteSelectedCollisionButton";
    _deleteSelectedCollisionButton.Text = "DelSel";
    _deleteSelectedCollisionButton.ToolTipText = "Delete selected collision wall segment (Delete)";

    _collisionSeparator1.Name = "_collisionSeparator1";

    _boundaryMarginLabel.Name = "_boundaryMarginLabel";
    _boundaryMarginLabel.Text = "B-Margin";

    _boundaryMarginUpDown.DecimalPlaces = 0;
    _boundaryMarginUpDown.Increment = 10;
    _boundaryMarginUpDown.Maximum = 50000;
    _boundaryMarginUpDown.Name = "_boundaryMarginUpDown";
    _boundaryMarginUpDown.Size = new Size(72, 23);
    _boundaryMarginUpDown.Value = 100;

    _boundaryMarginHost.AutoSize = false;
    _boundaryMarginHost.Name = "_boundaryMarginHost";
    _boundaryMarginHost.Size = new Size(72, 23);

    _boundaryHeightLabel.Name = "_boundaryHeightLabel";
    _boundaryHeightLabel.Text = "B-Height";

    _boundaryHeightUpDown.DecimalPlaces = 0;
    _boundaryHeightUpDown.Increment = 50;
    _boundaryHeightUpDown.Maximum = 50000;
    _boundaryHeightUpDown.Minimum = 1;
    _boundaryHeightUpDown.Name = "_boundaryHeightUpDown";
    _boundaryHeightUpDown.Size = new Size(72, 23);
    _boundaryHeightUpDown.Value = 3000;

    _boundaryHeightHost.AutoSize = false;
    _boundaryHeightHost.Name = "_boundaryHeightHost";
    _boundaryHeightHost.Size = new Size(72, 23);

    _boundaryBuryLabel.Name = "_boundaryBuryLabel";
    _boundaryBuryLabel.Text = "B-Bury";

    _boundaryEmbedDepthUpDown.DecimalPlaces = 0;
    _boundaryEmbedDepthUpDown.Increment = 10;
    _boundaryEmbedDepthUpDown.Maximum = 50000;
    _boundaryEmbedDepthUpDown.Name = "_boundaryEmbedDepthUpDown";
    _boundaryEmbedDepthUpDown.Size = new Size(72, 23);
    _boundaryEmbedDepthUpDown.Value = 80;

    _boundaryEmbedHost.AutoSize = false;
    _boundaryEmbedHost.Name = "_boundaryEmbedHost";
    _boundaryEmbedHost.Size = new Size(72, 23);

    _boundarySnapLabel.Name = "_boundarySnapLabel";
    _boundarySnapLabel.Text = "B-Snap";

    _boundarySnapDistanceUpDown.DecimalPlaces = 0;
    _boundarySnapDistanceUpDown.Increment = 5;
    _boundarySnapDistanceUpDown.Maximum = 10000;
    _boundarySnapDistanceUpDown.Name = "_boundarySnapDistanceUpDown";
    _boundarySnapDistanceUpDown.Size = new Size(72, 23);
    _boundarySnapDistanceUpDown.Value = 80;

    _boundarySnapHost.AutoSize = false;
    _boundarySnapHost.Name = "_boundarySnapHost";
    _boundarySnapHost.Size = new Size(72, 23);

    _undoCollisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _undoCollisionButton.Enabled = false;
    _undoCollisionButton.Name = "_undoCollisionButton";
    _undoCollisionButton.Text = "Undo";
    _undoCollisionButton.ToolTipText = "Undo last collision edit (Ctrl+Z)";

    _redoCollisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _redoCollisionButton.Enabled = false;
    _redoCollisionButton.Name = "_redoCollisionButton";
    _redoCollisionButton.Text = "Redo";
    _redoCollisionButton.ToolTipText = "Redo last undone collision edit (Ctrl+Y / Ctrl+Shift+Z)";

    _resetEditedCollisionButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _resetEditedCollisionButton.Name = "_resetEditedCollisionButton";
    _resetEditedCollisionButton.Text = "ResetMap";
    _resetEditedCollisionButton.ToolTipText = "Discard unsaved collision edits and reload current map from source BSP/EBP";

    _bspEditorStrip.Dock = DockStyle.Top;
    _bspEditorStrip.GripStyle = ToolStripGripStyle.Hidden;
    _bspEditorStrip.Items.AddRange(new ToolStripItem[]
    {
      _bspSelectModeButton,
      _bspDeleteButton,
      _bspEditorSeparator1,
      _bspMoveStepLabel,
      _bspMoveStepHost,
      _bspMoveNegXButton,
      _bspMovePosXButton,
      _bspMoveNegYButton,
      _bspMovePosYButton,
      _bspMoveNegZButton,
      _bspMovePosZButton,
    });
    _bspEditorStrip.Location = new Point(0, 74);
    _bspEditorStrip.Name = "_bspEditorStrip";
    _bspEditorStrip.RenderMode = ToolStripRenderMode.System;
    _bspEditorStrip.Size = new Size(1600, 25);
    _bspEditorStrip.TabIndex = 3;

    _bspSelectModeButton.CheckOnClick = true;
    _bspSelectModeButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspSelectModeButton.Name = "_bspSelectModeButton";
    _bspSelectModeButton.Text = "BSP Select";
    _bspSelectModeButton.ToolTipText = "Mouse select BSP mesh object/face in viewport";

    _bspDeleteButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspDeleteButton.Enabled = false;
    _bspDeleteButton.Name = "_bspDeleteButton";
    _bspDeleteButton.Text = "BSP Del";
    _bspDeleteButton.ToolTipText = "Delete selected BSP object/face";

    _bspEditorSeparator1.Name = "_bspEditorSeparator1";

    _bspMoveStepLabel.Name = "_bspMoveStepLabel";
    _bspMoveStepLabel.Text = "Step";

    _bspMoveStepUpDown.DecimalPlaces = 0;
    _bspMoveStepUpDown.Increment = 5;
    _bspMoveStepUpDown.Maximum = 5000;
    _bspMoveStepUpDown.Minimum = 1;
    _bspMoveStepUpDown.Name = "_bspMoveStepUpDown";
    _bspMoveStepUpDown.Size = new Size(72, 23);
    _bspMoveStepUpDown.Value = 50;

    _bspMoveStepHost.AutoSize = false;
    _bspMoveStepHost.Name = "_bspMoveStepHost";
    _bspMoveStepHost.Size = new Size(72, 23);

    _bspMoveNegXButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspMoveNegXButton.Enabled = false;
    _bspMoveNegXButton.Name = "_bspMoveNegXButton";
    _bspMoveNegXButton.Text = "X-";
    _bspMoveNegXButton.ToolTipText = "Move selection by -X";

    _bspMovePosXButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspMovePosXButton.Enabled = false;
    _bspMovePosXButton.Name = "_bspMovePosXButton";
    _bspMovePosXButton.Text = "X+";
    _bspMovePosXButton.ToolTipText = "Move selection by +X";

    _bspMoveNegYButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspMoveNegYButton.Enabled = false;
    _bspMoveNegYButton.Name = "_bspMoveNegYButton";
    _bspMoveNegYButton.Text = "Y-";
    _bspMoveNegYButton.ToolTipText = "Move selection by -Y";

    _bspMovePosYButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspMovePosYButton.Enabled = false;
    _bspMovePosYButton.Name = "_bspMovePosYButton";
    _bspMovePosYButton.Text = "Y+";
    _bspMovePosYButton.ToolTipText = "Move selection by +Y";

    _bspMoveNegZButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspMoveNegZButton.Enabled = false;
    _bspMoveNegZButton.Name = "_bspMoveNegZButton";
    _bspMoveNegZButton.Text = "Z-";
    _bspMoveNegZButton.ToolTipText = "Move selection by -Z";

    _bspMovePosZButton.DisplayStyle = ToolStripItemDisplayStyle.Text;
    _bspMovePosZButton.Enabled = false;
    _bspMovePosZButton.Name = "_bspMovePosZButton";
    _bspMovePosZButton.Text = "Z+";
    _bspMovePosZButton.ToolTipText = "Move selection by +Z";

    _statusStrip.Items.AddRange(new ToolStripItem[] { _statusLabel });
    _statusStrip.Location = new Point(0, 878);
    _statusStrip.Name = "_statusStrip";
    _statusStrip.Size = new Size(1600, 22);
    _statusStrip.TabIndex = 4;

    _statusLabel.Name = "_statusLabel";
    _statusLabel.Text = "Designer preview";

    _viewerHostPanel.BackColor = Color.FromArgb(24, 28, 34);
    _viewerHostPanel.Controls.Add(_viewerHintLabel);
    _viewerHostPanel.Dock = DockStyle.Fill;
    _viewerHostPanel.Location = new Point(0, 99);
    _viewerHostPanel.Name = "_viewerHostPanel";
    _viewerHostPanel.Size = new Size(1600, 779);
    _viewerHostPanel.TabIndex = 4;

    _viewerHintLabel.Dock = DockStyle.Fill;
    _viewerHintLabel.ForeColor = Color.Gainsboro;
    _viewerHintLabel.Location = new Point(0, 0);
    _viewerHintLabel.Name = "_viewerHintLabel";
    _viewerHintLabel.Size = new Size(1600, 779);
    _viewerHintLabel.TabIndex = 0;
    _viewerHintLabel.Text = "Runtime OpenGL viewport appears when app runs.";
    _viewerHintLabel.TextAlign = ContentAlignment.MiddleCenter;

    _statusTimer.Enabled = false;
    _statusTimer.Interval = 400;

    AutoScaleDimensions = new SizeF(7F, 15F);
    AutoScaleMode = AutoScaleMode.Font;
    ClientSize = new Size(1600, 900);
    Controls.Add(_viewerHostPanel);
    Controls.Add(_statusStrip);
    Controls.Add(_bspEditorStrip);
    Controls.Add(_collisionStrip);
    Controls.Add(_speedStrip);
    Controls.Add(_mainMenuStrip);
    KeyPreview = true;
    MainMenuStrip = _mainMenuStrip;
    MinimumSize = new Size(1200, 700);
    Name = "MainForm";
    StartPosition = FormStartPosition.CenterScreen;
    Text = "RF MapEditor (Viewer)";
    ResumeLayout(false);
    PerformLayout();
  }
}

#nullable restore
