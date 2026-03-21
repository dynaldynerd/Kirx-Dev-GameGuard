using System;
using System.Windows.Forms;

namespace AccountServer.Settings;

public partial class WorldEntryForm : Form
{
    private readonly WorldEntry _entry;

    public WorldEntryForm()
        : this(new WorldEntry())
    {
    }

    public WorldEntryForm(WorldEntry entry)
    {
        _entry = entry;
        InitializeComponent();
        LoadEntry();
    }

    private void LoadEntry()
    {
        txtName.Text = _entry.Name;
        txtAddress.Text = string.IsNullOrWhiteSpace(_entry.Address)
            ? DbProfile.TrustedSqlServerHost
            : _entry.Address;
        txtDbName.Text = _entry.DbName;
        cmbType.SelectedIndex = _entry.Type == 1 ? 1 : 0;
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
            MessageBox.Show(this, "Database server address/IP is required.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
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
        _entry.Type = cmbType.SelectedIndex == 1 ? 1 : 0;

        DialogResult = DialogResult.OK;
        Close();
    }

    private void OnCancel(object? sender, EventArgs e)
    {
        DialogResult = DialogResult.Cancel;
        Close();
    }

    private void OnInstallDb(object? sender, EventArgs e)
    {
        string dbName = txtDbName.Text.Trim();
        if (string.IsNullOrEmpty(dbName))
        {
            MessageBox.Show(this, "DB name is required before installation.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            txtDbName.Focus();
            return;
        }

        string serverAddress = txtAddress.Text.Trim();
        if (string.IsNullOrEmpty(serverAddress))
        {
            serverAddress = DbProfile.TrustedSqlServerHost;
        }

        bool useTrustedConnection = string.Equals(
            serverAddress,
            DbProfile.TrustedSqlServerHost,
            StringComparison.OrdinalIgnoreCase);

        using var form = new global::AccountServer.WorldDatabaseInstallForm(dbName, serverAddress, useTrustedConnection);
        form.ShowDialog(this);
    }
}
