using System;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Windows.Forms;
using AccountServer.Security;

namespace AccountServer.Settings;

public sealed class SettingsForm : Form
{
    private readonly AppSettings _settings;
    private readonly DatabaseSnapshot _userDb;
    private readonly DatabaseSnapshot _billingDb;
    private readonly BindingList<WorldEntry> _worlds;
    private readonly BindingList<string> _gmPrefixes;
    private bool _loading;

    private TabControl tabMain = null!;
    private ComboBox cboDbProfile = null!;
    private TextBox txtDbHost = null!;
    private TextBox txtDbPort = null!;
    private TextBox txtDbName = null!;
    private TextBox txtDbUser = null!;
    private TextBox txtDbPass = null!;
    private TextBox txtArgon2Salt = null!;
    private Button btnSaltRegenerate = null!;
    private RadioButton radAuthTrusted = null!;
    private RadioButton radAuthSql = null!;
    private ListBox lstGmPrefixes = null!;
    private TextBox txtGmPrefix = null!;
    private Button btnGmAdd = null!;
    private Button btnGmRemove = null!;
    private DataGridView gridWorlds = null!;
    private Button btnWorldAdd = null!;
    private Button btnSave = null!;
    private Button btnCancel = null!;

    public SettingsForm(AppSettings settings)
    {
        _settings = settings;
        _userDb = DatabaseSnapshot.From(settings.Database.User);
        _billingDb = DatabaseSnapshot.From(settings.Database.Billing);
        _worlds = new BindingList<WorldEntry>(settings.WorldList.Worlds.Select(CloneWorldEntry).ToList());
        _gmPrefixes = new BindingList<string>(settings.GmFilter.Prefixes.ToList());
        InitializeComponent();
        LoadFromSettings();
    }

    private static WorldEntry CloneWorldEntry(WorldEntry entry)
    {
        return new WorldEntry
        {
            Name = entry.Name,
            Address = entry.Address,
            DbName = entry.DbName,
            Type = entry.Type
        };
    }

    private void InitializeComponent()
    {
        Text = "Account Server Settings";
        MinimumSize = new Size(640, 480);
        StartPosition = FormStartPosition.CenterParent;

        tabMain = new TabControl
        {
            Dock = DockStyle.Fill
        };

        var dbTab = new TabPage("Database");
        dbTab.Controls.Add(BuildDatabaseTab());
        tabMain.TabPages.Add(dbTab);

        var gmTab = new TabPage("GM Filter");
        gmTab.Controls.Add(BuildGmTab());
        tabMain.TabPages.Add(gmTab);

        var worldTab = new TabPage("World List");
        worldTab.Controls.Add(BuildWorldTab());
        tabMain.TabPages.Add(worldTab);

        var buttons = new FlowLayoutPanel
        {
            Dock = DockStyle.Bottom,
            FlowDirection = FlowDirection.RightToLeft,
            Padding = new Padding(10),
            Height = 48
        };

        btnSave = new Button
        {
            Text = "Save",
            AutoSize = true
        };
        btnSave.Click += OnSave;
        btnCancel = new Button
        {
            Text = "Cancel",
            AutoSize = true
        };
        btnCancel.Click += OnCancel;
        buttons.Controls.Add(btnSave);
        buttons.Controls.Add(btnCancel);

        Controls.Add(tabMain);
        Controls.Add(buttons);
        AcceptButton = btnSave;
        CancelButton = btnCancel;
    }

