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
            entity.HasKey(e => e.IdHmac);
            entity.Property(e => e.IdHmac).HasColumnName("id_hmac");
            entity.Property(e => e.IdEnc).HasColumnName("id_enc");
            entity.Property(e => e.PasswordHash).HasColumnName("password_hash");
            entity.Property(e => e.AccountType).HasColumnName("accounttype");
        });
    }

    public DbSet<Account> Accounts => Set<Account>();
}
