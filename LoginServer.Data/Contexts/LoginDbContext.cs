using Microsoft.EntityFrameworkCore;

namespace LoginServer.Data.Contexts;

public sealed class LoginDbContext : DbContext
{
    public LoginDbContext(DbContextOptions<LoginDbContext> options) : base(options)
    {
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);
        // configure entities here when available
    }
}