    private Control BuildDatabaseTab()
    {
        cboDbProfile = new ComboBox
        {
            DropDownStyle = ComboBoxStyle.DropDownList,
            Dock = DockStyle.Fill
        };
        cboDbProfile.SelectedIndexChanged += OnDbProfileChanged;

        txtDbHost = new TextBox { Dock = DockStyle.Fill };
        txtDbPort = new TextBox { Dock = DockStyle.Fill };
        txtDbName = new TextBox { Dock = DockStyle.Fill };
        txtDbUser = new TextBox { Dock = DockStyle.Fill };
        txtDbPass = new TextBox { Dock = DockStyle.Fill, UseSystemPasswordChar = true };
        txtArgon2Salt = new TextBox { Dock = DockStyle.Fill };
        btnSaltRegenerate = new Button { Text = "Regenerate", AutoSize = true };
        btnSaltRegenerate.Click += OnRegenerateSalt;

        radAuthTrusted = new RadioButton { Text = "Trusted Connection" };
        radAuthSql = new RadioButton { Text = "SQL Login" };
        radAuthTrusted.CheckedChanged += OnAuthModeChanged;
        radAuthSql.CheckedChanged += OnAuthModeChanged;

        var authPanel = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill,
            AutoSize = true
        };
        authPanel.Controls.Add(radAuthTrusted);
        authPanel.Controls.Add(radAuthSql);

