using System;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Windows.Forms;
using AccountServer.Security;

namespace AccountServer.Settings;

public partial class SettingsForm : Form
{
    private readonly AppSettings _settings;
    private readonly DatabaseSnapshot _userDb;
    private readonly DatabaseSnapshot _billingDb;
    private readonly BindingList<WorldEntry> _worlds;
    private readonly BindingList<string> _gmPrefixes;
    private bool _loading;
    private int _currentDbProfileIndex;

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
        _billingDb = DatabaseSnapshot.From(settings.Database.Billing);
        _worlds = new BindingList<WorldEntry>(settings.WorldList.Worlds.Select(CloneWorldEntry).ToList());
        _gmPrefixes = new BindingList<string>(settings.GmFilter.Prefixes.ToList());
        InitializeComponent();
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
        _loading = true;
        cboDbProfile.Items.Clear();
        cboDbProfile.Items.AddRange(new object[] { "User DB", "Billing DB" });
        cboDbProfile.SelectedIndex = 0;
        _currentDbProfileIndex = cboDbProfile.SelectedIndex;
        _loading = false;
        LoadDbProfileToControls();
        LoadGeneralSettings();
    }

    private void LoadDbProfileToControls()
    {
        var db = cboDbProfile.SelectedIndex == 1 ? _billingDb : _userDb;
        txtDbHost.Text = db.Host;
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
        txtArgon2Salt.Text = _settings.Security.Argon2SaltBase64;
        txtListenHost.Text = _settings.Listener.Host;

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

        var db = _currentDbProfileIndex == 1 ? _billingDb : _userDb;
        db.Host = txtDbHost.Text.Trim();
        db.Port = port;
        db.Database = txtDbName.Text.Trim();
        db.User = txtDbUser.Text.Trim();
        db.Password = txtDbPass.Text;
        db.Trusted = radAuthTrusted.Checked;
        return true;
    }

    private bool SaveToSettings()
    {
        if (!SaveCurrentDbProfile())
        {
            return false;
        }

        _userDb.ApplyTo(_settings.Database.User);
        _billingDb.ApplyTo(_settings.Database.Billing);
        var saltText = txtArgon2Salt.Text.Trim();
        if (!TryParseBase64(saltText, out var saltBytes) || saltBytes.Length < 16)
        {
            MessageBox.Show(this, "Argon2 salt must be valid Base64 and at least 16 bytes.", "Invalid Value", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }
        _settings.Security.Argon2SaltBase64 = saltText;
        _settings.GmFilter.Prefixes = _gmPrefixes.Where(p => !string.IsNullOrWhiteSpace(p)).ToList();
        _settings.WorldList.Worlds = _worlds.Select(CloneWorldEntry).ToList();
        _settings.MaxActiveClients = (int)numMaxActive.Value;
        _settings.Listener.Host = txtListenHost.Text.Trim();
        _settings.Listener.LoginPort = (int)numLoginPort.Value;
        _settings.Listener.WorldPort = (int)numWorldPort.Value;
        _settings.Listener.ControlPort = (int)numControlPort.Value;
        return true;
    }

    private void OnDbProfileChanged(object? sender, EventArgs e)
    {
        if (_loading)
        {
            return;
        }
        int newIndex = cboDbProfile.SelectedIndex;
        if (!SaveCurrentDbProfile())
        {
            cboDbProfile.SelectedIndex = _currentDbProfileIndex;
            return;
        }
        _currentDbProfileIndex = newIndex;
        LoadDbProfileToControls();
    }

    private void OnAuthModeChanged(object? sender, EventArgs e)
    {
        UpdateAuthFields();
    }

    private void UpdateAuthFields()
    {
        bool sqlAuth = radAuthSql.Checked;
        txtDbUser.Enabled = sqlAuth;
        txtDbPass.Enabled = sqlAuth;
    }

    private void OnRegenerateSalt(object? sender, EventArgs e)
    {
        var r = MessageBox.Show("Change Salt?", "Are you sure you wanted to change salt? all user password will be gone and had to rehash if you replace this, make sure there's no user account before doing this", MessageBoxButtons.YesNo);
        if(r == DialogResult.Yes)
            txtArgon2Salt.Text = CryptoHelper.GenerateRandomBase64(16);
    }

    private static bool TryParseBase64(string value, out byte[] bytes)
    {
        try
        {
            bytes = Convert.FromBase64String(value);
            return true;
        }
        catch (FormatException)
        {
            bytes = Array.Empty<byte>();
            return false;
        }
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
