#nullable enable
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace LoginServer;

partial class Setup
{
    private IContainer? components = null;

    private Label lblStepCount = null!;
    private Label lblStepTitle = null!;
    private Label lblStepDescription = null!;
    private TabControl pageSetup = null!;
    private TabPage pageBilling = null!;
    private TabPage pageAccount = null!;
    private Label lblBillingProvider = null!;
    private ComboBox cboBillingProvider = null!;
    private GroupBox grpBillingDatabase = null!;
    private Label lblBillingHost = null!;
    private TextBox txtBillingHost = null!;
    private Label lblBillingPort = null!;
    private TextBox txtBillingPort = null!;
    private Label lblBillingDbName = null!;
    private TextBox txtBillingDbName = null!;
    private CheckBox chkBillingTrusted = null!;
    private Label lblBillingUser = null!;
    private TextBox txtBillingUser = null!;
    private Label lblBillingPass = null!;
    private TextBox txtBillingPass = null!;
    private Button btnSaveSettings = null!;
    private Button btnInstallBillingDb = null!;
    private GroupBox grpAccountConnection = null!;
    private Label lblAccountHost = null!;
    private TextBox txtAccountHost = null!;
    private Label lblAccountPort = null!;
    private NumericUpDown numAccountPort = null!;
    private Label lblClientPort = null!;
    private NumericUpDown numClientPort = null!;
    private GroupBox grpUserDatabase = null!;
    private Label lblUserDbNote = null!;
    private Label lblUserDbProvider = null!;
    private ComboBox cboUserDbProvider = null!;
    private Label lblUserDbHost = null!;
    private TextBox txtUserDbHost = null!;
    private Label lblUserDbPort = null!;
    private TextBox txtUserDbPort = null!;
    private CheckBox chkUserDbTrusted = null!;
    private Label lblUserDbUser = null!;
    private TextBox txtUserDbUser = null!;
    private Label lblUserDbPass = null!;
    private TextBox txtUserDbPass = null!;
    private FlowLayoutPanel panelButtons = null!;
    private Button btnCancel = null!;
    private Button btnFinish = null!;
    private Button btnNext = null!;
    private Button btnBack = null!;

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
        lblStepCount = new Label();
        lblStepTitle = new Label();
        lblStepDescription = new Label();
        pageSetup = new TabControl();
        pageBilling = new TabPage();
        btnSaveSettings = new Button();
        btnInstallBillingDb = new Button();
        grpBillingDatabase = new GroupBox();
        lblBillingPass = new Label();
        txtBillingPass = new TextBox();
        lblBillingUser = new Label();
        txtBillingUser = new TextBox();
        chkBillingTrusted = new CheckBox();
        lblBillingDbName = new Label();
        txtBillingDbName = new TextBox();
        lblBillingPort = new Label();
        txtBillingPort = new TextBox();
        lblBillingHost = new Label();
        txtBillingHost = new TextBox();
        cboBillingProvider = new ComboBox();
        lblBillingProvider = new Label();
        pageAccount = new TabPage();
        grpUserDatabase = new GroupBox();
        lblUserDbPass = new Label();
        txtUserDbPass = new TextBox();
        lblUserDbUser = new Label();
        txtUserDbUser = new TextBox();
        chkUserDbTrusted = new CheckBox();
        lblUserDbPort = new Label();
        txtUserDbPort = new TextBox();
        lblUserDbHost = new Label();
        txtUserDbHost = new TextBox();
        cboUserDbProvider = new ComboBox();
        lblUserDbProvider = new Label();
        lblUserDbNote = new Label();
        grpAccountConnection = new GroupBox();
        lblClientPort = new Label();
        numClientPort = new NumericUpDown();
        lblAccountPort = new Label();
        numAccountPort = new NumericUpDown();
        lblAccountHost = new Label();
        txtAccountHost = new TextBox();
        panelButtons = new FlowLayoutPanel();
        btnCancel = new Button();
        btnFinish = new Button();
        btnNext = new Button();
        btnBack = new Button();
        pageSetup.SuspendLayout();
        pageBilling.SuspendLayout();
        grpBillingDatabase.SuspendLayout();
        pageAccount.SuspendLayout();
        grpUserDatabase.SuspendLayout();
        grpAccountConnection.SuspendLayout();
        ((ISupportInitialize)numClientPort).BeginInit();
        ((ISupportInitialize)numAccountPort).BeginInit();
        panelButtons.SuspendLayout();
        SuspendLayout();
        // 
        // lblStepCount
        // 
        lblStepCount.AutoSize = true;
        lblStepCount.Location = new Point(12, 12);
        lblStepCount.Name = "lblStepCount";
        lblStepCount.Size = new Size(61, 15);
        lblStepCount.TabIndex = 0;
        lblStepCount.Text = "Step 1 of 2";
        // 
        // lblStepTitle
        // 
        lblStepTitle.AutoSize = true;
        lblStepTitle.Font = new Font("Segoe UI", 14F, FontStyle.Bold);
        lblStepTitle.Location = new Point(12, 33);
        lblStepTitle.Name = "lblStepTitle";
        lblStepTitle.Size = new Size(145, 25);
        lblStepTitle.TabIndex = 1;
        lblStepTitle.Text = "Billing Database";
        // 
        // lblStepDescription
        // 
        lblStepDescription.AutoSize = true;
        lblStepDescription.Location = new Point(12, 62);
        lblStepDescription.MaximumSize = new Size(736, 0);
        lblStepDescription.Name = "lblStepDescription";
        lblStepDescription.Size = new Size(712, 30);
        lblStepDescription.TabIndex = 2;
        lblStepDescription.Text = "Configure the Billing database only. Save Setting assumes Billing already exists; Install New DB recreates Billing. Next stays locked until one of those actions succeeds.";
        // 
        // pageSetup
        // 
        pageSetup.Appearance = TabAppearance.FlatButtons;
        pageSetup.Controls.Add(pageBilling);
        pageSetup.Controls.Add(pageAccount);
        pageSetup.ItemSize = new Size(1, 1);
        pageSetup.Location = new Point(12, 103);
        pageSetup.Multiline = true;
        pageSetup.Name = "pageSetup";
        pageSetup.Padding = new Point(0, 0);
        pageSetup.SelectedIndex = 0;
        pageSetup.Size = new Size(736, 382);
        pageSetup.SizeMode = TabSizeMode.Fixed;
        pageSetup.TabIndex = 3;
        // 
        // pageBilling
        // 
        pageBilling.Controls.Add(btnSaveSettings);
        pageBilling.Controls.Add(btnInstallBillingDb);
        pageBilling.Controls.Add(grpBillingDatabase);
        pageBilling.Controls.Add(cboBillingProvider);
        pageBilling.Controls.Add(lblBillingProvider);
        pageBilling.Location = new Point(4, 5);
        pageBilling.Name = "pageBilling";
        pageBilling.Padding = new Padding(10);
        pageBilling.Size = new Size(728, 373);
        pageBilling.TabIndex = 0;
        pageBilling.Text = "Billing";
        pageBilling.UseVisualStyleBackColor = true;
        // 
        // btnSaveSettings
        // 
        btnSaveSettings.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
        btnSaveSettings.Location = new Point(435, 334);
        btnSaveSettings.Name = "btnSaveSettings";
        btnSaveSettings.Size = new Size(122, 27);
        btnSaveSettings.TabIndex = 7;
        btnSaveSettings.Text = "Save Setting";
        btnSaveSettings.UseVisualStyleBackColor = true;
        btnSaveSettings.Click += BtnSaveSettings_Click;
        // 
        // btnInstallBillingDb
        // 
        btnInstallBillingDb.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
        btnInstallBillingDb.Location = new Point(563, 334);
        btnInstallBillingDb.Name = "btnInstallBillingDb";
        btnInstallBillingDb.Size = new Size(152, 27);
        btnInstallBillingDb.TabIndex = 8;
        btnInstallBillingDb.Text = "Install New DB";
        btnInstallBillingDb.UseVisualStyleBackColor = true;
        btnInstallBillingDb.Click += BtnInstallBillingDb_Click;
        // 
        // grpBillingDatabase
        // 
        grpBillingDatabase.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
        grpBillingDatabase.Controls.Add(lblBillingPass);
        grpBillingDatabase.Controls.Add(txtBillingPass);
        grpBillingDatabase.Controls.Add(lblBillingUser);
        grpBillingDatabase.Controls.Add(txtBillingUser);
        grpBillingDatabase.Controls.Add(chkBillingTrusted);
        grpBillingDatabase.Controls.Add(lblBillingDbName);
        grpBillingDatabase.Controls.Add(txtBillingDbName);
        grpBillingDatabase.Controls.Add(lblBillingPort);
        grpBillingDatabase.Controls.Add(txtBillingPort);
        grpBillingDatabase.Controls.Add(lblBillingHost);
        grpBillingDatabase.Controls.Add(txtBillingHost);
        grpBillingDatabase.Location = new Point(13, 49);
        grpBillingDatabase.Name = "grpBillingDatabase";
        grpBillingDatabase.Size = new Size(702, 273);
        grpBillingDatabase.TabIndex = 2;
        grpBillingDatabase.TabStop = false;
        grpBillingDatabase.Text = "Billing DB";
        // 
        // lblBillingPass
        // 
        lblBillingPass.AutoSize = true;
        lblBillingPass.Location = new Point(353, 118);
        lblBillingPass.Name = "lblBillingPass";
        lblBillingPass.Size = new Size(57, 15);
        lblBillingPass.TabIndex = 9;
        lblBillingPass.Text = "Password";
        // 
        // txtBillingPass
        // 
        txtBillingPass.Location = new Point(441, 115);
        txtBillingPass.Name = "txtBillingPass";
        txtBillingPass.Size = new Size(220, 23);
        txtBillingPass.TabIndex = 6;
        txtBillingPass.UseSystemPasswordChar = true;
        // 
        // lblBillingUser
        // 
        lblBillingUser.AutoSize = true;
        lblBillingUser.Location = new Point(353, 88);
        lblBillingUser.Name = "lblBillingUser";
        lblBillingUser.Size = new Size(60, 15);
        lblBillingUser.TabIndex = 7;
        lblBillingUser.Text = "Username";
        // 
        // txtBillingUser
        // 
        txtBillingUser.Location = new Point(441, 85);
        txtBillingUser.Name = "txtBillingUser";
        txtBillingUser.Size = new Size(220, 23);
        txtBillingUser.TabIndex = 5;
        // 
        // chkBillingTrusted
        // 
        chkBillingTrusted.AutoSize = true;
        chkBillingTrusted.Location = new Point(353, 57);
        chkBillingTrusted.Name = "chkBillingTrusted";
        chkBillingTrusted.Size = new Size(124, 19);
        chkBillingTrusted.TabIndex = 4;
        chkBillingTrusted.Text = "Trusted Connection";
        chkBillingTrusted.UseVisualStyleBackColor = true;
        chkBillingTrusted.CheckedChanged += OnBillingTrustedChanged;
        // 
        // lblBillingDbName
        // 
        lblBillingDbName.AutoSize = true;
        lblBillingDbName.Location = new Point(18, 118);
        lblBillingDbName.Name = "lblBillingDbName";
        lblBillingDbName.Size = new Size(91, 15);
        lblBillingDbName.TabIndex = 4;
        lblBillingDbName.Text = "Database name";
        // 
        // txtBillingDbName
        // 
        txtBillingDbName.Location = new Point(124, 115);
        txtBillingDbName.Name = "txtBillingDbName";
        txtBillingDbName.Size = new Size(180, 23);
        txtBillingDbName.TabIndex = 3;
        // 
        // lblBillingPort
        // 
        lblBillingPort.AutoSize = true;
        lblBillingPort.Location = new Point(18, 88);
        lblBillingPort.Name = "lblBillingPort";
        lblBillingPort.Size = new Size(29, 15);
        lblBillingPort.TabIndex = 2;
        lblBillingPort.Text = "Port";
        // 
        // txtBillingPort
        // 
        txtBillingPort.Location = new Point(124, 85);
        txtBillingPort.Name = "txtBillingPort";
        txtBillingPort.Size = new Size(106, 23);
        txtBillingPort.TabIndex = 2;
        // 
        // lblBillingHost
        // 
        lblBillingHost.AutoSize = true;
        lblBillingHost.Location = new Point(18, 58);
        lblBillingHost.Name = "lblBillingHost";
        lblBillingHost.Size = new Size(74, 15);
        lblBillingHost.TabIndex = 0;
        lblBillingHost.Text = "DB host/IP";
        // 
        // txtBillingHost
        // 
        txtBillingHost.Location = new Point(124, 55);
        txtBillingHost.Name = "txtBillingHost";
        txtBillingHost.Size = new Size(180, 23);
        txtBillingHost.TabIndex = 1;
        // 
        // cboBillingProvider
        // 
        cboBillingProvider.DropDownStyle = ComboBoxStyle.DropDownList;
        cboBillingProvider.FormattingEnabled = true;
        cboBillingProvider.Items.AddRange(new object[] { "SQL Server", "MariaDB", "SQLite" });
        cboBillingProvider.Location = new Point(111, 17);
        cboBillingProvider.Name = "cboBillingProvider";
        cboBillingProvider.Size = new Size(149, 23);
        cboBillingProvider.TabIndex = 0;
        cboBillingProvider.SelectedIndexChanged += OnBillingProviderChanged;
        // 
        // lblBillingProvider
        // 
        lblBillingProvider.AutoSize = true;
        lblBillingProvider.Location = new Point(13, 20);
        lblBillingProvider.Name = "lblBillingProvider";
        lblBillingProvider.Size = new Size(92, 15);
        lblBillingProvider.TabIndex = 0;
        lblBillingProvider.Text = "Billing provider";
        // 
        // pageAccount
        // 
        pageAccount.Controls.Add(grpUserDatabase);
        pageAccount.Controls.Add(grpAccountConnection);
        pageAccount.Location = new Point(4, 5);
        pageAccount.Name = "pageAccount";
        pageAccount.Padding = new Padding(10);
        pageAccount.Size = new Size(728, 373);
        pageAccount.TabIndex = 1;
        pageAccount.Text = "Account";
        pageAccount.UseVisualStyleBackColor = true;
        // 
        // grpUserDatabase
        // 
        grpUserDatabase.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
        grpUserDatabase.Controls.Add(lblUserDbPass);
        grpUserDatabase.Controls.Add(txtUserDbPass);
        grpUserDatabase.Controls.Add(lblUserDbUser);
        grpUserDatabase.Controls.Add(txtUserDbUser);
        grpUserDatabase.Controls.Add(chkUserDbTrusted);
        grpUserDatabase.Controls.Add(lblUserDbPort);
        grpUserDatabase.Controls.Add(txtUserDbPort);
        grpUserDatabase.Controls.Add(lblUserDbHost);
        grpUserDatabase.Controls.Add(txtUserDbHost);
        grpUserDatabase.Controls.Add(cboUserDbProvider);
        grpUserDatabase.Controls.Add(lblUserDbProvider);
        grpUserDatabase.Controls.Add(lblUserDbNote);
        grpUserDatabase.Location = new Point(13, 145);
        grpUserDatabase.Name = "grpUserDatabase";
        grpUserDatabase.Size = new Size(702, 198);
        grpUserDatabase.TabIndex = 1;
        grpUserDatabase.TabStop = false;
        grpUserDatabase.Text = "RF_User DB";
        // 
        // lblUserDbPass
        // 
        lblUserDbPass.AutoSize = true;
        lblUserDbPass.Location = new Point(353, 151);
        lblUserDbPass.Name = "lblUserDbPass";
        lblUserDbPass.Size = new Size(57, 15);
        lblUserDbPass.TabIndex = 11;
        lblUserDbPass.Text = "Password";
        // 
        // txtUserDbPass
        // 
        txtUserDbPass.Location = new Point(441, 148);
        txtUserDbPass.Name = "txtUserDbPass";
        txtUserDbPass.Size = new Size(220, 23);
        txtUserDbPass.TabIndex = 6;
        txtUserDbPass.UseSystemPasswordChar = true;
        // 
        // lblUserDbUser
        // 
        lblUserDbUser.AutoSize = true;
        lblUserDbUser.Location = new Point(353, 121);
        lblUserDbUser.Name = "lblUserDbUser";
        lblUserDbUser.Size = new Size(60, 15);
        lblUserDbUser.TabIndex = 9;
        lblUserDbUser.Text = "Username";
        // 
        // txtUserDbUser
        // 
        txtUserDbUser.Location = new Point(441, 118);
        txtUserDbUser.Name = "txtUserDbUser";
        txtUserDbUser.Size = new Size(220, 23);
        txtUserDbUser.TabIndex = 5;
        // 
        // chkUserDbTrusted
        // 
        chkUserDbTrusted.AutoSize = true;
        chkUserDbTrusted.Location = new Point(353, 90);
        chkUserDbTrusted.Name = "chkUserDbTrusted";
        chkUserDbTrusted.Size = new Size(124, 19);
        chkUserDbTrusted.TabIndex = 4;
        chkUserDbTrusted.Text = "Trusted Connection";
        chkUserDbTrusted.UseVisualStyleBackColor = true;
        chkUserDbTrusted.CheckedChanged += OnUserDbTrustedChanged;
        // 
        // lblUserDbPort
        // 
        lblUserDbPort.AutoSize = true;
        lblUserDbPort.Location = new Point(18, 121);
        lblUserDbPort.Name = "lblUserDbPort";
        lblUserDbPort.Size = new Size(29, 15);
        lblUserDbPort.TabIndex = 5;
        lblUserDbPort.Text = "Port";
        // 
        // txtUserDbPort
        // 
        txtUserDbPort.Location = new Point(124, 118);
        txtUserDbPort.Name = "txtUserDbPort";
        txtUserDbPort.Size = new Size(106, 23);
        txtUserDbPort.TabIndex = 3;
        // 
        // lblUserDbHost
        // 
        lblUserDbHost.AutoSize = true;
        lblUserDbHost.Location = new Point(18, 91);
        lblUserDbHost.Name = "lblUserDbHost";
        lblUserDbHost.Size = new Size(74, 15);
        lblUserDbHost.TabIndex = 3;
        lblUserDbHost.Text = "DB host/IP";
        // 
        // txtUserDbHost
        // 
        txtUserDbHost.Location = new Point(124, 88);
        txtUserDbHost.Name = "txtUserDbHost";
        txtUserDbHost.Size = new Size(180, 23);
        txtUserDbHost.TabIndex = 2;
        // 
        // cboUserDbProvider
        // 
        cboUserDbProvider.DropDownStyle = ComboBoxStyle.DropDownList;
        cboUserDbProvider.FormattingEnabled = true;
        cboUserDbProvider.Items.AddRange(new object[] { "SQL Server", "MariaDB", "SQLite" });
        cboUserDbProvider.Location = new Point(124, 58);
        cboUserDbProvider.Name = "cboUserDbProvider";
        cboUserDbProvider.Size = new Size(149, 23);
        cboUserDbProvider.TabIndex = 1;
        cboUserDbProvider.SelectedIndexChanged += OnUserDbProviderChanged;
        // 
        // lblUserDbProvider
        // 
        lblUserDbProvider.AutoSize = true;
        lblUserDbProvider.Location = new Point(18, 61);
        lblUserDbProvider.Name = "lblUserDbProvider";
        lblUserDbProvider.Size = new Size(96, 15);
        lblUserDbProvider.TabIndex = 1;
        lblUserDbProvider.Text = "RF_User provider";
        // 
        // lblUserDbNote
        // 
        lblUserDbNote.AutoSize = true;
        lblUserDbNote.Location = new Point(18, 25);
        lblUserDbNote.MaximumSize = new Size(640, 0);
        lblUserDbNote.Name = "lblUserDbNote";
        lblUserDbNote.Size = new Size(577, 15);
        lblUserDbNote.TabIndex = 0;
        lblUserDbNote.Text = "RF_User database name is supplied by AccountServer at runtime, so this setup only stores how LoginServer should reach it.";
        // 
        // grpAccountConnection
        // 
        grpAccountConnection.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
        grpAccountConnection.Controls.Add(lblClientPort);
        grpAccountConnection.Controls.Add(numClientPort);
        grpAccountConnection.Controls.Add(lblAccountPort);
        grpAccountConnection.Controls.Add(numAccountPort);
        grpAccountConnection.Controls.Add(lblAccountHost);
        grpAccountConnection.Controls.Add(txtAccountHost);
        grpAccountConnection.Location = new Point(13, 13);
        grpAccountConnection.Name = "grpAccountConnection";
        grpAccountConnection.Size = new Size(702, 118);
        grpAccountConnection.TabIndex = 0;
        grpAccountConnection.TabStop = false;
        grpAccountConnection.Text = "AccountServer";
        // 
        // lblClientPort
        // 
        lblClientPort.AutoSize = true;
        lblClientPort.Location = new Point(18, 88);
        lblClientPort.Name = "lblClientPort";
        lblClientPort.Size = new Size(88, 15);
        lblClientPort.TabIndex = 4;
        lblClientPort.Text = "Client listen port";
        // 
        // numClientPort
        // 
        numClientPort.Location = new Point(153, 85);
        numClientPort.Maximum = new decimal(new int[] { 65535, 0, 0, 0 });
        numClientPort.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numClientPort.Name = "numClientPort";
        numClientPort.Size = new Size(120, 23);
        numClientPort.TabIndex = 3;
        numClientPort.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // lblAccountPort
        // 
        lblAccountPort.AutoSize = true;
        lblAccountPort.Location = new Point(18, 58);
        lblAccountPort.Name = "lblAccountPort";
        lblAccountPort.Size = new Size(103, 15);
        lblAccountPort.TabIndex = 2;
        lblAccountPort.Text = "AccountServer port";
        // 
        // numAccountPort
        // 
        numAccountPort.Location = new Point(153, 55);
        numAccountPort.Maximum = new decimal(new int[] { 65535, 0, 0, 0 });
        numAccountPort.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numAccountPort.Name = "numAccountPort";
        numAccountPort.Size = new Size(120, 23);
        numAccountPort.TabIndex = 2;
        numAccountPort.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // lblAccountHost
        // 
        lblAccountHost.AutoSize = true;
        lblAccountHost.Location = new Point(18, 28);
        lblAccountHost.Name = "lblAccountHost";
        lblAccountHost.Size = new Size(104, 15);
        lblAccountHost.TabIndex = 0;
        lblAccountHost.Text = "AccountServer host";
        // 
        // txtAccountHost
        // 
        txtAccountHost.Location = new Point(153, 25);
        txtAccountHost.Name = "txtAccountHost";
        txtAccountHost.Size = new Size(220, 23);
        txtAccountHost.TabIndex = 1;
        // 
        // panelButtons
        // 
        panelButtons.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
        panelButtons.Controls.Add(btnCancel);
        panelButtons.Controls.Add(btnFinish);
        panelButtons.Controls.Add(btnNext);
        panelButtons.Controls.Add(btnBack);
        panelButtons.FlowDirection = FlowDirection.RightToLeft;
        panelButtons.Location = new Point(12, 491);
        panelButtons.Name = "panelButtons";
        panelButtons.Size = new Size(736, 34);
        panelButtons.TabIndex = 4;
        // 
        // btnCancel
        // 
        btnCancel.DialogResult = DialogResult.Cancel;
        btnCancel.Location = new Point(658, 3);
        btnCancel.Name = "btnCancel";
        btnCancel.Size = new Size(75, 27);
        btnCancel.TabIndex = 3;
        btnCancel.Text = "Cancel";
        btnCancel.UseVisualStyleBackColor = true;
        // 
        // btnFinish
        // 
        btnFinish.Location = new Point(577, 3);
        btnFinish.Name = "btnFinish";
        btnFinish.Size = new Size(75, 27);
        btnFinish.TabIndex = 2;
        btnFinish.Text = "Finish";
        btnFinish.UseVisualStyleBackColor = true;
        btnFinish.Visible = false;
        btnFinish.Click += BtnFinish_Click;
        // 
        // btnNext
        // 
        btnNext.Enabled = false;
        btnNext.Location = new Point(496, 3);
        btnNext.Name = "btnNext";
        btnNext.Size = new Size(75, 27);
        btnNext.TabIndex = 1;
        btnNext.Text = "Next >";
        btnNext.UseVisualStyleBackColor = true;
        btnNext.Click += BtnNext_Click;
        // 
        // btnBack
        // 
        btnBack.Enabled = false;
        btnBack.Location = new Point(415, 3);
        btnBack.Name = "btnBack";
        btnBack.Size = new Size(75, 27);
        btnBack.TabIndex = 0;
        btnBack.Text = "< Back";
        btnBack.UseVisualStyleBackColor = true;
        btnBack.Click += BtnBack_Click;
        // 
        // Setup
        // 
        AutoScaleDimensions = new SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        CancelButton = btnCancel;
        ClientSize = new Size(760, 537);
        Controls.Add(panelButtons);
        Controls.Add(pageSetup);
        Controls.Add(lblStepDescription);
        Controls.Add(lblStepTitle);
        Controls.Add(lblStepCount);
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        Name = "Setup";
        StartPosition = FormStartPosition.CenterScreen;
        Text = "LoginServer Setup";
        pageSetup.ResumeLayout(false);
        pageBilling.ResumeLayout(false);
        pageBilling.PerformLayout();
        grpBillingDatabase.ResumeLayout(false);
        grpBillingDatabase.PerformLayout();
        pageAccount.ResumeLayout(false);
        grpUserDatabase.ResumeLayout(false);
        grpUserDatabase.PerformLayout();
        grpAccountConnection.ResumeLayout(false);
        grpAccountConnection.PerformLayout();
        ((ISupportInitialize)numClientPort).EndInit();
        ((ISupportInitialize)numAccountPort).EndInit();
        panelButtons.ResumeLayout(false);
        ResumeLayout(false);
        PerformLayout();
    }
}
