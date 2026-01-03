using System;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>SocketAsyncEventArgs-based outbound connector (single connection).</summary>
public sealed class NetworkConnector : IAsyncDisposable
{
    private readonly INetworkHandler _handler;
    private SaeaConnection? _connection;
    private CancellationTokenSource? _cts;
    private Task? _runTask;

    public NetworkConnector(INetworkHandler handler)
    {
        _handler = handler;
    }

    public bool IsConnected => _connection?.Socket.Connected == true;

    public event Action<string>? Log;

    public async Task ConnectAsync(string host, int port, CancellationToken cancellationToken)
    {
        if (_connection != null)
        {
            throw new InvalidOperationException("Connector already running.");
        }

        _cts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        await socket.ConnectAsync(host, port, _cts.Token).ConfigureAwait(false);
        socket.NoDelay = true;

        _connection = new SaeaConnection(1, socket, _handler, Log);
        _connection.Closed += async (c, ex) => await OnClosedAsync(c, ex).ConfigureAwait(false);
        Log?.Invoke($"Connected to {host}:{port}");

        _runTask = Task.Run(async () =>
        {
            await _connection.OnConnectedAsync(CancellationToken.None).ConfigureAwait(false);
            _connection.StartReceive();
        }, CancellationToken.None);

        await Task.Yield();
    }

    public Task SendAsync(PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (_connection == null) throw new InvalidOperationException("Not connected.");
        return _connection.SendAsync(packet, cancellationToken);
    }

    public async Task StopAsync()
    {
        _cts?.Cancel();
        if (_runTask != null)
        {
            try { await _runTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
        }

        if (_connection != null)
        {
            await _handler.OnDisconnectedAsync(_connection.PublicConnection, CancellationToken.None).ConfigureAwait(false);
            await _connection.DisposeAsync().ConfigureAwait(false);
            _connection = null;
        }

        _cts?.Dispose();
        _cts = null;
        _runTask = null;
        Log?.Invoke("Connector stopped.");
    }

    private async Task OnClosedAsync(SaeaConnection connection, Exception? ex)
    {
        await _handler.OnDisconnectedAsync(connection.PublicConnection, CancellationToken.None).ConfigureAwait(false);
        await connection.DisposeAsync().ConfigureAwait(false);
        if (ex != null)
        {
            Log?.Invoke($"Connector error: {ex.Message}");
        }
        else
        {
            Log?.Invoke("Connector disconnected.");
        }
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync().ConfigureAwait(false);
    }
}
