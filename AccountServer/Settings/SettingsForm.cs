using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Data.SqlClient;
using Microsoft.Data.Sqlite;
using MySqlConnector;

namespace AccountServer.Settings;

public partial class SettingsForm : Form
{
    private readonly AppSettings _settings;
    private readonly DatabaseSnapshot _userDb;
    private readonly BindingList<WorldEntry> _worlds;
    private readonly CheckBox _chkAutostart = new();
    private readonly CheckBox _chkVerboseLogging = new();
    private readonly bool _worldEntryEditingLocked;

    public SettingsForm()
        : this(new AppSettings(), true, false)
    {
    }

    public SettingsForm(AppSettings settings)
        : this(settings, false, false)
    {
    }

    public SettingsForm(AppSettings settings, bool worldEntryEditingLocked)
        : this(settings, false, worldEntryEditingLocked)
    {
    }

    private SettingsForm(AppSettings settings, bool designTime, bool worldEntryEditingLocked)
    {
        _settings = settings;
        _userDb = DatabaseSnapshot.From(settings.Database.User);
        _worlds = new BindingList<WorldEntry>(settings.WorldList.Worlds.Select(CloneWorldEntry).ToList());
        _worldEntryEditingLocked = worldEntryEditingLocked;
        InitializeComponent();
        HideDatabasePortControls();
        InitializeAutostartControl();
        InitializeWorldListEditingLock();
        gridWorlds.DataSource = _worlds;
        HideExtraWorldColumns();
        if (!designTime)
        {
            LoadFromSettings();
        }
    }

    private void HideExtraWorldColumns()
    {
        var columns = gridWorlds?.Columns;
        if (columns == null)
        {
            return;
        }
        if (columns.Contains("IsService"))
        {
            var col = columns["IsService"];
            if (col != null)
            {
                col.Visible = false;
            }
        }
        if (columns.Contains("GateIp"))
        {
            var col = columns["GateIp"];
            if (col != null)
            {
                col.Visible = false;
            }
        }
        if (columns.Contains("GatePort"))
        {
            var col = columns["GatePort"];
            if (col != null)
            {
                col.Visible = false;
            }
        }
    }

    private void HideDatabasePortControls()
    {
        lblDbPort.Visible = false;
        txtDbPort.Visible = false;
    }

    private void InitializeWorldListEditingLock()
    {
        if (!_worldEntryEditingLocked)
        {
            return;
        }

        btnWorldAdd.Enabled = false;
        tabMain.Selecting += OnTabMainSelecting;
    }

    private void OnTabMainSelecting(object? sender, TabControlCancelEventArgs e)
    {
        if (_worldEntryEditingLocked && ReferenceEquals(e.TabPage, tabWorldList))
        {
            e.Cancel = true;
            ShowWorldEntryEditingLockedMessage();
        }
    }

    private static WorldEntry CloneWorldEntry(WorldEntry entry)
    {
        return new WorldEntry
        {
            Name = entry.Name,
            Address = entry.Address,
            DbName = entry.DbName,
            Type = entry.Type,
            IsService = entry.IsService,
            GateIp = entry.GateIp,
            GatePort = entry.GatePort
        };
    }


    private void LoadFromSettings()
    {
        LoadDbProfileToControls();
        LoadGeneralSettings();
    }

