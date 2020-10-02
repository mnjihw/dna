using System;
using System.Threading.Tasks;
using Telegram.Bot;
using Telegram.Bot.Types;
using System.Timers;
using System.Text.RegularExpressions;
using System.Runtime.CompilerServices;
using System.Net.Http;
using HtmlAgilityPack;
using OpenQA.Selenium.Edge;
using OpenQA.Selenium;
using System.IO;
using OpenQA.Selenium.Support.UI;
using OpenQA.Selenium.Interactions;
using System.Collections.ObjectModel;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Runtime.InteropServices.WindowsRuntime;
using OpenQA.Selenium.DevTools.Page;
using Newtonsoft.Json.Serialization;
using System.ComponentModel.Design;
using System.Web;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Security.Principal;

namespace ConsoleApp2
{





    class Program
    {
        private static HttpClient Client { get; } = new HttpClient();



        static async Task Main()
        {
          
            return;
            string result;


            Client.DefaultRequestHeaders.Add("Authorization", "bearer guest");
            const string restaurantId = "10759827";
            const int limit = 50;

            var keyword = "네네치킨";



            result = await Client.GetStringAsync($"http://shopdp-api.baemin.com/v1/SEARCH/shops?keyword=%EB%84%A4%EB%84%A4%EC%B9%98%ED%82%A8&filter=&sort=SORT__DEFAULT&kind=DEFAULT&limit=25&latitude=35.882825&longitude=128.671131");
            var json = JsonDocument.Parse(result);
            foreach (var shop in json.RootElement.GetProperty("data").GetProperty("shops").EnumerateArray())
            {
                Console.WriteLine(shop.GetProperty("shopInfo").GetProperty("shopName").GetString());
            }
            /*
            var keyword = "냉면";
            var result = await client.GetStringAsync($"http://review-api.baemin.com/v1/shops/{restaurantId}/reviews?sort=MOST_RECENT&filter=ALL&offset=0&limit={limit}&adid=NONE&osver=22&oscd=2");
            var json = JsonDocument.Parse(result);
            var reviews = json.RootElement.GetProperty("data").GetProperty("reviews");

            foreach(var review in reviews.EnumerateArray())
            {
                foreach(var menu in review.GetProperty("menus").EnumerateArray())
                {
                    var menuName = menu.GetProperty("name").GetString();

                    if(menuName.Contains(keyword) && review.TryGetProperty("images", out var images))
                    {
                        foreach(var image in images.EnumerateArray())
                        {
                            var url = image.GetProperty("url").GetString();
                            Console.WriteLine(url);
                        }
                    }

                }

                

                *//*if(review.TryGetProperty("contents", out var contents) && !string.IsNullOrWhiteSpace(contents.GetString()))
                {
                    Console.WriteLine("리뷰: " + contents.GetString());
                }*//*
                
            }*/


            return;

            var edgeOptions = new EdgeOptions
            {
                UseChromium = true,
                BinaryLocation = @"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
            };

            var service = EdgeDriverService.CreateDefaultServiceFromOptions(Directory.GetCurrentDirectory(), "msedgedriver.exe", edgeOptions);

            using IWebDriver driver = new EdgeDriver(service, edgeOptions)
            {
                Url = "https://music.youtube.com"
            };
            driver.Navigate();
            var wait = new WebDriverWait(driver, TimeSpan.FromSeconds(300));

            #region Login

            driver.FindElement(By.XPath("//a[@class='sign-in-link style-scope ytmusic-nav-bar']")).Click();

            var idInput = driver.FindElement(By.Id("identifierId"));
            idInput.SendKeys("mnjihw0210");
            idInput.SendKeys(Keys.Enter);
            IWebElement passwordInput = default;

            wait.Until(d =>
            {
                passwordInput = d.FindElement(By.XPath("//input[@name='password']"));
                return passwordInput.Displayed;
            });
            passwordInput.SendKeys("flema0288");
            passwordInput.SendKeys(Keys.Enter);
            #endregion

            await Task.Delay(5000);
            driver.Url = "https://music.youtube.com";


            wait.Until(d => d.FindElement(By.XPath("//*[@id='progress']/..")).GetAttribute("hidden"));
            var files = Directory.GetFiles(@"D:\모든 파일\음악");

            for (int i = 242; i < files.Length; ++i)
            {
                var match = Regex.Match(files[i].Split('\\')[^1].Replace(".mp3", ""), @".+-\d{2}-([^-]+)");

                if (match.Success)
                {
                    driver.FindElement(By.XPath("//paper-icon-button[@class='search-icon style-scope ytmusic-search-box']")).Click();
                    //driver.FindElement(By.Id("placeholder")).Click();
                    var input = driver.FindElement(By.XPath("//input[@id='input']"));

                    input.SendKeys(match.Groups[1].Value);
                    input.SendKeys(Keys.Enter);

                    wait.Until(d => d.FindElement(By.XPath("//*[@id='progress']/..")).GetAttribute("hidden"));


                    Console.Write($"[{i}] {match.Groups[1]} 추가할 번호 입력: ");
                    int number = int.Parse(Console.ReadLine());
                    if (number != 0)
                    {

                        var items = driver.FindElements(By.XPath("//ytmusic-shelf-renderer[@class='style-scope ytmusic-section-list-renderer']/div[@id='contents'][@class='style-scope ytmusic-shelf-renderer']/ytmusic-responsive-list-item-renderer"));


                        (driver as IJavaScriptExecutor).ExecuteScript("arguments[0].click();", items[number - 1].FindElement(By.XPath("ytmusic-menu-renderer/paper-icon-button")));


                        var likeButton = driver.FindElements(By.XPath("//ytmusic-toggle-menu-service-item-renderer"))[^1]; //좋아요 추가 버튼

                        if (likeButton.Text.Contains("추가"))
                            likeButton.Click();
                    }
                    driver.FindElement(By.XPath("//paper-icon-button[@id='clear']")).Click();
                    //await Task.Delay(100);
                }
                else
                {
                    //Console.WriteLine(file.Split('\\')[^1]);
                }

            }




            Console.ReadLine();
        }
    }

}


