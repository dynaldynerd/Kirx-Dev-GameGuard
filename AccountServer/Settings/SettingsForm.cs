using System;
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
    private readonly BindingList<string> _gmPrefixes;
    private readonly CheckBox _chkAutostart = new();

    public SettingsForm()
        : this(new AppSettings(), true)
    {
    }

    public SettingsForm(AppSettings settings)
        : this(settings, false)
    {
    }

    private SettingsForm(AppSettings settings, bool designTime)
    {
        _settings = settings;
        _userDb = DatabaseSnapshot.From(settings.Database.User);
        _worlds = new BindingList<WorldEntry>(settings.WorldList.Worlds.Select(CloneWorldEntry).ToList());
        _gmPrefixes = new BindingList<string>(settings.GmFilter.Prefixes.ToList());
        InitializeComponent();
        InitializeAutostartControl();
        lstGmPrefixes.DataSource = _gmPrefixes;
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
        var lblAutostart = new Label
        {
            AutoSize = true,
            Text = "Autostart",
            Anchor = AnchorStyles.Left
        };

        _chkAutostart.AutoSize = true;
        _chkAutostart.Text = "Start server automatically on launch";
        _chkAutostart.Anchor = AnchorStyles.Left;

        int row = tblGeneral.RowCount;
        tblGeneral.RowCount = row + 1;
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.Controls.Add(lblAutostart, 0, row);
        tblGeneral.Controls.Add(_chkAutostart, 1, row);
    }

    private void LoadDbProfileToControls()
    {
        var db = _userDb;
        txtDbHost.Text = GetEffectiveDbHost(db.Host, db.Trusted);
        txtDbPort.Text = db.Port.ToString(CultureInfo.InvariantCulture);
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
        if (!int.TryParse(txtDbPort.Text, out var port) || port < 1 || port > 65535)
        {
            MessageBox.Show(this, "DB port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }

        _userDb.Host = GetEffectiveDbHost(txtDbHost.Text.Trim(), radAuthTrusted.Checked);
        _userDb.Port = port;
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

        _userDb.ApplyTo(_settings.Database.User);
        _settings.GmFilter.Prefixes = _gmPrefixes.Where(p => !string.IsNullOrWhiteSpace(p)).ToList();
        _settings.WorldList.Worlds = _worlds.Select(CloneWorldEntry).ToList();
        _settings.MaxActiveClients = (int)numMaxActive.Value;
        _settings.Autostart = _chkAutostart.Checked;
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
        txtDbPort.Enabled = !isSqlite;
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

    private void OnGmAdd(object? sender, EventArgs e)
    {
        var prefix = txtGmPrefix.Text.Trim();
        if (string.IsNullOrEmpty(prefix))
        {
            return;
        }
        if (_gmPrefixes.Contains(prefix))
        {
            return;
        }
        _gmPrefixes.Add(prefix);
        txtGmPrefix.Clear();
    }

    private void OnGmRemove(object? sender, EventArgs e)
    {
        if (lstGmPrefixes.SelectedItem is string value)
        {
            _gmPrefixes.Remove(value);
        }
    }

    private void OnWorldAdd(object? sender, EventArgs e)
    {
        var entry = new WorldEntry();
        using var form = new WorldEntryForm(entry);
        if (form.ShowDialog(this) != DialogResult.OK)
        {
            return;
        }
        _worlds.Add(entry);
    }

    private void OnWorldGridClick(object? sender, DataGridViewCellEventArgs e)
    {
        if (e.RowIndex < 0 || e.ColumnIndex < 0)
        {
            return;
        }

        var entry = _worlds[e.RowIndex];
        var column = gridWorlds.Columns[e.ColumnIndex];
        if (column.Name == "colEdit")
        {
            using var form = new WorldEntryForm(entry);
            if (form.ShowDialog(this) == DialogResult.OK)
            {
                _worlds.ResetItem(e.RowIndex);
            }
            return;
        }
        if (column.Name == "colDelete")
        {
            var confirm = MessageBox.Show(this, "Remove selected world?", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (confirm == DialogResult.Yes)
            {
                _worlds.RemoveAt(e.RowIndex);
            }
        }
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
            "Delete Database Too?",
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
        return _settings.Database.Provider switch
        {
            DatabaseProvider.Sqlite => DeleteConfiguredSqliteDatabaseAsync(),
            DatabaseProvider.MariaDb => DeleteConfiguredMariaDbDatabaseAsync(),
            _ => DeleteConfiguredSqlServerDatabaseAsync()
        };
    }

    private async Task DeleteConfiguredSqlServerDatabaseAsync()
    {
        string connString = BuildSqlServerConnectionString(_settings.Database.User, "master");
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
        cmd.Parameters.AddWithValue("@name", _settings.Database.User.Database);
        await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
    }

    private async Task DeleteConfiguredMariaDbDatabaseAsync()
    {
        string connString = BuildMariaDbConnectionString(_settings.Database.User, "information_schema");
        await using var conn = new MySqlConnection(connString);
        await conn.OpenAsync().ConfigureAwait(true);

        string escapedName = EscapeMariaDbIdentifier(_settings.Database.User.Database);
        string dropSql = $"DROP DATABASE IF EXISTS `{escapedName}`;";
        await using var cmd = new MySqlCommand(dropSql, conn);
        await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
    }

    private Task DeleteConfiguredSqliteDatabaseAsync()
    {
        string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _settings.Database.User.Database);
        if (File.Exists(dbPath))
        {
            File.Delete(dbPath);
        }

        return Task.CompletedTask;
    }

    private static string BuildSqlServerConnectionString(DbProfile profile, string database)
    {
        string host = profile.GetEffectiveSqlServerHost();
        if (profile.TrustedConnection)
        {
            return $"Server={host},{profile.Port};Database={database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }

        return $"Server={host},{profile.Port};Database={database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
    }

    private static string BuildMariaDbConnectionString(DbProfile profile, string database)
    {
        return $"Server={profile.Host};Port={profile.Port};Database={database};User ID={profile.User};Password={profile.Password};";
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
