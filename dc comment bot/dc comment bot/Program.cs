using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace dc_comment_bot
{
    class Program
    {
        static async Task Main()
        {
            var a = "6937d98f88a7af58d6a5f841f8f5c1ea";
            var input = "m";
            Console.WriteLine(a);

            var data = Encoding.UTF8.GetBytes($"{input}");
            var c = System.Security.Cryptography.MD5.HashData(data);
            Console.WriteLine(Convert.ToHexString(c));


            /*var data = Encoding.UTF8.GetBytes(input);
            for (int i = 0; ; ++i)
            {
                data = System.Security.Cryptography.MD5.HashData(data);
                var c = Convert.ToHexString(data);
                if(c.IndexOf(a, StringComparison.OrdinalIgnoreCase) != -1)
                {
                    Console.WriteLine("찾았다");
                    break;
                }
                Console.WriteLine(c);
            }*/
            return;
            const string serverIP = "127.0.0.1";
            const int port = 3306;
            const string databaseName = "issue";
            const string userName = "root";
            const string password = "54s1VOLid74I6oLoyi7ugeWIvoYetA";


            using var db = DBConnection.Instance;
            if (!await db.ConnectAsync(serverIP, port, databaseName, userName, password))
            {
                await Logger.LogAsync("연결 실패!");
                Console.ReadLine();
                return;
            }
            using var sqlExecutor = new SqlExecutor(db.SqlConnection);
            HttpManager httpManager = HttpManager.Instance;
            var dc = DCCrawler.GetInstance(httpManager);
            
            
            await dc.WriteCommentAsync(new DCPostMetadata { GalleryName = "bitcoins", PostId = 9468516, IsMinor = false }, "ㅇㅇ", "0210", "test123");

            return;
            var galleries = new (string name, string description, bool isMinor)[] {
                ("electronicmoney", "전자화폐", true),
                ("bitcoin", "비트코인", false), 
            };
            var keywords = new[] { "dfdf"};
            while(true)
            {
                foreach(var (galleryName, description, isMinor) in galleries)
                {
                    try
                    {
                        var postIds = await dc.GetPostIdsAsync(new DCPostMetadata { GalleryName = galleryName, IsMinor = isMinor }, 1, keywords);
                        var postIdsFromDatabase = await sqlExecutor.GetPostIdsAsync(galleryName);

                        foreach (var postId in postIds.Except(postIdsFromDatabase))
                        {
                            
                        }
                    }
                    catch(Exception e)
                    {
                        Console.WriteLine(e);
                        return;
                    }
                }
            }
        }
    }
}
