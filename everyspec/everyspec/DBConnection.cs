using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace everyspec
{
    public sealed class DBConnection : IDisposable
    {
        public MySqlConnection SqlConnection { get; set; }

        private bool DisposedValue { get; set; }

        private static Lazy<DBConnection> InstanceHolder { get; } = new Lazy<DBConnection>(() => new DBConnection());
        public static DBConnection Instance => InstanceHolder.Value;
        private DBConnection() { }


        public async Task<bool> ConnectAsync(string serverIP, int port, string databaseName, string userName, string password)
        {
            SqlConnection = new MySqlConnection($"Server={serverIP};Port={port};Database={databaseName};Uid={userName};Password={password};CharSet=utf8;");

            try
            {
                await SqlConnection.OpenAsync();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }

            return true;
        }





        #region Dispose
        private void Dispose(bool disposing)
        {
            if (!DisposedValue)
            {
                if (disposing)
                {
                    // TODO: 관리형 상태(관리형 개체)를 삭제합니다.
                }
                SqlConnection.Dispose();
                // TODO: 비관리형 리소스(비관리형 개체)를 해제하고 종료자를 재정의합니다.
                // TODO: 큰 필드를 null로 설정합니다.
                DisposedValue = true;
            }
        }

        // // TODO: 비관리형 리소스를 해제하는 코드가 'Dispose(bool disposing)'에 포함된 경우에만 종료자를 재정의합니다.
        ~DBConnection()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(false);
        }

        public void Dispose()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
