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

    private static uint ReadUInt32(SqlDataReader reader, int ordinal)
    {
        var value = reader.GetValue(ordinal);
        return value is uint u ? u : Convert.ToUInt32(value);
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
