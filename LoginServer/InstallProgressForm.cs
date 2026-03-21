using System.Drawing;
using System.Windows.Forms;

namespace LoginServer;

internal sealed class InstallProgressForm : Form
{
    private readonly Label _statusLabel;
    private readonly ProgressBar _progressBar;

    public InstallProgressForm(string title)
    {
        Text = title;
        FormBorderStyle = FormBorderStyle.FixedDialog;
        StartPosition = FormStartPosition.CenterParent;
        ClientSize = new Size(420, 100);
        MaximizeBox = false;
        MinimizeBox = false;
        ControlBox = false;
        ShowInTaskbar = false;

        _statusLabel = new Label
        {
            AutoEllipsis = true,
            Location = new Point(16, 16),
            Size = new Size(388, 32),
            Text = "Preparing install..."
        };

        _progressBar = new ProgressBar
        {
            Location = new Point(16, 58),
            Size = new Size(388, 20),
            Style = ProgressBarStyle.Marquee,
            MarqueeAnimationSpeed = 25
        };

        Controls.Add(_statusLabel);
        Controls.Add(_progressBar);
    }

    public void SetMarquee(string status)
    {
        _statusLabel.Text = status;
        _progressBar.Style = ProgressBarStyle.Marquee;
        _progressBar.MarqueeAnimationSpeed = 25;
        Refresh();
    }

    public void SetProgress(string status, int completed, int total)
    {
        _statusLabel.Text = status;
        if (total <= 1)
        {
            _progressBar.Style = ProgressBarStyle.Marquee;
            _progressBar.MarqueeAnimationSpeed = 25;
        }
        else
        {
            _progressBar.Style = ProgressBarStyle.Continuous;
            _progressBar.MarqueeAnimationSpeed = 0;
            _progressBar.Minimum = 0;
            _progressBar.Maximum = total;
            _progressBar.Value = completed < 0 ? 0 : (completed > total ? total : completed);
        }

        Refresh();
    }
}
