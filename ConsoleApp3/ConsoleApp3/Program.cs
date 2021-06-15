using System;
using System.Net.Http;
using System.Threading.Tasks;
using System.Net;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Text.Json;
using System.Net.Http.Headers;
using S22.Imap;
using System.Net.Sockets;

namespace ConsoleApp3
{
    public class Program
    {
        
        public static async Task Main()
        {
            var client = new HttpClient();

            var json = JsonSerializer.Serialize(new 
            {
                to = "dbo6t7oCTwefmjERaWUX8F:APA91bGOOpGUjYQvrD9Z8USw1SGqSuvdHRx-cYPz9QEWnYs63sNd9Itqq4wxiA4AUK1YBhgdSZ8B-AXgjnfpqeIha1rl6wUcJJoGE5DwhkQV3lwcwlPuFKR8eOpEwhB4W33l9ahY63cK",
                notification = new
                {
                    title = "test",
                    body = "hi123",
                },
            });
            var postData = new StringContent(json, Encoding.UTF8, "application/json");

            client.DefaultRequestHeaders.Add("Sender", "id=809218041682");
            //client.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/json");
            //client.DefaultRequestHeaders.TryAddWithoutValidation("Authorization", "key=AAAAvGknZ1I:APA91bHMf6cgA0L2DcTCWuO1Bh_vI1vUX4G8Tj7dp0CT3TzcCh9lQOSXH-EoJZa19K_0yiY8lZhzhQnJCgmLZ37RVVwaVLsg1gJdGAHzz9ibkAKrt_e5eRfW4ytyMNrxesoHRsmeIe-t");
            client.DefaultRequestHeaders.Authorization = new AuthenticationHeaderValue("key", "=AAAAvGknZ1I:APA91bHMf6cgA0L2DcTCWuO1Bh_vI1vUX4G8Tj7dp0CT3TzcCh9lQOSXH-EoJZa19K_0yiY8lZhzhQnJCgmLZ37RVVwaVLsg1gJdGAHzz9ibkAKrt_e5eRfW4ytyMNrxesoHRsmeIe-t");


            Console.WriteLine(client.DefaultRequestHeaders.Authorization);

            var result = await client.PostAsync("https://fcm.googleapis.com/fcm/send", postData);
            var res2 = await result.Content.ReadAsStringAsync();
            Console.WriteLine(res2);
            

            return;
            /*
            var tcpListener = new TcpListener(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 993));
            tcpListener.Start();
            var tcpClient = tcpListener.AcceptTcpClient();
            var stream = tcpClient.GetStream();

            var buf = Encoding.UTF8.GetBytes($"* OK{Environment.NewLine}");
            stream.Write(buf);
            while(true)
            {
                
            }
            return;
            //var client2 = new ImapClient("imap.naver.com", 993, "mnjihw", "oBcH5Ly!Ny9MH#", AuthMethod.Auto, true);

            Console.WriteLine("접속한다");

            var client2 = new ImapClient("127.0.0.1", 993, false);
            
            Console.WriteLine("완료");
            
            Console.ReadLine();
            return;
            var uids = client2.Search(SearchCondition.All());
            var messages = client2.GetMessages(uids.Take(10));

            foreach(var message in messages)
            {
                Console.WriteLine(message.Subject );
            }

            return;

            var cookieContainer = new CookieContainer();
            var handler = new HttpClientHandler { CookieContainer = cookieContainer };
            var client = new HttpClient(handler);
            var a = new[] { "https://www.naver.com", "", "https://www.google.co.kr", ""};
            IEnumerable<Task<string>> b = default;
            try
            {
                b = a.Select(d => client.GetStringAsync(d));
                await Task.WhenAll(b);
                foreach (var c in b)
                {
                    Console.WriteLine(c.IsFaulted);
                    Console.WriteLine(c.IsCompleted);
                }
            }
            catch
            {
                Console.WriteLine("DD");

            }
            for(int i = 0; i < b.Count(); ++i)
            {
                Console.WriteLine(b.ElementAt(i).IsFaulted);
            }
            return;


            client.DefaultRequestHeaders.Add("Referer", "https://www.instagram.com/");
            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.141 Safari/537.36 Edg/87.0.664.75");
            client.DefaultRequestHeaders.Add("X-IG-App-ID", "936619743392459"); //tray 드갈때도 보냄
            client.DefaultRequestHeaders.Add("X-IG-WWW-Claim", "0"); //tray드갈때 hmac 보냄
            client.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest"); //post에서 보냄


            var result = await client.GetStringAsync("https://www.instagram.com/data/shared_data/");
            await Task.Delay(300);
            var json = JsonDocument.Parse(result);
            var csrfToken = json.RootElement.GetProperty("config").GetProperty("csrf_token").GetString();
            var rolloutHash = json.RootElement.GetProperty("rollout_hash").GetString();
            var encryption = json.RootElement.GetProperty("encryption");
            var keyId = encryption.GetProperty("key_id").GetString();
            var publicKey = encryption.GetProperty("public_key").GetString();
            var version = encryption.GetProperty("version").GetString();

            client.DefaultRequestHeaders.Add("X-CSRFToken", csrfToken); //로그인시만
            client.DefaultRequestHeaders.Add("X-Instagram-AJAX", rolloutHash); //로그인시만


            var enc = Encrypt.GenerateEncPassword("flema0210~", publicKey, keyId, version);

            var postData = new Dictionary<string, string>
            {
                ["username"] = "_mnjihw",
                ["enc_password"] = enc,
                ["queryParams"] = "{}",
                ["optIntoOneTap"] = "false",
            };


            using var formUrlEncodedContent = new FormUrlEncodedContent(postData);
            formUrlEncodedContent.Headers.ContentType = MediaTypeHeaderValue.Parse("application/x-www-form-urlencoded");
            result = await (await client.PostAsync("https://www.instagram.com/accounts/login/ajax/", formUrlEncodedContent)).Content.ReadAsStringAsync();
            Console.WriteLine(result);

            */

            //result = await client.GetAsync("https://i.instagram.com/api/v1/feed/reels_tray/");


            return;

            /*
            var cookie = new CookieContainer();
            var handler = new HttpClientHandler { CookieContainer = cookie };
            var client = new HttpClient(handler);

            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36 Edg/87.0.664.66");
            client.DefaultRequestHeaders.Add("Update-Insecure-Requests", "1");
            client.DefaultRequestHeaders.Add("Referer", "https://www.ppomppu.co.kr/zboard/login.php");
            client.DefaultRequestHeaders.Add("Origin", "https://www.ppomppu.co.kr");
            client.DefaultRequestHeaders.Add("Host", "www.ppomppu.co.kr");

            var postData = new Dictionary<string, string>
            {
                ["s_url"] = "%2F",
                ["user_id"] = "mnjihw",
                ["password"] = "flema0210"
            };
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);


            var result = await (await client.PostAsync("https://www.ppomppu.co.kr/zboard/login_check.php?s_url=%2F", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();
            result = await client.GetStringAsync("http://www.ppomppu.co.kr/");
            Console.WriteLine(result);
            Console.WriteLine(result.Contains("리듬"));
            var s = cookie.GetCookieHeader(new Uri("http://www.ppomppu.co.kr"));
            */
            /*
            
                        
                var handler = new HttpClientHandler { AutomaticDecompression = DecompressionMethods.All };
                var client = new HttpClient(handler) { Timeout = TimeSpan.FromSeconds(10) };
                var document = new HtmlDocument();
                client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36 Edg/87.0.664.66");



                ScriptControl sc = new ScriptControl
                {
                    Language = "JavaScript"
                };


                Console.Write("URL 입력: ");
                var url = Console.ReadLine();

                var result = await client.GetStringAsync(url);
                var match = Regex.Match(result, @"(var flashvars_.+)playerObjList", RegexOptions.Singleline);
                if(!match.Success)
                {
                    Console.WriteLine("이상함");
                    return;
                }
                var data = match.Groups[1].Value;


                sc.ExecuteStatement(data);
                var match2 = Regex.Match(data, @"var (flashvars_\d+)");
                if (!match2.Success)
                {
                    Console.WriteLine("머냐 이상함");
                    return;
                }
                var name = match2.Groups[1].Value;
                int length = (int)sc.Eval($"{name}['mediaDefinitions'].length");

                for(int i = 0; i < length; ++i)
                {
                    var s = sc.Eval($"{name}['mediaDefinitions'][{i}]['videoUrl']");
                    Console.WriteLine(s);

                }
                Console.WriteLine();
*/




            /*



                        return;




                        //Referer 없으면 안됨
                        client.DefaultRequestHeaders.Add("Referer", "https://gall.dcinside.com/mgallery/board/view/?id=game_nintendo&no=633702&_rk=CFQ&exception_mode=recommend&page=1");

                        //압축 방식은 안바뀌는것같고 한 HttpClient 인스턴스를 공유해서 써야되니 중간에 바꾸는건 힘들어보임

                        //그래서 그냥 All로 고정으로 박고 쓰자
                        Console.Write("디시 URL 입력: ");
                         url = Console.ReadLine();
                        match = Regex.Match(url, @"&no=(\d+)");
                        var articleNumber = match.Groups[1].Value;

                        Directory.CreateDirectory(articleNumber);

                        while (true)
                        {
                            try
                            {
                                result = await client.GetStringAsync(url);
                            }
                            catch (HttpRequestException e)
                            {
                                Console.WriteLine(e);
                                await Task.Delay(2000);
                                continue;
                            }
                            break;
                        }

                        document.LoadHtml(result);


                        var title = document.DocumentNode.SelectSingleNode(@"//span[@class='title_subject']").InnerText;

                        var contentNode = document.DocumentNode.SelectSingleNode("//div[@class='writing_view_box']");


                        var nodes = contentNode.SelectNodes("//div[@class='writing_view_box']//img");


                        Console.Write("작동중");


                        var list = new List<Task<HttpResponseMessage>>();

                        foreach (var node in nodes)
                        {
                            list.Add(client.GetAsync(node.Attributes["src"].Value));
                        }
                        await Task.WhenAll(list);


                        for(int i = 0; i < list.Count; ++i)
                        {
                            var message = await list[i];
                            var fileName = message.Content.Headers.ContentDisposition.FileName;
                            var extension = fileName[fileName.LastIndexOf('.')..];
                            nodes[i].Attributes["src"].Value = $"{i}{extension}";
                            Console.WriteLine($"{i}{extension}");

                            using var fs = new FileStream($@"{articleNumber}\{i}{extension}", FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
                            await message.Content.CopyToAsync(fs);
                        }


                        Console.WriteLine();

                        await File.WriteAllTextAsync($@"{articleNumber}\content.html", title + contentNode.InnerHtml);

                        Console.WriteLine("완료");
                        Console.ReadLine();
            */

        }
    }
}