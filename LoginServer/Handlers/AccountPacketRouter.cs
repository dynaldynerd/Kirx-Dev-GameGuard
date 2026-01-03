using System;
using System.Buffers.Binary;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using RFNetworking;

namespace LoginServer.Handlers;

/// <summary>
/// Mirrors CNetworkEX::AccountLineAnalysis dispatch for packets from account server (op 1).
/// </summary>
public sealed class AccountPacketRouter
{
    private readonly Action<string> _log;

    public AccountPacketRouter(Action<string> log)
    {
        _log = log;
    }

    public async Task<bool> HandleAsync(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        if (packet.OpCode != 1)
        {
            return false;
        }

        byte sub = packet.SubCode;
        switch (sub)
        {
            case 11:
                return await WorldListResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 100:
                return await InformOpenWorld(connection, packet, cancellationToken).ConfigureAwait(false);
            case 101:
                return await InformCloseWorld(connection, packet, cancellationToken).ConfigureAwait(false);
            case 102:
                return await InformUserNumWorld(connection, packet, cancellationToken).ConfigureAwait(false);
            case 2:
                return await JoinAccountResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 4:
                return await LoginAccountResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 6:
                return await SelectWorldResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 8:
                return await PushCloseResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 200:
                return await ForceCloseCommand(connection, packet, cancellationToken).ConfigureAwait(false);
            case 12:
                return await LoginStatRequest(connection, packet, cancellationToken).ConfigureAwait(false);
            case 14:
                return await HolyQuestNowStat(connection, packet, cancellationToken).ConfigureAwait(false);
            case 16:
                return await AccountDBInfoResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 19:
                return await NotifyManageAccountAuthInfo(connection, packet, cancellationToken).ConfigureAwait(false);
            case 21:
                return await ManageAccountAuthResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 23:
                return await ManageClientLimitRunAccountResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 24:
                return await ManageClientLimitRunWorldResult(connection, packet, cancellationToken).ConfigureAwait(false);
            case 26:
                return await ManageClientForceExitResult(connection, packet, cancellationToken).ConfigureAwait(false);
            default:
                _log($"Unhandled account opcode 1/{sub}");
                return false;
        }
    }

    private Task<bool> WorldListResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        const int entrySize = 41; // bool + 33 name + 4 IP + 2 port + 1 type

        var span = packet.Payload.AsSpan();
        if (span.Length < 2)
        {
            _log("WorldListResult: payload too small");
            return Task.FromResult(false);
        }

        byte serviceWorldNum = span[0];
        byte worldNum = span[1];
        if (worldNum > 40)
        {
            _log($"WorldListResult: invalid worldNum {worldNum}");
            return Task.FromResult(false);
        }

        int required = 2 + worldNum * entrySize;
        if (span.Length < required)
        {
            _log($"WorldListResult: payload length {span.Length} < required {required}");
            return Task.FromResult(false);
        }

        int offset = 2;
        for (int i = 0; i < worldNum; i++)
        {
            bool bOpen = span[offset] != 0;
            var nameBytes = span.Slice(offset + 1, 33);
            int zeroIdx = nameBytes.IndexOf((byte)0);
            int nameLen = zeroIdx >= 0 ? zeroIdx : 33;
            if (nameLen > 0x20)
            {
                _log($"WorldListResult: world {i} name too long ({nameLen})");
                return Task.FromResult(false);
            }

            uint gateIp = BinaryPrimitives.ReadUInt32LittleEndian(span.Slice(offset + 34, 4));
            ushort gatePort = BinaryPrimitives.ReadUInt16LittleEndian(span.Slice(offset + 38, 2));
            byte type = span[offset + 40];

            string name = System.Text.Encoding.ASCII.GetString(nameBytes.Slice(0, nameLen));
            string ipStr = new System.Net.IPAddress(gateIp).ToString();
            _log($"World {i}: open={bOpen} name='{name}' ip={ipStr}:{gatePort} type={type}");

            offset += entrySize;
        }

        _log($"WorldListResult: serviceWorldNum={serviceWorldNum} worldNum={worldNum}");
        return Task.FromResult(true);
    }

    private Task<bool> InformOpenWorld(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account InformOpenWorld len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> InformCloseWorld(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account InformCloseWorld len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> InformUserNumWorld(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account InformUserNumWorld len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> JoinAccountResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account JoinAccountResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginAccountResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account LoginAccountResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> SelectWorldResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account SelectWorldResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> PushCloseResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account PushCloseResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ForceCloseCommand(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account ForceCloseCommand len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> LoginStatRequest(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account LoginStatRequest len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> HolyQuestNowStat(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account HolyQuestNowStat len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> AccountDBInfoResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account AccountDBInfoResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> NotifyManageAccountAuthInfo(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account NotifyManageAccountAuthInfo len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageAccountAuthResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account ManageAccountAuthResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunAccountResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account ManageClientLimitRunAccountResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientLimitRunWorldResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account ManageClientLimitRunWorldResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }

    private Task<bool> ManageClientForceExitResult(PublicConnection connection, PacketEnvelope packet, CancellationToken cancellationToken)
    {
        _log($"Account ManageClientForceExitResult len={packet.Payload.Length}");
        return Task.FromResult(true);
    }
}
