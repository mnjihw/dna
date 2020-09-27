using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Linq;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Web;
using System.Net.Http;

namespace 리듬_키워드_알리미
{
    public class QuasarzoneKeywordAlert : IKeywordAlert
    {
        public ReadOnlyCollection<string> Keywords { get; }
        private HtmlDocument Document { get; } = new HtmlDocument();
        private List<string> AlertedList { get; } = new List<string>();
        private MainWindow MainWindow => Application.Current.MainWindow as MainWindow;

        public QuasarzoneKeywordAlert(ReadOnlyCollection<string> keywords)
        {
            Keywords = keywords;
        } 
         
        public async Task Run()
        {
            while(MainWindow.Running)
            {
                string result = default;

                try
                {
                    result = await MainWindow.Client.GetStringAsync("https://quasarzone.co.kr/bbs/board.php?bo_table=qb_saleinfo&sca=%ED%95%98%EB%93%9C%EC%9B%A8%EC%96%B4");
                    Document.LoadHtml(result);

                    var nodes = Document.DocumentNode.SelectNodes(@"//ul[@class='list-body']/li[not(contains(@class, 'bg-black'))]/div[@class='wr-subject']/a");

                    foreach (var node in nodes)
                    {
                        var match = Regex.Match(node.InnerText, @"[\t\r\n\d\s]*(.+)\s*");
                        var title = match.Groups[1].Value.Trim();
                        var url = HttpUtility.HtmlDecode(node.Attributes["href"].Value);
                        match = Regex.Match(url, @"wr_id=(\d+)&");
                        var articleNumber = match.Groups[1].Value;

                        if (Keywords.Any(s => title.IndexOf(s, StringComparison.OrdinalIgnoreCase) != -1) && !AlertedList.Contains(articleNumber))
                        {
                            await MainWindow.Bot.SendTextMessageAsync("581720134", $"{url} {title}");
                            AlertedList.Add(articleNumber);
                        }
                    }
                }
                catch (HttpRequestException)
                {
                    continue;
                }
                catch (Exception e)
                {
                    await MainWindow.Bot.SendTextMessageAsync("581720134", $"이런 예외 뜸 {e}");
                }
                await Task.Delay(TimeSpan.FromMinutes(10));
            }
        }
    }
}
