using LoginServer.Handlers;
using LoginServer.Settings;
using LoginServer.State;
using RFNetworking;

namespace LoginServer;

public partial class Form1 : Form
{
    private AppSettings _settings = AppSettings.Load();
    private NetworkListener? _clientListener;
    private NetworkConnector? _accountConnector;
    private CancellationTokenSource? _cts;
    private LoginHandler? _clientHandler;
    private LoginHandler? _accountHandler;
    private ClientPacketRouter? _clientRouter;
    private AccountPacketRouter? _accountRouter;
    private bool _verboseLog = true;

    public Form1()
    {
        InitializeComponent();
        ApplySettingsToUi();
    }

    private void ApplySettingsToUi()
    {
        verboseLogToolStripMenuItem.Checked = _verboseLog;
    }

    private void ReloadSettings()
    {
        _settings = AppSettings.Load();
        ApplySettingsToUi();
        AppendLog("Settings reloaded from disk.");
    }

    private async void OnStartClicked(object sender, EventArgs e)
    {
        startButton.Enabled = false;
        stopButton.Enabled = true;
        statusLabel.Text = "Status: Starting...";

        _cts = new CancellationTokenSource();

        _clientRouter = new ClientPacketRouter(AppendLog);
        _clientHandler = new LoginHandler("Client", AppendLog, _clientRouter, null);
        _clientListener = new NetworkListener(_clientHandler, _settings.Network.MaxConnections);
        _clientListener.Log += AppendLog;

        _accountRouter = new AccountPacketRouter(AppendLog);
        _accountHandler = new LoginHandler("Account", AppendLog, null, _accountRouter);
        _accountConnector = new NetworkConnector(_accountHandler);
        _accountConnector.Log += AppendLog;

        try
        {
            await _clientListener.StartAsync(_settings.Network.ClientPort, _cts.Token);
            await _accountConnector.ConnectAsync(_settings.Network.AccountHost, _settings.Network.AccountPort, _cts.Token);
            statusLabel.Text = $"Status: Listening on {_settings.Network.ClientPort}, account {_settings.Network.AccountHost}:{_settings.Network.AccountPort}";
        }
        catch (Exception ex)
        {
            AppendLog($"Start failed: {ex.Message}");
            statusLabel.Text = "Status: Stopped";
            startButton.Enabled = true;
            stopButton.Enabled = false;
            await StopAllAsync();
        }
    }

    private async void OnStopClicked(object sender, EventArgs e)
    {
        stopButton.Enabled = false;
        startButton.Enabled = true;
        await StopAllAsync();
        statusLabel.Text = "Status: Stopped";
    }

    private async Task StopAllAsync()
    {
        _cts?.Cancel();

        try
        {
            if (_accountConnector != null)
            {
                await _accountConnector.StopAsync();
            }
        }
        catch (Exception ex)
        {
            AppendLog($"Account connector stop error: {ex.Message}");
        }

        try
        {
            if (_clientListener != null)
            {
                await _clientListener.StopAsync();
            }
        }
        catch (Exception ex)
        {
            AppendLog($"Listener stop error: {ex.Message}");
        }

        _cts?.Dispose();
        _cts = null;
        _clientListener = null;
        _accountConnector = null;
        _clientHandler = null;
        _accountHandler = null;
        _clientRouter = null;
        _accountRouter = null;
    }

    private void AppendLog(string message)
    {
        if (InvokeRequired)
        {
            BeginInvoke(() => AppendLog(message));
            return;
        }

        string line = $"[{DateTime.Now:HH:mm:ss}] {message}";
        if (_verboseLog || IsImportant(message))
        {
            logTextBox.AppendText(line + Environment.NewLine);
        }
    }

    private bool IsImportant(string message)
    {
        // Simple heuristic: always show errors/warnings
        return message.IndexOf("error", StringComparison.OrdinalIgnoreCase) >= 0 ||
               message.IndexOf("warn", StringComparison.OrdinalIgnoreCase) >= 0;
    }

    private void OnOpenSettings(object? sender, EventArgs e)
    {
        using var form = new SettingsForm(_settings);
        if (form.ShowDialog(this) == DialogResult.OK)
        {
            ApplySettingsToUi();
            AppendLog("Settings saved.");
        }
    }

    private void OnReloadSettings(object? sender, EventArgs e)
    {
        ReloadSettings();
    }

    private void OnToggleVerboseLog(object? sender, EventArgs e)
    {
        _verboseLog = verboseLogToolStripMenuItem.Checked;
    }

    private sealed class LoginHandler : NetworkHandlerBase
    {
        private readonly string _role;
        private readonly Action<string> _log;
        private readonly ClientPacketRouter? _clientRouter;
        private readonly AccountPacketRouter? _accountRouter;

        public LoginHandler(string role, Action<string> log, ClientPacketRouter? clientRouter, AccountPacketRouter? accountRouter)
        {
            _role = role;
            _log = log;
            _clientRouter = clientRouter;
            _accountRouter = accountRouter;
        }

        public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
        {
            _log($"{_role} connected {connection.RemoteEndPoint} (id {connection.ConnectionId})");
            if (_clientRouter != null)
            {
                MainContext.Instance.RegisterClientConnection(connection);
            }
            return Task.CompletedTask;
        }

        public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
        {
            _log($"{_role} disconnected (id {connection.ConnectionId})");
            if (_clientRouter != null)
            {
                MainContext.Instance.UnregisterClientConnection(connection);
            }
            return Task.CompletedTask;
        }

        public override async Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
        {
            if (_clientRouter != null && packet.OpCode == 21)
            {
                bool handled = await _clientRouter.HandleAsync(connection, packet, cancellationToken).ConfigureAwait(false);
                if (!handled)
                {
                    _log($"{_role} invalid client packet 21/{packet.SubCode} len={packet.Payload.Length}");
                    throw new InvalidOperationException($"Unhandled client packet 21/{packet.SubCode}");
                }
                return;
            }

            if (_accountRouter != null && packet.OpCode == 1)
            {
                bool handled = await _accountRouter.HandleAsync(connection, packet, cancellationToken).ConfigureAwait(false);
                if (!handled)
                {
                    _log($"{_role} invalid account packet 1/{packet.SubCode} len={packet.Payload.Length}");
                    throw new InvalidOperationException($"Unhandled account packet 1/{packet.SubCode}");
                }
                return;
            }

            _log($"{_role} data op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            throw new InvalidOperationException($"Unhandled packet op={packet.OpCode} sub={packet.SubCode}");
        }

        public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
        {
            _log($"{_role} internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            return Task.CompletedTask;
        }
    }
}
