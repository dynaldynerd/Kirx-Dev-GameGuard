using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AccountServer.Security;
using AccountServer.Settings;
using Microsoft.Data.SqlClient;
using Microsoft.Data.Sqlite;
using MySqlConnector;

namespace AccountServer
{
    public partial class Setup : Form
    {
        private const int DatabasePageIndex = 0;
        private const int WorldPageIndex = 1;
        private const int SecurityPageIndex = 2;

        private readonly AppSettings _settings;
        private bool _databaseStepReady;

        public Setup()
        {
            InitializeComponent();
            _settings = AppSettings.Load();
            EnsureSetupSalt();
            LoadFromSettings();
            WireDatabaseChangeTracking();
            MarkDatabaseStepPending(updateUi: false);
            UpdateDbUiForProvider();
            UpdateWizardUi();
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            UpdateWizardUi();
        }

        private void EnsureSetupSalt()
        {
            _settings.Security ??= new SecuritySettings();
            if (!_settings.Security.HasValidArgon2Salt())
            {
                _settings.Security.Argon2SaltBase64 = SecuritySettings.CreateArgon2SaltBase64();
            }
        }

        private void LoadFromSettings()
        {
            cmbProvider.SelectedIndex = _settings.Database.Provider switch
            {
                DatabaseProvider.MariaDb => 1,
                DatabaseProvider.Sqlite => 2,
                _ => 0
            };

            txtUserHost.Text = _settings.Database.User.Host;
            numUserPort.Value = ClampPort(_settings.Database.User.Port);
            txtUserDb.Text = _settings.Database.User.Database;
            txtUserUser.Text = _settings.Database.User.User;
            txtUserPass.Text = _settings.Database.User.Password;
            chkUserTrusted.Checked = _settings.Database.User.TrustedConnection;

            var world = _settings.WorldList.Worlds.Count > 0
                ? _settings.WorldList.Worlds[0]
                : new WorldEntry();

            txtWorldName.Text = world.Name;
            txtWorldAddress.Text = world.Address;
            txtWorldDb.Text = world.DbName;
            cmbWorldType.SelectedIndex = GetWorldTypeIndex(world.Type);

            txtArgon2Salt.Text = _settings.Security.Argon2SaltBase64;
        }

        private static decimal ClampPort(int port)
        {
            if (port < 1) return 1;
            if (port > 65535) return 65535;
            return port;
        }

        private DatabaseProvider GetSelectedProvider()
        {
            return cmbProvider.SelectedIndex switch
            {
                1 => DatabaseProvider.MariaDb,
                2 => DatabaseProvider.Sqlite,
                _ => DatabaseProvider.SqlServer
            };
        }

        private void UpdateDbUiForProvider()
        {
            var provider = GetSelectedProvider();
            bool isSqlite = provider == DatabaseProvider.Sqlite;
            bool isSqlServer = provider == DatabaseProvider.SqlServer;

            SetProfileEnabled(isSqlite, isSqlServer, chkUserTrusted, txtUserHost, numUserPort, txtUserUser, txtUserPass);
        }

        private static void SetProfileEnabled(
            bool isSqlite,
            bool isSqlServer,
            CheckBox trusted,
            TextBox host,
            NumericUpDown port,
            TextBox user,
            TextBox pass)
        {
            if (isSqlite)
            {
                trusted.Enabled = false;
                host.Enabled = false;
                port.Enabled = false;
                user.Enabled = false;
                pass.Enabled = false;
                return;
            }

            trusted.Enabled = isSqlServer;
            host.Enabled = true;
            port.Enabled = true;
            bool useTrusted = isSqlServer && trusted.Checked;
            user.Enabled = !useTrusted;
            pass.Enabled = !useTrusted;
        }

        private void ProviderChanged(object? sender, EventArgs e)
        {
            MarkDatabaseStepPending(updateUi: pageSetup.SelectedIndex == DatabasePageIndex);
            UpdateDbUiForProvider();
        }

        private void TrustedChanged(object? sender, EventArgs e)
        {
            MarkDatabaseStepPending(updateUi: pageSetup.SelectedIndex == DatabasePageIndex);
            UpdateDbUiForProvider();
        }

        private void WireDatabaseChangeTracking()
        {
            txtUserHost.TextChanged += DatabaseInputChanged;
            numUserPort.ValueChanged += DatabaseInputChanged;
            txtUserDb.TextChanged += DatabaseInputChanged;
            txtUserUser.TextChanged += DatabaseInputChanged;
            txtUserPass.TextChanged += DatabaseInputChanged;
        }

