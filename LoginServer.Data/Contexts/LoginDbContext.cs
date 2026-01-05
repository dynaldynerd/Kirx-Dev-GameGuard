using Microsoft.EntityFrameworkCore;
using LoginServer.Data.Entities;

namespace LoginServer.Data.Contexts;

public sealed class LoginDbContext : DbContext
{
    public LoginDbContext(DbContextOptions<LoginDbContext> options) : base(options)
    {
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);
        modelBuilder.Entity<Account>(entity =>
        {
            entity.ToTable("tbl_rfaccount");
            entity.HasKey(e => e.Id);
            entity.Property(e => e.Id).HasMaxLength(13);
            entity.Property(e => e.Password).HasMaxLength(32);
        });
    }

    public DbSet<Account> Accounts => Set<Account>();
}
