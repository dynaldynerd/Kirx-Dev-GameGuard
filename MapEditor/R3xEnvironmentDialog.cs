using MapEditor.Formats;
using OpenTK.Mathematics;

namespace MapEditor;

internal sealed class R3xEnvironmentDialog : Form
{
  private readonly CheckBox _fogEnabledCheckBox;
  private readonly NumericUpDown _fogStartUpDown;
  private readonly NumericUpDown _fogEndUpDown;
  private readonly NumericUpDown _fogRedUpDown;
  private readonly NumericUpDown _fogGreenUpDown;
  private readonly NumericUpDown _fogBlueUpDown;
  private readonly CheckBox _fogRangeCheckBox;
  private readonly CheckBox _noFogSkyCheckBox;
  private readonly CheckBox _lensFlareCheckBox;
  private readonly NumericUpDown _lensXUpDown;
  private readonly NumericUpDown _lensYUpDown;
  private readonly NumericUpDown _lensZUpDown;
  private readonly Button _okButton;
  private readonly Button _cancelButton;

  public MapEnvironmentSettings EditedSettings { get; private set; }

  public R3xEnvironmentDialog(MapEnvironmentSettings source)
  {
    EditedSettings = source;
    Text = "R3X Environment";
    FormBorderStyle = FormBorderStyle.FixedDialog;
    StartPosition = FormStartPosition.CenterParent;
    MinimizeBox = false;
    MaximizeBox = false;
    ShowInTaskbar = false;
    AutoScaleMode = AutoScaleMode.Font;
    ClientSize = new Size(430, 360);

    TableLayoutPanel layout = new()
    {
      Dock = DockStyle.Fill,
      ColumnCount = 4,
      RowCount = 8,
      Padding = new Padding(10),
    };
    layout.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130));
    layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33));
    layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 33));
    layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 34));

    _fogEnabledCheckBox = new CheckBox { Text = "Fog Enabled", AutoSize = true };
    _fogStartUpDown = CreateFloatInput(0, 200000, 2);
    _fogEndUpDown = CreateFloatInput(0, 200000, 2);
    _fogRedUpDown = CreateFloatInput(0, 255, 0);
    _fogGreenUpDown = CreateFloatInput(0, 255, 0);
    _fogBlueUpDown = CreateFloatInput(0, 255, 0);
    _fogRangeCheckBox = new CheckBox { Text = "Fog Range", AutoSize = true };
    _noFogSkyCheckBox = new CheckBox { Text = "No Fog Sky", AutoSize = true };
    _lensFlareCheckBox = new CheckBox { Text = "Lens Flare", AutoSize = true };
    _lensXUpDown = CreateFloatInput(-500000, 500000, 2);
    _lensYUpDown = CreateFloatInput(-500000, 500000, 2);
    _lensZUpDown = CreateFloatInput(-500000, 500000, 2);

    AddRow(layout, 0, "Fog", _fogEnabledCheckBox, null, null);
    AddRow(layout, 1, "Fog Start", _fogStartUpDown, new Label { Text = "Fog End", TextAlign = ContentAlignment.MiddleLeft, Dock = DockStyle.Fill }, _fogEndUpDown);
    AddRow(layout, 2, "Fog Color RGB", _fogRedUpDown, _fogGreenUpDown, _fogBlueUpDown);
    AddRow(layout, 3, "Flags", _fogRangeCheckBox, _noFogSkyCheckBox, _lensFlareCheckBox);
    AddRow(layout, 4, "Lens X", _lensXUpDown, new Label { Text = "Lens Y", TextAlign = ContentAlignment.MiddleLeft, Dock = DockStyle.Fill }, _lensYUpDown);
    AddRow(layout, 5, "Lens Z", _lensZUpDown, null, null);

    FlowLayoutPanel buttonPanel = new()
    {
      FlowDirection = FlowDirection.RightToLeft,
      Dock = DockStyle.Fill,
      AutoSize = true,
      WrapContents = false,
      Padding = new Padding(0, 10, 0, 0),
    };

    _okButton = new Button { Text = "OK", DialogResult = DialogResult.None, Width = 90 };
    _cancelButton = new Button { Text = "Cancel", DialogResult = DialogResult.Cancel, Width = 90 };
    _okButton.Click += OnOkClicked;
    buttonPanel.Controls.Add(_okButton);
    buttonPanel.Controls.Add(_cancelButton);

    layout.Controls.Add(buttonPanel, 0, 7);
    layout.SetColumnSpan(buttonPanel, 4);

    Controls.Add(layout);
    AcceptButton = _okButton;
    CancelButton = _cancelButton;

    LoadFrom(source);
  }

  private static NumericUpDown CreateFloatInput(decimal min, decimal max, int decimals)
  {
    return new NumericUpDown
    {
      Dock = DockStyle.Fill,
      DecimalPlaces = decimals,
      Minimum = min,
      Maximum = max,
      Increment = decimals > 0 ? 0.5m : 1m,
      ThousandsSeparator = true,
    };
  }

  private static void AddRow(TableLayoutPanel layout, int row, string labelText, Control first, Control? second, Control? third)
  {
    Label label = new()
    {
      Text = labelText,
      TextAlign = ContentAlignment.MiddleLeft,
      Dock = DockStyle.Fill,
      AutoSize = true,
    };

    layout.Controls.Add(label, 0, row);
    layout.Controls.Add(first, 1, row);
    if (second != null)
    {
      layout.Controls.Add(second, 2, row);
    }

    if (third != null)
    {
      layout.Controls.Add(third, 3, row);
    }
  }

  private void LoadFrom(MapEnvironmentSettings source)
  {
    _fogEnabledCheckBox.Checked = source.FogEnabled;
    _fogStartUpDown.Value = ClampDecimal(source.FogStart, _fogStartUpDown.Minimum, _fogStartUpDown.Maximum);
    _fogEndUpDown.Value = ClampDecimal(source.FogEnd, _fogEndUpDown.Minimum, _fogEndUpDown.Maximum);
    _fogRedUpDown.Value = ClampDecimal(source.FogColor.X * 255.0f, _fogRedUpDown.Minimum, _fogRedUpDown.Maximum);
    _fogGreenUpDown.Value = ClampDecimal(source.FogColor.Y * 255.0f, _fogGreenUpDown.Minimum, _fogGreenUpDown.Maximum);
    _fogBlueUpDown.Value = ClampDecimal(source.FogColor.Z * 255.0f, _fogBlueUpDown.Minimum, _fogBlueUpDown.Maximum);
    _fogRangeCheckBox.Checked = source.FogRangeEnabled;
    _noFogSkyCheckBox.Checked = source.NoFogSky;
    _lensFlareCheckBox.Checked = source.HasLensFlare;
    _lensXUpDown.Value = ClampDecimal(source.LensFlarePosition.X, _lensXUpDown.Minimum, _lensXUpDown.Maximum);
    _lensYUpDown.Value = ClampDecimal(source.LensFlarePosition.Y, _lensYUpDown.Minimum, _lensYUpDown.Maximum);
    _lensZUpDown.Value = ClampDecimal(source.LensFlarePosition.Z, _lensZUpDown.Minimum, _lensZUpDown.Maximum);
  }

  private void OnOkClicked(object? sender, EventArgs e)
  {
    float fogStart = (float)_fogStartUpDown.Value;
    float fogEnd = (float)_fogEndUpDown.Value;
    if (_fogEnabledCheckBox.Checked && fogEnd <= fogStart)
    {
      MessageBox.Show(this, "Fog End must be greater than Fog Start.", "R3X Environment", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      return;
    }

    Vector3 fogColor = new(
      (float)_fogRedUpDown.Value / 255.0f,
      (float)_fogGreenUpDown.Value / 255.0f,
      (float)_fogBlueUpDown.Value / 255.0f);

    Vector3 lensPosition = new(
      (float)_lensXUpDown.Value,
      (float)_lensYUpDown.Value,
      (float)_lensZUpDown.Value);

    EditedSettings = new MapEnvironmentSettings(
      _fogEnabledCheckBox.Checked,
      fogStart,
      fogEnd,
      fogColor,
      _fogRangeCheckBox.Checked,
      _noFogSkyCheckBox.Checked,
      _lensFlareCheckBox.Checked,
      lensPosition);

    DialogResult = DialogResult.OK;
    Close();
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
}
