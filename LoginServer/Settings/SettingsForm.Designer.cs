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
        this.btnSave = new System.Windows.Forms.Button();
        this.btnCancel = new System.Windows.Forms.Button();
        this.lblDbHost = new System.Windows.Forms.Label();
        this.txtDbHost = new System.Windows.Forms.TextBox();
        this.lblDbPort = new System.Windows.Forms.Label();
        this.txtDbPort = new System.Windows.Forms.TextBox();
        this.lblDbName = new System.Windows.Forms.Label();
        this.txtDbName = new System.Windows.Forms.TextBox();
        this.lblDbUser = new System.Windows.Forms.Label();
        this.txtDbUser = new System.Windows.Forms.TextBox();
        this.lblDbPass = new System.Windows.Forms.Label();
        this.txtDbPass = new System.Windows.Forms.TextBox();
        this.lblClientPort = new System.Windows.Forms.Label();
        this.txtClientPort = new System.Windows.Forms.TextBox();
        this.lblAccountHost = new System.Windows.Forms.Label();
        this.txtAccountHost = new System.Windows.Forms.TextBox();
        this.lblAccountPort = new System.Windows.Forms.Label();
        this.txtAccountPort = new System.Windows.Forms.TextBox();
        this.lblMaxConn = new System.Windows.Forms.Label();
        this.txtMaxConn = new System.Windows.Forms.TextBox();
        this.SuspendLayout();
        // 
        // btnSave
        // 
        this.btnSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
        this.btnSave.Location = new System.Drawing.Point(348, 284);
        this.btnSave.Name = "btnSave";
        this.btnSave.Size = new System.Drawing.Size(112, 34);
        this.btnSave.TabIndex = 6;
        this.btnSave.Text = "Save";
        this.btnSave.UseVisualStyleBackColor = true;
        this.btnSave.Click += new System.EventHandler(this.OnSave);
        // 
        // btnCancel
        // 
        this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
        this.btnCancel.Location = new System.Drawing.Point(230, 284);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.Size = new System.Drawing.Size(112, 34);
        this.btnCancel.TabIndex = 7;
        this.btnCancel.Text = "Cancel";
        this.btnCancel.UseVisualStyleBackColor = true;
        this.btnCancel.Click += new System.EventHandler(this.OnCancel);
        // 
        // lblDbHost
        // 
        this.lblDbHost.AutoSize = true;
        this.lblDbHost.Location = new System.Drawing.Point(20, 20);
        this.lblDbHost.Name = "lblDbHost";
        this.lblDbHost.Size = new System.Drawing.Size(78, 25);
        this.lblDbHost.TabIndex = 0;
        this.lblDbHost.Text = "DB Host";
        // 
        // txtDbHost
        // 
        this.txtDbHost.Location = new System.Drawing.Point(110, 16);
        this.txtDbHost.Name = "txtDbHost";
        this.txtDbHost.Size = new System.Drawing.Size(200, 31);
        this.txtDbHost.TabIndex = 1;
        // 
        // lblDbPort
        // 
        this.lblDbPort.AutoSize = true;
        this.lblDbPort.Location = new System.Drawing.Point(330, 20);
        this.lblDbPort.Name = "lblDbPort";
        this.lblDbPort.Size = new System.Drawing.Size(44, 25);
        this.lblDbPort.TabIndex = 2;
        this.lblDbPort.Text = "Port";
        // 
        // txtDbPort
        // 
        this.txtDbPort.Location = new System.Drawing.Point(380, 16);
        this.txtDbPort.Name = "txtDbPort";
        this.txtDbPort.Size = new System.Drawing.Size(80, 31);
        this.txtDbPort.TabIndex = 2;
        // 
        // lblDbName
        // 
        this.lblDbName.AutoSize = true;
        this.lblDbName.Location = new System.Drawing.Point(20, 64);
        this.lblDbName.Name = "lblDbName";
        this.lblDbName.Size = new System.Drawing.Size(66, 25);
        this.lblDbName.TabIndex = 3;
        this.lblDbName.Text = "DB DB";
        // 
        // txtDbName
        // 
        this.txtDbName.Location = new System.Drawing.Point(110, 60);
        this.txtDbName.Name = "txtDbName";
        this.txtDbName.Size = new System.Drawing.Size(200, 31);
        this.txtDbName.TabIndex = 3;
        // 
        // lblDbUser
        // 
        this.lblDbUser.AutoSize = true;
        this.lblDbUser.Location = new System.Drawing.Point(20, 108);
        this.lblDbUser.Name = "lblDbUser";
        this.lblDbUser.Size = new System.Drawing.Size(88, 25);
        this.lblDbUser.TabIndex = 4;
        this.lblDbUser.Text = "DB User";
        // 
        // txtDbUser
        // 
        this.txtDbUser.Location = new System.Drawing.Point(110, 104);
        this.txtDbUser.Name = "txtDbUser";
        this.txtDbUser.Size = new System.Drawing.Size(200, 31);
        this.txtDbUser.TabIndex = 4;
        // 
        // lblDbPass
        // 
        this.lblDbPass.AutoSize = true;
        this.lblDbPass.Location = new System.Drawing.Point(20, 152);
        this.lblDbPass.Name = "lblDbPass";
        this.lblDbPass.Size = new System.Drawing.Size(75, 25);
        this.lblDbPass.TabIndex = 5;
        this.lblDbPass.Text = "DB Pass";
        // 
        // txtDbPass
        // 
        this.txtDbPass.Location = new System.Drawing.Point(110, 148);
        this.txtDbPass.Name = "txtDbPass";
        this.txtDbPass.Size = new System.Drawing.Size(200, 31);
        this.txtDbPass.TabIndex = 5;
        this.txtDbPass.UseSystemPasswordChar = true;
        // 
        // lblClientPort
        // 
        this.lblClientPort.AutoSize = true;
        this.lblClientPort.Location = new System.Drawing.Point(20, 200);
        this.lblClientPort.Name = "lblClientPort";
        this.lblClientPort.Size = new System.Drawing.Size(99, 25);
        this.lblClientPort.TabIndex = 2;
        this.lblClientPort.Text = "Client Port";
        // 
        // txtClientPort
        // 
        this.txtClientPort.Location = new System.Drawing.Point(140, 196);
        this.txtClientPort.Name = "txtClientPort";
        this.txtClientPort.Size = new System.Drawing.Size(120, 31);
        this.txtClientPort.TabIndex = 3;
        // 
        // lblAccountHost
        // 
        this.lblAccountHost.AutoSize = true;
        this.lblAccountHost.Location = new System.Drawing.Point(20, 244);
        this.lblAccountHost.Name = "lblAccountHost";
        this.lblAccountHost.Size = new System.Drawing.Size(121, 25);
        this.lblAccountHost.TabIndex = 4;
        this.lblAccountHost.Text = "Account Host";
        // 
        // txtAccountHost
        // 
        this.txtAccountHost.Location = new System.Drawing.Point(140, 240);
        this.txtAccountHost.Name = "txtAccountHost";
        this.txtAccountHost.Size = new System.Drawing.Size(160, 31);
        this.txtAccountHost.TabIndex = 4;
        // 
        // lblAccountPort
        // 
        this.lblAccountPort.AutoSize = true;
        this.lblAccountPort.Location = new System.Drawing.Point(320, 244);
        this.lblAccountPort.Name = "lblAccountPort";
        this.lblAccountPort.Size = new System.Drawing.Size(46, 25);
        this.lblAccountPort.TabIndex = 6;
        this.lblAccountPort.Text = "Port";
        // 
        // txtAccountPort
        // 
        this.txtAccountPort.Location = new System.Drawing.Point(370, 240);
        this.txtAccountPort.Name = "txtAccountPort";
        this.txtAccountPort.Size = new System.Drawing.Size(90, 31);
        this.txtAccountPort.TabIndex = 5;
        // 
        // lblMaxConn
        // 
        this.lblMaxConn.AutoSize = true;
        this.lblMaxConn.Location = new System.Drawing.Point(20, 288);
        this.lblMaxConn.Name = "lblMaxConn";
        this.lblMaxConn.Size = new System.Drawing.Size(155, 25);
        this.lblMaxConn.TabIndex = 8;
        this.lblMaxConn.Text = "Max Connections";
        // 
        // txtMaxConn
        // 
        this.txtMaxConn.Location = new System.Drawing.Point(180, 284);
        this.txtMaxConn.Name = "txtMaxConn";
        this.txtMaxConn.Size = new System.Drawing.Size(120, 31);
        this.txtMaxConn.TabIndex = 6;
        // 
        // SettingsForm
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(482, 330);
        this.Controls.Add(this.txtMaxConn);
        this.Controls.Add(this.lblMaxConn);
        this.Controls.Add(this.txtAccountPort);
        this.Controls.Add(this.lblAccountPort);
        this.Controls.Add(this.txtAccountHost);
        this.Controls.Add(this.lblAccountHost);
        this.Controls.Add(this.txtClientPort);
        this.Controls.Add(this.lblClientPort);
        this.Controls.Add(this.txtDbPass);
        this.Controls.Add(this.lblDbPass);
        this.Controls.Add(this.txtDbUser);
        this.Controls.Add(this.lblDbUser);
        this.Controls.Add(this.txtDbName);
        this.Controls.Add(this.lblDbName);
        this.Controls.Add(this.txtDbPort);
        this.Controls.Add(this.lblDbPort);
        this.Controls.Add(this.txtDbHost);
        this.Controls.Add(this.lblDbHost);
        this.Controls.Add(this.btnCancel);
        this.Controls.Add(this.btnSave);
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "SettingsForm";
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
        this.Text = "Settings";
        this.ResumeLayout(false);
        this.PerformLayout();

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
}
