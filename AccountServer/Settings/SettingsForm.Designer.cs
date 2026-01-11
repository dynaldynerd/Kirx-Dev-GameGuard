#nullable disable
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace AccountServer.Settings;

public partial class SettingsForm
{
    private IContainer? components = null;
    private TabControl tabMain = null!;
    private TabPage tabGeneral = null!;
    private TabPage tabDatabase = null!;
    private TabPage tabGmFilter = null!;
    private TabPage tabWorldList = null!;
    private Panel panelGeneral = null!;
    private TableLayoutPanel tblGeneral = null!;
    private Label lblArgon2Salt = null!;
    private Label lblMaxActive = null!;
    private Label lblListenHost = null!;
    private Label lblLoginPort = null!;
    private Label lblWorldPort = null!;
    private Label lblControlPort = null!;
    private TableLayoutPanel tblSalt = null!;
    private TextBox txtArgon2Salt = null!;
    private Button btnSaltRegenerate = null!;
    private NumericUpDown numMaxActive = null!;
    private TextBox txtListenHost = null!;
    private NumericUpDown numLoginPort = null!;
    private NumericUpDown numWorldPort = null!;
    private NumericUpDown numControlPort = null!;
    private Panel panelDb = null!;
    private TableLayoutPanel tblDatabase = null!;
    private Label lblDbProfile = null!;
    private Label lblDbHost = null!;
    private Label lblDbPort = null!;
    private Label lblDbName = null!;
    private Label lblDbAuth = null!;
    private Label lblDbUser = null!;
    private Label lblDbPass = null!;
    private ComboBox cboDbProfile = null!;
    private TextBox txtDbHost = null!;
    private TextBox txtDbPort = null!;
    private TextBox txtDbName = null!;
    private TextBox txtDbUser = null!;
    private TextBox txtDbPass = null!;
    private FlowLayoutPanel panelAuth = null!;
    private RadioButton radAuthTrusted = null!;
    private RadioButton radAuthSql = null!;
    private TableLayoutPanel tblGm = null!;
    private ListBox lstGmPrefixes = null!;
    private FlowLayoutPanel panelGmButtons = null!;
    private Label lblGmPrefix = null!;
    private TextBox txtGmPrefix = null!;
    private Button btnGmAdd = null!;
    private Button btnGmRemove = null!;
    private Panel panelWorld = null!;
    private TableLayoutPanel tblWorldLayout = null!;
    private TableLayoutPanel tblWorldHeader = null!;
    private Label lblWorldList = null!;
    private Button btnWorldAdd = null!;
    private DataGridView gridWorlds = null!;
    private DataGridViewTextBoxColumn colWorldName = null!;
    private DataGridViewTextBoxColumn colWorldAddress = null!;
    private DataGridViewTextBoxColumn colWorldDbName = null!;
    private DataGridViewTextBoxColumn colWorldType = null!;
    private DataGridViewButtonColumn colWorldEdit = null!;
    private DataGridViewButtonColumn colWorldDelete = null!;
    private FlowLayoutPanel panelButtons = null!;
    private Button btnSave = null!;
    private Button btnCancel = null!;

    protected override void Dispose(bool disposing)
    {
        if (disposing && components != null)
        {
            components.Dispose();
        }
        base.Dispose(disposing);
    }

