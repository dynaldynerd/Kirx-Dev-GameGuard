using System;
using System.Globalization;
using System.Windows.Forms;
using LoginServer.Data;
using LoginServer.Settings;

namespace LoginServer.Settings;

public partial class SettingsForm : Form
{
    private readonly AppSettings _settings;
    private readonly DatabaseSnapshot _userDb;
    private readonly DatabaseSnapshot _billingDb;

    public SettingsForm(AppSettings settings)
    {
        _settings = settings;
        _userDb = new DatabaseSnapshot
        {
            Provider = settings.Database.Provider,
            Host = settings.Database.Host,
            Port = settings.Database.Port,
            Database = settings.Database.Database,
            User = settings.Database.User,
            Password = settings.Database.Password,
            Trusted = settings.Database.TrustedConnection
        };
        _billingDb = new DatabaseSnapshot
        {
            Provider = settings.Database.BillingProvider,
            Host = settings.Database.BillingHost,
            Port = settings.Database.BillingPort,
            Database = settings.Database.BillingDatabase,
            User = settings.Database.BillingUser,
            Password = settings.Database.BillingPassword,
            Trusted = settings.Database.BillingTrustedConnection
        };
        InitializeComponent();
        LoadFromSettings();
    }

    private void LoadFromSettings()
    {
        cboDbProfile.Items.Clear();
        cboDbProfile.Items.AddRange(new object[] { "User DB", "Billing DB" });
        cboDbProfile.SelectedIndex = 0;
        LoadDbProfileToControls();
        txtClientPort.Text = _settings.Network.ClientPort.ToString(CultureInfo.InvariantCulture);
        txtAccountHost.Text = _settings.Network.AccountHost;
        txtAccountPort.Text = _settings.Network.AccountPort.ToString(CultureInfo.InvariantCulture);
        txtMaxConn.Text = _settings.Network.MaxConnections.ToString(CultureInfo.InvariantCulture);
        var loads = _settings.Network.UserLoadThresholds ?? Array.Empty<int>();
        txtLoadLow.Text = (loads.Length > 0 ? loads[0] : 500).ToString(CultureInfo.InvariantCulture);
        txtLoadMid.Text = (loads.Length > 1 ? loads[1] : 1000).ToString(CultureInfo.InvariantCulture);
        txtLoadHigh.Text = (loads.Length > 2 ? loads[2] : 1500).ToString(CultureInfo.InvariantCulture);
    }