        private void DatabaseInputChanged(object? sender, EventArgs e)
        {
            MarkDatabaseStepPending(updateUi: pageSetup.SelectedIndex == DatabasePageIndex);
        }

        private void MarkDatabaseStepPending(bool updateUi)
        {
            _databaseStepReady = false;
            if (updateUi)
            {
                UpdateWizardUi();
            }
        }

        private void MarkDatabaseStepReady()
        {
            _databaseStepReady = true;
            if (pageSetup.SelectedIndex == DatabasePageIndex)
            {
                UpdateWizardUi();
            }
        }

        private void UpdateWizardUi()
        {
            int currentStep = pageSetup.SelectedIndex + 1;
            lblStepCount.Text = $"Step {currentStep} of {pageSetup.TabCount}";

            switch (pageSetup.SelectedIndex)
            {
                case DatabasePageIndex:
                    lblStepTitle.Text = "Database Setup";
                    lblStepDescription.Text = "Choose the RF_User database provider and connection used by AccountServer.";
                    break;
                case WorldPageIndex:
                    lblStepTitle.Text = "World Registration";
                    lblStepDescription.Text = "Define the first world entry that LoginServer clients will see.";
                    break;
                default:
                    lblStepTitle.Text = "Security Seed";
                    lblStepDescription.Text = "This Argon2 salt also seeds account ID protection. Keep it stable after the first setup.";
                    break;
            }

            bool isDatabasePage = pageSetup.SelectedIndex == DatabasePageIndex;
            btnBack.Enabled = pageSetup.SelectedIndex > DatabasePageIndex;
            btnNext.Visible = pageSetup.SelectedIndex < pageSetup.TabCount - 1;
            btnNext.Enabled = !isDatabasePage || _databaseStepReady;
            btnFinish.Visible = pageSetup.SelectedIndex == pageSetup.TabCount - 1;
            btnFinish.Enabled = btnFinish.Visible;
            AcceptButton = btnFinish.Visible ? btnFinish : (btnNext.Enabled ? btnNext : null);
            panelButtons.PerformLayout();
        }

        private void BtnBack_Click(object? sender, EventArgs e)
        {
            if (pageSetup.SelectedIndex <= DatabasePageIndex)
            {
                return;
            }

            pageSetup.SelectedIndex--;
            UpdateWizardUi();
        }