    private void InitializeAutostartControl()
    {
        var autostartPanel = new TableLayoutPanel
        {
            AutoSize = true,
            AutoSizeMode = AutoSizeMode.GrowAndShrink,
            BackColor = System.Drawing.Color.FromArgb(255, 247, 223),
            ColumnCount = 2,
            Dock = DockStyle.Fill,
            Margin = new Padding(3, 6, 3, 0),
            Padding = new Padding(12, 10, 12, 10)
        };
        autostartPanel.ColumnStyles.Add(new ColumnStyle());
        autostartPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));

        var lblAutostart = new Label
        {
            AutoSize = true,
            Text = "Autostart",
            Anchor = AnchorStyles.Left,
            ForeColor = System.Drawing.Color.FromArgb(138, 82, 0),
            BackColor = autostartPanel.BackColor,
            Margin = new Padding(0, 3, 16, 0)
        };

        _chkAutostart.AutoSize = true;
        _chkAutostart.Text = "Start server automatically on launch";
        _chkAutostart.Anchor = AnchorStyles.Left;
        _chkAutostart.BackColor = autostartPanel.BackColor;
        _chkAutostart.ForeColor = System.Drawing.Color.FromArgb(138, 82, 0);
        _chkAutostart.UseVisualStyleBackColor = false;

        var lblVerboseLogging = new Label
        {
            AutoSize = true,
            Text = "Verbose Logging",
            Anchor = AnchorStyles.Left,
            ForeColor = System.Drawing.Color.FromArgb(138, 82, 0),
            BackColor = autostartPanel.BackColor,
            Margin = new Padding(0, 9, 16, 0)
        };

        _chkVerboseLogging.AutoSize = true;
        _chkVerboseLogging.Text = "Show packet-level and connection debug logs";
        _chkVerboseLogging.Anchor = AnchorStyles.Left;
        _chkVerboseLogging.BackColor = autostartPanel.BackColor;
        _chkVerboseLogging.ForeColor = System.Drawing.Color.FromArgb(138, 82, 0);
        _chkVerboseLogging.UseVisualStyleBackColor = false;

        autostartPanel.RowCount = 2;
        autostartPanel.RowStyles.Add(new RowStyle());
        autostartPanel.RowStyles.Add(new RowStyle());
        autostartPanel.Controls.Add(lblAutostart, 0, 0);
        autostartPanel.Controls.Add(_chkAutostart, 1, 0);
        autostartPanel.Controls.Add(lblVerboseLogging, 0, 1);
        autostartPanel.Controls.Add(_chkVerboseLogging, 1, 1);

        int row = tblGeneral.RowCount;
        tblGeneral.RowCount = row + 1;
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.Controls.Add(autostartPanel, 0, row);
        tblGeneral.SetColumnSpan(autostartPanel, 2);
    }

    private void LoadDbProfileToControls()
    {
        var db = _userDb;
        txtDbHost.Text = DbProfile.GetDisplayHost(
            _settings.Database.Provider,
            db.Host,
            db.Port,
            db.Trusted);
        txtDbName.Text = db.Database;
        txtDbUser.Text = db.User;
        txtDbPass.Text = db.Password;
        radAuthTrusted.Checked = db.Trusted;
        radAuthSql.Checked = !db.Trusted;
        UpdateAuthFields();
    }

    private void LoadGeneralSettings()
    {
        txtListenHost.Text = _settings.Listener.Host;
        _chkAutostart.Checked = _settings.Autostart;
        _chkVerboseLogging.Checked = _settings.VerboseLogging;

        numLoginPort.Value = ClampNumeric(numLoginPort, _settings.Listener.LoginPort);
        numWorldPort.Value = ClampNumeric(numWorldPort, _settings.Listener.WorldPort);
        numControlPort.Value = ClampNumeric(numControlPort, _settings.Listener.ControlPort);
        numMaxActive.Value = ClampNumeric(numMaxActive, _settings.MaxActiveClients);
    }

    private static decimal ClampNumeric(NumericUpDown control, int value)
    {
        decimal min = control.Minimum;
        decimal max = control.Maximum;
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    private bool SaveCurrentDbProfile()
    {
        _userDb.Host = DbProfile.NormalizeHostForStorage(
            _settings.Database.Provider,
            txtDbHost.Text.Trim(),
            radAuthTrusted.Checked);
        _userDb.Port = DbProfile.GetEffectivePort(
            _settings.Database.Provider,
            _userDb.Host,
            _settings.Database.Provider == DatabaseProvider.MariaDb
                ? DbProfile.DefaultMariaDbPort
                : DbProfile.DefaultSqlServerPort);
        _userDb.Database = txtDbName.Text.Trim();
        _userDb.User = txtDbUser.Text.Trim();
        _userDb.Password = txtDbPass.Text;
        _userDb.Trusted = radAuthTrusted.Checked;
        return true;
    }

    private bool SaveToSettings()
    {
        if (!SaveCurrentDbProfile())
        {
            return false;
        }

        if (_worlds.Count == 0)
        {
            MessageBox.Show(
                this,
                "At least one world entry is required.",
                "World Entry Required",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning);
            return false;
        }

        _userDb.ApplyTo(_settings.Database.User);
        _settings.WorldList.Worlds = _worlds.Select(CloneWorldEntry).ToList();
        _settings.MaxActiveClients = (int)numMaxActive.Value;
        _settings.Autostart = _chkAutostart.Checked;
        _settings.VerboseLogging = _chkVerboseLogging.Checked;
        _settings.Listener.Host = txtListenHost.Text.Trim();
        _settings.Listener.LoginPort = (int)numLoginPort.Value;
        _settings.Listener.WorldPort = (int)numWorldPort.Value;
        _settings.Listener.ControlPort = (int)numControlPort.Value;
        return true;
    }


    private void OnAuthModeChanged(object? sender, EventArgs e)
    {
        UpdateAuthFields();
    }

    private void UpdateAuthFields()
    {
        bool isSqlServer = _settings.Database.Provider == DatabaseProvider.SqlServer;
        bool isSqlite = _settings.Database.Provider == DatabaseProvider.Sqlite;

        if (!isSqlServer && radAuthTrusted.Checked)
        {
            radAuthSql.Checked = true;
        }

        bool useTrusted = isSqlServer && radAuthTrusted.Checked;
        bool sqlAuth = !isSqlite && (!isSqlServer || radAuthSql.Checked);

        if (useTrusted && !string.Equals(txtDbHost.Text, DbProfile.TrustedSqlServerHost, StringComparison.Ordinal))
        {
            txtDbHost.Text = DbProfile.TrustedSqlServerHost;
        }

        txtDbHost.Enabled = !isSqlite && !useTrusted;
        radAuthTrusted.Enabled = isSqlServer;
        radAuthSql.Enabled = !isSqlite;
        txtDbUser.Enabled = sqlAuth;
        txtDbPass.Enabled = sqlAuth;
    }

    private string GetEffectiveDbHost(string host, bool trusted)
    {
        return _settings.Database.Provider == DatabaseProvider.SqlServer && trusted
            ? DbProfile.TrustedSqlServerHost
            : host;
    }

    private void OnWorldAdd(object? sender, EventArgs e)
    {
        if (!EnsureWorldListEditable())
        {
            return;
        }

        var entry = new WorldEntry();
        using var form = new WorldEntryForm(entry);
        if (form.ShowDialog(this) != DialogResult.OK)
        {
            return;
        }
        _worlds.Add(entry);
    }

    private async void OnWorldGridClick(object? sender, DataGridViewCellEventArgs e)
    {
        if (!EnsureWorldListEditable())
        {
            return;
        }

        if (e.RowIndex < 0 || e.ColumnIndex < 0)
        {
            return;
        }

        var entry = _worlds[e.RowIndex];
        var column = gridWorlds.Columns[e.ColumnIndex];
        if (ReferenceEquals(column, colWorldEdit))
        {
            using var form = new WorldEntryForm(entry);
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                _worlds.ResetItem(e.RowIndex);
            }
            return;
        }
        if (ReferenceEquals(column, colWorldDelete))
        {
            if (_worlds.Count <= 1)
            {
                MessageBox.Show(
                    this,
                    "At least one world entry is required. Add another world entry before removing this one.",
                    "World Entry Required",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Warning);
                return;
            }

            DialogResult deleteChoice = MessageBox.Show(
                this,
                BuildDeleteWorldPrompt(entry),
                "Remove World Entry",
                MessageBoxButtons.YesNoCancel,
                MessageBoxIcon.Question);
            if (deleteChoice == DialogResult.Cancel)
            {
                return;
            }

            if (deleteChoice == DialogResult.Yes)
            {
                try
                {
                    await DeleteWorldDatabaseAsync(entry).ConfigureAwait(true);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(
                        this,
                        $"Failed to delete world database '{entry.DbName}'.\n\n{ex.Message}",
                        "Delete World Database Failed",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                    return;
                }
            }

            _worlds.RemoveAt(e.RowIndex);
        }
    }

    private bool EnsureWorldListEditable()
    {
        if (!_worldEntryEditingLocked)
        {
            return true;
        }

        ShowWorldEntryEditingLockedMessage();
        return false;
    }

    private void ShowWorldEntryEditingLockedMessage()
    {
        MessageBox.Show(
            this,
            "Stop AccountServer first before editing world entries.",
            "World Entry Editing Locked",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information);
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
            "Reinstall AccountServer?\n\n" +
            "This will reset Properties.Settings, delete the JSON settings file, restart the application, and force the setup wizard to appear again on the next launch.";
        if (MessageBox.Show(this, prompt, "Reinstall AccountServer", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes)
        {
            return;
        }

        string databaseName = _settings.Database.User.Database.Trim();
        var deleteDatabaseChoice = MessageBox.Show(
            this,
            $"Delete the configured RF_User database too?\n\n" +
            $"Database: {databaseName}\n\n" +
            "Yes = delete the configured database before reinstall.\n" +
            "No = keep the database and only reset AccountServer setup.\n" +
            "Cancel = abort reinstall.",
            "Delete RF_User Database Too?",
            MessageBoxButtons.YesNoCancel,
            MessageBoxIcon.Question);
        if (deleteDatabaseChoice == DialogResult.Cancel)
        {
            return;
        }

        var configuredWorldDatabases = GetDistinctWorldDatabases(_settings.WorldList.Worlds);
        DialogResult deleteWorldDatabasesChoice = MessageBox.Show(
            this,
            BuildDeleteWorldDatabasesPrompt(configuredWorldDatabases),
            "Delete World Databases Too?",
            MessageBoxButtons.YesNoCancel,
            MessageBoxIcon.Question);
        if (deleteWorldDatabasesChoice == DialogResult.Cancel)
        {
            return;
        }

        if (deleteDatabaseChoice == DialogResult.Yes)
        {
            try
            {
                await DeleteConfiguredUserDatabaseAsync().ConfigureAwait(true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(
                    this,
                    $"Failed to delete the configured RF_User database.\n\n{ex.Message}",
                    "Delete Database Failed",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
        }

        if (deleteWorldDatabasesChoice == DialogResult.Yes)
        {
            try
            {
                await DeleteWorldDatabasesAsync(configuredWorldDatabases).ConfigureAwait(true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(
                    this,
                    $"Failed to delete one or more configured world databases.\n\n{ex.Message}",
                    "Delete World Databases Failed",
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

    private Task DeleteConfiguredUserDatabaseAsync()
    {
        return DeleteDatabaseAsync(_settings.Database.User);
    }

    private Task DeleteWorldDatabaseAsync(WorldEntry entry)
    {
        return DeleteDatabaseAsync(CreateWorldDatabaseProfile(entry));
    }

    private async Task DeleteWorldDatabasesAsync(IReadOnlyList<WorldEntry> worlds)
    {
        foreach (var world in worlds)
        {
            await DeleteWorldDatabaseAsync(world).ConfigureAwait(true);
        }
    }

    private Task DeleteDatabaseAsync(DbProfile profile)
    {
        return _settings.Database.Provider switch
        {
            DatabaseProvider.Sqlite => DeleteSqliteDatabaseAsync(profile),
            DatabaseProvider.MariaDb => DeleteMariaDbDatabaseAsync(profile),
            _ => DeleteSqlServerDatabaseAsync(profile)
        };
    }

    private async Task DeleteSqlServerDatabaseAsync(DbProfile profile)
    {
        string connString = BuildSqlServerConnectionString(profile, "master");
        await using var conn = new SqlConnection(connString);
        await conn.OpenAsync().ConfigureAwait(true);

        const string dropSql =
            """
            IF DB_ID(@name) IS NOT NULL
            BEGIN
                DECLARE @quotedName nvarchar(258) = QUOTENAME(@name);
                DECLARE @dropSql nvarchar(max) =
                    N'ALTER DATABASE ' + @quotedName + N' SET SINGLE_USER WITH ROLLBACK IMMEDIATE; ' +
                    N'DROP DATABASE ' + @quotedName + N';';
                EXEC(@dropSql);
            END
            """;

        await using var cmd = new SqlCommand(dropSql, conn);
        cmd.Parameters.AddWithValue("@name", profile.Database);
        await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
    }

    private async Task DeleteMariaDbDatabaseAsync(DbProfile profile)
    {
        string connString = BuildMariaDbConnectionString(profile, "information_schema");
        await using var conn = new MySqlConnection(connString);
        await conn.OpenAsync().ConfigureAwait(true);

        string escapedName = EscapeMariaDbIdentifier(profile.Database);
        string dropSql = $"DROP DATABASE IF EXISTS `{escapedName}`;";
        await using var cmd = new MySqlCommand(dropSql, conn);
        await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
    }

    private Task DeleteSqliteDatabaseAsync(DbProfile profile)
    {
        string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, profile.Database);
        if (File.Exists(dbPath))
        {
            File.Delete(dbPath);
        }

        return Task.CompletedTask;
    }

    private DbProfile CreateWorldDatabaseProfile(WorldEntry entry)
    {
        return new DbProfile
        {
            Host = WorldEntry.NormalizeDatabaseServerAddress(entry.Address),
            Port = DbProfile.GetEffectivePort(
                _settings.Database.Provider,
                _settings.Database.User.Host,
                _settings.Database.User.Port),
            Database = entry.DbName.Trim(),
            User = _settings.Database.User.User,
            Password = _settings.Database.User.Password,
            TrustedConnection = _settings.Database.User.TrustedConnection
        };
    }

    private static string BuildDeleteWorldPrompt(WorldEntry entry)
    {
        return
            $"Remove world entry '{entry.Name}'?\n\n" +
            $"DB Server/IP: {WorldEntry.NormalizeDatabaseServerAddress(entry.Address)}\n" +
            $"DB Name: {entry.DbName}\n\n" +
            "Yes = remove the entry and delete the world database.\n" +
            "No = remove the entry only.\n" +
            "Cancel = keep everything unchanged.";
    }

    private static string BuildDeleteWorldDatabasesPrompt(IReadOnlyList<WorldEntry> worlds)
    {
        if (worlds.Count == 0)
        {
            return
                "Delete all configured world databases too?\n\n" +
                "No configured world DB entries were found.\n\n" +
                "Yes = continue reinstall with no world DB deletion needed.\n" +
                "No = continue reinstall and keep current world DB state.\n" +
                "Cancel = abort reinstall.";
        }

        string worldList = string.Join(
            Environment.NewLine,
            worlds.Select(world =>
                $"- {world.Name} ({WorldEntry.NormalizeDatabaseServerAddress(world.Address)} / {world.DbName})"));

        return
            "Delete all configured world databases too?\n\n" +
            $"{worldList}\n\n" +
            "Yes = delete all listed world databases before reinstall.\n" +
            "No = keep the world databases and only reset AccountServer setup.\n" +
            "Cancel = abort reinstall.";
    }

    private static List<WorldEntry> GetDistinctWorldDatabases(IEnumerable<WorldEntry> worlds)
    {
        var result = new List<WorldEntry>();
        var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        foreach (var world in worlds)
        {
            if (string.IsNullOrWhiteSpace(world.DbName))
            {
                continue;
            }

            string address = WorldEntry.NormalizeDatabaseServerAddress(world.Address);
            string key = $"{address}\n{world.DbName.Trim()}";
            if (!seen.Add(key))
            {
                continue;
            }

            result.Add(CloneWorldEntry(world));
        }

        return result;
    }

    private static string BuildSqlServerConnectionString(DbProfile profile, string database)
    {
        string host = DbProfile.BuildSqlServerDataSource(profile.Host, profile.Port, profile.TrustedConnection);
        if (profile.TrustedConnection)
        {
            return $"Server={host};Database={database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }

        return $"Server={host};Database={database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
    }

    private static string BuildMariaDbConnectionString(DbProfile profile, string database)
    {
        DbProfile.ResolveMariaDbEndpoint(profile.Host, profile.Port, out string host, out int port);
        return $"Server={host};Port={port};Database={database};User ID={profile.User};Password={profile.Password};";
    }

    private static string EscapeMariaDbIdentifier(string identifier)
    {
        return identifier.Replace("`", "``", StringComparison.Ordinal);
    }
}

internal sealed class DatabaseSnapshot
{
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; }
    public string Database { get; set; } = string.Empty;
    public string User { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
    public bool Trusted { get; set; }

    public static DatabaseSnapshot From(DbProfile profile)
    {
        return new DatabaseSnapshot
        {
            Host = profile.Host,
            Port = profile.Port,
            Database = profile.Database,
            User = profile.User,
            Password = profile.Password,
            Trusted = profile.TrustedConnection
        };
    }

    public void ApplyTo(DbProfile profile)
    {
        profile.Host = Host;
        profile.Port = Port;
        profile.Database = Database;
        profile.User = User;
        profile.Password = Password;
        profile.TrustedConnection = Trusted;
    }
}
