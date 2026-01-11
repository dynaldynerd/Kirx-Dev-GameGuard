using System;
using System.Drawing;
using System.Windows.Forms;

namespace AccountServer.Settings;

public sealed class WorldEntryForm : Form
{
    private readonly WorldEntry _entry;
    private TextBox txtName = null!;
    private TextBox txtAddress = null!;
    private TextBox txtDbName = null!;
    private NumericUpDown numType = null!;
    private Button btnOk = null!;
    private Button btnCancel = null!;

    public WorldEntryForm(WorldEntry entry)
    {
        _entry = entry;
        InitializeComponent();
        LoadEntry();
    }

    private void InitializeComponent()
    {
        Text = "World Entry";
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        StartPosition = FormStartPosition.CenterParent;
        ClientSize = new Size(360, 220);

        txtName = new TextBox { Dock = DockStyle.Fill };
        txtAddress = new TextBox { Dock = DockStyle.Fill };
        txtDbName = new TextBox { Dock = DockStyle.Fill };
        numType = new NumericUpDown
        {
            Minimum = 0,
            Maximum = 65535
        };

        var layout = new TableLayoutPanel
        {
            Dock = DockStyle.Top,
            AutoSize = true,
            Padding = new Padding(10),
            ColumnCount = 2
        };
        layout.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 100));
        layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));

        layout.Controls.Add(new Label { Text = "Name", AutoSize = true }, 0, 0);
        layout.Controls.Add(txtName, 1, 0);
        layout.Controls.Add(new Label { Text = "Address", AutoSize = true }, 0, 1);
        layout.Controls.Add(txtAddress, 1, 1);
        layout.Controls.Add(new Label { Text = "DB Name", AutoSize = true }, 0, 2);
        layout.Controls.Add(txtDbName, 1, 2);
        layout.Controls.Add(new Label { Text = "Type", AutoSize = true }, 0, 3);
        layout.Controls.Add(numType, 1, 3);

        var buttons = new FlowLayoutPanel
        {
            Dock = DockStyle.Bottom,
            FlowDirection = FlowDirection.RightToLeft,
            Padding = new Padding(10),
            Height = 44
        };
        btnOk = new Button { Text = "OK", AutoSize = true };
        btnCancel = new Button { Text = "Cancel", AutoSize = true };
        btnOk.Click += OnOk;
        btnCancel.Click += OnCancel;
        buttons.Controls.Add(btnOk);
        buttons.Controls.Add(btnCancel);

        Controls.Add(layout);
        Controls.Add(buttons);
        AcceptButton = btnOk;
        CancelButton = btnCancel;
    }

    private void LoadEntry()
    {
        txtName.Text = _entry.Name;
        txtAddress.Text = _entry.Address;
        txtDbName.Text = _entry.DbName;
        numType.Value = Math.Clamp(_entry.Type, (int)numType.Minimum, (int)numType.Maximum);
    }

    private void OnOk(object? sender, EventArgs e)
    {
        var name = txtName.Text.Trim();
        var address = txtAddress.Text.Trim();
        var dbName = txtDbName.Text.Trim();
        if (string.IsNullOrEmpty(name))
        {
            MessageBox.Show(this, "Name is required.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }
        if (string.IsNullOrEmpty(address))
        {
            MessageBox.Show(this, "Address is required.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }
        if (string.IsNullOrEmpty(dbName))
        {
            MessageBox.Show(this, "DB name is required.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        _entry.Name = name;
        _entry.Address = address;
        _entry.DbName = dbName;
        _entry.Type = (int)numType.Value;

        DialogResult = DialogResult.OK;
        Close();
    }

    private void OnCancel(object? sender, EventArgs e)
    {
        DialogResult = DialogResult.Cancel;
        Close();
    }
}
