using AccountServer.Server;
using AccountServer.Settings;
using RFNetworking;

namespace AccountServer.UI;

public partial class MainForm : Form
{
    private readonly AppSettings _settings;
    private AccountHandler _handler;
    private NetworkListener? _loginListener;
    private NetworkListener? _worldListener;
    private NetworkListener? _controlListener;
    private CancellationTokenSource? _cts;
    private bool _running;

    public MainForm()
    {
        InitializeComponent();
        _settings = AppSettings.Load();
        _handler = CreateHandler();
    }

    private AccountHandler CreateHandler()
    {
        var connString = _settings.Database.BuildUserConnectionString();
        return new AccountHandler(AppendLog, connString);
    }

    private async void btnStart_Click(object sender, EventArgs e)
    {
        if (_running) return;
        _running = true;
        btnStart.Enabled = false;
        btnStop.Enabled = true;
        AppendLog("Starting account server...");

        _cts = new CancellationTokenSource();
        try
        {
            _loginListener = new NetworkListener(_handler) { PingPacket = new PacketEnvelope { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } } };
            _worldListener = new NetworkListener(_handler) { PingPacket = new PacketEnvelope { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } } };
            _controlListener = new NetworkListener(_handler) { PingPacket = new PacketEnvelope { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } } };

            await _loginListener.StartAsync(2700, _cts.Token);
            await _worldListener.StartAsync(29000, _cts.Token);
            await _controlListener.StartAsync(28000, _cts.Token);

            AppendLog("Listening on ports: login 2700, world 29000, control 28000");
        }
        catch (Exception ex)
        {
            AppendLog($"Failed to start: {ex.Message}");
            await StopAsync();
        }
    }

    private async void btnStop_Click(object sender, EventArgs e)
    {
        await StopAsync();
    }

    private async Task StopAsync()
    {
        if (!_running) return;
        _running = false;
        btnStart.Enabled = true;
        btnStop.Enabled = false;
        AppendLog("Stopping account server...");

        _cts?.Cancel();
        if (_loginListener != null) await _loginListener.DisposeAsync();
        if (_worldListener != null) await _worldListener.DisposeAsync();
        if (_controlListener != null) await _controlListener.DisposeAsync();
        _loginListener = _worldListener = _controlListener = null;
        _cts?.Dispose();
        _cts = null;
        AppendLog("Stopped.");
    }

    private void OnOpenSettings(object sender, EventArgs e)
    {
        using var form = new SettingsForm(_settings);
        if (form.ShowDialog(this) != DialogResult.OK)
        {
            return;
        }

        if (_running)
        {
            AppendLog("Settings saved. Restart server to apply DB changes.");
            return;
        }

        _handler = CreateHandler();
        AppendLog("Settings applied.");
    }

    private void AppendLog(string message)
    {
        if (InvokeRequired)
        {
            BeginInvoke(new Action<string>(AppendLog), message);
            return;
        }
        txtLog.AppendText($"[{DateTime.Now:HH:mm:ss}] {message}{Environment.NewLine}");
    }

    protected override void OnFormClosing(FormClosingEventArgs e)
    {
        base.OnFormClosing(e);
        _cts?.Cancel();
    }
}
