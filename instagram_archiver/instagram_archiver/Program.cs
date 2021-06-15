using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using System.Security.Cryptography;
using Instagram_archiverML.Model;
using Instagram_archiver.Util;
using Instagram_archiver.Db;
using Instagram_archiver.Instagram;
using Instagram_archiver.Http;
using HtmlAgilityPack;
using System.Text.RegularExpressions;
using System.Text;
using System.Net.Http;
using System.Diagnostics;
using System.Linq;
using System.Text.Json.Node;
using System.Text.Json;

namespace Instagram_archiver
{
    
    class Program
    {
        private static string ComputeSHA256Hash(Stream stream)
        {
            using SHA256 sha256 = SHA256.Create();
            var hash = sha256.ComputeHash(stream);
            stream.Seek(0, SeekOrigin.Begin);
            return Convert.ToHexString(hash);
        }

        private static async Task FinalizeImagesAsync2(SqlExecutor sqlExecutor, string finalPath)
        {
            var hashes = new List<string>();

            foreach (var directory in Directory.GetDirectories(finalPath))
            {
                var directoryName = new DirectoryInfo(directory).Name;

                foreach (var file in Directory.GetFiles(directory))
                {
                    var fileName = Path.GetFileName(file);
                    if (directoryName != "hot female")
                        hashes.Add($"'{Path.GetFileNameWithoutExtension(fileName)}'");
                }

            }
            Console.WriteLine($"{string.Join(", ", hashes)} 여자 사진 아니어서 디비에서 지움");
            
            var count = await sqlExecutor.DeleteImagesAsync(hashes);
            Console.WriteLine($"{count}개 지움");
        }

        private static async Task FinalizeImagesAsync(SqlExecutor sqlExecutor, string intermediatePath, string finalPath)
        {
            var hashes = new List<string>();

            foreach (var directory in Directory.GetDirectories(intermediatePath))
            {
                var directoryName = new DirectoryInfo(directory).Name;

                foreach (var file in Directory.GetFiles(directory))
                {
                    var fileName = Path.GetFileName(file);
                    if(directoryName != "hot female")
                        hashes.Add($"'{Path.GetFileNameWithoutExtension(fileName)}'"); 
                    File.Move(file, Path.Combine(finalPath, directoryName, fileName));
                    Console.WriteLine($"{file}을 {Path.Combine(finalPath, directoryName, fileName)}로 이동함");
                }

            }
            Console.WriteLine($"{string.Join(", ", hashes)} 여자 사진 아니어서 디비에서 지움");
            var count = await sqlExecutor.DeleteImagesAsync(hashes);
            Console.WriteLine($"{count}개 지움");
        }
        
