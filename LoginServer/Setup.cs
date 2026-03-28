using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using LoginServer.Data;
using LoginServer.Settings;
using Microsoft.Data.SqlClient;
using Microsoft.Data.Sqlite;
using MySqlConnector;

namespace LoginServer;

public partial class Setup : Form
{
    private const int BillingPageIndex = 0;
    private readonly AppSettings _settings;
    private readonly DatabaseSnapshot _userDb;
    private readonly DatabaseSnapshot _billingDb;
    private bool _billingStepReady;
    private bool _loadingControls;

    public Setup()
    {
        InitializeComponent();
        _settings = AppSettings.Load();
        _userDb = new DatabaseSnapshot
        {
            Provider = _settings.Database.Provider,
            Host = _settings.Database.Host,
            Port = _settings.Database.Port,
            User = _settings.Database.User,
            Password = _settings.Database.Password,
            Trusted = _settings.Database.TrustedConnection
        };
        _billingDb = new DatabaseSnapshot
        {
            Provider = _settings.Database.BillingProvider,
            Host = _settings.Database.BillingHost,
            Port = _settings.Database.BillingPort,
            Database = _settings.Database.BillingDatabase,
            User = _settings.Database.BillingUser,
            Password = _settings.Database.BillingPassword,
            Trusted = _settings.Database.BillingTrustedConnection
        };

        HideDatabasePortControls();
        LoadFromSettings();
        WireBillingChangeTracking();
        MarkBillingStepPending(updateUi: false);
        UpdateBillingUiForProvider();
        UpdateUserDbUiForProvider();
        UpdateWizardUi();
    }

    private void HideDatabasePortControls()
    {
        lblBillingPort.Visible = false;
        txtBillingPort.Visible = false;
        lblUserDbPort.Visible = false;
        txtUserDbPort.Visible = false;
    }

    protected override void OnShown(EventArgs e)
    {
        base.OnShown(e);
        UpdateWizardUi();
    }

    private void LoadFromSettings()
    {
        _loadingControls = true;

        cboBillingProvider.SelectedIndex = GetProviderIndex(_billingDb.Provider);
        txtBillingHost.Text = DatabaseSettings.GetDisplayHost(
            _billingDb.Provider,
            _billingDb.Host,
            _billingDb.Port,
            _billingDb.Provider == LoginDatabaseProvider.SqlServer && _billingDb.Trusted);
        txtBillingDbName.Text = _billingDb.Database;
        chkBillingTrusted.Checked = _billingDb.Trusted;
        txtBillingUser.Text = _billingDb.User;
        txtBillingPass.Text = _billingDb.Password;

        txtAccountHost.Text = _settings.Network.AccountHost;
        numAccountPort.Value = ClampPort(_settings.Network.AccountPort);
        numClientPort.Value = ClampPort(_settings.Network.ClientPort);

        cboUserDbProvider.SelectedIndex = GetProviderIndex(_userDb.Provider);
        txtUserDbHost.Text = DatabaseSettings.GetDisplayHost(
            _userDb.Provider,
            _userDb.Host,
            _userDb.Port,
            _userDb.Provider == LoginDatabaseProvider.SqlServer && _userDb.Trusted);
        chkUserDbTrusted.Checked = _userDb.Trusted;
        txtUserDbUser.Text = _userDb.User;
        txtUserDbPass.Text = _userDb.Password;

        _loadingControls = false;
    }

    private static decimal ClampPort(int port)
    {
        if (port < 1) return 1;
        if (port > 65535) return 65535;
        return port;
    }

    private void WireBillingChangeTracking()
    {
        txtBillingHost.TextChanged += BillingInputChanged;
        txtBillingDbName.TextChanged += BillingInputChanged;
        txtBillingUser.TextChanged += BillingInputChanged;
        txtBillingPass.TextChanged += BillingInputChanged;
    }

    private void BillingInputChanged(object? sender, EventArgs e)
    {
        if (_loadingControls)
        {
            return;
        }

        MarkBillingStepPending(updateUi: pageSetup.SelectedIndex == BillingPageIndex);
    }