    private bool SaveToSettings()
    {
        if (!SaveCurrentDbProfile())
        {
            return false;
        }

        _settings.Database.Host = _userDb.Host;
        _settings.Database.Port = _userDb.Port;
        _settings.Database.User = _userDb.User;
        _settings.Database.Password = _userDb.Password;
        _settings.Database.TrustedConnection = _userDb.Trusted;
        _settings.Database.Provider = _userDb.Provider;

        _settings.Database.BillingHost = _billingDb.Host;
        _settings.Database.BillingPort = _billingDb.Port;
        _settings.Database.BillingDatabase = _billingDb.Database;
        _settings.Database.BillingUser = _billingDb.User;
        _settings.Database.BillingPassword = _billingDb.Password;
        _settings.Database.BillingTrustedConnection = _billingDb.Trusted;
        _settings.Database.BillingProvider = _billingDb.Provider;

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
        if (!int.TryParse(txtLoadLow.Text, out var load0) ||
            !int.TryParse(txtLoadMid.Text, out var load1) ||
            !int.TryParse(txtLoadHigh.Text, out var load2) ||
            load0 < 0 || load1 < 0 || load2 < 0 ||
            !(load0 < load1 && load1 < load2))
        {
            MessageBox.Show(this, "User load thresholds must be ascending non-negative integers.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        _settings.Network.UserLoadThresholds = new[] { load0, load1, load2 };
        return true;
    }

    private void LoadDbProfileToControls()
    {
        bool isBillingProfile = cboDbProfile.SelectedIndex == 1;
        var db = isBillingProfile ? _billingDb : _userDb;
        cboDbProvider.SelectedIndex = GetProviderIndex(db.Provider);
        txtDbHost.Text = db.Host;
        txtDbPort.Text = db.Port.ToString(CultureInfo.InvariantCulture);
        txtDbName.Text = isBillingProfile ? db.Database : "(from AccountServer)";
        txtDbName.ReadOnly = !isBillingProfile;
        txtDbUser.Text = db.User;
        txtDbPass.Text = db.Password;
        radAuthTrusted.Checked = db.Trusted;
        radAuthSql.Checked = !db.Trusted;
        UpdateAuthFields();
    }

    private bool SaveCurrentDbProfile()
    {
        var provider = GetSelectedProvider();
        int port = cboDbProfile.SelectedIndex == 1 ? _billingDb.Port : _userDb.Port;
        if (provider != LoginDatabaseProvider.Sqlite &&
            (!int.TryParse(txtDbPort.Text, out port) || port < 1 || port > 65535))
        {
            MessageBox.Show(this, "DB port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        if (provider == LoginDatabaseProvider.Sqlite &&
            int.TryParse(txtDbPort.Text, out var sqlitePort) &&
            sqlitePort >= 1 &&
            sqlitePort <= 65535)
        {
            port = sqlitePort;
        }

        var db = cboDbProfile.SelectedIndex == 1 ? _billingDb : _userDb;
        db.Provider = provider;
        db.Host = txtDbHost.Text.Trim();
        db.Port = port;
        if (cboDbProfile.SelectedIndex == 1)
        {
            db.Database = txtDbName.Text.Trim();
        }
        db.User = txtDbUser.Text.Trim();
        db.Password = txtDbPass.Text;
        db.Trusted = radAuthTrusted.Checked;
        return true;
    }

    private void OnDbProfileChanged(object? sender, EventArgs e)
    {
        // Save current profile before switching
        if (!SaveCurrentDbProfile())
        {
            return;
        }
        LoadDbProfileToControls();
    }

    private void OnAuthModeChanged(object? sender, EventArgs e)
    {
        UpdateAuthFields();
    }

    private void OnDbProviderChanged(object? sender, EventArgs e)
    {
        UpdateAuthFields();
    }

    private void UpdateAuthFields()
    {
        var provider = GetSelectedProvider();
        bool isSqlite = provider == LoginDatabaseProvider.Sqlite;
        bool isSqlServer = provider == LoginDatabaseProvider.SqlServer;
        bool sqlAuth = radAuthSql.Checked;

        txtDbHost.Enabled = !isSqlite;
        txtDbPort.Enabled = !isSqlite;
        grpAuth.Enabled = !isSqlite;

        if (!isSqlite && !isSqlServer && radAuthTrusted.Checked)
        {
            radAuthSql.Checked = true;
        }

        radAuthTrusted.Enabled = !isSqlite && isSqlServer;
        radAuthSql.Enabled = !isSqlite;
        txtDbUser.Enabled = !isSqlite && sqlAuth;
        txtDbPass.Enabled = !isSqlite && sqlAuth;
    }

    private LoginDatabaseProvider GetSelectedProvider()
    {
        return cboDbProvider.SelectedIndex switch
        {
            1 => LoginDatabaseProvider.MariaDb,
            2 => LoginDatabaseProvider.Sqlite,
            _ => LoginDatabaseProvider.SqlServer
        };
    }

    private static int GetProviderIndex(LoginDatabaseProvider provider)
    {
        return provider switch
        {
            LoginDatabaseProvider.MariaDb => 1,
            LoginDatabaseProvider.Sqlite => 2,
            _ => 0
        };
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

internal class DatabaseSnapshot
{
    public LoginDatabaseProvider Provider { get; set; } = LoginDatabaseProvider.SqlServer;
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; }
    public string Database { get; set; } = string.Empty;
    public string User { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
    public bool Trusted { get; set; }
}