        private static void ClassifyImages(string unclassifiedPath, string classifiedPath)
        {
            foreach (var file in Directory.GetFiles(unclassifiedPath))
            {
                var predictionResult = ConsumeModel.Predict(new() { ImageSource = file });

                var newPath = Path.Combine(classifiedPath, predictionResult.Prediction);
                Directory.CreateDirectory(newPath);
                File.Move(file, Path.Combine(newPath, Path.GetFileName(file)));
            }
        }
        static async Task Main()
        {
            
#if true
            var apiKey = "sFbP6ag_pC702mu11cZGqH7B_Fta4iLzsGYPhK04";
            var secretKey = "n8e-x0CuO55jkBqScXHjQ5NJy6s_5rYWhwvRBHOG";
            var client = new HttpClient();

            var nonce = DateTimeOffset.Now.ToUnixTimeMilliseconds();

            using var hmac = new HMACSHA256(Encoding.UTF8.GetBytes(secretKey));
            var hash = hmac.ComputeHash(Encoding.UTF8.GetBytes($"{nonce}GET/api/wallet/deposits"));

            client.DefaultRequestHeaders.Add("FTX-KEY", apiKey);
            client.DefaultRequestHeaders.Add("FTX-TS", nonce.ToString());
            client.DefaultRequestHeaders.Add("FTX-SIGN", Convert.ToHexString(hash).ToLower());

            string result;
            JsonDocument json;
            try
            {
                result = await client.GetStringAsync($"https://ftx.com/api/wallet/deposits");
                json = JsonDocument.Parse(result);
            }
            catch(Exception)
            {
                return;
            }
            
            
            if(json.RootElement.GetProperty("success").GetBoolean())
            {
                foreach(var element in json.RootElement.GetProperty("result").EnumerateArray())
                {
                    var confirmedTime = element.GetProperty("confirmedTime").GetString();

                    if(confirmedTime != null && (DateTime.Now - DateTime.Parse(confirmedTime)).TotalMinutes <= 5)
                    {
                        Console.WriteLine("D");
                    }
                        
                }
            }
            
            Console.WriteLine(result);
            
            return;
#endif
            var random = new Random();
            var httpManager = HttpManager.Instance;
            var igManager = InstagramManager.GetInstance(httpManager);

            const string serverIP = "127.0.0.1";
            const int port = 3306;
            const string databaseName = "issue";
            const string userName = "root";
            const string password = "54s1VOLid74I6oLoyi7ugeWIvoYetA";

            var imagesPath = Path.GetFullPath(Path.Combine("..", "..", "..", "images"));
            var unclassifiedPath = Path.Combine(imagesPath, "unclassified");
            var classifiedPath = Path.Combine(imagesPath, "classified");
            var intermediatePath = Path.Combine(imagesPath, "intermediate");
            var finalPath = Path.Combine(imagesPath, "final");


            Logger.Initialize(@"", "instagram_archiver", "1557742912:AAHJpIVYERHRxiXwME78ahwnYjg6-7tAbuE", 1390930895);
            using var db = DbConnection.Instance; 
            if (!await db.ConnectAsync(serverIP, port, databaseName, userName, password))
            {
                await Logger.LogAsync("연결 실패!");
                Console.ReadLine();
                return;
            }
            using var sqlExecutor = new SqlExecutor(db.SqlConnection);

            //await FinalizeImagesAsync2(sqlExecutor, finalPath);
            


            if (!await igManager.LoginAsync("larp.royal", "rB$i%QcXN*8Z4o1"))
            {
                Console.WriteLine("로그인 실패!");
                return;
            }
            var xe = XpressEngineManager.GetInstance(httpManager, "issue.house");
            if (!await xe.LoginAsync("ushort65535", "Ka8i6ud1GoTI6uQo"))
            {
                await Logger.LogAsync("로그인 실패!");
                Console.ReadLine();
                return;
            }

            var basePath = @"C:\Users\mnjihw\Documents\Visual Studio 2019\Projects\instagram_archiver\instagram_archiver\images\final\hot female";



            var users = await sqlExecutor.SelectImagesToUpload();
            foreach (var user in users)
            {
                foreach (var post in user.Posts)
                {
                    if(await xe.WritePostAsync("sns_instagram", basePath, user, post, true) == WritePostResult.Success)
                    {
                        await Logger.LogAsync("글쓰기 완료");
                        await sqlExecutor.UpdatePostToUploadedAsync(post.Id);
                        await Logger.LogAsync("DB에 업로드 여부 반영 완료");
                    }
                    else
                    {
                        await Logger.LogAsync("머냐 이상함");
                    }
                    //글올리는것까진 얼추됐고 이미지 읽어와서 디비에 넣는거랑
                }
            }

            return;

            for (int i = 0; ; ++i)
            {
                //var images = await igManager.GetImagesAsync(i);
                //var users = await igManager.GetUsersAsync(i);
                users = await igManager.GetUsersAsync(i);

                foreach (var user in users)
                {
                    foreach (var post in user.Posts)
                    {
                        foreach (var image in post.Images)
                        {
                            image.Hash = ComputeSHA256Hash(image.Stream);

                            var tuple = await sqlExecutor.SelectImageAsync(image.Hash);

                            if (tuple == default)
                            {
                                Console.WriteLine($"{image.Id} 처음 보는 이미지라 다운받고 디비에 추가함");

                                bool succeeded = true;
                                try
                                {
                                    await using var fs = new FileStream($@"{unclassifiedPath}\{image.Hash}.jpg", FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
                                    await image.Stream.CopyToAsync(fs);
                                }
                                catch (Exception e)
                                {
                                    succeeded = false;
                                    await Logger.SendTelegramMessageAsync(e.ToString());
                                }
                                if (succeeded)
                                {
                                    /*await sqlExecutor.InsertUserAsync(image.Post.Writer);
                                    await sqlExecutor.InsertPostAsync(image.Post);*/ //나중에 인서트문 합치든 3개로나누든 어쩃든 추가
                                    //한 유저의 한 포스트의 여러 이미지에 대해 돌았는데 디비에 어케 넣을지 고민
                                    await sqlExecutor.InsertImageAsync(image);
                                }
                            }
                        }
                    }
                }
                
                ClassifyImages(unclassifiedPath, classifiedPath);


                Console.WriteLine($"{i + 1}번 돌았음");

                if (i % 20 == 19)
                {
                    await Logger.LogAsync("이미지 분류하고 엔터치면 final로 옮김");
                    Console.ReadLine();
                }
                await FinalizeImagesAsync(sqlExecutor, intermediatePath, finalPath);
                await Task.Delay(10000 + (random.Next() % 500) + 300);
            }
        }
    }
}