    private void MarkBillingStepPending(bool updateUi)
    {
        _billingStepReady = false;
        if (updateUi)
        {
            UpdateWizardUi();
        }
    }

    private void MarkBillingStepReady()
    {
        _billingStepReady = true;
        if (pageSetup.SelectedIndex == BillingPageIndex)
        {
            UpdateWizardUi();
        }
    }

    private void UpdateWizardUi()
    {
        int currentStep = pageSetup.SelectedIndex + 1;
        lblStepCount.Text = $"Step {currentStep} of {pageSetup.TabCount}";

        if (pageSetup.SelectedIndex == BillingPageIndex)
        {
            lblStepTitle.Text = "Billing Database";
            lblStepDescription.Text =
                "Configure the Billing database only. Save Setting assumes Billing already exists; Install New DB recreates Billing. Next stays locked until one of those actions succeeds.";
        }
        else
        {
            lblStepTitle.Text = "Account Setup";
            lblStepDescription.Text =
                "Configure the AccountServer endpoint and RF_User access. RF_User database name comes from AccountServer, so only provider, host, and auth are configured here.";
        }

        bool isBillingPage = pageSetup.SelectedIndex == BillingPageIndex;
        btnBack.Enabled = pageSetup.SelectedIndex > BillingPageIndex;
        btnNext.Visible = pageSetup.SelectedIndex < pageSetup.TabCount - 1;
        btnNext.Enabled = !isBillingPage || _billingStepReady;
        btnFinish.Visible = pageSetup.SelectedIndex == pageSetup.TabCount - 1;
        btnFinish.Enabled = btnFinish.Visible;
        AcceptButton = btnFinish.Visible ? btnFinish : (btnNext.Enabled ? btnNext : null);
        panelButtons.PerformLayout();
    }

