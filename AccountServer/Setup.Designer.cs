namespace AccountServer
{
    partial class Setup
    {
        private System.ComponentModel.IContainer components = null;

        private System.Windows.Forms.TabControl tabSetup;
        private System.Windows.Forms.TabPage tabDatabase;
        private System.Windows.Forms.TabPage tabWorld;
        private System.Windows.Forms.ComboBox cmbProvider;
        private System.Windows.Forms.Label lblProvider;
        private System.Windows.Forms.GroupBox grpUserDb;
        private System.Windows.Forms.Label lblUserHost;
        private System.Windows.Forms.Label lblUserPort;
        private System.Windows.Forms.Label lblUserDb;
        private System.Windows.Forms.Label lblUserUser;
        private System.Windows.Forms.Label lblUserPass;
        private System.Windows.Forms.TextBox txtUserHost;
        private System.Windows.Forms.NumericUpDown numUserPort;
        private System.Windows.Forms.TextBox txtUserDb;
        private System.Windows.Forms.TextBox txtUserUser;
        private System.Windows.Forms.TextBox txtUserPass;
        private System.Windows.Forms.CheckBox chkUserTrusted;
        private System.Windows.Forms.Button btnInstallUserDb;
        private System.Windows.Forms.GroupBox grpWorld;
        private System.Windows.Forms.Label lblWorldName;
        private System.Windows.Forms.Label lblWorldAddress;
        private System.Windows.Forms.Label lblWorldDb;
        private System.Windows.Forms.Label lblWorldType;
        private System.Windows.Forms.TextBox txtWorldName;
        private System.Windows.Forms.TextBox txtWorldAddress;
        private System.Windows.Forms.TextBox txtWorldDb;
        private System.Windows.Forms.NumericUpDown numWorldType;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnCancel;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent()
        {
            this.tabSetup = new System.Windows.Forms.TabControl();
            this.tabDatabase = new System.Windows.Forms.TabPage();
            this.btnInstallUserDb = new System.Windows.Forms.Button();
            this.grpUserDb = new System.Windows.Forms.GroupBox();
            this.chkUserTrusted = new System.Windows.Forms.CheckBox();
            this.txtUserPass = new System.Windows.Forms.TextBox();
            this.txtUserUser = new System.Windows.Forms.TextBox();
            this.txtUserDb = new System.Windows.Forms.TextBox();
            this.numUserPort = new System.Windows.Forms.NumericUpDown();
            this.txtUserHost = new System.Windows.Forms.TextBox();
            this.lblUserPass = new System.Windows.Forms.Label();
            this.lblUserUser = new System.Windows.Forms.Label();
            this.lblUserDb = new System.Windows.Forms.Label();
            this.lblUserPort = new System.Windows.Forms.Label();
            this.lblUserHost = new System.Windows.Forms.Label();
            this.cmbProvider = new System.Windows.Forms.ComboBox();
            this.lblProvider = new System.Windows.Forms.Label();
            this.tabWorld = new System.Windows.Forms.TabPage();
            this.grpWorld = new System.Windows.Forms.GroupBox();
            this.numWorldType = new System.Windows.Forms.NumericUpDown();
            this.txtWorldDb = new System.Windows.Forms.TextBox();
            this.txtWorldAddress = new System.Windows.Forms.TextBox();
            this.txtWorldName = new System.Windows.Forms.TextBox();
            this.lblWorldType = new System.Windows.Forms.Label();
            this.lblWorldDb = new System.Windows.Forms.Label();
            this.lblWorldAddress = new System.Windows.Forms.Label();
            this.lblWorldName = new System.Windows.Forms.Label();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.tabSetup.SuspendLayout();
            this.tabDatabase.SuspendLayout();
            this.grpUserDb.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numUserPort)).BeginInit();
            this.tabWorld.SuspendLayout();
            this.grpWorld.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numWorldType)).BeginInit();
            this.SuspendLayout();
            // 
            // tabSetup
            // 
            this.tabSetup.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabSetup.Controls.Add(this.tabDatabase);
            this.tabSetup.Controls.Add(this.tabWorld);
            this.tabSetup.Location = new System.Drawing.Point(12, 12);
            this.tabSetup.Name = "tabSetup";
            this.tabSetup.SelectedIndex = 0;
            this.tabSetup.Size = new System.Drawing.Size(776, 388);
            this.tabSetup.TabIndex = 0;
            // 
            // tabDatabase
            // 
            this.tabDatabase.Controls.Add(this.btnInstallUserDb);
            this.tabDatabase.Controls.Add(this.grpUserDb);
            this.tabDatabase.Controls.Add(this.cmbProvider);
            this.tabDatabase.Controls.Add(this.lblProvider);
            this.tabDatabase.Location = new System.Drawing.Point(4, 24);
            this.tabDatabase.Name = "tabDatabase";
            this.tabDatabase.Padding = new System.Windows.Forms.Padding(10);
            this.tabDatabase.Size = new System.Drawing.Size(768, 360);
            this.tabDatabase.TabIndex = 0;
            this.tabDatabase.Text = "Database";
            this.tabDatabase.UseVisualStyleBackColor = true;
            // 
            // btnInstallUserDb
            // 
            this.btnInstallUserDb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnInstallUserDb.Location = new System.Drawing.Point(597, 321);
            this.btnInstallUserDb.Name = "btnInstallUserDb";
            this.btnInstallUserDb.Size = new System.Drawing.Size(158, 27);
            this.btnInstallUserDb.TabIndex = 4;
            this.btnInstallUserDb.Text = "Install User DB";
            this.btnInstallUserDb.UseVisualStyleBackColor = true;
            this.btnInstallUserDb.Click += new System.EventHandler(this.BtnInstallUserDb_Click);
            // 
            // grpUserDb
            // 
            this.grpUserDb.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right));
            this.grpUserDb.Controls.Add(this.chkUserTrusted);
            this.grpUserDb.Controls.Add(this.txtUserPass);
            this.grpUserDb.Controls.Add(this.txtUserUser);
            this.grpUserDb.Controls.Add(this.txtUserDb);
            this.grpUserDb.Controls.Add(this.numUserPort);
            this.grpUserDb.Controls.Add(this.txtUserHost);
            this.grpUserDb.Controls.Add(this.lblUserPass);
            this.grpUserDb.Controls.Add(this.lblUserUser);
            this.grpUserDb.Controls.Add(this.lblUserDb);
            this.grpUserDb.Controls.Add(this.lblUserPort);
            this.grpUserDb.Controls.Add(this.lblUserHost);
            this.grpUserDb.Location = new System.Drawing.Point(13, 49);
            this.grpUserDb.Name = "grpUserDb";
            this.grpUserDb.Size = new System.Drawing.Size(742, 260);
            this.grpUserDb.TabIndex = 2;
            this.grpUserDb.TabStop = false;
            this.grpUserDb.Text = "User DB";
            // 
            // chkUserTrusted
            // 
            this.chkUserTrusted.AutoSize = true;
            this.chkUserTrusted.Location = new System.Drawing.Point(120, 161);
            this.chkUserTrusted.Name = "chkUserTrusted";
            this.chkUserTrusted.Size = new System.Drawing.Size(124, 19);
            this.chkUserTrusted.TabIndex = 10;
            this.chkUserTrusted.Text = "Trusted Connection";
            this.chkUserTrusted.UseVisualStyleBackColor = true;
            this.chkUserTrusted.CheckedChanged += new System.EventHandler(this.TrustedChanged);
            // 
            // txtUserPass
            // 
            this.txtUserPass.Location = new System.Drawing.Point(120, 219);
            this.txtUserPass.Name = "txtUserPass";
            this.txtUserPass.PasswordChar = '*';
            this.txtUserPass.Size = new System.Drawing.Size(220, 23);
            this.txtUserPass.TabIndex = 9;
            // 
            // txtUserUser
            // 
            this.txtUserUser.Location = new System.Drawing.Point(120, 189);
            this.txtUserUser.Name = "txtUserUser";
            this.txtUserUser.Size = new System.Drawing.Size(220, 23);
            this.txtUserUser.TabIndex = 8;
            // 
            // txtUserDb
            // 
            this.txtUserDb.Location = new System.Drawing.Point(120, 129);
            this.txtUserDb.Name = "txtUserDb";
            this.txtUserDb.Size = new System.Drawing.Size(220, 23);
            this.txtUserDb.TabIndex = 7;
            // 
            // numUserPort
            // 
            this.numUserPort.Location = new System.Drawing.Point(120, 98);
            this.numUserPort.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.numUserPort.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numUserPort.Name = "numUserPort";
            this.numUserPort.Size = new System.Drawing.Size(120, 23);
            this.numUserPort.TabIndex = 6;
            this.numUserPort.Value = new decimal(new int[] {
            1433,
            0,
            0,
            0});
            // 
            // txtUserHost
            // 
            this.txtUserHost.Location = new System.Drawing.Point(120, 69);
            this.txtUserHost.Name = "txtUserHost";
            this.txtUserHost.Size = new System.Drawing.Size(220, 23);
            this.txtUserHost.TabIndex = 5;
            // 
            // lblUserPass
            // 
            this.lblUserPass.AutoSize = true;
            this.lblUserPass.Location = new System.Drawing.Point(16, 222);
            this.lblUserPass.Name = "lblUserPass";
            this.lblUserPass.Size = new System.Drawing.Size(57, 15);
            this.lblUserPass.TabIndex = 4;
            this.lblUserPass.Text = "Password";
            // 
            // lblUserUser
            // 
            this.lblUserUser.AutoSize = true;
            this.lblUserUser.Location = new System.Drawing.Point(16, 192);
            this.lblUserUser.Name = "lblUserUser";
            this.lblUserUser.Size = new System.Drawing.Size(30, 15);
            this.lblUserUser.TabIndex = 3;
            this.lblUserUser.Text = "User";
            // 
            // lblUserDb
            // 
            this.lblUserDb.AutoSize = true;
            this.lblUserDb.Location = new System.Drawing.Point(16, 132);
            this.lblUserDb.Name = "lblUserDb";
            this.lblUserDb.Size = new System.Drawing.Size(55, 15);
            this.lblUserDb.TabIndex = 2;
            this.lblUserDb.Text = "Database";
            // 
            // lblUserPort
            // 
            this.lblUserPort.AutoSize = true;
            this.lblUserPort.Location = new System.Drawing.Point(16, 100);
            this.lblUserPort.Name = "lblUserPort";
            this.lblUserPort.Size = new System.Drawing.Size(29, 15);
            this.lblUserPort.TabIndex = 1;
            this.lblUserPort.Text = "Port";
            // 
            // lblUserHost
            // 
            this.lblUserHost.AutoSize = true;
            this.lblUserHost.Location = new System.Drawing.Point(16, 72);
            this.lblUserHost.Name = "lblUserHost";
            this.lblUserHost.Size = new System.Drawing.Size(32, 15);
            this.lblUserHost.TabIndex = 0;
            this.lblUserHost.Text = "Host";
            // 
            // cmbProvider
            // 
            this.cmbProvider.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProvider.FormattingEnabled = true;
            this.cmbProvider.Items.AddRange(new object[] {
            "SQL Server",
            "MariaDB",
            "SQLite"});
            this.cmbProvider.Location = new System.Drawing.Point(78, 15);
            this.cmbProvider.Name = "cmbProvider";
            this.cmbProvider.Size = new System.Drawing.Size(192, 23);
            this.cmbProvider.TabIndex = 1;
            this.cmbProvider.SelectedIndexChanged += new System.EventHandler(this.ProviderChanged);
            // 
            // lblProvider
            // 
            this.lblProvider.AutoSize = true;
            this.lblProvider.Location = new System.Drawing.Point(13, 18);
            this.lblProvider.Name = "lblProvider";
            this.lblProvider.Size = new System.Drawing.Size(52, 15);
            this.lblProvider.TabIndex = 0;
            this.lblProvider.Text = "Provider";
            // 
            // tabWorld
            // 
            this.tabWorld.Controls.Add(this.grpWorld);
            this.tabWorld.Location = new System.Drawing.Point(4, 24);
            this.tabWorld.Name = "tabWorld";
            this.tabWorld.Padding = new System.Windows.Forms.Padding(10);
            this.tabWorld.Size = new System.Drawing.Size(768, 360);
            this.tabWorld.TabIndex = 1;
            this.tabWorld.Text = "World";
            this.tabWorld.UseVisualStyleBackColor = true;
            // 
            // grpWorld
            // 
            this.grpWorld.Controls.Add(this.numWorldType);
            this.grpWorld.Controls.Add(this.txtWorldDb);
            this.grpWorld.Controls.Add(this.txtWorldAddress);
            this.grpWorld.Controls.Add(this.txtWorldName);
            this.grpWorld.Controls.Add(this.lblWorldType);
            this.grpWorld.Controls.Add(this.lblWorldDb);
            this.grpWorld.Controls.Add(this.lblWorldAddress);
            this.grpWorld.Controls.Add(this.lblWorldName);
            this.grpWorld.Location = new System.Drawing.Point(13, 14);
            this.grpWorld.Name = "grpWorld";
            this.grpWorld.Size = new System.Drawing.Size(742, 158);
            this.grpWorld.TabIndex = 0;
            this.grpWorld.TabStop = false;
            this.grpWorld.Text = "World Entry";
            // 
            // numWorldType
            // 
            this.numWorldType.Location = new System.Drawing.Point(120, 115);
            this.numWorldType.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numWorldType.Name = "numWorldType";
            this.numWorldType.Size = new System.Drawing.Size(120, 23);
            this.numWorldType.TabIndex = 7;
            // 
            // txtWorldDb
            // 
            this.txtWorldDb.Location = new System.Drawing.Point(120, 86);
            this.txtWorldDb.Name = "txtWorldDb";
            this.txtWorldDb.Size = new System.Drawing.Size(240, 23);
            this.txtWorldDb.TabIndex = 6;
            // 
            // txtWorldAddress
            // 
            this.txtWorldAddress.Location = new System.Drawing.Point(120, 57);
            this.txtWorldAddress.Name = "txtWorldAddress";
            this.txtWorldAddress.Size = new System.Drawing.Size(240, 23);
            this.txtWorldAddress.TabIndex = 5;
            // 
            // txtWorldName
            // 
            this.txtWorldName.Location = new System.Drawing.Point(120, 28);
            this.txtWorldName.Name = "txtWorldName";
            this.txtWorldName.Size = new System.Drawing.Size(240, 23);
            this.txtWorldName.TabIndex = 4;
            // 
            // lblWorldType
            // 
            this.lblWorldType.AutoSize = true;
            this.lblWorldType.Location = new System.Drawing.Point(16, 118);
            this.lblWorldType.Name = "lblWorldType";
            this.lblWorldType.Size = new System.Drawing.Size(31, 15);
            this.lblWorldType.TabIndex = 3;
            this.lblWorldType.Text = "Type";
            // 
            // lblWorldDb
            // 
            this.lblWorldDb.AutoSize = true;
            this.lblWorldDb.Location = new System.Drawing.Point(16, 89);
            this.lblWorldDb.Name = "lblWorldDb";
            this.lblWorldDb.Size = new System.Drawing.Size(89, 15);
            this.lblWorldDb.TabIndex = 2;
            this.lblWorldDb.Text = "World DB Name";
            // 
            // lblWorldAddress
            // 
            this.lblWorldAddress.AutoSize = true;
            this.lblWorldAddress.Location = new System.Drawing.Point(16, 60);
            this.lblWorldAddress.Name = "lblWorldAddress";
            this.lblWorldAddress.Size = new System.Drawing.Size(49, 15);
            this.lblWorldAddress.TabIndex = 1;
            this.lblWorldAddress.Text = "Address";
            // 
            // lblWorldName
            // 
            this.lblWorldName.AutoSize = true;
            this.lblWorldName.Location = new System.Drawing.Point(16, 31);
            this.lblWorldName.Name = "lblWorldName";
            this.lblWorldName.Size = new System.Drawing.Size(39, 15);
            this.lblWorldName.TabIndex = 0;
            this.lblWorldName.Text = "Name";
            // 
            // btnSave
            // 
            this.btnSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSave.Location = new System.Drawing.Point(632, 410);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 27);
            this.btnSave.TabIndex = 1;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.BtnSave_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(713, 410);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 27);
            this.btnCancel.TabIndex = 2;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // Setup
            // 
            this.AcceptButton = this.btnSave;
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.tabSetup);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Setup";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Account Server Setup";
            this.tabSetup.ResumeLayout(false);
            this.tabDatabase.ResumeLayout(false);
            this.tabDatabase.PerformLayout();
            this.grpUserDb.ResumeLayout(false);
            this.grpUserDb.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numUserPort)).EndInit();
            this.tabWorld.ResumeLayout(false);
            this.grpWorld.ResumeLayout(false);
            this.grpWorld.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numWorldType)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
    }
}
