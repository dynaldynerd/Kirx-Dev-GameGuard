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
    private bool _verboseLog;
    private Task? _accountReconnectTask;
    private bool _isStopping;
    private bool _running;
    private bool _accountConnectAttemptLogged;
    private bool _accountConnectFailureLogged;

    public MainWindow()
    {
        InitializeComponent();
        _verboseLog = _settings.VerboseLogging;
        ApplySettingsToUi();
    }

    private void ApplySettingsToUi()
    {
        _verboseLog = _settings.VerboseLogging;
        verboseLogToolStripMenuItem.Checked = _verboseLog;
        UpdateExternalOpenButton(MainContext.Instance.ExternalOpen);
    }

    private void ReloadSettings()
    {
        _settings = AppSettings.Load();
        ApplySettingsToUi();
        AppendLog("Settings reloaded from disk.");
    }

    private async void OnStartClicked(object sender, EventArgs e)
    {
        await StartServerAsync();
    }

    private async Task StartServerAsync()
    {
        if (_running)
        {
            return;
        }

        _running = true;
        startButton.Enabled = false;
        stopButton.Enabled = true;
        UpdateStatus("Status: Starting...");
        SetExternalOpen(_settings.AutoOpenExternalConnection, logChange: false);

        _isStopping = false;
        _cts = new CancellationTokenSource();

        _clientRouter = new ClientPacketRouter(AppendLog, IsVerboseLoggingEnabled, _settings);
        _clientHandler = new LoginHandler("Client", AppendLog, IsVerboseLoggingEnabled, _clientRouter, null);
        _clientListener = new NetworkListener(_clientHandler, _settings.Network.MaxConnections);
        _clientListener.Log += OnClientListenerLog;
        MainContext.Instance.MaxConnections = _settings.Network.MaxConnections;

        _accountRouter = new AccountPacketRouter(AppendLog, IsVerboseLoggingEnabled);
        _accountHandler = new LoginHandler("Account", AppendLog, IsVerboseLoggingEnabled, null, _accountRouter, OnAccountConnected, OnAccountDisconnected);
        _accountConnector = new NetworkConnector(_accountHandler);
        _accountConnector.Log += OnAccountConnectorLog;
        ResetAccountReconnectLogState();
        StartAccountReconnectLoop();
        await Task.CompletedTask;
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
        _running = false;
        _isStopping = true;
        _cts?.Cancel();
        SetExternalOpen(false, logChange: false);
        var reconnectTask = _accountReconnectTask;

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

        if (reconnectTask != null)
        {
            try
            {
                await reconnectTask.ConfigureAwait(false);
            }
            catch (OperationCanceledException)
            {
            }
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

        logTextBox.AppendText($"[{DateTime.Now:HH:mm:ss}] {message}{Environment.NewLine}");
    }

    private void AppendVerboseLog(string message)
    {
        if (_verboseLog)
        {
            AppendLog(message);
        }
    }

    private bool IsVerboseLoggingEnabled()
    {
        return _verboseLog;
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
        _settings.VerboseLogging = _verboseLog;
        _settings.Save();
    }

    private void OnToggleExternalOpenClicked(object? sender, EventArgs e)
    {
        SetExternalOpen(!MainContext.Instance.ExternalOpen, logChange: true);
    }

    private void UpdateExternalOpenButton(bool isOpen)
    {
        if (InvokeRequired)
        {
            BeginInvoke(() => UpdateExternalOpenButton(isOpen));
            return;
        }

        externalOpenButton.Text = isOpen ? "External: Open" : "External: Closed";
    }

    private void SetExternalOpen(bool isOpen, bool logChange)
    {
        bool previous = MainContext.Instance.ExternalOpen;
        MainContext.Instance.ExternalOpen = isOpen;
        UpdateExternalOpenButton(isOpen);

        if (logChange && previous != isOpen)
        {
            AppendLog($"External login {(isOpen ? "opened" : "closed")}.");
        }
    }

    private async Task ConnectAccountAsync(CancellationToken token)
    {
        if (_accountConnector == null || token.IsCancellationRequested) return;
        if (_accountConnector.IsConnected) return;

        UpdateStatus($"Status: Waiting for account {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");
        if (!_accountConnectAttemptLogged)
        {
            AppendLog($"Connecting to account server {_settings.Network.AccountHost}:{_settings.Network.AccountPort}...");
            _accountConnectAttemptLogged = true;
        }
        await _accountConnector.ConnectAsync(_settings.Network.AccountHost, _settings.Network.AccountPort, token).ConfigureAwait(false);
        _accountConnectAttemptLogged = false;
        _accountConnectFailureLogged = false;
        AppendLog($"Connected to account server {_settings.Network.AccountHost}:{_settings.Network.AccountPort}.");
        UpdateStatus($"Status: Account connected {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");
    }

    private void StartAccountReconnectLoop()
    {
        if (_cts == null || _accountConnector == null)
        {
            return;
        }

        if (_accountReconnectTask != null && !_accountReconnectTask.IsCompleted)
        {
            return;
        }

        _accountReconnectTask = Task.Run(() => EnsureAccountConnectionLoopAsync(_cts.Token), CancellationToken.None);
    }

    private async Task EnsureAccountConnectionLoopAsync(CancellationToken token)
    {
        while (!token.IsCancellationRequested && !_isStopping)
        {
            if (_accountConnector == null)
            {
                return;
            }

            if (_accountConnector.IsConnected)
            {
                try
                {
                    await Task.Delay(TimeSpan.FromSeconds(1), token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                continue;
            }

            try
            {
                await ConnectAccountAsync(token).ConfigureAwait(false);
            }
            catch (OperationCanceledException)
            {
                break;
            }
            catch (Exception ex)
            {
                if (!_accountConnectFailureLogged)
                {
                    AppendLog($"Failed to connect to account server: {ex.Message}. Retrying until AccountServer is available.");
                    _accountConnectFailureLogged = true;
                }
                UpdateStatus($"Status: Waiting for account {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");

                try
                {
                    await Task.Delay(TimeSpan.FromSeconds(1), token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
            }
        }
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
            MainContext.Instance.BeginAwaitAccountDbInfo();
            await Task.Delay(100).ConfigureAwait(false); // slight delay to ensure readiness
            await SendAccountDbInfoRequestAsync(_cts.Token).ConfigureAwait(false);
            await SendWorldListRequestAsync(_cts.Token).ConfigureAwait(false);
            using var dbInfoWaitCts = CancellationTokenSource.CreateLinkedTokenSource(_cts.Token);
            dbInfoWaitCts.CancelAfter(TimeSpan.FromSeconds(5));
            await MainContext.Instance.WaitForAccountDbInfoAsync(dbInfoWaitCts.Token).ConfigureAwait(false);

            if (string.IsNullOrWhiteSpace(MainContext.Instance.AccountDbName))
            {
                throw new InvalidOperationException("Account server did not provide an RF_User database name.");
            }

            if (string.IsNullOrWhiteSpace(MainContext.Instance.AccountArgon2SaltBase64))
            {
                throw new InvalidOperationException("Account server did not provide an Argon2 salt.");
            }

            await StartClientListenerAsync(_cts.Token).ConfigureAwait(false);
            UpdateStatus($"Status: Listening on {_settings.Network.ClientPort}, account {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");
        }
        catch (Exception ex)
        {
            AppendLog($"Start client listener after account connect failed: {ex.Message}");
        }
    }

    private async void OnAccountDisconnected()
    {
        if (_isStopping) return;
        AppendLog("Account server disconnected. Retrying until AccountServer is available.");
        ResetAccountReconnectLogState();
        await StopClientListenerAsync().ConfigureAwait(false);
        UpdateStatus($"Status: Waiting for account {_settings.Network.AccountHost}:{_settings.Network.AccountPort}");
        StartAccountReconnectLoop();
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
            AppendVerboseLog("Sent world list request to account server.");
        }
        catch (Exception ex)
        {
            AppendLog($"World list request send failed: {ex.Message}");
        }
    }

    private async Task SendAccountDbInfoRequestAsync(CancellationToken token)
    {
        var accountConn = MainContext.Instance.AccountConnection;
        if (accountConn == null)
        {
            return;
        }

        var env = new PacketEnvelope
        {
            OpCode = 1,
            SubCode = 15,
            Payload = Array.Empty<byte>()
        };
        try
        {
            await accountConn.SendAsync(env, token).ConfigureAwait(false);
            AppendVerboseLog("Sent account DB info request to account server.");
        }
        catch (Exception ex)
        {
            AppendLog($"Account DB info request send failed: {ex.Message}");
            throw;
        }
    }

    private void ResetAccountReconnectLogState()
    {
        _accountConnectAttemptLogged = false;
        _accountConnectFailureLogged = false;
    }

    private void OnClientListenerLog(string message)
    {
        if (message.StartsWith("Client ", StringComparison.Ordinal) &&
            (message.Contains(" connected from ", StringComparison.Ordinal) ||
             message.Contains(" disconnected.", StringComparison.Ordinal)))
        {
            AppendVerboseLog(message);
            return;
        }

        AppendLog(message);
    }

    private void OnAccountConnectorLog(string message)
    {
        AppendVerboseLog(message);
    }

    protected override async void OnShown(EventArgs e)
    {
        base.OnShown(e);

        if (_settings.Autostart)
        {
            await StartServerAsync();
        }
    }

    private sealed class LoginHandler : NetworkHandlerBase
    {
        private readonly string _role;
        private readonly Action<string> _log;
        private readonly Func<bool> _isVerboseLoggingEnabled;
        private readonly ClientPacketRouter? _clientRouter;
        private readonly AccountPacketRouter? _accountRouter;
        private readonly Action? _onAccountConnected;
        private readonly Action? _onAccountDisconnected;

        public LoginHandler(string role, Action<string> log, Func<bool> isVerboseLoggingEnabled, ClientPacketRouter? clientRouter, AccountPacketRouter? accountRouter, Action? onAccountConnected = null, Action? onAccountDisconnected = null)
        {
            _role = role;
            _log = log;
            _isVerboseLoggingEnabled = isVerboseLoggingEnabled;
            _clientRouter = clientRouter;
            _accountRouter = accountRouter;
            _onAccountConnected = onAccountConnected;
            _onAccountDisconnected = onAccountDisconnected;
        }

        private void LogVerbose(string message)
        {
            if (_isVerboseLoggingEnabled())
            {
                _log(message);
            }
        }

        public override Task OnConnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
        {
            LogVerbose($"{_role} connected {connection.RemoteEndPoint} (id {connection.ConnectionId})");
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
            LogVerbose($"{_role} disconnected (id {connection.ConnectionId})");
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
            LogVerbose($"{_role} internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            return Task.CompletedTask;
        }
    }
}
