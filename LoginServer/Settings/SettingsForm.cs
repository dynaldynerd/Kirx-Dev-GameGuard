using System;
using System.Globalization;
using System.Windows.Forms;

namespace LoginServer.Settings;

public partial class SettingsForm : Form
{
    private readonly AppSettings _settings;

    public SettingsForm(AppSettings settings)
    {
        _settings = settings;
        InitializeComponent();
        LoadFromSettings();
    }

    private void LoadFromSettings()
    {
        txtConnection.Text = _settings.Database.ConnectionString;
        txtClientPort.Text = _settings.Network.ClientPort.ToString(CultureInfo.InvariantCulture);
        txtAccountHost.Text = _settings.Network.AccountHost;
        txtAccountPort.Text = _settings.Network.AccountPort.ToString(CultureInfo.InvariantCulture);
        txtMaxConn.Text = _settings.Network.MaxConnections.ToString(CultureInfo.InvariantCulture);
    }

    private bool SaveToSettings()
    {
        _settings.Database.ConnectionString = txtConnection.Text.Trim();
        if (!int.TryParse(txtClientPort.Text, out var cPort) || cPort < 1 || cPort > 65535)
        {
            MessageBox.Show(this, "Client port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        if (!int.TryParse(txtAccountPort.Text, out var aPort) || aPort < 1 || aPort > 65535)
        {
            MessageBox.Show(this, "Account port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        if (!int.TryParse(txtMaxConn.Text, out var maxConn) || maxConn < -1)
        {
            MessageBox.Show(this, "Max connections must be -1 or >=0.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }

        _settings.Network.ClientPort = cPort;
        _settings.Network.AccountHost = txtAccountHost.Text.Trim();
        _settings.Network.AccountPort = aPort;
        _settings.Network.MaxConnections = maxConn;
        return true;
    }

    private void OnSave(object? sender, EventArgs e)
    {
        if (!SaveToSettings())
        {
            return;
        }
        _settings.Save();
        DialogResult = DialogResult.OK;
        Close();
    }

    private void OnCancel(object? sender, EventArgs e)
    {
        DialogResult = DialogResult.Cancel;
        Close();
    }
}
