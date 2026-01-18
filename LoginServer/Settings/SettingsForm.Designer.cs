namespace LoginServer.Settings;

partial class SettingsForm
{
    private System.ComponentModel.IContainer components = null;

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
        tabMain = new System.Windows.Forms.TabControl();
        tabDatabase = new System.Windows.Forms.TabPage();
        grpAuth = new System.Windows.Forms.GroupBox();
        radAuthTrusted = new System.Windows.Forms.RadioButton();
        radAuthSql = new System.Windows.Forms.RadioButton();
        lblDbPass = new System.Windows.Forms.Label();
        txtDbPass = new System.Windows.Forms.TextBox();
        lblDbUser = new System.Windows.Forms.Label();
        txtDbUser = new System.Windows.Forms.TextBox();
        lblDbName = new System.Windows.Forms.Label();
        txtDbName = new System.Windows.Forms.TextBox();
        lblArgon2Salt = new System.Windows.Forms.Label();
        txtArgon2Salt = new System.Windows.Forms.TextBox();
        lblDbPort = new System.Windows.Forms.Label();
        txtDbPort = new System.Windows.Forms.TextBox();
        lblDbHost = new System.Windows.Forms.Label();
        txtDbHost = new System.Windows.Forms.TextBox();
        lblDbProfile = new System.Windows.Forms.Label();
        cboDbProfile = new System.Windows.Forms.ComboBox();
        tabConnection = new System.Windows.Forms.TabPage();
        lblLoadHighSep = new System.Windows.Forms.Label();
        txtLoadHigh = new System.Windows.Forms.TextBox();
        lblLoadMidSep = new System.Windows.Forms.Label();
        txtLoadMid = new System.Windows.Forms.TextBox();
        txtLoadLow = new System.Windows.Forms.TextBox();
        lblLoadLow = new System.Windows.Forms.Label();
        txtMaxConn = new System.Windows.Forms.TextBox();
        lblMaxConn = new System.Windows.Forms.Label();
        txtAccountPort = new System.Windows.Forms.TextBox();
        lblAccountPort = new System.Windows.Forms.Label();
        txtAccountHost = new System.Windows.Forms.TextBox();
        lblAccountHost = new System.Windows.Forms.Label();
        txtClientPort = new System.Windows.Forms.TextBox();
        lblClientPort = new System.Windows.Forms.Label();
        btnSave = new System.Windows.Forms.Button();
        btnCancel = new System.Windows.Forms.Button();
        tabMain.SuspendLayout();
        tabDatabase.SuspendLayout();
        grpAuth.SuspendLayout();
        tabConnection.SuspendLayout();
        SuspendLayout();
        // 
        // tabMain
        // 
        tabMain.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
        tabMain.Controls.Add(tabDatabase);
        tabMain.Controls.Add(tabConnection);
        tabMain.Location = new System.Drawing.Point(10, 10);
        tabMain.Margin = new System.Windows.Forms.Padding(2);
        tabMain.Name = "tabMain";
        tabMain.SelectedIndex = 0;
        tabMain.Size = new System.Drawing.Size(520, 260);
        tabMain.TabIndex = 0;
        // 
        // tabDatabase
        // 
        tabDatabase.Controls.Add(grpAuth);
        tabDatabase.Controls.Add(lblDbPass);
        tabDatabase.Controls.Add(txtDbPass);
        tabDatabase.Controls.Add(lblDbUser);
        tabDatabase.Controls.Add(txtDbUser);
        tabDatabase.Controls.Add(lblDbName);
        tabDatabase.Controls.Add(txtDbName);
        tabDatabase.Controls.Add(lblArgon2Salt);
        tabDatabase.Controls.Add(txtArgon2Salt);
        tabDatabase.Controls.Add(lblDbPort);
        tabDatabase.Controls.Add(txtDbPort);
        tabDatabase.Controls.Add(lblDbHost);
        tabDatabase.Controls.Add(txtDbHost);
        tabDatabase.Controls.Add(lblDbProfile);
        tabDatabase.Controls.Add(cboDbProfile);
        tabDatabase.Location = new System.Drawing.Point(4, 24);
        tabDatabase.Margin = new System.Windows.Forms.Padding(2);
        tabDatabase.Name = "tabDatabase";
        tabDatabase.Padding = new System.Windows.Forms.Padding(8);
        tabDatabase.Size = new System.Drawing.Size(512, 232);
        tabDatabase.TabIndex = 0;
        tabDatabase.Text = "Database";
        tabDatabase.UseVisualStyleBackColor = true;
        // 
        // grpAuth
        // 
        grpAuth.Controls.Add(radAuthTrusted);
        grpAuth.Controls.Add(radAuthSql);
        grpAuth.Location = new System.Drawing.Point(11, 140);
        grpAuth.Margin = new System.Windows.Forms.Padding(2);
        grpAuth.Name = "grpAuth";
        grpAuth.Padding = new System.Windows.Forms.Padding(8);
        grpAuth.Size = new System.Drawing.Size(182, 78);
        grpAuth.TabIndex = 12;
        grpAuth.TabStop = false;
        grpAuth.Text = "Authentication";
        // 
        // radAuthTrusted
        // 
        radAuthTrusted.AutoSize = true;
        radAuthTrusted.Location = new System.Drawing.Point(11, 49);
        radAuthTrusted.Margin = new System.Windows.Forms.Padding(2);
        radAuthTrusted.Name = "radAuthTrusted";
        radAuthTrusted.Size = new System.Drawing.Size(118, 19);
        radAuthTrusted.TabIndex = 1;
        radAuthTrusted.TabStop = true;
        radAuthTrusted.Text = "Trusted (Windows)";
        radAuthTrusted.UseVisualStyleBackColor = true;
        radAuthTrusted.CheckedChanged += OnAuthModeChanged;
        // 
        // radAuthSql
        // 
        radAuthSql.AutoSize = true;
        radAuthSql.Location = new System.Drawing.Point(11, 23);
        radAuthSql.Margin = new System.Windows.Forms.Padding(2);
        radAuthSql.Name = "radAuthSql";
        radAuthSql.Size = new System.Drawing.Size(99, 19);
        radAuthSql.TabIndex = 0;
        radAuthSql.TabStop = true;
        radAuthSql.Text = "SQL username";
        radAuthSql.UseVisualStyleBackColor = true;
        radAuthSql.CheckedChanged += OnAuthModeChanged;
        // 
        // lblDbPass
        // 
        lblDbPass.AutoSize = true;
        lblDbPass.Location = new System.Drawing.Point(221, 113);
        lblDbPass.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblDbPass.Name = "lblDbPass";
        lblDbPass.Size = new System.Drawing.Size(63, 15);
        lblDbPass.TabIndex = 11;
        lblDbPass.Text = "Password";
        // 
        // txtDbPass
        // 
        txtDbPass.Location = new System.Drawing.Point(298, 110);
        txtDbPass.Margin = new System.Windows.Forms.Padding(2);
        txtDbPass.Name = "txtDbPass";
        txtDbPass.Size = new System.Drawing.Size(191, 23);
        txtDbPass.TabIndex = 10;
        txtDbPass.UseSystemPasswordChar = true;
        // 
        // lblDbUser
        // 
        lblDbUser.AutoSize = true;
        lblDbUser.Location = new System.Drawing.Point(221, 83);
        lblDbUser.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblDbUser.Name = "lblDbUser";
        lblDbUser.Size = new System.Drawing.Size(66, 15);
        lblDbUser.TabIndex = 9;
        lblDbUser.Text = "User name";
        // 
        // txtDbUser
        // 
        txtDbUser.Location = new System.Drawing.Point(298, 80);
        txtDbUser.Margin = new System.Windows.Forms.Padding(2);
        txtDbUser.Name = "txtDbUser";
        txtDbUser.Size = new System.Drawing.Size(191, 23);
        txtDbUser.TabIndex = 8;
        // 
        // lblDbName
        // 
        lblDbName.AutoSize = true;
        lblDbName.Location = new System.Drawing.Point(8, 113);
        lblDbName.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblDbName.Name = "lblDbName";
        lblDbName.Size = new System.Drawing.Size(77, 15);
        lblDbName.TabIndex = 7;
        lblDbName.Text = "Database DB";
        // 
        // txtDbName
        // 
        txtDbName.Location = new System.Drawing.Point(93, 110);
        txtDbName.Margin = new System.Windows.Forms.Padding(2);
        txtDbName.Name = "txtDbName";
        txtDbName.Size = new System.Drawing.Size(110, 23);
        txtDbName.TabIndex = 6;
        // 
        // lblArgon2Salt
        // 
        lblArgon2Salt.AutoSize = true;
        lblArgon2Salt.Location = new System.Drawing.Point(221, 142);
        lblArgon2Salt.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblArgon2Salt.Name = "lblArgon2Salt";
        lblArgon2Salt.Size = new System.Drawing.Size(115, 15);
        lblArgon2Salt.TabIndex = 14;
        lblArgon2Salt.Text = "Argon2 Salt (Base64)";
        // 
        // txtArgon2Salt
        // 
        txtArgon2Salt.Location = new System.Drawing.Point(221, 160);
        txtArgon2Salt.Margin = new System.Windows.Forms.Padding(2);
        txtArgon2Salt.Name = "txtArgon2Salt";
        txtArgon2Salt.Size = new System.Drawing.Size(268, 23);
        txtArgon2Salt.TabIndex = 13;
        // 
        // lblDbPort
        // 
        lblDbPort.AutoSize = true;
        lblDbPort.Location = new System.Drawing.Point(221, 54);
        lblDbPort.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblDbPort.Name = "lblDbPort";
        lblDbPort.Size = new System.Drawing.Size(29, 15);
        lblDbPort.TabIndex = 5;
        lblDbPort.Text = "Port";
        // 
        // txtDbPort
        // 
        txtDbPort.Location = new System.Drawing.Point(298, 51);
        txtDbPort.Margin = new System.Windows.Forms.Padding(2);
        txtDbPort.Name = "txtDbPort";
        txtDbPort.Size = new System.Drawing.Size(74, 23);
        txtDbPort.TabIndex = 4;
        // 
        // lblDbHost
        // 
        lblDbHost.AutoSize = true;
        lblDbHost.Location = new System.Drawing.Point(8, 54);
        lblDbHost.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblDbHost.Name = "lblDbHost";
        lblDbHost.Size = new System.Drawing.Size(64, 15);
        lblDbHost.TabIndex = 3;
        lblDbHost.Text = "DB Server";
        // 
        // txtDbHost
        // 
        txtDbHost.Location = new System.Drawing.Point(93, 51);
        txtDbHost.Margin = new System.Windows.Forms.Padding(2);
        txtDbHost.Name = "txtDbHost";
        txtDbHost.Size = new System.Drawing.Size(110, 23);
        txtDbHost.TabIndex = 2;
        // 
        // lblDbProfile
        // 
        lblDbProfile.AutoSize = true;
        lblDbProfile.Location = new System.Drawing.Point(8, 19);
        lblDbProfile.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblDbProfile.Name = "lblDbProfile";
        lblDbProfile.Size = new System.Drawing.Size(42, 15);
        lblDbProfile.TabIndex = 1;
        lblDbProfile.Text = "Profile";
        // 
        // cboDbProfile
        // 
        cboDbProfile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
        cboDbProfile.FormattingEnabled = true;
        cboDbProfile.Location = new System.Drawing.Point(93, 16);
        cboDbProfile.Margin = new System.Windows.Forms.Padding(2);
        cboDbProfile.Name = "cboDbProfile";
        cboDbProfile.Size = new System.Drawing.Size(150, 23);
        cboDbProfile.TabIndex = 0;
        cboDbProfile.SelectedIndexChanged += OnDbProfileChanged;
        // 
        // tabConnection
        // 
        tabConnection.Controls.Add(lblLoadHighSep);
        tabConnection.Controls.Add(txtLoadHigh);
        tabConnection.Controls.Add(lblLoadMidSep);
        tabConnection.Controls.Add(txtLoadMid);
        tabConnection.Controls.Add(txtLoadLow);
        tabConnection.Controls.Add(lblLoadLow);
        tabConnection.Controls.Add(txtMaxConn);
        tabConnection.Controls.Add(lblMaxConn);
        tabConnection.Controls.Add(txtAccountPort);
        tabConnection.Controls.Add(lblAccountPort);
        tabConnection.Controls.Add(txtAccountHost);
        tabConnection.Controls.Add(lblAccountHost);
        tabConnection.Controls.Add(txtClientPort);
        tabConnection.Controls.Add(lblClientPort);
        tabConnection.Location = new System.Drawing.Point(4, 24);
        tabConnection.Margin = new System.Windows.Forms.Padding(2);
        tabConnection.Name = "tabConnection";
        tabConnection.Padding = new System.Windows.Forms.Padding(8);
        tabConnection.Size = new System.Drawing.Size(512, 232);
        tabConnection.TabIndex = 1;
        tabConnection.Text = "Connection";
        tabConnection.UseVisualStyleBackColor = true;
        // 
        // lblLoadHighSep
        // 
        lblLoadHighSep.AutoSize = true;
        lblLoadHighSep.Location = new System.Drawing.Point(266, 151);
        lblLoadHighSep.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblLoadHighSep.Name = "lblLoadHighSep";
        lblLoadHighSep.Size = new System.Drawing.Size(12, 15);
        lblLoadHighSep.TabIndex = 13;
        lblLoadHighSep.Text = "/";
        // 
        // txtLoadHigh
        // 
        txtLoadHigh.Location = new System.Drawing.Point(282, 147);
        txtLoadHigh.Margin = new System.Windows.Forms.Padding(2);
        txtLoadHigh.Name = "txtLoadHigh";
        txtLoadHigh.Size = new System.Drawing.Size(60, 23);
        txtLoadHigh.TabIndex = 6;
        // 
        // lblLoadMidSep
        // 
        lblLoadMidSep.AutoSize = true;
        lblLoadMidSep.Location = new System.Drawing.Point(190, 151);
        lblLoadMidSep.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblLoadMidSep.Name = "lblLoadMidSep";
        lblLoadMidSep.Size = new System.Drawing.Size(12, 15);
        lblLoadMidSep.TabIndex = 11;
        lblLoadMidSep.Text = "/";
        // 
        // txtLoadMid
        // 
        txtLoadMid.Location = new System.Drawing.Point(206, 147);
        txtLoadMid.Margin = new System.Windows.Forms.Padding(2);
        txtLoadMid.Name = "txtLoadMid";
        txtLoadMid.Size = new System.Drawing.Size(60, 23);
        txtLoadMid.TabIndex = 5;
        // 
        // txtLoadLow
        // 
        txtLoadLow.Location = new System.Drawing.Point(124, 147);
        txtLoadLow.Margin = new System.Windows.Forms.Padding(2);
        txtLoadLow.Name = "txtLoadLow";
        txtLoadLow.Size = new System.Drawing.Size(60, 23);
        txtLoadLow.TabIndex = 4;
        // 
        // lblLoadLow
        // 
        lblLoadLow.AutoSize = true;
        lblLoadLow.Location = new System.Drawing.Point(10, 151);
        lblLoadLow.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblLoadLow.Name = "lblLoadLow";
        lblLoadLow.Size = new System.Drawing.Size(110, 15);
        lblLoadLow.TabIndex = 9;
        lblLoadLow.Text = "User load low/med/high";
        // 
        // txtMaxConn
        // 
        txtMaxConn.Location = new System.Drawing.Point(124, 105);
        txtMaxConn.Margin = new System.Windows.Forms.Padding(2);
        txtMaxConn.Name = "txtMaxConn";
        txtMaxConn.Size = new System.Drawing.Size(85, 23);
        txtMaxConn.TabIndex = 3;
        // 
        // lblMaxConn
        // 
        lblMaxConn.AutoSize = true;
        lblMaxConn.Location = new System.Drawing.Point(10, 108);
        lblMaxConn.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblMaxConn.Name = "lblMaxConn";
        lblMaxConn.Size = new System.Drawing.Size(99, 15);
        lblMaxConn.TabIndex = 7;
        lblMaxConn.Text = "Max connections";
        // 
        // txtAccountPort
        // 
        txtAccountPort.Location = new System.Drawing.Point(294, 64);
        txtAccountPort.Margin = new System.Windows.Forms.Padding(2);
        txtAccountPort.Name = "txtAccountPort";
        txtAccountPort.Size = new System.Drawing.Size(74, 23);
        txtAccountPort.TabIndex = 2;
        // 
        // lblAccountPort
        // 
        lblAccountPort.AutoSize = true;
        lblAccountPort.Location = new System.Drawing.Point(259, 67);
        lblAccountPort.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblAccountPort.Name = "lblAccountPort";
        lblAccountPort.Size = new System.Drawing.Size(29, 15);
        lblAccountPort.TabIndex = 5;
        lblAccountPort.Text = "Port";
        // 
        // txtAccountHost
        // 
        txtAccountHost.Location = new System.Drawing.Point(124, 64);
        txtAccountHost.Margin = new System.Windows.Forms.Padding(2);
        txtAccountHost.Name = "txtAccountHost";
        txtAccountHost.Size = new System.Drawing.Size(121, 23);
        txtAccountHost.TabIndex = 1;
        // 
        // lblAccountHost
        // 
        lblAccountHost.AutoSize = true;
        lblAccountHost.Location = new System.Drawing.Point(10, 67);
        lblAccountHost.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblAccountHost.Name = "lblAccountHost";
        lblAccountHost.Size = new System.Drawing.Size(80, 15);
        lblAccountHost.TabIndex = 3;
        lblAccountHost.Text = "Account host";
        // 
        // txtClientPort
        // 
        txtClientPort.Location = new System.Drawing.Point(124, 23);
        txtClientPort.Margin = new System.Windows.Forms.Padding(2);
        txtClientPort.Name = "txtClientPort";
        txtClientPort.Size = new System.Drawing.Size(85, 23);
        txtClientPort.TabIndex = 0;
        // 
        // lblClientPort
        // 
        lblClientPort.AutoSize = true;
        lblClientPort.Location = new System.Drawing.Point(10, 26);
        lblClientPort.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        lblClientPort.Name = "lblClientPort";
        lblClientPort.Size = new System.Drawing.Size(63, 15);
        lblClientPort.TabIndex = 1;
        lblClientPort.Text = "Client port";
        // 
        // btnSave
        // 
        btnSave.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right;
        btnSave.Location = new System.Drawing.Point(442, 278);
        btnSave.Margin = new System.Windows.Forms.Padding(2);
        btnSave.Name = "btnSave";
        btnSave.Size = new System.Drawing.Size(88, 25);
        btnSave.TabIndex = 2;
        btnSave.Text = "Save";
        btnSave.UseVisualStyleBackColor = true;
        btnSave.Click += OnSave;
        // 
        // btnCancel
        // 
        btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right;
        btnCancel.Location = new System.Drawing.Point(350, 278);
        btnCancel.Margin = new System.Windows.Forms.Padding(2);
        btnCancel.Name = "btnCancel";
        btnCancel.Size = new System.Drawing.Size(88, 25);
        btnCancel.TabIndex = 1;
        btnCancel.Text = "Cancel";
        btnCancel.UseVisualStyleBackColor = true;
        btnCancel.Click += OnCancel;
        // 
        // SettingsForm
        // 
        AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
        AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        ClientSize = new System.Drawing.Size(540, 314);
        Controls.Add(btnCancel);
        Controls.Add(btnSave);
        Controls.Add(tabMain);
        FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        Margin = new System.Windows.Forms.Padding(2);
        MaximizeBox = false;
        MinimizeBox = false;
        Name = "SettingsForm";
        StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
        Text = "Settings";
        tabMain.ResumeLayout(false);
        tabDatabase.ResumeLayout(false);
        tabDatabase.PerformLayout();
        grpAuth.ResumeLayout(false);
        grpAuth.PerformLayout();
        tabConnection.ResumeLayout(false);
        tabConnection.PerformLayout();
        ResumeLayout(false);
    }

    #endregion

    private System.Windows.Forms.TabControl tabMain;
    private System.Windows.Forms.TabPage tabDatabase;
    private System.Windows.Forms.TabPage tabConnection;
    private System.Windows.Forms.ComboBox cboDbProfile;
    private System.Windows.Forms.Label lblDbProfile;
    private System.Windows.Forms.Label lblDbHost;
    private System.Windows.Forms.TextBox txtDbHost;
    private System.Windows.Forms.Label lblDbPort;
    private System.Windows.Forms.TextBox txtDbPort;
    private System.Windows.Forms.Label lblDbName;
    private System.Windows.Forms.TextBox txtDbName;
    private System.Windows.Forms.Label lblArgon2Salt;
    private System.Windows.Forms.TextBox txtArgon2Salt;
    private System.Windows.Forms.GroupBox grpAuth;
    private System.Windows.Forms.RadioButton radAuthTrusted;
    private System.Windows.Forms.RadioButton radAuthSql;
    private System.Windows.Forms.Label lblDbUser;
    private System.Windows.Forms.TextBox txtDbUser;
    private System.Windows.Forms.Label lblDbPass;
    private System.Windows.Forms.TextBox txtDbPass;
    private System.Windows.Forms.TextBox txtClientPort;
    private System.Windows.Forms.Label lblClientPort;
    private System.Windows.Forms.Label lblAccountHost;
    private System.Windows.Forms.TextBox txtAccountHost;
    private System.Windows.Forms.TextBox txtAccountPort;
    private System.Windows.Forms.Label lblAccountPort;
    private System.Windows.Forms.TextBox txtMaxConn;
    private System.Windows.Forms.Label lblMaxConn;
    private System.Windows.Forms.Label lblLoadLow;
    private System.Windows.Forms.TextBox txtLoadLow;
    private System.Windows.Forms.Label lblLoadMidSep;
    private System.Windows.Forms.TextBox txtLoadMid;
    private System.Windows.Forms.Label lblLoadHighSep;
    private System.Windows.Forms.TextBox txtLoadHigh;
    private System.Windows.Forms.Button btnSave;
    private System.Windows.Forms.Button btnCancel;
}
