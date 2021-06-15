using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Telegram.Bot;

namespace 임경민2
{
    class Program
    {
        static async Task Main()
        {
            var htmlDocument = new HtmlDocument();
            var httpClient = new HttpClient { Timeout = TimeSpan.FromSeconds(10)};
            var postIds = new List<int>();
            var bot  = new TelegramBotClient("1714871392:AAEuSi327mCCTfA28Rv1ly30XDGZQCDGK9A"); //1
            var chatId = 1742992868;


            while(true)
            {
                string result = "";
                try
                {
                    result = await httpClient.GetStringAsync($"https://quasarzone.com/bbs/qf_hwjoin?page=1");
                }
                catch(TaskCanceledException)
                {

                }
                catch(HttpRequestException)
                {

                }
                catch(Exception e)
                {
                    Console.WriteLine(e);
                }
                htmlDocument.LoadHtml(result);

                var nodes = htmlDocument.DocumentNode.SelectNodes("//div[@class='dabate-type-list']/table/tbody/tr/td[3]");
                if(nodes == null)
                {
                    Console.WriteLine($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {htmlDocument.DocumentNode.OuterHtml}");
                    await bot.SendTextMessageAsync(chatId, $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {htmlDocument.DocumentNode.OuterHtml}");
                }

                foreach(var node in nodes)
                {
                    var titleNode = node.SelectSingleNode("p/a");
                    if (titleNode != null)
                    {
                        var url = $"https://quasarzone.com{titleNode.Attributes["href"].Value}";
                        var match = Regex.Match(url, @"views/(\d+)?");
                        if(!int.TryParse(match.Groups[1].Value, out int postId))
                        {
                            Console.WriteLine($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {match.Groups[1].Value} 값 이상함");
                            await bot.SendTextMessageAsync(chatId, $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {match.Groups[1].Value} 값 이상함");
                        }
                        
                        if (postIds.Contains(postId))
                            continue;
                        postIds.Add(postId);
                        var title = titleNode.InnerText.Trim();
                        Console.WriteLine($"{DateTime.Now:yyyy-MM-dd HH:mm:ss}{title}");
                        await bot.SendTextMessageAsync(chatId, $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}, {title}] {url}");
                    }
                    else
                    {
                        Console.WriteLine($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {node.OuterHtml}");
                        await bot.SendTextMessageAsync(chatId, $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {node.OuterHtml}");
                    }
                }

                Console.WriteLine($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] 10초 쉼");

                await Task.Delay(TimeSpan.FromSeconds(10));
            }
            
        }
    }
}
