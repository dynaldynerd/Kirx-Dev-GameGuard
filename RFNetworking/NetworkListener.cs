using System;
using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

/// <summary>SocketAsyncEventArgs-based TCP listener.</summary>
public sealed class NetworkListener : IAsyncDisposable
{
    private readonly INetworkHandler _handler;
    private readonly ConcurrentDictionary<ulong, SaeaConnection> _connections = new();
    private readonly ConcurrentQueue<ulong> _freeIds = new();
    private Socket? _listenSocket;
    private CancellationTokenSource? _cts;
    private Task? _acceptLoop;
    private long _nextId;
    private int _nextSerial;
    private readonly int _maxConnections;

    public NetworkListener(INetworkHandler handler, int maxConnections = -1)
    {
        _handler = handler;
        _maxConnections = maxConnections;
    }

    public bool IsRunning => _listenSocket != null;

    public int CurrentConnections => _connections.Count;

    public event Action<string>? Log;
    public PacketEnvelope PingPacket { get; set; } = new() { OpCode = 101, SubCode = 1, Payload = new byte[] { 0 } };
    public TimeSpan PingInterval { get; set; } = TimeSpan.FromSeconds(2.5);

    public async Task StartAsync(int port, CancellationToken cancellationToken)
    {
        await StartAsync(IPAddress.Any, port, cancellationToken).ConfigureAwait(false);
    }

    public async Task StartAsync(string host, int port, CancellationToken cancellationToken)
    {
        var bindAddress = ResolveBindAddress(host);
        await StartAsync(bindAddress, port, cancellationToken).ConfigureAwait(false);
    }

    private async Task StartAsync(IPAddress bindAddress, int port, CancellationToken cancellationToken)
    {
        if (_listenSocket != null)
        {
            throw new InvalidOperationException("Listener already running.");
        }

        _cts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        _listenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        _listenSocket.Bind(new IPEndPoint(bindAddress, port));
        _listenSocket.Listen(512);
        Log?.Invoke($"Listening on {bindAddress}:{port}");

        _acceptLoop = Task.Run(() => AcceptLoopAsync(_cts.Token), CancellationToken.None);
        await Task.Yield();
    }

    private static IPAddress ResolveBindAddress(string host)
    {
        if (string.IsNullOrWhiteSpace(host) || host == "0.0.0.0" || host == "*" || host == "any")
        {
            return IPAddress.Any;
        }

        if (IPAddress.TryParse(host, out var ip))
        {
            return ip;
        }

        try
        {
            var addresses = Dns.GetHostAddresses(host);
            foreach (var address in addresses)
            {
                if (address.AddressFamily == AddressFamily.InterNetwork)
                {
                    return address;
                }
            }
        }
        catch
        {
            return IPAddress.Any;
        }

        return IPAddress.Any;
    }

    public async Task StopAsync()
    {
        if (_listenSocket == null)
        {
            return;
        }

        _cts?.Cancel();
        try { _listenSocket.Close(); } catch { }
        _listenSocket = null;

        if (_acceptLoop != null)
        {
            try { await _acceptLoop.ConfigureAwait(false); } catch (OperationCanceledException) { }
        }

        foreach (var conn in _connections.Values)
        {
            await _handler.OnDisconnectedAsync(conn.PublicConnection, CancellationToken.None).ConfigureAwait(false);
            await conn.DisposeAsync().ConfigureAwait(false);
        }

        _connections.Clear();
        _cts?.Dispose();
        _cts = null;
        _acceptLoop = null;
        Log?.Invoke("Listener stopped.");
    }

    private async Task AcceptLoopAsync(CancellationToken cancellationToken)
    {
        while (!cancellationToken.IsCancellationRequested)
        {
            Socket? client = null;
            try
            {
                client = await _listenSocket!.AcceptAsync(cancellationToken).ConfigureAwait(false);
                client.NoDelay = true;
                if (_maxConnections > 0 && _connections.Count >= _maxConnections && _freeIds.IsEmpty)
                {
                    Log?.Invoke($"Connection refused: limit {_maxConnections} reached.");
                    client.Dispose();
                    continue;
                }

                ulong id;
                if (!_freeIds.TryDequeue(out id))
                {
                    id = (ulong)Interlocked.Increment(ref _nextId);
                }

                uint serial = (uint)Interlocked.Increment(ref _nextSerial);
                var conn = new SaeaConnection(id, serial, client, _handler, Log);
                _connections[id] = conn;
                Log?.Invoke($"Client {id} connected from {client.RemoteEndPoint}");

                conn.Closed += async (c, ex) => await OnClosedAsync(c, ex).ConfigureAwait(false);
                await conn.OnConnectedAsync(CancellationToken.None).ConfigureAwait(false);
                StartPingLoop(conn);
                conn.StartReceive();
            }
            catch (OperationCanceledException)
            {
                break;
            }
            catch (Exception ex)
            {
                client?.Dispose();
                if (!cancellationToken.IsCancellationRequested)
                {
                    Log?.Invoke($"Accept error: {ex.Message}");
                }
            }
        }
    }

    private async Task OnClosedAsync(SaeaConnection connection, Exception? ex)
    {
        _connections.TryRemove(connection.ConnectionId, out _);
        _freeIds.Enqueue(connection.ConnectionId);
        await _handler.OnDisconnectedAsync(connection.PublicConnection, CancellationToken.None).ConfigureAwait(false);
        await connection.DisposeAsync().ConfigureAwait(false);
        if (ex != null)
        {
            Log?.Invoke($"Client {connection.ConnectionId} error: {ex.Message}");
        }
        else
        {
            Log?.Invoke($"Client {connection.ConnectionId} disconnected.");
        }
    }

    private void StartPingLoop(SaeaConnection conn)
    {
        conn.StartPing(PingPacket, PingInterval, Log);
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync().ConfigureAwait(false);
    }
}
