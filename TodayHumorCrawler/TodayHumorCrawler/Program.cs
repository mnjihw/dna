using CrawlerHelper.Db;
using CrawlerHelper.Http;
using CrawlerHelper.Util;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace TodayHumorCrawler
{
    class Program
    {
        static string ComputeSHA256Hash(string text) => Convert.ToBase64String(SHA256.HashData(Encoding.UTF8.GetBytes(text)));
        static async Task Main()
        {
            const string serverIP = "127.0.0.1";
            const int port = 3306;
            const string databaseName = "issue";
            const string userName = "root";
            const string password = "54s1VOLid74I6oLoyi7ugeWIvoYetA";

            Logger.Initialize(@"C:\crawler", "crawler_todayhumor", "1557742912:AAHJpIVYERHRxiXwME78ahwnYjg6-7tAbuE", 1390930895);
            using var db = DbConnection.Instance;
            if (!await db.ConnectAsync(serverIP, port, databaseName, userName, password))
            {
                await Logger.LogAsync("연결 실패!");
                Console.ReadLine();
                return;
            }
            using var sqlExecutor = new SqlExecutor(db.SqlConnection);

            var httpManager = HttpManager.Instance;
            var xe = XpressEngineManager.GetInstance(httpManager, "issue.house");
            if (!await xe.Login("ushort65535", "Ka8i6ud1GoTI6uQo"))
            {
                await Logger.LogAsync("로그인 실패!");
                Console.ReadLine();
                return;
            }
            await Logger.LogAsync("로그인 성공");

            string basePath = @"C:\Users\User\Desktop\이미지\오늘의유머";
            if (!Directory.Exists(basePath))
                basePath = @"D:\이미지\오늘의유머";

            var todayHumor = TodayHumorCrawler.GetInstance(httpManager);
            var boardName = "bestofbest";

            while (true)
            {
                for (int i = 1; i <= 10; ++i)
                {
                    try
                    {
                        var postIds = await todayHumor.GetPostIdsAsync(i);
                        var postIdsFromDatabase = await sqlExecutor.GetPostIdsAsync();

                        foreach (var postId in postIds.Except(postIdsFromDatabase))
                        {
                            var imageDatas = new List<(string fileName, Stream stream)>();
                            var (succeeded, title, content) = await todayHumor.GetPost(postId, imageDatas);

                            if (succeeded)
                            {
                                await Logger.LogAsync($"{boardName}/{postId} - [{title}] 긁음");

                                var titleHash = ComputeSHA256Hash(title);
                                var contentHash = ComputeSHA256Hash(content);

                                if (await sqlExecutor.TitleHashExistsAsync(titleHash) || await sqlExecutor.ContentHashExistsAsync(contentHash))
                                {
                                    await Logger.LogAsync("중복이라 20초 쉼");
                                    await Task.Delay(TimeSpan.FromSeconds(20));
                                    continue;
                                }
                                Directory.CreateDirectory($@"{basePath}\{boardName}\{postId}");
                                foreach (var imageData in imageDatas)
                                {
                                    if (string.Compare(Path.GetExtension(imageData.fileName), ".webp", true) == 0 ||
                                        string.Compare(Path.GetExtension(imageData.fileName), ".gif", true) == 0)
                                    {
                                        using var fs = new FileStream(@$"{basePath}\{boardName}\{postId}\{imageData.fileName}", FileMode.Create);
                                        await imageData.stream.CopyToAsync(fs);
                                    }
                                    else
                                    {
                                        using var image = Image.FromStream(imageData.stream);

                                        int desiredHeight;

                                        if (image.Height > 800)
                                            desiredHeight = 800;
                                        else
                                        {
                                            if (image.Height <= 50)
                                                desiredHeight = image.Height;
                                            else
                                                desiredHeight = image.Height - 50;
                                        }

                                        using var bitmap = new Bitmap(image.Width, desiredHeight);
                                        using var graphics = Graphics.FromImage(bitmap);
                                        graphics.DrawImage(image, 0, 0);
                                        bitmap.Save(@$"{basePath}\{boardName}\{postId}\{imageData.fileName}");

                                    }
                                }
                                var url = $"http://www.todayhumor.co.kr/board/view.php?table={boardName}&no={postId}";

                                await File.WriteAllTextAsync($@"{basePath}\{boardName}\{postId}\content.html",
                                    $@"<div id=""title"">{title}</div>" +
                                    $@"<div id=""content"">{content}" +
                                    $@"<div><p style=""text-align:center;"">...(중략)</p><br><p style=""text-align:center;"">" +
                                    $@"<a href=""{url}"" style=""font-size:24px;color:#000;"" rel=""nofollow"" target=""_blank"">" +
                                    $@"<strong>[오늘의유머에 남은 내용 보러가기]</strong></a></p></div></div>");

                                var writePostResult = await xe.WritePost("issue_todayhumor", @$"{basePath}\{boardName}\{postId}\content.html", true);

                                if (writePostResult == WritePostResult.Success)
                                {
                                    await Logger.LogAsync("글쓰기 완료");

                                    await sqlExecutor.InsertTitleHashAsync(titleHash);
                                    await sqlExecutor.InsertContentHashAsync(contentHash);
                                    await sqlExecutor.InsertPostIdAsync(boardName, postId);

                                    await Logger.LogAsync("DB에 글 정보 인서트 완료");
                                }
                                else
                                {
                                    Console.WriteLine("머냐 이상함" + writePostResult);
                                }
                            }
                            else
                            {
                                switch (title)
                                {
                                    case "1":
                                        await Logger.LogAsync($"{boardName}/{postId} 읽기 실패");
                                        break;
                                    case "2":
                                        await Logger.LogAsync($"{boardName}/{postId}이 하루 이상 된 글이라 스킵함");
                                        break;
                                    case "3":
                                        await Logger.LogAsync($"{boardName}/{postId}이 키워드 필터링에 걸림");
                                        break;
                                    case "4":
                                        await Logger.LogAsync($"{boardName}/{postId}에 이미지가 없어서 스킵함");
                                        break;
                                    default:
                                        await Logger.LogAsync($"제목이 {title}인데 실패했음");
                                        break;
                                }
                            }
                            await Logger.LogAsync("글 읽어서 20초 쉼");
                            await Task.Delay(TimeSpan.FromSeconds(20));
                        }

                        await Logger.LogAsync($"{i}페이지 다 저장해서 1분 쉼");
                        await Task.Delay(TimeSpan.FromMinutes(1));
                    }
                    catch (Exception e)
                    {
                        await Logger.SendTelegramMessageAsync(e.ToString());
                    }
                }
            }
        }
    }
}
