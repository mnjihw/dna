using CrawlerHelper.Util;
using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Data;
using System.Text;
using System.Threading.Tasks;

namespace TodayHumorCrawler
{
    public class SqlExecutor : IDisposable
    {
        public MySqlCommand SqlCommand { get; set; }
        private bool DisposedValue { get; set; }

        public SqlExecutor(MySqlConnection sqlConnection)
        {
            SqlCommand = sqlConnection.CreateCommand();
            SqlCommand.Connection = sqlConnection;
            SqlCommand.CommandTimeout = 500;
        }


        private async Task BeginTransactionAsync()
        {
            SqlCommand.Transaction = await SqlCommand.Connection.BeginTransactionAsync();
        }

        private async Task CommitAsync()
        {
            await SqlCommand.Transaction.CommitAsync();
        }
        private async Task RollbackAsync()
        {
            await SqlCommand.Transaction.RollbackAsync();
        }

        private async Task DisposeTransactionAsync()
        {
            await SqlCommand.Transaction.DisposeAsync();
        }

        public async Task<IEnumerable<long>> GetPostIdsAsync()
        {
            var list = new List<long>();

            SqlCommand.Parameters.Clear();
            SqlCommand.CommandText = $"SELECT * FROM archived_posts WHERE site_name = 'todayhumor'";

            try
            {
                await using var reader = await SqlCommand.ExecuteReaderAsync();

                while (await reader.ReadAsync())
                    list.Add(reader.GetInt64("post_id"));
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }

            return list;
        }

        public async Task<bool> TitleHashExistsAsync(string hash)
        {
            using var commandBuilder = new MySqlCommandBuilder();

            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 50);


            SqlCommand.CommandText = $"SELECT COUNT(*) FROM title_hashes WHERE hash = @hash";
            SqlCommand.Parameters["@hash"].Value = hash;

            try
            {
                return (long)await SqlCommand.ExecuteScalarAsync() != 0;
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
                return false;
            }


        }
        public async Task<bool> ContentHashExistsAsync(string hash)
        {
            using var commandBuilder = new MySqlCommandBuilder();

            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 50);


            SqlCommand.CommandText = $"SELECT COUNT(*) FROM content_hashes WHERE hash = @hash";
            SqlCommand.Parameters["@hash"].Value = hash;

            try
            {
                return (long)await SqlCommand.ExecuteScalarAsync() != 0;
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
                return false;
            }


        }


        public async Task<bool> InsertTitleHashAsync(string hash)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 50).Value = hash;
            SqlCommand.CommandText = $"INSERT INTO title_hashes (hash) VALUES (@hash)";

            await BeginTransactionAsync();

            try
            {
                await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
                return false;
            }


            return true;
        }

        public async Task<bool> InsertContentHashAsync(string hash)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 50).Value = hash;
            SqlCommand.CommandText = $"INSERT INTO content_hashes (hash) VALUES (@hash)";

            await BeginTransactionAsync();

            try
            {
                await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
                return false;
            }

            return true;
        }

        public async Task<bool> InsertPostIdAsync(string boardName, long postId)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@board_name", MySqlDbType.VarChar, 50).Value = boardName;
            SqlCommand.Parameters.Add("@post_id", MySqlDbType.Int64).Value = postId;
            SqlCommand.CommandText = "INSERT INTO archived_posts (site_name, board_name, post_id) VALUES ('todayhumor', @board_name, @post_id)";

            await BeginTransactionAsync();

            try
            {
                await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
                return false;
            }

            return true;
        }


        #region Dispose
        protected virtual void Dispose(bool disposing)
        {
            if (!DisposedValue)
            {
                if (disposing)
                {
                    // TODO: 관리형 상태(관리형 개체)를 삭제합니다.
                }
                SqlCommand.Dispose();

                // TODO: 비관리형 리소스(비관리형 개체)를 해제하고 종료자를 재정의합니다.
                // TODO: 큰 필드를 null로 설정합니다.
                DisposedValue = true;
            }
        }

        // // TODO: 비관리형 리소스를 해제하는 코드가 'Dispose(bool disposing)'에 포함된 경우에만 종료자를 재정의합니다.
        ~SqlExecutor()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(false);
        }

        public void Dispose()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
