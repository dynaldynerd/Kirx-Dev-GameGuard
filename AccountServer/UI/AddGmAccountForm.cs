using AccountServer.Data;

namespace AccountServer.UI;

public sealed class AddGmAccountForm : Form
{
    private readonly AccountAdminService _service;

    private TextBox txtPrefix = null!;
    private TextBox txtUsername = null!;
    private TextBox txtPassword = null!;
    private NumericUpDown nudGrade = null!;
    private NumericUpDown nudSubGrade = null!;
    private DateTimePicker dtpExpire = null!;
    private Button btnCreate = null!;
    private Button btnCancel = null!;

    public AddGmAccountForm(AccountAdminService service)
    {
        _service = service;
        InitializeComponent();
    }

    private void InitializeComponent()
    {
        Text = "Add GM Account";
        StartPosition = FormStartPosition.CenterParent;
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        ClientSize = new Size(430, 240);

        var table = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 3,
            RowCount = 5,
            Padding = new Padding(12)
        };
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 90));
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 38));
        table.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));

        var lblUsername = new Label
        {
            Text = "Username",
            AutoSize = true,
            Anchor = AnchorStyles.Left
        };
        txtPrefix = new TextBox
        {
            ReadOnly = true,
            Enabled = false,
            Text = "!",
            TextAlign = HorizontalAlignment.Center,
            Anchor = AnchorStyles.Left | AnchorStyles.Right
        };
        txtUsername = new TextBox
        {
            MaxLength = 11,
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

        var lblGrade = new Label
        {
            Text = "Grade",
            AutoSize = true,
            Anchor = AnchorStyles.Left
        };
        nudGrade = new NumericUpDown
        {
            Minimum = 1,
            Maximum = 255,
            Value = 1,
            Anchor = AnchorStyles.Left
        };

        var lblSubGrade = new Label
        {
            Text = "SubGrade",
            AutoSize = true,
            Anchor = AnchorStyles.Left
        };
        nudSubGrade = new NumericUpDown
        {
            Minimum = 0,
            Maximum = 255,
            Value = 0,
            Anchor = AnchorStyles.Left
        };

        var lblExpire = new Label
        {
            Text = "Expire Date",
            AutoSize = true,
            Anchor = AnchorStyles.Left
        };
        dtpExpire = new DateTimePicker
        {
            Format = DateTimePickerFormat.Custom,
            CustomFormat = "yyyy-MM-dd HH:mm:ss",
            Value = DateTime.Now.AddMonths(1),
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
        table.Controls.Add(txtPrefix, 1, 0);
        table.Controls.Add(txtUsername, 2, 0);
        table.Controls.Add(lblPassword, 0, 1);
        table.Controls.Add(txtPassword, 2, 1);
        table.Controls.Add(lblGrade, 0, 2);
        table.Controls.Add(nudGrade, 2, 2);
        table.Controls.Add(lblSubGrade, 0, 3);
        table.Controls.Add(nudSubGrade, 2, 3);
        table.Controls.Add(lblExpire, 0, 4);
        table.Controls.Add(dtpExpire, 2, 4);

        var host = new Panel
        {
            Dock = DockStyle.Bottom,
            Height = 52,
            Padding = new Padding(12, 0, 12, 12)
        };
        panelButtons.Dock = DockStyle.Fill;
        host.Controls.Add(panelButtons);

        Controls.Add(table);
        Controls.Add(host);

        AcceptButton = btnCreate;
        CancelButton = btnCancel;
    }

    private async void BtnCreate_Click(object? sender, EventArgs e)
    {
        SetBusy(true);
        try
        {
            AccountAdminResult result = await _service.CreateGmAccountAsync(
                txtUsername.Text,
                txtPassword.Text,
                (byte)nudGrade.Value,
                (byte)nudSubGrade.Value,
                dtpExpire.Value,
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
            MessageBox.Show(this, $"Failed to create GM account: {ex.Message}", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
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
        nudGrade.Enabled = !busy;
        nudSubGrade.Enabled = !busy;
        dtpExpire.Enabled = !busy;
        Cursor = busy ? Cursors.WaitCursor : Cursors.Default;
    }
}
