using System;

namespace AccountServer.Data;

public sealed class StaffAccount
{
    public int Serial { get; set; }
    public byte[] IdHmac { get; set; } = Array.Empty<byte>();
    public byte[] IdEnc { get; set; } = Array.Empty<byte>();
    public string PwHash { get; set; } = string.Empty;
    public byte Grade { get; set; }
    public byte SubGrade { get; set; }
    public int TotalLogMin { get; set; }
    public DateTime LastLoginDt { get; set; }
    public DateTime LastLogoffDt { get; set; }
    public string LastConnIp { get; set; } = string.Empty;
    public DateTime ExpireDt { get; set; }
}

public sealed class UserAccount
{
    public int Serial { get; set; }
    public byte[] IdHmac { get; set; } = Array.Empty<byte>();
    public byte[] IdEnc { get; set; } = Array.Empty<byte>();
    public DateTime CreateTime { get; set; }
    public string CreateIp { get; set; } = string.Empty;
    public DateTime LastLoginTime { get; set; }
    public DateTime LastLogoffTime { get; set; }
    public int TotalLogMin { get; set; }
    public string LastConnectIp { get; set; } = string.Empty;
    public DateTime PushCloseTime { get; set; }
    public string PusherIp { get; set; } = string.Empty;
    public int JoinCode { get; set; }
    public byte LoginFailureCnt { get; set; }
    public DateTime FireOn { get; set; }
    public DateTime FireWarning { get; set; }
    public byte UiLock { get; set; }
    public string UiLockPw { get; set; } = string.Empty;
    public byte UiLockFailCnt { get; set; }
    public DateTime UiLockUpdate { get; set; }
    public byte UiLockHintIndex { get; set; }
    public string UiLockHintAnswer { get; set; } = string.Empty;
    public byte UiLockFindPassFailCount { get; set; }
}

public sealed class UserBan
{
    public int AccountSerial { get; set; }
    public DateTime StartDate { get; set; }
    public int Period { get; set; }
    public byte Kind { get; set; }
    public string? Reason { get; set; }
    public string GmReason { get; set; } = string.Empty;
    public string GmWriter { get; set; } = string.Empty;
    public byte ReasonType { get; set; }
}

public sealed class UserBanLog
{
    public int Serial { get; set; }
    public int AccountSerial { get; set; }
    public DateTime StartDate { get; set; }
    public int Period { get; set; }
    public byte Kind { get; set; }
    public string? Reason { get; set; }
    public string GmReason { get; set; } = string.Empty;
    public string GmWriter { get; set; } = string.Empty;
}

public sealed class UserCurrentState
{
    public int Serial { get; set; }
    public int State { get; set; }
}

public sealed class UserPushLog
{
    public int AccountSerial { get; set; }
    public DateTime Date { get; set; }
    public string PushIp { get; set; } = string.Empty;
    public string CloseIp { get; set; } = string.Empty;
}

public sealed class GmActParam
{
    public int Id { get; set; }
    public byte[]? Param1 { get; set; }
    public byte[]? Param2 { get; set; }
}

public sealed class UserCountLog
{
    public int Serial { get; set; }
    public DateTime Date { get; set; }
    public int AverageUser { get; set; }
    public int MaxUser { get; set; }
    public int MaxUserTestServer { get; set; }
}

public sealed class ServerUserLog
{
    public int Serial { get; set; }
    public DateTime Date { get; set; }
    public int AverageUser { get; set; }
    public int MaxUser { get; set; }
    public string ServerName { get; set; } = string.Empty;
    public int BellaUser { get; set; }
    public int CoraUser { get; set; }
    public int AccUser { get; set; }
}

public sealed class WorldServerList
{
    public int Serial { get; set; }
    public string Name { get; set; } = string.Empty;
    public int State { get; set; }
    public DateTime LastUpdate { get; set; }
}

public sealed class BlockIp
{
    public int Serial { get; set; }
    public byte Addr0 { get; set; }
    public byte Addr1 { get; set; }
    public byte Addr2 { get; set; }
    public byte Addr3 { get; set; }
    public byte Addr4 { get; set; }
    public byte Addr2Range { get; set; }
}

public sealed class FgDetectedAccount
{
    public string Account { get; set; } = string.Empty;
    public DateTime DetectTime { get; set; }
}

public sealed class FgDetectedIp
{
    public int Ip { get; set; }
    public DateTime DetectTime { get; set; }
}
