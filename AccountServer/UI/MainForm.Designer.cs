namespace AccountServer.UI
{
    partial class MainForm
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
            menuStrip1 = new MenuStrip();
            fileToolStripMenuItem = new ToolStripMenuItem();
            settingsToolStripMenuItem = new ToolStripMenuItem();
            accountToolStripMenuItem = new ToolStripMenuItem();
            addToolStripMenuItem = new ToolStripMenuItem();
            normalAccountToolStripMenuItem = new ToolStripMenuItem();
            gMAccountToolStripMenuItem = new ToolStripMenuItem();
            banAccountToolStripMenuItem = new ToolStripMenuItem();
            helpToolStripMenuItem = new ToolStripMenuItem();
            aboutToolStripMenuItem = new ToolStripMenuItem();
            argon2SaltToolStripMenuItem = new ToolStripMenuItem();
            btnStart = new Button();
            btnStop = new Button();
            txtLog = new TextBox();
            menuStrip1.SuspendLayout();
            SuspendLayout();
            // 
            // menuStrip1
            // 
            menuStrip1.Items.AddRange(new ToolStripItem[] { fileToolStripMenuItem, accountToolStripMenuItem, helpToolStripMenuItem });
            menuStrip1.Location = new Point(0, 0);
            menuStrip1.Name = "menuStrip1";
            menuStrip1.Size = new Size(484, 24);
            menuStrip1.TabIndex = 0;
            menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            fileToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { settingsToolStripMenuItem });
            fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            fileToolStripMenuItem.Size = new Size(37, 20);
            fileToolStripMenuItem.Text = "File";
            // 
            // settingsToolStripMenuItem
            // 
            settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            settingsToolStripMenuItem.Size = new Size(116, 22);
            settingsToolStripMenuItem.Text = "Settings";
            settingsToolStripMenuItem.Click += OnOpenSettings;
            // 
            // accountToolStripMenuItem
            // 
            accountToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { addToolStripMenuItem });
            accountToolStripMenuItem.Name = "accountToolStripMenuItem";
            accountToolStripMenuItem.Size = new Size(64, 20);
            accountToolStripMenuItem.Text = "Account";
            // 
            // addToolStripMenuItem
            // 
            addToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { normalAccountToolStripMenuItem, gMAccountToolStripMenuItem, banAccountToolStripMenuItem });
            addToolStripMenuItem.Name = "addToolStripMenuItem";
            addToolStripMenuItem.Size = new Size(180, 22);
            addToolStripMenuItem.Text = "Add";
            // 
            // normalAccountToolStripMenuItem
            // 
            normalAccountToolStripMenuItem.Name = "normalAccountToolStripMenuItem";
            normalAccountToolStripMenuItem.Size = new Size(164, 22);
            normalAccountToolStripMenuItem.Text = "Normal Account";
            normalAccountToolStripMenuItem.Click += OnAddNormalAccount;
            // 
            // gMAccountToolStripMenuItem
            // 
            gMAccountToolStripMenuItem.Name = "gMAccountToolStripMenuItem";
            gMAccountToolStripMenuItem.Size = new Size(164, 22);
            gMAccountToolStripMenuItem.Text = "GM Account";
            gMAccountToolStripMenuItem.Click += OnAddGmAccount;
            // 
            // banAccountToolStripMenuItem
            // 
            banAccountToolStripMenuItem.Enabled = false;
            banAccountToolStripMenuItem.Name = "banAccountToolStripMenuItem";
            banAccountToolStripMenuItem.Size = new Size(164, 22);
            banAccountToolStripMenuItem.Text = "Ban Account";
            // 
            // helpToolStripMenuItem
            // 
            helpToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { aboutToolStripMenuItem });
            helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            helpToolStripMenuItem.Size = new Size(44, 20);
            helpToolStripMenuItem.Text = "Help";
            // 
            // aboutToolStripMenuItem
            // 
            aboutToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { argon2SaltToolStripMenuItem });
            aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            aboutToolStripMenuItem.Size = new Size(107, 22);
            aboutToolStripMenuItem.Text = "About";
            // 
            // argon2SaltToolStripMenuItem
            // 
            argon2SaltToolStripMenuItem.Name = "argon2SaltToolStripMenuItem";
            argon2SaltToolStripMenuItem.Size = new Size(136, 22);
            argon2SaltToolStripMenuItem.Text = "Argon2 Salt";
            argon2SaltToolStripMenuItem.Click += OnShowArgon2Salt;
            // 
            // btnStart
            // 
            btnStart.Location = new Point(12, 36);
            btnStart.Name = "btnStart";
            btnStart.Size = new Size(75, 23);
            btnStart.TabIndex = 1;
            btnStart.Text = "Start";
            btnStart.UseVisualStyleBackColor = true;
            btnStart.Click += btnStart_Click;
            // 
            // btnStop
            // 
            btnStop.Enabled = false;
            btnStop.Location = new Point(93, 36);
            btnStop.Name = "btnStop";
            btnStop.Size = new Size(75, 23);
            btnStop.TabIndex = 2;
            btnStop.Text = "Stop";
            btnStop.UseVisualStyleBackColor = true;
            btnStop.Click += btnStop_Click;
            // 
            // txtLog
            // 
            txtLog.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            txtLog.Location = new Point(12, 74);
            txtLog.Multiline = true;
            txtLog.Name = "txtLog";
            txtLog.ReadOnly = true;
            txtLog.ScrollBars = ScrollBars.Vertical;
            txtLog.Size = new Size(460, 275);
            txtLog.TabIndex = 3;
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(484, 361);
            Controls.Add(txtLog);
            Controls.Add(btnStop);
            Controls.Add(btnStart);
            Controls.Add(menuStrip1);
            MainMenuStrip = menuStrip1;
            MinimumSize = new Size(400, 300);
            Name = "MainForm";
            Text = "Account Server";
            menuStrip1.ResumeLayout(false);
            menuStrip1.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private MenuStrip menuStrip1;
        private ToolStripMenuItem fileToolStripMenuItem;
        private ToolStripMenuItem settingsToolStripMenuItem;
        private ToolStripMenuItem accountToolStripMenuItem;
        private ToolStripMenuItem addToolStripMenuItem;
        private ToolStripMenuItem normalAccountToolStripMenuItem;
        private ToolStripMenuItem gMAccountToolStripMenuItem;
        private ToolStripMenuItem banAccountToolStripMenuItem;
        private ToolStripMenuItem helpToolStripMenuItem;
        private ToolStripMenuItem aboutToolStripMenuItem;
        private ToolStripMenuItem argon2SaltToolStripMenuItem;
        private Button btnStart;
        private Button btnStop;
        private TextBox txtLog;
    }
}
