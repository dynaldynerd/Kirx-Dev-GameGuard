using System.Globalization;
using MapEditor.Formats;

namespace MapEditor;

internal sealed class R3mMaterialsDialog : Form
{
  private readonly SplitContainer _split;
  private readonly ListBox _materialListBox;
  private readonly TextBox _flagsTextBox;
  private readonly NumericUpDown _detailSurfaceUpDown;
  private readonly NumericUpDown _detailScaleUpDown;
  private readonly Label _layerCountLabel;
  private readonly DataGridView _layersGrid;
  private readonly Button _addLayerButton;
  private readonly Button _removeLayerButton;
  private readonly Button _okButton;
  private readonly Button _cancelButton;
  private readonly EditableMaterial[] _materials;
  private bool _suppressEvents;
  private int _currentMaterialIndex = -1;

  public MaterialDefinition[] EditedMaterials { get; private set; }

  public R3mMaterialsDialog(MaterialDefinition[] sourceMaterials)
  {
    _materials = BuildEditableMaterials(sourceMaterials);
    EditedMaterials = sourceMaterials ?? Array.Empty<MaterialDefinition>();

    Text = "R3M Materials";
    FormBorderStyle = FormBorderStyle.FixedDialog;
    StartPosition = FormStartPosition.CenterParent;
    MinimizeBox = false;
    MaximizeBox = false;
    ShowInTaskbar = false;
    AutoScaleMode = AutoScaleMode.Font;
    ClientSize = new Size(1160, 620);
    MinimumSize = new Size(1240, 720);

    _split = new SplitContainer()
    {
      Dock = DockStyle.Fill,
      Orientation = Orientation.Vertical,
      IsSplitterFixed = false,
    };

    _materialListBox = new ListBox
    {
      Dock = DockStyle.Fill,
      IntegralHeight = false,
    };
    _materialListBox.SelectedIndexChanged += (_, _) => OnSelectedMaterialChanged();
    _split.Panel1.Controls.Add(_materialListBox);

    TableLayoutPanel editor = new()
    {
      Dock = DockStyle.Fill,
      ColumnCount = 2,
      RowCount = 6,
      Padding = new Padding(8),
    };
    editor.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 150));
    editor.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
    editor.RowStyles.Add(new RowStyle(SizeType.Absolute, 30));
    editor.RowStyles.Add(new RowStyle(SizeType.Absolute, 30));
    editor.RowStyles.Add(new RowStyle(SizeType.Absolute, 30));
    editor.RowStyles.Add(new RowStyle(SizeType.Absolute, 34));
    editor.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
    editor.RowStyles.Add(new RowStyle(SizeType.Absolute, 46));

    _flagsTextBox = new TextBox { Dock = DockStyle.Fill };
    _detailSurfaceUpDown = CreateIntInput(-2_000_000, 2_000_000);
    _detailScaleUpDown = CreateFloatInput(0, 1000, 3);
    _layerCountLabel = new Label
    {
      Dock = DockStyle.Fill,
      TextAlign = ContentAlignment.MiddleLeft,
      AutoSize = true,
    };

    AddEditorRow(editor, 0, "Material Flags", _flagsTextBox);
    AddEditorRow(editor, 1, "Detail Surface", _detailSurfaceUpDown);
    AddEditorRow(editor, 2, "Detail Scale", _detailScaleUpDown);
    AddEditorRow(editor, 3, "Layer Count", _layerCountLabel);

    Panel layerPanel = new() { Dock = DockStyle.Fill };
    _layersGrid = CreateLayersGrid();
    layerPanel.Controls.Add(_layersGrid);

    FlowLayoutPanel layerButtons = new()
    {
      Dock = DockStyle.Top,
      Height = 32,
      FlowDirection = FlowDirection.LeftToRight,
      WrapContents = false,
    };
    _addLayerButton = new Button { Text = "Add Layer", Width = 100 };
    _removeLayerButton = new Button { Text = "Remove Layer", Width = 110 };
    _addLayerButton.Click += OnAddLayerClicked;
    _removeLayerButton.Click += OnRemoveLayerClicked;
    layerButtons.Controls.Add(_addLayerButton);
    layerButtons.Controls.Add(_removeLayerButton);
    layerPanel.Controls.Add(layerButtons);

    editor.Controls.Add(layerPanel, 0, 4);
    editor.SetColumnSpan(layerPanel, 2);

    FlowLayoutPanel buttonPanel = new()
    {
      FlowDirection = FlowDirection.RightToLeft,
      Dock = DockStyle.Fill,
      AutoSize = true,
      WrapContents = false,
      Padding = new Padding(0, 8, 0, 0),
    };
    _okButton = new Button { Text = "OK", Width = 90 };
    _cancelButton = new Button { Text = "Cancel", Width = 90, DialogResult = DialogResult.Cancel };
    _okButton.Click += OnOkClicked;
    buttonPanel.Controls.Add(_okButton);
    buttonPanel.Controls.Add(_cancelButton);
    editor.Controls.Add(buttonPanel, 0, 5);
    editor.SetColumnSpan(buttonPanel, 2);

    _split.Panel2.Controls.Add(editor);
    Controls.Add(_split);

    _flagsTextBox.Leave += (_, _) => SaveCurrentMaterialFromUi(showErrors: false);
    _detailSurfaceUpDown.ValueChanged += (_, _) => SaveCurrentMaterialFromUi(showErrors: false);
    _detailScaleUpDown.ValueChanged += (_, _) => SaveCurrentMaterialFromUi(showErrors: false);
    _layersGrid.CellValueChanged += (_, _) => SaveCurrentMaterialFromUi(showErrors: false);
    _layersGrid.CurrentCellDirtyStateChanged += (_, _) =>
    {
      if (_layersGrid.IsCurrentCellDirty)
      {
        _layersGrid.CommitEdit(DataGridViewDataErrorContexts.Commit);
      }
    };

    PopulateMaterialList();
    if (_materialListBox.Items.Count > 0)
    {
      _materialListBox.SelectedIndex = 0;
    }

    Shown += (_, _) => EnsureEditorPanelVisible();
    Resize += (_, _) => EnsureEditorPanelVisible();
  }

  private void EnsureEditorPanelVisible()
  {
    if (!IsHandleCreated || _split.Width <= 0)
    {
      return;
    }

    const int desiredPanel1Min = 260;
    const int desiredPanel2Min = 840;

    int available = _split.Width - _split.SplitterWidth;
    if (available <= 1)
    {
      return;
    }

    int panel2Min = Math.Min(desiredPanel2Min, Math.Max(0, available - 1));
    int panel1Min = Math.Min(desiredPanel1Min, Math.Max(0, available - panel2Min));

    int min = panel1Min;
    int max = available - panel2Min;
    if (max < min)
    {
      min = Math.Max(0, max);
    }

    int preferred = Math.Clamp((int)(_split.Width * 0.35f), min, max);

    try
    {
      if (_split.Panel1MinSize != panel1Min)
      {
        _split.Panel1MinSize = panel1Min;
      }

      if (_split.Panel2MinSize != panel2Min)
      {
        _split.Panel2MinSize = panel2Min;
      }

      if (_split.SplitterDistance != preferred)
      {
        _split.SplitterDistance = preferred;
      }
    }
    catch (ArgumentOutOfRangeException)
    {
      // Ignore transient layout-time splitter bounds races.
      return;
    }
  }

  private static EditableMaterial[] BuildEditableMaterials(MaterialDefinition[] sourceMaterials)
  {
    MaterialDefinition[] source = sourceMaterials ?? Array.Empty<MaterialDefinition>();
    EditableMaterial[] editable = new EditableMaterial[source.Length];
    for (int materialIndex = 0; materialIndex < source.Length; ++materialIndex)
    {
      MaterialDefinition material = source[materialIndex];
      MaterialLayerDefinition[] sourceLayers = material.Layers ?? Array.Empty<MaterialLayerDefinition>();
      List<MaterialLayerDefinition> layers = new(sourceLayers.Length);
      for (int layerIndex = 0; layerIndex < sourceLayers.Length; ++layerIndex)
      {
        layers.Add(sourceLayers[layerIndex]);
      }

      editable[materialIndex] = new EditableMaterial
      {
        MaterialFlags = material.MaterialFlags,
        DetailSurface = material.DetailSurfaceId,
        DetailScale = material.DetailScale,
        Layers = layers,
      };
    }

    return editable;
  }

  private void PopulateMaterialList()
  {
    _materialListBox.Items.Clear();
    for (int materialIndex = 0; materialIndex < _materials.Length; ++materialIndex)
    {
      _materialListBox.Items.Add(BuildMaterialTitle(materialIndex, _materials[materialIndex].Layers.Count));
    }
  }

  private static string BuildMaterialTitle(int materialIndex, int layerCount)
  {
    return $"Material {materialIndex} ({layerCount} layer{(layerCount == 1 ? string.Empty : "s")})";
  }

  private void RefreshMaterialTitle(int materialIndex)
  {
    if ((uint)materialIndex >= (uint)_materials.Length || (uint)materialIndex >= (uint)_materialListBox.Items.Count)
    {
      return;
    }

    _materialListBox.Items[materialIndex] = BuildMaterialTitle(materialIndex, _materials[materialIndex].Layers.Count);
  }

  private void OnSelectedMaterialChanged()
  {
    if (!SaveCurrentMaterialFromUi(showErrors: true))
    {
      _suppressEvents = true;
      try
      {
        _materialListBox.SelectedIndex = _currentMaterialIndex;
      }
      finally
      {
        _suppressEvents = false;
      }
      return;
    }

    int selectedIndex = _materialListBox.SelectedIndex;
    if ((uint)selectedIndex >= (uint)_materials.Length)
    {
      return;
    }

    _currentMaterialIndex = selectedIndex;
    LoadCurrentMaterialToUi();
  }

  private void LoadCurrentMaterialToUi()
  {
    if ((uint)_currentMaterialIndex >= (uint)_materials.Length)
    {
      return;
    }

    EditableMaterial material = _materials[_currentMaterialIndex];
    _suppressEvents = true;
    try
    {
      _flagsTextBox.Text = $"0x{material.MaterialFlags:X8}";
      _detailSurfaceUpDown.Value = ClampDecimal(material.DetailSurface, _detailSurfaceUpDown.Minimum, _detailSurfaceUpDown.Maximum);
      _detailScaleUpDown.Value = ClampDecimal(material.DetailScale, _detailScaleUpDown.Minimum, _detailScaleUpDown.Maximum);
      _layerCountLabel.Text = material.Layers.Count.ToString(CultureInfo.InvariantCulture);

      _layersGrid.Rows.Clear();
      for (int layerIndex = 0; layerIndex < material.Layers.Count; ++layerIndex)
      {
        MaterialLayerDefinition layer = material.Layers[layerIndex];
        _layersGrid.Rows.Add(
          layer.TileAniTextureCount,
          layer.SurfaceId,
          FormatHex(layer.AlphaType),
          FormatHex(layer.Argb),
          FormatHex(layer.LayerFlags),
          layer.UvLavaWave,
          layer.UvLavaSpeed,
          layer.UvScrollU,
          layer.UvScrollV,
          layer.UvRotate,
          layer.UvScaleStart,
          layer.UvScaleEnd,
          layer.UvScaleSpeed,
          layer.UvMetal,
          layer.AniAlphaFlicker,
          layer.AniAlphaFlickerRange,
          layer.AniTexFrame,
          layer.AniTexSpeed,
          layer.GradientAlpha);
      }
    }
    finally
    {
      _suppressEvents = false;
    }
  }

  private bool SaveCurrentMaterialFromUi(bool showErrors)
  {
    if (_suppressEvents || (uint)_currentMaterialIndex >= (uint)_materials.Length)
    {
      return true;
    }

    if (!TryParseUInt(_flagsTextBox.Text, out uint flags))
    {
      if (showErrors)
      {
        MessageBox.Show(this, $"Material {_currentMaterialIndex}: invalid Material Flags.", "R3M Materials", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        _flagsTextBox.Focus();
      }
      return false;
    }

    List<MaterialLayerDefinition> layers = new(_layersGrid.Rows.Count);
    for (int rowIndex = 0; rowIndex < _layersGrid.Rows.Count; ++rowIndex)
    {
      DataGridViewRow row = _layersGrid.Rows[rowIndex];
      if (row.IsNewRow)
      {
        continue;
      }

      if (!TryReadLayerFromRow(row, rowIndex, out MaterialLayerDefinition layer, out string error))
      {
        if (showErrors)
        {
          MessageBox.Show(this, $"Material {_currentMaterialIndex}: {error}", "R3M Materials", MessageBoxButtons.OK, MessageBoxIcon.Warning);
          if (_layersGrid.CurrentCell == null && row.Cells.Count > 0)
          {
            _layersGrid.CurrentCell = row.Cells[0];
          }
          _layersGrid.Focus();
        }
        return false;
      }

      layers.Add(layer);
    }

    _materials[_currentMaterialIndex] = new EditableMaterial
    {
      MaterialFlags = flags,
      DetailSurface = (int)_detailSurfaceUpDown.Value,
      DetailScale = (float)_detailScaleUpDown.Value,
      Layers = layers,
    };

    _layerCountLabel.Text = layers.Count.ToString(CultureInfo.InvariantCulture);
    RefreshMaterialTitle(_currentMaterialIndex);
    return true;
  }

  private void OnAddLayerClicked(object? sender, EventArgs e)
  {
    if ((uint)_currentMaterialIndex >= (uint)_materials.Length)
    {
      return;
    }

    if (!SaveCurrentMaterialFromUi(showErrors: true))
    {
      return;
    }

    _suppressEvents = true;
    try
    {
      _layersGrid.Rows.Add(
        (short)0,
        0,
        "0x00000000",
        "0x00000000",
        "0x00000000",
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (short)0,
        (ushort)0,
        (short)0,
        (short)0,
        (short)0);
      if (_layersGrid.Rows.Count > 0)
      {
        _layersGrid.CurrentCell = _layersGrid.Rows[_layersGrid.Rows.Count - 1].Cells[0];
      }
    }
    finally
    {
      _suppressEvents = false;
    }

    SaveCurrentMaterialFromUi(showErrors: false);
  }

  private void OnRemoveLayerClicked(object? sender, EventArgs e)
  {
    if ((uint)_currentMaterialIndex >= (uint)_materials.Length)
    {
      return;
    }

    if (_layersGrid.CurrentCell == null)
    {
      return;
    }

    int rowIndex = _layersGrid.CurrentCell.RowIndex;
    if ((uint)rowIndex >= (uint)_layersGrid.Rows.Count)
    {
      return;
    }

    _suppressEvents = true;
    try
    {
      if (!_layersGrid.Rows[rowIndex].IsNewRow)
      {
        _layersGrid.Rows.RemoveAt(rowIndex);
      }
    }
    finally
    {
      _suppressEvents = false;
    }

    SaveCurrentMaterialFromUi(showErrors: false);
  }

  private void OnOkClicked(object? sender, EventArgs e)
  {
    if (!SaveCurrentMaterialFromUi(showErrors: true))
    {
      return;
    }

    MaterialDefinition[] output = new MaterialDefinition[_materials.Length];
    for (int materialIndex = 0; materialIndex < _materials.Length; ++materialIndex)
    {
      EditableMaterial material = _materials[materialIndex];
      output[materialIndex] = new MaterialDefinition(
        material.MaterialFlags,
        material.DetailSurface,
        material.DetailScale,
        material.Layers.ToArray());
    }

    EditedMaterials = output;
    DialogResult = DialogResult.OK;
    Close();
  }

  private DataGridView CreateLayersGrid()
  {
    DataGridView grid = new()
    {
      Dock = DockStyle.Fill,
      AllowUserToAddRows = false,
      AllowUserToDeleteRows = false,
      AllowUserToResizeRows = false,
      RowHeadersVisible = false,
      SelectionMode = DataGridViewSelectionMode.CellSelect,
      MultiSelect = false,
      AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.AllCells,
    };

    grid.Columns.Add(CreateTextColumn("TileAniTextureCount", "TileAniCnt"));
    grid.Columns.Add(CreateTextColumn("SurfaceId", "Surface"));
    grid.Columns.Add(CreateTextColumn("AlphaType", "AlphaType"));
    grid.Columns.Add(CreateTextColumn("Argb", "ARGB"));
    grid.Columns.Add(CreateTextColumn("LayerFlags", "LayerFlags"));
    grid.Columns.Add(CreateTextColumn("UvLavaWave", "UvLavaWave"));
    grid.Columns.Add(CreateTextColumn("UvLavaSpeed", "UvLavaSpeed"));
    grid.Columns.Add(CreateTextColumn("UvScrollU", "UvScrollU"));
    grid.Columns.Add(CreateTextColumn("UvScrollV", "UvScrollV"));
    grid.Columns.Add(CreateTextColumn("UvRotate", "UvRotate"));
    grid.Columns.Add(CreateTextColumn("UvScaleStart", "UvScaleStart"));
    grid.Columns.Add(CreateTextColumn("UvScaleEnd", "UvScaleEnd"));
    grid.Columns.Add(CreateTextColumn("UvScaleSpeed", "UvScaleSpeed"));
    grid.Columns.Add(CreateTextColumn("UvMetal", "UvMetal"));
    grid.Columns.Add(CreateTextColumn("AniAlphaFlicker", "AniAlphaFlicker"));
    grid.Columns.Add(CreateTextColumn("AniAlphaFlickerRange", "AniAlphaFlickerRange"));
    grid.Columns.Add(CreateTextColumn("AniTexFrame", "AniTexFrame"));
    grid.Columns.Add(CreateTextColumn("AniTexSpeed", "AniTexSpeed"));
    grid.Columns.Add(CreateTextColumn("GradientAlpha", "GradientAlpha"));
    return grid;
  }

  private static DataGridViewTextBoxColumn CreateTextColumn(string name, string header)
  {
    return new DataGridViewTextBoxColumn
    {
      Name = name,
      HeaderText = header,
      SortMode = DataGridViewColumnSortMode.NotSortable,
    };
  }

  private static void AddEditorRow(TableLayoutPanel editor, int row, string labelText, Control control)
  {
    Label label = new()
    {
      Text = labelText,
      Dock = DockStyle.Fill,
      TextAlign = ContentAlignment.MiddleLeft,
      AutoSize = true,
    };
    editor.Controls.Add(label, 0, row);
    editor.Controls.Add(control, 1, row);
  }

  private static NumericUpDown CreateIntInput(decimal min, decimal max)
  {
    return new NumericUpDown
    {
      Dock = DockStyle.Fill,
      DecimalPlaces = 0,
      Minimum = min,
      Maximum = max,
      Increment = 1,
      ThousandsSeparator = true,
    };
  }

  private static NumericUpDown CreateFloatInput(decimal min, decimal max, int decimals)
  {
    return new NumericUpDown
    {
      Dock = DockStyle.Fill,
      DecimalPlaces = decimals,
      Minimum = min,
      Maximum = max,
      Increment = decimals > 0 ? 0.1m : 1m,
      ThousandsSeparator = true,
    };
  }

  private static string FormatHex(uint value)
  {
    return $"0x{value:X8}";
  }

  private static bool TryReadLayerFromRow(
    DataGridViewRow row,
    int rowIndex,
    out MaterialLayerDefinition layer,
    out string error)
  {
    error = string.Empty;
    layer = default;

    if (!TryParseShort(GetCellValue(row, "TileAniTextureCount"), out short tileAni))
    {
      error = $"Layer {rowIndex}: invalid TileAniTextureCount.";
      return false;
    }

    if (!TryParseInt(GetCellValue(row, "SurfaceId"), out int surfaceId))
    {
      error = $"Layer {rowIndex}: invalid SurfaceId.";
      return false;
    }

    if (!TryParseUInt(GetCellValue(row, "AlphaType"), out uint alphaType))
    {
      error = $"Layer {rowIndex}: invalid AlphaType.";
      return false;
    }

    if (!TryParseUInt(GetCellValue(row, "Argb"), out uint argb))
    {
      error = $"Layer {rowIndex}: invalid ARGB.";
      return false;
    }

    if (!TryParseUInt(GetCellValue(row, "LayerFlags"), out uint layerFlags))
    {
      error = $"Layer {rowIndex}: invalid LayerFlags.";
      return false;
    }

    if (!TryParseShort(GetCellValue(row, "UvLavaWave"), out short uvLavaWave)
      || !TryParseShort(GetCellValue(row, "UvLavaSpeed"), out short uvLavaSpeed)
      || !TryParseShort(GetCellValue(row, "UvScrollU"), out short uvScrollU)
      || !TryParseShort(GetCellValue(row, "UvScrollV"), out short uvScrollV)
      || !TryParseShort(GetCellValue(row, "UvRotate"), out short uvRotate)
      || !TryParseShort(GetCellValue(row, "UvScaleStart"), out short uvScaleStart)
      || !TryParseShort(GetCellValue(row, "UvScaleEnd"), out short uvScaleEnd)
      || !TryParseShort(GetCellValue(row, "UvScaleSpeed"), out short uvScaleSpeed)
      || !TryParseShort(GetCellValue(row, "UvMetal"), out short uvMetal)
      || !TryParseShort(GetCellValue(row, "AniAlphaFlicker"), out short aniAlphaFlicker)
      || !TryParseUShort(GetCellValue(row, "AniAlphaFlickerRange"), out ushort aniAlphaFlickerRange)
      || !TryParseShort(GetCellValue(row, "AniTexFrame"), out short aniTexFrame)
      || !TryParseShort(GetCellValue(row, "AniTexSpeed"), out short aniTexSpeed)
      || !TryParseShort(GetCellValue(row, "GradientAlpha"), out short gradientAlpha))
    {
      error = $"Layer {rowIndex}: one or more numeric fields are invalid.";
      return false;
    }

    layer = new MaterialLayerDefinition(
      tileAni,
      surfaceId,
      alphaType,
      argb,
      layerFlags,
      uvLavaWave,
      uvLavaSpeed,
      uvScrollU,
      uvScrollV,
      uvRotate,
      uvScaleStart,
      uvScaleEnd,
      uvScaleSpeed,
      uvMetal,
      aniAlphaFlicker,
      aniAlphaFlickerRange,
      aniTexFrame,
      aniTexSpeed,
      gradientAlpha);

    return true;
  }

  private static string GetCellValue(DataGridViewRow row, string columnName)
  {
    object? value = row.Cells[columnName].Value;
    return value?.ToString() ?? "0";
  }

  private static bool TryParseShort(string text, out short value)
  {
    if (short.TryParse(text, NumberStyles.Integer, CultureInfo.InvariantCulture, out value))
    {
      return true;
    }

    if (TryParseInt(text, out int parsed) && parsed >= short.MinValue && parsed <= short.MaxValue)
    {
      value = (short)parsed;
      return true;
    }

    value = 0;
    return false;
  }

  private static bool TryParseUShort(string text, out ushort value)
  {
    if (ushort.TryParse(text, NumberStyles.Integer, CultureInfo.InvariantCulture, out value))
    {
      return true;
    }

    if (TryParseUInt(text, out uint parsed) && parsed <= ushort.MaxValue)
    {
      value = (ushort)parsed;
      return true;
    }

    value = 0;
    return false;
  }

  private static bool TryParseInt(string text, out int value)
  {
    string trimmed = (text ?? string.Empty).Trim();
    if (int.TryParse(trimmed, NumberStyles.Integer, CultureInfo.InvariantCulture, out value))
    {
      return true;
    }

    if (trimmed.StartsWith("0x", StringComparison.OrdinalIgnoreCase)
      && int.TryParse(trimmed[2..], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value))
    {
      return true;
    }

    return int.TryParse(trimmed, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
  }

  private static bool TryParseUInt(string text, out uint value)
  {
    string trimmed = (text ?? string.Empty).Trim();
    if (trimmed.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
    {
      return uint.TryParse(trimmed[2..], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
    }

    if (uint.TryParse(trimmed, NumberStyles.Integer, CultureInfo.InvariantCulture, out value))
    {
      return true;
    }

    return uint.TryParse(trimmed, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
  }

  private static decimal ClampDecimal(float value, decimal min, decimal max)
  {
    if (!float.IsFinite(value))
    {
      return min;
    }

    decimal dec = (decimal)value;
    if (dec < min)
    {
      return min;
    }

    if (dec > max)
    {
      return max;
    }

    return dec;
  }

  private static decimal ClampDecimal(int value, decimal min, decimal max)
  {
    decimal dec = value;
    if (dec < min)
    {
      return min;
    }

    if (dec > max)
    {
      return max;
    }

    return dec;
  }

  private sealed class EditableMaterial
  {
    public uint MaterialFlags { get; set; }
    public int DetailSurface { get; set; }
    public float DetailScale { get; set; }
    public required List<MaterialLayerDefinition> Layers { get; init; }
  }
}
