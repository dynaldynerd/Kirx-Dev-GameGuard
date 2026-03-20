using AccountServer.Data;

namespace AccountServer.UI;

public sealed class AddNormalAccountForm : Form
{
    private readonly AccountAdminService _service;

    private TextBox txtUsername = null!;
    private TextBox txtPassword = null!;
    private Button btnCreate = null!;
    private Button btnCancel = null!;

    public AddNormalAccountForm(AccountAdminService service)
    {
        _service = service;
        InitializeComponent();
    }

    private void InitializeComponent()
    {
        Text = "Add Normal Account";
        StartPosition = FormStartPosition.CenterParent;
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        ClientSize = new Size(380, 160);

        var table = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 2,
            RowCount = 3,
            Padding = new Padding(12)
        };
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 90));
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
        table.RowStyles.Add(new RowStyle(SizeType.AutoSize));
        table.RowStyles.Add(new RowStyle(SizeType.AutoSize));
        table.RowStyles.Add(new RowStyle(SizeType.Percent, 100));

        var lblUsername = new Label
        {
            Text = "Username",
            AutoSize = true,
            Anchor = AnchorStyles.Left
        };
        txtUsername = new TextBox
        {
            MaxLength = 12,
            Anchor = AnchorStyles.Left | AnchorStyles.Right
        };

        var lblPassword = new Label
        {
            Text = "Password",
            AutoSize = true,
            Anchor = AnchorStyles.Left
        };
        txtPassword = new TextBox
        {
            MaxLength = 12,
            UseSystemPasswordChar = true,
            Anchor = AnchorStyles.Left | AnchorStyles.Right
        };

        var panelButtons = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill,
            FlowDirection = FlowDirection.RightToLeft
        };
        btnCreate = new Button
        {
            Text = "Create",
            AutoSize = true
        };
        btnCreate.Click += BtnCreate_Click;
        btnCancel = new Button
        {
            Text = "Cancel",
            AutoSize = true,
            DialogResult = DialogResult.Cancel
        };
        panelButtons.Controls.Add(btnCreate);
        panelButtons.Controls.Add(btnCancel);

        table.Controls.Add(lblUsername, 0, 0);
        table.Controls.Add(txtUsername, 1, 0);
        table.Controls.Add(lblPassword, 0, 1);
        table.Controls.Add(txtPassword, 1, 1);
        table.Controls.Add(panelButtons, 0, 2);
        table.SetColumnSpan(panelButtons, 2);

        AcceptButton = btnCreate;
        CancelButton = btnCancel;
        Controls.Add(table);
    }

    private async void BtnCreate_Click(object? sender, EventArgs e)
    {
        SetBusy(true);
        try
        {
            AccountAdminResult result = await _service.CreateNormalAccountAsync(
                txtUsername.Text,
                txtPassword.Text,
                CancellationToken.None).ConfigureAwait(true);

            if (!result.IsSuccess)
            {
                MessageBox.Show(this, result.Message, Text, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            MessageBox.Show(this, result.Message, Text, MessageBoxButtons.OK, MessageBoxIcon.Information);
            DialogResult = DialogResult.OK;
            Close();
        }
        catch (Exception ex)
        {
            MessageBox.Show(this, $"Failed to create account: {ex.Message}", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            SetBusy(false);
        }
    }

    private void SetBusy(bool busy)
    {
        btnCreate.Enabled = !busy;
        btnCancel.Enabled = !busy;
        txtUsername.Enabled = !busy;
        txtPassword.Enabled = !busy;
        Cursor = busy ? Cursors.WaitCursor : Cursors.Default;
    }
}
