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
        startButton = new Button();
        stopButton = new Button();
        statusLabel = new Label();
        logTextBox = new RichTextBox();
        menuStrip1 = new MenuStrip();
        fileToolStripMenuItem = new ToolStripMenuItem();
        reloadSettingsToolStripMenuItem = new ToolStripMenuItem();
        settingsToolStripMenuItem = new ToolStripMenuItem();
        openSettingsToolStripMenuItem = new ToolStripMenuItem();
        logToolStripMenuItem = new ToolStripMenuItem();
        verboseLogToolStripMenuItem = new ToolStripMenuItem();
        menuStrip1.SuspendLayout();
        SuspendLayout();
        // 
        // startButton
        // 
        startButton.Location = new Point(17, 26);
        startButton.Margin = new Padding(2);
        startButton.Name = "startButton";
        startButton.Size = new Size(84, 35);
        startButton.TabIndex = 0;
        startButton.Text = "Start Server";
        startButton.UseVisualStyleBackColor = true;
        startButton.Click += OnStartClicked;
        // 
        // stopButton
        // 
        stopButton.Enabled = false;
        stopButton.Location = new Point(112, 26);
        stopButton.Margin = new Padding(2);
        stopButton.Name = "stopButton";
        stopButton.Size = new Size(84, 35);
        stopButton.TabIndex = 1;
        stopButton.Text = "Stop Server";
        stopButton.UseVisualStyleBackColor = true;
        stopButton.Click += OnStopClicked;
        // 
        // statusLabel
        // 
        statusLabel.Anchor = AnchorStyles.Top | AnchorStyles.Right;
        statusLabel.Location = new Point(228, 26);
        statusLabel.Margin = new Padding(2, 0, 2, 0);
        statusLabel.Name = "statusLabel";
        statusLabel.Size = new Size(179, 35);
        statusLabel.TabIndex = 4;
        statusLabel.Text = "Status: Stopped";
        statusLabel.TextAlign = ContentAlignment.MiddleLeft;
        // 
        // logTextBox
        // 
        logTextBox.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
        logTextBox.BorderStyle = BorderStyle.FixedSingle;
        logTextBox.Location = new Point(17, 65);
        logTextBox.Margin = new Padding(2);
        logTextBox.Name = "logTextBox";
        logTextBox.ReadOnly = true;
        logTextBox.Size = new Size(621, 283);
        logTextBox.TabIndex = 4;
        logTextBox.Text = "";
        logTextBox.WordWrap = false;
        // 
        // menuStrip1
        // 
        menuStrip1.ImageScalingSize = new Size(24, 24);
        menuStrip1.Items.AddRange(new ToolStripItem[] { fileToolStripMenuItem, settingsToolStripMenuItem, logToolStripMenuItem });
        menuStrip1.Location = new Point(0, 0);
        menuStrip1.Name = "menuStrip1";
        menuStrip1.Padding = new Padding(4, 1, 0, 1);
        menuStrip1.Size = new Size(654, 24);
        menuStrip1.TabIndex = 11;
        menuStrip1.Text = "menuStrip1";
        // 
        // fileToolStripMenuItem
        // 
        fileToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { reloadSettingsToolStripMenuItem });
        fileToolStripMenuItem.Name = "fileToolStripMenuItem";
        fileToolStripMenuItem.Size = new Size(37, 22);
        fileToolStripMenuItem.Text = "File";
        // 
        // reloadSettingsToolStripMenuItem
        // 
        reloadSettingsToolStripMenuItem.Name = "reloadSettingsToolStripMenuItem";
        reloadSettingsToolStripMenuItem.Size = new Size(155, 22);
        reloadSettingsToolStripMenuItem.Text = "Reload Settings";
        reloadSettingsToolStripMenuItem.Click += OnReloadSettings;
        // 
        // settingsToolStripMenuItem
        // 
        settingsToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { openSettingsToolStripMenuItem });
        settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
        settingsToolStripMenuItem.Size = new Size(61, 22);
        settingsToolStripMenuItem.Text = "Settings";
        // 
        // openSettingsToolStripMenuItem
        // 
        openSettingsToolStripMenuItem.Name = "openSettingsToolStripMenuItem";
        openSettingsToolStripMenuItem.Size = new Size(148, 22);
        openSettingsToolStripMenuItem.Text = "Open Settings";
        openSettingsToolStripMenuItem.Click += OnOpenSettings;
        // 
        // logToolStripMenuItem
        // 
        logToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { verboseLogToolStripMenuItem });
        logToolStripMenuItem.Name = "logToolStripMenuItem";
        logToolStripMenuItem.Size = new Size(39, 22);
        logToolStripMenuItem.Text = "Log";
        // 
        // verboseLogToolStripMenuItem
        // 
        verboseLogToolStripMenuItem.CheckOnClick = true;
        verboseLogToolStripMenuItem.Name = "verboseLogToolStripMenuItem";
        verboseLogToolStripMenuItem.Size = new Size(138, 22);
        verboseLogToolStripMenuItem.Text = "Verbose Log";
        verboseLogToolStripMenuItem.Click += OnToggleVerboseLog;
        // 
        // Form1
        // 
        AutoScaleDimensions = new SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        ClientSize = new Size(654, 366);
        Controls.Add(logTextBox);
        Controls.Add(menuStrip1);
        Controls.Add(statusLabel);
        Controls.Add(stopButton);
        Controls.Add(startButton);
        MainMenuStrip = menuStrip1;
        Margin = new Padding(2);
        MinimumSize = new Size(495, 256);
        Name = "Form1";
        StartPosition = FormStartPosition.CenterScreen;
        Text = "Login Server Control";
        menuStrip1.ResumeLayout(false);
        menuStrip1.PerformLayout();
        ResumeLayout(false);
        PerformLayout();
    }

    #endregion

    private System.Windows.Forms.Button startButton;
    private System.Windows.Forms.Button stopButton;
    private System.Windows.Forms.Label statusLabel;
    private System.Windows.Forms.RichTextBox logTextBox;
    private System.Windows.Forms.MenuStrip menuStrip1;
    private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem reloadSettingsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem openSettingsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem logToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem verboseLogToolStripMenuItem;
}
