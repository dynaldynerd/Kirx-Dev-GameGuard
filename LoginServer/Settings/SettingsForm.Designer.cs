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
        btnSave = new Button();
        btnCancel = new Button();
        lblDbHost = new Label();
        txtDbHost = new TextBox();
        lblDbPort = new Label();
        txtDbPort = new TextBox();
        lblDbName = new Label();
        txtDbName = new TextBox();
        lblDbUser = new Label();
        txtDbUser = new TextBox();
        lblDbPass = new Label();
        txtDbPass = new TextBox();
        lblBillingHost = new Label();
        txtBillingHost = new TextBox();
        lblBillingPort = new Label();
        txtBillingPort = new TextBox();
        lblBillingDb = new Label();
        txtBillingDb = new TextBox();
        lblBillingUser = new Label();
        txtBillingUser = new TextBox();
        lblBillingPass = new Label();
        txtBillingPass = new TextBox();
        lblClientPort = new Label();
        txtClientPort = new TextBox();
        lblAccountHost = new Label();
        txtAccountHost = new TextBox();
        lblAccountPort = new Label();
        txtAccountPort = new TextBox();
        lblMaxConn = new Label();
        txtMaxConn = new TextBox();
        SuspendLayout();
        // 
        // btnSave
        // 
        btnSave.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
        btnSave.Location = new Point(438, 255);
        btnSave.Margin = new Padding(2, 2, 2, 2);
        btnSave.Name = "btnSave";
        btnSave.Size = new Size(78, 20);
        btnSave.TabIndex = 14;
        btnSave.Text = "Save";
        btnSave.UseVisualStyleBackColor = true;
        btnSave.Click += OnSave;
        //
        // btnCancel
        //
        btnCancel.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
        btnCancel.Location = new Point(355, 255);
        btnCancel.Margin = new Padding(2, 2, 2, 2);
        btnCancel.Name = "btnCancel";
        btnCancel.Size = new Size(78, 20);
        btnCancel.TabIndex = 15;
        btnCancel.Text = "Cancel";
        btnCancel.UseVisualStyleBackColor = true;
        btnCancel.Click += OnCancel;
        //
        // lblDbHost
        // 
        lblDbHost.AutoSize = true;
        lblDbHost.Location = new Point(14, 12);
        lblDbHost.Margin = new Padding(2, 0, 2, 0);
        lblDbHost.Name = "lblDbHost";
        lblDbHost.Size = new Size(50, 15);
        lblDbHost.TabIndex = 0;
        lblDbHost.Text = "DB Host";
        // 
        // txtDbHost
        // 
        txtDbHost.Location = new Point(77, 10);
        txtDbHost.Margin = new Padding(2, 2, 2, 2);
        txtDbHost.Name = "txtDbHost";
        txtDbHost.Size = new Size(141, 23);
        txtDbHost.TabIndex = 1;
        // 
        // lblDbPort
        // 
        lblDbPort.AutoSize = true;
        lblDbPort.Location = new Point(231, 12);
        lblDbPort.Margin = new Padding(2, 0, 2, 0);
        lblDbPort.Name = "lblDbPort";
        lblDbPort.Size = new Size(29, 15);
        lblDbPort.TabIndex = 2;
        lblDbPort.Text = "Port";
        // 
        // txtDbPort
        // 
        txtDbPort.Location = new Point(266, 10);
        txtDbPort.Margin = new Padding(2, 2, 2, 2);
        txtDbPort.Name = "txtDbPort";
        txtDbPort.Size = new Size(57, 23);
        txtDbPort.TabIndex = 2;
        // 
        // lblDbName
        // 
        lblDbName.AutoSize = true;
        lblDbName.Location = new Point(14, 38);
        lblDbName.Margin = new Padding(2, 0, 2, 0);
        lblDbName.Name = "lblDbName";
        lblDbName.Size = new Size(40, 15);
        lblDbName.TabIndex = 3;
        lblDbName.Text = "DB DB";
        // 
        // txtDbName
        // 
        txtDbName.Location = new Point(77, 36);
        txtDbName.Margin = new Padding(2, 2, 2, 2);
        txtDbName.Name = "txtDbName";
        txtDbName.Size = new Size(141, 23);
        txtDbName.TabIndex = 3;
        // 
        // lblDbUser
        // 
        lblDbUser.AutoSize = true;
        lblDbUser.Location = new Point(14, 65);
        lblDbUser.Margin = new Padding(2, 0, 2, 0);
        lblDbUser.Name = "lblDbUser";
        lblDbUser.Size = new Size(48, 15);
        lblDbUser.TabIndex = 4;
        lblDbUser.Text = "DB User";
        // 
        // txtDbUser
        // 
        txtDbUser.Location = new Point(77, 62);
        txtDbUser.Margin = new Padding(2, 2, 2, 2);
        txtDbUser.Name = "txtDbUser";
        txtDbUser.Size = new Size(141, 23);
        txtDbUser.TabIndex = 4;
        // 
        // lblDbPass
        // 
        lblDbPass.AutoSize = true;
        lblDbPass.Location = new Point(14, 91);
        lblDbPass.Margin = new Padding(2, 0, 2, 0);
        lblDbPass.Name = "lblDbPass";
        lblDbPass.Size = new Size(48, 15);
        lblDbPass.TabIndex = 5;
        lblDbPass.Text = "DB Pass";
        // 
        // txtDbPass
        // 
        txtDbPass.Location = new Point(77, 89);
        txtDbPass.Margin = new Padding(2, 2, 2, 2);
        txtDbPass.Name = "txtDbPass";
        txtDbPass.Size = new Size(141, 23);
        txtDbPass.TabIndex = 5;
        txtDbPass.UseSystemPasswordChar = true;
        //
        // lblBillingHost
        //
        lblBillingHost.AutoSize = true;
        lblBillingHost.Location = new Point(231, 38);
        lblBillingHost.Margin = new Padding(2, 0, 2, 0);
        lblBillingHost.Name = "lblBillingHost";
        lblBillingHost.Size = new Size(68, 15);
        lblBillingHost.TabIndex = 16;
        lblBillingHost.Text = "Billing Host";
        //
        // txtBillingHost
        //
        txtBillingHost.Location = new Point(313, 36);
        txtBillingHost.Margin = new Padding(2, 2, 2, 2);
        txtBillingHost.Name = "txtBillingHost";
        txtBillingHost.Size = new Size(141, 23);
        txtBillingHost.TabIndex = 6;
        //
        // lblBillingPort
        //
        lblBillingPort.AutoSize = true;
        lblBillingPort.Location = new Point(465, 38);
        lblBillingPort.Margin = new Padding(2, 0, 2, 0);
        lblBillingPort.Name = "lblBillingPort";
        lblBillingPort.Size = new Size(29, 15);
        lblBillingPort.TabIndex = 18;
        lblBillingPort.Text = "Port";
        //
        // txtBillingPort
        //
        txtBillingPort.Location = new Point(501, 36);
        txtBillingPort.Margin = new Padding(2, 2, 2, 2);
        txtBillingPort.Name = "txtBillingPort";
        txtBillingPort.Size = new Size(57, 23);
        txtBillingPort.TabIndex = 7;
        //
        // lblBillingDb
        //
        lblBillingDb.AutoSize = true;
        lblBillingDb.Location = new Point(231, 65);
        lblBillingDb.Margin = new Padding(2, 0, 2, 0);
        lblBillingDb.Name = "lblBillingDb";
        lblBillingDb.Size = new Size(64, 15);
        lblBillingDb.TabIndex = 20;
        lblBillingDb.Text = "Billing DB";
        //
        // txtBillingDb
        //
        txtBillingDb.Location = new Point(313, 62);
        txtBillingDb.Margin = new Padding(2, 2, 2, 2);
        txtBillingDb.Name = "txtBillingDb";
        txtBillingDb.Size = new Size(141, 23);
        txtBillingDb.TabIndex = 8;
        //
        // lblBillingUser
        //
        lblBillingUser.AutoSize = true;
        lblBillingUser.Location = new Point(231, 91);
        lblBillingUser.Margin = new Padding(2, 0, 2, 0);
        lblBillingUser.Name = "lblBillingUser";
        lblBillingUser.Size = new Size(72, 15);
        lblBillingUser.TabIndex = 22;
        lblBillingUser.Text = "Billing User";
        //
        // txtBillingUser
        //
        txtBillingUser.Location = new Point(313, 89);
        txtBillingUser.Margin = new Padding(2, 2, 2, 2);
        txtBillingUser.Name = "txtBillingUser";
        txtBillingUser.Size = new Size(141, 23);
        txtBillingUser.TabIndex = 9;
        //
        // lblBillingPass
        //
        lblBillingPass.AutoSize = true;
        lblBillingPass.Location = new Point(231, 117);
        lblBillingPass.Margin = new Padding(2, 0, 2, 0);
        lblBillingPass.Name = "lblBillingPass";
        lblBillingPass.Size = new Size(72, 15);
        lblBillingPass.TabIndex = 24;
        lblBillingPass.Text = "Billing Pass";
        //
        // txtBillingPass
        //
        txtBillingPass.Location = new Point(313, 115);
        txtBillingPass.Margin = new Padding(2, 2, 2, 2);
        txtBillingPass.Name = "txtBillingPass";
        txtBillingPass.Size = new Size(141, 23);
        txtBillingPass.TabIndex = 10;
        txtBillingPass.UseSystemPasswordChar = true;
        //
        // lblClientPort
        //
        lblClientPort.AutoSize = true;
        lblClientPort.Location = new Point(14, 120);
        lblClientPort.Margin = new Padding(2, 0, 2, 0);
        lblClientPort.Name = "lblClientPort";
        lblClientPort.Size = new Size(63, 15);
        lblClientPort.TabIndex = 2;
        lblClientPort.Text = "Client Port";
        // 
        // txtClientPort
        // 
        txtClientPort.Location = new Point(98, 118);
        txtClientPort.Margin = new Padding(2, 2, 2, 2);
        txtClientPort.Name = "txtClientPort";
        txtClientPort.Size = new Size(85, 23);
        txtClientPort.TabIndex = 3;
        //
        // lblAccountHost
        // 
        lblAccountHost.AutoSize = true;
        lblAccountHost.Location = new Point(14, 146);
        lblAccountHost.Margin = new Padding(2, 0, 2, 0);
        lblAccountHost.Name = "lblAccountHost";
        lblAccountHost.Size = new Size(80, 15);
        lblAccountHost.TabIndex = 4;
        lblAccountHost.Text = "Account Host";
        // 
        // txtAccountHost
        // 
        txtAccountHost.Location = new Point(98, 144);
        txtAccountHost.Margin = new Padding(2, 2, 2, 2);
        txtAccountHost.Name = "txtAccountHost";
        txtAccountHost.Size = new Size(113, 23);
        txtAccountHost.TabIndex = 4;
        //
        // lblAccountPort
        // 
        lblAccountPort.AutoSize = true;
        lblAccountPort.Location = new Point(224, 146);
        lblAccountPort.Margin = new Padding(2, 0, 2, 0);
        lblAccountPort.Name = "lblAccountPort";
        lblAccountPort.Size = new Size(29, 15);
        lblAccountPort.TabIndex = 6;
        lblAccountPort.Text = "Port";
        // 
        // txtAccountPort
        // 
        txtAccountPort.Location = new Point(259, 144);
        txtAccountPort.Margin = new Padding(2, 2, 2, 2);
        txtAccountPort.Name = "txtAccountPort";
        txtAccountPort.Size = new Size(64, 23);
        txtAccountPort.TabIndex = 5;
        //
        // lblMaxConn
        // 
        lblMaxConn.AutoSize = true;
        lblMaxConn.Location = new Point(14, 173);
        lblMaxConn.Margin = new Padding(2, 0, 2, 0);
        lblMaxConn.Name = "lblMaxConn";
        lblMaxConn.Size = new Size(99, 15);
        lblMaxConn.TabIndex = 8;
        lblMaxConn.Text = "Max Connections";
        //
        // txtMaxConn
        //
        txtMaxConn.Location = new Point(126, 170);
        txtMaxConn.Margin = new Padding(2, 2, 2, 2);
        txtMaxConn.Name = "txtMaxConn";
        txtMaxConn.Size = new Size(85, 23);
        txtMaxConn.TabIndex = 6;
        //
        // SettingsForm
        //
        AutoScaleDimensions = new SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        ClientSize = new Size(596, 286);
        Controls.Add(txtBillingPass);
        Controls.Add(lblBillingPass);
        Controls.Add(txtBillingUser);
        Controls.Add(lblBillingUser);
        Controls.Add(txtBillingDb);
        Controls.Add(lblBillingDb);
        Controls.Add(txtBillingPort);
        Controls.Add(lblBillingPort);
        Controls.Add(txtBillingHost);
        Controls.Add(lblBillingHost);
        Controls.Add(txtMaxConn);
        Controls.Add(lblMaxConn);
        Controls.Add(txtAccountPort);
        Controls.Add(lblAccountPort);
        Controls.Add(txtAccountHost);
        Controls.Add(lblAccountHost);
        Controls.Add(txtClientPort);
        Controls.Add(lblClientPort);
        Controls.Add(txtDbPass);
        Controls.Add(lblDbPass);
        Controls.Add(txtDbUser);
        Controls.Add(lblDbUser);
        Controls.Add(txtDbName);
        Controls.Add(lblDbName);
        Controls.Add(txtDbPort);
        Controls.Add(lblDbPort);
        Controls.Add(txtDbHost);
        Controls.Add(lblDbHost);
        Controls.Add(btnCancel);
        Controls.Add(btnSave);
        FormBorderStyle = FormBorderStyle.FixedDialog;
        Margin = new Padding(2, 2, 2, 2);
        MaximizeBox = false;
        MinimizeBox = false;
        Name = "SettingsForm";
        StartPosition = FormStartPosition.CenterParent;
        Text = "Settings";
        ResumeLayout(false);
        PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button btnSave;
    private System.Windows.Forms.Button btnCancel;
    private System.Windows.Forms.Label lblClientPort;
    private System.Windows.Forms.TextBox txtClientPort;
    private System.Windows.Forms.Label lblAccountHost;
    private System.Windows.Forms.TextBox txtAccountHost;
    private System.Windows.Forms.Label lblAccountPort;
    private System.Windows.Forms.TextBox txtAccountPort;
    private System.Windows.Forms.Label lblMaxConn;
    private System.Windows.Forms.TextBox txtMaxConn;
    private System.Windows.Forms.Label lblDbHost;
    private System.Windows.Forms.TextBox txtDbHost;
    private System.Windows.Forms.Label lblDbPort;
    private System.Windows.Forms.TextBox txtDbPort;
    private System.Windows.Forms.Label lblDbName;
    private System.Windows.Forms.TextBox txtDbName;
    private System.Windows.Forms.Label lblDbUser;
    private System.Windows.Forms.TextBox txtDbUser;
    private System.Windows.Forms.Label lblDbPass;
    private System.Windows.Forms.TextBox txtDbPass;
    private System.Windows.Forms.Label lblBillingHost;
    private System.Windows.Forms.TextBox txtBillingHost;
    private System.Windows.Forms.Label lblBillingPort;
    private System.Windows.Forms.TextBox txtBillingPort;
    private System.Windows.Forms.Label lblBillingDb;
    private System.Windows.Forms.TextBox txtBillingDb;
    private System.Windows.Forms.Label lblBillingUser;
    private System.Windows.Forms.TextBox txtBillingUser;
    private System.Windows.Forms.Label lblBillingPass;
    private System.Windows.Forms.TextBox txtBillingPass;
}