    private void InitializeComponent()
    {
        tabMain = new TabControl();
        tabGeneral = new TabPage();
        panelGeneral = new Panel();
        tblGeneral = new TableLayoutPanel();
        lblArgon2Salt = new Label();
        tblSalt = new TableLayoutPanel();
        txtArgon2Salt = new TextBox();
        btnSaltRegenerate = new Button();
        lblMaxActive = new Label();
        numMaxActive = new NumericUpDown();
        lblListenHost = new Label();
        txtListenHost = new TextBox();
        lblLoginPort = new Label();
        numLoginPort = new NumericUpDown();
        lblWorldPort = new Label();
        numWorldPort = new NumericUpDown();
        lblControlPort = new Label();
        numControlPort = new NumericUpDown();
        tabDatabase = new TabPage();
        panelDb = new Panel();
        tblDatabase = new TableLayoutPanel();
        lblDbProfile = new Label();
        cboDbProfile = new ComboBox();
        lblDbHost = new Label();
        txtDbHost = new TextBox();
        lblDbPort = new Label();
        txtDbPort = new TextBox();
        lblDbName = new Label();
        txtDbName = new TextBox();
        lblDbAuth = new Label();
        panelAuth = new FlowLayoutPanel();
        radAuthTrusted = new RadioButton();
        radAuthSql = new RadioButton();
        lblDbUser = new Label();
        txtDbUser = new TextBox();
        lblDbPass = new Label();
        txtDbPass = new TextBox();
        tabGmFilter = new TabPage();
        tblGm = new TableLayoutPanel();
        lstGmPrefixes = new ListBox();
        panelGmButtons = new FlowLayoutPanel();
        lblGmPrefix = new Label();
        txtGmPrefix = new TextBox();
        btnGmAdd = new Button();
        btnGmRemove = new Button();
        tabWorldList = new TabPage();
        panelWorld = new Panel();
        tblWorldLayout = new TableLayoutPanel();
        tblWorldHeader = new TableLayoutPanel();
        lblWorldList = new Label();
        btnWorldAdd = new Button();
        gridWorlds = new DataGridView();
        colWorldEdit = new DataGridViewButtonColumn();
        colWorldDelete = new DataGridViewButtonColumn();
        panelButtons = new FlowLayoutPanel();
        btnSave = new Button();
        btnCancel = new Button();
        tabMain.SuspendLayout();
        tabGeneral.SuspendLayout();
        panelGeneral.SuspendLayout();
        tblGeneral.SuspendLayout();
        tblSalt.SuspendLayout();
        ((ISupportInitialize)numMaxActive).BeginInit();
        ((ISupportInitialize)numLoginPort).BeginInit();
        ((ISupportInitialize)numWorldPort).BeginInit();
        ((ISupportInitialize)numControlPort).BeginInit();
        tabDatabase.SuspendLayout();
        panelDb.SuspendLayout();
        tblDatabase.SuspendLayout();
        panelAuth.SuspendLayout();
        tabGmFilter.SuspendLayout();
        tblGm.SuspendLayout();
        panelGmButtons.SuspendLayout();
        tabWorldList.SuspendLayout();
        panelWorld.SuspendLayout();
        tblWorldLayout.SuspendLayout();
        tblWorldHeader.SuspendLayout();
        ((ISupportInitialize)gridWorlds).BeginInit();
        panelButtons.SuspendLayout();
        SuspendLayout();
        // 
        // tabMain
        // 
        tabMain.Controls.Add(tabGeneral);
        tabMain.Controls.Add(tabDatabase);
        tabMain.Controls.Add(tabGmFilter);
        tabMain.Controls.Add(tabWorldList);
        tabMain.Dock = DockStyle.Fill;
        tabMain.Location = new Point(0, 0);
        tabMain.Name = "tabMain";
        tabMain.SelectedIndex = 0;
        tabMain.Size = new Size(720, 472);
        tabMain.TabIndex = 0;
        // 
        // tabGeneral
        // 
        tabGeneral.Controls.Add(panelGeneral);
        tabGeneral.Location = new Point(4, 24);
        tabGeneral.Name = "tabGeneral";
        tabGeneral.Size = new Size(712, 444);
        tabGeneral.TabIndex = 0;
        tabGeneral.Text = "General";
        tabGeneral.UseVisualStyleBackColor = true;
        // 
        // panelGeneral
        // 
        panelGeneral.AutoScroll = true;
        panelGeneral.Controls.Add(tblGeneral);
        panelGeneral.Dock = DockStyle.Fill;
        panelGeneral.Location = new Point(0, 0);
        panelGeneral.Name = "panelGeneral";
        panelGeneral.Size = new Size(712, 444);
        panelGeneral.TabIndex = 0;
        // 
        // tblGeneral
        // 
        tblGeneral.AutoSize = true;
        tblGeneral.ColumnCount = 2;
        tblGeneral.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 180F));
        tblGeneral.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        tblGeneral.Controls.Add(lblArgon2Salt, 0, 0);
        tblGeneral.Controls.Add(tblSalt, 1, 0);
        tblGeneral.Controls.Add(lblMaxActive, 0, 1);
        tblGeneral.Controls.Add(numMaxActive, 1, 1);
        tblGeneral.Controls.Add(lblListenHost, 0, 2);
        tblGeneral.Controls.Add(txtListenHost, 1, 2);
        tblGeneral.Controls.Add(lblLoginPort, 0, 3);
        tblGeneral.Controls.Add(numLoginPort, 1, 3);
        tblGeneral.Controls.Add(lblWorldPort, 0, 4);
        tblGeneral.Controls.Add(numWorldPort, 1, 4);
        tblGeneral.Controls.Add(lblControlPort, 0, 5);
        tblGeneral.Controls.Add(numControlPort, 1, 5);
        tblGeneral.Dock = DockStyle.Top;
        tblGeneral.Location = new Point(0, 0);
        tblGeneral.Name = "tblGeneral";
        tblGeneral.Padding = new Padding(8);
        tblGeneral.RowCount = 6;
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.RowStyles.Add(new RowStyle());
        tblGeneral.Size = new Size(712, 198);
        tblGeneral.TabIndex = 0;
        // 
        // lblArgon2Salt
        // 
        lblArgon2Salt.AutoSize = true;
        lblArgon2Salt.Location = new Point(11, 8);
        lblArgon2Salt.Name = "lblArgon2Salt";
        lblArgon2Salt.Size = new Size(115, 15);
        lblArgon2Salt.TabIndex = 0;
        lblArgon2Salt.Text = "Argon2 Salt (Base64)";
        // 
        // tblSalt
        // 
        tblSalt.AutoSize = true;
        tblSalt.AutoSizeMode = AutoSizeMode.GrowAndShrink;
        tblSalt.ColumnCount = 2;
        tblSalt.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        tblSalt.ColumnStyles.Add(new ColumnStyle());
        tblSalt.Controls.Add(txtArgon2Salt, 0, 0);
        tblSalt.Controls.Add(btnSaltRegenerate, 1, 0);
        tblSalt.Dock = DockStyle.Fill;
        tblSalt.Location = new Point(191, 11);
        tblSalt.Name = "tblSalt";
        tblSalt.RowCount = 1;
        tblSalt.RowStyles.Add(new RowStyle());
        tblSalt.Size = new Size(510, 31);
        tblSalt.TabIndex = 1;
        // 
        // txtArgon2Salt
        // 
        txtArgon2Salt.Dock = DockStyle.Fill;
        txtArgon2Salt.Location = new Point(3, 3);
        txtArgon2Salt.Name = "txtArgon2Salt";
        txtArgon2Salt.Size = new Size(422, 23);
        txtArgon2Salt.TabIndex = 0;
        // 
        // btnSaltRegenerate
        // 
        btnSaltRegenerate.AutoSize = true;
        btnSaltRegenerate.Location = new Point(431, 3);
        btnSaltRegenerate.Name = "btnSaltRegenerate";
        btnSaltRegenerate.Size = new Size(76, 25);
        btnSaltRegenerate.TabIndex = 1;
        btnSaltRegenerate.Text = "Regenerate";
        btnSaltRegenerate.Click += OnRegenerateSalt;
        // 
        // lblMaxActive
        // 
        lblMaxActive.AutoSize = true;
        lblMaxActive.Location = new Point(11, 45);
        lblMaxActive.Name = "lblMaxActive";
        lblMaxActive.Size = new Size(104, 15);
        lblMaxActive.TabIndex = 2;
        lblMaxActive.Text = "Max Active Clients";
        // 
        // numMaxActive
        // 
        numMaxActive.Location = new Point(191, 48);
        numMaxActive.Maximum = new decimal(new int[] { 1000000, 0, 0, 0 });
        numMaxActive.Minimum = new decimal(new int[] { 1, 0, 0, int.MinValue });
        numMaxActive.Name = "numMaxActive";
        numMaxActive.Size = new Size(120, 23);
        numMaxActive.TabIndex = 3;
        // 
        // lblListenHost
        // 
        lblListenHost.AutoSize = true;
        lblListenHost.Location = new Point(11, 74);
        lblListenHost.Name = "lblListenHost";
        lblListenHost.Size = new Size(66, 15);
        lblListenHost.TabIndex = 4;
        lblListenHost.Text = "Listen Host";
        // 
        // txtListenHost
        // 
        txtListenHost.Dock = DockStyle.Fill;
        txtListenHost.Location = new Point(191, 77);
        txtListenHost.Name = "txtListenHost";
        txtListenHost.Size = new Size(510, 23);
        txtListenHost.TabIndex = 5;
        // 
        // lblLoginPort
        // 
        lblLoginPort.AutoSize = true;
        lblLoginPort.Location = new Point(11, 103);
        lblLoginPort.Name = "lblLoginPort";
        lblLoginPort.Size = new Size(62, 15);
        lblLoginPort.TabIndex = 6;
        lblLoginPort.Text = "Login Port";
        // 
        // numLoginPort
        // 
        numLoginPort.Location = new Point(191, 106);
        numLoginPort.Maximum = new decimal(new int[] { 65535, 0, 0, 0 });
        numLoginPort.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numLoginPort.Name = "numLoginPort";
        numLoginPort.Size = new Size(120, 23);
        numLoginPort.TabIndex = 7;
        numLoginPort.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // lblWorldPort
        // 
        lblWorldPort.AutoSize = true;
        lblWorldPort.Location = new Point(11, 132);
        lblWorldPort.Name = "lblWorldPort";
        lblWorldPort.Size = new Size(64, 15);
        lblWorldPort.TabIndex = 8;
        lblWorldPort.Text = "World Port";
        // 
        // numWorldPort
        // 
        numWorldPort.Location = new Point(191, 135);
        numWorldPort.Maximum = new decimal(new int[] { 65535, 0, 0, 0 });
        numWorldPort.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numWorldPort.Name = "numWorldPort";
        numWorldPort.Size = new Size(120, 23);
        numWorldPort.TabIndex = 9;
        numWorldPort.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // lblControlPort
        // 
        lblControlPort.AutoSize = true;
        lblControlPort.Location = new Point(11, 161);
        lblControlPort.Name = "lblControlPort";
        lblControlPort.Size = new Size(72, 15);
        lblControlPort.TabIndex = 10;
        lblControlPort.Text = "Control Port";
        // 
        // numControlPort
        // 
        numControlPort.Location = new Point(191, 164);
        numControlPort.Maximum = new decimal(new int[] { 65535, 0, 0, 0 });
        numControlPort.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numControlPort.Name = "numControlPort";
        numControlPort.Size = new Size(120, 23);
        numControlPort.TabIndex = 11;
        numControlPort.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // tabDatabase
        // 
        tabDatabase.Controls.Add(panelDb);
        tabDatabase.Location = new Point(4, 24);
        tabDatabase.Name = "tabDatabase";
        tabDatabase.Size = new Size(712, 444);
        tabDatabase.TabIndex = 1;
        tabDatabase.Text = "Database";
        tabDatabase.UseVisualStyleBackColor = true;
        // 
        // panelDb
        // 
        panelDb.AutoScroll = true;
        panelDb.Controls.Add(tblDatabase);
        panelDb.Dock = DockStyle.Fill;
        panelDb.Location = new Point(0, 0);
        panelDb.Name = "panelDb";
        panelDb.Size = new Size(712, 444);
        panelDb.TabIndex = 0;
        // 
        // tblDatabase
        // 
        tblDatabase.AutoSize = true;
        tblDatabase.ColumnCount = 2;
        tblDatabase.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 140F));
        tblDatabase.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        tblDatabase.Controls.Add(lblDbProfile, 0, 0);
        tblDatabase.Controls.Add(cboDbProfile, 1, 0);
        tblDatabase.Controls.Add(lblDbHost, 0, 1);
        tblDatabase.Controls.Add(txtDbHost, 1, 1);
        tblDatabase.Controls.Add(lblDbPort, 0, 2);
        tblDatabase.Controls.Add(txtDbPort, 1, 2);
        tblDatabase.Controls.Add(lblDbName, 0, 3);
        tblDatabase.Controls.Add(txtDbName, 1, 3);
        tblDatabase.Controls.Add(lblDbAuth, 0, 4);
        tblDatabase.Controls.Add(panelAuth, 1, 4);
        tblDatabase.Controls.Add(lblDbUser, 0, 5);
        tblDatabase.Controls.Add(txtDbUser, 1, 5);
        tblDatabase.Controls.Add(lblDbPass, 0, 6);
        tblDatabase.Controls.Add(txtDbPass, 1, 6);
        tblDatabase.Dock = DockStyle.Top;
        tblDatabase.Location = new Point(0, 0);
        tblDatabase.Name = "tblDatabase";
        tblDatabase.Padding = new Padding(8);
        tblDatabase.RowCount = 7;
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.RowStyles.Add(new RowStyle());
        tblDatabase.Size = new Size(712, 221);
        tblDatabase.TabIndex = 0;
        // 
        // lblDbProfile
        // 
        lblDbProfile.AutoSize = true;
        lblDbProfile.Location = new Point(11, 8);
        lblDbProfile.Name = "lblDbProfile";
        lblDbProfile.Size = new Size(41, 15);
        lblDbProfile.TabIndex = 0;
        lblDbProfile.Text = "Profile";
        // 
        // cboDbProfile
        // 
        cboDbProfile.Dock = DockStyle.Fill;
        cboDbProfile.DropDownStyle = ComboBoxStyle.DropDownList;
        cboDbProfile.Location = new Point(151, 11);
        cboDbProfile.Name = "cboDbProfile";
        cboDbProfile.Size = new Size(550, 23);
        cboDbProfile.TabIndex = 1;
        cboDbProfile.SelectedIndexChanged += OnDbProfileChanged;
        cboDbProfile.SelectionChangeCommitted += OnDbProfileChanged;
        // 
        // lblDbHost
        // 
        lblDbHost.AutoSize = true;
        lblDbHost.Location = new Point(11, 37);
        lblDbHost.Name = "lblDbHost";
        lblDbHost.Size = new Size(32, 15);
        lblDbHost.TabIndex = 2;
        lblDbHost.Text = "Host";
        // 
        // txtDbHost
        // 
        txtDbHost.Dock = DockStyle.Fill;
        txtDbHost.Location = new Point(151, 40);
        txtDbHost.Name = "txtDbHost";
        txtDbHost.Size = new Size(550, 23);
        txtDbHost.TabIndex = 3;
        // 
        // lblDbPort
        // 
        lblDbPort.AutoSize = true;
        lblDbPort.Location = new Point(11, 66);
        lblDbPort.Name = "lblDbPort";
        lblDbPort.Size = new Size(29, 15);
        lblDbPort.TabIndex = 4;
        lblDbPort.Text = "Port";
        // 
        // txtDbPort
        // 
        txtDbPort.Dock = DockStyle.Fill;
        txtDbPort.Location = new Point(151, 69);
        txtDbPort.Name = "txtDbPort";
        txtDbPort.Size = new Size(550, 23);
        txtDbPort.TabIndex = 5;
        // 
        // lblDbName
        // 
        lblDbName.AutoSize = true;
        lblDbName.Location = new Point(11, 95);
        lblDbName.Name = "lblDbName";
        lblDbName.Size = new Size(55, 15);
        lblDbName.TabIndex = 6;
        lblDbName.Text = "Database";
        // 
        // txtDbName
        // 
        txtDbName.Dock = DockStyle.Fill;
        txtDbName.Location = new Point(151, 98);
        txtDbName.Name = "txtDbName";
        txtDbName.Size = new Size(550, 23);
        txtDbName.TabIndex = 7;
        // 
        // lblDbAuth
        // 
        lblDbAuth.AutoSize = true;
        lblDbAuth.Location = new Point(11, 124);
        lblDbAuth.Name = "lblDbAuth";
        lblDbAuth.Size = new Size(86, 15);
        lblDbAuth.TabIndex = 8;
        lblDbAuth.Text = "Authentication";
        // 
        // panelAuth
        // 
        panelAuth.AutoSize = true;
        panelAuth.Controls.Add(radAuthTrusted);
        panelAuth.Controls.Add(radAuthSql);
        panelAuth.Dock = DockStyle.Fill;
        panelAuth.Location = new Point(151, 127);
        panelAuth.Name = "panelAuth";
        panelAuth.Size = new Size(550, 25);
        panelAuth.TabIndex = 9;
        // 
        // radAuthTrusted
        // 
        radAuthTrusted.AutoSize = true;
        radAuthTrusted.Location = new Point(3, 3);
        radAuthTrusted.Name = "radAuthTrusted";
        radAuthTrusted.Size = new Size(129, 19);
        radAuthTrusted.TabIndex = 0;
        radAuthTrusted.Text = "Trusted Connection";
        radAuthTrusted.CheckedChanged += OnAuthModeChanged;
        // 
        // radAuthSql
        // 
        radAuthSql.AutoSize = true;
        radAuthSql.Location = new Point(138, 3);
        radAuthSql.Name = "radAuthSql";
        radAuthSql.Size = new Size(79, 19);
        radAuthSql.TabIndex = 1;
        radAuthSql.Text = "SQL Login";
        radAuthSql.CheckedChanged += OnAuthModeChanged;
        // 
        // lblDbUser
        // 
        lblDbUser.AutoSize = true;
        lblDbUser.Location = new Point(11, 155);
        lblDbUser.Name = "lblDbUser";
        lblDbUser.Size = new Size(30, 15);
        lblDbUser.TabIndex = 10;
        lblDbUser.Text = "User";
        // 
        // txtDbUser
        // 
        txtDbUser.Dock = DockStyle.Fill;
        txtDbUser.Location = new Point(151, 158);
        txtDbUser.Name = "txtDbUser";
        txtDbUser.Size = new Size(550, 23);
        txtDbUser.TabIndex = 11;
        // 
        // lblDbPass
        // 
        lblDbPass.AutoSize = true;
        lblDbPass.Location = new Point(11, 184);
        lblDbPass.Name = "lblDbPass";
        lblDbPass.Size = new Size(57, 15);
        lblDbPass.TabIndex = 12;
        lblDbPass.Text = "Password";
        // 
        // txtDbPass
        // 
        txtDbPass.Dock = DockStyle.Fill;
        txtDbPass.Location = new Point(151, 187);
        txtDbPass.Name = "txtDbPass";
        txtDbPass.Size = new Size(550, 23);
        txtDbPass.TabIndex = 13;
        txtDbPass.UseSystemPasswordChar = true;
        // 
        // tabGmFilter
        // 
        tabGmFilter.Controls.Add(tblGm);
        tabGmFilter.Location = new Point(4, 24);
        tabGmFilter.Name = "tabGmFilter";
        tabGmFilter.Size = new Size(712, 444);
        tabGmFilter.TabIndex = 2;
        tabGmFilter.Text = "GM Filter";
        tabGmFilter.UseVisualStyleBackColor = true;
        // 
        // tblGm
        // 
        tblGm.ColumnCount = 2;
        tblGm.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 70F));
        tblGm.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 30F));
        tblGm.Controls.Add(lstGmPrefixes, 0, 0);
        tblGm.Controls.Add(panelGmButtons, 1, 0);
        tblGm.Dock = DockStyle.Fill;
        tblGm.Location = new Point(0, 0);
        tblGm.Name = "tblGm";
        tblGm.Padding = new Padding(8);
        tblGm.RowStyles.Add(new RowStyle(SizeType.Absolute, 20F));
        tblGm.Size = new Size(712, 444);
        tblGm.TabIndex = 0;
        // 
        // lstGmPrefixes
        // 
        lstGmPrefixes.Dock = DockStyle.Fill;
        lstGmPrefixes.Location = new Point(11, 11);
        lstGmPrefixes.Name = "lstGmPrefixes";
        lstGmPrefixes.Size = new Size(481, 422);
        lstGmPrefixes.TabIndex = 0;
        // 
        // panelGmButtons
        // 
        panelGmButtons.AutoSize = true;
        panelGmButtons.Controls.Add(lblGmPrefix);
        panelGmButtons.Controls.Add(txtGmPrefix);
        panelGmButtons.Controls.Add(btnGmAdd);
        panelGmButtons.Controls.Add(btnGmRemove);
        panelGmButtons.Dock = DockStyle.Fill;
        panelGmButtons.FlowDirection = FlowDirection.TopDown;
        panelGmButtons.Location = new Point(498, 11);
        panelGmButtons.Name = "panelGmButtons";
        panelGmButtons.Padding = new Padding(6);
        panelGmButtons.Size = new Size(203, 422);
        panelGmButtons.TabIndex = 1;
        // 
        // lblGmPrefix
        // 
        lblGmPrefix.AutoSize = true;
        lblGmPrefix.Location = new Point(9, 6);
        lblGmPrefix.Name = "lblGmPrefix";
        lblGmPrefix.Size = new Size(36, 15);
        lblGmPrefix.TabIndex = 0;
        lblGmPrefix.Text = "Prefix";
        // 
        // txtGmPrefix
        // 
        txtGmPrefix.Location = new Point(9, 24);
        txtGmPrefix.Name = "txtGmPrefix";
        txtGmPrefix.Size = new Size(140, 23);
        txtGmPrefix.TabIndex = 1;
        // 
        // btnGmAdd
        // 
        btnGmAdd.AutoSize = true;
        btnGmAdd.Location = new Point(9, 53);
        btnGmAdd.Name = "btnGmAdd";
        btnGmAdd.Size = new Size(75, 25);
        btnGmAdd.TabIndex = 2;
        btnGmAdd.Text = "Add";
        btnGmAdd.Click += OnGmAdd;
        // 
        // btnGmRemove
        // 
        btnGmRemove.AutoSize = true;
        btnGmRemove.Location = new Point(9, 84);
        btnGmRemove.Name = "btnGmRemove";
        btnGmRemove.Size = new Size(75, 25);
        btnGmRemove.TabIndex = 3;
        btnGmRemove.Text = "Remove";
        btnGmRemove.Click += OnGmRemove;
        // 
        // tabWorldList
        // 
        tabWorldList.Controls.Add(panelWorld);
        tabWorldList.Location = new Point(4, 24);
        tabWorldList.Name = "tabWorldList";
        tabWorldList.Size = new Size(712, 444);
        tabWorldList.TabIndex = 3;
        tabWorldList.Text = "World List";
        tabWorldList.UseVisualStyleBackColor = true;
        // 
        // panelWorld
        // 
        panelWorld.Controls.Add(tblWorldLayout);
        panelWorld.Dock = DockStyle.Fill;
        panelWorld.Location = new Point(0, 0);
        panelWorld.Name = "panelWorld";
        panelWorld.Padding = new Padding(8);
        panelWorld.Size = new Size(712, 444);
        panelWorld.TabIndex = 0;
        // 
        // tblWorldLayout
        // 
        tblWorldLayout.ColumnCount = 1;
        tblWorldLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        tblWorldLayout.Controls.Add(tblWorldHeader, 0, 0);
        tblWorldLayout.Controls.Add(gridWorlds, 0, 1);
        tblWorldLayout.Dock = DockStyle.Fill;
        tblWorldLayout.Location = new Point(8, 8);
        tblWorldLayout.Name = "tblWorldLayout";
        tblWorldLayout.RowCount = 2;
        tblWorldLayout.RowStyles.Add(new RowStyle());
        tblWorldLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100F));
        tblWorldLayout.Size = new Size(696, 428);
        tblWorldLayout.TabIndex = 0;
        // 
        // tblWorldHeader
        // 
        tblWorldHeader.AutoSize = true;
        tblWorldHeader.AutoSizeMode = AutoSizeMode.GrowAndShrink;
        tblWorldHeader.ColumnCount = 2;
        tblWorldHeader.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        tblWorldHeader.ColumnStyles.Add(new ColumnStyle());
        tblWorldHeader.Controls.Add(lblWorldList, 0, 0);
        tblWorldHeader.Controls.Add(btnWorldAdd, 1, 0);
        tblWorldHeader.Dock = DockStyle.Top;
        tblWorldHeader.Location = new Point(3, 3);
        tblWorldHeader.Name = "tblWorldHeader";
        tblWorldHeader.Padding = new Padding(0, 5, 8, 5);
        tblWorldHeader.RowCount = 1;
        tblWorldHeader.RowStyles.Add(new RowStyle());
        tblWorldHeader.Size = new Size(690, 39);
        tblWorldHeader.TabIndex = 0;
        // 
        // lblWorldList
        // 
        lblWorldList.AutoSize = true;
        lblWorldList.Location = new Point(3, 5);
        lblWorldList.Name = "lblWorldList";
        lblWorldList.Size = new Size(77, 15);
        lblWorldList.TabIndex = 0;
        lblWorldList.Text = "World Entries";
        // 
        // btnWorldAdd
        // 
        btnWorldAdd.Anchor = AnchorStyles.Top | AnchorStyles.Right;
        btnWorldAdd.Location = new Point(604, 8);
        btnWorldAdd.Name = "btnWorldAdd";
        btnWorldAdd.Size = new Size(75, 23);
        btnWorldAdd.TabIndex = 1;
        btnWorldAdd.Text = "Add";
        btnWorldAdd.Click += OnWorldAdd;
        // 
        // gridWorlds
        // 
        gridWorlds.AllowUserToAddRows = false;
        gridWorlds.AllowUserToDeleteRows = false;
        gridWorlds.Columns.AddRange(new DataGridViewColumn[] { colWorldEdit, colWorldDelete });
        gridWorlds.Dock = DockStyle.Fill;
        gridWorlds.Location = new Point(3, 48);
        gridWorlds.MultiSelect = false;
        gridWorlds.Name = "gridWorlds";
        gridWorlds.ReadOnly = true;
        gridWorlds.RowHeadersVisible = false;
        gridWorlds.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
        gridWorlds.Size = new Size(690, 377);
        gridWorlds.TabIndex = 1;
        gridWorlds.CellContentClick += OnWorldGridClick;
        // 
        // colWorldEdit
        // 
        colWorldEdit.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
        colWorldEdit.HeaderText = "";
        colWorldEdit.Name = "colWorldEdit";
        colWorldEdit.ReadOnly = true;
        colWorldEdit.Text = "Edit";
        colWorldEdit.UseColumnTextForButtonValue = true;
        colWorldEdit.Width = 5;
        // 
        // colWorldDelete
        // 
        colWorldDelete.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
        colWorldDelete.HeaderText = "";
        colWorldDelete.Name = "colWorldDelete";
        colWorldDelete.ReadOnly = true;
        colWorldDelete.Text = "Delete";
        colWorldDelete.UseColumnTextForButtonValue = true;
        colWorldDelete.Width = 5;
        // 
        // panelButtons
        // 
        panelButtons.Controls.Add(btnSave);
        panelButtons.Controls.Add(btnCancel);
        panelButtons.Dock = DockStyle.Bottom;
        panelButtons.FlowDirection = FlowDirection.RightToLeft;
        panelButtons.Location = new Point(0, 472);
        panelButtons.Name = "panelButtons";
        panelButtons.Padding = new Padding(10);
        panelButtons.Size = new Size(720, 48);
        panelButtons.TabIndex = 1;
        // 
        // btnSave
        // 
        btnSave.AutoSize = true;
        btnSave.Location = new Point(622, 13);
        btnSave.Name = "btnSave";
        btnSave.Size = new Size(75, 25);
        btnSave.TabIndex = 0;
        btnSave.Text = "Save";
        btnSave.Click += OnSave;
        // 
        // btnCancel
        // 
        btnCancel.AutoSize = true;
        btnCancel.Location = new Point(541, 13);
        btnCancel.Name = "btnCancel";
        btnCancel.Size = new Size(75, 25);
        btnCancel.TabIndex = 1;
        btnCancel.Text = "Cancel";
        btnCancel.Click += OnCancel;
        // 
        // SettingsForm
        // 
        AcceptButton = btnSave;
        AutoScaleDimensions = new SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        CancelButton = btnCancel;
        ClientSize = new Size(720, 520);
        Controls.Add(tabMain);
        Controls.Add(panelButtons);
        MinimumSize = new Size(720, 520);
        Name = "SettingsForm";
        StartPosition = FormStartPosition.CenterParent;
        Text = "Account Server Settings";
        tabMain.ResumeLayout(false);
        tabGeneral.ResumeLayout(false);
        panelGeneral.ResumeLayout(false);
        panelGeneral.PerformLayout();
        tblGeneral.ResumeLayout(false);
        tblGeneral.PerformLayout();
        tblSalt.ResumeLayout(false);
        tblSalt.PerformLayout();
        ((ISupportInitialize)numMaxActive).EndInit();
        ((ISupportInitialize)numLoginPort).EndInit();
        ((ISupportInitialize)numWorldPort).EndInit();
        ((ISupportInitialize)numControlPort).EndInit();
        tabDatabase.ResumeLayout(false);
        panelDb.ResumeLayout(false);
        panelDb.PerformLayout();
        tblDatabase.ResumeLayout(false);
        tblDatabase.PerformLayout();
        panelAuth.ResumeLayout(false);
        panelAuth.PerformLayout();
        tabGmFilter.ResumeLayout(false);
        tblGm.ResumeLayout(false);
        tblGm.PerformLayout();
        panelGmButtons.ResumeLayout(false);
        panelGmButtons.PerformLayout();
        tabWorldList.ResumeLayout(false);
        panelWorld.ResumeLayout(false);
        tblWorldLayout.ResumeLayout(false);
        tblWorldLayout.PerformLayout();
        tblWorldHeader.ResumeLayout(false);
        tblWorldHeader.PerformLayout();
        ((ISupportInitialize)gridWorlds).EndInit();
        panelButtons.ResumeLayout(false);
        panelButtons.PerformLayout();
        ResumeLayout(false);
    }
}
