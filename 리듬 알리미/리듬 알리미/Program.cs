using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Telegram.Bot;

namespace 리듬_알리미
{
    /* 공통 input: TimeSpan
     * 
     * 주식 - input: 종목, 가격; output: 종목, 가격, 시점
     * 디시 - input: 키워드, 갤; output: 제목, url
     * 스위치 - input: 뚜렷한거 없음, 코드로 짜야함; output: 제목, url
     * 구인구직 - input: 키워드; output: 제목, url
     * 
      */

    class Program
    {
        public static TelegramBotClient Bot { get; set; } = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
        static async Task Main(string[] args)
        {
            HttpClient client = new HttpClient();
            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-F907N/KOS3ATB4) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/11.1 Chrome/75.0.3770.143 Safari/537.36");
            List<string> articleNumbers = new List<string>();
            string[] Keywords = { "무망호"};
            var htmlDocument = new HtmlDocument();


            while (true)
            {
                var result = await client.GetStringAsync($"https://m.dcinside.com/board/game_nintendo");

                htmlDocument.LoadHtml(result);
                var articles = htmlDocument.DocumentNode.SelectNodes("//ul[@class='gall-detail-lst']/li/div/a[@class='lt']");

                foreach (var article in articles)
                {
                    var title = article.SelectSingleNode("span[@class='subject']/span[@class='detail-txt']").InnerText;

                    if (Keywords.Any(s => title.Contains(s)))
                    {
                        var number = Regex.Match(article.Attributes["href"].Value, @"/(\d+)$").Groups[1].Value;

                        if (!articleNumbers.Contains(number))
                        {
                            articleNumbers.Add(number);
                            //await Program.Bot.SendTextMessageAsync("", $"https://m.dcinside.com/board/game_nintendo/{number}: {title}");
                            Console.WriteLine(title);
                            Console.Beep(200, 500);
                        }
                    }
                }
                await Task.Delay(TimeSpan.FromSeconds(7));
            }

            return;

           /* HttpClient client = new HttpClient();
            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.162 Safari/537.36 Edg/80.0.361.109");
              
            while (true)
            {
                var result = await client.GetStringAsync("http://www.shinsegaetvshopping.com/display/detail/10573631");
                HtmlDocument document = new HtmlDocument();
                document.LoadHtml(result);
                var text = document.DocumentNode.SelectSingleNode("//span[@class='label--1']").InnerText.Trim();

                if(!text.Contains("준비"))
                {
                    await Bot.SendTextMessageAsync("581720134", "떴다 http://www.shinsegaetvshopping.com/display/detail/10573631");
                    await Bot.SendTextMessageAsync("991922024", "떴다 http://www.shinsegaetvshopping.com/display/detail/10573631");
                    Console.WriteLine("떴다");
                }

                Console.WriteLine("도는중..." + DateTime.Now);
                await Task.Delay(TimeSpan.FromSeconds(20));
            }
*/
        }
    }
}
