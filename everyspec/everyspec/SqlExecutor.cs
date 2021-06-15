using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace everyspec
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

        //public async Task<IEnumerable<>> Get()
        //{

        //}

        /*public async Task<IEnumerable<RuliwebPostMetadata>> GetPostMetadatasAsync()
        {
            var list = new List<RuliwebPostMetadata>();

            SqlCommand.Parameters.Clear();
            SqlCommand.CommandText = $"SELECT * FROM archived_posts WHERE site_name = 'ruliweb'";

            try
            {
                await using var reader = await SqlCommand.ExecuteReaderAsync();

                while (await reader.ReadAsync())
                    list.Add(new RuliwebPostMetadata { BoardName = reader.GetString("board_name"), PostId = reader.GetInt32("post_id") });
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }

            return list;
        }*/



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
