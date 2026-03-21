using System.Drawing;
using System.Windows.Forms;

namespace AccountServer.UI;

public sealed class Argon2SaltForm : Form
{
    private readonly string _argon2SaltBase64;
    private readonly TextBox _txtSalt;
    private readonly Button _btnCopy;
    private readonly Button _btnClose;

    public Argon2SaltForm(string argon2SaltBase64)
    {
        _argon2SaltBase64 = argon2SaltBase64 ?? string.Empty;

        Text = "Argon2 Salt";
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        MinimizeBox = false;
        StartPosition = FormStartPosition.CenterParent;
        ClientSize = new Size(560, 154);

        var lblInfo = new Label
        {
            AutoSize = false,
            Location = new Point(12, 12),
            Size = new Size(536, 34),
            Text = "Copy this value when you need to restore an existing RF_User backup or configure an external registration page."
        };

        _txtSalt = new TextBox
        {
            Location = new Point(12, 55),
            Size = new Size(536, 23),
            ReadOnly = true,
            Text = _argon2SaltBase64
        };

        _btnCopy = new Button
        {
            Location = new Point(392, 108),
            Size = new Size(75, 27),
            Text = "Copy"
        };
        _btnCopy.Click += OnCopy;

        _btnClose = new Button
        {
            Location = new Point(473, 108),
            Size = new Size(75, 27),
            Text = "Close",
            DialogResult = DialogResult.OK
        };

        Controls.Add(lblInfo);
        Controls.Add(_txtSalt);
        Controls.Add(_btnCopy);
        Controls.Add(_btnClose);

        AcceptButton = _btnClose;
    }

    private void OnCopy(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(_argon2SaltBase64))
        {
            MessageBox.Show(this, "Argon2 salt is empty.", "Argon2 Salt", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        Clipboard.SetText(_argon2SaltBase64);
        _txtSalt.SelectAll();
        _btnCopy.Text = "Copied";
    }
}
