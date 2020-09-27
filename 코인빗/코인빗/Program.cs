using HtmlAgilityPack;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Serialization;
using OpenQA.Selenium;
using OpenQA.Selenium.Edge;
using OpenQA.Selenium.Support.UI;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Runtime.CompilerServices;
using System.Text.Json;
using System.Threading.Tasks;
using Telegram.Bot;
using Telegram.Bot.Types;

namespace 코인빗
{
    class Program
    {
        
        static async Task Main()
        {
            var bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
            var myChatId = "581720134";
            var minseoksChatId = "455892097";
            var client = new HttpClient();
            
            var document = new HtmlDocument();
            var keywords = new[] { "이벤트", "에어드롭", "에어드랍", "스냅샷", "상장", "응모", "선착"};

            client.DefaultRequestHeaders.Add("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36 Edg/83.0.478.45");

            var now = DateTime.Now;

            Console.Title = "코인빗";


            bool eventStarted = true;            

            if(eventStarted)
            {
                var edgeOptions = new EdgeOptions
                {
                    UseChromium = true,
                    BinaryLocation = @"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
                };

                var service = EdgeDriverService.CreateDefaultServiceFromOptions(Directory.GetCurrentDirectory(), "msedgedriver.exe", edgeOptions);

                //service.HideCommandPromptWindow = true;
                bool condition = false;

                using IWebDriver driver = new EdgeDriver(service, edgeOptions)
                {
                    Url = "https://www.coinbit.co.kr/login"
                };
                var wait = new WebDriverWait(driver, TimeSpan.FromSeconds(300));

                wait.Until(d => d.FindElements(By.XPath("//div[@class='menu login']")).Count != 0);

                driver.FindElement(By.XPath("//input[@class='email']")).SendKeys("mnjihw@naver.com");
                driver.FindElement(By.XPath("//input[@class='password']")).SendKeys("flema0288!");
                driver.FindElement(By.Id("submit-login-btn")).Click();
                
                wait.Until(d => d.FindElements(By.XPath("//div[@class='TradingPairBoxLarge']")).Count != 0);
                
                
                var checkBox = driver.FindElement(By.XPath("//img[@class='checkbox']"));
                if (checkBox != null)
                {
                    checkBox.Click();
                    driver.FindElement(By.XPath("//div[@class='close-button styled-blue-button']")).Click();
                }
                driver.Url = "https://community.coinbit.co.kr/bbs/board.php?bo_table=free";

                while (true)
                {
                    //버튼 1개 혹은 정상페이지중 뭐가 먼저 뜰떄까지 루프돌다가 버튼 나오면 클릭후 프로그램 종료
                    //정상페이지 나오면 3초 후에 새로고침
                    Console.WriteLine(driver.FindElements(By.TagName("button")).Count);
                    await Task.Delay(300);
                }
                Console.ReadLine();

            }
            
            while (true)
            {
                string result = "";

                try
                {
                    if (eventStarted)
                        result = await client.GetStringAsync("https://community.coinbit.co.kr/bbs/board.php?bo_table=free");
                    else
                        result = await client.GetStringAsync("https://production-api.coinbit.global/api/v1.0/announcements/");
                }
                catch(Exception e)
                {
                    Console.WriteLine(e);
                    await bot.SendTextMessageAsync(myChatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(30));
                    continue;
                }

                Console.WriteLine($"[{DateTime.Now:HH:mm:ss}]작동중...");
                JsonDocument json;
                if (eventStarted)
                {
                    document.LoadHtml(result);
                    var nodes = document.DocumentNode.SelectNodes("//button");

                    if (nodes.Count == 1)
                    {
                        //nodes[0].
                    }
                }
                else
                {
                    try
                    {
                        json = JsonDocument.Parse(result);
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                        await bot.SendTextMessageAsync(myChatId, e.ToString());
                        await Task.Delay(TimeSpan.FromSeconds(30));
                        continue;
                    }

                    if (!json.RootElement.TryGetProperty("results", out var results))
                    {
                        Console.WriteLine("json 파싱 안됨 " + result);
                        await bot.SendTextMessageAsync(myChatId, "json 파싱 안됨 " + result);
                        await Task.Delay(TimeSpan.FromSeconds(30));
                        continue;
                    }

                    foreach (var element in results.EnumerateArray())
                    {
                        var created = element.GetProperty("created").GetString();
                        if (!DateTime.TryParse(created, out var createdTime))
                        {
                            Console.WriteLine("글 작성 시간 파싱 안됨 " + created);
                            await bot.SendTextMessageAsync(myChatId, "글 작성 시간 파싱 안됨 " + created);
                            continue;
                        }
                        if (createdTime > now)
                        {
                            var title = element.GetProperty("korean_title").GetString();
                            var content = element.GetProperty("korean_content").GetString();
                            string message;

                            if (keywords.Any(keyword =>
                             {
                                 return title.Contains(keyword) || content.Contains(keyword);
                             }))
                                message = $"[이벤트] {title}";
                            else
                                message = title;
                            await bot.SendTextMessageAsync(myChatId, message);
                            await bot.SendTextMessageAsync(minseoksChatId, message);
                            Console.WriteLine(message);
                            now = DateTime.Now;
                        }
                    }
                }
                await Task.Delay(TimeSpan.FromSeconds(30));
            }
        }
    }
}
