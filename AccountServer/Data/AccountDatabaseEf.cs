using System;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using AccountServer.Security;
using AccountServer.Settings;
using Microsoft.EntityFrameworkCore;
using Pomelo.EntityFrameworkCore.MySql.Infrastructure;

namespace AccountServer.Data;

/// <summary>
/// EF Core implementation of the account DB contract.
/// Mirrors the stored-proc behavior using EF queries/updates.
/// </summary>
public sealed class AccountDatabaseEf : IAccountDatabase
{
    private const string LimitRunParam1Hex =
        "0xEFC6616D39F3CAB14D9D6264B2552A4A2D365809F15163DEC0A05B4A233CD4593F62B0F2FDDAEB0C45A55CB21662DD5326EE58D35AF3CAC85FCC128BD0CB96F6";

    private readonly DbContextOptions<AccountDbContext> _options;
    private readonly DatabaseProvider _provider;
    private readonly byte[] _hmacKey;
    private readonly byte[] _aesKey;
    private readonly byte[] _limitRunParam1Bytes;
    public AccountDatabaseEf(DatabaseProvider provider, string connectionString, SecuritySettings security)
    {
        _provider = provider;
        _hmacKey = Convert.FromBase64String(security.Argon2SaltBase64);
        _aesKey = SHA256.HashData(_hmacKey);
        _limitRunParam1Bytes = HexToBytes(LimitRunParam1Hex);

        var builder = new DbContextOptionsBuilder<AccountDbContext>();
        switch (provider)
        {
            case DatabaseProvider.Sqlite:
                builder.UseSqlite(connectionString);
                break;
            case DatabaseProvider.MariaDb:
                builder.UseMySql(connectionString, new MariaDbServerVersion(new Version(10, 4, 0)));
                break;
            default:
                builder.UseSqlServer(connectionString);
                break;
        }

        _options = builder.Options;
    }

    private AccountDbContext CreateContext() => new(_options);

    private byte[] ComputeIdHmac(string id)
    {
        return CryptoHelper.ComputeHmacSha256(Encoding.UTF8.GetBytes(id), _hmacKey);
    }

    private byte[] EncryptId(string id)
    {
        return CryptoHelper.EncryptAesGcm(_aesKey, Encoding.UTF8.GetBytes(id));
    }

    private static byte[] HexToBytes(string hex)
    {
        string cleaned = hex.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? hex[2..] : hex;
        return Convert.FromHexString(cleaned);
    }

    public async Task<StaffInfoResult> Select_StaffInfoExAsync(string id, CancellationToken token)
    {
        var result = new StaffInfoResult { Ret = 1, Info = new StaffInfo(), LoginDate = DateTime.MinValue };
        try
        {
            var idHmac = ComputeIdHmac(id);
            await using var db = CreateContext();
            var staff = await db.StaffAccounts.AsNoTracking()
                .FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (staff == null)
            {
                result.Ret = 2;
                return result;
            }

            result.Info.Password = staff.PwHash.TrimEnd('\0');
            result.Info.Serial = (uint)staff.Serial;
            result.Info.TotalLogMin = (uint)staff.TotalLogMin;
            result.Info.Grade = staff.Grade;
            result.Info.SubGrade = staff.SubGrade;
            result.LoginDate = DateTime.Now;
            result.Ret = 0;
            return result;
        }
        catch
        {
            return result;
        }
    }

    public async Task<bool> Select_StaffExpireAsync(string id, CancellationToken token)
    {
        try
        {
            var idHmac = ComputeIdHmac(id);
            await using var db = CreateContext();
            var staff = await db.StaffAccounts.AsNoTracking()
                .FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (staff == null)
            {
                return false;
            }

            return staff.ExpireDt > DateTime.Now;
        }
        catch
        {
            return false;
        }
    }

