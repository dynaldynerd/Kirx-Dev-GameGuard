using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
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
            HideDatabasePortControl();
            EnsureSetupSalt();
            LoadFromSettings();
            WireDatabaseChangeTracking();
            MarkDatabaseStepPending(updateUi: false);
            UpdateDbUiForProvider();
            UpdateWizardUi();
        }

        private void HideDatabasePortControl()
        {
            lblUserPort.Visible = false;
            numUserPort.Visible = false;
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

            txtUserHost.Text = DbProfile.GetDisplayHost(
                _settings.Database.Provider,
                _settings.Database.User.Host,
                _settings.Database.User.Port,
                _settings.Database.User.TrustedConnection);
            txtUserDb.Text = _settings.Database.User.Database;
            txtUserUser.Text = _settings.Database.User.User;
            txtUserPass.Text = _settings.Database.User.Password;
            chkUserTrusted.Checked = _settings.Database.User.TrustedConnection;

            var world = _settings.WorldList.Worlds.Count > 0
                ? _settings.WorldList.Worlds[0]
                : new WorldEntry();

            txtWorldName.Text = world.Name;
            txtWorldAddress.Text = WorldEntry.NormalizeDatabaseServerAddress(world.Address);
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

            SetProfileEnabled(isSqlite, isSqlServer, chkUserTrusted, txtUserHost, txtUserUser, txtUserPass);
        }

        private static void SetProfileEnabled(
            bool isSqlite,
            bool isSqlServer,
            CheckBox trusted,
            TextBox host,
            TextBox user,
            TextBox pass)
        {
            if (isSqlite)
            {
                trusted.Enabled = false;
                host.Enabled = false;
                user.Enabled = false;
                pass.Enabled = false;
                return;
            }

            trusted.Enabled = isSqlServer;
            bool useTrusted = isSqlServer && trusted.Checked;
            if (useTrusted && !string.Equals(host.Text, DbProfile.TrustedSqlServerHost, StringComparison.Ordinal))
            {
                host.Text = DbProfile.TrustedSqlServerHost;
            }
            host.Enabled = !useTrusted;
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
                    lblStepDescription.Text = "Define the first world entry that LoginServer clients will see. The address here is the RF_World database server/IP. Installing RF_World from this step is optional and can apply a country-specific SQL collation.";
                    break;
                default:
                    lblStepTitle.Text = "Security Seed";
                    lblStepDescription.Text = "This Argon2 salt also seeds account ID protection. Paste the original salt here if you are restoring an existing RF_User backup, and keep it stable after setup.";
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
                "Generate a different Argon2 salt for this setup?\n\n" +
                "Do not do this if you are restoring an existing RF_User backup, because the original salt is required for account ID lookup and password verification.";
            if (MessageBox.Show(this, prompt, "Regenerate Salt", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes)
            {
                return;
            }

            txtArgon2Salt.Text = SecuritySettings.CreateArgon2SaltBase64();
        }

        private void BtnInstallWorldDb_Click(object? sender, EventArgs e)
        {
            string worldDatabaseName = txtWorldDb.Text.Trim();
            string worldDatabaseAddress = txtWorldAddress.Text.Trim();
            if (string.IsNullOrWhiteSpace(worldDatabaseName))
            {
                MessageBox.Show(this, "Enter the world DB name first.", "World Registration", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                txtWorldDb.Focus();
                return;
            }

            if (string.IsNullOrWhiteSpace(worldDatabaseAddress))
            {
                worldDatabaseAddress = DbProfile.TrustedSqlServerHost;
            }

            bool useTrustedConnection = string.Equals(
                worldDatabaseAddress,
                DbProfile.TrustedSqlServerHost,
                StringComparison.OrdinalIgnoreCase);

            using var dialog = new WorldDatabaseInstallForm(worldDatabaseName, worldDatabaseAddress, useTrustedConnection);
            dialog.ShowDialog(this);
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
                message = "World name, database server address/IP, and DB name are required.";
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
            DatabaseProvider provider = GetSelectedProvider();
            _settings.Database.Provider = provider;

            _settings.Database.User.Host = DbProfile.NormalizeHostForStorage(
                provider,
                txtUserHost.Text.Trim(),
                provider == DatabaseProvider.SqlServer && chkUserTrusted.Checked);
            _settings.Database.User.Port = DbProfile.GetEffectivePort(
                provider,
                _settings.Database.User.Host,
                provider == DatabaseProvider.MariaDb ? DbProfile.DefaultMariaDbPort : DbProfile.DefaultSqlServerPort);
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
                    Address = WorldEntry.NormalizeDatabaseServerAddress(txtWorldAddress.Text),
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

            InstallProgressForm? progress = null;
            try
            {
                string script = await LoadUserDatabaseScriptAsync(provider).ConfigureAwait(true);
                script = RewriteInstallScriptDatabaseName(script);

                IReadOnlyList<string> commands = provider == DatabaseProvider.SqlServer
                    ? EmbeddedSqlScripts.SplitSqlServerBatches(script)
                    : EmbeddedSqlScripts.SplitDelimitedStatements(script);

                int totalSteps = commands.Count + (force ? 1 : 0);
                progress = new InstallProgressForm("Install RF_User DB");
                progress.Show(this);
                progress.SetMarquee("Preparing RF_User install...");

                int completedSteps = 0;
                if (force)
                {
                    progress.SetProgress("Removing existing RF_User database...", completedSteps, totalSteps);
                    await DeleteExistingUserDatabaseAsync(provider).ConfigureAwait(true);
                    completedSteps++;
                }

                if (provider == DatabaseProvider.Sqlite)
                {
                    return await InstallSqliteAsync(progress, commands, completedSteps, totalSteps).ConfigureAwait(true);
                }
                if (provider == DatabaseProvider.MariaDb)
                {
                    return await InstallMariaDbAsync(progress, commands, completedSteps, totalSteps).ConfigureAwait(true);
                }
                return await InstallSqlServerAsync(progress, commands, completedSteps, totalSteps).ConfigureAwait(true);
            }
            catch (Exception ex)
            {
                if (progress != null && !progress.IsDisposed)
                {
                    progress.Close();
                }
                MessageBox.Show(this, $"Install failed: {ex.Message}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            finally
            {
                if (progress != null && !progress.IsDisposed)
                {
                    progress.Close();
                }
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

        private static Task<string> LoadUserDatabaseScriptAsync(DatabaseProvider provider)
        {
            return provider switch
            {
                DatabaseProvider.Sqlite => EmbeddedSqlScripts.LoadTextAsync("AccountServer.Schemas.RF_User_sqlite.sql"),
                DatabaseProvider.MariaDb => EmbeddedSqlScripts.LoadTextAsync("AccountServer.Schemas.RF_User_mariadb.sql"),
                _ => EmbeddedSqlScripts.LoadTextAsync("AccountServer.Schemas.RF_User_mssql.sql")
            };
        }

        private static string DescribeInstallUnit(DatabaseProvider provider)
        {
            return provider == DatabaseProvider.SqlServer ? "batch" : "statement";
        }

        private async Task<bool> InstallSqlServerAsync(InstallProgressForm progress, IReadOnlyList<string> commands, int completedSteps, int totalSteps)
        {
            string connString = BuildSqlServerConnectionString(_settings.Database.User, "master");
            await using var conn = new SqlConnection(connString);
            progress.SetProgress("Connecting to SQL Server...", completedSteps, totalSteps);
            await conn.OpenAsync().ConfigureAwait(true);
            for (int i = 0; i < commands.Count; i++)
            {
                progress.SetProgress(
                    $"Executing RF_User {DescribeInstallUnit(DatabaseProvider.SqlServer)} {i + 1} of {commands.Count}...",
                    completedSteps,
                    totalSteps);
                await using var cmd = new SqlCommand(commands[i], conn);
                await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
                completedSteps++;
            }
            return true;
        }

        private async Task<bool> InstallMariaDbAsync(InstallProgressForm progress, IReadOnlyList<string> commands, int completedSteps, int totalSteps)
        {
            string connString = BuildMariaDbConnectionString(_settings.Database.User, "information_schema");
            await using var conn = new MySqlConnection(connString);
            progress.SetProgress("Connecting to MariaDB...", completedSteps, totalSteps);
            await conn.OpenAsync().ConfigureAwait(true);
            for (int i = 0; i < commands.Count; i++)
            {
                progress.SetProgress(
                    $"Executing RF_User {DescribeInstallUnit(DatabaseProvider.MariaDb)} {i + 1} of {commands.Count}...",
                    completedSteps,
                    totalSteps);
                await using var cmd = new MySqlCommand(commands[i], conn);
                await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
                completedSteps++;
            }
            return true;
        }

        private async Task<bool> InstallSqliteAsync(InstallProgressForm progress, IReadOnlyList<string> commands, int completedSteps, int totalSteps)
        {
            string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _settings.Database.User.Database);
            Directory.CreateDirectory(Path.GetDirectoryName(dbPath) ?? AppContext.BaseDirectory);
            string connString = $"Data Source={dbPath};";
            await using var conn = new SqliteConnection(connString);
            progress.SetProgress("Creating SQLite RF_User database...", completedSteps, totalSteps);
            await conn.OpenAsync().ConfigureAwait(true);
            for (int i = 0; i < commands.Count; i++)
            {
                progress.SetProgress(
                    $"Executing RF_User {DescribeInstallUnit(DatabaseProvider.Sqlite)} {i + 1} of {commands.Count}...",
                    completedSteps,
                    totalSteps);
                await using var cmd = new SqliteCommand(commands[i], conn);
                await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
                completedSteps++;
            }
            return true;
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

        private string RewriteInstallScriptDatabaseName(string script)
        {
            const string defaultDatabaseName = "RF_User";
            string targetDatabaseName = _settings.Database.User.Database.Trim();
            if (string.IsNullOrWhiteSpace(targetDatabaseName) ||
                string.Equals(targetDatabaseName, defaultDatabaseName, StringComparison.OrdinalIgnoreCase))
            {
                return script;
            }

            string pattern = $@"(?<![A-Za-z0-9_]){Regex.Escape(defaultDatabaseName)}(?![A-Za-z0-9_])";
            return Regex.Replace(script, pattern, targetDatabaseName, RegexOptions.IgnoreCase | RegexOptions.CultureInvariant);
        }

        private static string EscapeMariaDbIdentifier(string identifier)
        {
            return identifier.Replace("`", "``", StringComparison.Ordinal);
        }

    }
}
