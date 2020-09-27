using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace 리듬_알리미
{
    public class DcinsideMonitor// : IWebsiteMonitor
    {
        private static Lazy<DcinsideMonitor> InstanceHolder { get; } = new Lazy<DcinsideMonitor>(() => new DcinsideMonitor());
        public static DcinsideMonitor Instance => InstanceHolder.Value;
        private HttpClient Client { get; } = new HttpClient();

        public List<(string telegramId, string gallery, string keywords)> UsersInfo { get; } = new List<(string, string, string)>();
        public string[] Keywords;

        public Dictionary<string, string> galleries = new Dictionary<string, string>
        {
            ["닌텐도"] = "game_nintendo",
        };
        private readonly List<string> articleNumbers = new List<string>();

        private DcinsideMonitor() 
        {
            Client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-F907N/KOS3ATB4) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/11.1 Chrome/75.0.3770.143 Safari/537.36");
        }
        public async void Function()
        {
            var result = await Client.GetStringAsync($"https://m.dcinside.com/board/{galleries["닌텐도"]}");
            var htmlDocument = new HtmlDocument();
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
                        await Program.Bot.SendTextMessageAsync("", $"https://m.dcinside.com/board/{galleries["닌텐도"]}/{number}: {title}");
                    }
                }
            }
            await Task.Delay(TimeSpan.FromSeconds(30));

        }
    }
}
