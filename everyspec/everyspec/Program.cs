using System;
using System.Net.Http;
using System.Threading.Tasks;

namespace everyspec
{
    /*
     * 1. 컬럼 조건식: = <= >= 3개(!=가 필요한지는 검토해보기)
     * 2. 컬럼 값 타입 종류: string, bool, double, int(TryParse 할때 int를 double보다 먼저 해야 함)
     * 3. 컬럼 추가시 문자열, 숫자, 참거짓 여부를 입력받고 참거짓일 경우 예/아니요만 선택 가능하게, 단위 어케할지
     */
    class Program
    {
        static async Task Main()
        {
            //if(int) ==> int
            //else if(double) ==> double
            var a = new Spec("D", "Dd");
            
            return;
            const string serverIP = "127.0.0.1";
            const int port = 3306;
            const string databaseName = "issue";
            const string userName = "root";
            const string password = "54s1VOLid74I6oLoyi7ugeWIvoYetA";
            using var db = DBConnection.Instance;
            if (!await db.ConnectAsync(serverIP, port, databaseName, userName, password))
            {
                Console.WriteLine("연결 실패!");
                return;
            }
            using var sqlExecutor = new SqlExecutor(db.SqlConnection);
           

        }
    }
}
