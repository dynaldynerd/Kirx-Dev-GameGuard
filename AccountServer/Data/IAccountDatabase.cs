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
    Task<(bool Ok, uint Serial, string Date)> Select_UserSerialAsync(string id, CancellationToken token);
    Task<bool> Update_StaffLoginDateAsync(string id, string ip, CancellationToken token);
    Task<bool> Update_UserLoginDateAsync(string id, string ip, CancellationToken token);
}
