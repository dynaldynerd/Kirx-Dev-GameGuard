using System;
using System.Globalization;
using System.Windows.Forms;
using LoginServer.Settings;

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
        txtDbHost.Text = _settings.Database.Host;
        txtDbPort.Text = _settings.Database.Port.ToString(CultureInfo.InvariantCulture);
        txtDbName.Text = _settings.Database.Database;
        txtDbUser.Text = _settings.Database.User;
        txtDbPass.Text = _settings.Database.Password;
        txtBillingHost.Text = _settings.Database.BillingHost;
        txtBillingPort.Text = _settings.Database.BillingPort.ToString(CultureInfo.InvariantCulture);
        txtBillingDb.Text = _settings.Database.BillingDatabase;
        txtBillingUser.Text = _settings.Database.BillingUser;
        txtBillingPass.Text = _settings.Database.BillingPassword;
        txtClientPort.Text = _settings.Network.ClientPort.ToString(CultureInfo.InvariantCulture);
        txtAccountHost.Text = _settings.Network.AccountHost;
        txtAccountPort.Text = _settings.Network.AccountPort.ToString(CultureInfo.InvariantCulture);
        txtMaxConn.Text = _settings.Network.MaxConnections.ToString(CultureInfo.InvariantCulture);
    }

    private bool SaveToSettings()
    {
        _settings.Database.Host = txtDbHost.Text.Trim();
        if (!int.TryParse(txtDbPort.Text, out var dbPort) || dbPort < 1 || dbPort > 65535)
        {
            MessageBox.Show(this, "DB port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        _settings.Database.Port = dbPort;
        _settings.Database.Database = txtDbName.Text.Trim();
        _settings.Database.User = txtDbUser.Text.Trim();
        _settings.Database.Password = txtDbPass.Text;
        _settings.Database.BillingHost = txtBillingHost.Text.Trim();
        if (!int.TryParse(txtBillingPort.Text, out var billingPort) || billingPort < 1 || billingPort > 65535)
        {
            MessageBox.Show(this, "Billing port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        _settings.Database.BillingPort = billingPort;
        _settings.Database.BillingDatabase = txtBillingDb.Text.Trim();
        _settings.Database.BillingUser = txtBillingUser.Text.Trim();
        _settings.Database.BillingPassword = txtBillingPass.Text;
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
