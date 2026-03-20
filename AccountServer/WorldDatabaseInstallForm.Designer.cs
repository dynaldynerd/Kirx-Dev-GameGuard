#nullable enable
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace AccountServer
{
    partial class WorldDatabaseInstallForm
    {
        private IContainer? components = null;

        private Label lblTitle = null!;
        private Label lblDatabaseName = null!;
        private TextBox txtDatabaseName = null!;
        private Label lblServerAddress = null!;
        private TextBox txtServerAddress = null!;
        private CheckBox chkTrustedConnection = null!;
        private Label lblUsername = null!;
        private TextBox txtUsername = null!;
        private Label lblPassword = null!;
        private TextBox txtPassword = null!;
        private FlowLayoutPanel panelButtons = null!;
        private Button btnInstall = null!;
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
            lblTitle = new Label();
            lblDatabaseName = new Label();
            txtDatabaseName = new TextBox();
            lblServerAddress = new Label();
            txtServerAddress = new TextBox();
            chkTrustedConnection = new CheckBox();
            lblUsername = new Label();
            txtUsername = new TextBox();
            lblPassword = new Label();
            txtPassword = new TextBox();
            panelButtons = new FlowLayoutPanel();
            btnInstall = new Button();
            btnCancel = new Button();
            panelButtons.SuspendLayout();
            SuspendLayout();
            // 
            // lblTitle
            // 
            lblTitle.AutoSize = true;
            lblTitle.Font = new Font("Segoe UI", 11F, FontStyle.Bold);
            lblTitle.Location = new Point(12, 12);
            lblTitle.Name = "lblTitle";
            lblTitle.Size = new Size(156, 20);
            lblTitle.TabIndex = 0;
            lblTitle.Text = "Install DB (Optional)";
            // 
            // lblDatabaseName
            // 
            lblDatabaseName.AutoSize = true;
            lblDatabaseName.Location = new Point(16, 50);
            lblDatabaseName.Name = "lblDatabaseName";
            lblDatabaseName.Size = new Size(89, 15);
            lblDatabaseName.TabIndex = 1;
            lblDatabaseName.Text = "World DB Name";
            // 
            // txtDatabaseName
            // 
            txtDatabaseName.Location = new Point(129, 47);
            txtDatabaseName.Name = "txtDatabaseName";
            txtDatabaseName.ReadOnly = true;
            txtDatabaseName.Size = new Size(239, 23);
            txtDatabaseName.TabIndex = 2;
            // 
            // lblServerAddress
            // 
            lblServerAddress.AutoSize = true;
            lblServerAddress.Location = new Point(16, 82);
            lblServerAddress.Name = "lblServerAddress";
            lblServerAddress.Size = new Size(82, 15);
            lblServerAddress.TabIndex = 3;
            lblServerAddress.Text = "Server Address";
            // 
            // txtServerAddress
            // 
            txtServerAddress.Location = new Point(129, 79);
            txtServerAddress.Name = "txtServerAddress";
            txtServerAddress.Size = new Size(239, 23);
            txtServerAddress.TabIndex = 4;
            // 
            // chkTrustedConnection
            // 
            chkTrustedConnection.AutoSize = true;
            chkTrustedConnection.Location = new Point(129, 111);
            chkTrustedConnection.Name = "chkTrustedConnection";
            chkTrustedConnection.Size = new Size(124, 19);
            chkTrustedConnection.TabIndex = 5;
            chkTrustedConnection.Text = "Trusted Connection";
            chkTrustedConnection.UseVisualStyleBackColor = true;
            chkTrustedConnection.CheckedChanged += TrustedConnectionChanged;
            // 
            // lblUsername
            // 
            lblUsername.AutoSize = true;
            lblUsername.Location = new Point(16, 142);
            lblUsername.Name = "lblUsername";
            lblUsername.Size = new Size(60, 15);
            lblUsername.TabIndex = 6;
            lblUsername.Text = "Username";
            // 
            // txtUsername
            // 
            txtUsername.Location = new Point(129, 139);
            txtUsername.Name = "txtUsername";
            txtUsername.Size = new Size(239, 23);
            txtUsername.TabIndex = 7;
            // 
            // lblPassword
            // 
            lblPassword.AutoSize = true;
            lblPassword.Location = new Point(16, 171);
            lblPassword.Name = "lblPassword";
            lblPassword.Size = new Size(57, 15);
            lblPassword.TabIndex = 8;
            lblPassword.Text = "Password";
            // 
            // txtPassword
            // 
            txtPassword.Location = new Point(129, 168);
            txtPassword.Name = "txtPassword";
            txtPassword.PasswordChar = '*';
            txtPassword.Size = new Size(239, 23);
            txtPassword.TabIndex = 9;
            // 
            // panelButtons
            // 
            panelButtons.Controls.Add(btnInstall);
            panelButtons.Controls.Add(btnCancel);
            panelButtons.Dock = DockStyle.Bottom;
            panelButtons.FlowDirection = FlowDirection.RightToLeft;
            panelButtons.Location = new Point(0, 211);
            panelButtons.Name = "panelButtons";
            panelButtons.Padding = new Padding(10);
            panelButtons.Size = new Size(384, 50);
            panelButtons.TabIndex = 10;
            // 
            // btnInstall
            // 
            btnInstall.AutoSize = true;
            btnInstall.Location = new Point(298, 13);
            btnInstall.Name = "btnInstall";
            btnInstall.Size = new Size(63, 25);
            btnInstall.TabIndex = 0;
            btnInstall.Text = "Install";
            btnInstall.UseVisualStyleBackColor = true;
            btnInstall.Click += BtnInstall_Click;
            // 
            // btnCancel
            // 
            btnCancel.AutoSize = true;
            btnCancel.DialogResult = DialogResult.Cancel;
            btnCancel.Location = new Point(217, 13);
            btnCancel.Name = "btnCancel";
            btnCancel.Size = new Size(75, 25);
            btnCancel.TabIndex = 1;
            btnCancel.Text = "Cancel";
            btnCancel.UseVisualStyleBackColor = true;
            // 
            // WorldDatabaseInstallForm
            // 
            AcceptButton = btnInstall;
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            CancelButton = btnCancel;
            ClientSize = new Size(384, 261);
            Controls.Add(panelButtons);
            Controls.Add(txtPassword);
            Controls.Add(lblPassword);
            Controls.Add(txtUsername);
            Controls.Add(lblUsername);
            Controls.Add(chkTrustedConnection);
            Controls.Add(txtServerAddress);
            Controls.Add(lblServerAddress);
            Controls.Add(txtDatabaseName);
            Controls.Add(lblDatabaseName);
            Controls.Add(lblTitle);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "WorldDatabaseInstallForm";
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterParent;
            Text = "Install RF_World DB (Optional)";
            panelButtons.ResumeLayout(false);
            panelButtons.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }
    }
}
