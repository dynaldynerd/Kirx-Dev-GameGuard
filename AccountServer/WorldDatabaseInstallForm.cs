using System;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using AccountServer.Settings;
using Microsoft.Data.SqlClient;

namespace AccountServer
{
    public partial class WorldDatabaseInstallForm : Form
    {
        private const string DefaultDatabaseName = "RF_World";
        private readonly string _databaseName;

        public WorldDatabaseInstallForm(string databaseName, string? serverAddress = null, bool trustedConnection = true)
        {
            InitializeComponent();
            BindCountryOptions();
            _databaseName = databaseName;
            txtDatabaseName.Text = databaseName;
            txtServerAddress.Text = string.IsNullOrWhiteSpace(serverAddress)
                ? DbProfile.TrustedSqlServerHost
                : serverAddress.Trim();
            chkTrustedConnection.Checked = trustedConnection;
            UpdateCredentialUi();
        }

        private void BindCountryOptions()
        {
            cmbCountry.DisplayMember = nameof(WorldDatabaseCollationOption.DisplayName);
            cmbCountry.ValueMember = nameof(WorldDatabaseCollationOption.Key);
            cmbCountry.DataSource = WorldDatabaseCollationOptions.All;
            cmbCountry.SelectedItem = WorldDatabaseCollationOptions.Default;
        }

        private WorldDatabaseCollationOption GetSelectedCollationOption()
        {
            return cmbCountry.SelectedItem as WorldDatabaseCollationOption
                ?? WorldDatabaseCollationOptions.Default;
        }

        private void TrustedConnectionChanged(object? sender, EventArgs e)
        {
            UpdateCredentialUi();
        }

        private void UpdateCredentialUi()
        {
            bool useTrusted = chkTrustedConnection.Checked;
            if (useTrusted && !string.Equals(txtServerAddress.Text, DbProfile.TrustedSqlServerHost, StringComparison.Ordinal))
            {
                txtServerAddress.Text = DbProfile.TrustedSqlServerHost;
            }
            txtServerAddress.Enabled = !useTrusted;
            txtUsername.Enabled = !useTrusted;
            txtPassword.Enabled = !useTrusted;
        }

