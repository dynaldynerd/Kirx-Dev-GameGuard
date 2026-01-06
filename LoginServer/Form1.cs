using LoginServer.Handlers;
using LoginServer.Packets;
using LoginServer.Settings;
using LoginServer.State;
using RFNetworking;
using System.Runtime.InteropServices;

namespace LoginServer;

public partial class MainWindow : Form
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
    private Task? _accountReconnectTask;
    private bool _isStopping;

    public MainWindow()
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
        UpdateStatus("Status: Starting...");

        _isStopping = false;
        _cts = new CancellationTokenSource();

        _clientRouter = new ClientPacketRouter(AppendLog, _settings);
        _clientHandler = new LoginHandler("Client", AppendLog, _clientRouter, null);
        _clientListener = new NetworkListener(_clientHandler, _settings.Network.MaxConnections);
        _clientListener.Log += AppendLog;
        MainContext.Instance.MaxConnections = _settings.Network.MaxConnections;

        _accountRouter = new AccountPacketRouter(AppendLog);
        _accountHandler = new LoginHandler("Account", AppendLog, null, _accountRouter, OnAccountConnected, OnAccountDisconnected);
        _accountConnector = new NetworkConnector(_accountHandler);
        _accountConnector.Log += AppendLog;

        try
        {
            await ConnectAccountAsync(_cts.Token).ConfigureAwait(true);
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
        UpdateStatus("Status: Stopped");
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
        _accountReconnectTask = null;
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

    private void UpdateStatus(string text)
    {
        if (InvokeRequired)
        {
            BeginInvoke(() => UpdateStatus(text));
            return;
        }
        statusLabel.Text = text;
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

    private async Task ConnectAccountAsync(CancellationToken token)
    {
        if (_accountConnector == null || token.IsCancellationRequested) return;
        if (_accountConnector.IsConnected) return;

        AppendLog($"Connecting to account server {_settings.Network.AccountHost}:{_settings.Network.AccountPort}...");
        await _accountConnector.ConnectAsync(_settings.Network.AccountHost, _settings.Network.AccountPort, token).ConfigureAwait(false);
            UpdateStatus($"Status: Account connected {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");
    }

    private async Task StartClientListenerAsync(CancellationToken token)
    {
        if (_clientListener == null)
        {
            return;
        }
        if (_clientListener.IsRunning)
        {
            return;
        }
        await _clientListener.StartAsync(_settings.Network.ClientPort, token).ConfigureAwait(false);
    }

    private async Task StopClientListenerAsync()
    {
        if (_clientListener != null && _clientListener.IsRunning)
        {
            await _clientListener.StopAsync().ConfigureAwait(false);
        }
    }

    private async void OnAccountConnected()
    {
        if (_cts == null || _isStopping) return;
        try
        {
            await Task.Delay(100).ConfigureAwait(false); // slight delay to ensure readiness
            await SendWorldListRequestAsync(_cts.Token).ConfigureAwait(false);
            await StartClientListenerAsync(_cts.Token).ConfigureAwait(false);
            UpdateStatus($"Status: Listening on {_settings.Network.ClientPort}, account {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");
        }
        catch (Exception ex)
        {
            AppendLog($"Start client listener after account connect failed: {ex.Message}");
        }
        _accountReconnectTask = null;
    }

    private async void OnAccountDisconnected()
    {
        if (_isStopping) return;
        AppendLog("Account server disconnected; stopping client listener and retrying...");
        await StopClientListenerAsync().ConfigureAwait(false);

        if (_cts == null || (_accountReconnectTask != null && !_accountReconnectTask.IsCompleted))
        {
            return;
        }

        _accountReconnectTask = Task.Run(async () =>
        {
            try
            {
                await Task.Delay(TimeSpan.FromSeconds(1), _cts.Token).ConfigureAwait(false);
                await ConnectAccountAsync(_cts.Token).ConfigureAwait(false);
            }
            catch (OperationCanceledException)
            {
            }
            catch (Exception ex)
            {
                AppendLog($"Account reconnect loop error: {ex.Message}");
            }
        });
    }

    private async Task SendWorldListRequestAsync(CancellationToken token)
    {
        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null) return;

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 10, // world list request loac
            Payload = Array.Empty<byte>()
        };
        try
        {
            await accountConn.SendAsync(env, token).ConfigureAwait(false);
            AppendLog("Sent world list request to account server.");
        }
        catch (Exception ex)
        {
            AppendLog($"World list request send failed: {ex.Message}");
        }
    }

    private sealed class LoginHandler : NetworkHandlerBase
    {
        private readonly string _role;
        private readonly Action<string> _log;
        private readonly ClientPacketRouter? _clientRouter;
        private readonly AccountPacketRouter? _accountRouter;
        private readonly Action? _onAccountConnected;
        private readonly Action? _onAccountDisconnected;

        public LoginHandler(string role, Action<string> log, ClientPacketRouter? clientRouter, AccountPacketRouter? accountRouter, Action? onAccountConnected = null, Action? onAccountDisconnected = null)
        {
            _role = role;
            _log = log;
            _clientRouter = clientRouter;
            _accountRouter = accountRouter;
            _onAccountConnected = onAccountConnected;
            _onAccountDisconnected = onAccountDisconnected;
        }

        public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
        {
            _log($"{_role} connected {connection.RemoteEndPoint} (id {connection.ConnectionId})");
            if (_clientRouter != null)
            {
                MainContext.Instance.RegisterClientConnection(connection);
            }
            if (_accountRouter != null)
            {
                MainContext.Instance.SetAccountConnection(connection);
                _onAccountConnected?.Invoke();
            }
            return Task.CompletedTask;
        }

        public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
        {
            _log($"{_role} disconnected (id {connection.ConnectionId})");
            if (_clientRouter != null)
            {
                var session = MainContext.Instance.GetClient((uint)connection.ConnectionId);
                if (session != null && !session.SelectedWorld && session.GlobalId.dwSerial != uint.MaxValue)
                {
                    var accountConn = MainContext.Instance.AccountConnection;
                    if (accountConn != null)
                    {
                        var logout = new _logout_account_request_loac { gidGlobal = session.GlobalId };
                        var env = new PacketEnvelope
                        {
                            OpCode = 1,
                            SubCode = 9,
                            Payload = logout.ToArray()
                        };
                        _ = accountConn.SendAsync(env, cancellationToken);
                    }
                }

                MainContext.Instance.UnregisterClientConnection(connection);
            }
            if (_accountRouter != null)
            {
                MainContext.Instance.SetAccountConnection(null);
                _onAccountDisconnected?.Invoke();
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
                    connection.Close();
                }
                return;
            }

            if (_accountRouter != null && packet.OpCode == 1)
            {
                bool handled = await _accountRouter.HandleAsync(connection, packet, cancellationToken).ConfigureAwait(false);
                if (!handled)
                {
                    _log($"{_role} invalid account packet 1/{packet.SubCode} len={packet.Payload.Length}");
                    connection.Close();
                }
                return;
            }

            _log($"{_role} data op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            connection.Close();
        }

        public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
        {
            _log($"{_role} internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            return Task.CompletedTask;
        }
    }
}