        var layout = new TableLayoutPanel
        {
            Dock = DockStyle.Top,
            AutoSize = true,
            Padding = new Padding(8),
            ColumnCount = 2
        };
        layout.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 140));
        layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));

        layout.Controls.Add(new Label { Text = "Profile", AutoSize = true }, 0, 0);
        layout.Controls.Add(cboDbProfile, 1, 0);
        layout.Controls.Add(new Label { Text = "Host", AutoSize = true }, 0, 1);
        layout.Controls.Add(txtDbHost, 1, 1);
        layout.Controls.Add(new Label { Text = "Port", AutoSize = true }, 0, 2);
        layout.Controls.Add(txtDbPort, 1, 2);
        layout.Controls.Add(new Label { Text = "Database", AutoSize = true }, 0, 3);
        layout.Controls.Add(txtDbName, 1, 3);
        layout.Controls.Add(new Label { Text = "Authentication", AutoSize = true }, 0, 4);
        layout.Controls.Add(authPanel, 1, 4);
        layout.Controls.Add(new Label { Text = "User", AutoSize = true }, 0, 5);
        layout.Controls.Add(txtDbUser, 1, 5);
        layout.Controls.Add(new Label { Text = "Password", AutoSize = true }, 0, 6);
        layout.Controls.Add(txtDbPass, 1, 6);
        layout.Controls.Add(new Label { Text = "Argon2 Salt (Base64)", AutoSize = true }, 0, 7);
        layout.Controls.Add(BuildSaltRow(), 1, 7);

        var panel = new Panel
        {
            Dock = DockStyle.Fill,
            AutoScroll = true
        };
        panel.Controls.Add(layout);
        return panel;
    }

    private Control BuildGmTab()
    {
        lstGmPrefixes = new ListBox
        {
            Dock = DockStyle.Fill
        };
        lstGmPrefixes.DataSource = _gmPrefixes;

        txtGmPrefix = new TextBox { Width = 140 };
        btnGmAdd = new Button { Text = "Add", AutoSize = true };
        btnGmRemove = new Button { Text = "Remove", AutoSize = true };
        btnGmAdd.Click += OnGmAdd;
        btnGmRemove.Click += OnGmRemove;

        var rightPanel = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill,
            FlowDirection = FlowDirection.TopDown,
            AutoSize = true,
            Padding = new Padding(6)
        };
        rightPanel.Controls.Add(new Label { Text = "Prefix", AutoSize = true });
        rightPanel.Controls.Add(txtGmPrefix);
        rightPanel.Controls.Add(btnGmAdd);
        rightPanel.Controls.Add(btnGmRemove);

        var layout = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 2,
            Padding = new Padding(8)
        };
        layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 70));
        layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 30));
        layout.Controls.Add(lstGmPrefixes, 0, 0);
        layout.Controls.Add(rightPanel, 1, 0);
        return layout;
    }

    private Control BuildWorldTab()
    {
        btnWorldAdd = new Button
        {
            Text = "Add",
            Size = new Size(75, 23),
            Anchor = AnchorStyles.Top | AnchorStyles.Right
        };
        btnWorldAdd.Click += OnWorldAdd;

        var header = new FlowLayoutPanel
        {
            Dock = DockStyle.Top,
            Height = 34,
            FlowDirection = FlowDirection.RightToLeft,
            Padding = new Padding(0, 5, 8, 5)
        };
        header.Controls.Add(btnWorldAdd);

        gridWorlds = new DataGridView
        {
            Dock = DockStyle.Fill,
            AutoGenerateColumns = false,
            AllowUserToAddRows = false,
            AllowUserToDeleteRows = false,
            ReadOnly = true,
            RowHeadersVisible = false,
            SelectionMode = DataGridViewSelectionMode.FullRowSelect,
            MultiSelect = false
        };
        gridWorlds.CellContentClick += OnWorldGridClick;

        gridWorlds.Columns.Add(new DataGridViewTextBoxColumn
        {
            HeaderText = "Name",
            DataPropertyName = "Name",
            AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill,
            FillWeight = 30
        });
        gridWorlds.Columns.Add(new DataGridViewTextBoxColumn
        {
            HeaderText = "Address",
            DataPropertyName = "Address",
            AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill,
            FillWeight = 40
        });
        gridWorlds.Columns.Add(new DataGridViewTextBoxColumn
        {
            HeaderText = "DB Name",
            DataPropertyName = "DbName",
            AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill,
            FillWeight = 25
        });
        gridWorlds.Columns.Add(new DataGridViewTextBoxColumn
        {
            HeaderText = "Type",
            DataPropertyName = "Type",
            AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells
        });
        gridWorlds.Columns.Add(new DataGridViewButtonColumn
        {
            Name = "colEdit",
            HeaderText = "",
            Text = "Edit",
            UseColumnTextForButtonValue = true,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells
        });
        gridWorlds.Columns.Add(new DataGridViewButtonColumn
        {
            Name = "colDelete",
            HeaderText = "",
            Text = "Delete",
            UseColumnTextForButtonValue = true,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells
        });

        gridWorlds.DataSource = _worlds;

        var panel = new Panel
        {
            Dock = DockStyle.Fill,
            Padding = new Padding(8)
        };
        panel.Controls.Add(header);
        panel.Controls.Add(gridWorlds);
        return panel;
    }

    private void LoadFromSettings()
    {
        _loading = true;
        cboDbProfile.Items.Clear();
        cboDbProfile.Items.AddRange(new object[] { "User DB", "Billing DB" });
        cboDbProfile.SelectedIndex = 0;
        _loading = false;
        LoadDbProfileToControls();
    }

    private void LoadDbProfileToControls()
    {
        var db = cboDbProfile.SelectedIndex == 1 ? _billingDb : _userDb;
        txtDbHost.Text = db.Host;
        txtDbPort.Text = db.Port.ToString(CultureInfo.InvariantCulture);
        txtDbName.Text = db.Database;
        txtDbUser.Text = db.User;
        txtDbPass.Text = db.Password;
        txtArgon2Salt.Text = _settings.Security.Argon2SaltBase64;
        radAuthTrusted.Checked = db.Trusted;
        radAuthSql.Checked = !db.Trusted;
        UpdateAuthFields();
    }

    private bool SaveCurrentDbProfile()
    {
        if (!int.TryParse(txtDbPort.Text, out var port) || port < 1 || port > 65535)
        {
            MessageBox.Show(this, "DB port must be 1-65535.", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
        }

        var db = cboDbProfile.SelectedIndex == 1 ? _billingDb : _userDb;
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
        return true;
    }

    private void OnDbProfileChanged(object? sender, EventArgs e)
    {
        if (_loading)
        {
            return;
        }
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

    private void UpdateAuthFields()
    {
        bool sqlAuth = radAuthSql.Checked;
        txtDbUser.Enabled = sqlAuth;
        txtDbPass.Enabled = sqlAuth;
    }

    private Control BuildSaltRow()
    {
        var row = new TableLayoutPanel
        {
            ColumnCount = 2,
            Dock = DockStyle.Fill,
            AutoSize = true
        };
        row.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
        row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        row.Controls.Add(txtArgon2Salt, 0, 0);
        row.Controls.Add(btnSaltRegenerate, 1, 0);
        return row;
    }

    private void OnRegenerateSalt(object? sender, EventArgs e)
    {
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
