using System;
using System.Globalization;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using LoginServer.Data;
using LoginServer.Data.Contexts;
using LoginServer.Settings;
using Microsoft.EntityFrameworkCore;

namespace LoginServer.Settings;

public partial class SettingsForm : Form
{
    private readonly AppSettings _settings;
    private readonly DatabaseSnapshot _userDb;
    private readonly DatabaseSnapshot _billingDb;
    private readonly CheckBox _chkAutostart = new();
    private readonly TabPage _tabGeneral = new();

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
        InitializeAutostartControl();
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
        _chkAutostart.Checked = _settings.Autostart;
        var loads = _settings.Network.UserLoadThresholds ?? Array.Empty<int>();
        txtLoadLow.Text = (loads.Length > 0 ? loads[0] : 500).ToString(CultureInfo.InvariantCulture);
        txtLoadMid.Text = (loads.Length > 1 ? loads[1] : 1000).ToString(CultureInfo.InvariantCulture);
        txtLoadHigh.Text = (loads.Length > 2 ? loads[2] : 1500).ToString(CultureInfo.InvariantCulture);
    }

    private void InitializeAutostartControl()
    {
        _tabGeneral.Text = "General";
        _tabGeneral.Padding = new Padding(8);
        _tabGeneral.UseVisualStyleBackColor = true;

        var lblAutostart = new Label
        {
            AutoSize = true,
            Location = new System.Drawing.Point(10, 20),
            Name = "lblAutostart",
            Text = "Autostart"
        };

        _chkAutostart.AutoSize = true;
        _chkAutostart.Location = new System.Drawing.Point(124, 18);
        _chkAutostart.Name = "chkAutostart";
        _chkAutostart.TabIndex = 0;
        _chkAutostart.Text = "Start server automatically on launch";
        _chkAutostart.UseVisualStyleBackColor = true;

        _tabGeneral.Controls.Add(lblAutostart);
        _tabGeneral.Controls.Add(_chkAutostart);
        tabMain.TabPages.Insert(0, _tabGeneral);
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
        _settings.Autostart = _chkAutostart.Checked;
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
        txtDbHost.Text = DatabaseSettings.NormalizeSqlServerHost(
            db.Host,
            db.Provider == LoginDatabaseProvider.SqlServer && db.Trusted);
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
        db.Host = DatabaseSettings.NormalizeSqlServerHost(
            txtDbHost.Text.Trim(),
            provider == LoginDatabaseProvider.SqlServer && radAuthTrusted.Checked);
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

        txtDbPort.Enabled = !isSqlite;
        grpAuth.Enabled = !isSqlite;

        if (!isSqlite && !isSqlServer && radAuthTrusted.Checked)
        {
            radAuthSql.Checked = true;
        }

        bool useTrusted = isSqlServer && radAuthTrusted.Checked;
        if (useTrusted && !string.Equals(txtDbHost.Text, DatabaseSettings.TrustedSqlServerHost, StringComparison.Ordinal))
        {
            txtDbHost.Text = DatabaseSettings.TrustedSqlServerHost;
        }

        txtDbHost.Enabled = !isSqlite && !useTrusted;
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

    private async void OnReinstall(object? sender, EventArgs e)
    {
        const string prompt =
            "Reinstall LoginServer?\n\n" +
            "This will reset Properties.Settings, delete the JSON settings file, restart the application, and force the setup wizard to appear again on the next launch.";
        if (MessageBox.Show(this, prompt, "Reinstall LoginServer", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes)
        {
            return;
        }

        string databaseName = _settings.Database.BillingDatabase.Trim();
        var deleteDatabaseChoice = MessageBox.Show(
            this,
            $"Delete the configured Billing database too?\n\n" +
            $"Database: {databaseName}\n\n" +
            "Yes = delete the configured Billing database before reinstall.\n" +
            "No = keep the database and only reset LoginServer setup.\n" +
            "Cancel = abort reinstall.",
            "Delete Billing Database Too?",
            MessageBoxButtons.YesNoCancel,
            MessageBoxIcon.Question);
        if (deleteDatabaseChoice == DialogResult.Cancel)
        {
            return;
        }

        if (deleteDatabaseChoice == DialogResult.Yes)
        {
            try
            {
                await DeleteConfiguredBillingDatabaseAsync().ConfigureAwait(true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(
                    this,
                    $"Failed to delete the configured Billing database.\n\n{ex.Message}",
                    "Delete Database Failed",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
        }

        Properties.Settings.Default.Reset();
        Properties.Settings.Default.Save();
        if (File.Exists(AppSettings.DefaultPath))
        {
            File.Delete(AppSettings.DefaultPath);
        }
        Application.Restart();
        Application.Exit();
    }

    private async Task DeleteConfiguredBillingDatabaseAsync()
    {
        if (string.IsNullOrWhiteSpace(_settings.Database.BillingDatabase))
        {
            return;
        }

        string billingConnStr = _settings.Database.BuildBillingConnectionString();
        var options = DbContextOptionsFactory.Create<BillingDbContext>(_settings.Database.BillingProvider, billingConnStr);
        await using var billingDb = new BillingDbContext(options);
        await billingDb.Database.EnsureDeletedAsync().ConfigureAwait(true);
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
