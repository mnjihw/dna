using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Documents;


namespace 빌런세척기
{
    public sealed class DCManager
    {
        private readonly Telegram.Bot.TelegramBotClient bot = new Telegram.Bot.TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
        private readonly string chatId = "581720134";//나
        private static Lazy<DCManager> InstanceHolder { get; } = new Lazy<DCManager>(() => new DCManager());
        public static DCManager Instance => InstanceHolder.Value;

        private static readonly HtmlDocument htmlDocument = new HtmlDocument();
        private static readonly CookieContainer cookieContainer = new CookieContainer();
        private static readonly HttpClientHandler httpClientHandler = new HttpClientHandler
        {
            CookieContainer = cookieContainer,
        };
        private static readonly HttpClient httpClient = new HttpClient(httpClientHandler) { Timeout = TimeSpan.FromSeconds(10) };

        private bool LoggedIn { get; set; }
        private DCManager()
        {

        }
        public async Task<bool> Login(string id, string password)
        {
            var postData = new Dictionary<string, string>
            {
                ["user_id"] = id,
                ["pw"] = password,
                ["s_url"] = "//www.dcinside.com/",
                ["ssl"] = "Y",
            };
            string result;

            while (true)
            {
                try
                {
                    result = await httpClient.GetStringAsync("https://www.dcinside.com/");
                }
                catch (Exception e)
                {
                    await bot.SendTextMessageAsync(chatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(5));
                    continue;
                }
                break;
            }
            htmlDocument.LoadHtml(result);
            var node = htmlDocument.DocumentNode.SelectSingleNode(@"//form[@id='login_process']/input[not(contains(@name, 'ssl')) and not(contains(@name, 's_url'))]");

            var secureKey = node.Attributes["name"].Value;
            var secureValue = node.Attributes["value"].Value;
            postData.Add(secureKey, secureValue);

            httpClient.DefaultRequestHeaders.Remove("User-Agent");
            httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36 Edg/81.0.416.72");
            httpClient.DefaultRequestHeaders.Remove("Referer");
            httpClient.DefaultRequestHeaders.Add("Referer", "https://www.dcinside.com/");

            var formUrlEncodedContent = new FormUrlEncodedContent(postData);
            while (true)
            {
                try
                {
                    result = await (await httpClient.PostAsync("https://dcid.dcinside.com/join/member_check.php", formUrlEncodedContent)).Content.ReadAsStringAsync();
                }
                catch (Exception e)
                {
                    await bot.SendTextMessageAsync(chatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(5));
                    continue;
                }
                break;
            }
            formUrlEncodedContent.Dispose();
            LoggedIn = !result.Contains("아이디 또는 비밀번호가 잘못되었습니다");

            return LoggedIn;
        }
        public async Task<bool> DeleteArticles(string galleryName, string[] articleNumbers)
        {
            MessageBox.Show("1");
            if (!LoggedIn)
                return false;
            List<KeyValuePair<string, string>> postData = new List<KeyValuePair<string, string>>
            {
                new KeyValuePair<string, string>("id", galleryName),
            }; MessageBox.Show("2");

            foreach (Cookie cookie in cookieContainer.GetCookies(new Uri("https://dcinside.com")))
            {
                MessageBox.Show("3");
                if (cookie.Name == "ci_c")
                {
                    postData.Add(new KeyValuePair<string, string>("ci_t", cookie.Value));
                    MessageBox.Show("4");
                    break;
                }
            }
            MessageBox.Show("5");
            foreach (var articleNumber in articleNumbers)
                postData.Add(new KeyValuePair<string, string>("nos[]", articleNumber));

            httpClient.DefaultRequestHeaders.Remove("X-Requested-With");
            httpClient.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");
            MessageBox.Show("6");
            var formUrlEncodedContent = new FormUrlEncodedContent(postData);

            string result;
            while (true)
            {
                try
                {
                    result = await (await httpClient.PostAsync("https://gall.dcinside.com/ajax/minor_manager_board_ajax/delete_list", formUrlEncodedContent)).Content.ReadAsStringAsync();
                }
                catch (Exception e)
                {
                    await bot.SendTextMessageAsync(chatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(5));
                    continue;
                }
                break;
            }
            formUrlEncodedContent.Dispose();
            using JsonDocument jsonDocument = JsonDocument.Parse(result);
            return jsonDocument.RootElement.GetProperty("result").ToString() == "success";
        }

        public async Task<List<string>> GetSpamArticlesNumbers(string galleryName, bool isMinorGallery, string[] writers, int pageNumber)
        {
            var articleNumbers = new List<string>();

            string result = default;

            while (true)
            {
                try
                {
                    result = await httpClient.GetStringAsync($"https://gall.dcinside.com/{(isMinorGallery ? "mgallery/" : "")}board/lists/?id={galleryName}&page={pageNumber}");
                }
                catch (Exception e)
                {
                    await bot.SendTextMessageAsync(chatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(5));
                    continue;
                }
                break;
            }
            htmlDocument.OptionAutoCloseOnEnd = true;
            htmlDocument.LoadHtml(result);

            var nodes = htmlDocument.DocumentNode.SelectNodes(@"//table[@class='gall_list ']/tbody/tr[@class='ub-content us-post' and not(contains(@data-type, 'icon_notice'))]");

            foreach (var node in nodes)
            {
                var articleNumber = node.SelectSingleNode("td[@class='gall_num']").InnerText;
                var writer = node.SelectSingleNode("td[@class='gall_writer ub-writer']").Attributes["data-nick"].Value;

                if(writers.Any(s => s.IndexOf(writer, StringComparison.OrdinalIgnoreCase) != -1))
                {
                    articleNumbers.Add(articleNumber);
                }
                
            }
            return articleNumbers;
        }

        public async Task<bool> HasPermission(string galleryName, bool isMinorGallery)
        {
            string result;

            while (true)
            {
                try
                {
                    result = await httpClient.GetStringAsync($"https://gall.dcinside.com/{(isMinorGallery ? "mgallery/" : "")}board/lists/?id={galleryName}");
                }
                catch (Exception e)
                {
                    await bot.SendTextMessageAsync(chatId, e.ToString());
                    await Task.Delay(TimeSpan.FromSeconds(5));
                    continue;
                }
                break;
            }

            htmlDocument.LoadHtml(result);

            return htmlDocument.DocumentNode.SelectSingleNode(@"//script[@id='minor_td-tmpl']") != null;
        }

    }
}
