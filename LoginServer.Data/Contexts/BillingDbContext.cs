using LoginServer.Data.Entities;
using Microsoft.EntityFrameworkCore;

namespace LoginServer.Data.Contexts;

public sealed class BillingDbContext : DbContext
{
    public BillingDbContext(DbContextOptions<BillingDbContext> options) : base(options)
    {
    }

    public DbSet<BillingUserStatus> UserStatuses => Set<BillingUserStatus>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);
        modelBuilder.Entity<BillingUserStatus>(entity =>
        {
            entity.ToTable("tbl_UserStatus");
            entity.HasKey(e => e.Id);
            entity.Property(e => e.Id).HasMaxLength(16);
            entity.Property(e => e.Status).HasColumnName("Status");
            entity.Property(e => e.DtStartPrem).HasColumnName("DTStartPrem");
            entity.Property(e => e.DtEndPrem).HasColumnName("DTEndPrem");
            entity.Property(e => e.Cash).HasColumnName("Cash");
        });
    }
}
