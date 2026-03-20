#nullable enable
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace AccountServer
{
    partial class Setup
    {
        private IContainer? components = null;

        private Label lblStepCount = null!;
        private Label lblStepTitle = null!;
        private Label lblStepDescription = null!;
        private TabControl pageSetup = null!;
        private TabPage pageDatabase = null!;
        private TabPage pageWorld = null!;
        private TabPage pageSecurity = null!;
        private ComboBox cmbProvider = null!;
        private Label lblProvider = null!;
        private GroupBox grpUserDb = null!;
        private Label lblUserHost = null!;
        private Label lblUserPort = null!;
        private Label lblUserDb = null!;
        private Label lblUserUser = null!;
        private Label lblUserPass = null!;
        private TextBox txtUserHost = null!;
        private NumericUpDown numUserPort = null!;
        private TextBox txtUserDb = null!;
        private TextBox txtUserUser = null!;
        private TextBox txtUserPass = null!;
        private CheckBox chkUserTrusted = null!;
        private Button btnSaveSettings = null!;
        private Button btnInstallUserDb = null!;
        private GroupBox grpWorld = null!;
        private Label lblWorldName = null!;
        private Label lblWorldAddress = null!;
        private Label lblWorldDb = null!;
        private Label lblWorldType = null!;
        private TextBox txtWorldName = null!;
        private TextBox txtWorldAddress = null!;
        private TextBox txtWorldDb = null!;
        private ComboBox cmbWorldType = null!;
        private Button btnInstallWorldDb = null!;
        private Label lblWorldInstallNote = null!;
        private GroupBox grpSecurity = null!;
        private Label lblArgon2Salt = null!;
        private TextBox txtArgon2Salt = null!;
        private Button btnRegenerateSalt = null!;
        private Label lblSecurityNote = null!;
        private FlowLayoutPanel panelButtons = null!;
        private Button btnFinish = null!;
        private Button btnNext = null!;
        private Button btnBack = null!;
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
            lblStepCount = new Label();
            lblStepTitle = new Label();
            lblStepDescription = new Label();
            pageSetup = new TabControl();
            pageDatabase = new TabPage();
            btnSaveSettings = new Button();
            btnInstallUserDb = new Button();
            grpUserDb = new GroupBox();
            chkUserTrusted = new CheckBox();
            txtUserPass = new TextBox();
            txtUserUser = new TextBox();
            txtUserDb = new TextBox();
            numUserPort = new NumericUpDown();
            txtUserHost = new TextBox();
            lblUserPass = new Label();
            lblUserUser = new Label();
            lblUserDb = new Label();
            lblUserPort = new Label();
            lblUserHost = new Label();
            cmbProvider = new ComboBox();
            lblProvider = new Label();
            pageWorld = new TabPage();
            grpWorld = new GroupBox();
            btnInstallWorldDb = new Button();
            lblWorldInstallNote = new Label();
            cmbWorldType = new ComboBox();
            txtWorldDb = new TextBox();
            txtWorldAddress = new TextBox();
            txtWorldName = new TextBox();
            lblWorldType = new Label();
            lblWorldDb = new Label();
            lblWorldAddress = new Label();
            lblWorldName = new Label();
            pageSecurity = new TabPage();
            grpSecurity = new GroupBox();
            lblSecurityNote = new Label();
            btnRegenerateSalt = new Button();
            txtArgon2Salt = new TextBox();
            lblArgon2Salt = new Label();
            panelButtons = new FlowLayoutPanel();
            btnFinish = new Button();
            btnNext = new Button();
            btnBack = new Button();
            btnCancel = new Button();
            pageSetup.SuspendLayout();
            pageDatabase.SuspendLayout();
            grpUserDb.SuspendLayout();
            ((ISupportInitialize)numUserPort).BeginInit();
            pageWorld.SuspendLayout();
            grpWorld.SuspendLayout();
            pageSecurity.SuspendLayout();
            grpSecurity.SuspendLayout();
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
            lblStepCount.Text = "Step 1 of 3";
            // 
            // lblStepTitle
            // 
            lblStepTitle.AutoSize = true;
            lblStepTitle.Font = new Font("Segoe UI", 14F, FontStyle.Bold);
            lblStepTitle.Location = new Point(12, 33);
            lblStepTitle.Name = "lblStepTitle";
            lblStepTitle.Size = new Size(132, 25);
            lblStepTitle.TabIndex = 1;
            lblStepTitle.Text = "Database Setup";
            // 
            // lblStepDescription
            // 
            lblStepDescription.AutoSize = true;
            lblStepDescription.Location = new Point(12, 62);
            lblStepDescription.MaximumSize = new Size(760, 0);
            lblStepDescription.Name = "lblStepDescription";
            lblStepDescription.Size = new Size(392, 15);
            lblStepDescription.TabIndex = 2;
            lblStepDescription.Text = "Choose the RF_User database provider and connection used by AccountServer.";
            // 
            // pageSetup
            // 
            pageSetup.Appearance = TabAppearance.FlatButtons;
            pageSetup.Controls.Add(pageDatabase);
            pageSetup.Controls.Add(pageWorld);
            pageSetup.Controls.Add(pageSecurity);
            pageSetup.ItemSize = new Size(1, 1);
            pageSetup.Location = new Point(12, 92);
            pageSetup.Multiline = true;
            pageSetup.Name = "pageSetup";
            pageSetup.Padding = new Point(0, 0);
            pageSetup.SelectedIndex = 0;
            pageSetup.Size = new Size(776, 346);
            pageSetup.SizeMode = TabSizeMode.Fixed;
            pageSetup.TabIndex = 3;
            // 
            // pageDatabase
            // 
            pageDatabase.Controls.Add(btnSaveSettings);
            pageDatabase.Controls.Add(btnInstallUserDb);
            pageDatabase.Controls.Add(grpUserDb);
            pageDatabase.Controls.Add(cmbProvider);
            pageDatabase.Controls.Add(lblProvider);
            pageDatabase.Location = new Point(4, 5);
            pageDatabase.Name = "pageDatabase";
            pageDatabase.Padding = new Padding(10);
            pageDatabase.Size = new Size(768, 337);
            pageDatabase.TabIndex = 0;
            pageDatabase.Text = "Database";
            pageDatabase.UseVisualStyleBackColor = true;
            // 
            // btnSaveSettings
            // 
            btnSaveSettings.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            btnSaveSettings.Location = new Point(470, 297);
            btnSaveSettings.Name = "btnSaveSettings";
            btnSaveSettings.Size = new Size(122, 27);
            btnSaveSettings.TabIndex = 13;
            btnSaveSettings.Text = "Save Setting";
            btnSaveSettings.UseVisualStyleBackColor = true;
            btnSaveSettings.Click += BtnSaveSettings_Click;
            // 
            // btnInstallUserDb
            // 
            btnInstallUserDb.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            btnInstallUserDb.Location = new Point(598, 297);
            btnInstallUserDb.Name = "btnInstallUserDb";
            btnInstallUserDb.Size = new Size(157, 27);
            btnInstallUserDb.TabIndex = 14;
            btnInstallUserDb.Text = "Install New DB";
            btnInstallUserDb.UseVisualStyleBackColor = true;
            btnInstallUserDb.Click += BtnInstallUserDb_Click;
            // 
            // grpUserDb
            // 
            grpUserDb.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            grpUserDb.Controls.Add(chkUserTrusted);
            grpUserDb.Controls.Add(txtUserPass);
            grpUserDb.Controls.Add(txtUserUser);
            grpUserDb.Controls.Add(txtUserDb);
            grpUserDb.Controls.Add(numUserPort);
            grpUserDb.Controls.Add(txtUserHost);
            grpUserDb.Controls.Add(lblUserPass);
            grpUserDb.Controls.Add(lblUserUser);
            grpUserDb.Controls.Add(lblUserDb);
            grpUserDb.Controls.Add(lblUserPort);
            grpUserDb.Controls.Add(lblUserHost);
            grpUserDb.Location = new Point(13, 49);
            grpUserDb.Name = "grpUserDb";
            grpUserDb.Size = new Size(742, 234);
            grpUserDb.TabIndex = 2;
            grpUserDb.TabStop = false;
            grpUserDb.Text = "User DB";
            // 
            // chkUserTrusted
            // 
            chkUserTrusted.AutoSize = true;
            chkUserTrusted.Location = new Point(120, 131);
            chkUserTrusted.Name = "chkUserTrusted";
            chkUserTrusted.Size = new Size(124, 19);
            chkUserTrusted.TabIndex = 9;
            chkUserTrusted.Text = "Trusted Connection";
            chkUserTrusted.UseVisualStyleBackColor = true;
            chkUserTrusted.CheckedChanged += TrustedChanged;
            // 
            // txtUserPass
            // 
            txtUserPass.Location = new Point(120, 189);
            txtUserPass.Name = "txtUserPass";
            txtUserPass.PasswordChar = '*';
            txtUserPass.Size = new Size(220, 23);
            txtUserPass.TabIndex = 12;
            // 
            // txtUserUser
            // 
            txtUserUser.Location = new Point(120, 159);
            txtUserUser.Name = "txtUserUser";
            txtUserUser.Size = new Size(220, 23);
            txtUserUser.TabIndex = 10;
            // 
            // txtUserDb
            // 
            txtUserDb.Location = new Point(120, 98);
            txtUserDb.Name = "txtUserDb";
            txtUserDb.Size = new Size(220, 23);
            txtUserDb.TabIndex = 8;
            // 
            // numUserPort
            // 
            numUserPort.Location = new Point(120, 69);
            numUserPort.Maximum = new decimal(new int[] { 65535, 0, 0, 0 });
            numUserPort.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
            numUserPort.Name = "numUserPort";
            numUserPort.Size = new Size(120, 23);
            numUserPort.TabIndex = 7;
            numUserPort.Value = new decimal(new int[] { 1433, 0, 0, 0 });
            // 
            // txtUserHost
            // 
            txtUserHost.Location = new Point(120, 40);
            txtUserHost.Name = "txtUserHost";
            txtUserHost.Size = new Size(220, 23);
            txtUserHost.TabIndex = 6;
            // 
            // lblUserPass
            // 
            lblUserPass.AutoSize = true;
            lblUserPass.Location = new Point(16, 192);
            lblUserPass.Name = "lblUserPass";
            lblUserPass.Size = new Size(57, 15);
            lblUserPass.TabIndex = 4;
            lblUserPass.Text = "Password";
            // 
            // lblUserUser
            // 
            lblUserUser.AutoSize = true;
            lblUserUser.Location = new Point(16, 162);
            lblUserUser.Name = "lblUserUser";
            lblUserUser.Size = new Size(30, 15);
            lblUserUser.TabIndex = 3;
            lblUserUser.Text = "User";
            // 
            // lblUserDb
            // 
            lblUserDb.AutoSize = true;
            lblUserDb.Location = new Point(16, 101);
            lblUserDb.Name = "lblUserDb";
            lblUserDb.Size = new Size(55, 15);
            lblUserDb.TabIndex = 2;
            lblUserDb.Text = "Database";
            // 
            // lblUserPort
            // 
            lblUserPort.AutoSize = true;
            lblUserPort.Location = new Point(16, 72);
            lblUserPort.Name = "lblUserPort";
            lblUserPort.Size = new Size(29, 15);
            lblUserPort.TabIndex = 1;
            lblUserPort.Text = "Port";
            // 
            // lblUserHost
            // 
            lblUserHost.AutoSize = true;
            lblUserHost.Location = new Point(16, 43);
            lblUserHost.Name = "lblUserHost";
            lblUserHost.Size = new Size(32, 15);
            lblUserHost.TabIndex = 0;
            lblUserHost.Text = "Host";
            // 
            // cmbProvider
            // 
            cmbProvider.DropDownStyle = ComboBoxStyle.DropDownList;
            cmbProvider.FormattingEnabled = true;
            cmbProvider.Items.AddRange(new object[] { "SQL Server", "MariaDB", "SQLite" });
            cmbProvider.Location = new Point(78, 15);
            cmbProvider.Name = "cmbProvider";
            cmbProvider.Size = new Size(192, 23);
            cmbProvider.TabIndex = 1;
            cmbProvider.SelectedIndexChanged += ProviderChanged;
            // 
            // lblProvider
            // 
            lblProvider.AutoSize = true;
            lblProvider.Location = new Point(13, 18);
            lblProvider.Name = "lblProvider";
            lblProvider.Size = new Size(52, 15);
            lblProvider.TabIndex = 0;
            lblProvider.Text = "Provider";
            // 
            // pageWorld
            // 
            pageWorld.Controls.Add(grpWorld);
            pageWorld.Location = new Point(4, 5);
            pageWorld.Name = "pageWorld";
            pageWorld.Padding = new Padding(10);
            pageWorld.Size = new Size(768, 337);
            pageWorld.TabIndex = 1;
            pageWorld.Text = "World";
            pageWorld.UseVisualStyleBackColor = true;
            // 
            // grpWorld
            // 
            grpWorld.Controls.Add(btnInstallWorldDb);
            grpWorld.Controls.Add(lblWorldInstallNote);
            grpWorld.Controls.Add(cmbWorldType);
            grpWorld.Controls.Add(txtWorldDb);
            grpWorld.Controls.Add(txtWorldAddress);
            grpWorld.Controls.Add(txtWorldName);
            grpWorld.Controls.Add(lblWorldType);
            grpWorld.Controls.Add(lblWorldDb);
            grpWorld.Controls.Add(lblWorldAddress);
            grpWorld.Controls.Add(lblWorldName);
            grpWorld.Location = new Point(13, 14);
            grpWorld.Name = "grpWorld";
            grpWorld.Size = new Size(742, 196);
            grpWorld.TabIndex = 0;
            grpWorld.TabStop = false;
            grpWorld.Text = "World Entry";
            // 
            // btnInstallWorldDb
            // 
            btnInstallWorldDb.Location = new Point(528, 83);
            btnInstallWorldDb.Name = "btnInstallWorldDb";
            btnInstallWorldDb.Size = new Size(184, 27);
            btnInstallWorldDb.TabIndex = 8;
            btnInstallWorldDb.Text = "Install DB (Optional)";
            btnInstallWorldDb.UseVisualStyleBackColor = true;
            btnInstallWorldDb.Click += BtnInstallWorldDb_Click;
            // 
            // lblWorldInstallNote
            // 
            lblWorldInstallNote.Location = new Point(120, 147);
            lblWorldInstallNote.Name = "lblWorldInstallNote";
            lblWorldInstallNote.Size = new Size(592, 34);
            lblWorldInstallNote.TabIndex = 9;
            lblWorldInstallNote.Text = "Optional: install the RF_World schema with a separate SQL Server connection. World registration still only stores the DB name.";
            // 
            // cmbWorldType
            // 
            cmbWorldType.DropDownStyle = ComboBoxStyle.DropDownList;
            cmbWorldType.FormattingEnabled = true;
            cmbWorldType.Items.AddRange(new object[] { "0 - Regular/Normal Server", "1 - Test Server" });
            cmbWorldType.Location = new Point(120, 115);
            cmbWorldType.Name = "cmbWorldType";
            cmbWorldType.Size = new Size(240, 23);
            cmbWorldType.TabIndex = 7;
            // 
            // txtWorldDb
            // 
            txtWorldDb.Location = new Point(120, 86);
            txtWorldDb.Name = "txtWorldDb";
            txtWorldDb.Size = new Size(392, 23);
            txtWorldDb.TabIndex = 6;
            // 
            // txtWorldAddress
            // 
            txtWorldAddress.Location = new Point(120, 57);
            txtWorldAddress.Name = "txtWorldAddress";
            txtWorldAddress.Size = new Size(240, 23);
            txtWorldAddress.TabIndex = 5;
            // 
            // txtWorldName
            // 
            txtWorldName.Location = new Point(120, 28);
            txtWorldName.Name = "txtWorldName";
            txtWorldName.Size = new Size(240, 23);
            txtWorldName.TabIndex = 4;
            // 
            // lblWorldType
            // 
            lblWorldType.AutoSize = true;
            lblWorldType.Location = new Point(16, 118);
            lblWorldType.Name = "lblWorldType";
            lblWorldType.Size = new Size(31, 15);
            lblWorldType.TabIndex = 3;
            lblWorldType.Text = "Type";
            // 
            // lblWorldDb
            // 
            lblWorldDb.AutoSize = true;
            lblWorldDb.Location = new Point(16, 89);
            lblWorldDb.Name = "lblWorldDb";
            lblWorldDb.Size = new Size(89, 15);
            lblWorldDb.TabIndex = 2;
            lblWorldDb.Text = "World DB Name";
            // 
            // lblWorldAddress
            // 
            lblWorldAddress.AutoSize = true;
            lblWorldAddress.Location = new Point(16, 60);
            lblWorldAddress.Name = "lblWorldAddress";
            lblWorldAddress.Size = new Size(49, 15);
            lblWorldAddress.TabIndex = 1;
            lblWorldAddress.Text = "Address";
            // 
            // lblWorldName
            // 
            lblWorldName.AutoSize = true;
            lblWorldName.Location = new Point(16, 31);
            lblWorldName.Name = "lblWorldName";
            lblWorldName.Size = new Size(39, 15);
            lblWorldName.TabIndex = 0;
            lblWorldName.Text = "Name";
            // 
            // pageSecurity
            // 
            pageSecurity.Controls.Add(grpSecurity);
            pageSecurity.Location = new Point(4, 5);
            pageSecurity.Name = "pageSecurity";
            pageSecurity.Padding = new Padding(10);
            pageSecurity.Size = new Size(768, 337);
            pageSecurity.TabIndex = 2;
            pageSecurity.Text = "Security";
            pageSecurity.UseVisualStyleBackColor = true;
            // 
            // grpSecurity
            // 
            grpSecurity.Controls.Add(lblSecurityNote);
            grpSecurity.Controls.Add(btnRegenerateSalt);
            grpSecurity.Controls.Add(txtArgon2Salt);
            grpSecurity.Controls.Add(lblArgon2Salt);
            grpSecurity.Location = new Point(13, 14);
            grpSecurity.Name = "grpSecurity";
            grpSecurity.Size = new Size(742, 181);
            grpSecurity.TabIndex = 0;
            grpSecurity.TabStop = false;
            grpSecurity.Text = "Security";
            // 
            // lblSecurityNote
            // 
            lblSecurityNote.Location = new Point(16, 79);
            lblSecurityNote.Name = "lblSecurityNote";
            lblSecurityNote.Size = new Size(696, 60);
            lblSecurityNote.TabIndex = 3;
            lblSecurityNote.Text = "This value is generated once during setup. AccountServer uses it for Argon2 password verification and as the seed for protected account ID storage. Changing it later will break existing account lookups.";
            // 
            // btnRegenerateSalt
            // 
            btnRegenerateSalt.Location = new Point(614, 38);
            btnRegenerateSalt.Name = "btnRegenerateSalt";
            btnRegenerateSalt.Size = new Size(98, 27);
            btnRegenerateSalt.TabIndex = 2;
            btnRegenerateSalt.Text = "Regenerate";
            btnRegenerateSalt.UseVisualStyleBackColor = true;
            btnRegenerateSalt.Click += BtnRegenerateSalt_Click;
            // 
            // txtArgon2Salt
            // 
            txtArgon2Salt.Location = new Point(16, 40);
            txtArgon2Salt.Name = "txtArgon2Salt";
            txtArgon2Salt.ReadOnly = true;
            txtArgon2Salt.Size = new Size(592, 23);
            txtArgon2Salt.TabIndex = 1;
            // 
            // lblArgon2Salt
            // 
            lblArgon2Salt.AutoSize = true;
            lblArgon2Salt.Location = new Point(16, 22);
            lblArgon2Salt.Name = "lblArgon2Salt";
            lblArgon2Salt.Size = new Size(115, 15);
            lblArgon2Salt.TabIndex = 0;
            lblArgon2Salt.Text = "Argon2 Salt (Base64)";
            // 
            // panelButtons
            // 
            panelButtons.Controls.Add(btnFinish);
            panelButtons.Controls.Add(btnNext);
            panelButtons.Controls.Add(btnBack);
            panelButtons.Controls.Add(btnCancel);
            panelButtons.Dock = DockStyle.Bottom;
            panelButtons.FlowDirection = FlowDirection.RightToLeft;
            panelButtons.Location = new Point(0, 452);
            panelButtons.Name = "panelButtons";
            panelButtons.Padding = new Padding(10);
            panelButtons.Size = new Size(800, 48);
            panelButtons.TabIndex = 4;
            // 
            // btnFinish
            // 
            btnFinish.AutoSize = true;
            btnFinish.Location = new Point(715, 13);
            btnFinish.Name = "btnFinish";
            btnFinish.Size = new Size(62, 25);
            btnFinish.TabIndex = 0;
            btnFinish.Text = "Finish";
            btnFinish.UseVisualStyleBackColor = true;
            btnFinish.Visible = false;
            btnFinish.Click += BtnFinish_Click;
            // 
            // btnNext
            // 
            btnNext.AutoSize = true;
            btnNext.Location = new Point(649, 13);
            btnNext.Name = "btnNext";
            btnNext.Size = new Size(60, 25);
            btnNext.TabIndex = 1;
            btnNext.Enabled = false;
            btnNext.Text = "Next >";
            btnNext.UseVisualStyleBackColor = true;
            btnNext.Click += BtnNext_Click;
            // 
            // btnBack
            // 
            btnBack.AutoSize = true;
            btnBack.Location = new Point(579, 13);
            btnBack.Name = "btnBack";
            btnBack.Size = new Size(64, 25);
            btnBack.TabIndex = 2;
            btnBack.Text = "< Back";
            btnBack.UseVisualStyleBackColor = true;
            btnBack.Click += BtnBack_Click;
            // 
            // btnCancel
            // 
            btnCancel.AutoSize = true;
            btnCancel.DialogResult = DialogResult.Cancel;
            btnCancel.Location = new Point(498, 13);
            btnCancel.Name = "btnCancel";
            btnCancel.Size = new Size(75, 25);
            btnCancel.TabIndex = 3;
            btnCancel.Text = "Cancel";
            btnCancel.UseVisualStyleBackColor = true;
            // 
            // Setup
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            CancelButton = btnCancel;
            ClientSize = new Size(800, 500);
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
            Text = "Account Server Setup";
            pageSetup.ResumeLayout(false);
            pageDatabase.ResumeLayout(false);
            pageDatabase.PerformLayout();
            grpUserDb.ResumeLayout(false);
            grpUserDb.PerformLayout();
            ((ISupportInitialize)numUserPort).EndInit();
            pageWorld.ResumeLayout(false);
            grpWorld.ResumeLayout(false);
            grpWorld.PerformLayout();
            pageSecurity.ResumeLayout(false);
            grpSecurity.ResumeLayout(false);
            grpSecurity.PerformLayout();
            panelButtons.ResumeLayout(false);
            panelButtons.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }
    }
}