    private void BtnBack_Click(object? sender, EventArgs e)
    {
        if (pageSetup.SelectedIndex <= BillingPageIndex)
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
        if (!ValidateBillingInputs(out string message))
        {
            MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        const string savePrompt =
            "Continue with Save Setting?\n\n" +
            "Save Setting only stores the Billing database connection and assumes Billing is already installed.\n\n" +
            "Use Install New DB instead if the Billing database does not exist yet.";
        if (MessageBox.Show(this, savePrompt, "Save Setting", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
        {
            return;
        }

        ApplyBillingSettings();
        _settings.Save();
        MarkBillingStepReady();
        MessageBox.Show(this, "Billing settings saved. You can continue to the next step.", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Information);
    }

    private async void BtnInstallBillingDb_Click(object? sender, EventArgs e)
    {
        if (!ValidateBillingInputs(out string message))
        {
            MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        const string installPrompt =
            "Continue with Install New DB?\n\n" +
            "Install New DB assumes the Billing database does not exist yet and will recreate it from the bundled schema.\n\n" +
            "Use Save Setting instead if Billing is already installed.";
        if (MessageBox.Show(this, installPrompt, "Install New DB", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
        {
            return;
        }

        ApplyBillingSettings();
        _settings.Save();

        bool ok = await InstallBillingDatabaseAsync(force: true).ConfigureAwait(true);
        if (ok)
        {
            MarkBillingStepReady();
            MessageBox.Show(this, "Billing database installed. You can continue to the next step.", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }

    private void BtnFinish_Click(object? sender, EventArgs e)
    {
        if (!ValidateAllInputs(out string message))
        {
            MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        ApplyBillingSettings();
        ApplyUserDbSettings();
        ApplyAccountSettings();
        _settings.Save();
        Properties.Settings.Default.IsSetup = true;
        Properties.Settings.Default.Save();
        DialogResult = DialogResult.OK;
        Close();
    }

    private void OnBillingProviderChanged(object? sender, EventArgs e)
    {
        if (_loadingControls)
        {
            return;
        }

        MarkBillingStepPending(updateUi: pageSetup.SelectedIndex == BillingPageIndex);
        UpdateBillingUiForProvider();
    }

    private void OnBillingTrustedChanged(object? sender, EventArgs e)
    {
        if (_loadingControls)
        {
            return;
        }

        MarkBillingStepPending(updateUi: pageSetup.SelectedIndex == BillingPageIndex);
        UpdateBillingUiForProvider();
    }

    private void OnUserDbProviderChanged(object? sender, EventArgs e)
    {
        if (_loadingControls)
        {
            return;
        }

        UpdateUserDbUiForProvider();
    }

    private void OnUserDbTrustedChanged(object? sender, EventArgs e)
    {
        if (_loadingControls)
        {
            return;
        }

        UpdateUserDbUiForProvider();
    }

    private void UpdateBillingUiForProvider()
    {
        UpdateAuthFieldsForProvider(GetSelectedProvider(cboBillingProvider), chkBillingTrusted, txtBillingHost, txtBillingUser, txtBillingPass);
    }

    private void UpdateUserDbUiForProvider()
    {
        UpdateAuthFieldsForProvider(GetSelectedProvider(cboUserDbProvider), chkUserDbTrusted, txtUserDbHost, txtUserDbUser, txtUserDbPass);
    }

    private void UpdateAuthFieldsForProvider(LoginDatabaseProvider provider, CheckBox trustedCheckBox, TextBox hostTextBox, TextBox userTextBox, TextBox passTextBox)
    {
        bool isSqlite = provider == LoginDatabaseProvider.Sqlite;
        bool isSqlServer = provider == LoginDatabaseProvider.SqlServer;

        if (!isSqlite && !isSqlServer && trustedCheckBox.Checked)
        {
            _loadingControls = true;
            trustedCheckBox.Checked = false;
            _loadingControls = false;
        }

        trustedCheckBox.Enabled = !isSqlite && isSqlServer;

        bool useTrusted = isSqlServer && trustedCheckBox.Checked;
        if (useTrusted && !string.Equals(hostTextBox.Text, DatabaseSettings.TrustedSqlServerHost, StringComparison.Ordinal))
        {
            _loadingControls = true;
            hostTextBox.Text = DatabaseSettings.TrustedSqlServerHost;
            _loadingControls = false;
        }

        hostTextBox.Enabled = !isSqlite && !useTrusted;

        bool useSqlCredentials = !isSqlite && (provider == LoginDatabaseProvider.MariaDb || !trustedCheckBox.Checked);
        userTextBox.Enabled = useSqlCredentials;
        passTextBox.Enabled = useSqlCredentials;
    }

    private static LoginDatabaseProvider GetSelectedProvider(ComboBox comboBox)
    {
        return comboBox.SelectedIndex switch
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

    private bool SaveBillingSnapshot(bool showMessage, out string message)
    {
        var provider = GetSelectedProvider(cboBillingProvider);
        _billingDb.Provider = provider;
        _billingDb.Host = DatabaseSettings.NormalizeHostForStorage(
            provider,
            txtBillingHost.Text.Trim(),
            provider == LoginDatabaseProvider.SqlServer && chkBillingTrusted.Checked);
        _billingDb.Port = DatabaseSettings.GetEffectivePort(
            provider,
            _billingDb.Host,
            provider == LoginDatabaseProvider.MariaDb ? DatabaseSettings.DefaultMariaDbPort : DatabaseSettings.DefaultSqlServerPort);
        _billingDb.Database = txtBillingDbName.Text.Trim();
        _billingDb.User = txtBillingUser.Text.Trim();
        _billingDb.Password = txtBillingPass.Text;
        _billingDb.Trusted = chkBillingTrusted.Checked;
        message = string.Empty;
        return true;
    }

    private bool SaveUserDbSnapshot(bool showMessage, out string message)
    {
        var provider = GetSelectedProvider(cboUserDbProvider);
        _userDb.Provider = provider;
        _userDb.Host = DatabaseSettings.NormalizeHostForStorage(
            provider,
            txtUserDbHost.Text.Trim(),
            provider == LoginDatabaseProvider.SqlServer && chkUserDbTrusted.Checked);
        _userDb.Port = DatabaseSettings.GetEffectivePort(
            provider,
            _userDb.Host,
            provider == LoginDatabaseProvider.MariaDb ? DatabaseSettings.DefaultMariaDbPort : DatabaseSettings.DefaultSqlServerPort);
        _userDb.User = txtUserDbUser.Text.Trim();
        _userDb.Password = txtUserDbPass.Text;
        _userDb.Trusted = chkUserDbTrusted.Checked;
        message = string.Empty;
        return true;
    }

    private bool ValidateCurrentPage(out string message)
    {
        return pageSetup.SelectedIndex == BillingPageIndex ? ValidateBillingInputs(out message) : ValidateAccountInputs(out message);
    }

    private bool ValidateAllInputs(out string message)
    {
        if (!ValidateBillingInputs(out message))
        {
            pageSetup.SelectedIndex = BillingPageIndex;
            UpdateWizardUi();
            return false;
        }

        if (!_billingStepReady)
        {
            pageSetup.SelectedIndex = BillingPageIndex;
            UpdateWizardUi();
            message = "Use Save Setting or Install New DB before continuing.";
            return false;
        }

        if (!ValidateAccountInputs(out message))
        {
            pageSetup.SelectedIndex = 1;
            UpdateWizardUi();
            return false;
        }

        message = string.Empty;
        return true;
    }

    private bool ValidateBillingInputs(out string message)
    {
        if (!SaveBillingSnapshot(showMessage: false, out message))
        {
            return false;
        }

        return ValidateDbSnapshot(_billingDb, "Billing DB", requireDatabaseName: true, out message);
    }

    private bool ValidateAccountInputs(out string message)
    {
        if (string.IsNullOrWhiteSpace(txtAccountHost.Text))
        {
            message = "AccountServer host is required.";
            return false;
        }

        if (numAccountPort.Value < 1 || numAccountPort.Value > 65535)
        {
            message = "AccountServer port must be 1-65535.";
            return false;
        }

        if (numClientPort.Value < 1 || numClientPort.Value > 65535)
        {
            message = "Client port must be 1-65535.";
            return false;
        }

        if (!SaveUserDbSnapshot(showMessage: false, out message))
        {
            return false;
        }

        return ValidateDbSnapshot(_userDb, "RF_User DB", requireDatabaseName: false, out message);
    }

    private static bool ValidateDbSnapshot(DatabaseSnapshot snapshot, string profileName, bool requireDatabaseName, out string message)
    {
        if (requireDatabaseName && string.IsNullOrWhiteSpace(snapshot.Database))
        {
            message = $"{profileName} database name is required.";
            return false;
        }

        if (snapshot.Provider != LoginDatabaseProvider.Sqlite)
        {
            if (string.IsNullOrWhiteSpace(snapshot.Host))
            {
                message = $"{profileName} host is required.";
                return false;
            }

            if ((snapshot.Provider == LoginDatabaseProvider.MariaDb || !snapshot.Trusted) &&
                string.IsNullOrWhiteSpace(snapshot.User))
            {
                message = $"{profileName} username is required.";
                return false;
            }
        }

        message = string.Empty;
        return true;
    }

    private void ApplyBillingSettings()
    {
        _settings.Database.BillingProvider = _billingDb.Provider;
        _settings.Database.BillingHost = _billingDb.Host;
        _settings.Database.BillingPort = _billingDb.Port;
        _settings.Database.BillingDatabase = _billingDb.Database;
        _settings.Database.BillingUser = _billingDb.User;
        _settings.Database.BillingPassword = _billingDb.Password;
        _settings.Database.BillingTrustedConnection = _billingDb.Trusted;
    }

    private void ApplyUserDbSettings()
    {
        _settings.Database.Provider = _userDb.Provider;
        _settings.Database.Host = _userDb.Host;
        _settings.Database.Port = _userDb.Port;
        _settings.Database.User = _userDb.User;
        _settings.Database.Password = _userDb.Password;
        _settings.Database.TrustedConnection = _userDb.Trusted;
    }

    private void ApplyAccountSettings()
    {
        _settings.Network.AccountHost = txtAccountHost.Text.Trim();
        _settings.Network.AccountPort = (int)numAccountPort.Value;
        _settings.Network.ClientPort = (int)numClientPort.Value;
    }

    private async Task<bool> InstallBillingDatabaseAsync(bool force)
    {
        InstallProgressForm? progress = null;
        try
        {
            string script = await LoadBillingScriptAsync(_billingDb.Provider).ConfigureAwait(true);
            script = RewriteInstallScriptDatabaseName(script, "Billing", _billingDb.Database);

            IReadOnlyList<string> commands = _billingDb.Provider == LoginDatabaseProvider.SqlServer
                ? EmbeddedSqlScripts.SplitSqlServerBatches(script)
                : EmbeddedSqlScripts.SplitDelimitedStatements(script);

            int totalSteps = commands.Count + (force ? 1 : 0);
            progress = new InstallProgressForm("Install Billing DB");
            progress.Show(this);
            progress.SetMarquee("Preparing Billing install...");

            int completedSteps = 0;
            if (force)
            {
                progress.SetProgress("Removing existing Billing database...", completedSteps, totalSteps);
                await DeleteExistingBillingDatabaseAsync().ConfigureAwait(true);
                completedSteps++;
            }

            bool ok = _billingDb.Provider switch
            {
                LoginDatabaseProvider.Sqlite => await InstallSqliteBillingAsync(progress, commands, completedSteps, totalSteps).ConfigureAwait(true),
                LoginDatabaseProvider.MariaDb => await InstallMariaDbBillingAsync(progress, commands, completedSteps, totalSteps).ConfigureAwait(true),
                _ => await InstallSqlServerBillingAsync(progress, commands, completedSteps, totalSteps).ConfigureAwait(true)
            };
            return ok;
        }
        catch (Exception ex)
        {
            if (progress != null && !progress.IsDisposed)
            {
                progress.Close();
            }
            MessageBox.Show(this, $"Billing install failed: {ex.Message}", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
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

    private async Task DeleteExistingBillingDatabaseAsync()
    {
        switch (_billingDb.Provider)
        {
            case LoginDatabaseProvider.Sqlite:
                DeleteExistingSqliteBillingDatabase();
                return;
            case LoginDatabaseProvider.MariaDb:
                await DeleteExistingMariaDbBillingDatabaseAsync().ConfigureAwait(true);
                return;
            default:
                await DeleteExistingSqlServerBillingDatabaseAsync().ConfigureAwait(true);
                return;
        }
    }

    private async Task DeleteExistingSqlServerBillingDatabaseAsync()
    {
        string connString = BuildSqlServerConnectionString(_billingDb, "master");
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
        cmd.Parameters.AddWithValue("@name", _billingDb.Database);
        await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
    }

    private async Task DeleteExistingMariaDbBillingDatabaseAsync()
    {
        string connString = BuildMariaDbConnectionString(_billingDb, "information_schema");
        await using var conn = new MySqlConnection(connString);
        await conn.OpenAsync().ConfigureAwait(true);
        string escapedName = EscapeMariaDbIdentifier(_billingDb.Database);
        string dropSql = $"DROP DATABASE IF EXISTS `{escapedName}`;";
        await using var cmd = new MySqlCommand(dropSql, conn);
        await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
    }

    private void DeleteExistingSqliteBillingDatabase()
    {
        string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _billingDb.Database);
        if (File.Exists(dbPath))
        {
            File.Delete(dbPath);
        }
    }

    private static Task<string> LoadBillingScriptAsync(LoginDatabaseProvider provider)
    {
        return provider switch
        {
            LoginDatabaseProvider.Sqlite => EmbeddedSqlScripts.LoadTextAsync("LoginServer.Schemas.Billing_sqlite.sql"),
            LoginDatabaseProvider.MariaDb => EmbeddedSqlScripts.LoadTextAsync("LoginServer.Schemas.Billing_mariadb.sql"),
            _ => EmbeddedSqlScripts.LoadTextAsync("LoginServer.Schemas.Billing_mssql.sql")
        };
    }

    private static string DescribeInstallUnit(LoginDatabaseProvider provider)
    {
        return provider == LoginDatabaseProvider.SqlServer ? "batch" : "statement";
    }

    private async Task<bool> InstallSqlServerBillingAsync(InstallProgressForm progress, IReadOnlyList<string> commands, int completedSteps, int totalSteps)
    {
        string connString = BuildSqlServerConnectionString(_billingDb, "master");
        await using var conn = new SqlConnection(connString);
        progress.SetProgress("Connecting to SQL Server...", completedSteps, totalSteps);
        await conn.OpenAsync().ConfigureAwait(true);
        for (int i = 0; i < commands.Count; i++)
        {
            progress.SetProgress(
                $"Executing Billing {DescribeInstallUnit(LoginDatabaseProvider.SqlServer)} {i + 1} of {commands.Count}...",
                completedSteps,
                totalSteps);
            await using var cmd = new SqlCommand(commands[i], conn);
            await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
            completedSteps++;
        }

        return true;
    }

    private async Task<bool> InstallMariaDbBillingAsync(InstallProgressForm progress, IReadOnlyList<string> commands, int completedSteps, int totalSteps)
    {
        string connString = BuildMariaDbConnectionString(_billingDb, "information_schema");
        await using var conn = new MySqlConnection(connString);
        progress.SetProgress("Connecting to MariaDB...", completedSteps, totalSteps);
        await conn.OpenAsync().ConfigureAwait(true);
        for (int i = 0; i < commands.Count; i++)
        {
            progress.SetProgress(
                $"Executing Billing {DescribeInstallUnit(LoginDatabaseProvider.MariaDb)} {i + 1} of {commands.Count}...",
                completedSteps,
                totalSteps);
            await using var cmd = new MySqlCommand(commands[i], conn);
            await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
            completedSteps++;
        }

        return true;
    }

    private async Task<bool> InstallSqliteBillingAsync(InstallProgressForm progress, IReadOnlyList<string> commands, int completedSteps, int totalSteps)
    {
        string dbPath = DatabaseSettings.GetSqlitePath(AppContext.BaseDirectory, _billingDb.Database);
        Directory.CreateDirectory(Path.GetDirectoryName(dbPath) ?? AppContext.BaseDirectory);
        string connString = $"Data Source={dbPath};";
        await using var conn = new SqliteConnection(connString);
        progress.SetProgress("Creating SQLite Billing database...", completedSteps, totalSteps);
        await conn.OpenAsync().ConfigureAwait(true);
        for (int i = 0; i < commands.Count; i++)
        {
            progress.SetProgress(
                $"Executing Billing {DescribeInstallUnit(LoginDatabaseProvider.Sqlite)} {i + 1} of {commands.Count}...",
                completedSteps,
                totalSteps);
            await using var cmd = new SqliteCommand(commands[i], conn);
            await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
            completedSteps++;
        }

        return true;
    }

    private static string BuildSqlServerConnectionString(DatabaseSnapshot profile, string database)
    {
        string host = DatabaseSettings.BuildSqlServerDataSource(profile.Host, profile.Port, profile.Trusted);
        if (profile.Trusted)
        {
            return $"Server={host};Database={database};Integrated Security=True;TrustServerCertificate=True;Encrypt=False;";
        }

        return $"Server={host};Database={database};User ID={profile.User};Password={profile.Password};TrustServerCertificate=True;Encrypt=False;";
    }

    private static string BuildMariaDbConnectionString(DatabaseSnapshot profile, string database)
    {
        DatabaseSettings.ResolveMariaDbEndpoint(profile.Host, profile.Port, out string host, out int port);
        return $"Server={host};Port={port};Database={database};User ID={profile.User};Password={profile.Password};";
    }

    private static string RewriteInstallScriptDatabaseName(string script, string defaultDatabaseName, string targetDatabaseName)
    {
        if (string.IsNullOrWhiteSpace(targetDatabaseName) ||
            string.Equals(defaultDatabaseName, targetDatabaseName, StringComparison.OrdinalIgnoreCase))
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