/*
 *   var path = @"C:\Users\mnjihw\Desktop\Camera";
            var files = Directory.GetFiles(path);

            int i = 0;
            Stopwatch s = new Stopwatch();
            s.Start();
            foreach (var file in files)
            {
                ++i;
                if (i == 1000)
                    break;
                var info = Image.Identify(file);
                //var info = Image.Identify(@"C:\Users\mnjihw\Desktop\20200527_231155.jpg");
                var exifProfile = info?.Metadata?.ExifProfile;

                var latitude = exifProfile?.GetValue(ExifTag.GPSLatitude);
                var latitudeRef = exifProfile?.GetValue(ExifTag.GPSLatitudeRef);
                var longitude = exifProfile?.GetValue(ExifTag.GPSLongitude);
                var longitudeRef = exifProfile?.GetValue(ExifTag.GPSLongitudeRef);

                if (latitude != null && latitudeRef != null && longitude != null && longitudeRef != null)
                {
                 //   Console.WriteLine($"{latitude.Value[0]} {latitude.Value[1]} {latitude.Value[2]} {longitude.Value[0]} {longitude.Value[1]} {longitude.Value[2]}");
                }
                else
                {
                    if (latitude == null && latitudeRef == null && longitude == null && longitudeRef == null) ;
                    else
                        Console.WriteLine("뭔가 이상");
                }


                continue;
                foreach(var v in info.Metadata.ExifProfile.Values)
                {
                    //Console.WriteLine($"{ v.Tag} {v}");
                }
            }
            s.Stop();
            Console.WriteLine( s.ElapsedMilliseconds);
            return;
            HtmlDocument document = new HtmlDocument();
            CookieContainer cookie = new CookieContainer();
            HttpClientHandler handler = new HttpClientHandler { CookieContainer = cookie};
            HttpClient client = new HttpClient(handler);

            var result = await client.GetStringAsync("https://gall.dcinside.com/mgallery/board/lists?id=of");
            document.LoadHtml(result);
            var n = document.DocumentNode.SelectSingleNode(@"//div[@class='info_contbox']");
            Console.WriteLine(n.InnerText.Contains("언제나"));
                
            return;
            
            
            var postData = new Dictionary<string, string>
            {
                ["user_id"] = "akfwhtla2",
                ["pw"] = "rbtkarns1",
                ["s_url"] = "//www.dcinside.com/",
                ["ssl"] = "Y",
            };

            //var result = await client.GetStringAsync("https://www.dcinside.com/");
            
            document.LoadHtml(result);
            var node = document.DocumentNode.SelectSingleNode(@"//form[@id='login_process']/input[not(contains(@name, 'ssl')) and not(contains(@name, 's_url'))]");
            var key = node.Attributes["name"].Value;
            var value = node.Attributes["value"].Value;
            postData.Add(key, value);
            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36 Edg/81.0.416.72");
            client.DefaultRequestHeaders.Add("Referer", "https://www.dcinside.com/");





            result = await (await client.PostAsync("https://dcid.dcinside.com/join/member_check.php", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();

            Console.WriteLine(result);
            result = await client.GetStringAsync("https://www.dcinside.com/");
            //Console.WriteLine(result);
            Console.WriteLine(!result.Contains("아이디 또는 비밀번호"));
            postData.Clear();
            var cookies = cookie.GetCookies(new Uri("https://dcinside.com"));
            
            foreach(Cookie cookie2 in cookies)
            {
                if(cookie2.Name == "ci_c")
                {
                    postData.Add("ci_t", cookie2.Value);
                    break;
                }
            }

            postData.Add("id", "of");
            postData.Add("nos[]", "124953");

            await client.GetStringAsync("https://gall.dcinside.com/mgallery/board/lists?id=of");

            client.DefaultRequestHeaders.Remove("Referer");
            //client.DefaultRequestHeaders.Add("Referer", "https://gall.dcinside.com/mgallery/board/lists/?id=of&page=7&search_pos=&s_type=search_all&s_keyword=Ejdnah");
            //client.DefaultRequestHeaders.Add("fcmToken", "cpMpWHpW48U:APA91bFPWxSdrGPtdkuiUYueYoF2lUUgpH9jSzr6p2VbRHfP-Lsf34PWwilh-ALBqrafA4O38wyiN_jZoWsl7nS-L_fgG4WtUHlmzz0tYD7W-JCaYqpyaqXI8embrYFBBxIJTRNN4Uh7");
            
            client.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");
            result = await (await client.PostAsync("https://gall.dcinside.com/ajax/minor_manager_board_ajax/delete_list", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();


            return;

            /* var handler = new HttpClientHandler { };
             var httpClient = new HttpClient(handler);
             var htmlDocument = new HtmlDocument();
             bool isMinor;
             string[] keywords = { "무망호"};
             var galleries = new Dictionary<string, string>
             {
                 ["닌텐도"] = "game_nintendo",
             };
             var list = new List<string>();


             isMinor = false;

             while (true)
             {
                 string result = "";

                 for (int i = 0; i < 2; ++i)
                 {
                     result = await httpClient.GetStringAsync($"https://gall.dcinside.com/{(isMinor ? "mgallery/" : "")}board/lists?id=game_nintendo");
                     if (result.Contains("location.replace"))
                         isMinor = true;                        
                     else
                         break;
                 }

                 htmlDocument.LoadHtml(result);
                 var articles = htmlDocument.DocumentNode.SelectNodes("//table[@class='gall_list ']/tbody/tr[@class='ub-content us-post'][not(@data-type='icon_notice')]/td[@class='gall_tit ub-word']/a[not(@class)]");

                 foreach (var article in articles)
                 {
                     var title = article.InnerText;

                     if (keywords.Any(s => title.Contains(s)))
                     {
                         var number = Regex.Match(article.Attributes["href"].Value, @"no=(\d+)&").Groups[1].Value;

                         if(!list.Contains(number))
                         {
                             list.Add(number);
                             Console.WriteLine("새글뜸 " + title);
                         }
                     }
                 }
                 await Task.Delay(5000);
             }


             return;
             using (var ws = new ClientWebSocket())
                         {
                             var cancellationTokenSource = new CancellationTokenSource(5000);

                             Task task;
                             try
                             {
                                 task = ws.ConnectAsync(new Uri("wss://stream.binance.com:9443/ws/!ticker@arr"), cancellationTokenSource.Token);                   
                             }
                             catch(WebSocketException e)
                             {
                     Console.WriteLine(e);
                                 return;
                             }
                             while (!task.IsCompleted && !cancellationTokenSource.IsCancellationRequested)
                             {
                                 await Task.Delay(300);
                             }
                             if (cancellationTokenSource.IsCancellationRequested)
                             {
                                 Console.WriteLine("타임아웃");
                                 //continue;
                             }

                             byte[] buf = new byte[16384];

                             using (var stream = new MemoryStream())
                             {
                                 while (ws.State == WebSocketState.Open)
                                 {
                                     stream.SetLength(0);

                                     while (true)
                                     {
                                         WebSocketReceiveResult response;
                                         Task<WebSocketReceiveResult> receiveTask;

                                         try
                                         {
                                             receiveTask = ws.ReceiveAsync(new ArraySegment<byte>(buf), cancellationTokenSource.Token);
                                         }
                                         catch (WebSocketException e)
                                         {
                                 Console.WriteLine(e);
                                             continue;
                                         }

                                         while (!receiveTask.IsCompleted && !cancellationTokenSource.IsCancellationRequested)
                                         {
                                             await Task.Delay(300);
                                         }

                                         response = await receiveTask;

                                         if (cancellationTokenSource.IsCancellationRequested)
                                         {
                                             Console.WriteLine("타임아웃");
                                             //continue;
                                         }


                                         stream.Write(buf, 0, response.Count);

                                         if (response.EndOfMessage)
                                         {
                                             var res = Encoding.UTF8.GetString(stream.ToArray());

                                             JArray a = null;

                                             a = JArray.Parse(res);


                                             for (int i = 0; i < a.Count; ++i)
                                             {
                                                 Console.WriteLine($"{a[i]["s"]} {a[i]["c"]}");

                                             }
                                             break;
                                         }

                                         if (response.MessageType == WebSocketMessageType.Close)
                                         {
                                             Console.WriteLine("끊겼음");
                                             await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, "closed", CancellationToken.None);
                                         }
                                     }
                                 }
                             }



                         }
              */
