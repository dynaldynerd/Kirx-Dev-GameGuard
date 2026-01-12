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
    private NumericUpDown numType = null!;
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
        panelButtons = new FlowLayoutPanel();
        lblName = new Label();
        lblAddress = new Label();
        lblDbName = new Label();
        lblType = new Label();
        txtName = new TextBox();
        txtAddress = new TextBox();
        txtDbName = new TextBox();
        numType = new NumericUpDown();
        btnOk = new Button();
        btnCancel = new Button();
        ((ISupportInitialize)numType).BeginInit();
        layoutMain.SuspendLayout();
        panelButtons.SuspendLayout();
        SuspendLayout();
        // 
        // layoutMain
        // 
        layoutMain.AutoSize = true;
        layoutMain.ColumnCount = 2;
        layoutMain.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 100F));
        layoutMain.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
        layoutMain.Controls.Add(lblName, 0, 0);
        layoutMain.Controls.Add(txtName, 1, 0);
        layoutMain.Controls.Add(lblAddress, 0, 1);
        layoutMain.Controls.Add(txtAddress, 1, 1);
        layoutMain.Controls.Add(lblDbName, 0, 2);
        layoutMain.Controls.Add(txtDbName, 1, 2);
        layoutMain.Controls.Add(lblType, 0, 3);
        layoutMain.Controls.Add(numType, 1, 3);
        layoutMain.Dock = DockStyle.Top;
        layoutMain.Padding = new Padding(10);
        layoutMain.RowCount = 4;
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.RowStyles.Add(new RowStyle());
        layoutMain.Name = "layoutMain";
        // 
        // panelButtons
        // 
        panelButtons.Dock = DockStyle.Bottom;
        panelButtons.FlowDirection = FlowDirection.RightToLeft;
        panelButtons.Padding = new Padding(10);
        panelButtons.Height = 44;
        panelButtons.Name = "panelButtons";
        panelButtons.Controls.Add(btnOk);
        panelButtons.Controls.Add(btnCancel);
        // 
        // lblName
        // 
        lblName.AutoSize = true;
        lblName.Text = "Name";
        lblName.Name = "lblName";
        // 
        // lblAddress
        // 
        lblAddress.AutoSize = true;
        lblAddress.Text = "Address";
        lblAddress.Name = "lblAddress";
        // 
        // lblDbName
        // 
        lblDbName.AutoSize = true;
        lblDbName.Text = "DB Name";
        lblDbName.Name = "lblDbName";
        // 
        // lblType
        // 
        lblType.AutoSize = true;
        lblType.Text = "Type";
        lblType.Name = "lblType";
        // 
        // txtName
        // 
        txtName.Dock = DockStyle.Fill;
        txtName.Name = "txtName";
        // 
        // txtAddress
        // 
        txtAddress.Dock = DockStyle.Fill;
        txtAddress.Name = "txtAddress";
        // 
        // txtDbName
        // 
        txtDbName.Dock = DockStyle.Fill;
        txtDbName.Name = "txtDbName";
        // 
        // numType
        // 
        numType.Minimum = 0;
        numType.Maximum = 65535;
        numType.Name = "numType";
        // 
        // btnOk
        // 
        btnOk.Text = "OK";
        btnOk.AutoSize = true;
        btnOk.Name = "btnOk";
        btnOk.Click += OnOk;
        // 
        // btnCancel
        // 
        btnCancel.Text = "Cancel";
        btnCancel.AutoSize = true;
        btnCancel.Name = "btnCancel";
        btnCancel.Click += OnCancel;
        // 
        // WorldEntryForm
        // 
        AcceptButton = btnOk;
        CancelButton = btnCancel;
        AutoScaleDimensions = new SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        ClientSize = new Size(360, 220);
        Controls.Add(layoutMain);
        Controls.Add(panelButtons);
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        Name = "WorldEntryForm";
        StartPosition = FormStartPosition.CenterParent;
        Text = "World Entry";
        ((ISupportInitialize)numType).EndInit();
        layoutMain.ResumeLayout(false);
        layoutMain.PerformLayout();
        panelButtons.ResumeLayout(false);
        panelButtons.PerformLayout();
        ResumeLayout(false);
        PerformLayout();
    }
}
