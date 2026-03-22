using AccountServer.Server;
using AccountServer.Settings;
using AccountServer.State;
using AccountServer.Data;
using RFNetworking;

namespace AccountServer.UI;

public partial class MainForm : Form
{
    private readonly AppSettings _settings;
    private LoginHandler _loginHandler;
    private WorldHandler _worldHandler;
    private ControlHandler _controlHandler;
    private NetworkListener? _loginListener;
    private NetworkListener? _worldListener;
    private NetworkListener? _controlListener;
    private CancellationTokenSource? _cts;
    private bool _running;

    public MainForm()
        : this(AppSettings.Load())
    {
    }

    public MainForm(AppSettings settings)
    {
        InitializeComponent();
        _settings = settings;
        AccountMainContext.Instance.LoadWorldList(_settings.WorldList.Worlds);
        _loginHandler = CreateLoginHandler();
        _worldHandler = CreateWorldHandler();
        _controlHandler = CreateControlHandler();
    }

    private LoginHandler CreateLoginHandler()
    {
        var connString = _settings.Database.BuildUserConnectionString(_settings.Database.Provider, AppContext.BaseDirectory);
        return new LoginHandler(AppendLog, () => _settings.VerboseLogging, _settings, connString);
    }

    private WorldHandler CreateWorldHandler()
    {
        var connString = _settings.Database.BuildUserConnectionString(_settings.Database.Provider, AppContext.BaseDirectory);
        return new WorldHandler(AppendLog, () => _settings.VerboseLogging, _settings, connString);
    }

    private ControlHandler CreateControlHandler()
    {
        var connString = _settings.Database.BuildUserConnectionString(_settings.Database.Provider, AppContext.BaseDirectory);
        return new ControlHandler(AppendLog, () => _settings.VerboseLogging, _settings, connString);
    }

    private async void btnStart_Click(object sender, EventArgs e)
    {
        await StartServerAsync();
    }

    private async Task StartServerAsync()
    {
        if (_running) return;
        if (!EnsureWorldEntriesConfigured())
        {
            return;
        }

        _running = true;
        btnStart.Enabled = false;
        btnStop.Enabled = true;
        AppendLog("Starting account server...");

        _cts = new CancellationTokenSource();
        try
        {
            _loginListener = new NetworkListener(_loginHandler) { PingPacket = new PacketEnvelope { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } } };
            _worldListener = new NetworkListener(_worldHandler) { PingPacket = new PacketEnvelope { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } } };
            _controlListener = new NetworkListener(_controlHandler) { PingPacket = new PacketEnvelope { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } } };

            var host = _settings.Listener.Host;
            await _loginListener.StartAsync(host, _settings.Listener.LoginPort, _cts.Token);
            await _worldListener.StartAsync(host, _settings.Listener.WorldPort, _cts.Token);
            await _controlListener.StartAsync(host, _settings.Listener.ControlPort, _cts.Token);

            AppendLog($"Listening on {host}: login {_settings.Listener.LoginPort}, world {_settings.Listener.WorldPort}, control {_settings.Listener.ControlPort}");
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
        using var form = new SettingsForm(_settings, _running);
        if (form.ShowDialog(this) != DialogResult.OK)
        {
            return;
        }

        if (_running)
        {
            AppendLog("Settings saved. Restart server to apply DB changes.");
            return;
        }

        AccountMainContext.Instance.LoadWorldList(_settings.WorldList.Worlds);
        _loginHandler = CreateLoginHandler();
        _worldHandler = CreateWorldHandler();
        _controlHandler = CreateControlHandler();
        AppendLog("Settings applied.");
    }

    private void OnAddNormalAccount(object? sender, EventArgs e)
    {
        using var form = new AddNormalAccountForm(new AccountAdminService(_settings));
        form.ShowDialog(this);
    }

    private void OnAddGmAccount(object? sender, EventArgs e)
    {
        using var form = new AddGmAccountForm(new AccountAdminService(_settings));
        form.ShowDialog(this);
    }

    private void OnShowArgon2Salt(object? sender, EventArgs e)
    {
        using var form = new Argon2SaltForm(_settings.Security.Argon2SaltBase64);
        form.ShowDialog(this);
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

    protected override async void OnShown(EventArgs e)
    {
        base.OnShown(e);

        if (_settings.Autostart)
        {
            await StartServerAsync();
        }
    }

    private bool EnsureWorldEntriesConfigured()
    {
        bool hasWorldEntries = _settings.WorldList?.Worlds != null && _settings.WorldList.Worlds.Count > 0;
        if (hasWorldEntries)
        {
            return true;
        }

        const string message = "AccountServer requires at least one configured world entry before it can start.";
        AppendLog(message);
        MessageBox.Show(this, message, "World Entry Required", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return false;
    }
}
