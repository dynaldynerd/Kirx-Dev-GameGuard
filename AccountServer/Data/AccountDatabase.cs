using System;
using System.Data;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using LoginServer.Packets;
using Microsoft.Data.SqlClient;

namespace AccountServer.Data;

/// <summary>
/// Thin ADO wrapper for account DB stored procedures, mirroring the native CRFAccountDatabase calls.
/// </summary>
public sealed class AccountDatabase : IAccountDatabase
{
    private const string LimitRunParam1Hex =
        "0xEFC6616D39F3CAB14D9D6264B2552A4A2D365809F15163DEC0A05B4A233CD4593F62B0F2FDDAEB0C45A55CB21662DD5326EE58D35AF3CAC85FCC128BD0CB96F6";
    private readonly string _connectionString;
    private readonly bool _saveDbLog;

    public AccountDatabase(string connectionString, bool saveDbLog = false)
    {
        _connectionString = connectionString ?? throw new ArgumentNullException(nameof(connectionString));
        _saveDbLog = saveDbLog;
    }

    public async Task<StaffInfoResult> Select_StaffInfoExAsync(string id, CancellationToken token)
    {
        var result = new StaffInfoResult { Ret = 1, Info = new StaffInfo(), LoginDate = DateTime.MinValue };
        const string proc = "pSelect_StaffInfoEx";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(proc, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                result.Ret = 2;
                return result;
            }

            result.Info.Password = ReadStringTrimmed(reader, 0);
            result.Info.Serial = ReadUInt32(reader, 1);
            result.Info.TotalLogMin = ReadUInt32(reader, 2);
            result.Info.Grade = reader.GetByte(3);
            result.Info.SubGrade = reader.GetByte(4);
            result.LoginDate = ReadDateTime(reader, 5);
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
        const string proc = "pSelect_StaffExpire";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(proc, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false)) return false;
            var expire = ReadDateTime(reader, 0);
            var now = ReadDateTime(reader, 1);
            return expire > now;
        }
        catch
        {
            return false;
        }
    }

    public async Task<UserInfoResult> Select_UserInfoAsync(string id, CancellationToken token)
    {
        var result = new UserInfoResult { Ret = 1 };
        const string proc = "pSelect_UserInfo_20070612";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(proc, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                result.Ret = 2;
                return result;
            }

            result.Serial = ReadUInt32(reader, 0);
            result.TotalLogMin = ReadUInt32(reader, 1);
            result.LoginDate = ReadDateTime(reader, 2);
            result.UiLock = reader.GetByte(3);
            result.UiLockPw = ReadStringTrimmed(reader, 4);
            result.UiLockFailCnt = reader.GetByte(5);
            result.HintIndex = reader.GetByte(6);
            result.HintAnswer = ReadStringTrimmed(reader, 7);
            result.UiLockFindPassFailCount = reader.GetByte(8);
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
        const string proc = "pProcess_UserBan";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(proc, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            cmd.Parameters.AddWithValue("@serial", accountSerial);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false)) return 2;
            var code = reader.GetInt16(0);
            return code switch
            {
                1 => 1,
                2 => 2,
                3 => 3,
                _ => 1
            };
        }
        catch
        {
            return 1;
        }
    }

    public async Task<(int Ret, string BlockReason)> Select_UserAccountBlockInfoAsync(uint accountSerial, CancellationToken token)
    {
        string reason = string.Empty;
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand("select GMReason from tbl_userban where nAccountSerial = @serial", conn);
            cmd.Parameters.AddWithValue("@serial", accountSerial);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false)) return (2, reason);
            reason = ReadStringTrimmed(reader, 0);
            return (0, reason);
        }
        catch
        {
            return (1, reason);
        }
    }

    public async Task<byte> Update_User_StateAsync(uint serial, byte state, CancellationToken token)
    {
        const string proc = "pUpdate_UserCurrentState";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(proc, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            cmd.Parameters.AddWithValue("@serial", serial);
            cmd.Parameters.AddWithValue("@state", state);
            await conn.OpenAsync(token).ConfigureAwait(false);
            await cmd.ExecuteNonQueryAsync(token).ConfigureAwait(false);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public Task<bool> Insert_User_StateAsync(uint serial, byte state, CancellationToken token)
    {
        const string proc = "pInsert_UserCurrentState";
        return ExecNonQueryAsync(proc, token, ("@serial", serial), ("@state", state));
    }

    public Task<bool> Insert_UserAsync(string id, string ip, CancellationToken token)
    {
        const string proc = "pInsert_User";
        return ExecNonQueryAsync(proc, token, ("@id", id), ("@ip", ip));
    }

    public Task<bool> Insert_UserPushLogAsync(int serial, string pushIP, string closeIP, CancellationToken token)
    {
        const string proc = "pInsert_UserPushLog";
        return ExecNonQueryAsync(proc, token, ("@serial", serial), ("@pushIP", pushIP), ("@closeIP", closeIP));
    }

    public async Task<(bool Ok, uint Serial, DateTime Date)> Select_UserSerialAsync(string id, CancellationToken token)
    {
        uint serial = 0;
        DateTime date = DateTime.MinValue;
        const string proc = "pSelect_UserSerial";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(proc, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false)) return (false, serial, date);
            serial = ReadUInt32(reader, 0);
            date = ReadDateTime(reader, 1);
            return (true, serial, date);
        }
        catch
        {
            return (false, serial, date);
        }
    }

    public Task<bool> Update_StaffLoginDateAsync(string id, string ip, CancellationToken token)
    {
        const string proc = "pUpdate_StaffLoginDate";
        return ExecNonQueryAsync(proc, token, ("@id", id), ("@ip", ip));
    }

    public Task<bool> Update_UserLoginDateAsync(string id, string ip, CancellationToken token)
    {
        const string proc = "pUpdate_UserLoginDate";
        return ExecNonQueryAsync(proc, token, ("@id", id), ("@ip", ip));
    }

    public async Task<bool> Update_Login_Failure_CountAsync(string id, byte byType, CancellationToken token)
    {
        string query;
        if (byType == 0)
        {
            query = "Update tbl_useraccount set LoginFailureCnt = LoginFailureCnt+1 where id = convert(binary, @id)";
        }
        else if (byType == 1)
        {
            query = "Update tbl_useraccount set LoginFailureCnt = 0 where id = convert(binary, @id)";
        }
        else
        {
            return false;
        }

        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            await cmd.ExecuteNonQueryAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    public async Task<(byte Ret, uint Serial)> Select_Limit_Run_Record_SerialAsync(CancellationToken token)
    {
        const string query = "select [ID] from [dbo].[tbl_GMActParam] where [Param1] = " + LimitRunParam1Hex;
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                return (2, 0);
            }
            uint serial = ReadUInt32(reader, 0);
            return (0, serial);
        }
        catch
        {
            return (1, 0);
        }
    }

    public async Task<byte> Insert_Set_Limit_RunAsync(string param1Str, CancellationToken token)
    {
        string query =
            "insert into [dbo].[tbl_GMActParam] ([Param1], [Param2]) values (" + LimitRunParam1Hex + ", @param2)";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@param2", param1Str);
            await conn.OpenAsync(token).ConfigureAwait(false);
            await cmd.ExecuteNonQueryAsync(token).ConfigureAwait(false);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public async Task<byte> Update_Set_Limit_RunAsync(uint serial, string param1Str, CancellationToken token)
    {
        const string query = "update [dbo].[tbl_GMActParam] set [Param2] = @param2 where [ID] = @id";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@param2", param1Str);
            cmd.Parameters.AddWithValue("@id", serial);
            await conn.OpenAsync(token).ConfigureAwait(false);
            await cmd.ExecuteNonQueryAsync(token).ConfigureAwait(false);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public Task<bool> Insert_UserCountLogAsync(uint averageUser, uint maxUser, CancellationToken token)
    {
        const string query = "exec pInsert_UserCountLog @avg, @max";
        return ExecNonQueryTextAsync(query, token, ("@avg", averageUser), ("@max", maxUser));
    }

    public Task<bool> Insert_UserCountLogAsync(uint averageUser, uint maxUserRegular, uint maxUserTestServer, CancellationToken token)
    {
        const string query = "exec pInsert_UserCountLog20080218 @avg, @max, @maxTest";
        return ExecNonQueryTextAsync(
            query,
            token,
            ("@avg", averageUser),
            ("@max", maxUserRegular),
            ("@maxTest", maxUserTestServer));
    }

    public Task<bool> Insert_ServerUserLogAsync(
        uint averageUser,
        uint maxUser,
        uint[] playerPerRace,
        string serverName,
        string logDate,
        CancellationToken token)
    {
        uint race0 = playerPerRace != null && playerPerRace.Length > 0 ? playerPerRace[0] : 0;
        uint race1 = playerPerRace != null && playerPerRace.Length > 1 ? playerPerRace[1] : 0;
        uint race2 = playerPerRace != null && playerPerRace.Length > 2 ? playerPerRace[2] : 0;
        const string query = "exec pInsert_ServerUserLog @avg, @max, @r0, @r1, @r2, @server, @date";
        return ExecNonQueryTextAsync(
            query,
            token,
            ("@avg", averageUser),
            ("@max", maxUser),
            ("@r0", race0),
            ("@r1", race1),
            ("@r2", race2),
            ("@server", serverName),
            ("@date", logDate));
    }

    public Task<bool> Update_WorldServerAsync(int serial, int state, CancellationToken token)
    {
        const string query = "exec pUpdate_WorldServer @serial, @state";
        return ExecNonQueryTextAsync(query, token, ("@serial", serial), ("@state", state));
    }

    public async Task<(byte Ret, byte Kind, ushort Period, string StartDate)> Select_UserBanAsync(
        uint accountSerial,
        CancellationToken token)
    {
        byte kind = 0;
        ushort period = 0;
        string startDate = string.Empty;
        const string query = "exec pSelect_UserBan @serial";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@serial", accountSerial);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                return (2, kind, period, startDate);
            }

            startDate = ReadStringTrimmed(reader, 0);
            period = ReadUInt16(reader, 1);
            kind = reader.GetByte(2);
            return (0, kind, period, startDate);
        }
        catch
        {
            return (1, kind, period, startDate);
        }
    }

    public Task<bool> Insert_UserBanAsync(
        uint accountSerial,
        byte kind,
        uint period,
        string reason,
        string writer,
        byte reasonType,
        CancellationToken token)
    {
        const string query =
            "exec pInsert_UserBan_20071016 @serial, @period, @kind, @reason, @writer, @reasonType";
        return ExecNonQueryTextAsync(
            query,
            token,
            ("@serial", accountSerial),
            ("@period", period),
            ("@kind", kind),
            ("@reason", reason),
            ("@writer", writer),
            ("@reasonType", reasonType));
    }

    public Task<bool> Update_UserBanAsync(uint accountSerial, byte kind, uint period, string reason, CancellationToken token)
    {
        const string query = "exec pUpdate_UserBan @serial, @period, @kind, @reason";
        return ExecNonQueryTextAsync(
            query,
            token,
            ("@serial", accountSerial),
            ("@period", period),
            ("@kind", kind),
            ("@reason", reason));
    }

    public Task<bool> Insert_UserBan_LogAsync(uint accountSerial, byte kind, uint period, string reason, CancellationToken token)
    {
        const string query = "exec pInsert_UserBanLog @serial, @period, @kind, @reason";
        return ExecNonQueryTextAsync(
            query,
            token,
            ("@serial", accountSerial),
            ("@period", period),
            ("@kind", kind),
            ("@reason", reason));
    }

    public Task<bool> Insert_UserBan_ApexAsync(uint accountSerial, CancellationToken token)
    {
        const string query = "exec pInsert_UserBan_Apex @serial";
        return ExecNonQueryTextAsync(query, token, ("@serial", accountSerial));
    }

    public async Task<byte> IsBlockIPAsync(uint ip, CancellationToken token)
    {
        byte b0 = (byte)(ip & 0xFF);
        byte b1 = (byte)((ip >> 8) & 0xFF);
        byte b2 = (byte)((ip >> 16) & 0xFF);
        byte b3 = (byte)((ip >> 24) & 0xFF);
        const string query = "exec pSelect_BlockIP_20070122 @b0, @b1, @b2, @b3";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@b0", b0);
            cmd.Parameters.AddWithValue("@b1", b1);
            cmd.Parameters.AddWithValue("@b2", b2);
            cmd.Parameters.AddWithValue("@b3", b3);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (await reader.ReadAsync(token).ConfigureAwait(false))
            {
                return 0;
            }
            return 2;
        }
        catch
        {
            return 1;
        }
    }

    public Task<bool> Insert_BlockIPAsync(uint ip, CancellationToken token)
    {
        byte b0 = (byte)(ip & 0xFF);
        byte b1 = (byte)((ip >> 8) & 0xFF);
        byte b2 = (byte)((ip >> 16) & 0xFF);
        byte b3 = (byte)((ip >> 24) & 0xFF);
        const string query = "exec pInsert_BlockIP @b0, @b1, @b2, @b3";
        return ExecNonQueryTextAsync(
            query,
            token,
            ("@b0", b0),
            ("@b1", b1),
            ("@b2", b2),
            ("@b3", b3));
    }

    public async Task<(byte Ret, int AccountCount, int IpCount)> Fireguard_Block_Type1Async(
        string accountId,
        uint ip,
        CancellationToken token)
    {
        const string query = "exec prc_process_fg_detection @id, @ip";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@id", accountId);
            cmd.Parameters.AddWithValue("@ip", ip);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                return (2, 0, 0);
            }
            int accountCount = ReadInt32(reader, 0);
            int ipCount = ReadInt32(reader, 1);
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
        const string query = "exec pUpdate_FireguardBlock20071016 @id, 30, 3";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(query, conn);
            cmd.Parameters.AddWithValue("@id", accountId);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                return (2, 0, 0);
            }
            int err = ReadInt32(reader, 0);
            int ret = ReadInt32(reader, 1);
            return (0, err, ret);
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
        const string query =
            "declare @result tinyint; exec pUpdate_UILock_Init @serial, @pw, @hint, @answer, @ret = @result output; select @result;";
        return await ExecuteScalarByteAsync(
            query,
            token,
            ("@serial", accountSerial),
            ("@pw", uiLockPw),
            ("@hint", hintIndex),
            ("@answer", hintAnswer)).ConfigureAwait(false);
    }

    public async Task<(byte Ret, byte Result)> UILock_UpdateAsync(
        uint accountSerial,
        string uiLockPw,
        byte hintIndex,
        string hintAnswer,
        CancellationToken token)
    {
        const string query =
            "declare @result tinyint; exec pUpdate_UILock_Update @serial, @pw, @hint, @answer, @ret = @result output; select @result;";
        return await ExecuteScalarByteAsync(
            query,
            token,
            ("@serial", accountSerial),
            ("@pw", uiLockPw),
            ("@hint", hintIndex),
            ("@answer", hintAnswer)).ConfigureAwait(false);
    }

    public async Task<(byte Ret, byte Result)> UILock_RefreshAsync(
        uint accountSerial,
        byte failCount,
        byte findPassFailCount,
        CancellationToken token)
    {
        const string query =
            "declare @result tinyint; exec pUpdate_UILock_Refresh @serial, @failCnt, @findCnt, @ret = @result output; select @result;";
        return await ExecuteScalarByteAsync(
            query,
            token,
            ("@serial", accountSerial),
            ("@failCnt", failCount),
            ("@findCnt", findPassFailCount)).ConfigureAwait(false);
    }

    public Task<bool> Update_StaffLogoffDateAsync(uint accountSerial, CancellationToken token)
    {
        const string query = "exec pUpdate_StaffLogoffDate @serial";
        return ExecNonQueryTextAsync(query, token, ("@serial", accountSerial));
    }

    public Task<bool> Update_UserLogoffDateAsync(uint accountSerial, CancellationToken token)
    {
        const string query = "exec pUpdate_UserLogoffDate @serial";
        return ExecNonQueryTextAsync(query, token, ("@serial", accountSerial));
    }

    private async Task<bool> ExecNonQueryAsync(string procName, CancellationToken token, params (string name, object value)[] parameters)
    {
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(procName, conn)
            {
                CommandType = CommandType.StoredProcedure
            };
            foreach (var (name, value) in parameters)
            {
                cmd.Parameters.AddWithValue(name, value ?? DBNull.Value);
            }
            await conn.OpenAsync(token).ConfigureAwait(false);
            await cmd.ExecuteNonQueryAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    private async Task<bool> ExecNonQueryTextAsync(string sql, CancellationToken token, params (string name, object value)[] parameters)
    {
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(sql, conn);
            foreach (var (name, value) in parameters)
            {
                cmd.Parameters.AddWithValue(name, value ?? DBNull.Value);
            }
            await conn.OpenAsync(token).ConfigureAwait(false);
            await cmd.ExecuteNonQueryAsync(token).ConfigureAwait(false);
            return true;
        }
        catch
        {
            return false;
        }
    }

    private async Task<(byte Ret, byte Result)> ExecuteScalarByteAsync(
        string sql,
        CancellationToken token,
        params (string name, object value)[] parameters)
    {
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand(sql, conn);
            foreach (var (name, value) in parameters)
            {
                cmd.Parameters.AddWithValue(name, value ?? DBNull.Value);
            }
            await conn.OpenAsync(token).ConfigureAwait(false);
            object? result = await cmd.ExecuteScalarAsync(token).ConfigureAwait(false);
            byte resultValue = result == null || result is DBNull ? (byte)0 : Convert.ToByte(result);
            return (0, resultValue);
        }
        catch
        {
            return (1, 0);
        }
    }

    private static uint ReadUInt32(SqlDataReader reader, int ordinal)
    {
        var value = reader.GetValue(ordinal);
        return value is uint u ? u : Convert.ToUInt32(value);
    }

    private static ushort ReadUInt16(SqlDataReader reader, int ordinal)
    {
        var value = reader.GetValue(ordinal);
        return value is ushort u ? u : Convert.ToUInt16(value);
    }

    private static int ReadInt32(SqlDataReader reader, int ordinal)
    {
        var value = reader.GetValue(ordinal);
        return value is int i ? i : Convert.ToInt32(value);
    }

    private static DateTime ReadDateTime(SqlDataReader reader, int ordinal)
    {
        if (reader.IsDBNull(ordinal))
        {
            return DateTime.MinValue;
        }
        var value = reader.GetValue(ordinal);
        return value is DateTime dt ? dt : Convert.ToDateTime(value);
    }

    private static string ReadStringTrimmed(SqlDataReader reader, int ordinal)
    {
        if (reader.IsDBNull(ordinal))
        {
            return string.Empty;
        }
        var value = reader.GetValue(ordinal);
        var text = value as string ?? Convert.ToString(value) ?? string.Empty;
        return text.TrimEnd('\0');
    }
}

public sealed class StaffInfo
{
    public string Password { get; set; } = string.Empty;
    public uint Serial { get; set; }
    public uint TotalLogMin { get; set; }
    public byte Grade { get; set; }
    public byte SubGrade { get; set; }
    public byte AgeLimit { get; set; } = 1;
}

public sealed class StaffInfoResult
{
    public byte Ret { get; set; }
    public StaffInfo Info { get; set; } = new();
    public DateTime LoginDate { get; set; } = DateTime.MinValue;
}

public sealed class UserInfoResult
{
    public byte Ret { get; set; }
    public uint Serial { get; set; }
    public uint TotalLogMin { get; set; }
    public DateTime LoginDate { get; set; } = DateTime.MinValue;
    public byte UiLock { get; set; }
    public string UiLockPw { get; set; } = string.Empty;
    public byte UiLockFailCnt { get; set; }
    public byte HintIndex { get; set; }
    public string HintAnswer { get; set; } = string.Empty;
    public byte UiLockFindPassFailCount { get; set; }
}