    public async Task<UserInfoResult> Select_UserInfoAsync(string id, CancellationToken token)
    {
        var result = new UserInfoResult { Ret = 1 };
        try
        {
            var idHmac = ComputeIdHmac(id);
            await using var db = CreateContext();
            var user = await db.UserAccounts.AsNoTracking()
                .FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                result.Ret = 2;
                return result;
            }

            result.Serial = (uint)user.Serial;
            result.TotalLogMin = (uint)user.TotalLogMin;
            result.LoginDate = DateTime.Now;
            result.UiLock = user.UiLock;
            result.UiLockPw = user.UiLockPw.TrimEnd('\0');
            result.UiLockFailCnt = user.UiLockFailCnt;
            result.HintIndex = user.UiLockHintIndex;
            result.HintAnswer = user.UiLockHintAnswer.TrimEnd('\0');
            result.UiLockFindPassFailCount = user.UiLockFindPassFailCount;
            result.Ret = 0;
            return result;
        }
        catch
        {
            return result;
        }
    }

    public async Task<byte> Exist_UserBanAsync(uint accountSerial, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var ban = await db.UserBans.AsNoTracking()
                .FirstOrDefaultAsync(x => x.AccountSerial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (ban == null)
            {
                return 0;
            }

            if (ban.Period == 999)
            {
                return 2;
            }

            var banEnd = ban.StartDate.AddHours(ban.Period);
            if (banEnd <= DateTime.Now)
            {
                return 1;
            }

            return ban.Kind == 0 ? (byte)2 : (byte)3;
        }
        catch
        {
            return 1;
        }
    }

    public async Task<(int Ret, string BlockReason)> Select_UserAccountBlockInfoAsync(uint accountSerial, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var ban = await db.UserBans.AsNoTracking()
                .FirstOrDefaultAsync(x => x.AccountSerial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (ban == null)
            {
                return (2, string.Empty);
            }
            return (0, (ban.GmReason ?? string.Empty).TrimEnd('\0'));
        }
        catch
        {
            return (1, string.Empty);
        }
    }

    public async Task<byte> Update_User_StateAsync(uint serial, byte state, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var row = await db.UserCurrentStates.FirstOrDefaultAsync(x => x.Serial == (int)serial, token)
                .ConfigureAwait(false);
            if (row == null)
            {
                return 1;
            }

            row.State = state;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public async Task<bool> Insert_User_StateAsync(uint serial, byte state, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var exists = await db.UserCurrentStates.AsNoTracking()
                .AnyAsync(x => x.Serial == (int)serial, token)
                .ConfigureAwait(false);
            if (exists)
            {
                return false;
            }

            db.UserCurrentStates.Add(new UserCurrentState { Serial = (int)serial, State = state });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_UserAsync(string id, string ip, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var idHmac = ComputeIdHmac(id);
            var exists = await db.UserAccounts.AsNoTracking()
                .AnyAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (exists)
            {
                return true;
            }

            var now = DateTime.Now;
            var user = new UserAccount
            {
                IdHmac = idHmac,
                IdEnc = EncryptId(id),
                CreateTime = now,
                CreateIp = ip,
                LastLoginTime = now,
                LastLogoffTime = now,
                TotalLogMin = 0,
                LastConnectIp = ip,
                PushCloseTime = now,
                PusherIp = ip,
                JoinCode = 0,
                LoginFailureCnt = 0,
                FireOn = now,
                FireWarning = now,
                UiLock = 0,
                UiLockPw = string.Empty,
                UiLockFailCnt = 0,
                UiLockUpdate = now,
                UiLockHintIndex = 0,
                UiLockHintAnswer = string.Empty,
                UiLockFindPassFailCount = 0
            };

            db.UserAccounts.Add(user);
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_UserPushLogAsync(int serial, string pushIP, string closeIP, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            db.UserPushLogs.Add(new UserPushLog
            {
                AccountSerial = serial,
                Date = DateTime.Now,
                PushIp = pushIP,
                CloseIp = closeIP
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<(bool Ok, uint Serial, DateTime Date)> Select_UserSerialAsync(string id, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var idHmac = ComputeIdHmac(id);
            var user = await db.UserAccounts.AsNoTracking()
                .FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return (false, 0, DateTime.MinValue);
            }
            return (true, (uint)user.Serial, DateTime.Now);
        }
        catch
        {
            return (false, 0, DateTime.MinValue);
        }
    }

    public async Task<bool> Update_StaffLoginDateAsync(string id, string ip, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var idHmac = ComputeIdHmac(id);
            var staff = await db.StaffAccounts.FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (staff == null)
            {
                return false;
            }

            staff.LastLoginDt = DateTime.Now;
            staff.LastConnIp = ip;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Update_UserLoginDateAsync(string id, string ip, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var idHmac = ComputeIdHmac(id);
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return false;
            }

            user.LastLoginTime = DateTime.Now;
            user.LastConnectIp = ip;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Update_Login_Failure_CountAsync(string id, byte byType, CancellationToken token)
    {
        if (byType != 0 && byType != 1)
        {
            return false;
        }

        try
        {
            await using var db = CreateContext();
            var idHmac = ComputeIdHmac(id);
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return false;
            }

            if (byType == 0)
            {
                user.LoginFailureCnt++;
            }
            else
            {
                user.LoginFailureCnt = 0;
            }

            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<(byte Ret, uint Serial)> Select_Limit_Run_Record_SerialAsync(CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            GmActParam? row;
            try
            {
                row = await db.GmActParams.AsNoTracking()
                    .FirstOrDefaultAsync(x => x.Param1 != null && x.Param1 == _limitRunParam1Bytes, token)
                    .ConfigureAwait(false);
            }
            catch
            {
                var all = await db.GmActParams.AsNoTracking().ToListAsync(token).ConfigureAwait(false);
                row = all.FirstOrDefault(x => x.Param1 != null && x.Param1.SequenceEqual(_limitRunParam1Bytes));
            }

            if (row == null)
            {
                return (2, 0);
            }
            return (0, (uint)row.Id);
        }
        catch
        {
            return (1, 0);
        }
    }

    public async Task<byte> Insert_Set_Limit_RunAsync(string param1Str, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            db.GmActParams.Add(new GmActParam
            {
                Param1 = _limitRunParam1Bytes,
                Param2 = Encoding.ASCII.GetBytes(param1Str)
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public async Task<byte> Update_Set_Limit_RunAsync(uint serial, string param1Str, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var row = await db.GmActParams.FirstOrDefaultAsync(x => x.Id == (int)serial, token)
                .ConfigureAwait(false);
            if (row == null)
            {
                return 1;
            }
            row.Param2 = Encoding.ASCII.GetBytes(param1Str);
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public async Task<bool> Insert_UserCountLogAsync(uint averageUser, uint maxUser, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            db.UserCountLogs.Add(new UserCountLog
            {
                Date = DateTime.Now,
                AverageUser = (int)averageUser,
                MaxUser = (int)maxUser,
                MaxUserTestServer = 0
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_UserCountLogAsync(uint averageUser, uint maxUserRegular, uint maxUserTestServer, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            db.UserCountLogs.Add(new UserCountLog
            {
                Date = DateTime.Now,
                AverageUser = (int)averageUser,
                MaxUser = (int)maxUserRegular,
                MaxUserTestServer = (int)maxUserTestServer
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_ServerUserLogAsync(
        uint averageUser,
        uint maxUser,
        uint[] playerPerRace,
        string serverName,
        string logDate,
        CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            uint race0 = playerPerRace != null && playerPerRace.Length > 0 ? playerPerRace[0] : 0;
            uint race1 = playerPerRace != null && playerPerRace.Length > 1 ? playerPerRace[1] : 0;
            uint race2 = playerPerRace != null && playerPerRace.Length > 2 ? playerPerRace[2] : 0;
            if (!DateTime.TryParse(logDate, out var date))
            {
                date = DateTime.Now;
            }
            db.ServerUserLogs.Add(new ServerUserLog
            {
                Date = date,
                AverageUser = (int)averageUser,
                MaxUser = (int)maxUser,
                ServerName = serverName,
                BellaUser = (int)race0,
                CoraUser = (int)race1,
                AccUser = (int)race2
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Update_WorldServerAsync(int serial, int state, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var world = await db.WorldServerLists.FirstOrDefaultAsync(x => x.Serial == serial, token)
                .ConfigureAwait(false);
            if (world == null)
            {
                return false;
            }
            world.State = state;
            world.LastUpdate = DateTime.Now;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<(byte Ret, byte Kind, ushort Period, string StartDate)> Select_UserBanAsync(
        uint accountSerial,
        CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var ban = await db.UserBans.AsNoTracking()
                .FirstOrDefaultAsync(x => x.AccountSerial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (ban == null)
            {
                return (2, 0, 0, string.Empty);
            }
            return (0, ban.Kind, (ushort)ban.Period, ban.StartDate.ToString("yyyy-MM-dd HH:mm:ss"));
        }
        catch
        {
            return (1, 0, 0, string.Empty);
        }
    }

    public async Task<bool> Insert_UserBanAsync(
        uint accountSerial,
        byte kind,
        uint period,
        string reason,
        string writer,
        byte reasonType,
        CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var existing = await db.UserBans.FirstOrDefaultAsync(x => x.AccountSerial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (existing != null)
            {
                existing.Period = (int)period;
                existing.Kind = kind;
                existing.StartDate = DateTime.Now;
                existing.Reason = reason;
                existing.GmReason = reason;
                existing.GmWriter = writer;
                existing.ReasonType = reasonType;
            }
            else
            {
                db.UserBans.Add(new UserBan
                {
                    AccountSerial = (int)accountSerial,
                    StartDate = DateTime.Now,
                    Period = (int)period,
                    Kind = kind,
                    Reason = reason,
                    GmReason = reason,
                    GmWriter = writer,
                    ReasonType = reasonType
                });
            }

            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Update_UserBanAsync(uint accountSerial, byte kind, uint period, string reason, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var ban = await db.UserBans.FirstOrDefaultAsync(x => x.AccountSerial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (ban == null)
            {
                return false;
            }
            ban.Period = (int)period;
            ban.Kind = kind;
            ban.StartDate = DateTime.Now;
            ban.Reason = reason;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_UserBan_LogAsync(uint accountSerial, byte kind, uint period, string reason, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            db.UserBanLogs.Add(new UserBanLog
            {
                AccountSerial = (int)accountSerial,
                StartDate = DateTime.Now,
                Period = (int)period,
                Kind = kind,
                Reason = reason,
                GmReason = reason,
                GmWriter = "system"
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_UserBan_ApexAsync(uint accountSerial, CancellationToken token)
    {
        try
        {
            const string reason = "APEX";
            await using var db = CreateContext();
            var ban = new UserBan
            {
                AccountSerial = (int)accountSerial,
                StartDate = DateTime.Now,
                Period = 999,
                Kind = 0,
                Reason = reason,
                GmReason = reason,
                GmWriter = "Apex",
                ReasonType = 0
            };
            db.UserBans.Add(ban);
            db.UserBanLogs.Add(new UserBanLog
            {
                AccountSerial = (int)accountSerial,
                StartDate = DateTime.Now,
                Period = 999,
                Kind = 0,
                Reason = reason,
                GmReason = reason,
                GmWriter = "Apex"
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<byte> IsBlockIPAsync(uint ip, CancellationToken token)
    {
        try
        {
            byte b0 = (byte)(ip & 0xFF);
            byte b1 = (byte)((ip >> 8) & 0xFF);
            byte b2 = (byte)((ip >> 16) & 0xFF);
            byte b3 = (byte)((ip >> 24) & 0xFF);

            await using var db = CreateContext();
            bool exists = await db.BlockIps.AsNoTracking()
                .AnyAsync(
                    x => x.Addr0 == b0
                         && x.Addr1 == b1
                         && b2 >= x.Addr2
                         && b2 <= x.Addr2Range
                         && b3 >= x.Addr3
                         && b3 <= x.Addr4,
                    token)
                .ConfigureAwait(false);
            return exists ? (byte)0 : (byte)2;
        }
        catch
        {
            return 1;
        }
    }

    public async Task<bool> Insert_BlockIPAsync(uint ip, CancellationToken token)
    {
        try
        {
            byte b0 = (byte)(ip & 0xFF);
            byte b1 = (byte)((ip >> 8) & 0xFF);
            byte b2 = (byte)((ip >> 16) & 0xFF);
            byte b3 = (byte)((ip >> 24) & 0xFF);

            await using var db = CreateContext();
            db.BlockIps.Add(new BlockIp
            {
                Addr0 = b0,
                Addr1 = b1,
                Addr2 = b2,
                Addr3 = b3,
                Addr4 = b3,
                Addr2Range = b2
            });
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<(byte Ret, int AccountCount, int IpCount)> Fireguard_Block_Type1Async(
        string accountId,
        uint ip,
        CancellationToken token)
    {
        try
        {
            var now = DateTime.Now;
            var cutoff = now.AddHours(-12);
            await using var db = CreateContext();
            db.FgDetectedAccounts.Add(new FgDetectedAccount { Account = accountId, DetectTime = now });
            db.FgDetectedIps.Add(new FgDetectedIp { Ip = unchecked((int)ip), DetectTime = now });
            await db.SaveChangesAsync(token).ConfigureAwait(false);

            int accountCount = await db.FgDetectedAccounts.AsNoTracking()
                .CountAsync(x => x.Account == accountId && x.DetectTime >= cutoff, token)
                .ConfigureAwait(false);
            int ipCount = await db.FgDetectedIps.AsNoTracking()
                .CountAsync(x => x.Ip == unchecked((int)ip) && x.DetectTime >= cutoff, token)
                .ConfigureAwait(false);
            return (0, accountCount, ipCount);
        }
        catch
        {
            return (1, 0, 0);
        }
    }

    public async Task<(byte Ret, int ErrorCode, int ResultCode)> Check_Fireguard_BlockAsync(
        string accountId,
        CancellationToken token)
    {
        try
        {
            const int targetMinute = 30;
            const byte reasonType = 3;

            await using var db = CreateContext();
            var idHmac = ComputeIdHmac(accountId);
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.IdHmac == idHmac, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return (0, 0, -1);
            }

            int ret;
            var now = DateTime.Now;
            var diffOn = (int)(now - user.FireOn).TotalMinutes;
            var diffWarning = (int)(now - user.FireWarning).TotalMinutes;

            if (user.LoginFailureCnt == 0)
            {
                user.LoginFailureCnt = 1;
                user.FireOn = now;
                ret = 1;
            }
            else if (user.LoginFailureCnt == 1)
            {
                if (diffOn <= targetMinute)
                {
                    user.LoginFailureCnt = 2;
                    user.FireWarning = now;
                    ret = 2;
                }
                else
                {
                    user.LoginFailureCnt = 1;
                    user.FireOn = now;
                    ret = 1;
                }
            }
            else if (user.LoginFailureCnt == 2)
            {
                if (diffOn <= targetMinute)
                {
                    const string reason = "Fireguard";
                    db.UserBans.Add(new UserBan
                    {
                        AccountSerial = user.Serial,
                        StartDate = now,
                        Period = 999,
                        Kind = 0,
                        Reason = reason,
                        GmReason = reason,
                        GmWriter = "Fireguard",
                        ReasonType = reasonType
                    });
                    db.UserBanLogs.Add(new UserBanLog
                    {
                        AccountSerial = user.Serial,
                        StartDate = now,
                        Period = 999,
                        Kind = 0,
                        Reason = reason,
                        GmReason = reason,
                        GmWriter = "Fireguard"
                    });
                    user.LoginFailureCnt = 0;
                    ret = 3;
                }
                else if (diffWarning <= targetMinute)
                {
                    user.FireOn = user.FireWarning;
                    user.FireWarning = now;
                    ret = 2;
                }
                else
                {
                    user.LoginFailureCnt = 1;
                    user.FireOn = now;
                    ret = 1;
                }
            }
            else
            {
                user.LoginFailureCnt = 1;
                user.FireOn = now;
                ret = 1;
            }

            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return (0, 0, ret);
        }
        catch
        {
            return (1, 0, 0);
        }
    }

    public async Task<(byte Ret, byte Result)> UILock_InitAsync(
        uint accountSerial,
        string uiLockPw,
        byte hintIndex,
        string hintAnswer,
        CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.Serial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return (0, 2);
            }

            if (user.UiLock != 0)
            {
                return (0, 1);
            }

            user.UiLock = 1;
            user.UiLockPw = uiLockPw;
            user.UiLockFailCnt = 0;
            user.UiLockUpdate = DateTime.Now;
            user.UiLockHintIndex = hintIndex;
            user.UiLockHintAnswer = hintAnswer;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return (0, 0);
        }
        catch
        {
            return (1, 0);
        }
    }

    public async Task<(byte Ret, byte Result)> UILock_UpdateAsync(
        uint accountSerial,
        string uiLockPw,
        byte hintIndex,
        string hintAnswer,
        CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.Serial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return (0, 2);
            }
            if (user.UiLock == 0)
            {
                return (0, 1);
            }

            user.UiLockPw = uiLockPw;
            user.UiLockFailCnt = 0;
            user.UiLockUpdate = DateTime.Now;
            user.UiLockHintIndex = hintIndex;
            user.UiLockHintAnswer = hintAnswer;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return (0, 0);
        }
        catch
        {
            return (1, 0);
        }
    }

    public async Task<(byte Ret, byte Result)> UILock_RefreshAsync(
        uint accountSerial,
        byte failCount,
        byte findPassFailCount,
        CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.Serial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return (0, 2);
            }
            if (user.UiLock == 0)
            {
                return (0, 1);
            }
            if (user.UiLockFailCnt >= 5)
            {
                return (0, 3);
            }
            if (user.UiLockFindPassFailCount >= 5)
            {
                return (0, 4);
            }

            user.UiLockFailCnt = failCount;
            user.UiLockFindPassFailCount = findPassFailCount;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return (0, 0);
        }
        catch
        {
            return (1, 0);
        }
    }

    public async Task<bool> Update_StaffLogoffDateAsync(uint accountSerial, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var staff = await db.StaffAccounts.FirstOrDefaultAsync(x => x.Serial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (staff == null)
            {
                return false;
            }
            staff.LastLogoffDt = DateTime.Now;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Update_UserLogoffDateAsync(uint accountSerial, CancellationToken token)
    {
        try
        {
            await using var db = CreateContext();
            var user = await db.UserAccounts.FirstOrDefaultAsync(x => x.Serial == (int)accountSerial, token)
                .ConfigureAwait(false);
            if (user == null)
            {
                return false;
            }
            user.LastLogoffTime = DateTime.Now;
            await db.SaveChangesAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Create_LogOutTableAsync(string date, CancellationToken token)
    {
        try
        {
            string tableName = $"tbl_UserLogout_Log{date}";
            await using var db = CreateContext();
            if (_provider == DatabaseProvider.SqlServer)
            {
                string createTable = $@"
IF OBJECT_ID(N'[dbo].[{tableName}]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[{tableName}](
        [idx] [int] IDENTITY (1, 1) NOT NULL,
        [nAccountSerial] [int] NOT NULL,
        [nWorldCode] [int] NOT NULL,
        [dtLoginDate] [datetime] NOT NULL,
        [dtLogoutDate] [datetime] NOT NULL,
        [nBillingType] [int] NOT NULL,
        [nLevel] [int] NULL,
        [nAvatorSerial] [int] NULL,
        [Account] [char](17) NULL,
        [ip] [char](16) NOT NULL
    );
    ALTER TABLE [dbo].[{tableName}] WITH NOCHECK ADD CONSTRAINT [PK_{tableName}] PRIMARY KEY NONCLUSTERED ([idx]);
    CREATE CLUSTERED INDEX [IX_{tableName}_Logout] ON [dbo].[{tableName}] ([dtLogoutDate]);
    CREATE INDEX [IX_{tableName}_Login] ON [dbo].[{tableName}] ([dtLoginDate]);
END";
                await db.Database.ExecuteSqlRawAsync(createTable, token).ConfigureAwait(false);
                return true;
            }

            if (_provider == DatabaseProvider.MariaDb)
            {
                string createTable = $@"
CREATE TABLE IF NOT EXISTS `{tableName}`(
    `idx` INT NOT NULL AUTO_INCREMENT,
    `nAccountSerial` INT NOT NULL,
    `nWorldCode` INT NOT NULL,
    `dtLoginDate` DATETIME NOT NULL,
    `dtLogoutDate` DATETIME NOT NULL,
    `nBillingType` INT NOT NULL,
    `nLevel` INT NULL,
    `nAvatorSerial` INT NULL,
    `Account` CHAR(17) NULL,
    `ip` CHAR(16) NOT NULL,
    PRIMARY KEY (`idx`)
);";
                await db.Database.ExecuteSqlRawAsync(createTable, token).ConfigureAwait(false);
                return true;
            }

            string sqliteTable = $@"
CREATE TABLE IF NOT EXISTS `{tableName}`(
    idx INTEGER PRIMARY KEY AUTOINCREMENT,
    nAccountSerial INTEGER NOT NULL,
    nWorldCode INTEGER NOT NULL,
    dtLoginDate TEXT NOT NULL,
    dtLogoutDate TEXT NOT NULL,
    nBillingType INTEGER NOT NULL,
    nLevel INTEGER,
    nAvatorSerial INTEGER,
    Account TEXT,
    ip TEXT NOT NULL
);";
            await db.Database.ExecuteSqlRawAsync(sqliteTable, token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<bool> Insert_UserLogoutLogAsync(
        uint accountSerial,
        string loginDate,
        string ip,
        uint worldCode,
        byte billingType,
        byte level,
        uint avatarSerial,
        string account,
        CancellationToken token)
    {
        try
        {
            string tableName = $"tbl_UserLogout_Log{DateTime.Now:yyyyMMdd}";
            if (!DateTime.TryParse(loginDate, out var loginDt))
            {
                loginDt = DateTime.Now;
            }
            var logoutDt = DateTime.Now;

            await using var db = CreateContext();
            string sql =
                $"INSERT INTO {tableName} (nAccountSerial, dtLoginDate, dtLogoutDate, ip, nWorldCode, nBillingType, nLevel, nAvatorSerial, Account) " +
                "VALUES ({0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8});";

            await db.Database.ExecuteSqlRawAsync(
                    sql,
                    accountSerial,
                    loginDt,
                    logoutDt,
                    ip,
                    worldCode,
                    billingType,
                    level,
                    avatarSerial,
                    account)
                .ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }
}
