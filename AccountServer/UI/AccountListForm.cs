using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AccountServer.Data;
using AccountServer.Security;
using AccountServer.Settings;
using Microsoft.EntityFrameworkCore;
using Pomelo.EntityFrameworkCore.MySql.Infrastructure;

namespace AccountServer.UI;

public enum AccountListMode
{
    NormalAccount,
    GmAccount,
    BanAccount
}

public sealed class AccountListForm : Form
{
    private readonly AppSettings _settings;
    private readonly AccountListMode _mode;
    private readonly byte[] _aesKey;

    private Label lblTitle = null!;
    private DataGridView gridAccounts = null!;
    private FlowLayoutPanel panelButtons = null!;
    private Button btnRefresh = null!;
    private Button btnClose = null!;

    public AccountListForm(AppSettings settings, AccountListMode mode)
    {
        _settings = settings;
        _mode = mode;
        _aesKey = CreateAesKey(settings.Security);
        InitializeComponent();

        string title = mode switch
        {
            AccountListMode.GmAccount => "GM Account",
            AccountListMode.BanAccount => "Ban Account",
            _ => "Normal Account"
        };

        Text = title;
        lblTitle.Text = title;
    }

    protected override async void OnShown(EventArgs e)
    {
        base.OnShown(e);
        await LoadRowsAsync().ConfigureAwait(true);
    }

