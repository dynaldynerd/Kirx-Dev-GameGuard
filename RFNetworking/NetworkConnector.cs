using System;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>SocketAsyncEventArgs-based outbound connector (single connection).</summary>
public sealed class NetworkConnector : IAsyncDisposable
{
    private readonly INetworkHandler _handler;
    private readonly object _sync = new();
    private SaeaConnection? _connection;
    private CancellationTokenSource? _cts;
    private Task? _runTask;
    private Task? _pingTask;
    private TimeSpan _pingInterval = TimeSpan.FromSeconds(2.5);
    private int _nextSerial;

    public NetworkConnector(INetworkHandler handler)
    {
        _handler = handler;
    }

    public bool IsConnected => _connection?.Socket.Connected == true;
    /// <summary>Optional ping packet to send periodically while connected.</summary>
    public PacketEnvelope PingPacket { get; set; } = new() { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } };
    /// <summary>Ping interval (default 2.5s).</summary>
    public TimeSpan PingInterval
    {
        get => _pingInterval;
        set => _pingInterval = value <= TimeSpan.Zero ? TimeSpan.FromSeconds(2.5) : value;
    }

    public event Action<string>? Log;

    public async Task ConnectAsync(string host, int port, CancellationToken cancellationToken)
    {
        lock (_sync)
        {
            if (_connection != null)
            {
                throw new InvalidOperationException("Connector already running.");
            }
        }

        CancellationTokenSource linkedCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        try
        {
            await socket.ConnectAsync(host, port, linkedCts.Token).ConfigureAwait(false);
            socket.NoDelay = true;

            uint serial = (uint)Interlocked.Increment(ref _nextSerial);
            var connection = new SaeaConnection(1, serial, socket, _handler, Log);
            connection.Closed += async (c, ex) => await OnClosedAsync(c, ex).ConfigureAwait(false);

            lock (_sync)
            {
                if (_connection != null)
                {
                    throw new InvalidOperationException("Connector already running.");
                }

                _cts = linkedCts;
                _connection = connection;
            }

            Log?.Invoke($"Connected to {host}:{port}");

            _runTask = Task.Run(async () =>
            {
                await connection.OnConnectedAsync(CancellationToken.None).ConfigureAwait(false);
                connection.StartReceive();
                StartPingLoop(connection, linkedCts.Token);
            }, CancellationToken.None);
        }
        catch
        {
            try { socket.Dispose(); } catch { }
            linkedCts.Dispose();
            throw;
        }

        await Task.Yield();
    }

    public Task SendAsync(PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (_connection == null) throw new InvalidOperationException("Not connected.");
        return _connection.SendAsync(packet, cancellationToken);
    }

    public async Task StopAsync()
    {
        CancellationTokenSource? cts;
        SaeaConnection? connection;
        Task? runTask;
        Task? pingTask;

        lock (_sync)
        {
            cts = _cts;
            connection = _connection;
            runTask = _runTask;
            pingTask = _pingTask;
            _cts = null;
            _connection = null;
            _runTask = null;
            _pingTask = null;
        }

        cts?.Cancel();
        if (runTask != null)
        {
            try { await runTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
        }

        if (connection != null)
        {
            await _handler.OnDisconnectedAsync(connection.PublicConnection, CancellationToken.None).ConfigureAwait(false);
            await connection.DisposeAsync().ConfigureAwait(false);
        }
        if (pingTask != null)
        {
            try { await pingTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
        }

        cts?.Dispose();
        Log?.Invoke("Connector stopped.");
    }

    private async Task OnClosedAsync(SaeaConnection connection, Exception? ex)
    {
        CancellationTokenSource? cts = null;
        Task? pingTask = null;

        lock (_sync)
        {
            if (ReferenceEquals(_connection, connection))
            {
                _connection = null;
                _runTask = null;
                cts = _cts;
                _cts = null;
            }

            pingTask = _pingTask;
            _pingTask = null;
        }

        cts?.Cancel();
        await _handler.OnDisconnectedAsync(connection.PublicConnection, CancellationToken.None).ConfigureAwait(false);
        await connection.DisposeAsync().ConfigureAwait(false);
        if (pingTask != null)
        {
            try { await pingTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
        }
        cts?.Dispose();
        if (ex != null)
        {
            Log?.Invoke($"Connector error: {ex.Message}");
        }
        else
        {
            Log?.Invoke("Connector disconnected.");
        }
    }

    private void StartPingLoop(SaeaConnection connection, CancellationToken token)
    {
        if (_pingTask != null && !_pingTask.IsCompleted) return;

        var packet = PingPacket;
        _pingTask = Task.Run(async () =>
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    await Task.Delay(_pingInterval, token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }

                try
                {
                    await connection.SendAsync(packet, token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    Log?.Invoke($"Connector ping failed: {ex.Message}");
                    connection.RequestClose(ex);
                    break;
                }

            }
        }, token);
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync().ConfigureAwait(false);
    }
}
