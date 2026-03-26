using System;
using System.Drawing;
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
    private readonly CheckBox _chkAutoOpenExternalConnection = new();
    private readonly CheckBox _chkVerboseLogging = new();
    private readonly CheckBox _chkTrustedConnection = new();
    private readonly TabPage _tabGeneral = new();
    private bool _suppressDbProfileEvents;
    private int _activeDbProfileIndex;

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
        ApplyModernLayout();
        HideDbPortControls();
        LoadFromSettings();
    }

    private void ApplyModernLayout()
    {
        SuspendLayout();
        Text = "Login Server Settings";
        ClientSize = new Size(700, 470);
        MinimumSize = new Size(700, 470);
        tabMain.Padding = new Point(12, 6);

        InitializeAutostartControl();
        RedesignDatabaseTab();
        RedesignConnectionTab();
        ResumeLayout(true);
    }

    private void HideDbPortControls()
    {
        lblDbPort.Visible = false;
        txtDbPort.Visible = false;
    }

    private void LoadFromSettings()
    {
        _suppressDbProfileEvents = true;
        try
        {
            cboDbProfile.Items.Clear();
            cboDbProfile.Items.AddRange(new object[] { "User DB", "Billing DB" });
            _activeDbProfileIndex = 0;
            cboDbProfile.SelectedIndex = _activeDbProfileIndex;
            LoadDbProfileToControls();
            txtClientPort.Text = _settings.Network.ClientPort.ToString(CultureInfo.InvariantCulture);
            txtAccountHost.Text = _settings.Network.AccountHost;
            txtAccountPort.Text = _settings.Network.AccountPort.ToString(CultureInfo.InvariantCulture);
            txtMaxConn.Text = _settings.Network.MaxConnections.ToString(CultureInfo.InvariantCulture);
            _chkAutostart.Checked = _settings.Autostart;
            _chkAutoOpenExternalConnection.Checked = _settings.AutoOpenExternalConnection;
            _chkVerboseLogging.Checked = _settings.VerboseLogging;
            var loads = _settings.Network.UserLoadThresholds ?? Array.Empty<int>();
            txtLoadLow.Text = (loads.Length > 0 ? loads[0] : 500).ToString(CultureInfo.InvariantCulture);
            txtLoadMid.Text = (loads.Length > 1 ? loads[1] : 1000).ToString(CultureInfo.InvariantCulture);
            txtLoadHigh.Text = (loads.Length > 2 ? loads[2] : 1500).ToString(CultureInfo.InvariantCulture);
        }
        finally
        {
            _suppressDbProfileEvents = false;
        }
    }

    private void InitializeAutostartControl()
    {
        _tabGeneral.Text = "General";
        _tabGeneral.Padding = new Padding(12);
        _tabGeneral.UseVisualStyleBackColor = true;

        var autostartPanel = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            BackColor = Color.FromArgb(255, 247, 223),
            ColumnCount = 2,
            Dock = DockStyle.Top,
            Margin = new Padding(0),
            Padding = new Padding(12, 10, 12, 10)
        };
        autostartPanel.ColumnStyles.Add(new ColumnStyle());
        autostartPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));

        var lblAutostart = new Label
        {
            AutoSize = true,
            Name = "lblAutostart",
            Text = "Autostart",
            Anchor = AnchorStyles.Left,
            ForeColor = Color.FromArgb(138, 82, 0),
            BackColor = autostartPanel.BackColor,
            Margin = new Padding(0, 3, 16, 0)
        };

        _chkAutostart.AutoSize = true;
        _chkAutostart.Name = "chkAutostart";
        _chkAutostart.TabIndex = 0;
        _chkAutostart.Text = "Start server automatically on launch";
        _chkAutostart.Anchor = AnchorStyles.Left;
        _chkAutostart.BackColor = autostartPanel.BackColor;
        _chkAutostart.ForeColor = Color.FromArgb(138, 82, 0);
        _chkAutostart.UseVisualStyleBackColor = false;

        var lblAutoOpenExternal = new Label
        {
            AutoSize = true,
            Name = "lblAutoOpenExternal",
            Text = "External Login",
            Anchor = AnchorStyles.Left,
            ForeColor = Color.FromArgb(138, 82, 0),
            BackColor = autostartPanel.BackColor,
            Margin = new Padding(0, 9, 16, 0)
        };

        _chkAutoOpenExternalConnection.AutoSize = true;
        _chkAutoOpenExternalConnection.Name = "chkAutoOpenExternalConnection";
        _chkAutoOpenExternalConnection.TabIndex = 1;
        _chkAutoOpenExternalConnection.Text = "Automatically open external connection on start";
        _chkAutoOpenExternalConnection.Anchor = AnchorStyles.Left;
        _chkAutoOpenExternalConnection.BackColor = autostartPanel.BackColor;
        _chkAutoOpenExternalConnection.ForeColor = Color.FromArgb(138, 82, 0);
        _chkAutoOpenExternalConnection.UseVisualStyleBackColor = false;

        var lblVerboseLogging = new Label
        {
            AutoSize = true,
            Name = "lblVerboseLogging",
            Text = "Verbose Logging",
            Anchor = AnchorStyles.Left,
            ForeColor = Color.FromArgb(138, 82, 0),
            BackColor = autostartPanel.BackColor,
            Margin = new Padding(0, 9, 16, 0)
        };

        _chkVerboseLogging.AutoSize = true;
        _chkVerboseLogging.Name = "chkVerboseLogging";
        _chkVerboseLogging.TabIndex = 2;
        _chkVerboseLogging.Text = "Show packet-level and client connection debug logs";
        _chkVerboseLogging.Anchor = AnchorStyles.Left;
        _chkVerboseLogging.BackColor = autostartPanel.BackColor;
        _chkVerboseLogging.ForeColor = Color.FromArgb(138, 82, 0);
        _chkVerboseLogging.UseVisualStyleBackColor = false;

        ConfigureDangerGroup();

        var generalLayout = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 1,
            Padding = new Padding(0),
            RowCount = 3
        };
        generalLayout.RowStyles.Add(new RowStyle());
        generalLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100F));
        generalLayout.RowStyles.Add(new RowStyle());

        var generalInfo = new Label
        {
            AutoSize = true,
            Text = "General application behavior and maintenance actions.",
            ForeColor = SystemColors.GrayText,
            Margin = new Padding(0, 18, 0, 0)
        };

        autostartPanel.RowCount = 3;
        autostartPanel.RowStyles.Add(new RowStyle());
        autostartPanel.RowStyles.Add(new RowStyle());
        autostartPanel.RowStyles.Add(new RowStyle());
        autostartPanel.Controls.Add(lblAutostart, 0, 0);
        autostartPanel.Controls.Add(_chkAutostart, 1, 0);
        autostartPanel.Controls.Add(lblAutoOpenExternal, 0, 1);
        autostartPanel.Controls.Add(_chkAutoOpenExternalConnection, 1, 1);
        autostartPanel.Controls.Add(lblVerboseLogging, 0, 2);
        autostartPanel.Controls.Add(_chkVerboseLogging, 1, 2);

        generalLayout.Controls.Add(autostartPanel, 0, 0);
        generalLayout.Controls.Add(generalInfo, 0, 1);
        generalLayout.Controls.Add(grpDanger, 0, 2);

        _tabGeneral.Controls.Clear();
        _tabGeneral.Controls.Add(generalLayout);
        if (!tabMain.TabPages.Contains(_tabGeneral))
        {
            tabMain.TabPages.Insert(0, _tabGeneral);
        }
    }

    private void ConfigureDangerGroup()
    {
        grpDanger.SuspendLayout();
        grpDanger.Controls.Clear();
        grpDanger.AutoSize = true;
        grpDanger.AutoSizeMode = AutoSizeMode.GrowAndShrink;
        grpDanger.Dock = DockStyle.Fill;
        grpDanger.Margin = new Padding(0, 24, 0, 0);
        grpDanger.Padding = new Padding(12);

        lblReinstallWarning.AutoSize = true;
        lblReinstallWarning.MaximumSize = new Size(600, 0);
        lblReinstallWarning.Margin = new Padding(0, 0, 0, 10);

        btnReinstall.AutoSize = true;
        btnReinstall.Margin = new Padding(0);

        var dangerLayout = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            ColumnCount = 1,
            Dock = DockStyle.Top,
            Margin = new Padding(0)
        };
        dangerLayout.RowStyles.Add(new RowStyle());
        dangerLayout.RowStyles.Add(new RowStyle());
        dangerLayout.Controls.Add(lblReinstallWarning, 0, 0);
        dangerLayout.Controls.Add(btnReinstall, 0, 1);

        grpDanger.Controls.Add(dangerLayout);
        grpDanger.ResumeLayout(true);
    }

    private void RedesignDatabaseTab()
    {
        lblDbHost.Text = "Server / endpoint";
        lblDbName.Text = "Database name";
        lblDbUser.Text = "Username";
        lblDbPass.Text = "Password";

        cboDbProfile.Dock = DockStyle.Fill;
        cboDbProvider.Dock = DockStyle.Fill;
        txtDbHost.Dock = DockStyle.Fill;
        txtDbName.Dock = DockStyle.Fill;
        txtDbUser.Dock = DockStyle.Fill;
        txtDbPass.Dock = DockStyle.Fill;

        grpAuth.Visible = false;
        radAuthSql.Visible = false;
        radAuthTrusted.Visible = false;

        _chkTrustedConnection.AutoSize = true;
        _chkTrustedConnection.Margin = new Padding(0, 3, 0, 0);
        _chkTrustedConnection.Text = "Trusted connection (Windows)";
        _chkTrustedConnection.CheckedChanged -= OnAuthModeChanged;
        _chkTrustedConnection.CheckedChanged += OnAuthModeChanged;

        var lblAuthentication = new Label
        {
            AutoSize = true,
            Text = "Authentication",
            Anchor = AnchorStyles.Left,
            Margin = new Padding(0, 6, 0, 0)
        };

        var root = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            ColumnCount = 2,
            Dock = DockStyle.Top,
            Margin = new Padding(0)
        };
        root.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130F));
        root.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());

        var endpointHint = new Label
        {
            AutoSize = true,
            ForeColor = SystemColors.GrayText,
            Margin = new Padding(0, 2, 0, 8),
            Text = "For SQL Server, use (local) or append the port directly in the host like 127.0.0.1,1433."
        };

        root.Controls.Add(lblDbProfile, 0, 0);
        root.Controls.Add(cboDbProfile, 1, 0);
        root.Controls.Add(lblDbProvider, 0, 1);
        root.Controls.Add(cboDbProvider, 1, 1);
        root.Controls.Add(lblDbHost, 0, 2);
        root.Controls.Add(txtDbHost, 1, 2);
        root.Controls.Add(endpointHint, 1, 3);
        root.Controls.Add(lblDbName, 0, 4);
        root.Controls.Add(txtDbName, 1, 4);
        root.Controls.Add(lblAuthentication, 0, 5);
        root.Controls.Add(_chkTrustedConnection, 1, 5);
        root.Controls.Add(lblDbUser, 0, 6);
        root.Controls.Add(txtDbUser, 1, 6);
        root.Controls.Add(lblDbPass, 0, 7);
        root.Controls.Add(txtDbPass, 1, 7);

        var container = new Panel
        {
            Dock = DockStyle.Fill,
            AutoScroll = true,
            Padding = new Padding(12)
        };
        container.Controls.Add(root);

        tabDatabase.SuspendLayout();
        tabDatabase.Controls.Clear();
        tabDatabase.Padding = new Padding(0);
        tabDatabase.Controls.Add(container);
        tabDatabase.ResumeLayout(true);
    }

    private void RedesignConnectionTab()
    {
        txtClientPort.Width = 120;
        txtAccountHost.Width = 220;
        txtAccountPort.Width = 120;
        txtMaxConn.Width = 120;
        txtLoadLow.Width = 72;
        txtLoadMid.Width = 72;
        txtLoadHigh.Width = 72;

        var networkGroup = CreateSectionGroup("Network");
        var networkTable = CreateTwoColumnGrid(3);
        lblAccountHost.Text = "AccountServer host";
        lblAccountPort.Text = "AccountServer port";
        networkTable.Controls.Add(lblClientPort, 0, 0);
        networkTable.Controls.Add(txtClientPort, 1, 0);
        networkTable.Controls.Add(lblAccountHost, 0, 1);
        networkTable.Controls.Add(txtAccountHost, 1, 1);
        networkTable.Controls.Add(lblAccountPort, 0, 2);
        networkTable.Controls.Add(txtAccountPort, 1, 2);
        networkGroup.Controls.Add(networkTable);

        var capacityGroup = CreateSectionGroup("Capacity");
        var capacityLayout = CreateTwoColumnGrid(3);
        var loadHint = new Label
        {
            AutoSize = true,
            ForeColor = SystemColors.GrayText,
            Margin = new Padding(0, 3, 0, 0),
            Text = "User load thresholds"
        };

        var thresholdTable = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            ColumnCount = 6,
            Dock = DockStyle.Left,
            Margin = new Padding(0)
        };
        thresholdTable.ColumnStyles.Add(new ColumnStyle());
        thresholdTable.ColumnStyles.Add(new ColumnStyle());
        thresholdTable.ColumnStyles.Add(new ColumnStyle());
        thresholdTable.ColumnStyles.Add(new ColumnStyle());
        thresholdTable.ColumnStyles.Add(new ColumnStyle());
        thresholdTable.ColumnStyles.Add(new ColumnStyle());

        var lblLoadLowValue = new Label { AutoSize = true, Text = "Low", Anchor = AnchorStyles.Left, Margin = new Padding(0, 5, 8, 0) };
        var lblLoadMidValue = new Label { AutoSize = true, Text = "Medium", Anchor = AnchorStyles.Left, Margin = new Padding(16, 5, 8, 0) };
        var lblLoadHighValue = new Label { AutoSize = true, Text = "High", Anchor = AnchorStyles.Left, Margin = new Padding(16, 5, 8, 0) };
        thresholdTable.Controls.Add(lblLoadLowValue, 0, 0);
        thresholdTable.Controls.Add(txtLoadLow, 1, 0);
        thresholdTable.Controls.Add(lblLoadMidValue, 2, 0);
        thresholdTable.Controls.Add(txtLoadMid, 3, 0);
        thresholdTable.Controls.Add(lblLoadHighValue, 4, 0);
        thresholdTable.Controls.Add(txtLoadHigh, 5, 0);

        capacityLayout.Controls.Add(lblMaxConn, 0, 0);
        capacityLayout.Controls.Add(txtMaxConn, 1, 0);
        capacityLayout.Controls.Add(loadHint, 0, 1);
        capacityLayout.SetColumnSpan(loadHint, 2);
        capacityLayout.Controls.Add(thresholdTable, 1, 2);

        capacityGroup.Controls.Add(capacityLayout);

        var root = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            ColumnCount = 1,
            Dock = DockStyle.Top,
            Margin = new Padding(0)
        };
        root.RowStyles.Add(new RowStyle());
        root.RowStyles.Add(new RowStyle());
        root.Controls.Add(networkGroup, 0, 0);
        root.Controls.Add(capacityGroup, 0, 1);

        var container = new Panel
        {
            Dock = DockStyle.Fill,
            AutoScroll = true,
            Padding = new Padding(12)
        };
        container.Controls.Add(root);

        tabConnection.SuspendLayout();
        tabConnection.Controls.Clear();
        tabConnection.Padding = new Padding(0);
        tabConnection.Controls.Add(container);
        tabConnection.ResumeLayout(true);
    }

    private static GroupBox CreateSectionGroup(string text)
    {
        return new GroupBox
        {
            Text = text,
            Dock = DockStyle.Top,
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            Padding = new Padding(12),
            Margin = new Padding(0, 0, 0, 14)
        };
    }

    private static TableLayoutPanel CreateTwoColumnGrid(int rowCount)
    {
        var table = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            ColumnCount = 2,
            Dock = DockStyle.Top,
            Margin = new Padding(0)
        };
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130F));
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        for (int i = 0; i < rowCount; i++)
        {
            table.RowStyles.Add(new RowStyle());
        }
        return table;
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
        _settings.AutoOpenExternalConnection = _chkAutoOpenExternalConnection.Checked;
        _settings.VerboseLogging = _chkVerboseLogging.Checked;
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
        bool isBillingProfile = _activeDbProfileIndex == 1;
        var db = isBillingProfile ? _billingDb : _userDb;
        cboDbProvider.SelectedIndex = GetProviderIndex(db.Provider);
        txtDbHost.Text = DatabaseSettings.GetDisplayHost(
            db.Provider,
            db.Host,
            db.Port,
            db.Provider == LoginDatabaseProvider.SqlServer && db.Trusted);
        txtDbName.Text = isBillingProfile ? db.Database : "(from AccountServer)";
        txtDbName.ReadOnly = !isBillingProfile;
        _chkTrustedConnection.Checked = db.Trusted;
        txtDbUser.Text = db.User;
        txtDbPass.Text = db.Password;
        UpdateAuthFields();
    }

    private bool SaveCurrentDbProfile()
    {
        var provider = GetSelectedProvider();
        var db = _activeDbProfileIndex == 1 ? _billingDb : _userDb;
        db.Provider = provider;
        db.Host = DatabaseSettings.NormalizeHostForStorage(
            provider,
            txtDbHost.Text.Trim(),
            provider == LoginDatabaseProvider.SqlServer && _chkTrustedConnection.Checked);
        db.Port = DatabaseSettings.GetEffectivePort(
            provider,
            db.Host,
            provider == LoginDatabaseProvider.MariaDb ? DatabaseSettings.DefaultMariaDbPort : DatabaseSettings.DefaultSqlServerPort);
        if (_activeDbProfileIndex == 1)
        {
            db.Database = txtDbName.Text.Trim();
        }
        db.User = txtDbUser.Text.Trim();
        db.Password = txtDbPass.Text;
        db.Trusted = _chkTrustedConnection.Checked;
        return true;
    }

    private void OnDbProfileChanged(object? sender, EventArgs e)
    {
        if (_suppressDbProfileEvents)
        {
            return;
        }

        int newProfileIndex = cboDbProfile.SelectedIndex;
        if (newProfileIndex == _activeDbProfileIndex)
        {
            return;
        }

        if (!SaveCurrentDbProfile())
        {
            _suppressDbProfileEvents = true;
            try
            {
                cboDbProfile.SelectedIndex = _activeDbProfileIndex;
            }
            finally
            {
                _suppressDbProfileEvents = false;
            }
            return;
        }

        _activeDbProfileIndex = newProfileIndex;
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

        if (!isSqlite && !isSqlServer && _chkTrustedConnection.Checked)
        {
            _chkTrustedConnection.Checked = false;
            return;
        }

        bool useTrusted = isSqlServer && _chkTrustedConnection.Checked;
        bool useSqlCredentials = !isSqlite && (provider == LoginDatabaseProvider.MariaDb || !useTrusted);

        if (useTrusted && !string.Equals(txtDbHost.Text, DatabaseSettings.TrustedSqlServerHost, StringComparison.Ordinal))
        {
            txtDbHost.Text = DatabaseSettings.TrustedSqlServerHost;
        }

        txtDbHost.Enabled = !isSqlite && !useTrusted;
        _chkTrustedConnection.Enabled = !isSqlite && isSqlServer;
        txtDbUser.Enabled = useSqlCredentials;
        txtDbPass.Enabled = useSqlCredentials;
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
        AppSettings.DeletePersistedSettingsFiles();
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
