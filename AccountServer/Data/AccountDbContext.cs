using Microsoft.EntityFrameworkCore;

namespace AccountServer.Data;

public sealed class AccountDbContext : DbContext
{
    public AccountDbContext(DbContextOptions<AccountDbContext> options)
        : base(options)
    {
    }

    public DbSet<StaffAccount> StaffAccounts => Set<StaffAccount>();
    public DbSet<UserAccount> UserAccounts => Set<UserAccount>();
    public DbSet<UserBan> UserBans => Set<UserBan>();
    public DbSet<UserBanLog> UserBanLogs => Set<UserBanLog>();
    public DbSet<UserCurrentState> UserCurrentStates => Set<UserCurrentState>();
    public DbSet<UserPushLog> UserPushLogs => Set<UserPushLog>();
    public DbSet<GmActParam> GmActParams => Set<GmActParam>();
    public DbSet<UserCountLog> UserCountLogs => Set<UserCountLog>();
    public DbSet<ServerUserLog> ServerUserLogs => Set<ServerUserLog>();
    public DbSet<WorldServerList> WorldServerLists => Set<WorldServerList>();
    public DbSet<BlockIp> BlockIps => Set<BlockIp>();
    public DbSet<FgDetectedAccount> FgDetectedAccounts => Set<FgDetectedAccount>();
    public DbSet<FgDetectedIp> FgDetectedIps => Set<FgDetectedIp>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<StaffAccount>(entity =>
        {
            entity.ToTable("tbl_StaffAccount");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("Serial");
            entity.Property(e => e.IdHmac).HasColumnName("ID_hmac");
            entity.Property(e => e.IdEnc).HasColumnName("ID_enc");
            entity.Property(e => e.PwHash).HasColumnName("PW_hash");
            entity.Property(e => e.Grade).HasColumnName("Grade");
            entity.Property(e => e.SubGrade).HasColumnName("SubGrade");
            entity.Property(e => e.TotalLogMin).HasColumnName("TotalLogMin");
            entity.Property(e => e.LastLoginDt).HasColumnName("LastLoginDT");
            entity.Property(e => e.LastLogoffDt).HasColumnName("LastLogoffDT");
            entity.Property(e => e.LastConnIp).HasColumnName("LastConnIP");
            entity.Property(e => e.ExpireDt).HasColumnName("ExpireDT");
        });

        modelBuilder.Entity<UserAccount>(entity =>
        {
            entity.ToTable("tbl_UserAccount");
            entity.HasKey(e => e.IdHmac);
            entity.Property(e => e.IdHmac).HasColumnName("id_hmac");
            entity.Property(e => e.IdEnc).HasColumnName("id_enc");
            entity.Property(e => e.Serial).HasColumnName("serial").ValueGeneratedOnAdd();
            entity.Property(e => e.CreateTime).HasColumnName("createtime");
            entity.Property(e => e.CreateIp).HasColumnName("createip");
            entity.Property(e => e.LastLoginTime).HasColumnName("lastlogintime");
            entity.Property(e => e.LastLogoffTime).HasColumnName("lastlogofftime");
            entity.Property(e => e.TotalLogMin).HasColumnName("totallogmin");
            entity.Property(e => e.LastConnectIp).HasColumnName("lastconnectip");
            entity.Property(e => e.PushCloseTime).HasColumnName("pushclosetime");
            entity.Property(e => e.PusherIp).HasColumnName("pusherip");
            entity.Property(e => e.JoinCode).HasColumnName("JoinCode");
            entity.Property(e => e.LoginFailureCnt).HasColumnName("LoginFailureCnt");
            entity.Property(e => e.FireOn).HasColumnName("fire_on");
            entity.Property(e => e.FireWarning).HasColumnName("fire_warning");
            entity.Property(e => e.UiLock).HasColumnName("uilock");
            entity.Property(e => e.UiLockPw).HasColumnName("uilock_pw");
            entity.Property(e => e.UiLockFailCnt).HasColumnName("uilock_failcnt");
            entity.Property(e => e.UiLockUpdate).HasColumnName("uilock_update");
            entity.Property(e => e.UiLockHintIndex).HasColumnName("uilock_hintindex");
            entity.Property(e => e.UiLockHintAnswer).HasColumnName("uilock_hintanswer");
            entity.Property(e => e.UiLockFindPassFailCount).HasColumnName("uilock_find_pass_failcnt");
        });

        modelBuilder.Entity<UserBan>(entity =>
        {
            entity.ToTable("tbl_UserBan");
            entity.HasKey(e => e.AccountSerial);
            entity.Property(e => e.AccountSerial).HasColumnName("nAccountSerial");
            entity.Property(e => e.StartDate).HasColumnName("dtStartDate");
            entity.Property(e => e.Period).HasColumnName("nPeriod");
            entity.Property(e => e.Kind).HasColumnName("nKind");
            entity.Property(e => e.Reason).HasColumnName("szReason");
            entity.Property(e => e.GmReason).HasColumnName("GMReason");
            entity.Property(e => e.GmWriter).HasColumnName("GMWriter");
            entity.Property(e => e.ReasonType).HasColumnName("ReasonType");
        });