    private void InitializeComponent()
    {
        lblTitle = new Label();
        gridAccounts = new DataGridView();
        panelButtons = new FlowLayoutPanel();
        btnRefresh = new Button();
        btnClose = new Button();
        ((System.ComponentModel.ISupportInitialize)gridAccounts).BeginInit();
        panelButtons.SuspendLayout();
        SuspendLayout();
        // 
        // lblTitle
        // 
        lblTitle.AutoSize = true;
        lblTitle.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Bold);
        lblTitle.Location = new System.Drawing.Point(12, 12);
        lblTitle.Name = "lblTitle";
        lblTitle.Size = new System.Drawing.Size(64, 21);
        lblTitle.TabIndex = 0;
        lblTitle.Text = "Account";
        // 
        // gridAccounts
        // 
        gridAccounts.AllowUserToAddRows = false;
        gridAccounts.AllowUserToDeleteRows = false;
        gridAccounts.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
        gridAccounts.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.AllCells;
        gridAccounts.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
        gridAccounts.Location = new System.Drawing.Point(12, 46);
        gridAccounts.MultiSelect = false;
        gridAccounts.Name = "gridAccounts";
        gridAccounts.ReadOnly = true;
        gridAccounts.RowHeadersVisible = false;
        gridAccounts.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
        gridAccounts.Size = new System.Drawing.Size(960, 442);
        gridAccounts.TabIndex = 1;
        // 
        // panelButtons
        // 
        panelButtons.Controls.Add(btnClose);
        panelButtons.Controls.Add(btnRefresh);
        panelButtons.Dock = DockStyle.Bottom;
        panelButtons.FlowDirection = FlowDirection.RightToLeft;
        panelButtons.Location = new System.Drawing.Point(0, 500);
        panelButtons.Name = "panelButtons";
        panelButtons.Padding = new Padding(10);
        panelButtons.Size = new System.Drawing.Size(984, 50);
        panelButtons.TabIndex = 2;
        // 
        // btnRefresh
        // 
        btnRefresh.AutoSize = true;
        btnRefresh.Location = new System.Drawing.Point(814, 13);
        btnRefresh.Name = "btnRefresh";
        btnRefresh.Size = new System.Drawing.Size(75, 25);
        btnRefresh.TabIndex = 1;
        btnRefresh.Text = "Refresh";
        btnRefresh.UseVisualStyleBackColor = true;
        btnRefresh.Click += BtnRefresh_Click;
        // 
        // btnClose
        // 
        btnClose.AutoSize = true;
        btnClose.DialogResult = DialogResult.OK;
        btnClose.Location = new System.Drawing.Point(895, 13);
        btnClose.Name = "btnClose";
        btnClose.Size = new System.Drawing.Size(66, 25);
        btnClose.TabIndex = 0;
        btnClose.Text = "Close";
        btnClose.UseVisualStyleBackColor = true;
        // 
        // AccountListForm
        // 
        AcceptButton = btnRefresh;
        AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
        AutoScaleMode = AutoScaleMode.Font;
        CancelButton = btnClose;
        ClientSize = new System.Drawing.Size(984, 550);
        Controls.Add(panelButtons);
        Controls.Add(gridAccounts);
        Controls.Add(lblTitle);
        MinimumSize = new System.Drawing.Size(820, 420);
        Name = "AccountListForm";
        StartPosition = FormStartPosition.CenterParent;
        ((System.ComponentModel.ISupportInitialize)gridAccounts).EndInit();
        panelButtons.ResumeLayout(false);
        panelButtons.PerformLayout();
        ResumeLayout(false);
        PerformLayout();
    }

    private async void BtnRefresh_Click(object? sender, EventArgs e)
    {
        await LoadRowsAsync().ConfigureAwait(true);
    }

    private async Task LoadRowsAsync()
    {
        btnRefresh.Enabled = false;
        Cursor previousCursor = Cursor;
        Cursor = Cursors.WaitCursor;

        try
        {
            object rows = _mode switch
            {
                AccountListMode.GmAccount => await LoadGmAccountsAsync().ConfigureAwait(true),
                AccountListMode.BanAccount => await LoadBanAccountsAsync().ConfigureAwait(true),
                _ => await LoadNormalAccountsAsync().ConfigureAwait(true)
            };

            gridAccounts.DataSource = rows;
        }
        catch (Exception ex)
        {
            MessageBox.Show(this, $"Failed to load account data: {ex.Message}", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            Cursor = previousCursor;
            btnRefresh.Enabled = true;
        }
    }

    private async Task<List<NormalAccountRow>> LoadNormalAccountsAsync()
    {
        await using AccountDbContext db = CreateContext();
        List<UserAccount> users = await db.UserAccounts.AsNoTracking()
            .OrderByDescending(x => x.Serial)
            .ToListAsync()
            .ConfigureAwait(true);

        return users.Select(user => new NormalAccountRow
        {
            Serial = user.Serial,
            AccountId = DecodeAccountId(user.IdEnc),
            CreateTime = user.CreateTime,
            CreateIp = user.CreateIp,
            LastLoginTime = user.LastLoginTime,
            LastLogoffTime = user.LastLogoffTime,
            LastConnectIp = user.LastConnectIp,
            TotalLogMin = user.TotalLogMin,
            JoinCode = user.JoinCode,
            LoginFailureCnt = user.LoginFailureCnt,
            UiLock = user.UiLock
        }).ToList();
    }

    private async Task<List<GmAccountRow>> LoadGmAccountsAsync()
    {
        await using AccountDbContext db = CreateContext();
        List<StaffAccount> staffAccounts = await db.StaffAccounts.AsNoTracking()
            .OrderByDescending(x => x.Serial)
            .ToListAsync()
            .ConfigureAwait(true);

        return staffAccounts.Select(staff => new GmAccountRow
        {
            Serial = staff.Serial,
            AccountId = DecodeAccountId(staff.IdEnc),
            Grade = staff.Grade,
            SubGrade = staff.SubGrade,
            LastLoginDt = staff.LastLoginDt,
            LastLogoffDt = staff.LastLogoffDt,
            LastConnIp = staff.LastConnIp,
            ExpireDt = staff.ExpireDt,
            TotalLogMin = staff.TotalLogMin
        }).ToList();
    }

    private async Task<List<BanAccountRow>> LoadBanAccountsAsync()
    {
        await using AccountDbContext db = CreateContext();
        List<UserBan> bans = await db.UserBans.AsNoTracking()
            .OrderByDescending(x => x.StartDate)
            .ToListAsync()
            .ConfigureAwait(true);

        int[] serials = bans.Select(x => x.AccountSerial).Distinct().ToArray();
        Dictionary<int, UserAccount> users = await db.UserAccounts.AsNoTracking()
            .Where(x => serials.Contains(x.Serial))
            .ToDictionaryAsync(x => x.Serial)
            .ConfigureAwait(true);

        return bans.Select(ban => new BanAccountRow
        {
            AccountSerial = ban.AccountSerial,
            AccountId = users.TryGetValue(ban.AccountSerial, out UserAccount? user)
                ? DecodeAccountId(user.IdEnc)
                : string.Empty,
            StartDate = ban.StartDate,
            Period = ban.Period,
            Kind = ban.Kind,
            Reason = ban.Reason ?? string.Empty,
            GmReason = ban.GmReason,
            GmWriter = ban.GmWriter,
            ReasonType = ban.ReasonType
        }).ToList();
    }

    private AccountDbContext CreateContext()
    {
        string connectionString = _settings.Database.BuildUserConnectionString(_settings.Database.Provider, AppContext.BaseDirectory);
        var builder = new DbContextOptionsBuilder<AccountDbContext>();

        switch (_settings.Database.Provider)
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

        return new AccountDbContext(builder.Options);
    }

    private static byte[] CreateAesKey(SecuritySettings security)
    {
        if (!SecuritySettings.TryDecodeArgon2Salt(security.Argon2SaltBase64, out byte[] seed) || seed.Length == 0)
        {
            return Array.Empty<byte>();
        }

        return SHA256.HashData(seed);
    }

    private string DecodeAccountId(byte[] encryptedId)
    {
        if (_aesKey.Length == 0 || encryptedId == null || encryptedId.Length == 0)
        {
            return string.Empty;
        }

        if (!CryptoHelper.TryDecryptAesGcm(_aesKey, encryptedId, out byte[] plaintext))
        {
            return "<decrypt failed>";
        }

        return Encoding.UTF8.GetString(plaintext).TrimEnd('\0');
    }

    private sealed class NormalAccountRow
    {
        public int Serial { get; init; }
        public string AccountId { get; init; } = string.Empty;
        public DateTime CreateTime { get; init; }
        public string CreateIp { get; init; } = string.Empty;
        public DateTime LastLoginTime { get; init; }
        public DateTime LastLogoffTime { get; init; }
        public string LastConnectIp { get; init; } = string.Empty;
        public int TotalLogMin { get; init; }
        public int JoinCode { get; init; }
        public byte LoginFailureCnt { get; init; }
        public byte UiLock { get; init; }
    }

    private sealed class GmAccountRow
    {
        public int Serial { get; init; }
        public string AccountId { get; init; } = string.Empty;
        public byte Grade { get; init; }
        public byte SubGrade { get; init; }
        public DateTime LastLoginDt { get; init; }
        public DateTime LastLogoffDt { get; init; }
        public string LastConnIp { get; init; } = string.Empty;
        public DateTime ExpireDt { get; init; }
        public int TotalLogMin { get; init; }
    }

    private sealed class BanAccountRow
    {
        public int AccountSerial { get; init; }
        public string AccountId { get; init; } = string.Empty;
        public DateTime StartDate { get; init; }
        public int Period { get; init; }
        public byte Kind { get; init; }
        public string Reason { get; init; } = string.Empty;
        public string GmReason { get; init; } = string.Empty;
        public string GmWriter { get; init; } = string.Empty;
        public byte ReasonType { get; init; }
    }
}
