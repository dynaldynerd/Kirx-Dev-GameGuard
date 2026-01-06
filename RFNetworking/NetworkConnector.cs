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
        if (_connection != null)
        {
            throw new InvalidOperationException("Connector already running.");
        }

        _cts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        await socket.ConnectAsync(host, port, _cts.Token).ConfigureAwait(false);
        socket.NoDelay = true;

        uint serial = (uint)Interlocked.Increment(ref _nextSerial);
        _connection = new SaeaConnection(1, serial, socket, _handler, Log);
        _connection.Closed += async (c, ex) => await OnClosedAsync(c, ex).ConfigureAwait(false);
        Log?.Invoke($"Connected to {host}:{port}");

        _runTask = Task.Run(async () =>
        {
            await _connection.OnConnectedAsync(CancellationToken.None).ConfigureAwait(false);
            _connection.StartReceive();
            StartPingLoop(_cts.Token);
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
        if (_pingTask != null)
        {
            try { await _pingTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
            _pingTask = null;
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
        if (_pingTask != null)
        {
            try { await _pingTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
            _pingTask = null;
        }
        if (ex != null)
        {
            Log?.Invoke($"Connector error: {ex.Message}");
        }
        else
        {
            Log?.Invoke("Connector disconnected.");
        }
    }

    private void StartPingLoop(CancellationToken token)
    {
        if (_connection == null) return;
        if (_pingTask != null && !_pingTask.IsCompleted) return;

        var packet = PingPacket;
        _pingTask = Task.Run(async () =>
        {
            while (!token.IsCancellationRequested && _connection != null)
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
                    await _connection.SendAsync(packet, token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    Log?.Invoke($"Connector ping failed: {ex.Message}");
                }

            }
        }, token);
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync().ConfigureAwait(false);
    }
}