        modelBuilder.Entity<UserBanLog>(entity =>
        {
            entity.ToTable("tbl_UserBan_Log");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("nSerial");
            entity.Property(e => e.AccountSerial).HasColumnName("nAccountSerial");
            entity.Property(e => e.StartDate).HasColumnName("dtStartdate");
            entity.Property(e => e.Period).HasColumnName("nPeriod");
            entity.Property(e => e.Kind).HasColumnName("nKind");
            entity.Property(e => e.Reason).HasColumnName("szReason");
            entity.Property(e => e.GmReason).HasColumnName("GMReason");
            entity.Property(e => e.GmWriter).HasColumnName("GMWriter");
        });

        modelBuilder.Entity<UserCurrentState>(entity =>
        {
            entity.ToTable("tbl_usercurrentstate");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("serial");
            entity.Property(e => e.State).HasColumnName("state");
        });

        modelBuilder.Entity<UserPushLog>(entity =>
        {
            entity.ToTable("tbl_UserPush_Log");
            entity.HasKey(e => new { e.AccountSerial, e.Date });
            entity.Property(e => e.AccountSerial).HasColumnName("nAccountSerial");
            entity.Property(e => e.Date).HasColumnName("dtDate");
            entity.Property(e => e.PushIp).HasColumnName("PushIP");
            entity.Property(e => e.CloseIp).HasColumnName("CloseIP");
        });

        modelBuilder.Entity<GmActParam>(entity =>
        {
            entity.ToTable("tbl_GMActParam");
            entity.HasKey(e => e.Id);
            entity.Property(e => e.Id).HasColumnName("ID");
            entity.Property(e => e.Param1).HasColumnName("Param1");
            entity.Property(e => e.Param2).HasColumnName("Param2");
        });

        modelBuilder.Entity<UserCountLog>(entity =>
        {
            entity.ToTable("tbl_UserCount_Log");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("serial");
            entity.Property(e => e.Date).HasColumnName("dtDate");
            entity.Property(e => e.AverageUser).HasColumnName("nAverageUser");
            entity.Property(e => e.MaxUser).HasColumnName("nMaxUser");
            entity.Property(e => e.MaxUserTestServer).HasColumnName("nMaxUserTestServer");
        });

        modelBuilder.Entity<ServerUserLog>(entity =>
        {
            entity.ToTable("tbl_ServerUser_Log");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("serial");
            entity.Property(e => e.Date).HasColumnName("dtDate");
            entity.Property(e => e.AverageUser).HasColumnName("nAverageUser");
            entity.Property(e => e.MaxUser).HasColumnName("nMaxUser");
            entity.Property(e => e.ServerName).HasColumnName("ServerName");
            entity.Property(e => e.BellaUser).HasColumnName("nBellaUser");
            entity.Property(e => e.CoraUser).HasColumnName("nCoraUser");
            entity.Property(e => e.AccUser).HasColumnName("nAccUser");
        });

        modelBuilder.Entity<WorldServerList>(entity =>
        {
            entity.ToTable("tbl_WorldServer_List");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("Serial");
            entity.Property(e => e.State).HasColumnName("State");
            entity.Property(e => e.LastUpdate).HasColumnName("LastUpdate");
            entity.Property(e => e.Name).HasColumnName("Name");
        });

        modelBuilder.Entity<BlockIp>(entity =>
        {
            entity.ToTable("tbl_block_ip");
            entity.HasKey(e => e.Serial);
            entity.Property(e => e.Serial).HasColumnName("nSerial");
            entity.Property(e => e.Addr0).HasColumnName("addr0");
            entity.Property(e => e.Addr1).HasColumnName("addr1");
            entity.Property(e => e.Addr2).HasColumnName("addr2");
            entity.Property(e => e.Addr3).HasColumnName("addr3");
            entity.Property(e => e.Addr4).HasColumnName("addr4");
            entity.Property(e => e.Addr2Range).HasColumnName("addr2range");
        });

        modelBuilder.Entity<FgDetectedAccount>(entity =>
        {
            entity.ToTable("tbl_fg_detected_account");
            entity.HasKey(e => new { e.Account, e.DetectTime });
            entity.Property(e => e.Account).HasColumnName("account");
            entity.Property(e => e.DetectTime).HasColumnName("detect_time");
        });

        modelBuilder.Entity<FgDetectedIp>(entity =>
        {
            entity.ToTable("tbl_fg_detected_ip");
            entity.HasKey(e => new { e.Ip, e.DetectTime });
            entity.Property(e => e.Ip).HasColumnName("ip");
            entity.Property(e => e.DetectTime).HasColumnName("detect_time");
        });
    }
}
