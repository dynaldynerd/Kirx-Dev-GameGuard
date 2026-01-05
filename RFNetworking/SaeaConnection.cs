using System;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace RFNetworking;

internal sealed class SaeaConnection : IAsyncDisposable
{
    private readonly Socket _socket;
    private readonly INetworkHandler _handler;
    private readonly InternalPacketProcessor _internalProcessor;
    private readonly byte[] _buffer;
    private readonly PublicConnection _publicConnection;
    private readonly SocketAsyncEventArgs _receiveArgs;
    private readonly SemaphoreSlim _sendLock = new(1, 1);
    private int _received;
    private bool _disposed;
    private Task? _pingTask;
    private CancellationTokenSource? _pingCts;

    public SaeaConnection(ulong id, uint serial, Socket socket, INetworkHandler handler, Action<string>? log)
    {
        ConnectionId = id;
        Serial = serial;
        _socket = socket;
        _handler = handler;
        _internalProcessor = new InternalPacketProcessor(log);
        Log = log;

        _buffer = BufferPool.Rent();
        _publicConnection = new PublicConnection(this, socket);
        _receiveArgs = new SocketAsyncEventArgs();
        _receiveArgs.SetBuffer(_buffer, 0, _buffer.Length);
        _receiveArgs.Completed += OnReceiveCompleted;
    }

    public ulong ConnectionId { get; }
    public Action<string>? Log { get; }
    public uint Serial { get; }
    internal PublicConnection PublicConnection => _publicConnection;
    internal Socket Socket => _socket;

    public event Action<SaeaConnection, Exception?>? Closed;

    public void StartReceive()
    {
        if (!ReceiveAsync())
        {
            ProcessReceive(_receiveArgs);
        }
    }

    public Task OnConnectedAsync(CancellationToken cancellationToken) =>
        _handler.OnConnectedAsync(_publicConnection, cancellationToken);

    public async Task SendAsync(PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (_disposed) throw new ObjectDisposedException(nameof(SaeaConnection));

        await _sendLock.WaitAsync(cancellationToken).ConfigureAwait(false);
        try
        {
            int size = PacketFramer.HeaderSize + (packet.Payload?.Length ?? 0);
            var buffer = BufferPool.Rent(size);
            try
            {
                int written = PacketFramer.WritePacket(buffer.AsSpan(0, buffer.Length), packet);
                var segment = new ArraySegment<byte>(buffer, 0, written);
                int sent = 0;
                while (sent < written)
                {
                    int s = await _socket.SendAsync(segment.Slice(sent), SocketFlags.None, cancellationToken).ConfigureAwait(false);
                    if (s == 0)
                    {
                        throw new SocketException((int)SocketError.ConnectionReset);
                    }
                    sent += s;
                }
            }
            finally
            {
                BufferPool.Return(buffer);
            }
        }
        finally
        {
            _sendLock.Release();
        }
    }

    private bool ReceiveAsync()
    {
        _receiveArgs.SetBuffer(_received, _buffer.Length - _received);
        return _socket.ReceiveAsync(_receiveArgs);
    }

    private async void OnReceiveCompleted(object? sender, SocketAsyncEventArgs e)
    {
        await Task.Yield(); // ensure async context
        ProcessReceive(e);
    }

    private void ProcessReceive(SocketAsyncEventArgs e)
    {
        if (_disposed) return;

        if (e.BytesTransferred == 0 || e.SocketError != SocketError.Success)
        {
            Close(new SocketException((int)e.SocketError));
            return;
        }

        _received += e.BytesTransferred;

        try
        {
            ProcessBuffer();
        }
        catch (Exception ex)
        {
            Close(ex);
            return;
        }

        if (!ReceiveAsync())
        {
            ProcessReceive(_receiveArgs);
        }
    }

    private void ProcessBuffer()
    {
        int offset = 0;
        while (true)
        {
            var span = new ReadOnlySpan<byte>(_buffer, offset, _received - offset);
            if (!PacketFramer.TryExtractPacket(span, out var packet, out int consumed))
            {
                break;
            }

            offset += consumed;
            if (packet != null)
            {
                _ = DispatchPacketAsync(packet);
            }
        }

        if (offset > 0 && offset < _received)
        {
            Buffer.BlockCopy(_buffer, offset, _buffer, 0, _received - offset);
        }

        _received -= offset;
    }

    private async Task DispatchPacketAsync(PacketEnvelope packet)
    {
        try
        {
            if (packet.OpCode > 100)
            {
                bool handled = await _internalProcessor.HandleAsync(_publicConnection, packet, CancellationToken.None).ConfigureAwait(false);
                if (handled)
                {
                    return;
                }

                await _handler.OnInternalPacketAsync(_publicConnection, packet, CancellationToken.None).ConfigureAwait(false);
                return;
            }

            await _handler.OnPacketAsync(_publicConnection, packet, CancellationToken.None).ConfigureAwait(false);
        }
        catch (Exception ex)
        {
            Log?.Invoke($"Packet handler error: {ex.Message}");
            Close(ex);
        }
    }

    private void Close(Exception? ex)
    {
        if (_disposed) return;
        _disposed = true;
        Closed?.Invoke(this, ex);
    }

    internal void RequestClose(Exception? ex = null)
    {
        Close(ex);
    }

    internal void StartPing(PacketEnvelope packet, TimeSpan interval, Action<string>? log)
    {
        if (_pingTask != null && !_pingTask.IsCompleted) return;
        _pingCts?.Cancel();
        _pingCts?.Dispose();
        _pingCts = new CancellationTokenSource();
        var token = _pingCts.Token;
        _pingTask = Task.Run(async () =>
        {
            while (!token.IsCancellationRequested && !_disposed)
            {
                try
                {
                    await SendAsync(packet, token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    log?.Invoke($"Ping error: {ex.Message}");
                    break;
                }

                try
                {
                    await Task.Delay(interval, token).ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
            }
        }, token);
    }

    public async ValueTask DisposeAsync()
    {
        if (_disposed) return;
        _disposed = true;
        _pingCts?.Cancel();
        if (_pingTask != null)
        {
            try { await _pingTask.ConfigureAwait(false); } catch (OperationCanceledException) { }
        }
        _pingCts?.Dispose();
        try { _socket.Shutdown(SocketShutdown.Both); } catch { }
        try { _socket.Dispose(); } catch { }
        BufferPool.Return(_buffer);
        _receiveArgs.Dispose();
        _sendLock.Dispose();
        await Task.CompletedTask;
    }
}