        private async void BtnInstall_Click(object? sender, EventArgs e)
        {
            if (!ValidateInputs(out string message))
            {
                MessageBox.Show(this, message, "Invalid Settings", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            btnInstall.Enabled = false;
            btnCancel.Enabled = false;

            try
            {
                bool installed = await InstallWorldDatabaseAsync().ConfigureAwait(true);
                if (!installed)
                {
                    return;
                }

                MessageBox.Show(this, "World database installed successfully.", "Install RF_World DB", MessageBoxButtons.OK, MessageBoxIcon.Information);
                DialogResult = DialogResult.OK;
                Close();
            }
            finally
            {
                if (!IsDisposed)
                {
                    btnInstall.Enabled = true;
                    btnCancel.Enabled = true;
                }
            }
        }

        private bool ValidateInputs(out string message)
        {
            if (string.IsNullOrWhiteSpace(_databaseName))
            {
                message = "World DB name is required before installation.";
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtServerAddress.Text))
            {
                message = "Server address is required.";
                return false;
            }

            if (!chkTrustedConnection.Checked && string.IsNullOrWhiteSpace(txtUsername.Text))
            {
                message = "Username is required when trusted connection is disabled.";
                return false;
            }

            message = string.Empty;
            return true;
        }

        private async Task<bool> InstallWorldDatabaseAsync()
        {
            string script = await EmbeddedSqlScripts.LoadTextAsync("AccountServer.Schemas.RF_World_mssql.sql").ConfigureAwait(true);
            script = RewriteInstallScriptDatabaseName(script, DefaultDatabaseName, _databaseName);
            WorldDatabaseCollationOption collationOption = GetSelectedCollationOption();
            script = RewriteInstallScriptCollation(script, collationOption.SqlServerCollation);
            IReadOnlyList<string> batches = EmbeddedSqlScripts.SplitSqlServerBatches(script);

            string masterConnectionString = BuildConnectionString("master");
            InstallProgressForm? progress = null;

            try
            {
                await using var conn = new SqlConnection(masterConnectionString);
                await conn.OpenAsync().ConfigureAwait(true);
                bool deleteExisting = false;

                if (await DatabaseExistsAsync(conn).ConfigureAwait(true))
                {
                    string prompt =
                        $"The world database '{_databaseName}' already exists.\n\n" +
                        "Reinstalling will delete the existing database and create it again from the bundled RF_World schema.\n\n" +
                        $"Selected country: {collationOption.Summary}\n\n" +
                        "Do you want to continue?";

                    if (MessageBox.Show(this, prompt, "Install RF_World DB", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes)
                    {
                        return false;
                    }
                    deleteExisting = true;
                }

                int totalSteps = batches.Count + (deleteExisting ? 1 : 0);
                int completedSteps = 0;
                progress = new InstallProgressForm("Install RF_World DB");
                progress.Show(this);
                progress.SetMarquee("Preparing RF_World install...");

                if (deleteExisting)
                {
                    progress.SetProgress("Removing existing RF_World database...", completedSteps, totalSteps);
                    await DeleteExistingDatabaseAsync(conn).ConfigureAwait(true);
                    completedSteps++;
                }

                for (int i = 0; i < batches.Count; i++)
                {
                    progress.SetProgress(
                        $"Executing RF_World batch {i + 1} of {batches.Count}...",
                        completedSteps,
                        totalSteps);
                    await using var cmd = new SqlCommand(batches[i], conn);
                    await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
                    completedSteps++;
                }

                return true;
            }
            catch (Exception ex)
            {
                if (progress != null && !progress.IsDisposed)
                {
                    progress.Close();
                }
                MessageBox.Show(this, $"World database install failed: {ex.Message}", "Install RF_World DB", MessageBoxButtons.OK, MessageBoxIcon.Error);
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

        private async Task<bool> DatabaseExistsAsync(SqlConnection conn)
        {
            await using var cmd = new SqlCommand("SELECT DB_ID(@name)", conn);
            cmd.Parameters.AddWithValue("@name", _databaseName);
            object? result = await cmd.ExecuteScalarAsync().ConfigureAwait(true);
            return result != null && result != DBNull.Value;
        }

        private async Task DeleteExistingDatabaseAsync(SqlConnection conn)
        {
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
            cmd.Parameters.AddWithValue("@name", _databaseName);
            await cmd.ExecuteNonQueryAsync().ConfigureAwait(true);
        }

        private string BuildConnectionString(string database)
        {
            var builder = new SqlConnectionStringBuilder
            {
                DataSource = chkTrustedConnection.Checked ? DbProfile.TrustedSqlServerHost : txtServerAddress.Text.Trim(),
                InitialCatalog = database,
                TrustServerCertificate = true,
                Encrypt = false
            };

            if (chkTrustedConnection.Checked)
            {
                builder.IntegratedSecurity = true;
            }
            else
            {
                builder.UserID = txtUsername.Text.Trim();
                builder.Password = txtPassword.Text;
            }

            return builder.ConnectionString;
        }

        private static string RewriteInstallScriptDatabaseName(string script, string defaultDatabaseName, string targetDatabaseName)
        {
            if (string.IsNullOrWhiteSpace(targetDatabaseName) ||
                string.Equals(targetDatabaseName, defaultDatabaseName, StringComparison.OrdinalIgnoreCase))
            {
                return script;
            }

            string pattern = $@"(?<![A-Za-z0-9_]){Regex.Escape(defaultDatabaseName)}(?![A-Za-z0-9_])";
            return Regex.Replace(script, pattern, targetDatabaseName, RegexOptions.IgnoreCase | RegexOptions.CultureInvariant);
        }

        private static string RewriteInstallScriptCollation(string script, string sqlServerCollation)
        {
            if (string.IsNullOrWhiteSpace(sqlServerCollation))
            {
                return script;
            }

            var createDatabasePattern = new Regex(
                @"^(?<prefix>\s*CREATE\s+DATABASE\s+\[[^\]]+\])(?:\s+COLLATE\s+\S+)?\s*$",
                RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.CultureInvariant);

            return createDatabasePattern.Replace(
                script,
                match => $"{match.Groups["prefix"].Value} COLLATE {sqlServerCollation}",
                1);
        }

    }
}
