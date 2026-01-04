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
        this.lblConn = new System.Windows.Forms.Label();
        this.txtConnection = new System.Windows.Forms.TextBox();
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
        this.btnSave.Location = new System.Drawing.Point(348, 232);
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
        this.btnCancel.Location = new System.Drawing.Point(230, 232);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.Size = new System.Drawing.Size(112, 34);
        this.btnCancel.TabIndex = 7;
        this.btnCancel.Text = "Cancel";
        this.btnCancel.UseVisualStyleBackColor = true;
        this.btnCancel.Click += new System.EventHandler(this.OnCancel);
        // 
        // lblConn
        // 
        this.lblConn.AutoSize = true;
        this.lblConn.Location = new System.Drawing.Point(20, 20);
        this.lblConn.Name = "lblConn";
        this.lblConn.Size = new System.Drawing.Size(144, 25);
        this.lblConn.TabIndex = 0;
        this.lblConn.Text = "DB Connection";
        // 
        // txtConnection
        // 
        this.txtConnection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
        this.txtConnection.Location = new System.Drawing.Point(20, 48);
        this.txtConnection.Name = "txtConnection";
        this.txtConnection.Size = new System.Drawing.Size(440, 31);
        this.txtConnection.TabIndex = 1;
        // 
        // lblClientPort
        // 
        this.lblClientPort.AutoSize = true;
        this.lblClientPort.Location = new System.Drawing.Point(20, 92);
        this.lblClientPort.Name = "lblClientPort";
        this.lblClientPort.Size = new System.Drawing.Size(99, 25);
        this.lblClientPort.TabIndex = 2;
        this.lblClientPort.Text = "Client Port";
        // 
        // txtClientPort
        // 
        this.txtClientPort.Location = new System.Drawing.Point(140, 88);
        this.txtClientPort.Name = "txtClientPort";
        this.txtClientPort.Size = new System.Drawing.Size(120, 31);
        this.txtClientPort.TabIndex = 3;
        // 
        // lblAccountHost
        // 
        this.lblAccountHost.AutoSize = true;
        this.lblAccountHost.Location = new System.Drawing.Point(20, 136);
        this.lblAccountHost.Name = "lblAccountHost";
        this.lblAccountHost.Size = new System.Drawing.Size(121, 25);
        this.lblAccountHost.TabIndex = 4;
        this.lblAccountHost.Text = "Account Host";
        // 
        // txtAccountHost
        // 
        this.txtAccountHost.Location = new System.Drawing.Point(140, 132);
        this.txtAccountHost.Name = "txtAccountHost";
        this.txtAccountHost.Size = new System.Drawing.Size(160, 31);
        this.txtAccountHost.TabIndex = 4;
        // 
        // lblAccountPort
        // 
        this.lblAccountPort.AutoSize = true;
        this.lblAccountPort.Location = new System.Drawing.Point(320, 136);
        this.lblAccountPort.Name = "lblAccountPort";
        this.lblAccountPort.Size = new System.Drawing.Size(46, 25);
        this.lblAccountPort.TabIndex = 6;
        this.lblAccountPort.Text = "Port";
        // 
        // txtAccountPort
        // 
        this.txtAccountPort.Location = new System.Drawing.Point(370, 132);
        this.txtAccountPort.Name = "txtAccountPort";
        this.txtAccountPort.Size = new System.Drawing.Size(90, 31);
        this.txtAccountPort.TabIndex = 5;
        // 
        // lblMaxConn
        // 
        this.lblMaxConn.AutoSize = true;
        this.lblMaxConn.Location = new System.Drawing.Point(20, 180);
        this.lblMaxConn.Name = "lblMaxConn";
        this.lblMaxConn.Size = new System.Drawing.Size(155, 25);
        this.lblMaxConn.TabIndex = 8;
        this.lblMaxConn.Text = "Max Connections";
        // 
        // txtMaxConn
        // 
        this.txtMaxConn.Location = new System.Drawing.Point(180, 176);
        this.txtMaxConn.Name = "txtMaxConn";
        this.txtMaxConn.Size = new System.Drawing.Size(120, 31);
        this.txtMaxConn.TabIndex = 6;
        // 
        // SettingsForm
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(482, 278);
        this.Controls.Add(this.txtMaxConn);
        this.Controls.Add(this.lblMaxConn);
        this.Controls.Add(this.txtAccountPort);
        this.Controls.Add(this.lblAccountPort);
        this.Controls.Add(this.txtAccountHost);
        this.Controls.Add(this.lblAccountHost);
        this.Controls.Add(this.txtClientPort);
        this.Controls.Add(this.lblClientPort);
        this.Controls.Add(this.txtConnection);
        this.Controls.Add(this.lblConn);
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
    private System.Windows.Forms.Label lblConn;
    private System.Windows.Forms.TextBox txtConnection;
    private System.Windows.Forms.Label lblClientPort;
    private System.Windows.Forms.TextBox txtClientPort;
    private System.Windows.Forms.Label lblAccountHost;
    private System.Windows.Forms.TextBox txtAccountHost;
    private System.Windows.Forms.Label lblAccountPort;
    private System.Windows.Forms.TextBox txtAccountPort;
    private System.Windows.Forms.Label lblMaxConn;
    private System.Windows.Forms.TextBox txtMaxConn;
}
