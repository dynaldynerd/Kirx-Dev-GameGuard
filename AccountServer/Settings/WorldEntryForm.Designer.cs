#nullable enable
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace AccountServer.Settings;

public partial class WorldEntryForm
{
    private IContainer? components = null;
    private TableLayoutPanel layoutMain = null!;
    private FlowLayoutPanel panelButtons = null!;
    private Label lblName = null!;
    private Label lblAddress = null!;
    private Label lblDbName = null!;
    private Label lblType = null!;
    private TextBox txtName = null!;
    private TextBox txtAddress = null!;
    private TextBox txtDbName = null!;
    private ComboBox cmbType = null!;
    private Button btnInstallDb = null!;
    private Button btnOk = null!;
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
        layoutMain = new TableLayoutPanel();
        lblName = new Label();
        txtName = new TextBox();
        lblAddress = new Label();
        txtAddress = new TextBox();
        lblDbName = new Label();
        txtDbName = new TextBox();
        btnInstallDb = new Button();
        lblType = new Label();
        cmbType = new ComboBox();
        panelButtons = new FlowLayoutPanel();
        btnOk = new Button();
        btnCancel = new Button();
        layoutMain.SuspendLayout();
        panelButtons.SuspendLayout();
        SuspendLayout();
        // 
        // layoutMain
        // 
        layoutMain.AutoSize = true;
        layoutMain.ColumnCount = 3;
        layoutMain.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 100F));
        layoutMain.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        layoutMain.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 150F));
        layoutMain.Controls.Add(lblName, 0, 0);
        layoutMain.Controls.Add(txtName, 1, 0);
        layoutMain.Controls.Add(lblAddress, 0, 1);
        layoutMain.Controls.Add(txtAddress, 1, 1);
        layoutMain.Controls.Add(lblDbName, 0, 2);
        layoutMain.Controls.Add(txtDbName, 1, 2);
        layoutMain.Controls.Add(btnInstallDb, 2, 2);
        layoutMain.Controls.Add(lblType, 0, 3);
        layoutMain.Controls.Add(cmbType, 1, 3);
        layoutMain.Dock = DockStyle.Top;
        layoutMain.Location = new Point(0, 0);
        layoutMain.Name = "layoutMain";
        layoutMain.Padding = new Padding(10);
        layoutMain.RowCount = 4;
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.Size = new Size(540, 138);
        layoutMain.TabIndex = 0;
        // 
        // lblName
        // 
        lblName.AutoSize = true;
        lblName.Location = new Point(13, 10);
        lblName.Name = "lblName";
        lblName.Size = new Size(39, 15);
        lblName.TabIndex = 0;
        lblName.Text = "Name";
        // 
        // txtName
        // 
        layoutMain.SetColumnSpan(txtName, 2);
        txtName.Dock = DockStyle.Fill;
        txtName.Location = new Point(113, 13);
        txtName.Name = "txtName";
        txtName.Size = new Size(414, 23);
        txtName.TabIndex = 1;
        // 
        // lblAddress
        // 
        lblAddress.AutoSize = true;
        lblAddress.Location = new Point(13, 39);
        lblAddress.Name = "lblAddress";
        lblAddress.Size = new Size(49, 15);
        lblAddress.TabIndex = 2;
        lblAddress.Text = "Address";
        // 
        // txtAddress
        // 
        layoutMain.SetColumnSpan(txtAddress, 2);
        txtAddress.Dock = DockStyle.Fill;
        txtAddress.Location = new Point(113, 42);
        txtAddress.Name = "txtAddress";
        txtAddress.Size = new Size(414, 23);
        txtAddress.TabIndex = 3;
        // 
        // lblDbName
        // 
        lblDbName.AutoSize = true;
        lblDbName.Location = new Point(13, 68);
        lblDbName.Name = "lblDbName";
        lblDbName.Size = new Size(57, 15);
        lblDbName.TabIndex = 4;
        lblDbName.Text = "DB Name";
        // 
        // txtDbName
        // 
        txtDbName.Dock = DockStyle.Fill;
        txtDbName.Location = new Point(113, 71);
        txtDbName.Name = "txtDbName";
        txtDbName.Size = new Size(264, 23);
        txtDbName.TabIndex = 5;
        // 
        // btnInstallDb
        // 
        btnInstallDb.AutoSize = true;
        btnInstallDb.Location = new Point(383, 71);
        btnInstallDb.Name = "btnInstallDb";
        btnInstallDb.Size = new Size(123, 25);
        btnInstallDb.TabIndex = 6;
        btnInstallDb.Text = "Install DB (Optional)";
        btnInstallDb.Click += OnInstallDb;
        // 
        // lblType
        // 
        lblType.AutoSize = true;
        lblType.Location = new Point(13, 99);
        lblType.Name = "lblType";
        lblType.Size = new Size(32, 15);
        lblType.TabIndex = 7;
        lblType.Text = "Type";
        // 
        // cmbType
        // 
        layoutMain.SetColumnSpan(cmbType, 2);
        cmbType.Dock = DockStyle.Fill;
        cmbType.DropDownStyle = ComboBoxStyle.DropDownList;
        cmbType.Items.AddRange(new object[] { "0 - Regular/Normal Server", "1 - Test Server" });
        cmbType.Location = new Point(113, 102);
        cmbType.Name = "cmbType";
        cmbType.Size = new Size(414, 23);
        cmbType.TabIndex = 8;
        // 
        // panelButtons
        // 
        panelButtons.Controls.Add(btnOk);
        panelButtons.Controls.Add(btnCancel);
        panelButtons.Dock = DockStyle.Bottom;
        panelButtons.FlowDirection = FlowDirection.RightToLeft;
        panelButtons.Location = new Point(0, 176);
        panelButtons.Name = "panelButtons";
        panelButtons.Padding = new Padding(10);
        panelButtons.Size = new Size(540, 44);
        panelButtons.TabIndex = 1;
        // 
        // btnOk
        // 
        btnOk.AutoSize = true;
        btnOk.Location = new Point(442, 13);
        btnOk.Name = "btnOk";
        btnOk.Size = new Size(75, 25);
        btnOk.TabIndex = 0;
        btnOk.Text = "OK";
        btnOk.Click += OnOk;
        // 
        // btnCancel
        // 
        btnCancel.AutoSize = true;
        btnCancel.Location = new Point(361, 13);
        btnCancel.Name = "btnCancel";
        btnCancel.Size = new Size(75, 25);
        btnCancel.TabIndex = 1;
        btnCancel.Text = "Cancel";
        btnCancel.Click += OnCancel;
        // 
        // WorldEntryForm
        // 
        AcceptButton = btnOk;
        AutoScaleDimensions = new SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        CancelButton = btnCancel;
        ClientSize = new Size(540, 220);
        Controls.Add(layoutMain);
        Controls.Add(panelButtons);
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        Name = "WorldEntryForm";
        StartPosition = FormStartPosition.CenterParent;
        Text = "World Entry";
        layoutMain.ResumeLayout(false);
        layoutMain.PerformLayout();
        panelButtons.ResumeLayout(false);
        panelButtons.PerformLayout();
        ResumeLayout(false);
        PerformLayout();
    }
}
