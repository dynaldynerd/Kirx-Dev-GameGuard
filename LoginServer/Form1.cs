using LoginServer.Handlers;
using RFNetworking;

namespace LoginServer;

public partial class Form1 : Form
{
    private NetworkListener? _clientListener;
    private NetworkConnector? _accountConnector;
    private CancellationTokenSource? _cts;
    private LoginHandler? _clientHandler;
    private LoginHandler? _accountHandler;
    private ClientPacketRouter? _clientRouter;
    private AccountPacketRouter? _accountRouter;

    public Form1()
    {
        InitializeComponent();
    }

    private async void OnStartClicked(object sender, EventArgs e)
    {
        if (!int.TryParse(portTextBox.Text, out int clientPort) || clientPort < 1 || clientPort > 65535)
        {
            MessageBox.Show(this, "Enter a valid client TCP port (1-65535).", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        if (!int.TryParse(accountPortTextBox.Text, out int accountPort) || accountPort < 1 || accountPort > 65535)
        {
            MessageBox.Show(this, "Enter a valid account TCP port (1-65535).", "Invalid Port", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        string accountHost = accountHostTextBox.Text.Trim();
        if (string.IsNullOrWhiteSpace(accountHost))
        {
            MessageBox.Show(this, "Enter an account server host.", "Invalid Host", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        startButton.Enabled = false;
        stopButton.Enabled = true;
        statusLabel.Text = "Status: Starting...";

        _cts = new CancellationTokenSource();

        _clientRouter = new ClientPacketRouter(AppendLog);
        _clientHandler = new LoginHandler("Client", AppendLog, _clientRouter, null);
        _clientListener = new NetworkListener(_clientHandler);
        _clientListener.Log += AppendLog;

        _accountRouter = new AccountPacketRouter(AppendLog);
        _accountHandler = new LoginHandler("Account", AppendLog, null, _accountRouter);
        _accountConnector = new NetworkConnector(_accountHandler);
        _accountConnector.Log += AppendLog;

        try
        {
            await _clientListener.StartAsync(clientPort, _cts.Token);
            await _accountConnector.ConnectAsync(accountHost, accountPort, _cts.Token);
            statusLabel.Text = $"Status: Listening on {clientPort}, account {accountHost}:{accountPort}";
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
        logTextBox.AppendText(line + Environment.NewLine);
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
            return Task.CompletedTask;
        }

        public override Task OnDisconnectedAsync(PublicConnection connection, CancellationToken cancellationToken)
        {
            _log($"{_role} disconnected (id {connection.ConnectionId})");
            return Task.CompletedTask;
        }

        public override Task OnPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
        {
            if (_clientRouter != null && packet.OpCode == 21)
            {
                return _clientRouter.HandleAsync(connection, packet, cancellationToken);
            }

            if (_accountRouter != null && packet.OpCode == 1)
            {
                return _accountRouter.HandleAsync(connection, packet, cancellationToken);
            }

            _log($"{_role} data op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            return Task.CompletedTask;
        }

        public override Task OnInternalPacketAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
        {
            _log($"{_role} internal op={packet.OpCode} sub={packet.SubCode} len={packet.Payload.Length}");
            return Task.CompletedTask;
        }
    }
}
