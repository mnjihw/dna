using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Timers;
using System.Threading.Tasks;
using Xamarin.Forms;
using System.Text.RegularExpressions;

namespace android_test
{
    [DesignTimeVisible(false)]
    public partial class MainPage : ContentPage
    {
        private static readonly HttpClient httpClient = new HttpClient();
        private readonly Timer timer = new Timer { Interval = 10000, Enabled = true};
        private readonly List<string> listToBeNotified = new List<string>();
        private Dictionary<string, string> galleries = new Dictionary<string, string>
        {
            ["닌텐도"] = "game_nintendo",
            ["국내야구"] = "baseball_new8"
        };

        public MainPage()
        {
            InitializeComponent();

            galleryPicker.ItemsSource = galleries.Keys.ToList();
            galleryPicker.SelectedIndex = 0;
        }




        private void Button_Clicked(object sender, EventArgs e)
        {

            if (string.IsNullOrWhiteSpace(keywordEditor.Text))
            {
                DependencyService.Get<IToast>().Show("키워드 입력 안 됨!");
                return;
            }
            var keywords = keywordEditor.Text.Split(',');

            for (int i = 0; i < keywords.Length; ++i)
                keywords[i] = keywords[i].Trim();


            httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-F907N/KOS3ATB4) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/11.1 Chrome/75.0.3770.143 Safari/537.36");

            timer.Elapsed += async (_sender, _e) =>
            {
                var result = await httpClient.GetStringAsync($"https://m.dcinside.com/board/{galleries[galleryPicker.SelectedItem.ToString()]}");
                var htmlDocument = new HtmlDocument();
                htmlDocument.LoadHtml(result);
                var articles = htmlDocument.DocumentNode.SelectNodes("//ul[@class='gall-detail-lst']/li/div/a[@class='lt']");

                foreach (var article in articles)
                {
                    var title = article.SelectSingleNode("span[@class='subject']/span[@class='detail-txt']").InnerText;

                    if (keywords.Any(s => title.Contains(s)))
                    {
                        var number = Regex.Match(article.Attributes["href"].Value, @"/(\d+)$").Groups[1].Value;

                        if (!listToBeNotified.Contains(number))
                        {
                            listToBeNotified.Add(number);
                            DependencyService.Get<INotification>().Notify("떴다", title, $"https://m.dcinside.com/board/{galleries[galleryPicker.SelectedItem.ToString()]}/{number}");
                        }
                    }
                }


            };
        }
    }
}
