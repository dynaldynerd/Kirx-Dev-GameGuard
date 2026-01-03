namespace LoginServer;

partial class Form1
{
    /// <summary>
    ///  Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    ///  Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
        if (disposing && (components != null))
        {
            components.Dispose();
        }
        base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    ///  Required method for Designer support - do not modify
    ///  the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
        this.startButton = new System.Windows.Forms.Button();
        this.stopButton = new System.Windows.Forms.Button();
        this.portLabel = new System.Windows.Forms.Label();
        this.portTextBox = new System.Windows.Forms.TextBox();
        this.accountHostLabel = new System.Windows.Forms.Label();
        this.accountHostTextBox = new System.Windows.Forms.TextBox();
        this.accountPortLabel = new System.Windows.Forms.Label();
        this.accountPortTextBox = new System.Windows.Forms.TextBox();
        this.statusLabel = new System.Windows.Forms.Label();
        this.logTextBox = new System.Windows.Forms.RichTextBox();
        this.SuspendLayout();
        // 
        // startButton
        // 
        this.startButton.Location = new System.Drawing.Point(24, 24);
        this.startButton.Name = "startButton";
        this.startButton.Size = new System.Drawing.Size(120, 34);
        this.startButton.TabIndex = 0;
        this.startButton.Text = "Start Server";
        this.startButton.UseVisualStyleBackColor = true;
        this.startButton.Click += new System.EventHandler(this.OnStartClicked);
        // 
        // stopButton
        // 
        this.stopButton.Enabled = false;
        this.stopButton.Location = new System.Drawing.Point(160, 24);
        this.stopButton.Name = "stopButton";
        this.stopButton.Size = new System.Drawing.Size(120, 34);
        this.stopButton.TabIndex = 1;
        this.stopButton.Text = "Stop Server";
        this.stopButton.UseVisualStyleBackColor = true;
        this.stopButton.Click += new System.EventHandler(this.OnStopClicked);
        // 
        // portLabel
        // 
        this.portLabel.AutoSize = true;
        this.portLabel.Location = new System.Drawing.Point(24, 78);
        this.portLabel.Name = "portLabel";
        this.portLabel.Size = new System.Drawing.Size(167, 25);
        this.portLabel.TabIndex = 2;
        this.portLabel.Text = "Client Port (listen)";
        // 
        // portTextBox
        // 
        this.portTextBox.Location = new System.Drawing.Point(112, 74);
        this.portTextBox.Name = "portTextBox";
        this.portTextBox.Size = new System.Drawing.Size(136, 31);
        this.portTextBox.TabIndex = 3;
        this.portTextBox.Text = "10001";
        // 
        // accountHostLabel
        // 
        this.accountHostLabel.AutoSize = true;
        this.accountHostLabel.Location = new System.Drawing.Point(280, 78);
        this.accountHostLabel.Name = "accountHostLabel";
        this.accountHostLabel.Size = new System.Drawing.Size(122, 25);
        this.accountHostLabel.TabIndex = 6;
        this.accountHostLabel.Text = "Account Host";
        // 
        // accountHostTextBox
        // 
        this.accountHostTextBox.Location = new System.Drawing.Point(408, 74);
        this.accountHostTextBox.Name = "accountHostTextBox";
        this.accountHostTextBox.Size = new System.Drawing.Size(176, 31);
        this.accountHostTextBox.TabIndex = 7;
        this.accountHostTextBox.Text = "127.0.0.1";
        // 
        // accountPortLabel
        // 
        this.accountPortLabel.AutoSize = true;
        this.accountPortLabel.Location = new System.Drawing.Point(600, 78);
        this.accountPortLabel.Name = "accountPortLabel";
        this.accountPortLabel.Size = new System.Drawing.Size(43, 25);
        this.accountPortLabel.TabIndex = 8;
        this.accountPortLabel.Text = "Port";
        // 
        // accountPortTextBox
        // 
        this.accountPortTextBox.Location = new System.Drawing.Point(649, 74);
        this.accountPortTextBox.Name = "accountPortTextBox";
        this.accountPortTextBox.Size = new System.Drawing.Size(96, 31);
        this.accountPortTextBox.TabIndex = 9;
        this.accountPortTextBox.Text = "27000";
        // 
        // statusLabel
        // 
        this.statusLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
        this.statusLabel.Location = new System.Drawing.Point(320, 24);
        this.statusLabel.Name = "statusLabel";
        this.statusLabel.Size = new System.Drawing.Size(256, 34);
        this.statusLabel.TabIndex = 4;
        this.statusLabel.Text = "Status: Stopped";
        this.statusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // logTextBox
        // 
        this.logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
        this.logTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
        this.logTextBox.Location = new System.Drawing.Point(24, 128);
        this.logTextBox.Name = "logTextBox";
        this.logTextBox.ReadOnly = true;
        this.logTextBox.Size = new System.Drawing.Size(880, 408);
        this.logTextBox.TabIndex = 10;
        this.logTextBox.Text = "";
        this.logTextBox.WordWrap = false;
        // 
        // Form1
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(928, 568);
        this.Controls.Add(this.accountPortTextBox);
        this.Controls.Add(this.accountPortLabel);
        this.Controls.Add(this.accountHostTextBox);
        this.Controls.Add(this.accountHostLabel);
        this.Controls.Add(this.logTextBox);
        this.Controls.Add(this.statusLabel);
        this.Controls.Add(this.portTextBox);
        this.Controls.Add(this.portLabel);
        this.Controls.Add(this.stopButton);
        this.Controls.Add(this.startButton);
        this.MinimumSize = new System.Drawing.Size(700, 400);
        this.Name = "Form1";
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
        this.Text = "Login Server Control";
        this.ResumeLayout(false);
        this.PerformLayout();
    }

    #endregion

    private System.Windows.Forms.Button startButton;
    private System.Windows.Forms.Button stopButton;
    private System.Windows.Forms.Label portLabel;
    private System.Windows.Forms.TextBox portTextBox;
    private System.Windows.Forms.Label statusLabel;
    private System.Windows.Forms.RichTextBox logTextBox;
    private System.Windows.Forms.Label accountHostLabel;
    private System.Windows.Forms.TextBox accountHostTextBox;
    private System.Windows.Forms.Label accountPortLabel;
    private System.Windows.Forms.TextBox accountPortTextBox;
}
