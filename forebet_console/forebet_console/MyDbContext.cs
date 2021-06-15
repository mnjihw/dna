using Microsoft.EntityFrameworkCore;
using MySqlConnector;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace forebet_console
{
    class MyDbContext : DbContext
    {
        public DbSet<MatchData> MatchDatas { get; set; }
        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            const string serverIP = "127.0.0.1";
            const int port = 3306;
            const string databaseName = "suggestion";
            const string userName = "suggestion";
            const string password = "SIsEQe85rekASOfar6mAB115qAf82u";

            var connectionString = $"Server={serverIP};Port={port};Database={databaseName};Uid={userName};Password={password};CharSet=utf8;";
            optionsBuilder.UseMySql(connectionString, ServerVersion.AutoDetect(connectionString));
        }

        
    }
}