        private void BtnNext_Click(object? sender, EventArgs e)
        {
            if (!ValidateCurrentPage(out string message))
            {
                MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (pageSetup.SelectedIndex >= pageSetup.TabCount - 1)
            {
                return;
            }

            pageSetup.SelectedIndex++;
            UpdateWizardUi();
        }

        private void BtnSaveSettings_Click(object? sender, EventArgs e)
        {
            if (!ValidateDatabaseInputs(out string message))
            {
                MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            const string savePrompt =
                "Continue with Save Setting?\n\n" +
                "Save Setting only stores the connection details and assumes the RF_User database is already installed.\n\n" +
                "Use Install New DB instead if the database does not exist yet.";
            if (MessageBox.Show(this, savePrompt, "Save Setting", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            {
                return;
            }

            ApplyDatabaseSettings();
            ApplySecuritySettings();
            _settings.Save();
            MarkDatabaseStepReady();
            MessageBox.Show(this, "Database settings saved. You can continue to the next step.", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private async void BtnFinish_Click(object? sender, EventArgs e)
        {
            if (!ValidateAllInputs(out string message))
            {
                MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            ApplyToSettings();
            _settings.Save();

            bool installed = await EnsureUserDatabaseInstalledAsync().ConfigureAwait(true);
            if (!installed)
            {
                return;
            }

            Properties.Settings.Default.IsSetup = true;
            Properties.Settings.Default.Save();
            DialogResult = DialogResult.OK;
            Close();
        }

        private async void BtnInstallUserDb_Click(object? sender, EventArgs e)
        {
            if (!ValidateDatabaseInputs(out string message))
            {
                MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            const string installPrompt =
                "Continue with Install New DB?\n\n" +
                "Install New DB assumes the RF_User database does not exist yet and will create a fresh database from the bundled schema.\n\n" +
                "Use Save Setting instead if the database is already installed.";
            if (MessageBox.Show(this, installPrompt, "Install New DB", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            {
                return;
            }

            ApplyDatabaseSettings();
            ApplySecuritySettings();
            _settings.Save();

            bool ok = await InstallUserDatabaseAsync(force: true).ConfigureAwait(true);
            if (ok)
            {
                MarkDatabaseStepReady();
                MessageBox.Show(this, "User database installed. You can continue to the next step.", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void BtnRegenerateSalt_Click(object? sender, EventArgs e)
        {
            const string prompt =
                "Generate a different Argon2 salt for this fresh setup?\n\n" +
                "After accounts exist, changing it would break account ID lookup and password verification.";
            if (MessageBox.Show(this, prompt, "Regenerate Salt", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes)
            {
                return;
            }

            txtArgon2Salt.Text = SecuritySettings.CreateArgon2SaltBase64();
        }

        private bool ValidateCurrentPage(out string message)
        {
            return pageSetup.SelectedIndex switch
            {
                DatabasePageIndex => ValidateDatabaseInputs(out message),
                WorldPageIndex => ValidateWorldInputs(out message),
                _ => ValidateSecurityInputs(out message)
            };
        }

        private bool ValidateAllInputs(out string message)
        {
            if (!ValidateDatabaseInputs(out message))
            {
                pageSetup.SelectedIndex = DatabasePageIndex;
                UpdateWizardUi();
                return false;
            }

            if (!ValidateWorldInputs(out message))
            {
                pageSetup.SelectedIndex = WorldPageIndex;
                UpdateWizardUi();
                return false;
            }

            if (!ValidateSecurityInputs(out message))
            {
                pageSetup.SelectedIndex = SecurityPageIndex;
                UpdateWizardUi();
                return false;
            }

            message = string.Empty;
            return true;
        }

        private bool ValidateDatabaseInputs(out string message)
        {
            var provider = GetSelectedProvider();
            if (string.IsNullOrWhiteSpace(txtUserDb.Text))
            {
                message = "User database name is required.";
                return false;
            }
            if (provider != DatabaseProvider.Sqlite)
            {
                if (string.IsNullOrWhiteSpace(txtUserHost.Text))
                {
                    message = "Database host is required.";
                    return false;
                }
                if (numUserPort.Value < 1 || numUserPort.Value > 65535)
                {
                    message = "Database port must be 1-65535.";
                    return false;
                }
                if (provider == DatabaseProvider.MariaDb)
                {
                    if (string.IsNullOrWhiteSpace(txtUserUser.Text))
                    {
                        message = "MariaDB requires a username.";
                        return false;
                    }
                }
            }

            message = string.Empty;
            return true;
        }

        private bool ValidateWorldInputs(out string message)
        {
            if (string.IsNullOrWhiteSpace(txtWorldName.Text) ||
                string.IsNullOrWhiteSpace(txtWorldAddress.Text) ||
                string.IsNullOrWhiteSpace(txtWorldDb.Text))
            {
                message = "World entry fields are required.";
                return false;
            }

            message = string.Empty;
            return true;
        }

        private bool ValidateSecurityInputs(out string message)
        {
            string saltText = txtArgon2Salt.Text.Trim();
            if (!SecuritySettings.TryDecodeArgon2Salt(saltText, out var saltBytes) || saltBytes.Length < 16)
            {
                message = "Argon2 salt must be valid Base64 and at least 16 bytes.";
                return false;
            }

            message = string.Empty;
            return true;
        }

        private void ApplyToSettings()
        {
            ApplyDatabaseSettings();
            ApplyWorldSettings();
            ApplySecuritySettings();
        }

        private void ApplyDatabaseSettings()
        {
            _settings.Database.Provider = GetSelectedProvider();

            _settings.Database.User.Host = txtUserHost.Text.Trim();
            _settings.Database.User.Port = (int)numUserPort.Value;
            _settings.Database.User.Database = txtUserDb.Text.Trim();
            _settings.Database.User.User = txtUserUser.Text.Trim();
            _settings.Database.User.Password = txtUserPass.Text;
            _settings.Database.User.TrustedConnection = chkUserTrusted.Checked;
        }

        private void ApplyWorldSettings()
        {
            _settings.WorldList.Worlds = new List<WorldEntry>
            {
                new()
                {
                    Name = txtWorldName.Text.Trim(),
                    Address = txtWorldAddress.Text.Trim(),
                    DbName = txtWorldDb.Text.Trim(),
                    Type = GetSelectedWorldType()
                }
            };
        }

        private void ApplySecuritySettings()
        {
            _settings.Security.Argon2SaltBase64 = txtArgon2Salt.Text.Trim();
        }

        private static int GetWorldTypeIndex(int worldType)
        {
            return worldType == 1 ? 1 : 0;
        }

        private int GetSelectedWorldType()
        {
            return cmbWorldType.SelectedIndex == 1 ? 1 : 0;
        }

        private async Task<bool> EnsureUserDatabaseInstalledAsync()
        {
            bool exists = await UserDatabaseExistsAsync().ConfigureAwait(true);
            if (exists)
            {
                return true;
            }
            return await InstallUserDatabaseAsync(force: false).ConfigureAwait(true);
        }

        private async Task<bool> UserDatabaseExistsAsync()
        {
            var provider = _settings.Database.Provider;
            if (provider == DatabaseProvider.Sqlite)
            {
                string path = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _settings.Database.User.Database);
                return File.Exists(path);
            }

            if (provider == DatabaseProvider.MariaDb)
            {
                string connString = BuildMariaDbConnectionString(_settings.Database.User, "information_schema");
                try
                {
                    await using var conn = new MySqlConnection(connString);
                    await conn.OpenAsync().ConfigureAwait(true);
                    await using var cmd = new MySqlCommand(
                        "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = @name",
                        conn);
                    cmd.Parameters.AddWithValue("@name", _settings.Database.User.Database);
                    object? result = await cmd.ExecuteScalarAsync().ConfigureAwait(true);
                    return result != null;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(this, $"MariaDB check failed: {ex.Message}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
            }

            string masterConn = BuildSqlServerConnectionString(_settings.Database.User, "master");
            try
            {
                await using var conn = new SqlConnection(masterConn);
                await conn.OpenAsync().ConfigureAwait(true);
                await using var cmd = new SqlCommand("SELECT DB_ID(@name)", conn);
                cmd.Parameters.AddWithValue("@name", _settings.Database.User.Database);
                object? result = await cmd.ExecuteScalarAsync().ConfigureAwait(true);
                return result != null && result != DBNull.Value;
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, $"SQL Server check failed: {ex.Message}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
        }

        private async Task<bool> InstallUserDatabaseAsync(bool force)
        {
            var provider = _settings.Database.Provider;
            if (!force)
            {
                bool exists = await UserDatabaseExistsAsync().ConfigureAwait(true);
                if (exists)
                {
                    return true;
                }
            }

            try
            {
                if (force)
                {
                    await DeleteExistingUserDatabaseAsync(provider).ConfigureAwait(true);
                }

                if (provider == DatabaseProvider.Sqlite)
                {
                    return await InstallSqliteAsync().ConfigureAwait(true);
                }
                if (provider == DatabaseProvider.MariaDb)
                {
                    return await InstallMariaDbAsync().ConfigureAwait(true);
                }
                return await InstallSqlServerAsync().ConfigureAwait(true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, $"Install failed: {ex.Message}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
        }

        private async Task DeleteExistingUserDatabaseAsync(DatabaseProvider provider)
        {
            switch (provider)
            {
                case DatabaseProvider.Sqlite:
                    DeleteExistingSqliteDatabase();
                    return;
                case DatabaseProvider.MariaDb:
                    await DeleteExistingMariaDbDatabaseAsync().ConfigureAwait(true);
                    return;
                default:
                    await DeleteExistingSqlServerDatabaseAsync().ConfigureAwait(true);
                    return;
            }
        }

        private async Task DeleteExistingSqlServerDatabaseAsync()
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

        private async Task DeleteExistingMariaDbDatabaseAsync()
        {
            string connString = BuildMariaDbConnectionString(_settings.Database.User, "information_schema");
            await using var conn = new MySqlConnection(connString);
            await conn.OpenAsync().ConfigureAwait(true);

            string escapedName = EscapeMariaDbIdentifier(_settings.Database.User.Database);
            string dropSql = $"DROP DATABASE IF EXISTS `{escapedName}`;";
            await using var cmd = new MySqlCommand(dropSql, conn);
            await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
        }

        private void DeleteExistingSqliteDatabase()
        {
            string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _settings.Database.User.Database);
            if (!File.Exists(dbPath))
            {
                return;
            }

            File.Delete(dbPath);
        }

        private async Task<bool> InstallSqlServerAsync()
        {
            string scriptPath = ResolveScriptPath("RF_User_mssql.sql");
            if (!File.Exists(scriptPath))
            {
                MessageBox.Show(this, $"Missing script: {scriptPath}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            string script = await File.ReadAllTextAsync(scriptPath).ConfigureAwait(true);
            script = RewriteInstallScriptDatabaseName(script);
            string connString = BuildSqlServerConnectionString(_settings.Database.User, "master");
            await using var conn = new SqlConnection(connString);
            await conn.OpenAsync().ConfigureAwait(true);
            foreach (string batch in SplitSqlServerBatches(script))
            {
                if (string.IsNullOrWhiteSpace(batch))
                {
                    continue;
                }
                await using var cmd = new SqlCommand(batch, conn);
                await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
            }
            return true;
        }

        private async Task<bool> InstallMariaDbAsync()
        {
            string scriptPath = ResolveScriptPath("RF_User_mariadb.sql");
            if (!File.Exists(scriptPath))
            {
                MessageBox.Show(this, $"Missing script: {scriptPath}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            string script = await File.ReadAllTextAsync(scriptPath).ConfigureAwait(true);
            script = RewriteInstallScriptDatabaseName(script);
            string connString = BuildMariaDbConnectionString(_settings.Database.User, "information_schema");
            await using var conn = new MySqlConnection(connString);
            await conn.OpenAsync().ConfigureAwait(true);
            foreach (string stmt in SplitStatements(script))
            {
                if (string.IsNullOrWhiteSpace(stmt))
                {
                    continue;
                }
                await using var cmd = new MySqlCommand(stmt, conn);
                await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
            }
            return true;
        }

        private async Task<bool> InstallSqliteAsync()
        {
            string scriptPath = ResolveScriptPath("RF_User_sqlite.sql");
            if (!File.Exists(scriptPath))
            {
                MessageBox.Show(this, $"Missing script: {scriptPath}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _settings.Database.User.Database);
            Directory.CreateDirectory(Path.GetDirectoryName(dbPath) ?? AppContext.BaseDirectory);

            string script = await File.ReadAllTextAsync(scriptPath).ConfigureAwait(true);
            string connString = $"Data Source={dbPath};";
            await using var conn = new SqliteConnection(connString);
            await conn.OpenAsync().ConfigureAwait(true);
            foreach (string stmt in SplitStatements(script))
            {
                if (string.IsNullOrWhiteSpace(stmt))
                {
                    continue;
                }
                await using var cmd = new SqliteCommand(stmt, conn);
                await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
            }
            return true;
        }

        private static string BuildSqlServerConnectionString(DbProfile profile, string database)
        {
            if (profile.TrustedConnection)
            {
                return $"Server={profile.Host},{profile.Port};Database={database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
            }
            return $"Server={profile.Host},{profile.Port};Database={database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
        }

        private static string BuildMariaDbConnectionString(DbProfile profile, string database)
        {
            return $"Server={profile.Host};Port={profile.Port};Database={database};User ID={profile.User};Password={profile.Password};";
        }

        private string RewriteInstallScriptDatabaseName(string script)
        {
            const string defaultDatabaseName = "RF_User";
            string targetDatabaseName = _settings.Database.User.Database.Trim();
            if (string.IsNullOrWhiteSpace(targetDatabaseName) ||
                string.Equals(targetDatabaseName, defaultDatabaseName, StringComparison.Ordinal))
            {
                return script;
            }

            return script.Replace(defaultDatabaseName, targetDatabaseName, StringComparison.Ordinal);
        }

        private static string EscapeMariaDbIdentifier(string identifier)
        {
            return identifier.Replace("`", "``", StringComparison.Ordinal);
        }

        private static string ResolveScriptPath(string fileName)
        {
            string baseDir = AppContext.BaseDirectory;
            string schemePath = Path.Combine(baseDir, "scheme", fileName);
            if (File.Exists(schemePath))
            {
                return schemePath;
            }
            return Path.Combine(baseDir, fileName);
        }

        private static IEnumerable<string> SplitSqlServerBatches(string script)
        {
            var sb = new StringBuilder();
            using var reader = new StringReader(script);
            string? line;
            while ((line = reader.ReadLine()) != null)
            {
                if (line.Trim().Equals("GO", StringComparison.OrdinalIgnoreCase))
                {
                    yield return sb.ToString();
                    sb.Clear();
                    continue;
                }
                sb.AppendLine(line);
            }
            if (sb.Length > 0)
            {
                yield return sb.ToString();
            }
        }

        private static IEnumerable<string> SplitStatements(string script)
        {
            var sb = new StringBuilder();
            bool inString = false;
            for (int i = 0; i < script.Length; i++)
            {
                char c = script[i];
                if (c == '\'' && (i == 0 || script[i - 1] != '\\'))
                {
                    inString = !inString;
                }
                if (c == ';' && !inString)
                {
                    yield return sb.ToString();
                    sb.Clear();
                    continue;
                }
                sb.Append(c);
            }
            if (sb.Length > 0)
            {
                yield return sb.ToString();
            }
        }
    }
}
