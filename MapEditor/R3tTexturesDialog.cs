using System.Globalization;
using System.Text;
using MapEditor.Formats;

namespace MapEditor;

internal sealed class R3tTexturesDialog : Form
{
  private readonly SplitContainer _split;
  private readonly ListBox _textureListBox;
  private readonly TextBox _nameTextBox;
  private readonly NumericUpDown _surfaceIdUpDown;
  private readonly Label _sizeLabel;
  private readonly TextBox _ddsInfoTextBox;
  private readonly Button _replaceButton;
  private readonly Button _exportButton;
  private readonly Button _addButton;
  private readonly Button _removeButton;
  private readonly Button _importManyButton;
  private readonly Button _moveUpButton;
  private readonly Button _moveDownButton;
  private readonly Button _okButton;
  private readonly Button _cancelButton;
  private readonly List<EditableTexture> _textures;
  private bool _suppressUiEvents;
  private int _selectedIndex = -1;

  public R3TextureBlob[] EditedTextures { get; private set; }

  public R3tTexturesDialog(R3TextureBlob[] sourceTextures)
  {
    R3TextureBlob[] source = sourceTextures ?? Array.Empty<R3TextureBlob>();
    _textures = new List<EditableTexture>(source.Length);
    for (int index = 0; index < source.Length; ++index)
    {
      byte[] sourceDds = source[index].DdsBytes ?? Array.Empty<byte>();
      byte[] copied = new byte[sourceDds.Length];
      if (sourceDds.Length > 0)
      {
        Buffer.BlockCopy(sourceDds, 0, copied, 0, sourceDds.Length);
      }

      _textures.Add(new EditableTexture(source[index].SurfaceId, source[index].Name ?? string.Empty, copied));
    }

    EditedTextures = source;
    Text = "R3T Textures";
    FormBorderStyle = FormBorderStyle.FixedDialog;
    StartPosition = FormStartPosition.CenterParent;
    MinimizeBox = false;
    MaximizeBox = false;
    ShowInTaskbar = false;
    AutoScaleMode = AutoScaleMode.Font;
    ClientSize = new Size(900, 600);
    MinimumSize = new Size(980, 680);

    _split = new SplitContainer()
    {
      Dock = DockStyle.Fill,
      Orientation = Orientation.Vertical,
      IsSplitterFixed = false,
    };

    _textureListBox = new ListBox
    {
      Dock = DockStyle.Fill,
      IntegralHeight = false,
    };
    _textureListBox.SelectedIndexChanged += (_, _) => OnTextureSelectionChanged();
    _split.Panel1.Controls.Add(_textureListBox);

    TableLayoutPanel right = new()
    {
      Dock = DockStyle.Fill,
      ColumnCount = 2,
      RowCount = 9,
      Padding = new Padding(10),
    };
    right.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130));
    right.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 30));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 30));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 30));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
    right.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
    right.RowStyles.Add(new RowStyle(SizeType.Absolute, 46));

    _nameTextBox = new TextBox { Dock = DockStyle.Fill };
    _surfaceIdUpDown = new NumericUpDown
    {
      Dock = DockStyle.Fill,
      DecimalPlaces = 0,
      Minimum = -2_000_000,
      Maximum = 2_000_000,
      Increment = 1,
      ThousandsSeparator = true,
    };
    _sizeLabel = new Label
    {
      Dock = DockStyle.Fill,
      TextAlign = ContentAlignment.MiddleLeft,
      AutoSize = true,
    };

    AddRow(right, 0, "Name", _nameTextBox);
    AddRow(right, 1, "Surface ID", _surfaceIdUpDown);
    AddRow(right, 2, "DDS Bytes", _sizeLabel);

    FlowLayoutPanel rowButtons1 = new()
    {
      Dock = DockStyle.Fill,
      FlowDirection = FlowDirection.LeftToRight,
      WrapContents = false,
      AutoSize = true,
    };
    _replaceButton = new Button { Text = "Replace DDS...", Width = 120 };
    _exportButton = new Button { Text = "Export DDS...", Width = 120 };
    rowButtons1.Controls.Add(_replaceButton);
    rowButtons1.Controls.Add(_exportButton);
    AddRow(right, 3, "Slot", rowButtons1);

    FlowLayoutPanel rowButtons2 = new()
    {
      Dock = DockStyle.Fill,
      FlowDirection = FlowDirection.LeftToRight,
      WrapContents = false,
      AutoSize = true,
    };
    _addButton = new Button { Text = "Add Slot...", Width = 120 };
    _removeButton = new Button { Text = "Remove Slot", Width = 120 };
    rowButtons2.Controls.Add(_addButton);
    rowButtons2.Controls.Add(_removeButton);
    AddRow(right, 4, "Edit", rowButtons2);

    FlowLayoutPanel rowButtons3 = new()
    {
      Dock = DockStyle.Fill,
      FlowDirection = FlowDirection.LeftToRight,
      WrapContents = false,
      AutoSize = true,
    };
    _importManyButton = new Button { Text = "Import Many...", Width = 120 };
    _moveUpButton = new Button { Text = "Move Up", Width = 100 };
    _moveDownButton = new Button { Text = "Move Down", Width = 100 };
    rowButtons3.Controls.Add(_importManyButton);
    rowButtons3.Controls.Add(_moveUpButton);
    rowButtons3.Controls.Add(_moveDownButton);
    AddRow(right, 5, "Batch", rowButtons3);

    _ddsInfoTextBox = new TextBox
    {
      Dock = DockStyle.Fill,
      Multiline = true,
      ReadOnly = true,
      ScrollBars = ScrollBars.Vertical,
      Font = new Font(FontFamily.GenericMonospace, 9.0f),
    };
    AddRow(right, 7, "DDS Preview", _ddsInfoTextBox);

    FlowLayoutPanel okPanel = new()
    {
      FlowDirection = FlowDirection.RightToLeft,
      Dock = DockStyle.Fill,
      AutoSize = true,
      WrapContents = false,
      Padding = new Padding(0, 8, 0, 0),
    };
    _okButton = new Button { Text = "OK", Width = 90 };
    _cancelButton = new Button { Text = "Cancel", Width = 90, DialogResult = DialogResult.Cancel };
    okPanel.Controls.Add(_okButton);
    okPanel.Controls.Add(_cancelButton);
    right.Controls.Add(okPanel, 0, 8);
    right.SetColumnSpan(okPanel, 2);

    _split.Panel2.Controls.Add(right);
    Controls.Add(_split);

    _nameTextBox.TextChanged += (_, _) => ApplyUiToSelection();
    _surfaceIdUpDown.ValueChanged += (_, _) => ApplyUiToSelection();
    _replaceButton.Click += OnReplaceTextureClicked;
    _exportButton.Click += OnExportTextureClicked;
    _addButton.Click += OnAddTextureClicked;
    _removeButton.Click += OnRemoveTextureClicked;
    _importManyButton.Click += OnImportManyClicked;
    _moveUpButton.Click += (_, _) => MoveSelectedTexture(-1);
    _moveDownButton.Click += (_, _) => MoveSelectedTexture(+1);
    _okButton.Click += OnOkClicked;

    RefreshTextureList();
    if (_textureListBox.Items.Count > 0)
    {
      _textureListBox.SelectedIndex = 0;
    }
    else
    {
      UpdateEditorEnabledState();
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

    const int desiredPanel1Min = 300;
    const int desiredPanel2Min = 520;

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

    int preferred = Math.Clamp(_split.Width / 2, min, max);

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

  private static void AddRow(TableLayoutPanel layout, int row, string labelText, Control control)
  {
    Label label = new()
    {
      Text = labelText,
      Dock = DockStyle.Fill,
      TextAlign = ContentAlignment.MiddleLeft,
      AutoSize = true,
    };
    layout.Controls.Add(label, 0, row);
    layout.Controls.Add(control, 1, row);
  }

  private void RefreshTextureList()
  {
    _textureListBox.Items.Clear();
    for (int index = 0; index < _textures.Count; ++index)
    {
      _textureListBox.Items.Add(BuildTextureTitle(index, _textures[index]));
    }
  }

  private static string BuildTextureTitle(int index, EditableTexture texture)
  {
    string name = string.IsNullOrWhiteSpace(texture.Name) ? "<no-name>" : texture.Name;
    return $"{index}: sid={texture.SurfaceId} | {name}";
  }

  private void RefreshTextureTitle(int index)
  {
    if ((uint)index >= (uint)_textures.Count || (uint)index >= (uint)_textureListBox.Items.Count)
    {
      return;
    }

    _textureListBox.Items[index] = BuildTextureTitle(index, _textures[index]);
  }

  private void OnTextureSelectionChanged()
  {
    _selectedIndex = _textureListBox.SelectedIndex;
    LoadSelectionToUi();
  }

  private void LoadSelectionToUi()
  {
    if ((uint)_selectedIndex >= (uint)_textures.Count)
    {
      _suppressUiEvents = true;
      try
      {
        _nameTextBox.Text = string.Empty;
        _surfaceIdUpDown.Value = 0;
        _sizeLabel.Text = "0";
        _ddsInfoTextBox.Text = "No texture selected.";
      }
      finally
      {
        _suppressUiEvents = false;
      }

      UpdateEditorEnabledState();
      return;
    }

    EditableTexture texture = _textures[_selectedIndex];
    _suppressUiEvents = true;
    try
    {
      _nameTextBox.Text = texture.Name;
      _surfaceIdUpDown.Value = ClampDecimal(texture.SurfaceId, _surfaceIdUpDown.Minimum, _surfaceIdUpDown.Maximum);
      _sizeLabel.Text = texture.DdsBytes.Length.ToString("N0", CultureInfo.InvariantCulture);
      _ddsInfoTextBox.Text = BuildDdsInfoText(texture.DdsBytes);
    }
    finally
    {
      _suppressUiEvents = false;
    }

    UpdateEditorEnabledState();
  }

  private void UpdateEditorEnabledState()
  {
    bool hasSelection = (uint)_selectedIndex < (uint)_textures.Count;
    _nameTextBox.Enabled = hasSelection;
    _surfaceIdUpDown.Enabled = hasSelection;
    _replaceButton.Enabled = hasSelection;
    _exportButton.Enabled = hasSelection;
    _removeButton.Enabled = hasSelection;
    _moveUpButton.Enabled = hasSelection && _selectedIndex > 0;
    _moveDownButton.Enabled = hasSelection && _selectedIndex < _textures.Count - 1;
  }

  private void ApplyUiToSelection()
  {
    if (_suppressUiEvents || (uint)_selectedIndex >= (uint)_textures.Count)
    {
      return;
    }

    EditableTexture texture = _textures[_selectedIndex] with
    {
      Name = _nameTextBox.Text ?? string.Empty,
      SurfaceId = (int)_surfaceIdUpDown.Value,
    };

    _textures[_selectedIndex] = texture;
    RefreshTextureTitle(_selectedIndex);
  }

  private void OnReplaceTextureClicked(object? sender, EventArgs e)
  {
    if ((uint)_selectedIndex >= (uint)_textures.Count)
    {
      return;
    }

    if (!TryPickDdsFile("Select DDS texture", multiSelect: false, out string[] files))
    {
      return;
    }

    byte[] bytes;
    try
    {
      bytes = File.ReadAllBytes(files[0]);
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    if (!IsDds(bytes))
    {
      MessageBox.Show(this, "Selected file is not a valid DDS file.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    EditableTexture texture = _textures[_selectedIndex] with
    {
      DdsBytes = bytes,
      Name = string.IsNullOrWhiteSpace(_nameTextBox.Text)
        ? Path.GetFileName(files[0])
        : _nameTextBox.Text,
    };
    _textures[_selectedIndex] = texture;
    LoadSelectionToUi();
    RefreshTextureTitle(_selectedIndex);
  }

  private void OnExportTextureClicked(object? sender, EventArgs e)
  {
    if ((uint)_selectedIndex >= (uint)_textures.Count)
    {
      return;
    }

    EditableTexture texture = _textures[_selectedIndex];
    using SaveFileDialog dialog = new()
    {
      Title = "Export DDS texture",
      Filter = "DDS texture (*.dds)|*.dds|All files (*.*)|*.*",
      DefaultExt = "dds",
      AddExtension = true,
      OverwritePrompt = true,
      FileName = string.IsNullOrWhiteSpace(texture.Name) ? $"texture_{_selectedIndex:D4}.dds" : EnsureDdsExtension(texture.Name),
    };

    if (dialog.ShowDialog(this) != DialogResult.OK)
    {
      return;
    }

    try
    {
      File.WriteAllBytes(dialog.FileName, texture.DdsBytes);
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Error);
    }
  }

  private void OnAddTextureClicked(object? sender, EventArgs e)
  {
    if (!TryPickDdsFile("Add texture slot from DDS", multiSelect: false, out string[] files))
    {
      return;
    }

    byte[] bytes;
    try
    {
      bytes = File.ReadAllBytes(files[0]);
    }
    catch (Exception ex)
    {
      MessageBox.Show(this, ex.Message, "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Error);
      return;
    }

    if (!IsDds(bytes))
    {
      MessageBox.Show(this, "Selected file is not a valid DDS file.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    int nextSurfaceId = ResolveNextSurfaceId();
    EditableTexture texture = new(nextSurfaceId, Path.GetFileName(files[0]), bytes);
    _textures.Add(texture);
    RefreshTextureList();
    _textureListBox.SelectedIndex = _textures.Count - 1;
  }

  private void OnRemoveTextureClicked(object? sender, EventArgs e)
  {
    if ((uint)_selectedIndex >= (uint)_textures.Count)
    {
      return;
    }

    DialogResult result = MessageBox.Show(
      this,
      $"Remove selected texture slot #{_selectedIndex}?",
      "R3T Textures",
      MessageBoxButtons.YesNo,
      MessageBoxIcon.Question);
    if (result != DialogResult.Yes)
    {
      return;
    }

    _textures.RemoveAt(_selectedIndex);
    RefreshTextureList();
    if (_textures.Count == 0)
    {
      _selectedIndex = -1;
      _textureListBox.SelectedIndex = -1;
      LoadSelectionToUi();
      return;
    }

    int next = Math.Min(_selectedIndex, _textures.Count - 1);
    _textureListBox.SelectedIndex = next;
  }

  private void OnImportManyClicked(object? sender, EventArgs e)
  {
    if (!TryPickDdsFile("Import multiple DDS textures", multiSelect: true, out string[] files))
    {
      return;
    }

    int added = 0;
    int nextSurfaceId = ResolveNextSurfaceId();
    for (int fileIndex = 0; fileIndex < files.Length; ++fileIndex)
    {
      byte[] bytes;
      try
      {
        bytes = File.ReadAllBytes(files[fileIndex]);
      }
      catch
      {
        continue;
      }

      if (!IsDds(bytes))
      {
        continue;
      }

      _textures.Add(new EditableTexture(nextSurfaceId++, Path.GetFileName(files[fileIndex]), bytes));
      ++added;
    }

    if (added == 0)
    {
      MessageBox.Show(this, "No valid DDS files were imported.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return;
    }

    RefreshTextureList();
    _textureListBox.SelectedIndex = _textures.Count - added;
  }

  private int ResolveNextSurfaceId()
  {
    int max = 0;
    for (int index = 0; index < _textures.Count; ++index)
    {
      if (_textures[index].SurfaceId > max)
      {
        max = _textures[index].SurfaceId;
      }
    }

    return Math.Max(1, max + 1);
  }

  private void MoveSelectedTexture(int delta)
  {
    if ((uint)_selectedIndex >= (uint)_textures.Count)
    {
      return;
    }

    int destination = _selectedIndex + delta;
    if ((uint)destination >= (uint)_textures.Count)
    {
      return;
    }

    (_textures[_selectedIndex], _textures[destination]) = (_textures[destination], _textures[_selectedIndex]);
    RefreshTextureList();
    _textureListBox.SelectedIndex = destination;
  }

  private static bool TryPickDdsFile(string title, bool multiSelect, out string[] files)
  {
    using OpenFileDialog dialog = new()
    {
      Title = title,
      Filter = "DDS texture (*.dds)|*.dds|All files (*.*)|*.*",
      CheckFileExists = true,
      Multiselect = multiSelect,
    };

    if (dialog.ShowDialog() != DialogResult.OK)
    {
      files = Array.Empty<string>();
      return false;
    }

    files = dialog.FileNames;
    return files.Length > 0;
  }

  private void OnOkClicked(object? sender, EventArgs e)
  {
    ApplyUiToSelection();

    HashSet<int> usedSurfaceIds = new();
    R3TextureBlob[] output = new R3TextureBlob[_textures.Count];
    for (int index = 0; index < _textures.Count; ++index)
    {
      EditableTexture texture = _textures[index];
      if (texture.SurfaceId <= 0)
      {
        MessageBox.Show(this, $"Texture {index} has invalid Surface ID ({texture.SurfaceId}).", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return;
      }

      if (!usedSurfaceIds.Add(texture.SurfaceId))
      {
        MessageBox.Show(this, $"Texture {index} duplicates Surface ID {texture.SurfaceId}.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return;
      }

      if (texture.DdsBytes.Length == 0 || !IsDds(texture.DdsBytes))
      {
        MessageBox.Show(this, $"Texture {index} does not contain valid DDS data.", "R3T Textures", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return;
      }

      output[index] = new R3TextureBlob(texture.SurfaceId, texture.Name ?? string.Empty, texture.DdsBytes);
    }

    EditedTextures = output;
    DialogResult = DialogResult.OK;
    Close();
  }

  private static string EnsureDdsExtension(string fileName)
  {
    if (fileName.EndsWith(".dds", StringComparison.OrdinalIgnoreCase))
    {
      return fileName;
    }

    return fileName + ".dds";
  }

  private static bool IsDds(byte[] bytes)
  {
    return bytes.Length >= 4
      && bytes[0] == (byte)'D'
      && bytes[1] == (byte)'D'
      && bytes[2] == (byte)'S'
      && bytes[3] == (byte)' ';
  }

  private static string BuildDdsInfoText(byte[] bytes)
  {
    if (!IsDds(bytes))
    {
      return "Invalid DDS signature.";
    }

    if (bytes.Length < 128)
    {
      return $"DDS header truncated ({bytes.Length} bytes).";
    }

    int headerSize = ReadInt32(bytes, 4);
    int height = ReadInt32(bytes, 12);
    int width = ReadInt32(bytes, 16);
    int mipCount = ReadInt32(bytes, 28);
    int pfSize = ReadInt32(bytes, 76);
    int pfFlags = ReadInt32(bytes, 80);
    uint fourCC = ReadUInt32(bytes, 84);
    int rgbBitCount = ReadInt32(bytes, 88);
    int pitchOrLinear = ReadInt32(bytes, 20);

    StringBuilder sb = new();
    _ = sb.AppendLine($"Size: {bytes.Length:N0} bytes");
    _ = sb.AppendLine($"HeaderSize: {headerSize}");
    _ = sb.AppendLine($"Width x Height: {width} x {height}");
    _ = sb.AppendLine($"MipMapCount: {mipCount}");
    _ = sb.AppendLine($"Pitch/LinearSize: {pitchOrLinear}");
    _ = sb.AppendLine($"PixelFormat.Size: {pfSize}");
    _ = sb.AppendLine($"PixelFormat.Flags: 0x{pfFlags:X8}");
    _ = sb.AppendLine($"PixelFormat.FourCC: {FormatFourCc(fourCC)}");
    _ = sb.AppendLine($"PixelFormat.RGBBitCount: {rgbBitCount}");
    return sb.ToString();
  }

  private static int ReadInt32(byte[] bytes, int offset)
  {
    if (offset < 0 || offset + 4 > bytes.Length)
    {
      return 0;
    }

    return BitConverter.ToInt32(bytes, offset);
  }

  private static uint ReadUInt32(byte[] bytes, int offset)
  {
    if (offset < 0 || offset + 4 > bytes.Length)
    {
      return 0u;
    }

    return BitConverter.ToUInt32(bytes, offset);
  }

  private static string FormatFourCc(uint value)
  {
    char c0 = (char)(value & 0xFF);
    char c1 = (char)((value >> 8) & 0xFF);
    char c2 = (char)((value >> 16) & 0xFF);
    char c3 = (char)((value >> 24) & 0xFF);
    if (char.IsControl(c0) || char.IsControl(c1) || char.IsControl(c2) || char.IsControl(c3))
    {
      return $"0x{value:X8}";
    }

    return $"{c0}{c1}{c2}{c3} (0x{value:X8})";
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

  private readonly record struct EditableTexture(int SurfaceId, string Name, byte[] DdsBytes);
}
