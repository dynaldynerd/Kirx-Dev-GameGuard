using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net;

namespace LoginServer.State;

/// <summary>
/// Simplified CMainThread analogue to hold shared server state.
/// Singleton, thread-safe.
/// </summary>
public sealed class MainContext
{
    private readonly object _lock = new();
    private readonly WorldData[] _worlds = new WorldData[40];
    private readonly ConcurrentDictionary<ushort, ClientSession> _clients = new();

    private MainContext()
    {
    }

    public static MainContext Instance { get; } = new();

    public int WorldCount { get; private set; }
    public int ServiceWorldNum { get; private set; }

    public IReadOnlyList<WorldData> Worlds
    {
        get
        {
            lock (_lock)
            {
                return (WorldData[])_worlds.Clone();
            }
        }
    }

    public ClientSession? GetClient(ushort index)
    {
        _clients.TryGetValue(index, out var session);
        return session;
    }

    public void UpdateWorldList(byte serviceWorldNum, IReadOnlyList<WorldData> worlds)
    {
        lock (_lock)
        {
            WorldCount = Math.Min(worlds.Count, _worlds.Length);
            ServiceWorldNum = serviceWorldNum;
            for (int i = 0; i < _worlds.Length; i++)
            {
                _worlds[i] = i < worlds.Count ? worlds[i] : default;
            }
        }
    }

    public void SetWorldOpen(int worldIndex, IPAddress gateIp, ushort gatePort)
    {
        if (worldIndex < 0 || worldIndex >= _worlds.Length) return;
        lock (_lock)
        {
            var w = _worlds[worldIndex];
            w.IsOpen = true;
            w.GateIp = gateIp;
            w.GatePort = gatePort;
            _worlds[worldIndex] = w;
        }
    }

    public void SetWorldClosed(int worldIndex)
    {
        if (worldIndex < 0 || worldIndex >= _worlds.Length) return;
        lock (_lock)
        {
            var w = _worlds[worldIndex];
            w.IsOpen = false;
            _worlds[worldIndex] = w;
        }
    }

    public void UpdateUserCounts(byte serviceWorldNum, ushort[] counts)
    {
        lock (_lock)
        {
            ServiceWorldNum = serviceWorldNum;
            int len = Math.Min(counts.Length, _worlds.Length);
            for (int i = 0; i < len; i++)
            {
                var w = _worlds[i];
                w.UserCount = counts[i];
                _worlds[i] = w;
            }
        }
    }

    public void RecordJoinResult(ushort index, byte retCode)
    {
        var session = _clients.GetOrAdd(index, _ => new ClientSession(index));
        session.JoinRetCode = retCode;
    }

    public void RecordLoginResult(ushort index, byte retCode, uint accountSerial, byte userGrade, byte subGrade, int nTrans)
    {
        var session = _clients.GetOrAdd(index, _ => new ClientSession(index));
        session.LoginRetCode = retCode;
        session.AccountSerial = accountSerial;
        session.UserGrade = userGrade;
        session.SubGrade = subGrade;
        session.Trans = nTrans;
    }

    public void RecordSelectWorldResult(ushort index, byte retCode, uint[] masterKey)
    {
        var session = _clients.GetOrAdd(index, _ => new ClientSession(index));
        session.SelectRetCode = retCode;
        session.WorldMasterKey = masterKey;
    }

    public void RecordPushCloseResult(ushort index, byte retCode)
    {
        var session = _clients.GetOrAdd(index, _ => new ClientSession(index));
        session.PushCloseRetCode = retCode;
    }

    public void RecordForceClose(ushort index)
    {
        var session = _clients.GetOrAdd(index, _ => new ClientSession(index));
        session.ForcedClosed = true;
    }
}

public struct WorldData
{
    public bool IsOpen;
    public string Name;
    public IPAddress? GateIp;
    public ushort GatePort;
    public int UserCount;
    public byte Type;
    public bool FreeServer;
}

public sealed class ClientSession
{
    public ClientSession(ushort index)
    {
        Index = index;
    }

    public ushort Index { get; }
    public byte? JoinRetCode { get; set; }
    public byte? LoginRetCode { get; set; }
    public uint AccountSerial { get; set; }
    public byte UserGrade { get; set; }
    public byte SubGrade { get; set; }
    public int Trans { get; set; }
    public byte? SelectRetCode { get; set; }
    public uint[]? WorldMasterKey { get; set; }
    public byte? PushCloseRetCode { get; set; }
    public bool ForcedClosed { get; set; }
}
