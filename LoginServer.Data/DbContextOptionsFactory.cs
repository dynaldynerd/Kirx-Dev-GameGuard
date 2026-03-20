using Microsoft.EntityFrameworkCore;
using Pomelo.EntityFrameworkCore.MySql.Infrastructure;

namespace LoginServer.Data;

public enum LoginDatabaseProvider
{
    SqlServer = 0,
    MariaDb = 1,
    Sqlite = 2
}

public static class DbContextOptionsFactory
{
    public static DbContextOptions<TContext> Create<TContext>(LoginDatabaseProvider provider, string connectionString)
        where TContext : DbContext
    {
        var builder = new DbContextOptionsBuilder<TContext>();

        switch (provider)
        {
            case LoginDatabaseProvider.Sqlite:
                builder.UseSqlite(connectionString);
                break;
            case LoginDatabaseProvider.MariaDb:
                builder.UseMySql(connectionString, new MariaDbServerVersion(new Version(10, 4, 0)));
                break;
            default:
                builder.UseSqlServer(connectionString);
                break;
        }

        return builder.Options;
    }
}
