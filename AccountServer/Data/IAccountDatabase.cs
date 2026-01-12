using System;
using System.Threading;
using System.Threading.Tasks;
using LoginServer.Packets;

namespace AccountServer.Data;

/// <summary>
/// Contract for account database access. Keep handlers free of concrete ADO
/// details so we can swap in EF Core later without touching packet analysis.
/// </summary>
public interface IAccountDatabase
{
    Task<StaffInfoResult> Select_StaffInfoExAsync(string id, CancellationToken token);
    Task<bool> Select_StaffExpireAsync(string id, CancellationToken token);
    Task<UserInfoResult> Select_UserInfoAsync(string id, CancellationToken token);
    Task<byte> Exist_UserBanAsync(uint accountSerial, CancellationToken token);
    Task<(int Ret, string BlockReason)> Select_UserAccountBlockInfoAsync(uint accountSerial, CancellationToken token);
    Task<byte> Update_User_StateAsync(uint serial, byte state, CancellationToken token);
    Task<bool> Insert_User_StateAsync(uint serial, byte state, CancellationToken token);
    Task<bool> Insert_UserAsync(string id, string ip, CancellationToken token);
    Task<bool> Insert_UserPushLogAsync(int serial, string pushIP, string closeIP, CancellationToken token);
    Task<(bool Ok, uint Serial, DateTime Date)> Select_UserSerialAsync(string id, CancellationToken token);
    Task<bool> Update_StaffLoginDateAsync(string id, string ip, CancellationToken token);
    Task<bool> Update_UserLoginDateAsync(string id, string ip, CancellationToken token);
    Task<bool> Update_Login_Failure_CountAsync(string id, byte byType, CancellationToken token);
    Task<(byte Ret, uint Serial)> Select_Limit_Run_Record_SerialAsync(CancellationToken token);
    Task<byte> Insert_Set_Limit_RunAsync(string param1Str, CancellationToken token);
    Task<byte> Update_Set_Limit_RunAsync(uint serial, string param1Str, CancellationToken token);
    Task<bool> Insert_UserCountLogAsync(uint averageUser, uint maxUser, CancellationToken token);
    Task<bool> Insert_UserCountLogAsync(uint averageUser, uint maxUserRegular, uint maxUserTestServer, CancellationToken token);
    Task<bool> Insert_ServerUserLogAsync(uint averageUser, uint maxUser, uint[] playerPerRace, string serverName, string logDate, CancellationToken token);
    Task<bool> Update_WorldServerAsync(int serial, int state, CancellationToken token);
    Task<(byte Ret, byte Kind, ushort Period, string StartDate)> Select_UserBanAsync(uint accountSerial, CancellationToken token);
    Task<bool> Insert_UserBanAsync(uint accountSerial, byte kind, uint period, string reason, string writer, byte reasonType, CancellationToken token);
    Task<bool> Update_UserBanAsync(uint accountSerial, byte kind, uint period, string reason, CancellationToken token);
    Task<bool> Insert_UserBan_LogAsync(uint accountSerial, byte kind, uint period, string reason, CancellationToken token);
    Task<bool> Insert_UserBan_ApexAsync(uint accountSerial, CancellationToken token);
    Task<byte> IsBlockIPAsync(uint ip, CancellationToken token);
    Task<bool> Insert_BlockIPAsync(uint ip, CancellationToken token);
    Task<(byte Ret, int AccountCount, int IpCount)> Fireguard_Block_Type1Async(string accountId, uint ip, CancellationToken token);
    Task<(byte Ret, int ErrorCode, int ResultCode)> Check_Fireguard_BlockAsync(string accountId, CancellationToken token);
    Task<(byte Ret, byte Result)> UILock_InitAsync(uint accountSerial, string uiLockPw, byte hintIndex, string hintAnswer, CancellationToken token);
    Task<(byte Ret, byte Result)> UILock_UpdateAsync(uint accountSerial, string uiLockPw, byte hintIndex, string hintAnswer, CancellationToken token);
    Task<(byte Ret, byte Result)> UILock_RefreshAsync(uint accountSerial, byte failCount, byte findPassFailCount, CancellationToken token);
    Task<bool> Update_StaffLogoffDateAsync(uint accountSerial, CancellationToken token);
    Task<bool> Update_UserLogoffDateAsync(uint accountSerial, CancellationToken token);
    Task<bool> Create_LogOutTableAsync(string date, CancellationToken token);
    Task<bool> Insert_UserLogoutLogAsync(
        uint accountSerial,
        string loginDate,
        string ip,
        uint worldCode,
        byte billingType,
        byte level,
        uint avatarSerial,
        string account,
        CancellationToken token);
}
