using System;
using System.Data;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Data.SqlClient;

namespace AccountServer.Data;

/// <summary>
/// Thin ADO wrapper for account DB stored procedures, mirroring the native CRFAccountDatabase calls.
/// </summary>
public sealed class AccountDatabase : IAccountDatabase
{
    private readonly string _connectionString;
    private readonly bool _saveDbLog;

    public AccountDatabase(string connectionString, bool saveDbLog = false)
    {
        _connectionString = connectionString ?? throw new ArgumentNullException(nameof(connectionString));
        _saveDbLog = saveDbLog;
    }

    public async Task<StaffInfoResult> Select_StaffInfoExAsync(string id, CancellationToken token)
    {
        var result = new StaffInfoResult { Ret = 1, Info = new StaffInfo(), LoginDate = string.Empty };
        const string proc = "pSelect_StaffInfoEx";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand($"{{ CALL {proc}(@id) }}", conn);
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                result.Ret = 2;
                return result;
            }

            result.Info.Password = reader.GetString(0);
            result.Info.Serial = reader.GetFieldValue<uint>(1);
            result.Info.TotalLogMin = reader.GetFieldValue<uint>(2);
            result.Info.Grade = reader.GetByte(3);
            result.Info.SubGrade = reader.GetByte(4);
            result.LoginDate = reader.GetString(5);
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
            using var cmd = new SqlCommand($"{{ CALL {proc}(@id) }}", conn);
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false)) return false;
            var expire = reader.GetString(0);
            var now = reader.GetString(1);
            return string.Compare(expire, now, StringComparison.Ordinal) > 0;
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
            using var cmd = new SqlCommand($"{{ CALL {proc}(@id) }}", conn);
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false))
            {
                result.Ret = 2;
                return result;
            }

            result.Serial = reader.GetFieldValue<uint>(0);
            result.TotalLogMin = reader.GetFieldValue<uint>(1);
            result.LoginDate = reader.GetString(2);
            result.UiLock = reader.GetByte(3);
            result.UiLockPw = reader.GetString(4);
            result.UiLockFailCnt = reader.GetByte(5);
            result.HintIndex = reader.GetByte(6);
            result.HintAnswer = reader.GetString(7);
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
            using var cmd = new SqlCommand($"{{ CALL {proc}(@serial) }}", conn);
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
            reason = reader.GetString(0);
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
            using var cmd = new SqlCommand($"{{ CALL {proc}(@serial, @state) }}", conn);
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

    public async Task<(bool Ok, uint Serial, string Date)> Select_UserSerialAsync(string id, CancellationToken token)
    {
        uint serial = 0;
        string date = string.Empty;
        const string proc = "pSelect_UserSerial";
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand($"{{ CALL {proc}(@id) }}", conn);
            cmd.Parameters.AddWithValue("@id", id);
            await conn.OpenAsync(token).ConfigureAwait(false);
            using var reader = await cmd.ExecuteReaderAsync(token).ConfigureAwait(false);
            if (!await reader.ReadAsync(token).ConfigureAwait(false)) return (false, serial, date);
            serial = reader.GetFieldValue<uint>(0);
            date = reader.GetString(1);
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

    private async Task<bool> ExecNonQueryAsync(string procName, CancellationToken token, params (string name, object value)[] parameters)
    {
        try
        {
            using var conn = new SqlConnection(_connectionString);
            using var cmd = new SqlCommand($"{{ CALL {procName}({string.Join(",", parameters.Select(p => p.name))}) }}", conn);
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
    public string LoginDate { get; set; } = string.Empty;
}

public sealed class UserInfoResult
{
    public byte Ret { get; set; }
    public uint Serial { get; set; }
    public uint TotalLogMin { get; set; }
    public string LoginDate { get; set; } = string.Empty;
    public byte UiLock { get; set; }
    public string UiLockPw { get; set; } = string.Empty;
    public byte UiLockFailCnt { get; set; }
    public byte HintIndex { get; set; }
    public string HintAnswer { get; set; } = string.Empty;
    public byte UiLockFindPassFailCount { get; set; }
}
