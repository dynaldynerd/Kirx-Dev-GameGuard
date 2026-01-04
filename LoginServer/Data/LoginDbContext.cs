using Microsoft.EntityFrameworkCore;

namespace LoginServer.Data;

/// <summary>
/// Placeholder EF Core DbContext for the login server. Add DbSet<T> as models are defined.
/// </summary>
public sealed class LoginDbContext : DbContext
{
    public LoginDbContext(DbContextOptions<LoginDbContext> options) : base(options)
    {
    }

    // TODO: add DbSet<Account>, DbSet<World>, etc.

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);
        // configure entities here when available
    }
}
